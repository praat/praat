/* praat_TextGrid_init.c
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
 * pb 2007/11/05
 */

#include "praat.h"

#include "Pitch_AnyTier_to_PitchTier.h"
#include "SpectrumEditor.h"
#include "SpellingChecker.h"
#include "TextGridEditor.h"
#include "TextGrid_extensions.h"
#include "WordList.h"

void praat_dia_timeRange (Any dia);
void praat_get_timeRange (Any dia, double *tmin, double *tmax);
int praat_get_frequencyRange (Any dia, double *fmin, double *fmax);

static const wchar_t *STRING_FROM_FREQUENCY_HZ = L"left Frequency range (Hz)";
static const wchar_t *STRING_TO_FREQUENCY_HZ = L"right Frequency range (Hz)";
static const wchar_t *STRING_TIER_NUMBER = L"Tier number";
static const wchar_t *STRING_INTERVAL_NUMBER = L"Interval number";
static const wchar_t *STRING_POINT_NUMBER = L"Point number";

void praat_TimeFunction_modify_init (void *klas);   // Modify buttons for time-based subclasses of Function.

/***** ANYTIER (generic) *****/

DIRECT (AnyTier_into_TextGrid)
	TextGrid grid = TextGrid_createWithoutTiers (1e30, -1e30);
	if (grid == NULL) return 0;
	WHERE (SELECTED) if (! TextGrid_add (grid, OBJECT)) { forget (grid); return 0; }
	if (! praat_new1 (grid, L"grid")) return 0;
END

/***** INTERVALTIER *****/

FORM (IntervalTier_downto_TableOfReal, L"IntervalTier: Down to TableOfReal", 0)
	SENTENCE (L"Label", L"")
	OK
DO
	EVERY_TO (IntervalTier_downto_TableOfReal (OBJECT, GET_STRING (L"Label")))
END

DIRECT (IntervalTier_downto_TableOfReal_any)
	EVERY_TO (IntervalTier_downto_TableOfReal_any (OBJECT))
END

FORM (IntervalTier_getCentrePoints, L"IntervalTier: Get centre points", 0)
	SENTENCE (L"Text", L"")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new1 (IntervalTier_getCentrePoints (OBJECT, GET_STRING (L"Text")), GET_STRING (L"Text"))) return 0;
END

FORM (IntervalTier_getEndPoints, L"IntervalTier: Get end points", 0)
	SENTENCE (L"Text", L"")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new1 (IntervalTier_getEndPoints (OBJECT, GET_STRING (L"Text")), GET_STRING (L"Text"))) return 0;
END

FORM (IntervalTier_getStartingPoints, L"IntervalTier: Get starting points", 0)
	SENTENCE (L"Text", L"")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new1 (IntervalTier_getStartingPoints (OBJECT, GET_STRING (L"Text")), GET_STRING (L"Text"))) return 0;
END

DIRECT (IntervalTier_help) Melder_help (L"IntervalTier"); END

FORM_WRITE (IntervalTier_writeToXwaves, L"Xwaves label file", 0, 0)
	if (! IntervalTier_writeToXwaves (ONLY_OBJECT, file)) return 0;
END

/***** INTERVALTIER & POINTPROCESS *****/

FORM (IntervalTier_PointProcess_endToCentre, L"From end to centre", L"IntervalTier & PointProcess: End to centre...")
	REAL (L"Phase (0-1)", L"0.5")
	OK
DO
	IntervalTier tier = ONLY (classIntervalTier);
	PointProcess point = ONLY (classPointProcess);
	double phase = GET_REAL (L"Phase");
	if (! praat_new5 (IntervalTier_PointProcess_endToCentre (tier, point, phase),
		tier -> name, L"_", point -> name, L"_", Melder_integer ((long) (100 * phase)))) return 0;
END

FORM (IntervalTier_PointProcess_startToCentre, L"From start to centre", L"IntervalTier & PointProcess: Start to centre...")
	REAL (L"Phase (0-1)", L"0.5")
	OK
DO
	IntervalTier tier = ONLY (classIntervalTier);
	PointProcess point = ONLY (classPointProcess);
	double phase = GET_REAL (L"Phase");
	if (! praat_new5 (IntervalTier_PointProcess_startToCentre (tier, point, phase),
		tier -> name, L"_", point -> name, L"_", Melder_integer ((long) (100 * phase)))) return 0;
END

/***** LABEL (obsolete) *****/

DIRECT (Label_Sound_to_TextGrid)
	Label label = ONLY (classLabel);
	Sound sound = ONLY (classSound);
	if (! praat_new1 (Label_Function_to_TextGrid (label, sound), sound -> name)) return 0;
END

DIRECT (info_Label_Sound_to_TextGrid)
	Melder_information1 (L"This is an old-style Label object. To turn it into a TextGrid, L"
		"select it together with a Sound of the appropriate duration, and click \"To TextGrid\".");
END

/***** PITCH & TEXTGRID *****/

static int pr_TextGrid_Pitch_draw (Any dia, int speckle, int unit) {
	double tmin, tmax, fmin, fmax;
	praat_get_timeRange (dia, & tmin, & tmax);
	if (! praat_get_frequencyRange (dia, & fmin, & fmax)) return 0;
	praat_picture_open ();
	TextGrid_Pitch_draw (ONLY (classTextGrid), ONLY (classPitch), GRAPHICS,
		GET_INTEGER (STRING_TIER_NUMBER), tmin, tmax, fmin, fmax, GET_INTEGER (L"Font size"),
		GET_INTEGER (L"Use text styles"), GET_INTEGER (L"Text alignment") - 1, GET_INTEGER (L"Garnish"), speckle, unit);
	praat_picture_close ();
	return 1;
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
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_HERTZ)) return 0;
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
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_ERB)) return 0;
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
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_HERTZ_LOGARITHMIC)) return 0;
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
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_MEL)) return 0;
END

FORM (TextGrid_Pitch_drawSemitones, L"TextGrid & Pitch: Draw semitones", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	LABEL (L"", L"Range in semitones re 100 Hertz:")
	REAL (L"left Frequency range (st)", L"-12.0")
	REAL (L"right Frequency range (st)", L"30.0")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, kPitch_unit_SEMITONES_100)) return 0;
