/* AmplitudeTier.cpp
 *
 * Copyright (C) 2003-2011,2014,2015,2016,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AmplitudeTier.h"

Thing_implement (AmplitudeTier, RealTier, 0);

autoAmplitudeTier AmplitudeTier_create (double tmin, double tmax) {
	try {
		autoAmplitudeTier me = Thing_new (AmplitudeTier);
		RealTier_init (me.get(), tmin, tmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"AmplitudeTier not created.");
	}
}

void AmplitudeTier_draw (AmplitudeTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, conststring32 method, bool garnish)
{
	RealTier_draw (me, g, tmin, tmax, ymin, ymax, garnish, method, U"Sound pressure (Pa)");
}

autoAmplitudeTier PointProcess_upto_AmplitudeTier (PointProcess me, double soundPressure) {
	try {
		autoAmplitudeTier thee = PointProcess_upto_RealTier (me, soundPressure, classAmplitudeTier).static_cast_move<structAmplitudeTier>();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to AmplitudeTier.");
	}
}

autoAmplitudeTier IntensityTier_to_AmplitudeTier (IntensityTier me) {
	try {
		autoAmplitudeTier thee = Thing_new (AmplitudeTier);
		my structRealTier :: v_copy (thee.get());
		for (integer i = 1; i <= thy points.size; i ++) {
			RealPoint point = thy points.at [i];
			point -> value = pow (10.0, point -> value / 20.0) * 2.0e-5;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to AmplitudeTier.");
	}
}

autoIntensityTier AmplitudeTier_to_IntensityTier (AmplitudeTier me, double threshold_dB) {
	try {
		double threshold_Pa = pow (10.0, threshold_dB / 20.0) * 2.0e-5;   // often zero!
		autoIntensityTier thee = Thing_new (IntensityTier);
		my structRealTier :: v_copy (thee.get());
		for (integer i = 1; i <= thy points.size; i ++) {
			RealPoint point = thy points.at [i];
			double absoluteValue = fabs (point -> value);
			point -> value = absoluteValue <= threshold_Pa ? threshold_dB : 20.0 * log10 (absoluteValue / 2.0e-5);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to IntensityTier.");
	}
}

autoTableOfReal AmplitudeTier_downto_TableOfReal (AmplitudeTier me) {
	return RealTier_downto_TableOfReal (me, U"Time (s)", U"Sound pressure (Pa)");
}

void Sound_AmplitudeTier_multiply_inplace (Sound me, AmplitudeTier amplitude) {
	if (amplitude -> points.size == 0) return;
	for (integer isamp = 1; isamp <= my nx; isamp ++) {
		double t = my x1 + (isamp - 1) * my dx;
		double factor = RealTier_getValueAtTime (amplitude, t);
		for (integer channel = 1; channel <= my ny; channel ++) {
			my z [channel] [isamp] *= factor;
		}
	}
}

autoSound Sound_AmplitudeTier_multiply (Sound me, AmplitudeTier amplitude) {
	try {
		autoSound thee = Data_copy (me);
		Sound_AmplitudeTier_multiply_inplace (thee.get(), amplitude);
		Vector_scale (thee.get(), 0.9);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not multiplied by ", amplitude, U".");
	}
}

autoAmplitudeTier PointProcess_Sound_to_AmplitudeTier_point (PointProcess me, Sound you) {
	try {
		const MelderIntegerRange peaks = PointProcess_getWindowPoints (me, my xmin, my xmax);
		if (peaks.size() < 3)
			return autoAmplitudeTier();
		autoAmplitudeTier him = AmplitudeTier_create (my xmin, my xmax);
		for (integer i = peaks.first; i <= peaks.last; i ++) {
			const double value = Vector_getValueAtX (you, my t [i], Vector_CHANNEL_AVERAGE, kVector_valueInterpolation :: SINC700);
			if (isdefined (value))
				RealTier_addPoint (him.get(), my t [i], value);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", you, U": not converted to AmplitudeTier.");
	}
}
/*
static double Sound_getPeak (Sound me, double tmin, double tmax, integer channel) {
	double *y = my z [channel];
	integer imin, imax;
	if (Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax) < 3) return undefined;
	double minimum = y [imin];
	double maximum = y [imin];
	integer sampleOfMinimum = imin;
	integer sampleOfMaximum = imin;
	for (integer i = imin + 1; i <= imax; i ++) {
		if (y [i] < minimum) { minimum = y [i]; sampleOfMinimum = i; }
		if (y [i] > maximum) { maximum = y [i]; sampleOfMaximum = i; }
	}
	double timeOfMinimum = my x1 + (sampleOfMinimum - 1) * my dx;
	double timeOfMaximum = my x1 + (sampleOfMaximum - 1) * my dx;
	Vector_getMinimumAndX (me, timeOfMinimum - my dx, timeOfMinimum + my dx, NUM_PEAK_INTERPOLATE_SINC70, & minimum, & timeOfMinimum);
	Vector_getMaximumAndX (me, timeOfMaximum - my dx, timeOfMaximum + my dx, NUM_PEAK_INTERPOLATE_SINC70, & maximum, & timeOfMaximum);
	return maximum - minimum;
}
*/
static double Sound_getHannWindowedRms (Sound me, double tmid, double widthLeft, double widthRight) {
	integer imin, imax;
	if (Sampled_getWindowSamples (me, tmid - widthLeft, tmid + widthRight, & imin, & imax) < 3)
		return undefined;
	longdouble sumOfSquares = 0.0, windowSumOfSquares = 0.0;
	for (integer i = imin; i <= imax; i ++) {
		double t = my x1 + (i - 1) * my dx;
		double width = t < tmid ? widthLeft : widthRight;
		double windowPhase = (t - tmid) / width;   /* in [-1 .. 1] */
		double window = 0.5 + 0.5 * cos (NUMpi * windowPhase);   /* Hann */
		double windowedValue = ( my ny == 1 ? my z [1] [i] : 0.5 * (my z [1] [i] + my z [2] [i]) ) * window;
		sumOfSquares += windowedValue * windowedValue;
		windowSumOfSquares += window * window;
	}
	return sqrt (double (sumOfSquares / windowSumOfSquares));
}
autoAmplitudeTier PointProcess_Sound_to_AmplitudeTier_period (PointProcess me, Sound you, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor)
{
	try {
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		const MelderIntegerRange peaks = PointProcess_getWindowPoints (me, tmin, tmax);
		if (peaks.size() < 3)
			Melder_throw (U"Too few pulses between ", tmin, U" and ", tmax, U" seconds.");
		autoAmplitudeTier him = AmplitudeTier_create (tmin, tmax);
		for (integer i = peaks.first + 1; i < peaks.last; i ++) {
			double p1 = my t [i] - my t [i - 1], p2 = my t [i + 1] - my t [i];
			double intervalFactor = p1 > p2 ? p1 / p2 : p2 / p1;
			if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax && intervalFactor <= maximumPeriodFactor)) {
				double peak = Sound_getHannWindowedRms (you, my t [i], 0.2 * p1, 0.2 * p2);
				if (isdefined (peak) && peak > 0.0)
					RealTier_addPoint (him.get(), my t [i], peak);
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", you, U": not converted to AmplitudeTier.");
	}
}
double AmplitudeTier_getShimmer_local (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	integer numberOfPeaks = 0;
	longdouble numerator = 0.0, denominator = 0.0;
	RealPoint *points = & my points.at [0];
	for (integer i = 2; i <= my points.size; i ++) {
		double p = points [i] -> number - points [i - 1] -> number;
		if (pmin == pmax || (p >= pmin && p <= pmax)) {
			double a1 = points [i - 1] -> value, a2 = points [i] -> value;
			double amplitudeFactor = a1 > a2 ? a1 / a2 : a2 / a1;
			if (amplitudeFactor <= maximumAmplitudeFactor) {
				numerator += fabs (a1 - a2);
				numberOfPeaks ++;
			}
		}
	}
	if (numberOfPeaks < 1) return undefined;
	numerator /= numberOfPeaks;
	numberOfPeaks = 0;
	for (integer i = 1; i < my points.size; i ++) {
		denominator += points [i] -> value;
		numberOfPeaks ++;
	}
	denominator /= numberOfPeaks;
	if (denominator == 0.0) return undefined;
	return double (numerator / denominator);
}
double AmplitudeTier_getShimmer_local_dB (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	integer numberOfPeaks = 0;
	longdouble result = 0.0;
	RealPoint *points = & my points.at [0];
	for (integer i = 2; i <= my points.size; i ++) {
		double p = points [i] -> number - points [i - 1] -> number;
		if (pmin == pmax || (p >= pmin && p <= pmax)) {
			double a1 = points [i - 1] -> value, a2 = points [i] -> value;
			double amplitudeFactor = a1 > a2 ? a1 / a2 : a2 / a1;
			if (amplitudeFactor <= maximumAmplitudeFactor) {
				result += fabs (log10 (a1 / a2));
				numberOfPeaks ++;
			}
		}
	}
	if (numberOfPeaks < 1) return undefined;
	result /= numberOfPeaks;
	return double (20.0 * result);
}
double AmplitudeTier_getShimmer_apq3 (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	integer numberOfPeaks = 0;
	longdouble numerator = 0.0, denominator = 0.0;
	RealPoint *points = & my points.at [0];
	for (integer i = 2; i <= my points.size - 1; i ++) {
		double
			p1 = points [i] -> number - points [i - 1] -> number,
			p2 = points [i + 1] -> number - points [i] -> number;
		if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax)) {
			double a1 = points [i - 1] -> value, a2 = points [i] -> value, a3 = points [i + 1] -> value;
			double f1 = a1 > a2 ? a1 / a2 : a2 / a1, f2 = a2 > a3 ? a2 / a3 : a3 / a2;
			if (f1 <= maximumAmplitudeFactor && f2 <= maximumAmplitudeFactor) {
				double threePointAverage = (a1 + a2 + a3) / 3.0;
				numerator += fabs (a2 - threePointAverage);
				numberOfPeaks ++;
			}
		}
	}
	if (numberOfPeaks < 1) return undefined;
	numerator /= numberOfPeaks;
	numberOfPeaks = 0;
	for (integer i = 1; i < my points.size; i ++) {
		denominator += points [i] -> value;
		numberOfPeaks ++;
	}
	denominator /= numberOfPeaks;
	if (denominator == 0.0) return undefined;
	return double (numerator / denominator);
}
double AmplitudeTier_getShimmer_apq5 (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	integer numberOfPeaks = 0;
	longdouble numerator = 0.0, denominator = 0.0;
	RealPoint *points = & my points.at [0];
	for (integer i = 3; i <= my points.size - 2; i ++) {
		double
			p1 = points [i - 1] -> number - points [i - 2] -> number,
			p2 = points [i] -> number - points [i - 1] -> number,
			p3 = points [i + 1] -> number - points [i] -> number,
			p4 = points [i + 2] -> number - points [i + 1] -> number;
		if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax
			&& p3 >= pmin && p3 <= pmax && p4 >= pmin && p4 <= pmax))
		{
			double a1 = points [i - 2] -> value, a2 = points [i - 1] -> value, a3 = points [i] -> value,
				a4 = points [i + 1] -> value, a5 = points [i + 2] -> value;
			double f1 = a1 > a2 ? a1 / a2 : a2 / a1, f2 = a2 > a3 ? a2 / a3 : a3 / a2,
				f3 = a3 > a4 ? a3 / a4 : a4 / a3, f4 = a4 > a5 ? a4 / a5 : a5 / a4;
			if (f1 <= maximumAmplitudeFactor && f2 <= maximumAmplitudeFactor &&
			    f3 <= maximumAmplitudeFactor && f4 <= maximumAmplitudeFactor)
			{
				double fivePointAverage = ((a1 + a2 + a3) + (a4 + a5)) / 5.0;
				numerator += fabs (a3 - fivePointAverage);
				numberOfPeaks ++;
			}
		}
	}
	if (numberOfPeaks < 1) return undefined;
	numerator /= numberOfPeaks;
	numberOfPeaks = 0;
	for (integer i = 1; i < my points.size; i ++) {
		denominator += points [i] -> value;
		numberOfPeaks ++;
	}
	denominator /= numberOfPeaks;
	if (denominator == 0.0) return undefined;
	return double (numerator / denominator);
}
double AmplitudeTier_getShimmer_apq11 (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	integer numberOfPeaks = 0;
	longdouble numerator = 0.0, denominator = 0.0;
	RealPoint *points = & my points.at [0];
	for (integer i = 6; i <= my points.size - 5; i ++) {
		double
			p1 = points [i - 4] -> number - points [i - 5] -> number,
			p2 = points [i - 3] -> number - points [i - 4] -> number,
			p3 = points [i - 2] -> number - points [i - 3] -> number,
			p4 = points [i - 1] -> number - points [i - 2] -> number,
			p5 = points [i] -> number - points [i - 1] -> number,
			p6 = points [i + 1] -> number - points [i] -> number,
			p7 = points [i + 2] -> number - points [i + 1] -> number,
			p8 = points [i + 3] -> number - points [i + 2] -> number,
			p9 = points [i + 4] -> number - points [i + 3] -> number,
			p10 = points [i + 5] -> number - points [i + 4] -> number;
		if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax
			&& p3 >= pmin && p3 <= pmax && p4 >= pmin && p4 <= pmax && p5 >= pmin && p5 <= pmax
			&& p6 >= pmin && p6 <= pmax && p7 >= pmin && p7 <= pmax && p8 >= pmin && p8 <= pmax
			&& p9 >= pmin && p9 <= pmax && p10 >= pmin && p10 <= pmax))
		{
			double a1 = points [i - 5] -> value, a2 = points [i - 4] -> value, a3 = points [i - 3] -> value,
				a4 = points [i - 2] -> value, a5 = points [i - 1] -> value, a6 = points [i] -> value,
				a7 = points [i + 1] -> value, a8 = points [i + 2] -> value, a9 = points [i + 3] -> value,
				a10 = points [i + 4] -> value, a11 = points [i + 5] -> value;
			double f1 = a1 > a2 ? a1 / a2 : a2 / a1, f2 = a2 > a3 ? a2 / a3 : a3 / a2,
				f3 = a3 > a4 ? a3 / a4 : a4 / a3, f4 = a4 > a5 ? a4 / a5 : a5 / a4,
				f5 = a5 > a6 ? a5 / a6 : a6 / a5, f6 = a6 > a7 ? a6 / a7 : a7 / a6,
				f7 = a7 > a8 ? a7 / a8 : a8 / a7, f8 = a8 > a9 ? a8 / a9 : a9 / a8,
				f9 = a9 > a10 ? a9 / a10 : a10 / a9, f10 = a10 > a11 ? a10 / a11 : a11 / a10;
			if (f1 <= maximumAmplitudeFactor && f2 <= maximumAmplitudeFactor &&
			    f3 <= maximumAmplitudeFactor && f4 <= maximumAmplitudeFactor &&
			    f5 <= maximumAmplitudeFactor && f6 <= maximumAmplitudeFactor &&
			    f7 <= maximumAmplitudeFactor && f8 <= maximumAmplitudeFactor &&
			    f9 <= maximumAmplitudeFactor && f10 <= maximumAmplitudeFactor)
			{
				double elevenPointAverage = (((a1 + a2 + a3) + (a4 + a5 + a6)) + ((a7 + a8 + a9) + (a10 + a11))) / 11.0;
				numerator += fabs (a6 - elevenPointAverage);
				numberOfPeaks ++;
			}
		}
	}
	if (numberOfPeaks < 1) return undefined;
	numerator /= numberOfPeaks;
	numberOfPeaks = 0;
	for (integer i = 1; i < my points.size; i ++) {
		denominator += points [i] -> value;
		numberOfPeaks ++;
	}
	denominator /= numberOfPeaks;
	if (denominator == 0.0) return undefined;
	return double (numerator / denominator);
}
double AmplitudeTier_getShimmer_dda (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	double apq3 = AmplitudeTier_getShimmer_apq3 (me, pmin, pmax, maximumAmplitudeFactor);
	return ( isdefined (apq3) ? 3.0 * apq3 : undefined );
}

