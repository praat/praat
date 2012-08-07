/* TextGridEditor.cpp
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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
/* Erez Volk added FLAC support in 2007 */

#include "TextGridEditor.h"
#include "EditorM.h"
#include "SoundEditor.h"
#include "Sound_and_Spectrogram.h"
#include "TextGrid_Sound.h"

#include "enums_getText.h"
#include "TextGridEditor_enums.h"
#include "enums_getValue.h"
#include "TextGridEditor_enums.h"

Thing_implement (TextGridEditor, TimeSoundAnalysisEditor, 0);

/********** PREFERENCES **********/

/*
 * If you change any of the following, you may want to raise a version number in TextGridEditor_prefs ().
 */
#define TextGridEditor_DEFAULT_USE_TEXT_STYLES  false
#define TextGridEditor_DEFAULT_FONT_SIZE  18
	#define TextGridEditor_DEFAULT_FONT_SIZE_STRING  L"18"
#define TextGridEditor_DEFAULT_SHIFT_DRAG_MULTIPLE  true
#define TextGridEditor_DEFAULT_GREEN_STRING  L"some text here for green paint"

static struct {
	bool useTextStyles, shiftDragMultiple;
	int fontSize;
	enum kGraphics_horizontalAlignment alignment;
	enum kTextGridEditor_showNumberOf showNumberOf;
	enum kMelder_string greenMethod;
	wchar greenString [Preferences_STRING_BUFFER_SIZE];
	struct {
		bool showBoundaries;
		bool garnish;
		struct {
			bool speckle;
		} pitch;
	} picture;
}
	preferences;

void TextGridEditor_prefs (void) {
	Preferences_addBool (L"TextGridEditor.useTextStyles", & preferences.useTextStyles, TextGridEditor_DEFAULT_USE_TEXT_STYLES);
	Preferences_addInt (L"TextGridEditor.fontSize2", & preferences.fontSize, TextGridEditor_DEFAULT_FONT_SIZE);
	Preferences_addEnum (L"TextGridEditor.alignment", & preferences.alignment, kGraphics_horizontalAlignment, DEFAULT);
	Preferences_addBool (L"TextGridEditor.shiftDragMultiple2", & preferences.shiftDragMultiple, TextGridEditor_DEFAULT_SHIFT_DRAG_MULTIPLE);
	Preferences_addEnum (L"TextGridEditor.showNumberOf2", & preferences.showNumberOf, kTextGridEditor_showNumberOf, DEFAULT);
	Preferences_addEnum (L"TextGridEditor.greenMethod", & preferences.greenMethod, kMelder_string, DEFAULT);
	Preferences_addString (L"TextGridEditor.greenString", & preferences.greenString [0], TextGridEditor_DEFAULT_GREEN_STRING);
	Preferences_addBool (L"TextGridEditor.picture.showBoundaries", & preferences.picture.showBoundaries, true);
	Preferences_addBool (L"TextGridEditor.picture.garnish", & preferences.picture.garnish, true);
	Preferences_addBool (L"TextGridEditor.picture.pitch.speckle", & preferences.picture.pitch.speckle, false);
}

void structTextGridEditor :: v_info () {
	TextGridEditor_Parent :: v_info ();
	MelderInfo_writeLine2 (L"Selected tier: ", Melder_integer (selectedTier));
	MelderInfo_writeLine2 (L"TextGrid uses text styles: ", Melder_boolean (useTextStyles));
	MelderInfo_writeLine2 (L"TextGrid font size: ", Melder_integer (fontSize));
	MelderInfo_writeLine2 (L"TextGrid alignment: ", kGraphics_horizontalAlignment_getText (alignment));
}

/********** UTILITIES **********/

static double _TextGridEditor_computeSoundY (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	int numberOfTiers = grid -> tiers -> size;
	bool showAnalysis = my v_hasAnalysis () && (my spectrogram.show || my pitch.show || my intensity.show || my formant.show) && (my d_longSound.data || my d_sound.data);
	int numberOfVisibleChannels = my d_sound.data ? (my d_sound.data -> ny > 8 ? 8 : my d_sound.data -> ny) :
		my d_longSound.data ? (my d_longSound.data -> numberOfChannels > 8 ? 8 : my d_longSound.data -> numberOfChannels) : 1;
	return my d_sound.data || my d_longSound.data ? numberOfTiers / (2.0 * numberOfVisibleChannels + numberOfTiers * (showAnalysis ? 1.8 : 1.3)) : 1.0;
}

static void _AnyTier_identifyClass (Function anyTier, IntervalTier *intervalTier, TextTier *textTier) {
	if (anyTier -> classInfo == classIntervalTier) {
		*intervalTier = (IntervalTier) anyTier;
		*textTier = NULL;
	} else {
		*intervalTier = NULL;
		*textTier = (TextTier) anyTier;
	}
}

static int _TextGridEditor_yWCtoTier (TextGridEditor me, double yWC) {
	TextGrid grid = (TextGrid) my data;
	int ntier = grid -> tiers -> size;
	double soundY = _TextGridEditor_computeSoundY (me);
	int itier = ntier - (int) floor (yWC / soundY * (double) ntier);
	if (itier < 1) itier = 1; if (itier > ntier) itier = ntier;
	return itier;
}

static void _TextGridEditor_timeToInterval (TextGridEditor me, double t, int itier, double *tmin, double *tmax) {
	TextGrid grid = (TextGrid) my data;
	IntervalTier intervalTier;
	TextTier textTier;
	_AnyTier_identifyClass ((Function) grid -> tiers -> item [itier], & intervalTier, & textTier);
	if (intervalTier) {
		long iinterval = IntervalTier_timeToIndex (intervalTier, t);
		TextInterval interval;
		if (iinterval == 0) {
			if (t < my tmin) {
				iinterval = 1;
			} else {
				iinterval = intervalTier -> intervals -> size;
			}
		}
		interval = (TextInterval) intervalTier -> intervals -> item [iinterval];
		*tmin = interval -> xmin;
		*tmax = interval -> xmax;
	} else {
		long n = textTier -> points -> size;
		if (n == 0) {
			*tmin = my tmin;
			*tmax = my tmax;
		} else {
			long ipointleft = AnyTier_timeToLowIndex (textTier, t);
			*tmin = ipointleft == 0 ? my tmin : ((TextPoint) textTier -> points -> item [ipointleft]) -> number;
			*tmax = ipointleft == n ? my tmax : ((TextPoint) textTier -> points -> item [ipointleft + 1]) -> number;
		}
	}
	if (*tmin < my tmin) *tmin = my tmin;   /* Clip by FunctionEditor's time domain. */
	if (*tmax > my tmax) *tmax = my tmax;
}

static void checkTierSelection (TextGridEditor me, const wchar_t *verbPhrase) {
	TextGrid grid = (TextGrid) my data;
	if (my selectedTier < 1 || my selectedTier > grid -> tiers -> size)
		Melder_throw ("To ", verbPhrase, ", first select a tier by clicking anywhere inside it.");
}

static long getSelectedInterval (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	IntervalTier tier = (IntervalTier) grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> classInfo == classIntervalTier);
	return IntervalTier_timeToIndex (tier, my startSelection);
}

static long getSelectedLeftBoundary (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	IntervalTier tier = (IntervalTier) grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> classInfo == classIntervalTier);
	return IntervalTier_hasBoundary (tier, my startSelection);
}

static long getSelectedPoint (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	TextTier tier = (TextTier) grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> classInfo == classTextTier);
	return AnyTier_hasPoint (tier, my startSelection);
}

static void scrollToView (TextGridEditor me, double t) {
	if (t <= my startWindow) {
		FunctionEditor_shift (me, t - my startWindow - 0.618 * (my endWindow - my startWindow));
	} else if (t >= my endWindow) {
		FunctionEditor_shift (me, t - my endWindow + 0.618 * (my endWindow - my startWindow));
	} else {
		FunctionEditor_marksChanged (me);
	}
}

/********** METHODS **********/

/*
 * The main invariant of the TextGridEditor is that the selected interval
 * always has the cursor in it, and that the cursor always selects an interval
 * if the selected tier is an interval tier.
 */

/***** FILE MENU *****/

static void menu_cb_ExtractSelectedTextGrid_preserveTimes (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my endSelection <= my startSelection) Melder_throw ("No selection.");
	autoTextGrid extract = TextGrid_extractPart ((TextGrid) my data, my startSelection, my endSelection, true);
	my broadcastPublication (extract.transfer());
}

static void menu_cb_ExtractSelectedTextGrid_timeFromZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my endSelection <= my startSelection) Melder_throw ("No selection.");
	autoTextGrid extract = TextGrid_extractPart ((TextGrid) my data, my startSelection, my endSelection, false);
	my broadcastPublication (extract.transfer());
}

void structTextGridEditor :: v_createMenuItems_file_extract (EditorMenu menu) {
	TextGridEditor_Parent :: v_createMenuItems_file_extract (menu);
	extractSelectedTextGridPreserveTimesButton =
		EditorMenu_addCommand (menu, L"Extract selected TextGrid (preserve times)", 0, menu_cb_ExtractSelectedTextGrid_preserveTimes);
	extractSelectedTextGridTimeFromZeroButton =
		EditorMenu_addCommand (menu, L"Extract selected TextGrid (time from 0)", 0, menu_cb_ExtractSelectedTextGrid_timeFromZero);
}

static void menu_cb_WriteToTextFile (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM_WRITE (L"Save as TextGrid text file", 0)
		swprintf (defaultName, 300, L"%ls.TextGrid", ((Thing) my data) -> name);
	EDITOR_DO_WRITE
		Data_writeToTextFile (my data, file);
	EDITOR_END
}

void structTextGridEditor :: v_createMenuItems_file_write (EditorMenu menu) {
	TextGridEditor_Parent :: v_createMenuItems_file_write (menu);
	EditorMenu_addCommand (menu, L"Save TextGrid as text file...", 'S', menu_cb_WriteToTextFile);
}

