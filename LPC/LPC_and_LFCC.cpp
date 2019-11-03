/* LPC_and_LFCC.cpp
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

/*
 djmw 20020812 GPL header
 djmw 20030205 Latest modifiation
*/

#include "LPC_and_LFCC.h"
#include "NUM2.h"

void LPC_Frame_into_CC_Frame (LPC_Frame me, CC_Frame thee) {

	thy c0 = 0.5 * log (my gain);
	if (my nCoefficients < 1) return;

	thy c [1] = - my a [1];
	for (integer n = 2; n <= std::min ((integer) my nCoefficients, thy numberOfCoefficients); n ++) {
		longdouble s = 0.0;
		for (integer k = 1; k < n; k ++)
			s += my a [k] * thy c [n - k] * (n - k);
		thy c [n] = - my a [n] - s / n;
	}
	for (integer n = my nCoefficients + 1; n <= thy numberOfCoefficients; n ++) {
		longdouble s = 0.0;
		for (integer k = 1; k <= my nCoefficients; k ++)
			s += my a [k] * thy c [n - k] * (n - k);
		thy c [n] = - s / n;
	}
}

void CC_Frame_into_LPC_Frame (CC_Frame me, LPC_Frame thee) {
	integer n = std::min (my numberOfCoefficients, (integer) thy nCoefficients);
	thy gain = exp (2.0 * my c0);

	if (n < 1) return;

	thy a [1] = - my c [1];
	for (integer i = 2; i <= n; i ++) {
		longdouble ai = my c [i] * i;
		for (integer j = 1; j < i; j ++)
			ai += thy a [j] * my c [i - j] * (i - j);
		thy a [i] = - double (ai) / i;
	}
}

autoLFCC LPC_to_LFCC (LPC me, integer numberOfCoefficients) {
	try {
		if (numberOfCoefficients < 1)
			numberOfCoefficients = my maxnCoefficients;

		autoLFCC thee = LFCC_create (my xmin, my xmax, my nx, my dx, my x1, numberOfCoefficients, 0, 0.5 / my samplingPeriod);

		for (integer i = 1; i <= my nx; i ++) {
			CC_Frame_init (& thy frame [i], numberOfCoefficients);
			LPC_Frame_into_CC_Frame (& my d_frames [i], & thy frame [i]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LFCC created.");
	}
}

autoLPC LFCC_to_LPC (LFCC me, integer numberOfCoefficients) {
	try {
		if (numberOfCoefficients < 1)
			numberOfCoefficients = my maximumNumberOfCoefficients;

		numberOfCoefficients = std::min (numberOfCoefficients, my maximumNumberOfCoefficients);
		autoLPC thee = LPC_create (my xmin, my xmax, my nx, my dx, my x1, numberOfCoefficients, 0.5 / my fmax);

		for (integer i = 1; i <= my nx; i ++) {
			LPC_Frame_init (& thy d_frames [i], numberOfCoefficients);
			CC_Frame_into_LPC_Frame (& my frame [i], & thy d_frames [i]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC created.");
	}
}

/* End of file LPC_and_LFCC.cpp  */
