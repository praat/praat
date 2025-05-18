/* AmplitudeTier.cpp
 *
 * Copyright (C) 2003-2012,2014-2022,2024,2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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
		autoAmplitudeTier thee = PointProcess_upto_RealTier (me, soundPressure, classAmplitudeTier).static_cast_move <structAmplitudeTier>();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to AmplitudeTier.");
	}
}

autoAmplitudeTier RealTier_to_AmplitudeTier (RealTier me) {
	try {
		autoAmplitudeTier thee = Thing_new (AmplitudeTier);
		my structRealTier :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to AmplitudeTier.");
	}
}

autoAmplitudeTier IntensityTier_to_AmplitudeTier (IntensityTier me) {
	try {
		autoAmplitudeTier thee = Thing_new (AmplitudeTier);
		my structRealTier :: v1_copy (thee.get());
		for (integer i = 1; i <= thy points.size; i ++) {
			const mutableRealPoint point = thy points.at [i];
			point -> value = pow (10.0, point -> value / 20.0) * 2.0e-5;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to AmplitudeTier.");
	}
}

autoIntensityTier AmplitudeTier_to_IntensityTier (AmplitudeTier me, double threshold_dB) {
	try {
		const double threshold_Pa = pow (10.0, threshold_dB / 20.0) * 2.0e-5;   // often zero!
		autoIntensityTier thee = Thing_new (IntensityTier);
		my structRealTier :: v1_copy (thee.get());
		for (integer i = 1; i <= thy points.size; i ++) {
			const mutableRealPoint point = thy points.at [i];
			const double absoluteValue = fabs (point -> value);
			point -> value = ( absoluteValue <= threshold_Pa ? threshold_dB : 20.0 * log10 (absoluteValue / 2.0e-5) );
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
	if (amplitude -> points.size == 0)
		return;
	for (integer isamp = 1; isamp <= my nx; isamp ++) {
		const double t = my x1 + (isamp - 1) * my dx;
		const double factor = RealTier_getValueAtTime (amplitude, t);
		for (integer channel = 1; channel <= my ny; channel ++)
			my z [channel] [isamp] *= factor;
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
		const double t = my x1 + (i - 1) * my dx;
		const double width = t < tmid ? widthLeft : widthRight;
		const double windowPhase = (t - tmid) / width;   /* in [-1 .. 1] */
		const double window = 0.5 + 0.5 * cos (NUMpi * windowPhase);   // Hann
		const double windowedValue = ( my ny == 1 ? my z [1] [i] : 0.5 * (my z [1] [i] + my z [2] [i]) ) * window;
		sumOfSquares += sqr (windowedValue);
		windowSumOfSquares += sqr (window);
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
			const double p1 = my t [i] - my t [i - 1];
			const double p2 = my t [i + 1] - my t [i];
			const double intervalFactor = ( p1 > p2 ? p1 / p2 : p2 / p1 );
			if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax && intervalFactor <= maximumPeriodFactor)) {
				const double peak = Sound_getHannWindowedRms (you, my t [i], 0.2 * p1, 0.2 * p2);
				if (isdefined (peak) && peak > 0.0)
					RealTier_addPoint (him.get(), my t [i], peak);
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", you, U": not converted to AmplitudeTier.");
	}
}
double AmplitudeTier_getShimmer_local_u (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	/* mutable count */ integer numberOfPeaks = 0;
	/* mutable accumulate */ longdouble numerator = 0.0, denominator = 0.0;
	RealPoint *points = & my points.at [0];
	for (integer i = 2; i <= my points.size; i ++) {
		const double p = points [i] -> number - points [i - 1] -> number;
		if (pmin == pmax || (p >= pmin && p <= pmax)) {
			const double a1 = points [i - 1] -> value, a2 = points [i] -> value;
			const double amplitudeFactor = ( a1 > a2 ? a1 / a2 : a2 / a1 );
			if (amplitudeFactor <= maximumAmplitudeFactor) {
				numerator += fabs (a1 - a2);
				numberOfPeaks ++;
			}
		}
	}
	if (numberOfPeaks < 1)
		return undefined;
	numerator /= numberOfPeaks;
	numberOfPeaks = 0;
	for (integer i = 1; i < my points.size; i ++) {
		denominator += points [i] -> value;
		numberOfPeaks ++;
	}
	denominator /= numberOfPeaks;
	if (denominator == 0.0)
		return undefined;
	return double (numerator / denominator);
}
double AmplitudeTier_getShimmer_local_dB_u (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	/* mutable count */ integer numberOfPeaks = 0;
	/* mutable accumulate */ longdouble result = 0.0;
	RealPoint *points = & my points.at [0];
	for (integer i = 2; i <= my points.size; i ++) {
		const double p = points [i] -> number - points [i - 1] -> number;
		if (pmin == pmax || (p >= pmin && p <= pmax)) {
			const double a1 = points [i - 1] -> value, a2 = points [i] -> value;
			const double amplitudeFactor = ( a1 > a2 ? a1 / a2 : a2 / a1 );
			if (amplitudeFactor <= maximumAmplitudeFactor) {
				result += fabs (log10 (a1 / a2));
				numberOfPeaks ++;
			}
		}
	}
	if (numberOfPeaks < 1)
		return undefined;
	result /= numberOfPeaks;
	return double (20.0 * result);
}
double AmplitudeTier_getShimmer_apq3_u (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	/* mutable count */ integer numberOfPeaks = 0;
	/* mutable accumulate */ longdouble numerator = 0.0, denominator = 0.0;
	RealPoint *points = & my points.at [0];
	for (integer i = 2; i <= my points.size - 1; i ++) {
		const double p1 = points [i] -> number - points [i - 1] -> number;
		const double p2 = points [i + 1] -> number - points [i] -> number;
		if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax)) {
			const double a1 = points [i - 1] -> value;
			const double a2 = points [i] -> value;
			const double a3 = points [i + 1] -> value;
			const double f1 = ( a1 > a2 ? a1 / a2 : a2 / a1 );
			const double f2 = ( a2 > a3 ? a2 / a3 : a3 / a2 );
			if (f1 <= maximumAmplitudeFactor && f2 <= maximumAmplitudeFactor) {
				const double threePointAverage = (a1 + a2 + a3) / 3.0;
				numerator += fabs (a2 - threePointAverage);
				numberOfPeaks ++;
			}
		}
	}
	if (numberOfPeaks < 1)
		return undefined;
	numerator /= numberOfPeaks;
	numberOfPeaks = 0;
	for (integer i = 1; i < my points.size; i ++) {
		denominator += points [i] -> value;
		numberOfPeaks ++;
	}
	denominator /= numberOfPeaks;
	if (denominator == 0.0)
		return undefined;
	return double (numerator / denominator);
}
double AmplitudeTier_getShimmer_apq5_u (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	/* mutable count */ integer numberOfPeaks = 0;
	/* mutable accumulate */ longdouble numerator = 0.0, denominator = 0.0;
	RealPoint *points = & my points.at [0];
	for (integer i = 3; i <= my points.size - 2; i ++) {
		const double p1 = points [i - 1] -> number - points [i - 2] -> number;
		const double p2 = points [i] -> number - points [i - 1] -> number;
		const double p3 = points [i + 1] -> number - points [i] -> number;
		const double p4 = points [i + 2] -> number - points [i + 1] -> number;
		if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax
			&& p3 >= pmin && p3 <= pmax && p4 >= pmin && p4 <= pmax))
		{
			const double a1 = points [i - 2] -> value;
			const double a2 = points [i - 1] -> value;
			const double a3 = points [i] -> value;
			const double a4 = points [i + 1] -> value;
			const double a5 = points [i + 2] -> value;
			const double f1 = ( a1 > a2 ? a1 / a2 : a2 / a1 );
			const double f2 = ( a2 > a3 ? a2 / a3 : a3 / a2 );
			const double f3 = ( a3 > a4 ? a3 / a4 : a4 / a3 );
			const double f4 = ( a4 > a5 ? a4 / a5 : a5 / a4 );
			if (f1 <= maximumAmplitudeFactor && f2 <= maximumAmplitudeFactor &&
			    f3 <= maximumAmplitudeFactor && f4 <= maximumAmplitudeFactor)
			{
				const double fivePointAverage = ((a1 + a2 + a3) + (a4 + a5)) / 5.0;
				numerator += fabs (a3 - fivePointAverage);
				numberOfPeaks ++;
			}
		}
	}
	if (numberOfPeaks < 1)
		return undefined;
	numerator /= numberOfPeaks;
	numberOfPeaks = 0;
	for (integer i = 1; i < my points.size; i ++) {
		denominator += points [i] -> value;
		numberOfPeaks ++;
	}
	denominator /= numberOfPeaks;
	if (denominator == 0.0)
		return undefined;
	return double (numerator / denominator);
}
double AmplitudeTier_getShimmer_apq11_u (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	/* mutable count */ integer numberOfPeaks = 0;
	/* mutable accumulate */ longdouble numerator = 0.0, denominator = 0.0;
	RealPoint *points = & my points.at [0];
	for (integer i = 6; i <= my points.size - 5; i ++) {
		const double p1 = points [i - 4] -> number - points [i - 5] -> number;
		const double p2 = points [i - 3] -> number - points [i - 4] -> number;
		const double p3 = points [i - 2] -> number - points [i - 3] -> number;
		const double p4 = points [i - 1] -> number - points [i - 2] -> number;
		const double p5 = points [i] -> number - points [i - 1] -> number;
		const double p6 = points [i + 1] -> number - points [i] -> number;
		const double p7 = points [i + 2] -> number - points [i + 1] -> number;
		const double p8 = points [i + 3] -> number - points [i + 2] -> number;
		const double p9 = points [i + 4] -> number - points [i + 3] -> number;
		const double p10 = points [i + 5] -> number - points [i + 4] -> number;
		if (pmin == pmax || (p1 >= pmin && p1 <= pmax && p2 >= pmin && p2 <= pmax
			&& p3 >= pmin && p3 <= pmax && p4 >= pmin && p4 <= pmax && p5 >= pmin && p5 <= pmax
			&& p6 >= pmin && p6 <= pmax && p7 >= pmin && p7 <= pmax && p8 >= pmin && p8 <= pmax
			&& p9 >= pmin && p9 <= pmax && p10 >= pmin && p10 <= pmax))
		{
			const double a1 = points [i - 5] -> value;
			const double a2 = points [i - 4] -> value;
			const double a3 = points [i - 3] -> value;
			const double a4 = points [i - 2] -> value;
			const double a5 = points [i - 1] -> value;
			const double a6 = points [i] -> value;
			const double a7 = points [i + 1] -> value;
			const double a8 = points [i + 2] -> value;
			const double a9 = points [i + 3] -> value;
			const double a10 = points [i + 4] -> value;
			const double a11 = points [i + 5] -> value;
			const double f1 = ( a1 > a2 ? a1 / a2 : a2 / a1 );
			const double f2 = ( a2 > a3 ? a2 / a3 : a3 / a2 );
			const double f3 = ( a3 > a4 ? a3 / a4 : a4 / a3 );
			const double f4 = ( a4 > a5 ? a4 / a5 : a5 / a4 );
			const double f5 = ( a5 > a6 ? a5 / a6 : a6 / a5 );
			const double f6 = ( a6 > a7 ? a6 / a7 : a7 / a6 );
			const double f7 = ( a7 > a8 ? a7 / a8 : a8 / a7 );
			const double f8 = ( a8 > a9 ? a8 / a9 : a9 / a8 );
			const double f9 = ( a9 > a10 ? a9 / a10 : a10 / a9 );
			const double f10 = ( a10 > a11 ? a10 / a11 : a11 / a10 );
			if (f1 <= maximumAmplitudeFactor && f2 <= maximumAmplitudeFactor &&
			    f3 <= maximumAmplitudeFactor && f4 <= maximumAmplitudeFactor &&
			    f5 <= maximumAmplitudeFactor && f6 <= maximumAmplitudeFactor &&
			    f7 <= maximumAmplitudeFactor && f8 <= maximumAmplitudeFactor &&
			    f9 <= maximumAmplitudeFactor && f10 <= maximumAmplitudeFactor)
			{
				const double elevenPointAverage = (((a1 + a2 + a3) + (a4 + a5 + a6)) + ((a7 + a8 + a9) + (a10 + a11))) / 11.0;
				numerator += fabs (a6 - elevenPointAverage);
				numberOfPeaks ++;
			}
		}
	}
	if (numberOfPeaks < 1)
		return undefined;
	numerator /= numberOfPeaks;
	numberOfPeaks = 0;
	for (integer i = 1; i < my points.size; i ++) {
		denominator += points [i] -> value;
		numberOfPeaks ++;
	}
	denominator /= numberOfPeaks;
	if (denominator == 0.0)
		return undefined;
	return double (numerator / denominator);
}
double AmplitudeTier_getShimmer_dda_u (AmplitudeTier me, double pmin, double pmax, double maximumAmplitudeFactor) {
	const double apq3 = AmplitudeTier_getShimmer_apq3_u (me, pmin, pmax, maximumAmplitudeFactor);
	return ( isdefined (apq3) ? 3.0 * apq3 : undefined );
}

autoSound AmplitudeTier_to_Sound (AmplitudeTier me, double samplingFrequency, integer interpolationDepth) {
	try {
		const integer sound_nt = 1 + Melder_ifloor ((my xmax - my xmin) * samplingFrequency);   // >= 1
		const double dt = 1.0 / samplingFrequency;
		const double tmid = (my xmin + my xmax) / 2;
		const double t1 = tmid - 0.5 * (sound_nt - 1) * dt;
		autoSound you = Sound_create (1, my xmin, my xmax, sound_nt, dt, t1);
		VEC sound = your z.row (1);
		for (integer it = 1; it <= my points.size; it ++) {
			const constRealPoint point = my points.at [it];
			const double t = point -> number;
			const double amplitude = point -> value;
			const integer mid = Sampled_xToNearestIndex (you.get(), t);
			const integer begin = Melder_clippedLeft (1_integer, mid - interpolationDepth);
			const integer end   = Melder_clippedRight (mid + interpolationDepth, your nx);
			/* mutable cycle */ double angle = NUMpi * (Sampled_indexToX (you.get(), begin) - t) / your dx;
			/* mutable cycle */ double halfampsinangle = 0.5 * amplitude * sin (angle);
			for (integer j = begin; j <= end; j ++) {
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