static void menu_cb_DrawVisibleTextGrid (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Draw visible TextGrid", 0)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (L"Garnish", preferences.picture.garnish);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		preferences.picture.garnish = GET_INTEGER (L"Garnish");
		Editor_openPraatPicture (me);
		TextGrid_Sound_draw ((TextGrid) my data, NULL, my pictureGraphics, my startWindow, my endWindow, true, my useTextStyles,
			preferences.picture.garnish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_DrawVisibleSoundAndTextGrid (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Draw visible sound and TextGrid", 0)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (L"Garnish", preferences.picture.garnish);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		preferences.picture.garnish = GET_INTEGER (L"Garnish");
		Editor_openPraatPicture (me);
		Sound publish = my d_longSound.data ?
			LongSound_extractPart (my d_longSound.data, my startWindow, my endWindow, true) :
			Sound_extractPart (my d_sound.data, my startWindow, my endWindow, kSound_windowShape_RECTANGULAR, 1.0, true);
		TextGrid_Sound_draw ((TextGrid) my data, publish, my pictureGraphics, my startWindow, my endWindow, true, my useTextStyles, preferences.picture.garnish);
		forget (publish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

void structTextGridEditor :: v_createMenuItems_file_draw (EditorMenu menu) {
	TextGridEditor_Parent :: v_createMenuItems_file_draw (menu);
	EditorMenu_addCommand (menu, L"Draw visible TextGrid...", 0, menu_cb_DrawVisibleTextGrid);
	if (d_sound.data || d_longSound.data)
		EditorMenu_addCommand (menu, L"Draw visible sound and TextGrid...", 0, menu_cb_DrawVisibleSoundAndTextGrid);
}

/***** EDIT MENU *****/

#ifndef macintosh
static void menu_cb_Cut (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	GuiText_cut (my text);
}
static void menu_cb_Copy (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	GuiText_copy (my text);
}
static void menu_cb_Paste (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	GuiText_paste (my text);
}
static void menu_cb_Erase (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	GuiText_remove (my text);
}
#endif

static void menu_cb_Genericize (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	Editor_save (me, L"Convert to Backslash Trigraphs");
	TextGrid_genericize ((TextGrid) my data);
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_Nativize (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	Editor_save (me, L"Convert to Unicode");
	TextGrid_nativize ((TextGrid) my data);
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

/***** QUERY MENU *****/

static void menu_cb_GetStartingPointOfInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, L"query the starting point of an interval");
	Function anyTier = (Function) grid -> tiers -> item [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		double time = iinterval < 1 || iinterval > tier -> intervals -> size ? NUMundefined :
			((TextInterval) tier -> intervals -> item [iinterval]) -> xmin;
		Melder_informationReal (time, L"seconds");
	} else {
		Melder_throw ("The selected tier is not an interval tier.");
	}
}

static void menu_cb_GetEndPointOfInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, L"query the end point of an interval");
	Function anyTier = (Function) grid -> tiers -> item [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		double time = iinterval < 1 || iinterval > tier -> intervals -> size ? NUMundefined :
			((TextInterval) tier -> intervals -> item [iinterval]) -> xmax;
		Melder_informationReal (time, L"seconds");
	} else {
		Melder_throw ("The selected tier is not an interval tier.");
	}
}

static void menu_cb_GetLabelOfInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, L"query the label of an interval");
	Function anyTier = (Function) grid -> tiers -> item [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		const wchar_t *label = iinterval < 1 || iinterval > tier -> intervals -> size ? L"" :
			((TextInterval) tier -> intervals -> item [iinterval]) -> text;
		Melder_information (label);
	} else {
		Melder_throw ("The selected tier is not an interval tier.");
	}
}

/***** VIEW MENU *****/

static void do_selectAdjacentTier (TextGridEditor me, bool previous) {
	TextGrid grid = (TextGrid) my data;
	long n = grid -> tiers -> size;
	if (n >= 2) {
		my selectedTier = previous ?
			my selectedTier > 1 ? my selectedTier - 1 : n :
			my selectedTier < n ? my selectedTier + 1 : 1;
		_TextGridEditor_timeToInterval (me, my startSelection, my selectedTier, & my startSelection, & my endSelection);
		FunctionEditor_marksChanged (me);
	}
}

static void menu_cb_SelectPreviousTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentTier (me, true);
}

static void menu_cb_SelectNextTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentTier (me, false);
}

static void do_selectAdjacentInterval (TextGridEditor me, bool previous, bool shift) {
	TextGrid grid = (TextGrid) my data;
	IntervalTier intervalTier;
	TextTier textTier;
	if (my selectedTier < 1 || my selectedTier > grid -> tiers -> size) return;
	_AnyTier_identifyClass ((Function) grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);
	if (intervalTier) {
		long n = intervalTier -> intervals -> size;
		if (n >= 2) {
			TextInterval interval;
			long iinterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
			if (shift) {
				long binterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
				long einterval = IntervalTier_timeToIndex (intervalTier, my endSelection);
				if (my endSelection == intervalTier -> xmax) einterval ++;
				if (binterval < iinterval && einterval > iinterval + 1) {
					interval = (TextInterval) intervalTier -> intervals -> item [iinterval];
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
				} else if (previous) {
					if (einterval > iinterval + 1) {
						if (einterval <= n + 1) {
							interval = (TextInterval) intervalTier -> intervals -> item [einterval - 1];
							my endSelection = interval -> xmin;
						}
					} else if (binterval > 1) {
						interval = (TextInterval) intervalTier -> intervals -> item [binterval - 1];
						my startSelection = interval -> xmin;
					}
				} else {
					if (binterval < iinterval) {
						if (binterval > 0) {
							interval = (TextInterval) intervalTier -> intervals -> item [binterval];
							my startSelection = interval -> xmax;
						}
					} else if (einterval <= n) {
						interval = (TextInterval) intervalTier -> intervals -> item [einterval];
						my endSelection = interval -> xmax;
					}
				}
			} else {
				iinterval = previous ?
					iinterval > 1 ? iinterval - 1 : n :
					iinterval < n ? iinterval + 1 : 1;
				interval = (TextInterval) intervalTier -> intervals -> item [iinterval];
				my startSelection = interval -> xmin;
				my endSelection = interval -> xmax;
			}
			scrollToView (me, iinterval == n ? my startSelection : iinterval == 1 ? my endSelection : (my startSelection + my endSelection) / 2);
		}
	} else {
		long n = textTier -> points -> size;
		if (n >= 2) {
			TextPoint point;
			long ipoint = AnyTier_timeToHighIndex (textTier, my startSelection);
			ipoint = previous ?
				ipoint > 1 ? ipoint - 1 : n :
				ipoint < n ? ipoint + 1 : 1;
			point = (TextPoint) textTier -> points -> item [ipoint];
			my startSelection = my endSelection = point -> number;
			scrollToView (me, my startSelection);
		}
	}
}

static void menu_cb_SelectPreviousInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentInterval (me, true, false);
}

static void menu_cb_SelectNextInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentInterval (me, false, false);
}

static void menu_cb_ExtendSelectPreviousInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentInterval (me, true, true);
}

static void menu_cb_ExtendSelectNextInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentInterval (me, false, true);
}

static void menu_cb_MoveBtoZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	double zero = Sound_getNearestZeroCrossing (my d_sound.data, my startSelection, 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my startSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (me);
	}
}

static void menu_cb_MoveCursorToZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	double zero = Sound_getNearestZeroCrossing (my d_sound.data, 0.5 * (my startSelection + my endSelection), 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my startSelection = my endSelection = zero;
		FunctionEditor_marksChanged (me);
	}
}

static void menu_cb_MoveEtoZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	double zero = Sound_getNearestZeroCrossing (my d_sound.data, my endSelection, 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my endSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (me);
	}
}

/***** PITCH MENU *****/

static void menu_cb_DrawTextGridAndPitch (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Draw TextGrid and Pitch separately", 0)
		my v_form_pictureWindow (cmd);
		LABEL (L"", L"TextGrid:")
		BOOLEAN (L"Show boundaries and points", 1);
		LABEL (L"", L"Pitch:")
		BOOLEAN (L"Speckle", 0);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_INTEGER (L"Show boundaries and points", preferences.picture.showBoundaries);
		SET_INTEGER (L"Speckle", preferences.picture.pitch.speckle);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (L"Garnish", preferences.picture.garnish);
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		preferences.picture.showBoundaries = GET_INTEGER (L"Show boundaries and points");
		preferences.picture.pitch.speckle = GET_INTEGER (L"Speckle");
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		preferences.picture.garnish = GET_INTEGER (L"Garnish");
		if (! my pitch.show)
			Melder_throw ("No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my pitch.data) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my pitch.data) Melder_throw ("Cannot compute pitch.");
		}
		Editor_openPraatPicture (me);
		double pitchFloor_hidden = Function_convertStandardToSpecialUnit (my pitch.data, my pitch.floor, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (my pitch.data, my pitch.ceiling, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchFloor_overt = Function_convertToNonlogarithmic (my pitch.data, pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchCeiling_overt = Function_convertToNonlogarithmic (my pitch.data, pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchViewFrom_overt = my pitch.viewFrom < my pitch.viewTo ? my pitch.viewFrom : pitchFloor_overt;
		double pitchViewTo_overt = my pitch.viewFrom < my pitch.viewTo ? my pitch.viewTo : pitchCeiling_overt;
		TextGrid_Pitch_drawSeparately ((TextGrid) my data, my pitch.data, my pictureGraphics, my startWindow, my endWindow,
			pitchViewFrom_overt, pitchViewTo_overt, GET_INTEGER (L"Show boundaries and points"), my useTextStyles, GET_INTEGER (L"Garnish"),
			GET_INTEGER (L"Speckle"), my pitch.unit);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

/***** INTERVAL MENU *****/

static void insertBoundaryOrPoint (TextGridEditor me, int itier, double t1, double t2, bool insertSecond) {
	TextGrid grid = (TextGrid) my data;
	IntervalTier intervalTier;
	TextTier textTier;
	int ntiers = grid -> tiers -> size;
	if (itier < 1 || itier > ntiers)
		Melder_throw ("No tier ", itier, ".");
	_AnyTier_identifyClass ((Function) grid -> tiers -> item [itier], & intervalTier, & textTier);
	Melder_assert (t1 <= t2);

	if (intervalTier) {
		TextInterval rightNewInterval = NULL, midNewInterval = NULL;
		bool t1IsABoundary = IntervalTier_hasTime (intervalTier, t1);
		bool t2IsABoundary = IntervalTier_hasTime (intervalTier, t2);
		if (t1 == t2 && t1IsABoundary)
			Melder_throw ("Cannot add a boundary at ", Melder_fixed (t1, 6), " seconds, because there is already a boundary there.");
		if (t1IsABoundary && t2IsABoundary)
			Melder_throw ("Cannot add boundaries at ", Melder_fixed (t1, 6), " and ", Melder_fixed (t2, 6), " seconds, because there are already boundaries there.");
		long iinterval = IntervalTier_timeToIndex (intervalTier, t1);
		//Melder_casual ("iinterval %ld, t = %f", iinterval, t1);
		long iinterval2 = t1 == t2 ? iinterval : IntervalTier_timeToIndex (intervalTier, t2);
		//Melder_casual ("iinterval2 %ld, t = %f", iinterval2, t2);
		if (iinterval == 0 || iinterval2 == 0)
			Melder_throw ("The selection is outside the time domain of the intervals.");
		long correctedIinterval2 = t2IsABoundary && iinterval2 == intervalTier -> intervals -> size ? iinterval2 + 1 : iinterval2;
		if (correctedIinterval2 > iinterval + 1 || (correctedIinterval2 > iinterval && ! t2IsABoundary))
			Melder_throw ("The selection straddles a boundary.");
		TextInterval interval = (TextInterval) intervalTier -> intervals -> item [iinterval];

		if (t1 == t2) {
			Editor_save (me, L"Add boundary");
		} else {
			Editor_save (me, L"Add interval");
		}

		if (itier == my selectedTier) {
			/*
			 * Divide up the label text into left, mid and right, depending on where the text selection is.
			 */
			long left, right;
			wchar_t *text = GuiText_getStringAndSelectionPosition (my text, & left, & right);
			rightNewInterval = TextInterval_create (t2, interval -> xmax, text + right);
			text [right] = '\0';
			midNewInterval = TextInterval_create (t1, t2, text + left);
			text [left] = '\0';
			TextInterval_setText (interval, text);
			Melder_free (text);
		} else {
			/*
			 * Move the text to the left of the boundary.
			 */
			rightNewInterval = TextInterval_create (t2, interval -> xmax, L"");
			midNewInterval = TextInterval_create (t1, t2, L"");
		}
		if (t1IsABoundary) {
			/*
			 * Merge mid with left interval.
			 */
			if (interval -> xmin != t1)
				Melder_fatal ("Boundary unequal: %.17g versus %.17g.", interval -> xmin, t1);
			interval -> xmax = t2;
			TextInterval_setText (interval, Melder_wcscat (interval -> text, midNewInterval -> text));
			forget (midNewInterval);
		} else if (t2IsABoundary) {
			/*
			 * Merge mid and right interval.
			 */
			if (interval -> xmax != t2)
				Melder_fatal ("Boundary unequal: %.17g versus %.17g.", interval -> xmax, t2);
			interval -> xmax = t1;
			Melder_assert (rightNewInterval -> xmin == t2);
			Melder_assert (rightNewInterval -> xmax == t2);
			rightNewInterval -> xmin = t1;
			TextInterval_setText (rightNewInterval, Melder_wcscat (midNewInterval -> text, rightNewInterval -> text));
			forget (midNewInterval);
		} else {
			interval -> xmax = t1;
			if (t1 != t2) Collection_addItem (intervalTier -> intervals, midNewInterval);
		}
		Collection_addItem (intervalTier -> intervals, rightNewInterval);
		if (insertSecond && ntiers >= 2) {
			/*
			 * Find the last time before t on another tier.
			 */
			double tlast = interval -> xmin, tmin, tmax;
			for (int jtier = 1; jtier <= ntiers; jtier ++) if (jtier != itier) {
				_TextGridEditor_timeToInterval (me, t1, jtier, & tmin, & tmax);
				if (tmin > tlast) {
					tlast = tmin;
				}
			}
			if (tlast > interval -> xmin && tlast < t1) {
				TextInterval newInterval = TextInterval_create (tlast, t1, L"");
				interval -> xmax = tlast;
				Collection_addItem (intervalTier -> intervals, newInterval);
			}
		}
	} else {
		TextPoint newPoint;
		if (AnyTier_hasPoint (textTier, t1))
			Melder_throw ("Cannot add a point at ", Melder_fixed (t1, 6), " seconds, because there is already a point there.");

		Editor_save (me, L"Add point");

		newPoint = TextPoint_create (t1, L"");
		Collection_addItem (textTier -> points, newPoint);
	}
	my startSelection = my endSelection = t1;
}

static void do_insertIntervalOnTier (TextGridEditor me, int itier) {
	try {
		insertBoundaryOrPoint (me, itier,
			my playingCursor || my playingSelection ? my playCursor : my startSelection,
			my playingCursor || my playingSelection ? my playCursor : my endSelection,
			true);
		my selectedTier = itier;
		FunctionEditor_marksChanged (me);
		my broadcastDataChanged ();
	} catch (MelderError) {
		Melder_throw ("Interval not inserted.");
	}
}

static void menu_cb_InsertIntervalOnTier1 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 1); }
static void menu_cb_InsertIntervalOnTier2 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 2); }
static void menu_cb_InsertIntervalOnTier3 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 3); }
static void menu_cb_InsertIntervalOnTier4 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 4); }
static void menu_cb_InsertIntervalOnTier5 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 5); }
static void menu_cb_InsertIntervalOnTier6 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 6); }
static void menu_cb_InsertIntervalOnTier7 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 7); }
static void menu_cb_InsertIntervalOnTier8 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 8); }

