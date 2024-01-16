/* PowerCepstrum.cpp
 *
 * Copyright (C) 2012-2024 David Weenink
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

Thing_implement (PowerCepstrum, Cepstrum, 2);   // derives from Matrix; therefore also version 2

double structPowerCepstrum :: v_getValueAtSample (integer isamp, integer row, int units) const {
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
		for (integer i = 1; i <= my nx; i ++)
			thy z [1] [i] = sqr (my z [1] [i]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" not converted.");
	}
}

autoPowerCepstrum PowerCepstrum_create (double qmax, integer nq) {
	try {
		autoPowerCepstrum me = Thing_new (PowerCepstrum);
		const double dq = qmax / (nq - 1);
		Matrix_init (me.get(), 0.0, qmax, nq, dq, 0.0, 1.0, 1.0, 1, 1, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PowerCepstrum not created.");
	}
}

static autoMatrix PowerCepstrum_as_Matrix_dB (PowerCepstrum me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		for (integer i = 1; i <= my nx; i ++)
			thy z [1] [i] = my v_getValueAtSample (i, 1, 1); // 10 log val^2
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not conver to Matrix.");
	}
}

void PowerCepstrum_draw (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, bool garnish) {
	Cepstrum_draw (me, g, qmin, qmax, dBminimum, dBmaximum, true, garnish);
}

void PowerCepstrum_drawTrendLine (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, double qstart, double qend, kCepstrum_trendType lineType, kCepstrum_trendFit method) {

	Graphics_setInner (g);

	if (qmax <= qmin) {
		qmin = my xmin;
		qmax = my xmax;
	}

	if (dBminimum >= dBmaximum) {   // autoscaling
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
	qend = ( qend == 0.0 ? my xmax : qend );
	if (qend <= qstart) {
		qend = my xmax;
		qstart = my xmin;
	}
	Melder_clipLeft (my xmin, & qstart);
	Melder_clipRight (& qend, my xmax);

	double slope, intercept;
	PowerCepstrum_fitTrendLine (me, qstart, qend, & slope, & intercept, lineType, method);
	/*
		Don't draw part outside window
	*/
	const double lineWidth = Graphics_inqLineWidth (g);
	Graphics_setLineWidth (g, 2);
	if (lineType == kCepstrum_trendType::EXPONENTIAL_DECAY ) {
		integer n = 500;
		const double dq = (qend - qstart) / (n + 1);
		const double q1 = qstart;
		if (qstart <= 0.0) {
			qstart = 0.1 * dq;   // some small offset to avoid log(0)
			n --;
		}
		autoVEC y = raw_VEC (n);
		
		for (integer i = 1; i <= n; i ++) {
			const double q = q1 + (i - 1) * dq;
			y [i] = slope * log (q) + intercept;
		}
		Graphics_function (g, y.asArgumentToFunctionThatExpectsOneBasedArray(), 1, n, qstart, qend);
	} else {
		const double y1 = slope * qstart + intercept;
		const double y2 = slope * qend + intercept;
		if (y1 >= dBminimum && y2 >= dBminimum) {
			Graphics_line (g, qstart, y1, qend, y2);
		} else if (y1 < dBminimum) {
			qstart = (dBminimum - intercept) / slope;
			Graphics_line (g, qstart, dBminimum, qend, y2);
		} else if (y2 < dBminimum) {
			qend = (dBminimum - intercept) / slope;
			Graphics_line (g, qstart, y1, qend, dBminimum);
		} else {
			// don't draw anything below lower limit?
		}
	}
	Graphics_setLineWidth (g, lineWidth);
	Graphics_unsetInner (g);
}

