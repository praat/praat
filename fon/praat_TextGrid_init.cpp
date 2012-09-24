/* praat_TextGrid_init.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

void praat_dia_timeRange (Any dia);
void praat_get_timeRange (Any dia, double *tmin, double *tmax);
int praat_get_frequencyRange (Any dia, double *fmin, double *fmax);

static const wchar_t *STRING_FROM_FREQUENCY_HZ = L"left Frequency range (Hz)";
static const wchar_t *STRING_TO_FREQUENCY_HZ = L"right Frequency range (Hz)";
static const wchar_t *STRING_TIER_NUMBER = L"Tier number";
static const wchar_t *STRING_INTERVAL_NUMBER = L"Interval number";
static const wchar_t *STRING_POINT_NUMBER = L"Point number";

void praat_TimeFunction_modify_init (ClassInfo klas);   // Modify buttons for time-based subclasses of Function.

/***** ANYTIER (generic) *****/

DIRECT (AnyTier_into_TextGrid)
	autoTextGrid grid = TextGrid_createWithoutTiers (1e30, -1e30);
	LOOP {
		iam (AnyTier);
		TextGrid_addTier (grid.peek(), me);
	}
	praat_new (grid.transfer(), L"grid");
END

/***** INTERVALTIER *****/

FORM (IntervalTier_downto_TableOfReal, L"IntervalTier: Down to TableOfReal", 0)
	SENTENCE (L"Label", L"")
	OK
DO
	LOOP {
		iam (IntervalTier);
		autoTableOfReal thee = IntervalTier_downto_TableOfReal (me, GET_STRING (L"Label"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (IntervalTier_downto_TableOfReal_any)
	LOOP {
		iam (IntervalTier);
		autoTableOfReal thee = IntervalTier_downto_TableOfReal_any (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (IntervalTier_getCentrePoints, L"IntervalTier: Get centre points", 0)
	SENTENCE (L"Text", L"")
	OK
DO
	LOOP {
		iam (IntervalTier);
		autoPointProcess thee = IntervalTier_getCentrePoints (me, GET_STRING (L"Text"));
		praat_new (thee.transfer(), GET_STRING (L"Text"));
	}
END

FORM (IntervalTier_getEndPoints, L"IntervalTier: Get end points", 0)
	SENTENCE (L"Text", L"")
	OK
DO
	LOOP {
		iam (IntervalTier);
		autoPointProcess thee = IntervalTier_getEndPoints (me, GET_STRING (L"Text"));
		praat_new (thee.transfer(), GET_STRING (L"Text"));
	}
END

FORM (IntervalTier_getStartingPoints, L"IntervalTier: Get starting points", 0)
	SENTENCE (L"Text", L"")
	OK
DO
	LOOP {
		iam (IntervalTier);
		autoPointProcess thee = IntervalTier_getStartingPoints (me, GET_STRING (L"Text"));
		praat_new (thee.transfer(), GET_STRING (L"Text"));
	}
END

DIRECT (IntervalTier_help) Melder_help (L"IntervalTier"); END

FORM_WRITE (IntervalTier_writeToXwaves, L"Xwaves label file", 0, 0)
	LOOP {
		iam (IntervalTier);
		IntervalTier_writeToXwaves (me, file);
	}
END

/***** INTERVALTIER & POINTPROCESS *****/

FORM (IntervalTier_PointProcess_endToCentre, L"From end to centre", L"IntervalTier & PointProcess: End to centre...")
	REAL (L"Phase (0-1)", L"0.5")
	OK
DO
	IntervalTier tier = NULL;
	PointProcess point = NULL;
	LOOP {
		if (CLASS == classIntervalTier) tier = (IntervalTier) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
	}
	double phase = GET_REAL (L"Phase");
	autoPointProcess thee = IntervalTier_PointProcess_endToCentre (tier, point, phase);
	praat_new (thee.transfer(), tier -> name, L"_", point -> name, L"_", Melder_integer ((long) (100 * phase)));
END

FORM (IntervalTier_PointProcess_startToCentre, L"From start to centre", L"IntervalTier & PointProcess: Start to centre...")
	REAL (L"Phase (0-1)", L"0.5")
	OK
DO
	IntervalTier tier = NULL;
	PointProcess point = NULL;
	LOOP {
		if (CLASS == classIntervalTier) tier = (IntervalTier) OBJECT;
		if (CLASS == classPointProcess) point = (PointProcess) OBJECT;
	}
	double phase = GET_REAL (L"Phase");
	autoPointProcess thee = IntervalTier_PointProcess_startToCentre (tier, point, phase);
	praat_new (thee.transfer(), tier -> name, L"_", point -> name, L"_", Melder_integer ((long) (100 * phase)));
END

/***** LABEL (obsolete) *****/

DIRECT (Label_Sound_to_TextGrid)
	Label label = NULL;
	Sound sound = NULL;
	LOOP {
		if (CLASS == classLabel) label = (Label) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoTextGrid thee = Label_Function_to_TextGrid (label, sound);
	praat_new (thee.transfer(), sound -> name);
END

DIRECT (info_Label_Sound_to_TextGrid)
	Melder_information (L"This is an old-style Label object. To turn it into a TextGrid, L"
		"select it together with a Sound of the appropriate duration, and click \"To TextGrid\".");
END

/***** PITCH & TEXTGRID *****/

static void pr_TextGrid_Pitch_draw (Any dia, int speckle, int unit) {
	TextGrid grid = NULL;
	Pitch pitch = NULL;
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
		GET_INTEGER (STRING_TIER_NUMBER), tmin, tmax, fmin, fmax, GET_INTEGER (L"Font size"),
		GET_INTEGER (L"Use text styles"), GET_INTEGER (L"Text alignment") - 1, GET_INTEGER (L"Garnish"), speckle, unit);
}

FORM (TextGrid_Pitch_draw, L"TextGrid & Pitch: Draw", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, L"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_HERTZ);
END

FORM (TextGrid_Pitch_drawErb, L"TextGrid & Pitch: Draw erb", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	REAL (L"left Frequency range (ERB)", L"0")
	REAL (L"right Frequency range (ERB)", L"10.0")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_ERB);
END

FORM (TextGrid_Pitch_drawLogarithmic, L"TextGrid & Pitch: Draw logarithmic", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, L"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_HERTZ_LOGARITHMIC);
END

FORM (TextGrid_Pitch_drawMel, L"TextGrid & Pitch: Draw mel", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	REAL (L"left Frequency range (mel)", L"0")
	REAL (L"right Frequency range (mel)", L"500")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_MEL);
END

FORM (TextGrid_Pitch_drawSemitones, L"TextGrid & Pitch: Draw semitones", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	LABEL (L"", L"Range in semitones re 100 hertz:")
	REAL (L"left Frequency range (st)", L"-12.0")
	REAL (L"right Frequency range (st)", L"30.0")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_SEMITONES_100);
END

static void pr_TextGrid_Pitch_drawSeparately (Any dia, int speckle, int unit) {
	TextGrid grid = NULL;
	Pitch pitch = NULL;
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
		tmin, tmax, fmin, fmax, GET_INTEGER (L"Show boundaries"),
		GET_INTEGER (L"Use text styles"), GET_INTEGER (L"Garnish"), speckle, unit);
}

FORM (TextGrid_Pitch_drawSeparately, L"TextGrid & Pitch: Draw separately", 0)
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, L"0.0")
	REAL (STRING_TO_FREQUENCY_HZ, L"500.0")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_HERTZ);
END

FORM (TextGrid_Pitch_drawSeparatelyErb, L"TextGrid & Pitch: Draw separately erb", 0)
	praat_dia_timeRange (dia);
	REAL (L"left Frequency range (ERB)", L"0")
	REAL (L"right Frequency range (ERB)", L"10.0")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_ERB);
END

FORM (TextGrid_Pitch_drawSeparatelyLogarithmic, L"TextGrid & Pitch: Draw separately logarithmic", 0)
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, L"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_HERTZ_LOGARITHMIC);
END

FORM (TextGrid_Pitch_drawSeparatelyMel, L"TextGrid & Pitch: Draw separately mel", 0)
	praat_dia_timeRange (dia);
	REAL (L"left Frequency range (mel)", L"0")
	REAL (L"right Frequency range (mel)", L"500")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_MEL);
END

FORM (TextGrid_Pitch_drawSeparatelySemitones, L"TextGrid & Pitch: Draw separately semitones", 0)
	praat_dia_timeRange (dia);
	LABEL (L"", L"Range in semitones re 100 hertz:")
	REAL (L"left Frequency range (st)", L"-12.0")
	REAL (L"right Frequency range (st)", L"30.0")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_SEMITONES_100);
END

FORM (TextGrid_Pitch_speckle, L"TextGrid & Pitch: Speckle", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, L"0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_HERTZ);
END

FORM (TextGrid_Pitch_speckleErb, L"TextGrid & Pitch: Speckle erb", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	REAL (L"left Frequency range (ERB)", L"0")
	REAL (L"right Frequency range (ERB)", L"10.0")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_ERB);
END

FORM (TextGrid_Pitch_speckleLogarithmic, L"TextGrid & Pitch: Speckle logarithmic", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, L"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_HERTZ_LOGARITHMIC);
END

FORM (TextGrid_Pitch_speckleMel, L"TextGrid & Pitch: Speckle mel", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	REAL (L"left Frequency range (mel)", L"0")
	REAL (L"right Frequency range (mel)", L"500")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_MEL);
END

FORM (TextGrid_Pitch_speckleSemitones, L"TextGrid & Pitch: Speckle semitones", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	LABEL (L"", L"Range in semitones re 100 hertz:")
	REAL (L"left Frequency range (st)", L"-12.0")
	REAL (L"right Frequency range (st)", L"30.0")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_SEMITONES_100);
END

FORM (TextGrid_Pitch_speckleSeparately, L"TextGrid & Pitch: Speckle separately", 0)
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, L"0.0")
	REAL (STRING_TO_FREQUENCY_HZ, L"500.0")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_HERTZ);
END

FORM (TextGrid_Pitch_speckleSeparatelyErb, L"TextGrid & Pitch: Speckle separately erb", 0)
	praat_dia_timeRange (dia);
	REAL (L"left Frequency range (ERB)", L"0")
	REAL (L"right Frequency range (ERB)", L"10.0")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_ERB);
