/* EGG.cpp
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
#include "EGG.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"
#include "Sound_to_PointProcess.h"

#include "enums_getText.h"
#include "EGG_enums.h"
#include "enums_getValue.h"
#include "EGG_enums.h"

Thing_implement (EGG, Vector, 2);

void structEGG :: v_info () {
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

autoEGG EGG_create (double xmin, double xmax, integer nx, double dx, double x1) {
	try {
		autoEGG me = Thing_new (EGG);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, 1, 1, 1, 1, 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"EGG not created.");
	}
}

autoEGG Sound_extractEGG (Sound me, integer channelNumber) {
	try {
		Melder_require (channelNumber >= 1 && channelNumber <= my ny,
			U"There is no channel ", channelNumber, U".");
		autoEGG thee = EGG_create (my xmin, my xmax, my nx, my dx, my x1);
		thy z.row (1) <<= my z.row (channelNumber);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": channel ", channelNumber, U" not extracted.");
	}
}

Thing_implement (DEGG, Vector, 2);

void structDEGG :: v_info () {
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


autoDEGG DEGG_create (double xmin, double xmax, integer nx, double dx, double x1) {
	try {
		autoDEGG me = Thing_new (DEGG);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, 1, 1, 1, 1, 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"EGG not created.");
	}
}

//autoTextTier EGG_to_TextTier (EGG me, double pitchFloor, double PitchCeiling, double closingThreshold, kEGG_findClosedIntervalMethod method);

autoAmplitudeTier EGG_and_AmplitudeTiers_getLevels (EGG me, AmplitudeTier peaks, AmplitudeTier valleys, double closingThreshold) {
		try {
			Melder_require (my xmin == peaks -> xmin && my xmax == peaks -> xmax,
				U"The domain of the EGG and the peaks should be equal.");
			Melder_require (my xmin == valleys -> xmin && my xmax == valleys -> xmax,
				U"The domain of the EGG and the peaks should be equal.");
			Melder_require (peaks -> points. size > 1 && valleys -> points. size > 1,
				U"The AmplitudeTiers cannot be empty.");
			autoAmplitudeTier thee = AmplitudeTier_create (my xmin, my xmax);
			double peakAmplitudeLeft = RealTier_getValueAtIndex (peaks, 1);
			double peakTimeLeft = peaks -> points.at [1] -> number;
			for (integer ipoint = 2; ipoint <= peaks -> points. size - 1; ipoint ++) {
				double peakAmplitudeRight = RealTier_getValueAtIndex (peaks, ipoint);
				double timePeakRight = peaks -> points.at [ipoint] -> number;
				integer index = AnyTier_timeToNearestIndex ((AnyTier) valleys, timePeakRight);
				double timeValley = valleys -> points.at [index] -> number;
				if (timeValley > peakTimeLeft && timeValley < timePeakRight) {
					double valleyAmplitude = RealTier_getValueAtIndex (valleys, index);
					double amplitudeRange = peakAmplitudeLeft - valleyAmplitude;
					double level = valleyAmplitude + amplitudeRange * closingThreshold;
					RealTier_addPoint (thee.get(), peakTimeLeft, level);
				}
			}
			return thee;
		} catch (MelderError) {
			Melder_throw (me, U": Levels not determined.");
		}
}

autoIntervalTier EGG_to_TextTier_peaks (EGG me, double pitchFloor, double pitchCeiling, double closingThreshold, double silenceThreshold) {
	try {
		bool maximumIsClosedGlottis = false; // because we might multiply the EGG by -1
		autoPointProcess peakPositions = Sound_to_PointProcess_periodic_peaks ((Sound) me, pitchFloor, pitchCeiling, true, false);
		autoPointProcess valleyPositions = Sound_to_PointProcess_periodic_peaks ((Sound) me, pitchFloor, pitchCeiling, false, true);
		/*
			Get the values of the peaks and valleys
		*/
		autoAmplitudeTier peaks = PointProcess_Sound_to_AmplitudeTier_point (peakPositions.get(), (Sound) me);
		autoAmplitudeTier valleys = PointProcess_Sound_to_AmplitudeTier_point (valleyPositions.get(), (Sound) me);
		double minimum = RealTier_getMinimumValue (valleys.get());
		double maximum = RealTier_getMaximumValue (peaks.get());
		double minimumPeakAmplitude = maximum * silenceThreshold;
		maximumIsClosedGlottis = maximum > minimum;
		autoAmplitudeTier levels = EGG_and_AmplitudeTiers_getLevels (me, peaks.get(), valleys.get(), closingThreshold);
		autoIntervalTier intervalTier = IntervalTier_create (my xmin, my xmax);
		double previousOpeningTime = my xmin;
		bool closing = false, opening = false;
		for (integer ipoint = 1; ipoint <= peaks -> points. size; ipoint ++) {
			RealPoint peak = peaks -> points.at [ipoint];
			double peakPosition = peak -> number;
			double peakAmplitude = peak -> value;
			double closingTime = undefined, openingTime = undefined;
			if (peakAmplitude > minimumPeakAmplitude) {
				double level = RealTier_getValueAtIndex (levels.get(), peakPosition);
				closingTime = Sound_getNearestLevelCrossing ((Sound) me, 1, peakPosition, level, kSoundSearchDirection::Left);
				openingTime = Sound_getNearestLevelCrossing ((Sound) me, 1, peakPosition, level, kSoundSearchDirection::Right);
				if (isdefined (closingTime) && isdefined (openingTime) && closingTime != previousOpeningTime) {
					integer intervalNumberc = IntervalTier_timeToIndex (intervalTier.get(), closingTime);
					TextInterval intervalc = intervalTier -> intervals.at [intervalNumberc];
					autoTextInterval newIntervalc = TextInterval_create (closingTime, intervalc -> xmax, U"");
					intervalc -> xmax = closingTime;
					intervalTier -> intervals. addItem_move (newIntervalc.move());
					integer intervalNumbero = IntervalTier_timeToIndex (intervalTier.get(), openingTime);
					TextInterval intervalo = intervalTier -> intervals.at [intervalNumbero];
					autoTextInterval newIntervalo = TextInterval_create (openingTime, intervalo -> xmax, U"c");
					intervalo -> xmax = openingTime;
					intervalTier -> intervals. addItem_move (newIntervalo.move());
					previousOpeningTime = openingTime;
					Melder_assert (intervalNumbero == intervalNumberc + 1);
				}
			}
		}
		
		return intervalTier;
		
	} catch (MelderError) {
		Melder_throw (me, U": TextTier not created.");
	}
	
}

