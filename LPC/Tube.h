#ifndef _Tube_h_
#define _Tube_h_
/* Tube.h
 *
 * Copyright (C) 1994-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Sampled.h"

#include "Tube_def.h"
oo_CLASS_CREATE (Tube, Sampled);

/*
	Tube's as a function of time.
	Tube_frame: c[1] -> mouth
	            c[nSegments] -> glottis.
*/

void Tube_Frame_init (Tube_Frame me, long nSegments, double length);

void Tube_Frame_free (Tube_Frame me);

void Tube_Frames_rc_into_area (Tube_Frame me, Tube_Frame thee);

void Tube_init (I, double tmin, double tmax, long nt, double dt, double t1,
	long maxnSegments, double defaultLength);

Thing_define (Area, Tube) {
};

/*
	Areas as a function of time.
	units in m^2.
*/

void Area_init (Area me, double tmin, double tmax, long nt, double dt, double t1,
	long maxnSegments, double defaultLength);

Area Area_create (double tmin, double tmax, long nt, double dt, double t1,
	long maxnSegments, double defaultLength);

Thing_define (RC, Tube) {
};

/*
	Reflection Coefficients as a function of time.

*/


void RC_init (RC me, double tmin, double tmax, long nt, double dt, double t1,
	long maxnCoefficients, double defaultLength);

RC RC_create (double tmin, double tmax, long nt, double dt, double t1,
	long maxnCoefficients, double defaultLength);

#endif // _Tube_h_