END

static int pr_TextGrid_Pitch_drawSeparately (Any dia, int speckle, int unit) {
	double tmin, tmax, fmin, fmax;
	praat_get_timeRange (dia, & tmin, & tmax);
	if (! praat_get_frequencyRange (dia, & fmin, & fmax)) return 0;
	praat_picture_open ();
	TextGrid_Pitch_drawSeparately (ONLY (classTextGrid), ONLY (classPitch), GRAPHICS,
		tmin, tmax, fmin, fmax, GET_INTEGER (L"Show boundaries"),
		GET_INTEGER (L"Use text styles"), GET_INTEGER (L"Garnish"), speckle, unit);
	praat_picture_close ();
	return 1;
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
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_HERTZ)) return 0;
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
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_ERB)) return 0;
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
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_HERTZ_LOGARITHMIC)) return 0;
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
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_MEL)) return 0;
END

FORM (TextGrid_Pitch_drawSeparatelySemitones, L"TextGrid & Pitch: Draw separately semitones", 0)
	praat_dia_timeRange (dia);
	LABEL (L"", L"Range in semitones re 100 Hertz:")
	REAL (L"left Frequency range (st)", L"-12.0")
	REAL (L"right Frequency range (st)", L"30.0")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, kPitch_unit_SEMITONES_100)) return 0;
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
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_HERTZ)) return 0;
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
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_ERB)) return 0;
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
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_HERTZ_LOGARITHMIC)) return 0;
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
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_MEL)) return 0;
END

FORM (TextGrid_Pitch_speckleSemitones, L"TextGrid & Pitch: Speckle semitones", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	praat_dia_timeRange (dia);
	LABEL (L"", L"Range in semitones re 100 Hertz:")
	REAL (L"left Frequency range (st)", L"-12.0")
	REAL (L"right Frequency range (st)", L"30.0")
	INTEGER (L"Font size (points)", L"18")
	BOOLEAN (L"Use text styles", 1)
	OPTIONMENU (L"Text alignment", 2) OPTION (L"Left") OPTION (L"Centre") OPTION (L"Right")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, kPitch_unit_SEMITONES_100)) return 0;
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
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_HERTZ)) return 0;
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
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_ERB)) return 0;
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
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_HERTZ_LOGARITHMIC)) return 0;
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
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_MEL)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparatelySemitones, L"TextGrid & Pitch: Speckle separately semitones", 0)
	praat_dia_timeRange (dia);
	LABEL (L"", L"Range in semitones re 100 Hertz:")
	REAL (L"left Frequency range (st)", L"-12.0")
	REAL (L"right Frequency range (st)", L"30.0")
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, kPitch_unit_SEMITONES_100)) return 0;
END

/***** PITCH & TEXTTIER *****/

FORM (Pitch_TextTier_to_PitchTier, L"Pitch & TextTier to PitchTier", L"Pitch & TextTier: To PitchTier...")
	RADIO (L"Unvoiced strategy", 3)
		RADIOBUTTON (L"Zero")
		RADIOBUTTON (L"Error")
		RADIOBUTTON (L"Interpolate")
	OK
DO
	if (! praat_new1 (Pitch_AnyTier_to_PitchTier (ONLY (classPitch), ONLY (classTextTier),
		GET_INTEGER (L"Unvoiced strategy") - 1), ((Pitch) (ONLY (classPitch))) -> name)) return 0;
END

/***** SOUND & TEXTGRID *****/

FORM (TextGrid_Sound_draw, L"TextGrid & Sound: Draw...", 0)
	praat_dia_timeRange (dia);
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	praat_picture_open ();
	TextGrid_Sound_draw (ONLY (classTextGrid), ONLY (classSound), GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Show boundaries"),
		GET_INTEGER (L"Use text styles"), GET_INTEGER (L"Garnish"));
	praat_picture_close ();
END

