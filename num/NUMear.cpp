/* NUMear.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 1999/06/26
 * pb 2002/03/07 GPL
 * pb 2011/03/29 C++
 */

#include "NUM.h"

double NUMhertzToBark (double hertz) {
	double r = hertz / 650;
	return hertz < 0 ? NUMundefined : 7.0 * log (r + sqrt (1 + r * r));
}
double NUMbarkToHertz (double bark) {
	return bark < 0 ? NUMundefined : 650.0 * sinh (bark / 7.0);
}

double NUMphonToDifferenceLimens (double phon) {
	return phon < 0 ? NUMundefined : 30 * (pow (61.0 / 60, phon) - 1);
}
double NUMdifferenceLimensToPhon (double ndli) {
	return ndli < 0 ? NUMundefined : log (1 + ndli / 30) / log (61.0 / 60);
}

double NUMsoundPressureToPhon (double soundPressure, double bark) {
	double result = 0, dum;
	if (soundPressure <= 0 || bark < 0) return NUMundefined;

	/*  dB = 20 * log10 (soundPressure / threshold)  */
	if (soundPressure > 0)
		result = 20 * log10 (soundPressure / 2.0e-5);   /* First approximation: phon = dB */

	/*  Phones from dB  */
	if (result < 90 && bark < 8.0) {
		dum = (90 - result) * (8.0 - bark);
		result -= dum * dum / 2500;
	}
	dum = bark / 3.6 - 5;
	result += 5 * exp (- dum * dum);
	if (bark > 20.0) { dum = bark - 20; result -= 0.5 * dum * dum; }
	if (result < 0) result = 0;
	return result;
}

double NUMhertzToMel (double hertz) {
	return hertz < 0 ? NUMundefined : 550.0 * log (1.0 + hertz / 550.0);
}
double NUMmelToHertz (double mel) {
	return mel < 0 ? NUMundefined : 550.0 * (exp (mel / 550.0) - 1);
}

double NUMhertzToSemitones (double hertz) {
	return hertz <= 0.0 ? NUMundefined : 12.0 * log (hertz / 100.0) / NUMln2;
}
double NUMsemitonesToHertz (double semitones) {
	return semitones == NUMundefined ? NUMundefined : 100.0 * exp (semitones * (NUMln2 / 12.0));
}

/* Moore & Glasberg 1983 JASA 74: 750 */

double NUMerb (double f) {
	return 6.23e-6 * f * f + 0.09339 * f + 28.52;
}
double NUMhertzToErb (double hertz) {
	return hertz < 0 ? NUMundefined : 11.17 * log ((hertz + 312.0) / (hertz + 14680.0)) + 43.0;
}
double NUMerbToHertz (double erb) {
	double dum = exp ((erb - 43.0) / 11.17);
	return erb < 0 ? NUMundefined : (14680.0 * dum - 312.0) / (1.0 - dum);
}

/* End of file NUMear.cpp */
