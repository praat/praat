/* Sound_to_Pitch.cpp
 *
 * Copyright (C) 1992-2011,2014,2015,2016,2017 Paul Boersma
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
 * pb 2002/10/11 removed some assertions
 * pb 2003/05/20 default time step is four times oversampling
 * pb 2003/07/02 checks on NUMrealft
 * pb 2004/05/10 better error messages
 * pb 2004/10/18 auto maxnCandidates
 * pb 2004/10/18 use of constant FFT tables speeds up AC method by a factor of 1.9
 * pb 2006/12/31 compatible with stereo sounds
 * pb 2007/01/30 loop split for stereo speeds up CC method by a factor of 6
 * pb 2008/01/19 double
 * pb 2010/12/07 compatible with sounds with any number of channels
 * pb 2011/03/08 C++
 * pb 2014/05/23 threads
 */

#include "Sound_to_Pitch.h"
#include "NUM2.h"
#include "MelderThread.h"

#define AC_HANNING  0
#define AC_GAUSS  1
#define FCC_NORMAL  2
#define FCC_ACCURATE  3

static void Sound_into_PitchFrame (Sound me, Pitch_Frame pitchFrame, double t,
	double minimumPitch, int maxnCandidates, int method, double voicingThreshold, double octaveCost,
	NUMfft_Table fftTable, double dt_window, long nsamp_window, long halfnsamp_window,
	long maximumLag, long nsampFFT, long nsamp_period, long halfnsamp_period,
	long brent_ixmax, long brent_depth, double globalPeak,
	double **frame, double *ac, double *window, double *windowR,
	double *r, long *imax, double *localMean)
{
	integer leftSample = Sampled_xToLowIndex (me, t), rightSample = leftSample + 1;
	integer startSample, endSample;

	for (integer channel = 1; channel <= my ny; channel ++) {
		/*
		 * Compute the local mean; look one longest period to both sides.
		 */
		startSample = rightSample - nsamp_period;
		endSample = leftSample + nsamp_period;
		Melder_assert (startSample >= 1);
		Melder_assert (endSample <= my nx);
		localMean [channel] = 0.0;
		for (integer i = startSample; i <= endSample; i ++) {
			localMean [channel] += my z [channel] [i];
		}
		localMean [channel] /= 2 * nsamp_period;

		/*
		 * Copy a window to a frame and subtract the local mean.
		 * We are going to kill the DC component before windowing.
		 */
		startSample = rightSample - halfnsamp_window;
		endSample = leftSample + halfnsamp_window;
		Melder_assert (startSample >= 1);
		Melder_assert (endSample <= my nx);
		if (method < FCC_NORMAL) {
			for (integer j = 1, i = startSample; j <= nsamp_window; j ++)
				frame [channel] [j] = (my z [channel] [i ++] - localMean [channel]) * window [j];
			for (integer j = nsamp_window + 1; j <= nsampFFT; j ++)
				frame [channel] [j] = 0.0;
		} else {
			for (integer j = 1, i = startSample; j <= nsamp_window; j ++)
				frame [channel] [j] = my z [channel] [i ++] - localMean [channel];
		}
	}

	/*
	 * Compute the local peak; look half a longest period to both sides.
	 */
	real localPeak = 0.0;
	if ((startSample = halfnsamp_window + 1 - halfnsamp_period) < 1) startSample = 1;
	if ((endSample = halfnsamp_window + halfnsamp_period) > nsamp_window) endSample = nsamp_window;
	for (integer channel = 1; channel <= my ny; channel ++) {
		for (integer j = startSample; j <= endSample; j ++) {
			real value = fabs (frame [channel] [j]);
			if (value > localPeak) localPeak = value;
		}
	}
	pitchFrame -> intensity =
		localPeak > globalPeak ? 1.0 : localPeak / globalPeak;

	/*
	 * Compute the correlation into the array 'r'.
	 */
	if (method >= FCC_NORMAL) {
		real startTime = t - 0.5 * (1.0 / minimumPitch + dt_window);
		integer localSpan = maximumLag + nsamp_window, localMaximumLag, offset;
		if ((startSample = Sampled_xToLowIndex (me, startTime)) < 1) startSample = 1;
		if (localSpan > my nx + 1 - startSample) localSpan = my nx + 1 - startSample;
		localMaximumLag = localSpan - nsamp_window;
		offset = startSample - 1;
		real80 sumx2 = 0.0;   // sum of squares
		for (integer channel = 1; channel <= my ny; channel ++) {
			real *amp = my z [channel] + offset;
			for (integer i = 1; i <= nsamp_window; i ++) {
				real x = amp [i] - localMean [channel];
				sumx2 += x * x;
			}
		}
		real80 sumy2 = sumx2;   // at zero lag, these are still equal
		r [0] = 1.0;
		for (integer i = 1; i <= localMaximumLag; i ++) {
			real80 product = 0.0;
			for (integer channel = 1; channel <= my ny; channel ++) {
				real *amp = my z [channel] + offset;
				real y0 = amp [i] - localMean [channel];
				real yZ = amp [i + nsamp_window] - localMean [channel];
				sumy2 += yZ * yZ - y0 * y0;
				for (integer j = 1; j <= nsamp_window; j ++) {
					real x = amp [j] - localMean [channel];
					real y = amp [i + j] - localMean [channel];
					product += x * y;
				}
			}
			r [- i] = r [i] = (real) product / sqrt ((real) sumx2 * (real) sumy2);
		}
	} else {

		/*
		 * The FFT of the autocorrelation is the power spectrum.
		 */
		for (integer i = 1; i <= nsampFFT; i ++) {
			ac [i] = 0.0;
		}
		for (integer channel = 1; channel <= my ny; channel ++) {
			NUMfft_forward (fftTable, frame [channel]);   // complex spectrum
			ac [1] += frame [channel] [1] * frame [channel] [1];   // DC component
			for (integer i = 2; i < nsampFFT; i += 2) {
				ac [i] += frame [channel] [i] * frame [channel] [i] + frame [channel] [i+1] * frame [channel] [i+1];   // power spectrum
			}
			ac [nsampFFT] += frame [channel] [nsampFFT] * frame [channel] [nsampFFT];   // Nyquist frequency
		}
		NUMfft_backward (fftTable, ac);   /* Autocorrelation. */

		/*
		 * Normalize the autocorrelation to the value with zero lag,
		 * and divide it by the normalized autocorrelation of the window.
		 */
		r [0] = 1.0;
		for (integer i = 1; i <= brent_ixmax; i ++)
			r [- i] = r [i] = ac [i + 1] / (ac [1] * windowR [i + 1]);
	}

	/*
	 * Register the first candidate, which is always present: voicelessness.
	 */
	pitchFrame -> nCandidates = 1;
	pitchFrame -> candidate [1]. frequency = 0.0;   // voiceless: always present
	pitchFrame -> candidate [1]. strength = 0.0;

	/*
	 * Shortcut: absolute silence is always voiceless.
	 * We are done for this frame.
	 */
	if (localPeak == 0.0) return;

	/*
	 * Find the strongest maxima of the correlation of this frame, 
	 * and register them as candidates.
	 */
	imax [1] = 0;
	for (long i = 2; i < maximumLag && i < brent_ixmax; i ++)
		if (r [i] > 0.5 * voicingThreshold &&   // not too unvoiced?
			r [i] > r [i-1] && r [i] >= r [i+1])   // maximum?
	{
		int place = 0;

		/*
		 * Use parabolic interpolation for first estimate of frequency,
		 * and sin(x)/x interpolation to compute the strength of this frequency.
		 */
		double dr = 0.5 * (r [i+1] - r [i-1]), d2r = 2.0 * r [i] - r [i-1] - r [i+1];
		double frequencyOfMaximum = 1.0 / my dx / (i + dr / d2r);
		long offset = - brent_ixmax - 1;
		double strengthOfMaximum = /* method & 1 ? */
			NUM_interpolate_sinc (& r [offset], brent_ixmax - offset, 1 / my dx / frequencyOfMaximum - offset, 30)
			/* : r [i] + 0.5 * dr * dr / d2r */;
		/* High values due to short windows are to be reflected around 1. */
		if (strengthOfMaximum > 1.0) strengthOfMaximum = 1.0 / strengthOfMaximum;

		/*
		 * Find a place for this maximum.
		 */
		if (pitchFrame->nCandidates < maxnCandidates) {   // is there still a free place?
			place = ++ pitchFrame -> nCandidates;
		} else {
			/* Try the place of the weakest candidate so far. */
			double weakest = 2.0;
			for (int iweak = 2; iweak <= maxnCandidates; iweak ++) {
				/* High frequencies are to be favoured */
				/* if we want to analyze a perfectly periodic signal correctly. */
				double localStrength = pitchFrame -> candidate [iweak]. strength - octaveCost *
					NUMlog2 (minimumPitch / pitchFrame -> candidate [iweak]. frequency);
				if (localStrength < weakest) {
					weakest = localStrength;
					place = iweak;
				}
			}
			/* If this maximum is weaker than the weakest candidate so far, give it no place. */
			if (strengthOfMaximum - octaveCost * NUMlog2 (minimumPitch / frequencyOfMaximum) <= weakest)
				place = 0;
		}
		if (place) {   // have we found a place for this candidate?
			pitchFrame -> candidate [place]. frequency = frequencyOfMaximum;
			pitchFrame -> candidate [place]. strength = strengthOfMaximum;
			imax [place] = i;
		}
	}

	/*
	 * Second pass: for extra precision, maximize sin(x)/x interpolation ('sinc').
	 */
	for (long i = 2; i <= pitchFrame -> nCandidates; i ++) {
		if (method != AC_HANNING || pitchFrame -> candidate [i]. frequency > 0.0 / my dx) {
			double xmid, ymid;
			long offset = - brent_ixmax - 1;
			ymid = NUMimproveMaximum (& r [offset], brent_ixmax - offset, imax [i] - offset,
				pitchFrame -> candidate [i]. frequency > 0.3 / my dx ? NUM_PEAK_INTERPOLATE_SINC700 : brent_depth, & xmid);
			xmid += offset;
			pitchFrame -> candidate [i]. frequency = 1.0 / my dx / xmid;
			if (ymid > 1.0) ymid = 1.0 / ymid;
			pitchFrame -> candidate [i]. strength = ymid;
		}
	}
}

