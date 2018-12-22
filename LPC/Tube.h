#ifndef _Tube_h_
#define _Tube_h_
/* Tube.h
 *
 * Copyright (C) 1994-2018 David Weenink
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

#include "Sampled.h"

#include "Tube_def.h"

/*
	Tube's as a function of time.
	Tube_frame: c[1] -> mouth
	            c[numberOfSegments] -> glottis.
*/

void Tube_Frame_init (Tube_Frame me, integer numberOfSegments, double length);

void Tube_Frame_free (Tube_Frame me);

void Tube_Frames_rc_into_area (Tube_Frame me, Tube_Frame thee);

void Tube_init (Tube me, double tmin, double tmax, integer nt, double dt, double t1, integer maxNumberOfSegments, double defaultLength);

Thing_define (Area, Tube) {
};

/*
	Areas as a function of time.
	units in m^2.
*/

void Area_init (Area me, double tmin, double tmax, integer nt, double dt, double t1, integer maxNumberOfSegments, double defaultLength);

autoArea Area_create (double tmin, double tmax, integer nt, double dt, double t1, integer maxNumberOfSegments, double defaultLength);

Thing_define (RC, Tube) {
};

// Reflection Coefficients as a function of time.

void RC_init (RC me, double tmin, double tmax, integer nt, double dt, double t1, integer maxnCoefficients, double defaultLength);

autoRC RC_create (double tmin, double tmax, integer nt, double dt, double t1, integer maxnCoefficients, double defaultLength);

#endif // _Tube_h_
