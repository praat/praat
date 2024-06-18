/* LPC_and_Polynomial.cpp
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
 djmw 20020812 GPL header
*/

#include "LPC_and_Polynomial.h"

autoPolynomial LPC_Frame_to_Polynomial (constLPC_Frame me) {
	Melder_assert (my nCoefficients == my a.size); // check invariant
	const integer numberOfPolynomialCoefficients = my nCoefficients + 1;
	autoPolynomial thee = Polynomial_create (-1, 1, my nCoefficients);
	for (integer icof = 1; icof <= my nCoefficients; icof ++)
		thy coefficients [icof] = my a [numberOfPolynomialCoefficients - icof];
	thy coefficients [numberOfPolynomialCoefficients] = 1.0;
	return thee;
}

autoPolynomial LPC_to_Polynomial (constLPC me, double time) {
	try {
		integer iFrame = Sampled_xToIndex (me, time);
		Melder_clip (1_integer, & iFrame, my nx);   // constant extrapolation
		autoPolynomial thee = LPC_Frame_to_Polynomial (& my d_frames [iFrame]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Polynomial created.");
	}
}

/* End of file LPC_and_Polynomial.cpp */
