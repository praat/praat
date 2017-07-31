/* praat_Tiers.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016,2017 Paul Boersma
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

#include "Ltas.h"
#include "PitchTier_to_PointProcess.h"
#include "PitchTier_to_Sound.h"
#include "Matrix_and_PointProcess.h"
#include "PointProcess_and_Sound.h"
#include "Sound_PointProcess.h"
#include "SpectrumTier.h"
#include "TextGrid.h"
#include "VoiceAnalysis.h"

#include "praat_Tiers.h"

// MARK: - AMPLITUDETIER

// MARK: New

FORM (NEW1_AmplitudeTier_create, U"Create empty AmplitudeTier", nullptr) {
	WORD4 (name, U"Name", U"empty")
	REAL4 (startTime, U"Start time (s)", U"0.0")
	REAL4 (endTime, U"End time (s)", U"1.0")
	OK
DO
	if (endTime <= startTime) Melder_throw (U"The end time should be greater than the start time.");
	CREATE_ONE
		autoAmplitudeTier result = AmplitudeTier_create (startTime, endTime);
	CREATE_ONE_END (name)
}

// MARK: Help

DIRECT (HELP_AmplitudeTier_help) {
	HELP (U"AmplitudeTier")
}

// MARK: View & Edit

DIRECT (WINDOW_AmplitudeTier_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an AmplitudeTier from batch.");
	FIND_TWO_WITH_IOBJECT (AmplitudeTier, Sound)   // Sound may be null
		autoAmplitudeTierEditor editor = AmplitudeTierEditor_create (ID_AND_FULL_NAME, me, you, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

DIRECT (HINT_AmplitudeTier_Sound_viewAndEdit) {
	INFO_NONE
		Melder_information (U"To include a copy of a Sound in your AmplitudeTier window:\n"
			"   select an AmplitudeTier and a Sound, and click \"View & Edit\".");
	INFO_NONE_END
}

// MARK: Query

FORM (REAL_AmplitudeTier_getShimmer_local, U"AmplitudeTier: Get shimmer (local)", U"AmplitudeTier: Get shimmer (local)...") {
	REAL4 (shortestPeriod, U"Shortest period (s)", U"0.0001")
	REAL4 (longestPeriod, U"Longest period (s)", U"0.02")
	POSITIVE4 (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_ONE (AmplitudeTier)
		double result = AmplitudeTier_getShimmer_local (me, shortestPeriod, longestPeriod, maximumAmplitudeFactor);
	NUMBER_ONE_END (U" (local shimmer)")
}

FORM (REAL_AmplitudeTier_getShimmer_local_dB, U"AmplitudeTier: Get shimmer (local, dB)", U"AmplitudeTier: Get shimmer (local, dB)...") {
	REAL4 (shortestPeriod, U"Shortest period (s)", U"0.0001")
	REAL4 (longestPeriod, U"Longest period (s)", U"0.02")
	POSITIVE4 (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_ONE (AmplitudeTier)
		double result = AmplitudeTier_getShimmer_local_dB (me, shortestPeriod, longestPeriod, maximumAmplitudeFactor);
	NUMBER_ONE_END (U" dB (local shimmer)")
}

FORM (REAL_AmplitudeTier_getShimmer_apq3, U"AmplitudeTier: Get shimmer (apq3)", U"AmplitudeTier: Get shimmer (apq3)...") {
	REAL4 (shortestPeriod, U"Shortest period (s)", U"0.0001")
	REAL4 (longestPeriod, U"Longest period (s)", U"0.02")
	POSITIVE4 (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_ONE (AmplitudeTier)
		double result = AmplitudeTier_getShimmer_apq3 (me, shortestPeriod, longestPeriod, maximumAmplitudeFactor);
	NUMBER_ONE_END (U" (apq3 shimmer)")
}

FORM (REAL_AmplitudeTier_getShimmer_apq5, U"AmplitudeTier: Get shimmer (apq5)", U"AmplitudeTier: Get shimmer (apq5)...") {
	REAL4 (shortestPeriod, U"Shortest period (s)", U"0.0001")
	REAL4 (longestPeriod, U"Longest period (s)", U"0.02")
	POSITIVE4 (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_ONE (AmplitudeTier)
		double result = AmplitudeTier_getShimmer_apq5 (me, shortestPeriod, longestPeriod, maximumAmplitudeFactor);
	NUMBER_ONE_END (U" (apq5 shimmer)")
}

FORM (REAL_AmplitudeTier_getShimmer_apq11, U"AmplitudeTier: Get shimmer (apq11)", U"AmplitudeTier: Get shimmer (apq11)...") {
	REAL4 (shortestPeriod, U"Shortest period (s)", U"0.0001")
	REAL4 (longestPeriod, U"Longest period (s)", U"0.02")
	POSITIVE4 (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_ONE (AmplitudeTier)
		double result = AmplitudeTier_getShimmer_apq11 (me, shortestPeriod, longestPeriod, maximumAmplitudeFactor);
	NUMBER_ONE_END (U" (apq11 shimmer)")
}

FORM (REAL_AmplitudeTier_getShimmer_dda, U"AmplitudeTier: Get shimmer (dda)", U"AmplitudeTier: Get shimmer (dda)...") {
	REAL4 (shortestPeriod, U"Shortest period (s)", U"0.0001")
	REAL4 (longestPeriod, U"Longest period (s)", U"0.02")
	POSITIVE4 (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_ONE (AmplitudeTier)
		double result = AmplitudeTier_getShimmer_dda (me, shortestPeriod, longestPeriod, maximumAmplitudeFactor);
	NUMBER_ONE_END (U" (dda shimmer)")
}

/*
FORM (REAL_AmplitudeTier_getValueAtTime, U"Get AmplitudeTier value", U"AmplitudeTier: Get value at time...") {
	REAL4 (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (AmplitudeTier)
		double result = RealTier_getValueAtTime (me, time);
	NUMBER_ONE_END (U" Pa")
}
	
FORM (REAL_AmplitudeTier_getValueAtIndex, U"Get AmplitudeTier value", U"AmplitudeTier: Get value at index...") {
	INTEGER4 (pointNumber, U"Point number", U"10")
	OK
DO
	NUMBER_ONE (AmplitudeTier)
		double result = RealTier_getValueAtIndex (me, pointNumber);
	NUMBER_ONE_END (U" Pa")
}
*/

// MARK: Modify

FORM (MODIFY_AmplitudeTier_addPoint, U"Add one point", U"AmplitudeTier: Add point...") {
	REAL4 (time, U"Time (s)", U"0.5")
	REAL4 (soundPressure, U"Sound pressure (Pa)", U"0.8")
	OK
DO
	MODIFY_EACH (AmplitudeTier)
		RealTier_addPoint (me, time, soundPressure);
	MODIFY_EACH_END
}

FORM (MODIFY_AmplitudeTier_formula, U"AmplitudeTier: Formula", U"AmplitudeTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in Pascal")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD4 (formula, U"formula", U"- self ; upside down")
	OK
DO
	MODIFY_EACH_WEAK (AmplitudeTier)
		RealTier_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

// MARK: Synthesize

