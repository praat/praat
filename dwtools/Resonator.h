#ifndef _Resonator_h_
#define _Resonator_h_
/* Resonator.h
 *
 * Copyright (C) 2008-2019 David Weenink
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
	double samplingPeriod;
	double a, b, c;
	double p1, p2;

	virtual double v_getOutput (double input);
	virtual void v_setCoefficients (double frequency, double bandwidth);
	virtual void v_resetMemory ();
};

Thing_define (Resonator, Filter) {
	bool normaliseAtDC;

	void v_setCoefficients (double frequency, double bandwidth)
		override;
};

Thing_define (AntiResonator, Resonator) {
	double v_getOutput (double input)
		override;
	void v_setCoefficients (double frequency, double bandwidth)
		override;
};

Thing_define (ConstantGainResonator, Filter) {
	double d;
	double p3, p4;

	double v_getOutput (double input)
		override;
	void v_setCoefficients (double frequency, double bandwidth)
		override;
	void v_resetMemory ()
		override;
};

autoResonator Resonator_create (double samplingPeriod, bool normaliseAtDC);

autoConstantGainResonator ConstantGainResonator_create (double samplingPeriod);

autoAntiResonator AntiResonator_create (double samplingPeriod);

/*
	Set a,b,c
	normaliseAtDC == true: H(0) = 1 -> a = 1 - b - c
	normaliseAtDC == false: H(Fmax) = 1 -> a = (1 + c)sin(2*pi*F*T)
*/
void Filter_setCoefficients (Filter me, double frequency, double bandwidth);

double Filter_getOutput (Filter me, double input);

void Filter_resetMemory (Filter me);

#endif /* _Resonator_h_ */

