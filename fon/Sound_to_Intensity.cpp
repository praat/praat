/* Sound_to_Intensity.cpp
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
 * pb 2003/05/20 default time step is four times oversampling
 * pb 2003/07/10 NUMbessel_i0_f
 * pb 2003/11/19 Sound_to_Intensity veryAccurate
 * pb 2003/12/15 removed bug introduced by previous change
 * pb 2004/10/27 subtractMean
 * pb 2006/12/31 compatible with stereo sounds
 * pb 2007/01/27 for stereo sounds, add channel energies
 * pb 2007/02/14 honoured precondition of Sampled_shortTermAnalysis (by checking whether minimumPitch is defined)
 * pb 2008/01/19 double
 * pb 2011/03/04 C++
 * pb 2011/03/28 C++
 */

#include "Sound_to_Intensity.h"

static autoIntensity Sound_to_Intensity_ (Sound me, double minimumPitch, double timeStep, bool subtractMeanPressure) {
	try {
		/*
		 * Preconditions.
		 */
		if (isundef (minimumPitch)) Melder_throw (U"(Sound-to-Intensity:) Minimum pitch undefined.");
		if (isundef (timeStep)) Melder_throw (U"(Sound-to-Intensity:) Time step undefined.");
		if (timeStep < 0.0) Melder_throw (U"(Sound-to-Intensity:) Time step should be zero or positive instead of ", timeStep, U".");
		if (my dx <= 0.0) Melder_throw (U"(Sound-to-Intensity:) The Sound's time step should be positive.");
		if (minimumPitch <= 0.0) Melder_throw (U"(Sound-to-Intensity:) Minimum pitch should be positive.");
		/*
		 * Defaults.
		 */
		if (timeStep == 0.0) timeStep = 0.8 / minimumPitch;   // default: four times oversampling Hanning-wise

		const double windowDuration = 6.4 / minimumPitch;
		Melder_assert (windowDuration > 0.0);
		const double halfWindowDuration = 0.5 * windowDuration;
		const integer halfWindowSamples = Melder_ifloor (halfWindowDuration / my dx);
		autoNUMvector <double> amplitude (- halfWindowSamples, halfWindowSamples);
		autoNUMvector <double> window (- halfWindowSamples, halfWindowSamples);

		for (integer i = - halfWindowSamples; i <= halfWindowSamples; i ++) {
			const double x = i * my dx / halfWindowDuration, root = 1 - x * x;
			window [i] = root <= 0.0 ? 0.0 : NUMbessel_i0_f ((2.0 * NUMpi * NUMpi + 0.5) * sqrt (root));
		}

		integer numberOfFrames;
		double thyFirstTime;
		try {
			Sampled_shortTermAnalysis (me, windowDuration, timeStep, & numberOfFrames, & thyFirstTime);
		} catch (MelderError) {
			Melder_throw (U"The physical duration of the sound (the number of samples times the sampling period) in an intensity analysis "
				"should be at least 6.4 divided by the minimum pitch (", minimumPitch, U" Hz), "
				U"i.e. at least ", 6.4 / minimumPitch, U" s, instead of ", my nx * my dx, U" s.");
		}
		autoIntensity thee = Intensity_create (my xmin, my xmax, numberOfFrames, timeStep, thyFirstTime);
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double midTime = Sampled_indexToX (thee.get(), iframe);
			const integer midSample = Sampled_xToNearestIndex (me, midTime);   // time accuracy is half a sampling period
			integer leftSample = midSample - halfWindowSamples, rightSample = midSample + halfWindowSamples;
			real80 sumxw = 0.0, sumw = 0.0;
			if (leftSample < 1) leftSample = 1;
			if (rightSample > my nx) rightSample = my nx;

			for (integer channel = 1; channel <= my ny; channel ++) {
				for (integer i = leftSample; i <= rightSample; i ++) {
					amplitude [i - midSample] = my z [channel] [i];
				}
				if (subtractMeanPressure) {
					real80 sum = 0.0;
					for (integer i = leftSample; i <= rightSample; i ++) {
						sum += amplitude [i - midSample];
					}
					double mean = (double) sum / (rightSample - leftSample + 1);
					for (integer i = leftSample; i <= rightSample; i ++) {
						amplitude [i - midSample] -= mean;
					}
				}
				for (integer i = leftSample; i <= rightSample; i ++) {
					sumxw += amplitude [i - midSample] * amplitude [i - midSample] * window [i - midSample];
					sumw += window [i - midSample];
				}
			}
			double intensity = double (sumxw / sumw);
			intensity /= 4.0e-10;
			thy z [1] [iframe] = intensity < 1.0e-30 ? -300.0 : 10.0 * log10 (intensity);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": intensity analysis not performed.");
	}
}

autoIntensity Sound_to_Intensity (Sound me, double minimumPitch, double timeStep, bool subtractMeanPressure) {
	const bool veryAccurate = false;
	if (veryAccurate) {
		autoSound up = Sound_upsample (me);   // because squaring doubles the frequency content, i.e. you get super-Nyquist components
		return Sound_to_Intensity_ (up.get(), minimumPitch, timeStep, subtractMeanPressure);
	} else {
		return Sound_to_Intensity_ (me, minimumPitch, timeStep, subtractMeanPressure);
	}
}

autoIntensityTier Sound_to_IntensityTier (Sound me, double minimumPitch, double timeStep, bool subtractMean) {
	try {
		autoIntensity intensity = Sound_to_Intensity (me, minimumPitch, timeStep, subtractMean);
		return Intensity_downto_IntensityTier (intensity.get());
	} catch (MelderError) {
		Melder_throw (me, U": no IntensityTier created.");
	}
}

/* End of file Sound_to_Intensity.cpp */
