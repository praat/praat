/* Resonator.cpp
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
 * djmw 20081124 +ConstantGainResonator
 * djmw 20110304 Thing_new
 */

#include "Resonator.h"

Thing_implement (Filter, Daata, 0);

#define SETBC(f,bw) \
	double r = exp (-NUMpi * dT * bw); \
	c = -(r * r); \
	b = 2.0 * r * cos (2.0 * NUMpi * f * dT);

void structFilter :: v_resetMemory () {
	p1 = p2 = 0;
}

void structFilter :: v_setFB (double f, double bw) {
	SETBC (f, bw)
	a = 1.0 - b - c;
}

double structFilter :: v_getOutput (double input) {
	double output = a * input + b * p1 + c * p2;
	p2 = p1;
	p1 = output;
	return output;
}

Thing_implement (Resonator, Filter, 0);

void structResonator :: v_setFB (double f, double bw) {
	SETBC (f, bw)
	a = normalisation == Resonator_NORMALISATION_H0 ? (1.0 - b - c) : (1 + c) * sin (2.0 * NUMpi * f * dT);
}

autoResonator Resonator_create (double dT, int normalisation) {
	try {
		autoResonator me = Thing_new (Resonator);
		my a = 1; // all-pass
		my dT = dT;
		my normalisation = normalisation;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Resonator not created.");
	}
}

Thing_implement (AntiResonator, Filter, 0);

void structAntiResonator :: v_setFB (double f, double bw) {
	if (f <= 0 && bw <= 0) {
		a = 1; b = -2; c = 1; // all-pass except dc
	} else {
		SETBC (f, bw)
		a = 1 / (1.0 - b - c);
		// The next equations are incorporated in the getOutput function
		//c *= - a; b *= - a;
	}
}

/* y[n] = a * (x[n] - b * x[n-1] - c * x[n-2]) */
double structAntiResonator :: v_getOutput (double input) {
	double output = a * (input - b * p1 - c * p2);
	p2 = p1;
	p1 = input;
	return output;
}

Thing_implement (ConstantGainResonator, Filter, 0);

void structConstantGainResonator :: v_resetMemory () {
	p1 = p2 = p3 = p4 = 0;
}

void structConstantGainResonator :: v_setFB (double f, double bw) {
	SETBC (f, bw)
	a = 1 - r;
	d = -r;
}

/* y[n] = a * (x[n] + d * x[n-2]) + b * y[n-1] + c * y[n-2] */
double structConstantGainResonator :: v_getOutput (double input) {
	double output = a * (input + d * p4) + b * p1 + c * p2;
	p2 = p1;
	p1 = output;
	p4 = p3;
	p3 = input;
	return output;
}

autoConstantGainResonator ConstantGainResonator_create (double dT) {
	try {
		autoConstantGainResonator me = Thing_new (ConstantGainResonator);
		my a = 1; // all-pass
		my dT = dT;
		return me;
	} catch (MelderError) {
		Melder_throw (U"ConstantGainResonator not created.");
	}
}

autoAntiResonator AntiResonator_create (double dT) {
	try {
		autoAntiResonator me = Thing_new (AntiResonator);
		my a = 1; // all-pass
		my dT = dT;
		return me;
	} catch (MelderError) {
		Melder_throw (U"AntiResonator not created.");
	}
}

void Filter_setFB (Filter me, double f, double b) {
	my v_setFB (f, b);
}

double Filter_getOutput (Filter me, double input) {
	return my v_getOutput (input);
}

void Filter_resetMemory (Filter me) {
	my v_resetMemory ();
}

/* End of file Resonator.cpp */
