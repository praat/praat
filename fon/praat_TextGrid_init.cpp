/* praat_TextGrid_init.cpp
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

#include "Pitch_AnyTier_to_PitchTier.h"
#include "SpectrumEditor.h"
#include "SpellingChecker.h"
#include "TextGridEditor.h"
#include "TextGrid_extensions.h"
#include "TextGrid_Sound.h"
#include "WordList.h"

#include "praat_TimeTier.h"
#include "praat_uvafon.h"

#undef iam
#define iam iam_LOOP

static const conststring32 STRING_FROM_FREQUENCY_HZ = U"left Frequency range (Hz)";
static const conststring32 STRING_TO_FREQUENCY_HZ = U"right Frequency range (Hz)";
static const conststring32 STRING_TIER_NUMBER = U"Tier number";
static const conststring32 STRING_INTERVAL_NUMBER = U"Interval number";
static const conststring32 STRING_POINT_NUMBER = U"Point number";

// MARK: - ANYTIER (generic)

DIRECT (NEW1_AnyTier_into_TextGrid) {
	CONVERT_LIST (Function)
		autoTextGrid result = TextGrid_createWithoutTiers (1e30, -1e30);
		for (integer i = 1; i <= list.size; i ++) {
			TextGrid_addTier_copy (result.get(), list.at [i]);
		}
	CONVERT_LIST_END (U"grid")
}

// MARK: - INTERVALTIER

FORM (NEW_IntervalTier_downto_TableOfReal, U"IntervalTier: Down to TableOfReal", nullptr) {
	SENTENCE (label, U"Label", U"")
	OK
DO
	CONVERT_EACH (IntervalTier)
		autoTableOfReal result = IntervalTier_downto_TableOfReal (me, label);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_IntervalTier_downto_TableOfReal_any) {
	CONVERT_EACH (IntervalTier)
		autoTableOfReal result = IntervalTier_downto_TableOfReal_any (me);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_IntervalTier_getCentrePoints, U"IntervalTier: Get centre points", nullptr) {
	SENTENCE (text, U"Text", U"")
	OK
DO
	CONVERT_EACH (IntervalTier)
		autoPointProcess result = IntervalTier_getCentrePoints (me, text);
	CONVERT_EACH_END (text)
}

FORM (NEW_IntervalTier_getEndPoints, U"IntervalTier: Get end points", nullptr) {
	SENTENCE (text, U"Text", U"")
	OK
DO
	CONVERT_EACH (IntervalTier)
		autoPointProcess result = IntervalTier_getEndPoints (me, text);
	CONVERT_EACH_END (text)
}

FORM (NEW_IntervalTier_getStartingPoints, U"IntervalTier: Get starting points", nullptr) {
	SENTENCE (text, U"Text", U"")
	OK
DO
	CONVERT_EACH (IntervalTier)
		autoPointProcess result = IntervalTier_getStartingPoints (me, text);
	CONVERT_EACH_END (text)
}

DIRECT (HELP_IntervalTier_help) {
	HELP (U"IntervalTier")
}

FORM_SAVE (SAVE_IntervalTier_writeToXwaves, U"Xwaves label file", nullptr, nullptr) {
	SAVE_ONE (IntervalTier)
		IntervalTier_writeToXwaves (me, file);
	SAVE_ONE_END
}

// MARK: - INTERVALTIER & POINTPROCESS

FORM (NEW1_IntervalTier_PointProcess_startToCentre, U"From start to centre", U"IntervalTier & PointProcess: Start to centre...") {
	REAL (phase, U"Phase (0-1)", U"0.5")
	OK
DO
	CONVERT_TWO (IntervalTier, PointProcess)
		autoPointProcess result = IntervalTier_PointProcess_startToCentre (me, you, phase);
	CONVERT_TWO_END (my name.get(), U"_", your name.get(), U"_", Melder_iround (100.0 * phase));
}

FORM (NEW1_IntervalTier_PointProcess_endToCentre, U"From end to centre", U"IntervalTier & PointProcess: End to centre...") {
	REAL (phase, U"Phase (0-1)", U"0.5")
	OK
DO
	CONVERT_TWO (IntervalTier, PointProcess)
		autoPointProcess result = IntervalTier_PointProcess_endToCentre (me, you, phase);
	CONVERT_TWO_END (my name.get(), U"_", your name.get(), U"_", Melder_iround (100.0 * phase));
}

// MARK: - LABEL (obsolete)

DIRECT (NEW1_Label_Sound_to_TextGrid) {
	CONVERT_TWO (Label, Sound)
		autoTextGrid result = Label_Function_to_TextGrid (me, you);
	CONVERT_TWO_END (your name.get())
}

DIRECT (HINT_Label_Sound_to_TextGrid) {
	INFO_NONE
		Melder_information (U"This is an old-style Label object. To turn it into a TextGrid, U"
			"select it together with a Sound of the appropriate duration, and click \"To TextGrid\".");
	INFO_NONE_END
}

// MARK: - PITCH & TEXTGRID

FORM (GRAPHICS_TextGrid_Pitch_draw, U"TextGrid & Pitch: Draw", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVE (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	POSITIVE (fontSize, U"Font size (points)", U"18")
	BOOLEAN (useTextStyles, U"Use text styles", true)
	OPTIONMENUx (textAlignment, U"Text alignment", 2, 0) OPTION (U"left") OPTION (U"centre") OPTION (U"right")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_draw (me, you, GRAPHICS, tierNumber, fromTime, toTime, fromFrequency, toFrequency,
			fontSize, useTextStyles, textAlignment, garnish, Pitch_speckle_NO, kPitch_unit::HERTZ);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_drawErb, U"TextGrid & Pitch: Draw erb", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (ERB)", U"0.0")
	REAL (toFrequency, U"right Frequency range (ERB)", U"10.0")
	POSITIVE (fontSize, U"Font size (points)", U"18")
	BOOLEAN (useTextStyles, U"Use text styles", true)
	OPTIONMENUx (textAlignment, U"Text alignment", 2, 0) OPTION (U"left") OPTION (U"centre") OPTION (U"right")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_draw (me, you, GRAPHICS, tierNumber, fromTime, toTime, fromFrequency, toFrequency,
			fontSize, useTextStyles, textAlignment, garnish, Pitch_speckle_NO, kPitch_unit::ERB);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_drawLogarithmic, U"TextGrid & Pitch: Draw logarithmic", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	POSITIVE (fontSize, U"Font size (points)", U"18")
	BOOLEAN (useTextStyles, U"Use text styles", true)
	OPTIONMENUx (textAlignment, U"Text alignment", 2, 0) OPTION (U"left") OPTION (U"centre") OPTION (U"right")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_draw (me, you, GRAPHICS, tierNumber, fromTime, toTime, fromFrequency, toFrequency,
			fontSize, useTextStyles, textAlignment, garnish, Pitch_speckle_NO, kPitch_unit::HERTZ_LOGARITHMIC);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_drawMel, U"TextGrid & Pitch: Draw mel", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"500.0")
	POSITIVE (fontSize, U"Font size (points)", U"18")
	BOOLEAN (useTextStyles, U"Use text styles", true)
	OPTIONMENUx (textAlignment, U"Text alignment", 2, 0) OPTION (U"left") OPTION (U"centre") OPTION (U"right")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_draw (me, you, GRAPHICS, tierNumber, fromTime, toTime, fromFrequency, toFrequency,
			fontSize, useTextStyles, textAlignment, garnish, Pitch_speckle_NO, kPitch_unit::MEL);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_drawSemitones, U"TextGrid & Pitch: Draw semitones", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	LABEL (U"Range in semitones re 100 hertz:")
	REAL (fromFrequency, U"left Frequency range (st)", U"-12.0")
	REAL (toFrequency, U"right Frequency range (st)", U"30.0")
	POSITIVE (fontSize, U"Font size (points)", U"18")
	BOOLEAN (useTextStyles, U"Use text styles", true)
	OPTIONMENUx (textAlignment, U"Text alignment", 2, 0) OPTION (U"left") OPTION (U"centre") OPTION (U"right")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_draw (me, you, GRAPHICS, tierNumber, fromTime, toTime, fromFrequency, toFrequency,
			fontSize, useTextStyles, textAlignment, garnish, Pitch_speckle_NO, kPitch_unit::SEMITONES_100);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_drawSeparately, U"TextGrid & Pitch: Draw separately", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"0.0")
	REAL (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_NO, kPitch_unit::HERTZ);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_drawSeparatelyErb, U"TextGrid & Pitch: Draw separately erb", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (ERB)", U"0.0")
	REAL (toFrequency, U"right Frequency range (ERB)", U"10.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_NO, kPitch_unit::ERB);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_drawSeparatelyLogarithmic, U"TextGrid & Pitch: Draw separately logarithmic", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_NO, kPitch_unit::HERTZ_LOGARITHMIC);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_drawSeparatelyMel, U"TextGrid & Pitch: Draw separately mel", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"500.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_NO, kPitch_unit::MEL);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_drawSeparatelySemitones, U"TextGrid & Pitch: Draw separately semitones", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	LABEL (U"Range in semitones re 100 hertz:")
	REAL (fromFrequency, U"left Frequency range (st)", U"-12.0")
	REAL (toFrequency, U"right Frequency range (st)", U"30.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_NO, kPitch_unit::SEMITONES_100);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_speckle, U"TextGrid & Pitch: Speckle", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVE (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_YES, kPitch_unit::HERTZ);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_speckleErb, U"TextGrid & Pitch: Speckle erb", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (ERB)", U"0.0")
	REAL (toFrequency, U"right Frequency range (ERB)", U"10.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_YES, kPitch_unit::ERB);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_speckleLogarithmic, U"TextGrid & Pitch: Speckle logarithmic", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_YES, kPitch_unit::HERTZ_LOGARITHMIC);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_speckleMel, U"TextGrid & Pitch: Speckle mel", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"500.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_YES, kPitch_unit::MEL);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_speckleSemitones, U"TextGrid & Pitch: Speckle semitones", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	LABEL (U"Range in semitones re 100 hertz:")
	REAL (fromFrequency, U"left Frequency range (st)", U"-12.0")
	REAL (toFrequency, U"right Frequency range (st)", U"30.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_YES, kPitch_unit::SEMITONES_100);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_speckleSeparately, U"TextGrid & Pitch: Speckle separately", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"0.0")
	REAL (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_YES, kPitch_unit::HERTZ);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_speckleSeparatelyErb, U"TextGrid & Pitch: Speckle separately erb", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (ERB)", U"0.0")
	REAL (toFrequency, U"right Frequency range (ERB)", U"10.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_YES, kPitch_unit::ERB);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_speckleSeparatelyLogarithmic, U"TextGrid & Pitch: Speckle separately logarithmic", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_YES, kPitch_unit::HERTZ_LOGARITHMIC);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_speckleSeparatelyMel, U"TextGrid & Pitch: Speckle separately mel", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"500.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_YES, kPitch_unit::MEL);
	GRAPHICS_TWO_END
}

FORM (GRAPHICS_TextGrid_Pitch_speckleSeparatelySemitones, U"TextGrid & Pitch: Speckle separately semitones", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	LABEL (U"Range in semitones re 100 hertz:")
	REAL (fromFrequency, U"left Frequency range (st)", U"-12.0")
	REAL (toFrequency, U"right Frequency range (st)", U"30.0")
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Pitch)
		TextGrid_Pitch_drawSeparately (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			showBoundaries, useTextStyles, garnish, Pitch_speckle_YES, kPitch_unit::MEL);
	GRAPHICS_TWO_END
}

// MARK: - PITCH & TEXTTIER

FORM (NEW1_Pitch_TextTier_to_PitchTier, U"Pitch & TextTier to PitchTier", U"Pitch & TextTier: To PitchTier...") {
	RADIOx (unvoicedStrategy, U"Unvoiced strategy", 3, 0)
		RADIOBUTTON (U"zero")
		RADIOBUTTON (U"error")
		RADIOBUTTON (U"interpolate")
	OK
DO
	CONVERT_TWO (Pitch, TextTier)
		autoPitchTier result = Pitch_AnyTier_to_PitchTier (me, you->asAnyTier(), unvoicedStrategy);
	CONVERT_TWO_END (my name.get())
}

// MARK: - SOUND & TEXTGRID

FORM (GRAPHICS_TextGrid_Sound_draw, U"TextGrid & Sound: Draw...", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_TWO (TextGrid, Sound)
		TextGrid_Sound_draw (me, you, GRAPHICS, fromTime, toTime, showBoundaries, useTextStyles, garnish);
	GRAPHICS_TWO_END
}

FORM (NEW1_TextGrid_Sound_extractAllIntervals, U"TextGrid & Sound: Extract all intervals", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	BOOLEAN (preserveTimes, U"Preserve times", false)
	OK
DO
	CONVERT_TWO (TextGrid, Sound)
		autoSoundList result = TextGrid_Sound_extractAllIntervals (me, you, tierNumber, preserveTimes);
		result -> classInfo = classCollection;   // YUCK, in order to force automatic unpacking
	CONVERT_TWO_END (U"dummy")
}

FORM (NEW1_TextGrid_Sound_extractNonemptyIntervals, U"TextGrid & Sound: Extract non-empty intervals", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	BOOLEAN (preserveTimes, U"Preserve times", false)
	OK
DO
	CONVERT_TWO (TextGrid, Sound)
		autoSoundList result = TextGrid_Sound_extractNonemptyIntervals (me, you, tierNumber, preserveTimes);
		result -> classInfo = classCollection;   // YUCK, in order to force automatic unpacking
	CONVERT_TWO_END (U"dummy")
}

FORM (NEW1_TextGrid_Sound_extractIntervals, U"TextGrid & Sound: Extract intervals", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	BOOLEAN (preserveTimes, U"Preserve times", false)
	SENTENCE (labelText, U"Label text", U"")
	OK
DO
	CONVERT_TWO (TextGrid, Sound)
		autoSoundList result = TextGrid_Sound_extractIntervalsWhere (me, you,
			tierNumber, kMelder_string::EQUAL_TO, labelText, preserveTimes);
		result -> classInfo = classCollection;   // YUCK, in order to force automatic unpacking
	CONVERT_TWO_END (U"dummy")
}

FORM (NEW1_TextGrid_Sound_extractIntervalsWhere, U"TextGrid & Sound: Extract intervals", nullptr) {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	BOOLEAN (preserveTimes, U"Preserve times", false)
	OPTIONMENU_ENUM (kMelder_string, extractEveryIntervalWhoseLabel___,
			U"Extract every interval whose label...", kMelder_string::DEFAULT)
	SENTENCE (__theText, U"...the text", U"")
	OK
DO
	CONVERT_TWO (TextGrid, Sound)
		autoSoundList result = TextGrid_Sound_extractIntervalsWhere (me, you, tierNumber,
			extractEveryIntervalWhoseLabel___, __theText, preserveTimes);
		result -> classInfo = classCollection;   // YUCK, in order to force automatic unpacking
	CONVERT_TWO_END (U"dummy")
}

DIRECT (MODIFY_TextGrid_Sound_scaleTimes) {
	MODIFY_FIRST_OF_TWO (TextGrid, Sound)
		Function_scaleXTo (me, your xmin, your xmax);
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (MODIFY_TextGrid_Sound_cloneTimeDomain) {
	MODIFY_FIRST_OF_TWO (Sound, TextGrid)
		my x1 += your xmin - your xmin;
		my xmin = your xmin;
		my xmax = your xmax;
	MODIFY_FIRST_OF_TWO_END
}

// MARK: - SPELLINGCHECKER

FORM (MODIFY_SpellingChecker_addNewWord, U"Add word to user dictionary", U"SpellingChecker") {
	SENTENCE (newWord, U"New word", U"")
	OK
DO
	MODIFY_EACH (SpellingChecker)
		SpellingChecker_addNewWord (me, newWord);
	MODIFY_EACH_END
}

FORM (WINDOW_SpellingChecker_viewAndEdit, U"Edit spelling checker", U"SpellingChecker") {
	LABEL (U"-- Syntax --")
	SENTENCE (forbiddenStrings, U"Forbidden strings", U"")
	BOOLEAN (checkMatchingParentheses, U"Check matching parentheses", false)
	SENTENCE (separatingCharacters, U"Separating characters", U"")
	BOOLEAN (allowAllParenthesized, U"Allow all parenthesized", false)
	LABEL (U"-- Capitals --")
	BOOLEAN (allowAllNames, U"Allow all names", false)
	SENTENCE (namePrefixes, U"Name prefixes", U"")
	BOOLEAN (allowAllAbbreviations, U"Allow all abbreviations", false)
	LABEL (U"-- Capitalization --")
	BOOLEAN (allowCapsSentenceInitially, U"Allow caps sentence-initially", false)
	BOOLEAN (allowCapsAfterColon, U"Allow caps after colon", false)
	LABEL (U"-- Word parts --")
	SENTENCE (allowAllWordsContaining, U"Allow all words containing", U"")
	SENTENCE (allowAllWordsStartingWith, U"Allow all words starting with", U"")
	SENTENCE (allowAllWordsEndingIn, U"Allow all words ending in", U"")
OK
	FIND_ONE (SpellingChecker)
		SET_STRING (forbiddenStrings, my forbiddenStrings.get())
		SET_BOOLEAN (checkMatchingParentheses, my checkMatchingParentheses)
		SET_STRING (separatingCharacters, my separatingCharacters.get())
		SET_BOOLEAN (allowAllParenthesized, my allowAllParenthesized)
		SET_BOOLEAN (allowAllNames, my allowAllNames)
		SET_STRING (namePrefixes, my namePrefixes.get())
		SET_BOOLEAN (allowAllAbbreviations, my allowAllAbbreviations)
		SET_BOOLEAN (allowCapsSentenceInitially, my allowCapsSentenceInitially)
		SET_BOOLEAN (allowCapsAfterColon, my allowCapsAfterColon)
		SET_STRING (allowAllWordsContaining, my allowAllWordsContaining.get())
		SET_STRING (allowAllWordsStartingWith, my allowAllWordsStartingWith.get())
		SET_STRING (allowAllWordsEndingIn, my allowAllWordsEndingIn.get())
DO
	MODIFY_EACH (SpellingChecker)
		my forbiddenStrings = Melder_dup (forbiddenStrings);
		my checkMatchingParentheses = checkMatchingParentheses;
		my separatingCharacters = Melder_dup (separatingCharacters);
		my allowAllParenthesized = allowAllParenthesized;
		my allowAllNames = allowAllNames;
		my namePrefixes = Melder_dup (namePrefixes);
		my allowAllAbbreviations = allowAllAbbreviations;
		my allowCapsSentenceInitially = allowCapsSentenceInitially;
		my allowCapsAfterColon = allowCapsAfterColon;
		my allowAllWordsContaining = Melder_dup (allowAllWordsContaining);
		my allowAllWordsStartingWith = Melder_dup (allowAllWordsStartingWith);
		my allowAllWordsEndingIn = Melder_dup (allowAllWordsEndingIn);
	MODIFY_EACH_END
}

DIRECT (NEW_SpellingChecker_extractWordList) {
	CONVERT_EACH (SpellingChecker)
		autoWordList result = SpellingChecker_extractWordList (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_SpellingChecker_extractUserDictionary) {
	CONVERT_EACH (SpellingChecker)
		autoStringSet result = SpellingChecker_extractUserDictionary (me);
	CONVERT_EACH_END (my name.get())
}

FORM (BOOLEAN_SpellingChecker_isWordAllowed, U"Is word allowed?", U"SpellingChecker") {
	SENTENCE (word, U"Word", U"")
	OK
DO
	NUMBER_ONE (SpellingChecker)
		integer result = SpellingChecker_isWordAllowed (me, word);
	NUMBER_ONE_END (result ? U" (allowed)" : U" (not allowed)")
}

FORM (STRING_SpellingChecker_nextNotAllowedWord, U"Next not allowed word?", U"SpellingChecker") {
	TEXTFIELD (sentence, U"Sentence:", U"")
	INTEGER (startingCharacter, U"Starting character", U"0")
	OK
DO
	STRING_ONE (SpellingChecker)
		if (startingCharacter < 0)
			Melder_throw (U"Your starting character should be 0 or positive.");
		if (startingCharacter > (int) str32len (sentence))
			Melder_throw (U"Your starting character should not exceed the end of the sentence.");
		conststring32 result = SpellingChecker_nextNotAllowedWord (me, sentence, & startingCharacter);
	STRING_ONE_END
}

DIRECT (MODIFY_SpellingChecker_replaceWordList) {
	MODIFY_FIRST_OF_TWO (SpellingChecker, WordList)
		SpellingChecker_replaceWordList (me, you);
	MODIFY_FIRST_OF_TWO_END
}

DIRECT (HINT_SpellingChecker_replaceWordList_help) {
	INFO_NONE
		Melder_information (U"To replace the checker's word list\nby the contents of a Strings object:\n"
			U"1. select the Strings;\n2. convert to a WordList object;\n3. select the SpellingChecker and the WordList;\n"
			U"4. choose Replace.");
	INFO_NONE_END
}

DIRECT (MODIFY_SpellingChecker_replaceUserDictionary) {
	MODIFY_FIRST_OF_TWO (SpellingChecker, StringSet)
		SpellingChecker_replaceUserDictionary (me, you);
	MODIFY_FIRST_OF_TWO_END
}

// MARK: - TEXTGRID

// MARK: Save

FORM_SAVE (SAVE_TextGrid_writeToChronologicalTextFile, U"Text file", nullptr, nullptr) {
	SAVE_ONE (TextGrid)
		TextGrid_writeToChronologicalTextFile (me, file);
	SAVE_ONE_END
}

// MARK: Help

DIRECT (HELP_TextGrid_help) {
	HELP (U"TextGrid")
}

// MARK: View & Edit

static void cb_TextGridEditor_publication (Editor /* editor */, autoDaata publication) {
	/*
	 * Keep the gate for error handling.
	 */
	try {
		bool isaSpectralSlice = Thing_isa (publication.get(), classSpectrum) && str32equ (Thing_getName (publication.get()), U"slice");
		praat_new (publication.move());
		praat_updateSelection ();
		if (isaSpectralSlice) {
			int IOBJECT;
			FIND_ONE_WITH_IOBJECT (Spectrum)
			autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
			praat_installEditor (editor2.get(), IOBJECT);
			editor2.releaseToUser();
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT (WINDOW_TextGrid_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a TextGrid from batch.");
	FIND_TWO_WITH_IOBJECT (TextGrid, Sound)   // Sound may be NULL
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, you, true, nullptr, nullptr);
		Editor_setPublicationCallback (editor.get(), cb_TextGridEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

FORM (WINDOW_TextGrid_viewAndEditWithCallback, U"TextGrid: View & Edit with callback", nullptr) {
	SENTENCE (callbackText, U"Callback text", U"r1")
	OK
DO
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a TextGrid from batch.");
	FIND_TWO_WITH_IOBJECT (TextGrid, Sound)   // Sound may be NULL
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, you, true, nullptr, callbackText);
		Editor_setPublicationCallback (editor.get(), cb_TextGridEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

DIRECT (WINDOW_TextGrid_LongSound_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a TextGrid from batch.");
	LongSound longSound = nullptr;
	int ilongSound = 0;
	LOOP {
		if (CLASS == classLongSound) longSound = (LongSound) OBJECT, ilongSound = IOBJECT;
	}
	Melder_assert (ilongSound != 0);
	LOOP if (CLASS == classTextGrid) {
		iam (TextGrid);
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, longSound, false, nullptr, nullptr);
		Editor_setPublicationCallback (editor.get(), cb_TextGridEditor_publication);
		praat_installEditor2 (editor.get(), IOBJECT, ilongSound);
		editor.releaseToUser();
	}
END }

DIRECT (WINDOW_TextGrid_SpellingChecker_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a TextGrid from batch.");
	SpellingChecker spellingChecker = nullptr;
	int ispellingChecker = 0;
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classSpellingChecker) spellingChecker = (SpellingChecker) OBJECT, ispellingChecker = IOBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay null
	}
	Melder_assert (ispellingChecker != 0);
	LOOP if (CLASS == classTextGrid) {
		iam (TextGrid);
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, sound, true, spellingChecker, nullptr);
		praat_installEditor2 (editor.get(), IOBJECT, ispellingChecker);
		editor.releaseToUser();
	}
END }

DIRECT (WINDOW_TextGrid_LongSound_SpellingChecker_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a TextGrid from batch.");
	LongSound longSound = nullptr;
	SpellingChecker spellingChecker = nullptr;
	int ilongSound = 0, ispellingChecker = 0;
	LOOP {
		if (CLASS == classLongSound) longSound = (LongSound) OBJECT, ilongSound = IOBJECT;
		if (CLASS == classSpellingChecker) spellingChecker = (SpellingChecker) OBJECT, ispellingChecker = IOBJECT;
	}
	Melder_assert (ilongSound != 0 && ispellingChecker != 0);
	LOOP if (CLASS == classTextGrid) {
		iam (TextGrid);
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, longSound, false, spellingChecker, nullptr);
		praat_installEditor3 (editor.get(), IOBJECT, ilongSound, ispellingChecker);
		editor.releaseToUser();
	}
END }

