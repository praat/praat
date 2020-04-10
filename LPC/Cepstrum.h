#ifndef _Cepstrum_h_
#define _Cepstrum_h_
/* Cepstrum.h
 *
 * Copyright (C) 1994-2020 David Weenink
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
 djmw 20010111
 djmw 20020812 GPL header
 djmw 20121117 Latest modification.
*/

/*
	The Cepstrum is a sequence of real numbers.
	It is the spectrum of the power spectrum of a (sound) signal.
*/

#include "Matrix.h"

Thing_define (Cepstrum, Matrix) {
	double v_getValueAtSample (integer isamp, integer which, int units)
		override;
};

#include "Cepstrum_enums.h"

/*
	xmin		// Lowest quefrency.
	xmax		// Highest quefrency.
	nx			// Number of quefrencies.
	dx			// Quefrency step.
	x1			// First quefrency.

	ymin = ymax = dy = y1 = 1
	ny = 1
*/

autoCepstrum Cepstrum_create (double qmax, integer nq);

void Cepstrum_draw (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, bool power, bool garnish);

void Cepstrum_drawLinear (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, bool garnish);

#endif /* _Cepstrum_h_ */
