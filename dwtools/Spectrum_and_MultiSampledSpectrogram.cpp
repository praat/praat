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

void Spectrum_into_MultiSampledSpectrogram (Spectrum me, MultiSampledSpectrogram thee, double approximateTimeOverSampling,
	kSound_windowShape filterShape) 
{
	try {
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
			autoFrequencyBin bin = FrequencyBin_create (thy tmin, thy tmax, his nx, his dx, his x1);
			bin -> z = his z.move();
			thy frequencyBins.addItem_move (bin.move());
			if (ifreq == 1) {
				/*
					DC_BIN:
					Fill with values from 0 Hz to the mid of the first window
					Multiply with a window only the part that overlaps with the first window.
				*/
				(void) Sampled_getWindowSamples (me, my xmin, 0.5 * (spectrum_fmin + spectrum_fmax), & spectrum_imin, & spectrum_imax);
				him = Spectrum_to_AnalyticSound_demodulateBand (me, spectrum_imin, spectrum_imax, approximateTimeOverSampling, 
					window.part (window.size / 2 + 1, window.size));
				autoFrequencyBin zeroBin = FrequencyBin_create (thy tmin, thy tmax, his nx, his dx, his x1);
				zeroBin -> z = his z.move();
				thy zeroBin = zeroBin.move();
			} 
			if (ifreq == thy nx) {
				/*
					NYQUIST_BIN:
					Fill with the part from the mid of the last window to the end
					Window only the part that overlaps the last window.
				*/
				(void) Sampled_getWindowSamples (me, 0.5 * (spectrum_fmin + spectrum_fmax), my xmax, & spectrum_imin, & spectrum_imax);
				him = Spectrum_to_AnalyticSound_demodulateBand (me, spectrum_imin, spectrum_imax, approximateTimeOverSampling, 
					window.part (1 , window.size / 2));
				autoFrequencyBin nyquistBin = FrequencyBin_create (thy tmin, thy tmax, his nx, his dx, his x1);
				nyquistBin -> z = his z.move();
				thy nyquistBin = nyquistBin.move();
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
		const integer numberOfSamples = Melder_iround (duration * samplingFrequency);
		const integer numberOfFFTSamples = Melder_clippedLeft (2_integer, Melder_iroundUpToPowerOfTwo (numberOfSamples));
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