autoIntervalTier EGG_and_DEGG_to_TextTier (EGG me, DEGG thee, double pitchFloor, double PitchCeiling, double closingThreshold) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"The domains should be equal.");
		autoIntervalTier intervalTier = IntervalTier_create (my xmin, thy xmin);
		return intervalTier;
	} catch (MelderError) {
		Melder_throw (me, thee, U"No IntervalTier created.");
	}
}

autoDEGG EGG_to_DEGG (EGG me, double fromFrequency, double toFrequency, double smoothing) {
		try {
			autoSpectrum thee = Sound_to_Spectrum ((Sound) me, false);
			for (integer ifreq = 1; ifreq <= thy nx; ifreq ++) {
				double f = Sampled_indexToX (thee.get(), ifreq);
				thy z [1] [ifreq] *= f;
				thy z [2] [ifreq] *= f;
			}
			Spectrum_passHannBand (thee.get(), fromFrequency, toFrequency, smoothing);
			autoSound sound = Spectrum_to_Sound (thee.get());
			Vector_scale (sound.get(), 0.99);
			autoDEGG him = DEGG_create (sound -> xmin, sound -> xmax, sound -> nx, sound -> dx, sound -> x1);
			his z.get() <<= sound -> z.get();
			return him;
		} catch (MelderError) {
			Melder_throw (me, U": cannot create DEGG.");
		}
}
