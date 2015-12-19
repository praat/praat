/* praat_TextGrid_init.cpp
 *
 * Copyright (C) 1992-2012,2014,2015 Paul Boersma
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

#include "praat.h"

#include "Pitch_AnyTier_to_PitchTier.h"
#include "SpectrumEditor.h"
#include "SpellingChecker.h"
#include "TextGridEditor.h"
#include "TextGrid_extensions.h"
#include "TextGrid_Sound.h"
#include "WordList.h"

#undef iam
#define iam iam_LOOP

void praat_dia_timeRange (UiForm dia);
void praat_get_timeRange (UiForm dia, double *tmin, double *tmax);
int praat_get_frequencyRange (UiForm dia, double *fmin, double *fmax);

static const char32 *STRING_FROM_FREQUENCY_HZ = U"left Frequency range (Hz)";
static const char32 *STRING_TO_FREQUENCY_HZ = U"right Frequency range (Hz)";
static const char32 *STRING_TIER_NUMBER = U"Tier number";
static const char32 *STRING_INTERVAL_NUMBER = U"Interval number";
static const char32 *STRING_POINT_NUMBER = U"Point number";

void praat_TimeFunction_modify_init (ClassInfo klas);   // Modify buttons for time-based subclasses of Function.

/***** ANYTIER (generic) *****/

DIRECT2 (AnyTier_into_TextGrid) {
	autoTextGrid grid = TextGrid_createWithoutTiers (1e30, -1e30);
	LOOP {
		iam (AnyTier);
		TextGrid_addTier_copy (grid.peek(), me);
	}
	praat_new (grid.move(), U"grid");
END2 }

/***** INTERVALTIER *****/

FORM (IntervalTier_downto_TableOfReal, U"IntervalTier: Down to TableOfReal", 0) {
	SENTENCE (U"Label", U"")
	OK2
DO
	LOOP {
		iam (IntervalTier);
		autoTableOfReal thee = IntervalTier_downto_TableOfReal (me, GET_STRING (U"Label"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (IntervalTier_downto_TableOfReal_any) {
	LOOP {
		iam (IntervalTier);
		autoTableOfReal thee = IntervalTier_downto_TableOfReal_any (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (IntervalTier_getCentrePoints, U"IntervalTier: Get centre points", 0) {
	SENTENCE (U"Text", U"")
	OK2
DO
	LOOP {
		iam (IntervalTier);
		autoPointProcess thee = IntervalTier_getCentrePoints (me, GET_STRING (U"Text"));
		praat_new (thee.move(), GET_STRING (U"Text"));
	}
END2 }

FORM (IntervalTier_getEndPoints, U"IntervalTier: Get end points", 0) {
	SENTENCE (U"Text", U"")
	OK2
DO
	LOOP {
		iam (IntervalTier);
		autoPointProcess thee = IntervalTier_getEndPoints (me, GET_STRING (U"Text"));
		praat_new (thee.move(), GET_STRING (U"Text"));
	}
END2 }

FORM (IntervalTier_getStartingPoints, U"IntervalTier: Get starting points", 0) {
	SENTENCE (U"Text", U"")
	OK2
DO
	LOOP {
		iam (IntervalTier);
		autoPointProcess thee = IntervalTier_getStartingPoints (me, GET_STRING (U"Text"));
		praat_new (thee.move(), GET_STRING (U"Text"));
	}
END2 }

DIRECT2 (IntervalTier_help) {
	Melder_help (U"IntervalTier");
END2 }

FORM_WRITE2 (IntervalTier_writeToXwaves, U"Xwaves label file", 0, 0) {
	LOOP {
		iam (IntervalTier);
		IntervalTier_writeToXwaves (me, file);
	}
END2 }

/***** INTERVALTIER & POINTPROCESS *****/

FORM (IntervalTier_PointProcess_endToCentre, U"From end to centre", U"IntervalTier & PointProcess: End to centre...") {
	REAL (U"Phase (0-1)", U"0.5")
	OK2
DO
	IntervalTier tier = nullptr;
	PointProcess point = nullptr;
	LOOP {
		if (CLASS == classIntervalTier) tier = (IntervalTier) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
	}
	double phase = GET_REAL (U"Phase");
	autoPointProcess thee = IntervalTier_PointProcess_endToCentre (tier, point, phase);
	praat_new (thee.move(), tier -> name, U"_", point -> name, U"_", lround (100.0 * phase));
END2 }

FORM (IntervalTier_PointProcess_startToCentre, U"From start to centre", U"IntervalTier & PointProcess: Start to centre...") {
	REAL (U"Phase (0-1)", U"0.5")
	OK2
DO
	IntervalTier tier = nullptr;
	PointProcess point = nullptr;
	LOOP {
		if (CLASS == classIntervalTier) tier = (IntervalTier) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
	}
	double phase = GET_REAL (U"Phase");
	autoPointProcess thee = IntervalTier_PointProcess_startToCentre (tier, point, phase);
	praat_new (thee.move(), tier -> name, U"_", point -> name, U"_", lround (100.0 * phase));
END2 }

/***** LABEL (obsolete) *****/

DIRECT2 (Label_Sound_to_TextGrid) {
	Label label = nullptr;
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classLabel) label = (Label) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoTextGrid thee = Label_Function_to_TextGrid (label, sound);
	praat_new (thee.move(), sound -> name);
END2 }

DIRECT2 (info_Label_Sound_to_TextGrid) {
	Melder_information (U"This is an old-style Label object. To turn it into a TextGrid, U"
		"select it together with a Sound of the appropriate duration, and click \"To TextGrid\".");
END2 }

/***** PITCH & TEXTGRID *****/

static void pr_TextGrid_Pitch_draw (UiForm dia, int speckle, int unit) {
	TextGrid grid = nullptr;
	Pitch pitch = nullptr;
	int IOBJECT;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classPitch) pitch = (Pitch) OBJECT;
	}
	double tmin, tmax, fmin, fmax;
	praat_get_timeRange (dia, & tmin, & tmax);
	praat_get_frequencyRange (dia, & fmin, & fmax);
	autoPraatPicture picture;
	TextGrid_Pitch_draw (grid, pitch, GRAPHICS,
		GET_INTEGER (STRING_TIER_NUMBER), tmin, tmax, fmin, fmax, GET_INTEGER (U"Font size"),
		GET_INTEGER (U"Use text styles"), GET_INTEGER (U"Text alignment") - 1, GET_INTEGER (U"Garnish"), speckle, unit);
}

FORM (TextGrid_Pitch_draw, U"TextGrid & Pitch: Draw", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	INTEGER (U"Font size (points)", U"18")
	BOOLEAN (U"Use text styles", true)
	OPTIONMENU (U"Text alignment", 2) OPTION (U"Left") OPTION (U"Centre") OPTION (U"Right")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_HERTZ);
END2 }

FORM (TextGrid_Pitch_drawErb, U"TextGrid & Pitch: Draw erb", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	praat_dia_timeRange (dia);
	REAL (U"left Frequency range (ERB)", U"0")
	REAL (U"right Frequency range (ERB)", U"10.0")
	INTEGER (U"Font size (points)", U"18")
	BOOLEAN (U"Use text styles", true)
	OPTIONMENU (U"Text alignment", 2) OPTION (U"Left") OPTION (U"Centre") OPTION (U"Right")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_ERB);
END2 }

FORM (TextGrid_Pitch_drawLogarithmic, U"TextGrid & Pitch: Draw logarithmic", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	INTEGER (U"Font size (points)", U"18")
	BOOLEAN (U"Use text styles", true)
	OPTIONMENU (U"Text alignment", 2) OPTION (U"Left") OPTION (U"Centre") OPTION (U"Right")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_HERTZ_LOGARITHMIC);
END2 }

FORM (TextGrid_Pitch_drawMel, U"TextGrid & Pitch: Draw mel", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	praat_dia_timeRange (dia);
	REAL (U"left Frequency range (mel)", U"0")
	REAL (U"right Frequency range (mel)", U"500")
	INTEGER (U"Font size (points)", U"18")
	BOOLEAN (U"Use text styles", true)
	OPTIONMENU (U"Text alignment", 2) OPTION (U"Left") OPTION (U"Centre") OPTION (U"Right")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_MEL);
END2 }

FORM (TextGrid_Pitch_drawSemitones, U"TextGrid & Pitch: Draw semitones", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	praat_dia_timeRange (dia);
	LABEL (U"", U"Range in semitones re 100 hertz:")
	REAL (U"left Frequency range (st)", U"-12.0")
	REAL (U"right Frequency range (st)", U"30.0")
	INTEGER (U"Font size (points)", U"18")
	BOOLEAN (U"Use text styles", true)
	OPTIONMENU (U"Text alignment", 2) OPTION (U"Left") OPTION (U"Centre") OPTION (U"Right")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_SEMITONES_100);
END2 }

static void pr_TextGrid_Pitch_drawSeparately (UiForm dia, int speckle, int unit) {
	TextGrid grid = nullptr;
	Pitch pitch = nullptr;
	int IOBJECT;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classPitch) pitch = (Pitch) OBJECT;
	}
	double tmin, tmax, fmin, fmax;
	praat_get_timeRange (dia, & tmin, & tmax);
	praat_get_frequencyRange (dia, & fmin, & fmax);
	autoPraatPicture picture;
	TextGrid_Pitch_drawSeparately (grid, pitch, GRAPHICS,
		tmin, tmax, fmin, fmax, GET_INTEGER (U"Show boundaries"),
		GET_INTEGER (U"Use text styles"), GET_INTEGER (U"Garnish"), speckle, unit);
}

