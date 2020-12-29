/* PowerCepstrum.cpp
 *
 * Copyright (C) 2012-2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "PowerCepstrum.h"
#include "NUM2.h"
#include "Vector.h"

Thing_implement (PowerCepstrum, Cepstrum, 2); // derives from Matrix therefore also version 2

double structPowerCepstrum :: v_getValueAtSample (integer isamp, integer row, int units) {
	double result = undefined;
	if (row == 1) {
		if (units == 0)
			result = z [1] [isamp];
		else
			result = 10.0 * log10 (z [1] [isamp] + 1e-30); // always positive
	}
	return result;
}

autoPowerCepstrum Cepstrum_downto_PowerCepstrum (Cepstrum me ) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my xmax, my nx);
		for (integer i = 1; i <= my nx; i ++) {
			thy z [1] [i] = my z [1] [i] * my z [1] [i];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" not converted.");
	}
}

autoPowerCepstrum PowerCepstrum_create (double qmax, integer nq) {
	try {
		autoPowerCepstrum me = Thing_new (PowerCepstrum);
		double dq = qmax / (nq - 1);

		Matrix_init (me.get(), 0.0, qmax, nq, dq, 0.0, 1.0, 1.0, 1, 1, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PowerCepstrum not created.");
	}
}

void PowerCepstrum_draw (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, bool garnish) {
	Cepstrum_draw (me, g, qmin, qmax, dBminimum, dBmaximum, true, garnish);
}

void PowerCepstrum_drawTrendLine (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, double qstart, double qend, kCepstrumTrendType lineType, kCepstrumTrendFit method) {

	Graphics_setInner (g);

	if (qmax <= qmin) {
		qmin = my xmin;
		qmax = my xmax;
	}

	if (dBminimum >= dBmaximum) { // autoscaling
		integer imin, imax;
		integer numberOfPoints = Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax);
		if (numberOfPoints == 0)
			return;
		MelderExtremaWithInit extrema_db;
		for (integer i = imin; i <= imax; i ++)
			extrema_db.update (my v_getValueAtSample (i, 1, 1));
		dBmaximum = extrema_db.max;
		dBminimum = extrema_db.min;
	}

	Graphics_setWindow (g, qmin, qmax, dBminimum, dBmaximum);
	qend = ( qend == 0 ? my xmax : qend );
	if (qend <= qstart) {
		qend = my xmax;
		qstart = my xmin;
	}
	Melder_clipLeft (my xmin, & qstart);
	Melder_clipRight (& qend, my xmax);

	double a, intercept;
	PowerCepstrum_fitTrendLine (me, qstart, qend, & a, & intercept, lineType, method);
	/*
	 * Don't draw part outside window
	 */
	double lineWidth =  Graphics_inqLineWidth (g);
	Graphics_setLineWidth (g, 2);
	if (lineType == kCepstrumTrendType::EXPONENTIAL_DECAY ) {
		integer n = 500;
		const double dq = (qend - qstart) / (n + 1);
		const double q1 = qstart;
		if (qstart <= 0.0) {
			qstart = 0.1 * dq; // some small offset to avoid log(0)
			n--; 
		}
		autoVEC y = raw_VEC (n);
		
		for (integer i = 1; i <= n; i ++) {
			const double q = q1 + (i - 1) * dq;
			y [i] = a * log (q) + intercept;
		}
		Graphics_function (g, y.asArgumentToFunctionThatExpectsOneBasedArray(), 1, n, qstart, qend);
	} else {
		const double y1 = a * qstart + intercept;
		const double y2 = a * qend + intercept;
		if (y1 >= dBminimum && y2 >= dBminimum) {
			Graphics_line (g, qstart, y1, qend, y2);
		} else if (y1 < dBminimum) {
			qstart = (dBminimum - intercept) / a;
			Graphics_line (g, qstart, dBminimum, qend, y2);
		} else if (y2 < dBminimum) {
			qend = (dBminimum - intercept) / a;
			Graphics_line (g, qstart, y1, qend, dBminimum);
		} else {
			// don't draw anything below lower limit?
		}
	}
	Graphics_setLineWidth (g, lineWidth);
	Graphics_unsetInner (g);
}

