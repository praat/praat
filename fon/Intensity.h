#ifndef _Intensity_h_
#define _Intensity_h_
/* Intensity.h
 *
 * Copyright (C) 1992-2005,2007,2011,2015-2017,2019 Paul Boersma
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

/*
	class Intensity = Vector;

	x == time (seconds)
	nx == number of frames
	dx == distance between frames (seconds)
	x1 == centre of first frame (seconds)
	z == intensity (dB relative to 2e-5 N/m2 or 1e-12 W/m2)
*/

#include "Vector.h"

Thing_define (Intensity, Vector) {
	void v_info ()
		override;
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
	double v_convertStandardToSpecialUnit (double value, integer level, int unit)
		override;
	double v_convertSpecialToStandardUnit (double value, integer level, int unit)
		override;
};

autoIntensity Intensity_create (double tmin, double tmax, integer nt, double dt, double t1);

void Intensity_init (Intensity me, double tmin, double tmax, integer nt, double dt, double t1);

autoMatrix Intensity_to_Matrix (Intensity me);

autoIntensity Matrix_to_Intensity (Matrix me);

void Intensity_drawInside (Intensity me, Graphics g, double tmin, double tmax,
	double minimum, double maximum);

void Intensity_draw (Intensity me, Graphics g, double tmin, double tmax,
	double minimum, double maximum, bool garnish);

double Intensity_getQuantile (Intensity me, double tmin, double tmax, double quantile);

#define Intensity_units_ENERGY  1
#define Intensity_units_SONES  2
#define Intensity_units_DB  3

#define Intensity_averaging_MEDIAN  0
#define Intensity_averaging_ENERGY  1
#define Intensity_averaging_SONES  2
#define Intensity_averaging_DB  3
double Intensity_getAverage (Intensity me, double tmin, double tmax, int averagingMethod);

/* End of file Intensity.h */
#endif
