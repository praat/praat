/* LPC_and_Formant.cpp
 *
 * Copyright (C) 1994-2013, 2015-2016 David Weenink
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

void Formant_Frame_init (Formant_Frame me, integer nFormants) {
	my nFormants = nFormants;
	if (nFormants > 0) {
		my formant = NUMvector<structFormant_Formant> (1, my nFormants);
	}
}

void Formant_Frame_scale (Formant_Frame me, double scale) {
	for (integer i = 1; i <= my nFormants; i ++) {
		my formant [i].frequency *= scale;
		my formant [i].bandwidth *= scale;
	}
}

void Roots_into_Formant_Frame (Roots me, Formant_Frame thee, double samplingFrequency, double margin) {
	integer n = my max - my min + 1;
	autoNUMvector<double> fc (1, n);
	autoNUMvector<double> bc (1, n);

	// Determine the formants and bandwidths

	thy nFormants = 0;
	double fLow = margin, fHigh = samplingFrequency / 2 - margin;
	for (integer i = my min; i <= my max; i ++) {
		if (my v [i].im < 0) {
			continue;
		}
		double f = fabs (atan2 (my v [i].im, my v [i].re)) * samplingFrequency / 2.0 / NUMpi;
		if (f >= fLow && f <= fHigh) {
			/*b = - log (my v [i].re * my v [i].re + my v [i].im * my v [i].im) * samplingFrequency / 2 / NUMpi;*/
			double b = - log (dcomplex_abs (my v [i])) * samplingFrequency / NUMpi;
			thy nFormants ++;
			fc [thy nFormants] = f;
			bc [thy nFormants] = b;
		}
	}

	Formant_Frame_init (thee, thy nFormants);

	for (integer i = 1; i <= thy nFormants; i ++) {
		thy formant [i].frequency = fc [i];
		thy formant [i].bandwidth = bc [i];
	}
}

void LPC_Frame_into_Formant_Frame (LPC_Frame me, Formant_Frame thee, double samplingPeriod, double margin) {
	thy intensity = my gain;
	if (my nCoefficients == 0) {
		return;
	}

	autoPolynomial p = LPC_Frame_to_Polynomial (me);
	autoRoots r = Polynomial_to_Roots (p.get());
	Roots_fixIntoUnitCircle (r.get());
	Roots_into_Formant_Frame (r.get(), thee, 1 / samplingPeriod, margin);
}

autoFormant LPC_to_Formant (LPC me, double margin) {
	try {
		double samplingFrequency = 1.0 / my samplingPeriod;
		integer nmax = my maxnCoefficients, err = 0;
		integer interval = nmax > 20 ? 1 : 10;
		Melder_require (nmax < 100, U"We cannot find the roots of a polynomial of order > 99.");
		Melder_require (margin < samplingFrequency / 4, U"Margin should be smaller than ", samplingFrequency / 4, U".");

		autoFormant thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, (nmax + 1) / 2);

		autoMelderProgress progress (U"LPC to Formant");

		for (integer i = 1; i <= my nx; i ++) {
			Formant_Frame formant = & thy d_frames [i];
			LPC_Frame lpc = & my d_frames [i];

			// Initialisation of Formant_Frame is taken care of in Roots_into_Formant_Frame!

			try {
				LPC_Frame_into_Formant_Frame (lpc, formant, my samplingPeriod, margin);
			} catch (MelderError) {
				Melder_clearError();
				err ++;
			}

			if ((interval == 1 || (i % interval) == 1)) {
				Melder_progress ( (double) i / my nx, U"LPC to Formant: frame ", i, U" out of ", my nx, U".");
			}
		}

		Formant_sort (thee.get());
		if (err > 0) {
			Melder_warning (err, U" formant frames out of ", my nx, U" are suspect.");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Formant created.");
	}
}

void Formant_Frame_into_LPC_Frame (Formant_Frame me, LPC_Frame thee, double samplingPeriod) {
	integer m = 2, n = 2 * my nFormants;

	if (my nFormants < 1) {
		return;
	}
	autoNUMvector<double> lpc (-1, n);

	lpc [0] = 1;
	double nyquist = 2.0 / samplingPeriod;
	for (integer i = 1; i <= my nFormants; i ++) {
		double f = my formant [i].frequency;

		if (f > nyquist) {
			continue;
		}

		// D(z): 1 + p z^-1 + q z^-2

		double r = exp (- NUMpi * my formant [i].bandwidth * samplingPeriod);
		double p = - 2 * r * cos (2 * NUMpi * f * samplingPeriod);
		double q = r * r;

		for (integer j = m; j > 0; j --) {
			lpc [j] += p * lpc [j - 1] + q * lpc [j - 2];
		}

		m += 2;
	}

	n = thy nCoefficients < n ? thy nCoefficients : n;

	for (integer i = 1; i <= n ; i ++) {
		thy a [i] = lpc [i];
	}

	thy gain = my intensity;
}

autoLPC Formant_to_LPC (Formant me, double samplingPeriod) {
	try {
		autoLPC thee = LPC_create (my xmin, my xmax, my nx, my dx, my x1, 2 * my maxnFormants, samplingPeriod);

		for (integer i = 1; i <= my nx; i ++) {
			Formant_Frame f = & my d_frames [i];
			LPC_Frame lpc = & thy d_frames [i];
			integer m = 2 * f -> nFormants;

			LPC_Frame_init (lpc, m);
			Formant_Frame_into_LPC_Frame (f, lpc, samplingPeriod);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC created.");
	}
}

/* End of file LPC_and_Formant.cpp */
