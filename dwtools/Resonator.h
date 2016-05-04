#ifndef _Resonator_h_
#define _Resonator_h_
/* Resonator.h
 *
 * Copyright (C) 2008-2011, 2015 David Weenink
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

/*
 * djmw 20081029
 * djmw 20110306 Latest modification
 */
	
#include "Sound.h"

Thing_define (Filter, Daata) {
	double dT;
	double a, b, c;
	double p1, p2;

	virtual double v_getOutput (double input);
	virtual void v_setFB (double f, double b);
	virtual void v_resetMemory ();
};

Thing_define (Resonator, Filter) {
	int normalisation;

	void v_setFB (double f, double b)
		override;
};

Thing_define (AntiResonator, Resonator) {
	double v_getOutput (double input)
		override;
	void v_setFB (double f, double b)
		override;
};

Thing_define (ConstantGainResonator, Filter) {
	double d;
	double p3, p4;

	double v_getOutput (double input)
		override;
	void v_setFB (double f, double b)
		override;
	void v_resetMemory ()
		override;
};

#define Resonator_NORMALISATION_H0 0
#define Resonator_NORMALISATION_HMAX 1


autoResonator Resonator_create (double dT, int normalisation);

autoConstantGainResonator ConstantGainResonator_create (double dT);

autoAntiResonator AntiResonator_create (double dT);

/*
	Set a,b,c
	normalisation == 0: H(0) = 1 -> a = 1 -b - c
	normalisation == 1: H(Fmax) = 1 -> a = (1 + c)sin(2*pi*F*T)
*/
void Filter_setFB (Filter me, double f, double b);

double Filter_getOutput (Filter me, double input);

void Filter_resetMemory (Filter me);

#endif /* _Resonator_h_ */