FORM (TextGrid_Pitch_drawSeparately, U"TextGrid & Pitch: Draw separately", nullptr) {
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, U"0.0")
	REAL (STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_HERTZ);
END2 }

FORM (TextGrid_Pitch_drawSeparatelyErb, U"TextGrid & Pitch: Draw separately erb", nullptr) {
	praat_dia_timeRange (dia);
	REAL (U"left Frequency range (ERB)", U"0")
	REAL (U"right Frequency range (ERB)", U"10.0")
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_ERB);
END2 }

FORM (TextGrid_Pitch_drawSeparatelyLogarithmic, U"TextGrid & Pitch: Draw separately logarithmic", nullptr) {
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_HERTZ_LOGARITHMIC);
END2 }

FORM (TextGrid_Pitch_drawSeparatelyMel, U"TextGrid & Pitch: Draw separately mel", nullptr) {
	praat_dia_timeRange (dia);
	REAL (U"left Frequency range (mel)", U"0")
	REAL (U"right Frequency range (mel)", U"500")
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_MEL);
END2 }

FORM (TextGrid_Pitch_drawSeparatelySemitones, U"TextGrid & Pitch: Draw separately semitones", nullptr) {
	praat_dia_timeRange (dia);
	LABEL (U"", U"Range in semitones re 100 hertz:")
	REAL (U"left Frequency range (st)", U"-12.0")
	REAL (U"right Frequency range (st)", U"30.0")
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_SEMITONES_100);
END2 }

FORM (TextGrid_Pitch_speckle, U"TextGrid & Pitch: Speckle", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	INTEGER (U"Font size (points)", U"18")
	BOOLEAN (U"Use text styles", true)
	OPTIONMENU (U"Text alignment", 2) OPTION (U"Left") OPTION (U"Centre") OPTION (U"Right")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_HERTZ);
END2 }

FORM (TextGrid_Pitch_speckleErb, U"TextGrid & Pitch: Speckle erb", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	praat_dia_timeRange (dia);
	REAL (U"left Frequency range (ERB)", U"0")
	REAL (U"right Frequency range (ERB)", U"10.0")
	INTEGER (U"Font size (points)", U"18")
	BOOLEAN (U"Use text styles", true)
	OPTIONMENU (U"Text alignment", 2) OPTION (U"Left") OPTION (U"Centre") OPTION (U"Right")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_ERB);
END2 }

FORM (TextGrid_Pitch_speckleLogarithmic, U"TextGrid & Pitch: Speckle logarithmic", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	INTEGER (U"Font size (points)", U"18")
	BOOLEAN (U"Use text styles", true)
	OPTIONMENU (U"Text alignment", 2) OPTION (U"Left") OPTION (U"Centre") OPTION (U"Right")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_HERTZ_LOGARITHMIC);
END2 }

FORM (TextGrid_Pitch_speckleMel, U"TextGrid & Pitch: Speckle mel", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	praat_dia_timeRange (dia);
	REAL (U"left Frequency range (mel)", U"0")
	REAL (U"right Frequency range (mel)", U"500")
	INTEGER (U"Font size (points)", U"18")
	BOOLEAN (U"Use text styles", true)
	OPTIONMENU (U"Text alignment", 2) OPTION (U"Left") OPTION (U"Centre") OPTION (U"Right")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_MEL);
END2 }

FORM (TextGrid_Pitch_speckleSemitones, U"TextGrid & Pitch: Speckle semitones", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	praat_dia_timeRange (dia);
	LABEL (U"", U"Range in semitones re 100 hertz:")
	REAL (U"left Frequency range (st)", U"-12.0")
	REAL (U"right Frequency range (st)", U"30.0")
	INTEGER (U"Font size (points)", U"18")
	BOOLEAN (U"Use text styles", true)
	OPTIONMENU (U"Text alignment", 2) OPTION (U"Left") OPTION (U"Centre") OPTION (U"Right")
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_SEMITONES_100);
END2 }

FORM (TextGrid_Pitch_speckleSeparately, U"TextGrid & Pitch: Speckle separately", nullptr) {
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, U"0.0")
	REAL (STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_HERTZ);
END2 }

FORM (TextGrid_Pitch_speckleSeparatelyErb, U"TextGrid & Pitch: Speckle separately erb", nullptr) {
	praat_dia_timeRange (dia);
	REAL (U"left Frequency range (ERB)", U"0")
	REAL (U"right Frequency range (ERB)", U"10.0")
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_ERB);
END2 }

FORM (TextGrid_Pitch_speckleSeparatelyLogarithmic, U"TextGrid & Pitch: Speckle separately logarithmic", nullptr) {
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_HERTZ_LOGARITHMIC);
END2 }

FORM (TextGrid_Pitch_speckleSeparatelyMel, U"TextGrid & Pitch: Speckle separately mel", nullptr) {
	praat_dia_timeRange (dia);
	REAL (U"left Frequency range (mel)", U"0")
	REAL (U"right Frequency range (mel)", U"500")
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_MEL);
END2 }

FORM (TextGrid_Pitch_speckleSeparatelySemitones, U"TextGrid & Pitch: Speckle separately semitones", nullptr) {
	praat_dia_timeRange (dia);
	LABEL (U"", U"Range in semitones re 100 hertz:")
	REAL (U"left Frequency range (st)", U"-12.0")
	REAL (U"right Frequency range (st)", U"30.0")
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_SEMITONES_100);
END2 }

/***** PITCH & TEXTTIER *****/

FORM (Pitch_TextTier_to_PitchTier, U"Pitch & TextTier to PitchTier", U"Pitch & TextTier: To PitchTier...") {
	RADIO (U"Unvoiced strategy", 3)
		RADIOBUTTON (U"Zero")
		RADIOBUTTON (U"Error")
		RADIOBUTTON (U"Interpolate")
	OK2
DO
	Pitch pitch = nullptr;
	TextTier tier = nullptr;
	LOOP {
		iam (Daata);
		if (CLASS == classPitch) pitch = (Pitch) me;
		if (CLASS == classTextTier) tier = (TextTier) me;
	}
	autoPitchTier thee = Pitch_AnyTier_to_PitchTier (pitch, (AnyTier) tier, GET_INTEGER (U"Unvoiced strategy") - 1);
	praat_new (thee.move(), pitch -> name);
END2 }

/***** SOUND & TEXTGRID *****/

FORM (TextGrid_Sound_draw, U"TextGrid & Sound: Draw...", nullptr) {
	praat_dia_timeRange (dia);
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	TextGrid textgrid = nullptr;
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classTextGrid) textgrid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoPraatPicture picture;
	TextGrid_Sound_draw (textgrid, sound, GRAPHICS,
		GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Show boundaries"),
		GET_INTEGER (U"Use text styles"), GET_INTEGER (U"Garnish"));
END2 }

FORM (TextGrid_Sound_extractAllIntervals, U"TextGrid & Sound: Extract all intervals", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	BOOLEAN (U"Preserve times", false)
	OK2
DO
	TextGrid textgrid = nullptr;
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classTextGrid) textgrid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoCollection thee = TextGrid_Sound_extractAllIntervals (textgrid, sound,
		GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (U"Preserve times"));
	praat_new (thee.move(), U"dummy");
END2 }

FORM (TextGrid_Sound_extractNonemptyIntervals, U"TextGrid & Sound: Extract non-empty intervals", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	BOOLEAN (U"Preserve times", false)
	OK2
DO
	TextGrid textgrid = nullptr;
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classTextGrid) textgrid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoCollection thee = TextGrid_Sound_extractNonemptyIntervals (textgrid, sound,
		GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (U"Preserve times"));
	praat_new (thee.move(), U"dummy");
END2 }

FORM (TextGrid_Sound_extractIntervals, U"TextGrid & Sound: Extract intervals", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	BOOLEAN (U"Preserve times", false)
	SENTENCE (U"Label text", U"")
	OK2
DO
	TextGrid textgrid = nullptr;
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classTextGrid) textgrid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoCollection thee = TextGrid_Sound_extractIntervalsWhere (textgrid, sound,
		GET_INTEGER (STRING_TIER_NUMBER), kMelder_string_EQUAL_TO, GET_STRING (U"Label text"),
		GET_INTEGER (U"Preserve times"));
	praat_new (thee.move(), GET_STRING (U"Label text"));
END2 }

FORM (TextGrid_Sound_extractIntervalsWhere, U"TextGrid & Sound: Extract intervals", nullptr) {
	INTEGER (STRING_TIER_NUMBER, U"1")
	BOOLEAN (U"Preserve times", false)
	OPTIONMENU_ENUM (U"Extract every interval whose label...", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"")
	OK2
DO
	TextGrid textgrid = nullptr;
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classTextGrid) textgrid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoCollection thee = TextGrid_Sound_extractIntervalsWhere (textgrid, sound,
		GET_INTEGER (STRING_TIER_NUMBER),
		GET_ENUM (kMelder_string, U"Extract every interval whose label..."),
		GET_STRING (U"...the text"),
		GET_INTEGER (U"Preserve times"));
	praat_new (thee.move(), GET_STRING (U"...the text"));
END2 }

DIRECT2 (TextGrid_Sound_scaleTimes) {
	TextGrid textgrid = nullptr;
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classTextGrid) textgrid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	Function_scaleXTo (textgrid, sound -> xmin, sound -> xmax);
	praat_dataChanged (textgrid);
END2 }