autoSound AmplitudeTier_to_Sound (AmplitudeTier me, double samplingFrequency, integer interpolationDepth) {
	try {
		integer sound_nt = 1 + Melder_ifloor ((my xmax - my xmin) * samplingFrequency);   // >= 1
		double dt = 1.0 / samplingFrequency;
		double tmid = (my xmin + my xmax) / 2;
		double t1 = tmid - 0.5 * (sound_nt - 1) * dt;
		autoSound you = Sound_create (1, my xmin, my xmax, sound_nt, dt, t1);
		VEC sound = your z.row (1);
		for (integer it = 1; it <= my points.size; it ++) {
			RealPoint point = my points.at [it];
			double t = point -> number, amplitude = point -> value, angle, halfampsinangle;
			integer mid = Sampled_xToNearestIndex (you.get(), t), j;
			integer begin = mid - interpolationDepth, end = mid + interpolationDepth;
			if (begin < 1) begin = 1;
			if (end > your nx) end = your nx;
			angle = NUMpi * (Sampled_indexToX (you.get(), begin) - t) / your dx;
			halfampsinangle = 0.5 * amplitude * sin (angle);
			for (j = begin; j <= end; j ++) {
				if (fabs (angle) < 1e-6)
					sound [j] += amplitude;
				else if (angle < 0.0)
					sound [j] += halfampsinangle *
						(1.0 + cos (angle / (mid - begin + 1))) / angle;
				else
					sound [j] += halfampsinangle *
						(1.0 + cos (angle / (end - mid + 1))) / angle;
				angle += NUMpi;
				halfampsinangle = - halfampsinangle;
			}
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

/* End of file AmplitudeTier.cpp */