/*
	Fit line y = aq+b or y = a log(q) + b  on interval [qmin,qmax]
*/
void PowerCepstrum_fitTrendLine (PowerCepstrum me, double qmin, double qmax, double *out_slope, double *out_intercept, kCepstrum_trendType lineType, kCepstrum_trendFit method) {
	try {
		Function_unidirectionalAutowindow (me, & qmin, & qmax);
		
		integer imin, imax;
		Melder_require (qmin >= my xmin && qmax <= my xmax,
			U"Your quefrency range is outside the domain.");
		Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax);
		Melder_clipLeft (2_integer, & imin); // never use q=0 in fitting
		const integer numberOfPoints = imax - imin + 1;
		Melder_require (numberOfPoints > 1,
			U"Not enough points for fit.");

		autoVEC y = raw_VEC (numberOfPoints);
		autoVEC x = raw_VEC (numberOfPoints);
		for (integer i = 1; i <= numberOfPoints; i ++) {
			const integer isamp = imin + i - 1;
			x [i] = my x1 + (isamp - 1) * my dx;
			if (lineType == kCepstrum_trendType::EXPONENTIAL_DECAY)
				x [i] = log (x [i]);
			y [i] = my v_getValueAtSample (isamp, 1, 1);
		}
		double slope, intercept;
		if (method == kCepstrum_trendFit::LEAST_SQUARES)
			NUMfitLine_LS (x.get(), y.get(), & slope, & intercept);
		else if (method == kCepstrum_trendFit::ROBUST_FAST)
			NUMfitLine_theil (x.get(), y.get(), & slope, & intercept, false);
		else if (method == kCepstrum_trendFit::ROBUST_SLOW)
			NUMfitLine_theil (x.get(), y.get(), & slope, & intercept, true);
		else
			Melder_throw (U"Invalid method.");
		if (out_intercept)
			*out_intercept = intercept;
		if (out_slope)
			*out_slope = slope;
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
		const double xq = lineType == 2 ? log(q) : q;
		const double db_background = slope * xq + intercept;
		const double db_cepstrum = my v_getValueAtSample (j, 1, 1);
		const double diff = exp ((db_cepstrum - db_background) * NUMln10 / 10);
		my z [1] [j] = diff;
	}
}
#endif


static inline double getTrendLineValueAtQuefrency (double quefrency, double slope, double intercept, kCepstrum_trendType lineType) {
	const double xq = ( lineType == kCepstrum_trendType::EXPONENTIAL_DECAY ? log (quefrency) : quefrency );
	return slope * xq + intercept;
}

// clip with tilt line
static void PowerCepstrum_subtractTrendLine_inplace (PowerCepstrum me, double slope, double intercept, kCepstrum_trendType lineType) {
	for (integer j = 1; j <= my nx; j ++) {
		/*
			For the exponential decay function, y = slope*log(quefrency)+intercept, the PowerCepstrum's first quefrency value (at x1)
			has quefrency == 0 and the value y(0) is not defined. As an approximation for y(0) we use y(0.5*dx).
			This is no problem because the value at quefrency == 0 is not relevant.
		*/
		const double quefrency = ( j == 1 && lineType == kCepstrum_trendType::EXPONENTIAL_DECAY ? 0.5 * my dx : (j - 1) * my dx );
		const double db_background = getTrendLineValueAtQuefrency (quefrency, slope, intercept, lineType);
		const double db_cepstrum = my v_getValueAtSample (j, 1, 1);
		const double diff = Melder_clippedLeft (0.0, db_cepstrum - db_background);
		my z [1] [j] = exp (diff * NUMln10 / 10.0);
	}
}

double PowerCepstrum_getTrendLineValue (PowerCepstrum me, double quefrency, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	double trend_db = undefined;
	if (quefrency >= my xmin && quefrency <= my xmax) {
		double slope, intercept;
		PowerCepstrum_fitTrendLine (me, qstartFit, qendFit, & slope, & intercept, lineType, fitMethod);
		trend_db =  getTrendLineValueAtQuefrency (quefrency, slope, intercept, lineType);
	}
	return trend_db;
}

void PowerCepstrum_subtractTrend_inplace (PowerCepstrum me, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	double slope, intercept;
	PowerCepstrum_fitTrendLine (me, qstartFit, qendFit, & slope, & intercept, lineType, fitMethod);
	PowerCepstrum_subtractTrendLine_inplace (me, slope, intercept, lineType);
}

