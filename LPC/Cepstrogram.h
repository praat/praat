#ifndef _Cepstrogram_h_
#define _Cepstrogram_h_
/* Cepstrogram.h
 *
 * Copyright (C) 2012-2020 David Weenink
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
 djmw 20121017
 djmw 20130502 Latest modification.
*/

/*
	The Cepstrogram is a matrix of real numbers.
	It is the inverse-fourier-transformed logarithm of the spectrum
	of a (sound) signal.
*/

#include "Cepstrum.h"
#include "Sound.h"
#include "Table.h"

Thing_define (Cepstrogram, Matrix) {
};

/*
	xmin, xmax : time domain
	nx, dx, x1	: sampling in the time domain
	ymin		// Lowest quefrency.
	ymax		// Highest quefrency.
	ny			// Number of quefrencies.
	dy			// Quefrency step.
	y1			// First quefrency.

*/

autoCepstrogram Cepstrogram_create (double tmin, double tmax, integer nt, double dt, double t1, double qmin, double qmax, integer nq, double dq, double q1);

#endif /* _Cepstrogram_h_ */