FORM (TextGrid_Sound_extractAllIntervals, L"TextGrid & Sound: Extract all intervals", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	BOOLEAN (L"Preserve times", 0)
	OK
DO
	if (! praat_new1 (TextGrid_Sound_extractAllIntervals (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (L"Preserve times")), L"dummy")) return 0;
END

FORM (TextGrid_Sound_extractNonemptyIntervals, L"TextGrid & Sound: Extract non-empty intervals", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	BOOLEAN (L"Preserve times", 0)
	OK
DO
	if (! praat_new1 (TextGrid_Sound_extractNonemptyIntervals (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (L"Preserve times")), L"dummy")) return 0;
END

FORM (TextGrid_Sound_extractIntervals, L"TextGrid & Sound: Extract intervals", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	BOOLEAN (L"Preserve times", 0)
	SENTENCE (L"Label text", L"")
	OK
DO
	if (! praat_new1 (TextGrid_Sound_extractIntervalsWhere (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER (STRING_TIER_NUMBER), kMelder_string_EQUAL_TO, GET_STRING (L"Label text"),
		GET_INTEGER (L"Preserve times")), GET_STRING (L"Label text"))) return 0;
END

FORM (TextGrid_Sound_extractIntervalsWhere, L"TextGrid & Sound: Extract intervals", 0)
	INTEGER (STRING_TIER_NUMBER, L"1")
	BOOLEAN (L"Preserve times", 0)
	OPTIONMENU_ENUM (L"Extract all intervals whose label...", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", L"")
	OK
DO
	if (! praat_new1 (TextGrid_Sound_extractIntervalsWhere (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER (STRING_TIER_NUMBER),
		GET_ENUM (kMelder_string, L"Extract all intervals whose label..."),
		GET_STRING (L"...the text"),
		GET_INTEGER (L"Preserve times")), GET_STRING (L"...the text"))) return 0;
END

DIRECT (TextGrid_Sound_scaleTimes)
	TextGrid grid = ONLY (classTextGrid);
	Sound sound = ONLY (classSound);
	Function_scaleXTo (grid, sound -> xmin, sound -> xmax);
	praat_dataChanged (grid);
END

DIRECT (TextGrid_Sound_cloneTimeDomain)
	TextGrid grid = ONLY (classTextGrid);
	Sound sound = ONLY (classSound);
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
	WHERE (SELECTED) {
		if (! SpellingChecker_addNewWord (OBJECT, GET_STRING (L"New word"))) return 0;
		praat_dataChanged (OBJECT);
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
SpellingChecker me = ONLY_OBJECT;
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
DO
	SpellingChecker me = ONLY_OBJECT;
	Melder_free (my forbiddenStrings); my forbiddenStrings = Melder_wcsdup (GET_STRING (L"Forbidden strings"));
	my checkMatchingParentheses = GET_INTEGER (L"Check matching parentheses");
	Melder_free (my separatingCharacters); my separatingCharacters = Melder_wcsdup (GET_STRING (L"Separating characters"));
	my allowAllParenthesized = GET_INTEGER (L"Allow all parenthesized");
	my allowAllNames = GET_INTEGER (L"Allow all names");
	Melder_free (my namePrefixes); my namePrefixes = Melder_wcsdup (GET_STRING (L"Name prefixes"));
	my allowAllAbbreviations = GET_INTEGER (L"Allow all abbreviations");
	my allowCapsSentenceInitially = GET_INTEGER (L"Allow caps sentence-initially");
	my allowCapsAfterColon = GET_INTEGER (L"Allow caps after colon");
	Melder_free (my allowAllWordsContaining); my allowAllWordsContaining = Melder_wcsdup (GET_STRING (L"Allow all words containing"));
	Melder_free (my allowAllWordsStartingWith); my allowAllWordsStartingWith = Melder_wcsdup (GET_STRING (L"Allow all words starting with"));
	Melder_free (my allowAllWordsEndingIn); my allowAllWordsEndingIn = Melder_wcsdup (GET_STRING (L"Allow all words ending in"));
	praat_dataChanged (me);
END

DIRECT (SpellingChecker_extractWordList)
	EVERY_TO (SpellingChecker_extractWordList (OBJECT))
END

DIRECT (SpellingChecker_extractUserDictionary)
	EVERY_TO (SpellingChecker_extractUserDictionary (OBJECT))
END

FORM (SpellingChecker_isWordAllowed, L"Is word allowed?", L"SpellingChecker")
	SENTENCE (L"Word", L"")
	OK
DO
	Melder_information1 (SpellingChecker_isWordAllowed (ONLY_OBJECT, GET_STRING (L"Word")) ?
		L"1 (allowed)" : L"0 (not allowed)");
END

FORM (SpellingChecker_nextNotAllowedWord, L"Next not allowed word?", L"SpellingChecker")
	LABEL (L"", L"Sentence:")
	TEXTFIELD (L"sentence", L"")
	INTEGER (L"Starting character", L"0")
	OK
DO
	wchar_t *sentence = GET_STRING (L"sentence");
	long startingCharacter = GET_INTEGER (L"Starting character");
	REQUIRE (startingCharacter >= 0, L"Starting character should be 0 or positive.")
	REQUIRE (startingCharacter <= (int) wcslen (sentence), L"Starting character should not exceed end of sentence.")
	Melder_information1 (SpellingChecker_nextNotAllowedWord (ONLY_OBJECT, sentence, & startingCharacter));
END

DIRECT (SpellingChecker_replaceWordList)
	if (! SpellingChecker_replaceWordList (ONLY (classSpellingChecker), ONLY (classWordList))) return 0;
END

DIRECT (SpellingChecker_replaceWordList_help)
	Melder_information1 (L"To replace the checker's word list\nby the contents of a Strings object:\n"
		"1. select the Strings;\n2. convert to a WordList object;\n3. select the SpellingChecker and the WordList;\n"
		"4. choose Replace.");
END

DIRECT (SpellingChecker_replaceUserDictionary)
	if (! SpellingChecker_replaceUserDictionary (ONLY (classSpellingChecker), ONLY (classSortedSetOfString))) return 0;
END

/***** TEXTGRID *****/

FORM (TextGrid_countLabels, L"Count labels", L"TextGrid: Count labels...")
	INTEGER (STRING_TIER_NUMBER, L"1")
	SENTENCE (L"Label text", L"a")
	OK
DO
	Melder_information2 (Melder_integer (TextGrid_countLabels (ONLY (classTextGrid),
		GET_INTEGER (STRING_TIER_NUMBER), GET_STRING (L"Label text"))), L" labels");
END

FORM (TextGrid_draw, L"TextGrid: Draw", 0)
	praat_dia_timeRange (dia);
	BOOLEAN (L"Show boundaries", 1)
	BOOLEAN (L"Use text styles", 1)
	BOOLEAN (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (TextGrid_Sound_draw (OBJECT, NULL, GRAPHICS,
		GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Show boundaries"),
		GET_INTEGER (L"Use text styles"), GET_INTEGER (L"Garnish")))
END

FORM (TextGrid_duplicateTier, L"TextGrid: Duplicate tier", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (L"Position", L"1 (= at top)")
	WORD (L"Name", L"")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int itier = GET_INTEGER (STRING_TIER_NUMBER);
		int position = GET_INTEGER (L"Position");
		const wchar_t *name = GET_STRING (L"Name");
		AnyTier newTier;
		if (itier > grid -> tiers -> size) itier = grid -> tiers -> size;
		newTier = Data_copy (grid -> tiers -> item [itier]);
		if (! newTier) return 0;
		Thing_setName (newTier, name);
		if (! Ordered_addItemPos (grid -> tiers, newTier, position)) return 0;
		praat_dataChanged (OBJECT);
	}
END

static void cb_TextGridEditor_publish (Any editor, void *closure, Any publish) {
	(void) editor;
	(void) closure;
	if (! praat_new1 (publish, NULL)) { Melder_flushError (NULL); return; }
	praat_updateSelection ();
	if (Thing_member (publish, classSpectrum) && wcsequ (Thing_getName (publish), L"slice")) {
		int IOBJECT;
		WHERE (SELECTED) {
			SpectrumEditor editor2 = SpectrumEditor_create (theCurrentPraat -> topShell, ID_AND_FULL_NAME, OBJECT);
			if (! editor2) return;
			if (! praat_installEditor (editor2, IOBJECT)) Melder_flushError (NULL);
		}
	}
}
DIRECT (TextGrid_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error1 (L"Cannot edit a TextGrid from batch.");
	} else {
		WHERE (SELECTED && CLASS == classTextGrid) {
			TextGridEditor editor = TextGridEditor_create (theCurrentPraat -> topShell, ID_AND_FULL_NAME,
				OBJECT, ONLY (classSound), NULL);
			if (! praat_installEditor (editor, IOBJECT)) return 0;
			Editor_setPublishCallback (editor, cb_TextGridEditor_publish, NULL);
		}
	}
END

DIRECT (TextGrid_LongSound_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error1 (L"Cannot edit a TextGrid from batch.");
	} else {
		LongSound longSound = NULL;
		int ilongSound = 0;
		WHERE (SELECTED)
			if (CLASS == classLongSound) longSound = OBJECT, ilongSound = IOBJECT;
		Melder_assert (ilongSound != 0);
		WHERE (SELECTED && CLASS == classTextGrid)
			if (! praat_installEditor2 (TextGridEditor_create (theCurrentPraat -> topShell, ID_AND_FULL_NAME,
				OBJECT, longSound, NULL), IOBJECT, ilongSound)) return 0;
	}
END

DIRECT (TextGrid_SpellingChecker_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error1 (L"Cannot edit a TextGrid from batch.");
	} else {
		SpellingChecker spellingChecker = NULL;
		int ispellingChecker = 0;
		WHERE (SELECTED)
			if (CLASS == classSpellingChecker) spellingChecker = OBJECT, ispellingChecker = IOBJECT;
		Melder_assert (ispellingChecker != 0);
		WHERE (SELECTED && CLASS == classTextGrid)
			if (! praat_installEditor2 (TextGridEditor_create (theCurrentPraat -> topShell, ID_AND_FULL_NAME,
				OBJECT, ONLY (classSound), spellingChecker), IOBJECT, ispellingChecker)) return 0;
	}
END

DIRECT (TextGrid_LongSound_SpellingChecker_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error1 (L"Cannot edit a TextGrid from batch.");
	} else {
		LongSound longSound = NULL;
		SpellingChecker spellingChecker = NULL;
		int ilongSound = 0, ispellingChecker = 0;
		WHERE (SELECTED) {
			if (CLASS == classLongSound) longSound = OBJECT, ilongSound = IOBJECT;
			if (CLASS == classSpellingChecker) spellingChecker = OBJECT, ispellingChecker = IOBJECT;
		}
		Melder_assert (ilongSound != 0 && ispellingChecker != 0);
		WHERE (SELECTED && CLASS == classTextGrid)
			if (! praat_installEditor3 (TextGridEditor_create (theCurrentPraat -> topShell, ID_AND_FULL_NAME,
				OBJECT, longSound, spellingChecker), IOBJECT, ilongSound, ispellingChecker)) return 0;
	}
END

FORM (TextGrid_extractPart, L"TextGrid: Extract part", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"1.0")
	BOOLEAN (L"Preserve times", 0)
	OK
DO
	TextGrid grid = ONLY (classTextGrid);
	if (! praat_new2 (TextGrid_extractPart (grid, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Preserve times")), grid -> name, L"_part")) return 0;
END

static Data pr_TextGrid_getTier (Any dia) {
	TextGrid grid = ONLY_OBJECT;
	long tierNumber = GET_INTEGER (STRING_TIER_NUMBER);
	if (tierNumber > grid -> tiers -> size) return Melder_errorp
		("Tier number (%ld) should not be larger than number of tiers (%ld).", tierNumber, grid -> tiers -> size);
	return grid -> tiers -> item [tierNumber];
}

static IntervalTier pr_TextGrid_getIntervalTier (Any dia) {
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return NULL;
	if (tier -> methods != (Data_Table) classIntervalTier) return Melder_errorp1 (L"Tier should be interval tier.");
	return (IntervalTier) tier;
}

static TextTier pr_TextGrid_getTextTier (Any dia) {
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return NULL;
	if (tier -> methods != (Data_Table) classTextTier) return Melder_errorp1 (L"Tier should be point tier (TextTier).");
	return (TextTier) tier;
}

static TextInterval pr_TextGrid_getInterval (Any dia) {
	int intervalNumber = GET_INTEGER (STRING_INTERVAL_NUMBER);
	IntervalTier intervalTier = pr_TextGrid_getIntervalTier (dia);
	if (! intervalTier) return NULL;
	if (intervalNumber > intervalTier -> intervals -> size) return Melder_errorp1 (L"Interval number too large.");
	return intervalTier -> intervals -> item [intervalNumber];
}

static TextPoint pr_TextGrid_getPoint (Any dia) {	
	long pointNumber = GET_INTEGER (STRING_POINT_NUMBER);
	TextTier textTier = pr_TextGrid_getTextTier (dia);
	if (! textTier) return NULL;
	if (pointNumber > textTier -> points -> size) return Melder_errorp1 (L"Point number too large.");
	return textTier -> points -> item [pointNumber];
}

FORM (TextGrid_extractTier, L"TextGrid: Extract tier", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return 0;
	if (! praat_new1 (Data_copy (tier), tier -> name)) return 0;
END

DIRECT (TextGrid_genericize)
	WHERE (SELECTED) {
		if (! TextGrid_genericize (OBJECT)) return 0;
		praat_dataChanged (OBJECT);
	}
END

DIRECT (TextGrid_nativize)
	WHERE (SELECTED) {
		if (! TextGrid_nativize (OBJECT)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_getHighIndexFromTime, L"Get high index", L"AnyTier: Get high index from time...")
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	TextTier textTier = pr_TextGrid_getTextTier (dia);
	if (! textTier) return 0;
	Melder_information1 (Melder_integer (AnyTier_timeToHighIndex (textTier, GET_REAL (L"Time"))));
END

FORM (TextGrid_getLowIndexFromTime, L"Get low index", L"AnyTier: Get low index from time...")
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	TextTier textTier = pr_TextGrid_getTextTier (dia);
	if (! textTier) return 0;
	Melder_information1 (Melder_integer (AnyTier_timeToLowIndex (textTier, GET_REAL (L"Time"))));
END

FORM (TextGrid_getNearestIndexFromTime, L"Get nearest index", L"AnyTier: Get nearest index from time...")
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	TextTier textTier = pr_TextGrid_getTextTier (dia);
	if (! textTier) return 0;
	Melder_information1 (Melder_integer (AnyTier_timeToNearestIndex (textTier, GET_REAL (L"Time"))));
END

FORM (TextGrid_getIntervalAtTime, L"TextGrid: Get interval at time", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	IntervalTier intervalTier = pr_TextGrid_getIntervalTier (dia);
	if (! intervalTier) return 0;
	Melder_information1 (Melder_integer (IntervalTier_timeToIndex (intervalTier, GET_REAL (L"Time"))));
END

FORM (TextGrid_getNumberOfIntervals, L"TextGrid: Get number of intervals", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	IntervalTier intervalTier = pr_TextGrid_getIntervalTier (dia);
	if (! intervalTier) return 0;
	Melder_information1 (Melder_integer (intervalTier -> intervals -> size));
END

DIRECT (TextGrid_getNumberOfTiers)
	TextGrid grid = ONLY_OBJECT;
	Melder_information1 (Melder_integer (grid -> tiers -> size));
END

FORM (TextGrid_getStartingPoint, L"TextGrid: Get starting point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"1")
	OK
DO
	TextInterval interval = pr_TextGrid_getInterval (dia);
	if (! interval) return 0;
	Melder_informationReal (interval -> xmin, L"seconds");
END
	
FORM (TextGrid_getEndPoint, L"TextGrid: Get end point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"1")
	OK
DO
	TextInterval interval = pr_TextGrid_getInterval (dia);
	if (! interval) return 0;
	Melder_informationReal (interval -> xmax, L"seconds");
END
	
FORM (TextGrid_getLabelOfInterval, L"TextGrid: Get label of interval", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"1")
	OK
DO
	TextInterval interval = pr_TextGrid_getInterval (dia);
	if (! interval) return 0;
	MelderInfo_open ();
	MelderInfo_write1 (interval -> text);
	MelderInfo_close ();
END
	
FORM (TextGrid_getNumberOfPoints, L"TextGrid: Get number of points", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	TextTier textTier = pr_TextGrid_getTextTier (dia);
	if (! textTier) return 0;
	Melder_information1 (Melder_integer (textTier -> points -> size));
END
	
FORM (TextGrid_getTierName, L"TextGrid: Get tier name", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return 0;
	Melder_information1 (tier -> name);
END

FORM (TextGrid_getTimeOfPoint, L"TextGrid: Get time of point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_POINT_NUMBER, L"1")
	OK
DO
	TextPoint point = pr_TextGrid_getPoint (dia);
	if (! point) return 0;
	Melder_informationReal (point -> time, L"seconds");
END
	
FORM (TextGrid_getLabelOfPoint, L"TextGrid: Get label of point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_POINT_NUMBER, L"1")
	OK
DO
	TextPoint point = pr_TextGrid_getPoint (dia);
	if (! point) return 0;
	Melder_information1 (point -> mark);
END
	
DIRECT (TextGrid_help) Melder_help (L"TextGrid"); END

FORM (TextGrid_insertBoundary, L"TextGrid: Insert boundary", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_insertBoundary (OBJECT, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL (L"Time"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_insertIntervalTier, L"TextGrid: Insert interval tier", 0)
	NATURAL (L"Position", L"1 (= at top)")
	WORD (L"Name", L"")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int position = GET_INTEGER (L"Position");
		wchar_t *name = GET_STRING (L"Name");
		IntervalTier tier = IntervalTier_create (grid -> xmin, grid -> xmax);
		if (! tier) return 0;
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (tier, name);
		if (! Ordered_addItemPos (grid -> tiers, tier, position)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_insertPoint, L"TextGrid: Insert point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	LABEL (L"", L"Text:")
	TEXTFIELD (L"text", L"")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_insertPoint (OBJECT, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL (L"Time"), GET_STRING (L"text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_insertPointTier, L"TextGrid: Insert point tier", 0)
	NATURAL (L"Position", L"1 (= at top)")
	WORD (L"Name", L"")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int position = GET_INTEGER (L"Position");
		wchar_t *name = GET_STRING (L"Name");
		TextTier tier = TextTier_create (grid -> xmin, grid -> xmax);
		if (! tier) return 0;
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (tier, name);
		if (! Ordered_addItemPos (grid -> tiers, tier, position)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_isIntervalTier, L"TextGrid: Is interval tier?", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return 0;
	if (tier -> methods == (Data_Table) classIntervalTier) {
		Melder_information3 (L"1 (yes, tier ", Melder_integer (GET_INTEGER (STRING_TIER_NUMBER)), L" is an interval tier)");
	} else {
		Melder_information3 (L"0 (no, tier ", Melder_integer (GET_INTEGER (STRING_TIER_NUMBER)), L" is a point tier)");
	}
END

DIRECT (TextGrids_merge)
	Collection textGrids;
	int n = 0;
	WHERE (SELECTED) n ++;
	textGrids = Collection_create (classTextGrid, n);
	WHERE (SELECTED) Collection_addItem (textGrids, OBJECT);
	if (! praat_new1 (TextGrid_merge (textGrids), L"merged")) {
		textGrids -> size = 0;   /* Undangle. */
		forget (textGrids);
		return 0;
	}
	textGrids -> size = 0;   /* Undangle. */
	forget (textGrids);
END

DIRECT (info_TextGrid_Pitch_draw)
	Melder_information1 (L"You can draw a TextGrid together with a Pitch after selecting them both.");
END

FORM (TextGrid_removeBoundaryAtTime, L"TextGrid: Remove boundary at time", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_removeBoundaryAtTime (OBJECT, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL (L"Time"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_removeLeftBoundary, L"TextGrid: Remove left boundary", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"2")
	OK
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long iinterval = GET_INTEGER (STRING_INTERVAL_NUMBER);
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		IntervalTier intervalTier;
		if (itier > grid -> tiers -> size)
			return Melder_error7 (L"You cannot remove a boundary from tier ", Melder_integer (itier), L" of ", Thing_messageName (grid),
				L", because that TextGrid has only ", Melder_integer (grid -> tiers -> size), L" tiers.");
		intervalTier = grid -> tiers -> item [itier];
		if (intervalTier -> methods != classIntervalTier)
			return Melder_error5 (L"You cannot remove a boundary from tier ", Melder_integer (itier), L" of ", Thing_messageName (grid),
				L", because that tier is a point tier instead of an interval tier.");
		if (iinterval > intervalTier -> intervals -> size)
			return Melder_error9 (L"You cannot remove a boundary from interval ", Melder_integer (iinterval), L" of tier ", Melder_integer (itier), L" of ",
				Thing_messageName (grid), L", because that tier has only ", Melder_integer (intervalTier -> intervals -> size), L" intervals.");
		if (iinterval == 1)
			return Melder_error5 (L"You cannot remove the left boundary from interval 1 of tier ", Melder_integer (itier), L" of ", Thing_messageName (grid),
				L", because this is at the left edge of the tier.");
		if (! IntervalTier_removeLeftBoundary (intervalTier, iinterval)) return 0;
		praat_dataChanged (grid);
	}
END

FORM (TextGrid_removePoint, L"TextGrid: Remove point", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_POINT_NUMBER, L"2")
	OK
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long ipoint = GET_INTEGER (STRING_POINT_NUMBER);
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		TextTier pointTier;
		if (itier > grid -> tiers -> size)
			return Melder_error7 (L"You cannot remove a point from tier ", Melder_integer (itier), L" of ", Thing_messageName (grid),
				L", because that TextGrid has only ", Melder_integer (grid -> tiers -> size), L" tiers.");
		pointTier = grid -> tiers -> item [itier];
		if (pointTier -> methods != classTextTier)
			return Melder_error5 (L"You cannot remove a point from tier ", Melder_integer (itier), L" of ", Thing_messageName (grid),
				L", because that tier is an interval tier instead of a point tier.");
		if (ipoint > pointTier -> points -> size)
			return Melder_error9 (L"You cannot remove point ", Melder_integer (ipoint), L" from tier ", Melder_integer (itier), L" of ", Thing_messageName (grid),
				L", because that tier has only ", Melder_integer (pointTier -> points -> size), L" points.");
		TextTier_removePoint (pointTier, ipoint);
		praat_dataChanged (grid);
	}
END

FORM (TextGrid_removeRightBoundary, L"TextGrid: Remove right boundary", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"1")
	OK
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long iinterval = GET_INTEGER (STRING_INTERVAL_NUMBER);
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		IntervalTier intervalTier;
		if (itier > grid -> tiers -> size)
			return Melder_error7 (L"You cannot remove a boundary from tier ", Melder_integer (itier), L" of ", Thing_messageName (grid),
				L", because that TextGrid has only ", Melder_integer (grid -> tiers -> size), L" tiers.");
		intervalTier = grid -> tiers -> item [itier];
		if (intervalTier -> methods != classIntervalTier)
			return Melder_error5 (L"You cannot remove a boundary from tier ", Melder_integer (itier), L" of ", Thing_messageName (grid),
				L", because that tier is a point tier instead of an interval tier.");
		if (iinterval > intervalTier -> intervals -> size)
			return Melder_error9 (L"You cannot remove a boundary from interval ", Melder_integer (iinterval), L" of tier ", Melder_integer (itier), L" of ", Thing_messageName (grid),
				L", because that tier has only ", Melder_integer (intervalTier -> intervals -> size), L" intervals.");
		if (iinterval == intervalTier -> intervals -> size)
			return Melder_error7 (L"You cannot remove the right boundary from interval ", Melder_integer (iinterval), L" of tier ", Melder_integer (itier), L" of ", Thing_messageName (grid),
				L", because this is at the right edge of the tier.");
		if (! IntervalTier_removeLeftBoundary (intervalTier, iinterval + 1)) return 0;
		praat_dataChanged (grid);
	}
END

FORM (TextGrid_removeTier, L"TextGrid: Remove tier", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int itier = GET_INTEGER (STRING_TIER_NUMBER);
		if (grid -> tiers -> size <= 1) {
			return Melder_error1 (L"Sorry, I refuse to remove the last tier.");
		}
		if (itier > grid -> tiers -> size) itier = grid -> tiers -> size;
		Collection_removeItem (grid -> tiers, itier);
		praat_dataChanged (grid);
	}
END

DIRECT (info_TextGrid_Sound_edit)
	Melder_information1 (L"To include a copy of a Sound in your TextGrid editor:\n"
		"   select a TextGrid and a Sound, and click \"Edit\".");
END

DIRECT (info_TextGrid_Sound_draw)
	Melder_information1 (L"You can draw a TextGrid together with a Sound after selecting them both.");
END

FORM (TextGrid_setIntervalText, L"TextGrid: Set interval text", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_INTERVAL_NUMBER, L"1")
	LABEL (L"", L"Text:")
	TEXTFIELD (L"text", L"")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_setIntervalText (OBJECT, GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (STRING_INTERVAL_NUMBER), GET_STRING (L"text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_setPointText, L"TextGrid: Set point text", 0)
	NATURAL (STRING_TIER_NUMBER, L"1")
	NATURAL (STRING_POINT_NUMBER, L"1")
	LABEL (L"", L"Text:")
	TEXTFIELD (L"text", L"")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_setPointText (OBJECT, GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (STRING_POINT_NUMBER), GET_STRING (L"text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM_WRITE (TextGrid_writeToChronologicalTextFile, L"Text file", 0, 0)
	if (! TextGrid_writeToChronologicalTextFile (ONLY_OBJECT, file)) return 0;
END

/***** TEXTGRID & ANYTIER *****/

DIRECT (TextGrid_AnyTier_append)
	TextGrid oldGrid = ONLY (classTextGrid), newGrid = Data_copy (oldGrid);
	if (! newGrid) return 0;
	WHERE (SELECTED && OBJECT != oldGrid) if (! TextGrid_add (newGrid, OBJECT)) { forget (newGrid); return 0; }
	if (! praat_new1 (newGrid, oldGrid -> name)) return 0;
END

/***** TEXTGRID & LONGSOUND *****/

DIRECT (TextGrid_LongSound_scaleTimes)
	TextGrid grid = ONLY (classTextGrid);
	LongSound longSound = ONLY (classLongSound);
	Function_scaleXTo (grid, longSound -> xmin, longSound -> xmax);
	praat_dataChanged (grid);
END

/***** TEXTTIER *****/

FORM (TextTier_addPoint, L"TextTier: Add point", L"TextTier: Add point...")
	REAL (L"Time (s)", L"0.5")
	SENTENCE (L"Text", L"")
	OK
DO
	WHERE (SELECTED) {
		if (! TextTier_addPoint (OBJECT, GET_REAL (L"Time"), GET_STRING (L"Text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

DIRECT (TextTier_downto_PointProcess)
	EVERY_TO (AnyTier_downto_PointProcess (OBJECT))
END

FORM (TextTier_downto_TableOfReal, L"TextTier: Down to TableOfReal", 0)
	SENTENCE (L"Label", L"")
	OK
DO
	EVERY_TO (TextTier_downto_TableOfReal (OBJECT, GET_STRING (L"Label")))
END

DIRECT (TextTier_downto_TableOfReal_any)
	EVERY_TO (TextTier_downto_TableOfReal_any (OBJECT))
END

FORM (TextTier_getLabelOfPoint, L"Get label of point", 0)
	NATURAL (L"Point number", L"1")
	OK
DO
	TextTier me = ONLY_OBJECT;
	long ipoint = GET_INTEGER (L"Point number");
	REQUIRE (ipoint <= my points -> size, L"No such point.")
	TextPoint point = my points -> item [ipoint];
	Melder_information1 (point -> mark);
END

FORM (TextTier_getPoints, L"Get points", 0)
	SENTENCE (L"Text", L"")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new1 (TextTier_getPoints (OBJECT, GET_STRING (L"Text")), GET_STRING (L"Text"))) return 0;
END

DIRECT (TextTier_help) Melder_help (L"TextTier"); END

/***** WORDLIST *****/

FORM (WordList_hasWord, L"Does word occur in list?", L"WordList")
	SENTENCE (L"Word", L"")
	OK
DO
	Melder_information1 (WordList_hasWord (ONLY_OBJECT, Melder_peekWcsToUtf8 (GET_STRING (L"Word"))) ? L"1" : L"0");
END

DIRECT (WordList_to_Strings)
	EVERY_TO (WordList_to_Strings (OBJECT))
END

DIRECT (WordList_upto_SpellingChecker)
	EVERY_TO (WordList_upto_SpellingChecker (OBJECT))
END

/***** buttons *****/

void praat_TimeFunction_query_init (void *klas);
void praat_TimeTier_query_init (void *klas);
void praat_TimeTier_modify_init (void *klas);

void praat_uvafon_TextGrid_init (void);
void praat_uvafon_TextGrid_init (void) {
	Thing_recognizeClassByOtherName (classTextTier, L"MarkTier");

	TextGridEditor_prefs ();

	praat_addAction1 (classIntervalTier, 0, L"IntervalTier help", 0, 0, DO_IntervalTier_help);
	praat_addAction1 (classIntervalTier, 1, L"Write to Xwaves label file...", 0, 0, DO_IntervalTier_writeToXwaves);
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

	praat_addAction1 (classSpellingChecker, 1, L"Edit...", 0, 0, DO_SpellingChecker_edit);
	praat_addAction1 (classSpellingChecker, 0, L"Query", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 1, L"Is word allowed...", 0, 0, DO_SpellingChecker_isWordAllowed);
	praat_addAction1 (classSpellingChecker, 1, L"Next not allowed word...", 0, 0, DO_SpellingChecker_nextNotAllowedWord);
	praat_addAction1 (classSpellingChecker, 0, L"Modify", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 0, L"Add new word...", 0, 0, DO_SpellingChecker_addNewWord);
	praat_addAction1 (classSpellingChecker, 0, L"Analyze", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 0, L"Extract WordList", 0, 0, DO_SpellingChecker_extractWordList);
	praat_addAction1 (classSpellingChecker, 0, L"Extract user dictionary", 0, 0, DO_SpellingChecker_extractUserDictionary);

	praat_addAction1 (classTextGrid, 0, L"TextGrid help", 0, 0, DO_TextGrid_help);
	praat_addAction1 (classTextGrid, 1, L"Write to chronological text file...", 0, 0, DO_TextGrid_writeToChronologicalTextFile);
	praat_addAction1 (classTextGrid, 0, L"Edit", 0, 0, 0);
	praat_addAction1 (classTextGrid, 1, L"Edit", 0, 0, DO_TextGrid_edit);
	praat_addAction1 (classTextGrid, 1, L"& Sound: Edit?", 0, 0, DO_info_TextGrid_Sound_edit);
	praat_addAction1 (classTextGrid, 0, L"Draw", 0, 0, 0);
	praat_addAction1 (classTextGrid, 0, L"Draw...", 0, 0, DO_TextGrid_draw);
	praat_addAction1 (classTextGrid, 1, L"& Sound: Draw?", 0, 0, DO_info_TextGrid_Sound_draw);
	praat_addAction1 (classTextGrid, 1, L"& Pitch: Draw?", 0, 0, DO_info_TextGrid_Pitch_draw);
	praat_addAction1 (classTextGrid, 0, L"Query -          ", 0, 0, 0);
		praat_TimeFunction_query_init (classTextGrid);
		praat_addAction1 (classTextGrid, 1, L"-- query tiers --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, L"Get number of tiers", 0, 1, DO_TextGrid_getNumberOfTiers);
		praat_addAction1 (classTextGrid, 1, L"Get tier name...", 0, 1, DO_TextGrid_getTierName);
		praat_addAction1 (classTextGrid, 1, L"Is interval tier...", 0, 1, DO_TextGrid_isIntervalTier);
		praat_addAction1 (classTextGrid, 1, L"-- query intervals --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, L"Get number of intervals...", 0, 1, DO_TextGrid_getNumberOfIntervals);
		praat_addAction1 (classTextGrid, 1, L"Get start point...", 0, 1, DO_TextGrid_getStartingPoint);
		praat_addAction1 (classTextGrid, 1, L"Get starting point...", 0, praat_HIDDEN + praat_DEPTH_1, DO_TextGrid_getStartingPoint);   // hidden 2008
		praat_addAction1 (classTextGrid, 1, L"Get end point...", 0, 1, DO_TextGrid_getEndPoint);
		praat_addAction1 (classTextGrid, 1, L"Get label of interval...", 0, 1, DO_TextGrid_getLabelOfInterval);
		praat_addAction1 (classTextGrid, 1, L"Get interval at time...", 0, 1, DO_TextGrid_getIntervalAtTime);
		praat_addAction1 (classTextGrid, 1, L"-- query points --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, L"Get number of points...", 0, 1, DO_TextGrid_getNumberOfPoints);
		praat_addAction1 (classTextGrid, 1, L"Get time of point...", 0, 1, DO_TextGrid_getTimeOfPoint);
		praat_addAction1 (classTextGrid, 1, L"Get label of point...", 0, 1, DO_TextGrid_getLabelOfPoint);
		praat_addAction1 (classTextGrid, 1, L"Get low index from time...", 0, 1, DO_TextGrid_getLowIndexFromTime);
		praat_addAction1 (classTextGrid, 1, L"Get high index from time...", 0, 1, DO_TextGrid_getHighIndexFromTime);
		praat_addAction1 (classTextGrid, 1, L"Get nearest index from time...", 0, 1, DO_TextGrid_getNearestIndexFromTime);
		praat_addAction1 (classTextGrid, 1, L"-- query labels --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, L"Count labels...", 0, 1, DO_TextGrid_countLabels);
	praat_addAction1 (classTextGrid, 0, L"Modify -        ", 0, 0, 0);
		praat_addAction1 (classTextGrid, 0, L"Convert to backslash trigraphs", 0, 1, DO_TextGrid_genericize);
		praat_addAction1 (classTextGrid, 0, L"Genericize", 0, praat_HIDDEN + praat_DEPTH_1, DO_TextGrid_genericize);   // hidden 2007
		praat_addAction1 (classTextGrid, 0, L"Convert to Unicode", 0, 1, DO_TextGrid_nativize);
		praat_addAction1 (classTextGrid, 0, L"Nativize", 0, praat_HIDDEN + praat_DEPTH_1, DO_TextGrid_nativize);   // hidden 2007
		praat_TimeFunction_modify_init (classTextGrid);
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
		praat_addAction1 (classTextGrid, 0, L"-- modify tiers --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 0, L"Insert interval tier...", 0, 1, DO_TextGrid_insertIntervalTier);
		praat_addAction1 (classTextGrid, 0, L"Insert point tier...", 0, 1, DO_TextGrid_insertPointTier);
		praat_addAction1 (classTextGrid, 0, L"Duplicate tier...", 0, 1, DO_TextGrid_duplicateTier);
		praat_addAction1 (classTextGrid, 0, L"Remove tier...", 0, 1, DO_TextGrid_removeTier);
praat_addAction1 (classTextGrid, 0, L"Analyse", 0, 0, 0);
	praat_addAction1 (classTextGrid, 1, L"Extract tier...", 0, 0, DO_TextGrid_extractTier);
	praat_addAction1 (classTextGrid, 1, L"Extract part...", 0, 0, DO_TextGrid_extractPart);
praat_addAction1 (classTextGrid, 0, L"Synthesize", 0, 0, 0);
	praat_addAction1 (classTextGrid, 0, L"Merge", 0, 0, DO_TextGrids_merge);

	praat_addAction1 (classTextTier, 0, L"TextTier help", 0, 0, DO_TextTier_help);
	praat_addAction1 (classTextTier, 0, L"Query -          ", 0, 0, 0);
		praat_TimeTier_query_init (classTextTier);
		praat_addAction1 (classTextTier, 0, L"Get label of point...", 0, 1, DO_TextTier_getLabelOfPoint);
	praat_addAction1 (classTextTier, 0, L"Modify -          ", 0, 0, 0);
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
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, L"Edit", 0, 0, DO_TextGrid_LongSound_edit);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, L"Scale times", 0, 0, DO_TextGrid_LongSound_scaleTimes);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, L"Draw -           ", 0, 0, 0);
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
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Edit", 0, 0, DO_TextGrid_edit);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Draw...", 0, 0, DO_TextGrid_Sound_draw);
	praat_addAction2 (classSound, 1, classTextGrid, 1, L"Extract -       ", 0, 0, 0);
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
	praat_addAction2 (classSpellingChecker, 1, classTextGrid, 1, L"Edit", 0, 0, DO_TextGrid_SpellingChecker_edit);
	praat_addAction2 (classTextGrid, 1, classTextTier, 1, L"Append", 0, 0, DO_TextGrid_AnyTier_append);
	praat_addAction2 (classTextGrid, 1, classIntervalTier, 1, L"Append", 0, 0, DO_TextGrid_AnyTier_append);

	praat_addAction3 (classLongSound, 1, classSpellingChecker, 1, classTextGrid, 1, L"Edit", 0, 0, DO_TextGrid_LongSound_SpellingChecker_edit);
	praat_addAction3 (classSound, 1, classSpellingChecker, 1, classTextGrid, 1, L"Edit", 0, 0, DO_TextGrid_SpellingChecker_edit);
}

/* End of file praat_TextGrid_init.c */