/***** BOUNDARY/POINT MENU *****/

static void menu_cb_RemovePointOrBoundary (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, L"remove a point or boundary");
	Function anyTier = (Function) grid -> tiers -> item [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (! selectedLeftBoundary) Melder_throw ("To remove a boundary, first click on it.");

		Editor_save (me, L"Remove boundary");
		IntervalTier_removeLeftBoundary (tier, selectedLeftBoundary);
	} else {
		TextTier tier = (TextTier) anyTier;
		long selectedPoint = getSelectedPoint (me);
		if (! selectedPoint) Melder_throw ("To remove a point, first click on it.");

		Editor_save (me, L"Remove point");
		Collection_removeItem (tier -> points, selectedPoint);
	}
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void do_movePointOrBoundary (TextGridEditor me, int where) {
	double position;
	TextGrid grid = (TextGrid) my data;
	if (where == 0 && my d_sound.data == NULL) return;
	checkTierSelection (me, L"move a point or boundary");
	Function anyTier = (Function) grid -> tiers -> item [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		static const wchar_t *boundarySaveText [3] = { L"Move boundary to zero crossing", L"Move boundary to B", L"Move boundary to E" };
		TextInterval left, right;
		long selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (! selectedLeftBoundary)
			Melder_throw ("To move a boundary, first click on it.");
		left = (TextInterval) tier -> intervals -> item [selectedLeftBoundary - 1];
		right = (TextInterval) tier -> intervals -> item [selectedLeftBoundary];
		position = where == 1 ? my startSelection : where == 2 ? my endSelection :
			Sound_getNearestZeroCrossing (my d_sound.data, left -> xmax, 1);   // STEREO BUG
		if (position == NUMundefined)
			Melder_throw ("There is no zero crossing to move to.");
		if (position <= left -> xmin || position >= right -> xmax)
			Melder_throw ("Cannot move a boundary past its neighbour.");

		Editor_save (me, boundarySaveText [where]);

		left -> xmax = right -> xmin = my startSelection = my endSelection = position;
	} else {
		TextTier tier = (TextTier) anyTier;
		static const wchar_t *pointSaveText [3] = { L"Move point to zero crossing", L"Move point to B", L"Move point to E" };
		TextPoint point;
		long selectedPoint = getSelectedPoint (me);
		if (! selectedPoint)
			Melder_throw ("To move a point, first click on it.");
		point = (TextPoint) tier -> points -> item [selectedPoint];
		position = where == 1 ? my startSelection : where == 2 ? my endSelection :
			Sound_getNearestZeroCrossing (my d_sound.data, point -> number, 1);   // STEREO BUG
		if (position == NUMundefined)
			Melder_throw ("There is no zero crossing to move to.");

		Editor_save (me, pointSaveText [where]);

		point -> number = my startSelection = my endSelection = position;
	}
	FunctionEditor_marksChanged (me);   // because cursor has moved
	my broadcastDataChanged ();
}

static void menu_cb_MoveToB (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_movePointOrBoundary (me, 1);
}

static void menu_cb_MoveToE (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_movePointOrBoundary (me, 2);
}

static void menu_cb_MoveToZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_movePointOrBoundary (me, 0);
}

static void do_insertOnTier (TextGridEditor me, int itier) {
	try {
		insertBoundaryOrPoint (me, itier,
			my playingCursor || my playingSelection ? my playCursor : my startSelection,
			my playingCursor || my playingSelection ? my playCursor : my endSelection,
			false);
		my selectedTier = itier;
		FunctionEditor_marksChanged (me);
		my broadcastDataChanged ();
	} catch (MelderError) {
		Melder_throw ("Boundary or point not inserted.");
	}
}

static void menu_cb_InsertOnSelectedTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_insertOnTier (me, my selectedTier);
}

static void menu_cb_InsertOnTier1 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 1); }
static void menu_cb_InsertOnTier2 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 2); }
static void menu_cb_InsertOnTier3 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 3); }
static void menu_cb_InsertOnTier4 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 4); }
static void menu_cb_InsertOnTier5 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 5); }
static void menu_cb_InsertOnTier6 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 6); }
static void menu_cb_InsertOnTier7 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 7); }
static void menu_cb_InsertOnTier8 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 8); }

static void menu_cb_InsertOnAllTiers (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = (TextGrid) my data;
	int saveTier = my selectedTier;
	for (int itier = 1; itier <= grid -> tiers -> size; itier ++) {
		do_insertOnTier (me, itier);
	}
	my selectedTier = saveTier;   // only if everything went right; otherwise, the tier where something went wrong will stand selected
}

/***** SEARCH MENU *****/

static void findInTier (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, L"find a text");
	Function anyTier = (Function) grid -> tiers -> item [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals -> size) {
			TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
			wchar_t *text = interval -> text;
			if (text) {
				wchar_t *position = wcsstr (text, my findString);
				if (position) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					scrollToView (me, my startSelection);
					GuiText_setSelection (my text, position - text, position - text + wcslen (my findString));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals -> size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		long ipoint = AnyTier_timeToLowIndex (tier, my startSelection) + 1;
		while (ipoint <= tier -> points -> size) {
			TextPoint point = (TextPoint) tier -> points -> item [ipoint];
			wchar_t *text = point -> mark;
			if (text) {
				wchar_t *position = wcsstr (text, my findString);
				if (position) {
					my startSelection = my endSelection = point -> number;
					scrollToView (me, point -> number);
					GuiText_setSelection (my text, position - text, position - text + wcslen (my findString));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points -> size)
			Melder_beep ();
	}
}

static void do_find (TextGridEditor me) {
	if (my findString) {
		long left, right;
		autostring label = GuiText_getStringAndSelectionPosition (my text, & left, & right);
		wchar *position = wcsstr (label.peek() + right, my findString);   // CRLF BUG?
		if (position) {
			GuiText_setSelection (my text, position - label.peek(), position - label.peek() + wcslen (my findString));
		} else {
			findInTier (me);
		}
	}
}

static void menu_cb_Find (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Find text", 0)
		LABEL (L"", L"Text:")
		TEXTFIELD (L"string", L"")
	EDITOR_OK
	EDITOR_DO
		Melder_free (my findString);
		my findString = Melder_wcsdup_f (GET_STRING (L"string"));
		do_find (me);
	EDITOR_END
}

static void menu_cb_FindAgain (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_find (me);
}

static void checkSpellingInTier (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, L"check spelling");
	Function anyTier = (Function) grid -> tiers -> item [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals -> size) {
			TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
			wchar_t *text = interval -> text;
			if (text) {
				long position = 0;
				wchar_t *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					scrollToView (me, my startSelection);
					GuiText_setSelection (my text, position, position + wcslen (notAllowed));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals -> size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		long ipoint = AnyTier_timeToLowIndex (tier, my startSelection) + 1;
		while (ipoint <= tier -> points -> size) {
			TextPoint point = (TextPoint) tier -> points -> item [ipoint];
			wchar_t *text = point -> mark;
			if (text) {
				long position = 0;
				wchar_t *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = my endSelection = point -> number;
					scrollToView (me, point -> number);
					GuiText_setSelection (my text, position, position + wcslen (notAllowed));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points -> size)
			Melder_beep ();
	}
}

static void menu_cb_CheckSpelling (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my spellingChecker) {
		long left, right;
		autostring label = GuiText_getStringAndSelectionPosition (my text, & left, & right);
		long position = right;
		wchar *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.peek(), & position);
		if (notAllowed) {
			GuiText_setSelection (my text, position, position + wcslen (notAllowed));
		} else {
			checkSpellingInTier (me);
		}
	}
}

static void menu_cb_CheckSpellingInInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my spellingChecker) {
		long left, right;
		autostring label = GuiText_getStringAndSelectionPosition (my text, & left, & right);
		long position = right;
		wchar *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.peek(), & position);
		if (notAllowed) {
			GuiText_setSelection (my text, position, position + wcslen (notAllowed));
		}
	}
}