END

FORM (TextGrid_Pitch_speckleSeparatelyLogarithmic, L"TextGrid & Pitch: Speckle separately logarithmic", 0)
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, L"50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, L"500.0")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_HERTZ_LOGARITHMIC);
END

FORM (TextGrid_Pitch_speckleSeparatelyMel, L"TextGrid & Pitch: Speckle separately mel", 0)
	praat_dia_timeRange (dia);
	REAL (L"left Frequency range (mel)", L"0")
	REAL (L"right Frequency range (mel)", L"500")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_MEL);
END

FORM (TextGrid_Pitch_speckleSeparatelySemitones, L"TextGrid & Pitch: Speckle separately semitones", 0)
	praat_dia_timeRange (dia);
	LABEL (L"", L"Range in semitones re 100 hertz:")
	REAL (L"left Frequency range (st)", L"-12.0")
	REAL (L"right Frequency range (st)", L"30.0")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_SEMITONES_100);
END

/***** PITCH & TEXTTIER *****/

FORM (Pitch_TextTier_to_PitchTier, L"Pitch & TextTier to PitchTier", L"Pitch & TextTier: To PitchTier...")
	RADIO (L"Unvoiced strategy", 3)
		RADIOBUTTON (L"Zero")
		RADIOBUTTON (L"Error")
		RADIOBUTTON (L"Interpolate")
	OK
DO
	Pitch pitch = NULL;
	TextTier tier = NULL;
	LOOP {
		iam (Data);
		if (CLASS == classPitch) pitch = (Pitch) me;
		if (CLASS == classTextTier) tier = (TextTier) me;
	}
	autoPitchTier thee = Pitch_AnyTier_to_PitchTier (pitch, (AnyTier) tier, GET_INTEGER (L"Unvoiced strategy") - 1);
	praat_new (thee.transfer(), pitch -> name);
END

/***** SOUND & TEXTGRID *****/

FORM (TextGrid_Sound_draw, L"TextGrid & Sound: Draw...", 0)
	praat_dia_timeRange (dia);
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	TextGrid grid = NULL;
	Sound sound = NULL;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoPraatPicture picture;
	TextGrid_Sound_draw (grid, sound, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Show boundaries"),
		GET_INTEGER (L"Use text styles"), GET_INTEGER (L"Garnish"));
END

