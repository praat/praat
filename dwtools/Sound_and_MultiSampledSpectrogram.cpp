/* Sound_and_MultiSampledSpectrogram.cpp
 * 
 * Copyright (C) 2021-2022 David Weenink
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

#include "Sound_and_MultiSampledSpectrogram.h"
#include "Sound_and_Spectrum.h"
#include "Spectrum_extensions.h"
#include "Spectrum_and_MultiSampledSpectrogram.h"
#include "NUM2.h"

	
autoSound MultiSampledSpectrogram_to_Sound (MultiSampledSpectrogram me) {
	try {
		autoSpectrum spectrum = MultiSampledSpectrogram_to_Spectrum (me);
		autoSound thee = Spectrum_to_Sound (spectrum.get());
		const double synthesizedDuration = thy xmax - thy xmin;
		const double wantedDuration = my tmax - my tmin;
		if (synthesizedDuration > wantedDuration) {
			/*
				The number of samples in the original was not a power of 2 and to apply the FFT the number of samples had to be extended.
			*/
			autoSound part = Sound_extractPart (thee.get(), 0.0, wantedDuration, kSound_windowShape::RECTANGULAR, 1.0, false);
			part -> xmin = my tmin;
			part -> xmax = my tmax;
			thee = part.move();
		} else if (synthesizedDuration == wantedDuration) {
			thy xmin = my tmin;
			thy xmax = my tmax;
		} else {
			Melder_throw (U"The synthesized number of samples is too low!");
		}
		return thee;
		
	} catch (MelderError) {
		Melder_throw (me, U": could not create Sound.");
	}
}

autoConstantQLog2FSpectrogram Sound_to_ConstantQLog2FSpectrogram (Sound me, double lowestFrequency, double fmax,
	integer numberOfBinsPerOctave, double frequencyResolutionInBins, double timeOversamplingFactor,
	kSound_windowShape filterShape) {
	try {
		const double nyquistFrequency = 0.5 / my dx;
		if (fmax <= 0.0 || fmax > nyquistFrequency)
			fmax = nyquistFrequency;
		Melder_require (lowestFrequency < fmax,
			U"The lowest frequency should be smaller than the maximum frequency (", fmax, U" Hz).");
		Melder_require (frequencyResolutionInBins > 0.0,
			U"The frequency resolution should be larger than zero.");
		Melder_clipLeft (1.0, & timeOversamplingFactor);
		autoConstantQLog2FSpectrogram thee = ConstantQLog2FSpectrogram_create (my xmin, my xmax,
			lowestFrequency, fmax, numberOfBinsPerOctave, frequencyResolutionInBins);
		autoSpectrum him = Sound_and_MultiSampledSpectrogram_to_Spectrum (me, thee.get());
		Spectrum_into_MultiSampledSpectrogram (him.get(), thee.get(), timeOversamplingFactor, filterShape);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create ConstantQLog2FSpectrogram.");
	}
}

autoGaborSpectrogram Sound_to_GaborSpectrogram (Sound me, double fmax, double filterBandwidth,
	double frequencyStep, double timeOversamplingFactor, kSound_windowShape filterShape)
{
	try {
		const double nyquistFrequency = 0.5 / my dx;
		bool resample = true;
		if (fmax <= 0.0 || fmax >= nyquistFrequency) {
			fmax = nyquistFrequency;
			resample = false;
		}
		autoGaborSpectrogram thee = GaborSpectrogram_create (my xmin, my xmax, fmax, filterBandwidth, frequencyStep);
		autoSound resampled;
		if (resample)
			resampled = Sound_resample (me, 2.0 * fmax, 50);
		autoSpectrum him = Sound_to_Spectrum ((resample ? resampled.get() : me), true);
		Spectrum_into_MultiSampledSpectrogram (him.get(), thee.get(), timeOversamplingFactor, filterShape);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create GaborSpectrogram.");
	}
}
/* End of file Sound_and_MultiSampledSpectrogram.cpp */