static void menu_cb_AddToUserDictionary (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my spellingChecker) {
		autostring word = GuiText_getSelection (my text);
		SpellingChecker_addNewWord (my spellingChecker, word.peek());
		my broadcastDataChanged ();
	}
}

/***** TIER MENU *****/

static void menu_cb_RenameTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Rename tier", 0)
		SENTENCE (L"Name", L"");
	EDITOR_OK
		TextGrid grid = (TextGrid) my data;
		checkTierSelection (me, L"rename a tier");
		Data tier = (Data) grid -> tiers -> item [my selectedTier];
		SET_STRING (L"Name", tier -> name ? tier -> name : L"")
	EDITOR_DO
		TextGrid grid = (TextGrid) my data;
		checkTierSelection (me, L"rename a tier");
		Function tier = (Function) grid -> tiers -> item [my selectedTier];

		Editor_save (me, L"Rename tier");

		wchar *newName = GET_STRING (L"Name");
		Thing_setName (tier, newName);

		FunctionEditor_redraw (me);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_PublishTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, L"publish a tier");
	Function tier = (Function) grid -> tiers -> item [my selectedTier];
	autoTextGrid publish = TextGrid_createWithoutTiers (1e30, -1e30);
	TextGrid_addTier (publish.peek(), tier);
	Thing_setName (publish.peek(), tier -> name);
	my broadcastPublication (publish.transfer());
}

static void menu_cb_RemoveAllTextFromTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, L"remove all text from a tier");
	IntervalTier intervalTier;
	TextTier textTier;
	_AnyTier_identifyClass ((Function) grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);

	Editor_save (me, L"Remove text from tier");
	if (intervalTier) {
		IntervalTier_removeText (intervalTier);
	} else {
		TextTier_removeText (textTier);
	}

	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_RemoveTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = (TextGrid) my data;
	if (grid -> tiers -> size <= 1) {
		Melder_throw ("Sorry, I refuse to remove the last tier.");
	}
	checkTierSelection (me, L"remove a tier");

	Editor_save (me, L"Remove tier");
	Collection_removeItem (grid -> tiers, my selectedTier);

	my selectedTier = 1;
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_AddIntervalTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Add interval tier", 0)
		NATURAL (L"Position", L"1 (= at top)")
		SENTENCE (L"Name", L"")
	EDITOR_OK
		TextGrid grid = (TextGrid) my data;
		static MelderString text = { 0 };
		MelderString_empty (& text);
		MelderString_append (& text, Melder_integer (grid -> tiers -> size + 1), L" (= at bottom)");
		SET_STRING (L"Position", text.string)
		SET_STRING (L"Name", L"")
	EDITOR_DO
		TextGrid grid = (TextGrid) my data;
		int position = GET_INTEGER (L"Position");
		wchar_t *name = GET_STRING (L"Name");
		autoIntervalTier tier = IntervalTier_create (grid -> xmin, grid -> xmax);
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (tier.peek(), name);

		Editor_save (me, L"Add interval tier");
		Ordered_addItemPos (grid -> tiers, tier.transfer(), position);

		my selectedTier = position;
		FunctionEditor_updateText (me);
		FunctionEditor_redraw (me);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_AddPointTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Add point tier", 0)
		NATURAL (L"Position", L"1 (= at top)")
		SENTENCE (L"Name", L"");
	EDITOR_OK
		TextGrid grid = (TextGrid) my data;
		static MelderString text = { 0 };
		MelderString_empty (& text);
		MelderString_append (& text, Melder_integer (grid -> tiers -> size + 1), L" (= at bottom)");
		SET_STRING (L"Position", text.string)
		SET_STRING (L"Name", L"")
	EDITOR_DO
		TextGrid grid = (TextGrid) my data;
		int position = GET_INTEGER (L"Position");
		wchar_t *name = GET_STRING (L"Name");
		autoTextTier tier = TextTier_create (grid -> xmin, grid -> xmax);
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (tier.peek(), name);

		Editor_save (me, L"Add point tier");
		Ordered_addItemPos (grid -> tiers, tier.transfer(), position);

		my selectedTier = position;
		FunctionEditor_updateText (me);
		FunctionEditor_redraw (me);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_DuplicateTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Duplicate tier", 0)
		NATURAL (L"Position", L"1 (= at top)")
		SENTENCE (L"Name", L"")
	EDITOR_OK
		TextGrid grid = (TextGrid) my data;
		if (my selectedTier) {
			SET_STRING (L"Position", Melder_integer (my selectedTier + 1))
			SET_STRING (L"Name", ((AnyTier) grid -> tiers -> item [my selectedTier]) -> name)
		}
	EDITOR_DO
		TextGrid grid = (TextGrid) my data;
		int position = GET_INTEGER (L"Position");
		wchar *name = GET_STRING (L"Name");
		checkTierSelection (me, L"duplicate a tier");
		AnyTier tier = (AnyTier) grid -> tiers -> item [my selectedTier];
		autoAnyTier newTier = Data_copy (tier);
		if (position > grid -> tiers -> size)
			position = grid -> tiers -> size + 1;
		Thing_setName (newTier.peek(), name);

		Editor_save (me, L"Duplicate tier");
		Ordered_addItemPos (grid -> tiers, newTier.transfer(), position);

		my selectedTier = position;
		FunctionEditor_updateText (me);
		FunctionEditor_redraw (me);
		my broadcastDataChanged ();
	EDITOR_END
}

/***** HELP MENU *****/

static void menu_cb_TextGridEditorHelp (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); Melder_help (L"TextGridEditor"); }
static void menu_cb_AboutSpecialSymbols (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); Melder_help (L"Special symbols"); }
static void menu_cb_PhoneticSymbols (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); Melder_help (L"Phonetic symbols"); }
static void menu_cb_AboutTextStyles (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); Melder_help (L"Text styles"); }

void structTextGridEditor :: v_createMenus () {
	TextGridEditor_Parent :: v_createMenus ();
	EditorMenu menu;

	#ifndef macintosh
		Editor_addCommand (this, L"Edit", L"-- cut copy paste --", 0, NULL);
		Editor_addCommand (this, L"Edit", L"Cut text", 'X', menu_cb_Cut);
		Editor_addCommand (this, L"Edit", L"Cut", Editor_HIDDEN, menu_cb_Cut);
		Editor_addCommand (this, L"Edit", L"Copy text", 'C', menu_cb_Copy);
		Editor_addCommand (this, L"Edit", L"Copy", Editor_HIDDEN, menu_cb_Copy);
		Editor_addCommand (this, L"Edit", L"Paste text", 'V', menu_cb_Paste);
		Editor_addCommand (this, L"Edit", L"Paste", Editor_HIDDEN, menu_cb_Paste);
		Editor_addCommand (this, L"Edit", L"Erase text", 0, menu_cb_Erase);
		Editor_addCommand (this, L"Edit", L"Erase", Editor_HIDDEN, menu_cb_Erase);
	#endif
	Editor_addCommand (this, L"Edit", L"-- encoding --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Convert entire TextGrid to backslash trigraphs", 0, menu_cb_Genericize);
	Editor_addCommand (this, L"Edit", L"Genericize entire TextGrid", Editor_HIDDEN, menu_cb_Genericize);
	Editor_addCommand (this, L"Edit", L"Genericize", Editor_HIDDEN, menu_cb_Genericize);
	Editor_addCommand (this, L"Edit", L"Convert entire TextGrid to Unicode", 0, menu_cb_Nativize);
	Editor_addCommand (this, L"Edit", L"Nativize entire TextGrid", Editor_HIDDEN, menu_cb_Nativize);
	Editor_addCommand (this, L"Edit", L"Nativize", Editor_HIDDEN, menu_cb_Nativize);
	Editor_addCommand (this, L"Edit", L"-- search --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Find...", 'F', menu_cb_Find);
	Editor_addCommand (this, L"Edit", L"Find again", 'G', menu_cb_FindAgain);

	if (d_sound.data) {
		Editor_addCommand (this, L"Select", L"-- move to zero --", 0, 0);
		Editor_addCommand (this, L"Select", L"Move start of selection to nearest zero crossing", ',', menu_cb_MoveBtoZero);
		Editor_addCommand (this, L"Select", L"Move begin of selection to nearest zero crossing", Editor_HIDDEN, menu_cb_MoveBtoZero);
		Editor_addCommand (this, L"Select", L"Move cursor to nearest zero crossing", '0', menu_cb_MoveCursorToZero);
		Editor_addCommand (this, L"Select", L"Move end of selection to nearest zero crossing", '.', menu_cb_MoveEtoZero);
	}

	Editor_addCommand (this, L"Query", L"-- query interval --", 0, NULL);
	Editor_addCommand (this, L"Query", L"Get starting point of interval", 0, menu_cb_GetStartingPointOfInterval);
	Editor_addCommand (this, L"Query", L"Get end point of interval", 0, menu_cb_GetEndPointOfInterval);
	Editor_addCommand (this, L"Query", L"Get label of interval", 0, menu_cb_GetLabelOfInterval);

	menu = Editor_addMenu (this, L"Interval", 0);
	EditorMenu_addCommand (menu, L"Add interval on tier 1", GuiMenu_COMMAND | '1', menu_cb_InsertIntervalOnTier1);
	EditorMenu_addCommand (menu, L"Add interval on tier 2", GuiMenu_COMMAND | '2', menu_cb_InsertIntervalOnTier2);
	EditorMenu_addCommand (menu, L"Add interval on tier 3", GuiMenu_COMMAND | '3', menu_cb_InsertIntervalOnTier3);
	EditorMenu_addCommand (menu, L"Add interval on tier 4", GuiMenu_COMMAND | '4', menu_cb_InsertIntervalOnTier4);
	EditorMenu_addCommand (menu, L"Add interval on tier 5", GuiMenu_COMMAND | '5', menu_cb_InsertIntervalOnTier5);
	EditorMenu_addCommand (menu, L"Add interval on tier 6", GuiMenu_COMMAND | '6', menu_cb_InsertIntervalOnTier6);
	EditorMenu_addCommand (menu, L"Add interval on tier 7", GuiMenu_COMMAND | '7', menu_cb_InsertIntervalOnTier7);
	EditorMenu_addCommand (menu, L"Add interval on tier 8", GuiMenu_COMMAND | '8', menu_cb_InsertIntervalOnTier8);

	menu = Editor_addMenu (this, L"Boundary", 0);
	/*EditorMenu_addCommand (menu, L"Move to B", 0, menu_cb_MoveToB);
	EditorMenu_addCommand (menu, L"Move to E", 0, menu_cb_MoveToE);*/
	if (d_sound.data)
		EditorMenu_addCommand (menu, L"Move to nearest zero crossing", 0, menu_cb_MoveToZero);
	EditorMenu_addCommand (menu, L"-- insert boundary --", 0, NULL);
	EditorMenu_addCommand (menu, L"Add on selected tier", GuiMenu_ENTER, menu_cb_InsertOnSelectedTier);
	EditorMenu_addCommand (menu, L"Add on tier 1", GuiMenu_COMMAND | GuiMenu_F1, menu_cb_InsertOnTier1);
	EditorMenu_addCommand (menu, L"Add on tier 2", GuiMenu_COMMAND | GuiMenu_F2, menu_cb_InsertOnTier2);
	EditorMenu_addCommand (menu, L"Add on tier 3", GuiMenu_COMMAND | GuiMenu_F3, menu_cb_InsertOnTier3);
	EditorMenu_addCommand (menu, L"Add on tier 4", GuiMenu_COMMAND | GuiMenu_F4, menu_cb_InsertOnTier4);
	EditorMenu_addCommand (menu, L"Add on tier 5", GuiMenu_COMMAND | GuiMenu_F5, menu_cb_InsertOnTier5);
	EditorMenu_addCommand (menu, L"Add on tier 6", GuiMenu_COMMAND | GuiMenu_F6, menu_cb_InsertOnTier6);
	EditorMenu_addCommand (menu, L"Add on tier 7", GuiMenu_COMMAND | GuiMenu_F7, menu_cb_InsertOnTier7);
	EditorMenu_addCommand (menu, L"Add on tier 8", GuiMenu_COMMAND | GuiMenu_F8, menu_cb_InsertOnTier8);
	EditorMenu_addCommand (menu, L"Add on all tiers", GuiMenu_COMMAND | GuiMenu_F9, menu_cb_InsertOnAllTiers);
	EditorMenu_addCommand (menu, L"-- remove mark --", 0, NULL);
	EditorMenu_addCommand (menu, L"Remove", GuiMenu_OPTION | GuiMenu_BACKSPACE, menu_cb_RemovePointOrBoundary);

	menu = Editor_addMenu (this, L"Tier", 0);
	EditorMenu_addCommand (menu, L"Add interval tier...", 0, menu_cb_AddIntervalTier);
	EditorMenu_addCommand (menu, L"Add point tier...", 0, menu_cb_AddPointTier);
	EditorMenu_addCommand (menu, L"Duplicate tier...", 0, menu_cb_DuplicateTier);
	EditorMenu_addCommand (menu, L"Rename tier...", 0, menu_cb_RenameTier);
	EditorMenu_addCommand (menu, L"-- remove tier --", 0, NULL);
	EditorMenu_addCommand (menu, L"Remove all text from tier", 0, menu_cb_RemoveAllTextFromTier);
	EditorMenu_addCommand (menu, L"Remove entire tier", 0, menu_cb_RemoveTier);
	EditorMenu_addCommand (menu, L"-- extract tier --", 0, NULL);
	EditorMenu_addCommand (menu, L"Extract to list of objects:", GuiMenu_INSENSITIVE, menu_cb_PublishTier /* dummy */);
	EditorMenu_addCommand (menu, L"Extract entire selected tier", 0, menu_cb_PublishTier);

	if (spellingChecker) {
		menu = Editor_addMenu (this, L"Spell", 0);
		EditorMenu_addCommand (menu, L"Check spelling in tier", GuiMenu_COMMAND | GuiMenu_OPTION | 'L', menu_cb_CheckSpelling);
		EditorMenu_addCommand (menu, L"Check spelling in interval", 0, menu_cb_CheckSpellingInInterval);
		EditorMenu_addCommand (menu, L"-- edit lexicon --", 0, NULL);
		EditorMenu_addCommand (menu, L"Add selected word to user dictionary", 0, menu_cb_AddToUserDictionary);
	}

	if (d_sound.data || d_longSound.data) {
		if (v_hasAnalysis ()) {
			v_createMenus_analysis ();   // insert some of the ancestor's menus *after* the TextGrid menus
		}
	}
}