DIRECT2 (TextGrid_Sound_cloneTimeDomain) {
	TextGrid textgrid = nullptr;
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classTextGrid) textgrid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	sound -> x1 += textgrid -> xmin - sound -> xmin;
	sound -> xmin = textgrid -> xmin;
	sound -> xmax = textgrid -> xmax;
	praat_dataChanged (sound);
END2 }

/***** SPELLINGCHECKER *****/

FORM (SpellingChecker_addNewWord, U"Add word to user dictionary", U"SpellingChecker") {
	SENTENCE (U"New word", U"")
	OK2
DO
	LOOP {
		iam (SpellingChecker);
		SpellingChecker_addNewWord (me, GET_STRING (U"New word"));
		praat_dataChanged (me);
	}
END2 }

FORM (SpellingChecker_edit, U"Edit spelling checker", U"SpellingChecker") {
	LABEL (U"", U"-- Syntax --")
	SENTENCE (U"Forbidden strings", U"")
	BOOLEAN (U"Check matching parentheses", 0)
	SENTENCE (U"Separating characters", U"")
	BOOLEAN (U"Allow all parenthesized", 0)
	LABEL (U"", U"-- Capitals --")
	BOOLEAN (U"Allow all names", 0)
	SENTENCE (U"Name prefixes", U"")
	BOOLEAN (U"Allow all abbreviations", 0)
	LABEL (U"", U"-- Capitalization --")
	BOOLEAN (U"Allow caps sentence-initially", 0)
	BOOLEAN (U"Allow caps after colon", 0)
	LABEL (U"", U"-- Word parts --")
	SENTENCE (U"Allow all words containing", U"")
	SENTENCE (U"Allow all words starting with", U"")
	SENTENCE (U"Allow all words ending in", U"")
	OK2
int IOBJECT;
LOOP {
	iam (SpellingChecker);
	SET_STRING (U"Forbidden strings", my forbiddenStrings)
	SET_INTEGER (U"Check matching parentheses", my checkMatchingParentheses)
	SET_STRING (U"Separating characters", my separatingCharacters)
	SET_INTEGER (U"Allow all parenthesized", my allowAllParenthesized)
	SET_INTEGER (U"Allow all names", my allowAllNames)
	SET_STRING (U"Name prefixes", my namePrefixes)
	SET_INTEGER (U"Allow all abbreviations", my allowAllAbbreviations)
	SET_INTEGER (U"Allow caps sentence-initially", my allowCapsSentenceInitially)
	SET_INTEGER (U"Allow caps after colon", my allowCapsAfterColon)
	SET_STRING (U"Allow all words containing", my allowAllWordsContaining)
	SET_STRING (U"Allow all words starting with", my allowAllWordsStartingWith)
	SET_STRING (U"Allow all words ending in", my allowAllWordsEndingIn)
}
DO
	LOOP {
		iam (SpellingChecker);
		Melder_free (my forbiddenStrings); my forbiddenStrings = Melder_dup_f (GET_STRING (U"Forbidden strings"));
		my checkMatchingParentheses = GET_INTEGER (U"Check matching parentheses");
		Melder_free (my separatingCharacters); my separatingCharacters = Melder_dup_f (GET_STRING (U"Separating characters"));
		my allowAllParenthesized = GET_INTEGER (U"Allow all parenthesized");
		my allowAllNames = GET_INTEGER (U"Allow all names");
		Melder_free (my namePrefixes); my namePrefixes = Melder_dup_f (GET_STRING (U"Name prefixes"));
		my allowAllAbbreviations = GET_INTEGER (U"Allow all abbreviations");
		my allowCapsSentenceInitially = GET_INTEGER (U"Allow caps sentence-initially");
		my allowCapsAfterColon = GET_INTEGER (U"Allow caps after colon");
		Melder_free (my allowAllWordsContaining); my allowAllWordsContaining = Melder_dup_f (GET_STRING (U"Allow all words containing"));
		Melder_free (my allowAllWordsStartingWith); my allowAllWordsStartingWith = Melder_dup_f (GET_STRING (U"Allow all words starting with"));
		Melder_free (my allowAllWordsEndingIn); my allowAllWordsEndingIn = Melder_dup_f (GET_STRING (U"Allow all words ending in"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (SpellingChecker_extractWordList) {
	LOOP {
		iam (SpellingChecker);
		autoWordList thee = SpellingChecker_extractWordList (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (SpellingChecker_extractUserDictionary) {
	LOOP {
		iam (SpellingChecker);
		autoSortedSetOfString thee = SpellingChecker_extractUserDictionary (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (SpellingChecker_isWordAllowed, U"Is word allowed?", U"SpellingChecker") {
	SENTENCE (U"Word", U"")
	OK2
DO
	LOOP {
		iam (SpellingChecker);
		bool isWordAllowed = SpellingChecker_isWordAllowed (me, GET_STRING (U"Word"));
		Melder_information (isWordAllowed ? U"1 (allowed)" : U"0 (not allowed)");
	}
END2 }

FORM (SpellingChecker_nextNotAllowedWord, U"Next not allowed word?", U"SpellingChecker") {
	LABEL (U"", U"Sentence:")
	TEXTFIELD (U"sentence", U"")
	INTEGER (U"Starting character", U"0")
	OK2
DO
	LOOP {
		iam (SpellingChecker);
		char32 *sentence = GET_STRING (U"sentence");
		long startingCharacter = GET_INTEGER (U"Starting character");
		if (startingCharacter < 0) Melder_throw (U"Starting character should be 0 or positive.");
		if (startingCharacter > (int) str32len (sentence)) Melder_throw (U"Starting character should not exceed end of sentence.");
		char32 *nextNotAllowedWord = SpellingChecker_nextNotAllowedWord (me, sentence, & startingCharacter);
		Melder_information (nextNotAllowedWord);
	}
END2 }

DIRECT2 (SpellingChecker_replaceWordList) {
	SpellingChecker spellingChecker = nullptr;
	WordList wordList = nullptr;
	LOOP {
		if (CLASS == classSpellingChecker) spellingChecker = (SpellingChecker) OBJECT;
		if (CLASS == classWordList) wordList = (WordList) OBJECT;
		SpellingChecker_replaceWordList (spellingChecker, wordList);
		praat_dataChanged (spellingChecker);
	}
END2 }

DIRECT2 (SpellingChecker_replaceWordList_help) {
	Melder_information (U"To replace the checker's word list\nby the contents of a Strings object:\n"
		U"1. select the Strings;\n2. convert to a WordList object;\n3. select the SpellingChecker and the WordList;\n"
		U"4. choose Replace.");
END2 }

DIRECT2 (SpellingChecker_replaceUserDictionary) {
	SpellingChecker spellingChecker = nullptr;
	SortedSetOfString dictionary = nullptr;
	LOOP {
		if (CLASS == classSpellingChecker) spellingChecker = (SpellingChecker) OBJECT;
		if (CLASS == classSortedSetOfString) dictionary = (SortedSetOfString) OBJECT;
		SpellingChecker_replaceUserDictionary (spellingChecker, dictionary);
	}
END2 }

/***** TEXTGRID *****/

FORM (TextGrid_countIntervalsWhere, U"Count intervals", U"TextGrid: Count intervals where...") {
	INTEGER (STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (U"Count intervals whose label", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK2
DO
	const char32 *text = GET_STRING (U"...the text");
	LOOP {
		iam (TextGrid);
		long numberOfLabels = TextGrid_countIntervalsWhere (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, U"Count intervals whose label"), text);
		Melder_information (numberOfLabels, U" intervals");
	}
END2 }

FORM (TextGrid_countLabels, U"Count labels", U"TextGrid: Count labels...") {
	INTEGER (STRING_TIER_NUMBER, U"1")
	SENTENCE (U"Label text", U"a")
	OK2
DO
	LOOP {
		iam (TextGrid);
		long numberOfLabels = TextGrid_countLabels (me, GET_INTEGER (STRING_TIER_NUMBER), GET_STRING (U"Label text"));
		Melder_information (numberOfLabels, U" labels");
	}
END2 }

FORM (TextGrid_countPointsWhere, U"Count points", U"TextGrid: Count points where...") {
	INTEGER (STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (U"Count points whose label", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK2
DO
	const char32 *text = GET_STRING (U"...the text");
	LOOP {
		iam (TextGrid);
		long numberOfLabels = TextGrid_countPointsWhere (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, U"Count points whose label"), text);
		Melder_information (numberOfLabels, U" points");
	}
END2 }

FORM (TextGrid_downto_Table, U"TextGrid: Down to Table", nullptr) {
	BOOLEAN (U"Include line number", false)
	NATURAL (U"Time decimals", U"6")
	BOOLEAN (U"Include tier names", true)
	BOOLEAN (U"Include empty intervals", false)
	OK2
DO
	LOOP {
		iam (TextGrid);
		autoTable thee = TextGrid_downto_Table (me, GET_INTEGER (U"Include line number"), GET_INTEGER (U"Time decimals"),
			GET_INTEGER (U"Include tier names"), GET_INTEGER (U"Include empty intervals"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (TextGrid_draw, U"TextGrid: Draw", nullptr) {
	praat_dia_timeRange (dia);
	BOOLEAN (U"Show boundaries", true)
	BOOLEAN (U"Use text styles", true)
	BOOLEAN (U"Garnish", true)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (TextGrid);
		TextGrid_Sound_draw (me, nullptr, GRAPHICS,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Show boundaries"),
			GET_INTEGER (U"Use text styles"), GET_INTEGER (U"Garnish"));
	}
END2 }

FORM (TextGrid_duplicateTier, U"TextGrid: Duplicate tier", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (U"Position", U"1 (= at top)")
	WORD (U"Name", U"")
	OK2
DO
	LOOP {
		iam (TextGrid);
		int itier = GET_INTEGER (STRING_TIER_NUMBER);
		int position = GET_INTEGER (U"Position");
		const char32 *name = GET_STRING (U"Name");
		if (itier > my tiers -> size) itier = my tiers -> size;
		autoAnyTier newTier = Data_copy ((AnyTier) my tiers -> item [itier]);
		Thing_setName (newTier.peek(), name);
		Ordered_addItemAtPosition_move (my tiers.get(), newTier.move(), position);
		praat_dataChanged (me);
	}
END2 }

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
			LOOP {
				iam (Spectrum);
				autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
				praat_installEditor (editor2.get(), IOBJECT);
				editor2.releaseToUser();
			}
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT2 (TextGrid_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a TextGrid from batch.");
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay null
	}
	LOOP if (CLASS == classTextGrid) {
		iam (TextGrid);
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, sound, true, nullptr, nullptr);
		Editor_setPublicationCallback (editor.peek(), cb_TextGridEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

FORM (TextGrid_editWithCallback, U"TextGrid: View & Edit with callback", nullptr) {
	SENTENCE (U"Callback text", U"r1")
	OK2
DO
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a TextGrid from batch.");
	Sound sound = nullptr;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay null
	}
	LOOP if (CLASS == classTextGrid) {
		iam (TextGrid);
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, sound, true, nullptr, Melder_peek32to8 (GET_STRING (U"Callback text")));
		Editor_setPublicationCallback (editor.peek(), cb_TextGridEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

DIRECT2 (TextGrid_LongSound_edit) {
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
		Editor_setPublicationCallback (editor.peek(), cb_TextGridEditor_publication);
		praat_installEditor2 (editor.get(), IOBJECT, ilongSound);
		editor.releaseToUser();
	}
END2 }

DIRECT2 (TextGrid_SpellingChecker_edit) {
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
END2 }

DIRECT2 (TextGrid_LongSound_SpellingChecker_edit) {
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
END2 }

FORM (TextGrid_extractPart, U"TextGrid: Extract part", 0) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"1.0")
	BOOLEAN (U"Preserve times", 0)
	OK2
DO
	LOOP {
		iam (TextGrid);
		autoTextGrid thee = TextGrid_extractPart (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Preserve times"));
		praat_new (thee.move(), my name, U"_part");
	}
END2 }

static Function pr_TextGrid_peekTier (UiForm dia) {
	int IOBJECT;
	LOOP {
		iam (TextGrid);
		long tierNumber = GET_INTEGER (STRING_TIER_NUMBER);
		if (tierNumber > my tiers -> size)
			Melder_throw (U"Tier number (", tierNumber, U") should not be larger than number of tiers (", my tiers -> size, U").");
		return (Function) my tiers -> item [tierNumber];
	}
	return nullptr;   // should not occur
}

static IntervalTier pr_TextGrid_peekIntervalTier (UiForm dia) {
	Function tier = pr_TextGrid_peekTier (dia);
	if (tier -> classInfo != classIntervalTier) Melder_throw (U"Tier should be interval tier.");
	return (IntervalTier) tier;
}

static TextTier pr_TextGrid_peekTextTier (UiForm dia) {
	Function tier = pr_TextGrid_peekTier (dia);
	if (! tier) return nullptr;
	if (tier -> classInfo != classTextTier) Melder_throw (U"Tier should be point tier (TextTier).");
	return (TextTier) tier;
}

static TextInterval pr_TextGrid_peekInterval (UiForm dia) {
	int intervalNumber = GET_INTEGER (STRING_INTERVAL_NUMBER);
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (dia);
	if (intervalNumber > intervalTier -> intervals. size) Melder_throw (U"Interval number too large.");
	return intervalTier -> intervals [intervalNumber];
}

static TextPoint pr_TextGrid_peekPoint (UiForm dia) {	
	long pointNumber = GET_INTEGER (STRING_POINT_NUMBER);
	TextTier textTier = pr_TextGrid_peekTextTier (dia);
	if (pointNumber > textTier -> points -> size) Melder_throw (U"Point number too large.");
	return (TextPoint) textTier -> points -> item [pointNumber];
}

FORM (TextGrid_extractOneTier, U"TextGrid: Extract one tier", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OK2
DO
	Function tier = pr_TextGrid_peekTier (dia);   // a reference
	autoTextGrid grid = TextGrid_createWithoutTiers (1e30, -1e30);
	TextGrid_addTier_copy (grid.peek(), tier);   // no transfer of tier ownership, because a copy is made
	praat_new (grid.move(), tier -> name);
END2 }

FORM (TextGrid_extractTier, U"TextGrid: Extract tier", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OK2
DO
	Function tier = pr_TextGrid_peekTier (dia);
	autoFunction thee = Data_copy (tier);
	praat_new (thee.move(), tier -> name);
END2 }

DIRECT2 (TextGrid_genericize) {
	LOOP {
		iam (TextGrid);
		TextGrid_genericize (me);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (TextGrid_nativize) {
	LOOP {
		iam (TextGrid);
		TextGrid_nativize (me);
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_getHighIndexFromTime, U"Get high index", U"AnyTier: Get high index from time...") {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	TextTier textTier = pr_TextGrid_peekTextTier (dia);
	long highIndex = AnyTier_timeToHighIndex (textTier, GET_REAL (U"Time"));
	Melder_information (highIndex);
END2 }

FORM (TextGrid_getHighIntervalAtTime, U"TextGrid: Get high interval at time", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (dia);
	long index = IntervalTier_timeToLowIndex (intervalTier, GET_REAL (U"Time"));
	Melder_information (index);
END2 }

FORM (TextGrid_getIntervalBoundaryFromTime, U"TextGrid: Get interval boundary from time", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (dia);
	long index = IntervalTier_hasBoundary (intervalTier, GET_REAL (U"Time"));
	Melder_information (index);
END2 }

FORM (TextGrid_getIntervalEdgeFromTime, U"TextGrid: Get interval edge from time", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (dia);
	long index = IntervalTier_hasTime (intervalTier, GET_REAL (U"Time"));
	Melder_information (index);
END2 }

FORM (TextGrid_getLowIndexFromTime, U"Get low index", U"AnyTier: Get low index from time...") {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	TextTier textTier = pr_TextGrid_peekTextTier (dia);
	long lowIndex = AnyTier_timeToLowIndex (textTier, GET_REAL (U"Time"));
	Melder_information (lowIndex);
END2 }

FORM (TextGrid_getLowIntervalAtTime, U"TextGrid: Get low interval at time", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (dia);
	long index = IntervalTier_timeToHighIndex (intervalTier, GET_REAL (U"Time"));
	Melder_information (index);
END2 }

FORM (TextGrid_getNearestIndexFromTime, U"Get nearest index", U"AnyTier: Get nearest index from time...") {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	TextTier textTier = pr_TextGrid_peekTextTier (dia);
	long nearestIndex = AnyTier_timeToNearestIndex (textTier, GET_REAL (U"Time"));
	Melder_information (nearestIndex);
END2 }

FORM (TextGrid_getIntervalAtTime, U"TextGrid: Get interval at time", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (dia);
	long index = IntervalTier_timeToIndex (intervalTier, GET_REAL (U"Time"));
	Melder_information (index);
END2 }

FORM (TextGrid_getNumberOfIntervals, U"TextGrid: Get number of intervals", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OK2
DO
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (dia);
	long numberOfIntervals = intervalTier -> intervals.size;
	Melder_information (numberOfIntervals);
END2 }

DIRECT2 (TextGrid_getNumberOfTiers) {
	LOOP {
		iam (TextGrid);
		long numberOfTiers = my tiers -> size;
		Melder_information (numberOfTiers);
	}
END2 }

FORM (TextGrid_getStartingPoint, U"TextGrid: Get starting point", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (STRING_INTERVAL_NUMBER, U"1")
	OK2
DO
	TextInterval interval = pr_TextGrid_peekInterval (dia);
	double startingPoint = interval -> xmin;
	Melder_informationReal (startingPoint, U"seconds");
END2 }

FORM (TextGrid_getEndPoint, U"TextGrid: Get end point", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (STRING_INTERVAL_NUMBER, U"1")
	OK2
DO
	TextInterval interval = pr_TextGrid_peekInterval (dia);
	double endPoint = interval -> xmax;
	Melder_informationReal (endPoint, U"seconds");
END2 }

FORM (TextGrid_getLabelOfInterval, U"TextGrid: Get label of interval", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (STRING_INTERVAL_NUMBER, U"1")
	OK2
DO
	TextInterval interval = pr_TextGrid_peekInterval (dia);
	MelderInfo_open ();
	MelderInfo_write (interval -> text);
	MelderInfo_close ();
END2 }

FORM (TextGrid_getNumberOfPoints, U"TextGrid: Get number of points", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OK2
DO
	TextTier textTier = pr_TextGrid_peekTextTier (dia);
	long numberOfPoints = textTier -> points -> size;
	Melder_information (numberOfPoints);
END2 }

FORM (TextGrid_getTierName, U"TextGrid: Get tier name", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OK2
DO
	Daata tier = pr_TextGrid_peekTier (dia);
	Melder_information (tier -> name);
END2 }

FORM (TextGrid_getTimeOfPoint, U"TextGrid: Get time of point", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (STRING_POINT_NUMBER, U"1")
	OK2
DO
	TextPoint point = pr_TextGrid_peekPoint (dia);
	Melder_informationReal (point -> number, U"seconds");
END2 }

FORM (TextGrid_getLabelOfPoint, U"TextGrid: Get label of point", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (STRING_POINT_NUMBER, U"1")
	OK2
DO
	TextPoint point = pr_TextGrid_peekPoint (dia);
	Melder_information (point -> mark);
END2 }

DIRECT2 (TextGrid_help) {
	Melder_help (U"TextGrid");
END2 }

FORM (TextGrid_insertBoundary, U"TextGrid: Insert boundary", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (TextGrid);
		TextGrid_insertBoundary (me, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL (U"Time"));
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_insertIntervalTier, U"TextGrid: Insert interval tier", nullptr) {
	NATURAL (U"Position", U"1 (= at top)")
	WORD (U"Name", U"")
	OK2
DO
	LOOP {
		iam (TextGrid);
		int position = GET_INTEGER (U"Position");
		const char32 *name = GET_STRING (U"Name");
		autoIntervalTier tier = IntervalTier_create (my xmin, my xmax);
		if (position > my tiers -> size) position = my tiers -> size + 1;
		Thing_setName (tier.peek(), name);
		Ordered_addItemAtPosition_move (my tiers.get(), tier.move(), position);
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_insertPoint, U"TextGrid: Insert point", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	LABEL (U"", U"Text:")
	TEXTFIELD (U"text", U"")
	OK2
DO
	LOOP {
		iam (TextGrid);
		TextGrid_insertPoint (me, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL (U"Time"), GET_STRING (U"text"));
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_insertPointTier, U"TextGrid: Insert point tier", nullptr) {
	NATURAL (U"Position", U"1 (= at top)")
	WORD (U"Name", U"")
	OK2
DO
	LOOP {
		iam (TextGrid);
		int position = GET_INTEGER (U"Position");
		const char32 *name = GET_STRING (U"Name");
		autoTextTier tier = TextTier_create (my xmin, my xmax);
		if (position > my tiers -> size) position = my tiers -> size + 1;
		Thing_setName (tier.peek(), name);
		Ordered_addItemAtPosition_move (my tiers.get(), tier.move(), position);
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_isIntervalTier, U"TextGrid: Is interval tier?", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OK2
DO
	Daata tier = pr_TextGrid_peekTier (dia);
	if (tier -> classInfo == classIntervalTier) {
		Melder_information (U"1 (yes, tier ", GET_INTEGER (STRING_TIER_NUMBER), U" is an interval tier)");
	} else {
		Melder_information (U"0 (no, tier ", GET_INTEGER (STRING_TIER_NUMBER), U" is a point tier)");
	}
END2 }

FORM (TextGrid_list, U"TextGrid: List", nullptr) {
	BOOLEAN (U"Include line number", false)
	NATURAL (U"Time decimals", U"6")
	BOOLEAN (U"Include tier names", true)
	BOOLEAN (U"Include empty intervals", false)
	OK2
DO
	LOOP {
		iam (TextGrid);
		TextGrid_list (me, GET_INTEGER (U"Include line number"), GET_INTEGER (U"Time decimals"),
			GET_INTEGER (U"Include tier names"), GET_INTEGER (U"Include empty intervals"));
	}
END2 }

DIRECT2 (TextGrids_concatenate) {
	autoCollection textGrids = praat_getSelectedObjects ();
	autoTextGrid thee = TextGrids_concatenate (textGrids.peek());
	praat_new (thee.move(), U"chain");
END2 }

DIRECT2 (TextGrids_merge) {
	autoCollection textGrids = praat_getSelectedObjects ();
	autoTextGrid thee = TextGrid_merge (textGrids.peek());
	praat_new (thee.move(), U"merged");
END2 }

DIRECT2 (info_TextGrid_Pitch_draw) {
	Melder_information (U"You can draw a TextGrid together with a Pitch after selecting them both.");
END2 }

FORM (TextGrid_removeBoundaryAtTime, U"TextGrid: Remove boundary at time", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (TextGrid);
		TextGrid_removeBoundaryAtTime (me, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL (U"Time"));
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_getCentrePoints, U"TextGrid: Get centre points", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (U"Get centre points whose label", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK2
DO
	const char32 *text = GET_STRING (U"...the text");
	LOOP {
		iam (TextGrid);
		autoPointProcess thee = TextGrid_getCentrePoints (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, U"Get centre points whose label"), text);
		praat_new (thee.move(), my name, U"_", text);
	}
END2 }

FORM (TextGrid_getEndPoints, U"TextGrid: Get end points", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (U"Get end points whose label", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK2
DO
	const char32 *text = GET_STRING (U"...the text");
	LOOP {
		iam (TextGrid);
		autoPointProcess thee = TextGrid_getEndPoints (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, U"Get end points whose label"), text);
		praat_new (thee.move(), my name, U"_", text);
	}
END2 }

FORM (TextGrid_getStartingPoints, U"TextGrid: Get starting points", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (U"Get starting points whose label", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK2
DO
	const char32 *text = GET_STRING (U"...the text");
	LOOP {
		iam (TextGrid);
		autoPointProcess thee = TextGrid_getStartingPoints (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, U"Get starting points whose label"), text);
		praat_new (thee.move(), my name, U"_", text);
	}
END2 }

FORM (TextGrid_getPoints, U"Get points", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (U"Get points whose label", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK2
DO
	const char32 *text = GET_STRING (U"...the text");
	LOOP {
		iam (TextGrid);
		autoPointProcess thee = TextGrid_getPoints (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, U"Get points whose label"), text);
		praat_new (thee.move(), my name, U"_", text);
	}
END2 }

FORM (TextGrid_getPoints_followed, U"Get points (followed)", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (U"Get points whose label", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OPTIONMENU_ENUM (U"followed by a label that", kMelder_string, DEFAULT)
	SENTENCE (U" ...the text", U"there")
	OK2
DO
	const char32 *text = GET_STRING (U"...the text");
	const char32 *following = GET_STRING (U" ...the text");
	LOOP {
		iam (TextGrid);
		autoPointProcess thee = TextGrid_getPoints_followed (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, U"Get points whose label"), text,
			GET_ENUM (kMelder_string, U"followed by a label that"), following);
		praat_new (thee.move(), my name, U"_", text);
	}
END2 }

FORM (TextGrid_getPoints_preceded, U"Get points (preceded)", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (U"Get points whose label", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"there")
	OPTIONMENU_ENUM (U"preceded by a label that", kMelder_string, DEFAULT)
	SENTENCE (U" ...the text", U"hi")
	OK2
DO
	const char32 *text = GET_STRING (U"...the text");
	const char32 *preceding = GET_STRING (U" ...the text");
	LOOP {
		iam (TextGrid);
		autoPointProcess thee = TextGrid_getPoints_preceded (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, U"Get points whose label"), text,
			GET_ENUM (kMelder_string, U"preceded by a label that"), preceding);
		praat_new (thee.move(), my name, U"_", text);
	}
END2 }

FORM (TextGrid_removeLeftBoundary, U"TextGrid: Remove left boundary", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (STRING_INTERVAL_NUMBER, U"2")
	OK2
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long iinterval = GET_INTEGER (STRING_INTERVAL_NUMBER);
	LOOP {
		iam (TextGrid);
		IntervalTier intervalTier;
		if (itier > my tiers -> size)
			Melder_throw (U"You cannot remove a boundary from tier ", itier, U" of ", me,
				U", because that TextGrid has only ", my tiers -> size, U" tiers.");
		intervalTier = (IntervalTier) my tiers -> item [itier];
		if (intervalTier -> classInfo != classIntervalTier)
			Melder_throw (U"You cannot remove a boundary from tier ", itier, U" of ", me,
				U", because that tier is a point tier instead of an interval tier.");
		if (iinterval > intervalTier -> intervals. size)
			Melder_throw (U"You cannot remove a boundary from interval ", iinterval, U" of tier ", itier, U" of ", me,
				U", because that tier has only ", intervalTier -> intervals. size, U" intervals.");
		if (iinterval == 1)
			Melder_throw (U"You cannot remove the left boundary from interval 1 of tier ", itier, U" of ", me,
				U", because this is at the left edge of the tier.");
		IntervalTier_removeLeftBoundary (intervalTier, iinterval);
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_removePoint, U"TextGrid: Remove point", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (STRING_POINT_NUMBER, U"2")
	OK2
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long ipoint = GET_INTEGER (STRING_POINT_NUMBER);
	LOOP {
		iam (TextGrid);
		TextTier pointTier;
		if (itier > my tiers -> size)
			Melder_throw (U"You cannot remove a point from tier ", itier, U" of ", me,
				U", because that TextGrid has only ", my tiers -> size, U" tiers.");
		pointTier = (TextTier) my tiers -> item [itier];
		if (pointTier -> classInfo != classTextTier)
			Melder_throw (U"You cannot remove a point from tier ", itier, U" of ", me,
				U", because that tier is an interval tier instead of a point tier.");
		if (ipoint > pointTier -> points -> size)
			Melder_throw (U"You cannot remove point ", ipoint, U" from tier ", itier, U" of ", me,
				U", because that tier has only ", pointTier -> points -> size, U" points.");
		TextTier_removePoint (pointTier, ipoint);
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_removePoints, U"Remove points", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OPTIONMENU_ENUM (U"Remove every point whose label...", kMelder_string, DEFAULT)
	SENTENCE (U"...the text", U"hi")
	OK2
DO
	LOOP {
		iam (TextGrid);
		TextGrid_removePoints (me, GET_INTEGER (STRING_TIER_NUMBER), GET_ENUM (kMelder_string, U"Remove every point whose label..."), GET_STRING (U"...the text"));
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_removeRightBoundary, U"TextGrid: Remove right boundary", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (STRING_INTERVAL_NUMBER, U"1")
	OK2
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long iinterval = GET_INTEGER (STRING_INTERVAL_NUMBER);
	LOOP {
		iam (TextGrid);
		IntervalTier intervalTier;
		if (itier > my tiers -> size)
			Melder_throw (U"You cannot remove a boundary from tier ", itier, U" of ", me,
				U", because that TextGrid has only ", my tiers -> size, U" tiers.");
		intervalTier = (IntervalTier) my tiers -> item [itier];
		if (intervalTier -> classInfo != classIntervalTier)
			Melder_throw (U"You cannot remove a boundary from tier ", itier, U" of ", me,
				U", because that tier is a point tier instead of an interval tier.");
		if (iinterval > intervalTier -> intervals.size)
			Melder_throw (U"You cannot remove a boundary from interval ", iinterval, U" of tier ", itier, U" of ", me,
				U", because that tier has only ", intervalTier -> intervals.size, U" intervals.");
		if (iinterval == intervalTier -> intervals.size)
			Melder_throw (U"You cannot remove the right boundary from interval ", iinterval, U" of tier ", itier, U" of ", me,
				U", because this is at the right edge of the tier.");
		IntervalTier_removeLeftBoundary (intervalTier, iinterval + 1);
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_removeTier, U"TextGrid: Remove tier", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	OK2
DO
	LOOP {
		iam (TextGrid);
		int itier = GET_INTEGER (STRING_TIER_NUMBER);
		if (my tiers -> size <= 1)
			Melder_throw (U"Sorry, I refuse to remove the last tier.");
		if (itier > my tiers -> size) itier = my tiers -> size;
		Collection_removeItem (my tiers.get(), itier);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (info_TextGrid_Sound_edit) {
	Melder_information (U"To include a copy of a Sound in your TextGrid editor:\n"
		U"   select a TextGrid and a Sound, and click \"View & Edit\".");
END2 }

DIRECT2 (info_TextGrid_Sound_draw) {
	Melder_information (U"You can draw a TextGrid together with a Sound after selecting them both.");
END2 }

FORM (TextGrid_setIntervalText, U"TextGrid: Set interval text", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (STRING_INTERVAL_NUMBER, U"1")
	LABEL (U"", U"Text:")
	TEXTFIELD (U"text", U"")
	OK2
DO
	LOOP {
		iam (TextGrid);
		TextGrid_setIntervalText (me, GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (STRING_INTERVAL_NUMBER), GET_STRING (U"text"));
		praat_dataChanged (me);
	}
END2 }

FORM (TextGrid_setPointText, U"TextGrid: Set point text", nullptr) {
	NATURAL (STRING_TIER_NUMBER, U"1")
	NATURAL (STRING_POINT_NUMBER, U"1")
	LABEL (U"", U"Text:")
	TEXTFIELD (U"text", U"")
	OK2
DO
	LOOP {
		iam (TextGrid);
		TextGrid_setPointText (me, GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (STRING_POINT_NUMBER), GET_STRING (U"text"));
		praat_dataChanged (me);
	}
END2 }

FORM_WRITE2 (TextGrid_writeToChronologicalTextFile, U"Text file", nullptr, nullptr) {
	LOOP {
		iam (TextGrid);
		TextGrid_writeToChronologicalTextFile (me, file);
	}
END2 }

/***** TEXTGRID & ANYTIER *****/

DIRECT2 (TextGrid_AnyTier_append) {
	TextGrid oldGrid = nullptr;
	LOOP {
		if (CLASS == classTextGrid) oldGrid = (TextGrid) OBJECT;
	}
	autoTextGrid newGrid = Data_copy (oldGrid);
	LOOP if (OBJECT != oldGrid) {
		iam (AnyTier);
		TextGrid_addTier_copy (newGrid.peek(), me);
	}
	praat_new (newGrid.move(), oldGrid -> name);
END2 }

/***** TEXTGRID & LONGSOUND *****/

DIRECT2 (TextGrid_LongSound_scaleTimes) {
	TextGrid grid = nullptr;
	LongSound longSound = nullptr;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classLongSound) longSound = (LongSound) OBJECT;
	}
	Function_scaleXTo (grid, longSound -> xmin, longSound -> xmax);
	praat_dataChanged (grid);
END2 }

/***** TEXTTIER *****/

FORM (TextTier_addPoint, U"TextTier: Add point", U"TextTier: Add point...") {
	REAL (U"Time (s)", U"0.5")
	SENTENCE (U"Text", U"")
	OK2
DO
	LOOP {
		iam (TextTier);
		TextTier_addPoint (me, GET_REAL (U"Time"), GET_STRING (U"Text"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (TextTier_downto_PointProcess) {
	LOOP {
		iam (TextTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (TextTier_downto_TableOfReal, U"TextTier: Down to TableOfReal", nullptr) {
	SENTENCE (U"Label", U"")
	OK2
DO
	LOOP {
		iam (TextTier);
		autoTableOfReal thee = TextTier_downto_TableOfReal (me, GET_STRING (U"Label"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (TextTier_downto_TableOfReal_any) {
	LOOP {
		iam (TextTier);
		autoTableOfReal thee = TextTier_downto_TableOfReal_any (me);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (TextTier_getLabelOfPoint, U"Get label of point", nullptr) {
	NATURAL (U"Point number", U"1")
	OK2
DO
	LOOP {
		iam (TextTier);
		long ipoint = GET_INTEGER (U"Point number");
		if (ipoint > my points -> size) Melder_throw (U"No such point.");
		TextPoint point = (TextPoint) my points -> item [ipoint];
		Melder_information (point -> mark);
	}
END2 }

FORM (TextTier_getPoints, U"Get points", nullptr) {
	SENTENCE (U"Text", U"")
	OK2
DO
	LOOP {
		iam (TextTier);
		autoPointProcess thee = TextTier_getPoints (me, GET_STRING (U"Text"));
		praat_new (thee.move(), GET_STRING (U"Text"));
	}
END2 }

DIRECT2 (TextTier_help) {
	Melder_help (U"TextTier");
END2 }

/***** WORDLIST *****/

FORM (WordList_hasWord, U"Does word occur in list?", U"WordList") {
	SENTENCE (U"Word", U"")
	OK2
DO
	LOOP {
		iam (WordList);
		bool hasWord = WordList_hasWord (me, GET_STRING (U"Word"));
		Melder_information (hasWord ? U"1" : U"0");
	}
END2 }

DIRECT2 (WordList_to_Strings) {
	LOOP {
		iam (WordList);
		autoStrings thee = WordList_to_Strings (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (WordList_upto_SpellingChecker) {
	LOOP {
		iam (WordList);
		autoSpellingChecker thee = WordList_upto_SpellingChecker (me);
		praat_new (thee.move(), my name);
	}
END2 }

/***** buttons *****/

void praat_TimeFunction_query_init (ClassInfo klas);
void praat_TimeTier_query_init (ClassInfo klas);
void praat_TimeTier_modify_init (ClassInfo klas);

void praat_uvafon_TextGrid_init ();
void praat_uvafon_TextGrid_init () {
	Thing_recognizeClassByOtherName (classTextTier, U"MarkTier");

	structTextGridEditor :: f_preferences ();

	praat_addAction1 (classIntervalTier, 0, U"IntervalTier help", nullptr, 0, DO_IntervalTier_help);
	praat_addAction1 (classIntervalTier, 1, U"Save as Xwaves label file...", nullptr, 0, DO_IntervalTier_writeToXwaves);
	praat_addAction1 (classIntervalTier, 1, U"Write to Xwaves label file...", nullptr, praat_HIDDEN, DO_IntervalTier_writeToXwaves);
	praat_addAction1 (classIntervalTier, 0, U"Collect", nullptr, 0, nullptr);
	praat_addAction1 (classIntervalTier, 0, U"Into TextGrid", nullptr, 0, DO_AnyTier_into_TextGrid);
	praat_addAction1 (classIntervalTier, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classIntervalTier, 0, U"Get starting points...", nullptr, 0, DO_IntervalTier_getStartingPoints);
	praat_addAction1 (classIntervalTier, 0, U"Get centre points...", nullptr, 0, DO_IntervalTier_getCentrePoints);
	praat_addAction1 (classIntervalTier, 0, U"Get end points...", nullptr, 0, DO_IntervalTier_getEndPoints);
	praat_addAction1 (classIntervalTier, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classIntervalTier, 0, U"Down to TableOfReal (any)", nullptr, 0, DO_IntervalTier_downto_TableOfReal_any);
	praat_addAction1 (classIntervalTier, 0, U"Down to TableOfReal...", nullptr, 0, DO_IntervalTier_downto_TableOfReal);

	praat_addAction1 (classLabel, 0, U"& Sound: To TextGrid?", nullptr, 0, DO_info_Label_Sound_to_TextGrid);

	praat_addAction1 (classSpellingChecker, 1, U"View & Edit...", nullptr, praat_ATTRACTIVE, DO_SpellingChecker_edit);
	praat_addAction1 (classSpellingChecker, 1, U"Edit...", nullptr, praat_HIDDEN, DO_SpellingChecker_edit);
	praat_addAction1 (classSpellingChecker, 0, U"Query", nullptr, 0, nullptr);
	praat_addAction1 (classSpellingChecker, 1, U"Is word allowed...", nullptr, 0, DO_SpellingChecker_isWordAllowed);
	praat_addAction1 (classSpellingChecker, 1, U"Next not allowed word...", nullptr, 0, DO_SpellingChecker_nextNotAllowedWord);
	praat_addAction1 (classSpellingChecker, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classSpellingChecker, 0, U"Add new word...", nullptr, 0, DO_SpellingChecker_addNewWord);
	praat_addAction1 (classSpellingChecker, 0, U"Analyze", nullptr, 0, nullptr);
	praat_addAction1 (classSpellingChecker, 0, U"Extract WordList", nullptr, 0, DO_SpellingChecker_extractWordList);
	praat_addAction1 (classSpellingChecker, 0, U"Extract user dictionary", nullptr, 0, DO_SpellingChecker_extractUserDictionary);

	praat_addAction1 (classTextGrid, 0, U"TextGrid help", nullptr, 0, DO_TextGrid_help);
	praat_addAction1 (classTextGrid, 1, U"Save as chronological text file...", nullptr, 0, DO_TextGrid_writeToChronologicalTextFile);
	praat_addAction1 (classTextGrid, 1, U"Write to chronological text file...", nullptr, praat_HIDDEN, DO_TextGrid_writeToChronologicalTextFile);
	praat_addAction1 (classTextGrid, 1, U"View & Edit alone", nullptr, 0, DO_TextGrid_edit);
	praat_addAction1 (classTextGrid, 1, U"View & Edit", nullptr, praat_HIDDEN, DO_TextGrid_edit);
	praat_addAction1 (classTextGrid, 1, U"Edit", nullptr, praat_HIDDEN, DO_TextGrid_edit);
	praat_addAction1 (classTextGrid, 1, U"View & Edit with Sound?", nullptr, praat_ATTRACTIVE, DO_info_TextGrid_Sound_edit);
	praat_addAction1 (classTextGrid, 0, U"Draw -", nullptr, 0, nullptr);
	praat_addAction1 (classTextGrid, 0, U"Draw...", nullptr, 1, DO_TextGrid_draw);
	praat_addAction1 (classTextGrid, 1, U"Draw with Sound?", nullptr, 1, DO_info_TextGrid_Sound_draw);
	praat_addAction1 (classTextGrid, 1, U"Draw with Pitch?", nullptr, 1, DO_info_TextGrid_Pitch_draw);
	praat_addAction1 (classTextGrid, 1, U"List...", nullptr, 0, DO_TextGrid_list);
	praat_addAction1 (classTextGrid, 0, U"Down to Table...", nullptr, 0, DO_TextGrid_downto_Table);
	praat_addAction1 (classTextGrid, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classTextGrid);
		praat_addAction1 (classTextGrid, 1, U"-- query textgrid --", nullptr, 1, nullptr);
		praat_addAction1 (classTextGrid, 1, U"Get number of tiers", nullptr, 1, DO_TextGrid_getNumberOfTiers);
		praat_addAction1 (classTextGrid, 1, U"Get tier name...", nullptr, 1, DO_TextGrid_getTierName);
		praat_addAction1 (classTextGrid, 1, U"Is interval tier...", nullptr, 1, DO_TextGrid_isIntervalTier);
		praat_addAction1 (classTextGrid, 1, U"-- query tier --", nullptr, 1, nullptr);
		praat_addAction1 (classTextGrid, 1, U"Query interval tier", nullptr, 1, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Get number of intervals...", nullptr, 2, DO_TextGrid_getNumberOfIntervals);
			praat_addAction1 (classTextGrid, 1, U"Get starting point...", nullptr, 2, DO_TextGrid_getStartingPoint);
			praat_addAction1 (classTextGrid, 1, U"Get start point...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_TextGrid_getStartingPoint);
			praat_addAction1 (classTextGrid, 1, U"Get end point...", nullptr, 2, DO_TextGrid_getEndPoint);
			praat_addAction1 (classTextGrid, 1, U"Get label of interval...", nullptr, 2, DO_TextGrid_getLabelOfInterval);
			praat_addAction1 (classTextGrid, 1, U"-- query interval from time --", nullptr, 2, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Get interval at time...", nullptr, 2, DO_TextGrid_getIntervalAtTime);
			praat_addAction1 (classTextGrid, 1, U"Get low interval at time...", nullptr, 2, DO_TextGrid_getLowIntervalAtTime);
			praat_addAction1 (classTextGrid, 1, U"Get high interval at time...", nullptr, 2, DO_TextGrid_getHighIntervalAtTime);
			praat_addAction1 (classTextGrid, 1, U"Get interval edge from time...", nullptr, 2, DO_TextGrid_getIntervalEdgeFromTime);
			praat_addAction1 (classTextGrid, 1, U"Get interval boundary from time...", nullptr, 2, DO_TextGrid_getIntervalBoundaryFromTime);
			praat_addAction1 (classTextGrid, 1, U"-- query interval labels --", nullptr, 2, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Count intervals where...", nullptr, 2, DO_TextGrid_countIntervalsWhere);
		praat_addAction1 (classTextGrid, 1, U"Query point tier", nullptr, 1, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Get number of points...", nullptr, 2, DO_TextGrid_getNumberOfPoints);
			praat_addAction1 (classTextGrid, 1, U"Get time of point...", nullptr, 2, DO_TextGrid_getTimeOfPoint);
			praat_addAction1 (classTextGrid, 1, U"Get label of point...", nullptr, 2, DO_TextGrid_getLabelOfPoint);
			praat_addAction1 (classTextGrid, 1, U"-- query point from time --", nullptr, 2, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Get low index from time...", nullptr, 2, DO_TextGrid_getLowIndexFromTime);
			praat_addAction1 (classTextGrid, 1, U"Get high index from time...", nullptr, 2, DO_TextGrid_getHighIndexFromTime);
			praat_addAction1 (classTextGrid, 1, U"Get nearest index from time...", nullptr, 2, DO_TextGrid_getNearestIndexFromTime);
			praat_addAction1 (classTextGrid, 1, U"-- query point labels --", nullptr, 2, nullptr);
			praat_addAction1 (classTextGrid, 1, U"Count points where...", nullptr, 2, DO_TextGrid_countPointsWhere);
		praat_addAction1 (classTextGrid, 1, U"-- query labels --", nullptr, praat_HIDDEN + praat_DEPTH_1, nullptr);
		praat_addAction1 (classTextGrid, 1, U"Count labels...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_TextGrid_countLabels);   // hidden 2015
	praat_addAction1 (classTextGrid, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classTextGrid, 0, U"Convert to backslash trigraphs", nullptr, 1, DO_TextGrid_genericize);
		praat_addAction1 (classTextGrid, 0, U"Genericize", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_TextGrid_genericize);   // hidden 2007
		praat_addAction1 (classTextGrid, 0, U"Convert to Unicode", nullptr, 1, DO_TextGrid_nativize);
		praat_addAction1 (classTextGrid, 0, U"Nativize", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_TextGrid_nativize);   // hidden 2007
		praat_TimeFunction_modify_init (classTextGrid);
		praat_addAction1 (classTextGrid, 0, U"-- modify tiers --", nullptr, 1, nullptr);
		praat_addAction1 (classTextGrid, 0, U"Insert interval tier...", nullptr, 1, DO_TextGrid_insertIntervalTier);
		praat_addAction1 (classTextGrid, 0, U"Insert point tier...", nullptr, 1, DO_TextGrid_insertPointTier);
		praat_addAction1 (classTextGrid, 0, U"Duplicate tier...", nullptr, 1, DO_TextGrid_duplicateTier);
		praat_addAction1 (classTextGrid, 0, U"Remove tier...", nullptr, 1, DO_TextGrid_removeTier);
		praat_addAction1 (classTextGrid, 1, U"-- modify tier --", nullptr, 1, nullptr);
		praat_addAction1 (classTextGrid, 0, U"Modify interval tier", nullptr, 1, nullptr);
			praat_addAction1 (classTextGrid, 0, U"Insert boundary...", nullptr, 2, DO_TextGrid_insertBoundary);
			praat_addAction1 (classTextGrid, 0, U"Remove left boundary...", nullptr, 2, DO_TextGrid_removeLeftBoundary);
			praat_addAction1 (classTextGrid, 0, U"Remove right boundary...", nullptr, 2, DO_TextGrid_removeRightBoundary);
			praat_addAction1 (classTextGrid, 0, U"Remove boundary at time...", nullptr, 2, DO_TextGrid_removeBoundaryAtTime);
			praat_addAction1 (classTextGrid, 0, U"Set interval text...", nullptr, 2, DO_TextGrid_setIntervalText);
		praat_addAction1 (classTextGrid, 0, U"Modify point tier", nullptr, 1, nullptr);
			praat_addAction1 (classTextGrid, 0, U"Insert point...", nullptr, 2, DO_TextGrid_insertPoint);
			praat_addAction1 (classTextGrid, 0, U"Remove point...", nullptr, 2, DO_TextGrid_removePoint);
			praat_addAction1 (classTextGrid, 0, U"Remove points...", nullptr, 2, DO_TextGrid_removePoints);
			praat_addAction1 (classTextGrid, 0, U"Set point text...", nullptr, 2, DO_TextGrid_setPointText);
praat_addAction1 (classTextGrid, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classTextGrid, 1, U"Extract one tier...", nullptr, 0, DO_TextGrid_extractOneTier);
	praat_addAction1 (classTextGrid, 1, U"Extract tier...", nullptr, praat_HIDDEN, DO_TextGrid_extractTier);   // hidden 2010
	praat_addAction1 (classTextGrid, 1, U"Extract part...", nullptr, 0, DO_TextGrid_extractPart);
	praat_addAction1 (classTextGrid, 1, U"Analyse interval tier -", nullptr, 0, nullptr);
		praat_addAction1 (classTextGrid, 1, U"Get starting points...", nullptr, 1, DO_TextGrid_getStartingPoints);
		praat_addAction1 (classTextGrid, 1, U"Get end points...", nullptr, 1, DO_TextGrid_getEndPoints);
		praat_addAction1 (classTextGrid, 1, U"Get centre points...", nullptr, 1, DO_TextGrid_getCentrePoints);
	praat_addAction1 (classTextGrid, 1, U"Analyse point tier -", nullptr, 0, nullptr);
		praat_addAction1 (classTextGrid, 1, U"Get points...", nullptr, 1, DO_TextGrid_getPoints);
		praat_addAction1 (classTextGrid, 1, U"Get points (preceded)...", nullptr, 1, DO_TextGrid_getPoints_preceded);
		praat_addAction1 (classTextGrid, 1, U"Get points (followed)...", nullptr, 1, DO_TextGrid_getPoints_followed);
praat_addAction1 (classTextGrid, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classTextGrid, 0, U"Merge", nullptr, 0, DO_TextGrids_merge);
	praat_addAction1 (classTextGrid, 0, U"Concatenate", nullptr, 0, DO_TextGrids_concatenate);

	praat_addAction1 (classTextTier, 0, U"TextTier help", nullptr, 0, DO_TextTier_help);
	praat_addAction1 (classTextTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classTextTier);
		praat_addAction1 (classTextTier, 0, U"Get label of point...", nullptr, 1, DO_TextTier_getLabelOfPoint);
	praat_addAction1 (classTextTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classTextTier);
		praat_addAction1 (classTextTier, 0, U"Add point...", nullptr, 1, DO_TextTier_addPoint);
	praat_addAction1 (classTextTier, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classTextTier, 0, U"Get points...", nullptr, 0, DO_TextTier_getPoints);
	praat_addAction1 (classTextTier, 0, U"Collect", nullptr, 0, nullptr);
	praat_addAction1 (classTextTier, 0, U"Into TextGrid", nullptr, 0, DO_AnyTier_into_TextGrid);
	praat_addAction1 (classTextTier, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classTextTier, 0, U"Down to PointProcess", nullptr, 0, DO_TextTier_downto_PointProcess);
	praat_addAction1 (classTextTier, 0, U"Down to TableOfReal (any)", nullptr, 0, DO_TextTier_downto_TableOfReal_any);
	praat_addAction1 (classTextTier, 0, U"Down to TableOfReal...", nullptr, 0, DO_TextTier_downto_TableOfReal);

	praat_addAction1 (classWordList, 0, U"Query", nullptr, 0, nullptr);
		praat_addAction1 (classWordList, 1, U"Has word...", nullptr, 0, DO_WordList_hasWord);
	praat_addAction1 (classWordList, 0, U"Analyze", nullptr, 0, nullptr);
		praat_addAction1 (classWordList, 0, U"To Strings", nullptr, 0, DO_WordList_to_Strings);
	praat_addAction1 (classWordList, 0, U"Synthesize", nullptr, 0, nullptr);
		praat_addAction1 (classWordList, 0, U"Up to SpellingChecker", nullptr, 0, DO_WordList_upto_SpellingChecker);

	praat_addAction2 (classIntervalTier, 1, classPointProcess, 1, U"Start to centre...", nullptr, 0, DO_IntervalTier_PointProcess_startToCentre);
	praat_addAction2 (classIntervalTier, 1, classPointProcess, 1, U"End to centre...", nullptr, 0, DO_IntervalTier_PointProcess_endToCentre);
	praat_addAction2 (classIntervalTier, 0, classTextTier, 0, U"Collect", nullptr, 0, nullptr);
	praat_addAction2 (classIntervalTier, 0, classTextTier, 0, U"Into TextGrid", nullptr, 0, DO_AnyTier_into_TextGrid);
	praat_addAction2 (classLabel, 1, classSound, 1, U"To TextGrid", nullptr, 0, DO_Label_Sound_to_TextGrid);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_TextGrid_LongSound_edit);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, U"Edit", nullptr, praat_HIDDEN, DO_TextGrid_LongSound_edit);   // hidden 2011
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, U"Scale times", nullptr, 0, DO_TextGrid_LongSound_scaleTimes);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw -", nullptr, 0, nullptr);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw...", nullptr, 1, DO_TextGrid_Pitch_draw);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw logarithmic...", nullptr, 1, DO_TextGrid_Pitch_drawLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw semitones...", nullptr, 1, DO_TextGrid_Pitch_drawSemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw mel...", nullptr, 1, DO_TextGrid_Pitch_drawMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw erb...", nullptr, 1, DO_TextGrid_Pitch_drawErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle...", nullptr, 1, DO_TextGrid_Pitch_speckle);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle logarithmic...", nullptr, 1, DO_TextGrid_Pitch_speckleLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle semitones...", nullptr, 1, DO_TextGrid_Pitch_speckleSemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle mel...", nullptr, 1, DO_TextGrid_Pitch_speckleMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle erb...", nullptr, 1, DO_TextGrid_Pitch_speckleErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"-- draw separately --", nullptr, 1, nullptr);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw separately...", nullptr, 1, DO_TextGrid_Pitch_drawSeparately);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw separately (logarithmic)...", nullptr, 1, DO_TextGrid_Pitch_drawSeparatelyLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw separately (semitones)...", nullptr, 1, DO_TextGrid_Pitch_drawSeparatelySemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw separately (mel)...", nullptr, 1, DO_TextGrid_Pitch_drawSeparatelyMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Draw separately (erb)...", nullptr, 1, DO_TextGrid_Pitch_drawSeparatelyErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle separately...", nullptr, 1, DO_TextGrid_Pitch_speckleSeparately);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle separately (logarithmic)...", nullptr, 1, DO_TextGrid_Pitch_speckleSeparatelyLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle separately (semitones)...", nullptr, 1, DO_TextGrid_Pitch_speckleSeparatelySemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle separately (mel)...", nullptr, 1, DO_TextGrid_Pitch_speckleSeparatelyMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, U"Speckle separately (erb)...", nullptr, 1, DO_TextGrid_Pitch_speckleSeparatelyErb);
	praat_addAction2 (classPitch, 1, classTextTier, 1, U"To PitchTier...", nullptr, 0, DO_Pitch_TextTier_to_PitchTier);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_TextGrid_edit);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"View & Edit with callback...", nullptr, praat_HIDDEN, DO_TextGrid_editWithCallback);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Edit", nullptr, praat_HIDDEN, DO_TextGrid_edit);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Draw...", nullptr, 0, DO_TextGrid_Sound_draw);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Extract -", nullptr, 0, nullptr);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Extract all intervals...", nullptr, praat_DEPTH_1, DO_TextGrid_Sound_extractAllIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Extract non-empty intervals...", nullptr, praat_DEPTH_1, DO_TextGrid_Sound_extractNonemptyIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Extract intervals...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_TextGrid_Sound_extractIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Extract intervals where...", nullptr, praat_DEPTH_1, DO_TextGrid_Sound_extractIntervalsWhere);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Modify TextGrid", nullptr, 0, nullptr);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Scale times", nullptr, 0, DO_TextGrid_Sound_scaleTimes);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Modify Sound", nullptr, 0, nullptr);
	praat_addAction2 (classSound, 1, classTextGrid, 1, U"Clone time domain", nullptr, 0, DO_TextGrid_Sound_cloneTimeDomain);
	praat_addAction2 (classSpellingChecker, 1, classWordList, 1, U"Replace WordList", nullptr, 0, DO_SpellingChecker_replaceWordList);
	praat_addAction2 (classSpellingChecker, 1, classSortedSetOfString, 1, U"Replace user dictionary", nullptr, 0, DO_SpellingChecker_replaceUserDictionary);
	praat_addAction2 (classSpellingChecker, 1, classStrings, 1, U"Replace word list?", nullptr, 0, DO_SpellingChecker_replaceWordList_help);
	praat_addAction2 (classSpellingChecker, 1, classTextGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_TextGrid_SpellingChecker_edit);
	praat_addAction2 (classSpellingChecker, 1, classTextGrid, 1, U"Edit", nullptr, praat_HIDDEN, DO_TextGrid_SpellingChecker_edit);   // hidden 2011
	praat_addAction2 (classTextGrid, 1, classTextTier, 1, U"Append", nullptr, 0, DO_TextGrid_AnyTier_append);
	praat_addAction2 (classTextGrid, 1, classIntervalTier, 1, U"Append", nullptr, 0, DO_TextGrid_AnyTier_append);

	praat_addAction3 (classLongSound, 1, classSpellingChecker, 1, classTextGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_TextGrid_LongSound_SpellingChecker_edit);
	praat_addAction3 (classLongSound, 1, classSpellingChecker, 1, classTextGrid, 1, U"Edit", nullptr, praat_HIDDEN, DO_TextGrid_LongSound_SpellingChecker_edit);
	praat_addAction3 (classSound, 1, classSpellingChecker, 1, classTextGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_TextGrid_SpellingChecker_edit);
	praat_addAction3 (classSound, 1, classSpellingChecker, 1, classTextGrid, 1, U"Edit", nullptr, praat_HIDDEN, DO_TextGrid_SpellingChecker_edit);
}

/* End of file praat_TextGrid_init.cpp */