/*
	Fit line y = ax+b or y = a log(x) + b  on interval [qmin,qmax]
 */
void PowerCepstrum_fitTrendLine (PowerCepstrum me, double qmin, double qmax, double *out_a, double *out_intercept, kCepstrumTrendType lineType, kCepstrumTrendFit method) {
	try {
		double a, intercept;
		if (qmax <= qmin) {
			qmin = my xmin;
			qmax = my xmax;
		}

		integer imin, imax;
		if (Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax) == 0)
			return;
		if (imin == 1 && lineType == kCepstrumTrendType::EXPONENTIAL_DECAY)
			imin = 2; // because log(0) is undefined
		integer numberOfPoints = imax - imin + 1;
		Melder_require (numberOfPoints > 1,
			U"Not enough points for fit.");

		autoVEC y = raw_VEC (numberOfPoints);
		autoVEC x = raw_VEC (numberOfPoints);
		for (integer i = 1; i <= numberOfPoints; i ++) {
			integer isamp = imin + i - 1;
			x [i] = my x1 + (isamp - 1) * my dx;
			if (lineType == kCepstrumTrendType::EXPONENTIAL_DECAY)
				x [i] = log (x [i]);
			y [i] = my v_getValueAtSample (isamp, 1, 1);
		}
		if (method == kCepstrumTrendFit::LEAST_SQUARES)
			NUMlineFit_LS (x.get(), y.get(), & a, & intercept);
		else if (method == kCepstrumTrendFit::ROBUST_FAST)
			NUMlineFit_theil (x.get(), y.get(), & a, & intercept, false);
		else if (method == kCepstrumTrendFit::ROBUST_SLOW)
			NUMlineFit_theil (x.get(), y.get(), & a, & intercept, true);
		else {
			Melder_throw (U"Invalid method.");
		}
		if (out_intercept)
			*out_intercept = intercept;
		if (out_a)
			*out_a = a;
	} catch (MelderError) {
		Melder_throw (me, U": couldn't fit a line.");
	}
}

#if 0
// Hillenbrand subtracts dB values and if the result is negative it is made zero
static void PowerCepstrum_subtractTrendLine_inline2 (PowerCepstrum me, double slope, double intercept, int lineType) {
	for (integer j = 1; j <= my nx; j ++) {
		double q = my x1 + (j - 1) * my dx;
		q = j == 1 ? 0.5 * my dx : q; // approximation
		double xq = lineType == 2 ? log(q) : q;
		double db_background = slope * xq + intercept;
		double db_cepstrum = my v_getValueAtSample (j, 1, 1);
		double diff = exp ((db_cepstrum - db_background) * NUMln10 / 10) - 1e-30;
		my z [1] [j] = diff;
	}
}
#endif

// clip with tilt line
static void PowerCepstrum_subtractTrendLine_inplace (PowerCepstrum me, double slope, double intercept, kCepstrumTrendType lineType) {
	for (integer j = 1; j <= my nx; j ++) {
		double q = my x1 + (j - 1) * my dx;
		q = j == 1 ? 0.5 * my dx : q; // approximation
		double xq = lineType == kCepstrumTrendType::EXPONENTIAL_DECAY ? log(q) : q;
		double db_background = slope * xq + intercept;
		double db_cepstrum = my v_getValueAtSample (j, 1, 1);
		double diff = db_cepstrum - db_background;
		if (diff < 0) {
			diff = 0;
		}
		my z [1] [j] = exp (diff * NUMln10 / 10.0) - 1e-30;
	}
}


void PowerCepstrum_subtractTrend_inplace (PowerCepstrum me, double qstartFit, double qendFit, kCepstrumTrendType lineType, kCepstrumTrendFit fitMethod) {
	double slope, intercept;
	PowerCepstrum_fitTrendLine (me, qstartFit, qendFit, & slope, & intercept, lineType, fitMethod);
	PowerCepstrum_subtractTrendLine_inplace (me, slope, intercept, lineType);
}

autoPowerCepstrum PowerCepstrum_subtractTrend (PowerCepstrum me, double qstartFit, double qendFit, kCepstrumTrendType lineType, kCepstrumTrendFit fitMethod) {
	try {
		autoPowerCepstrum thee = Data_copy (me);
		PowerCepstrum_subtractTrend_inplace (thee.get(), qstartFit, qendFit, lineType, fitMethod);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": couldn't subtract tilt line.");
	}
}

