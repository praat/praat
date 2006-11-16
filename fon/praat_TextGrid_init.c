/* praat_TextGrid_init.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 * pb 2006/10/29
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

static const char *STRING_FROM_FREQUENCY_HZ = "left Frequency range (Hz)";
static const char *STRING_TO_FREQUENCY_HZ = "right Frequency range (Hz)";
static const char *STRING_TIER_NUMBER = "Tier number";
static const char *STRING_INTERVAL_NUMBER = "Interval number";
static const char *STRING_POINT_NUMBER = "Point number";

/***** ANYTIER (generic) *****/

DIRECT (AnyTier_into_TextGrid)
	TextGrid grid = TextGrid_createWithoutTiers (1e30, -1e30);
	if (grid == NULL) return 0;
	WHERE (SELECTED) if (! TextGrid_add (grid, OBJECT)) { forget (grid); return 0; }
	if (! praat_new (grid, "grid")) return 0;
END

/***** INTERVALTIER *****/

FORM (IntervalTier_downto_TableOfReal, "IntervalTier: Down to TableOfReal", 0)
	SENTENCE ("Label", "")
	OK
DO
	EVERY_TO (IntervalTier_downto_TableOfReal (OBJECT, GET_STRING ("Label")))
END

DIRECT (IntervalTier_downto_TableOfReal_any)
	EVERY_TO (IntervalTier_downto_TableOfReal_any (OBJECT))
END

FORM (IntervalTier_getCentrePoints, "IntervalTier: Get centre points", 0)
	SENTENCE ("Text", "")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new (IntervalTier_getCentrePoints (OBJECT, GET_STRING ("Text")), GET_STRING ("Text"))) return 0;
END

FORM (IntervalTier_getEndPoints, "IntervalTier: Get end points", 0)
	SENTENCE ("Text", "")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new (IntervalTier_getEndPoints (OBJECT, GET_STRING ("Text")), GET_STRING ("Text"))) return 0;
END

FORM (IntervalTier_getStartingPoints, "IntervalTier: Get starting points", 0)
	SENTENCE ("Text", "")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new (IntervalTier_getStartingPoints (OBJECT, GET_STRING ("Text")), GET_STRING ("Text"))) return 0;
END

DIRECT (IntervalTier_help) Melder_help ("IntervalTier"); END

FORM_WRITE (IntervalTier_writeToXwaves, "Xwaves label file", 0, 0)
	if (! IntervalTier_writeToXwaves (ONLY_OBJECT, file)) return 0;
END

/***** INTERVALTIER & POINTPROCESS *****/

FORM (IntervalTier_PointProcess_endToCentre, "From end to centre", "IntervalTier & PointProcess: End to centre...")
	REAL ("Phase (0-1)", "0.5")
	OK
DO
	IntervalTier tier = ONLY (classIntervalTier);
	PointProcess point = ONLY (classPointProcess);
	double phase = GET_REAL ("Phase");
	if (! praat_new (IntervalTier_PointProcess_endToCentre (tier, point, phase),
		"%s %s %ld", tier -> name, point -> name, (long) (100 * phase))) return 0;
END

FORM (IntervalTier_PointProcess_startToCentre, "From start to centre", "IntervalTier & PointProcess: Start to centre...")
	REAL ("Phase (0-1)", "0.5")
	OK
DO
	IntervalTier tier = ONLY (classIntervalTier);
	PointProcess point = ONLY (classPointProcess);
	double phase = GET_REAL ("Phase");
	if (! praat_new (IntervalTier_PointProcess_startToCentre (tier, point, phase),
		"%s %s %ld", tier -> name, point -> name, (long) (100 * phase))) return 0;
END

/***** LABEL (obsolete) *****/

DIRECT (Label_Sound_to_TextGrid)
	Label label = ONLY (classLabel);
	Sound sound = ONLY (classSound);
	if (! praat_new (Label_Function_to_TextGrid (label, sound), sound -> name)) return 0;
END

DIRECT (info_Label_Sound_to_TextGrid)
	Melder_information ("This is an old-style Label object. To turn it into a TextGrid, "
		"select it together with a Sound of the appropriate duration, and click \"To TextGrid\".");
END

/***** PITCH & TEXTGRID *****/

static int pr_TextGrid_Pitch_draw (Any dia, int speckle, int unit) {
	double tmin, tmax, fmin, fmax;
	praat_get_timeRange (dia, & tmin, & tmax);
	if (! praat_get_frequencyRange (dia, & fmin, & fmax)) return 0;
	praat_picture_open ();
	TextGrid_Pitch_draw (ONLY (classTextGrid), ONLY (classPitch), GRAPHICS,
		GET_INTEGER (STRING_TIER_NUMBER), tmin, tmax, fmin, fmax, GET_INTEGER ("Font size"),
		GET_INTEGER ("Use text styles"), GET_INTEGER ("Text alignment") - 1, GET_INTEGER ("Garnish"), speckle, unit);
	praat_picture_close ();
	return 1;
}

FORM (TextGrid_Pitch_draw, "TextGrid & Pitch: Draw", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, "0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	OPTIONMENU ("Text alignment", 2) OPTION ("Left") OPTION ("Centre") OPTION ("Right")
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, Pitch_UNIT_HERTZ)) return 0;
END

FORM (TextGrid_Pitch_drawErb, "TextGrid & Pitch: Draw erb", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	praat_dia_timeRange (dia);
	REAL ("left Frequency range (ERB)", "0")
	REAL ("right Frequency range (ERB)", "10.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	OPTIONMENU ("Text alignment", 2) OPTION ("Left") OPTION ("Centre") OPTION ("Right")
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, Pitch_UNIT_ERB)) return 0;
END

FORM (TextGrid_Pitch_drawLogarithmic, "TextGrid & Pitch: Draw logarithmic", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, "50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	OPTIONMENU ("Text alignment", 2) OPTION ("Left") OPTION ("Centre") OPTION ("Right")
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, Pitch_UNIT_HERTZ_LOGARITHMIC)) return 0;
END

FORM (TextGrid_Pitch_drawMel, "TextGrid & Pitch: Draw mel", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	praat_dia_timeRange (dia);
	REAL ("left Frequency range (mel)", "0")
	REAL ("right Frequency range (mel)", "500")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	OPTIONMENU ("Text alignment", 2) OPTION ("Left") OPTION ("Centre") OPTION ("Right")
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, Pitch_UNIT_MEL)) return 0;
END

FORM (TextGrid_Pitch_drawSemitones, "TextGrid & Pitch: Draw semitones", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	praat_dia_timeRange (dia);
	LABEL ("", "Range in semitones re 100 Hertz:")
	REAL ("left Frequency range (st)", "-12.0")
	REAL ("right Frequency range (st)", "30.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	OPTIONMENU ("Text alignment", 2) OPTION ("Left") OPTION ("Centre") OPTION ("Right")
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_NO, Pitch_UNIT_SEMITONES_100)) return 0;
END

