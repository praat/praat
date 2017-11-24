/* LFCC.cpp
 *
 *	Linear Frequency Cepstral Coefficients class.
 *
 * Copyright (C) 1993-2017 David Weenink
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

/*
 djmw 20001228
 djmw 20020813 GPL header
  djmw 20110304 Thing_new
*/

#include "LFCC.h"

Thing_implement (LFCC, CC, 1);

autoLFCC LFCC_create (double tmin, double tmax, integer nt, double dt, double t1, integer maximumNumberOfCoefficients, double fmin, double fmax) {
	try {
		autoLFCC me = Thing_new (LFCC);
		CC_init (me.get(), tmin, tmax, nt, dt, t1, maximumNumberOfCoefficients, fmin, fmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LFCC not created.");
	}
}

/* End of file LFCC.cpp */
