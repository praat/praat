/* Electroglottogram.cpp
 *
 * Copyright (C) 2019 David Weenink
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

#include "AmplitudeTier.h"
#include "Electroglottogram.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"
#include "Sound_to_PointProcess.h"

#include "enums_getText.h"
#include "Electroglottogram_enums.h"
#include "enums_getValue.h"
#include "Electroglottogram_enums.h"

Thing_implement (Electroglottogram, Vector, 2);

void IntervalTier_insertBoundary (IntervalTier me, double t) {
	try {
		if (IntervalTier_hasTime (me, t))
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (t, 6), U" seconds, because there is already a boundary there.");
		integer intervalNumber = IntervalTier_timeToIndex (me, t);
		if (intervalNumber == 0)
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (t, 6), U" seconds, because this is outside the time domain of the intervals.");
		TextInterval interval = my intervals.at [intervalNumber];
		/*
			Move the text to the left of the boundary.
		*/
		autoTextInterval newInterval = TextInterval_create (t, interval -> xmax, U"");
		interval -> xmax = t;
		my intervals. addItem_move (newInterval.move());
	} catch (MelderError) {
		Melder_throw (me, U": boundary not inserted.");
	}
}

void IntervalTier_setIntervalText (IntervalTier me, integer intervalNumber, conststring32 text) {
	try {
		if (intervalNumber < 1 || intervalNumber > my intervals.size)
			Melder_throw (U"Interval ", intervalNumber, U" does not exist.");
		TextInterval interval = my intervals.at [intervalNumber];
		TextInterval_setText (interval, text);
	} catch (MelderError) {
		Melder_throw (me, U": interval text not set.");
	}
}

void structElectroglottogram :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", our xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", our xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", our xmax - our xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of samples: ", our nx);
	MelderInfo_writeLine (U"   Sampling period: ", our dx, U" seconds");
	MelderInfo_writeLine (U"   Sampling frequency: ", Melder_single (1.0 / our dx), U" Hz");
	MelderInfo_writeLine (U"   First sample centred at: ", our x1, U" seconds");
}

autoElectroglottogram Electroglottogram_create (double xmin, double xmax, integer nx, double dx, double x1) {
	try {
		autoElectroglottogram me = Thing_new (Electroglottogram);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, 1, 1, 1, 1, 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Electroglottogram not created.");
	}
}

autoElectroglottogram Sound_extractElectroglottogram (Sound me, integer channelNumber) {
	try {
		Melder_require (channelNumber >= 1 && channelNumber <= my ny,
			U"There is no channel ", channelNumber, U".");
		autoElectroglottogram thee = Electroglottogram_create (my xmin, my xmax, my nx, my dx, my x1);
		thy z.row (1) <<= my z.row (channelNumber);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": channel ", channelNumber, U" not extracted.");
	}
}

autoAmplitudeTier Electroglottogram_to_AmplitudeTier_levels (Electroglottogram me, double pitchFloor, double pitchCeiling, double closingThreshold, autoAmplitudeTier *out_peaks, autoAmplitudeTier *out_valleys) {
	try {
		autoPointProcess peakPositions = Sound_to_PointProcess_periodic_peaks ((Sound) me, pitchFloor, pitchCeiling, true, false);
		autoPointProcess valleyPositions = Sound_to_PointProcess_periodic_peaks ((Sound) me, pitchFloor, pitchCeiling, false, true);
		/*
			Get the values of the peaks and valleys
		*/
		autoAmplitudeTier peaks = PointProcess_Sound_to_AmplitudeTier_point (peakPositions.get(), (Sound) me);
		autoAmplitudeTier valleys = PointProcess_Sound_to_AmplitudeTier_point (valleyPositions.get(), (Sound) me);
		autoAmplitudeTier thee = Electroglottogram_and_AmplitudeTiers_getLevels (me, peaks.get(), valleys.get(), closingThreshold);
		if (out_peaks)
			*out_peaks = peaks.move();
		if (out_valleys)
			*out_valleys = valleys.move();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": AmplitudeTier not created.");
	}
}

//autoTextTier Electroglottogram_to_TextTier (Electroglottogram me, double pitchFloor, double PitchCeiling, double closingThreshold, kElectroglottogram_findClosedIntervalMethod method);

autoAmplitudeTier Electroglottogram_and_AmplitudeTiers_getLevels (Electroglottogram me, AmplitudeTier peaks, AmplitudeTier valleys, double closingThreshold) {
		try {
			Melder_require (my xmin == peaks -> xmin && my xmax == peaks -> xmax,
				U"The domain of the Electroglottogram and the peaks should be equal.");
			Melder_require (my xmin == valleys -> xmin && my xmax == valleys -> xmax,
				U"The domain of the Electroglottogram and the peaks should be equal.");
			Melder_require (peaks -> points. size > 1 && valleys -> points. size > 1,
				U"The AmplitudeTiers cannot be empty.");
			autoAmplitudeTier thee = AmplitudeTier_create (my xmin, my xmax);
			double peakAmplitudeLeft = RealTier_getValueAtIndex (peaks, 1);
			double peakTimeLeft = peaks -> points.at [1] -> number;
			for (integer ipoint = 2; ipoint <= peaks -> points. size - 1; ipoint ++) {
				double peakAmplitudeRight = RealTier_getValueAtIndex (peaks, ipoint);
				double peakTimeRight = peaks -> points.at [ipoint] -> number;
				integer indexValley = AnyTier_timeToNearestIndex ((AnyTier) valleys, peakTimeRight);
				double timeValley = valleys -> points.at [indexValley] -> number;
				if (timeValley > peakTimeLeft && timeValley < peakTimeRight) {
					double valleyAmplitude = RealTier_getValueAtIndex (valleys, indexValley);
					double amplitudeRange = peakAmplitudeLeft - valleyAmplitude;
					double level = valleyAmplitude + amplitudeRange * closingThreshold;
					RealTier_addPoint (thee.get(), peakTimeLeft, level);
				}
				peakAmplitudeLeft = peakAmplitudeRight;
				peakTimeLeft = peakTimeRight;
			}
			return thee;
		} catch (MelderError) {
			Melder_throw (me, U": Levels not determined.");
		}
}

