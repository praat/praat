/* LPC_and_Cepstrumc.cpp
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 20020812 GPL header
 djmw 20080122 float -> double
*/
/* LPC_and_Cepstrumc.c */

#include "LPC_and_Cepstrumc.h"

void LPC_Frame_into_Cepstrumc_Frame (LPC_Frame me, Cepstrumc_Frame thee) {
	long n = my nCoefficients > thy nCoefficients ? thy nCoefficients : my nCoefficients;
	double *c = thy c, *a = my a;

	c[0] = 0.5 * log (my gain);
	if (n == 0) {
		return;
	}

	c[1] = -a[1];
	for (long i = 2; i <= n; i++) {
		c[i] = 0;
		for (long k = 1; k < i; k++) {
			c[i] += a[i - k] * c[k] * k;
		}
		c[i] = -a[i] - c[i] / i;
	}
}

void Cepstrumc_Frame_into_LPC_Frame (Cepstrumc_Frame me, LPC_Frame thee) {
	double *c = my c, *a = thy a;
	thy gain = exp (2 * c[0]);
	if (thy nCoefficients == 0) {
		return;
	}
	a[1] = -c[1];
	for (long i = 2; i <= thy nCoefficients; i++) {
		c[i] *= i;
	}
	for (long i = 2; i <= thy nCoefficients; i++) {
		a[i] = c[i];
		for (long j = 1 ; j < i; j++) {
			a[i] += a[j] * c[i - j];
		}
		a[i] /= -i;
	}
	for (long i = 2; i <= thy nCoefficients; i++) {
		c[i] /= i;
	}
}

Cepstrumc LPC_to_Cepstrumc (LPC me) {
	try {
		autoCepstrumc thee = Cepstrumc_create (my xmin, my xmax, my nx, my dx, my x1,
		                                       my maxnCoefficients, 1.0 / my samplingPeriod);

		for (long i = 1; i <= my nx; i++) {
			Cepstrumc_Frame_init (& thy frame[i], my d_frames[i].nCoefficients);
			LPC_Frame_into_Cepstrumc_Frame (& my d_frames[i], & thy frame[i]);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Cepstrum created.");
	}
}

LPC Cepstrumc_to_LPC (Cepstrumc me) {
	try {
		autoLPC thee = LPC_create (my xmin, my xmax, my nx, my dx, my x1,
		                           my maxnCoefficients, 1.0 / my samplingFrequency);
		for (long i = 1; i <= my nx; i++) {
			LPC_Frame_init (& thy d_frames[i], my frame[i].nCoefficients);
			Cepstrumc_Frame_into_LPC_Frame (& my frame[i], & thy d_frames[i]);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ":no LPC created.");
	}
}

/* End of file LPC_and_Cepstrumc.cpp  */
