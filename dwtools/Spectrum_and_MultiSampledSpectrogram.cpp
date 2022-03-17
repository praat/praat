/* Spectrum_and_MultiSampledSpectrogram.cpp
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

#include "Spectrum_and_AnalyticSound.h"
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
				target [i] = 0.5 * (1.0 - cos (NUM2pi * phase));
			}
		} break; case kSound_windowShape::HAMMING: {
			for (integer i = 1; i <= n; i ++) { 
				const double phase = (double) (i - 0.5) / n;
				target [i] = 0.54 - 0.46 * cos (NUM2pi * phase);
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
			const double factor = 1.0 / NUMbessel_i0_f (NUM2pi);
			for (integer i = 1; i <= n; i ++) { 
				const double phase = 2.0 * ((double) i - imid) / n;   // -1..+1
				const double root = 1.0 - phase * phase;
				target [i] = ( root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f (NUM2pi * sqrt (root)) );
			}
		} break; case kSound_windowShape::KAISER_2: {
			const double factor = 1.0 / NUMbessel_i0_f (NUM2pi * NUMpi + 0.5);
			for (integer i = 1; i <= n; i ++) { 
				const double phase = 2.0 * ((double) i - imid) / n;   // -1..+1
				const double root = 1.0 - phase * phase;
				target [i] = ( root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f ((NUM2pi * NUMpi + 0.5) * sqrt (root)) ); 
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

static autoFrequencyBin Spectrum_to_FrequencyBin (Spectrum me, double tmin, double tmax) {
	try {
		autoAnalyticSound him = Spectrum_to_AnalyticSound (me);
		autoFrequencyBin thee = FrequencyBin_create (tmin, tmax, his nx, his dx, his x1);
		thy z.get()   <<=  his z.get();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not convert to FrequencyBin.");
	}
}

void Spectrum_into_MultiSampledSpectrogram (Spectrum me, MultiSampledSpectrogram thee, double approximateTimeOverSampling,
	kSound_windowShape filterShape) 
{
	try {
		enum frequencyBinPosition { DC_BIN, NORMAL_BIN, NYQUIST_BIN };
		const integer maximumFilterSize = my nx;
		autoVEC window = raw_VEC (maximumFilterSize);
		for (integer ifreq = 1; ifreq <= thy nx; ifreq ++) {
			double spectrum_fmin, spectrum_fmax;
			MultiSampledSpectrogram_getFrequencyBand (thee, ifreq, & spectrum_fmin, & spectrum_fmax);
			integer spectrum_imin, spectrum_imax;
			const integer numberOfSpectralValues = Sampled_getWindowSamples (me, spectrum_fmin, spectrum_fmax, & spectrum_imin, & spectrum_imax);
			Melder_require (numberOfSpectralValues > 0,
				U"The number of spectral values for bin number ", ifreq, U" should be larger than zero.");
			window.resize (numberOfSpectralValues);
			windowShape_VEC_preallocated (window.get(), filterShape);
			autoAnalyticSound him = Spectrum_to_AnalyticSound_demodulateBand (me, spectrum_imin, spectrum_imax, approximateTimeOverSampling, window.get());		
			autoFrequencyBin bin = FrequencyBin_create (thy xmin, thy xmax, his nx, his dx, his x1);
			bin -> z.get() <<=  his z.get();
			thy frequencyBins.addItem_move (bin.move());
			if (ifreq == 1) {
				/*
					DC_BIN:
					Fill with values from 0 Hz to the mid of the first window
					Multiply with a window only the part that overlaps with the first window.
				*/
				spectrum_fmax = 0.5 * (spectrum_fmin + spectrum_fmax);
				spectrum_imax = Sampled_xToIndex (me, spectrum_fmax);
				him = Spectrum_to_AnalyticSound_demodulateBand (me, 1, spectrum_imax, approximateTimeOverSampling, 
					window.part (window.size - window.size / 2 + 1 /*?*/, window.size));
				autoFrequencyBin bin = FrequencyBin_create (thy xmin, thy xmax, his nx, his dx, his x1);
				thy zeroBin = bin.move();
			} 
			if (ifreq == thy nx) {
				/*
					NYQUIST_BIN:
					Fill with the part from the mid of the last window to the end
					Window only the part that overlaps the last window.
				*/
				spectrum_fmin = 0.5 * (spectrum_fmin + spectrum_fmax);
				spectrum_imin = Sampled_xToIndex (me, spectrum_fmin);
				him = Spectrum_to_AnalyticSound_demodulateBand (me, spectrum_imin, my nx, approximateTimeOverSampling, 
					window.part (1 , window.size / 2));
				autoFrequencyBin bin = FrequencyBin_create (thy xmin, thy xmax, his nx, his dx, his x1);
				thy zeroBin = bin.move();
			}
		}
		Melder_assert (thy frequencyBins.size == thy nx); // maintain invariant
	} catch (MelderError) {
		Melder_throw (me, U": cannot calculate MultiSampledSpectrogram.");
	}
}

