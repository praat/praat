/* Cepstrogram.cpp
 *
 * Copyright (C) 2013 - 2020 David Weenink
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
 djmw 20010514
 djmw 20020812 GPL header
 djmw 20080122 Version 1: float -> double
 djmw 20110304 Thing_new
*/

#include "Cepstrogram.h"
#include "Cepstrum_and_Spectrum.h"
#include "NUM2.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"

Thing_implement (Cepstrogram, Matrix, 2);

autoCepstrogram Cepstrogram_create (double tmin, double tmax, integer nt, double dt, double t1,
	double qmin, double qmax, integer nq, double dq, double q1) {
	try {
		autoCepstrogram me = Thing_new (Cepstrogram);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, qmin, qmax, nq, dq, q1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cepstrogram not created.");
	}
}

/* End of file Cepstrogram.cpp */
