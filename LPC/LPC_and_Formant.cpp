/* LPC_and_Formant.cpp
 *
 * Copyright (C) 1994-2020 David Weenink
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
#include <thread>
#include <atomic>

void Formant_Frame_init (Formant_Frame me, integer numberOfFormants) {
	my nFormants = numberOfFormants;
	if (numberOfFormants > 0)
		my formant = newvectorzero <structFormant_Formant> (numberOfFormants);
}

void Formant_Frame_scale (Formant_Frame me, double scale) {
	for (integer iformant = 1; iformant <= my nFormants; iformant ++) {
		my formant [iformant]. frequency *= scale;
		my formant [iformant]. bandwidth *= scale;
	}
}

void Roots_into_Formant_Frame (Roots me, Formant_Frame thee, double samplingFrequency, double margin) {
	/*
		Determine the formants and bandwidths
	*/
	Melder_assert (my roots.size <= 2 * thy formant.size);
	integer numberOfFormantsFound = 0;
	const double nyquistFrequency = 0.5 * samplingFrequency;
	const double fLow = margin, fHigh = nyquistFrequency - margin;
	for (integer iroot = 1; iroot <= my numberOfRoots; iroot ++) {
		if (my roots [iroot].imag() < 0.0)
			continue;
		const double frequency = fabs (atan2 (my roots [iroot].imag(), my roots [iroot].real())) * nyquistFrequency / NUMpi;
		if (frequency >= fLow && frequency <= fHigh) {
			const double bandwidth = - log (norm (my roots [iroot])) * nyquistFrequency / NUMpi;
			thy formant [++ numberOfFormantsFound]. frequency = frequency;
			thy formant [numberOfFormantsFound]. bandwidth = bandwidth;
		}
	}
	Melder_assert (numberOfFormantsFound <= thy formant.size);
	thy nFormants = numberOfFormantsFound;
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

void LPC_Frame_into_Formant_Frame_mt (LPC_Frame me, Formant_Frame thee, double samplingPeriod, double margin, Polynomial p, Roots r, VEC const& workspace) {
	thy intensity = my gain;
	if (my nCoefficients == 0)
		return;
	LPC_Frame_into_Polynomial (me, p);
	Polynomial_into_Roots (p, r, workspace);
	Roots_fixIntoUnitCircle (r);
	Roots_into_Formant_Frame (r, thee, 1.0 / samplingPeriod, margin);
}

