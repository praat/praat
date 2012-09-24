#ifndef _IntensityTier_h_
#define _IntensityTier_h_
/* IntensityTier.h
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

#include "RealTier.h"
#include "Intensity.h"
#include "TableOfReal.h"
#include "Sound.h"

Thing_define (IntensityTier, RealTier) {
	// overridden methods:
	protected:
		virtual int v_domainQuantity () { return MelderQuantity_TIME_SECONDS; }
};

IntensityTier IntensityTier_create (double tmin, double tmax);

void IntensityTier_draw (IntensityTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, const wchar_t *method, int garnish);

IntensityTier PointProcess_upto_IntensityTier (PointProcess me, double intensity);
IntensityTier Intensity_downto_IntensityTier (Intensity me);
IntensityTier Intensity_to_IntensityTier_peaks (Intensity me);
IntensityTier Intensity_to_IntensityTier_valleys (Intensity me);
IntensityTier Intensity_PointProcess_to_IntensityTier (Intensity me, PointProcess pp);
IntensityTier IntensityTier_PointProcess_to_IntensityTier (IntensityTier me, PointProcess pp);
TableOfReal IntensityTier_downto_TableOfReal (IntensityTier me);
void Sound_IntensityTier_multiply_inline (Sound me, IntensityTier intensity);
Sound Sound_IntensityTier_multiply (Sound me, IntensityTier intensity, int scale);

/* End of file IntensityTier.h */
#endif