void structTextGridEditor :: v_createHelpMenuItems (EditorMenu menu) {
	TextGridEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"TextGridEditor help", '?', menu_cb_TextGridEditorHelp);
	EditorMenu_addCommand (menu, L"About special symbols", 0, menu_cb_AboutSpecialSymbols);
	EditorMenu_addCommand (menu, L"Phonetic symbols", 0, menu_cb_PhoneticSymbols);
	EditorMenu_addCommand (menu, L"About text styles", 0, menu_cb_AboutTextStyles);
}

/***** CHILDREN *****/

static void gui_text_cb_change (I, GuiTextEvent event) {
	iam (TextGridEditor);
	(void) event;
	TextGrid grid = (TextGrid) my data;
	//Melder_casual ("gui_text_cb_change 1 in editor %ld", me);
	if (my suppressRedraw) return;   /* Prevent infinite loop if 'draw' method or Editor_broadcastChange calls GuiText_setString. */
	//Melder_casual ("gui_text_cb_change 2 in editor %ld", me);
	if (my selectedTier) {
		wchar_t *text = GuiText_getString (my text);
		IntervalTier intervalTier;
		TextTier textTier;
		_AnyTier_identifyClass ((Function) grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			long selectedInterval = getSelectedInterval (me);
			if (selectedInterval) {
				TextInterval interval = (TextInterval) intervalTier -> intervals -> item [selectedInterval];
				//Melder_casual ("gui_text_cb_change 3 in editor %ld", me);
				TextInterval_setText (interval, text);
				//Melder_casual ("gui_text_cb_change 4 in editor %ld", me);
				FunctionEditor_redraw (me);
				//Melder_casual ("gui_text_cb_change 5 in editor %ld", me);
				my broadcastDataChanged ();
				//Melder_casual ("gui_text_cb_change 6 in editor %ld", me);
			}
		} else {
			long selectedPoint = getSelectedPoint (me);
			if (selectedPoint) {
				TextPoint point = (TextPoint) textTier -> points -> item [selectedPoint];
				Melder_free (point -> mark);
				if (wcsspn (text, L" \n\t") != wcslen (text))   // any visible characters?
				point -> mark = Melder_wcsdup_f (text);
				FunctionEditor_redraw (me);
				my broadcastDataChanged ();
			}
		}
		Melder_free (text);
	}
}

void structTextGridEditor :: v_createChildren () {
	TextGridEditor_Parent :: v_createChildren ();
	GuiText_setChangeCallback (text, gui_text_cb_change, this);
}

void structTextGridEditor :: v_dataChanged () {
	TextGrid grid = (TextGrid) data;
	/*
	 * Perform a minimal selection change.
	 * Most changes will involve intervals and boundaries; however, there may also be tier removals.
	 * Do a simple guess.
	 */
	if (selectedTier > grid -> tiers -> size) {
		selectedTier = grid -> tiers -> size;
	}
	TextGridEditor_Parent :: v_dataChanged ();   // does all the updating
}

/********** DRAWING AREA **********/

