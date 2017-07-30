/* NUMear.cpp
 *
 * Copyright (C) 1992-2011,2017 Paul Boersma
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

#include "melder.h"

double NUMhertzToBark (double hertz) {
	double r = hertz / 650.0;
	return hertz < 0.0 ? undefined : 7.0 * log (r + sqrt (1.0 + r * r));
}
double NUMbarkToHertz (double bark) {
	return bark < 0.0 ? undefined : 650.0 * sinh (bark / 7.0);
}

double NUMphonToDifferenceLimens (double phon) {
	return phon < 0.0 ? undefined : 30.0 * (pow (61.0 / 60.0, phon) - 1.0);
}
double NUMdifferenceLimensToPhon (double ndli) {
	return ndli < 0.0 ? undefined : log (1.0 + ndli / 30.0) / log (61.0 / 60.0);
}

double NUMsoundPressureToPhon (double soundPressure, double bark) {
	double result = 0.0, dum;
	if (soundPressure <= 0.0 || bark < 0.0) return undefined;

	/*  dB = 20 * log10 (soundPressure / threshold)  */
	if (soundPressure > 0.0)
		result = 20.0 * log10 (soundPressure / 2.0e-5);   /* First approximation: phon = dB */

	/*  Phones from dB  */
	if (result < 90.0 && bark < 8.0) {
		dum = (90.0 - result) * (8.0 - bark);
		result -= dum * dum / 2500.0;
	}
	dum = bark / 3.6 - 5.0;
	result += 5.0 * exp (- dum * dum);
	if (bark > 20.0) { dum = bark - 20.0; result -= 0.5 * dum * dum; }
	if (result < 0.0) result = 0.0;
	return result;
}

double NUMhertzToMel (double hertz) {
	return hertz < 0.0 ? undefined : 550.0 * log (1.0 + hertz / 550.0);
}
double NUMmelToHertz (double mel) {
	return mel < 0.0 ? undefined : 550.0 * (exp (mel / 550.0) - 1.0);
}

double NUMhertzToSemitones (double hertz) {
	return hertz <= 0.0 ? undefined : 12.0 * log (hertz / 100.0) / NUMln2;
}
double NUMsemitonesToHertz (double semitones) {
	return isundef (semitones) ? undefined : 100.0 * exp (semitones * (NUMln2 / 12.0));
}

/* Moore & Glasberg 1983 JASA 74: 750 */

double NUMerb (double f) {
	return 6.23e-6 * f * f + 0.09339 * f + 28.52;
}
double NUMhertzToErb (double hertz) {
	return hertz < 0.0 ? undefined : 11.17 * log ((hertz + 312.0) / (hertz + 14680.0)) + 43.0;
}
double NUMerbToHertz (double erb) {
	double dum = exp ((erb - 43.0) / 11.17);
	return erb < 0.0 ? undefined : (14680.0 * dum - 312.0) / (1.0 - dum);
}

/* End of file NUMear.cpp */
