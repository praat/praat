#ifndef _Excitation_h_
#define _Excitation_h_
/* Excitation.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/06/04
 * pb 2002/07/16 GPL
 */

double Excitation_hertzToBark (double hertz);
double Excitation_barkToHertz (double bark);
double Excitation_phonToDifferenceLimens (double phon);
double Excitation_differenceLimensToPhon (double ndli);
double Excitation_soundPressureToPhon (double soundPressure, double bark);

#ifndef _Vector_h_
	#include "Vector.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#define Excitation_members  Vector_members
#define Excitation_methods  Vector_methods
class_create (Excitation, Vector)

Excitation Excitation_create (double df, long nf);
double Excitation_getDistance (Excitation me, Excitation thee);
double Excitation_getLoudness (Excitation me);
void Excitation_draw (Excitation me, Graphics g, double fmin, double fmax, double minimum, double maximum, int garnish);
Matrix Excitation_to_Matrix (Excitation me);
Excitation Matrix_to_Excitation (Matrix me);

/* End of file Excitation.h */
#endif
