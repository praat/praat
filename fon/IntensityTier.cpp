/* IntensityTier.cpp
 *
 * Copyright (C) 1992-2005,2007,2008,2010-2012,2015-2018,2020 Paul Boersma
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

#include "IntensityTier.h"

Thing_implement (IntensityTier, RealTier, 0);

autoIntensityTier IntensityTier_create (double tmin, double tmax) {
	try {
		autoIntensityTier me = Thing_new (IntensityTier);
		RealTier_init (me.get(), tmin, tmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"IntensityTier not created.");
	}
}

void IntensityTier_draw (IntensityTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, conststring32 method, bool garnish)
{
	RealTier_draw (me, g, tmin, tmax, ymin, ymax, garnish, method, U"Intensity (dB)");
}

autoIntensityTier PointProcess_upto_IntensityTier (PointProcess me, double intensity) {
	try {
		autoIntensityTier thee = PointProcess_upto_RealTier (me, intensity, classIntensityTier).static_cast_move<structIntensityTier>();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to IntensityTier.");
	}
}

autoIntensityTier Intensity_downto_IntensityTier (Intensity me) {
	try {
		autoIntensityTier thee = Vector_to_RealTier (me, 1, classIntensityTier).static_cast_move<structIntensityTier>();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to IntensityTier.");
	}
}

autoIntensityTier Intensity_to_IntensityTier_peaks (Intensity me) {
	try {
		autoIntensityTier thee = Vector_to_RealTier_peaks (me, 1, classIntensityTier).static_cast_move<structIntensityTier>();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": peaks not converted to IntensityTier.");
	}
}

autoIntensityTier Intensity_to_IntensityTier_valleys (Intensity me) {
	try {
		autoIntensityTier thee = Vector_to_RealTier_valleys (me, 1, classIntensityTier).static_cast_move<structIntensityTier>();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": valleys not converted to IntensityTier.");
	}
}

autoIntensityTier Intensity_PointProcess_to_IntensityTier (Intensity me, PointProcess pp) {
	try {
		autoIntensityTier temp = Intensity_downto_IntensityTier (me);
		autoIntensityTier thee = IntensityTier_PointProcess_to_IntensityTier (temp.get(), pp);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" & ", pp, U": not converted to IntensityTier.");
	}
}

autoIntensityTier IntensityTier_PointProcess_to_IntensityTier (IntensityTier me, PointProcess pp) {
	try {
		if (my points.size == 0) Melder_throw (U"No intensity points.");
		autoIntensityTier thee = IntensityTier_create (pp -> xmin, pp -> xmax);
		for (integer i = 1; i <= pp -> nt; i ++) {
			double time = pp -> t [i];
			double value = RealTier_getValueAtTime (me, time);
			RealTier_addPoint (thee.get(), time, value);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" & ", pp, U": not converted to IntensityTier.");
	}
}

autoTableOfReal IntensityTier_downto_TableOfReal (IntensityTier me) {
	return RealTier_downto_TableOfReal (me, U"Time (s)", U"Intensity (dB)");
}

void Sound_IntensityTier_multiply_inplace (Sound me, IntensityTier intensity) {
	if (intensity -> points.size == 0) return;
	for (integer isamp = 1; isamp <= my nx; isamp ++) {
		double t = my x1 + (isamp - 1) * my dx;
		double factor = pow (10, RealTier_getValueAtTime (intensity, t) / 20);
		for (integer channel = 1; channel <= my ny; channel ++) {
			my z [channel] [isamp] *= factor;
		}
	}
}

autoSound Sound_IntensityTier_multiply (Sound me, IntensityTier intensity, int scale) {
	try {
		autoSound thee = Data_copy (me);
		Sound_IntensityTier_multiply_inplace (thee.get(), intensity);
		if (scale) Vector_scale (thee.get(), 0.9);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not multiplied with ", intensity, U".");
	}
}

/* End of file IntensityTier.cpp */
