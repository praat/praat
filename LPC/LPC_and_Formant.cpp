/* LPC_and_Formant.cpp
 *
 * Copyright (C) 1994-2013 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20030616 Formant_Frame_into_LPC_Frame: remove formant with f >= Nyquist +
 		change lpc indexing from -1..m
 djmw 20080122 float -> double
*/

#include "LPC_and_Formant.h"
#include "LPC_and_Polynomial.h"
#include "NUM2.h"

void Formant_Frame_init (Formant_Frame me, long nFormants) {
	my nFormants = nFormants;
	if (nFormants > 0) {
		my formant = NUMvector<structFormant_Formant> (1, my nFormants);
	}
}

void Formant_Frame_scale (Formant_Frame me, double scale) {
	for (long i = 1; i <= my nFormants; i++) {
		my formant[i].frequency *= scale;
		my formant[i].bandwidth *= scale;
	}
}

void Roots_into_Formant_Frame (Roots me, Formant_Frame thee, double samplingFrequency, double margin) {
	long n = my max - my min + 1;
	autoNUMvector<double> fc (1, n);
	autoNUMvector<double> bc (1, n);

	// Determine the formants and bandwidths

	thy nFormants = 0;
	double fLow = margin, fHigh = samplingFrequency / 2 - margin;
	for (long i = my min; i <= my max; i++) {
		if (my v[i].im < 0) {
			continue;
		}
		double f = fabs (atan2 (my v[i].im, my v[i].re)) * samplingFrequency / 2 / NUMpi;
		if (f >= fLow && f <= fHigh) {
			/*b = - log (my v[i].re * my v[i].re + my v[i].im * my v[i].im) * samplingFrequency / 2 / NUMpi;*/
			double b = - log (dcomplex_abs (my v[i])) * samplingFrequency / NUMpi;
			thy nFormants++;
			fc[thy nFormants] = f;
			bc[thy nFormants] = b;
		}
	}

	Formant_Frame_init (thee, thy nFormants);

	for (long i = 1; i <= thy nFormants; i++) {
		thy formant[i].frequency = fc[i];
		thy formant[i].bandwidth = bc[i];
	}
}

void LPC_Frame_into_Formant_Frame (LPC_Frame me, Formant_Frame thee, double samplingPeriod, double margin) {
	thy intensity = my gain;
	if (my nCoefficients == 0) {
		return;
	}

	autoPolynomial p = LPC_Frame_to_Polynomial (me);
	autoRoots r = Polynomial_to_Roots (p.peek());
	Roots_fixIntoUnitCircle (r.peek());
	Roots_into_Formant_Frame (r.peek(), thee, 1 / samplingPeriod, margin);
}

Formant LPC_to_Formant (LPC me, double margin) {
	try {
		double samplingFrequency = 1.0 / my samplingPeriod;
		long nmax = my maxnCoefficients, err = 0;
		long interval = nmax > 20 ? 1 : 10;

		if (nmax > 99) {
			Melder_throw ("We cannot find the roots of a polynomial of order > 99.");
		}
		if (margin >= samplingFrequency / 4) {
			Melder_throw ("Margin must be smaller than ", samplingFrequency / 4, ".");
		}

		autoFormant thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, (nmax + 1) / 2);

		autoMelderProgress progress (L"LPC to Formant");

		for (long i = 1; i <= my nx; i++) {
			Formant_Frame formant = & thy d_frames[i];
			LPC_Frame lpc = & my d_frames[i];

			// Initialisation of Formant_Frame is taken care of in Roots_into_Formant_Frame!

			try {
				LPC_Frame_into_Formant_Frame (lpc, formant, my samplingPeriod, margin);
			} catch (MelderError) {
				Melder_clearError();
				err++;
			}

			if ( (interval == 1 || (i % interval) == 1)) {
				Melder_progress ( (double) i / my nx, L"LPC to Formant: frame ", Melder_integer (i),
				                   L" out of ", Melder_integer (my nx), L".");
			}
		}

		Formant_sort (thee.peek());
		if (err > 0) {
			Melder_warning (Melder_integer (err), L" formant frames out of ", Melder_integer (my nx), L" suspect.");
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Formant created.");
	}
}

void Formant_Frame_into_LPC_Frame (Formant_Frame me, LPC_Frame thee, double samplingPeriod) {
	long m = 2, n = 2 * my nFormants;

	if (my nFormants < 1) {
		return;
	}
	autoNUMvector<double> lpc (-1, n);

	lpc[0] = 1;
	double nyquist = 2.0 / samplingPeriod;
	for (long i = 1; i <= my nFormants; i++) {
		double f = my formant[i].frequency;

		if (f > nyquist) {
			continue;
		}

		// D(z): 1 + p z^-1 + q z^-2

		double r = exp (- NUMpi * my formant[i].bandwidth * samplingPeriod);
		double p = - 2 * r * cos (2 * NUMpi * f * samplingPeriod);
		double q = r * r;

		for (long j = m; j > 0; j--) {
			lpc[j] += p * lpc[j - 1] + q * lpc[j - 2];
		}

		m += 2;
	}

	n = thy nCoefficients < n ? thy nCoefficients : n;

	for (long i = 1; i <= n ; i++) {
		thy a[i] = lpc[i];
	}

	thy gain = my intensity;
}

LPC Formant_to_LPC (Formant me, double samplingPeriod) {
	try {
		autoLPC thee = LPC_create (my xmin, my xmax, my nx, my dx, my x1, 2 * my maxnFormants, samplingPeriod);

		for (long i = 1; i <= my nx; i++) {
			Formant_Frame f = & my d_frames[i];
			LPC_Frame lpc = & thy d_frames[i];
			long m = 2 * f -> nFormants;

			LPC_Frame_init (lpc, m);
			Formant_Frame_into_LPC_Frame (f, lpc, samplingPeriod);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no LPC created.");
	}
}

/* End of file LPC_and_Formant.cpp */
