/* LPC_to_Spectrogram.cpp
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
 djmw 20020812 GPL header
 djmw 20080122 float -> double
*/

#include "LPC_to_Spectrogram.h"

autoSpectrogram LPC_to_Spectrogram (LPC me, double dfMin, double bandwidthReduction, double deEmphasisFrequency) {
	try {
		const double samplingFrequency = 1.0 / my samplingPeriod;
		integer nfft = 2;
		if (dfMin <= 0.0) {
			nfft = 512;
			dfMin = samplingFrequency / nfft;
		}
		while (samplingFrequency / nfft > dfMin || nfft <= my maxnCoefficients)
			nfft *= 2;
		const double freqStep = samplingFrequency / nfft;

		autoSpectrogram thee = Spectrogram_create (my xmin, my xmax, my nx, my dx, my x1, 0.0, samplingFrequency / 2.0, nfft / 2 + 1, freqStep, 0.0);

		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const double t = Sampled_indexToX (me, iframe);
			autoSpectrum spec = LPC_to_Spectrum (me, t, dfMin, bandwidthReduction, deEmphasisFrequency);
			for (integer j = 1; j <= spec -> nx; j ++) {
				const double re = spec -> z [1] [j], im = spec -> z [2] [j];
				thy z [j] [iframe] = re * re + im * im;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrogram created.");
	}
}

/* End of file LPC_to_Spectrogram.cpp */
