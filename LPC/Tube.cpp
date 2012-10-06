/* Tube.cpp
 *
 * Copyright (C) 1994-2012 David Weenink
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

/*
 djmw 20030613 Creation
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20110304 Thing_new
*/

#include "Tube.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "Tube_def.h"
#include "oo_COPY.h"
#include "Tube_def.h"
#include "oo_EQUAL.h"
#include "Tube_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Tube_def.h"
#include "oo_WRITE_TEXT.h"
#include "Tube_def.h"
#include "oo_WRITE_BINARY.h"
#include "Tube_def.h"
#include "oo_READ_TEXT.h"
#include "Tube_def.h"
#include "oo_READ_BINARY.h"
#include "Tube_def.h"
#include "oo_DESCRIPTION.h"
#include "Tube_def.h"

Thing_implement (Tube, Sampled, 0);

void structTube :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Time domain: ", Melder_double (xmin), L" to ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (L"Maximum number of segments: ", Melder_integer (maxnSegments));
	MelderInfo_writeLine (L"Number of frames: ", Melder_integer (nx));
	MelderInfo_writeLine (L"Time step: ", Melder_double (dx), L" seconds");
	MelderInfo_writeLine (L"First frame at: ", Melder_double (x1), L" seconds");
}

void Tube_Frame_init (Tube_Frame me, long nSegments, double length) {
	my nSegments = nSegments;
	my length = length;
	if (nSegments <= 0) {
		Melder_throw ("Number of segments must be a natural number.");
	}
	my c = NUMvector<double> (1, nSegments);
}

/* Gray & Markel (1979), LPTRN */
void Tube_Frames_rc_into_area (Tube_Frame me, Tube_Frame thee) {
	if (my nSegments > thy nSegments) {
		Melder_throw ("Number of segments to big.");
	}

	double s = 0.0001; /* 1.0 cm^2 at glottis */
	double *rc = my c, *area = thy c;
	for (long i = my nSegments; i > 0; i--) {
		s *= (1.0 + rc[i]) / (1.0 - rc[i]);
		area[i] = s;
	}
}

static void Tube_setLengths (I, double length) {
	iam (Tube);
	for (long i = 1; i <= my nx; i++) {
		Tube_Frame f = & my frame[i];
		if (f) {
			f -> length = length;
		}
	}
}

void Tube_init (I, double tmin, double tmax, long nt, double dt, double t1,
                long maxnSegments, double defaultLength) {
	iam (Tube);
	my maxnSegments = maxnSegments;
	Sampled_init (me, tmin, tmax, nt, dt, t1);
	my frame = NUMvector<structTube_Frame> (1, nt);
	Tube_setLengths (me, defaultLength);
}

Thing_implement (Area, Tube, 0);

void Area_init (Area me, double tmin, double tmax, long nt, double dt, double t1,
                long maxnSegments, double defaultLength) {
	Tube_init (me, tmin, tmax, nt, dt, t1, maxnSegments, defaultLength);
}

Area Area_create (double tmin, double tmax, long nt, double dt, double t1,
                  long maxnSegments, double defaultLength) {
	try {
		autoArea me = Thing_new (Area);
		Area_init (me.peek(), tmin, tmax, nt, dt, t1, maxnSegments, defaultLength);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Area not crteated.");
	}
}

Thing_implement (RC, Tube, 0);

void RC_init (RC me, double tmin, double tmax, long nt, double dt, double t1,
              long maxnSegments, double defaultLength) {
	Tube_init (me, tmin, tmax, nt, dt, t1, maxnSegments, defaultLength);
}

RC RC_create (double tmin, double tmax, long nt, double dt, double t1,
              long maxnCoefficients, double defaultLength) {
	try {
		autoRC me = Thing_new (RC);
		RC_init (me.peek(), tmin, tmax, nt, dt, t1, maxnCoefficients, defaultLength);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("RC not crteated.");
	}
}

/* End of file Tube.cpp */
