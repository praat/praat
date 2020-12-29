/* Sound_to_Intensity.cpp
 *
 * Copyright (C) 1992-2012,2014-2020 Paul Boersma
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
			Preconditions.
		*/
		Melder_require (isdefined (minimumPitch),
			U"Minimum pitch is undefined.");
		Melder_require (isdefined (timeStep),
			U"Time step is undefined.");
		Melder_require (timeStep >= 0.0,
			U"Time step should be zero (= automatic) or positive, instead of ", timeStep, U" seconds.");
		Melder_require (my dx > 0.0,
			U"The Sound's time step should be positive, instead of ", my dx, U" seconds.");
		Melder_require (minimumPitch > 0.0,
			U"Minimum pitch should be positive, instead of ", minimumPitch, U" Hz.");
		/*
			Defaults.
		*/
		constexpr double minimumNumberOfPeriodsNeededForReliablePitchMeasurement = 3.2;
		const double maximumPeriod = 1.0 / minimumPitch;
		const double logicalWindowDuration = minimumNumberOfPeriodsNeededForReliablePitchMeasurement * maximumPeriod;   // == 3.2 / minimumPitch
		if (timeStep == 0.0) {
			constexpr double defaultOversampling = 4.0;
			timeStep = logicalWindowDuration / defaultOversampling;   // == 0.8 / minimumPitch
		}

		const double physicalWindowDuration = 2.0 * logicalWindowDuration;   // == 6.4 / minimumPitch
		Melder_assert (physicalWindowDuration > 0.0);
		const double halfWindowDuration = 0.5 * physicalWindowDuration;
		const integer halfWindowSamples = Melder_ifloor (halfWindowDuration / my dx);
		const integer windowNumberOfSamples = 2 * halfWindowSamples + 1;
		autoVEC amplitude = zero_VEC (windowNumberOfSamples);
		autoVEC window = zero_VEC (windowNumberOfSamples);
		const integer windowCentreSampleNumber = halfWindowSamples + 1;

		for (integer i = 1; i <= windowNumberOfSamples; i ++) {
			const double x = (i - windowCentreSampleNumber) * my dx / halfWindowDuration;
			const double root = sqrt (Melder_clippedLeft (0.0, 1.0 - sqr (x)));   // clipping should be rare
			window [i] = NUMbessel_i0_f ((2.0 * NUMpi * NUMpi + 0.5) * root);
		}

		integer numberOfFrames;
		double thyFirstTime;
		try {
			Sampled_shortTermAnalysis (me, physicalWindowDuration, timeStep, & numberOfFrames, & thyFirstTime);
		} catch (MelderError) {
			const double physicalSoundDuration = my nx * my dx;
			Melder_throw (U"The physical duration of the sound (the number of samples times the sampling period) in an intensity analysis "
				"should be at least 6.4 divided by the minimum pitch (", minimumPitch, U" Hz), "
				U"i.e. at least ", physicalWindowDuration, U" s, instead of ", physicalSoundDuration, U" s.");
		}
		autoIntensity thee = Intensity_create (my xmin, my xmax, numberOfFrames, timeStep, thyFirstTime);
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double midTime = Sampled_indexToX (thee.get(), iframe);
			const integer soundCentreSampleNumber = Sampled_xToNearestIndex (me, midTime);   // time accuracy is half a sampling period

			integer leftSample = soundCentreSampleNumber - halfWindowSamples;
			integer rightSample = soundCentreSampleNumber + halfWindowSamples;
			/*
				Catch some edge cases, which are uncommon because Sampled_shortTermAnalysis() filtered out most problems.
			*/
			Melder_clipLeft (1_integer, & leftSample);
			Melder_clipRight (& rightSample, my nx);
			Melder_require (rightSample >= leftSample,
				U"Unexpected edge case: right sample (", rightSample, U") less than left sample (", leftSample, U").");

			const integer windowFromSoundOffset = windowCentreSampleNumber - soundCentreSampleNumber;
			VEC amplitudePart = amplitude.part (windowFromSoundOffset + leftSample, windowFromSoundOffset + rightSample);
			constVEC windowPart = window.part (windowFromSoundOffset + leftSample, windowFromSoundOffset + rightSample);
			longdouble sumxw = 0.0, sumw = 0.0;
			for (integer ichan = 1; ichan <= my ny; ichan ++) {
				amplitudePart  <<=  my z [ichan].part (leftSample, rightSample);
				if (subtractMeanPressure)
					centre_VEC_inout (amplitudePart);
				for (integer isamp = 1; isamp <= amplitudePart.size; isamp ++) {
					sumxw += sqr (amplitudePart [isamp]) * windowPart [isamp];
					sumw += windowPart [isamp];
				}
			}
			const double intensity_in_Pa2 = double (sumxw / sumw);
			constexpr double hearingThreshold_in_Pa = 2.0e-5;
			constexpr double hearingThreshold_in_Pa2 = sqr (hearingThreshold_in_Pa);
			const double intensity_re_hearingThreshold = intensity_in_Pa2 / hearingThreshold_in_Pa2;
			const double intensity_in_dB_re_hearingThreshold = ( intensity_re_hearingThreshold < 1.0e-30 ? -300.0 :
					10.0 * log10 (intensity_re_hearingThreshold) );
			thy z [1] [iframe] = intensity_in_dB_re_hearingThreshold;
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