FORM (NEW_AmplitudeTier_to_Sound, U"AmplitudeTier: To Sound (pulse train)", U"AmplitudeTier: To Sound (pulse train)...") {
	POSITIVE4 (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	NATURAL4 (interpolationDepth, U"Interpolation depth (samples)", U"2000")
	OK
DO
	CONVERT_EACH (AmplitudeTier)
		autoSound result = AmplitudeTier_to_Sound (me, samplingFrequency, interpolationDepth);
	CONVERT_EACH_END (my name)
}

// MARK: Convert

DIRECT (NEW_AmplitudeTier_downto_PointProcess) {
	CONVERT_EACH (AmplitudeTier)
		autoPointProcess result = AnyTier_downto_PointProcess (me->asAnyTier());
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_AmplitudeTier_downto_TableOfReal) {
	CONVERT_EACH (AmplitudeTier)
		autoTableOfReal result = AmplitudeTier_downto_TableOfReal (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_AmplitudeTier_to_IntensityTier, U"AmplitudeTier: To IntensityTier", U"AmplitudeTier: To IntensityTier...") {
	REAL4 (threshold, U"Threshold (dB)", U"-10000.0")
	OK
DO
	CONVERT_EACH (AmplitudeTier)
		autoIntensityTier result = AmplitudeTier_to_IntensityTier (me, threshold);
	CONVERT_EACH_END (my name)
}

// MARK: - AMPLITUDETIER & SOUND

DIRECT (NEW1_Sound_AmplitudeTier_multiply) {
	CONVERT_TWO (Sound, AmplitudeTier)
		autoSound result = Sound_AmplitudeTier_multiply (me, you);
	CONVERT_TWO_END (my name, U"_amp")
}

// MARK: - DURATIONTIER

// MARK: New

FORM (NEW1_DurationTier_create, U"Create empty DurationTier", U"Create DurationTier...") {
	WORD4 (name, U"Name", U"empty")
	REAL4 (startTime, U"Start time (s)", U"0.0")
	REAL4 (endTime, U"End time (s)", U"1.0")
	OK
DO
	if (endTime <= startTime) Melder_throw (U"Your end time should be greater than your start time.");
	CREATE_ONE
		autoDurationTier result = DurationTier_create (startTime, endTime);
	CREATE_ONE_END (name)
}

// MARK: Help

DIRECT (HELP_DurationTier_help) {
	HELP (U"DurationTier")
}

// MARK: View & Edit

DIRECT (WINDOW_DurationTier_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a DurationTier from batch.");
	FIND_TWO_WITH_IOBJECT (DurationTier, Sound)   // Sound may be null
		autoDurationTierEditor editor = DurationTierEditor_create (ID_AND_FULL_NAME, me, you, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

DIRECT (HINT_DurationTier_Sound_edit) {
	INFO_NONE
		Melder_information (U"To include a copy of a Sound in your DurationTier window:\n"
			"   select a DurationTier and a Sound, and click \"View & Edit\".");
	INFO_NONE_END
}

DIRECT (HINT_DurationTier_Manipulation_replace) {
	INFO_NONE
		Melder_information (U"To replace the DurationTier in a Manipulation object,\n"
			"select a DurationTier object and a Manipulation object\nand choose \"Replace duration\".");
	INFO_NONE_END
}

// MARK: Query

FORM (REAL_DurationTier_getTargetDuration, U"Get target duration", nullptr) {
	REAL4 (fromSourceTime, U"left Source time range (s)", U"0.0")
	REAL4 (toSourceTime, U"right Source time range (s)", U"1.0")
	OK
DO
	NUMBER_ONE (DurationTier)
		double result = RealTier_getArea (me, fromSourceTime, toSourceTime);
	NUMBER_ONE_END (U" seconds")
}

FORM (REAL_DurationTier_getValueAtTime, U"Get DurationTier value", U"DurationTier: Get value at time...") {
	REAL4 (sourceTime, U"Source time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (DurationTier)
		double result = RealTier_getValueAtTime (me, sourceTime);
	NUMBER_ONE_END (U" (relative duration)")
}

FORM (REAL_DurationTier_getValueAtIndex, U"Get DurationTier value", U"Duration: Get value at index...") {
	INTEGER4 (pointNumber, U"Point number", U"10")
	OK
DO
	NUMBER_ONE (DurationTier)
		double result = RealTier_getValueAtIndex (me, pointNumber);
	NUMBER_ONE_END (U" (relative duration)")
}

// MARK: Modify

FORM (MODIFY_DurationTier_addPoint, U"Add one point to DurationTier", U"DurationTier: Add point...") {
	REAL4 (time, U"Time (s)", U"0.5")
	REAL4 (relativeDuration, U"Relative duration", U"1.5")
	OK
DO
	MODIFY_EACH (DurationTier)
		RealTier_addPoint (me, time, relativeDuration);
	MODIFY_EACH_END
}

FORM (MODIFY_DurationTier_formula, U"DurationTier: Formula", U"DurationTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in relative units")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD4 (formula, U"formula", U"self * 1.5 ; slow down")
	OK
DO
	MODIFY_EACH_WEAK (DurationTier)
		RealTier_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

// MARK: Convert

DIRECT (NEW_DurationTier_downto_PointProcess) {
	CONVERT_EACH (DurationTier)
		autoPointProcess result = AnyTier_downto_PointProcess (me->asAnyTier());
	CONVERT_EACH_END (my name)
}

// MARK: - FORMANTGRID

// MARK: New

FORM (NEW1_FormantGrid_create, U"Create FormantGrid", nullptr) {
	WORD4 (name, U"Name", U"schwa")
	REAL4 (startTime, U"Start time (s)", U"0.0")
	REAL4 (endTime, U"End time (s)", U"1.0")
	NATURAL4 (numberOfFormants, U"Number of formants", U"10")
	POSITIVE4 (initialFirstFormant, U"Initial first formant (Hz)", U"550.0")
	POSITIVE4 (initialFormatSpacing, U"Initial formant spacing (Hz)", U"1100.0")
	REAL4 (initialFirstBandwidth, U"Initial first bandwidth (Hz)", U"60.0")
	REAL4 (initialBandwidthSpacing, U"Initial bandwidth spacing (Hz)", U"50.0")
	OK
DO
	if (endTime <= startTime) Melder_throw (U"Your end time should be greater than your start time.");
	CREATE_ONE
		autoFormantGrid result = FormantGrid_create (startTime, endTime, numberOfFormants,
			initialFirstFormant, initialFormatSpacing, initialFirstBandwidth, initialBandwidthSpacing);
	CREATE_ONE_END (name)
}

// MARK: Help

DIRECT (HELP_FormantGrid_help) {
	HELP (U"FormantGrid")
}

// MARK: View & Edit

static void cb_FormantGridEditor_publish (Editor /* me */, autoDaata publish) {
	/*
	 * Keep the gate for error handling.
	 */
	try {
		praat_new (publish.move(), U"fromFormantGridEditor");
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT (WINDOW_FormantGrid_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a FormantGrid from batch.");
	FIND_ONE_WITH_IOBJECT (FormantGrid)
		autoFormantGridEditor editor = FormantGridEditor_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_FormantGridEditor_publish);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

// MARK: Modify

FORM (MODIFY_FormantGrid_formula_frequencies, U"FormantGrid: Formula (frequencies)", U"Formant: Formula (frequencies)...") {
	LABEL (U"", U"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	TEXTFIELD4 (formula, U"formula", U"if row = 2 then self + 200 else self fi")
	OK
DO
	MODIFY_EACH_WEAK (FormantGrid)
		FormantGrid_formula_frequencies (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_FormantGrid_formula_bandwidths, U"FormantGrid: Formula (bandwidths)", U"Formant: Formula (bandwidths)...") {
	LABEL (U"", U"row is formant number, col is point number: for row from 1 to nrow do for col from 1 to ncol do B (row, col) :=")
	LABEL (U"", U"self [] is the FormantGrid itself, so it returns frequencies, not bandwidths!")
	TEXTFIELD4 (formula, U"formula", U"self / 10 ; one tenth of the formant frequency")
	OK
DO
	MODIFY_EACH_WEAK (FormantGrid)
		FormantGrid_formula_bandwidths (me, formula, interpreter, nullptr);
			praat_dataChanged (me);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_FormantGrid_addFormantPoint, U"FormantGrid: Add formant point", U"FormantGrid: Add formant point...") {
	NATURAL4 (formantNumber, U"Formant number", U"1")
	REAL4 (time, U"Time (s)", U"0.5")
	POSITIVE4 (frequency, U"Frequency (Hz)", U"550.0")
	OK
DO
	MODIFY_EACH (FormantGrid)
		FormantGrid_addFormantPoint (me, formantNumber, time, frequency);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantGrid_addBandwidthPoint, U"FormantGrid: Add bandwidth point", U"FormantGrid: Add bandwidth point...") {
	NATURAL4 (formantNumber, U"Formant number", U"1")
	REAL4 (time, U"Time (s)", U"0.5")
	POSITIVE4 (bandwidth, U"Bandwidth (Hz)", U"100.0")
	OK
DO
	MODIFY_EACH (FormantGrid)
		FormantGrid_addBandwidthPoint (me, formantNumber, time, bandwidth);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantGrid_removeBandwidthPointsBetween, U"Remove bandwidth points between", U"FormantGrid: Remove bandwidth points between...") {
	NATURAL4 (formantNumber, U"Formant number", U"1")
	REAL4 (fromTime, U"From time (s)", U"0.3")
	REAL4 (toTime, U"To time (s)", U"0.7")
	OK
DO
	MODIFY_EACH (FormantGrid)
		FormantGrid_removeBandwidthPointsBetween (me, formantNumber, fromTime, toTime);
	MODIFY_EACH_END
}

FORM (MODIFY_FormantGrid_removeFormantPointsBetween, U"Remove formant points between", U"FormantGrid: Remove formant points between...") {
	NATURAL4 (formantNumber, U"Formant number", U"1")
	REAL4 (fromTime, U"From time (s)", U"0.3")
	REAL4 (toTime, U"To time (s)", U"0.7")
	OK
DO
	MODIFY_EACH (FormantGrid)
		FormantGrid_removeFormantPointsBetween (me, formantNumber, fromTime, toTime);
	MODIFY_EACH_END
}

// MARK: Convert

FORM (NEW_FormantGrid_to_Formant, U"FormantGrid: To Formant", nullptr) {
	POSITIVE4 (timeStep, U"Time step (s)", U"0.01")
	REAL4 (intensity, U"Intensity (Pa\u00B2)", U"0.1")
	OK
DO
	if (intensity < 0.0) Melder_throw (U"Intensity cannot be negative.");
	CONVERT_EACH (FormantGrid)
		autoFormant result = FormantGrid_to_Formant (me, timeStep, intensity);
	CONVERT_EACH_END (my name)
}

// MARK: - FORMANTGRID & SOUND

DIRECT (NEW1_Sound_FormantGrid_filter) {
	CONVERT_TWO (Sound, FormantGrid)
		autoSound result = Sound_FormantGrid_filter (me, you);
	CONVERT_TWO_END (my name, U"_filt")
}

DIRECT (NEW1_Sound_FormantGrid_filter_noscale) {
	CONVERT_TWO (Sound, FormantGrid)
		autoSound result = Sound_FormantGrid_filter_noscale (me, you);
	CONVERT_TWO_END (my name, U"_filt")
}

// MARK: - FORMANTTIER

// MARK: New

FORM (NEW1_FormantTier_create, U"Create empty FormantTier", nullptr) {
	WORD4 (name, U"Name", U"empty")
	REAL4 (startTime, U"Start time (s)", U"0.0")
	REAL4 (endTime, U"End time (s)", U"1.0")
	OK
DO
	if (endTime <= startTime) Melder_throw (U"Your end time should be greater than your start time.");
	CREATE_ONE
		autoFormantTier result = FormantTier_create (startTime, endTime);
	CREATE_ONE_END (name)
}

// MARK: Draw

FORM (GRAPHICS_FormantTier_speckle, U"Draw FormantTier", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE4 (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantTier)
		FormantTier_speckle (me, GRAPHICS, fromTime, toTime, maximumFrequency, garnish);
	GRAPHICS_EACH_END
}

// MARK: Query

FORM (REAL_FormantTier_getValueAtTime, U"FormantTier: Get value", U"FormantTier: Get value at time...") {
	NATURAL4 (formantNumber, U"Formant number", U"1")
	REAL4 (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (FormantTier)
		double result = FormantTier_getValueAtTime (me, formantNumber, time);
	NUMBER_ONE_END (U" Hz")
}

FORM (REAL_FormantTier_getBandwidthAtTime, U"FormantTier: Get bandwidth", U"FormantTier: Get bandwidth at time...") {
	NATURAL4 (formantNumber, U"Formant number", U"1")
	REAL4 (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (FormantTier)
		double result = FormantTier_getBandwidthAtTime (me, formantNumber, time);
	NUMBER_ONE_END (U" Hz")
}

// MARK: Modify

FORM (MODIFY_FormantTier_addPoint, U"Add one point", U"FormantTier: Add point...") {
	REAL4 (time, U"Time (s)", U"0.5")
	LABEL (U"", U"Frequencies and bandwidths (Hz):")
	TEXTFIELD4 (formantBandwidthPairs, U"fb pairs", U"500 50 1500 100 2500 150 3500 200 4500 300")
	OK
DO
	autoFormantPoint point = FormantPoint_create (time);
	double *f = point -> formant, *b = point -> bandwidth;
	char *fbpairs = Melder_peek32to8 (formantBandwidthPairs);
	int numberOfFormants = sscanf (fbpairs, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",
		f, b, f+1, b+1, f+2, b+2, f+3, b+3, f+4, b+4, f+5, b+5, f+6, b+6, f+7, b+7, f+8, b+8, f+9, b+9) / 2;
	if (numberOfFormants < 1)
		Melder_throw (U"Number of formant-bandwidth pairs must be at least 1.");
	point -> numberOfFormants = numberOfFormants;
	MODIFY_EACH (FormantTier)
		autoFormantPoint point2 = Data_copy (point.get());
		AnyTier_addPoint_move (me->asAnyTier(), point2.move());
	MODIFY_EACH_END
}

// MARK: Convert

FORM (NEW_FormantTier_downto_TableOfReal, U"Down to TableOfReal", nullptr) {
	BOOLEAN4 (includeFormants, U"Include formants", true)
	BOOLEAN4 (includeBandwidths, U"Include bandwidths", false)
	OK
DO
	CONVERT_EACH (FormantTier)
		autoTableOfReal result = FormantTier_downto_TableOfReal (me, includeFormants, includeBandwidths);
	CONVERT_EACH_END (my name)
}

// MARK: - FORMANTTIER & SOUND

DIRECT (NEW1_Sound_FormantTier_filter) {
	CONVERT_TWO (Sound, FormantTier)
		autoSound result = Sound_FormantTier_filter (me, you);
	CONVERT_TWO_END (my name, U"_filt")
}

DIRECT (NEW1_Sound_FormantTier_filter_noscale) {
	CONVERT_TWO (Sound, FormantTier)
		autoSound result = Sound_FormantTier_filter_noscale (me, you);
	CONVERT_TWO_END (my name, U"_filt")
}

// MARK: - INTENSITYTIER

// MARK: New

FORM (NEW1_IntensityTier_create, U"Create empty IntensityTier", nullptr) {
	WORD4 (name, U"Name", U"empty")
	REAL4 (startTime, U"Start time (s)", U"0.0")
	REAL4 (endTime, U"End time (s)", U"1.0")
	OK
DO
	if (endTime <= startTime) Melder_throw (U"Your end time should be greater than your start time.");
	CREATE_ONE
		autoIntensityTier result = IntensityTier_create (startTime, endTime);
	CREATE_ONE_END (name)
}

// MARK: Help

DIRECT (HELP_IntensityTier_help) {
	HELP (U"IntensityTier")
}

// MARK: View & Edit

DIRECT (WINDOW_IntensityTier_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an IntensityTier from batch.");
	FIND_TWO_WITH_IOBJECT (IntensityTier, Sound)   // Sound may be null
		autoIntensityTierEditor editor = IntensityTierEditor_create (ID_AND_FULL_NAME, me, you, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

DIRECT (HINT_IntensityTier_Sound_viewAndEdit) {
	INFO_NONE
		Melder_information (U"To include a copy of a Sound in your IntensityTier window:\n"
			"   select an IntensityTier and a Sound, and click \"View & Edit\".");
	INFO_NONE_END
}

// MARK: Query

FORM (REAL_IntensityTier_getValueAtTime, U"Get IntensityTier value", U"IntensityTier: Get value at time...") {
	REAL4 (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (IntensityTier)
		double result = RealTier_getValueAtTime (me, time);
	NUMBER_ONE_END (U" dB")
}

FORM (REAL_IntensityTier_getValueAtIndex, U"Get IntensityTier value", U"IntensityTier: Get value at index...") {
	INTEGER4 (pointNumber, U"Point number", U"10")
	OK
DO
	NUMBER_ONE (IntensityTier)
		double result = RealTier_getValueAtIndex (me, pointNumber);
	NUMBER_ONE_END (U" dB")
}

// MARK: Modify

FORM (MODIFY_IntensityTier_addPoint, U"Add one point", U"IntensityTier: Add point...") {
	REAL4 (time, U"Time (s)", U"0.5")
	REAL4 (intensity, U"Intensity (dB)", U"75")
	OK
DO
	MODIFY_EACH (IntensityTier)
		RealTier_addPoint (me, time, intensity);
	MODIFY_EACH_END
}

FORM (MODIFY_IntensityTier_formula, U"IntensityTier: Formula", U"IntensityTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in dB")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD4 (formula, U"formula", U"self + 3.0")
	OK
DO
	MODIFY_EACH_WEAK (IntensityTier)
		RealTier_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

// MARK: Convert

DIRECT (NEW_IntensityTier_downto_PointProcess) {
	CONVERT_EACH (IntensityTier)
		autoPointProcess result = AnyTier_downto_PointProcess (me->asAnyTier());
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_IntensityTier_downto_TableOfReal) {
	CONVERT_EACH (IntensityTier)
		autoTableOfReal result = IntensityTier_downto_TableOfReal (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_IntensityTier_to_AmplitudeTier) {
	CONVERT_EACH (IntensityTier)
		autoAmplitudeTier result = IntensityTier_to_AmplitudeTier (me);
	CONVERT_EACH_END (my name)
}

// MARK: - INTENSITYTIER & POINTPROCESS

DIRECT (NEW1_IntensityTier_PointProcess_to_IntensityTier) {
	CONVERT_TWO (IntensityTier, PointProcess)
		autoIntensityTier result = IntensityTier_PointProcess_to_IntensityTier (me, you);
	CONVERT_TWO_END (my name)
}

// MARK: - INTENSITYTIER & SOUND

DIRECT (NEW1_Sound_IntensityTier_multiply_old) {
	CONVERT_TWO (Sound, IntensityTier)
		autoSound result = Sound_IntensityTier_multiply (me, you, true);
	CONVERT_TWO_END (my name, U"_int")
}

FORM (NEW1_Sound_IntensityTier_multiply, U"Sound & IntervalTier: Multiply", nullptr) {
	BOOLEANVAR (scaleTo09, U"Scale to 0.9", true)
	OK
DO
	CONVERT_TWO (Sound, IntensityTier)
		autoSound result = Sound_IntensityTier_multiply (me, you, scaleTo09);
	CONVERT_TWO_END (my name, U"_int")
}

// MARK: - PITCHTIER

FORM (MODIFY_PitchTier_addPoint, U"PitchTier: Add point", U"PitchTier: Add point...") {
	REALVAR (time, U"Time (s)", U"0.5")
	REALVAR (pitch, U"Pitch (Hz)", U"200.0")
	OK
DO
	MODIFY_EACH (PitchTier)
		RealTier_addPoint (me, time, pitch);
	MODIFY_EACH_END
}

FORM (NEW1_PitchTier_create, U"Create empty PitchTier", nullptr) {
	WORDVAR (name, U"Name", U"empty")
	REALVAR (startTime, U"Start time (s)", U"0.0")
	REALVAR (endTime, U"End time (s)", U"1.0")
	OK
DO
	if (endTime <= startTime) Melder_throw (U"Your end time should be greater than your start time.");
	CREATE_ONE
		autoPitchTier result = PitchTier_create (startTime, endTime);
	CREATE_ONE_END (name)
}

DIRECT (NEW_PitchTier_downto_PointProcess) {
	CONVERT_EACH (PitchTier)
		autoPointProcess result = AnyTier_downto_PointProcess (me->asAnyTier());
	CONVERT_EACH_END (my name)
}

FORM (NEW_PitchTier_downto_TableOfReal, U"PitchTier: Down to TableOfReal", nullptr) {
	RADIO4x (unit, U"Unit", 1, 0)
		RADIOBUTTON (U"Hertz")
		RADIOBUTTON (U"Semitones")
	OK
DO
	CONVERT_EACH (PitchTier)
		autoTableOfReal result = PitchTier_downto_TableOfReal (me, unit);
	CONVERT_EACH_END (my name)
}

FORM (GRAPHICS_old_PitchTier_draw, U"PitchTier: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	POSITIVEVAR (toFrequency, U"right Frequency range (Hz)", U"500.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	OK
DO
	if (toFrequency <= fromFrequency)
		Melder_throw (U"Your maximum frequency should be greater than your minimum frequency.");
	GRAPHICS_EACH (PitchTier)
		PitchTier_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			garnish, U"lines and speckles");
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_PitchTier_draw, U"PitchTier: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REALVAR (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	POSITIVEVAR (toFrequency, U"right Frequency range (Hz)", U"500.0")
	BOOLEANVAR (garnish, U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENUSTRVAR (drawingMethod, U"Drawing method", 1)
		OPTION (U"lines")
		OPTION (U"speckles")
		OPTION (U"lines and speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_old_PitchTier_draw)
	if (toFrequency <= fromFrequency)
		Melder_throw (U"Your maximum frequency should be greater than your minimum frequency.");
	GRAPHICS_EACH (PitchTier)
		PitchTier_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, drawingMethod);
	GRAPHICS_EACH_END
}

DIRECT (WINDOW_PitchTier_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a PitchTier from batch.");
	FIND_TWO_WITH_IOBJECT (PitchTier, Sound)   // Sound may be null
		autoPitchTierEditor editor = PitchTierEditor_create (ID_AND_FULL_NAME, me, you, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

FORM (MODIFY_PitchTier_formula, U"PitchTier: Formula", U"PitchTier: Formula...") {
	LABEL (U"", U"# ncol = the number of points")
	LABEL (U"", U"for col from 1 to ncol")
	LABEL (U"", U"   # x = the time of the colth point, in seconds")
	LABEL (U"", U"   # self = the value of the colth point, in hertz")
	LABEL (U"", U"   self = `formula'")
	LABEL (U"", U"endfor")
	TEXTFIELD4 (formula, U"formula", U"self * 2 ; one octave up")
	OK
DO
	MODIFY_EACH_WEAK (PitchTier)
		RealTier_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (REAL_PitchTier_getMean_curve, U"PitchTier: Get mean (curve)", U"PitchTier: Get mean (curve)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	NUMBER_ONE (PitchTier)
		double result = RealTier_getMean_curve (me, fromTime, toTime);
	NUMBER_ONE_END (U" Hz")
}
	
FORM (REAL_PitchTier_getMean_points, U"PitchTier: Get mean (points)", U"PitchTier: Get mean (points)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	NUMBER_ONE (PitchTier)
		double result = RealTier_getMean_points (me, fromTime, toTime);
	NUMBER_ONE_END (U" Hz")
}
	
FORM (REAL_PitchTier_getStandardDeviation_curve, U"PitchTier: Get standard deviation (curve)", U"PitchTier: Get standard deviation (curve)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	NUMBER_ONE (PitchTier)
		double result = RealTier_getStandardDeviation_curve (me, fromTime, toTime);
	NUMBER_ONE_END (U" Hz")
}
	
FORM (REAL_PitchTier_getStandardDeviation_points, U"PitchTier: Get standard deviation (points)", U"PitchTier: Get standard deviation (points)...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	NUMBER_ONE (PitchTier)
		double result = RealTier_getStandardDeviation_points (me, fromTime, toTime);
	NUMBER_ONE_END (U" Hz")
}
	
FORM (REAL_PitchTier_getValueAtTime, U"PitchTier: Get value at time", U"PitchTier: Get value at time...") {
	REAL4 (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (PitchTier)
		double result = RealTier_getValueAtTime (me, time);
	NUMBER_ONE_END (U" Hz")
}
	
FORM (REAL_PitchTier_getValueAtIndex, U"PitchTier: Get value at index", U"PitchTier: Get value at index...") {
	INTEGER4 (pointNumber, U"Point number", U"10")
	OK
DO
	NUMBER_ONE (PitchTier)
		double result = RealTier_getValueAtIndex (me, pointNumber);
	NUMBER_ONE_END (U" Hz")
}

DIRECT (HELP_PitchTier_help) {
	HELP (U"PitchTier")
}

DIRECT (PLAY_PitchTier_hum) {
	PLAY_EACH (PitchTier)
		PitchTier_hum (me);
	PLAY_EACH_END
}

FORM (MODIFY_PitchTier_interpolateQuadratically, U"PitchTier: Interpolate quadratically", nullptr) {
	NATURAL4 (numberOfPointsPerParabola, U"Number of points per parabola", U"4")
	RADIO4x (unit, U"Unit", 2, 0)
		RADIOBUTTON (U"Hz")
		RADIOBUTTON (U"Semitones")
	OK
DO
	MODIFY_EACH (PitchTier)
		RealTier_interpolateQuadratically (me, numberOfPointsPerParabola, unit);
	MODIFY_EACH_END
}

DIRECT (PLAY_PitchTier_play) {
	PLAY_EACH (PitchTier)
		PitchTier_play (me);
	PLAY_EACH_END
}

DIRECT (PLAY_PitchTier_playSine) {
	PLAY_EACH (PitchTier)
		PitchTier_playPart_sine (me, 0.0, 0.0);
	PLAY_EACH_END
}

FORM (MODIFY_PitchTier_shiftFrequencies, U"PitchTier: Shift frequencies", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"1000.0")
	REALVAR (frequencyShift, U"Frequency shift", U"-20.0")
	OPTIONMENUVAR (unit, U"Unit", 1)
		OPTION (U"Hertz")
		OPTION (U"mel")
		OPTION (U"logHertz")
		OPTION (U"semitones")
		OPTION (U"ERB")
	OK
DO
	unit =
		unit == 1 ? kPitch_unit_HERTZ :
		unit == 2 ? kPitch_unit_MEL :
		unit == 3 ? kPitch_unit_LOG_HERTZ :
		unit == 4 ? kPitch_unit_SEMITONES_1 :
		kPitch_unit_ERB;
	MODIFY_EACH_WEAK (PitchTier)
		PitchTier_shiftFrequencies (me, fromTime, toTime, frequencyShift, unit);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_PitchTier_multiplyFrequencies, U"PitchTier: Multiply frequencies", nullptr){
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"1000.0")
	POSITIVEVAR (factor, U"Factor", U"1.2")
	OK
DO
	MODIFY_EACH (PitchTier)
		PitchTier_multiplyFrequencies (me, fromTime, toTime, factor);
	MODIFY_EACH_END
}

FORM (MODIFY_PitchTier_stylize, U"PitchTier: Stylize", U"PitchTier: Stylize...") {
	REALVAR (frequencyResolution, U"Frequency resolution", U"4.0")
	RADIOVARx (unit, U"Unit", 2, 0)
		RADIOBUTTON (U"Hz")
		RADIOBUTTON (U"Semitones")
	OK
DO
	MODIFY_EACH (PitchTier)
		PitchTier_stylize (me, frequencyResolution, unit);
	MODIFY_EACH_END
}

DIRECT (NEW_PitchTier_to_PointProcess) {
	CONVERT_EACH (PitchTier)
		autoPointProcess result = PitchTier_to_PointProcess (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PitchTier_to_Sound_phonation, U"PitchTier: To Sound (phonation)", nullptr) {
	POSITIVEVAR (samplingFrequency, U"Sampling frequency (Hz)", U"44100")
	POSITIVEVAR (adaptationFactor, U"Adaptation factor", U"1.0")
	POSITIVEVAR (maximumPeriod, U"Maximum period (s)", U"0.05")
	POSITIVEVAR (openPhase, U"Open phase", U"0.7")
	REALVAR (collisionPhase, U"Collision phase", U"0.03")
	POSITIVEVAR (power1, U"Power 1", U"3.0")
	POSITIVEVAR (power2, U"Power 2", U"4.0")
	BOOLEANVAR (hum, U"Hum", false)
	OK
DO
	CONVERT_EACH (PitchTier)
		autoSound result = PitchTier_to_Sound_phonation (me, samplingFrequency,
			adaptationFactor, maximumPeriod, openPhase, collisionPhase, power1, power2, hum);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PitchTier_to_Sound_pulseTrain, U"PitchTier: To Sound (pulse train)", nullptr) {
	POSITIVEVAR (samplingFrequency, U"Sampling frequency (Hz)", U"44100")
	POSITIVEVAR (adaptationFactor, U"Adaptation factor", U"1.0")
	POSITIVEVAR (adaptationTime, U"Adaptation time", U"0.05")
	NATURALVAR (interpolationDepth, U"Interpolation depth (samples)", U"2000")
	BOOLEANVAR (hum, U"Hum", false)
	OK
DO
	CONVERT_EACH (PitchTier)
		autoSound result = PitchTier_to_Sound_pulseTrain (me, samplingFrequency,
			adaptationFactor, adaptationTime, interpolationDepth, hum);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PitchTier_to_Sound_sine, U"PitchTier: To Sound (sine)", nullptr) {
	POSITIVEVAR (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	OK
DO
	CONVERT_EACH (PitchTier)
		autoSound result = PitchTier_to_Sound_sine (me, 0.0, 0.0, samplingFrequency);
	CONVERT_EACH_END (my name)
}

DIRECT (HINT_PitchTier_Sound_viewAndEdit) {
	Melder_information (U"To include a copy of a Sound in your PitchTier window:\n"
		"   select a PitchTier and a Sound, and click \"View & Edit\".");
END }

FORM_SAVE (SAVE_PitchTier_writeToPitchTierSpreadsheetFile, U"Save PitchTier as spreadsheet", nullptr, U"PitchTier") {
	SAVE_ONE (PitchTier)
		PitchTier_writeToPitchTierSpreadsheetFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_PitchTier_writeToHeaderlessSpreadsheetFile, U"Save PitchTier as spreadsheet", nullptr, U"txt") {
	SAVE_ONE (PitchTier)
		PitchTier_writeToHeaderlessSpreadsheetFile (me, file);
	SAVE_ONE_END
}

DIRECT (INFO_PitchTier_Manipulation_replace) {
	INFO_NONE
		Melder_information (U"To replace the PitchTier in a Manipulation object,\n"
			"select a PitchTier object and a Manipulation object\nand choose \"Replace pitch\".");
	INFO_NONE_END
}

// MARK: - PITCHTIER & POINTPROCESS

DIRECT (NEW1_PitchTier_PointProcess_to_PitchTier) {
	CONVERT_TWO (PitchTier, PointProcess)
		autoPitchTier result = PitchTier_PointProcess_to_PitchTier (me, you);
	CONVERT_TWO_END (my name)
}

// MARK: - POINTPROCESS

FORM (MODIFY_PointProcess_addPoint, U"PointProcess: Add point", U"PointProcess: Add point...") {
	REALVAR (time, U"Time (s)", U"0.5")
	OK
DO
	MODIFY_EACH (PointProcess)
		PointProcess_addPoint (me, time);
	MODIFY_EACH_END
}

FORM (NEW1_PointProcess_createEmpty, U"Create an empty PointProcess", U"Create empty PointProcess...") {
	WORDVAR (name, U"Name", U"empty")
	REALVAR (startTime, U"Start time (s)", U"0.0")
	REALVAR (endTime, U"End time (s)", U"1.0")
	OK
DO
	if (endTime < startTime)
		Melder_throw (U"Your end time (", endTime, U") should not be less than your start time (", startTime, U").");
	CREATE_ONE
		autoPointProcess result = PointProcess_create (startTime, endTime, 0L);
	CREATE_ONE_END (name)
}

FORM (NEW1_PointProcess_createPoissonProcess, U"Create Poisson process", U"Create Poisson process...") {
	WORDVAR (name, U"Name", U"poisson")
	REALVAR (startTime, U"Start time (s)", U"0.0")
	REALVAR (endTime, U"End time (s)", U"1.0")
	POSITIVEVAR (density, U"Density (/s)", U"100.0")
	OK
DO
	if (endTime < startTime)
		Melder_throw (U"Your end time (", endTime, U") should not be less than your start time (", startTime, U").");
	CREATE_ONE
		autoPointProcess result = PointProcess_createPoissonProcess (startTime, endTime, density);
	CREATE_ONE_END (name)
}

DIRECT (NEW1_PointProcesses_difference) {
	CONVERT_COUPLE (PointProcess)
		autoPointProcess result = PointProcesses_difference (me, you);
	CONVERT_COUPLE_END (U"difference")
}

FORM (GRAPHICS_PointProcess_draw, U"PointProcess: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (PointProcess)
		PointProcess_draw (me, GRAPHICS, fromTime, toTime, garnish);
	GRAPHICS_EACH_END
}

DIRECT (WINDOW_PointProcess_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a PointProcess from batch.");
	FIND_TWO_WITH_IOBJECT (PointProcess, Sound)   // Sound may be null
		autoPointEditor editor = PointEditor_create (ID_AND_FULL_NAME, me, you);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

FORM (MODIFY_PointProcess_fill, U"PointProcess: Fill", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE4 (period, U"Period (s)", U"0.01")
	OK
DO
	MODIFY_EACH_WEAK (PointProcess)
		PointProcess_fill (me, fromTime, toTime, period);
	MODIFY_EACH_WEAK_END
}

FORM (REAL_PointProcess_getInterval, U"PointProcess: Get interval", U"PointProcess: Get interval...") {
	REAL4 (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (PointProcess)
		double result = PointProcess_getInterval (me, time);
	NUMBER_ONE_END (U" seconds")
}

#define dia_PointProcess_getRangeProperty(fromTime,toTime,shortestPeriod,longestPeriod,maximumPeriodfactor) \
	praat_TimeFunction_RANGE (fromTime, toTime) \
	REALVAR (shortestPeriod, U"Shortest period (s)", U"0.0001") \
	REALVAR (longestPeriod, U"Longest period (s)", U"0.02") \
	POSITIVEVAR (maximumPeriodFactor, U"Maximum period factor", U"1.3")

FORM (REAL_PointProcess_getJitter_local, U"PointProcess: Get jitter (local)", U"PointProcess: Get jitter (local)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	NUMBER_ONE (PointProcess)
		double result = PointProcess_getJitter_local (me, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor);
	NUMBER_ONE_END (U" (local jitter)")
}

FORM (REAL_PointProcess_getJitter_local_absolute, U"PointProcess: Get jitter (local, absolute)", U"PointProcess: Get jitter (local, absolute)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	NUMBER_ONE (PointProcess)
		double result = PointProcess_getJitter_local_absolute (me, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor);
	NUMBER_ONE_END (U" seconds (local absolute jitter)")
}

FORM (REAL_PointProcess_getJitter_rap, U"PointProcess: Get jitter (rap)", U"PointProcess: Get jitter (rap)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	NUMBER_ONE (PointProcess)
		double result = PointProcess_getJitter_rap (me, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor);
	NUMBER_ONE_END (U" (rap jitter)")
}

FORM (REAL_PointProcess_getJitter_ppq5, U"PointProcess: Get jitter (ppq5)", U"PointProcess: Get jitter (ppq5)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	NUMBER_ONE (PointProcess)
		double result = PointProcess_getJitter_ppq5 (me, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor);
	NUMBER_ONE_END (U" (ppq5 jitter)")
}

FORM (REAL_PointProcess_getJitter_ddp, U"PointProcess: Get jitter (ddp)", U"PointProcess: Get jitter (ddp)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	NUMBER_ONE (PointProcess)
		double result = PointProcess_getJitter_ddp (me, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor);
	NUMBER_ONE_END (U" (ddp jitter)")
}

FORM (REAL_PointProcess_getMeanPeriod, U"PointProcess: Get mean period", U"PointProcess: Get mean period...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	NUMBER_ONE (PointProcess)
		double result = PointProcess_getMeanPeriod (me, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor);
	NUMBER_ONE_END (U" seconds (mean period)")
}

FORM (REAL_PointProcess_getStdevPeriod, U"PointProcess: Get stdev period", U"PointProcess: Get stdev period...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	NUMBER_ONE (PointProcess)
		double result = PointProcess_getStdevPeriod (me, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor);
	NUMBER_ONE_END (U" (stdev period)")
}

FORM (INTEGER_PointProcess_getLowIndex, U"PointProcess: Get low index", U"PointProcess: Get low index...") {
	REALVAR (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (PointProcess)
		long result = PointProcess_getLowIndex (me, time);
	NUMBER_ONE_END (U" (low index)")
}

FORM (INTEGER_PointProcess_getHighIndex, U"PointProcess: Get high index", U"PointProcess: Get high index...") {
	REALVAR (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (PointProcess)
		long result = PointProcess_getHighIndex (me, time);
	NUMBER_ONE_END (U" (high index)")
}

FORM (INTEGER_PointProcess_getNearestIndex, U"PointProcess: Get nearest index", U"PointProcess: Get nearest index...") {
	REALVAR (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (PointProcess)
		long result = PointProcess_getNearestIndex (me, time);
	NUMBER_ONE_END (U" (nearest index)")
}

DIRECT (INTEGER_PointProcess_getNumberOfPoints) {
	NUMBER_ONE (PointProcess)
		long result = my nt;
	NUMBER_ONE_END (U" points")
}

FORM (INTEGER_PointProcess_getNumberOfPeriods, U"PointProcess: Get number of periods", U"PointProcess: Get number of periods...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	NUMBER_ONE (PointProcess)
		long result = PointProcess_getNumberOfPeriods (me, fromTime, toTime,
		shortestPeriod, longestPeriod, maximumPeriodFactor);
	NUMBER_ONE_END (U" periods")
}

FORM (REAL_PointProcess_getTimeFromIndex, U"Get time", 0 /*"PointProcess: Get time from index..."*/) {
	NATURAL4 (pointNumber, U"Point number", U"10")
	OK
DO
	NUMBER_ONE (PointProcess)
		double result =
			pointNumber > my nt ? undefined : my t [pointNumber];
	NUMBER_ONE_END (U" seconds")
}

DIRECT (HELP_PointProcess_help) {
	HELP (U"PointProcess")
}

DIRECT (PLAY_PointProcess_hum) {
	PLAY_EACH (PointProcess)
		PointProcess_hum (me, my xmin, my xmax);
	PLAY_EACH_END
}

DIRECT (NEW1_PointProcesses_intersection) {
	CONVERT_COUPLE (PointProcess)
		autoPointProcess result = PointProcesses_intersection (me, you);
	CONVERT_COUPLE_END (U"intersection")
}

DIRECT (PLAY_PointProcess_play) {
	PLAY_EACH (PointProcess)
		PointProcess_play (me);
	PLAY_EACH_END
}

FORM (MODIFY_PointProcess_removePoint, U"PointProcess: Remove point", U"PointProcess: Remove point...") {
	NATURAL4 (pointNumber, U"Point number", U"1")
	OK
DO
	MODIFY_EACH (PointProcess)
		PointProcess_removePoint (me, pointNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_PointProcess_removePointNear, U"PointProcess: Remove point near", U"PointProcess: Remove point near...") {
	REAL4 (time, U"Time (s)", U"0.5")
	OK
DO
	MODIFY_EACH (PointProcess)
		PointProcess_removePointNear (me, time);
	MODIFY_EACH_END
}

FORM (MODIFY_PointProcess_removePoints, U"PointProcess: Remove points", U"PointProcess: Remove points...") {
	NATURAL4 (fromPointNumber, U"From point number", U"1")
	NATURAL4 (toPointNumber, U"To point number", U"10")
	OK
DO
	MODIFY_EACH (PointProcess)
		PointProcess_removePoints (me, fromPointNumber, toPointNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_PointProcess_removePointsBetween, U"PointProcess: Remove points between", U"PointProcess: Remove points between...") {
	REAL4 (fromTime, U"left Time range (s)", U"0.3")
	REAL4 (toTime, U"right Time range (s)", U"0.7")
	OK
DO
	MODIFY_EACH (PointProcess)
		PointProcess_removePointsBetween (me, fromTime, toTime);
	MODIFY_EACH_END
}

DIRECT (NEW_PointProcess_to_IntervalTier) {
	CONVERT_EACH (PointProcess)
		autoIntervalTier result = IntervalTier_create (my xmin, my xmax);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_PointProcess_to_Matrix) {
	CONVERT_EACH (PointProcess)
		autoMatrix result = PointProcess_to_Matrix (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PointProcess_to_PitchTier, U"PointProcess: To PitchTier", U"PointProcess: To PitchTier...") {
	POSITIVEVAR (maximumInterval, U"Maximum interval (s)", U"0.02")
	OK
DO
	CONVERT_EACH (PointProcess)
		autoPitchTier result = PointProcess_to_PitchTier (me, maximumInterval);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PointProcess_to_TextGrid, U"PointProcess: To TextGrid...", U"PointProcess: To TextGrid...") {
	SENTENCE4 (tierNames, U"Tier names", U"Mary John bell")
	SENTENCE4 (pointTiers, U"Point tiers", U"bell")
	OK
DO
	CONVERT_EACH (PointProcess)
		autoTextGrid result = TextGrid_create (my xmin, my xmax, tierNames, pointTiers);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PointProcess_to_TextGrid_vuv, U"PointProcess: To TextGrid (vuv)...", U"PointProcess: To TextGrid (vuv)...") {
	POSITIVE4 (maximumPeriod, U"Maximum period (s)", U"0.02")
	REAL4 (meanPeriod, U"Mean period (s)", U"0.01")
	OK
DO
	CONVERT_EACH (PointProcess)
		autoTextGrid result = PointProcess_to_TextGrid_vuv (me, maximumPeriod, meanPeriod);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_PointProcess_to_TextTier) {
	CONVERT_EACH (PointProcess)
		autoTextTier result = TextTier_create (my xmin, my xmax);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PointProcess_to_Sound_phonation, U"PointProcess: To Sound (phonation)", U"PointProcess: To Sound (phonation)...") {
	POSITIVE4 (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	POSITIVE4 (adaptationFactor, U"Adaptation factor", U"1.0")
	POSITIVE4 (maximumPeriod, U"Maximum period (s)", U"0.05")
	POSITIVE4 (openPhase, U"Open phase", U"0.7")
	REAL4 (collisionPhase, U"Collision phase", U"0.03")
	POSITIVE4 (power1, U"Power 1", U"3.0")
	POSITIVE4 (power2, U"Power 2", U"4.0")
	OK
DO
	CONVERT_EACH (PointProcess)
		autoSound result = PointProcess_to_Sound_phonation (me, samplingFrequency,
			adaptationFactor, maximumPeriod, openPhase, collisionPhase, power1, power2);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PointProcess_to_Sound_pulseTrain, U"PointProcess: To Sound (pulse train)", U"PointProcess: To Sound (pulse train)...") {
	POSITIVE4 (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	POSITIVE4 (adaptationFactor, U"Adaptation factor", U"1.0")
	POSITIVE4 (adaptationTime, U"Adaptation time (s)", U"0.05")
	NATURAL4 (interpolationDepth, U"Interpolation depth (samples)", U"2000")
	OK
DO
	CONVERT_EACH (PointProcess)
		autoSound result = PointProcess_to_Sound_pulseTrain (me, samplingFrequency,
			adaptationFactor, adaptationTime, interpolationDepth);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_PointProcess_to_Sound_hum) {
	CONVERT_EACH (PointProcess)
		autoSound result = PointProcess_to_Sound_hum (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW1_PointProcesses_union) {
	CONVERT_COUPLE (PointProcess)
		autoPointProcess result = PointProcesses_union (me, you);
	CONVERT_COUPLE_END (U"union")
}

FORM (NEW_PointProcess_upto_IntensityTier, U"PointProcess: Up to IntensityTier", U"PointProcess: Up to IntensityTier...") {
	POSITIVE4 (intensity, U"Intensity (dB)", U"70.0")
	OK
DO
	CONVERT_EACH (PointProcess)
		autoIntensityTier result = PointProcess_upto_IntensityTier (me, intensity);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PointProcess_upto_PitchTier, U"PointProcess: Up to PitchTier", U"PointProcess: Up to PitchTier...") {
	POSITIVE4 (frequency, U"Frequency (Hz)", U"190.0")
	OK
DO
	CONVERT_EACH (PointProcess)
		autoPitchTier result = PointProcess_upto_PitchTier (me, frequency);
	CONVERT_EACH_END (my name)
}

FORM (NEW_PointProcess_upto_TextTier, U"PointProcess: Up to TextTier", U"PointProcess: Up to TextTier...") {
	SENTENCE4 (text, U"Text", U"")
	OK
DO
	CONVERT_EACH (PointProcess)
		autoTextTier result = PointProcess_upto_TextTier (me, text);
	CONVERT_EACH_END (my name)
}

FORM (MODIFY_PointProcess_voice, U"PointProcess: Fill unvoiced parts", nullptr) {
	POSITIVEVAR (period, U"Period (s)", U"0.01")
	POSITIVEVAR (maximumVoicedPeriod, U"Maximum voiced period (s)", U"0.02000000001")
	OK
DO
	MODIFY_EACH_WEAK (PointProcess)
		PointProcess_voice (me, period, maximumVoicedPeriod);
	MODIFY_EACH_WEAK_END
}

DIRECT (HINT_PointProcess_Sound_viewAndEdit) {
	INFO_NONE
		Melder_information (U"To include a copy of a Sound in your PointProcess window:\n"
			"   select a PointProcess and a Sound, and click \"View & Edit\".");
	INFO_NONE_END
}

// MARK: - POINTPROCESS & SOUND

DIRECT (MODIFY_Point_Sound_transplantDomain) {
	MODIFY_FIRST_OF_TWO (PointProcess, Sound)
		my xmin = your xmin;
		my xmax = your xmax;
	MODIFY_FIRST_OF_TWO_END
}

FORM (REAL_Point_Sound_getShimmer_local, U"PointProcess & Sound: Get shimmer (local)", U"PointProcess & Sound: Get shimmer (local)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVEVAR (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_TWO (PointProcess, Sound)
		double result = PointProcess_Sound_getShimmer_local (me, you, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor, maximumAmplitudeFactor);
	NUMBER_TWO_END (U" (local shimmer)");
}

FORM (REAL_Point_Sound_getShimmer_local_dB, U"PointProcess & Sound: Get shimmer (local, dB)", U"PointProcess & Sound: Get shimmer (local, dB)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVEVAR (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_TWO (PointProcess, Sound)
		double result = PointProcess_Sound_getShimmer_local_dB (me, you, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor, maximumAmplitudeFactor);
	NUMBER_TWO_END (U" dB (local shimmer)");
}

FORM (REAL_Point_Sound_getShimmer_apq3, U"PointProcess & Sound: Get shimmer (apq3)", U"PointProcess & Sound: Get shimmer (apq3)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVEVAR (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_TWO (PointProcess, Sound)
		double result = PointProcess_Sound_getShimmer_apq3 (me, you, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor, maximumAmplitudeFactor);
	NUMBER_TWO_END (U" (apq3 shimmer)");
}

FORM (REAL_Point_Sound_getShimmer_apq5, U"PointProcess & Sound: Get shimmer (apq)", U"PointProcess & Sound: Get shimmer (apq5)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVEVAR (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_TWO (PointProcess, Sound)
		double result = PointProcess_Sound_getShimmer_apq5 (me, you, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor, maximumAmplitudeFactor);
	NUMBER_TWO_END (U" (apq5 shimmer)");
}

FORM (REAL_Point_Sound_getShimmer_apq11, U"PointProcess & Sound: Get shimmer (apq11)", U"PointProcess & Sound: Get shimmer (apq11)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVEVAR (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_TWO (PointProcess, Sound)
		double result = PointProcess_Sound_getShimmer_apq11 (me, you, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor, maximumAmplitudeFactor);
	NUMBER_TWO_END (U" (apq11 shimmer)");
}

FORM (REAL_Point_Sound_getShimmer_dda, U"PointProcess & Sound: Get shimmer (dda)", U"PointProcess & Sound: Get shimmer (dda)...") {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	POSITIVEVAR (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	OK
DO
	NUMBER_TWO (PointProcess, Sound)
		double result = PointProcess_Sound_getShimmer_dda (me, you, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor, maximumAmplitudeFactor);
	NUMBER_TWO_END (U" (dda shimmer)");
}

FORM (NEW1_PointProcess_Sound_to_AmplitudeTier_period, U"PointProcess & Sound: To AmplitudeTier (period)", nullptr) {
	dia_PointProcess_getRangeProperty (fromTime, toTime, shortestPeriod, longestPeriod, maximumPeriodfactor)
	OK
DO
	CONVERT_TWO (PointProcess, Sound)
		autoAmplitudeTier result = PointProcess_Sound_to_AmplitudeTier_period (me, you, fromTime, toTime,
			shortestPeriod, longestPeriod, maximumPeriodFactor);
	CONVERT_TWO_END (your name, U"_", my name)
}

DIRECT (NEW1_PointProcess_Sound_to_AmplitudeTier_point) {
	CONVERT_TWO (PointProcess, Sound)
		autoAmplitudeTier result = PointProcess_Sound_to_AmplitudeTier_point (me, you);
	CONVERT_TWO_END (your name, U"_", my name);
}

FORM (NEW1_PointProcess_Sound_to_Ltas, U"PointProcess & Sound: To Ltas", nullptr) {
	POSITIVE4 (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE4 (bandwidth, U"Band width (Hz)", U"100.0")
	REAL4 (shortestPeriod, U"Shortest period (s)", U"0.0001")
	REAL4 (longestPeriod, U"Longest period (s)", U"0.02")
	POSITIVE4 (maximumPeriodFactor, U"Maximum period factor", U"1.3")
	OK
DO
	CONVERT_TWO (PointProcess, Sound)
		autoLtas result = PointProcess_Sound_to_Ltas (me, you,
			maximumFrequency, bandwidth, shortestPeriod, longestPeriod, maximumPeriodFactor);
	CONVERT_TWO_END (your name)
}

FORM (NEW1_PointProcess_Sound_to_Ltas_harmonics, U"PointProcess & Sound: To Ltas (harmonics", nullptr) {
	NATURAL4 (maximumHarmonic, U"Maximum harmonic", U"20")
	REAL4 (shortestPeriod, U"Shortest period (s)", U"0.0001")
	REAL4 (longestPeriod, U"Longest period (s)", U"0.02")
	POSITIVE4 (maximumPeriodFactor, U"Maximum period factor", U"1.3")
	OK
DO
	CONVERT_TWO (PointProcess, Sound)
		autoLtas result = PointProcess_Sound_to_Ltas_harmonics (me, you,
			maximumHarmonic, shortestPeriod, longestPeriod, maximumPeriodFactor);
	CONVERT_TWO_END (your name)
}

FORM (NEW1_Sound_PointProcess_to_SoundEnsemble_correlate, U"Sound & PointProcess: To SoundEnsemble (correlate)", nullptr) {
	REAL4 (fromTime, U"From time (s)", U"-0.1")
	REAL4 (toTime, U"To time (s)", U"1.0")
	OK
DO
	CONVERT_TWO (Sound, PointProcess)
		autoSound result = Sound_PointProcess_to_SoundEnsemble_correlate (me, you, fromTime, toTime);
	CONVERT_TWO_END (your name)
}

// MARK: - SPECTRUMTIER

DIRECT (NEW_SpectrumTier_downto_Table) {
	CONVERT_EACH (SpectrumTier)
		autoTable result = SpectrumTier_downto_Table (me, true, true, true);
	CONVERT_EACH_END (my name)
}

FORM (GRAPHICS_old_SpectrumTier_draw, U"SpectrumTier: Draw", nullptr) {   // 2010-10-19
	REAL4 (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL4 (toFrequency, U"right Frequency range (Hz)", U"10000.0")
	REAL4 (fromPower, U"left Power range (dB)", U"20.0")
	REAL4 (toPower, U"right Power range (dB)", U"80.0")
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (SpectrumTier)
		SpectrumTier_draw (me, GRAPHICS, fromFrequency, toFrequency,
			fromPower, toPower, garnish, U"lines and speckles");
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_SpectrumTier_draw, U"SpectrumTier: Draw", nullptr) {
	REAL4 (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL4 (toFrequency, U"right Frequency range (Hz)", U"10000.0")
	REAL4 (fromPower, U"left Power range (dB)", U"20.0")
	REAL4 (toPower, U"right Power range (dB)", U"80.0")
	BOOLEAN4 (garnish, U"Garnish", true)
	LABEL (U"", U"")
	OPTIONMENUSTR4 (drawingMethod, U"Drawing method", 1)
		OPTION (U"lines")
		OPTION (U"speckles")
		OPTION (U"lines and speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_old_SpectrumTier_draw)
	GRAPHICS_EACH (SpectrumTier)
		SpectrumTier_draw (me, GRAPHICS, fromFrequency, toFrequency,
			fromPower, toPower, garnish, drawingMethod);
	GRAPHICS_EACH_END
}

FORM (LIST_SpectrumTier_list, U"SpectrumTier: List", nullptr) {
	BOOLEAN4 (includeIndexes, U"Include indexes", true)
	BOOLEAN4 (includeFrequency, U"Include frequency", true)
	BOOLEAN4 (includePowerDensity, U"Include power density", true)
	OK
DO
	INFO_ONE (SpectrumTier)
		SpectrumTier_list (me, includeIndexes, includeFrequency, includePowerDensity);
	INFO_ONE_END
}

FORM (MODIFY_SpectrumTier_removePointsBelow, U"SpectrumTier: Remove points below", nullptr) {
	REAL4 (removeAllPointsBelow, U"Remove all points below (dB)", U"40.0")
	OK
DO
	MODIFY_EACH (SpectrumTier)
		RealTier_removePointsBelow (me, removeAllPointsBelow);
	MODIFY_EACH_END
}

// MARK: - buttons

void praat_Tiers_init () {
	Thing_recognizeClassesByName (classPointProcess,
		classRealPoint, classRealTier,
		classPitchTier, classIntensityTier, classDurationTier, classAmplitudeTier,
		classSpectrumTier,
		classFormantPoint, classFormantTier, classFormantGrid,
		nullptr);

	praat_addMenuCommand (U"Objects", U"New", U"Tiers", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create empty PointProcess...", nullptr, 1, NEW1_PointProcess_createEmpty);
		praat_addMenuCommand (U"Objects", U"New", U"Create Poisson process...", nullptr, 1, NEW1_PointProcess_createPoissonProcess);
		praat_addMenuCommand (U"Objects", U"New", U"-- new tiers ---", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create PitchTier...", nullptr, 1, NEW1_PitchTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create FormantGrid...", nullptr, 1, NEW1_FormantGrid_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create FormantTier...", nullptr, praat_HIDDEN | praat_DEPTH_1, NEW1_FormantTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create IntensityTier...", nullptr, 1, NEW1_IntensityTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create DurationTier...", nullptr, 1, NEW1_DurationTier_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create AmplitudeTier...", nullptr, 1, NEW1_AmplitudeTier_create);

	praat_addAction1 (classAmplitudeTier, 0, U"AmplitudeTier help", nullptr, 0, HELP_AmplitudeTier_help);
	praat_addAction1 (classAmplitudeTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_AmplitudeTier_viewAndEdit);
	praat_addAction1 (classAmplitudeTier, 1,   U"Edit", nullptr, praat_DEPRECATED_2011, WINDOW_AmplitudeTier_viewAndEdit);
	praat_addAction1 (classAmplitudeTier, 0, U"View & Edit with Sound?", nullptr, 0, HINT_AmplitudeTier_Sound_viewAndEdit);
	praat_addAction1 (classAmplitudeTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classAmplitudeTier);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (local)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_local);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (local_dB)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_local_dB);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (apq3)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_apq3);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (apq5)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_apq5);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (apq11)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_apq11);
		praat_addAction1 (classAmplitudeTier, 1, U"Get shimmer (dda)...", nullptr, 1, REAL_AmplitudeTier_getShimmer_dda);
	praat_addAction1 (classAmplitudeTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classAmplitudeTier);
		praat_addAction1 (classAmplitudeTier, 0, U"Add point...", nullptr, 1, MODIFY_AmplitudeTier_addPoint);
		praat_addAction1 (classAmplitudeTier, 0, U"Formula...", nullptr, 1, MODIFY_AmplitudeTier_formula);
	praat_addAction1 (classAmplitudeTier, 0, U"Synthesize", nullptr, 0, nullptr);
		praat_addAction1 (classAmplitudeTier, 0, U"To Sound (pulse train)...", nullptr, 0, NEW_AmplitudeTier_to_Sound);
	praat_addAction1 (classAmplitudeTier, 0, U"Convert", nullptr, 0, nullptr);
		praat_addAction1 (classAmplitudeTier, 0, U"To IntensityTier...", nullptr, 0, NEW_AmplitudeTier_to_IntensityTier);
		praat_addAction1 (classAmplitudeTier, 0, U"Down to PointProcess", nullptr, 0, NEW_AmplitudeTier_downto_PointProcess);
		praat_addAction1 (classAmplitudeTier, 0, U"Down to TableOfReal", nullptr, 0, NEW_AmplitudeTier_downto_TableOfReal);

	praat_addAction1 (classDurationTier, 0, U"DurationTier help", nullptr, 0, HELP_DurationTier_help);
	praat_addAction1 (classDurationTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_DurationTier_edit);
	praat_addAction1 (classDurationTier, 1,   U"Edit", nullptr, praat_DEPRECATED_2011, WINDOW_DurationTier_edit);
	praat_addAction1 (classDurationTier, 0, U"View & Edit with Sound?", nullptr, 0, HINT_DurationTier_Sound_edit);
	praat_addAction1 (classDurationTier, 0, U"& Manipulation: Replace?", nullptr, 0, HINT_DurationTier_Manipulation_replace);
	praat_addAction1 (classDurationTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classDurationTier);
		praat_addAction1 (classDurationTier, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classDurationTier, 1, U"Get value at time...", nullptr, 1, REAL_DurationTier_getValueAtTime);
		praat_addAction1 (classDurationTier, 1, U"Get value at index...", nullptr, 1, REAL_DurationTier_getValueAtIndex);
		praat_addAction1 (classDurationTier, 1, U"Get target duration...", nullptr, 1, REAL_DurationTier_getTargetDuration);
	praat_addAction1 (classDurationTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classDurationTier);
		praat_addAction1 (classDurationTier, 0, U"Add point...", nullptr, 1, MODIFY_DurationTier_addPoint);
		praat_addAction1 (classDurationTier, 0, U"Formula...", nullptr, 1, MODIFY_DurationTier_formula);
	praat_addAction1 (classDurationTier, 0, U"Convert", nullptr, 0, nullptr);
		praat_addAction1 (classDurationTier, 0, U"Down to PointProcess", nullptr, 0, NEW_DurationTier_downto_PointProcess);

	praat_addAction1 (classFormantGrid, 0, U"FormantGrid help", nullptr, 0, HELP_FormantGrid_help);
	praat_addAction1 (classFormantGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_FormantGrid_edit);
	praat_addAction1 (classFormantGrid, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_FormantGrid_edit);
	praat_addAction1 (classFormantGrid, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classFormantGrid);
		praat_addAction1 (classFormantGrid, 0, U"Formula (frequencies)...", nullptr, 1, MODIFY_FormantGrid_formula_frequencies);
		//praat_addAction1 (classFormantGrid, 0, U"Formula (bandwidths)...", nullptr, 1, MODIFY_FormantGrid_formula_bandwidths);
		praat_addAction1 (classFormantGrid, 0, U"Add formant point...", nullptr, 1, MODIFY_FormantGrid_addFormantPoint);
		praat_addAction1 (classFormantGrid, 0, U"Add bandwidth point...", nullptr, 1, MODIFY_FormantGrid_addBandwidthPoint);
		praat_addAction1 (classFormantGrid, 0, U"Remove formant points between...", nullptr, 1, MODIFY_FormantGrid_removeFormantPointsBetween);
		praat_addAction1 (classFormantGrid, 0, U"Remove bandwidth points between...", nullptr, 1, MODIFY_FormantGrid_removeBandwidthPointsBetween);
	praat_addAction1 (classFormantGrid, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classFormantGrid, 0, U"To Formant...", nullptr, 1, NEW_FormantGrid_to_Formant);

	praat_addAction1 (classFormantTier, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classFormantTier, 0, U"Speckle...", nullptr, 1, GRAPHICS_FormantTier_speckle);
	praat_addAction1 (classFormantTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classFormantTier);
		praat_addAction1 (classFormantTier, 1, U"-- get value --", nullptr, 1, nullptr);
		praat_addAction1 (classFormantTier, 1, U"Get value at time...", nullptr, 1, REAL_FormantTier_getValueAtTime);
		praat_addAction1 (classFormantTier, 1, U"Get bandwidth at time...", nullptr, 1, REAL_FormantTier_getBandwidthAtTime);
	praat_addAction1 (classFormantTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classFormantTier);
		praat_addAction1 (classFormantTier, 0, U"Add point...", nullptr, 1, MODIFY_FormantTier_addPoint);
	praat_addAction1 (classFormantTier, 0, U"Down", nullptr, 0, nullptr);
		praat_addAction1 (classFormantTier, 0, U"Down to TableOfReal...", nullptr, 0, NEW_FormantTier_downto_TableOfReal);

	praat_addAction1 (classIntensityTier, 0, U"IntensityTier help", nullptr, 0, HELP_IntensityTier_help);
	praat_addAction1 (classIntensityTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_IntensityTier_viewAndEdit);
	praat_addAction1 (classIntensityTier, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_IntensityTier_viewAndEdit);
	praat_addAction1 (classIntensityTier, 0, U"View & Edit with Sound?", nullptr, 0, HINT_IntensityTier_Sound_viewAndEdit);
	praat_addAction1 (classIntensityTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classIntensityTier);
		praat_addAction1 (classIntensityTier, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classIntensityTier, 1, U"Get value at time...", nullptr, 1, REAL_IntensityTier_getValueAtTime);
		praat_addAction1 (classIntensityTier, 1, U"Get value at index...", nullptr, 1, REAL_IntensityTier_getValueAtIndex);
	praat_addAction1 (classIntensityTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classIntensityTier);
		praat_addAction1 (classIntensityTier, 0, U"Add point...", nullptr, 1, MODIFY_IntensityTier_addPoint);
		praat_addAction1 (classIntensityTier, 0, U"Formula...", nullptr, 1, MODIFY_IntensityTier_formula);
	praat_addAction1 (classIntensityTier, 0, U"Convert", nullptr, 0, nullptr);
		praat_addAction1 (classIntensityTier, 0, U"To AmplitudeTier", nullptr, 0, NEW_IntensityTier_to_AmplitudeTier);
		praat_addAction1 (classIntensityTier, 0, U"Down to PointProcess", nullptr, 0, NEW_IntensityTier_downto_PointProcess);
		praat_addAction1 (classIntensityTier, 0, U"Down to TableOfReal", nullptr, 0, NEW_IntensityTier_downto_TableOfReal);

	praat_addAction1 (classPitchTier, 1, U"Save as PitchTier spreadsheet file...", nullptr, 0, SAVE_PitchTier_writeToPitchTierSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1,   U"Write to PitchTier spreadsheet file...", U"*Save as PitchTier spreadsheet file...", praat_DEPRECATED_2011, SAVE_PitchTier_writeToPitchTierSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1, U"Save as headerless spreadsheet file...", nullptr, 0, SAVE_PitchTier_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classPitchTier, 1,   U"Write to headerless spreadsheet file...", U"*Save as headerless spreadsheet file...", praat_DEPRECATED_2011, SAVE_PitchTier_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classPitchTier, 0, U"PitchTier help", nullptr, 0, HELP_PitchTier_help);
	praat_addAction1 (classPitchTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_PitchTier_viewAndEdit);
	praat_addAction1 (classPitchTier, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_PitchTier_viewAndEdit);
	praat_addAction1 (classPitchTier, 0, U"View & Edit with Sound?", nullptr, 0, HINT_PitchTier_Sound_viewAndEdit);
	praat_addAction1 (classPitchTier, 0, U"Play pulses", nullptr, 0, PLAY_PitchTier_play);
	praat_addAction1 (classPitchTier, 0, U"Hum", nullptr, 0, PLAY_PitchTier_hum);
	praat_addAction1 (classPitchTier, 0, U"Play sine", nullptr, 0, PLAY_PitchTier_playSine);
	praat_addAction1 (classPitchTier, 0, U"Draw...", nullptr, 0, GRAPHICS_PitchTier_draw);
	praat_addAction1 (classPitchTier, 0, U"& Manipulation: Replace?", nullptr, 0, INFO_PitchTier_Manipulation_replace);
	praat_addAction1 (classPitchTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classPitchTier);
		praat_addAction1 (classPitchTier, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 1, U"Get value at time...", nullptr, 1, REAL_PitchTier_getValueAtTime);
		praat_addAction1 (classPitchTier, 1, U"Get value at index...", nullptr, 1, REAL_PitchTier_getValueAtIndex);
		praat_addAction1 (classPitchTier, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 1, U"Get mean (curve)...", nullptr, 1, REAL_PitchTier_getMean_curve);
		praat_addAction1 (classPitchTier, 1, U"Get mean (points)...", nullptr, 1, REAL_PitchTier_getMean_points);
		praat_addAction1 (classPitchTier, 1, U"Get standard deviation (curve)...", nullptr, 1, REAL_PitchTier_getStandardDeviation_curve);
		praat_addAction1 (classPitchTier, 1, U"Get standard deviation (points)...", nullptr, 1, REAL_PitchTier_getStandardDeviation_points);
	praat_addAction1 (classPitchTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classPitchTier);
		praat_addAction1 (classPitchTier, 0, U"Add point...", nullptr, 1, MODIFY_PitchTier_addPoint);
		praat_addAction1 (classPitchTier, 0, U"Formula...", nullptr, 1, MODIFY_PitchTier_formula);
		praat_addAction1 (classPitchTier, 0, U"-- stylize --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 0, U"Stylize...", nullptr, 1, MODIFY_PitchTier_stylize);
		praat_addAction1 (classPitchTier, 0, U"Interpolate quadratically...", nullptr, 1, MODIFY_PitchTier_interpolateQuadratically);
		praat_addAction1 (classPitchTier, 0, U"-- modify frequencies --", nullptr, 1, nullptr);
		praat_addAction1 (classPitchTier, 0, U"Shift frequencies...", nullptr, 1, MODIFY_PitchTier_shiftFrequencies);
		praat_addAction1 (classPitchTier, 0, U"Multiply frequencies...", nullptr, 1, MODIFY_PitchTier_multiplyFrequencies);
	praat_addAction1 (classPitchTier, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (classPitchTier, 0, U"To PointProcess", nullptr, 1, NEW_PitchTier_to_PointProcess);
		praat_addAction1 (classPitchTier, 0, U"To Sound (pulse train)...", nullptr, 1, NEW_PitchTier_to_Sound_pulseTrain);
		praat_addAction1 (classPitchTier, 0, U"To Sound (phonation)...", nullptr, 1, NEW_PitchTier_to_Sound_phonation);
		praat_addAction1 (classPitchTier, 0, U"To Sound (sine)...", nullptr, 1, NEW_PitchTier_to_Sound_sine);
	praat_addAction1 (classPitchTier, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classPitchTier, 0, U"Down to PointProcess", nullptr, 1, NEW_PitchTier_downto_PointProcess);
		praat_addAction1 (classPitchTier, 0, U"Down to TableOfReal...", nullptr, 1, NEW_PitchTier_downto_TableOfReal);

	praat_addAction1 (classPointProcess, 0, U"PointProcess help", nullptr, 0, HELP_PointProcess_help);
	praat_addAction1 (classPointProcess, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_PointProcess_viewAndEdit);
	praat_addAction1 (classPointProcess, 1,   U"View & Edit alone", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_PointProcess_viewAndEdit);
	praat_addAction1 (classPointProcess, 1,   U"Edit alone", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_PointProcess_viewAndEdit);
	praat_addAction1 (classPointProcess, 0, U"View & Edit with Sound?", nullptr, praat_NO_API, HINT_PointProcess_Sound_viewAndEdit);
	praat_addAction1 (classPointProcess, 0, U"Play -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"Play as pulse train", nullptr, 1, PLAY_PointProcess_play);
		praat_addAction1 (classPointProcess, 0, U"Hum", nullptr, 1, PLAY_PointProcess_hum);
		praat_addAction1 (classPointProcess, 0, U"-- to sound --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To Sound (pulse train)...", nullptr, 1, NEW_PointProcess_to_Sound_pulseTrain);
		praat_addAction1 (classPointProcess, 0, U"To Sound (phonation)...", nullptr, 1, NEW_PointProcess_to_Sound_phonation);
		praat_addAction1 (classPointProcess, 0, U"To Sound (hum)", nullptr, 1, NEW_PointProcess_to_Sound_hum);
	praat_addAction1 (classPointProcess, 0, U"Draw...", nullptr, 0, GRAPHICS_PointProcess_draw);
	praat_addAction1 (classPointProcess, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classPointProcess);
		praat_addAction1 (classPointProcess, 1, U"-- script get --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 1, U"Get number of points", nullptr, 1, INTEGER_PointProcess_getNumberOfPoints);
		praat_addAction1 (classPointProcess, 1, U"Get low index...", nullptr, 1, INTEGER_PointProcess_getLowIndex);
		praat_addAction1 (classPointProcess, 1, U"Get high index...", nullptr, 1, INTEGER_PointProcess_getHighIndex);
		praat_addAction1 (classPointProcess, 1, U"Get nearest index...", nullptr, 1, INTEGER_PointProcess_getNearestIndex);
		praat_addAction1 (classPointProcess, 1, U"Get time from index...", nullptr, 1, REAL_PointProcess_getTimeFromIndex);
		praat_addAction1 (classPointProcess, 1, U"Get interval...", nullptr, 1, REAL_PointProcess_getInterval);
		praat_addAction1 (classPointProcess, 1, U"-- periods --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 1, U"Get number of periods...", nullptr, 1, INTEGER_PointProcess_getNumberOfPeriods);
		praat_addAction1 (classPointProcess, 1, U"Get mean period...", nullptr, 1, REAL_PointProcess_getMeanPeriod);
		praat_addAction1 (classPointProcess, 1, U"Get stdev period...", nullptr, 1, REAL_PointProcess_getStdevPeriod);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (local)...", nullptr, 1, REAL_PointProcess_getJitter_local);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (local, absolute)...", nullptr, 1, REAL_PointProcess_getJitter_local_absolute);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (rap)...", nullptr, 1, REAL_PointProcess_getJitter_rap);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (ppq5)...", nullptr, 1, REAL_PointProcess_getJitter_ppq5);
		praat_addAction1 (classPointProcess, 1, U"Get jitter (ddp)...", nullptr, 1, REAL_PointProcess_getJitter_ddp);
	praat_addAction1 (classPointProcess, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classPointProcess);
		praat_addAction1 (classPointProcess, 0, U"Add point...", nullptr, 1, MODIFY_PointProcess_addPoint);
		praat_addAction1 (classPointProcess, 0, U"Remove point...", nullptr, 1, MODIFY_PointProcess_removePoint);
		praat_addAction1 (classPointProcess, 0, U"Remove point near...", nullptr, 1, MODIFY_PointProcess_removePointNear);
		praat_addAction1 (classPointProcess, 0, U"Remove points...", nullptr, 1, MODIFY_PointProcess_removePoints);
		praat_addAction1 (classPointProcess, 0, U"Remove points between...", nullptr, 1, MODIFY_PointProcess_removePointsBetween);
		praat_addAction1 (classPointProcess, 0, U"-- voice --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 0, U"Fill...", nullptr, 1, MODIFY_PointProcess_fill);
		praat_addAction1 (classPointProcess, 0, U"Voice...", nullptr, 1, MODIFY_PointProcess_voice);
	praat_addAction1 (classPointProcess, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To TextGrid...", nullptr, 1, NEW_PointProcess_to_TextGrid);
		praat_addAction1 (classPointProcess, 0, U"-- to single tier --", nullptr, 1, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To TextTier", nullptr, 1, NEW_PointProcess_to_TextTier);
		praat_addAction1 (classPointProcess, 0, U"To IntervalTier", nullptr, 1, NEW_PointProcess_to_IntervalTier);
	praat_addAction1 (classPointProcess, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"To PitchTier...", nullptr, 1, NEW_PointProcess_to_PitchTier);
		praat_addAction1 (classPointProcess, 0, U"To TextGrid (vuv)...", nullptr, 1, NEW_PointProcess_to_TextGrid_vuv);
	praat_addAction1 (classPointProcess, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 0, U"Hack", nullptr, 1, nullptr);
			praat_addAction1 (classPointProcess, 0, U"To Matrix", nullptr, 2, NEW_PointProcess_to_Matrix);
			praat_addAction1 (classPointProcess, 0, U"Up to TextTier...", nullptr, 2, NEW_PointProcess_upto_TextTier);
			praat_addAction1 (classPointProcess, 0, U"Up to PitchTier...", nullptr, 2, NEW_PointProcess_upto_PitchTier);
			praat_addAction1 (classPointProcess, 0, U"Up to IntensityTier...", nullptr, 2, NEW_PointProcess_upto_IntensityTier);
	praat_addAction1 (classPointProcess, 0, U"Combine -", nullptr, 0, nullptr);
		praat_addAction1 (classPointProcess, 2, U"Union", nullptr, 1, NEW1_PointProcesses_union);
		praat_addAction1 (classPointProcess, 2, U"Intersection", nullptr, 1, NEW1_PointProcesses_intersection);
		praat_addAction1 (classPointProcess, 2, U"Difference", nullptr, 1, NEW1_PointProcesses_difference);

	praat_addAction1 (classSpectrumTier, 0, U"Draw...", nullptr, 0, GRAPHICS_SpectrumTier_draw);
	praat_addAction1 (classSpectrumTier, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrumTier, 1, U"List...", nullptr, 1, LIST_SpectrumTier_list);
		praat_addAction1 (classSpectrumTier, 0, U"Down to Table", nullptr, 1, NEW_SpectrumTier_downto_Table);
	praat_addAction1 (classSpectrumTier, 0, U"Remove points below...", nullptr, 0, MODIFY_SpectrumTier_removePointsBelow);

	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_AmplitudeTier_viewAndEdit);
	praat_addAction2 (classAmplitudeTier, 1, classSound, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_AmplitudeTier_viewAndEdit);
	praat_addAction2 (classAmplitudeTier, 1, classSound, 1, U"Multiply", nullptr, 0, NEW1_Sound_AmplitudeTier_multiply);
	praat_addAction2 (classDurationTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_DurationTier_edit);
	praat_addAction2 (classDurationTier, 1, classSound, 1,   U"Edit", nullptr, praat_DEPRECATED_2011, WINDOW_DurationTier_edit);
	praat_addAction2 (classFormantGrid, 1, classSound, 1, U"Filter", nullptr, 0, NEW1_Sound_FormantGrid_filter);
	praat_addAction2 (classFormantGrid, 1, classSound, 1, U"Filter (no scale)", nullptr, 0, NEW1_Sound_FormantGrid_filter_noscale);
	praat_addAction2 (classFormantTier, 1, classSound, 1, U"Filter", nullptr, 0, NEW1_Sound_FormantTier_filter);
	praat_addAction2 (classFormantTier, 1, classSound, 1, U"Filter (no scale)", nullptr, 0, NEW1_Sound_FormantTier_filter_noscale);
	praat_addAction2 (classIntensityTier, 1, classPointProcess, 1, U"To IntensityTier", nullptr, 0, NEW1_IntensityTier_PointProcess_to_IntensityTier);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_IntensityTier_viewAndEdit);
	praat_addAction2 (classIntensityTier, 1, classSound, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_IntensityTier_viewAndEdit);
	praat_addAction2 (classIntensityTier, 1, classSound, 1, U"Multiply...", nullptr, 0, NEW1_Sound_IntensityTier_multiply);
	praat_addAction2 (classIntensityTier, 1, classSound, 1,   U"Multiply", U"*Multiply...", praat_DEPRECATED_2005, NEW1_Sound_IntensityTier_multiply_old);
	praat_addAction2 (classPitchTier, 1, classPointProcess, 1, U"To PitchTier", nullptr, 0, NEW1_PitchTier_PointProcess_to_PitchTier);
	praat_addAction2 (classPitchTier, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_PitchTier_viewAndEdit);
	praat_addAction2 (classPitchTier, 1, classSound, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_PitchTier_viewAndEdit);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_PointProcess_viewAndEdit);
	praat_addAction2 (classPointProcess, 1, classSound, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_PointProcess_viewAndEdit);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Query", nullptr, 0, nullptr);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (local)...", nullptr, 0, REAL_Point_Sound_getShimmer_local);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (local_dB)...", nullptr, 0, REAL_Point_Sound_getShimmer_local_dB);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (apq3)...", nullptr, 0, REAL_Point_Sound_getShimmer_apq3);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (apq5)...", nullptr, 0, REAL_Point_Sound_getShimmer_apq5);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (apq11)...", nullptr, 0, REAL_Point_Sound_getShimmer_apq11);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Get shimmer (dda)...", nullptr, 0, REAL_Point_Sound_getShimmer_dda);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Modify", nullptr, 0, nullptr);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"Transplant time domain", nullptr, 0, MODIFY_Point_Sound_transplantDomain);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Analyse", nullptr, 0, nullptr);
	/*praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Manipulation", nullptr, 0, NEW1_Sound_PointProcess_to_Manipulation);*/
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To AmplitudeTier (point)", nullptr, 0, NEW1_PointProcess_Sound_to_AmplitudeTier_point);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To AmplitudeTier (period)...", nullptr, 0, NEW1_PointProcess_Sound_to_AmplitudeTier_period);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Ltas...", nullptr, 0, NEW1_PointProcess_Sound_to_Ltas);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Ltas (only harmonics)...", nullptr, 0, NEW1_PointProcess_Sound_to_Ltas_harmonics);
praat_addAction2 (classPointProcess, 1, classSound, 1, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction2 (classPointProcess, 1, classSound, 1, U"To Sound ensemble...", nullptr, 0, NEW1_Sound_PointProcess_to_SoundEnsemble_correlate);
}

/* End of file praat_Tiers.cpp */
