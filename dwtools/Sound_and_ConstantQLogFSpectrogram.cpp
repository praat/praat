/*
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

#include "Sound_and_ConstantQLogFSpectrogram.h"
#include "Sound_and_Spectrum.h"
#include "Spectrum_extensions.h"
#include "Spectrum_and_MultiSampledSpectrogram.h"
#include "NUM2.h"

autoConstantQLogFSpectrogram Sound_to_ConstantQLogFSpectrogram (Sound me, double lowestFrequency, double fmax,integer numberOfBinsPerOctave, double frequencyResolutionInBins, double timeOversamplingFactor) {
	try {
		const double nyquistFrequency = 0.5 / my dx;
		if (fmax <= 0.0)
			fmax = nyquistFrequency;
		Melder_require (fmax  <= nyquistFrequency,
			U"The maximum frequency should not exceed the nyquist frequency (", nyquistFrequency, U" Hz).");
		Melder_require (lowestFrequency < fmax,
			U"The lowest frequency should be smaller than the maximum frequency (", fmax, U" Hz).");
		Melder_clipLeft (1.0, & timeOversamplingFactor);
		autoConstantQLogFSpectrogram thee = ConstantQLogFSpectrogram_create (my xmin, my xmax,
			lowestFrequency, fmax, numberOfBinsPerOctave, frequencyResolutionInBins);
		autoSpectrum him = Sound_and_MultiSampledSpectrogram_to_Spectrum (me, thee.get());
		Spectrum_into_MultiSampledSpectrogram (him.get(), thee.get(), timeOversamplingFactor);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create ConstantQLogFSpectrogram.");
	}
}

static autoConstantQLogFSpectrogram Sound_to_ConstantQLogFSpectrogram_brown (Sound me, double lowestFrequency, double q, integer numberOfBinsPerOctave, integer numberOfSteps, double timeOversamplingFactor) {
	try {
		
		autoConstantQLogFSpectrogram thee = ConstantQLogFSpectrogram_create (my xmin, my xmax, lowestFrequency, numberOfBinsPerOctave, numberOfSteps, q);
		const double samplingFrequency = 1.0 / my dx, nyquistFrequency = 0.5 * samplingFrequency;
		Melder_require (thy v_myFrequencyUnitToHertz (thy xmax) <= nyquistFrequency,
			U"The number of steps you want result in a maximum frequency which is above the Nyquist frequency of the sound. "
			"The maximum number of steps should not exceed ", Melder_iroundDown (numberOfBinsPerOctave * log2 (nyquistFrequency / lowestFrequency)), U".");
		if (timeOversamplingFactor < 1.0)
			timeOversamplingFactor = 4.0; // default oversampling
		const integer maximumNumberOfAnalysisSamples = samplingFrequency * q / lowestFrequency + 1;
		autoVEC window = raw_VEC (maximumNumberOfAnalysisSamples);
		autoMAT windowedExp = raw_MAT (2, maximumNumberOfAnalysisSamples);
		VEC windowedCos = windowedExp.row(1), windowedSin = windowedExp.row(2);
		MelderInfo_open ();
		for (integer ifreq = 1; ifreq <= numberOfSteps; ifreq ++) {
			const double frequency =  exp2 (Sampled_indexToX (thee.get(), ifreq));
			const integer halfNumberOfWindowSamples = Melder_iroundDown (0.5 * samplingFrequency * q / frequency);
			const integer numberOfWindowSamples = 2 * halfNumberOfWindowSamples + 1;
			double windowLength = numberOfWindowSamples * my dx;
			Melder_clipRight (& windowLength, my nx * my dx);
			const double dt = windowLength / timeOversamplingFactor;
			integer numberOfFrames;
			double t1;
			Sampled_shortTermAnalysis (me, windowLength, dt, & numberOfFrames, & t1);
			autoFrequencyBin frequencyBin = FrequencyBin_create (my xmin, my xmax, numberOfFrames, dt, t1);
			const integer windowCentreSampleNumber = halfNumberOfWindowSamples + 1;
			for (integer i = 1; i <= numberOfWindowSamples; i ++) {
				const double x = (i - windowCentreSampleNumber) * my dx / (0.5 * windowLength);
				const double root = sqrt (Melder_clippedLeft (0.0, 1.0 - sqr (x)));   // clipping should be rare
				window [i] = NUMbessel_i0_f ((2.0 * NUMpi * NUMpi + 0.5) * root);
			}
			window.resize (numberOfWindowSamples);
			for (integer i = 1; i <= numberOfWindowSamples; i ++) {
				windowedCos [i] =   window [i] * cos (NUM2pi * q * (i - 1) / numberOfWindowSamples);
				windowedSin [i] = - window [i] * sin (NUM2pi * q * (i - 1) / numberOfWindowSamples);
			}
			for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
				const double midTime = Sampled_indexToX (frequencyBin.get(), iframe);
				const integer soundCentreSampleNumber = Sampled_xToNearestIndex (me, midTime);   // time accuracy is half a sampling period

				integer leftSample = soundCentreSampleNumber - halfNumberOfWindowSamples;
				integer rightSample = soundCentreSampleNumber + halfNumberOfWindowSamples;
				/*
					Catch some edge cases, which are uncommon because Sampled_shortTermAnalysis() filtered out most problems.
				*/
				Melder_clipLeft (1_integer, & leftSample);
				Melder_clipRight (& rightSample, my nx);
				Melder_require (rightSample >= leftSample,
					U"Unexpected edge case: right sample (", rightSample, U") less than left sample (", leftSample, U").");
				const integer actualNumberOfSamples = rightSample - leftSample + 1;
				const integer windowFromSoundOffset = windowCentreSampleNumber - soundCentreSampleNumber;
				const double windowSq = NUMsum2 (window.part (windowFromSoundOffset + leftSample, windowFromSoundOffset + rightSample));
				const double scaleFactor = 1.0 / (actualNumberOfSamples * sqrt (windowSq)) ;
				VEC amplitudePart = my z.row(1).part (leftSample, rightSample);
				frequencyBin -> z [1] [iframe] = NUMinner (amplitudePart, windowedCos.part (windowFromSoundOffset + leftSample,
					windowFromSoundOffset + rightSample)) * scaleFactor;
				frequencyBin -> z [2] [iframe] = NUMinner (amplitudePart, windowedSin.part (windowFromSoundOffset + leftSample, 
					windowFromSoundOffset + rightSample)) * scaleFactor;
			}
			thy frequencyBins.addItem_move (frequencyBin.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create ConstantQLogFSpectrogram.");
	}
}

/* End of file Sound_and_ConstantQLogFSpectrogram.cpp */