autoIntervalTier Electroglottogram_to_TextTier_peaks (Electroglottogram me, double pitchFloor, double pitchCeiling, double closingThreshold, double silenceThreshold) {
	try {
		bool maximumIsClosedGlottis = false; // because we might multiply the Electroglottogram by -1
		
		autoAmplitudeTier peaks, valleys;
		autoAmplitudeTier levels = Electroglottogram_to_AmplitudeTier_levels (me, pitchFloor, pitchCeiling, closingThreshold, & peaks,  & valleys);
		
		double minimum = RealTier_getMinimumValue (valleys.get());
		double maximum = RealTier_getMaximumValue (peaks.get());
		double minimumPeakAmplitude = maximum * silenceThreshold;
		maximumIsClosedGlottis = maximum > minimum;

		autoIntervalTier intervalTier = IntervalTier_create (my xmin, my xmax);
		double previousOpeningTime = my xmin;
		for (integer ipoint = 1; ipoint <= peaks -> points. size; ipoint ++) {
			RealPoint peak = peaks -> points.at [ipoint];
			double peakPosition = peak -> number;
			double peakAmplitude = peak -> value;
			double closingTime = undefined, openingTime = undefined;
			if (peakAmplitude > minimumPeakAmplitude) {
				double level = RealTier_getValueAtTime (levels.get(), peakPosition);
				closingTime = Sound_getNearestLevelCrossing ((Sound) me, 1, peakPosition, level, kSoundSearchDirection::Left);
				openingTime = Sound_getNearestLevelCrossing ((Sound) me, 1, peakPosition, level, kSoundSearchDirection::Right);
				if (isdefined (closingTime) && isdefined (openingTime) && closingTime != previousOpeningTime) {
					IntervalTier_insertBoundary (intervalTier.get(), closingTime);
					IntervalTier_insertBoundary (intervalTier.get(), openingTime);
					double midPoint = 0.5 * (closingTime + openingTime);
					integer intervalNumber = IntervalTier_timeToIndex (intervalTier.get(), midPoint);
					IntervalTier_setIntervalText (intervalTier.get(), intervalNumber, U"c");
					previousOpeningTime = openingTime;
				}
			}
		}
		
		return intervalTier;
		
	} catch (MelderError) {
		Melder_throw (me, U": TextTier not created.");
	}
	
}

autoElectroglottogram Electroglottogram_derivative (Electroglottogram me, double lowPassFrequency, double smoothing) {
		try {
			autoSpectrum thee = Sound_to_Spectrum ((Sound) me, false);
			for (integer ifreq = 1; ifreq <= thy nx; ifreq ++) {
				double f = Sampled_indexToX (thee.get(), ifreq);
				thy z [1] [ifreq] *= f;
				thy z [2] [ifreq] *= f;
			}
			Spectrum_passHannBand (thee.get(), - smoothing, lowPassFrequency, smoothing);
			autoSound sound = Spectrum_to_Sound (thee.get());
			Vector_scale (sound.get(), 0.99);
			autoElectroglottogram him = Electroglottogram_create (sound -> xmin, sound -> xmax, sound -> nx, sound -> dx, sound -> x1);
			his z.get() <<= sound -> z.get();
			return him;
		} catch (MelderError) {
			Melder_throw (me, U": cannot create derivative of Electroglottogram.");
		}
}

// Very simple and strict
autoSound Sound_Electroglottograms_combine (Sound me, OrderedOf<structElectroglottogram>* thee) {
	try {
		integer numberOfChannels = my ny + thy size;
		for (integer iegg = 1; iegg <= thy size; iegg ++) {
			Electroglottogram egg = thy at [iegg];
			Melder_require (egg -> xmin == my xmin && egg -> xmax == my xmax,
				U"The Electroglottograms should have the same domain as the Sound");
			Melder_require (egg -> nx == my nx, 
				U"The Electroglottograms should have the same number of samples as the Sound.");
			Melder_require (egg -> dx == my dx,
				U"The Electroglottograms should have the same sampling frequency as the Sound.");
		}
		autoSound him = Sound_create (numberOfChannels,my xmin, my xmax, my nx, my dx, my x1);
		for (integer ichan = 1; ichan <= my ny; ichan ++)
			his z.row (ichan) <<= my z.row(ichan);
	
		for (integer ichan = my ny + 1; ichan <= numberOfChannels; ichan ++) {
			Electroglottogram egg = thy at [ichan - my ny];
			his z.row (ichan) <<= egg -> z.row (1);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, thee, U" not combined.");
	}
}

/* End of file Electroglottogram.cpp */
