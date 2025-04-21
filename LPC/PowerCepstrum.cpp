/* PowerCepstrum.cpp
 *
 * Copyright (C) 2012-2025 David Weenink
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
#include "PowerCepstrumWorkspace.h"
#include "NUM2.h"

Thing_implement (PowerCepstrum, Cepstrum, 2);   // derives from Matrix; therefore also version 2

double structPowerCepstrum :: v_getValueAtSample (integer isamp, integer row, int units) const {
	double result = undefined;
	if (row == 1) {
		if (units == 0)
			result = z [1] [isamp];
		else
			result = 10.0 * log10 (z [1] [isamp] + 1e-30); // result >= -300
	}
	return result;
}

autoPowerCepstrum Cepstrum_downto_PowerCepstrum (constCepstrum me) {
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

void PowerCepstrum_draw (constPowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, bool garnish) {
	Cepstrum_draw (me, g, qmin, qmax, dBminimum, dBmaximum, true, garnish);
}

void PowerCepstrum_fitTrendLine (constPowerCepstrum me, double qmin, double qmax, double *out_slope, double *out_intercept, kCepstrum_trendType lineType, kCepstrum_trendFit method) {
	autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (me, qmin, qmax, lineType, method);
	ws -> getSlopeAndIntercept ();
	if (out_slope) 
		*out_slope = ws -> slope;
	if (out_intercept)
		*out_intercept = ws -> intercept;
}

void PowerCepstrum_drawTrendLine (constPowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum,
	double qstart, double qend, kCepstrum_trendType lineType, kCepstrum_trendFit method)
{
	Function_unidirectionalAutowindow (me, & qmin, & qmax);
	
	autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (me, qstart, qend, lineType, method);
	ws -> getSlopeAndIntercept();

	if (dBminimum >= dBmaximum) {   // autoscaling
		MelderExtremaWithInit extrema_db;
		for (integer i = ws -> imin; i <= ws -> imax; i ++)
			extrema_db.update (ws -> y[i]);
		dBmaximum = extrema_db.max;
		dBminimum = extrema_db.min;
	}
	
	Graphics_setInner (g);
	Graphics_setWindow (g, qmin, qmax, dBminimum, dBmaximum);
	const double slope = ws -> slope, intercept = ws -> intercept;
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
			// don't draw anything below lower limit
		}
	}
	Graphics_setLineWidth (g, lineWidth);
	Graphics_unsetInner (g);
}

double PowerCepstrum_getTrendLineValue (constPowerCepstrum me, double quefrency, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	double trenddb = undefined;
	if (quefrency >= my xmin && quefrency <= my xmax) {
		autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (me, qstartFit, qendFit, lineType, fitMethod);
		trenddb = ws -> getTrend (quefrency);
	}
	return trenddb;
}

void PowerCepstrum_subtractTrend_inplace (mutablePowerCepstrum me, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (me, qstartFit, qendFit, lineType, fitMethod);
	ws -> subtractTrend ();
}

autoPowerCepstrum PowerCepstrum_subtractTrend (constPowerCepstrum me, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod) {
	try {
		autoPowerCepstrum thee = Data_copy (me);
		PowerCepstrum_subtractTrend_inplace (thee.get(), qstartFit, qendFit, lineType, fitMethod);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": couldn't subtract trend line.");
	}
}

static void PowerCepstrum_smooth_inplaceRectangular (mutablePowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
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

static void PowerCepstrum_smooth_inplaceRectangular_old (mutablePowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
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

static void PowerCepstrum_smooth_inplaceGaussian (mutablePowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
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

void PowerCepstrum_smooth_inplace (mutablePowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	if (Melder_debug == -4)
		PowerCepstrum_smooth_inplaceRectangular_old (me, quefrencyAveragingWindow, numberOfIterations);
	else if (Melder_debug == -5)
		PowerCepstrum_smooth_inplaceGaussian (me, quefrencyAveragingWindow, numberOfIterations);
	else
		PowerCepstrum_smooth_inplaceRectangular (me, quefrencyAveragingWindow, numberOfIterations);
}

autoPowerCepstrum PowerCepstrum_smooth (constPowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations) {
	autoPowerCepstrum thee = Data_copy (me);
	PowerCepstrum_smooth_inplace (thee.get(), quefrencyAveragingWindow, numberOfIterations);
	return thee;
}


void PowerCepstrum_getMaximumAndQuefrency_q (constPowerCepstrum me, double qminSearchInterval, double qmaxSearchInterval,
	kCepstrum_peakInterpolation peakInterpolationType, double& peakdB, double& quefrency)
{
	kVector_peakInterpolation interpolation = ( peakInterpolationType == kCepstrum_peakInterpolation :: PARABOLIC ?
		kVector_peakInterpolation :: PARABOLIC : (peakInterpolationType == kCepstrum_peakInterpolation :: CUBIC ?
		kVector_peakInterpolation :: CUBIC : kVector_peakInterpolation :: NONE)
	);
	autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (me, qminSearchInterval, qmaxSearchInterval, kCepstrum_trendType::LINEAR,
		kCepstrum_trendFit::ROBUST_FAST);
	PowerCepstrumWorkspace_initPeakSearchPart (ws.get(), qminSearchInterval, qmaxSearchInterval, interpolation);
	ws -> getPeakAndPosition();
	peakdB = ws -> peakdB;
	quefrency = ws -> peakQuefrency;	
}

//TODO
void PowerCepstrum_getMaximumAndQuefrency_pitch (constPowerCepstrum me, double pitchFloor, double pitchCeiling,
	kVector_peakInterpolation peakInterpolationType, double& peakdB, double& quefrency)
{
	const double qminSearchInterval = 1.0 / pitchCeiling;
	const double qmaxSearchInterval = 1.0 / pitchFloor;
	autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (me, qminSearchInterval, qmaxSearchInterval, kCepstrum_trendType::LINEAR,
		kCepstrum_trendFit::ROBUST_FAST);
	PowerCepstrumWorkspace_initPeakSearchPart (ws.get(), qminSearchInterval, qmaxSearchInterval, peakInterpolationType);
	ws -> getPeakAndPosition();
	peakdB = ws -> peakdB;
	quefrency = ws -> peakQuefrency;	
}

autoTable PowerCepstrum_tabulateRhamonics (constPowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType) {
	try {
		const double qminSearchInterval = 1.0 / pitchCeiling;
		const double qmaxSearchInterval = 1.0 / pitchFloor;
		autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (me, qminSearchInterval, qmaxSearchInterval, kCepstrum_trendType::LINEAR,
			kCepstrum_trendFit::ROBUST_FAST);

		PowerCepstrumWorkspace_initPeakSearchPart (ws.get(), qminSearchInterval, qmaxSearchInterval, peakInterpolationType);
		ws -> getRhamonicPeaks (qminSearchInterval, qmaxSearchInterval);
		
		const conststring32 columnNames [] = { U"peak(dB)", U"quefrency(s)" };
		autoTable thee = Table_createWithColumnNames (ws -> numberOfRhamonics, ARRAY_TO_STRVEC (columnNames));

		for (integer rhamonic = 1; rhamonic <= ws -> numberOfRhamonics; rhamonic ++) {
			Table_setNumericValue (thee.get(), rhamonic, 1, ws -> rhamonics [rhamonic][1]);
			Table_setNumericValue (thee.get(), rhamonic, 2, ws -> rhamonics [rhamonic][2]);
		}		
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rhamonics could not be tabulated.");
	}
}

static autoMAT PowerCepstrum_getRhamonicsPower (constPowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {
	try {
		const double qminSearchInterval = 1.0 / pitchCeiling;
		const double qmaxSearchInterval = 1.0 / pitchFloor;
		autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (me, qminSearchInterval, qmaxSearchInterval, kCepstrum_trendType::LINEAR,
			kCepstrum_trendFit::ROBUST_FAST);
		PowerCepstrumWorkspace_initPeakSearchPart (ws.get(), qminSearchInterval, qmaxSearchInterval, kVector_peakInterpolation :: CUBIC);
		ws -> getRhamonicsPower (qminSearchInterval, qmaxSearchInterval, f0fractionalWidth);
		autoMAT result = copy_MAT (ws -> rhamonics.get());
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": rhamonics could not be calculated.");
	}
}

double PowerCepstrum_getRNR (constPowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth) {
	const double qminSearchInterval = 1.0 / pitchCeiling;
	const double qmaxSearchInterval = 1.0 / pitchFloor;
	autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (me, qminSearchInterval, qmaxSearchInterval, kCepstrum_trendType::LINEAR,
		kCepstrum_trendFit::ROBUST_FAST);
	PowerCepstrumWorkspace_initPeakSearchPart (ws.get(), qminSearchInterval, qmaxSearchInterval, kVector_peakInterpolation :: CUBIC);
	const double rnr = ws -> getRNR (qminSearchInterval, qmaxSearchInterval, f0fractionalWidth);
	return rnr;
}

double PowerCepstrum_getPeakProminence_hillenbrand (constPowerCepstrum me, double pitchFloor, double pitchCeiling, double& qpeak) {
	double slope, intercept, peakdB;
	autoPowerCepstrum thee = Data_copy (me);
	autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (thee.get(), 0.001, my xmax, kCepstrum_trendType::LINEAR, kCepstrum_trendFit::LEAST_SQUARES);
	const double qmaxSearchInterval = 1.0 / pitchFloor, qminSearchInterval = 1.0 /pitchCeiling;
	PowerCepstrumWorkspace_initPeakSearchPart (ws.get(), qminSearchInterval,qmaxSearchInterval, kVector_peakInterpolation :: NONE);
	ws -> getCPP ();
	qpeak = ws -> peakQuefrency;
	return ws -> cpp;
	return peakdB;
}

double PowerCepstrum_getPeakProminence (constPowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType,
	double qstartFit, double qendFit, kCepstrum_trendType trendLineType, kCepstrum_trendFit fitMethod, double& qpeak)
{
	autoPowerCepstrum thee = Data_copy (me);
	autoPowerCepstrumWorkspace ws = PowerCepstrumWorkspace_create (thee.get(), qstartFit, qendFit, trendLineType, fitMethod);
	const double qmaxSearchInterval = 1.0 / pitchFloor, qminSearchInterval = 1.0 /pitchCeiling;
	PowerCepstrumWorkspace_initPeakSearchPart (ws.get(), qminSearchInterval,qmaxSearchInterval, peakInterpolationType);
	ws -> getCPP ();
	qpeak = ws -> peakQuefrency;
	return ws -> cpp;
}

autoMatrix PowerCepstrum_to_Matrix (constPowerCepstrum me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix created.");
	}
}

autoPowerCepstrum Matrix_to_PowerCepstrum (constMatrix me) {
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

autoPowerCepstrum Matrix_to_PowerCepstrum_row (constMatrix me, integer row) {
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

autoPowerCepstrum Matrix_to_PowerCepstrum_column (constMatrix me, integer col) {
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
