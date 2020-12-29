/* Sound_to_SPINET.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
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
 djmw 20020813 GPL header
 djmw 20070103 Sound interface changes
*/

#include "Sound_to_SPINET.h"
#include "NUM2.h"

static double fgamma (double x, integer n) {
	const double x2p1 = 1.0 + x * x;
	double d = x2p1;
	for (integer i = 2; i <= n; i ++)
		d *= x2p1;
	return 1.0 / d;
}

/*
	precondition:
	0 < minimumFrequencyHz < maximumFrequencyHz
*/

autoSPINET Sound_to_SPINET (Sound me, double timeStep, double windowDuration, double minimumFrequencyHz, double maximumFrequencyHz, integer numberOfGammaFilters, double excitationErbProportion, double inhibitionErbProportion) {
	try {
		const double b = 1.02, samplingFrequency = 1.0 / my dx;

		if (timeStep < my dx)
			timeStep = my dx;
		if (maximumFrequencyHz > samplingFrequency / 2.0)
			maximumFrequencyHz = samplingFrequency / 2.0;

		integer numberOfFrames;
		double firstTime;
		Sampled_shortTermAnalysis (me, windowDuration, timeStep, & numberOfFrames, & firstTime);
		autoSPINET thee = SPINET_create (my xmin, my xmax, numberOfFrames, timeStep, firstTime, minimumFrequencyHz, maximumFrequencyHz, numberOfGammaFilters, excitationErbProportion, inhibitionErbProportion);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoSound frame = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoVEC f = raw_VEC (numberOfGammaFilters);
		autoVEC bw = raw_VEC (numberOfGammaFilters);
		autoVEC aex = raw_VEC (numberOfGammaFilters);
		autoVEC ain = raw_VEC (numberOfGammaFilters);
		/*
			Cochlear filterbank: gammatone.
		*/
		for (integer i = 1; i <= numberOfGammaFilters; i ++) {
			f [i] = NUMerbToHertz (thy y1 + (i - 1) * thy dy);
			bw [i] = NUM2pi * b * (f [i] * (6.23e-6 * f [i] + 93.39e-3) + 28.52);
		}

		autoMelderProgress progress (U"SPINET analysis");

		for (integer i = 1; i <= numberOfGammaFilters; i ++) {
			const double bb = (f [i] / 1000.0) * exp (- f [i] / 1000.0); // outer & middle ear and phase locking
			const double tgammaMax = (thy gamma - 1) / bw [i]; // the time where the gamma function envelope has its maximum
			const double gammaMaxAmplitude = pow ((thy gamma - 1) / (NUMe * bw [i]), thy gamma - 1);
			const double timeCorrection = tgammaMax - windowDuration / 2.0;

			autoSound gammaTone = Sound_createGammaTone (0.0, 0.1, samplingFrequency, thy gamma, b, f [i], 0.0, 0.0, false);
			autoSound filtered = Sounds_convolve (me, gammaTone.get(), kSounds_convolve_scaling::SUM, kSounds_convolve_signalOutsideTimeDomain::ZERO);
			/*
				To energy measure: weigh with broad-band transfer function.
			*/
			for (integer j = 1; j <= numberOfFrames; j ++) {
				Sound_into_Sound (filtered.get(), frame.get(), Sampled_indexToX (thee.get(), j) + timeCorrection);
				Sounds_multiply (frame.get(), window.get());
				thy y [i] [j] = Sound_power (frame.get()) * bb / gammaMaxAmplitude;
			}
			Melder_progress ((double) i / numberOfGammaFilters, U"SPINET: filter ", i, U" from ", numberOfGammaFilters, U".");
		}
		/*
			Excitatory and inhibitory area functions.
		*/
		for (integer i = 1; i <= numberOfGammaFilters; i ++) {
			for (integer k = 1; k <= numberOfGammaFilters; k ++) {
				const double fr = (f [k] - f [i]) / bw [i];
				aex [i] += fgamma (fr / thy excitationErbProportion, thy gamma);
				ain [i] += fgamma (fr / thy inhibitionErbProportion, thy gamma);
			}
		}
		/*
			On-center off-surround interactions
		*/
		for (integer j = 1; j <= numberOfFrames; j ++)
			for (integer i = 1; i <= numberOfGammaFilters; i ++) {
				longdouble a = 0.0;
				for (integer k = 1; k <= numberOfGammaFilters; k ++) {
					const double fr = (f [k] - f [i]) / bw [i];
					const double hexsq = fgamma (fr / thy excitationErbProportion, thy gamma);
					const double hinsq = fgamma (fr / thy inhibitionErbProportion, thy gamma);
					a += thy y [k] [j] * (hexsq / aex [i] - hinsq / ain [i]);
				}
				thy s [i] [j] = a > 0.0 ? (double) a : 0.0;
			}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U":  no SPINET created.");
	}
}

/* End of file Sound_to_SPINET.cpp */
