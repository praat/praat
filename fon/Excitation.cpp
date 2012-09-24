/* Excitation.cpp
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

#include "Excitation.h"

Thing_implement (Excitation, Vector, 2);

double Excitation_hertzToBark (double hertz) {
	double h650 = hertz / 650;
	return 7.0 * log (h650 + sqrt (1 + h650 * h650));
}

double Excitation_barkToHertz (double bark) {
	return 650.0 * sinh (bark / 7.0);
}

double Excitation_phonToDifferenceLimens (double phon) {
	return 30 * (pow (61.0 / 60, phon) - 1);
}

double Excitation_differenceLimensToPhon (double ndli) {
	return log (1 + ndli / 30) / log (61.0 / 60);
}

double Excitation_soundPressureToPhon (double soundPressure, double bark) {
	double result, dum;

	if (soundPressure <= 0.0) return 0.0;

	/*  dB = 20 * log10 (soundPressure / threshold)  */
	result = 20 * log10 (soundPressure / 2.0e-5);   /* First approximation: phon = dB */

	/*  Phones from dB  */
	if (result < 90 && bark < 8.0)
	{
		dum = (90 - result) * (8.0 - bark);
		result -= dum * dum / 2500;
	}
	dum = bark / 3.6 - 5;
	result += 5 * exp (- dum * dum);
	if (bark > 20.0) { dum = bark - 20; result -= 0.5 * dum * dum; }
	if (result < 0) result = 0;
	return result;
}

void structExcitation :: v_info () {
	double *y = z [1];
	long numberOfMaxima = 0;
	structData :: v_info ();
	MelderInfo_writeLine (L"Loudness: ", Melder_half (Excitation_getLoudness (this)), L" sones");
	for (long i = 2; i < nx; i ++) if (y [i] > y [i - 1] && y [i] >= y [i + 1]) {
		double i_real, formant_bark, strength;
		if (++ numberOfMaxima > 15) break;
		strength = NUMimproveMaximum (z [1], nx, i, NUM_PEAK_INTERPOLATE_SINC70, & i_real);
		formant_bark = x1 + (i_real - 1) * dx;
		MelderInfo_write (L"Peak at ", Melder_single (formant_bark), L" Bark");
		MelderInfo_write (L", ", Melder_integer ((long) NUMbarkToHertz (formant_bark)), L" Hz");
		MelderInfo_writeLine (L", ", Melder_half (strength), L" phon.");
	}
}

Excitation Excitation_create (double df, long nf) {
	try {
		autoExcitation me = Thing_new (Excitation);
		Matrix_init (me.peek(), 0.0, nf * df, nf, df, 0.5 * df, 1, 1, 1, 1, 1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Excitation not created.");
	}
}

double Excitation_getDistance (Excitation me, Excitation thee) {
	double distance = 0.0, mean = 0.0;
	Melder_assert (my nx == thy nx);
	for (long i = 1; i <= my nx; i ++) {
		double dper = my z [1] [i] - thy z [1] [i];
		mean += dper;
		distance += dper * dper;
	}
	mean /= my nx;
	distance /= my nx;
	/* distance -= mean * mean; */
	return sqrt (distance);
}

double Excitation_getLoudness (Excitation me) {
	double loudness = 0.0;
	for (int i = 1; i <= my nx; i ++)
		/*  Sones = 2 ** ((Phones - 40) / 10)  */
		loudness += pow (2, (my z [1] [i] - 40) / 10);
	return my dx * loudness;
}

void Excitation_draw (Excitation me, Graphics g,
	double fmin, double fmax, double minimum, double maximum, int garnish)
{
	if (fmax <= fmin) { fmin = my xmin; fmax = my xmax; }
	long ifmin, ifmax;
	Matrix_getWindowSamplesX (me, fmin, fmax, & ifmin, & ifmax);
	if (maximum <= minimum)
		Matrix_getWindowExtrema (me, ifmin, ifmax, 1, 1, & minimum, & maximum);
	if (maximum <= minimum) { minimum -= 20; maximum += 20; }
	Graphics_setInner (g);
	Graphics_setWindow (g, fmin, fmax, minimum, maximum);
	Graphics_function (g, my z [1], ifmin, ifmax,
		Matrix_columnToX (me, ifmin), Matrix_columnToX (me, ifmax));
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Frequency (Bark)");
		Graphics_textLeft (g, 1, L"Excitation (phon)");
		Graphics_marksBottomEvery (g, 1, 5, 1, 1, 0);
		Graphics_marksLeftEvery (g, 1, 20, 1, 1, 0);
	}
}

Matrix Excitation_to_Matrix (Excitation me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.peek());   // BUG: safe, but compiler should be able to check
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

Excitation Matrix_to_Excitation (Matrix me) {
	try {
		autoExcitation thee = Thing_new (Excitation);
		my structMatrix :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Excitation.");
	}
}

/* End of file Excitation.cpp */
