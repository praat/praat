/* Sound_and_Spectrum_dft.cpp
 *
 * Copyright (C) 2021 David Weenink, Paul Boersma
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

autoSpectrum Sound_to_Spectrum_resampled (Sound me, integer interpolationDepth) {
	const double xmin_saved = my xmin, xmax_saved = my xmax;
	try {
		const integer fftNumberOfSamples = Melder_iroundUpToPowerOfTwo (my nx);
		if (fftNumberOfSamples == my nx)
			return Sound_to_Spectrum (me, true);   // FFT without resampling
		const double samplingFrequency = 1.0 / my dx;
		const double df = samplingFrequency / my nx;
		const double upSamplingFrequency = fftNumberOfSamples * df;
		/*
			Temporary domain correction:
			
			For a sound with 44100 Hz sampling frequency, xmin = 0.0, xmax = 0.02531645569620253 and nx = 1116,
			we would calculate fftNumberOfSamples as 2048.
			Sound_resample would calculate numberOfValues as Melder_iround ((my xmax - my xmin) * newSamplingFrequency 
			which gives 2049. We have to make sure that these two different calculation result in the same number.
			We can do this by adapting the sound's domain temporarily.
		*/
		my xmin = 0.0;
		my xmax = my nx * my dx;
		autoSound resampled = Sound_resample (me, upSamplingFrequency, interpolationDepth);
		my xmin = xmin_saved;
		my xmax = xmax_saved;
		autoSpectrum extendedSpectrum = Sound_to_Spectrum (resampled.get(), true);   // FFT after resampling
		const integer numberOfFrequencies = my nx / 2 + 1;
		autoSpectrum thee = Spectrum_create (0.5 * samplingFrequency, numberOfFrequencies);
		thy dx = df;   // override, just in case my nx is odd
		thy z.get()  <<=  extendedSpectrum -> z.part (1, 2, 1, numberOfFrequencies);
		if (my nx % 2 == 0)
			thy z [2] [numberOfFrequencies] = 0.0;   // set imaginary value at Nyquist to zero
		return thee;
	} catch (MelderError) {
		my xmin = xmin_saved;
		my xmax = xmax_saved;
		Melder_throw (me, U": could not convert to Spectrum by resampling.");
	}
}

autoSound Spectrum_to_Sound_resampled (Spectrum me, integer interpolationDepth) {
	try {
		const integer fftNumberOfSamples = Melder_iroundUpToPowerOfTwo (my nx - 1);
		autoSound thee;
		if (fftNumberOfSamples == my nx - 1)
			return Spectrum_to_Sound (me);   // FFT without resampling
		const integer newNumberOfFrequencies = fftNumberOfSamples + 1;
		autoSpectrum extendedSpectrum = Spectrum_create (my xmax, newNumberOfFrequencies);
		extendedSpectrum -> z.part (1, 2, 1, my nx)  <<=  my z.get();
		extendedSpectrum -> dx = my dx;
		autoSound upsampled = Spectrum_to_Sound (extendedSpectrum.get());
		return Sound_resample (upsampled.get(), Melder_iround (2.0 * my xmax), interpolationDepth);
	} catch (MelderError) {
		Melder_throw (me, U": could not convert to Sound by resampling.");
	}
}

/* End of file Sound_and_Spectrum_dft.cpp */