static int pr_TextGrid_Pitch_drawSeparately (Any dia, int speckle, int unit) {
	double tmin, tmax, fmin, fmax;
	praat_get_timeRange (dia, & tmin, & tmax);
	if (! praat_get_frequencyRange (dia, & fmin, & fmax)) return 0;
	praat_picture_open ();
	TextGrid_Pitch_drawSeparately (ONLY (classTextGrid), ONLY (classPitch), GRAPHICS,
		tmin, tmax, fmin, fmax, GET_INTEGER ("Show boundaries"),
		GET_INTEGER ("Use text styles"), GET_INTEGER ("Garnish"), speckle, unit);
	praat_picture_close ();
	return 1;
}

FORM (TextGrid_Pitch_drawSeparately, "TextGrid & Pitch: Draw separately", 0)
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, "0.0")
	REAL (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, Pitch_UNIT_HERTZ)) return 0;
END

FORM (TextGrid_Pitch_drawSeparatelyErb, "TextGrid & Pitch: Draw separately erb", 0)
	praat_dia_timeRange (dia);
	REAL ("left Frequency range (ERB)", "0")
	REAL ("right Frequency range (ERB)", "10.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, Pitch_UNIT_ERB)) return 0;
END

FORM (TextGrid_Pitch_drawSeparatelyLogarithmic, "TextGrid & Pitch: Draw separately logarithmic", 0)
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, "50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, Pitch_UNIT_HERTZ_LOGARITHMIC)) return 0;
END

FORM (TextGrid_Pitch_drawSeparatelyMel, "TextGrid & Pitch: Draw separately mel", 0)
	praat_dia_timeRange (dia);
	REAL ("left Frequency range (mel)", "0")
	REAL ("right Frequency range (mel)", "500")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, Pitch_UNIT_MEL)) return 0;
END

FORM (TextGrid_Pitch_drawSeparatelySemitones, "TextGrid & Pitch: Draw separately semitones", 0)
	praat_dia_timeRange (dia);
	LABEL ("", "Range in semitones re 100 Hertz:")
	REAL ("left Frequency range (st)", "-12.0")
	REAL ("right Frequency range (st)", "30.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_NO, Pitch_UNIT_SEMITONES_100)) return 0;
END

FORM (TextGrid_Pitch_speckle, "TextGrid & Pitch: Speckle", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, "0.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	OPTIONMENU ("Text alignment", 2) OPTION ("Left") OPTION ("Centre") OPTION ("Right")
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, Pitch_UNIT_HERTZ)) return 0;
END

FORM (TextGrid_Pitch_speckleErb, "TextGrid & Pitch: Speckle erb", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	praat_dia_timeRange (dia);
	REAL ("left Frequency range (ERB)", "0")
	REAL ("right Frequency range (ERB)", "10.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	OPTIONMENU ("Text alignment", 2) OPTION ("Left") OPTION ("Centre") OPTION ("Right")
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, Pitch_UNIT_ERB)) return 0;
END

FORM (TextGrid_Pitch_speckleLogarithmic, "TextGrid & Pitch: Speckle logarithmic", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, "50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	OPTIONMENU ("Text alignment", 2) OPTION ("Left") OPTION ("Centre") OPTION ("Right")
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, Pitch_UNIT_HERTZ_LOGARITHMIC)) return 0;
END

FORM (TextGrid_Pitch_speckleMel, "TextGrid & Pitch: Speckle mel", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	praat_dia_timeRange (dia);
	REAL ("left Frequency range (mel)", "0")
	REAL ("right Frequency range (mel)", "500")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	OPTIONMENU ("Text alignment", 2) OPTION ("Left") OPTION ("Centre") OPTION ("Right")
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, Pitch_UNIT_MEL)) return 0;
END

FORM (TextGrid_Pitch_speckleSemitones, "TextGrid & Pitch: Speckle semitones", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	praat_dia_timeRange (dia);
	LABEL ("", "Range in semitones re 100 Hertz:")
	REAL ("left Frequency range (st)", "-12.0")
	REAL ("right Frequency range (st)", "30.0")
	INTEGER ("Font size (points)", "18")
	BOOLEAN ("Use text styles", 1)
	OPTIONMENU ("Text alignment", 2) OPTION ("Left") OPTION ("Centre") OPTION ("Right")
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_draw (dia, Pitch_speckle_YES, Pitch_UNIT_SEMITONES_100)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparately, "TextGrid & Pitch: Speckle separately", 0)
	praat_dia_timeRange (dia);
	REAL (STRING_FROM_FREQUENCY_HZ, "0.0")
	REAL (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, Pitch_UNIT_HERTZ)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparatelyErb, "TextGrid & Pitch: Speckle separately erb", 0)
	praat_dia_timeRange (dia);
	REAL ("left Frequency range (ERB)", "0")
	REAL ("right Frequency range (ERB)", "10.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, Pitch_UNIT_ERB)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparatelyLogarithmic, "TextGrid & Pitch: Speckle separately logarithmic", 0)
	praat_dia_timeRange (dia);
	POSITIVE (STRING_FROM_FREQUENCY_HZ, "50.0")
	POSITIVE (STRING_TO_FREQUENCY_HZ, "500.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, Pitch_UNIT_HERTZ_LOGARITHMIC)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparatelyMel, "TextGrid & Pitch: Speckle separately mel", 0)
	praat_dia_timeRange (dia);
	REAL ("left Frequency range (mel)", "0")
	REAL ("right Frequency range (mel)", "500")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, Pitch_UNIT_MEL)) return 0;
END

FORM (TextGrid_Pitch_speckleSeparatelySemitones, "TextGrid & Pitch: Speckle separately semitones", 0)
	praat_dia_timeRange (dia);
	LABEL ("", "Range in semitones re 100 Hertz:")
	REAL ("left Frequency range (st)", "-12.0")
	REAL ("right Frequency range (st)", "30.0")
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	if (! pr_TextGrid_Pitch_drawSeparately (dia, Pitch_speckle_YES, Pitch_UNIT_SEMITONES_100)) return 0;
END

/***** PITCH & TEXTTIER *****/

FORM (Pitch_TextTier_to_PitchTier, "Pitch & TextTier to PitchTier", "Pitch & TextTier: To PitchTier...")
	RADIO ("Unvoiced strategy", 3)
	RADIOBUTTON ("Zero")
	RADIOBUTTON ("Error")
	RADIOBUTTON ("Interpolate")
	OK
DO
	if (! praat_new (Pitch_AnyTier_to_PitchTier (ONLY (classPitch), ONLY (classTextTier),
		GET_INTEGER ("Unvoiced strategy") - 1), ((Pitch) (ONLY (classPitch))) -> name)) return 0;
END

/***** SOUND & TEXTGRID *****/

FORM (TextGrid_Sound_draw, "TextGrid & Sound: Draw...", 0)
	praat_dia_timeRange (dia);
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	praat_picture_open ();
	TextGrid_Sound_draw (ONLY (classTextGrid), ONLY (classSound), GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Show boundaries"),
		GET_INTEGER ("Use text styles"), GET_INTEGER ("Garnish"));
	praat_picture_close ();
END

