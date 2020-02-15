/* LPC_and_Formant.cpp
 *
 * Copyright (C) 1994-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20030616 Formant_Frame_into_LPC_Frame: remove formant with f >= Nyquist +
 		change lpc indexing from -1..m
 djmw 20080122 float -> double
*/

#include "LPC_and_Formant.h"
#include "LPC_and_Polynomial.h"
#include "NUM2.h"

void Formant_Frame_init (Formant_Frame me, integer numberOfFormants) {
	my nFormants = numberOfFormants;
	if (numberOfFormants > 0)
		my formant = newvectorzero <structFormant_Formant> (my nFormants);
}

void Formant_Frame_scale (Formant_Frame me, double scale) {
	for (integer i = 1; i <= my nFormants; i ++) {
		my formant [i]. frequency *= scale;
		my formant [i]. bandwidth *= scale;
	}
}

void Roots_into_Formant_Frame (Roots me, Formant_Frame thee, double samplingFrequency, double margin) {
	const integer n = my max - my min + 1;
	autoVEC fc = newVECzero (n);
	autoVEC bc = newVECzero (n);
	/*
		Determine the formants and bandwidths
	*/
	thy nFormants = 0;
	const double fLow = margin, fHigh = samplingFrequency / 2 - margin;
	for (integer i = my min; i <= my max; i ++) {
		if (my v [i].imag() < 0.0)
			continue;
		const double f = fabs (atan2 (my v [i].imag(), my v [i].real())) * samplingFrequency / 2.0 / NUMpi;
		if (f >= fLow && f <= fHigh) {
			double b = - log (dcomplex_abs (my v [i])) * samplingFrequency / NUMpi;
			thy nFormants ++;
			fc [thy nFormants] = f;
			bc [thy nFormants] = b;
		}
	}

	Formant_Frame_init (thee, thy nFormants);

	for (integer i = 1; i <= thy nFormants; i ++) {
		thy formant [i]. frequency = fc [i];
		thy formant [i]. bandwidth = bc [i];
	}
}

void LPC_Frame_into_Formant_Frame (LPC_Frame me, Formant_Frame thee, double samplingPeriod, double margin) {
	thy intensity = my gain;
	if (my nCoefficients == 0)
		return;
	autoPolynomial p = LPC_Frame_to_Polynomial (me);
	autoRoots r = Polynomial_to_Roots (p.get());
	Roots_fixIntoUnitCircle (r.get());
	Roots_into_Formant_Frame (r.get(), thee, 1.0 / samplingPeriod, margin);
}

autoFormant LPC_to_Formant (LPC me, double margin) {
	try {
		const double samplingFrequency = 1.0 / my samplingPeriod;
		const integer nmax = my maxnCoefficients;
		integer numberOfSuspectFrames = 0;
		const integer interval = ( nmax > 20 ? 1 : 10 );
		Melder_require (nmax < 100,
			U"We cannot find the roots of a polynomial of order > 99.");
		Melder_require (margin < samplingFrequency / 4.0,
			U"Margin should be smaller than ", samplingFrequency / 4.0, U".");

		autoFormant thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, (nmax + 1) / 2);

		autoMelderProgress progress (U"LPC to Formant");

		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame formant = & thy frames [iframe];
			const LPC_Frame lpc = & my d_frames [iframe];
			/*
				Initialisation of Formant_Frame is taken care of in Roots_into_Formant_Frame!
			*/
			try {
				LPC_Frame_into_Formant_Frame (lpc, formant, my samplingPeriod, margin);
			} catch (MelderError) {
				Melder_clearError();
				numberOfSuspectFrames ++;
			}
			if (interval == 1 || iframe % interval == 1)
				Melder_progress ((double) iframe / my nx, U"LPC to Formant: frame ", iframe, U" out of ", my nx, U".");
		}
		Formant_sort (thee.get());
		if (numberOfSuspectFrames > 0)
			Melder_warning (numberOfSuspectFrames, U" formant frames out of ", my nx, U" are suspect.");
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Formant created.");
	}
}

void Formant_Frame_into_LPC_Frame (Formant_Frame me, LPC_Frame thee, double samplingPeriod) {
	if (my nFormants < 1)
		return;
	const double nyquistFrequency = 0.5 / samplingPeriod;
	integer numberOfPoles = 2 * my nFormants;
	autoVEC lpc = newVECzero (numberOfPoles + 2);   // all odd coefficients have to be initialized to zero
	lpc [2] = 1.0;
	integer m = 2;
	for (integer iformant = 1; iformant <= my nFormants; iformant ++) {
		const double formantFrequency = my formant [iformant]. frequency;
		if (formantFrequency > nyquistFrequency)
			continue;
		/*
			D(z): 1 + p z^-1 + q z^-2
		*/
		const double r = exp (- NUMpi * my formant [iformant]. bandwidth * samplingPeriod);
		const double p = - 2.0 * r * cos (NUM2pi * formantFrequency * samplingPeriod);
		const double q = r * r;
		/*
			By setting the two extra elements (0, 1) in the lpc vector we can avoid boundary testing;
			lpc [3..n+2] come to contain the coefficients.
		*/
		for (integer j = m + 2; j > 2; j --)
			lpc [j] += p * lpc [j - 1] + q * lpc [j - 2];
		m += 2;
	}
	if (thy nCoefficients < numberOfPoles)
		numberOfPoles = thy nCoefficients;
	for (integer i = 1; i <= numberOfPoles; i ++)
		thy a [i] = lpc [i + 2];
	thy gain = my intensity;
}

autoLPC Formant_to_LPC (Formant me, double samplingPeriod) {
	try {
		autoLPC thee = LPC_create (my xmin, my xmax, my nx, my dx, my x1, 2 * my maxnFormants, samplingPeriod);

		for (integer i = 1; i <= my nx; i ++) {
			const Formant_Frame f = & my frames [i];
			const LPC_Frame lpc = & thy d_frames [i];
			const integer numberOfCoefficients = 2 * f -> nFormants;
			LPC_Frame_init (lpc, numberOfCoefficients);
			Formant_Frame_into_LPC_Frame (f, lpc, samplingPeriod);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC created.");
	}
}

/* End of file LPC_and_Formant.cpp */
