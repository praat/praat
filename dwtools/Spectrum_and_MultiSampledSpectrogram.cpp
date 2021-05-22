/* Spectrum_and_MultiSampledSpectrogram.h
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

#include "Spectrum_and_MultiSampledSpectrogram.h"
#include "Sound_and_Spectrum.h"
#include "Spectrum_extensions.h"
#include "NUM2.h"

static void windowShape_VEC_preallocated (VEC const& target, kSound_windowShape windowShape) {
	const integer n = target.size;
	const double imid = 0.5 * (double) (n + 1);
	switch (windowShape) {
		case kSound_windowShape::RECTANGULAR: {
			target  <<=  1.0; 
		} break; case kSound_windowShape::TRIANGULAR: {  // "Bartlett"
			for (integer i = 1; i <= n; i ++) {
				const double phase = (double) (i - 0.5) / n;
				target [i] = 1.0 - fabs ((2.0 * phase - 1.0));
			} 
		} break; case kSound_windowShape::PARABOLIC: {  // "Welch"
			for (integer i = 1; i <= n; i ++) { 
				const double phase = (double) (i - 0.5) / n;
				target [i] = 1.0 - (2.0 * phase - 1.0) * (2.0 * phase - 1.0);
			}
		} break; case kSound_windowShape::HANNING: {
			for (integer i = 1; i <= n; i ++) {
				const double phase = (double) (i - 0.5) / n;
				target [i] = 0.5 * (1.0 - cos (2.0 * NUMpi * phase));
			}
		} break; case kSound_windowShape::HAMMING: {
			for (integer i = 1; i <= n; i ++) { 
				const double phase = (double) (i - 0.5) / n;
				target [i] = 0.54 - 0.46 * cos (2.0 * NUMpi * phase);
			}
		} break; case kSound_windowShape::GAUSSIAN_1: {
			const double edge = exp (-3.0), onebyedge1 = 1.0 / (1.0 - edge);   // -0.5..+0.5
			for (integer i = 1; i <= n; i ++) {
				const double phase = ((double) i - imid) / n;
				target [i] = (exp (-12.0 * phase * phase) - edge) * onebyedge1;
			}
		} break; case kSound_windowShape::GAUSSIAN_2: {
			const double edge = exp (-12.0), onebyedge1 = 1.0 / (1.0 - edge);
			for (integer i = 1; i <= n; i ++) {
				const double phase = ((double) i - imid) / n;
				target [i] = (exp (-48.0 * phase * phase) - edge) * onebyedge1;
			}
		} break; case kSound_windowShape::GAUSSIAN_3: {
			const double edge = exp (-27.0), onebyedge1 = 1.0 / (1.0 - edge);
			for (integer i = 1; i <= n; i ++) {
				const double phase = ((double) i - imid) / n;
				target [i] = (exp (-108.0 * phase * phase) - edge) * onebyedge1;
			}
		} break; case kSound_windowShape::GAUSSIAN_4: {
			const double edge = exp (-48.0), onebyedge1 = 1.0 / (1.0 - edge);
			for (integer i = 1; i <= n; i ++) { 
				const double phase = ((double) i - imid) / n;
				target [i] = (exp (-192.0 * phase * phase) - edge) * onebyedge1; 
			}
		} break; case kSound_windowShape::GAUSSIAN_5: {
			const double edge = exp (-75.0), onebyedge1 = 1.0 / (1.0 - edge);
			for (integer i = 1; i <= n; i ++) { 
				const double phase = ((double) i - imid) / n;
				target [i] = (exp (-300.0 * phase * phase) - edge) * onebyedge1;
			}
		} break; case kSound_windowShape::KAISER_1: {
			const double factor = 1.0 / NUMbessel_i0_f (2 * NUMpi);
			for (integer i = 1; i <= n; i ++) { 
				const double phase = 2.0 * ((double) i - imid) / n;   // -1..+1
				const double root = 1.0 - phase * phase;
				target [i] = root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f (2.0 * NUMpi * sqrt (root));
			}
		} break; case kSound_windowShape::KAISER_2: {
			const double factor = 1.0 / NUMbessel_i0_f (2 * NUMpi * NUMpi + 0.5);
			for (integer i = 1; i <= n; i ++) { 
				const double phase = 2.0 * ((double) i - imid) / n;   // -1..+1
				const double root = 1.0 - phase * phase;
				target [i] = root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f ((2.0 * NUMpi * NUMpi + 0.5) * sqrt (root)); 
			}
		} break; default: {
			target  <<=  1.0;
		}
	}
}

static autoVEC windowShape_VEC (integer n, kSound_windowShape windowShape) {
	autoVEC result = raw_VEC (n);
	windowShape_VEC_preallocated (result.get(), windowShape);
	return result;
}

void Spectrum_into_MultiSampledSpectrogram (Spectrum me, MultiSampledSpectrogram thee, double approximateTimeOverSampling,
	kSound_windowShape filterShape) 
{
	try {
		const integer maximumFilterSize = 2 * my nx;
		autoVEC filterWindow = raw_VEC (maximumFilterSize);
		for (integer ifreq = 1; ifreq <= thy nx; ifreq ++) {
			double flow, fhigh;
			integer iflow, ifhigh;
			MultiSampledSpectrogram_getFrequencyBand (thee, ifreq, & flow, & fhigh);
			const integer numberOfSamplesFromSpectrum = Sampled_getWindowSamples (me, flow, fhigh, & iflow, & ifhigh);
			Melder_require (numberOfSamplesFromSpectrum > 1,
				U"The number of spectral filter values should be larger than 1.");
			integer numberOfFilterValues = numberOfSamplesFromSpectrum;
			if (approximateTimeOverSampling > 1.0)
				numberOfFilterValues = Melder_iroundUp (approximateTimeOverSampling * numberOfSamplesFromSpectrum);
			integer numberOfSamplesFFT = 2;
			while (numberOfSamplesFFT < numberOfFilterValues)
				numberOfSamplesFFT *= 2;
			const double filterBandwidth = (fhigh - flow) * numberOfSamplesFFT / numberOfSamplesFromSpectrum;
			const integer numberOfFrequencies = numberOfSamplesFFT + 1;
			autoSpectrum filter = Spectrum_create (filterBandwidth, numberOfFrequencies);
			filter -> z.part (1, 2, 1, numberOfSamplesFromSpectrum)  <<=  my z.part (1, 2, iflow, ifhigh);
			filterWindow.resize (numberOfSamplesFromSpectrum);
			windowShape_VEC_preallocated (filterWindow.get(), filterShape);
			filter -> z.part (1, 2, 1, numberOfSamplesFromSpectrum)  *=  filterWindow.get();
			autoSound filtered = Spectrum_to_Sound (filter.get());
			autoFrequencyBin frequencyBin = FrequencyBin_create (thy tmin, thy tmax, filtered -> nx, filtered -> dx, filtered -> x1);
			frequencyBin -> z.row (1)  <<=  filtered -> z.row (1);
			Spectrum_shiftPhaseBy90Degrees (filter.get());
			autoSound filtered90 = Spectrum_to_Sound (filter.get());
			frequencyBin -> z.row (2)  <<=  filtered90 -> z.row (1);
			thy frequencyBins.addItem_move (frequencyBin.move());
		}
		Melder_assert (thy frequencyBins.size == thy nx); // maintain invariant
	} catch (MelderError) {
		Melder_throw (me, U": cannot calculate MultiSampledSpectrogram.");
	}
}

autoSpectrum MultiSampledSpectrogram_to_Spectrum (MultiSampledSpectrogram me) {
	try {
		const double duration = my tmax - my tmin;
		const double nyquistFrequency = my v_myFrequencyUnitToHertz (my xmax);
		const double samplingFrequency = 2.0 * nyquistFrequency;
		const integer numberOfSamples = duration * samplingFrequency;
		integer numberOfFFTSamples = 2;
		while (numberOfFFTSamples < numberOfSamples)
			numberOfFFTSamples *= 2;
		autoSpectrum thee = Spectrum_create (nyquistFrequency, numberOfFFTSamples / 2 + 1);
		for (integer ifreq = 1; ifreq <= my nx; ifreq ++) {
			const FrequencyBin frequencyBin = my frequencyBins.at [ifreq];
			autoSound sound = FrequencyBin_to_Sound (frequencyBin);
			autoSpectrum filter = Sound_to_Spectrum (sound.get(), false);
			double flow, fhigh;
			MultiSampledSpectrogram_getFrequencyBand (me, ifreq, & flow, & fhigh);
			integer iflow, ifhigh;
			(void) Sampled_getWindowSamples (thee.get(), flow, fhigh, & iflow, & ifhigh);
			thy z.part (1, 2, iflow, ifhigh)  +=  filter -> z.part (1, 2, 1, ifhigh - iflow + 1);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot convert to Spectrum.");
	}
}

autoSpectrum Sound_and_MultiSampledSpectrogram_to_Spectrum (Sound me, MultiSampledSpectrogram thee) {
	try {
		autoSpectrum him;
		const double nyquistFrequency = 0.5 / my dx;
		const double maximumFrequency = thy v_myFrequencyUnitToHertz (thy xmax);
		if (maximumFrequency < nyquistFrequency) {
			autoSound resampled = Sound_resample (me, 2.0 * maximumFrequency, 50);
			him = Sound_to_Spectrum (resampled.get(), true);
		} else {
			him = Sound_to_Spectrum (me, true);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" and", thee, U" cannot create a Spectrum.");
	}
}
	
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
		thy z.get()  /=  my frequencyResolutionInBins;
		return thee;
		
	} catch (MelderError) {
		Melder_throw (me, U": could not create Sound.");
	}
}

autoSound MultiSampledSpectrogram_to_Sound_frequencyBin (MultiSampledSpectrogram me, integer frequencyBinNumber) {
	try  {
		Melder_require (frequencyBinNumber > 0 && frequencyBinNumber <= my nx,
			U"The frequency bin number should be in the interval from 1 to ", my nx, U".");
		FrequencyBin frequencyBin = my frequencyBins.at [frequencyBinNumber];
		return FrequencyBin_to_Sound (frequencyBin);
	} catch (MelderError) {
		Melder_throw (me, U": could not create Sound from frequency bin ", frequencyBinNumber, U".");
	}
}


autoAnalyticSound MultiSampledSpectrogram_to_AnalyticSound_frequencyBin (MultiSampledSpectrogram me, integer frequencyBinNumber) {
	try  {
		Melder_require (frequencyBinNumber > 0 && frequencyBinNumber <= my nx,
			U"The frequency bin number should be in the interval from 1 to ", my nx, U".");
		FrequencyBin frequencyBin = my frequencyBins.at [frequencyBinNumber];
		return FrequencyBin_to_AnalyticSound (frequencyBin);
	} catch (MelderError) {
		Melder_throw (me, U": could not create Sound from frequency bin ", frequencyBinNumber, U".");
	}
}

/* End of file  Spectrum_and_MultiSampledSpectrogram.cpp */
