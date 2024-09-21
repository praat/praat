/* praat_sensors_init.cpp
 *
 * Copyright (C) 2023 David Weenink
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

#include <string.h>
#include "Electroglottogram.h"

#include "EMA.h"
#include "EMArawData.h"
#include "praat_sensors_init.h"
#include "Sound_extensions.h"

static autoDaata CarstensEMA50xV3PosFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread > 69 && strnequ (header, "AG50xDATA_V00", 13) &&
		(strstr (header, "NumberOfChannels=") != nullptr) &&
		(strstr (header, "SamplingFrequencyHz=") != nullptr) &&
		(strstr (header, "Calf_Channel_0") == nullptr))
		return EMA_readFromCarstensEMA50xPosFile (file);
	else
		return autoDaata ();
}

static autoDaata CarstensEMA50xV3AmpFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread > 69 && strnequ (header, "AG50xDATA_V00", 13) &&
		(strstr (header, "NumberOfChannels=") != nullptr) && 
		(strstr (header, "SamplingFrequencyHz=") != nullptr) &&
		(strstr (header, "Calf_Channel_0") != nullptr))
		return EMArawData_readFromCarstensEMA50xAmpFile (file);
	else
		return autoDaata ();
}

/******************** Electroglottogram ********************************************/

FORM (CONVERT_EACH_TO_ONE__Electroglottogram_highPassFilter, U"Electroglottogram: High-pass filter", U"Electroglottogram: High-pass filter...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"100.0")
	POSITIVE (smoothing, U"Smoothing (Hz)", U"100.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Electroglottogram)
		autoElectroglottogram result = Electroglottogram_highPassFilter (me, fromFrequency, smoothing);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_filtered")
}

FORM (CONVERT_EACH_TO_ONE__Electroglottogram_to_TextGrid_closedGlottis, U"Electroglottogram: To TextGrid (closed glottis)",
	U"Electroglottogram: To TextGrid (closed glottis)...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"500.0")
	POSITIVE (closingThreshold, U"Closing threshold", U"0.30")
	POSITIVE (peakThresholdFraction, U"Peak threshold (0-1)", U"0.05")
	OK
DO
	Melder_require (closingThreshold < 1.0,
		U"The closing threshold should be smaller than 1.");
	CONVERT_EACH_TO_ONE (Electroglottogram)
		autoTextGrid result = Electroglottogram_to_TextGrid_closedGlottis (me, pitchFloor, pitchCeiling, 
			closingThreshold, peakThresholdFraction
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Electroglottogram_to_AmplitudeTier_levels, U"Electroglottogram: To AmplitudeTier (levels)", U"") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"500.0")
	POSITIVE (closingThreshold, U"Closing threshold", U"0.30")
	BOOLEAN (wantPeaks, U"Peaks", 0)
	BOOLEAN (wantValleys, U"Valleys", 0)
	OK
DO
	CONVERT_EACH_TO_ONE (Electroglottogram)
		autoAmplitudeTier peaks, valleys;
		autoAmplitudeTier result = Electroglottogram_to_AmplitudeTier_levels (me, pitchFloor, pitchCeiling, 
			closingThreshold, & peaks, & valleys
		);
		if (wantPeaks)
			praat_new (peaks.move(), my name.get(), U"_peaks");
		if (wantValleys)
			praat_new (valleys.move(), my name.get(), U"_valleys");
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Electroglottogram_derivative, U"Electroglottogram: Derivative", U"Electroglottogram: Derivative...") {
	POSITIVE (lowPassFrequency, U"Low-pass frequency (Hz)", U"5000.0")
	POSITIVE (smoothing, U"Smoothing (Hz)", U"100.0")
	REAL (newAbsolutePeak, U"New absolute peak", U"0.0 (= don't scale)")
	OK
DO
	CONVERT_EACH_TO_ONE (Electroglottogram)
		autoSound result = Sound_derivative (me, lowPassFrequency, smoothing, newAbsolutePeak);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_derivative")
}

FORM (CONVERT_EACH_TO_ONE__Electroglottogram_firstCentralDifference, U"Electroglottogram: First central difference", U"Electroglottogram: First central difference...") {
	REAL (scalePeak, U"New absolute peak", U"0.0 (= don't scale)")
	OK
DO
	CONVERT_EACH_TO_ONE (Electroglottogram)
		autoSound result = Electroglottogram_firstCentralDifference (me, scalePeak);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_cdiff")
}

DIRECT (CONVERT_EACH_TO_ONE__Electroglottogram_to_Sound) {
	CONVERT_EACH_TO_ONE (Electroglottogram)
		autoSound result = Electroglottogram_to_Sound (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_extractElectroglottogram, U"Sound: Extract Electroglottogram", U"Sound: Extract Electroglottogram...") {
	NATURAL (channelNumber, U"Channel number", U"1")
	BOOLEAN (invert, U"Invert", 0)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoElectroglottogram result = Sound_extractElectroglottogram (me, channelNumber, invert);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

void praat_uvafon_sensors_init () {
	Thing_recognizeClassesByName (classElectroglottogram, classEMA, classEMArawData, nullptr);

	Data_recognizeFileType (CarstensEMA50xV3PosFileRecognizer);
	Data_recognizeFileType (CarstensEMA50xV3AmpFileRecognizer);
	
	praat_addAction1 (classElectroglottogram, 0, U"High-pass filter...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__Electroglottogram_highPassFilter);
	praat_addAction1 (classElectroglottogram, 0, U"To TextGrid (closed glottis)... || Get closed glottis intervals...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Electroglottogram_to_TextGrid_closedGlottis);
	praat_addAction1 (classElectroglottogram, 0, U"To AmplitudeTier (levels)...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__Electroglottogram_to_AmplitudeTier_levels);
	praat_addAction1 (classElectroglottogram, 0, U"Derivative...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__Electroglottogram_derivative);
	praat_addAction1 (classElectroglottogram, 0, U"First central difference...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__Electroglottogram_firstCentralDifference);
	praat_addAction1 (classElectroglottogram, 0, U"To Sound", nullptr, 0, 
			CONVERT_EACH_TO_ONE__Electroglottogram_to_Sound);
   
	praat_addAction1 (classSound, 0, U"Extract Electroglottogram...", U"Extract part for overlap...", 1,
			CONVERT_EACH_TO_ONE__Sound_extractElectroglottogram);

}

/* End of file praat_sensors_init.cpp */

