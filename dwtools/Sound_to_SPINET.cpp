/* Sound_to_SPINET.cpp
 *
 * Copyright (C) 1993-2013 David Weenink
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
 djmw 20020813 GPL header
 djmw 20070103 Sound interface changes
*/

#include "Sound_to_SPINET.h"
#include "NUM2.h"

static double fgamma (double x, long n) {
	double x2p1 = 1 + x * x, d = x2p1;
	for (long i = 2; i <= n; i++) {
		d *= x2p1;
	}
	return 1 / d;
}

/*
	precondition:
	0 < minimumFrequencyHz < maximumFrequencyHz
*/

SPINET Sound_to_SPINET (Sound me, double timeStep, double windowDuration,
                        double minimumFrequencyHz, double maximumFrequencyHz, long nFilters,
                        double excitationErbProportion, double inhibitionErbProportion) {
	try {
		double firstTime, b = 1.02, samplingFrequency = 1 / my dx;

		if (timeStep < my dx) {
			timeStep = my dx;
		}
		if (maximumFrequencyHz > samplingFrequency / 2) {
			maximumFrequencyHz = samplingFrequency / 2;
		}

		long numberOfFrames;
		Sampled_shortTermAnalysis (me, windowDuration, timeStep, &numberOfFrames, &firstTime);
		autoSPINET thee = SPINET_create (my xmin, my xmax, numberOfFrames, timeStep, firstTime,
		                                 minimumFrequencyHz, maximumFrequencyHz, nFilters, excitationErbProportion, inhibitionErbProportion);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoSound frame = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoNUMvector<double> f (1, nFilters);
		autoNUMvector<double> bw (1, nFilters);
		autoNUMvector<double> aex (1, nFilters);
		autoNUMvector<double> ain (1, nFilters);

		// Cochlear filterbank: gammatone

		for (long i = 1; i <= nFilters; i++) {
			f[i] = NUMerbToHertz (thy y1 + (i - 1) * thy dy);
			bw[i] = 2 * NUMpi * b * (f[i] * (6.23e-6 * f[i] + 93.39e-3) + 28.52);
		}

		autoMelderProgress progress (L"SPINET analysis");

		for (long i = 1; i <= nFilters; i++) {
			double bb = (f[i] / 1000) * exp (- f[i] / 1000); // outer & middle ear and phase locking
			double tgammaMax = (thy gamma - 1) / bw[i]; // Time where gammafunction envelope has maximum
			double gammaMaxAmplitude = pow ( (thy gamma - 1) / (NUMe * bw[i]), (thy gamma - 1)); // tgammaMax
			double timeCorrection = tgammaMax - windowDuration / 2;

			autoSound gammaTone = Sound_createGammaTone (0, 0.1, samplingFrequency,
			                      thy gamma, b, f[i], 0, 0, 0);
			autoSound filtered = Sounds_convolve (me, gammaTone.peek(), kSounds_convolve_scaling_SUM, kSounds_convolve_signalOutsideTimeDomain_ZERO);

			// To energy measure: weigh with broad-band transfer function

			for (long j = 1; j <= numberOfFrames; j++) {
				Sound_into_Sound (filtered.peek(), frame.peek(), Sampled_indexToX (thee.peek(), j) + timeCorrection);
				Sounds_multiply (frame.peek(), window.peek());
				thy y[i][j] = Sound_power (frame.peek()) * bb / gammaMaxAmplitude;
			}
			Melder_progress ( (double) i / nFilters, L"SPINET: filter ", Melder_integer (i), L" from ",
			                   Melder_integer (nFilters), L".");
		}

		// Excitatory and inhibitory area functions

		for (long i = 1; i <= nFilters; i++) {
			for (long k = 1; k <= nFilters; k++) {
				double fr = (f[k] - f[i]) / bw[i];
				aex[i] += fgamma (fr / thy excitationErbProportion, thy gamma);
				ain[i] += fgamma (fr / thy inhibitionErbProportion, thy gamma);
			}
		}

		// On-center off-surround interactions

		for (long j = 1; j <= numberOfFrames; j++)
			for (long i = 1; i <= nFilters; i++) {
				double a = 0;
				for (long k = 1; k <= nFilters; k++) {
					double fr = (f[k] - f[i]) / bw[i];
					double hexsq = fgamma (fr / thy excitationErbProportion, thy gamma);
					double hinsq = fgamma (fr / thy inhibitionErbProportion, thy gamma);
					a += thy y[k][j] * (hexsq / aex[i] - hinsq / ain[i]);
				}
				thy s[i][j] = a > 0 ? a : 0;
			}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ":  no SPINET created.");
	}
}

/* End of file Sound_to_SPINET.cpp */
