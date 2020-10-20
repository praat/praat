#ifndef _IntensityTier_h_
#define _IntensityTier_h_
/* IntensityTier.h
 *
 * Copyright (C) 1992-2005,2007,2010-2012,2015-2018,2020 Paul Boersma
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

#include "RealTier.h"
#include "Intensity.h"
#include "TableOfReal.h"
#include "Sound.h"

Thing_define (IntensityTier, RealTier) {
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
};

autoIntensityTier IntensityTier_create (double tmin, double tmax);

void IntensityTier_draw (IntensityTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, conststring32 method, bool garnish);

autoIntensityTier PointProcess_upto_IntensityTier (PointProcess me, double intensity);
autoIntensityTier Intensity_downto_IntensityTier (Intensity me);
autoIntensityTier Intensity_to_IntensityTier_peaks (Intensity me);
autoIntensityTier Intensity_to_IntensityTier_valleys (Intensity me);
autoIntensityTier Intensity_PointProcess_to_IntensityTier (Intensity me, PointProcess pp);
autoIntensityTier IntensityTier_PointProcess_to_IntensityTier (IntensityTier me, PointProcess pp);
autoTableOfReal IntensityTier_downto_TableOfReal (IntensityTier me);
void Sound_IntensityTier_multiply_inplace (Sound me, IntensityTier intensity);
autoSound Sound_IntensityTier_multiply (Sound me, IntensityTier intensity, int scale);

/* End of file IntensityTier.h */
#endif