void structTextGridEditor :: v_prepareDraw () {
	if (d_longSound.data) {
		try {
			LongSound_haveWindow (d_longSound.data, startWindow, endWindow);
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

static void do_drawIntervalTier (TextGridEditor me, IntervalTier tier, int itier) {
	#if gtk || defined (macintosh)
		bool platformUsesAntiAliasing = true;
	#else
		bool platformUsesAntiAliasing = false;
	#endif
	long x1DC, x2DC, yDC;
	int selectedInterval = itier == my selectedTier ? getSelectedInterval (me) : 0, iinterval, ninterval = tier -> intervals -> size;
	Graphics_WCtoDC (my graphics, my startWindow, 0.0, & x1DC, & yDC);
	Graphics_WCtoDC (my graphics, my endWindow, 0.0, & x2DC, & yDC);
	Graphics_setPercentSignIsItalic (my graphics, my useTextStyles);
	Graphics_setNumberSignIsBold (my graphics, my useTextStyles);
	Graphics_setCircumflexIsSuperscript (my graphics, my useTextStyles);
	Graphics_setUnderscoreIsSubscript (my graphics, my useTextStyles);

	/*
	 * Highlight interval: yellow (selected) or green (matching label).
	 */
	
	for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
		TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		if (tmax > my startWindow && tmin < my endWindow) {   /* Interval visible? */
			int intervalIsSelected = iinterval == selectedInterval;
			int labelMatches = Melder_stringMatchesCriterion (interval -> text, my greenMethod, my greenString);
			if (tmin < my startWindow) tmin = my startWindow;
			if (tmax > my endWindow) tmax = my endWindow;
			if (labelMatches) {
				Graphics_setColour (my graphics, Graphics_LIME);
				Graphics_fillRectangle (my graphics, tmin, tmax, 0.0, 1.0);
			}
			if (intervalIsSelected) {
				if (labelMatches) {
					tmin = 0.85 * tmin + 0.15 * tmax;
					tmax = 0.15 * tmin + 0.85 * tmax;
				}
				Graphics_setColour (my graphics, Graphics_YELLOW);
				Graphics_fillRectangle (my graphics, tmin, tmax, labelMatches ? 0.15 : 0.0, labelMatches? 0.85: 1.0);
			}
		}
	}
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_line (my graphics, my endWindow, 0.0, my endWindow, 1.0);

	/*
	 * Draw a grey bar and a selection button at the cursor position.
	 */
	if (my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow) {
		int cursorAtBoundary = FALSE;
		for (iinterval = 2; iinterval <= ninterval; iinterval ++) {
			TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
			if (interval -> xmin == my startSelection) cursorAtBoundary = TRUE;
		}
		if (! cursorAtBoundary) {
			double dy = Graphics_dyMMtoWC (my graphics, 1.5);
			Graphics_setGrey (my graphics, 0.8);
			Graphics_setLineWidth (my graphics, platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics, my startSelection, 0.0, my startSelection, 1.0);
			Graphics_setLineWidth (my graphics, 1.0);
			Graphics_setColour (my graphics, Graphics_BLUE);
			Graphics_circle_mm (my graphics, my startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics, my alignment, Graphics_HALF);
	for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
		TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		if (tmin < my tmin) tmin = my tmin; if (tmax > my tmax) tmax = my tmax;
		if (tmin >= tmax) continue;
		bool intervalIsSelected = selectedInterval == iinterval;

		/*
		 * Draw left boundary.
		 */
		if (tmin >= my startWindow && tmin <= my endWindow && iinterval > 1) {
			bool boundaryIsSelected = ( my selectedTier == itier && tmin == my startSelection );
			Graphics_setColour (my graphics, boundaryIsSelected ? Graphics_RED : Graphics_BLUE);
			Graphics_setLineWidth (my graphics, platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics, tmin, 0.0, tmin, 1.0);

			/*
			 * Show alignment with cursor.
			 */
			if (tmin == my startSelection) {
				Graphics_setColour (my graphics, Graphics_YELLOW);
				Graphics_setLineWidth (my graphics, platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics, tmin, 0.0, tmin, 1.0);
			}
		}
		Graphics_setLineWidth (my graphics, 1.0);

		/*
		 * Draw label text.
		 */
		if (interval -> text && tmax >= my startWindow && tmin <= my endWindow) {
			double t1 = my startWindow > tmin ? my startWindow : tmin;
			double t2 = my endWindow < tmax ? my endWindow : tmax;
			Graphics_setColour (my graphics, intervalIsSelected ? Graphics_RED : Graphics_BLACK);
			Graphics_textRect (my graphics, t1, t2, 0.0, 1.0, interval -> text);
			Graphics_setColour (my graphics, Graphics_BLACK);
		}

	}
	Graphics_setPercentSignIsItalic (my graphics, TRUE);
	Graphics_setNumberSignIsBold (my graphics, TRUE);
	Graphics_setCircumflexIsSuperscript (my graphics, TRUE);
	Graphics_setUnderscoreIsSubscript (my graphics, TRUE);
}

static void do_drawTextTier (TextGridEditor me, TextTier tier, int itier) {
	#if gtk || defined (macintosh)
		bool platformUsesAntiAliasing = true;
	#else
		bool platformUsesAntiAliasing = false;
	#endif
	int ipoint, npoint = tier -> points -> size;
	Graphics_setPercentSignIsItalic (my graphics, my useTextStyles);
	Graphics_setNumberSignIsBold (my graphics, my useTextStyles);
	Graphics_setCircumflexIsSuperscript (my graphics, my useTextStyles);
	Graphics_setUnderscoreIsSubscript (my graphics, my useTextStyles);

	/*
	 * Draw a grey bar and a selection button at the cursor position.
	 */
	if (my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow) {
		int cursorAtPoint = FALSE;
		for (ipoint = 1; ipoint <= npoint; ipoint ++) {
			TextPoint point = (TextPoint) tier -> points -> item [ipoint];
			if (point -> number == my startSelection) cursorAtPoint = TRUE;
		}
		if (! cursorAtPoint) {
			double dy = Graphics_dyMMtoWC (my graphics, 1.5);
			Graphics_setGrey (my graphics, 0.8);
			Graphics_setLineWidth (my graphics, platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics, my startSelection, 0.0, my startSelection, 1.0);
			Graphics_setLineWidth (my graphics, 1.0);
			Graphics_setColour (my graphics, Graphics_BLUE);
			Graphics_circle_mm (my graphics, my startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
	for (ipoint = 1; ipoint <= npoint; ipoint ++) {
		TextPoint point = (TextPoint) tier -> points -> item [ipoint];
		double t = point -> number;
		if (t >= my startWindow && t <= my endWindow) {
			bool pointIsSelected = ( itier == my selectedTier && t == my startSelection );
			Graphics_setColour (my graphics, pointIsSelected ? Graphics_RED : Graphics_BLUE);
			Graphics_setLineWidth (my graphics, platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics, t, 0.0, t, 0.2);
			Graphics_line (my graphics, t, 0.8, t, 1);
			Graphics_setLineWidth (my graphics, 1.0);

			/*
			 * Wipe out the cursor where the text is going to be.
			 */
			Graphics_setColour (my graphics, Graphics_WHITE);
			Graphics_line (my graphics, t, 0.2, t, 0.8);

			/*
			 * Show alignment with cursor.
			 */
			if (my startSelection == my endSelection && t == my startSelection) {
				Graphics_setColour (my graphics, Graphics_YELLOW);
				Graphics_setLineWidth (my graphics, platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics, t, 0.0, t, 0.2);
				Graphics_line (my graphics, t, 0.8, t, 1.0);
			}
			Graphics_setColour (my graphics, pointIsSelected ? Graphics_RED : Graphics_BLUE);
			if (point -> mark) Graphics_text (my graphics, t, 0.5, point -> mark);
		}
	}
	Graphics_setPercentSignIsItalic (my graphics, TRUE);
	Graphics_setNumberSignIsBold (my graphics, TRUE);
	Graphics_setCircumflexIsSuperscript (my graphics, TRUE);
	Graphics_setUnderscoreIsSubscript (my graphics, TRUE);
}

void structTextGridEditor :: v_draw () {
	TextGrid grid = (TextGrid) data;
	Graphics_Viewport vp1, vp2;
	long itier, ntier = grid -> tiers -> size;
	enum kGraphics_font oldFont = Graphics_inqFont (graphics);
	int oldFontSize = Graphics_inqFontSize (graphics);
	bool showAnalysis = v_hasAnalysis () && (spectrogram.show || pitch.show || intensity.show || formant.show) && (d_longSound.data || d_sound.data);
	double soundY = _TextGridEditor_computeSoundY (this), soundY2 = showAnalysis ? 0.5 * (1.0 + soundY) : soundY;

	/*
	 * Draw optional sound.
	 */
	if (d_longSound.data || d_sound.data) {
		vp1 = Graphics_insetViewport (graphics, 0.0, 1.0, soundY2, 1.0);
		Graphics_setColour (graphics, Graphics_WHITE);
		Graphics_setWindow (graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (graphics, 0, 1, 0, 1);
		TimeSoundEditor_draw_sound (this, -1.0, 1.0);
		Graphics_flushWs (graphics);
		Graphics_resetViewport (graphics, vp1);
	}

	/*
	 * Draw tiers.
	 */
	if (d_longSound.data || d_sound.data) vp1 = Graphics_insetViewport (graphics, 0.0, 1.0, 0.0, soundY);
	Graphics_setColour (graphics, Graphics_WHITE);
	Graphics_setWindow (graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (graphics, 0, 1, 0, 1);
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_rectangle (graphics, 0, 1, 0, 1);
	Graphics_setWindow (graphics, startWindow, endWindow, 0.0, 1.0);
	for (itier = 1; itier <= ntier; itier ++) {
		Function anyTier = (Function) grid -> tiers -> item [itier];
		bool tierIsSelected = itier == selectedTier;
		bool isIntervalTier = anyTier -> classInfo == classIntervalTier;
		vp2 = Graphics_insetViewport (graphics, 0.0, 1.0,
			1.0 - (double) itier / (double) ntier,
			1.0 - (double) (itier - 1) / (double) ntier);
		Graphics_setColour (graphics, Graphics_BLACK);
		if (itier != 1) Graphics_line (graphics, startWindow, 1.0, endWindow, 1.0);

		/*
		 * Show the number and the name of the tier.
		 */
		Graphics_setColour (graphics, tierIsSelected ? Graphics_RED : Graphics_BLACK);
		Graphics_setFont (graphics, oldFont);
		Graphics_setFontSize (graphics, 14);
		Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
		Graphics_text2 (graphics, startWindow, 0.5, tierIsSelected ? L"\\pf " : L"", Melder_integer (itier));
		Graphics_setFontSize (graphics, oldFontSize);
		if (anyTier -> name && anyTier -> name [0]) {
			Graphics_setTextAlignment (graphics, Graphics_LEFT,
				showNumberOf == kTextGridEditor_showNumberOf_NOTHING ? Graphics_HALF : Graphics_BOTTOM);
			Graphics_text (graphics, endWindow, 0.5, anyTier -> name);
		}
		if (showNumberOf != kTextGridEditor_showNumberOf_NOTHING) {
			Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_TOP);
			if (showNumberOf == kTextGridEditor_showNumberOf_INTERVALS_OR_POINTS) {
				long count = isIntervalTier ? ((IntervalTier) anyTier) -> intervals -> size : ((TextTier) anyTier) -> points -> size;
				long position = itier == selectedTier ? ( isIntervalTier ? getSelectedInterval (this) : getSelectedPoint (this) ) : 0;
				if (position) {
					Graphics_text5 (graphics, endWindow, 0.5, L"(", Melder_integer (position), L"/", Melder_integer (count), L")");
				} else {
					Graphics_text3 (graphics, endWindow, 0.5, L"(", Melder_integer (count), L")");
				}
			} else {
				Melder_assert (kTextGridEditor_showNumberOf_NONEMPTY_INTERVALS_OR_POINTS);
				long count = 0;
				if (isIntervalTier) {
					IntervalTier tier = (IntervalTier) anyTier;
					long ninterval = tier -> intervals -> size, iinterval;
					for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
						TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
						if (interval -> text != NULL && interval -> text [0] != '\0') {
							count ++;
						}
					}
				} else {
					TextTier tier = (TextTier) anyTier;
					long npoint = tier -> points -> size, ipoint;
					for (ipoint = 1; ipoint <= npoint; ipoint ++) {
						TextPoint point = (TextPoint) tier -> points -> item [ipoint];
						if (point -> mark != NULL && point -> mark [0] != '\0') {
							count ++;
						}
					}
				}
				Graphics_text3 (graphics, endWindow, 0.5, L"(##", Melder_integer (count), L"#)");
			}
		}

		Graphics_setColour (graphics, Graphics_BLACK);
		Graphics_setFont (graphics, kGraphics_font_TIMES);
		Graphics_setFontSize (graphics, fontSize);
		if (isIntervalTier)
			do_drawIntervalTier (this, (IntervalTier) anyTier, itier);
		else
			do_drawTextTier (this, (TextTier) anyTier, itier);
		Graphics_resetViewport (graphics, vp2);
	}
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_setFont (graphics, oldFont);
	Graphics_setFontSize (graphics, oldFontSize);
	if (d_longSound.data || d_sound.data) Graphics_resetViewport (graphics, vp1);
	Graphics_flushWs (graphics);

	if (showAnalysis) {
		vp1 = Graphics_insetViewport (graphics, 0.0, 1.0, soundY, soundY2);
		v_draw_analysis ();
		Graphics_flushWs (graphics);
		Graphics_resetViewport (graphics, vp1);
		/* Draw pulses. */
		if (pulses.show) {
			vp1 = Graphics_insetViewport (graphics, 0.0, 1.0, soundY2, 1.0);
			v_draw_analysis_pulses ();
			TimeSoundEditor_draw_sound (this, -1.0, 1.0);   // second time, partially across the pulses
			Graphics_flushWs (graphics);
			Graphics_resetViewport (graphics, vp1);
		}
	}
	Graphics_setWindow (graphics, startWindow, endWindow, 0.0, 1.0);
	if (d_longSound.data || d_sound.data) {
		Graphics_line (graphics, startWindow, soundY, endWindow, soundY);
		if (showAnalysis) {
			Graphics_line (graphics, startWindow, soundY2, endWindow, soundY2);
			Graphics_line (graphics, startWindow, soundY, startWindow, soundY2);
			Graphics_line (graphics, endWindow, soundY, endWindow, soundY2);
		}
	}

	/*
	 * Finally, us usual, update the menus.
	 */
	v_updateMenuItems_file ();
}

static void do_drawWhileDragging (TextGridEditor me, double numberOfTiers, int *selectedTier, double x, double soundY) {
	long itier;
	for (itier = 1; itier <= numberOfTiers; itier ++) if (selectedTier [itier]) {
		double ymin = soundY * (1.0 - (double) itier / numberOfTiers);
		double ymax = soundY * (1.0 - (double) (itier - 1) / numberOfTiers);
		Graphics_setLineWidth (my graphics, 7);
		Graphics_line (my graphics, x, ymin, x, ymax);
	}
	Graphics_setLineWidth (my graphics, 1);
	Graphics_line (my graphics, x, 0, x, 1.01);
	Graphics_text1 (my graphics, x, 1.01, Melder_fixed (x, 6));
}

static void do_dragBoundary (TextGridEditor me, double xbegin, int iClickedTier, int shiftKeyPressed) {
	TextGrid grid = (TextGrid) my data;
	int itier, numberOfTiers = grid -> tiers -> size, itierDrop;
	double xWC = xbegin, yWC;
	double leftDraggingBoundary = my tmin, rightDraggingBoundary = my tmax;   /* Initial dragging range. */
	int selectedTier [100];
	double soundY = _TextGridEditor_computeSoundY (me);

	/*
	 * Determine the set of selected boundaries and points, and the dragging range.
	 */
	for (itier = 1; itier <= numberOfTiers; itier ++) {
		selectedTier [itier] = FALSE;   /* The default. */
		/*
		 * If she has pressed the shift key, let her drag all the boundaries and points at this time.
		 * Otherwise, let her only drag the boundary or point on the clicked tier.
		 */
		if (itier == iClickedTier || shiftKeyPressed == my shiftDragMultiple) {
			IntervalTier intervalTier;
			TextTier textTier;
			_AnyTier_identifyClass ((Function) grid -> tiers -> item [itier], & intervalTier, & textTier);
			if (intervalTier) {
				long ibound = IntervalTier_hasBoundary (intervalTier, xbegin);
				if (ibound) {
					TextInterval leftInterval = (TextInterval) intervalTier -> intervals -> item [ibound - 1];
					TextInterval rightInterval = (TextInterval) intervalTier -> intervals -> item [ibound];
					selectedTier [itier] = TRUE;
					/*
					 * Prevent her to drag the boundary past its left or right neighbours on the same tier.
					 */
					if (leftInterval -> xmin > leftDraggingBoundary) {
						leftDraggingBoundary = leftInterval -> xmin;
					}
					if (rightInterval -> xmax < rightDraggingBoundary) {
						rightDraggingBoundary = rightInterval -> xmax;
					}
				}
			} else {
				if (AnyTier_hasPoint (textTier, xbegin)) {
					/*
					 * Other than with boundaries on interval tiers,
					 * points on text tiers can be dragged past their neighbours.
					 */
					selectedTier [itier] = TRUE;
				}
			}
		}
	}

	Graphics_xorOn (my graphics, Graphics_MAROON);
	Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_BOTTOM);
	do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // draw at old position
	while (Graphics_mouseStillDown (my graphics)) {
		double xWC_new;
		Graphics_getMouseLocation (my graphics, & xWC_new, & yWC);
		if (xWC_new != xWC) {
			do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // undraw at old position
			xWC = xWC_new;
			do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // draw at new position
		}
	}
	do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // undraw at new position
	Graphics_xorOff (my graphics);

	/*
	 * The simplest way to cancel the dragging operation, is to drag outside the window.
	 */
	if (xWC <= my startWindow || xWC >= my endWindow) {
		return;
	}

	/*
	 * If she dropped near an existing boundary in an unselected tier or near the cursor, we snap to that mark.
	 */
	itierDrop = _TextGridEditor_yWCtoTier (me, yWC);
	if (yWC > 0.0 && yWC < soundY && ! selectedTier [itierDrop]) {   // dropped inside an unselected tier?
		Function anyTierDrop = (Function) grid -> tiers -> item [itierDrop];
		if (anyTierDrop -> classInfo == classIntervalTier) {
			IntervalTier tierDrop = (IntervalTier) anyTierDrop;
			long ibound;
			for (ibound = 1; ibound < tierDrop -> intervals -> size; ibound ++) {
				TextInterval left = (TextInterval) tierDrop -> intervals -> item [ibound];
				if (fabs (Graphics_dxWCtoMM (my graphics, xWC - left -> xmax)) < 1.5) {   // near a boundary?
					/*
					 * Snap to boundary.
					 */
					xWC = left -> xmax;
				}
			}
		} else {
			TextTier tierDrop = (TextTier) anyTierDrop;
			long ipoint;
			for (ipoint = 1; ipoint <= tierDrop -> points -> size; ipoint ++) {
				TextPoint point = (TextPoint) tierDrop -> points -> item [ipoint];
				if (fabs (Graphics_dxWCtoMM (my graphics, xWC - point -> number)) < 1.5) {   // near a point?
					/*
					 * Snap to point.
					 */
					xWC = point -> number;
				}
			}
		}
	} else if (xbegin != my startSelection && fabs (Graphics_dxWCtoMM (my graphics, xWC - my startSelection)) < 1.5) {   // near the cursor?
		/*
		 * Snap to cursor.
		 */
		xWC = my startSelection;
	} else if (xbegin != my endSelection && fabs (Graphics_dxWCtoMM (my graphics, xWC - my endSelection)) < 1.5) {   // near the cursor?
		/*
		 * Snap to cursor.
		 */
		xWC = my endSelection;
	}

	/*
	 * We cannot move a boundary out of the dragging range.
	 */
	if (xWC <= leftDraggingBoundary || xWC >= rightDraggingBoundary) {
		Melder_beep ();
		return;
	}

	Editor_save (me, L"Drag");

	for (itier = 1; itier <= numberOfTiers; itier ++) if (selectedTier [itier]) {
		IntervalTier intervalTier;
		TextTier textTier;
		_AnyTier_identifyClass ((Function) grid -> tiers -> item [itier], & intervalTier, & textTier);
		if (intervalTier) {
			long ibound, numberOfIntervals = intervalTier -> intervals -> size;
			Any *intervals = intervalTier -> intervals -> item;
			for (ibound = 2; ibound <= numberOfIntervals; ibound ++) {
				TextInterval left = (TextInterval) intervals [ibound - 1], right = (TextInterval) intervals [ibound];
				if (left -> xmax == xbegin) {   // boundary dragged?
					left -> xmax = right -> xmin = xWC;   // move boundary to drop site
					break;
				}
			}
		} else {
			long iDraggedPoint = AnyTier_hasPoint (textTier, xbegin);
			if (iDraggedPoint) {
				long dropSiteHasPoint = AnyTier_hasPoint (textTier, xWC);
				if (dropSiteHasPoint) {
					Melder_warning (L"Cannot drop point on an existing point.");
				} else {
					TextPoint point = (TextPoint) textTier -> points -> item [iDraggedPoint];
					/*
					 * Move point to drop site. May have passed another point.
					 */
					TextPoint newPoint = Data_copy (point);
					newPoint -> number = xWC;   // move point to drop site
					Collection_removeItem (textTier -> points, iDraggedPoint);
					Collection_addItem (textTier -> points, newPoint);
				}
			}
		}
	}

	/*
	 * Select the drop site.
	 */
	if (my startSelection == xbegin)
		my startSelection = xWC;
	if (my endSelection == xbegin)
		my endSelection = xWC;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	FunctionEditor_marksChanged (me);
	my broadcastDataChanged ();
}

int structTextGridEditor :: v_click (double xclick, double yWC, bool shiftKeyPressed) {
	TextGrid grid = (TextGrid) data;
	double tmin, tmax, x, y;
	long ntiers = grid -> tiers -> size, iClickedTier, iClickedInterval, iClickedPoint;
	int clickedLeftBoundary = 0, nearBoundaryOrPoint, nearCursorCircle, drag = FALSE;
	IntervalTier intervalTier;
	TextTier textTier;
	TextInterval interval = NULL;
	TextPoint point = NULL;
	double soundY = _TextGridEditor_computeSoundY (this);
	double tnear;

	/*
	 * In answer to a click in the sound part,
	 * we keep the same tier selected and move the cursor or drag the "yellow" selection.
	 */
	if (yWC > soundY) {   /* Clicked in sound part? */
		if ((spectrogram.show || formant.show) && yWC < 0.5 * (soundY + 1.0)) {
			spectrogram.cursor = spectrogram.viewFrom +
				2.0 * (yWC - soundY) / (1.0 - soundY) * (spectrogram.viewTo - spectrogram.viewFrom);
		}
		TextGridEditor_Parent :: v_click (xclick, yWC, shiftKeyPressed);
		return FunctionEditor_UPDATE_NEEDED;
	}

	/*
	 * She clicked in the grid part.
	 * We select the tier in which she clicked.
	 */
	iClickedTier = _TextGridEditor_yWCtoTier (this, yWC);

	if (xclick <= startWindow || xclick >= endWindow) {
		selectedTier = iClickedTier;
		return FunctionEditor_UPDATE_NEEDED;
	}

	_TextGridEditor_timeToInterval (this, xclick, iClickedTier, & tmin, & tmax);
	_AnyTier_identifyClass ((Function) grid -> tiers -> item [iClickedTier], & intervalTier, & textTier);

	/*
	 * Get the time of the nearest boundary or point.
	 */
	tnear = NUMundefined;
	if (intervalTier) {
		iClickedInterval = IntervalTier_timeToIndex (intervalTier, xclick);
		if (iClickedInterval) {
			interval = (TextInterval) intervalTier -> intervals -> item [iClickedInterval];
			if (xclick > 0.5 * (interval -> xmin + interval -> xmax)) {
				tnear = interval -> xmax;
				clickedLeftBoundary = iClickedInterval + 1;
			} else {
				tnear = interval -> xmin;
				clickedLeftBoundary = iClickedInterval;
			}
		} else {
			/*
			 * She clicked outside time domain of intervals.
			 * This can occur when we are grouped with a longer time function.
			 */
			selectedTier = iClickedTier;
			return FunctionEditor_UPDATE_NEEDED;
		}
	} else {
		iClickedPoint = AnyTier_timeToNearestIndex (textTier, xclick);
		if (iClickedPoint) {
			point = (TextPoint) textTier -> points -> item [iClickedPoint];
			tnear = point -> number;
		}
	}
	Melder_assert (! (intervalTier && ! clickedLeftBoundary));

	/*
	 * Where did she click?
	 */
	nearBoundaryOrPoint = tnear != NUMundefined && fabs (Graphics_dxWCtoMM (graphics, xclick - tnear)) < 1.5;
	nearCursorCircle = startSelection == endSelection && Graphics_distanceWCtoMM (graphics, xclick, yWC,
		startSelection, (ntiers + 1 - iClickedTier) * soundY / ntiers - Graphics_dyMMtoWC (graphics, 1.5)) < 1.5;

	/*
	 * Find out whether this is a click or a drag.
	 */
	while (Graphics_mouseStillDown (graphics)) {
		Graphics_getMouseLocation (graphics, & x, & y);
		if (x < startWindow) x = startWindow;
		if (x > endWindow) x = endWindow;
		if (fabs (Graphics_dxWCtoMM (graphics, x - xclick)) > 1.5) {
			drag = TRUE;
			break;
		}
	}

	if (nearBoundaryOrPoint) {
		/*
		 * Possibility 1: she clicked near a boundary or point.
		 * Select or drag it.
		 */
		if (intervalTier && (clickedLeftBoundary < 2 || clickedLeftBoundary > intervalTier -> intervals -> size)) {		
			/*
			 * Ignore click on left edge of first interval or right edge of last interval.
			 */
			selectedTier = iClickedTier;
		} else if (drag) {
			/*
			 * The tier that has been clicked becomes the new selected tier.
			 * This has to be done before the next Update, i.e. also before do_dragBoundary!
			 */
			selectedTier = iClickedTier;
			do_dragBoundary (this, tnear, iClickedTier, shiftKeyPressed);
			return FunctionEditor_NO_UPDATE_NEEDED;
		} else {
			/*
			 * If she clicked on an unselected boundary or point, we select it.
			 */
			if (shiftKeyPressed) {
				if (tnear > 0.5 * (startSelection + endSelection))
					endSelection = tnear;
				else
					startSelection = tnear;
			} else {
				startSelection = endSelection = tnear;   /* Move cursor so that the boundary or point is selected. */
			}
			selectedTier = iClickedTier;
		}
	} else if (nearCursorCircle) {
		/*
		 * Possibility 2: she clicked near the cursor circle.
		 * Insert boundary or point. There is no danger that we insert on top of an existing boundary or point,
		 * because we are not 'nearBoundaryOrPoint'.
		 */
		insertBoundaryOrPoint (this, iClickedTier, startSelection, startSelection, false);
		selectedTier = iClickedTier;
		FunctionEditor_marksChanged (this);
		broadcastDataChanged ();
		if (drag) Graphics_waitMouseUp (graphics);
		return FunctionEditor_NO_UPDATE_NEEDED;
	} else {
		/*
		 * Possibility 3: she clicked in empty space.
		 */
		if (intervalTier) {
			startSelection = tmin;
			endSelection = tmax;
		}
		selectedTier = iClickedTier;
	}
	if (drag) Graphics_waitMouseUp (graphics);
	return FunctionEditor_UPDATE_NEEDED;
}

int structTextGridEditor :: v_clickB (double t, double yWC) {
	double soundY = _TextGridEditor_computeSoundY (this);

	if (yWC > soundY) {   // clicked in sound part?
		startSelection = t;
		if (startSelection > endSelection) {
			double dummy = startSelection;
			startSelection = endSelection;
			endSelection = dummy;
		}
		return FunctionEditor_UPDATE_NEEDED;
	}
	int itier = _TextGridEditor_yWCtoTier (this, yWC);
	double tmin, tmax;
	_TextGridEditor_timeToInterval (this, t, itier, & tmin, & tmax);
	startSelection = t - tmin < tmax - t ? tmin : tmax;   // to nearest boundary
	if (startSelection > endSelection) {
		double dummy = startSelection;
		startSelection = endSelection;
		endSelection = dummy;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

int structTextGridEditor :: v_clickE (double t, double yWC) {
	double soundY = _TextGridEditor_computeSoundY (this);

	if (yWC > soundY) {   // clicked in sound part?
		endSelection = t;
		if (startSelection > endSelection) {
			double dummy = startSelection;
			startSelection = endSelection;
			endSelection = dummy;
		}
		return FunctionEditor_UPDATE_NEEDED;
	}
	int itier = _TextGridEditor_yWCtoTier (this, yWC);
	double tmin, tmax;
	_TextGridEditor_timeToInterval (this, t, itier, & tmin, & tmax);
	endSelection = t - tmin < tmax - t ? tmin : tmax;
	if (startSelection > endSelection) {
		double dummy = startSelection;
		startSelection = endSelection;
		endSelection = dummy;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

void structTextGridEditor :: v_play (double tmin, double tmax) {
	if (d_longSound.data) {
		LongSound_playPart (d_longSound.data, tmin, tmax, theFunctionEditor_playCallback, this);
	} else if (d_sound.data) {
		Sound_playPart (d_sound.data, tmin, tmax, theFunctionEditor_playCallback, this);
	}
}

void structTextGridEditor :: v_updateText () {
	TextGrid grid = (TextGrid) data;
	const wchar *newText = L"";
	if (selectedTier) {
		IntervalTier intervalTier;
		TextTier textTier;
		_AnyTier_identifyClass ((Function) grid -> tiers -> item [selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			long iinterval = IntervalTier_timeToIndex (intervalTier, startSelection);
			if (iinterval) {
				TextInterval interval = (TextInterval) intervalTier -> intervals -> item [iinterval];
				if (interval -> text) {
					newText = interval -> text;
				}
			}
		} else {
			long ipoint = AnyTier_hasPoint (textTier, startSelection);
			if (ipoint) {
				TextPoint point = (TextPoint) textTier -> points -> item [ipoint];
				if (point -> mark) {
					newText = point -> mark;
				}
			}
		}
	}
	//Melder_casual ("v_updateText in editor %ld %ls %d", this, name, (int) suppressRedraw);
	suppressRedraw = TRUE;   // prevent valueChangedCallback from redrawing
	GuiText_setString (text, newText);
	long cursor = wcslen (newText);   // at end
	GuiText_setSelection (text, cursor, cursor);
	suppressRedraw = FALSE;
}

void structTextGridEditor :: v_prefs_addFields (EditorCommand cmd) {
	Any radio;
	NATURAL (L"Font size (points)", TextGridEditor_DEFAULT_FONT_SIZE_STRING)
	OPTIONMENU_ENUM (L"Text alignment in intervals", kGraphics_horizontalAlignment, DEFAULT)
	OPTIONMENU (L"The symbols %#_^ in labels", TextGridEditor_DEFAULT_USE_TEXT_STYLES + 1)
		OPTION (L"are shown as typed")
		OPTION (L"mean italic/bold/sub/super")
	OPTIONMENU (L"With the shift key, you drag", TextGridEditor_DEFAULT_SHIFT_DRAG_MULTIPLE + 1)
		OPTION (L"a single boundary")
		OPTION (L"multiple boundaries")
	OPTIONMENU_ENUM (L"Show number of", kTextGridEditor_showNumberOf, DEFAULT)
	OPTIONMENU_ENUM (L"Paint intervals green whose label...", kMelder_string, DEFAULT)
	SENTENCE (L"...the text", TextGridEditor_DEFAULT_GREEN_STRING)
}
void structTextGridEditor :: v_prefs_setValues (EditorCommand cmd) {
	SET_INTEGER (L"The symbols %#_^ in labels", useTextStyles + 1)
	SET_INTEGER (L"Font size", fontSize)
	SET_ENUM (L"Text alignment in intervals", kGraphics_horizontalAlignment, alignment)
	SET_INTEGER (L"With the shift key, you drag", shiftDragMultiple + 1)
	SET_ENUM (L"Show number of", kTextGridEditor_showNumberOf, showNumberOf)
	SET_ENUM (L"Paint intervals green whose label...", kMelder_string, greenMethod)
	SET_STRING (L"...the text", greenString)
}
void structTextGridEditor :: v_prefs_getValues (EditorCommand cmd) {
	preferences.useTextStyles = useTextStyles = GET_INTEGER (L"The symbols %#_^ in labels") - 1;
	preferences.fontSize = fontSize = GET_INTEGER (L"Font size");
	preferences.alignment = alignment = GET_ENUM (kGraphics_horizontalAlignment, L"Text alignment in intervals");
	preferences.shiftDragMultiple = shiftDragMultiple = GET_INTEGER (L"With the shift key, you drag") - 1;
	preferences.showNumberOf = showNumberOf = GET_ENUM (kTextGridEditor_showNumberOf, L"Show number of");
	preferences.greenMethod = greenMethod = GET_ENUM (kMelder_string, L"Paint intervals green whose label...");
	wcsncpy (greenString, GET_STRING (L"...the text"), Preferences_STRING_BUFFER_SIZE);
	greenString [Preferences_STRING_BUFFER_SIZE - 1] = '\0';
	wcscpy (preferences.greenString, greenString);
	FunctionEditor_redraw (this);
}

void structTextGridEditor :: v_createMenuItems_view_timeDomain (EditorMenu menu) {
	TextGridEditor_Parent :: v_createMenuItems_view_timeDomain (menu);
	EditorMenu_addCommand (menu, L"Select previous tier", GuiMenu_OPTION | GuiMenu_UP_ARROW, menu_cb_SelectPreviousTier);
	EditorMenu_addCommand (menu, L"Select next tier", GuiMenu_OPTION | GuiMenu_DOWN_ARROW, menu_cb_SelectNextTier);
	EditorMenu_addCommand (menu, L"Select previous interval", GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_SelectPreviousInterval);
	EditorMenu_addCommand (menu, L"Select next interval", GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_SelectNextInterval);
	EditorMenu_addCommand (menu, L"Extend-select left", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_ExtendSelectPreviousInterval);
	EditorMenu_addCommand (menu, L"Extend-select right", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_ExtendSelectNextInterval);
}

void structTextGridEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	if (v_hasAnalysis () && spectrogram.show && (d_longSound.data || d_sound.data)) {
		TextGrid grid = (TextGrid) data;
		double soundY = _TextGridEditor_computeSoundY (this), soundY2 = 0.5 * (1.0 + soundY);
		Graphics_highlight (graphics, left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_highlight (graphics, left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_highlight (graphics, left, right, bottom, top);
	}
}

void structTextGridEditor :: v_unhighlightSelection (double left, double right, double bottom, double top) {
	if (v_hasAnalysis () && spectrogram.show && (d_longSound.data || d_sound.data)) {
		TextGrid grid = (TextGrid) data;
		double soundY = _TextGridEditor_computeSoundY (this), soundY2 = 0.5 * (1.0 + soundY);
		Graphics_unhighlight (graphics, left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_unhighlight (graphics, left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_unhighlight (graphics, left, right, bottom, top);
	}
}

double structTextGridEditor :: v_getBottomOfSoundArea () {
	return _TextGridEditor_computeSoundY (this);
}

double structTextGridEditor :: v_getBottomOfSoundAndAnalysisArea () {
	return _TextGridEditor_computeSoundY (this);
}

void structTextGridEditor :: v_createMenuItems_pitch_picture (EditorMenu menu) {
	TextGridEditor_Parent :: v_createMenuItems_pitch_picture (menu);
	EditorMenu_addCommand (menu, L"Draw visible pitch contour and TextGrid...", 0, menu_cb_DrawTextGridAndPitch);
}

void structTextGridEditor :: v_updateMenuItems_file () {
	TextGridEditor_Parent :: v_updateMenuItems_file ();
	GuiObject_setSensitive (extractSelectedTextGridPreserveTimesButton, endSelection > startSelection);
	GuiObject_setSensitive (extractSelectedTextGridTimeFromZeroButton, endSelection > startSelection);
}

/********** EXPORTED **********/

void structTextGridEditor :: f_init (GuiObject parent, const wchar *title, TextGrid grid, Sampled a_sound, bool a_ownSound, SpellingChecker a_spellingChecker)
{
	this -> spellingChecker = a_spellingChecker;   // set in time

	TimeSoundAnalysisEditor_init (this, parent, title, grid, a_sound, a_ownSound);

	this -> useTextStyles = preferences.useTextStyles;
	this -> fontSize = preferences.fontSize;
	this -> alignment = preferences.alignment;
	this -> shiftDragMultiple = preferences.shiftDragMultiple;
	this -> showNumberOf = preferences.showNumberOf;
	this -> greenMethod = preferences.greenMethod;
	wcscpy (this -> greenString, preferences.greenString);
	this -> selectedTier = 1;
	if (this -> endWindow - this -> startWindow > 30.0) {
		this -> endWindow = this -> startWindow + 30.0;
		if (this -> startWindow == this -> tmin)
			this -> startSelection = this -> endSelection = 0.5 * (this -> startWindow + this -> endWindow);
		FunctionEditor_marksChanged (this);
	}
	if (a_spellingChecker != NULL)
		GuiText_setSelection (this -> text, 0, 0);
}

TextGridEditor TextGridEditor_create (GuiObject parent, const wchar *title, TextGrid grid, Sampled sound, bool ownSound, SpellingChecker spellingChecker) {
	try {
		autoTextGridEditor me = Thing_new (TextGridEditor);
		my f_init (parent, title, grid, sound, ownSound, spellingChecker);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrid window not created.");
	}
}

/* End of file TextGridEditor.cpp */
