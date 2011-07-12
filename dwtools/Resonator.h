#ifndef _Resonator_h_
#define _Resonator_h_
/* Resonator.h
 *
 * Copyright (C) 2008-2011 David Weenink
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
 * djmw 20110306 Latest modification
 */
	
#include "Sound.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (Filter);
struct structFilter : public structData {
	double dT;
	double a, b, c;
	double p1, p2;
};
#define Filter__methods(klas) Data__methods(klas) \
	double (*getOutput) (I, double input); \
	void (*setFB) (I, double f, double b); \
	void (*resetMemory)(I);
Thing_declare2cpp (Filter, Data);

Thing_declare1cpp (Resonator);
struct structResonator : public structFilter {
	int normalisation;
};
#define Resonator__methods(klas) Filter__methods(klas)
Thing_declare2cpp (Resonator, Filter);

Thing_declare1cpp (AntiResonator);
struct structAntiResonator : public structResonator {
};
#define AntiResonator__methods(klas) Resonator__methods(klas)
Thing_declare2cpp (AntiResonator, Filter);

Thing_declare1cpp (ConstantGainResonator);
struct structConstantGainResonator : public structFilter {
	double d;
	double p3, p4;
};
#define ConstantGainResonator__methods(klas) Filter__methods(klas)
Thing_declare2cpp (ConstantGainResonator, Filter);

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

#ifdef __cplusplus
	}
#endif

#endif /* _Resonator_h_ */

