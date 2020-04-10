/* LPC_and_Cepstrumc.cpp
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
 djmw 20080122 float -> double
*/

#include "LPC_and_Cepstrumc.h"

void LPC_Frame_into_Cepstrumc_Frame (LPC_Frame me, Cepstrumc_Frame thee) {
	Melder_assert (my nCoefficients == my a.size); // check invariant
	thy c.resize (my nCoefficients);
	thy nCoefficients = thy c.size; // maintain invariant
	thy c0 = 0.5 * log (my gain);
	if (my nCoefficients == 0)
		return;
	thy c [1] = - my a [1];
	for (integer i = 2; i <= my nCoefficients; i ++) {
		thy c [i] = 0.0;
		for (integer k = 1; k < i; k ++)
			thy c [i] += my a [i - k] * thy c [k] * k;
		thy c [i] = - my a [i] - thy c [i] / i;
	}
}

void Cepstrumc_Frame_into_LPC_Frame (Cepstrumc_Frame me, LPC_Frame thee) {
	Melder_assert (my nCoefficients == my c.size); // Check invariant
	thy a.resize (my nCoefficients);
	thy nCoefficients = thy a.size; // maintain invariant
	thy gain = exp (2.0 * my c0);
	if (thy nCoefficients == 0)
		return;
	thy a [1] = - my c [1];
	for (integer i = 2; i <= thy nCoefficients; i ++)
		my c [i] *= i;
	for (integer i = 2; i <= thy nCoefficients; i ++) {
		thy a [i] = my c [i];
		for (integer j = 1 ; j < i; j ++)
			thy a [i] += thy a [j] * my c [i - j];
		thy a [i] /= -i;
	}
	/*
		Undo the modification of the c array
	*/
	for (integer i = 2; i <= thy nCoefficients; i ++)
		my c [i] /= i;
}

autoCepstrumc LPC_to_Cepstrumc (LPC me) {
	try {
		autoCepstrumc thee = Cepstrumc_create (my xmin, my xmax, my nx, my dx, my x1, my maxnCoefficients, 1.0 / my samplingPeriod);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			Cepstrumc_Frame_init (& thy frame [iframe], my d_frames [iframe]. nCoefficients);
			LPC_Frame_into_Cepstrumc_Frame (& my d_frames [iframe], & thy frame [iframe]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Cepstrum created.");
	}
}

autoLPC Cepstrumc_to_LPC (Cepstrumc me) {
	try {
		autoLPC thee = LPC_create (my xmin, my xmax, my nx, my dx, my x1, my maxnCoefficients, 1.0 / my samplingFrequency);
		for (integer i = 1; i <= my nx; i ++) {
			LPC_Frame_init (& thy d_frames [i], my frame [i].nCoefficients);
			Cepstrumc_Frame_into_LPC_Frame (& my frame [i], & thy d_frames [i]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U":no LPC created.");
	}
}

/* End of file LPC_and_Cepstrumc.cpp  */
