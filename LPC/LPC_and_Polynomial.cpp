/* LPC_and_Polynomial.cpp
 *
 * Copyright (C) 1994-2011, 2015-2017 David Weenink
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
 djmw 20020812 GPL header
*/

#include "LPC_and_Polynomial.h"

autoPolynomial LPC_Frame_to_Polynomial (LPC_Frame me) {
	integer degree = (integer) my nCoefficients;
	autoPolynomial thee = Polynomial_create (-1, 1, degree);
	for (integer i = 1; i <= degree; i ++) {
		thy coefficients [i] = my a [degree - i + 1];
	}
	thy coefficients[degree + 1] = 1.0;
	return thee;
}

autoPolynomial LPC_to_Polynomial (LPC me, double time) {
	try {
		integer iFrame = Sampled_xToIndex (me, time);
		if (iFrame < 1) {
			iFrame = 1;
		}
		if (iFrame > my nx) {
			iFrame = my nx;
		}
		autoPolynomial thee = LPC_Frame_to_Polynomial (& my d_frames [iFrame]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U":no Polynomial created.");
	}
}

/* End of file LPC_and_Polynomial.cpp */
