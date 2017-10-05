#ifndef _Excitation_h_
#define _Excitation_h_
/* Excitation.h
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Vector.h"
#include "Graphics.h"

Thing_define (Excitation, Vector) {
	void v_info ()
		override;
	int v_domainQuantity ()
		override { return MelderQuantity_FREQUENCY_BARK; }
};

double Excitation_hertzToBark (double hertz);
double Excitation_barkToHertz (double bark);
double Excitation_phonToDifferenceLimens (double phon);
double Excitation_differenceLimensToPhon (double ndli);
double Excitation_soundPressureToPhon (double soundPressure, double bark);

autoExcitation Excitation_create (double frequencyStep, integer numberOfFrequencies);
double Excitation_getDistance (Excitation me, Excitation thee);
double Excitation_getLoudness (Excitation me);
void Excitation_draw (Excitation me, Graphics g, double fmin, double fmax, double minimum, double maximum, bool garnish);
autoMatrix Excitation_to_Matrix (Excitation me);
autoExcitation Matrix_to_Excitation (Matrix me);

/* End of file Excitation.h */
#endif
