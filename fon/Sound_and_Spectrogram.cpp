/* Sound_and_Spectrogram.cpp
 *
 * Copyright (C) 1992-2011,2014-2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Sound_and_Spectrogram.h"
#include "NUM2.h"

#include "enums_getText.h"
#include "Sound_and_Spectrogram_enums.h"
#include "enums_getValue.h"
#include "Sound_and_Spectrogram_enums.h"

autoSpectrogram Sound_to_Spectrogram (Sound me, double effectiveAnalysisWidth, double fmax,
	double minimumTimeStep1, double minimumFreqStep1, kSound_to_Spectrogram_windowShape windowType,
	double maximumTimeOversampling, double maximumFreqOversampling)
{
	try {
		const double nyquist = 0.5 / my dx;
		const double physicalAnalysisWidth =
			( windowType == kSound_to_Spectrogram_windowShape::GAUSSIAN ? 2.0 * effectiveAnalysisWidth : effectiveAnalysisWidth );
		const double effectiveTimeWidth = effectiveAnalysisWidth / sqrt (NUMpi);
		const double effectiveFreqWidth = 1.0 / effectiveTimeWidth;
		const double minimumTimeStep2 = effectiveTimeWidth / maximumTimeOversampling;
		const double minimumFreqStep2 = effectiveFreqWidth / maximumFreqOversampling;
		const double timeStep = std::max (minimumTimeStep1, minimumTimeStep2);
		double freqStep = std::max (minimumFreqStep1, minimumFreqStep2);
		const double physicalDuration = my dx * my nx;

		/*
			Compute the time sampling.
		*/
		const integer approximateNumberOfSamplesPerWindow = Melder_ifloor (physicalAnalysisWidth / my dx);
		const integer halfnsamp_window = approximateNumberOfSamplesPerWindow / 2 - 1;
		const integer nsamp_window = halfnsamp_window * 2;
		if (nsamp_window < 1)
			Melder_throw (U"Your analysis window is too short: less than two samples.");
		if (physicalAnalysisWidth > physicalDuration)
			Melder_throw (U"Your sound is too short:\n"
				U"it should be at least as long as ",
				windowType == kSound_to_Spectrogram_windowShape::GAUSSIAN ? U"two window lengths." : U"one window length.");
		const integer numberOfTimes = 1 + Melder_ifloor ((physicalDuration - physicalAnalysisWidth) / timeStep);   // >= 1
		const double t1 = my x1 + 0.5 * ((my nx - 1) * my dx - (numberOfTimes - 1) * timeStep);   // centre of first frame

		/*
			Compute the frequency sampling of the FFT spectrum.
		*/
		if (fmax <= 0.0 || fmax > nyquist)
			fmax = nyquist;
		integer numberOfFreqs = Melder_ifloor (fmax / freqStep);
		if (numberOfFreqs < 1)
			return autoSpectrogram ();
		integer nsampFFT = 1;
		while (nsampFFT < nsamp_window || nsampFFT < 2 * numberOfFreqs * (nyquist / fmax))
			nsampFFT *= 2;
		const integer half_nsampFFT = nsampFFT / 2;

		/*
			Compute the frequency sampling of the spectrogram.
		*/
		const integer binWidth_samples = std::max (1_integer, Melder_ifloor (freqStep * my dx * nsampFFT));
		double binWidth_hertz = 1.0 / (my dx * nsampFFT);
		freqStep = binWidth_samples * binWidth_hertz;
		numberOfFreqs = Melder_ifloor (fmax / freqStep);
		if (numberOfFreqs < 1)
			return autoSpectrogram ();

		autoSpectrogram thee = Spectrogram_create (my xmin, my xmax, numberOfTimes, timeStep, t1,
				0.0, fmax, numberOfFreqs, freqStep, 0.5 * (freqStep - binWidth_hertz));

		autoVEC window = zero_VEC (nsamp_window);
		longdouble windowssq = 0.0;
		for (integer i = 1; i <= nsamp_window; i ++) {
			const double nSamplesPerWindow_f = physicalAnalysisWidth / my dx;
			switch (windowType) {
				case kSound_to_Spectrogram_windowShape::SQUARE: {
					window [i] = 1.0;
				} break;
				case kSound_to_Spectrogram_windowShape::HAMMING: {
					const double phase = (double) i / nSamplesPerWindow_f;   // 0 .. 1
					window [i] = 0.54 - 0.46 * cos (2.0 * NUMpi * phase);
				} break;
				case kSound_to_Spectrogram_windowShape::BARTLETT: {
					const double phase = (double) i / nSamplesPerWindow_f;   // 0 .. 1
					window [i] = 1.0 - fabs ((2.0 * phase - 1.0));
				} break;
				case kSound_to_Spectrogram_windowShape::WELCH: {
					const double phase = (double) i / nSamplesPerWindow_f;   // 0 .. 1
					window [i] = 1.0 - (2.0 * phase - 1.0) * (2.0 * phase - 1.0);
				} break;
				case kSound_to_Spectrogram_windowShape::HANNING: {
					const double phase = (double) i / nSamplesPerWindow_f;   // 0 .. 1
					window [i] = 0.5 * (1.0 - cos (2.0 * NUMpi * phase));
				} break;
				case kSound_to_Spectrogram_windowShape::GAUSSIAN: {
					const double imid = 0.5 * (double) (nsamp_window + 1), edge = exp (-12.0);
					const double phase = ((double) i - imid) / nSamplesPerWindow_f;   // -0.5 .. +0.5
					window [i] = (exp (-48.0 * phase * phase) - edge) / (1.0 - edge);
					break;
				}
				break; default:
					window [i] = 1.0;
			}
			windowssq += window [i] * window [i];
		}
		const double oneByBinWidth = 1.0 / double (windowssq) / binWidth_samples;

		autoVEC data = zero_VEC (nsampFFT);
		autoVEC spectrum = zero_VEC (half_nsampFFT + 1);
		autoNUMfft_Table fftTable;
		NUMfft_Table_init (& fftTable, nsampFFT);

		autoMelderProgress progress (U"Sound to Spectrogram...");

		for (integer iframe = 1; iframe <= numberOfTimes; iframe ++) {
			const double t = Sampled_indexToX (thee.get(), iframe);
			const integer leftSample = Sampled_xToLowIndex (me, t), rightSample = leftSample + 1;
			const integer startSample = rightSample - halfnsamp_window;
			const integer endSample = leftSample + halfnsamp_window;
			Melder_assert (startSample >= 1);
			Melder_assert (endSample <= my nx);

			spectrum.all()  <<=  0.0;
			/*
				For multichannel sounds, the power spectrogram should represent the
				average power in the channels,
				so that the result for a stereo sound in which the
				left channel has the same waveform as the right channel,
				is identical to the result for the corresponding mono (= averaged) sound.
				Averaging starts by adding up the powers of the channels.
			*/
			for (integer channel = 1; channel <= my ny; channel ++) {
				for (integer j = 1, i = startSample; j <= nsamp_window; j ++)
					data [j] = my z [channel] [i ++] * window [j];
				for (integer j = nsamp_window + 1; j <= nsampFFT; j ++)
					data [j] = 0.0f;

				Melder_progress (iframe / (numberOfTimes + 1.0),
					U"Sound to Spectrogram: analysis of frame ", iframe, U" out of ", numberOfTimes);

				/*
					Compute the Fast Fourier Transform of the frame.
				*/
				NUMfft_forward (& fftTable, data.get());   // data := complex spectrum

				/*
					Convert from complex to power spectrum,
					accumulating the power spectra of the channels.
				*/
				spectrum [1] += data [1] * data [1];   // DC component
				for (integer i = 2; i <= half_nsampFFT; i ++)
					spectrum [i] += data [i + i - 2] * data [i + i - 2] + data [i + i - 1] * data [i + i - 1];
				spectrum [half_nsampFFT + 1] += data [nsampFFT] * data [nsampFFT];   // Nyquist frequency. Correct??
			}
			/*
				Power averaging ends by dividing the summed power by the number of channels,
			*/
			if (my ny > 1 )
				spectrum.all()  /=  my ny;

			/*
				Binning.
			*/
			for (integer iband = 1; iband <= numberOfFreqs; iband ++) {
				const integer lowerSample = (iband - 1) * binWidth_samples + 1;
				const integer higherSample = lowerSample + binWidth_samples;
				const double power = NUMsum (spectrum.part (lowerSample, higherSample - 1));
				thy z [iband] [iframe] = power * oneByBinWidth;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": spectrogram analysis not performed.");
	}
}

autoSound Spectrogram_to_Sound (Spectrogram me, double fsamp) {
	try {
		const double dt = 1.0 / fsamp;
		const integer n = Melder_ifloor ((my xmax - my xmin) / dt);
		if (n < 0)
			return autoSound ();
		autoSound you = Sound_create (1, my xmin, my xmax, n, dt, 0.5 * dt);
		for (integer i = 1; i <= n; i ++) {
			const double t = Sampled_indexToX (you.get(), i);
			const double rframe = Sampled_xToIndex (me, t);
			if (rframe < 1 || rframe >= my nx)
				continue;
			const integer leftFrame = Melder_ifloor (rframe);
			const integer rightFrame = leftFrame + 1;
			const double phase = rframe - leftFrame;
			longdouble value = 0.0;
			for (integer j = 1; j <= my ny; j ++) {
				const double f = Matrix_rowToY (me, j);
				const double power = my z [j] [leftFrame] * (1.0 - phase) + my z [j] [rightFrame] * phase;
				value += sqrt (power) * sin (2 * NUMpi * f * t);
			}
			your z [1] [i] = double (value);
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

/* End of file Sound_and_Spectrogram.cpp */
