/* Sound_and_Spectrogram.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2003/07/02 checks on NUMrealft
 * pb 2003/11/30 Sound_to_Spectrogram_windowShapeText
 * pb 2004/03/13 bins are a fixed number of frequency samples wide;
 *               this improves the positioning of peaks; thanks to Gabriel Beckers for his persistence
 * pb 2004/10/18 use of FFT tables speeds everything up by a factor of 2.5
 * pb 2004/10/20 progress bar
 */

#include "Sound_and_Spectrogram.h"
#include "NUM2.h"

const char * Sound_to_Spectrogram_windowShapeText (int windowShape) {
	return
		windowShape == Sound_to_Spectrogram_WINDOW_SHAPE_SQUARE ? "Square (rectangular)" :
		windowShape == Sound_to_Spectrogram_WINDOW_SHAPE_HAMMING ? "Hamming (raised sine-squared)" :
		windowShape == Sound_to_Spectrogram_WINDOW_SHAPE_BARTLETT ? "Bartlett (triangular)" :
		windowShape == Sound_to_Spectrogram_WINDOW_SHAPE_WELCH ? "Welch (parabolic)" :
		windowShape == Sound_to_Spectrogram_WINDOW_SHAPE_HANNING ? "Hanning (sine-squared)" :
		windowShape == Sound_to_Spectrogram_WINDOW_SHAPE_GAUSSIAN ? "Gaussian" :
		"(unknown)";
}

