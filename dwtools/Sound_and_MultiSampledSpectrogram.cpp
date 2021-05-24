/* Sound_and_MultiSampledSpectrogram.cpp
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

#include "Sound_and_MultiSampledSpectrogram.h"
#include "Sound_and_Spectrum.h"
#include "Spectrum_extensions.h"
#include "Spectrum_and_MultiSampledSpectrogram.h"
#include "NUM2.h"

autoConstantQLog2FSpectrogram Sound_to_ConstantQLog2FSpectrogram (Sound me, double lowestFrequency, double fmax,
	integer numberOfBinsPerOctave, double frequencyResolutionInBins, double timeOversamplingFactor,
	kSound_windowShape filterShape) {
	try {
		const double nyquistFrequency = 0.5 / my dx;
		if (fmax <= 0.0)
			fmax = nyquistFrequency;
		Melder_require (fmax  <= nyquistFrequency,
			U"The maximum frequency should not exceed the nyquist frequency (", nyquistFrequency, U" Hz).");
		Melder_require (lowestFrequency < fmax,
			U"The lowest frequency should be smaller than the maximum frequency (", fmax, U" Hz).");
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
		if (fmax <= 0.0)
			fmax = nyquistFrequency;
		Melder_require (fmax  <= nyquistFrequency,
			U"The maximum frequency should not exceed the nyquist frequency (", nyquistFrequency, U" Hz).");
		autoGaborSpectrogram thee = GaborSpectrogram_create (my xmin, my xmax, fmax, filterBandwidth, frequencyStep);
		autoSpectrum him = Sound_and_MultiSampledSpectrogram_to_Spectrum (me, thee.get());
		Spectrum_into_MultiSampledSpectrogram (him.get(), thee.get(), timeOversamplingFactor, filterShape);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create GaborSpectrogram.");
	}
}
/* End of file Sound_and_MultiSampledSpectrogram.cpp */
