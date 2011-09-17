/* LFCC.cpp
 *
 *	Linear Frequency Cepstral Coefficients class.
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20001228
 djmw 20020813 GPL header
  djmw 20110304 Thing_new
*/

#include "LFCC.h"

Thing_implement (LFCC, CC, 1);

LFCC LFCC_create (double tmin, double tmax, long nt, double dt, double t1,
                  long maximumNumberOfCoefficients, double fmin, double fmax) {
	try {
		autoLFCC me = Thing_new (LFCC);
		CC_init (me.peek(), tmin, tmax, nt, dt, t1, maximumNumberOfCoefficients, fmin, fmax);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("LFCC not created.");
	}
}

/* End of file LFCC.cpp */