Spectrogram Sound_to_Spectrogram (Sound me, double effectiveAnalysisWidth, double fmax,
	double minimumTimeStep1, double minimumFreqStep1, int windowType,
	double maximumTimeOversampling, double maximumFreqOversampling)
{
	Spectrogram thee = NULL;
	double nyquist = 0.5 / my dx;
	double physicalAnalysisWidth = windowType == 5 ? 2 * effectiveAnalysisWidth : effectiveAnalysisWidth;
	double effectiveTimeWidth = effectiveAnalysisWidth / sqrt (NUMpi);
	double effectiveFreqWidth = 1 / effectiveTimeWidth;
	double minimumTimeStep2 = effectiveTimeWidth / maximumTimeOversampling;
	double minimumFreqStep2 = effectiveFreqWidth / maximumFreqOversampling;
	double timeStep = minimumTimeStep1 > minimumTimeStep2 ? minimumTimeStep1 : minimumTimeStep2;
	double freqStep = minimumFreqStep1 > minimumFreqStep2 ? minimumFreqStep1 : minimumFreqStep2;

	long nsamp_window, halfnsamp_window, numberOfTimes, numberOfFreqs, nsampFFT = 1, half_nsampFFT;
	long iframe, iband, i, j;
	float *amplitude = my z [1], *frame = NULL, *window = NULL, oneByBinWidth;
	long binWidth_samples;
	double duration = my dx * (double) my nx, t1, windowssq = 0.0, binWidth_hertz;

	struct NUMfft_Table_f fftTable_struct;
	NUMfft_Table_f fftTable = & fftTable_struct;
	fftTable -> trigcache = NULL;
	fftTable -> splitcache = NULL;

	/*
	 * Compute the time sampling.
	 */
	nsamp_window = (long) floor (physicalAnalysisWidth / my dx);
	halfnsamp_window = nsamp_window / 2 - 1;
	nsamp_window = halfnsamp_window * 2;
	if (physicalAnalysisWidth > duration)
		return Melder_errorp ("(Sound_to_Spectrogram:) Your sound is too short:\n"
			"it should be at least as long as %s.",
			windowType == 5 ? "two window lengths" : "one window length");
	numberOfTimes = 1 + (long) floor ((duration - physicalAnalysisWidth) / timeStep);   /* >= 1 */
	t1 = my x1 + 0.5 * ((double) (my nx - 1) * my dx - (double) (numberOfTimes - 1) * timeStep);
		/* Centre of first frame. */

	/*
	 * Compute the frequency sampling of the FFT spectrum.
	 */
	if (fmax <= 0.0 || fmax > nyquist) fmax = nyquist;
	numberOfFreqs = (long) floor (fmax / freqStep);
	if (numberOfFreqs < 1) return NULL;
	nsampFFT = 1;
	while (nsampFFT < nsamp_window || nsampFFT < 2 * numberOfFreqs * (nyquist / fmax))
		nsampFFT *= 2;
	half_nsampFFT = nsampFFT / 2;

	/*
	 * Compute the frequency sampling of the spectrogram.
	 */
	binWidth_samples = freqStep * my dx * nsampFFT;
	if (binWidth_samples < 1) binWidth_samples = 1;
	binWidth_hertz = 1.0 / (my dx * nsampFFT);
	freqStep = binWidth_samples * binWidth_hertz;
	numberOfFreqs = floor (fmax / freqStep);
	if (numberOfFreqs < 1) return NULL;

	thee = Spectrogram_create (my xmin, my xmax, numberOfTimes, timeStep, t1,
			0.0, fmax, numberOfFreqs, freqStep, 0.5 * (freqStep - binWidth_hertz)); cherror

	frame = NUMfvector (1, nsampFFT); cherror
	window = NUMfvector (1, nsamp_window); cherror
	NUMfft_Table_init_f (fftTable, nsampFFT); cherror

	Melder_progress (0.0, "Sound to Spectrogram...");
	for (i = 1; i <= nsamp_window; i ++) {
		double nSamplesPerWindow_f = physicalAnalysisWidth / my dx;
		double phase = (double) i / nSamplesPerWindow_f;   /* 0 .. 1 */
		double value;
		switch (windowType) {
			case 0: /* None (rectangular). */
				value = 1.0;
			break; case 1: /* Hamming (raised sine-squared). */
				value = 0.54 - 0.46 * cos (2.0 * NUMpi * phase);
			break; case 2: /* Bartlett (triangular). */
				value = 1.0 - fabs ((2.0 * phase - 1.0));
			break; case 3: /* Welch (parabolic). */
				value = 1.0 - (2.0 * phase - 1.0) * (2.0 * phase - 1.0);
			break; case 4: /* Hanning (sine-squared). */
				value = 0.5 * (1.0 - cos (2.0 * NUMpi * phase));
			break; case 5: /* Gaussian. */
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
	oneByBinWidth = 1.0 / windowssq / binWidth_samples;

	for (iframe = 1; iframe <= numberOfTimes; iframe ++) {
		double t = Sampled_indexToX (thee, iframe);
		long leftSample = Sampled_xToLowIndex (me, t), rightSample = leftSample + 1;
		long startSample = rightSample - halfnsamp_window;
		long endSample = leftSample + halfnsamp_window;
		Melder_assert (startSample >= 1);
		Melder_assert (endSample <= my nx);
		for (j = 1, i = startSample; j <= nsamp_window; j ++)
			frame [j] = amplitude [i ++] * window [j];
		for (j = nsamp_window + 1; j <= nsampFFT; j ++) frame [j] = 0.0f;

		Melder_progress (iframe / (numberOfTimes + 1.0),
			"Sound to Spectrogram: analysis of frame %ld out of %ld", iframe, numberOfTimes); cherror

		/* Compute Fast Fourier Transform of the frame. */

		NUMfft_forward_f (fftTable, frame);   /* Complex spectrum. */

		/* Put power spectrum in frame [1..half_nsampFFT + 1]. */

		frame [1] *= frame [1];   /* DC component. */
		for (i = 2; i <= half_nsampFFT; i ++)
			frame [i] = frame [i + i - 2] * frame [i + i - 2] + frame [i + i - 1] * frame [i + i - 1];
		frame [half_nsampFFT + 1] = frame [nsampFFT] * frame [nsampFFT];   /* Nyquist frequency. Correct?? */

		/* Bin into frame [1..nBands]. */
		for (iband = 1; iband <= numberOfFreqs; iband ++) {
			long leftsample = (iband - 1) * binWidth_samples + 1, rightsample = leftsample + binWidth_samples;
			float power = 0.0f;
			for (i = leftsample; i < rightsample; i ++) power += frame [i];
			thy z [iband] [iframe] = power * oneByBinWidth;
		}
	}
end:
	Melder_progress (1.0, NULL);
	NUMfvector_free (frame, 1);
	NUMfvector_free (window, 1);
	NUMfft_Table_free_f (fftTable);
	iferror forget (thee);
	return thee;
}

Sound Spectrogram_to_Sound (Spectrogram me, double fsamp) {
	double dt = 1 / fsamp;
	long n = (my xmax - my xmin) / dt, i, j;
	Sound thee = NULL;
	if (n < 0) return NULL;
	thee = Sound_create (my xmin, my xmax, n, dt, 0.5 * dt); cherror
	for (i = 1; i <= n; i ++) {
		double t = Sampled_indexToX (thee, i);
		double rframe = Sampled_xToIndex (me, t), phase, value = 0.0;
		long leftFrame, rightFrame;
		if (rframe < 1 || rframe >= my nx) continue;
		leftFrame = floor (rframe), rightFrame = leftFrame + 1, phase = rframe - leftFrame;
		for (j = 1; j <= my ny; j ++) {
			double f = Matrix_rowToY (me, j);
			double power = my z [j] [leftFrame] * (1 - phase) + my z [j] [rightFrame] * phase;
			value += sqrt (power) * sin (2 * NUMpi * f * t);
		}
		thy z [1] [i] = value;
	}
end:
	iferror forget (thee);
	return thee;
}

/* End of file Sound_and_Spectrogram.c */