autoFormant LPC_to_Formant (LPC me, double margin) {
	try {
		const double samplingFrequency = 1.0 / my samplingPeriod;
		/*
			In very extreme case all roots of the lpc polynomial might be real.
			A real root gives either a frequency at 0 Hz or at the Nyquist frequency.
			If margin > 0 these frequencies are filtered out and the number of formants can never exceed
			my maxnCoefficients / 2.
		*/
		const integer maximumNumberOfFormants = ( margin == 0.0 ? my maxnCoefficients : my maxnCoefficients / 2 );
		const integer maximumNumberOfPolynomialCoefficients = my maxnCoefficients + 1;
		integer numberOfSuspectFrames = 0;
		const integer interval = ( my maxnCoefficients > 20 ? 1 : 10 );
		Melder_require (my maxnCoefficients < 100,
			U"We cannot find the roots of a polynomial of order > 99.");
		Melder_require (margin < samplingFrequency / 4.0,
			U"Margin should be smaller than ", samplingFrequency / 4.0, U".");
		autoFormant thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, maximumNumberOfFormants);
		autoPolynomial polynomial = Polynomial_create (-1.0, 1.0, my maxnCoefficients);
		autoRoots roots = Roots_create (my maxnCoefficients);
		autoVEC workspace = newVECraw (maximumNumberOfPolynomialCoefficients * (maximumNumberOfPolynomialCoefficients + 3));
		autoMelderProgress progress (U"LPC to Formant");
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const LPC_Frame lpcFrame = & my d_frames [iframe];
			const Formant_Frame formantFrame = & thy frames [iframe];
			Formant_Frame_init (formantFrame, maximumNumberOfFormants);
			try {
				LPC_Frame_into_Formant_Frame_mt (lpcFrame, formantFrame, my samplingPeriod, margin, polynomial.get(), roots.get(), workspace.get());
				//LPC_Frame_into_Formant_Frame (lpcFrame, formant, my samplingPeriod, margin);
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

/*
	20200404: This routine cannot be used because our version of LAPACK is not thread-safe yet.
*/
autoFormant LPC_to_Formant_mt (LPC me, double margin) {
	try {
		const double samplingFrequency = 1.0 / my samplingPeriod;
		Melder_require (my maxnCoefficients < 100,
			U"We cannot find the roots of a polynomial of order > 99.");
		Melder_require (margin < samplingFrequency / 4.0,
			U"Margin should be smaller than ", samplingFrequency / 4.0, U".");
		/*
			In a very extreme case all roots of the lpc polynomial might be real.
			A real root gives either a frequency at 0 Hz or at the Nyquist frequency.
			To play it safely, the maximum number of formants is than equal to the number of coefficients.
			However, to get the acoustically "real" formant frequencies we always neglect frequencies at 0 Hz
			and at the Nyquist.
			If margin > 0 these frequencies are filtered out and the number of formants can never exceed
			int ( my maxnCoefficients / 2 ) because if maxnCoefficients is uneven than at least one of the roots is real.
		*/
		const integer maximumNumberOfFormants = ( margin == 0.0 ? my maxnCoefficients : my maxnCoefficients / 2 );
		const integer maximumNumberOfPolynomialCoefficients = my maxnCoefficients + 1;
		const integer numberOfFrames = my nx;
		autoFormant thee = Formant_create (my xmin, my xmax, numberOfFrames, my dx, my x1, maximumNumberOfFormants);
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const Formant_Frame formantFrame = & thy frames [iframe];
			Formant_Frame_init (formantFrame, maximumNumberOfFormants);
		}
		
		constexpr integer maximumNumberOfThreads = 16;
		integer numberOfThreads, numberOfFramesPerThread = 25;
		const integer numberOfProcessors = std::thread::hardware_concurrency ();
		NUMgetThreadingInfo (numberOfFrames, std::min (numberOfProcessors, maximumNumberOfThreads), & numberOfFramesPerThread, & numberOfThreads);
		/*
			Reserve working memory for each thread
			We would like:
			autovector<autoPolynomial> polynomials = newveczero<autoPolynomial> (numberOfThreads);
		*/
		autoPolynomial polynomials [maximumNumberOfThreads + 1];
		autoRoots roots [maximumNumberOfThreads + 1];
		for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
			polynomials [ithread] = Polynomial_create (-1.0, 1.0, my maxnCoefficients);
			roots [ithread] = Roots_create (my maxnCoefficients);
		}
		autoMAT workspaces = newMATraw (numberOfThreads, maximumNumberOfPolynomialCoefficients * (maximumNumberOfPolynomialCoefficients + 3));
		autovector<std::thread> thread = newvectorzero<std::thread> (numberOfThreads); // TODO memory leak?
		std::atomic<integer> numberOfSuspectFrames (0);
		
		try {
			for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
				Polynomial p = polynomials [ithread]. get ();
				Roots r = roots [ithread]. get ();
				Formant formant = thee. get ();
				LPC lpc = me;
				VEC workspace = workspaces. row (ithread);
				const integer firstFrame = 1 + (ithread - 1) * numberOfFramesPerThread;
				const integer lastFrame = ( ithread == numberOfThreads ? numberOfFrames : firstFrame + numberOfFramesPerThread - 1 );

				thread [ithread] = std::thread ([=, & numberOfSuspectFrames] () {
					for (integer iframe = firstFrame; iframe <= lastFrame; iframe ++) {
						const LPC_Frame lpcFrame = & lpc -> d_frames [iframe];
						const Formant_Frame formantFrame = & formant -> frames [iframe];
						try {
							LPC_Frame_into_Formant_Frame_mt (lpcFrame, formantFrame, my samplingPeriod, margin, p, r, workspace);
						} catch (MelderError) {
							numberOfSuspectFrames ++;
						}
					}
				});
			}
		} catch (MelderError) {
			for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
				if (thread [ithread]. joinable ())
					thread [ithread]. join ();
			}
			throw;
		}
		for (integer ithread = 1; ithread <= numberOfThreads; ithread ++)
			thread [ithread]. join ();
	
				
		Formant_sort (thee. get ());
		if (numberOfSuspectFrames > 0)
			Melder_warning ((integer) numberOfSuspectFrames, U" formant frames out of ", numberOfFrames, U" are suspect.");
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