Thing_define (Sound_into_Pitch_Args, Thing) { public:
	Sound sound;
	Pitch pitch;
	long firstFrame, lastFrame;
	double minimumPitch;
	int maxnCandidates, method;
	double voicingThreshold, octaveCost, dt_window;
	long nsamp_window, halfnsamp_window, maximumLag, nsampFFT, nsamp_period, halfnsamp_period, brent_ixmax, brent_depth;
	double globalPeak, *window, *windowR;
	bool isMainThread;
	volatile int *cancelled;
};

Thing_implement (Sound_into_Pitch_Args, Thing, 0);

static autoSound_into_Pitch_Args Sound_into_Pitch_Args_create (Sound sound, Pitch pitch,
	long firstFrame, long lastFrame, double minimumPitch, int maxnCandidates, int method,
	double voicingThreshold, double octaveCost,
	double dt_window, long nsamp_window, long halfnsamp_window, long maximumLag, long nsampFFT,
	long nsamp_period, long halfnsamp_period, long brent_ixmax, long brent_depth,
	double globalPeak, double *window, double *windowR,
	bool isMainThread, volatile int *cancelled)
{
	autoSound_into_Pitch_Args me = Thing_new (Sound_into_Pitch_Args);
	my sound = sound;
	my pitch = pitch;
	my firstFrame = firstFrame;
	my lastFrame = lastFrame;
	my minimumPitch = minimumPitch;
	my maxnCandidates = maxnCandidates;
	my method = method;
	my voicingThreshold = voicingThreshold;
	my octaveCost = octaveCost;
	my dt_window = dt_window;
	my nsamp_window = nsamp_window;
	my halfnsamp_window = halfnsamp_window;
	my maximumLag = maximumLag;
	my nsampFFT = nsampFFT;
	my nsamp_period = nsamp_period;
	my halfnsamp_period = halfnsamp_period;
	my brent_ixmax = brent_ixmax;
	my brent_depth = brent_depth;
	my globalPeak = globalPeak;
	my window = window;
	my windowR = windowR;
	my isMainThread = isMainThread;
	my cancelled = cancelled;
	return me;
}

