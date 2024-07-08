/* LPC_and_Formant.cpp
 *
 * Copyright (C) 1994-2024 David Weenink
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

#include "LPCToFormantWorkspace.h"
#include "LPC_and_Formant.h"
#include "LPC_and_Polynomial.h"
#include "NUM2.h"
#include "Roots_and_Formant.h"

void LPC_into_Formant (constLPC me, mutableFormant thee, double margin) {
	Sampled_requireEqualDomainsAndSampling (me, thee);
	
	
}

autoFormant LPC_to_Formant (constLPC me, double margin) {
	try {
		/*
			In very extreme case all roots of the lpc polynomial might be real.
			A real root gives either a frequency at 0 Hz or at the Nyquist frequency.
			If margin > 0 these frequencies are filtered out and the number of formants can never exceed
			(my maxnCoefficients+1) / 2.
		*/
		const integer maximumNumberOfFormants = ( margin == 0.0 ? my maxnCoefficients : (my maxnCoefficients + 1) / 2 );
		Melder_require (my maxnCoefficients < 100,
			U"We cannot find the roots of a polynomial of order > 99.");
		autoFormant thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, maximumNumberOfFormants);
		autoLPCToFormantWorkspace ws = LPCToFormantWorkspace_create (me, thee.get(), margin);
		SampledToSampledWorkspace_analyseThreaded (ws.get());
		
		Formant_sort (thee.get());
		if (ws -> globalFrameErrorCount > 0)
			Melder_warning (ws -> globalFrameErrorCount, U" formant frames out of ", thy nx, U" are suspect.");
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Formant created.");
	}
}

void Formant_Frame_scale (Formant_Frame me, double scale) {
	for (integer iformant = 1; iformant <= my numberOfFormants; iformant ++) {
		my formant [iformant]. frequency *= scale;
		my formant [iformant]. bandwidth *= scale;
	}
}

void LPC_Frame_into_Formant_Frame (constLPC_Frame me, Formant_Frame thee, double samplingPeriod, double margin) {
	Melder_assert (my nCoefficients == my a.size); // check invariant
	thy intensity = my gain;
	if (my nCoefficients == 0) {
		thy formant.resize (0);
		thy numberOfFormants = thy formant.size; // maintain invariant
		return;
	}
	autoPolynomial p = LPC_Frame_to_Polynomial (me);
	autoRoots r = Polynomial_to_Roots (p.get());
	Roots_fixIntoUnitCircle (r.get());
	Roots_into_Formant_Frame (r.get(), thee, 1.0 / samplingPeriod, margin);
}

void Formant_Frame_into_LPC_Frame (constFormant_Frame me, LPC_Frame thee, double samplingPeriod) {
	if (my numberOfFormants < 1)
		return;
	const double nyquistFrequency = 0.5 / samplingPeriod;
	integer numberOfPoles = 2 * my numberOfFormants;
	autoVEC lpc = zero_VEC (numberOfPoles + 2);   // all odd coefficients have to be initialized to zero
	lpc [2] = 1.0;
	integer m = 2;
	for (integer iformant = 1; iformant <= my numberOfFormants; iformant ++) {
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

autoLPC Formant_to_LPC (constFormant me, double samplingPeriod) {
	try {
		autoLPC thee = LPC_create (my xmin, my xmax, my nx, my dx, my x1, 2 * my maxnFormants, samplingPeriod);

		for (integer i = 1; i <= my nx; i ++) {
			const Formant_Frame f = & my frames [i];
			const LPC_Frame lpc = & thy d_frames [i];
			const integer numberOfCoefficients = 2 * f -> numberOfFormants;
			LPC_Frame_init (lpc, numberOfCoefficients);
			Formant_Frame_into_LPC_Frame (f, lpc, samplingPeriod);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC created.");
	}
}

/* End of file LPC_and_Formant.cpp */
