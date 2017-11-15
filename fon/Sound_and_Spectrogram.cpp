/* Sound_and_Spectrogram.cpp
 *
 * Copyright (C) 1992-2011,2014,2015,2017 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2003/07/02 checks on NUMrealft
 * pb 2003/11/30 Sound_to_Spectrogram_windowShapeText
 * pb 2004/03/13 bins are a fixed number of frequency samples wide;
 *               this improves the positioning of peaks; thanks to Gabriel Beckers for his persistence
 * pb 2004/10/18 use of FFT tables speeds everything up by a factor of 2.5
 * pb 2004/10/20 progress bar
 * pb 2006/12/30 new Sound_create API
 * pb 2007/01/01 compatible with stereo sounds
 * pb 2007/12/06 enums
 * pb 2008/01/19 double
 * pb 2010/02/26 fixed a message
 * pb 2011/06/06 C++
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
		double nyquist = 0.5 / my dx;
		double physicalAnalysisWidth =
			windowType == kSound_to_Spectrogram_windowShape::GAUSSIAN ? 2 * effectiveAnalysisWidth : effectiveAnalysisWidth;
		double effectiveTimeWidth = effectiveAnalysisWidth / sqrt (NUMpi);
		double effectiveFreqWidth = 1 / effectiveTimeWidth;
		double minimumTimeStep2 = effectiveTimeWidth / maximumTimeOversampling;
		double minimumFreqStep2 = effectiveFreqWidth / maximumFreqOversampling;
		double timeStep = minimumTimeStep1 > minimumTimeStep2 ? minimumTimeStep1 : minimumTimeStep2;
		double freqStep = minimumFreqStep1 > minimumFreqStep2 ? minimumFreqStep1 : minimumFreqStep2;
		double duration = my dx * (double) my nx, windowssq = 0.0;

		/*
		 * Compute the time sampling.
		 */
		integer nsamp_window = Melder_ifloor (physicalAnalysisWidth / my dx);
		integer halfnsamp_window = nsamp_window / 2 - 1;
		nsamp_window = halfnsamp_window * 2;
		if (nsamp_window < 1)
			Melder_throw (U"Your analysis window is too short: less than two samples.");
		if (physicalAnalysisWidth > duration)
			Melder_throw (U"Your sound is too short:\n"
				U"it should be at least as long as ",
				windowType == kSound_to_Spectrogram_windowShape::GAUSSIAN ? U"two window lengths." : U"one window length.");
		integer numberOfTimes = 1 + Melder_ifloor ((duration - physicalAnalysisWidth) / timeStep);   // >= 1
		double t1 = my x1 + 0.5 * ((double) (my nx - 1) * my dx - (double) (numberOfTimes - 1) * timeStep);
			/* Centre of first frame. */

		/*
		 * Compute the frequency sampling of the FFT spectrum.
		 */
		if (fmax <= 0.0 || fmax > nyquist) fmax = nyquist;
		integer numberOfFreqs = Melder_ifloor (fmax / freqStep);
		if (numberOfFreqs < 1) return autoSpectrogram ();
		integer nsampFFT = 1;
		while (nsampFFT < nsamp_window || nsampFFT < 2 * numberOfFreqs * (nyquist / fmax))
			nsampFFT *= 2;
		integer half_nsampFFT = nsampFFT / 2;

		/*
		 * Compute the frequency sampling of the spectrogram.
		 */
		integer binWidth_samples = Melder_ifloor (freqStep * my dx * nsampFFT);
		if (binWidth_samples < 1) binWidth_samples = 1;
		double binWidth_hertz = 1.0 / (my dx * nsampFFT);
		freqStep = binWidth_samples * binWidth_hertz;
		numberOfFreqs = Melder_ifloor (fmax / freqStep);
		if (numberOfFreqs < 1) return autoSpectrogram ();

		autoSpectrogram thee = Spectrogram_create (my xmin, my xmax, numberOfTimes, timeStep, t1,
				0.0, fmax, numberOfFreqs, freqStep, 0.5 * (freqStep - binWidth_hertz));

		autoNUMvector <double> frame (1, nsampFFT);
		autoNUMvector <double> spec (1, nsampFFT);
		autoNUMvector <double> window (1, nsamp_window);
		autoNUMfft_Table fftTable;
		NUMfft_Table_init (& fftTable, nsampFFT);

		autoMelderProgress progress (U"Sound to Spectrogram...");
		for (integer i = 1; i <= nsamp_window; i ++) {
			double nSamplesPerWindow_f = physicalAnalysisWidth / my dx;
			double phase = (double) i / nSamplesPerWindow_f;   // 0 .. 1
			double value;
			switch (windowType) {
				case kSound_to_Spectrogram_windowShape::SQUARE:
					value = 1.0;
				break; case kSound_to_Spectrogram_windowShape::HAMMING:
					value = 0.54 - 0.46 * cos (2.0 * NUMpi * phase);
				break; case kSound_to_Spectrogram_windowShape::BARTLETT:
					value = 1.0 - fabs ((2.0 * phase - 1.0));
				break; case kSound_to_Spectrogram_windowShape::WELCH:
					value = 1.0 - (2.0 * phase - 1.0) * (2.0 * phase - 1.0);
				break; case kSound_to_Spectrogram_windowShape::HANNING:
					value = 0.5 * (1.0 - cos (2.0 * NUMpi * phase));
				break; case kSound_to_Spectrogram_windowShape::GAUSSIAN:
				{
					double imid = 0.5 * (double) (nsamp_window + 1), edge = exp (-12.0);
					phase = ((double) i - imid) / nSamplesPerWindow_f;   /* -0.5 .. +0.5 */
					value = (exp (-48.0 * phase * phase) - edge) / (1.0 - edge);
					break;
				}
				break; default:
					value = 1.0;
			}
			window [i] = (float) value;
			windowssq += value * value;
		}
		double oneByBinWidth = 1.0 / windowssq / binWidth_samples;

		for (integer iframe = 1; iframe <= numberOfTimes; iframe ++) {
			double t = Sampled_indexToX (thee.get(), iframe);
			integer leftSample = Sampled_xToLowIndex (me, t), rightSample = leftSample + 1;
			integer startSample = rightSample - halfnsamp_window;
			integer endSample = leftSample + halfnsamp_window;
			Melder_assert (startSample >= 1);
			Melder_assert (endSample <= my nx);
			for (integer i = 1; i <= half_nsampFFT; i ++) {
				spec [i] = 0.0;
			}
			for (integer channel = 1; channel <= my ny; channel ++) {
				for (integer j = 1, i = startSample; j <= nsamp_window; j ++) {
					frame [j] = my z [channel] [i ++] * window [j];
				}
				for (integer j = nsamp_window + 1; j <= nsampFFT; j ++) frame [j] = 0.0f;

				Melder_progress (iframe / (numberOfTimes + 1.0),
					U"Sound to Spectrogram: analysis of frame ", iframe, U" out of ", numberOfTimes);

				/* Compute Fast Fourier Transform of the frame. */

				NUMfft_forward (& fftTable, frame.peek());   // complex spectrum

				/* Put power spectrum in frame [1..half_nsampFFT + 1]. */

				spec [1] += frame [1] * frame [1];   // DC component
				for (integer i = 2; i <= half_nsampFFT; i ++)
					spec [i] += frame [i + i - 2] * frame [i + i - 2] + frame [i + i - 1] * frame [i + i - 1];
				spec [half_nsampFFT + 1] += frame [nsampFFT] * frame [nsampFFT];   // Nyquist frequency. Correct??
			}
			if (my ny > 1 ) for (integer i = 1; i <= half_nsampFFT; i ++) {
				spec [i] /= my ny;
			}

			/* Bin into frame [1..nBands]. */
			for (integer iband = 1; iband <= numberOfFreqs; iband ++) {
				integer leftsample = (iband - 1) * binWidth_samples + 1, rightsample = leftsample + binWidth_samples;
				float power = 0.0f;
				for (integer i = leftsample; i < rightsample; i ++) power += spec [i];
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
		double dt = 1.0 / fsamp;
		integer n = Melder_ifloor ((my xmax - my xmin) / dt);
		if (n < 0) return autoSound ();
		autoSound thee = Sound_create (1, my xmin, my xmax, n, dt, 0.5 * dt);
		for (integer i = 1; i <= n; i ++) {
			double t = Sampled_indexToX (thee.get(), i);
			double rframe = Sampled_xToIndex (me, t), phase, value = 0.0;
			integer leftFrame, rightFrame;
			if (rframe < 1 || rframe >= my nx) continue;
			leftFrame = Melder_ifloor (rframe), rightFrame = leftFrame + 1, phase = rframe - leftFrame;
			for (integer j = 1; j <= my ny; j ++) {
				double f = Matrix_rowToY (me, j);
				double power = my z [j] [leftFrame] * (1 - phase) + my z [j] [rightFrame] * phase;
				value += sqrt (power) * sin (2 * NUMpi * f * t);
			}
			thy z [1] [i] = value;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

/* End of file Sound_and_Spectrogram.cpp */