MelderThread_MUTEX (mutex);
bool mutex_inited;

static MelderThread_RETURN_TYPE Sound_into_Pitch (Sound_into_Pitch_Args me)
{
	autoNUMfft_Table fftTable;
	autoNUMmatrix <double> frame;
	autoNUMvector <double> ac, r, localMean;
	autoNUMvector <long> imax;
	{// scope
		MelderThread_LOCK (mutex);
		if (my method >= FCC_NORMAL) {   // cross-correlation
			frame.reset (1, my sound -> ny, 1, my nsamp_window);
		} else {   // autocorrelation
			NUMfft_Table_init (& fftTable, my nsampFFT);
			frame.reset (1, my sound -> ny, 1, my nsampFFT);
			ac.reset (1, my nsampFFT);
		}
		r.reset (- my nsamp_window, my nsamp_window);
		imax.reset (1, my maxnCandidates);
		localMean.reset (1, my sound -> ny);
		MelderThread_UNLOCK (mutex);
	}
	for (long iframe = my firstFrame; iframe <= my lastFrame; iframe ++) {
		Pitch_Frame pitchFrame = & my pitch -> frame [iframe];
		double t = Sampled_indexToX (my pitch, iframe);
		if (my isMainThread) {
			try {
				Melder_progress (0.1 + 0.8 * (iframe - my firstFrame) / (my lastFrame - my firstFrame),
					U"Sound to Pitch: analysing ", my lastFrame, U" frames");
			} catch (MelderError) {
				*my cancelled = 1;
				throw;
			}
		} else if (*my cancelled) {
			MelderThread_RETURN;
		}
		Sound_into_PitchFrame (my sound, pitchFrame, t,
			my minimumPitch, my maxnCandidates, my method, my voicingThreshold, my octaveCost,
			& fftTable, my dt_window, my nsamp_window, my halfnsamp_window,
			my maximumLag, my nsampFFT, my nsamp_period, my halfnsamp_period,
			my brent_ixmax, my brent_depth, my globalPeak,
			frame.peek(), ac.peek(), my window, my windowR,
			r.peek(), imax.peek(), localMean.peek());
	}
	MelderThread_RETURN;
}