FORM (TextGrid_Sound_extractAllIntervals, "TextGrid & Sound: Extract all intervals", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	BOOLEAN ("Preserve times", 0)
	OK
DO
	if (! praat_new (TextGrid_Sound_extractAllIntervals (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER ("Preserve times")), "dummy")) return 0;
END

FORM (TextGrid_Sound_extractNonemptyIntervals, "TextGrid & Sound: Extract non-empty intervals", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	BOOLEAN ("Preserve times", 0)
	OK
DO
	if (! praat_new (TextGrid_Sound_extractNonemptyIntervals (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER ("Preserve times")), "dummy")) return 0;
END

FORM (TextGrid_Sound_extractIntervals, "TextGrid & Sound: Extract intervals", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	BOOLEAN ("Preserve times", 0)
	SENTENCE ("Label text", "")
	OK
DO
	if (! praat_new (TextGrid_Sound_extractIntervalsWhere (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER (STRING_TIER_NUMBER), Melder_STRING_EQUAL_TO, GET_STRING ("Label text"),
		GET_INTEGER ("Preserve times")), GET_STRING ("Label text"))) return 0;
END

FORM (TextGrid_Sound_extractIntervalsWhere, "TextGrid & Sound: Extract intervals", 0)
	INTEGER (STRING_TIER_NUMBER, "1")
	BOOLEAN ("Preserve times", 0)
	OPTIONMENU ("Extract all intervals whose label...", 1)
	OPTIONS_ENUM (Melder_STRING_text_finiteVerb (itext), Melder_STRING_min, Melder_STRING_max)
	SENTENCE ("...the text", "")
	OK
DO
	if (! praat_new (TextGrid_Sound_extractIntervalsWhere (ONLY (classTextGrid), ONLY (classSound),
		GET_INTEGER (STRING_TIER_NUMBER),
		GET_INTEGER ("Extract all intervals whose label...") - 1 + Melder_STRING_min,
		GET_STRING ("...the text"),
		GET_INTEGER ("Preserve times")), GET_STRING ("...the text"))) return 0;
END

DIRECT (TextGrid_Sound_scaleTimes)
	TextGrid grid = ONLY (classTextGrid);
	TextGrid_Function_scaleTimes (grid, ONLY (classSound));
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

FORM (SpellingChecker_addNewWord, "Add word to user dictionary", "SpellingChecker")
	SENTENCE ("New word", "")
	OK
DO
	WHERE (SELECTED) {
		if (! SpellingChecker_addNewWord (OBJECT, GET_STRING ("New word"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (SpellingChecker_edit, "Edit spelling checker", "SpellingChecker")
	LABEL ("", "-- Syntax --")
	SENTENCE ("Forbidden strings", "")
	BOOLEAN ("Check matching parentheses", 0)
	SENTENCE ("Separating characters", "")
	BOOLEAN ("Allow all parenthesized", 0)
	LABEL ("", "-- Capitals --")
	BOOLEAN ("Allow all names", 0)
	SENTENCE ("Name prefixes", "")
	BOOLEAN ("Allow all abbreviations", 0)
	LABEL ("", "-- Capitalization --")
	BOOLEAN ("Allow caps sentence-initially", 0)
	BOOLEAN ("Allow caps after colon", 0)
	LABEL ("", "-- Word parts --")
	SENTENCE ("Allow all words containing", "")
	SENTENCE ("Allow all words starting with", "")
	SENTENCE ("Allow all words ending in", "")
	OK
SpellingChecker me = ONLY_OBJECT;
SET_STRING ("Forbidden strings", my forbiddenStrings)
SET_INTEGER ("Check matching parentheses", my checkMatchingParentheses)
SET_STRING ("Separating characters", my separatingCharacters)
SET_INTEGER ("Allow all parenthesized", my allowAllParenthesized)
SET_INTEGER ("Allow all names", my allowAllNames)
SET_STRING ("Name prefixes", my namePrefixes)
SET_INTEGER ("Allow all abbreviations", my allowAllAbbreviations)
SET_INTEGER ("Allow caps sentence-initially", my allowCapsSentenceInitially)
SET_INTEGER ("Allow caps after colon", my allowCapsAfterColon)
SET_STRING ("Allow all words containing", my allowAllWordsContaining)
SET_STRING ("Allow all words starting with", my allowAllWordsStartingWith)
SET_STRING ("Allow all words ending in", my allowAllWordsEndingIn)
DO
	SpellingChecker me = ONLY_OBJECT;
	Melder_free (my forbiddenStrings); my forbiddenStrings = Melder_strdup (GET_STRING ("Forbidden strings"));
	my checkMatchingParentheses = GET_INTEGER ("Check matching parentheses");
	Melder_free (my separatingCharacters); my separatingCharacters = Melder_strdup (GET_STRING ("Separating characters"));
	my allowAllParenthesized = GET_INTEGER ("Allow all parenthesized");
	my allowAllNames = GET_INTEGER ("Allow all names");
	Melder_free (my namePrefixes); my namePrefixes = Melder_strdup (GET_STRING ("Name prefixes"));
	my allowAllAbbreviations = GET_INTEGER ("Allow all abbreviations");
	my allowCapsSentenceInitially = GET_INTEGER ("Allow caps sentence-initially");
	my allowCapsAfterColon = GET_INTEGER ("Allow caps after colon");
	Melder_free (my allowAllWordsContaining); my allowAllWordsContaining = Melder_strdup (GET_STRING ("Allow all words containing"));
	Melder_free (my allowAllWordsStartingWith); my allowAllWordsStartingWith = Melder_strdup (GET_STRING ("Allow all words starting with"));
	Melder_free (my allowAllWordsEndingIn); my allowAllWordsEndingIn = Melder_strdup (GET_STRING ("Allow all words ending in"));
	praat_dataChanged (me);
END

DIRECT (SpellingChecker_extractWordList)
	EVERY_TO (SpellingChecker_extractWordList (OBJECT))
END

DIRECT (SpellingChecker_extractUserDictionary)
	EVERY_TO (SpellingChecker_extractUserDictionary (OBJECT))
END

FORM (SpellingChecker_isWordAllowed, "Is word allowed?", "SpellingChecker")
	SENTENCE ("Word", "")
	OK
DO
	Melder_information (SpellingChecker_isWordAllowed (ONLY_OBJECT, GET_STRING ("Word")) ?
		"1 (allowed)" : "0 (not allowed)");
END

FORM (SpellingChecker_nextNotAllowedWord, "Next not allowed word?", "SpellingChecker")
	LABEL ("", "Sentence:")
	TEXTFIELD ("sentence", "")
	INTEGER ("Starting character", "0")
	OK
DO
	char *sentence = GET_STRING ("sentence"), *result;
	int startingCharacter = GET_INTEGER ("Starting character");
	REQUIRE (startingCharacter >= 0, "Starting character should be 0 or positive.")
	REQUIRE (startingCharacter <= (int) strlen (sentence), "Starting character should not exceed end of sentence.")
	result = SpellingChecker_nextNotAllowedWord (ONLY_OBJECT, sentence, & startingCharacter);
	Melder_information ("%s", result ? result : "");
END

DIRECT (SpellingChecker_replaceWordList)
	if (! SpellingChecker_replaceWordList (ONLY (classSpellingChecker), ONLY (classWordList))) return 0;
END

DIRECT (SpellingChecker_replaceWordList_help)
	Melder_information ("To replace the checker's word list\nby the contents of a Strings object:\n"
		"1. select the Strings;\n2. convert to a WordList object;\n3. select the SpellingChecker and the WordList;\n"
		"4. choose Replace.");
END

DIRECT (SpellingChecker_replaceUserDictionary)
	if (! SpellingChecker_replaceUserDictionary (ONLY (classSpellingChecker), ONLY (classSortedSetOfString))) return 0;
END

/***** TEXTGRID *****/

FORM (TextGrid_countLabels, "Count labels", "TextGrid: Count labels...")
	INTEGER (STRING_TIER_NUMBER, "1")
	SENTENCE ("Label text", "a")
	OK
DO
	Melder_information ("%ld", TextGrid_countLabels (ONLY (classTextGrid),
		GET_INTEGER (STRING_TIER_NUMBER), GET_STRING ("Label text")));
END

FORM (TextGrid_draw, "TextGrid: Draw", 0)
	praat_dia_timeRange (dia);
	BOOLEAN ("Show boundaries", 1)
	BOOLEAN ("Use text styles", 1)
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (TextGrid_Sound_draw (OBJECT, NULL, GRAPHICS,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Show boundaries"),
		GET_INTEGER ("Use text styles"), GET_INTEGER ("Garnish")))
END

FORM (TextGrid_duplicateTier, "TextGrid: Duplicate tier", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL ("Position", "1 (= at top)")
	WORD ("Name", "")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int itier = GET_INTEGER (STRING_TIER_NUMBER);
		int position = GET_INTEGER ("Position");
		const char *name = GET_STRING ("Name");
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
	if (! praat_new (publish, NULL)) { Melder_flushError (NULL); return; }
	praat_updateSelection ();
	if (Thing_member (publish, classSpectrum) && strequ (Thing_getName (publish), "slice")) {
		int IOBJECT;
		WHERE (SELECTED) {
			SpectrumEditor editor2 = SpectrumEditor_create (praat.topShell, FULL_NAME, OBJECT);
			if (! editor2) return;
			if (! praat_installEditor (editor2, IOBJECT)) Melder_flushError (NULL);
		}
	}
}
DIRECT (TextGrid_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a TextGrid from batch.");
	} else {
		WHERE (SELECTED && CLASS == classTextGrid) {
			TextGridEditor editor = TextGridEditor_create (praat.topShell, FULL_NAME,
				OBJECT, ONLY (classSound), NULL);
			if (! praat_installEditor (editor, IOBJECT)) return 0;
			Editor_setPublishCallback (editor, cb_TextGridEditor_publish, NULL);
		}
	}
END

DIRECT (TextGrid_LongSound_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a TextGrid from batch.");
	} else {
		LongSound longSound = NULL;
		int ilongSound = 0;
		WHERE (SELECTED)
			if (CLASS == classLongSound) longSound = OBJECT, ilongSound = IOBJECT;
		Melder_assert (ilongSound != 0);
		WHERE (SELECTED && CLASS == classTextGrid)
			if (! praat_installEditor2 (TextGridEditor_create (praat.topShell, FULL_NAME,
				OBJECT, longSound, NULL), IOBJECT, ilongSound)) return 0;
	}
END

DIRECT (TextGrid_SpellingChecker_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a TextGrid from batch.");
	} else {
		SpellingChecker spellingChecker = NULL;
		int ispellingChecker = 0;
		WHERE (SELECTED)
			if (CLASS == classSpellingChecker) spellingChecker = OBJECT, ispellingChecker = IOBJECT;
		Melder_assert (ispellingChecker != 0);
		WHERE (SELECTED && CLASS == classTextGrid)
			if (! praat_installEditor2 (TextGridEditor_create (praat.topShell, FULL_NAME,
				OBJECT, ONLY (classSound), spellingChecker), IOBJECT, ispellingChecker)) return 0;
	}
END

DIRECT (TextGrid_LongSound_SpellingChecker_edit)
	if (praat.batch) {
		return Melder_error ("Cannot edit a TextGrid from batch.");
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
			if (! praat_installEditor3 (TextGridEditor_create (praat.topShell, FULL_NAME,
				OBJECT, longSound, spellingChecker), IOBJECT, ilongSound, ispellingChecker)) return 0;
	}
END

FORM (TextGrid_extractPart, "TextGrid: Extract part", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "1.0")
	BOOLEAN ("Preserve times", 0)
	OK
DO
	TextGrid grid = ONLY (classTextGrid);
	if (! praat_new (TextGrid_extractPart (grid, GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Preserve times")), "%s_part", grid -> name)) return 0;
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
	if (tier -> methods != (Data_Table) classIntervalTier) return Melder_errorp ("Tier should be interval tier.");
	return (IntervalTier) tier;
}

static TextTier pr_TextGrid_getTextTier (Any dia) {
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return NULL;
	if (tier -> methods != (Data_Table) classTextTier) return Melder_errorp ("Tier should be point tier (TextTier).");
	return (TextTier) tier;
}

static TextInterval pr_TextGrid_getInterval (Any dia) {
	int intervalNumber = GET_INTEGER (STRING_INTERVAL_NUMBER);
	IntervalTier intervalTier = pr_TextGrid_getIntervalTier (dia);
	if (! intervalTier) return NULL;
	if (intervalNumber > intervalTier -> intervals -> size) return Melder_errorp ("Interval number too large.");
	return intervalTier -> intervals -> item [intervalNumber];
}

static TextPoint pr_TextGrid_getPoint (Any dia) {	
	long pointNumber = GET_INTEGER (STRING_POINT_NUMBER);
	TextTier textTier = pr_TextGrid_getTextTier (dia);
	if (! textTier) return NULL;
	if (pointNumber > textTier -> points -> size) return Melder_errorp ("Point number too large.");
	return textTier -> points -> item [pointNumber];
}

FORM (TextGrid_extractTier, "TextGrid: Extract tier", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	OK
DO
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return 0;
	if (! praat_new (Data_copy (tier), "%s", tier -> name)) return 0;
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

FORM (TextGrid_getIntervalAtTime, "TextGrid: Get interval at time", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	REAL ("Time (s)", "0.5")
	OK
DO
	IntervalTier intervalTier = pr_TextGrid_getIntervalTier (dia);
	if (! intervalTier) return 0;
	Melder_information ("%ld", IntervalTier_timeToIndex (intervalTier, GET_REAL ("Time")));
END

FORM (TextGrid_getNumberOfIntervals, "TextGrid: Get number of intervals", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	OK
DO
	IntervalTier intervalTier = pr_TextGrid_getIntervalTier (dia);
	if (! intervalTier) return 0;
	Melder_information ("%ld", intervalTier -> intervals -> size);
END

DIRECT (TextGrid_getNumberOfTiers)
	TextGrid grid = ONLY_OBJECT;
	Melder_information ("%ld", grid -> tiers -> size);
END

FORM (TextGrid_getStartingPoint, "TextGrid: Get starting point", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL (STRING_INTERVAL_NUMBER, "1")
	OK
DO
	TextInterval interval = pr_TextGrid_getInterval (dia);
	if (! interval) return 0;
	Melder_informationReal (interval -> xmin, "seconds");
END
	
FORM (TextGrid_getEndPoint, "TextGrid: Get end point", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL (STRING_INTERVAL_NUMBER, "1")
	OK
DO
	TextInterval interval = pr_TextGrid_getInterval (dia);
	if (! interval) return 0;
	Melder_informationReal (interval -> xmax, "seconds");
END
	
FORM (TextGrid_getLabelOfInterval, "TextGrid: Get label of interval", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL (STRING_INTERVAL_NUMBER, "1")
	OK
DO
	TextInterval interval = pr_TextGrid_getInterval (dia);
	if (! interval) return 0;
	Melder_information (interval -> text ? "%s" : "", interval -> text);
END
	
FORM (TextGrid_getNumberOfPoints, "TextGrid: Get number of points", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	OK
DO
	TextTier textTier = pr_TextGrid_getTextTier (dia);
	if (! textTier) return 0;
	Melder_information ("%ld", textTier -> points -> size);
END
	
FORM (TextGrid_getTierName, "TextGrid: Get tier name", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	OK
DO
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return 0;
	Melder_information ("%s", tier -> name);
END

FORM (TextGrid_getTimeOfPoint, "TextGrid: Get time of point", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL (STRING_POINT_NUMBER, "1")
	OK
DO
	TextPoint point = pr_TextGrid_getPoint (dia);
	if (! point) return 0;
	Melder_informationReal (point -> time, "seconds");
END
	
FORM (TextGrid_getLabelOfPoint, "TextGrid: Get label of point", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL (STRING_POINT_NUMBER, "1")
	OK
DO
	TextPoint point = pr_TextGrid_getPoint (dia);
	if (! point) return 0;
	Melder_information (point -> mark ? "%s" : "", point -> mark);
END
	
DIRECT (TextGrid_help) Melder_help ("TextGrid"); END

FORM (TextGrid_insertBoundary, "TextGrid: Insert boundary", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	REAL ("Time (s)", "0.5")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_insertBoundary (OBJECT, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL ("Time"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_insertIntervalTier, "TextGrid: Insert interval tier", 0)
	NATURAL ("Position", "1 (= at top)")
	WORD ("Name", "")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int position = GET_INTEGER ("Position");
		char *name = GET_STRING ("Name");
		IntervalTier tier = IntervalTier_create (grid -> xmin, grid -> xmax);
		if (! tier) return 0;
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (tier, name);
		if (! Ordered_addItemPos (grid -> tiers, tier, position)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_insertPoint, "TextGrid: Insert point", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	REAL ("Time (s)", "0.5")
	LABEL ("", "Text:")
	TEXTFIELD ("text", "")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_insertPoint (OBJECT, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL ("Time"), GET_STRING ("text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_insertPointTier, "TextGrid: Insert point tier", 0)
	NATURAL ("Position", "1 (= at top)")
	WORD ("Name", "")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int position = GET_INTEGER ("Position");
		char *name = GET_STRING ("Name");
		TextTier tier = TextTier_create (grid -> xmin, grid -> xmax);
		if (! tier) return 0;
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (tier, name);
		if (! Ordered_addItemPos (grid -> tiers, tier, position)) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_isIntervalTier, "TextGrid: Is interval tier?", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	OK
DO
	Data tier = pr_TextGrid_getTier (dia);
	if (! tier) return 0;
	Melder_information (tier -> methods == (Data_Table) classIntervalTier ?
		"1 (yes, tier %ld is an interval tier)" : "0 (no, tier %ld is a point tier)",
		GET_INTEGER (STRING_TIER_NUMBER));
END

DIRECT (TextGrids_merge)
	Collection textGrids;
	int n = 0;
	WHERE (SELECTED) n ++;
	textGrids = Collection_create (classTextGrid, n);
	WHERE (SELECTED) Collection_addItem (textGrids, OBJECT);
	if (! praat_new (TextGrid_merge (textGrids), "merged")) {
		textGrids -> size = 0;   /* Undangle. */
		forget (textGrids);
		return 0;
	}
	textGrids -> size = 0;   /* Undangle. */
	forget (textGrids);
END

DIRECT (info_TextGrid_Pitch_draw)
	Melder_information ("You can draw a TextGrid together with a Pitch after selecting them both.");
END

FORM (TextGrid_removeBoundaryAtTime, "TextGrid: Remove boundary at time", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	REAL ("Time (s)", "0.5")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_removeBoundaryAtTime (OBJECT, GET_INTEGER (STRING_TIER_NUMBER), GET_REAL ("Time"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_removeLeftBoundary, "TextGrid: Remove left boundary", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL (STRING_INTERVAL_NUMBER, "2")
	OK
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long iinterval = GET_INTEGER (STRING_INTERVAL_NUMBER);
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		IntervalTier intervalTier;
		if (itier > grid -> tiers -> size)
			return Melder_error ("You cannot remove a boundary from tier %ld of TextGrid \"%s\", "
				"because that TextGrid has only %ld tiers.", itier, NAME, grid -> tiers -> size);
		intervalTier = grid -> tiers -> item [itier];
		if (intervalTier -> methods != classIntervalTier)
			return Melder_error ("You cannot remove a boundary from tier %ld of TextGrid \"%s\", "
				"because that tier is a point tier instead of an interval tier.", itier, NAME);
		if (iinterval > intervalTier -> intervals -> size)
			return Melder_error ("You cannot remove a boundary from interval %ld of tier %ld of TextGrid \"%s\", "
				"because that tier has only %ld intervals.", iinterval, itier, NAME, intervalTier -> intervals -> size);
		if (iinterval == 1)
			return Melder_error ("You cannot remove the left boundary from interval 1 of tier %ld of TextGrid \"%s\", "
				"because this is at the left edge of the tier.", itier, NAME);
		if (! IntervalTier_removeLeftBoundary (intervalTier, iinterval)) return 0;
		praat_dataChanged (grid);
	}
END

FORM (TextGrid_removePoint, "TextGrid: Remove point", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL (STRING_POINT_NUMBER, "2")
	OK
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long ipoint = GET_INTEGER (STRING_POINT_NUMBER);
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		TextTier pointTier;
		if (itier > grid -> tiers -> size)
			return Melder_error ("You cannot remove a point from tier %ld of TextGrid \"%s\", "
				"because that TextGrid has only %ld tiers.", itier, NAME, grid -> tiers -> size);
		pointTier = grid -> tiers -> item [itier];
		if (pointTier -> methods != classTextTier)
			return Melder_error ("You cannot remove a point from tier %ld of TextGrid \"%s\", "
				"because that tier is an interval tier instead of a point tier.", itier, NAME);
		if (ipoint > pointTier -> points -> size)
			return Melder_error ("You cannot remove point %ld from tier %ld of TextGrid \"%s\", "
				"because that tier has only %ld points.", ipoint, itier, NAME, pointTier -> points -> size);
		TextTier_removePoint (pointTier, ipoint);
		praat_dataChanged (grid);
	}
END

FORM (TextGrid_removeRightBoundary, "TextGrid: Remove right boundary", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL (STRING_INTERVAL_NUMBER, "1")
	OK
DO
	long itier = GET_INTEGER (STRING_TIER_NUMBER);
	long iinterval = GET_INTEGER (STRING_INTERVAL_NUMBER);
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		IntervalTier intervalTier;
		if (itier > grid -> tiers -> size)
			return Melder_error ("You cannot remove a boundary from tier %ld of TextGrid \"%s\", "
				"because that TextGrid has only %ld tiers.", itier, NAME, grid -> tiers -> size);
		intervalTier = grid -> tiers -> item [itier];
		if (intervalTier -> methods != classIntervalTier)
			return Melder_error ("You cannot remove a boundary from tier %ld of TextGrid \"%s\", "
				"because that tier is a point tier instead of an interval tier.", itier, NAME);
		if (iinterval > intervalTier -> intervals -> size)
			return Melder_error ("You cannot remove a boundary from interval %ld of tier %ld of TextGrid \"%s\", "
				"because that tier has only %ld intervals.", iinterval, itier, NAME, intervalTier -> intervals -> size);
		if (iinterval == intervalTier -> intervals -> size)
			return Melder_error ("You cannot remove the right boundary from interval %ld of tier %ld of TextGrid \"%s\", "
				"because this is at the right edge of the tier.", iinterval, itier, NAME);
		if (! IntervalTier_removeLeftBoundary (intervalTier, iinterval + 1)) return 0;
		praat_dataChanged (grid);
	}
END

FORM (TextGrid_removeTier, "TextGrid: Remove tier", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	OK
DO
	WHERE (SELECTED) {
		TextGrid grid = OBJECT;
		int itier = GET_INTEGER (STRING_TIER_NUMBER);
		if (grid -> tiers -> size <= 1) {
			return Melder_error ("Sorry, I refuse to remove the last tier.");
		}
		if (itier > grid -> tiers -> size) itier = grid -> tiers -> size;
		Collection_removeItem (grid -> tiers, itier);
		praat_dataChanged (grid);
	}
END

DIRECT (info_TextGrid_Sound_edit)
	Melder_information ("To include a copy of a Sound in your TextGrid editor:\n"
		"   select a TextGrid and a Sound, and click \"Edit\".");
END

DIRECT (info_TextGrid_Sound_draw)
	Melder_information ("You can draw a TextGrid together with a Sound after selecting them both.");
END

FORM (TextGrid_scaleTimes, "TextGrid: Scale times", 0)
	REAL ("left New time domain (s)", "0.0")
	REAL ("right New time domain (s)", "1.0")
	OK
DO
	double tmin = GET_REAL ("left New time domain"), tmax = GET_REAL ("right New time domain");
	if (tmin >= tmax) return Melder_error ("Duration of time domain must be positive.");
	WHERE (SELECTED) {
		TextGrid_scaleTimes (OBJECT, tmin, tmax);
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_setIntervalText, "TextGrid: Set interval text", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL (STRING_INTERVAL_NUMBER, "1")
	LABEL ("", "Text:")
	TEXTFIELD ("text", "")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_setIntervalText (OBJECT, GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (STRING_INTERVAL_NUMBER), GET_STRING ("text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_setPointText, "TextGrid: Set point text", 0)
	NATURAL (STRING_TIER_NUMBER, "1")
	NATURAL (STRING_POINT_NUMBER, "1")
	LABEL ("", "Text:")
	TEXTFIELD ("text", "")
	OK
DO
	WHERE (SELECTED) {
		if (! TextGrid_setPointText (OBJECT, GET_INTEGER (STRING_TIER_NUMBER), GET_INTEGER (STRING_POINT_NUMBER), GET_STRING ("text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (TextGrid_shiftTimes, "TextGrid: Shift times", 0)
	REAL ("Time shift (s)", "0.5")
	OK
DO
	WHERE (SELECTED) {
		TextGrid_shiftTimes (OBJECT, GET_REAL ("Time shift"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (TextGrid_shiftToZero)
	WHERE (SELECTED) {
		TextGrid_shiftToZero (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

FORM_WRITE (TextGrid_writeToChronologicalTextFile, "Text file", 0, 0)
	if (! TextGrid_writeToChronologicalTextFile (ONLY_OBJECT, file)) return 0;
END

/***** TEXTGRID & ANYTIER *****/

DIRECT (TextGrid_AnyTier_append)
	TextGrid oldGrid = ONLY (classTextGrid), newGrid = Data_copy (oldGrid);
	if (! newGrid) return 0;
	WHERE (SELECTED && OBJECT != oldGrid) if (! TextGrid_add (newGrid, OBJECT)) { forget (newGrid); return 0; }
	if (! praat_new (newGrid, oldGrid -> name)) return 0;
END

/***** TEXTGRID & LONGSOUND *****/

DIRECT (TextGrid_LongSound_scaleTimes)
	TextGrid grid = ONLY (classTextGrid);
	TextGrid_Function_scaleTimes (grid, ONLY (classLongSound));
	praat_dataChanged (grid);
END

/***** TEXTTIER *****/

FORM (TextTier_addPoint, "TextTier: Add point", "TextTier: Add point...")
	REAL ("Time (s)", "0.5")
	SENTENCE ("Text", "")
	OK
DO
	WHERE (SELECTED) {
		if (! TextTier_addPoint (OBJECT, GET_REAL ("Time"), GET_STRING ("Text"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

DIRECT (TextTier_downto_PointProcess)
	EVERY_TO (AnyTier_downto_PointProcess (OBJECT))
END

FORM (TextTier_downto_TableOfReal, "TextTier: Down to TableOfReal", 0)
	SENTENCE ("Label", "")
	OK
DO
	EVERY_TO (TextTier_downto_TableOfReal (OBJECT, GET_STRING ("Label")))
END

DIRECT (TextTier_downto_TableOfReal_any)
	EVERY_TO (TextTier_downto_TableOfReal_any (OBJECT))
END

FORM (TextTier_getPoints, "Get points", 0)
	SENTENCE ("Text", "")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new (TextTier_getPoints (OBJECT, GET_STRING ("Text")), GET_STRING ("Text"))) return 0;
END

DIRECT (TextTier_help) Melder_help ("TextTier"); END

/***** WORDLIST *****/

FORM (WordList_hasWord, "Does word occur in list?", "WordList")
	SENTENCE ("Word", "")
	OK
DO
	Melder_information (WordList_hasWord (ONLY_OBJECT, GET_STRING ("Word")) ? "1" : "0");
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
	Thing_recognizeClassByOtherName (classTextTier, "MarkTier");

	TextGridEditor_prefs ();

	praat_addAction1 (classIntervalTier, 0, "IntervalTier help", 0, 0, DO_IntervalTier_help);
	praat_addAction1 (classIntervalTier, 1, "Write to Xwaves label file...", 0, 0, DO_IntervalTier_writeToXwaves);
	praat_addAction1 (classIntervalTier, 0, "Collect", 0, 0, 0);
	praat_addAction1 (classIntervalTier, 0, "Into TextGrid", 0, 0, DO_AnyTier_into_TextGrid);
	praat_addAction1 (classIntervalTier, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classIntervalTier, 0, "Get starting points...", 0, 0, DO_IntervalTier_getStartingPoints);
	praat_addAction1 (classIntervalTier, 0, "Get centre points...", 0, 0, DO_IntervalTier_getCentrePoints);
	praat_addAction1 (classIntervalTier, 0, "Get end points...", 0, 0, DO_IntervalTier_getEndPoints);
	praat_addAction1 (classIntervalTier, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classIntervalTier, 0, "Down to TableOfReal (any)", 0, 0, DO_IntervalTier_downto_TableOfReal_any);
	praat_addAction1 (classIntervalTier, 0, "Down to TableOfReal...", 0, 0, DO_IntervalTier_downto_TableOfReal);

	praat_addAction1 (classLabel, 0, "& Sound: To TextGrid?", 0, 0, DO_info_Label_Sound_to_TextGrid);

	praat_addAction1 (classSpellingChecker, 1, "Edit...", 0, 0, DO_SpellingChecker_edit);
	praat_addAction1 (classSpellingChecker, 0, "Query", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 1, "Is word allowed...", 0, 0, DO_SpellingChecker_isWordAllowed);
	praat_addAction1 (classSpellingChecker, 1, "Next not allowed word...", 0, 0, DO_SpellingChecker_nextNotAllowedWord);
	praat_addAction1 (classSpellingChecker, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 0, "Add new word...", 0, 0, DO_SpellingChecker_addNewWord);
	praat_addAction1 (classSpellingChecker, 0, "Analyze", 0, 0, 0);
	praat_addAction1 (classSpellingChecker, 0, "Extract WordList", 0, 0, DO_SpellingChecker_extractWordList);
	praat_addAction1 (classSpellingChecker, 0, "Extract user dictionary", 0, 0, DO_SpellingChecker_extractUserDictionary);

	praat_addAction1 (classTextGrid, 0, "TextGrid help", 0, 0, DO_TextGrid_help);
	praat_addAction1 (classTextGrid, 1, "Write to chronological text file...", 0, 0, DO_TextGrid_writeToChronologicalTextFile);
	praat_addAction1 (classTextGrid, 0, "Edit", 0, 0, 0);
	praat_addAction1 (classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_edit);
	praat_addAction1 (classTextGrid, 1, "& Sound: Edit?", 0, 0, DO_info_TextGrid_Sound_edit);
	praat_addAction1 (classTextGrid, 0, "Draw", 0, 0, 0);
	praat_addAction1 (classTextGrid, 0, "Draw...", 0, 0, DO_TextGrid_draw);
	praat_addAction1 (classTextGrid, 1, "& Sound: Draw?", 0, 0, DO_info_TextGrid_Sound_draw);
	praat_addAction1 (classTextGrid, 1, "& Pitch: Draw?", 0, 0, DO_info_TextGrid_Pitch_draw);
	praat_addAction1 (classTextGrid, 0, "Query -          ", 0, 0, 0);
		praat_TimeFunction_query_init (classTextGrid);
		praat_addAction1 (classTextGrid, 1, "-- query tiers --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, "Get number of tiers", 0, 1, DO_TextGrid_getNumberOfTiers);
		praat_addAction1 (classTextGrid, 1, "Get tier name...", 0, 1, DO_TextGrid_getTierName);
		praat_addAction1 (classTextGrid, 1, "Is interval tier...", 0, 1, DO_TextGrid_isIntervalTier);
		praat_addAction1 (classTextGrid, 1, "-- query intervals --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, "Get number of intervals...", 0, 1, DO_TextGrid_getNumberOfIntervals);
		praat_addAction1 (classTextGrid, 1, "Get starting point...", 0, 1, DO_TextGrid_getStartingPoint);
		praat_addAction1 (classTextGrid, 1, "Get end point...", 0, 1, DO_TextGrid_getEndPoint);
		praat_addAction1 (classTextGrid, 1, "Get label of interval...", 0, 1, DO_TextGrid_getLabelOfInterval);
		praat_addAction1 (classTextGrid, 1, "Get interval at time...", 0, 1, DO_TextGrid_getIntervalAtTime);
		praat_addAction1 (classTextGrid, 1, "-- query points --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, "Get number of points...", 0, 1, DO_TextGrid_getNumberOfPoints);
		praat_addAction1 (classTextGrid, 1, "Get time of point...", 0, 1, DO_TextGrid_getTimeOfPoint);
		praat_addAction1 (classTextGrid, 1, "Get label of point...", 0, 1, DO_TextGrid_getLabelOfPoint);
		praat_addAction1 (classTextGrid, 1, "-- query labels --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 1, "Count labels...", 0, 1, DO_TextGrid_countLabels);
	praat_addAction1 (classTextGrid, 0, "Modify -        ", 0, 0, 0);
		praat_addAction1 (classTextGrid, 0, "Genericize", 0, 1, DO_TextGrid_genericize);
		praat_addAction1 (classTextGrid, 0, "Nativize", 0, 1, DO_TextGrid_nativize);
		praat_addAction1 (classTextGrid, 0, "Modify time domain", 0, 1, 0);
			praat_addAction1 (classTextGrid, 0, "Shift times...", 0, 2, DO_TextGrid_shiftTimes);
			praat_addAction1 (classTextGrid, 0, "Shift to zero", 0, 2, DO_TextGrid_shiftToZero);
			praat_addAction1 (classTextGrid, 0, "Scale times...", 0, 2, DO_TextGrid_scaleTimes);
		praat_addAction1 (classTextGrid, 0, "Modify interval tier", 0, 1, 0);
			praat_addAction1 (classTextGrid, 0, "Insert boundary...", 0, 2, DO_TextGrid_insertBoundary);
			praat_addAction1 (classTextGrid, 0, "Remove left boundary...", 0, 2, DO_TextGrid_removeLeftBoundary);
			praat_addAction1 (classTextGrid, 0, "Remove right boundary...", 0, 2, DO_TextGrid_removeRightBoundary);
			praat_addAction1 (classTextGrid, 0, "Remove boundary at time...", 0, 2, DO_TextGrid_removeBoundaryAtTime);
			praat_addAction1 (classTextGrid, 0, "Set interval text...", 0, 2, DO_TextGrid_setIntervalText);
		praat_addAction1 (classTextGrid, 0, "Modify point tier", 0, 1, 0);
			praat_addAction1 (classTextGrid, 0, "Insert point...", 0, 2, DO_TextGrid_insertPoint);
			praat_addAction1 (classTextGrid, 0, "Remove point...", 0, 2, DO_TextGrid_removePoint);
			praat_addAction1 (classTextGrid, 0, "Set point text...", 0, 2, DO_TextGrid_setPointText);
		praat_addAction1 (classTextGrid, 0, "-- modify tiers --", 0, 1, 0);
		praat_addAction1 (classTextGrid, 0, "Insert interval tier...", 0, 1, DO_TextGrid_insertIntervalTier);
		praat_addAction1 (classTextGrid, 0, "Insert point tier...", 0, 1, DO_TextGrid_insertPointTier);
		praat_addAction1 (classTextGrid, 0, "Duplicate tier...", 0, 1, DO_TextGrid_duplicateTier);
		praat_addAction1 (classTextGrid, 0, "Remove tier...", 0, 1, DO_TextGrid_removeTier);
praat_addAction1 (classTextGrid, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classTextGrid, 1, "Extract tier...", 0, 0, DO_TextGrid_extractTier);
	praat_addAction1 (classTextGrid, 1, "Extract part...", 0, 0, DO_TextGrid_extractPart);
praat_addAction1 (classTextGrid, 0, "Synthesize", 0, 0, 0);
	praat_addAction1 (classTextGrid, 0, "Merge", 0, 0, DO_TextGrids_merge);

	praat_addAction1 (classTextTier, 0, "TextTier help", 0, 0, DO_TextTier_help);
	praat_addAction1 (classTextTier, 0, "Query -          ", 0, 0, 0);
		praat_TimeTier_query_init (classTextTier);
	praat_addAction1 (classTextTier, 0, "Modify -          ", 0, 0, 0);
		praat_TimeTier_modify_init (classTextTier);
		praat_addAction1 (classTextTier, 0, "Add point...", 0, 1, DO_TextTier_addPoint);
	praat_addAction1 (classTextTier, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classTextTier, 0, "Get points...", 0, 0, DO_TextTier_getPoints);
	praat_addAction1 (classTextTier, 0, "Collect", 0, 0, 0);
	praat_addAction1 (classTextTier, 0, "Into TextGrid", 0, 0, DO_AnyTier_into_TextGrid);
	praat_addAction1 (classTextTier, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classTextTier, 0, "Down to PointProcess", 0, 0, DO_TextTier_downto_PointProcess);
	praat_addAction1 (classTextTier, 0, "Down to TableOfReal (any)", 0, 0, DO_TextTier_downto_TableOfReal_any);
	praat_addAction1 (classTextTier, 0, "Down to TableOfReal...", 0, 0, DO_TextTier_downto_TableOfReal);

	praat_addAction1 (classWordList, 0, "Query", 0, 0, 0);
		praat_addAction1 (classWordList, 1, "Has word...", 0, 0, DO_WordList_hasWord);
	praat_addAction1 (classWordList, 0, "Analyze", 0, 0, 0);
		praat_addAction1 (classWordList, 0, "To Strings", 0, 0, DO_WordList_to_Strings);
	praat_addAction1 (classWordList, 0, "Synthesize", 0, 0, 0);
		praat_addAction1 (classWordList, 0, "Up to SpellingChecker", 0, 0, DO_WordList_upto_SpellingChecker);

	praat_addAction2 (classIntervalTier, 1, classPointProcess, 1, "Start to centre...", 0, 0, DO_IntervalTier_PointProcess_startToCentre);
	praat_addAction2 (classIntervalTier, 1, classPointProcess, 1, "End to centre...", 0, 0, DO_IntervalTier_PointProcess_endToCentre);
	praat_addAction2 (classIntervalTier, 0, classTextTier, 0, "Collect", 0, 0, 0);
	praat_addAction2 (classIntervalTier, 0, classTextTier, 0, "Into TextGrid", 0, 0, DO_AnyTier_into_TextGrid);
	praat_addAction2 (classLabel, 1, classSound, 1, "To TextGrid", 0, 0, DO_Label_Sound_to_TextGrid);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_LongSound_edit);
	praat_addAction2 (classLongSound, 1, classTextGrid, 1, "Scale times", 0, 0, DO_TextGrid_LongSound_scaleTimes);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw -           ", 0, 0, 0);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw...", 0, 1, DO_TextGrid_Pitch_draw);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw logarithmic...", 0, 1, DO_TextGrid_Pitch_drawLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw semitones...", 0, 1, DO_TextGrid_Pitch_drawSemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw mel...", 0, 1, DO_TextGrid_Pitch_drawMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw erb...", 0, 1, DO_TextGrid_Pitch_drawErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle...", 0, 1, DO_TextGrid_Pitch_speckle);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle logarithmic...", 0, 1, DO_TextGrid_Pitch_speckleLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle semitones...", 0, 1, DO_TextGrid_Pitch_speckleSemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle mel...", 0, 1, DO_TextGrid_Pitch_speckleMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle erb...", 0, 1, DO_TextGrid_Pitch_speckleErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "-- draw separately --", 0, 1, 0);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw separately...", 0, 1, DO_TextGrid_Pitch_drawSeparately);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw separately (logarithmic)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelyLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw separately (semitones)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelySemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw separately (mel)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelyMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Draw separately (erb)...", 0, 1, DO_TextGrid_Pitch_drawSeparatelyErb);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle separately...", 0, 1, DO_TextGrid_Pitch_speckleSeparately);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle separately (logarithmic)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelyLogarithmic);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle separately (semitones)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelySemitones);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle separately (mel)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelyMel);
	praat_addAction2 (classPitch, 1, classTextGrid, 1, "Speckle separately (erb)...", 0, 1, DO_TextGrid_Pitch_speckleSeparatelyErb);
	praat_addAction2 (classPitch, 1, classTextTier, 1, "To PitchTier...", 0, 0, DO_Pitch_TextTier_to_PitchTier);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_edit);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Draw...", 0, 0, DO_TextGrid_Sound_draw);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Extract -       ", 0, 0, 0);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Extract all intervals...", 0, praat_DEPTH_1, DO_TextGrid_Sound_extractAllIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Extract non-empty intervals...", 0, praat_DEPTH_1, DO_TextGrid_Sound_extractNonemptyIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Extract intervals...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_TextGrid_Sound_extractIntervals);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Extract intervals where...", 0, praat_DEPTH_1, DO_TextGrid_Sound_extractIntervalsWhere);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Modify TextGrid", 0, 0, 0);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Scale times", 0, 0, DO_TextGrid_Sound_scaleTimes);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Modify Sound", 0, 0, 0);
	praat_addAction2 (classSound, 1, classTextGrid, 1, "Clone time domain", 0, 0, DO_TextGrid_Sound_cloneTimeDomain);
	praat_addAction2 (classSpellingChecker, 1, classWordList, 1, "Replace WordList", 0, 0, DO_SpellingChecker_replaceWordList);
	praat_addAction2 (classSpellingChecker, 1, classSortedSetOfString, 1, "Replace user dictionary", 0, 0, DO_SpellingChecker_replaceUserDictionary);
	praat_addAction2 (classSpellingChecker, 1, classStrings, 1, "Replace word list?", 0, 0, DO_SpellingChecker_replaceWordList_help);
	praat_addAction2 (classSpellingChecker, 1, classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_SpellingChecker_edit);
	praat_addAction2 (classTextGrid, 1, classTextTier, 1, "Append", 0, 0, DO_TextGrid_AnyTier_append);
	praat_addAction2 (classTextGrid, 1, classIntervalTier, 1, "Append", 0, 0, DO_TextGrid_AnyTier_append);

	praat_addAction3 (classLongSound, 1, classSpellingChecker, 1, classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_LongSound_SpellingChecker_edit);
	praat_addAction3 (classSound, 1, classSpellingChecker, 1, classTextGrid, 1, "Edit", 0, 0, DO_TextGrid_SpellingChecker_edit);
}

/* End of file praat_TextGrid_init.c */
