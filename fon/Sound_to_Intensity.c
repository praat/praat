/* Sound_to_Intensity.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2003/05/20 default time step is four times oversampling
 * pb 2003/07/10 NUMbessel_i0_f
 * pb 2003/11/19 Sound_to_Intensity veryAccurate
 * pb 2003/12/15 removed bug introduced by previous change
 * pb 2004/10/27 subtractMean
 * pb 2006/12/31 compatible with stereo sounds
 * pb 2007/01/27 for stereo sounds, add channel energies
 */

#include "Sound_to_Intensity.h"

Intensity Sound_to_Intensity (Sound me, double minimumPitch, double timeStep, int subtractMeanPressure) {
	int veryAccurate = FALSE;
	Sound save_me = me;
	Intensity smooth = NULL;
	long i, iframe, numberOfFrames;
	double windowDuration = 6.4 / minimumPitch, thyFirstTime;
	double halfWindowDuration = 0.5 * windowDuration;
	long halfWindowSamples = halfWindowDuration / my dx;
	float *amplitude = NUMfvector (- halfWindowSamples, halfWindowSamples);
	float *window = NUMfvector (- halfWindowSamples, halfWindowSamples); cherror

	for (i = - halfWindowSamples; i <= halfWindowSamples; i ++) {
		double x = i * my dx / halfWindowDuration, root = 1 - x * x;
		window [i] = root <= 0.0 ? 0.0 : NUMbessel_i0_f ((2 * NUMpi * NUMpi + 0.5) * sqrt (root));
	}

	/* Step 1: upsample by a factor of two. */
	if (veryAccurate) {
		me = Sound_upsample (me);   /* Because frequency content will be doubled in the next step. */
		cherror
	}

	/* Step 2: smooth and resample. */
	Melder_assert (minimumPitch > 0.0);
	if (timeStep <= 0.0) timeStep = 0.8 / minimumPitch;   /* Default: four times oversampling Hanning-wise. */
	if (! Sampled_shortTermAnalysis (me, windowDuration, timeStep, & numberOfFrames, & thyFirstTime)) {
		Melder_error ("Sound should be at least 6.4 / minimumPitch long.");
		goto end;
	}
	smooth = Intensity_create (my xmin, my xmax, numberOfFrames, timeStep, thyFirstTime); cherror
	for (iframe = 1; iframe <= numberOfFrames; iframe ++) {
		double midTime = Sampled_indexToX (smooth, iframe);
		long midSample = Sampled_xToNearestIndex (me, midTime);
		long leftSample = midSample - halfWindowSamples, rightSample = midSample + halfWindowSamples;
		double sumxw = 0.0, sumw = 0.0, intensity;
		if (leftSample < 1) leftSample = 1;
		if (rightSample > my nx) rightSample = my nx;

		for (long channel = 1; channel <= my ny; channel ++) {
			for (i = leftSample; i <= rightSample; i ++) {
				amplitude [i - midSample] = my z [channel] [i];
			}
			if (subtractMeanPressure) {
				double sum = 0.0;
				for (i = leftSample; i <= rightSample; i ++) {
					sum += amplitude [i - midSample];
				}
				double mean = sum / (rightSample - leftSample + 1);
				for (i = leftSample; i <= rightSample; i ++) {
					amplitude [i - midSample] -= mean;
				}
			}
			for (i = leftSample; i <= rightSample; i ++) {
				sumxw += amplitude [i - midSample] * amplitude [i - midSample] * window [i - midSample];
				sumw += window [i - midSample];
			}
		}
		intensity = sumxw / sumw;
		if (intensity != 0.0) intensity /= 4e-10;
		smooth -> z [1] [iframe] = intensity < 1e-30 ? -300 : 10 * log10 (intensity);
	}

	/* Clean up and return. */
end:
	if (veryAccurate) {
		forget (me);
		me = save_me;
	}
	NUMfvector_free (amplitude, - halfWindowSamples);
	NUMfvector_free (window, - halfWindowSamples);
	iferror return Melder_errorp ("Intensity analysis not performed.");
	return smooth;
}

IntensityTier Sound_to_IntensityTier (Sound me, double minimumPitch, double timeStep, int subtractMean) {
	Intensity intensity = Sound_to_Intensity (me, minimumPitch, timeStep, subtractMean);
	IntensityTier thee;
	if (! intensity) return NULL;
	thee = Intensity_downto_IntensityTier (intensity);
	forget (intensity);
	return thee;
}

/* End of file Sound_to_Intensity.c */