autoPitch Sound_to_Pitch_any (Sound me,
	double dt, double minimumPitch, double periodsPerWindow, int maxnCandidates,
	int method,
	double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost, double ceiling)
{
	try {
		autoNUMfft_Table fftTable;
		double t1;
		long nFrames, minimumLag, maximumLag;
		long nsampFFT;
		double interpolation_depth;
		long brent_ixmax, brent_depth;
		double globalPeak;

		Melder_assert (maxnCandidates >= 2);
		Melder_assert (method >= AC_HANNING && method <= FCC_ACCURATE);

		if (maxnCandidates < ceiling / minimumPitch) maxnCandidates = (long) floor (ceiling / minimumPitch);

		if (dt <= 0.0) dt = periodsPerWindow / minimumPitch / 4.0;   // e.g. 3 periods, 75 Hz: 10 milliseconds

		switch (method) {
			case AC_HANNING:
				brent_depth = NUM_PEAK_INTERPOLATE_SINC70;
				interpolation_depth = 0.5;
				break;
			case AC_GAUSS:
				periodsPerWindow *= 2;   // because Gaussian window is twice as long
				brent_depth = NUM_PEAK_INTERPOLATE_SINC700;
				interpolation_depth = 0.25;   // because Gaussian window is twice as long
				break;
			case FCC_NORMAL:
				brent_depth = NUM_PEAK_INTERPOLATE_SINC70;
				interpolation_depth = 1.0;
				break;
			case FCC_ACCURATE:
				brent_depth = NUM_PEAK_INTERPOLATE_SINC700;
				interpolation_depth = 1.0;
				break;
		}
		real duration = my dx * my nx;
		if (minimumPitch < periodsPerWindow / duration)
			Melder_throw (U"To analyse this Sound, ", U_LEFT_DOUBLE_QUOTE, U"minimum pitch", U_RIGHT_DOUBLE_QUOTE, U" must not be less than ", periodsPerWindow / duration, U" Hz.");

		/*
		 * Determine the number of samples in the longest period.
		 * We need this to compute the local mean of the sound (looking one period in both directions),
		 * and to compute the local peak of the sound (looking half a period in both directions).
		 */
		integer nsamp_period = (long) floor (1 / my dx / minimumPitch);
		integer halfnsamp_period = nsamp_period / 2 + 1;

		if (ceiling > 0.5 / my dx) ceiling = 0.5 / my dx;

		/*
		 * Determine window length in seconds and in samples.
		 */
		real dt_window = periodsPerWindow / minimumPitch;
		integer nsamp_window = (long) floor (dt_window / my dx);
		integer halfnsamp_window = nsamp_window / 2 - 1;
		if (halfnsamp_window < 2)
			Melder_throw (U"Analysis window too short.");
		nsamp_window = halfnsamp_window * 2;

		/*
		 * Determine the minimum and maximum lags.
		 */
		minimumLag = (long) floor (1.0 / my dx / ceiling);
		if (minimumLag < 2) minimumLag = 2;
		maximumLag = (long) floor (nsamp_window / periodsPerWindow) + 2;
		if (maximumLag > nsamp_window) maximumLag = nsamp_window;

		/*
		 * Determine the number of frames.
		 * Fit as many frames as possible symmetrically in the total duration.
		 * We do this even for the forward cross-correlation method,
		 * because that allows us to compare the two methods.
		 */
		try {
			Sampled_shortTermAnalysis (me, method >= FCC_NORMAL ? 1.0 / minimumPitch + dt_window : dt_window, dt, & nFrames, & t1);
		} catch (MelderError) {
			Melder_throw (U"The pitch analysis would give zero pitch frames.");
		}

		/*
		 * Create the resulting pitch contour.
		 */
		autoPitch thee = Pitch_create (my xmin, my xmax, nFrames, dt, t1, ceiling, maxnCandidates);

		/*
		 * Create (too much) space for candidates.
		 */
		for (long iframe = 1; iframe <= nFrames; iframe ++) {
			Pitch_Frame pitchFrame = & thy frame [iframe];
			Pitch_Frame_init (pitchFrame, maxnCandidates);
		}

		/*
		 * Compute the global absolute peak for determination of silence threshold.
		 */
		globalPeak = 0.0;
		for (long channel = 1; channel <= my ny; channel ++) {
			real80 sum = 0.0;
			for (long i = 1; i <= my nx; i ++) {
				sum += my z [channel] [i];
			}
			real mean = real (sum / my nx);
			for (long i = 1; i <= my nx; i ++) {
				double value = fabs (my z [channel] [i] - mean);
				if (value > globalPeak) globalPeak = value;
			}
		}
		if (globalPeak == 0.0) {
			return thee;
		}

		autoNUMvector <double> window;
		autoNUMvector <double> windowR;
		if (method >= FCC_NORMAL) {   /* For cross-correlation analysis. */

			nsampFFT = 0;
			brent_ixmax = (long) floor (nsamp_window * interpolation_depth);

		} else {   /* For autocorrelation analysis. */

			/*
			* Compute the number of samples needed for doing FFT.
			* To avoid edge effects, we have to append zeroes to the window.
			* The maximum lag considered for maxima is maximumLag.
			* The maximum lag used in interpolation is nsamp_window * interpolation_depth.
			*/
			nsampFFT = 1;
			while (nsampFFT < nsamp_window * (1 + interpolation_depth)) {
				nsampFFT *= 2;
			}

			/*
			* Create buffers for autocorrelation analysis.
			*/
			windowR.reset (1, nsampFFT);
			window.reset (1, nsamp_window);
			NUMfft_Table_init (& fftTable, nsampFFT);

			/*
			* A Gaussian or Hanning window is applied against phase effects.
			* The Hanning window is 2 to 5 dB better for 3 periods/window.
			* The Gaussian window is 25 to 29 dB better for 6 periods/window.
			*/
			if (method == AC_GAUSS) {   /* Gaussian window. */
				double imid = 0.5 * (nsamp_window + 1), edge = exp (-12.0);
				for (long i = 1; i <= nsamp_window; i ++) {
					window [i] = (exp (-48.0 * (i - imid) * (i - imid) /
						(nsamp_window + 1) / (nsamp_window + 1)) - edge) / (1.0 - edge);
				}
			} else {   // Hanning window
				for (long i = 1; i <= nsamp_window; i ++) {
					window [i] = 0.5 - 0.5 * cos (i * 2 * NUMpi / (nsamp_window + 1));
				}
			}

			/*
			* Compute the normalized autocorrelation of the window.
			*/
			for (long i = 1; i <= nsamp_window; i ++) {
				windowR [i] = window [i];
			}
			NUMfft_forward (& fftTable, windowR.peek());
			windowR [1] *= windowR [1];   // DC component
			for (long i = 2; i < nsampFFT; i += 2) {
				windowR [i] = windowR [i] * windowR [i] + windowR [i + 1] * windowR [i + 1];
				windowR [i + 1] = 0.0;   // power spectrum: square and zero
			}
			windowR [nsampFFT] *= windowR [nsampFFT];   // Nyquist frequency
			NUMfft_backward (& fftTable, windowR.peek());   // autocorrelation
			for (long i = 2; i <= nsamp_window; i ++) {
				windowR [i] /= windowR [1];   // normalize
			}
			windowR [1] = 1.0;   // normalize

			brent_ixmax = (long) floor (nsamp_window * interpolation_depth);
		}

		autoMelderProgress progress (U"Sound to Pitch...");

		long numberOfFramesPerThread = 20;
		int numberOfThreads = (nFrames - 1) / numberOfFramesPerThread + 1;
		const int numberOfProcessors = MelderThread_getNumberOfProcessors ();
		trace (numberOfProcessors, U" processors");
		if (numberOfThreads > numberOfProcessors) numberOfThreads = numberOfProcessors;
		if (numberOfThreads > 16) numberOfThreads = 16;
		if (numberOfThreads < 1) numberOfThreads = 1;
		numberOfFramesPerThread = (nFrames - 1) / numberOfThreads + 1;

		if (! mutex_inited) { MelderThread_MUTEX_INIT (mutex); mutex_inited = true; }
		autoSound_into_Pitch_Args args [16];
		long firstFrame = 1, lastFrame = numberOfFramesPerThread;
		volatile int cancelled = 0;
		for (int ithread = 1; ithread <= numberOfThreads; ithread ++) {
			if (ithread == numberOfThreads) lastFrame = nFrames;
			args [ithread - 1] = Sound_into_Pitch_Args_create (me, thee.get(),
				firstFrame, lastFrame, minimumPitch, maxnCandidates, method,
				voicingThreshold, octaveCost,
				dt_window, nsamp_window, halfnsamp_window, maximumLag,
				nsampFFT, nsamp_period, halfnsamp_period, brent_ixmax, brent_depth,
				globalPeak, window.peek(), windowR.peek(),
				ithread == numberOfThreads, & cancelled);
			firstFrame = lastFrame + 1;
			lastFrame += numberOfFramesPerThread;
		}
		MelderThread_run (Sound_into_Pitch, args, numberOfThreads);

		Melder_progress (0.95, U"Sound to Pitch: path finder");
		Pitch_pathFinder (thee.get(), silenceThreshold, voicingThreshold,
			octaveCost, octaveJumpCost, voicedUnvoicedCost, ceiling, Melder_debug == 31 ? true : false);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": pitch analysis not performed.");
	}
}

autoPitch Sound_to_Pitch (Sound me, double timeStep, double minimumPitch, double maximumPitch) {
	return Sound_to_Pitch_ac (me, timeStep, minimumPitch,
		3.0, 15, false, 0.03, 0.45, 0.01, 0.35, 0.14, maximumPitch);
}

autoPitch Sound_to_Pitch_ac (Sound me,
	double dt, double minimumPitch, double periodsPerWindow, int maxnCandidates, int accurate,
	double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost, double ceiling)
{
	return Sound_to_Pitch_any (me, dt, minimumPitch, periodsPerWindow, maxnCandidates, accurate,
		silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost, ceiling);
}

autoPitch Sound_to_Pitch_cc (Sound me,
	double dt, double minimumPitch, double periodsPerWindow, int maxnCandidates, int accurate,
	double silenceThreshold, double voicingThreshold,
	double octaveCost, double octaveJumpCost, double voicedUnvoicedCost, double ceiling)
{
	return Sound_to_Pitch_any (me, dt, minimumPitch, periodsPerWindow, maxnCandidates, 2 + accurate,
		silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost, ceiling);
}

/* End of file Sound_to_Pitch.cpp */