static void PowerCepstrum_smooth_inplaceRectangular (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	try {
		double halfWindwow = 0.5 * quefrencyAveragingWindow;
		double numberOfQuefrencyBins = quefrencyAveragingWindow / my dx;
		if (numberOfQuefrencyBins > 1.0) {
			autoVEC qout = raw_VEC (my nx);
			for (integer k = 1; k <= numberOfIterations; k ++) {
				for (integer isamp = 1; isamp <= my nx; isamp ++) {
					const double xmid = Sampled_indexToX (me, isamp);
					qout [isamp] = Sampled_getMean (me, xmid - halfWindwow, xmid + halfWindwow, 1, 0, true);
				}
				my z.row (1)  <<=  qout.all();
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}
static void PowerCepstrum_smooth_inplaceRectangular_old (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	try {
		integer numberOfQuefrencyBins = Melder_ifloor (quefrencyAveragingWindow / my dx);
		if (numberOfQuefrencyBins > 1) {
			autoVEC qin = copy_VEC (my z.row (1));
			autoVEC qout = raw_VEC (my nx);
			for (integer k = 1; k <= numberOfIterations; k ++)
				if (k % 2 == 1) 
					VECsmoothByMovingAverage_preallocated (qout.get(), qin.get(), numberOfQuefrencyBins);
				else
					VECsmoothByMovingAverage_preallocated (qin.get(), qout.get(), numberOfQuefrencyBins);
			my z.row (1) <<= ( numberOfIterations % 2 == 1 ? qout.get() : qin.get() );
		}
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

static void PowerCepstrum_smooth_inplaceGaussian (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	try {
		double numberOfQuefrencyBins = quefrencyAveragingWindow / my dx;
		if (numberOfQuefrencyBins > 1.0) {
			/*
				Applying  two Gaussians after another is associative: (G(s2)*(G(s1)*f) = (G(s2)*G(s1))*f.
				G(s2) * G(s1) = G(s), where s^2=s1^2+s2^2
			*/
			const double numberOfSigmasInWindow = 4.0;
			const double sigma = numberOfQuefrencyBins / numberOfSigmasInWindow; // 3sigma -> 99.7 % of the data (2sigma -> 95.4%)g 
			const double sigma_n = sqrt (numberOfIterations) * sigma;
			VECsmooth_gaussian_inplace (my z.row (1), sigma_n);
			/*
				Due to imprecise arithmatic some values might turn out to be negative
				(but very small). Just make them positive.
			*/
			VECabs_inplace (my z.row (1));
		}
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

void PowerCepstrum_smooth_inplace (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	if (Melder_debug == -4)
		PowerCepstrum_smooth_inplaceRectangular_old (me, quefrencyAveragingWindow, numberOfIterations);
	else if (Melder_debug == -5)
		PowerCepstrum_smooth_inplaceGaussian (me, quefrencyAveragingWindow, numberOfIterations);
	else
		PowerCepstrum_smooth_inplaceRectangular (me, quefrencyAveragingWindow, numberOfIterations);
}

autoPowerCepstrum PowerCepstrum_smooth (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_smooth_inplace (thee.get(), quefrencyAveragingWindow, numberOfIterations);
	return thee;
}

void PowerCepstrum_getMaximumAndQuefrency (PowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType, double *out_peakdB, double *out_quefrency) {
	autoPowerCepstrum thee = Data_copy (me);
	double lowestQuefrency = 1.0 / pitchCeiling, highestQuefrency = 1.0 / pitchFloor;
	for (integer i = 1; i <= my nx; i ++) {
		thy z [1] [i] = my v_getValueAtSample (i, 1, 1); // 10 log val^2
	}
	double peakdB, quefrency;
	Vector_getMaximumAndX ((Vector) thee.get(), lowestQuefrency, highestQuefrency, 1, peakInterpolationType, & peakdB, & quefrency);   // FIXME cast
	if (out_peakdB)
		*out_peakdB = peakdB;
	if (out_quefrency)
		*out_quefrency = quefrency;
}

double PowerCepstrum_getRNR (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {
	double rnr = undefined;
	double qmin = 1.0 / pitchCeiling, qmax = 1.0 / pitchFloor, peakdB, qpeak;
	PowerCepstrum_getMaximumAndQuefrency (me, pitchFloor, pitchCeiling, kVector_peakInterpolation :: CUBIC, & peakdB, & qpeak);
	integer imin, imax;
	if (Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax) == 0)
		return rnr;

	integer ndata = imax - imin + 1;
	if (ndata < 2)
		return rnr;	
	// how many peaks in interval ?
	integer npeaks = 2;
	while (qpeak > 0 && qpeak * npeaks <= qmax)
        npeaks++;
	npeaks--;
	
	double sum = 0.0, sumr = 0.0;
	for (integer i = imin; i <= imax; i ++) {
		double val = my v_getValueAtSample (i, 1, 0);
		double qx = my x1 + (i - 1) * my dx;
		sum += val;
		// is qx within an interval around a multiple of the peak's q ?
		for (integer j = 1; j <= npeaks; j ++) {
			double f0c = 1.0 / (j * qpeak);
			double f0clow = f0c * (1.0 - f0fractionalWidth);
			double f0chigh = f0c * (1.0 + f0fractionalWidth);
			double qclow =  1.0 / f0chigh;
			double qchigh = ( f0fractionalWidth >= 1 ? qmax : 1.0 / f0clow );
			if (qx >= qclow && qx <= qchigh) {   // yes in rahmonic interval
				sumr += val;
				break;
			}
		}
	}
	rnr = sumr >= sum ? 1000000.0 : sumr / (sum - sumr);
	return rnr;
}

double PowerCepstrum_getPeakProminence_hillenbrand (PowerCepstrum me, double pitchFloor, double pitchCeiling, double *out_qpeak) {
	double slope, intercept, quefrency, peakdB;
	PowerCepstrum_fitTrendLine (me, 0.001, 0, & slope, & intercept, kCepstrumTrendType::LINEAR, kCepstrumTrendFit::LEAST_SQUARES);
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_subtractTrendLine_inplace (thee.get(), slope, intercept, kCepstrumTrendType::LINEAR);
	PowerCepstrum_getMaximumAndQuefrency (thee.get(), pitchFloor, pitchCeiling, kVector_peakInterpolation :: NONE, & peakdB, & quefrency);
	if (out_qpeak)
		*out_qpeak = quefrency;
	return peakdB;
}

double PowerCepstrum_getPeakProminence (PowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType, double qstartFit, double qendFit, kCepstrumTrendType  lineType, kCepstrumTrendFit fitMethod, double *out_qpeak) {
	double slope, intercept, qpeak, peakdB;
	PowerCepstrum_fitTrendLine (me, qstartFit, qendFit, & slope, & intercept, lineType, fitMethod);
	PowerCepstrum_getMaximumAndQuefrency (me, pitchFloor, pitchCeiling, peakInterpolationType, & peakdB, & qpeak);
	double xq = lineType == kCepstrumTrendType::EXPONENTIAL_DECAY ? log(qpeak) : qpeak;
	double db_background = slope * xq + intercept;
	double cpp = peakdB - db_background;
	if (out_qpeak)
		*out_qpeak = qpeak;
	return cpp;
}

autoMatrix PowerCepstrum_to_Matrix (PowerCepstrum me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix created.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum (Matrix me) {
	try {
		if (my ny != 1) {
			Melder_throw (U"Matrix should have exactly 1 row.");
		}
		autoPowerCepstrum thee = Thing_new (PowerCepstrum);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PowerCepstrum.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum_row (Matrix me, integer row) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my xmax, my nx);
		Melder_require (row > 0 && row <= my ny,
			U"Row number should be between 1 and ", my ny, U" inclusive.");
		thy z.row (1) <<= my z.row (row);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrum created.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum_column (Matrix me, integer col) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my ymax, my ny);
		Melder_require (col > 0 && col <= my nx,
			U"Column number should be between 1 and ", my nx, U" inclusive.");
		thy z.row (1) <<= my z.column (col);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrum created.");
	}
}

/* End of file PowerCepstrum.cpp */