autoPowerCepstrum PowerCepstrum_subtractTrend (PowerCepstrum me, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
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
		const double halfWindwow = 0.5 * quefrencyAveragingWindow;
		const double numberOfQuefrencyBins = quefrencyAveragingWindow / my dx;
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
		const integer numberOfQuefrencyBins = Melder_ifloor (quefrencyAveragingWindow / my dx);
		if (numberOfQuefrencyBins > 1) {
			autoVEC qin = copy_VEC (my z.row (1));
			autoVEC qout = raw_VEC (my nx);
			for (integer k = 1; k <= numberOfIterations; k ++)
				if (k % 2 == 1) 
					VECsmoothByMovingAverage_preallocated (qout.get(), qin.get(), numberOfQuefrencyBins);
				else
					VECsmoothByMovingAverage_preallocated (qin.get(), qout.get(), numberOfQuefrencyBins);
			my z.row (1)  <<=  ( numberOfIterations % 2 == 1 ? qout.get() : qin.get() );
		}
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

static void PowerCepstrum_smooth_inplaceGaussian (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	try {
		const double numberOfQuefrencyBins = quefrencyAveragingWindow / my dx;
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
			abs_VEC_inout (my z.row (1));
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

void PowerCepstrum_getMaximumAndQuefrency (PowerCepstrum me, double fromQuefrency, double toQuefrency, kCepstrum_peakInterpolation peakInterpolationType, double *out_peakdB, double *out_quefrency) {
	autoMatrix thee = PowerCepstrum_as_Matrix_dB (me);
	kVector_peakInterpolation peakInterpolation = ( peakInterpolationType == kCepstrum_peakInterpolation::NONE ? kVector_peakInterpolation::NONE : peakInterpolationType == kCepstrum_peakInterpolation::CUBIC ? kVector_peakInterpolation::CUBIC : kVector_peakInterpolation::PARABOLIC );
	double peakdB, quefrency;
	Vector_getMaximumAndX ((Vector) thee.get(), fromQuefrency, toQuefrency, 1, peakInterpolation, & peakdB, & quefrency);
	if (out_peakdB)
		*out_peakdB = peakdB;
	if (out_quefrency)
		*out_quefrency = quefrency;
}

void PowerCepstrum_getMaximumAndQuefrency (PowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType, double *out_peakdB, double *out_quefrency) {
	autoMatrix thee = PowerCepstrum_as_Matrix_dB (me);
	const double lowestQuefrency = 1.0 / pitchCeiling, highestQuefrency = 1.0 / pitchFloor;
	double peakdB, quefrency;
	Vector_getMaximumAndX ((Vector) thee.get(), lowestQuefrency, highestQuefrency, 1, peakInterpolationType, & peakdB, & quefrency);
	if (out_peakdB)
		*out_peakdB = peakdB;
	if (out_quefrency)
		*out_quefrency = quefrency;
}

autoTable PowerCepstrum_tabulateRhamonics (PowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType) {
	try {
		autoTable thee;
		double  peakdB, qpeak;
		const double lowestQuefrency = 1.0 / pitchCeiling, highestQuefrency = 1.0 / pitchFloor;
		autoMatrix him = PowerCepstrum_as_Matrix_dB (me);
		Vector_getMaximumAndX ((Vector) him.get(), lowestQuefrency, highestQuefrency, 1, peakInterpolationType, & peakdB, & qpeak);		
		/*
			how many rhamonic peaks in interval [0, xmax] ?
		*/
		integer numberOfRhamonics = 2;
		while (qpeak > 0 && qpeak * numberOfRhamonics <= my xmax)
			numberOfRhamonics ++;
		numberOfRhamonics --;
		const conststring32 columnNames [] = { U"peak(dB)", U"quefrency(s)" };
		thee = Table_createWithColumnNames (numberOfRhamonics, ARRAY_TO_STRVEC (columnNames));

		Table_setNumericValue (thee.get(), 1, 1, peakdB);
		Table_setNumericValue (thee.get(), 1, 2, qpeak);
		kVector_valueInterpolation valueInterpolationType = ( peakInterpolationType > kVector_peakInterpolation :: NONE ?
			kVector_valueInterpolation :: LINEAR : kVector_valueInterpolation :: NEAREST );
		for (integer rhamonic = 2; rhamonic <= numberOfRhamonics; rhamonic ++) {
			const double quefrency = rhamonic * qpeak;
			const double amplitude = Vector_getValueAtX ((Vector) him.get(), quefrency, 1, valueInterpolationType);
			Table_setNumericValue (thee.get(), rhamonic, 1, amplitude);
			Table_setNumericValue (thee.get(), rhamonic, 2, quefrency);
		}		
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rhamonics could not be calculated.");
	}
}

static autoMAT PowerCepstrum_getRhamonicsPower (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {
	try {
		autoMAT rhamonics;
		double  peakdB, qpeak;
		PowerCepstrum_getMaximumAndQuefrency (me, pitchFloor, pitchCeiling, kVector_peakInterpolation :: CUBIC, & peakdB, & qpeak);
		/*
			how many rhamonic peaks in interval [0, xmax] ?
		*/
		integer numberOfRhamonics = 2;
		while (qpeak > 0 && qpeak * numberOfRhamonics <= my xmax)
			numberOfRhamonics++;
		numberOfRhamonics --;
		rhamonics = zero_MAT (numberOfRhamonics, 4);
		for (integer rhamonic = 1; rhamonic <= numberOfRhamonics; rhamonic ++) {
			const double quefrency = rhamonic * qpeak;
			const double f = 1.0 / quefrency;
			const double flow = f * (1.0 - f0fractionalWidth);
			const double fhigh = f * (1.0 + f0fractionalWidth);
			const double qlow =  1.0 / fhigh;
			const double qhigh = ( f0fractionalWidth >= 1.0 ? my xmax : 1.0 / flow );
			rhamonics [rhamonic] [2] = qlow;
			rhamonics [rhamonic] [3] = quefrency;
			rhamonics [rhamonic] [4] = qhigh;
			integer iqmin, iqmax;
			Matrix_getWindowSamplesX (me, qlow, qhigh, & iqmin, & iqmax);
			for (integer iq = iqmin; iq <= iqmax; iq ++)
				rhamonics [rhamonic] [1] += my v_getValueAtSample (iq, 1, 0);
		}		
		return rhamonics;
	} catch (MelderError) {
		Melder_throw (me, U": rhamonics could not be calculated.");
	}
}

#if 0
static double PowerCepstrum_nearestPeak (PowerCepstrum me, double /*quefrency*/) {   // TODO: make global by providing prototype, and use
	autoMatrix thee = PowerCepstrum_as_Matrix_dB (me);
	
	return undefined;   // TODO: implement and use
}

static autoTable PowerCepstrum_to_Table_rhamonics (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {   // TODO: make global by providing prototype, and use
	try {
		autoMAT rhamonics = PowerCepstrum_getRhamonicsPower (me, pitchFloor, pitchCeiling, f0fractionalWidth);
		const conststring32 columnNames [] = { U"power", U"qlow", U"quefrency", U"qhigh" };
		autoTable thee = Table_createWithColumnNames (rhamonics.nrow, ARRAY_TO_STRVEC (columnNames));
		for (integer irow = 1; irow <= rhamonics.nrow; irow ++)
			for (integer icol = 1; icol <= rhamonics.ncol; icol ++)
				Table_setNumericValue (thee.get(), irow, icol, rhamonics [irow] [icol]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create Table with rhamonics.");
	}
}
#endif

double PowerCepstrum_getRNR (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {
	double peakdB, qpeak;
	PowerCepstrum_getMaximumAndQuefrency (me, pitchFloor, pitchCeiling, kVector_peakInterpolation :: CUBIC, & peakdB, & qpeak);
	integer imin, imax;
	const double qmin = 1.0 / pitchCeiling, qmax = 1.0 / pitchFloor;
	const integer ndata = Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax);
	if (ndata < 2)
		return undefined;	
	/*
		how many rhamonic peaks in interval [qmin, qmax] ?
	*/
	autoMAT rhamonics = PowerCepstrum_getRhamonicsPower (me, pitchFloor, pitchCeiling, f0fractionalWidth);
	if (rhamonics.nrow == 0)
		return undefined;
	longdouble sum = 0.0;
	for (integer i = imin; i <= imax; i ++)
		sum += my v_getValueAtSample (i, 1, 0);

	const double sumOfRhamonics = NUMsum (rhamonics.column (1));
	const double rnr = ( sumOfRhamonics >= sum ? 1000000.0 : double (sumOfRhamonics / (sum - sumOfRhamonics)) );
	return rnr;
}

double PowerCepstrum_getPeakProminence_hillenbrand (PowerCepstrum me, double pitchFloor, double pitchCeiling, double *out_qpeak) {
	double slope, intercept, quefrency, peakdB;
	PowerCepstrum_fitTrendLine (me, 0.001, 0, & slope, & intercept, kCepstrum_trendType::LINEAR, kCepstrum_trendFit::LEAST_SQUARES);
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_subtractTrendLine_inplace (thee.get(), slope, intercept, kCepstrum_trendType::LINEAR);
	PowerCepstrum_getMaximumAndQuefrency (thee.get(), pitchFloor, pitchCeiling, kVector_peakInterpolation :: NONE, & peakdB, & quefrency);
	if (out_qpeak)
		*out_qpeak = quefrency;
	return peakdB;
}

double PowerCepstrum_getPeakProminence (PowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod, double *out_qpeak) {
	double slope, intercept, qpeak, peakdB;
	PowerCepstrum_fitTrendLine (me, qstartFit, qendFit, & slope, & intercept, lineType, fitMethod);
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_subtractTrendLine_inplace (thee.get(), slope, intercept,lineType);
	PowerCepstrum_getMaximumAndQuefrency (thee.get(), pitchFloor, pitchCeiling, peakInterpolationType, & peakdB, & qpeak);
	if (out_qpeak)
		*out_qpeak = qpeak;
	return peakdB;
}

autoMatrix PowerCepstrum_to_Matrix (PowerCepstrum me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix created.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum (Matrix me) {
	try {
		Melder_require (my ny == 1,
			U"Matrix should have exactly 1 row.");
		autoPowerCepstrum thee = Thing_new (PowerCepstrum);
		my structMatrix :: v1_copy (thee.get());
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
		thy z.row (1)  <<=  my z.row (row);
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
		thy z.row (1)  <<=  my z.column (col);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrum created.");
	}
}

/* End of file PowerCepstrum.cpp */
