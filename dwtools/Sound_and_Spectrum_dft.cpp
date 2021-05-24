/* Sound_and_Spectrum_dft.cpp
 *
 * Copyright (C) 2021 David Weenink
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

#include "Sound_and_Spectrum_dft.h"
#include "Sound_and_Spectrum.h"

autoSpectrum Sound_to_Spectrum_dft (Sound me, integer interpolationDepth) {
	try {
		const integer powerOf2 = Melder_iroundUp (log2 (my nx));
		autoSpectrum thee;
		if (powerOf2 != my nx) {
			const double samplingFrequency = 1.0 / my dx;
			const double df = samplingFrequency / my nx;
			const double newSamplingFrequency = exp2 (powerOf2) * df;
			autoSound resampled = Sound_resample (me, newSamplingFrequency, interpolationDepth);
			autoSpectrum extendedSpectrum = Sound_to_Spectrum (resampled.get(), true);
			const integer numberOfFrequencies = my nx / 2 + 1;
			thee = Spectrum_create (0.5 * samplingFrequency, numberOfFrequencies);
			thy z.get()  <<=  extendedSpectrum -> z.part (1, 2, 1, numberOfFrequencies);
			thy z [2] [numberOfFrequencies] = 0.0; // set imaginary value at Nyquist to zero
		} else
			thee = Sound_to_Spectrum (me, true);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not convert to Spectrum by DFT.");
	}
}

autoSound Spectrum_to_Sound_dft (Spectrum me, integer interpolationDepth) {
	try {
		const integer powerOf2 = Melder_iroundUp (log2 (my nx - 1));
		autoSound thee;
		if (powerOf2 + 1 != my nx) {
			const integer newNumberOfFrequencies = exp2 (powerOf2) + 1;
			const double newMaximumFrequency = my dx * newNumberOfFrequencies;
			autoSpectrum extendedSpectrum = Spectrum_create (newMaximumFrequency, newNumberOfFrequencies);
			extendedSpectrum -> z.part (1, 2, 1, my nx)  <<=  my z.get();
			autoSound upsampled = Spectrum_to_Sound (extendedSpectrum.get());
			thee = Sound_resample (upsampled.get(), round (2.0 * my xmax), interpolationDepth);
		} else
			thee = Spectrum_to_Sound (me);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not convert to Sound by DFT.");
	}
}


/* End of file Sound_and_Spectrum_dft.cpp */
