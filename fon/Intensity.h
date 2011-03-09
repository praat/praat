#ifndef _Intensity_h_
#define _Intensity_h_
/* Intensity.h
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

/*
	class Intensity = Vector;

	x = time (seconds)
	nx = number of frames
	dx = distance between frames (seconds)
	x1 = centre of first frame (seconds)
	z = intensity (dB relative to 2e-5 N/m2 or 1e-12 W/m2)
*/

#ifndef _Vector_h_
	#include "Vector.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

#define Intensity_members Vector_members
#define Intensity_methods Vector_methods
class_create (Intensity, Vector);

Intensity Intensity_create (double tmin, double tmax, long nt, double dt, double t1);

int Intensity_init (Intensity me, double tmin, double tmax, long nt, double dt, double t1);

Matrix Intensity_to_Matrix (Intensity me);

Intensity Matrix_to_Intensity (Matrix me);

void Intensity_drawInside (Intensity me, Graphics g, double tmin, double tmax,
	double minimum, double maximum);

void Intensity_draw (Intensity me, Graphics g, double tmin, double tmax,
	double minimum, double maximum, int garnish);

double Intensity_getQuantile (Intensity me, double tmin, double tmax, double quantile);

#define Intensity_units_ENERGY  1
#define Intensity_units_SONES  2
#define Intensity_units_DB  3

#define Intensity_averaging_MEDIAN  0
#define Intensity_averaging_ENERGY  1
#define Intensity_averaging_SONES  2
#define Intensity_averaging_DB  3
double Intensity_getAverage (Intensity me, double tmin, double tmax, int averagingMethod);

#ifdef __cplusplus
	}
#endif

/* End of file Intensity.h */
#endif
