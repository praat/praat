/* IntensityTier.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2003/05/31 use PointProcess_upto_RealTier
 * pb 2005/05/26 switch for scaling in multiply
 * pb 2007/01/27 made compatible with stereo sounds
 * pb 2007/03/17 domain quantity
 * pb 2007/08/12 wchar_t
 * pb 2010/10/19 allow drawing without speckles
 * pb 2011/05/31 C++
 */

#include "IntensityTier.h"

class_methods (IntensityTier, RealTier) {
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
	class_methods_end
}

IntensityTier IntensityTier_create (double tmin, double tmax) {
	try {
		autoIntensityTier me = Thing_new (IntensityTier);
		RealTier_init (me.peek(), tmin, tmax);
		return me.transfer();
	} catch (MelderError) {
		rethrowmzero ("IntensityTier not created.");
	}
}

void IntensityTier_draw (IntensityTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, const wchar_t *method, int garnish)
{
	RealTier_draw (me, g, tmin, tmax, ymin, ymax, garnish, method, L"Intensity (dB)");
}

IntensityTier PointProcess_upto_IntensityTier (PointProcess me, double intensity) {
	try {
		autoIntensityTier thee = (IntensityTier) PointProcess_upto_RealTier (me, intensity);
		Thing_overrideClass (thee.peek(), classIntensityTier);
		return thee.transfer();
	} catch (MelderError) {
		rethrowmzero (me, ": not converted to IntensityTier.");
	}
}

IntensityTier Intensity_downto_IntensityTier (Intensity me) {
	try {
		autoIntensityTier thee = (IntensityTier) Vector_to_RealTier (me, 1);
		Thing_overrideClass (thee.peek(), classIntensityTier);
		return thee.transfer();
	} catch (MelderError) {
		rethrowmzero (me, ": not converted to IntensityTier.");
	}
}

IntensityTier Intensity_to_IntensityTier_peaks (Intensity me) {
	try {
		autoIntensityTier thee = (IntensityTier) Vector_to_RealTier_peaks (me, 1);
		Thing_overrideClass (thee.peek(), classIntensityTier);
		return thee.transfer();
	} catch (MelderError) {
		rethrowmzero (me, ": peaks not converted to IntensityTier.");
	}
}

IntensityTier Intensity_to_IntensityTier_valleys (Intensity me) {
	try {
		autoIntensityTier thee = (IntensityTier) Vector_to_RealTier_valleys (me, 1);
		Thing_overrideClass (thee.peek(), classIntensityTier);
		return thee.transfer();
	} catch (MelderError) {
		rethrowmzero (me, ": valleys not converted to IntensityTier.");
	}
}

IntensityTier Intensity_PointProcess_to_IntensityTier (Intensity me, PointProcess pp) {
	try {
		autoIntensityTier temp = Intensity_downto_IntensityTier (me);
		autoIntensityTier thee = IntensityTier_PointProcess_to_IntensityTier (temp.peek(), pp);
		return thee.transfer();
	} catch (MelderError) {
		rethrowmzero (me, " & ", pp, ": not converted to IntensityTier.");
	}
}

IntensityTier IntensityTier_PointProcess_to_IntensityTier (IntensityTier me, PointProcess pp) {
	try {
		if (my points -> size == 0) Melder_throw ("No intensity points.");
		autoIntensityTier thee = IntensityTier_create (pp -> xmin, pp -> xmax);
		for (long i = 1; i <= pp -> nt; i ++) {
			double time = pp -> t [i];
			double value = RealTier_getValueAtTime (me, time);
			RealTier_addPoint (thee.peek(), time, value); therror
		}
		return thee.transfer();
	} catch (MelderError) {
		rethrowmzero (me, " & ", pp, ": not converted to IntensityTier.");
	}
}

TableOfReal IntensityTier_downto_TableOfReal (IntensityTier me) {
	return RealTier_downto_TableOfReal (me, L"Time (s)", L"Intensity (dB)");
}

void Sound_IntensityTier_multiply_inline (Sound me, IntensityTier intensity) {
	if (intensity -> points -> size == 0) return;
	for (long isamp = 1; isamp <= my nx; isamp ++) {
		double t = my x1 + (isamp - 1) * my dx;
		double factor = pow (10, RealTier_getValueAtTime (intensity, t) / 20);
		for (long channel = 1; channel <= my ny; channel ++) {
			my z [channel] [isamp] *= factor;
		}
	}
}

Sound Sound_IntensityTier_multiply (Sound me, IntensityTier intensity, int scale) {
	try {
		autoSound thee = (Sound) Data_copy (me);
		Sound_IntensityTier_multiply_inline (thee.peek(), intensity);
		if (scale) Vector_scale (thee.peek(), 0.9);
		return thee.transfer();
	} catch (MelderError) {
		rethrowmzero (me, ": not multiplied with ", intensity, ".");
	}
}

/* End of file IntensityTier.cpp */