FORM (TextGrid_Sound_extractAllIntervals, L"TextGrid & Sound: Extract all intervals", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	BOOLEAN (L"Preserve times", 0)
	OK
DO
	TextGrid grid = NULL;
	Sound sound = NULL;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoCollection thee = TextGrid_Sound_extractAllIntervals (grid, sound,
		GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (L"Preserve times"));
	praat_new (thee.transfer(), L"dummy");
END

FORM (TextGrid_Sound_extractNonemptyIntervals, L"TextGrid & Sound: Extract non-empty intervals", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	BOOLEAN (L"Preserve times", 0)
	OK
DO
	TextGrid grid = NULL;
	Sound sound = NULL;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoCollection thee = TextGrid_Sound_extractNonemptyIntervals (grid, sound,
		GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (L"Preserve times"));
	praat_new (thee.transfer(), L"dummy");
END

FORM (TextGrid_Sound_extractIntervals, L"TextGrid & Sound: Extract intervals", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	BOOLEAN (L"Preserve times", 0)
	SENTENCE (L"Label text", L"")
	OK
DO
	TextGrid grid = NULL;
	Sound sound = NULL;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoCollection thee = TextGrid_Sound_extractIntervalsWhere (grid, sound,
		GET_INTEGER (STRING_TIER_NUMBER), kMelder_string_EQUAL_TO, GET_STRING (L"Label text"),
		GET_INTEGER (L"Preserve times"));
	praat_new (thee.transfer(), GET_STRING (L"Label text"));
END

FORM (TextGrid_Sound_extractIntervalsWhere, L"TextGrid & Sound: Extract intervals", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	BOOLEAN (L"Preserve times", 0)
	OPTIONMENU_ENUM (L"Extract all intervals whose label...", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"")
	OK
DO
	TextGrid grid = NULL;
	Sound sound = NULL;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	autoCollection thee = TextGrid_Sound_extractIntervalsWhere (grid, sound,
		GET_INTEGER (STRING_TIER_NUMBER),
		GET_ENUM (kMelder_string, L"Extract all intervals whose label..."),
		GET_STRING (L"...the text"),
		GET_INTEGER (L"Preserve times"));
	praat_new (thee.transfer(), GET_STRING (L"...the text"));
END

DIRECT (TextGrid_Sound_scaleTimes)
	TextGrid grid = NULL;
	Sound sound = NULL;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	Function_scaleXTo (grid, sound -> xmin, sound -> xmax);
	praat_dataChanged (grid);
END

DIRECT (TextGrid_Sound_cloneTimeDomain)
	TextGrid grid = NULL;
	Sound sound = NULL;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;
	}
	sound -> x1 += grid -> xmin - sound -> xmin;
	sound -> xmin = grid -> xmin;
	sound -> xmax = grid -> xmax;
	praat_dataChanged (sound);
END

/***** SPELLINGCHECKER *****/

FORM (SpellingChecker_addNewWord, L"Add word to user dictionary", L"SpellingChecker")
	SENTENCE (L"New word", L"")
	OK
DO
	LOOP {
		iam (SpellingChecker);
		SpellingChecker_addNewWord (me, GET_STRING (L"New word"));
		praat_dataChanged (me);
	}
END

FORM (SpellingChecker_edit, L"Edit spelling checker", L"SpellingChecker")
	LABEL (L"", L"-- Syntax --")
	SENTENCE (L"Forbidden strings", L"")
	BOOLEAN (L"Check matching parentheses", 0)
	SENTENCE (L"Separating characters", L"")
	BOOLEAN (L"Allow all parenthesized", 0)
	LABEL (L"", L"-- Capitals --")
	BOOLEAN (L"Allow all names", 0)
	SENTENCE (L"Name prefixes", L"")
	BOOLEAN (L"Allow all abbreviations", 0)
	LABEL (L"", L"-- Capitalization --")
	BOOLEAN (L"Allow caps sentence-initially", 0)
	BOOLEAN (L"Allow caps after colon", 0)
	LABEL (L"", L"-- Word parts --")
	SENTENCE (L"Allow all words containing", L"")
	SENTENCE (L"Allow all words starting with", L"")
	SENTENCE (L"Allow all words ending in", L"")
	OK
int IOBJECT;
LOOP {
	iam (SpellingChecker);
	SET_STRING (L"Forbidden strings", my forbiddenStrings)
	SET_INTEGER (L"Check matching parentheses", my checkMatchingParentheses)
	SET_STRING (L"Separating characters", my separatingCharacters)
	SET_INTEGER (L"Allow all parenthesized", my allowAllParenthesized)
	SET_INTEGER (L"Allow all names", my allowAllNames)
	SET_STRING (L"Name prefixes", my namePrefixes)
	SET_INTEGER (L"Allow all abbreviations", my allowAllAbbreviations)
	SET_INTEGER (L"Allow caps sentence-initially", my allowCapsSentenceInitially)
	SET_INTEGER (L"Allow caps after colon", my allowCapsAfterColon)
	SET_STRING (L"Allow all words containing", my allowAllWordsContaining)
	SET_STRING (L"Allow all words starting with", my allowAllWordsStartingWith)
	SET_STRING (L"Allow all words ending in", my allowAllWordsEndingIn)
}
DO
	LOOP {
		iam (SpellingChecker);
		Melder_free (my forbiddenStrings); my forbiddenStrings = Melder_wcsdup_f (GET_STRING (L"Forbidden strings"));
		my checkMatchingParentheses = GET_INTEGER (L"Check matching parentheses");
		Melder_free (my separatingCharacters); my separatingCharacters = Melder_wcsdup_f (GET_STRING (L"Separating characters"));
		my allowAllParenthesized = GET_INTEGER (L"Allow all parenthesized");
		my allowAllNames = GET_INTEGER (L"Allow all names");
		Melder_free (my namePrefixes); my namePrefixes = Melder_wcsdup_f (GET_STRING (L"Name prefixes"));
		my allowAllAbbreviations = GET_INTEGER (L"Allow all abbreviations");
		my allowCapsSentenceInitially = GET_INTEGER (L"Allow caps sentence-initially");
		my allowCapsAfterColon = GET_INTEGER (L"Allow caps after colon");
		Melder_free (my allowAllWordsContaining); my allowAllWordsContaining = Melder_wcsdup_f (GET_STRING (L"Allow all words containing"));
		Melder_free (my allowAllWordsStartingWith); my allowAllWordsStartingWith = Melder_wcsdup_f (GET_STRING (L"Allow all words starting with"));
		Melder_free (my allowAllWordsEndingIn); my allowAllWordsEndingIn = Melder_wcsdup_f (GET_STRING (L"Allow all words ending in"));
		praat_dataChanged (me);
	}
END

DIRECT (SpellingChecker_extractWordList)
	LOOP {
		iam (SpellingChecker);
		autoWordList thee = SpellingChecker_extractWordList (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (SpellingChecker_extractUserDictionary)
	LOOP {
		iam (SpellingChecker);
		autoSortedSetOfString thee = SpellingChecker_extractUserDictionary (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (SpellingChecker_isWordAllowed, L"Is word allowed?", L"SpellingChecker")
	SENTENCE (L"Word", L"")
	OK
DO
	LOOP {
		iam (SpellingChecker);
		bool isWordAllowed = SpellingChecker_isWordAllowed (me, GET_STRING (L"Word"));
		Melder_information (isWordAllowed ? L"1 (allowed)" : L"0 (not allowed)");
	}
END

FORM (SpellingChecker_nextNotAllowedWord, L"Next not allowed word?", L"SpellingChecker")
	LABEL (L"", L"Sentence:")
	TEXTFIELD (L"sentence", L"")
	INTEGER (L"Starting character", L"0")
	OK
DO
	LOOP {
		iam (SpellingChecker);
		wchar_t *sentence = GET_STRING (L"sentence");
		long startingCharacter = GET_INTEGER (L"Starting character");
		if (startingCharacter < 0) Melder_throw ("Starting character should be 0 or positive.");
		if (startingCharacter > (int) wcslen (sentence)) Melder_throw ("Starting character should not exceed end of sentence.");
		wchar_t *nextNotAllowedWord = SpellingChecker_nextNotAllowedWord (me, sentence, & startingCharacter);
		Melder_information (nextNotAllowedWord);
	}
END

DIRECT (SpellingChecker_replaceWordList)
	SpellingChecker spellingChecker = NULL;
	WordList wordList = NULL;
	LOOP {
		if (CLASS == classSpellingChecker) spellingChecker = (SpellingChecker) OBJECT;
		if (CLASS == classWordList) wordList = (WordList) OBJECT;
		SpellingChecker_replaceWordList (spellingChecker, wordList);
		praat_dataChanged (spellingChecker);
	}
END

DIRECT (SpellingChecker_replaceWordList_help)
	Melder_information (L"To replace the checker's word list\nby the contents of a Strings object:\n"
		"1. select the Strings;\n2. convert to a WordList object;\n3. select the SpellingChecker and the WordList;\n"
		"4. choose Replace.");
END

DIRECT (SpellingChecker_replaceUserDictionary)
	SpellingChecker spellingChecker = NULL;
	SortedSetOfString dictionary = NULL;
	LOOP {
		if (CLASS == classSpellingChecker) spellingChecker = (SpellingChecker) OBJECT;
		if (CLASS == classSortedSetOfString) dictionary = (SortedSetOfString) OBJECT;
		SpellingChecker_replaceUserDictionary (spellingChecker, dictionary);
	}
END

/***** TEXTGRID *****/

FORM (TextGrid_countLabels, L"Count labels", L"TextGrid: Count labels...")
	INTEGER (STRING_TIER_NUMBER, L"1")
	SENTENCE (L"Label text", L"a")
	OK
DO
	LOOP {
		iam (TextGrid);
		long numberOfLabels = TextGrid_countLabels (me, GET_INTEGER (STRING_TIER_NUMBER), GET_STRING (L"Label text"));
		Melder_information (Melder_integer (numberOfLabels), L" labels");
	}
END

FORM (TextGrid_downto_Table, L"TextGrid: Down to Table", 0)
	BOOLEAN (L"Include line number", false)
	NATURAL (L"Time decimals", L"6")
	BOOLEAN (L"Include tier names", true)
	BOOLEAN (L"Include empty intervals", false)
	OK
DO
	LOOP {
		iam (TextGrid);
		autoTable thee = TextGrid_downto_Table (me, GET_INTEGER (L"Include line number"), GET_INTEGER (L"Time decimals"),
			GET_INTEGER (L"Include tier names"), GET_INTEGER (L"Include empty intervals"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (TextGrid_draw, L"TextGrid: Draw", 0)
	praat_dia_timeRange (dia);
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (TextGrid);
		TextGrid_Sound_draw (me, NULL, GRAPHICS,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Show boundaries"),
			GET_INTEGER (L"Use text styles"), GET_INTEGER (L"Garnish"));
	}
END

FORM (TextGrid_duplicateTier, L"TextGrid: Duplicate tier", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (L"Position", L"1 (= at top)")
	WORD (L"Name", L"")
	OK
DO
	LOOP {
		iam (TextGrid);
		int itier = GET_INTEGER (STRING_TIER_NUMBER);
		int position = GET_INTEGER (L"Position");
		const wchar_t *name = GET_STRING (L"Name");
		if (itier > my tiers -> size) itier = my tiers -> size;
		autoAnyTier newTier = Data_copy ((AnyTier) my tiers -> item [itier]);
		Thing_setName (newTier.peek(), name);
		Ordered_addItemPos (my tiers, newTier.transfer(), position);
		praat_dataChanged (me);
	}
END

static void cb_TextGridEditor_publication (Editor editor, void *closure, Data publication) {
	(void) editor;
	(void) closure;
	/*
	 * Keep the gate for error handling.
	 */
	try {
		praat_new (publication, NULL);
		praat_updateSelection ();
		if (Thing_member (publication, classSpectrum) && wcsequ (Thing_getName (publication), L"slice")) {
			int IOBJECT;
			LOOP {
				iam (Spectrum);
				autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
				praat_installEditor (editor2.transfer(), IOBJECT);
			}
		}
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
}
DIRECT (TextGrid_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a TextGrid from batch.");
	Sound sound = NULL;
	LOOP {
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay NULL
	}
	LOOP if (CLASS == classTextGrid) {
		iam (TextGrid);
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, sound, true, NULL);
		editor -> setPublicationCallback (cb_TextGridEditor_publication, NULL);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

DIRECT (TextGrid_LongSound_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a TextGrid from batch.");
	LongSound longSound = NULL;
	int ilongSound = 0;
	LOOP {
		if (CLASS == classLongSound) longSound = (LongSound) OBJECT, ilongSound = IOBJECT;
	}
	Melder_assert (ilongSound != 0);
	LOOP if (CLASS == classTextGrid) {
		iam (TextGrid);
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, longSound, false, NULL);
		editor -> setPublicationCallback (cb_TextGridEditor_publication, NULL);
		praat_installEditor2 (editor.transfer(), IOBJECT, ilongSound);
	}
END

DIRECT (TextGrid_SpellingChecker_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a TextGrid from batch.");
	SpellingChecker spellingChecker = NULL;
	int ispellingChecker = 0;
	Sound sound = NULL;
	LOOP {
		if (CLASS == classSpellingChecker) spellingChecker = (SpellingChecker) OBJECT, ispellingChecker = IOBJECT;
		if (CLASS == classSound) sound = (Sound) OBJECT;   // may stay NULL
	}
	Melder_assert (ispellingChecker != 0);
	LOOP if (CLASS == classTextGrid) {
		iam (TextGrid);
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, sound, true, spellingChecker);
		praat_installEditor2 (editor.transfer(), IOBJECT, ispellingChecker);
	}
END

DIRECT (TextGrid_LongSound_SpellingChecker_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a TextGrid from batch.");
	LongSound longSound = NULL;
	SpellingChecker spellingChecker = NULL;
	int ilongSound = 0, ispellingChecker = 0;
	LOOP {
		if (CLASS == classLongSound) longSound = (LongSound) OBJECT, ilongSound = IOBJECT;
		if (CLASS == classSpellingChecker) spellingChecker = (SpellingChecker) OBJECT, ispellingChecker = IOBJECT;
	}
	Melder_assert (ilongSound != 0 && ispellingChecker != 0);
	LOOP if (CLASS == classTextGrid) {
		iam (TextGrid);
		autoTextGridEditor editor = TextGridEditor_create (ID_AND_FULL_NAME, me, longSound, false, spellingChecker);
		praat_installEditor3 (editor.transfer(), IOBJECT, ilongSound, ispellingChecker);
	}
END

FORM (TextGrid_extractPart, L"TextGrid: Extract part", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"1.0")
	BOOLEAN (L"Preserve times", 0)
	OK
DO
	LOOP {
		iam (TextGrid);
		autoTextGrid thee = TextGrid_extractPart (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Preserve times"));
		praat_new (thee.transfer(), my name, L"_part");
	}
END

static Function pr_TextGrid_peekTier (Any dia) {
	int IOBJECT;
	LOOP {
		iam (TextGrid);
		long tierNumber = GET_INTEGER (STRING_TIER_NUMBER);
		if (tierNumber > my tiers -> size)
			Melder_throw ("Tier number (", tierNumber, ") should not be larger than number of tiers (", my tiers -> size, ").");
		return (Function) my tiers -> item [tierNumber];
	}
	return NULL;   // should not occur
}

static IntervalTier pr_TextGrid_peekIntervalTier (Any dia) {
	Function tier = pr_TextGrid_peekTier (dia);
	if (tier -> classInfo != classIntervalTier) Melder_throw ("Tier should be interval tier.");
	return (IntervalTier) tier;
}

static TextTier pr_TextGrid_peekTextTier (Any dia) {
	Function tier = pr_TextGrid_peekTier (dia);
	if (! tier) return NULL;
	if (tier -> classInfo != classTextTier) Melder_throw ("Tier should be point tier (TextTier).");
	return (TextTier) tier;
}

static TextInterval pr_TextGrid_peekInterval (Any dia) {
	int intervalNumber = GET_INTEGER (STRING_INTERVAL_NUMBER);
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (dia);
	if (intervalNumber > intervalTier -> intervals -> size) Melder_throw ("Interval number too large.");
	return (TextInterval) intervalTier -> intervals -> item [intervalNumber];
}

static TextPoint pr_TextGrid_peekPoint (Any dia) {	
	long pointNumber = GET_INTEGER (STRING_POINT_NUMBER);
	TextTier textTier = pr_TextGrid_peekTextTier (dia);
	if (pointNumber > textTier -> points -> size) Melder_throw ("Point number too large.");
	return (TextPoint) textTier -> points -> item [pointNumber];
}

FORM (TextGrid_extractOneTier, L"TextGrid: Extract one tier", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	Function tier = pr_TextGrid_peekTier (dia);   // a reference
	autoTextGrid grid = TextGrid_createWithoutTiers (1e30, -1e30);
	TextGrid_addTier (grid.peek(), tier);   // no transfer of tier ownership, because a copy is made
	praat_new (grid.transfer(), tier -> name);
END

FORM (TextGrid_extractTier, L"TextGrid: Extract tier", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	Function tier = pr_TextGrid_peekTier (dia);
	autoFunction thee = Data_copy (tier);
	praat_new (thee.transfer(), tier -> name);
END

DIRECT (TextGrid_genericize)
	LOOP {
		iam (TextGrid);
		TextGrid_genericize (me);
		praat_dataChanged (me);
	}
END

DIRECT (TextGrid_nativize)
	LOOP {
		iam (TextGrid);
		TextGrid_nativize (me);
		praat_dataChanged (me);
	}
END

FORM (TextGrid_getHighIndexFromTime, L"Get high index", L"AnyTier: Get high index from time...")
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	TextTier textTier = pr_TextGrid_peekTextTier (dia);
	long highIndex = AnyTier_timeToHighIndex (textTier, GET_REAL (L"Time"));
	Melder_information (Melder_integer (highIndex));
END

FORM (TextGrid_getLowIndexFromTime, L"Get low index", L"AnyTier: Get low index from time...")
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	TextTier textTier = pr_TextGrid_peekTextTier (dia);
	long lowIndex = AnyTier_timeToLowIndex (textTier, GET_REAL (L"Time"));
	Melder_information (Melder_integer (lowIndex));
END

FORM (TextGrid_getNearestIndexFromTime, L"Get nearest index", L"AnyTier: Get nearest index from time...")
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	TextTier textTier = pr_TextGrid_peekTextTier (dia);
	long nearestIndex = AnyTier_timeToNearestIndex (textTier, GET_REAL (L"Time"));
	Melder_information (Melder_integer (nearestIndex));
END

FORM (TextGrid_getIntervalAtTime, L"TextGrid: Get interval at time", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (dia);
	long index = IntervalTier_timeToIndex (intervalTier, GET_REAL (L"Time"));
	Melder_information (Melder_integer (index));
END

FORM (TextGrid_getNumberOfIntervals, L"TextGrid: Get number of intervals", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	IntervalTier intervalTier = pr_TextGrid_peekIntervalTier (dia);
	long numberOfIntervals = intervalTier -> intervals -> size;
	Melder_information (Melder_integer (numberOfIntervals));
END

DIRECT (TextGrid_getNumberOfTiers)
	LOOP {
		iam (TextGrid);
		long numberOfTiers = my tiers -> size;
		Melder_information (Melder_integer (numberOfTiers));
	}
END

FORM (TextGrid_getStartingPoint, L"TextGrid: Get start point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"1")
	OK
DO
	TextInterval interval = pr_TextGrid_peekInterval (dia);
	double startingPoint = interval -> xmin;
	Melder_informationReal (startingPoint, L"seconds");
END

FORM (TextGrid_getEndPoint, L"TextGrid: Get end point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"1")
	OK
DO
	TextInterval interval = pr_TextGrid_peekInterval (dia);
	double endPoint = interval -> xmax;
	Melder_informationReal (endPoint, L"seconds");
END
	
FORM (TextGrid_getLabelOfInterval, L"TextGrid: Get label of interval", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"1")
	OK
DO
	TextInterval interval = pr_TextGrid_peekInterval (dia);
	MelderInfo_open ();
	MelderInfo_write (interval -> text);
	MelderInfo_close ();
END
	
FORM (TextGrid_getNumberOfPoints, L"TextGrid: Get number of points", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	TextTier textTier = pr_TextGrid_peekTextTier (dia);
	long numberOfPoints = textTier -> points -> size;
	Melder_information (Melder_integer (numberOfPoints));
END
	
FORM (TextGrid_getTierName, L"TextGrid: Get tier name", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	Data tier = pr_TextGrid_peekTier (dia);
	Melder_information (tier -> name);
END

FORM (TextGrid_getTimeOfPoint, L"TextGrid: Get time of point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_POINT_NUMBER, L"1")
	OK
DO
	TextPoint point = pr_TextGrid_peekPoint (dia);
	Melder_informationReal (point -> number, L"seconds");
END
	
FORM (TextGrid_getLabelOfPoint, L"TextGrid: Get label of point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_POINT_NUMBER, L"1")
	OK
DO
	TextPoint point = pr_TextGrid_peekPoint (dia);
	Melder_information (point -> mark);
END
	
DIRECT (TextGrid_help) Melder_help (L"TextGrid"); END

FORM (TextGrid_insertBoundary, L"TextGrid: Insert boundary", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (TextGrid);
		TextGrid_insertBoundary (me, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL (L"Time"));
		praat_dataChanged (me);
	}
END

FORM (TextGrid_insertIntervalTier, L"TextGrid: Insert interval tier", 0)
	NATURAL (L"Position", L"1 (= at top)")
	WORD (L"Name", L"")
	OK
DO
	LOOP {
		iam (TextGrid);
		int position = GET_INTEGER (L"Position");
		wchar_t *name = GET_STRING (L"Name");
		autoIntervalTier tier = IntervalTier_create (my xmin, my xmax);
		if (position > my tiers -> size) position = my tiers -> size + 1;
		Thing_setName (tier.peek(), name);
		Ordered_addItemPos (my tiers, tier.transfer(), position);
		praat_dataChanged (me);
	}
END

FORM (TextGrid_insertPoint, L"TextGrid: Insert point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	LABEL (L"", L"Text:")
	TEXTFIELD (L"text", L"")
	OK
DO
	LOOP {
		iam (TextGrid);
		TextGrid_insertPoint (me, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL (L"Time"), GET_STRING (L"text"));
		praat_dataChanged (me);
	}
END

FORM (TextGrid_insertPointTier, L"TextGrid: Insert point tier", 0)
	NATURAL (L"Position", L"1 (= at top)")
	WORD (L"Name", L"")
	OK
DO
	LOOP {
		iam (TextGrid);
		int position = GET_INTEGER (L"Position");
		wchar_t *name = GET_STRING (L"Name");
		autoTextTier tier = TextTier_create (my xmin, my xmax);
		if (position > my tiers -> size) position = my tiers -> size + 1;
		Thing_setName (tier.peek(), name);
		Ordered_addItemPos (my tiers, tier.transfer(), position);
		praat_dataChanged (me);
	}
END

FORM (TextGrid_isIntervalTier, L"TextGrid: Is interval tier?", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	Data tier = pr_TextGrid_peekTier (dia);
	if (tier -> classInfo == classIntervalTier) {
		Melder_information (L"1 (yes, tier ", Melder_integer (GET_INTEGER (STRING_TIER_NUMBER)), L" is an interval tier)");
	} else {
		Melder_information (L"0 (no, tier ", Melder_integer (GET_INTEGER (STRING_TIER_NUMBER)), L" is a point tier)");
	}
END

FORM (TextGrid_list, L"TextGrid: List", 0)
	BOOLEAN (L"Include line number", false)
	NATURAL (L"Time decimals", L"6")
	BOOLEAN (L"Include tier names", true)
	BOOLEAN (L"Include empty intervals", false)
	OK
DO
	LOOP {
		iam (TextGrid);
		TextGrid_list (me, GET_INTEGER (L"Include line number"), GET_INTEGER (L"Time decimals"),
			GET_INTEGER (L"Include tier names"), GET_INTEGER (L"Include empty intervals"));
	}
END

DIRECT (TextGrids_concatenate)
	autoCollection textGrids = praat_getSelectedObjects ();
	autoTextGrid thee = TextGrids_concatenate (textGrids.peek());
	praat_new (thee.transfer(), L"chain");
END

DIRECT (TextGrids_merge)
	autoCollection textGrids = praat_getSelectedObjects ();
	autoTextGrid thee = TextGrid_merge (textGrids.peek());
	praat_new (thee.transfer(), L"merged");
END

DIRECT (info_TextGrid_Pitch_draw)
	Melder_information (L"You can draw a TextGrid together with a Pitch after selecting them both.");
END

FORM (TextGrid_removeBoundaryAtTime, L"TextGrid: Remove boundary at time", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (TextGrid);
		TextGrid_removeBoundaryAtTime (me, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL (L"Time"));
		praat_dataChanged (me);
	}
END

FORM (TextGrid_getCentrePoints, L"TextGrid: Get centre points", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OPTIONMENU_ENUM (L"Get centre points where label", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"hi")
	OK
DO
	wchar_t *text = GET_STRING (L"...the text");
	LOOP {
		iam (TextGrid);
		autoPointProcess thee = TextGrid_getCentrePoints (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, L"Get centre points where label"), text);
		praat_new (thee.transfer(), my name, L"_", text);
	}
END

FORM (TextGrid_getEndPoints, L"TextGrid: Get end points", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OPTIONMENU_ENUM (L"Get end points where label", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"hi")
	OK
DO
	wchar_t *text = GET_STRING (L"...the text");
	LOOP {
		iam (TextGrid);
		autoPointProcess thee = TextGrid_getEndPoints (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, L"Get end points where label"), text);
		praat_new (thee.transfer(), my name, L"_", text);
	}
END

FORM (TextGrid_getStartingPoints, L"TextGrid: Get starting points", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OPTIONMENU_ENUM (L"Get starting points where label", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"hi")
	OK
DO
	wchar_t *text = GET_STRING (L"...the text");
	LOOP {
		iam (TextGrid);
		autoPointProcess thee = TextGrid_getStartingPoints (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, L"Get starting points where label"), text);
		praat_new (thee.transfer(), my name, L"_", text);
	}
END

FORM (TextGrid_getPoints, L"Get points", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OPTIONMENU_ENUM (L"Get points where label", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"hi")
	OK
DO
	wchar_t *text = GET_STRING (L"...the text");
	LOOP {
		iam (TextGrid);
		autoPointProcess thee = TextGrid_getPoints (me, GET_INTEGER (STRING_TIER_NUMBER),
			GET_ENUM (kMelder_string, L"Get points where label"), text);
		praat_new (thee.transfer(), my name, L"_", text);
	}
END

FORM (TextGrid_removeLeftBoundary, L"TextGrid: Remove left boundary", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"2")
	OK
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long iinterval = GET_INTEGER (STRING_INTERVAL_NUMBER);
	LOOP {
		iam (TextGrid);
		IntervalTier intervalTier;
		if (itier > my tiers -> size)
			Melder_throw ("You cannot remove a boundary from tier ", itier, " of ", me,
				", because that TextGrid has only ", my tiers -> size, " tiers.");
		intervalTier = (IntervalTier) my tiers -> item [itier];
		if (intervalTier -> classInfo != classIntervalTier)
			Melder_throw ("You cannot remove a boundary from tier ", itier, " of ", me,
				", because that tier is a point tier instead of an interval tier.");
		if (iinterval > intervalTier -> intervals -> size)
			Melder_throw ("You cannot remove a boundary from interval ", iinterval, " of tier ", itier, " of ", me,
				", because that tier has only ", intervalTier -> intervals -> size, " intervals.");
		if (iinterval == 1)
			Melder_throw ("You cannot remove the left boundary from interval 1 of tier ", itier, " of ", me,
				", because this is at the left edge of the tier.");
		IntervalTier_removeLeftBoundary (intervalTier, iinterval);
		praat_dataChanged (me);
	}
END

FORM (TextGrid_removePoint, L"TextGrid: Remove point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_POINT_NUMBER, L"2")
	OK
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long ipoint = GET_INTEGER (STRING_POINT_NUMBER);
	LOOP {
		iam (TextGrid);
		TextTier pointTier;
		if (itier > my tiers -> size)
			Melder_throw ("You cannot remove a point from tier ", itier, " of ", me,
				", because that TextGrid has only ", my tiers -> size, " tiers.");
		pointTier = (TextTier) my tiers -> item [itier];
		if (pointTier -> classInfo != classTextTier)
			Melder_throw ("You cannot remove a point from tier ", itier, " of ", me,
				", because that tier is an interval tier instead of a point tier.");
		if (ipoint > pointTier -> points -> size)
			Melder_throw ("You cannot remove point ", ipoint, " from tier ", itier, " of ", me,
				", because that tier has only ", pointTier -> points -> size, " points.");
		TextTier_removePoint (pointTier, ipoint);
		praat_dataChanged (me);
	}
END

FORM (TextGrid_removeRightBoundary, L"TextGrid: Remove right boundary", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"1")
	OK
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long iinterval = GET_INTEGER (STRING_INTERVAL_NUMBER);
	LOOP {
		iam (TextGrid);
		IntervalTier intervalTier;
		if (itier > my tiers -> size)
			Melder_throw ("You cannot remove a boundary from tier ", itier, " of ", me,
				", because that TextGrid has only ", my tiers -> size, " tiers.");
		intervalTier = (IntervalTier) my tiers -> item [itier];
		if (intervalTier -> classInfo != classIntervalTier)
			Melder_throw ("You cannot remove a boundary from tier ", itier, " of ", me,
				L", because that tier is a point tier instead of an interval tier.");
		if (iinterval > intervalTier -> intervals -> size)
			Melder_throw ("You cannot remove a boundary from interval ", iinterval, " of tier ", itier, " of ", me,
				", because that tier has only ", intervalTier -> intervals -> size, " intervals.");
		if (iinterval == intervalTier -> intervals -> size)
			Melder_throw ("You cannot remove the right boundary from interval ", iinterval, " of tier ", itier, " of ", me,
				", because this is at the right edge of the tier.");
		IntervalTier_removeLeftBoundary (intervalTier, iinterval + 1);
		praat_dataChanged (me);
	}
END

FORM (TextGrid_removeTier, L"TextGrid: Remove tier", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	LOOP {
		iam (TextGrid);
		int itier = GET_INTEGER (STRING_TIER_NUMBER);
		if (my tiers -> size <= 1)
			Melder_throw (L"Sorry, I refuse to remove the last tier.");
		if (itier > my tiers -> size) itier = my tiers -> size;
		Collection_removeItem (my tiers, itier);
		praat_dataChanged (me);
	}
END

DIRECT (info_TextGrid_Sound_edit)
	Melder_information (L"To include a copy of a Sound in your TextGrid editor:\n"
		"   select a TextGrid and a Sound, and click \"View & Edit\".");
END

DIRECT (info_TextGrid_Sound_draw)
	Melder_information (L"You can draw a TextGrid together with a Sound after selecting them both.");
END

FORM (TextGrid_setIntervalText, L"TextGrid: Set interval text", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"1")
	LABEL (L"", L"Text:")
	TEXTFIELD (L"text", L"")
	OK
DO
	LOOP {
		iam (TextGrid);
		TextGrid_setIntervalText (me, GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (STRING_INTERVAL_NUMBER), GET_STRING (L"text"));
		praat_dataChanged (me);
	}
END

FORM (TextGrid_setPointText, L"TextGrid: Set point text", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_POINT_NUMBER, L"1")
	LABEL (L"", L"Text:")
	TEXTFIELD (L"text", L"")
	OK
DO
	LOOP {
		iam (TextGrid);
		TextGrid_setPointText (me, GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (STRING_POINT_NUMBER), GET_STRING (L"text"));
		praat_dataChanged (me);
	}
END

FORM_WRITE (TextGrid_writeToChronologicalTextFile, L"Text file", 0, 0)
	LOOP {
		iam (TextGrid);
		TextGrid_writeToChronologicalTextFile (me, file);
	}
END

/***** TEXTGRID & ANYTIER *****/

DIRECT (TextGrid_AnyTier_append)
	TextGrid oldGrid = NULL;
	LOOP {
		if (CLASS == classTextGrid) oldGrid = (TextGrid) OBJECT;
	}
	autoTextGrid newGrid = Data_copy (oldGrid);
	LOOP if (OBJECT != oldGrid) {
		iam (AnyTier);
		TextGrid_addTier (newGrid.peek(), me);
	}
	praat_new (newGrid.transfer(), oldGrid -> name);
END

/***** TEXTGRID & LONGSOUND *****/

DIRECT (TextGrid_LongSound_scaleTimes)
	TextGrid grid = NULL;
	LongSound longSound = NULL;
	LOOP {
		if (CLASS == classTextGrid) grid = (TextGrid) OBJECT;
		if (CLASS == classLongSound) longSound = (LongSound) OBJECT;
	}
	Function_scaleXTo (grid, longSound -> xmin, longSound -> xmax);
	praat_dataChanged (grid);
END

/***** TEXTTIER *****/

FORM (TextTier_addPoint, L"TextTier: Add point", L"TextTier: Add point...")
	REAL (L"Time (s)", L"0.5")
	SENTENCE (L"Text", L"")
	OK
DO
	LOOP {
		iam (TextTier);
		TextTier_addPoint (me, GET_REAL (L"Time"), GET_STRING (L"Text"));
		praat_dataChanged (me);
	}
END

DIRECT (TextTier_downto_PointProcess)
	LOOP {
		iam (TextTier);
		autoPointProcess thee = AnyTier_downto_PointProcess (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (TextTier_downto_TableOfReal, L"TextTier: Down to TableOfReal", 0)
	SENTENCE (L"Label", L"")
	OK
DO
	LOOP {
		iam (TextTier);
		autoTableOfReal thee = TextTier_downto_TableOfReal (me, GET_STRING (L"Label"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (TextTier_downto_TableOfReal_any)
	LOOP {
		iam (TextTier);
		autoTableOfReal thee = TextTier_downto_TableOfReal_any (me);
		praat_new (thee.transfer(), my name);
	}
END

FORM (TextTier_getLabelOfPoint, L"Get label of point", 0)
	NATURAL (L"Point number", L"1")
	OK
DO
	LOOP {
		iam (TextTier);
		long ipoint = GET_INTEGER (L"Point number");
		if (ipoint > my points -> size) Melder_throw ("No such point.");
		TextPoint point = (TextPoint) my points -> item [ipoint];
		Melder_information (point -> mark);
	}
END

FORM (TextTier_getPoints, L"Get points", 0)
	SENTENCE (L"Text", L"")
	OK
DO
	LOOP {
		iam (TextTier);
		autoPointProcess thee = TextTier_getPoints (me, GET_STRING (L"Text"));
		praat_new (thee.transfer(), GET_STRING (L"Text"));
	}
END

DIRECT (TextTier_help) Melder_help (L"TextTier"); END

/***** WORDLIST *****/

FORM (WordList_hasWord, L"Does word occur in list?", L"WordList")
	SENTENCE (L"Word", L"")
	OK
DO
	LOOP {
		iam (WordList);
		bool hasWord = WordList_hasWord (me, GET_STRING (L"Word"));
		Melder_information (hasWord ? L"1" : L"0");
	}
END

DIRECT (WordList_to_Strings)
	LOOP {
		iam (WordList);
		autoStrings thee = WordList_to_Strings (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (WordList_upto_SpellingChecker)
	LOOP {
		iam (WordList);
		autoSpellingChecker thee = WordList_upto_SpellingChecker (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** buttons *****/

void praat_TimeFunction_query_init (ClassInfo klas);
void praat_TimeTier_query_init (ClassInfo klas);
void praat_TimeTier_modify_init (ClassInfo klas);

void praat_uvafon_TextGrid_init (void);
void praat_uvafon_TextGrid_init (void) {
	Thing_recognizeClassByOtherName (classTextTier, L"MarkTier");

	structTextGridEditor :: f_preferences ();

	praat_addAction1 (classIntervalTier, 0, L"IntervalTier help", 0, 0, DO_IntervalTier_help);
	praat_addAction1 (classIntervalTier, 1, L"Save as Xwaves label file...", 0, 0, DO_IntervalTier_writeToXwaves);
	praat_addAction1 (classIntervalTier, 1, L"Write to Xwaves label file...", 0, praat_HIDDEN, DO_IntervalTier_writeToXwaves);
	praat_addAction1 (classIntervalTier, 0, L"Collect", 0, 0, 0);
	praat_addAction1 (classIntervalTier, 0, L"Into TextGrid", 0, 0, DO_AnyTier_into_TextGrid);
	praat_addAction1 (classIntervalTier, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classIntervalTier, 0, L"Get starting points...", 0, 0, DO_IntervalTier_getStartingPoints);
	praat_addAction1 (classIntervalTier, 0, L"Get centre points...", 0, 0, DO_IntervalTier_getCentrePoints);
	praat_addAction1 (classIntervalTier, 0, L"Get end points...", 0, 0, DO_IntervalTier_getEndPoints);
	praat_addAction1 (classIntervalTier, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classIntervalTier, 0, L"Down to TableOfReal (any)", 0, 0, DO_IntervalTier_downto_TableOfReal_any);
	praat_addAction1 (classIntervalTier, 0, L"Down to TableOfReal...", 0, 0, DO_IntervalTier_downto_TableOfReal);

	praat_addAction1 (classLabel, 0, L"& Sound: To TextGrid?", 0, 0, DO_info_Label_Sound_to_TextGrid);

	praat_addAction1 (classSpellingChecker, 1, L"View & Edit...", 0, praat_ATTRACTIVE, DO_SpellingChecker_edit);
	praat_addAction1 (classSpellingChecker, 1, L"Edit...", 0, praat_HIDDEN, DO_SpellingChecker_edit);
	praat_addAction1 (classSpellingChecker, 0, L"Query", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 1, L"Is word allowed...", 0, 0, DO_SpellingChecker_isWordAllowed);
	praat_addAction1 (classSpellingChecker, 1, L"Next not allowed word...", 0, 0, DO_SpellingChecker_nextNotAllowedWord);
	praat_addAction1 (classSpellingChecker, 0, L"Modify", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 0, L"Add new word...", 0, 0, DO_SpellingChecker_addNewWord);
	praat_addAction1 (classSpellingChecker, 0, L"Analyze", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 0, L"Extract WordList", 0, 0, DO_SpellingChecker_extractWordList);
	praat_addAction1 (classSpellingChecker, 0, L"Extract user dictionary", 0, 0, DO_SpellingChecker_extractUserDictionary);

	praat_addAction1 (classTextGrid, 0, L"TextGrid help", 0, 0, DO_TextGrid_help);
	praat_addAction1 (classTextGrid, 1, L"Save as chronological text file...", 0, 0, DO_TextGrid_writeToChronologicalTextFile);
	praat_addAction1 (classTextGrid, 1, L"Write to chronological text file...", 0, praat_HIDDEN, DO_TextGrid_writeToChronologicalTextFile);
	praat_addAction1 (classTextGrid, 1, L"View & Edit alone", 0, 0, DO_TextGrid_edit);
	praat_addAction1 (classTextGrid, 1, L"View & Edit", 0, praat_HIDDEN, DO_TextGrid_edit);
	praat_addAction1 (classTextGrid, 1, L"Edit", 0, praat_HIDDEN, DO_TextGrid_edit);
	praat_addAction1 (classTextGrid, 1, L"View & Edit with Sound?", 0, praat_ATTRACTIVE, DO_info_TextGrid_Sound_edit);
	praat_addAction1 (classTextGrid, 0, L"Draw -", 0, 0, 0);
	praat_addAction1 (classTextGrid, 0, L"Draw...", 0, 1, DO_TextGrid_draw);
	praat_addAction1 (classTextGrid, 1, L"Draw with Sound?", 0, 1, DO_info_TextGrid_Sound_draw);
	praat_addAction1 (classTextGrid, 1, L"Draw with Pitch?", 0, 1, DO_info_TextGrid_Pitch_draw);
	praat_addAction1 (classTextGrid, 1, L"List...", 0, 0, DO_TextGrid_list);
	praat_addAction1 (classTextGrid, 0, L"Down to Table...", 0, 0, DO_TextGrid_downto_Table);
	praat_addAction1 (classTextGrid, 0, L"Query -", 0, 0, 0);
		praat_TimeFunction_query_init (classTextGrid);
		praat_addAction1 (classTextGrid, 1, L"-- query textgrid --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, L"Get number of tiers", 0, 1, DO_TextGrid_getNumberOfTiers);
		praat_addAction1 (classTextGrid, 1, L"Get tier name...", 0, 1, DO_TextGrid_getTierName);
		praat_addAction1 (classTextGrid, 1, L"Is interval tier...", 0, 1, DO_TextGrid_isIntervalTier);
		praat_addAction1 (classTextGrid, 1, L"-- query tier --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, L"Query interval tier", 0, 1, 0);
			praat_addAction1 (classTextGrid, 1, L"Get number of intervals...", 0, 2, DO_TextGrid_getNumberOfIntervals);
			praat_addAction1 (classTextGrid, 1, L"Get start point...", 0, 2, DO_TextGrid_getStartingPoint);
			praat_addAction1 (classTextGrid, 1, L"Get starting point...", 0, praat_HIDDEN + praat_DEPTH_2, DO_TextGrid_getStartingPoint);   // hidden 2008
			praat_addAction1 (classTextGrid, 1, L"Get end point...", 0, 2, DO_TextGrid_getEndPoint);
			praat_addAction1 (classTextGrid, 1, L"Get label of interval...", 0, 2, DO_TextGrid_getLabelOfInterval);
			praat_addAction1 (classTextGrid, 1, L"Get interval at time...", 0, 2, DO_TextGrid_getIntervalAtTime);
		praat_addAction1 (classTextGrid, 1, L"Query point tier", 0, 1, 0);
			praat_addAction1 (classTextGrid, 1, L"Get number of points...", 0, 2, DO_TextGrid_getNumberOfPoints);
			praat_addAction1 (classTextGrid, 1, L"Get time of point...", 0, 2, DO_TextGrid_getTimeOfPoint);
			praat_addAction1 (classTextGrid, 1, L"Get label of point...", 0, 2, DO_TextGrid_getLabelOfPoint);
			praat_addAction1 (classTextGrid, 1, L"Get low index from time...", 0, 2, DO_TextGrid_getLowIndexFromTime);
			praat_addAction1 (classTextGrid, 1, L"Get high index from time...", 0, 2, DO_TextGrid_getHighIndexFromTime);
			praat_addAction1 (classTextGrid, 1, L"Get nearest index from time...", 0, 2, DO_TextGrid_getNearestIndexFromTime);
		praat_addAction1 (classTextGrid, 1, L"-- query labels --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, L"Count labels...", 0, 1, DO_TextGrid_countLabels);
	praat_addAction1 (classTextGrid, 0, L"Modify -", 0, 0, 0);
		praat_addAction1 (classTextGrid, 0, L"Convert to backslash trigraphs", 0, 1, DO_TextGrid_genericize);
		praat_addAction1 (classTextGrid, 0, L"Genericize", 0, praat_HIDDEN + praat_DEPTH_1, DO_TextGrid_genericize);   // hidden 2007
		praat_addAction1 (classTextGrid, 0, L"Convert to Unicode", 0, 1, DO_TextGrid_nativize);
		praat_addAction1 (classTextGrid, 0, L"Nativize", 0, praat_HIDDEN + praat_DEPTH_1, DO_TextGrid_nativize);   // hidden 2007
		praat_TimeFunction_modify_init (classTextGrid);
		praat_addAction1 (classTextGrid, 0, L"-- modify tiers --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 0, L"Insert interval tier...", 0, 1, DO_TextGrid_insertIntervalTier);
		praat_addAction1 (classTextGrid, 0, L"Insert point tier...", 0, 1, DO_TextGrid_insertPointTier);
		praat_addAction1 (classTextGrid, 0, L"Duplicate tier...", 0, 1, DO_TextGrid_duplicateTier);
		praat_addAction1 (classTextGrid, 0, L"Remove tier...", 0, 1, DO_TextGrid_removeTier);
		praat_addAction1 (classTextGrid, 1, L"-- modify tier --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 0, L"Modify interval tier", 0, 1, 0);
			praat_addAction1 (classTextGrid, 0, L"Insert boundary...", 0, 2, DO_TextGrid_insertBoundary);
			praat_addAction1 (classTextGrid, 0, L"Remove left boundary...", 0, 2, DO_TextGrid_removeLeftBoundary);
			praat_addAction1 (classTextGrid, 0, L"Remove right boundary...", 0, 2, DO_TextGrid_removeRightBoundary);
			praat_addAction1 (classTextGrid, 0, L"Remove boundary at time...", 0, 2, DO_TextGrid_removeBoundaryAtTime);
			praat_addAction1 (classTextGrid, 0, L"Set interval text...", 0, 2, DO_TextGrid_setIntervalText);
		praat_addAction1 (classTextGrid, 0, L"Modify point tier", 0, 1, 0);
			praat_addAction1 (classTextGrid, 0, L"Insert point...", 0, 2, DO_TextGrid_insertPoint);
			praat_addAction1 (classTextGrid, 0, L"Remove point...", 0, 2, DO_TextGrid_removePoint);
			praat_addAction1 (classTextGrid, 0, L"Set point text...", 0, 2, DO_TextGrid_setPointText);
praat_addAction1 (classTextGrid, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classTextGrid, 1, L"Extract one tier...", 0, 0, DO_TextGrid_extractOneTier);
	praat_addAction1 (classTextGrid, 1, L"Extract tier...", 0, praat_HIDDEN, DO_TextGrid_extractTier);   // hidden 2010
	praat_addAction1 (classTextGrid, 1, L"Extract part...", 0, 0, DO_TextGrid_extractPart);
	praat_addAction1 (classTextGrid, 1, L"Analyse interval tier -", 0, 0, 0);
		praat_addAction1 (classTextGrid, 1, L"Get starting points...", 0, 1, DO_TextGrid_getStartingPoints);
		praat_addAction1 (classTextGrid, 1, L"Get end points...", 0, 1, DO_TextGrid_getEndPoints);
		praat_addAction1 (classTextGrid, 1, L"Get centre points...", 0, 1, DO_TextGrid_getCentrePoints);
	praat_addAction1 (classTextGrid, 1, L"Analyse point tier -", 0, 0, 0);
		praat_addAction1 (classTextGrid, 1, L"Get points...", 0, 1, DO_TextGrid_getPoints);
praat_addAction1 (classTextGrid, 0, L"Synthesize", 0, 0, 0);
	praat_addAction1 (classTextGrid, 0, L"Merge", 0, 0, DO_TextGrids_merge);
	praat_addAction1 (classTextGrid, 0, L"Concatenate", 0, 0, DO_TextGrids_concatenate);

	praat_addAction1 (classTextTier, 0, L"TextTier help", 0, 0, DO_TextTier_help);
	praat_addAction1 (classTextTier, 0, L"Query -", 0, 0, 0);
		praat_TimeTier_query_init (classTextTier);
		praat_addAction1 (classTextTier, 0, L"Get label of point...", 0, 1, DO_TextTier_getLabelOfPoint);
	praat_addAction1 (classTextTier, 0, L"Modify -", 0, 0, 0);
		praat_TimeTier_modify_init (classTextTier);
		praat_addAction1 (classTextTier, 0, L"Add point...", 0, 1, DO_TextTier_addPoint);
	praat_addAction1 (classTextTier, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classTextTier, 0, L"Get points...", 0, 0, DO_TextTier_getPoints);
	praat_addAction1 (classTextTier, 0, L"Collect", 0, 0, 0);
	praat_addAction1 (classTextTier, 0, L"Into TextGrid", 0, 0, DO_AnyTier_into_TextGrid);
	praat_addAction1 (classTextTier, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classTextTier, 0, L"Down to PointProcess", 0, 0, DO_TextTier_downto_PointProcess);
	praat_addAction1 (classTextTier, 0, L"Down to TableOfReal (any)", 0, 0, DO_TextTier_downto_TableOfReal_any);
	praat_addAction1 (classTextTier, 0, L"Down to TableOfReal...", 0, 0, DO_TextTier_downto_TableOfReal);

	praat_addAction1 (classWordList, 0, L"Query", 0, 0, 0);
		praat_addAction1 (classWordList, 1, L"Has word...", 0, 0, DO_WordList_hasWord);
	praat_addAction1 (classWordList, 0, L"Analyze", 0, 0, 0);
		praat_addAction1 (classWordList, 0, L"To Strings", 0, 0, DO_WordList_to_Strings);
	praat_addAction1 (classWordList, 0, L"Synthesize", 0, 0, 0);
		praat_addAction1 (classWordList, 0, L"Up to SpellingChecker", 0, 0, DO_WordList_upto_SpellingChecker);

	praat_addAction2 (classIntervalTier, 1, classPointProcess, 1, L"Start to centre...", 0, 0, DO_IntervalTier_PointProcess_startToCentre);
	praat_addAction2 (classIntervalTier, 1, classPointProcess, 1, L"End to centre...", 0, 0, DO_IntervalTier_PointProcess_endToCentre);
	praat_addAction2 (classIntervalTier, 0, classTextTier, 0, L"Collect", 0, 0, 0);
	praat_addAction2 (classIntervalTier, 0, classTextTier, 0, L"Into TextGrid", 0, 0, DO_AnyTier_into_TextGrid);
	praat_addAction2 (classLabel, 1, classSound, 1, L"To TextGrid", 0, 0, DO_Label_Sound_to_TextGrid);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_TextGrid_LongSound_edit);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, L"Edit", 0, praat_HIDDEN, DO_TextGrid_LongSound_edit);   // hidden 2011
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, L"Scale times", 0, 0, DO_TextGrid_LongSound_scaleTimes);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw -", 0, 0, 0);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw...", 0, 1, DO_TextGrid_Pitch_draw);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw logarithmic...", 0, 1, DO_TextGrid_Pitch_drawLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw semitones...", 0, 1, DO_TextGrid_Pitch_drawSemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw mel...", 0, 1, DO_TextGrid_Pitch_drawMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw erb...", 0, 1, DO_TextGrid_Pitch_drawErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Speckle...", 0, 1, DO_TextGrid_Pitch_speckle);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Speckle logarithmic...", 0, 1, DO_TextGrid_Pitch_speckleLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Speckle semitones...", 0, 1, DO_TextGrid_Pitch_speckleSemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Speckle mel...", 0, 1, DO_TextGrid_Pitch_speckleMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Speckle erb...", 0, 1, DO_TextGrid_Pitch_speckleErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"-- draw separately --", 0, 1, 0);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw separately...", 0, 1, DO_TextGrid_Pitch_drawSeparately);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw separately (logarithmic)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelyLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw separately (semitones)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelySemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw separately (mel)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelyMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw separately (erb)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelyErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Speckle separately...", 0, 1, DO_TextGrid_Pitch_speckleSeparately);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Speckle separately (logarithmic)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelyLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Speckle separately (semitones)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelySemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Speckle separately (mel)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelyMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Speckle separately (erb)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelyErb);
	praat_addAction2 (classPitch, 1, classTextTier, 1, L"To PitchTier...", 0, 0, DO_Pitch_TextTier_to_PitchTier);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_TextGrid_edit);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Edit", 0, praat_HIDDEN, DO_TextGrid_edit);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Draw...", 0, 0, DO_TextGrid_Sound_draw);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Extract -", 0, 0, 0);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Extract all intervals...", 0, praat_DEPTH_1, DO_TextGrid_Sound_extractAllIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Extract non-empty intervals...", 0, praat_DEPTH_1, DO_TextGrid_Sound_extractNonemptyIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Extract intervals...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_TextGrid_Sound_extractIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Extract intervals where...", 0, praat_DEPTH_1, DO_TextGrid_Sound_extractIntervalsWhere);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Modify TextGrid", 0, 0, 0);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Scale times", 0, 0, DO_TextGrid_Sound_scaleTimes);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Modify Sound", 0, 0, 0);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Clone time domain", 0, 0, DO_TextGrid_Sound_cloneTimeDomain);
	praat_addAction2 (classSpellingChecker, 1, classWordList, 1, L"Replace WordList", 0, 0, DO_SpellingChecker_replaceWordList);
	praat_addAction2 (classSpellingChecker, 1, classSortedSetOfString, 1, L"Replace user dictionary", 0, 0, DO_SpellingChecker_replaceUserDictionary);
	praat_addAction2 (classSpellingChecker, 1, classStrings, 1, L"Replace word list?", 0, 0, DO_SpellingChecker_replaceWordList_help);
	praat_addAction2 (classSpellingChecker, 1, classTextGrid, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_TextGrid_SpellingChecker_edit);
	praat_addAction2 (classSpellingChecker, 1, classTextGrid, 1, L"Edit", 0, praat_HIDDEN, DO_TextGrid_SpellingChecker_edit);   // hidden 2011
	praat_addAction2 (classTextGrid, 1, classTextTier, 1, L"Append", 0, 0, DO_TextGrid_AnyTier_append);
	praat_addAction2 (classTextGrid, 1, classIntervalTier, 1, L"Append", 0, 0, DO_TextGrid_AnyTier_append);

	praat_addAction3 (classLongSound, 1, classSpellingChecker, 1, classTextGrid, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_TextGrid_LongSound_SpellingChecker_edit);
	praat_addAction3 (classLongSound, 1, classSpellingChecker, 1, classTextGrid, 1, L"Edit", 0, praat_HIDDEN, DO_TextGrid_LongSound_SpellingChecker_edit);
	praat_addAction3 (classSound, 1, classSpellingChecker, 1, classTextGrid, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_TextGrid_SpellingChecker_edit);
	praat_addAction3 (classSound, 1, classSpellingChecker, 1, classTextGrid, 1, L"Edit", 0, praat_HIDDEN, DO_TextGrid_SpellingChecker_edit);
}

/* End of file praat_TextGrid_init.cpp */
