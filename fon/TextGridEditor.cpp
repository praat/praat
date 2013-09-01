/* TextGridEditor.cpp
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma
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
#include "SpeechSynthesizer_and_TextGrid.h"

#include "enums_getText.h"
#include "TextGridEditor_enums.h"
#include "enums_getValue.h"
#include "TextGridEditor_enums.h"

Thing_implement (TextGridEditor, TimeSoundAnalysisEditor, 0);

#include "prefs_define.h"
#include "TextGridEditor_prefs.h"
#include "prefs_install.h"
#include "TextGridEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "TextGridEditor_prefs.h"

void structTextGridEditor :: v_info () {
	TextGridEditor_Parent :: v_info ();
	MelderInfo_writeLine (L"Selected tier: ", Melder_integer (selectedTier));
	MelderInfo_writeLine (L"TextGrid uses text styles: ", Melder_boolean (p_useTextStyles));
	MelderInfo_writeLine (L"TextGrid font size: ", Melder_integer (p_fontSize));
	MelderInfo_writeLine (L"TextGrid alignment: ", kGraphics_horizontalAlignment_getText (p_alignment));
}

/********** UTILITIES **********/

static double _TextGridEditor_computeSoundY (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	int numberOfTiers = grid -> tiers -> size;
	bool showAnalysis = my v_hasAnalysis () && (my p_spectrogram_show || my p_pitch_show || my p_intensity_show || my p_formant_show) && (my d_longSound.data || my d_sound.data);
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
			if (t < my d_tmin) {
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
			*tmin = my d_tmin;
			*tmax = my d_tmax;
		} else {
			long ipointleft = AnyTier_timeToLowIndex (textTier, t);
			*tmin = ipointleft == 0 ? my d_tmin : ((TextPoint) textTier -> points -> item [ipointleft]) -> number;
			*tmax = ipointleft == n ? my d_tmax : ((TextPoint) textTier -> points -> item [ipointleft + 1]) -> number;
		}
	}
	if (*tmin < my d_tmin) *tmin = my d_tmin;   // clip by FunctionEditor's time domain
	if (*tmax > my d_tmax) *tmax = my d_tmax;
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
	return IntervalTier_timeToIndex (tier, my d_startSelection);
}

static long getSelectedLeftBoundary (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	IntervalTier tier = (IntervalTier) grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> classInfo == classIntervalTier);
	return IntervalTier_hasBoundary (tier, my d_startSelection);
}

static long getSelectedPoint (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	TextTier tier = (TextTier) grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> classInfo == classTextTier);
	return AnyTier_hasPoint (tier, my d_startSelection);
}