void Spectrum_into_MultiSampledSpectrogram_old (Spectrum me, MultiSampledSpectrogram thee, double approximateTimeOverSampling,
	kSound_windowShape filterShape) 
{
	try {
		enum frequencyBinPosition { DC_BIN, NORMAL_BIN, NYQUIST_BIN };
		const integer maximumFilterSize = 2 * my nx;
		autoVEC filterWindow = raw_VEC (maximumFilterSize);
		for (integer ifreq = 1; ifreq <= thy nx; ifreq ++) {
			/*
				spectrum_fmin, spectrum_fmax: frequency interval of the spectrum to copy to the filter
				spectrum_imin, spectrum_imax: corresponding indices of the spectrum
				filter_fmin, filter_fmax: frequency interval to be filtered
				filter_imin, filter_imax: indices in the filter to be windowed
				window_fmin, window_fmax
				window_imin, window_imax: indices of the window to be used in the filtering
					invariant: filter_imax - filter_imin == window_imax - window_imin
			*/
			integer window_imax_previous = 0;
			double spectrum_fmin, spectrum_fmax;		
			MultiSampledSpectrogram_getFrequencyBand (thee, ifreq, & spectrum_fmin, & spectrum_fmax);
			double window_fmin = spectrum_fmin, window_fmax = spectrum_fmax;
			
			auto toFrequencyBin = [&] (enum frequencyBinPosition partOfSpectrum) -> autoFrequencyBin {
				const integer iextra = (partOfSpectrum != DC_BIN ? 1 : 0 );
				integer window_imin = 0, window_imax = 0;
				integer spectrum_imin, spectrum_imax;
				const integer numberOfSamplesFromSpectrum = Sampled_getWindowSamples (me, spectrum_fmin, spectrum_fmax, 
					& spectrum_imin, & spectrum_imax);			
				Melder_require (numberOfSamplesFromSpectrum > 1,
					U"The number of spectral filter values should be larger than 1.");
				integer numberOfFilterValues = numberOfSamplesFromSpectrum;
				if (partOfSpectrum == NORMAL_BIN) {
					filterWindow.resize (numberOfSamplesFromSpectrum);
					windowShape_VEC_preallocated (filterWindow.get(), filterShape);
					if (approximateTimeOverSampling > 1.0)
						numberOfFilterValues = Melder_iroundUp (approximateTimeOverSampling * numberOfSamplesFromSpectrum);
				}
				numberOfFilterValues += iextra;
				const integer numberOfSamplesFFT = Melder_clippedLeft (2_integer, Melder_iroundUpToPowerOfTwo (numberOfFilterValues));   // TODO: explain the edge case
				const integer numberOfFrequencies = numberOfSamplesFFT + 1;
				const double actual_fmax = thy v_myFrequencyUnitToHertz (Sampled_indexToX (me, spectrum_imax));
				const double filterDomain = (actual_fmax - spectrum_fmin) * numberOfSamplesFFT / numberOfSamplesFromSpectrum;
				autoSpectrum filter = Spectrum_create (filterDomain, numberOfFrequencies);
				filter -> z [1] [1] = filter -> z [2] [1] = 0.0;
				filter -> z.part (1, 2, 1 + iextra, numberOfSamplesFromSpectrum + iextra)  <<=  my z.part (1, 2, spectrum_imin, spectrum_imax);
				
				const integer numberToBeWindowed = Sampled_getWindowSamples (me, window_fmin, window_fmax,
					& window_imin, & window_imax);
				const integer filter_imin = window_imin - spectrum_imin + 1 + iextra;
				const integer filter_imax = window_imax - spectrum_imin + 1 + iextra;
				window_imax = ( partOfSpectrum == NORMAL_BIN ? filterWindow.size : 
					( partOfSpectrum == DC_BIN ? window_imax_previous : numberToBeWindowed ) );
				window_imin = window_imax - numberToBeWindowed + 1;
				window_imax_previous = window_imax;
				filter -> z.part (1, 2, filter_imin, filter_imax)  *=  
					filterWindow.part (window_imin, window_imax);
				autoFrequencyBin result = Spectrum_to_FrequencyBin (filter.get(), thy tmin, thy tmax);
				return result;
			};
			
			thy frequencyBins.addItem_move (toFrequencyBin (NORMAL_BIN).move());
			if (ifreq == 1) {
				/*
					DC_BIN:
					Fill with values from 0 Hz to the mid of the first window
					Multiply with a window only the part that overlaps with the first window.
				*/
				spectrum_fmax = 0.5 * (spectrum_fmin + spectrum_fmax);
				spectrum_fmin = 0.0;
				window_fmax = spectrum_fmax;
				thy zeroBin = toFrequencyBin (DC_BIN).move();
			} 
			if (ifreq == thy nx) {
				/*
					NYQUIST_BIN:
					Fill with the part from the mid of the last window to the end
					Window only the part that overlaps the last window.
				*/
				spectrum_fmin = 0.5 * (spectrum_fmin + spectrum_fmax);
				window_fmin = spectrum_fmin;
				spectrum_fmax = thy v_myFrequencyUnitToHertz (thy xmax);
				thy nyquistBin = toFrequencyBin (NYQUIST_BIN).move();
			}
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
		const integer numberOfFFTSamples = Melder_clippedLeft (2_integer, Melder_iroundUpToPowerOfTwo (numberOfSamples));   // TODO: explain the edge case
		const integer numberOfSpectralValues = numberOfFFTSamples / 2 + 1;
		autoSpectrum thee = Spectrum_create (nyquistFrequency, numberOfSpectralValues);
		for (integer ifreq = 1; ifreq <= my nx; ifreq ++) {
			integer iextra = 1;
			const FrequencyBin frequencyBin = my frequencyBins.at [ifreq];			
			double flow, fhigh;
			MultiSampledSpectrogram_getFrequencyBand (me, ifreq, & flow, & fhigh);
			auto fillSpectrumPart = [&] (FrequencyBin fbin) {
				autoSound sound = FrequencyBin_to_Sound (fbin);
				autoSpectrum filter = Sound_to_Spectrum (sound.get(), false);
				integer iflow, ifhigh;
				(void) Sampled_getWindowSamples (thee.get(), flow, fhigh, & iflow, & ifhigh);
				thy z.part (1, 2, iflow, ifhigh)  +=  filter -> z.part (1, 2, 1 + iextra, ifhigh - iflow + 1 + iextra);
			};
			fillSpectrumPart (frequencyBin);
			if (ifreq == 1) {
				fhigh = 0.5 * (flow + fhigh);
				flow = 0.0;
				iextra = 0;
				fillSpectrumPart (my zeroBin.get());
			}
			if (ifreq == my nx) {
				flow = 0.5 * (flow + fhigh);
				fhigh = my v_myFrequencyUnitToHertz (my xmax);
				fillSpectrumPart (my nyquistBin.get());
			}
		}
		/*
			Make sure the imaginary part of the last spectral value is zero.
		*/
		thy z [2] [numberOfSpectralValues] = 0.0;
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
