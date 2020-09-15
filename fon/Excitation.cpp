/* Excitation.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2015-2018 Paul Boersma
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

#include "Excitation.h"

Thing_implement (Excitation, Vector, 2);

double Excitation_hertzToBark (double hertz) {
	double h650 = hertz / 650;
	return 7.0 * log (h650 + sqrt (1.0 + h650 * h650));
}

double Excitation_barkToHertz (double bark) {
	return 650.0 * sinh (bark / 7.0);
}

double Excitation_phonToDifferenceLimens (double phon) {
	return 30.0 * (pow (61.0 / 60.0, phon) - 1);
}

double Excitation_differenceLimensToPhon (double ndli) {
	return log (1 + ndli / 30.0) / log (61.0 / 60.0);
}

double Excitation_soundPressureToPhon (double soundPressure, double bark) {
	double result, dum;

	if (soundPressure <= 0.0) return 0.0;

	/*  dB = 20 * log10 (soundPressure / threshold)  */
	result = 20.0 * log10 (soundPressure / 2.0e-5);   /* First approximation: phon = dB */

	/*  Phones from dB  */
	if (result < 90.0 && bark < 8.0)
	{
		dum = (90.0 - result) * (8.0 - bark);
		result -= dum * dum / 2500;
	}
	dum = bark / 3.6 - 5.0;
	result += 5.0 * exp (- dum * dum);
	if (bark > 20.0) { dum = bark - 20.0; result -= 0.5 * dum * dum; }
	if (result < 0.0) result = 0.0;
	return result;
}

void structExcitation :: v_info () {
	constVEC y = z.row (1);
	integer numberOfMaxima = 0;
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Loudness: ", Melder_half (Excitation_getLoudness (this)), U" sones");
	for (integer i = 2; i < nx; i ++) if (y [i] > y [i - 1] && y [i] >= y [i + 1]) {
		if (++ numberOfMaxima > 15)
			break;
		double i_real;
		double strength = NUMimproveMaximum (y, i, NUM_PEAK_INTERPOLATE_SINC70, & i_real);
		double formant_bark = x1 + (i_real - 1.0) * dx;
		MelderInfo_write (U"Peak at ", Melder_single (formant_bark), U" Bark");
		MelderInfo_write (U", ", (integer) NUMbarkToHertz (formant_bark), U" Hz");
		MelderInfo_writeLine (U", ", Melder_half (strength), U" phon.");
	}
}

autoExcitation Excitation_create (double frequencyStep, integer numberOfFrequencies) {
	try {
		autoExcitation me = Thing_new (Excitation);
		Matrix_init (me.get(),
			0.0, numberOfFrequencies * frequencyStep, numberOfFrequencies, frequencyStep, 0.5 * frequencyStep,
			1.0, 1.0, 1, 1.0, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Excitation not created.");
	}
}

double Excitation_getDistance (Excitation me, Excitation thee) {
	longdouble distance = 0.0, mean = 0.0;
	Melder_assert (my nx == thy nx);
	for (integer i = 1; i <= my nx; i ++) {
		double dper = my z [1] [i] - thy z [1] [i];
		mean += dper;
		distance += dper * dper;
	}
	mean /= my nx;
	distance /= my nx;
	/* distance -= mean * mean; */
	return sqrt ((double) distance);
}

double Excitation_getLoudness (Excitation me) {
	longdouble loudness = 0.0;
	for (integer i = 1; i <= my nx; i ++)
		/*  Sones = 2 ** ((Phones - 40) / 10)  */
		loudness += pow (2.0, (my z [1] [i] - 40.0) / 10.0);
	return my dx * (double) loudness;
}

void Excitation_draw (Excitation me, Graphics g,
	double fmin, double fmax, double minimum, double maximum, bool garnish)
{
	if (fmax <= fmin) { fmin = my xmin; fmax = my xmax; }
	integer ifmin, ifmax;
	Matrix_getWindowSamplesX (me, fmin, fmax, & ifmin, & ifmax);
	if (maximum <= minimum)
		Matrix_getWindowExtrema (me, ifmin, ifmax, 1, 1, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 20.0; maximum += 20.0; }
	Graphics_setInner (g);
	Graphics_setWindow (g, fmin, fmax, minimum, maximum);
	Graphics_function (g, & my z [1] [0], ifmin, ifmax,
		Matrix_columnToX (me, ifmin), Matrix_columnToX (me, ifmax));
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Frequency (Bark)");
		Graphics_textLeft (g, true, U"Excitation (phon)");
		Graphics_marksBottomEvery (g, 1.0, 5.0, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 20.0, true, true, false);
	}
}

autoMatrix Excitation_to_Matrix (Excitation me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.get());   // BUG: safe, but compiler should be able to check
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoExcitation Matrix_to_Excitation (Matrix me) {
	try {
		autoExcitation thee = Thing_new (Excitation);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Excitation.");
	}
}

/* End of file Excitation.cpp */