static void scrollToView (TextGridEditor me, double t) {
	if (t <= my d_startWindow) {
		FunctionEditor_shift (me, t - my d_startWindow - 0.618 * (my d_endWindow - my d_startWindow), true);
	} else if (t >= my d_endWindow) {
		FunctionEditor_shift (me, t - my d_endWindow + 0.618 * (my d_endWindow - my d_startWindow), true);
	} else {
		FunctionEditor_marksChanged (me, true);
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
	if (my d_endSelection <= my d_startSelection) Melder_throw ("No selection.");
	autoTextGrid extract = TextGrid_extractPart ((TextGrid) my data, my d_startSelection, my d_endSelection, true);
	my broadcastPublication (extract.transfer());
}

static void menu_cb_ExtractSelectedTextGrid_timeFromZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my d_endSelection <= my d_startSelection) Melder_throw ("No selection.");
	autoTextGrid extract = TextGrid_extractPart ((TextGrid) my data, my d_startSelection, my d_endSelection, false);
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
		BOOLEAN (L"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (L"Garnish", my pref_picture_garnish ());
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_picture_garnish () = GET_INTEGER (L"Garnish");
		Editor_openPraatPicture (me);
		TextGrid_Sound_draw ((TextGrid) my data, NULL, my pictureGraphics, my d_startWindow, my d_endWindow, true, my p_useTextStyles,
			my pref_picture_garnish ());
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
		BOOLEAN (L"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (L"Garnish", my pref_picture_garnish ());
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_picture_garnish () = GET_INTEGER (L"Garnish");
		Editor_openPraatPicture (me);
		Sound publish = my d_longSound.data ?
			LongSound_extractPart (my d_longSound.data, my d_startWindow, my d_endWindow, true) :
			Sound_extractPart (my d_sound.data, my d_startWindow, my d_endWindow, kSound_windowShape_RECTANGULAR, 1.0, true);
		TextGrid_Sound_draw ((TextGrid) my data, publish, my pictureGraphics, my d_startWindow, my d_endWindow, true, my p_useTextStyles, my pref_picture_garnish ());
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
	my text -> f_cut ();
}
static void menu_cb_Copy (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	my text -> f_copy ();
}
static void menu_cb_Paste (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	my text -> f_paste ();
}
static void menu_cb_Erase (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	my text -> f_remove ();
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
		long iinterval = IntervalTier_timeToIndex (tier, my d_startSelection);
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
		long iinterval = IntervalTier_timeToIndex (tier, my d_startSelection);
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
		long iinterval = IntervalTier_timeToIndex (tier, my d_startSelection);
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
		_TextGridEditor_timeToInterval (me, my d_startSelection, my selectedTier, & my d_startSelection, & my d_endSelection);
		FunctionEditor_marksChanged (me, true);
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
			long iinterval = IntervalTier_timeToIndex (intervalTier, my d_startSelection);
			if (shift) {
				long binterval = IntervalTier_timeToIndex (intervalTier, my d_startSelection);
				long einterval = IntervalTier_timeToIndex (intervalTier, my d_endSelection);
				if (my d_endSelection == intervalTier -> xmax) einterval ++;
				if (binterval < iinterval && einterval > iinterval + 1) {
					interval = (TextInterval) intervalTier -> intervals -> item [iinterval];
					my d_startSelection = interval -> xmin;
					my d_endSelection = interval -> xmax;
				} else if (previous) {
					if (einterval > iinterval + 1) {
						if (einterval <= n + 1) {
							interval = (TextInterval) intervalTier -> intervals -> item [einterval - 1];
							my d_endSelection = interval -> xmin;
						}
					} else if (binterval > 1) {
						interval = (TextInterval) intervalTier -> intervals -> item [binterval - 1];
						my d_startSelection = interval -> xmin;
					}
				} else {
					if (binterval < iinterval) {
						if (binterval > 0) {
							interval = (TextInterval) intervalTier -> intervals -> item [binterval];
							my d_startSelection = interval -> xmax;
						}
					} else if (einterval <= n) {
						interval = (TextInterval) intervalTier -> intervals -> item [einterval];
						my d_endSelection = interval -> xmax;
					}
				}
			} else {
				iinterval = previous ?
					iinterval > 1 ? iinterval - 1 : n :
					iinterval < n ? iinterval + 1 : 1;
				interval = (TextInterval) intervalTier -> intervals -> item [iinterval];
				my d_startSelection = interval -> xmin;
				my d_endSelection = interval -> xmax;
			}
			scrollToView (me, iinterval == n ? my d_startSelection : iinterval == 1 ? my d_endSelection : (my d_startSelection + my d_endSelection) / 2);
		}
	} else {
		long n = textTier -> points -> size;
		if (n >= 2) {
			TextPoint point;
			long ipoint = AnyTier_timeToHighIndex (textTier, my d_startSelection);
			ipoint = previous ?
				ipoint > 1 ? ipoint - 1 : n :
				ipoint < n ? ipoint + 1 : 1;
			point = (TextPoint) textTier -> points -> item [ipoint];
			my d_startSelection = my d_endSelection = point -> number;
			scrollToView (me, my d_startSelection);
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
	double zero = Sound_getNearestZeroCrossing (my d_sound.data, my d_startSelection, 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my d_startSelection = zero;
		if (my d_startSelection > my d_endSelection) {
			double dummy = my d_startSelection;
			my d_startSelection = my d_endSelection;
			my d_endSelection = dummy;
		}
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveCursorToZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	double zero = Sound_getNearestZeroCrossing (my d_sound.data, 0.5 * (my d_startSelection + my d_endSelection), 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my d_startSelection = my d_endSelection = zero;
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveEtoZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	double zero = Sound_getNearestZeroCrossing (my d_sound.data, my d_endSelection, 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my d_endSelection = zero;
		if (my d_startSelection > my d_endSelection) {
			double dummy = my d_startSelection;
			my d_startSelection = my d_endSelection;
			my d_endSelection = dummy;
		}
		FunctionEditor_marksChanged (me, true);
	}
}

/***** PITCH MENU *****/

static void menu_cb_DrawTextGridAndPitch (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Draw TextGrid and Pitch separately", 0)
		my v_form_pictureWindow (cmd);
		LABEL (L"", L"TextGrid:")
		BOOLEAN (L"Show boundaries and points", my default_picture_showBoundaries ());
		LABEL (L"", L"Pitch:")
		BOOLEAN (L"Speckle", my default_picture_pitch_speckle ());
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (L"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_INTEGER (L"Show boundaries and points", my pref_picture_showBoundaries ());
		SET_INTEGER (L"Speckle", my pref_picture_pitch_speckle ());
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (L"Garnish", my pref_picture_garnish ());
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my pref_picture_showBoundaries () = GET_INTEGER (L"Show boundaries and points");
		my pref_picture_pitch_speckle () = GET_INTEGER (L"Speckle");
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_picture_garnish () = GET_INTEGER (L"Garnish");
		if (! my p_pitch_show)
			Melder_throw ("No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my d_pitch) Melder_throw ("Cannot compute pitch.");
		}
		Editor_openPraatPicture (me);
		double pitchFloor_hidden = Function_convertStandardToSpecialUnit (my d_pitch, my p_pitch_floor, Pitch_LEVEL_FREQUENCY, my p_pitch_unit);
		double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (my d_pitch, my p_pitch_ceiling, Pitch_LEVEL_FREQUENCY, my p_pitch_unit);
		double pitchFloor_overt = Function_convertToNonlogarithmic (my d_pitch, pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, my p_pitch_unit);
		double pitchCeiling_overt = Function_convertToNonlogarithmic (my d_pitch, pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, my p_pitch_unit);
		double pitchViewFrom_overt = my p_pitch_viewFrom < my p_pitch_viewTo ? my p_pitch_viewFrom : pitchFloor_overt;
		double pitchViewTo_overt = my p_pitch_viewFrom < my p_pitch_viewTo ? my p_pitch_viewTo : pitchCeiling_overt;
		TextGrid_Pitch_drawSeparately ((TextGrid) my data, my d_pitch, my pictureGraphics, my d_startWindow, my d_endWindow,
			pitchViewFrom_overt, pitchViewTo_overt, GET_INTEGER (L"Show boundaries and points"), my p_useTextStyles, GET_INTEGER (L"Garnish"),
			GET_INTEGER (L"Speckle"), my p_pitch_unit);
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
			wchar_t *text = my text -> f_getStringAndSelectionPosition (& left, & right);
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
	my d_startSelection = my d_endSelection = t1;
}

static void do_insertIntervalOnTier (TextGridEditor me, int itier) {
	try {
		insertBoundaryOrPoint (me, itier,
			my playingCursor || my playingSelection ? my playCursor : my d_startSelection,
			my playingCursor || my playingSelection ? my playCursor : my d_endSelection,
			true);
		my selectedTier = itier;
		FunctionEditor_marksChanged (me, true);
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

static void menu_cb_AlignInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, L"align words");
	AnyTier tier = static_cast <AnyTier> (grid -> tiers -> item [my selectedTier]);
	if (tier -> classInfo != classIntervalTier)
		Melder_throw ("Alignment works only for interval tiers, whereas tier ", my selectedTier, " is a point tier.\nSelect an interval tier instead.");
	long intervalNumber = getSelectedInterval (me);
	if (! intervalNumber)
		Melder_throw ("Select an interval first");
	if (! my p_align_includeWords && ! my p_align_includePhonemes)
		Melder_throw ("Nothing to be done.\nPlease switch on \"Include words\" and/or \"Include phonemes\" in the \"Alignment settings\".");
	{// scope
		autoMelderProgressOff noprogress;
		Function anySound = my d_sound.data;
		if (my d_longSound.data) anySound = my d_longSound.data;
		Editor_save (me, L"Align interval");
		TextGrid_anySound_alignInterval (grid, anySound, my selectedTier, intervalNumber,
			my p_align_language, my p_align_includeWords, my p_align_includePhonemes);
	}
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_AlignmentSettings (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Alignment settings", 0)
		OPTIONMENU (L"Language", Strings_findString (espeakdata_voices_names, L"English"))
		for (long i = 1; i <= espeakdata_voices_names -> numberOfStrings; i ++) {
			OPTION ((const wchar_t *) espeakdata_voices_names -> strings [i]);
		}
		BOOLEAN (L"Include words", my default_align_includeWords ())
		BOOLEAN (L"Include phonemes", my default_align_includePhonemes ())
		BOOLEAN (L"Allow silences", my default_align_allowSilences ())
	EDITOR_OK
		long prefVoice = Strings_findString (espeakdata_voices_names, my p_align_language);
		if (prefVoice == 0) prefVoice = Strings_findString (espeakdata_voices_names, L"English");
		SET_INTEGER (L"Language", prefVoice);
		SET_INTEGER (L"Include words", my p_align_includeWords)
		SET_INTEGER (L"Include phonemes", my p_align_includePhonemes)
		SET_INTEGER (L"Allow silences", my p_align_allowSilences)
	EDITOR_DO
		//my pref_align_language () = my p_align_language = GET_ENUM (kTextGrid_language, L"Language");
		pref_wcscpy2 (my pref_align_language (), my p_align_language, GET_STRING (L"Language"));
		my pref_align_includeWords    () = my p_align_includeWords    = GET_INTEGER (L"Include words");
		my pref_align_includePhonemes () = my p_align_includePhonemes = GET_INTEGER (L"Include phonemes");
		my pref_align_allowSilences   () = my p_align_allowSilences   = GET_INTEGER (L"Allow silences");
	EDITOR_END
}

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
		position = where == 1 ? my d_startSelection : where == 2 ? my d_endSelection :
			Sound_getNearestZeroCrossing (my d_sound.data, left -> xmax, 1);   // STEREO BUG
		if (position == NUMundefined)
			Melder_throw ("There is no zero crossing to move to.");
		if (position <= left -> xmin || position >= right -> xmax)
			Melder_throw ("Cannot move a boundary past its neighbour.");

		Editor_save (me, boundarySaveText [where]);

		left -> xmax = right -> xmin = my d_startSelection = my d_endSelection = position;
	} else {
		TextTier tier = (TextTier) anyTier;
		static const wchar_t *pointSaveText [3] = { L"Move point to zero crossing", L"Move point to B", L"Move point to E" };
		TextPoint point;
		long selectedPoint = getSelectedPoint (me);
		if (! selectedPoint)
			Melder_throw ("To move a point, first click on it.");
		point = (TextPoint) tier -> points -> item [selectedPoint];
		position = where == 1 ? my d_startSelection : where == 2 ? my d_endSelection :
			Sound_getNearestZeroCrossing (my d_sound.data, point -> number, 1);   // STEREO BUG
		if (position == NUMundefined)
			Melder_throw ("There is no zero crossing to move to.");

		Editor_save (me, pointSaveText [where]);

		point -> number = my d_startSelection = my d_endSelection = position;
	}
	FunctionEditor_marksChanged (me, true);   // because cursor has moved
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
			my playingCursor || my playingSelection ? my playCursor : my d_startSelection,
			my playingCursor || my playingSelection ? my playCursor : my d_endSelection,
			false);
		my selectedTier = itier;
		FunctionEditor_marksChanged (me, true);
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
		long iinterval = IntervalTier_timeToIndex (tier, my d_startSelection) + 1;
		while (iinterval <= tier -> intervals -> size) {
			TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
			wchar_t *text = interval -> text;
			if (text) {
				wchar_t *position = wcsstr (text, my findString);
				if (position) {
					my d_startSelection = interval -> xmin;
					my d_endSelection = interval -> xmax;
					scrollToView (me, my d_startSelection);
					my text -> f_setSelection (position - text, position - text + wcslen (my findString));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals -> size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		long ipoint = AnyTier_timeToLowIndex (tier, my d_startSelection) + 1;
		while (ipoint <= tier -> points -> size) {
			TextPoint point = (TextPoint) tier -> points -> item [ipoint];
			wchar_t *text = point -> mark;
			if (text) {
				wchar_t *position = wcsstr (text, my findString);
				if (position) {
					my d_startSelection = my d_endSelection = point -> number;
					scrollToView (me, point -> number);
					my text -> f_setSelection (position - text, position - text + wcslen (my findString));
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
		autostring label = my text -> f_getStringAndSelectionPosition (& left, & right);
		wchar_t *position = wcsstr (label.peek() + right, my findString);   // CRLF BUG?
		if (position) {
			my text -> f_setSelection (position - label.peek(), position - label.peek() + wcslen (my findString));
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
		long iinterval = IntervalTier_timeToIndex (tier, my d_startSelection) + 1;
		while (iinterval <= tier -> intervals -> size) {
			TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
			wchar_t *text = interval -> text;
			if (text) {
				long position = 0;
				wchar_t *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my d_startSelection = interval -> xmin;
					my d_endSelection = interval -> xmax;
					scrollToView (me, my d_startSelection);
					my text -> f_setSelection (position, position + wcslen (notAllowed));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals -> size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		long ipoint = AnyTier_timeToLowIndex (tier, my d_startSelection) + 1;
		while (ipoint <= tier -> points -> size) {
			TextPoint point = (TextPoint) tier -> points -> item [ipoint];
			wchar_t *text = point -> mark;
			if (text) {
				long position = 0;
				wchar_t *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my d_startSelection = my d_endSelection = point -> number;
					scrollToView (me, point -> number);
					my text -> f_setSelection (position, position + wcslen (notAllowed));
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
		autostring label = my text -> f_getStringAndSelectionPosition (& left, & right);
		long position = right;
		wchar_t *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.peek(), & position);
		if (notAllowed) {
			my text -> f_setSelection (position, position + wcslen (notAllowed));
		} else {
			checkSpellingInTier (me);
		}
	}
}

static void menu_cb_CheckSpellingInInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my spellingChecker) {
		long left, right;
		autostring label = my text -> f_getStringAndSelectionPosition (& left, & right);
		long position = right;
		wchar_t *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.peek(), & position);
		if (notAllowed) {
			my text -> f_setSelection (position, position + wcslen (notAllowed));
		}
	}
}

static void menu_cb_AddToUserDictionary (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my spellingChecker) {
		autostring word = my text -> f_getSelection ();
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

		wchar_t *newName = GET_STRING (L"Name");
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
		wchar_t *name = GET_STRING (L"Name");
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
	if (d_sound.data || d_longSound.data) {
		EditorMenu_addCommand (menu, L"Align interval", 'D', menu_cb_AlignInterval);
		EditorMenu_addCommand (menu, L"Alignment settings...", 0, menu_cb_AlignmentSettings);
		EditorMenu_addCommand (menu, L"-- add interval --", 0, NULL);
	}
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
	if (d_sound.data) {
		EditorMenu_addCommand (menu, L"Move to nearest zero crossing", 0, menu_cb_MoveToZero);
		EditorMenu_addCommand (menu, L"-- insert boundary --", 0, NULL);
	}
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
		wchar_t *text = my text -> f_getString ();
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
	text -> f_setChangeCallback (gui_text_cb_change, this);
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
			LongSound_haveWindow (d_longSound.data, d_startWindow, d_endWindow);
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
	Graphics_WCtoDC (my d_graphics, my d_startWindow, 0.0, & x1DC, & yDC);
	Graphics_WCtoDC (my d_graphics, my d_endWindow, 0.0, & x2DC, & yDC);
	Graphics_setPercentSignIsItalic (my d_graphics, my p_useTextStyles);
	Graphics_setNumberSignIsBold (my d_graphics, my p_useTextStyles);
	Graphics_setCircumflexIsSuperscript (my d_graphics, my p_useTextStyles);
	Graphics_setUnderscoreIsSubscript (my d_graphics, my p_useTextStyles);

	/*
	 * Highlight interval: yellow (selected) or green (matching label).
	 */
	
	for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
		TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		if (tmax > my d_startWindow && tmin < my d_endWindow) {   // interval visible?
			int intervalIsSelected = iinterval == selectedInterval;
			int labelMatches = Melder_stringMatchesCriterion (interval -> text, my p_greenMethod, my p_greenString);
			if (tmin < my d_startWindow) tmin = my d_startWindow;
			if (tmax > my d_endWindow) tmax = my d_endWindow;
			if (labelMatches) {
				Graphics_setColour (my d_graphics, Graphics_LIME);
				Graphics_fillRectangle (my d_graphics, tmin, tmax, 0.0, 1.0);
			}
			if (intervalIsSelected) {
				if (labelMatches) {
					tmin = 0.85 * tmin + 0.15 * tmax;
					tmax = 0.15 * tmin + 0.85 * tmax;
				}
				Graphics_setColour (my d_graphics, Graphics_YELLOW);
				Graphics_fillRectangle (my d_graphics, tmin, tmax, labelMatches ? 0.15 : 0.0, labelMatches? 0.85: 1.0);
			}
		}
	}
	Graphics_setColour (my d_graphics, Graphics_BLACK);
	Graphics_line (my d_graphics, my d_endWindow, 0.0, my d_endWindow, 1.0);

	/*
	 * Draw a grey bar and a selection button at the cursor position.
	 */
	if (my d_startSelection == my d_endSelection && my d_startSelection >= my d_startWindow && my d_startSelection <= my d_endWindow) {
		int cursorAtBoundary = FALSE;
		for (iinterval = 2; iinterval <= ninterval; iinterval ++) {
			TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
			if (interval -> xmin == my d_startSelection) cursorAtBoundary = TRUE;
		}
		if (! cursorAtBoundary) {
			double dy = Graphics_dyMMtoWC (my d_graphics, 1.5);
			Graphics_setGrey (my d_graphics, 0.8);
			Graphics_setLineWidth (my d_graphics, platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my d_graphics, my d_startSelection, 0.0, my d_startSelection, 1.0);
			Graphics_setLineWidth (my d_graphics, 1.0);
			Graphics_setColour (my d_graphics, Graphics_BLUE);
			Graphics_circle_mm (my d_graphics, my d_startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my d_graphics, my p_alignment, Graphics_HALF);
	for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
		TextInterval interval = (TextInterval) tier -> intervals -> item [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		if (tmin < my d_tmin) tmin = my d_tmin; if (tmax > my d_tmax) tmax = my d_tmax;
		if (tmin >= tmax) continue;
		bool intervalIsSelected = selectedInterval == iinterval;

		/*
		 * Draw left boundary.
		 */
		if (tmin >= my d_startWindow && tmin <= my d_endWindow && iinterval > 1) {
			bool boundaryIsSelected = ( my selectedTier == itier && tmin == my d_startSelection );
			Graphics_setColour (my d_graphics, boundaryIsSelected ? Graphics_RED : Graphics_BLUE);
			Graphics_setLineWidth (my d_graphics, platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my d_graphics, tmin, 0.0, tmin, 1.0);

			/*
			 * Show alignment with cursor.
			 */
			if (tmin == my d_startSelection) {
				Graphics_setColour (my d_graphics, Graphics_YELLOW);
				Graphics_setLineWidth (my d_graphics, platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my d_graphics, tmin, 0.0, tmin, 1.0);
			}
		}
		Graphics_setLineWidth (my d_graphics, 1.0);

		/*
		 * Draw label text.
		 */
		if (interval -> text && tmax >= my d_startWindow && tmin <= my d_endWindow) {
			double t1 = my d_startWindow > tmin ? my d_startWindow : tmin;
			double t2 = my d_endWindow < tmax ? my d_endWindow : tmax;
			Graphics_setColour (my d_graphics, intervalIsSelected ? Graphics_RED : Graphics_BLACK);
			Graphics_textRect (my d_graphics, t1, t2, 0.0, 1.0, interval -> text);
			Graphics_setColour (my d_graphics, Graphics_BLACK);
		}

	}
	Graphics_setPercentSignIsItalic (my d_graphics, TRUE);
	Graphics_setNumberSignIsBold (my d_graphics, TRUE);
	Graphics_setCircumflexIsSuperscript (my d_graphics, TRUE);
	Graphics_setUnderscoreIsSubscript (my d_graphics, TRUE);
}

static void do_drawTextTier (TextGridEditor me, TextTier tier, int itier) {
	#if gtk || defined (macintosh)
		bool platformUsesAntiAliasing = true;
	#else
		bool platformUsesAntiAliasing = false;
	#endif
	int ipoint, npoint = tier -> points -> size;
	Graphics_setPercentSignIsItalic (my d_graphics, my p_useTextStyles);
	Graphics_setNumberSignIsBold (my d_graphics, my p_useTextStyles);
	Graphics_setCircumflexIsSuperscript (my d_graphics, my p_useTextStyles);
	Graphics_setUnderscoreIsSubscript (my d_graphics, my p_useTextStyles);

	/*
	 * Draw a grey bar and a selection button at the cursor position.
	 */
	if (my d_startSelection == my d_endSelection && my d_startSelection >= my d_startWindow && my d_startSelection <= my d_endWindow) {
		int cursorAtPoint = FALSE;
		for (ipoint = 1; ipoint <= npoint; ipoint ++) {
			TextPoint point = (TextPoint) tier -> points -> item [ipoint];
			if (point -> number == my d_startSelection) cursorAtPoint = TRUE;
		}
		if (! cursorAtPoint) {
			double dy = Graphics_dyMMtoWC (my d_graphics, 1.5);
			Graphics_setGrey (my d_graphics, 0.8);
			Graphics_setLineWidth (my d_graphics, platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my d_graphics, my d_startSelection, 0.0, my d_startSelection, 1.0);
			Graphics_setLineWidth (my d_graphics, 1.0);
			Graphics_setColour (my d_graphics, Graphics_BLUE);
			Graphics_circle_mm (my d_graphics, my d_startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my d_graphics, Graphics_CENTRE, Graphics_HALF);
	for (ipoint = 1; ipoint <= npoint; ipoint ++) {
		TextPoint point = (TextPoint) tier -> points -> item [ipoint];
		double t = point -> number;
		if (t >= my d_startWindow && t <= my d_endWindow) {
			bool pointIsSelected = ( itier == my selectedTier && t == my d_startSelection );
			Graphics_setColour (my d_graphics, pointIsSelected ? Graphics_RED : Graphics_BLUE);
			Graphics_setLineWidth (my d_graphics, platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my d_graphics, t, 0.0, t, 0.2);
			Graphics_line (my d_graphics, t, 0.8, t, 1);
			Graphics_setLineWidth (my d_graphics, 1.0);

			/*
			 * Wipe out the cursor where the text is going to be.
			 */
			Graphics_setColour (my d_graphics, Graphics_WHITE);
			Graphics_line (my d_graphics, t, 0.2, t, 0.8);

			/*
			 * Show alignment with cursor.
			 */
			if (my d_startSelection == my d_endSelection && t == my d_startSelection) {
				Graphics_setColour (my d_graphics, Graphics_YELLOW);
				Graphics_setLineWidth (my d_graphics, platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my d_graphics, t, 0.0, t, 0.2);
				Graphics_line (my d_graphics, t, 0.8, t, 1.0);
			}
			Graphics_setColour (my d_graphics, pointIsSelected ? Graphics_RED : Graphics_BLUE);
			if (point -> mark) Graphics_text (my d_graphics, t, 0.5, point -> mark);
		}
	}
	Graphics_setPercentSignIsItalic (my d_graphics, TRUE);
	Graphics_setNumberSignIsBold (my d_graphics, TRUE);
	Graphics_setCircumflexIsSuperscript (my d_graphics, TRUE);
	Graphics_setUnderscoreIsSubscript (my d_graphics, TRUE);
}

void structTextGridEditor :: v_draw () {
	TextGrid grid = (TextGrid) data;
	Graphics_Viewport vp1, vp2;
	long itier, ntier = grid -> tiers -> size;
	enum kGraphics_font oldFont = Graphics_inqFont (d_graphics);
	int oldFontSize = Graphics_inqFontSize (d_graphics);
	bool showAnalysis = v_hasAnalysis () && (p_spectrogram_show || p_pitch_show || p_intensity_show || p_formant_show) && (d_longSound.data || d_sound.data);
	double soundY = _TextGridEditor_computeSoundY (this), soundY2 = showAnalysis ? 0.5 * (1.0 + soundY) : soundY;

	/*
	 * Draw optional sound.
	 */
	if (d_longSound.data || d_sound.data) {
		vp1 = Graphics_insetViewport (d_graphics, 0.0, 1.0, soundY2, 1.0);
		Graphics_setColour (d_graphics, Graphics_WHITE);
		Graphics_setWindow (d_graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (d_graphics, 0, 1, 0, 1);
		f_drawSound (-1.0, 1.0);
		Graphics_flushWs (d_graphics);
		Graphics_resetViewport (d_graphics, vp1);
	}

	/*
	 * Draw tiers.
	 */
	if (d_longSound.data || d_sound.data) vp1 = Graphics_insetViewport (d_graphics, 0.0, 1.0, 0.0, soundY);
	Graphics_setColour (d_graphics, Graphics_WHITE);
	Graphics_setWindow (d_graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (d_graphics, 0, 1, 0, 1);
	Graphics_setColour (d_graphics, Graphics_BLACK);
	Graphics_rectangle (d_graphics, 0, 1, 0, 1);
	Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, 0.0, 1.0);
	for (itier = 1; itier <= ntier; itier ++) {
		Function anyTier = (Function) grid -> tiers -> item [itier];
		bool tierIsSelected = itier == selectedTier;
		bool isIntervalTier = anyTier -> classInfo == classIntervalTier;
		vp2 = Graphics_insetViewport (d_graphics, 0.0, 1.0,
			1.0 - (double) itier / (double) ntier,
			1.0 - (double) (itier - 1) / (double) ntier);
		Graphics_setColour (d_graphics, Graphics_BLACK);
		if (itier != 1) Graphics_line (d_graphics, d_startWindow, 1.0, d_endWindow, 1.0);

		/*
		 * Show the number and the name of the tier.
		 */
		Graphics_setColour (d_graphics, tierIsSelected ? Graphics_RED : Graphics_BLACK);
		Graphics_setFont (d_graphics, oldFont);
		Graphics_setFontSize (d_graphics, 14);
		Graphics_setTextAlignment (d_graphics, Graphics_RIGHT, Graphics_HALF);
		Graphics_text2 (d_graphics, d_startWindow, 0.5, tierIsSelected ? L"\\pf " : L"", Melder_integer (itier));
		Graphics_setFontSize (d_graphics, oldFontSize);
		if (anyTier -> name && anyTier -> name [0]) {
			Graphics_setTextAlignment (d_graphics, Graphics_LEFT,
				p_showNumberOf == kTextGridEditor_showNumberOf_NOTHING ? Graphics_HALF : Graphics_BOTTOM);
			Graphics_text (d_graphics, d_endWindow, 0.5, anyTier -> name);
		}
		if (p_showNumberOf != kTextGridEditor_showNumberOf_NOTHING) {
			Graphics_setTextAlignment (d_graphics, Graphics_LEFT, Graphics_TOP);
			if (p_showNumberOf == kTextGridEditor_showNumberOf_INTERVALS_OR_POINTS) {
				long count = isIntervalTier ? ((IntervalTier) anyTier) -> intervals -> size : ((TextTier) anyTier) -> points -> size;
				long position = itier == selectedTier ? ( isIntervalTier ? getSelectedInterval (this) : getSelectedPoint (this) ) : 0;
				if (position) {
					Graphics_text5 (d_graphics, d_endWindow, 0.5, L"(", Melder_integer (position), L"/", Melder_integer (count), L")");
				} else {
					Graphics_text3 (d_graphics, d_endWindow, 0.5, L"(", Melder_integer (count), L")");
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
				Graphics_text3 (d_graphics, d_endWindow, 0.5, L"(##", Melder_integer (count), L"#)");
			}
		}

		Graphics_setColour (d_graphics, Graphics_BLACK);
		Graphics_setFont (d_graphics, kGraphics_font_TIMES);
		Graphics_setFontSize (d_graphics, p_fontSize);
		if (isIntervalTier)
			do_drawIntervalTier (this, (IntervalTier) anyTier, itier);
		else
			do_drawTextTier (this, (TextTier) anyTier, itier);
		Graphics_resetViewport (d_graphics, vp2);
	}
	Graphics_setColour (d_graphics, Graphics_BLACK);
	Graphics_setFont (d_graphics, oldFont);
	Graphics_setFontSize (d_graphics, oldFontSize);
	if (d_longSound.data || d_sound.data) Graphics_resetViewport (d_graphics, vp1);
	Graphics_flushWs (d_graphics);

	if (showAnalysis) {
		vp1 = Graphics_insetViewport (d_graphics, 0.0, 1.0, soundY, soundY2);
		v_draw_analysis ();
		Graphics_flushWs (d_graphics);
		Graphics_resetViewport (d_graphics, vp1);
		/* Draw pulses. */
		if (p_pulses_show) {
			vp1 = Graphics_insetViewport (d_graphics, 0.0, 1.0, soundY2, 1.0);
			v_draw_analysis_pulses ();
			f_drawSound (-1.0, 1.0);   // second time, partially across the pulses
			Graphics_flushWs (d_graphics);
			Graphics_resetViewport (d_graphics, vp1);
		}
	}
	Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, 0.0, 1.0);
	if (d_longSound.data || d_sound.data) {
		Graphics_line (d_graphics, d_startWindow, soundY, d_endWindow, soundY);
		if (showAnalysis) {
			Graphics_line (d_graphics, d_startWindow, soundY2, d_endWindow, soundY2);
			Graphics_line (d_graphics, d_startWindow, soundY, d_startWindow, soundY2);
			Graphics_line (d_graphics, d_endWindow, soundY, d_endWindow, soundY2);
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
		Graphics_setLineWidth (my d_graphics, 7);
		Graphics_line (my d_graphics, x, ymin, x, ymax);
	}
	Graphics_setLineWidth (my d_graphics, 1);
	Graphics_line (my d_graphics, x, 0, x, 1.01);
	Graphics_text1 (my d_graphics, x, 1.01, Melder_fixed (x, 6));
}

static void do_dragBoundary (TextGridEditor me, double xbegin, int iClickedTier, int shiftKeyPressed) {
	TextGrid grid = (TextGrid) my data;
	int itier, numberOfTiers = grid -> tiers -> size, itierDrop;
	double xWC = xbegin, yWC;
	double leftDraggingBoundary = my d_tmin, rightDraggingBoundary = my d_tmax;   // initial dragging range
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
		if (itier == iClickedTier || shiftKeyPressed == my p_shiftDragMultiple) {
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

	Graphics_xorOn (my d_graphics, Graphics_MAROON);
	Graphics_setTextAlignment (my d_graphics, Graphics_CENTRE, Graphics_BOTTOM);
	do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // draw at old position
	while (Graphics_mouseStillDown (my d_graphics)) {
		double xWC_new;
		Graphics_getMouseLocation (my d_graphics, & xWC_new, & yWC);
		if (xWC_new != xWC) {
			do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // undraw at old position
			xWC = xWC_new;
			do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // draw at new position
		}
	}
	do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // undraw at new position
	Graphics_xorOff (my d_graphics);

	/*
	 * The simplest way to cancel the dragging operation, is to drag outside the window.
	 */
	if (xWC <= my d_startWindow || xWC >= my d_endWindow) {
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
				if (fabs (Graphics_dxWCtoMM (my d_graphics, xWC - left -> xmax)) < 1.5) {   // near a boundary?
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
				if (fabs (Graphics_dxWCtoMM (my d_graphics, xWC - point -> number)) < 1.5) {   // near a point?
					/*
					 * Snap to point.
					 */
					xWC = point -> number;
				}
			}
		}
	} else if (xbegin != my d_startSelection && fabs (Graphics_dxWCtoMM (my d_graphics, xWC - my d_startSelection)) < 1.5) {   // near the cursor?
		/*
		 * Snap to cursor.
		 */
		xWC = my d_startSelection;
	} else if (xbegin != my d_endSelection && fabs (Graphics_dxWCtoMM (my d_graphics, xWC - my d_endSelection)) < 1.5) {   // near the cursor?
		/*
		 * Snap to cursor.
		 */
		xWC = my d_endSelection;
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
	if (my d_startSelection == xbegin)
		my d_startSelection = xWC;
	if (my d_endSelection == xbegin)
		my d_endSelection = xWC;
	if (my d_startSelection > my d_endSelection) {
		double dummy = my d_startSelection;
		my d_startSelection = my d_endSelection;
		my d_endSelection = dummy;
	}
	FunctionEditor_marksChanged (me, true);
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
		if ((p_spectrogram_show || p_formant_show) && yWC < 0.5 * (soundY + 1.0)) {
			d_spectrogram_cursor = p_spectrogram_viewFrom +
				2.0 * (yWC - soundY) / (1.0 - soundY) * (p_spectrogram_viewTo - p_spectrogram_viewFrom);
		}
		TextGridEditor_Parent :: v_click (xclick, yWC, shiftKeyPressed);
		return FunctionEditor_UPDATE_NEEDED;
	}

	/*
	 * She clicked in the grid part.
	 * We select the tier in which she clicked.
	 */
	iClickedTier = _TextGridEditor_yWCtoTier (this, yWC);

	if (xclick <= d_startWindow || xclick >= d_endWindow) {
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
	nearBoundaryOrPoint = tnear != NUMundefined && fabs (Graphics_dxWCtoMM (d_graphics, xclick - tnear)) < 1.5;
	nearCursorCircle = d_startSelection == d_endSelection && Graphics_distanceWCtoMM (d_graphics, xclick, yWC,
		d_startSelection, (ntiers + 1 - iClickedTier) * soundY / ntiers - Graphics_dyMMtoWC (d_graphics, 1.5)) < 1.5;

	/*
	 * Find out whether this is a click or a drag.
	 */
	while (Graphics_mouseStillDown (d_graphics)) {
		Graphics_getMouseLocation (d_graphics, & x, & y);
		if (x < d_startWindow) x = d_startWindow;
		if (x > d_endWindow) x = d_endWindow;
		if (fabs (Graphics_dxWCtoMM (d_graphics, x - xclick)) > 1.5) {
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
				if (tnear > 0.5 * (d_startSelection + d_endSelection))
					d_endSelection = tnear;
				else
					d_startSelection = tnear;
			} else {
				d_startSelection = d_endSelection = tnear;   /* Move cursor so that the boundary or point is selected. */
			}
			selectedTier = iClickedTier;
		}
	} else if (nearCursorCircle) {
		/*
		 * Possibility 2: she clicked near the cursor circle.
		 * Insert boundary or point. There is no danger that we insert on top of an existing boundary or point,
		 * because we are not 'nearBoundaryOrPoint'.
		 */
		insertBoundaryOrPoint (this, iClickedTier, d_startSelection, d_startSelection, false);
		selectedTier = iClickedTier;
		FunctionEditor_marksChanged (this, true);
		broadcastDataChanged ();
		if (drag) Graphics_waitMouseUp (d_graphics);
		return FunctionEditor_NO_UPDATE_NEEDED;
	} else {
		/*
		 * Possibility 3: she clicked in empty space.
		 */
		if (intervalTier) {
			d_startSelection = tmin;
			d_endSelection = tmax;
		}
		selectedTier = iClickedTier;
	}
	if (drag) Graphics_waitMouseUp (d_graphics);
	return FunctionEditor_UPDATE_NEEDED;
}

int structTextGridEditor :: v_clickB (double t, double yWC) {
	double soundY = _TextGridEditor_computeSoundY (this);

	if (yWC > soundY) {   // clicked in sound part?
		d_startSelection = t;
		if (d_startSelection > d_endSelection) {
			double dummy = d_startSelection;
			d_startSelection = d_endSelection;
			d_endSelection = dummy;
		}
		return FunctionEditor_UPDATE_NEEDED;
	}
	int itier = _TextGridEditor_yWCtoTier (this, yWC);
	double tmin, tmax;
	_TextGridEditor_timeToInterval (this, t, itier, & tmin, & tmax);
	d_startSelection = t - tmin < tmax - t ? tmin : tmax;   // to nearest boundary
	if (d_startSelection > d_endSelection) {
		double dummy = d_startSelection;
		d_startSelection = d_endSelection;
		d_endSelection = dummy;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

int structTextGridEditor :: v_clickE (double t, double yWC) {
	double soundY = _TextGridEditor_computeSoundY (this);

	if (yWC > soundY) {   // clicked in sound part?
		d_endSelection = t;
		if (d_startSelection > d_endSelection) {
			double dummy = d_startSelection;
			d_startSelection = d_endSelection;
			d_endSelection = dummy;
		}
		return FunctionEditor_UPDATE_NEEDED;
	}
	int itier = _TextGridEditor_yWCtoTier (this, yWC);
	double tmin, tmax;
	_TextGridEditor_timeToInterval (this, t, itier, & tmin, & tmax);
	d_endSelection = t - tmin < tmax - t ? tmin : tmax;
	if (d_startSelection > d_endSelection) {
		double dummy = d_startSelection;
		d_startSelection = d_endSelection;
		d_endSelection = dummy;
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
	const wchar_t *newText = L"";
	trace ("selected tier %ld", selectedTier);
	if (selectedTier) {
		IntervalTier intervalTier;
		TextTier textTier;
		_AnyTier_identifyClass ((Function) grid -> tiers -> item [selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			long iinterval = IntervalTier_timeToIndex (intervalTier, d_startSelection);
			if (iinterval) {
				TextInterval interval = (TextInterval) intervalTier -> intervals -> item [iinterval];
				if (interval -> text) {
					newText = interval -> text;
				}
			}
		} else {
			long ipoint = AnyTier_hasPoint (textTier, d_startSelection);
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
	trace ("setting new text %ls", newText);
	text -> f_setString (newText);
	long cursor = wcslen (newText);   // at end
	text -> f_setSelection (cursor, cursor);
	suppressRedraw = FALSE;
}

void structTextGridEditor :: v_prefs_addFields (EditorCommand cmd) {
	Any radio;
	NATURAL (L"Font size (points)", default_fontSize ())
	OPTIONMENU_ENUM (L"Text alignment in intervals", kGraphics_horizontalAlignment, kGraphics_horizontalAlignment_DEFAULT)
	OPTIONMENU (L"The symbols %#_^ in labels", default_useTextStyles () + 1)
		OPTION (L"are shown as typed")
		OPTION (L"mean italic/bold/sub/super")
	OPTIONMENU (L"With the shift key, you drag", default_shiftDragMultiple () + 1)
		OPTION (L"a single boundary")
		OPTION (L"multiple boundaries")
	OPTIONMENU_ENUM (L"Show number of", kTextGridEditor_showNumberOf, kTextGridEditor_showNumberOf_DEFAULT)
	OPTIONMENU_ENUM (L"Paint intervals green whose label...", kMelder_string, kMelder_string_DEFAULT)
	SENTENCE (L"...the text", default_greenString ())
}
void structTextGridEditor :: v_prefs_setValues (EditorCommand cmd) {
	SET_INTEGER (L"The symbols %#_^ in labels", p_useTextStyles + 1)
	SET_INTEGER (L"Font size", p_fontSize)
	SET_ENUM (L"Text alignment in intervals", kGraphics_horizontalAlignment, p_alignment)
	SET_INTEGER (L"With the shift key, you drag", p_shiftDragMultiple + 1)
	SET_ENUM (L"Show number of", kTextGridEditor_showNumberOf, p_showNumberOf)
	SET_ENUM (L"Paint intervals green whose label...", kMelder_string, p_greenMethod)
	SET_STRING (L"...the text", p_greenString)
}
void structTextGridEditor :: v_prefs_getValues (EditorCommand cmd) {
	pref_useTextStyles () = p_useTextStyles = GET_INTEGER (L"The symbols %#_^ in labels") - 1;
	pref_fontSize () = p_fontSize = GET_INTEGER (L"Font size");
	pref_alignment () = p_alignment = GET_ENUM (kGraphics_horizontalAlignment, L"Text alignment in intervals");
	pref_shiftDragMultiple () = p_shiftDragMultiple = GET_INTEGER (L"With the shift key, you drag") - 1;
	pref_showNumberOf () = p_showNumberOf = GET_ENUM (kTextGridEditor_showNumberOf, L"Show number of");
	pref_greenMethod () = p_greenMethod = GET_ENUM (kMelder_string, L"Paint intervals green whose label...");
	pref_wcscpy2 (pref_greenString (), p_greenString, GET_STRING (L"...the text"));
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
	if (v_hasAnalysis () && p_spectrogram_show && (d_longSound.data || d_sound.data)) {
		TextGrid grid = (TextGrid) data;
		double soundY = _TextGridEditor_computeSoundY (this), soundY2 = 0.5 * (1.0 + soundY);
		Graphics_highlight (d_graphics, left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_highlight (d_graphics, left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_highlight (d_graphics, left, right, bottom, top);
	}
}

void structTextGridEditor :: v_unhighlightSelection (double left, double right, double bottom, double top) {
	if (v_hasAnalysis () && p_spectrogram_show && (d_longSound.data || d_sound.data)) {
		TextGrid grid = (TextGrid) data;
		double soundY = _TextGridEditor_computeSoundY (this), soundY2 = 0.5 * (1.0 + soundY);
		Graphics_unhighlight (d_graphics, left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_unhighlight (d_graphics, left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_unhighlight (d_graphics, left, right, bottom, top);
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
	extractSelectedTextGridPreserveTimesButton -> f_setSensitive (d_endSelection > d_startSelection);
	extractSelectedTextGridTimeFromZeroButton  -> f_setSensitive (d_endSelection > d_startSelection);
}

/********** EXPORTED **********/

void structTextGridEditor :: f_init (const wchar_t *title, TextGrid grid, Sampled a_sound, bool a_ownSound, SpellingChecker a_spellingChecker)
{
	this -> spellingChecker = a_spellingChecker;   // set in time

	structTimeSoundAnalysisEditor :: f_init (title, grid, a_sound, a_ownSound);

	this -> selectedTier = 1;
	v_updateText ();   // to reflect changed tier selection
	if (d_endWindow - d_startWindow > 30.0) {
		d_endWindow = d_startWindow + 30.0;
		if (d_startWindow == d_tmin)
			d_startSelection = d_endSelection = 0.5 * (d_startWindow + d_endWindow);
		FunctionEditor_marksChanged (this, false);
	}
	if (a_spellingChecker != NULL)
		this -> text -> f_setSelection (0, 0);
}

TextGridEditor TextGridEditor_create (const wchar_t *title, TextGrid grid, Sampled sound, bool ownSound, SpellingChecker spellingChecker) {
	try {
		autoTextGridEditor me = Thing_new (TextGridEditor);
		my f_init (title, grid, sound, ownSound, spellingChecker);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrid window not created.");
	}
}

/* End of file TextGridEditor.cpp */
