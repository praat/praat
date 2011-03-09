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

/*
 * pb 2011/03/03
 */

#ifndef _RealTier_h_
	#include "RealTier.h"
#endif
#ifndef _Intensity_h_
	#include "Intensity.h"
#endif
#ifndef _TableOfReal_h_
	#include "TableOfReal.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

/********** class IntensityTier **********/

#define IntensityTier_members RealTier_members
#define IntensityTier_methods RealTier_methods
class_create (IntensityTier, RealTier);

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

#ifdef __cplusplus
	}
#endif

/* End of file IntensityTier.h */
#endif