DIRECT (HINT_TextGrid_Sound_viewAndEdit) {
	INFO_NONE
		Melder_information (U"To include a copy of a Sound in your TextGrid window:\n"
			U"   select a TextGrid and a Sound, and click \"View & Edit\".");
	INFO_NONE_END
}

// MARK: Draw

FORM (GRAPHICS_TextGrid_draw, U"TextGrid: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	BOOLEAN (showBoundaries, U"Show boundaries", true)
	BOOLEAN (useTextStyles, U"Use text styles", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (TextGrid)
		TextGrid_Sound_draw (me, nullptr, GRAPHICS, fromTime, toTime, showBoundaries, useTextStyles, garnish);
	GRAPHICS_EACH_END
}

DIRECT (HINT_TextGrid_Sound_draw) {
	INFO_NONE
		Melder_information (U"You can draw a TextGrid together with a Sound after selecting them both.");
	INFO_NONE_END
}

DIRECT (HINT_TextGrid_Pitch_draw) {
	INFO_NONE
		Melder_information (U"You can draw a TextGrid together with a Pitch after selecting them both.");
	INFO_NONE_END
}

// MARK: Tabulate

FORM (LIST_TextGrid_list, U"TextGrid: List", nullptr) {
	BOOLEAN (includeLineNumber, U"Include line number", false)
	NATURAL (timeDecimals, U"Time decimals", U"6")
	BOOLEAN (includeTierNames, U"Include tier names", true)
	BOOLEAN (includeEmptyIntervals, U"Include empty intervals", false)
	OK
DO
	INFO_ONE (TextGrid)
		TextGrid_list (me, includeLineNumber, timeDecimals, includeTierNames, includeEmptyIntervals);
	INFO_ONE_END
}

FORM (NEW_TextGrid_downto_Table, U"TextGrid: Down to Table", nullptr) {
	BOOLEAN (includeLineNumber, U"Include line number", false)
	NATURAL (timeDecimals, U"Time decimals", U"6")
	BOOLEAN (includeTierNames, U"Include tier names", true)
	BOOLEAN (includeEmptyIntervals, U"Include empty intervals", false)
	OK
DO
	CONVERT_EACH (TextGrid)
		autoTable result = TextGrid_downto_Table (me, includeLineNumber, timeDecimals,
			includeTierNames, includeEmptyIntervals);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_TextGrid_tabulateOccurrences, U"TextGrid: Tabulate occurrences", nullptr) {
	NUMVEC (searchTiers, U"Search tiers:", U"{ 1, 2 }")
	OPTIONMENU_ENUM (kMelder_string, listEveryLabelThat___,
			U"List every label that...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hello")
	BOOLEAN (caseSensitive, U"Case-sensitive", false)
	OK
DO
	CONVERT_EACH (TextGrid)
		autoTable result = TextGrid_tabulateOccurrences (me, searchTiers, listEveryLabelThat___, ___theText, caseSensitive);
	CONVERT_EACH_END (my name.get(), U"_", ___theText)
}


// MARK: Query

DIRECT (INTEGER_TextGrid_getNumberOfTiers) {
	NUMBER_ONE (TextGrid)
		integer result = my tiers->size;
	NUMBER_ONE_END (U" tiers")
}

inline static void pr_TextGrid_checkTierNumber (TextGrid me, integer tierNumber) {
	if (tierNumber > my tiers->size)
		Melder_throw (U"Your tier number (", tierNumber,
			U") should not be greater than the number of tiers (", my tiers->size, U").");
}

inline static Function pr_TextGrid_peekTier (TextGrid me, integer tierNumber) {
	pr_TextGrid_checkTierNumber (me, tierNumber);
	return my tiers->at [tierNumber];
}

FORM (STRING_TextGrid_getTierName, U"TextGrid: Get tier name", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OK
DO
	STRING_ONE (TextGrid)
		Function tier = pr_TextGrid_peekTier (me, tierNumber);
		conststring32 result = tier -> name.get();
	STRING_ONE_END
}

FORM (BOOLEAN_TextGrid_isIntervalTier, U"TextGrid: Is interval tier?", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OK
DO
	NUMBER_ONE (TextGrid)
		Function tier = pr_TextGrid_peekTier (me, tierNumber);
		integer result = ( tier -> classInfo == classIntervalTier );
	NUMBER_ONE_END (result ? U" (yes, tier " : U" (no, tier ", tierNumber,
		result ? U" is an interval tier)" : U" is a point tier)")
}

static IntervalTier pr_TextGrid_peekIntervalTier (TextGrid me, integer tierNumber) {
	Function tier = pr_TextGrid_peekTier (me, tierNumber);
	if (tier -> classInfo != classIntervalTier)
		Melder_throw (U"Your tier should be an interval tier.");
	return (IntervalTier) tier;
}

FORM (INTEGER_TextGrid_getNumberOfIntervals, U"TextGrid: Get number of intervals", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OK
DO
	NUMBER_ONE (TextGrid)
		IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (me, tierNumber);
		integer result = intervalTier -> intervals.size;
	NUMBER_ONE_END (U" intervals")
}

static TextInterval pr_TextGrid_peekInterval (TextGrid me, integer tierNumber, integer intervalNumber) {
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (me, tierNumber);
	if (intervalNumber > intervalTier -> intervals.size) Melder_throw (U"Interval number too large.");
	return intervalTier -> intervals.at [intervalNumber];
}

FORM (REAL_TextGrid_getStartTimeOfInterval, U"TextGrid: Get start time of interval", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (intervalNumber, STRING_INTERVAL_NUMBER, U"1")
	OK
DO
	NUMBER_ONE (TextGrid)
		TextInterval interval = pr_TextGrid_peekInterval (me, tierNumber, intervalNumber);
		double result = interval -> xmin;
	NUMBER_ONE_END (U" seconds")
}

FORM (REAL_TextGrid_getEndTimeOfInterval, U"TextGrid: Get end time of interval", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (intervalNumber, STRING_INTERVAL_NUMBER, U"1")
	OK
DO
	NUMBER_ONE (TextGrid)
		TextInterval interval = pr_TextGrid_peekInterval (me, tierNumber, intervalNumber);
		double result = interval -> xmax;
	NUMBER_ONE_END (U" seconds")
}

FORM (STRING_TextGrid_getLabelOfInterval, U"TextGrid: Get label of interval", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (intervalNumber, STRING_INTERVAL_NUMBER, U"1")
	OK
DO
	STRING_ONE (TextGrid)
		TextInterval interval = pr_TextGrid_peekInterval (me, tierNumber, intervalNumber);
		conststring32 result = interval -> text.get();
	STRING_ONE_END
}

FORM (INTEGER_TextGrid_getIntervalAtTime, U"TextGrid: Get interval at time", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (TextGrid)
		IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (me, tierNumber);
		integer result = IntervalTier_timeToIndex (intervalTier, time);
	NUMBER_ONE_END (U" (interval number)")
}

FORM (INTEGER_TextGrid_getLowIntervalAtTime, U"TextGrid: Get low interval at time", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (TextGrid)
		IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (me, tierNumber);
		integer result = IntervalTier_timeToHighIndex (intervalTier, time);
	NUMBER_ONE_END (U" (low interval)")
}

FORM (INTEGER_TextGrid_getHighIntervalAtTime, U"TextGrid: Get high interval at time", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (TextGrid)
		IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (me, tierNumber);
		integer result = IntervalTier_timeToLowIndex (intervalTier, time);
	NUMBER_ONE_END (U" (high interval)")
}

FORM (INTEGER_TextGrid_getIntervalEdgeFromTime, U"TextGrid: Get interval edge from time", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (TextGrid)
		IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (me, tierNumber);
		integer result = IntervalTier_hasTime (intervalTier, time);
	NUMBER_ONE_END (U" (interval edge)")
}

FORM (INTEGER_TextGrid_getIntervalBoundaryFromTime, U"TextGrid: Get interval boundary from time", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (TextGrid)
		IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (me, tierNumber);
		integer result = IntervalTier_hasBoundary (intervalTier, time);
	NUMBER_ONE_END (U" (interval boundary)")
}

FORM (INTEGER_TextGrid_countIntervalsWhere, U"Count intervals", U"TextGrid: Count intervals where...") {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (kMelder_string, countIntervalsWhoseLabel___,
			U"Count intervals whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	NUMBER_ONE (TextGrid)
		integer result = TextGrid_countIntervalsWhere (me, tierNumber, countIntervalsWhoseLabel___, ___theText);
	NUMBER_ONE_END (U" intervals containing ", ___theText);
}

static TextTier pr_TextGrid_peekTextTier (TextGrid me, integer tierNumber) {
	Function tier = pr_TextGrid_peekTier (me, tierNumber);
	if (! tier) return nullptr;
	if (tier -> classInfo != classTextTier) Melder_throw (U"Your tier should be a point tier (TextTier).");
	return (TextTier) tier;
}

FORM (INTEGER_TextGrid_getNumberOfPoints, U"TextGrid: Get number of points", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OK
DO
	NUMBER_ONE (TextGrid)
		TextTier textTier = pr_TextGrid_peekTextTier (me, tierNumber);
		integer result = textTier -> points.size;
	NUMBER_ONE_END (U" (points")
}

static TextPoint pr_TextGrid_peekPoint (TextGrid me, integer tierNumber, integer pointNumber) {
	TextTier textTier = pr_TextGrid_peekTextTier (me, tierNumber);
	if (pointNumber > textTier -> points.size) Melder_throw (U"Point number too large.");
	return textTier -> points.at [pointNumber];
}

FORM (REAL_TextGrid_getTimeOfPoint, U"TextGrid: Get time of point", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (pointNumber, STRING_POINT_NUMBER, U"1")
	OK
DO
	NUMBER_ONE (TextGrid)
		TextPoint point = pr_TextGrid_peekPoint (me, tierNumber, pointNumber);
		double result = point -> number;
	NUMBER_ONE_END (U" seconds")
}

FORM (STRING_TextGrid_getLabelOfPoint, U"TextGrid: Get label of point", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (pointNumber, STRING_POINT_NUMBER, U"1")
	OK
DO
	STRING_ONE (TextGrid)
		TextPoint point = pr_TextGrid_peekPoint (me, tierNumber, pointNumber);
		conststring32 result = point -> mark.get();
	STRING_ONE_END
}

FORM (INTEGER_TextGrid_getLowIndexFromTime, U"Get low index", U"AnyTier: Get low index from time...") {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (TextGrid)
		TextTier textTier = pr_TextGrid_peekTextTier (me, tierNumber);
		integer result = AnyTier_timeToLowIndex (textTier->asAnyTier(), time);
	NUMBER_ONE_END (U" (low index)")
}

FORM (INTEGER_TextGrid_getHighIndexFromTime, U"Get high index", U"AnyTier: Get high index from time...") {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (TextGrid)
		TextTier textTier = pr_TextGrid_peekTextTier (me, tierNumber);
		integer result = AnyTier_timeToHighIndex (textTier->asAnyTier(), time);
	NUMBER_ONE_END (U" (high index)")
}

FORM (INTEGER_TextGrid_getNearestIndexFromTime, U"Get nearest index", U"AnyTier: Get nearest index from time...") {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (TextGrid)
		TextTier textTier = pr_TextGrid_peekTextTier (me, tierNumber);
		integer result = AnyTier_timeToNearestIndex (textTier->asAnyTier(), time);
	NUMBER_ONE_END (U" (nearest index)")
}

FORM (INTEGER_TextGrid_countPointsWhere, U"Count points", U"TextGrid: Count points where...") {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (kMelder_string, countPointsWhoseLabel___,
			U"Count points whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	NUMBER_ONE (TextGrid)
		integer result = TextGrid_countPointsWhere (me, tierNumber, countPointsWhoseLabel___, ___theText);
	NUMBER_ONE_END (U" points containing ", ___theText);
}

FORM (INTEGER_TextGrid_countLabels, U"Count labels", U"TextGrid: Count labels...") {
	INTEGER (tierNumber, STRING_TIER_NUMBER, U"1")
	SENTENCE (labelText, U"Label text", U"a")
	OK
DO
	NUMBER_ONE (TextGrid)
		integer result = TextGrid_countLabels (me, tierNumber, labelText);
	NUMBER_ONE_END (U" labels ", labelText)
}

// MARK: Modify

DIRECT (MODIFY_TextGrid_convertToBackslashTrigraphs) {
	MODIFY_EACH (TextGrid)
		TextGrid_convertToBackslashTrigraphs (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_TextGrid_convertToUnicode) {
	MODIFY_EACH (TextGrid)
		TextGrid_convertToUnicode (me);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_insertIntervalTier, U"TextGrid: Insert interval tier", nullptr) {
	NATURAL (position, U"Position", U"1 (= at top)")
	WORD (name, U"Name", U"")
	OK
DO
	MODIFY_EACH (TextGrid)
		{// scope
			autoIntervalTier tier = IntervalTier_create (my xmin, my xmax);
			if (position > my tiers->size) position = my tiers->size + 1;
			Thing_setName (tier.get(), name);
			my tiers -> addItemAtPosition_move (tier.move(), position);
		}
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_insertPointTier, U"TextGrid: Insert point tier", nullptr) {
	NATURAL (position, U"Position", U"1 (= at top)")
	WORD (name, U"Name", U"")
	OK
DO
	MODIFY_EACH (TextGrid)
		{// scope
			autoTextTier tier = TextTier_create (my xmin, my xmax);
			if (position > my tiers->size) position = my tiers->size + 1;
			Thing_setName (tier.get(), name);
			my tiers -> addItemAtPosition_move (tier.move(), position);
		}
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_duplicateTier, U"TextGrid: Duplicate tier", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (position, U"Position", U"1 (= at top)")
	WORD (name, U"Name", U"")
	OK
DO
	MODIFY_EACH (TextGrid)
		if (tierNumber > my tiers->size) tierNumber = my tiers->size;
		{// scope
			autoFunction newTier = Data_copy (my tiers->at [tierNumber]);
			Thing_setName (newTier.get(), name);
			my tiers -> addItemAtPosition_move (newTier.move(), position);
		}
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_removeTier, U"TextGrid: Remove tier", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OK
DO
	MODIFY_EACH (TextGrid)
		if (my tiers->size <= 1)
			Melder_throw (U"Sorry, I refuse to remove the last tier.");
		if (tierNumber > my tiers->size) tierNumber = my tiers->size;
		my tiers -> removeItem (tierNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_insertBoundary, U"TextGrid: Insert boundary", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_insertBoundary (me, tierNumber, time);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_removeLeftBoundary, U"TextGrid: Remove left boundary", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (intervalNumber, STRING_INTERVAL_NUMBER, U"2")
	OK
DO
	MODIFY_EACH (TextGrid)
		IntervalTier intervalTier;
		if (tierNumber > my tiers->size)
			Melder_throw (U"You cannot remove a boundary from tier ", tierNumber, U" of ", me,
				U", because that TextGrid has only ", my tiers->size, U" tiers.");
		intervalTier = (IntervalTier) my tiers->at [tierNumber];
		if (intervalTier -> classInfo != classIntervalTier)
			Melder_throw (U"You cannot remove a boundary from tier ", tierNumber, U" of ", me,
				U", because that tier is a point tier instead of an interval tier.");
		if (intervalNumber > intervalTier -> intervals.size)
			Melder_throw (U"You cannot remove a boundary from interval ", intervalNumber, U" of tier ", tierNumber, U" of ", me,
				U", because that tier has only ", intervalTier -> intervals.size, U" intervals.");
		if (intervalNumber == 1)
			Melder_throw (U"You cannot remove the left boundary from interval 1 of tier ", tierNumber, U" of ", me,
				U", because this is at the left edge of the tier.");
		IntervalTier_removeLeftBoundary (intervalTier, intervalNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_removeRightBoundary, U"TextGrid: Remove right boundary", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (intervalNumber, STRING_INTERVAL_NUMBER, U"2")
	OK
DO
	MODIFY_EACH (TextGrid)
		IntervalTier intervalTier;
		if (tierNumber > my tiers->size)
			Melder_throw (U"You cannot remove a boundary from tier ", tierNumber, U" of ", me,
				U", because that TextGrid has only ", my tiers->size, U" tiers.");
		intervalTier = (IntervalTier) my tiers->at [tierNumber];
		if (intervalTier -> classInfo != classIntervalTier)
			Melder_throw (U"You cannot remove a boundary from tier ", tierNumber, U" of ", me,
				U", because that tier is a point tier instead of an interval tier.");
		if (intervalNumber > intervalTier -> intervals.size)
			Melder_throw (U"You cannot remove a boundary from interval ", intervalNumber, U" of tier ", tierNumber, U" of ", me,
				U", because that tier has only ", intervalTier -> intervals.size, U" intervals.");
		if (intervalNumber == intervalTier -> intervals.size)
			Melder_throw (U"You cannot remove the right boundary from interval ", intervalNumber, U" of tier ", tierNumber, U" of ", me,
				U", because this is at the right edge of the tier.");
		IntervalTier_removeLeftBoundary (intervalTier, intervalNumber + 1);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_removeBoundaryAtTime, U"TextGrid: Remove boundary at time", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_removeBoundaryAtTime (me, tierNumber, time);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_setIntervalText, U"TextGrid: Set interval text", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (intervalNumber, STRING_INTERVAL_NUMBER, U"1")
	TEXTFIELD (text, U"Text:", U"")
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_setIntervalText (me, tierNumber, intervalNumber, text);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_insertPoint, U"TextGrid: Insert point", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	REAL (time, U"Time (s)", U"0.5")
	TEXTFIELD (text, U"Text:", U"")
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_insertPoint (me, tierNumber, time, text);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_removePoint, U"TextGrid: Remove point", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (pointNumber, STRING_POINT_NUMBER, U"2")
	OK
DO
	MODIFY_EACH (TextGrid)
		TextTier pointTier;
		if (tierNumber > my tiers->size)
			Melder_throw (U"You cannot remove a point from tier ", tierNumber, U" of ", me,
				U", because that TextGrid has only ", my tiers->size, U" tiers.");
		pointTier = (TextTier) my tiers->at [tierNumber];
		if (pointTier -> classInfo != classTextTier)
			Melder_throw (U"You cannot remove a point from tier ", tierNumber, U" of ", me,
				U", because that tier is an interval tier instead of a point tier.");
		if (pointNumber > pointTier -> points.size)
			Melder_throw (U"You cannot remove point ", pointNumber, U" from tier ", tierNumber, U" of ", me,
				U", because that tier has only ", pointTier -> points.size, U" points.");
		TextTier_removePoint (pointTier, pointNumber);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_removePoints, U"Remove points", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (kMelder_string, removeEveryPointWhoseLabel___,
			U"Remove every point whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_removePoints (me, tierNumber, removeEveryPointWhoseLabel___, ___theText);
	MODIFY_EACH_END
}

FORM (MODIFY_TextGrid_setPointText, U"TextGrid: Set point text", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	NATURAL (pointNumber, STRING_POINT_NUMBER, U"1")
	TEXTFIELD (text, U"Text:", U"")
	OK
DO
	MODIFY_EACH (TextGrid)
		TextGrid_setPointText (me, tierNumber, pointNumber, text);
	MODIFY_EACH_END
}

// MARK: Analyse

FORM (NEW1_TextGrid_extractOneTier, U"TextGrid: Extract one tier", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OK
DO
	CONVERT_EACH (TextGrid)
		Function tier = pr_TextGrid_peekTier (me, tierNumber);
		autoTextGrid result = TextGrid_createWithoutTiers (1e30, -1e30);
		TextGrid_addTier_copy (result.get(), tier);   // no transfer of tier ownership, because a copy is made
	CONVERT_EACH_END (tier -> name.get())
}

FORM (NEW1_TextGrid_extractTier, U"TextGrid: Extract tier", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OK
DO
	CONVERT_EACH (TextGrid)
		Function tier = pr_TextGrid_peekTier (me, tierNumber);
		autoFunction result = Data_copy (tier);
	CONVERT_EACH_END (tier -> name.get())
}

FORM (NEW_TextGrid_extractPart, U"TextGrid: Extract part", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"1.0")
	BOOLEAN (preserveTimes, U"Preserve times", false)
	OK
DO
	CONVERT_EACH (TextGrid)
		autoTextGrid result = TextGrid_extractPart (me, fromTime, toTime, preserveTimes);
	CONVERT_EACH_END (my name.get(), U"_part")
}

FORM (NEW_TextGrid_getStartingPoints, U"TextGrid: Get starting points", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (kMelder_string, getStartingPointsWhoseLabel___,
			U"Get starting points whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	CONVERT_EACH (TextGrid)
		autoPointProcess result = TextGrid_getStartingPoints (me, tierNumber, getStartingPointsWhoseLabel___, ___theText);
	CONVERT_EACH_END (my name.get(), U"_", ___theText)
}

FORM (NEW_TextGrid_getEndPoints, U"TextGrid: Get end points", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (kMelder_string, getEndPointsWhoseLabel___,
			U"Get end points whose label", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	CONVERT_EACH (TextGrid)
		autoPointProcess result = TextGrid_getEndPoints (me, tierNumber, getEndPointsWhoseLabel___, ___theText);
	CONVERT_EACH_END (my name.get(), U"_", ___theText)
}

FORM (NEW_TextGrid_getCentrePoints, U"TextGrid: Get centre points", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (kMelder_string, getCentrePointsWhoseLabel___,
			U"Get centre points whose label", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	CONVERT_EACH (TextGrid)
		autoPointProcess result = TextGrid_getCentrePoints (me, tierNumber, getCentrePointsWhoseLabel___, ___theText);
	CONVERT_EACH_END (my name.get(), U"_", ___theText)
}

FORM (NEW_TextGrid_getPoints, U"Get points", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (kMelder_string, getPointsWhoseLabel___,
			U"Get points whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OK
DO
	CONVERT_EACH (TextGrid)
		autoPointProcess result = TextGrid_getPoints (me, tierNumber, getPointsWhoseLabel___, ___theText);
	CONVERT_EACH_END (my name.get(), U"_", ___theText)
}

FORM (NEW_TextGrid_getPoints_preceded, U"Get points (preceded)", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (kMelder_string, getPointsWhoseLabel___,
			U"Get points whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"there")
	OPTIONMENU_ENUM (kMelder_string, ___precededByALabelThat___,
			U"...preceded by a label that...", kMelder_string::DEFAULT)
	SENTENCE (____theText, U" ...the text", U"hi")
	OK
DO
	CONVERT_EACH (TextGrid)
		autoPointProcess result = TextGrid_getPoints_preceded (me, tierNumber,
			(kMelder_string) getPointsWhoseLabel___, ___theText, ___precededByALabelThat___, ____theText);
	CONVERT_EACH_END (my name.get(), U"_", ___theText)
}

FORM (NEW_TextGrid_getPoints_followed, U"Get points (followed)", nullptr) {
	NATURAL (tierNumber, STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (kMelder_string, getPointsWhoseLabel___,
			U"Get points whose label...", kMelder_string::DEFAULT)
	SENTENCE (___theText, U"...the text", U"hi")
	OPTIONMENU_ENUM (kMelder_string, ___followedByALabelThat___,
			U"...followed by a label that...", kMelder_string::DEFAULT)
	SENTENCE (____theText, U" ...the text", U"there")
	OK
DO
	CONVERT_EACH (TextGrid)
		autoPointProcess result = TextGrid_getPoints_followed (me, tierNumber,
			(kMelder_string) getPointsWhoseLabel___, ___theText, ___followedByALabelThat___, ____theText);
	CONVERT_EACH_END (my name.get(), U"_", ___theText)
}

// MARK: Synthesize

DIRECT (NEW1_TextGrids_merge) {
	CONVERT_LIST (TextGrid)
		autoTextGrid result = TextGrids_merge (& list);
	CONVERT_LIST_END (U"merged")
}

DIRECT (NEW1_TextGrids_concatenate) {
	CONVERT_LIST (TextGrid)
		autoTextGrid result = TextGrids_concatenate (& list);
	CONVERT_LIST_END (U"chain")
}

// MARK: - TEXTGRID & ANYTIER

DIRECT (NEW1_TextGrid_IntervalTier_append) {
	CONVERT_TWO (TextGrid, IntervalTier)
		autoTextGrid result = Data_copy (me);
		TextGrid_addTier_copy (result.get(), you);
	CONVERT_TWO_END (my name.get())
}

DIRECT (NEW1_TextGrid_TextTier_append) {
	CONVERT_TWO (TextGrid, TextTier)
		autoTextGrid result = Data_copy (me);
		TextGrid_addTier_copy (result.get(), you);
	CONVERT_TWO_END (my name.get())
}

// MARK: - TEXTGRID & LONGSOUND

DIRECT (MODIFY_TextGrid_LongSound_scaleTimes) {
	MODIFY_FIRST_OF_TWO (TextGrid, LongSound)
		Function_scaleXTo (me, your xmin, your xmax);
	MODIFY_FIRST_OF_TWO_END
}

// MARK: - TEXTTIER

FORM (MODIFY_TextTier_addPoint, U"TextTier: Add point", U"TextTier: Add point...") {
	REAL (time, U"Time (s)", U"0.5")
	SENTENCE (text, U"Text", U"")
	OK
DO
	MODIFY_EACH (TextTier)
		TextTier_addPoint (me, time, text);
	MODIFY_EACH_END
}

DIRECT (NEW_TextTier_downto_PointProcess) {
	CONVERT_EACH (TextTier)
		autoPointProcess result = AnyTier_downto_PointProcess (me->asAnyTier());
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_TextTier_downto_TableOfReal, U"TextTier: Down to TableOfReal", nullptr) {
	SENTENCE (label, U"Label", U"")
	OK
DO
	CONVERT_EACH (TextTier)
		autoTableOfReal result = TextTier_downto_TableOfReal (me, label);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_TextTier_downto_TableOfReal_any) {
	CONVERT_EACH (TextTier)
		autoTableOfReal result = TextTier_downto_TableOfReal_any (me);
	CONVERT_EACH_END (my name.get())
}

FORM (STRING_TextTier_getLabelOfPoint, U"Get label of point", nullptr) {
	NATURAL (pointNumber, U"Point number", U"1")
	OK
DO
	STRING_ONE (TextTier)
		if (pointNumber > my points.size) Melder_throw (U"No such point.");
		TextPoint point = my points.at [pointNumber];
		conststring32 result = point -> mark.get();
	STRING_ONE_END
}

FORM (NEW_TextTier_getPoints, U"Get points", nullptr) {
	SENTENCE (text, U"Text", U"")
	OK
DO
	CONVERT_EACH (TextTier)
		autoPointProcess result = TextTier_getPoints (me, text);
	CONVERT_EACH_END (text)
}

DIRECT (HELP_TextTier_help) {
	HELP (U"TextTier")
}

// MARK: - WORDLIST

FORM (BOOLEAN_WordList_hasWord, U"Does word occur in list?", U"WordList") {
	SENTENCE (word, U"Word", U"")
	OK
DO
	NUMBER_ONE (WordList)
		integer result = WordList_hasWord (me, word);
	NUMBER_ONE_END (result ? U" (present)" : U" (absent)")
}

DIRECT (NEW_WordList_to_Strings) {
	CONVERT_EACH (WordList)
		autoStrings result = WordList_to_Strings (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_WordList_upto_SpellingChecker) {
	CONVERT_EACH (WordList)
		autoSpellingChecker result = WordList_upto_SpellingChecker (me);
	CONVERT_EACH_END (my name.get())
}

/***** buttons *****/

void praat_uvafon_TextGrid_init () {
	Thing_recognizeClassByOtherName (classTextTier, U"MarkTier");

	structTextGridEditor :: f_preferences ();

	praat_addAction1 (classIntervalTier, 1, U"Save as Xwaves label file...", nullptr, 0, SAVE_IntervalTier_writeToXwaves);
	praat_addAction1 (classIntervalTier, 1,   U"Write to Xwaves label file...", nullptr, praat_DEPRECATED_2011, SAVE_IntervalTier_writeToXwaves);
	praat_addAction1 (classIntervalTier, 0, U"IntervalTier help", nullptr, 0, HELP_IntervalTier_help);
	praat_addAction1 (classIntervalTier, 0, U"Collect", nullptr, 0, nullptr);
	praat_addAction1 (classIntervalTier, 0, U"Into TextGrid", nullptr, 0, NEW1_AnyTier_into_TextGrid);
	praat_addAction1 (classIntervalTier, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classIntervalTier, 0, U"Get starting points...", nullptr, 0, NEW_IntervalTier_getStartingPoints);
	praat_addAction1 (classIntervalTier, 0, U"Get centre points...", nullptr, 0, NEW_IntervalTier_getCentrePoints);
	praat_addAction1 (classIntervalTier, 0, U"Get end points...", nullptr, 0, NEW_IntervalTier_getEndPoints);
	praat_addAction1 (classIntervalTier, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classIntervalTier, 0, U"Down to TableOfReal (any)", nullptr, 0, NEW_IntervalTier_downto_TableOfReal_any);
	praat_addAction1 (classIntervalTier, 0, U"Down to TableOfReal...", nullptr, 0, NEW_IntervalTier_downto_TableOfReal);

	praat_addAction1 (classLabel, 0, U"& Sound: To TextGrid?", nullptr, 0, HINT_Label_Sound_to_TextGrid);

	praat_addAction1 (classSpellingChecker, 1, U"View & Edit...", nullptr, praat_ATTRACTIVE, WINDOW_SpellingChecker_viewAndEdit);
	praat_addAction1 (classSpellingChecker, 1,   U"Edit...", U"*View & Edit...", praat_DEPRECATED_2011, WINDOW_SpellingChecker_viewAndEdit);
	praat_addAction1 (classSpellingChecker, 0, U"Query", nullptr, 0, nullptr);
	praat_addAction1 (classSpellingChecker, 1, U"Is word allowed...", nullptr, 0, BOOLEAN_SpellingChecker_isWordAllowed);
	praat_addAction1 (classSpellingChecker, 1, U"Next not allowed word...", nullptr, 0, STRING_SpellingChecker_nextNotAllowedWord);
	praat_addAction1 (classSpellingChecker, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classSpellingChecker, 0, U"Add new word...", nullptr, 0, MODIFY_SpellingChecker_addNewWord);
	praat_addAction1 (classSpellingChecker, 0, U"Analyze", nullptr, 0, nullptr);
	praat_addAction1 (classSpellingChecker, 0, U"Extract WordList", nullptr, 0, NEW_SpellingChecker_extractWordList);
	praat_addAction1 (classSpellingChecker, 0, U"Extract user dictionary", nullptr, 0, NEW_SpellingChecker_extractUserDictionary);

	praat_addAction1 (classTextGrid, 1, U"Save as chronological text file...", nullptr, 0, SAVE_TextGrid_writeToChronologicalTextFile);
	praat_addAction1 (classTextGrid, 1,   U"Write to chronological text file...", nullptr, praat_HIDDEN, SAVE_TextGrid_writeToChronologicalTextFile);
	praat_addAction1 (classTextGrid, 0, U"TextGrid help", nullptr, 0, HELP_TextGrid_help);
	praat_addAction1 (classTextGrid, 1, U"View & Edit alone", nullptr, 0, WINDOW_TextGrid_viewAndEdit);
	praat_addAction1 (classTextGrid, 1,   U"View & Edit", U"*View & Edit alone", praat_DEPRECATED_2011, WINDOW_TextGrid_viewAndEdit);
	praat_addAction1 (classTextGrid, 1,   U"Edit", U"*View & Edit alone", praat_DEPRECATED_2011, WINDOW_TextGrid_viewAndEdit);
	praat_addAction1 (classTextGrid, 1, U"View & Edit with Sound?", nullptr, praat_ATTRACTIVE | praat_NO_API, HINT_TextGrid_Sound_viewAndEdit);
	praat_addAction1 (classTextGrid, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classTextGrid, 0, U"Draw...", nullptr, 1, GRAPHICS_TextGrid_draw);
	praat_addAction1 (classTextGrid, 1, U"Draw with Sound?", nullptr, 1, HINT_TextGrid_Sound_draw);
	praat_addAction1 (classTextGrid, 1, U"Draw with Pitch?", nullptr, 1, HINT_TextGrid_Pitch_draw);
	praat_addAction1 (classTextGrid, 1, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classTextGrid, 0, U"Down to Table...", nullptr, 1, NEW_TextGrid_downto_Table);
		praat_addAction1 (classTextGrid, 1, U"List...", nullptr, 1, LIST_TextGrid_list);
		praat_addAction1 (classTextGrid, 0, U"Tabulate occurrences...", nullptr, 1, NEW_TextGrid_tabulateOccurrences);
	praat_addAction1 (classTextGrid, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classTextGrid);
		praat_addAction1 (classTextGrid, 1, U"-- query textgrid --", nullptr, 1, nullptr);
		praat_addAction1 (classTextGrid, 1, U"Get number of tiers", nullptr, 1, INTEGER_TextGrid_getNumberOfTiers);
		praat_addAction1 (classTextGrid, 1, U"Get tier name...", nullptr, 1, STRING_TextGrid_getTierName);
		praat_addAction1 (classTextGrid, 1, U"Is interval tier...", nullptr, 1, BOOLEAN_TextGrid_isIntervalTier);
		praat_addAction1 (classTextGrid, 1, U"-- query tier --", nullptr, 1, nullptr);
		praat_addAction1 (classTextGrid, 1, U"Query interval tier", nullptr, 1, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Get number of intervals...", nullptr, praat_DEPTH_2, INTEGER_TextGrid_getNumberOfIntervals);
			praat_addAction1 (classTextGrid, 1, U"Get start time of interval...", nullptr, praat_DEPTH_2, REAL_TextGrid_getStartTimeOfInterval);
			praat_addAction1 (classTextGrid, 1,   U"Get starting point...", U"*Get start time of interval...", praat_DEPTH_2 | praat_DEPRECATED_2016, REAL_TextGrid_getStartTimeOfInterval);
			praat_addAction1 (classTextGrid, 1,   U"Get start point...", U"*Get start time of interval...", praat_DEPTH_2 | praat_DEPRECATED_2016, REAL_TextGrid_getStartTimeOfInterval);
			praat_addAction1 (classTextGrid, 1, U"Get end time of interval...", nullptr, praat_DEPTH_2, REAL_TextGrid_getEndTimeOfInterval);
			praat_addAction1 (classTextGrid, 1,   U"Get end point...", U"*Get end time of interval...", praat_DEPTH_2 | praat_DEPRECATED_2016, REAL_TextGrid_getEndTimeOfInterval);
			praat_addAction1 (classTextGrid, 1, U"Get label of interval...", nullptr, praat_DEPTH_2, STRING_TextGrid_getLabelOfInterval);
			praat_addAction1 (classTextGrid, 1, U"-- query interval from time --", nullptr, praat_DEPTH_2, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Get interval at time...", nullptr, 2, INTEGER_TextGrid_getIntervalAtTime);
			praat_addAction1 (classTextGrid, 1, U"Get low interval at time...", nullptr, 2, INTEGER_TextGrid_getLowIntervalAtTime);
			praat_addAction1 (classTextGrid, 1, U"Get high interval at time...", nullptr, 2, INTEGER_TextGrid_getHighIntervalAtTime);
			praat_addAction1 (classTextGrid, 1, U"Get interval edge from time...", nullptr, 2, INTEGER_TextGrid_getIntervalEdgeFromTime);
			praat_addAction1 (classTextGrid, 1, U"Get interval boundary from time...", nullptr, 2, INTEGER_TextGrid_getIntervalBoundaryFromTime);
			praat_addAction1 (classTextGrid, 1, U"-- query interval labels --", nullptr, 2, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Count intervals where...", nullptr, 2, INTEGER_TextGrid_countIntervalsWhere);
		praat_addAction1 (classTextGrid, 1, U"Query point tier", nullptr, 1, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Get number of points...", nullptr, 2, INTEGER_TextGrid_getNumberOfPoints);
			praat_addAction1 (classTextGrid, 1, U"Get time of point...", nullptr, 2, REAL_TextGrid_getTimeOfPoint);
			praat_addAction1 (classTextGrid, 1, U"Get label of point...", nullptr, 2, STRING_TextGrid_getLabelOfPoint);
			praat_addAction1 (classTextGrid, 1, U"-- query point from time --", nullptr, 2, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Get low index from time...", nullptr, 2, INTEGER_TextGrid_getLowIndexFromTime);
			praat_addAction1 (classTextGrid, 1, U"Get high index from time...", nullptr, 2, INTEGER_TextGrid_getHighIndexFromTime);
			praat_addAction1 (classTextGrid, 1, U"Get nearest index from time...", nullptr, 2, INTEGER_TextGrid_getNearestIndexFromTime);
			praat_addAction1 (classTextGrid, 1, U"-- query point labels --", nullptr, 2, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Count points where...", nullptr, 2, INTEGER_TextGrid_countPointsWhere);
		praat_addAction1 (classTextGrid, 1, U"-- query labels --", nullptr, praat_DEPTH_1 | praat_DEPRECATED_2015, nullptr);
		praat_addAction1 (classTextGrid, 1, U"Count labels...", nullptr, praat_DEPTH_1 | praat_DEPRECATED_2015, INTEGER_TextGrid_countLabels);
	praat_addAction1 (classTextGrid, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classTextGrid, 0, U"Convert to backslash trigraphs", nullptr, 1, MODIFY_TextGrid_convertToBackslashTrigraphs);
		praat_addAction1 (classTextGrid, 0, U"Convert to Unicode", nullptr, 1, MODIFY_TextGrid_convertToUnicode);
		praat_TimeFunction_modify_init (classTextGrid);
		praat_addAction1 (classTextGrid, 0, U"-- modify tiers --", nullptr, 1, nullptr);
		praat_addAction1 (classTextGrid, 0, U"Insert interval tier...", nullptr, 1, MODIFY_TextGrid_insertIntervalTier);
		praat_addAction1 (classTextGrid, 0, U"Insert point tier...", nullptr, 1, MODIFY_TextGrid_insertPointTier);
		praat_addAction1 (classTextGrid, 0, U"Duplicate tier...", nullptr, 1, MODIFY_TextGrid_duplicateTier);
		praat_addAction1 (classTextGrid, 0, U"Remove tier...", nullptr, 1, MODIFY_TextGrid_removeTier);
		praat_addAction1 (classTextGrid, 1, U"-- modify tier --", nullptr, 1, nullptr);
		praat_addAction1 (classTextGrid, 0, U"Modify interval tier", nullptr, 1, nullptr);
			praat_addAction1 (classTextGrid, 0, U"Insert boundary...", nullptr, 2, MODIFY_TextGrid_insertBoundary);
			praat_addAction1 (classTextGrid, 0, U"Remove left boundary...", nullptr, 2, MODIFY_TextGrid_removeLeftBoundary);
			praat_addAction1 (classTextGrid, 0, U"Remove right boundary...", nullptr, 2, MODIFY_TextGrid_removeRightBoundary);
			praat_addAction1 (classTextGrid, 0, U"Remove boundary at time...", nullptr, 2, MODIFY_TextGrid_removeBoundaryAtTime);
			praat_addAction1 (classTextGrid, 0, U"Set interval text...", nullptr, 2, MODIFY_TextGrid_setIntervalText);
		praat_addAction1 (classTextGrid, 0, U"Modify point tier", nullptr, 1, nullptr);
			praat_addAction1 (classTextGrid, 0, U"Insert point...", nullptr, 2, MODIFY_TextGrid_insertPoint);
			praat_addAction1 (classTextGrid, 0, U"Remove point...", nullptr, 2, MODIFY_TextGrid_removePoint);
			praat_addAction1 (classTextGrid, 0, U"Remove points...", nullptr, 2, MODIFY_TextGrid_removePoints);
			praat_addAction1 (classTextGrid, 0, U"Set point text...", nullptr, 2, MODIFY_TextGrid_setPointText);
praat_addAction1 (classTextGrid, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classTextGrid, 0, U"Extract one tier...", nullptr, 0, NEW1_TextGrid_extractOneTier);
	praat_addAction1 (classTextGrid, 0,   U"Extract tier...", U"*Extract one tier...", praat_DEPRECATED_2010, NEW1_TextGrid_extractTier);
	praat_addAction1 (classTextGrid, 0, U"Extract part...", nullptr, 0, NEW_TextGrid_extractPart);
	praat_addAction1 (classTextGrid, 0, U"Analyse interval tier -", nullptr, 0, nullptr);
		praat_addAction1 (classTextGrid, 0, U"Get starting points...", nullptr, 1, NEW_TextGrid_getStartingPoints);
		praat_addAction1 (classTextGrid, 0, U"Get end points...", nullptr, 1, NEW_TextGrid_getEndPoints);
		praat_addAction1 (classTextGrid, 0, U"Get centre points...", nullptr, 1, NEW_TextGrid_getCentrePoints);
	praat_addAction1 (classTextGrid, 0, U"Analyse point tier -", nullptr, 0, nullptr);
		praat_addAction1 (classTextGrid, 0, U"Get points...", nullptr, 1, NEW_TextGrid_getPoints);
		praat_addAction1 (classTextGrid, 0, U"Get points (preceded)...", nullptr, 1, NEW_TextGrid_getPoints_preceded);
		praat_addAction1 (classTextGrid, 0, U"Get points (followed)...", nullptr, 1, NEW_TextGrid_getPoints_followed);
praat_addAction1 (classTextGrid, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classTextGrid, 0, U"Merge", nullptr, 0, NEW1_TextGrids_merge);
	praat_addAction1 (classTextGrid, 0, U"Concatenate", nullptr, 0, NEW1_TextGrids_concatenate);

	praat_addAction1 (classTextTier, 0, U"TextTier help", nullptr, 0, HELP_TextTier_help);
	praat_addAction1 (classTextTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classTextTier);
		praat_addAction1 (classTextTier, 0, U"Get label of point...", nullptr, 1, STRING_TextTier_getLabelOfPoint);
	praat_addAction1 (classTextTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classTextTier);
		praat_addAction1 (classTextTier, 0, U"Add point...", nullptr, 1, MODIFY_TextTier_addPoint);
	praat_addAction1 (classTextTier, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classTextTier, 0, U"Get points...", nullptr, 0, NEW_TextTier_getPoints);
	praat_addAction1 (classTextTier, 0, U"Collect", nullptr, 0, nullptr);
	praat_addAction1 (classTextTier, 0, U"Into TextGrid", nullptr, 0, NEW1_AnyTier_into_TextGrid);
	praat_addAction1 (classTextTier, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classTextTier, 0, U"Down to PointProcess", nullptr, 0, NEW_TextTier_downto_PointProcess);
	praat_addAction1 (classTextTier, 0, U"Down to TableOfReal (any)", nullptr, 0, NEW_TextTier_downto_TableOfReal_any);
	praat_addAction1 (classTextTier, 0, U"Down to TableOfReal...", nullptr, 0, NEW_TextTier_downto_TableOfReal);

	praat_addAction1 (classWordList, 0, U"Query", nullptr, 0, nullptr);
		praat_addAction1 (classWordList, 1, U"Has word...", nullptr, 0, BOOLEAN_WordList_hasWord);
	praat_addAction1 (classWordList, 0, U"Analyze", nullptr, 0, nullptr);
		praat_addAction1 (classWordList, 0, U"To Strings", nullptr, 0, NEW_WordList_to_Strings);
	praat_addAction1 (classWordList, 0, U"Synthesize", nullptr, 0, nullptr);
		praat_addAction1 (classWordList, 0, U"Up to SpellingChecker", nullptr, 0, NEW_WordList_upto_SpellingChecker);

	praat_addAction2 (classIntervalTier, 1, classPointProcess, 1, U"Start to centre...", nullptr, 0, NEW1_IntervalTier_PointProcess_startToCentre);
	praat_addAction2 (classIntervalTier, 1, classPointProcess, 1, U"End to centre...", nullptr, 0, NEW1_IntervalTier_PointProcess_endToCentre);
	praat_addAction2 (classIntervalTier, 0, classTextTier, 0, U"Collect", nullptr, 0, nullptr);
	praat_addAction2 (classIntervalTier, 0, classTextTier, 0, U"Into TextGrid", nullptr, 0, NEW1_AnyTier_into_TextGrid);
	praat_addAction2 (classLabel, 1, classSound, 1, U"To TextGrid", nullptr, 0, NEW1_Label_Sound_to_TextGrid);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_TextGrid_LongSound_viewAndEdit);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_TextGrid_LongSound_viewAndEdit);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, U"Scale times", nullptr, 0, MODIFY_TextGrid_LongSound_scaleTimes);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw -", nullptr, 0, nullptr);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw...", nullptr, 1, GRAPHICS_TextGrid_Pitch_draw);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw logarithmic...", nullptr, 1, GRAPHICS_TextGrid_Pitch_drawLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw semitones...", nullptr, 1, GRAPHICS_TextGrid_Pitch_drawSemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw mel...", nullptr, 1, GRAPHICS_TextGrid_Pitch_drawMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw erb...", nullptr, 1, GRAPHICS_TextGrid_Pitch_drawErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle...", nullptr, 1, GRAPHICS_TextGrid_Pitch_speckle);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle logarithmic...", nullptr, 1, GRAPHICS_TextGrid_Pitch_speckleLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle semitones...", nullptr, 1, GRAPHICS_TextGrid_Pitch_speckleSemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle mel...", nullptr, 1, GRAPHICS_TextGrid_Pitch_speckleMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle erb...", nullptr, 1, GRAPHICS_TextGrid_Pitch_speckleErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"-- draw separately --", nullptr, 1, nullptr);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw separately...", nullptr, 1, GRAPHICS_TextGrid_Pitch_drawSeparately);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw separately (logarithmic)...", nullptr, 1, GRAPHICS_TextGrid_Pitch_drawSeparatelyLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw separately (semitones)...", nullptr, 1, GRAPHICS_TextGrid_Pitch_drawSeparatelySemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw separately (mel)...", nullptr, 1, GRAPHICS_TextGrid_Pitch_drawSeparatelyMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw separately (erb)...", nullptr, 1, GRAPHICS_TextGrid_Pitch_drawSeparatelyErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle separately...", nullptr, 1, GRAPHICS_TextGrid_Pitch_speckleSeparately);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle separately (logarithmic)...", nullptr, 1, GRAPHICS_TextGrid_Pitch_speckleSeparatelyLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle separately (semitones)...", nullptr, 1, GRAPHICS_TextGrid_Pitch_speckleSeparatelySemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle separately (mel)...", nullptr, 1, GRAPHICS_TextGrid_Pitch_speckleSeparatelyMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle separately (erb)...", nullptr, 1, GRAPHICS_TextGrid_Pitch_speckleSeparatelyErb);
	praat_addAction2 (classPitch, 1, classTextTier, 1, U"To PitchTier...", nullptr, 0, NEW1_Pitch_TextTier_to_PitchTier);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_TextGrid_viewAndEdit);
	praat_addAction2 (classSound, 1, classTextGrid, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_TextGrid_viewAndEdit);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"View & Edit with callback...", nullptr, praat_HIDDEN, WINDOW_TextGrid_viewAndEditWithCallback);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Draw...", nullptr, 0, GRAPHICS_TextGrid_Sound_draw);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Extract -", nullptr, 0, nullptr);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Extract all intervals...", nullptr, praat_DEPTH_1, NEW1_TextGrid_Sound_extractAllIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Extract non-empty intervals...", nullptr, praat_DEPTH_1, NEW1_TextGrid_Sound_extractNonemptyIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Extract intervals where...", nullptr, praat_DEPTH_1, NEW1_TextGrid_Sound_extractIntervalsWhere);
	praat_addAction2 (classSound, 1, classTextGrid, 1,   U"Extract intervals...", U"*Extract intervals where...", praat_DEPTH_1 | praat_DEPRECATED_2005, NEW1_TextGrid_Sound_extractIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Modify TextGrid", nullptr, 0, nullptr);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Scale times", nullptr, 0, MODIFY_TextGrid_Sound_scaleTimes);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Modify Sound", nullptr, 0, nullptr);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Clone time domain", nullptr, 0, MODIFY_TextGrid_Sound_cloneTimeDomain);
	praat_addAction2 (classSpellingChecker, 1, classWordList, 1, U"Replace WordList", nullptr, 0, MODIFY_SpellingChecker_replaceWordList);
	praat_addAction2 (classSpellingChecker, 1, classStringSet, 1, U"Replace user dictionary", nullptr, 0, MODIFY_SpellingChecker_replaceUserDictionary);
	praat_addAction2 (classSpellingChecker, 1, classStrings, 1, U"Replace word list?", nullptr, 0, HINT_SpellingChecker_replaceWordList_help);
	praat_addAction2 (classSpellingChecker, 1, classTextGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_TextGrid_SpellingChecker_viewAndEdit);
	praat_addAction2 (classSpellingChecker, 1, classTextGrid, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_TextGrid_SpellingChecker_viewAndEdit);
	praat_addAction2 (classTextGrid, 1, classIntervalTier, 1, U"Append", nullptr, 0, NEW1_TextGrid_IntervalTier_append);
	praat_addAction2 (classTextGrid, 1, classTextTier, 1, U"Append", nullptr, 0, NEW1_TextGrid_TextTier_append);

	praat_addAction3 (classLongSound, 1, classSpellingChecker, 1, classTextGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_TextGrid_LongSound_SpellingChecker_viewAndEdit);
	praat_addAction3 (classLongSound, 1, classSpellingChecker, 1, classTextGrid, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_TextGrid_LongSound_SpellingChecker_viewAndEdit);
	praat_addAction3 (classSound, 1, classSpellingChecker, 1, classTextGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_TextGrid_SpellingChecker_viewAndEdit);
	praat_addAction3 (classSound, 1, classSpellingChecker, 1, classTextGrid, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_TextGrid_SpellingChecker_viewAndEdit);
}

/* End of file praat_TextGrid_init.cpp */
