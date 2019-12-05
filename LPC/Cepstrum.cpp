/* Cepstrum.cpp
 *
 * Copyright (C) 1994-2019 David Weenink
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

/*
 djmw 20010514
 djmw 20020812 GPL header
 djmw 20080122 Version 1: float -> double
 djmw 20110304 Thing_new
*/

#include "Cepstrum.h"
#include "NUM2.h"
#include "Vector.h"

Thing_implement (Cepstrum, Matrix, 2);
Thing_implement (PowerCepstrum, Cepstrum, 2); // derives from Matrix therefore also version 2

double structCepstrum :: v_getValueAtSample (integer isamp, integer which, int /* units */) {
	if (which == 0) {
		return z [1] [isamp];
	} else {
		// dB's
		return 20.0 * log10 (fabs (z [1] [isamp]) + 1e-30);
	}
	return undefined;
}

double structPowerCepstrum :: v_getValueAtSample (integer isamp, integer which, int /* units */) {
	if (which == 0) {
		return z [1] [isamp];
	} else {
		// dB's
		return 10.0 * log10 (z [1] [isamp] + 1e-30); // always positive
	}
	return undefined;
}

autoCepstrum Cepstrum_create (double qmax, integer nq) {
	try {
		autoCepstrum me = Thing_new (Cepstrum);
		double dq = qmax / (nq - 1);

		Matrix_init (me.get(), 0.0, qmax, nq, dq, 0.0, 1.0, 1.0, 1, 1, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cepstrum not created.");
	}
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

static void _Cepstrum_draw (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, int power, bool garnish) {
	int autoscaling = minimum >= maximum;

	Graphics_setInner (g);

	if (qmax <= qmin) {
		qmin = my xmin;
		qmax = my xmax;
	}

	integer imin, imax;
	if (Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax) == 0)
		return;
	integer numberOfSelected = imax - imin + 1;
	autoVEC y = newVECraw (numberOfSelected);

	for (integer i = 1; i <= numberOfSelected; i ++)
		y [i] = my v_getValueAtSample (imin + i - 1, (power ? 1 : 0), 0);

	if (autoscaling)
		NUMextrema (y.get(), & minimum, & maximum);
	else
		VECclip_inplace_inline (y.get(), minimum, maximum);

	Graphics_setWindow (g, qmin, qmax, minimum, maximum);
	Graphics_function (g, y.at, 1, numberOfSelected, Matrix_columnToX (me, imin), Matrix_columnToX (me, imax));

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Quefrency (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textLeft (g, true, power ? U"Amplitude (dB)" : U"Amplitude");
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void Cepstrum_drawLinear (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, bool garnish) {
	_Cepstrum_draw (me, g, qmin, qmax, minimum, maximum, 0, garnish);
}

void PowerCepstrum_draw (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, bool garnish) {
	_Cepstrum_draw (me, g, qmin, qmax, dBminimum, dBmaximum, 1, garnish);
}

void PowerCepstrum_drawTrendLine (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, double qstart, double qend, kCepstrumTrendType lineType, kCepstrumTrendFit method) {

	Graphics_setInner (g);

	if (qmax <= qmin) {
		qmin = my xmin;
		qmax = my xmax;
	}

	if (dBminimum >= dBmaximum) { // autoscaling
		integer imin, imax;
		if (Matrix_getWindowSamplesX (me, qmin, qmax, & imin, & imax) == 0)
			return;
		integer numberOfPoints = imax - imin + 1;
		dBminimum = dBmaximum = my v_getValueAtSample (imin, 1, 0);
		for (integer i = 2; i <= numberOfPoints; i ++) {
			integer isamp = imin + i - 1;
			double y = my v_getValueAtSample (isamp, 1, 0);
			dBmaximum = y > dBmaximum ? y : dBmaximum;
			dBminimum = y < dBminimum ? y : dBminimum;
		}
	}

	Graphics_setWindow (g, qmin, qmax, dBminimum, dBmaximum);
	qend = qend == 0 ? my xmax : qend;
	if (qend <= qstart) {
		qend = my xmax;
		qstart = my xmin;
	}
	qstart = qstart < my xmin ? my xmin : qstart;
	qend = qend > my xmax ? my xmax : qend;

	double a, intercept;
	PowerCepstrum_fitTrendLine (me, qstart, qend, & a, & intercept, lineType, method);
	/*
	 * Don't draw part outside window
	 */
	double lineWidth =  Graphics_inqLineWidth (g);
	Graphics_setLineWidth (g, 2);
	if (lineType == kCepstrumTrendType::ExponentialDecay ) {
		integer n = 500;
		double dq = (qend - qstart) / (n + 1);
		double q1 = qstart;
		if (qstart <= 0) {
			qstart = 0.1 * dq; // some small offset to avoid log(0)
			n--; 
		}
		autoVEC y = newVECraw (n);
		
		for (integer i = 1; i <= n; i ++) {
			double q = q1 + (i - 1) * dq;
			y [i] = a * log (q) + intercept;
		}
		Graphics_function (g, y.at, 1, n, qstart, qend);
	} else {
		double y1 = a * qstart + intercept, y2 = a * qend + intercept;
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
		if (imin == 1 && lineType == kCepstrumTrendType::ExponentialDecay)
			imin = 2; // because log(0) is undefined
		integer numberOfPoints = imax - imin + 1;
		Melder_require (numberOfPoints > 1,
			U"Not enough points for fit.");

		autoVEC y = newVECraw (numberOfPoints);
		autoVEC x = newVECraw (numberOfPoints);
		for (integer i = 1; i <= numberOfPoints; i ++) {
			integer isamp = imin + i - 1;
			x [i] = my x1 + (isamp - 1) * my dx;
			if (lineType == kCepstrumTrendType::ExponentialDecay)
				x [i] = log (x [i]);
			y [i] = my v_getValueAtSample (isamp, 1, 0);
		}
		if (method == kCepstrumTrendFit::LeastSquares)
			NUMlineFit_LS (x.get(), y.get(), & a, & intercept);
		else if (method == kCepstrumTrendFit::RobustFast)
			NUMlineFit_theil (x.get(), y.get(), & a, & intercept, false);
		else if (method == kCepstrumTrendFit::RobustSlow)
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
		double db_cepstrum = my v_getValueAtSample (j, 1, 0);
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
		double xq = lineType == kCepstrumTrendType::ExponentialDecay ? log(q) : q;
		double db_background = slope * xq + intercept;
		double db_cepstrum = my v_getValueAtSample (j, 1, 0);
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

void PowerCepstrum_smooth_inplace (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	try {
		integer numberOfQuefrencyBins = Melder_ifloor (quefrencyAveragingWindow / my dx);
		if (numberOfQuefrencyBins > 1) {
			autoVEC qin = newVECcopy (my z.row (1));
			autoVEC qout = newVECraw (my nx);
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

autoPowerCepstrum PowerCepstrum_smooth (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_smooth_inplace (thee.get(), quefrencyAveragingWindow, numberOfIterations);
	return thee;
}

void PowerCepstrum_getMaximumAndQuefrency (PowerCepstrum me, double pitchFloor, double pitchCeiling, int interpolation, double *out_peakdB, double *out_quefrency) {
	autoPowerCepstrum thee = Data_copy (me);
	double lowestQuefrency = 1.0 / pitchCeiling, highestQuefrency = 1.0 / pitchFloor;
	for (integer i = 1; i <= my nx; i ++) {
		thy z [1] [i] = my v_getValueAtSample (i, 1, 0); // 10 log val^2
	}
	double peakdB, quefrency;
	Vector_getMaximumAndX ((Vector) thee.get(), lowestQuefrency, highestQuefrency, 1, interpolation, & peakdB, & quefrency);   // FIXME cast
	if (out_peakdB)
		*out_peakdB = peakdB;
	if (out_quefrency)
		*out_quefrency = quefrency;
}

double PowerCepstrum_getRNR (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {
	double rnr = undefined;
	double qmin = 1.0 / pitchCeiling, qmax = 1.0 / pitchFloor, peakdB, qpeak;
	PowerCepstrum_getMaximumAndQuefrency (me, pitchFloor, pitchCeiling, 2, & peakdB, & qpeak);
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
		double val = my v_getValueAtSample (i, 0, 0);
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
	PowerCepstrum_fitTrendLine (me, 0.001, 0, & slope, & intercept, kCepstrumTrendType::Linear, kCepstrumTrendFit::LeastSquares);
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_subtractTrendLine_inplace (thee.get(), slope, intercept, kCepstrumTrendType::Linear);
	PowerCepstrum_getMaximumAndQuefrency (thee.get(), pitchFloor, pitchCeiling, 0, & peakdB, & quefrency);
	if (out_qpeak)
		*out_qpeak = quefrency;
	return peakdB;
}

double PowerCepstrum_getPeakProminence (PowerCepstrum me, double pitchFloor, double pitchCeiling, int interpolation, double qstartFit, double qendFit, kCepstrumTrendType  lineType, kCepstrumTrendFit fitMethod, double *out_qpeak) {
	double slope, intercept, qpeak, peakdB;
	PowerCepstrum_fitTrendLine (me, qstartFit, qendFit, & slope, & intercept, lineType, fitMethod);
	PowerCepstrum_getMaximumAndQuefrency (me, pitchFloor, pitchCeiling, interpolation, & peakdB, & qpeak);
	double xq = lineType == kCepstrumTrendType::ExponentialDecay ? log(qpeak) : qpeak;
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

/* End of file Cepstrum.cpp */
