#ifndef _Resonator_h_
#define _Resonator_h_
/* Resonator.h
 *
 * Copyright (C) 2008-2009 David Weenink
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
 * djmw 20081029
 * djmw 20081124 +ConstantGainResonator
 */
	
#ifndef _Sound_h_
	#include "Sound.h"
#endif

#define Filter_members Data_members \
	double dT; \
	double a, b, c; \
	double p1, p2;

#define Filter_methods Data_methods \
	double (*getOutput) (I, double input); \
	void (*setFB) (I, double f, double b); \
	void (*resetMemory)(I);

class_create (Filter, Data);

#define Resonator_members Filter_members \
	int normalisation;
#define Resonator_methods Filter_methods
class_create (Resonator, Filter);

#define AntiResonator_members Resonator_members
#define AntiResonator_methods Resonator_methods
class_create (AntiResonator, Filter);

#define ConstantGainResonator_members Filter_members \
	double d; \
	double p3, p4;

#define ConstantGainResonator_methods Filter_methods
class_create (ConstantGainResonator, Filter);

#define Resonator_NORMALISATION_H0 0
#define Resonator_NORMALISATION_HMAX 1


Resonator Resonator_create (double dT, int normalisation);

ConstantGainResonator ConstantGainResonator_create (double dT);

AntiResonator AntiResonator_create (double dT);

/*
	Set a,b,c
	normalisation == 0: H(0) = 1 -> a = 1 -b - c
	normalisation == 1: H(Fmax) = 1 -> a = (1 + c)sin(2*pi*F*T)
*/
void Filter_setFB (I, double f, double b);

double Filter_getOutput (I, double input);

void Filter_resetMemory (I);

#endif /* _Resonator_h_ */

