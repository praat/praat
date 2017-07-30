/* TextGridEditor.cpp
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
/* Erez Volk added FLAC support in 2007 */

#include "TextGridEditor.h"
#include "EditorM.h"
#include "SoundEditor.h"
#include "Sound_and_MixingMatrix.h"
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
	MelderInfo_writeLine (U"Selected tier: ", selectedTier);
	MelderInfo_writeLine (U"TextGrid uses text styles: ", p_useTextStyles);
	MelderInfo_writeLine (U"TextGrid font size: ", p_fontSize);
	MelderInfo_writeLine (U"TextGrid alignment: ", kGraphics_horizontalAlignment_getText (p_alignment));
}

/********** UTILITIES **********/

static double _TextGridEditor_computeSoundY (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	int numberOfTiers = grid -> tiers->size;
	bool showAnalysis = my v_hasAnalysis () && (my p_spectrogram_show || my p_pitch_show || my p_intensity_show || my p_formant_show) && (my d_longSound.data || my d_sound.data);
	int numberOfVisibleChannels = my d_sound.data ? (my d_sound.data -> ny > 8 ? 8 : my d_sound.data -> ny) :
		my d_longSound.data ? (my d_longSound.data -> numberOfChannels > 8 ? 8 : my d_longSound.data -> numberOfChannels) : 1;
	return my d_sound.data || my d_longSound.data ? numberOfTiers / (2.0 * numberOfVisibleChannels + numberOfTiers * (showAnalysis ? 1.8 : 1.3)) : 1.0;
}

static void _AnyTier_identifyClass (Function anyTier, IntervalTier *intervalTier, TextTier *textTier) {
	if (anyTier -> classInfo == classIntervalTier) {
		*intervalTier = (IntervalTier) anyTier;
		*textTier = nullptr;
	} else {
		*intervalTier = nullptr;
		*textTier = (TextTier) anyTier;
	}
}

static int _TextGridEditor_yWCtoTier (TextGridEditor me, double yWC) {
	TextGrid grid = (TextGrid) my data;
	int ntier = grid -> tiers->size;
	double soundY = _TextGridEditor_computeSoundY (me);
	int itier = ntier - (int) floor (yWC / soundY * (double) ntier);
	if (itier < 1) itier = 1; if (itier > ntier) itier = ntier;
	return itier;
}

static void _TextGridEditor_timeToInterval (TextGridEditor me, double t, int itier, double *tmin, double *tmax) {
	TextGrid grid = (TextGrid) my data;
	Function tier = grid -> tiers->at [itier];
	IntervalTier intervalTier;
	TextTier textTier;
	_AnyTier_identifyClass (tier, & intervalTier, & textTier);
	if (intervalTier) {
		long iinterval = IntervalTier_timeToIndex (intervalTier, t);
		TextInterval interval;
		if (iinterval == 0) {
			if (t < my tmin) {
				iinterval = 1;
			} else {
				iinterval = intervalTier -> intervals.size;
			}
		}
		Melder_assert (iinterval >= 1);
		Melder_assert (iinterval <= intervalTier -> intervals.size);
		interval = intervalTier -> intervals.at [iinterval];
		*tmin = interval -> xmin;
		*tmax = interval -> xmax;
	} else {
		long n = textTier -> points.size;
		if (n == 0) {
			*tmin = my tmin;
			*tmax = my tmax;
		} else {
			long ipointleft = AnyTier_timeToLowIndex (textTier->asAnyTier(), t);
			*tmin = ipointleft == 0 ? my tmin : textTier -> points.at [ipointleft] -> number;
			*tmax = ipointleft == n ? my tmax : textTier -> points.at [ipointleft + 1] -> number;
		}
	}
	if (*tmin < my tmin) *tmin = my tmin;   // clip by FunctionEditor's time domain
	if (*tmax > my tmax) *tmax = my tmax;
}

static void checkTierSelection (TextGridEditor me, const char32 *verbPhrase) {
	TextGrid grid = (TextGrid) my data;
	if (my selectedTier < 1 || my selectedTier > grid -> tiers->size)
		Melder_throw (U"To ", verbPhrase, U", first select a tier by clicking anywhere inside it.");
}

static long getSelectedInterval (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers->size);
	IntervalTier tier = (IntervalTier) grid -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classIntervalTier);
	return IntervalTier_timeToIndex (tier, my startSelection);
}

static long getSelectedLeftBoundary (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers->size);
	IntervalTier tier = (IntervalTier) grid -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classIntervalTier);
	return IntervalTier_hasBoundary (tier, my startSelection);
}

static long getSelectedPoint (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers->size);
	TextTier tier = (TextTier) grid -> tiers->at [my selectedTier];
	Melder_assert (tier -> classInfo == classTextTier);
	return AnyTier_hasPoint (tier->asAnyTier(), my startSelection);
}

static void scrollToView (TextGridEditor me, double t) {
	if (t <= my startWindow) {
		FunctionEditor_shift (me, t - my startWindow - 0.618 * (my endWindow - my startWindow), true);
	} else if (t >= my endWindow) {
		FunctionEditor_shift (me, t - my endWindow + 0.618 * (my endWindow - my startWindow), true);
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

static void menu_cb_ExtractSelectedTextGrid_preserveTimes (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my endSelection <= my startSelection) Melder_throw (U"No selection.");
	autoTextGrid extract = TextGrid_extractPart ((TextGrid) my data, my startSelection, my endSelection, true);
	Editor_broadcastPublication (me, extract.move());
}

static void menu_cb_ExtractSelectedTextGrid_timeFromZero (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my endSelection <= my startSelection) Melder_throw (U"No selection.");
	autoTextGrid extract = TextGrid_extractPart ((TextGrid) my data, my startSelection, my endSelection, false);
	Editor_broadcastPublication (me, extract.move());
}

void structTextGridEditor :: v_createMenuItems_file_extract (EditorMenu menu) {
	TextGridEditor_Parent :: v_createMenuItems_file_extract (menu);
	extractSelectedTextGridPreserveTimesButton =
		EditorMenu_addCommand (menu, U"Extract selected TextGrid (preserve times)", 0, menu_cb_ExtractSelectedTextGrid_preserveTimes);
	extractSelectedTextGridTimeFromZeroButton =
		EditorMenu_addCommand (menu, U"Extract selected TextGrid (time from 0)", 0, menu_cb_ExtractSelectedTextGrid_timeFromZero);
}

static void menu_cb_WriteToTextFile (TextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save as TextGrid text file", nullptr)
		Melder_sprint (defaultName,300, my data -> name, U".TextGrid");
	EDITOR_DO_SAVE
		Data_writeToTextFile (my data, file);
	EDITOR_END
}

void structTextGridEditor :: v_createMenuItems_file_write (EditorMenu menu) {
	TextGridEditor_Parent :: v_createMenuItems_file_write (menu);
	EditorMenu_addCommand (menu, U"Save TextGrid as text file...", 'S', menu_cb_WriteToTextFile);
}

static void menu_cb_DrawVisibleTextGrid (TextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible TextGrid", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (U"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (U"Garnish", my pref_picture_garnish ());
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_picture_garnish () = GET_INTEGER (U"Garnish");
		Editor_openPraatPicture (me);
		TextGrid_Sound_draw ((TextGrid) my data, nullptr, my pictureGraphics, my startWindow, my endWindow, true, my p_useTextStyles,
			my pref_picture_garnish ());
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_DrawVisibleSoundAndTextGrid (TextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible sound and TextGrid", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (U"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (U"Garnish", my pref_picture_garnish ());
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_picture_garnish () = GET_INTEGER (U"Garnish");
		Editor_openPraatPicture (me);
		{// scope
			autoSound sound = my d_longSound.data ?
				LongSound_extractPart (my d_longSound.data, my startWindow, my endWindow, true) :
				Sound_extractPart (my d_sound.data, my startWindow, my endWindow,
					kSound_windowShape_RECTANGULAR, 1.0, true);
			TextGrid_Sound_draw ((TextGrid) my data, sound.get(), my pictureGraphics,
				my startWindow, my endWindow, true, my p_useTextStyles, my pref_picture_garnish ());
		}
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

void structTextGridEditor :: v_createMenuItems_file_draw (EditorMenu menu) {
	TextGridEditor_Parent :: v_createMenuItems_file_draw (menu);
	EditorMenu_addCommand (menu, U"Draw visible TextGrid...", 0, menu_cb_DrawVisibleTextGrid);
	if (d_sound.data || d_longSound.data)
		EditorMenu_addCommand (menu, U"Draw visible sound and TextGrid...", 0, menu_cb_DrawVisibleSoundAndTextGrid);
}

/***** EDIT MENU *****/

#ifndef macintosh
static void menu_cb_Cut (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_cut (my text);
}
static void menu_cb_Copy (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_copy (my text);
}
static void menu_cb_Paste (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_paste (my text);
}
static void menu_cb_Erase (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_remove (my text);
}
#endif

static void menu_cb_ConvertToBackslashTrigraphs (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Convert to Backslash Trigraphs");
	TextGrid_convertToBackslashTrigraphs ((TextGrid) my data);
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_ConvertToUnicode (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Convert to Unicode");
	TextGrid_convertToUnicode ((TextGrid) my data);
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

/***** QUERY MENU *****/

static void menu_cb_GetStartingPointOfInterval (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, U"query the starting point of an interval");
	Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		double time = iinterval < 1 || iinterval > tier -> intervals.size ? undefined :
			tier -> intervals.at [iinterval] -> xmin;
		Melder_informationReal (time, U"seconds");
	} else {
		Melder_throw (U"The selected tier is not an interval tier.");
	}
}

static void menu_cb_GetEndPointOfInterval (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, U"query the end point of an interval");
	Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		double time = iinterval < 1 || iinterval > tier -> intervals.size ? undefined :
			tier -> intervals.at [iinterval] -> xmax;
		Melder_informationReal (time, U"seconds");
	} else {
		Melder_throw (U"The selected tier is not an interval tier.");
	}
}

static void menu_cb_GetLabelOfInterval (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, U"query the label of an interval");
	Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		const char32 *label = iinterval < 1 || iinterval > tier -> intervals.size ? U"" :
			tier -> intervals.at [iinterval] -> text;
		Melder_information (label);
	} else {
		Melder_throw (U"The selected tier is not an interval tier.");
	}
}

/***** VIEW MENU *****/

static void do_selectAdjacentTier (TextGridEditor me, bool previous) {
	TextGrid grid = (TextGrid) my data;
	long n = grid -> tiers->size;
	if (n >= 2) {
		my selectedTier = previous ?
			my selectedTier > 1 ? my selectedTier - 1 : n :
			my selectedTier < n ? my selectedTier + 1 : 1;
		_TextGridEditor_timeToInterval (me, my startSelection, my selectedTier, & my startSelection, & my endSelection);
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_SelectPreviousTier (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentTier (me, true);
}

static void menu_cb_SelectNextTier (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentTier (me, false);
}

static void do_selectAdjacentInterval (TextGridEditor me, bool previous, bool shift) {
	TextGrid grid = (TextGrid) my data;
	IntervalTier intervalTier;
	TextTier textTier;
	if (my selectedTier < 1 || my selectedTier > grid -> tiers->size) return;
	_AnyTier_identifyClass (grid -> tiers->at [my selectedTier], & intervalTier, & textTier);
	if (intervalTier) {
		long n = intervalTier -> intervals.size;
		if (n >= 2) {
			TextInterval interval;
			long iinterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
			if (shift) {
				long binterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
				long einterval = IntervalTier_timeToIndex (intervalTier, my endSelection);
				if (my endSelection == intervalTier -> xmax) einterval ++;
				if (binterval < iinterval && einterval > iinterval + 1) {
					interval = intervalTier -> intervals.at [iinterval];
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
				} else if (previous) {
					if (einterval > iinterval + 1) {
						if (einterval <= n + 1) {
							interval = intervalTier -> intervals.at [einterval - 1];
							my endSelection = interval -> xmin;
						}
					} else if (binterval > 1) {
						interval = intervalTier -> intervals.at [binterval - 1];
						my startSelection = interval -> xmin;
					}
				} else {
					if (binterval < iinterval) {
						if (binterval > 0) {
							interval = intervalTier -> intervals.at [binterval];
							my startSelection = interval -> xmax;
						}
					} else if (einterval <= n) {
						interval = intervalTier -> intervals.at [einterval];
						my endSelection = interval -> xmax;
					}
				}
			} else {
				iinterval = previous ?
					iinterval > 1 ? iinterval - 1 : n :
					iinterval < n ? iinterval + 1 : 1;
				interval = intervalTier -> intervals.at [iinterval];
				my startSelection = interval -> xmin;
				my endSelection = interval -> xmax;
			}
			scrollToView (me, iinterval == n ? my startSelection : iinterval == 1 ? my endSelection : (my startSelection + my endSelection) / 2);
		}
	} else {
		long n = textTier -> points.size;
		if (n >= 2) {
			TextPoint point;
			long ipoint = AnyTier_timeToHighIndex (textTier->asAnyTier(), my startSelection);
			ipoint = previous ?
				ipoint > 1 ? ipoint - 1 : n :
				ipoint < n ? ipoint + 1 : 1;
			point = textTier -> points.at [ipoint];
			my startSelection = my endSelection = point -> number;
			scrollToView (me, my startSelection);
		}
	}
}

static void menu_cb_SelectPreviousInterval (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, true, false);
}

static void menu_cb_SelectNextInterval (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, false, false);
}

static void menu_cb_ExtendSelectPreviousInterval (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, true, true);
}

static void menu_cb_ExtendSelectNextInterval (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, false, true);
}

static void menu_cb_MoveBtoZero (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	double zero = Sound_getNearestZeroCrossing (my d_sound.data, my startSelection, 1);   // STEREO BUG
	if (isdefined (zero)) {
		my startSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveCursorToZero (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	double zero = Sound_getNearestZeroCrossing (my d_sound.data, 0.5 * (my startSelection + my endSelection), 1);   // STEREO BUG
	if (isdefined (zero)) {
		my startSelection = my endSelection = zero;
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveEtoZero (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	double zero = Sound_getNearestZeroCrossing (my d_sound.data, my endSelection, 1);   // STEREO BUG
	if (isdefined (zero)) {
		my endSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (me, true);
	}
}

/***** PITCH MENU *****/

static void menu_cb_DrawTextGridAndPitch (TextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw TextGrid and Pitch separately", nullptr)
		my v_form_pictureWindow (cmd);
		LABEL (U"", U"TextGrid:")
		BOOLEAN (U"Show boundaries and points", my default_picture_showBoundaries ());
		LABEL (U"", U"Pitch:")
		BOOLEAN (U"Speckle", my default_picture_pitch_speckle ());
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (U"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_INTEGER (U"Show boundaries and points", my pref_picture_showBoundaries ());
		SET_INTEGER (U"Speckle", my pref_picture_pitch_speckle ());
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (U"Garnish", my pref_picture_garnish ());
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my pref_picture_showBoundaries () = GET_INTEGER (U"Show boundaries and points");
		my pref_picture_pitch_speckle () = GET_INTEGER (U"Speckle");
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_picture_garnish () = GET_INTEGER (U"Garnish");
		if (! my p_pitch_show)
			Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my d_pitch) {
			TimeSoundAnalysisEditor_computePitch (me);
			if (! my d_pitch) Melder_throw (U"Cannot compute pitch.");
		}
		Editor_openPraatPicture (me);
		double pitchFloor_hidden = Function_convertStandardToSpecialUnit (my d_pitch.get(), my p_pitch_floor, Pitch_LEVEL_FREQUENCY, my p_pitch_unit);
		double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (my d_pitch.get(), my p_pitch_ceiling, Pitch_LEVEL_FREQUENCY, my p_pitch_unit);
		double pitchFloor_overt = Function_convertToNonlogarithmic (my d_pitch.get(), pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, my p_pitch_unit);
		double pitchCeiling_overt = Function_convertToNonlogarithmic (my d_pitch.get(), pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, my p_pitch_unit);
		double pitchViewFrom_overt = ( my p_pitch_viewFrom < my p_pitch_viewTo ? my p_pitch_viewFrom : pitchFloor_overt );
		double pitchViewTo_overt = ( my p_pitch_viewFrom < my p_pitch_viewTo ? my p_pitch_viewTo : pitchCeiling_overt );
		TextGrid_Pitch_drawSeparately ((TextGrid) my data, my d_pitch.get(), my pictureGraphics, my startWindow, my endWindow,
			pitchViewFrom_overt, pitchViewTo_overt, GET_INTEGER (U"Show boundaries and points"), my p_useTextStyles, GET_INTEGER (U"Garnish"),
			GET_INTEGER (U"Speckle"), my p_pitch_unit);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

/***** INTERVAL MENU *****/

static void insertBoundaryOrPoint (TextGridEditor me, int itier, double t1, double t2, bool insertSecond) {
	TextGrid grid = (TextGrid) my data;
	IntervalTier intervalTier;
	TextTier textTier;
	int ntiers = grid -> tiers->size;
	if (itier < 1 || itier > ntiers)
		Melder_throw (U"No tier ", itier, U".");
	_AnyTier_identifyClass (grid -> tiers->at [itier], & intervalTier, & textTier);
	Melder_assert (t1 <= t2);

	if (intervalTier) {
		autoTextInterval rightNewInterval, midNewInterval;
		bool t1IsABoundary = IntervalTier_hasTime (intervalTier, t1);
		bool t2IsABoundary = IntervalTier_hasTime (intervalTier, t2);
		if (t1 == t2 && t1IsABoundary)
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (t1, 6), U" seconds, because there is already a boundary there.");
		if (t1IsABoundary && t2IsABoundary)
			Melder_throw (U"Cannot add boundaries at ", Melder_fixed (t1, 6), U" and ", Melder_fixed (t2, 6), U" seconds, because there are already boundaries there.");
		long iinterval = IntervalTier_timeToIndex (intervalTier, t1);
		//Melder_casual ("iinterval %ld, t = %f", iinterval, t1);
		long iinterval2 = t1 == t2 ? iinterval : IntervalTier_timeToIndex (intervalTier, t2);
		//Melder_casual ("iinterval2 %ld, t = %f", iinterval2, t2);
		if (iinterval == 0 || iinterval2 == 0)
			Melder_throw (U"The selection is outside the time domain of the intervals.");
		long correctedIinterval2 = t2IsABoundary && iinterval2 == intervalTier -> intervals.size ? iinterval2 + 1 : iinterval2;
		if (correctedIinterval2 > iinterval + 1 || (correctedIinterval2 > iinterval && ! t2IsABoundary))
			Melder_throw (U"The selection straddles a boundary.");
		TextInterval interval = intervalTier -> intervals.at [iinterval];

		if (t1 == t2) {
			Editor_save (me, U"Add boundary");
		} else {
			Editor_save (me, U"Add interval");
		}

		if (itier == my selectedTier) {
			/*
			 * Divide up the label text into left, mid and right, depending on where the text selection is.
			 */
			long left, right;
			char32 *text = GuiText_getStringAndSelectionPosition (my text, & left, & right);
			bool wholeTextIsSelected = right - left == str32len (text);
			rightNewInterval = TextInterval_create (t2, interval -> xmax, text + right);
			text [right] = '\0';
			midNewInterval = TextInterval_create (t1, t2, text + left);
			if (! wholeTextIsSelected || t1 != t2) text [left] = U'\0';
			TextInterval_setText (interval, text);
			Melder_free (text);
		} else {
			/*
			 * Move the text to the left of the boundary.
			 */
			rightNewInterval = TextInterval_create (t2, interval -> xmax, U"");
			midNewInterval = TextInterval_create (t1, t2, U"");
		}
		if (t1IsABoundary) {
			/*
			 * Merge mid with left interval.
			 */
			if (interval -> xmin != t1)
				Melder_fatal (U"Boundary unequal: ", interval -> xmin, U" versus ", t1, U".");
			interval -> xmax = t2;
			TextInterval_setText (interval, Melder_cat (interval -> text, midNewInterval -> text));
		} else if (t2IsABoundary) {
			/*
			 * Merge mid and right interval.
			 */
			if (interval -> xmax != t2)
				Melder_fatal (U"Boundary unequal: ", interval -> xmax, U" versus ", t2, U".");
			interval -> xmax = t1;
			Melder_assert (rightNewInterval -> xmin == t2);
			Melder_assert (rightNewInterval -> xmax == t2);
			rightNewInterval -> xmin = t1;
			TextInterval_setText (rightNewInterval.get(), Melder_cat (midNewInterval -> text, rightNewInterval -> text));
		} else {
			interval -> xmax = t1;
			if (t1 != t2) intervalTier -> intervals.addItem_move (midNewInterval.move());
		}
		intervalTier -> intervals.addItem_move (rightNewInterval.move());
		if (insertSecond && ntiers >= 2 && t1 == t2) {
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
				autoTextInterval newInterval = TextInterval_create (tlast, t1, U"");
				interval -> xmax = tlast;
				intervalTier -> intervals.addItem_move (newInterval.move());
			}
		}
	} else {
		if (AnyTier_hasPoint (textTier->asAnyTier(), t1))
			Melder_throw (U"Cannot add a point at ", Melder_fixed (t1, 6), U" seconds, because there is already a point there.");

		Editor_save (me, U"Add point");

		autoTextPoint newPoint = TextPoint_create (t1, U"");
		textTier -> points. addItem_move (newPoint.move());
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
		FunctionEditor_marksChanged (me, true);
		Editor_broadcastDataChanged (me);
	} catch (MelderError) {
		Melder_throw (U"Interval not inserted.");
	}
}

static void menu_cb_InsertIntervalOnTier1 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 1); }
static void menu_cb_InsertIntervalOnTier2 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 2); }
static void menu_cb_InsertIntervalOnTier3 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 3); }
static void menu_cb_InsertIntervalOnTier4 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 4); }
static void menu_cb_InsertIntervalOnTier5 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 5); }
static void menu_cb_InsertIntervalOnTier6 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 6); }
static void menu_cb_InsertIntervalOnTier7 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 7); }
static void menu_cb_InsertIntervalOnTier8 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 8); }

static void menu_cb_AlignInterval (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, U"align words");
	AnyTier tier = static_cast <AnyTier> (grid -> tiers->at [my selectedTier]);
	if (tier -> classInfo != classIntervalTier)
		Melder_throw (U"Alignment works only for interval tiers, whereas tier ", my selectedTier, U" is a point tier.\nSelect an interval tier instead.");
	long intervalNumber = getSelectedInterval (me);
	if (! intervalNumber)
		Melder_throw (U"Select an interval first");
	if (! my p_align_includeWords && ! my p_align_includePhonemes)
		Melder_throw (U"Nothing to be done.\nPlease switch on \"Include words\" and/or \"Include phonemes\" in the \"Alignment settings\".");
	{// scope
		autoMelderProgressOff noprogress;
		Function anySound = my d_sound.data;
		if (my d_longSound.data) anySound = my d_longSound.data;
		Editor_save (me, U"Align interval");
		TextGrid_anySound_alignInterval (grid, anySound, my selectedTier, intervalNumber,
			my p_align_language, my p_align_includeWords, my p_align_includePhonemes);
	}
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_AlignmentSettings (TextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Alignment settings", nullptr)
		OPTIONMENU (U"Language", Strings_findString (espeakdata_voices_names.get(), U"English"))
		for (long i = 1; i <= espeakdata_voices_names -> numberOfStrings; i ++) {
			OPTION ((const char32 *) espeakdata_voices_names -> strings [i]);
		}
		BOOLEAN (U"Include words", my default_align_includeWords ())
		BOOLEAN (U"Include phonemes", my default_align_includePhonemes ())
		BOOLEAN (U"Allow silences", my default_align_allowSilences ())
	EDITOR_OK
		long prefVoice = Strings_findString (espeakdata_voices_names.get(), my p_align_language);
		if (prefVoice == 0) prefVoice = Strings_findString (espeakdata_voices_names.get(), U"English");
		SET_INTEGER (U"Language", prefVoice);
		SET_INTEGER (U"Include words", my p_align_includeWords)
		SET_INTEGER (U"Include phonemes", my p_align_includePhonemes)
		SET_INTEGER (U"Allow silences", my p_align_allowSilences)
	EDITOR_DO
		//my pref_align_language () = my p_align_language = GET_ENUM (kTextGrid_language, U"Language");
		pref_str32cpy2 (my pref_align_language (), my p_align_language, GET_STRING (U"Language"));
		my pref_align_includeWords    () = my p_align_includeWords    = GET_INTEGER (U"Include words");
		my pref_align_includePhonemes () = my p_align_includePhonemes = GET_INTEGER (U"Include phonemes");
		my pref_align_allowSilences   () = my p_align_allowSilences   = GET_INTEGER (U"Allow silences");
	EDITOR_END
}

/***** BOUNDARY/POINT MENU *****/

static void menu_cb_RemovePointOrBoundary (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, U"remove a point or boundary");
	Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (! selectedLeftBoundary) Melder_throw (U"To remove a boundary, first click on it.");

		Editor_save (me, U"Remove boundary");
		IntervalTier_removeLeftBoundary (tier, selectedLeftBoundary);
	} else {
		TextTier tier = (TextTier) anyTier;
		long selectedPoint = getSelectedPoint (me);
		if (! selectedPoint) Melder_throw (U"To remove a point, first click on it.");

		Editor_save (me, U"Remove point");
		tier -> points. removeItem (selectedPoint);
	}
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void do_movePointOrBoundary (TextGridEditor me, int where) {
	double position;
	TextGrid grid = (TextGrid) my data;
	if (where == 0 && ! my d_sound.data) return;
	checkTierSelection (me, U"move a point or boundary");
	Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		static const char32 *boundarySaveText [3] { U"Move boundary to zero crossing", U"Move boundary to B", U"Move boundary to E" };
		TextInterval left, right;
		long selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (! selectedLeftBoundary)
			Melder_throw (U"To move a boundary, first click on it.");
		left = tier -> intervals.at [selectedLeftBoundary - 1];
		right = tier -> intervals.at [selectedLeftBoundary];
		position = where == 1 ? my startSelection : where == 2 ? my endSelection :
			Sound_getNearestZeroCrossing (my d_sound.data, left -> xmax, 1);   // STEREO BUG
		if (isundef (position))
			Melder_throw (U"There is no zero crossing to move to.");
		if (position <= left -> xmin || position >= right -> xmax)
			Melder_throw (U"Cannot move a boundary past its neighbour.");

		Editor_save (me, boundarySaveText [where]);

		left -> xmax = right -> xmin = my startSelection = my endSelection = position;
	} else {
		TextTier tier = (TextTier) anyTier;
		static const char32 *pointSaveText [3] { U"Move point to zero crossing", U"Move point to B", U"Move point to E" };
		TextPoint point;
		long selectedPoint = getSelectedPoint (me);
		if (! selectedPoint)
			Melder_throw (U"To move a point, first click on it.");
		point = tier -> points.at [selectedPoint];
		position = where == 1 ? my startSelection : where == 2 ? my endSelection :
			Sound_getNearestZeroCrossing (my d_sound.data, point -> number, 1);   // STEREO BUG
		if (isundef (position))
			Melder_throw (U"There is no zero crossing to move to.");

		Editor_save (me, pointSaveText [where]);

		point -> number = my startSelection = my endSelection = position;
	}
	FunctionEditor_marksChanged (me, true);   // because cursor has moved
	Editor_broadcastDataChanged (me);
}

static void menu_cb_MoveToB (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_movePointOrBoundary (me, 1);
}

static void menu_cb_MoveToE (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_movePointOrBoundary (me, 2);
}

static void menu_cb_MoveToZero (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_movePointOrBoundary (me, 0);
}

static void do_insertOnTier (TextGridEditor me, int itier) {
	try {
		insertBoundaryOrPoint (me, itier,
			my playingCursor || my playingSelection ? my playCursor : my startSelection,
			my playingCursor || my playingSelection ? my playCursor : my endSelection,
			false);
		my selectedTier = itier;
		FunctionEditor_marksChanged (me, true);
		Editor_broadcastDataChanged (me);
	} catch (MelderError) {
		Melder_throw (U"Boundary or point not inserted.");
	}
}

static void menu_cb_InsertOnSelectedTier (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_insertOnTier (me, my selectedTier);
}

static void menu_cb_InsertOnTier1 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 1); }
static void menu_cb_InsertOnTier2 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 2); }
static void menu_cb_InsertOnTier3 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 3); }
static void menu_cb_InsertOnTier4 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 4); }
static void menu_cb_InsertOnTier5 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 5); }
static void menu_cb_InsertOnTier6 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 6); }
static void menu_cb_InsertOnTier7 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 7); }
static void menu_cb_InsertOnTier8 (TextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 8); }

static void menu_cb_InsertOnAllTiers (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	TextGrid grid = (TextGrid) my data;
	int saveTier = my selectedTier;
	for (int itier = 1; itier <= grid -> tiers->size; itier ++) {
		do_insertOnTier (me, itier);
	}
	my selectedTier = saveTier;   // only if everything went right; otherwise, the tier where something went wrong will stand selected
}

/***** SEARCH MENU *****/

static void findInTier (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, U"find a text");
	Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals.size) {
			TextInterval interval = tier -> intervals.at [iinterval];
			char32 *text = interval -> text;
			if (text) {
				char32 *position = str32str (text, my findString);
				if (position) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					scrollToView (me, my startSelection);
					GuiText_setSelection (my text, position - text, position - text + str32len (my findString));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals.size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		long ipoint = AnyTier_timeToLowIndex (tier->asAnyTier(), my startSelection) + 1;
		while (ipoint <= tier -> points.size) {
			TextPoint point = tier->points.at [ipoint];
			char32 *text = point -> mark;
			if (text) {
				char32 *position = str32str (text, my findString);
				if (position) {
					my startSelection = my endSelection = point -> number;
					scrollToView (me, point -> number);
					GuiText_setSelection (my text, position - text, position - text + str32len (my findString));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points.size)
			Melder_beep ();
	}
}

static void do_find (TextGridEditor me) {
	if (my findString) {
		long left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my text, & left, & right);
		char32 *position = str32str (label.peek() + right, my findString);   // CRLF BUG?
		if (position) {
			GuiText_setSelection (my text, position - label.peek(), position - label.peek() + str32len (my findString));
		} else {
			findInTier (me);
		}
	}
}

static void menu_cb_Find (TextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Find text", nullptr)
		LABEL (U"", U"Text:")
		TEXTFIELD (U"string", U"")
	EDITOR_OK
	EDITOR_DO
		Melder_free (my findString);
		my findString = Melder_dup_f (GET_STRING (U"string"));
		do_find (me);
	EDITOR_END
}

static void menu_cb_FindAgain (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_find (me);
}

static void checkSpellingInTier (TextGridEditor me) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, U"check spelling");
	Function anyTier = grid -> tiers->at [my selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals.size) {
			TextInterval interval = tier -> intervals.at [iinterval];
			char32 *text = interval -> text;
			if (text) {
				long position = 0;
				char32 *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					scrollToView (me, my startSelection);
					GuiText_setSelection (my text, position, position + str32len (notAllowed));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals.size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		long ipoint = AnyTier_timeToLowIndex (tier->asAnyTier(), my startSelection) + 1;
		while (ipoint <= tier -> points.size) {
			TextPoint point = tier -> points.at [ipoint];
			char32 *text = point -> mark;
			if (text) {
				long position = 0;
				char32 *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = my endSelection = point -> number;
					scrollToView (me, point -> number);
					GuiText_setSelection (my text, position, position + str32len (notAllowed));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points.size)
			Melder_beep ();
	}
}

static void menu_cb_CheckSpelling (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my spellingChecker) {
		long left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my text, & left, & right);
		long position = right;
		char32 *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.peek(), & position);
		if (notAllowed) {
			GuiText_setSelection (my text, position, position + str32len (notAllowed));
		} else {
			checkSpellingInTier (me);
		}
	}
}

static void menu_cb_CheckSpellingInInterval (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my spellingChecker) {
		long left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my text, & left, & right);
		long position = right;
		char32 *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.peek(), & position);
		if (notAllowed) {
			GuiText_setSelection (my text, position, position + str32len (notAllowed));
		}
	}
}

static void menu_cb_AddToUserDictionary (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my spellingChecker) {
		autostring32 word = GuiText_getSelection (my text);
		SpellingChecker_addNewWord (my spellingChecker, word.peek());
		Editor_broadcastDataChanged (me);
	}
}

/***** TIER MENU *****/

static void menu_cb_RenameTier (TextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Rename tier", nullptr)
		SENTENCE (U"Name", U"");
	EDITOR_OK
		TextGrid grid = (TextGrid) my data;
		checkTierSelection (me, U"rename a tier");
		Daata tier = grid -> tiers->at [my selectedTier];
		SET_STRING (U"Name", tier -> name ? tier -> name : U"")
	EDITOR_DO
		TextGrid grid = (TextGrid) my data;
		checkTierSelection (me, U"rename a tier");
		Function tier = grid -> tiers->at [my selectedTier];

		Editor_save (me, U"Rename tier");

		char32 *newName = GET_STRING (U"Name");
		Thing_setName (tier, newName);

		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_PublishTier (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, U"publish a tier");
	Function tier = grid -> tiers->at [my selectedTier];
	autoTextGrid publish = TextGrid_createWithoutTiers (1e30, -1e30);
	TextGrid_addTier_copy (publish.get(), tier);
	Thing_setName (publish.get(), tier -> name);
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_RemoveAllTextFromTier (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	TextGrid grid = (TextGrid) my data;
	checkTierSelection (me, U"remove all text from a tier");
	IntervalTier intervalTier;
	TextTier textTier;
	_AnyTier_identifyClass (grid -> tiers->at [my selectedTier], & intervalTier, & textTier);

	Editor_save (me, U"Remove text from tier");
	if (intervalTier) {
		IntervalTier_removeText (intervalTier);
	} else {
		TextTier_removeText (textTier);
	}

	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_RemoveTier (TextGridEditor me, EDITOR_ARGS_DIRECT) {
	TextGrid grid = (TextGrid) my data;
	if (grid -> tiers->size <= 1) {
		Melder_throw (U"Sorry, I refuse to remove the last tier.");
	}
	checkTierSelection (me, U"remove a tier");

	Editor_save (me, U"Remove tier");
	grid -> tiers-> removeItem (my selectedTier);

	my selectedTier = 1;
	FunctionEditor_updateText (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_AddIntervalTier (TextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add interval tier", nullptr)
		NATURAL (U"Position", U"1 (= at top)")
		SENTENCE (U"Name", U"")
	EDITOR_OK
		TextGrid grid = (TextGrid) my data;
		SET_STRING (U"Position", Melder_cat (grid -> tiers->size + 1, U" (= at bottom)"))
		SET_STRING (U"Name", U"")
	EDITOR_DO
		TextGrid grid = (TextGrid) my data;
		int position = GET_INTEGER (U"Position");
		char32 *name = GET_STRING (U"Name");
		{// scope
			autoIntervalTier tier = IntervalTier_create (grid -> xmin, grid -> xmax);
			if (position > grid -> tiers->size) position = grid -> tiers->size + 1;
			Thing_setName (tier.get(), name);

			Editor_save (me, U"Add interval tier");
			grid -> tiers -> addItemAtPosition_move (tier.move(), position);
		}

		my selectedTier = position;
		FunctionEditor_updateText (me);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_AddPointTier (TextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point tier", nullptr)
		NATURAL (U"Position", U"1 (= at top)")
		SENTENCE (U"Name", U"");
	EDITOR_OK
		TextGrid grid = (TextGrid) my data;
		SET_STRING (U"Position", Melder_cat (grid -> tiers->size + 1, U" (= at bottom)"))
		SET_STRING (U"Name", U"")
	EDITOR_DO
		TextGrid grid = (TextGrid) my data;
		int position = GET_INTEGER (U"Position");
		char32 *name = GET_STRING (U"Name");
		{// scope
			autoTextTier tier = TextTier_create (grid -> xmin, grid -> xmax);
			if (position > grid -> tiers->size) position = grid -> tiers->size + 1;
			Thing_setName (tier.get(), name);

			Editor_save (me, U"Add point tier");
			grid -> tiers -> addItemAtPosition_move (tier.move(), position);
		}

		my selectedTier = position;
		FunctionEditor_updateText (me);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_DuplicateTier (TextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Duplicate tier", nullptr)
		NATURAL (U"Position", U"1 (= at top)")
		SENTENCE (U"Name", U"")
	EDITOR_OK
		TextGrid grid = (TextGrid) my data;
		if (my selectedTier) {
			SET_STRING (U"Position", Melder_integer (my selectedTier + 1))
			SET_STRING (U"Name", grid -> tiers->at [my selectedTier] -> name)
		}
	EDITOR_DO
		TextGrid grid = (TextGrid) my data;
		int position = GET_INTEGER (U"Position");
		char32 *name = GET_STRING (U"Name");
		checkTierSelection (me, U"duplicate a tier");
		Function tier = grid -> tiers->at [my selectedTier];
		{// scope
			autoFunction newTier = Data_copy (tier);
			if (position > grid -> tiers->size)
				position = grid -> tiers->size + 1;
			Thing_setName (newTier.get(), name);

			Editor_save (me, U"Duplicate tier");
			grid -> tiers -> addItemAtPosition_move (newTier.move(), position);
		}

		my selectedTier = position;
		FunctionEditor_updateText (me);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

/***** HELP MENU *****/

static void menu_cb_TextGridEditorHelp (TextGridEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"TextGridEditor"); }
static void menu_cb_AboutSpecialSymbols (TextGridEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Special symbols"); }
static void menu_cb_PhoneticSymbols (TextGridEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Phonetic symbols"); }
static void menu_cb_AboutTextStyles (TextGridEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Text styles"); }

void structTextGridEditor :: v_createMenus () {
	TextGridEditor_Parent :: v_createMenus ();
	EditorMenu menu;

	#ifndef macintosh
		Editor_addCommand (this, U"Edit", U"-- cut copy paste --", 0, nullptr);
		Editor_addCommand (this, U"Edit", U"Cut text", 'X', menu_cb_Cut);
		Editor_addCommand (this, U"Edit", U"Cut", Editor_HIDDEN, menu_cb_Cut);
		Editor_addCommand (this, U"Edit", U"Copy text", 'C', menu_cb_Copy);
		Editor_addCommand (this, U"Edit", U"Copy", Editor_HIDDEN, menu_cb_Copy);
		Editor_addCommand (this, U"Edit", U"Paste text", 'V', menu_cb_Paste);
		Editor_addCommand (this, U"Edit", U"Paste", Editor_HIDDEN, menu_cb_Paste);
		Editor_addCommand (this, U"Edit", U"Erase text", 0, menu_cb_Erase);
		Editor_addCommand (this, U"Edit", U"Erase", Editor_HIDDEN, menu_cb_Erase);
	#endif
	Editor_addCommand (this, U"Edit", U"-- encoding --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Convert entire TextGrid to backslash trigraphs", 0, menu_cb_ConvertToBackslashTrigraphs);
	Editor_addCommand (this, U"Edit", U"Convert entire TextGrid to Unicode", 0, menu_cb_ConvertToUnicode);
	Editor_addCommand (this, U"Edit", U"-- search --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Find...", 'F', menu_cb_Find);
	Editor_addCommand (this, U"Edit", U"Find again", 'G', menu_cb_FindAgain);

	if (d_sound.data) {
		Editor_addCommand (this, U"Select", U"-- move to zero --", 0, 0);
		Editor_addCommand (this, U"Select", U"Move start of selection to nearest zero crossing", ',', menu_cb_MoveBtoZero);
		Editor_addCommand (this, U"Select", U"Move begin of selection to nearest zero crossing", Editor_HIDDEN, menu_cb_MoveBtoZero);
		Editor_addCommand (this, U"Select", U"Move cursor to nearest zero crossing", '0', menu_cb_MoveCursorToZero);
		Editor_addCommand (this, U"Select", U"Move end of selection to nearest zero crossing", '.', menu_cb_MoveEtoZero);
	}

	Editor_addCommand (this, U"Query", U"-- query interval --", 0, nullptr);
	Editor_addCommand (this, U"Query", U"Get starting point of interval", 0, menu_cb_GetStartingPointOfInterval);
	Editor_addCommand (this, U"Query", U"Get end point of interval", 0, menu_cb_GetEndPointOfInterval);
	Editor_addCommand (this, U"Query", U"Get label of interval", 0, menu_cb_GetLabelOfInterval);

	menu = Editor_addMenu (this, U"Interval", 0);
	if (d_sound.data || d_longSound.data) {
		EditorMenu_addCommand (menu, U"Align interval", 'D', menu_cb_AlignInterval);
		EditorMenu_addCommand (menu, U"Alignment settings...", 0, menu_cb_AlignmentSettings);
		EditorMenu_addCommand (menu, U"-- add interval --", 0, nullptr);
	}
	EditorMenu_addCommand (menu, U"Add interval on tier 1", GuiMenu_COMMAND | '1', menu_cb_InsertIntervalOnTier1);
	EditorMenu_addCommand (menu, U"Add interval on tier 2", GuiMenu_COMMAND | '2', menu_cb_InsertIntervalOnTier2);
	EditorMenu_addCommand (menu, U"Add interval on tier 3", GuiMenu_COMMAND | '3', menu_cb_InsertIntervalOnTier3);
	EditorMenu_addCommand (menu, U"Add interval on tier 4", GuiMenu_COMMAND | '4', menu_cb_InsertIntervalOnTier4);
	EditorMenu_addCommand (menu, U"Add interval on tier 5", GuiMenu_COMMAND | '5', menu_cb_InsertIntervalOnTier5);
	EditorMenu_addCommand (menu, U"Add interval on tier 6", GuiMenu_COMMAND | '6', menu_cb_InsertIntervalOnTier6);
	EditorMenu_addCommand (menu, U"Add interval on tier 7", GuiMenu_COMMAND | '7', menu_cb_InsertIntervalOnTier7);
	EditorMenu_addCommand (menu, U"Add interval on tier 8", GuiMenu_COMMAND | '8', menu_cb_InsertIntervalOnTier8);

	menu = Editor_addMenu (this, U"Boundary", 0);
	/*EditorMenu_addCommand (menu, U"Move to B", 0, menu_cb_MoveToB);
	EditorMenu_addCommand (menu, U"Move to E", 0, menu_cb_MoveToE);*/
	if (d_sound.data) {
		EditorMenu_addCommand (menu, U"Move to nearest zero crossing", 0, menu_cb_MoveToZero);
		EditorMenu_addCommand (menu, U"-- insert boundary --", 0, nullptr);
	}
	EditorMenu_addCommand (menu, U"Add on selected tier", GuiMenu_ENTER, menu_cb_InsertOnSelectedTier);
	EditorMenu_addCommand (menu, U"Add on tier 1", GuiMenu_COMMAND | GuiMenu_F1, menu_cb_InsertOnTier1);
	EditorMenu_addCommand (menu, U"Add on tier 2", GuiMenu_COMMAND | GuiMenu_F2, menu_cb_InsertOnTier2);
	EditorMenu_addCommand (menu, U"Add on tier 3", GuiMenu_COMMAND | GuiMenu_F3, menu_cb_InsertOnTier3);
	EditorMenu_addCommand (menu, U"Add on tier 4", GuiMenu_COMMAND | GuiMenu_F4, menu_cb_InsertOnTier4);
	EditorMenu_addCommand (menu, U"Add on tier 5", GuiMenu_COMMAND | GuiMenu_F5, menu_cb_InsertOnTier5);
	EditorMenu_addCommand (menu, U"Add on tier 6", GuiMenu_COMMAND | GuiMenu_F6, menu_cb_InsertOnTier6);
	EditorMenu_addCommand (menu, U"Add on tier 7", GuiMenu_COMMAND | GuiMenu_F7, menu_cb_InsertOnTier7);
	EditorMenu_addCommand (menu, U"Add on tier 8", GuiMenu_COMMAND | GuiMenu_F8, menu_cb_InsertOnTier8);
	EditorMenu_addCommand (menu, U"Add on all tiers", GuiMenu_COMMAND | GuiMenu_F9, menu_cb_InsertOnAllTiers);
	EditorMenu_addCommand (menu, U"-- remove mark --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Remove", GuiMenu_OPTION | GuiMenu_BACKSPACE, menu_cb_RemovePointOrBoundary);

	menu = Editor_addMenu (this, U"Tier", 0);
	EditorMenu_addCommand (menu, U"Add interval tier...", 0, menu_cb_AddIntervalTier);
	EditorMenu_addCommand (menu, U"Add point tier...", 0, menu_cb_AddPointTier);
	EditorMenu_addCommand (menu, U"Duplicate tier...", 0, menu_cb_DuplicateTier);
	EditorMenu_addCommand (menu, U"Rename tier...", 0, menu_cb_RenameTier);
	EditorMenu_addCommand (menu, U"-- remove tier --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Remove all text from tier", 0, menu_cb_RemoveAllTextFromTier);
	EditorMenu_addCommand (menu, U"Remove entire tier", 0, menu_cb_RemoveTier);
	EditorMenu_addCommand (menu, U"-- extract tier --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Extract to list of objects:", GuiMenu_INSENSITIVE, menu_cb_PublishTier /* dummy */);
	EditorMenu_addCommand (menu, U"Extract entire selected tier", 0, menu_cb_PublishTier);

	if (spellingChecker) {
		menu = Editor_addMenu (this, U"Spell", 0);
		EditorMenu_addCommand (menu, U"Check spelling in tier", GuiMenu_COMMAND | GuiMenu_OPTION | 'L', menu_cb_CheckSpelling);
		EditorMenu_addCommand (menu, U"Check spelling in interval", 0, menu_cb_CheckSpellingInInterval);
		EditorMenu_addCommand (menu, U"-- edit lexicon --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Add selected word to user dictionary", 0, menu_cb_AddToUserDictionary);
	}

	if (d_sound.data || d_longSound.data) {
		if (v_hasAnalysis ()) {
			v_createMenus_analysis ();   // insert some of the ancestor's menus *after* the TextGrid menus
		}
	}
}

void structTextGridEditor :: v_createHelpMenuItems (EditorMenu menu) {
	TextGridEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"TextGridEditor help", '?', menu_cb_TextGridEditorHelp);
	EditorMenu_addCommand (menu, U"About special symbols", 0, menu_cb_AboutSpecialSymbols);
	EditorMenu_addCommand (menu, U"Phonetic symbols", 0, menu_cb_PhoneticSymbols);
	EditorMenu_addCommand (menu, U"About text styles", 0, menu_cb_AboutTextStyles);
}

/***** CHILDREN *****/

static void gui_text_cb_changed (TextGridEditor me, GuiTextEvent /* event */) {
	TextGrid grid = (TextGrid) my data;
	//Melder_casual (U"gui_text_cb_change 1 in editor ", Melder_pointer (me));
	if (my suppressRedraw) return;   /* Prevent infinite loop if 'draw' method or Editor_broadcastChange calls GuiText_setString. */
	//Melder_casual (U"gui_text_cb_change 2 in editor ", me);
	if (my selectedTier) {
		char32 *text = GuiText_getString (my text);
		IntervalTier intervalTier;
		TextTier textTier;
		_AnyTier_identifyClass (grid -> tiers->at [my selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			long selectedInterval = getSelectedInterval (me);
			if (selectedInterval) {
				TextInterval interval = intervalTier -> intervals.at [selectedInterval];
				//Melder_casual (U"gui_text_cb_change 3 in editor ", Melder_pointer (me));
				TextInterval_setText (interval, text);
				//Melder_casual (U"gui_text_cb_change 4 in editor ", Melder_pointer (me));
				FunctionEditor_redraw (me);
				//Melder_casual (U"gui_text_cb_change 5 in editor ", Melder_pointer (me));
				Editor_broadcastDataChanged (me);
				//Melder_casual (U"gui_text_cb_change 6 in editor ", Melder_pointer (me));
			}
		} else {
			long selectedPoint = getSelectedPoint (me);
			if (selectedPoint) {
				TextPoint point = textTier -> points.at [selectedPoint];
				Melder_free (point -> mark);
				if (str32spn (text, U" \n\t") != str32len (text))   // any visible characters?
				point -> mark = Melder_dup_f (text);
				FunctionEditor_redraw (me);
				Editor_broadcastDataChanged (me);
			}
		}
		Melder_free (text);
	}
}

void structTextGridEditor :: v_createChildren () {
	TextGridEditor_Parent :: v_createChildren ();
	if (text) GuiText_setChangedCallback (text, gui_text_cb_changed, this);
}

void structTextGridEditor :: v_dataChanged () {
	TextGrid grid = (TextGrid) data;
	/*
	 * Perform a minimal selection change.
	 * Most changes will involve intervals and boundaries; however, there may also be tier removals.
	 * Do a simple guess.
	 */
	if (selectedTier > grid -> tiers->size) {
		selectedTier = grid -> tiers->size;
	}
	TextGridEditor_Parent :: v_dataChanged ();   // does all the updating
}

/********** DRAWING AREA **********/

void structTextGridEditor :: v_prepareDraw () {
	if (d_longSound.data) {
		try {
			LongSound_haveWindow (our d_longSound.data, our startWindow, our endWindow);
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
	int selectedInterval = itier == my selectedTier ? getSelectedInterval (me) : 0, iinterval, ninterval = tier -> intervals.size;
	Graphics_WCtoDC (my graphics.get(), my startWindow, 0.0, & x1DC, & yDC);
	Graphics_WCtoDC (my graphics.get(), my endWindow, 0.0, & x2DC, & yDC);
	Graphics_setPercentSignIsItalic (my graphics.get(), my p_useTextStyles);
	Graphics_setNumberSignIsBold (my graphics.get(), my p_useTextStyles);
	Graphics_setCircumflexIsSuperscript (my graphics.get(), my p_useTextStyles);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), my p_useTextStyles);

	/*
	 * Highlight interval: yellow (selected) or green (matching label).
	 */
	
	for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
		TextInterval interval = tier -> intervals.at [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		if (tmax > my startWindow && tmin < my endWindow) {   // interval visible?
			int intervalIsSelected = iinterval == selectedInterval;
			int labelMatches = Melder_stringMatchesCriterion (interval -> text, my p_greenMethod, my p_greenString);
			if (tmin < my startWindow) tmin = my startWindow;
			if (tmax > my endWindow) tmax = my endWindow;
			if (labelMatches) {
				Graphics_setColour (my graphics.get(), Graphics_LIME);
				Graphics_fillRectangle (my graphics.get(), tmin, tmax, 0.0, 1.0);
			}
			if (intervalIsSelected) {
				if (labelMatches) {
					tmin = 0.85 * tmin + 0.15 * tmax;
					tmax = 0.15 * tmin + 0.85 * tmax;
				}
				Graphics_setColour (my graphics.get(), Graphics_YELLOW);
				Graphics_fillRectangle (my graphics.get(), tmin, tmax, labelMatches ? 0.15 : 0.0, labelMatches? 0.85: 1.0);
			}
		}
	}
	Graphics_setColour (my graphics.get(), Graphics_BLACK);
	Graphics_line (my graphics.get(), my endWindow, 0.0, my endWindow, 1.0);

	/*
	 * Draw a grey bar and a selection button at the cursor position.
	 */
	if (my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow) {
		bool cursorAtBoundary = false;
		for (iinterval = 2; iinterval <= ninterval; iinterval ++) {
			TextInterval interval = tier -> intervals.at [iinterval];
			if (interval -> xmin == my startSelection) cursorAtBoundary = true;
		}
		if (! cursorAtBoundary) {
			double dy = Graphics_dyMMtoWC (my graphics.get(), 1.5);
			Graphics_setGrey (my graphics.get(), 0.8);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), my startSelection, 0.0, my startSelection, 1.0);
			Graphics_setLineWidth (my graphics.get(), 1.0);
			Graphics_setColour (my graphics.get(), Graphics_BLUE);
			Graphics_circle_mm (my graphics.get(), my startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics.get(), my p_alignment, Graphics_HALF);
	for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
		TextInterval interval = tier -> intervals.at [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		if (tmin < my tmin) tmin = my tmin; if (tmax > my tmax) tmax = my tmax;
		if (tmin >= tmax) continue;
		bool intervalIsSelected = selectedInterval == iinterval;

		/*
		 * Draw left boundary.
		 */
		if (tmin >= my startWindow && tmin <= my endWindow && iinterval > 1) {
			bool boundaryIsSelected = ( my selectedTier == itier && tmin == my startSelection );
			Graphics_setColour (my graphics.get(), boundaryIsSelected ? Graphics_RED : Graphics_BLUE);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), tmin, 0.0, tmin, 1.0);

			/*
			 * Show alignment with cursor.
			 */
			if (tmin == my startSelection) {
				Graphics_setColour (my graphics.get(), Graphics_YELLOW);
				Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics.get(), tmin, 0.0, tmin, 1.0);
			}
		}
		Graphics_setLineWidth (my graphics.get(), 1.0);

		/*
		 * Draw label text.
		 */
		if (interval -> text && tmax >= my startWindow && tmin <= my endWindow) {
			double t1 = my startWindow > tmin ? my startWindow : tmin;
			double t2 = my endWindow < tmax ? my endWindow : tmax;
			Graphics_setColour (my graphics.get(), intervalIsSelected ? Graphics_RED : Graphics_BLACK);
			Graphics_textRect (my graphics.get(), t1, t2, 0.0, 1.0, interval -> text);
			Graphics_setColour (my graphics.get(), Graphics_BLACK);
		}

	}
	Graphics_setPercentSignIsItalic (my graphics.get(), true);
	Graphics_setNumberSignIsBold (my graphics.get(), true);
	Graphics_setCircumflexIsSuperscript (my graphics.get(), true);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), true);
}

static void do_drawTextTier (TextGridEditor me, TextTier tier, int itier) {
	#if gtk || defined (macintosh)
		bool platformUsesAntiAliasing = true;
	#else
		bool platformUsesAntiAliasing = false;
	#endif
	int ipoint, npoint = tier -> points.size;
	Graphics_setPercentSignIsItalic (my graphics.get(), my p_useTextStyles);
	Graphics_setNumberSignIsBold (my graphics.get(), my p_useTextStyles);
	Graphics_setCircumflexIsSuperscript (my graphics.get(), my p_useTextStyles);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), my p_useTextStyles);

	/*
	 * Draw a grey bar and a selection button at the cursor position.
	 */
	if (my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow) {
		bool cursorAtPoint = false;
		for (ipoint = 1; ipoint <= npoint; ipoint ++) {
			TextPoint point = tier -> points.at [ipoint];
			if (point -> number == my startSelection) cursorAtPoint = true;
		}
		if (! cursorAtPoint) {
			double dy = Graphics_dyMMtoWC (my graphics.get(), 1.5);
			Graphics_setGrey (my graphics.get(), 0.8);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), my startSelection, 0.0, my startSelection, 1.0);
			Graphics_setLineWidth (my graphics.get(), 1.0);
			Graphics_setColour (my graphics.get(), Graphics_BLUE);
			Graphics_circle_mm (my graphics.get(), my startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
	for (ipoint = 1; ipoint <= npoint; ipoint ++) {
		TextPoint point = tier -> points.at [ipoint];
		double t = point -> number;
		if (t >= my startWindow && t <= my endWindow) {
			bool pointIsSelected = ( itier == my selectedTier && t == my startSelection );
			Graphics_setColour (my graphics.get(), pointIsSelected ? Graphics_RED : Graphics_BLUE);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), t, 0.0, t, 0.2);
			Graphics_line (my graphics.get(), t, 0.8, t, 1);
			Graphics_setLineWidth (my graphics.get(), 1.0);

			/*
			 * Wipe out the cursor where the text is going to be.
			 */
			Graphics_setColour (my graphics.get(), Graphics_WHITE);
			Graphics_line (my graphics.get(), t, 0.2, t, 0.8);

			/*
			 * Show alignment with cursor.
			 */
			if (my startSelection == my endSelection && t == my startSelection) {
				Graphics_setColour (my graphics.get(), Graphics_YELLOW);
				Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics.get(), t, 0.0, t, 0.2);
				Graphics_line (my graphics.get(), t, 0.8, t, 1.0);
				Graphics_setLineWidth (my graphics.get(), 1.0);
			}
			Graphics_setColour (my graphics.get(), pointIsSelected ? Graphics_RED : Graphics_BLUE);
			if (point -> mark) Graphics_text (my graphics.get(), t, 0.5, point -> mark);
		}
	}
	Graphics_setPercentSignIsItalic (my graphics.get(), true);
	Graphics_setNumberSignIsBold (my graphics.get(), true);
	Graphics_setCircumflexIsSuperscript (my graphics.get(), true);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), true);
}

void structTextGridEditor :: v_draw () {
	TextGrid grid = (TextGrid) data;
	Graphics_Viewport vp1, vp2;
	long itier, ntier = grid -> tiers->size;
	enum kGraphics_font oldFont = Graphics_inqFont (our graphics.get());
	int oldFontSize = Graphics_inqFontSize (our graphics.get());
	bool showAnalysis = v_hasAnalysis () && (p_spectrogram_show || p_pitch_show || p_intensity_show || p_formant_show) && (d_longSound.data || d_sound.data);
	double soundY = _TextGridEditor_computeSoundY (this), soundY2 = showAnalysis ? 0.5 * (1.0 + soundY) : soundY;

	/*
	 * Draw optional sound.
	 */
	if (d_longSound.data || d_sound.data) {
		vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundY2, 1.0);
		Graphics_setColour (our graphics.get(), Graphics_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		TimeSoundEditor_drawSound (this, -1.0, 1.0);
		//Graphics_flushWs (our graphics.get());
		Graphics_resetViewport (our graphics.get(), vp1);
	}

	/*
	 * Draw tiers.
	 */
	if (d_longSound.data || d_sound.data) vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, soundY);
	Graphics_setColour (our graphics.get(), Graphics_WHITE);
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
	Graphics_rectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
	for (itier = 1; itier <= ntier; itier ++) {
		Function anyTier = grid -> tiers->at [itier];
		bool tierIsSelected = itier == selectedTier;
		bool isIntervalTier = anyTier -> classInfo == classIntervalTier;
		vp2 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0,
			1.0 - (double) itier / (double) ntier,
			1.0 - (double) (itier - 1) / (double) ntier);
		Graphics_setColour (our graphics.get(), Graphics_BLACK);
		if (itier != 1) Graphics_line (our graphics.get(), our startWindow, 1.0, our endWindow, 1.0);

		/*
		 * Show the number and the name of the tier.
		 */
		Graphics_setColour (our graphics.get(), tierIsSelected ? Graphics_RED : Graphics_BLACK);
		Graphics_setFont (our graphics.get(), oldFont);
		Graphics_setFontSize (our graphics.get(), 14);
		Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (our graphics.get(), our startWindow, 0.5,   tierIsSelected ? U"☞ " : U"", itier);
		Graphics_setFontSize (our graphics.get(), oldFontSize);
		if (anyTier -> name && anyTier -> name [0]) {
			Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT,
				p_showNumberOf == kTextGridEditor_showNumberOf_NOTHING ? Graphics_HALF : Graphics_BOTTOM);
			Graphics_text (our graphics.get(), our endWindow, 0.5, anyTier -> name);
		}
		if (our p_showNumberOf != kTextGridEditor_showNumberOf_NOTHING) {
			Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_TOP);
			if (p_showNumberOf == kTextGridEditor_showNumberOf_INTERVALS_OR_POINTS) {
				long count = isIntervalTier ? ((IntervalTier) anyTier) -> intervals.size : ((TextTier) anyTier) -> points.size;
				long position = itier == selectedTier ? ( isIntervalTier ? getSelectedInterval (this) : getSelectedPoint (this) ) : 0;
				if (position) {
					Graphics_text (our graphics.get(), our endWindow, 0.5,   U"(", position, U"/", count, U")");
				} else {
					Graphics_text (our graphics.get(), our endWindow, 0.5,   U"(", count, U")");
				}
			} else {
				Melder_assert (kTextGridEditor_showNumberOf_NONEMPTY_INTERVALS_OR_POINTS);
				long count = 0;
				if (isIntervalTier) {
					IntervalTier tier = (IntervalTier) anyTier;
					long ninterval = tier -> intervals.size, iinterval;
					for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
						TextInterval interval = tier -> intervals.at [iinterval];
						if (interval -> text && interval -> text [0] != U'\0') {
							count ++;
						}
					}
				} else {
					TextTier tier = (TextTier) anyTier;
					long npoint = tier -> points.size, ipoint;
					for (ipoint = 1; ipoint <= npoint; ipoint ++) {
						TextPoint point = tier -> points.at [ipoint];
						if (point -> mark && point -> mark [0] != U'\0') {
							count ++;
						}
					}
				}
				Graphics_text (our graphics.get(), our endWindow, 0.5,   U"(##", count, U"#)");
			}
		}

		Graphics_setColour (our graphics.get(), Graphics_BLACK);
		Graphics_setFont (our graphics.get(), kGraphics_font_TIMES);
		Graphics_setFontSize (our graphics.get(), p_fontSize);
		if (isIntervalTier)
			do_drawIntervalTier (this, (IntervalTier) anyTier, itier);
		else
			do_drawTextTier (this, (TextTier) anyTier, itier);
		Graphics_resetViewport (our graphics.get(), vp2);
	}
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
	Graphics_setFont (our graphics.get(), oldFont);
	Graphics_setFontSize (our graphics.get(), oldFontSize);
	if (d_longSound.data || d_sound.data) Graphics_resetViewport (our graphics.get(), vp1);
	//Graphics_flushWs (our graphics.get());

	if (showAnalysis) {
		vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundY, soundY2);
		v_draw_analysis ();
		//Graphics_flushWs (our graphics.get());
		Graphics_resetViewport (our graphics.get(), vp1);
		/* Draw pulses. */
		if (p_pulses_show) {
			vp1 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, soundY2, 1.0);
			v_draw_analysis_pulses ();
			TimeSoundEditor_drawSound (this, -1.0, 1.0);   // second time, partially across the pulses
			//Graphics_flushWs (our graphics.get());
			Graphics_resetViewport (our graphics.get(), vp1);
		}
	}
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
	if (our d_longSound.data || our d_sound.data) {
		Graphics_line (our graphics.get(), our startWindow, soundY, our endWindow, soundY);
		if (showAnalysis) {
			Graphics_line (our graphics.get(), our startWindow, soundY2, our endWindow, soundY2);
			Graphics_line (our graphics.get(), our startWindow, soundY, our startWindow, soundY2);
			Graphics_line (our graphics.get(), our endWindow, soundY, our endWindow, soundY2);
		}
	}

	/*
	 * Finally, us usual, update the menus.
	 */
	v_updateMenuItems_file ();
}

static const char32 *characters [12] [10] = {
	{ U"ɑ", U"ɐ", U"ɒ", U"æ", U"ʙ", U"ɓ", U"β", U"ç", U"ɕ", U"ð" },
	{ U"ɗ", U"ɖ", U"ɛ", U"ɜ", U"ə", U"ɢ", U"ɠ", U"ʛ", U"ɣ", U"ɤ" },
	{ U"ˠ", U"ʜ", U"ɦ", U"ħ", U"ʰ", U"ɧ", U"ɪ", U"ɨ", U"ı", U"ɟ" },
	{ U"ʝ", U"ʄ", U"ᴊ", U"ʲ", U"ʟ", U"ɬ", U"ɭ", U"ɮ", U"ɫ", U"ˡ" },
	{ U"ɰ", U"ɯ", U"ɱ", U"ɴ", U"ŋ", U"ɲ", U"ɳ", U"ⁿ", U"ɔ", U"ɵ" },

	{ U"ø", U"œ", U"ɶ", U"ɸ", U"ɹ", U"ʀ", U"ʁ", U"ɾ", U"ɽ", U"ɺ" },
	{ U"ɻ", U"ʃ", U"ʂ", U"θ", U"ʈ", U"ʉ", U"ʊ", U"ʌ", U"ʋ", U"ʍ" },
	{ U"ʷ", U"χ", U"ʎ", U"ʏ", U"ɥ", U"ʒ", U"ʐ", U"ʑ", U"ˑ", U"ː" },
	{ U"ʔ", U"ʡ", U"ʕ", U"ʢ", U"ˤ", U"ǃ", U"ʘ", U"ǀ", U"ǁ", U"ǂ" },
	{ U"ʤ", U"ɘ", U"ɚ", U"ɝ", U"ʱ", U"ˢ", U"ʧ", U"ɞ", U"ʦ", U"ʣ" },

	{ U"ʨ", U"ʥ", U"z̊", U"z̥", U"z̰", U"z̪", U"z̻", U"z̩", U"z̝", U"z̞" },
	{ U"ý", U"ȳ", U"ỳ", U"ÿ", U"ỹ", U"o̟", U"o̱", U"o̹", U"o̜", U"t̚" },
};

void structTextGridEditor :: v_drawSelectionViewer () {
	TextGrid grid = (TextGrid) our data;
	Graphics_setWindow (our graphics.get(), 0.5, 10.5, 0.5, 12.5);
	Graphics_setColour (our graphics.get(), Graphics_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.5, 10.5, 0.5, 12.5);
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
	Graphics_setFont (our graphics.get(), kGraphics_font_TIMES);
	Graphics_setFontSize (our graphics.get(), 12);
	Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
	for (int irow = 1; irow <= 12; irow ++) {
		for (int icol = 1; icol <= 10; icol ++) {
			Graphics_text (our graphics.get(), icol, 13-irow, characters [irow-1] [icol-1]);
		}
	}
}

static void do_drawWhileDragging (TextGridEditor me, double numberOfTiers, bool selectedTier [], double x, double soundY) {
	long itier;
	for (itier = 1; itier <= numberOfTiers; itier ++) if (selectedTier [itier]) {
		double ymin = soundY * (1.0 - (double) itier / numberOfTiers);
		double ymax = soundY * (1.0 - (double) (itier - 1) / numberOfTiers);
		Graphics_setLineWidth (my graphics.get(), 7.0);
		Graphics_line (my graphics.get(), x, ymin, x, ymax);
	}
	Graphics_setLineWidth (my graphics.get(), 1);
	Graphics_line (my graphics.get(), x, 0.0, x, 1.01);
	Graphics_text (my graphics.get(), x, 1.01, Melder_fixed (x, 6));
}

static void do_dragBoundary (TextGridEditor me, double xbegin, int iClickedTier, int shiftKeyPressed) {
	TextGrid grid = (TextGrid) my data;
	int numberOfTiers = grid -> tiers->size, itierDrop;
	double xWC = xbegin, yWC;
	double leftDraggingBoundary = my tmin, rightDraggingBoundary = my tmax;   // initial dragging range
	bool selectedTier [1000];
	double soundY = _TextGridEditor_computeSoundY (me);

	/*
	 * Determine the set of selected boundaries and points, and the dragging range.
	 */
	for (int itier = 1; itier <= numberOfTiers; itier ++) {
		selectedTier [itier] = false;   // the default
		/*
		 * If she has pressed the shift key, let her drag all the boundaries and points at this time.
		 * Otherwise, let her only drag the boundary or point on the clicked tier.
		 */
		if (itier == iClickedTier || shiftKeyPressed == my p_shiftDragMultiple) {
			IntervalTier intervalTier;
			TextTier textTier;
			_AnyTier_identifyClass (grid -> tiers->at [itier], & intervalTier, & textTier);
			if (intervalTier) {
				long ibound = IntervalTier_hasBoundary (intervalTier, xbegin);
				if (ibound) {
					TextInterval leftInterval = intervalTier -> intervals.at [ibound - 1];
					TextInterval rightInterval = intervalTier -> intervals.at [ibound];
					selectedTier [itier] = true;
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
				if (AnyTier_hasPoint (textTier->asAnyTier(), xbegin)) {
					/*
					 * Other than with boundaries on interval tiers,
					 * points on text tiers can be dragged past their neighbours.
					 */
					selectedTier [itier] = true;
				}
			}
		}
	}

	Graphics_xorOn (my graphics.get(), Graphics_MAROON);
	Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_BOTTOM);
	do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // draw at old position
	while (Graphics_mouseStillDown (my graphics.get())) {
		double xWC_new;
		Graphics_getMouseLocation (my graphics.get(), & xWC_new, & yWC);
		if (xWC_new != xWC) {
			do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // undraw at old position
			xWC = xWC_new;
			do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // draw at new position
		}
	}
	do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);   // undraw at new position
	Graphics_xorOff (my graphics.get());

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
		Function anyTierDrop = grid -> tiers->at [itierDrop];
		if (anyTierDrop -> classInfo == classIntervalTier) {
			IntervalTier tierDrop = (IntervalTier) anyTierDrop;
			long ibound;
			for (ibound = 1; ibound < tierDrop -> intervals.size; ibound ++) {
				TextInterval left = tierDrop -> intervals.at [ibound];
				if (fabs (Graphics_dxWCtoMM (my graphics.get(), xWC - left -> xmax)) < 1.5) {   // near a boundary?
					/*
					 * Snap to boundary.
					 */
					xWC = left -> xmax;
				}
			}
		} else {
			TextTier tierDrop = (TextTier) anyTierDrop;
			long ipoint;
			for (ipoint = 1; ipoint <= tierDrop -> points.size; ipoint ++) {
				TextPoint point = tierDrop -> points.at [ipoint];
				if (fabs (Graphics_dxWCtoMM (my graphics.get(), xWC - point -> number)) < 1.5) {   // near a point?
					/*
					 * Snap to point.
					 */
					xWC = point -> number;
				}
			}
		}
	} else if (xbegin != my startSelection && fabs (Graphics_dxWCtoMM (my graphics.get(), xWC - my startSelection)) < 1.5) {   // near the cursor?
		/*
		 * Snap to cursor.
		 */
		xWC = my startSelection;
	} else if (xbegin != my endSelection && fabs (Graphics_dxWCtoMM (my graphics.get(), xWC - my endSelection)) < 1.5) {   // near the cursor?
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

	Editor_save (me, U"Drag");

	for (int itier = 1; itier <= numberOfTiers; itier ++) if (selectedTier [itier]) {
		IntervalTier intervalTier;
		TextTier textTier;
		_AnyTier_identifyClass (grid -> tiers->at [itier], & intervalTier, & textTier);
		if (intervalTier) {
			long ibound, numberOfIntervals = intervalTier -> intervals.size;
			for (ibound = 2; ibound <= numberOfIntervals; ibound ++) {
				TextInterval left = intervalTier -> intervals.at [ibound - 1], right = intervalTier -> intervals.at [ibound];
				if (left -> xmax == xbegin) {   // boundary dragged?
					left -> xmax = right -> xmin = xWC;   // move boundary to drop site
					break;
				}
			}
		} else {
			long iDraggedPoint = AnyTier_hasPoint (textTier->asAnyTier(), xbegin);
			if (iDraggedPoint) {
				long dropSiteHasPoint = AnyTier_hasPoint (textTier->asAnyTier(), xWC);
				if (dropSiteHasPoint) {
					Melder_warning (U"Cannot drop point on an existing point.");
				} else {
					TextPoint point = textTier -> points.at [iDraggedPoint];
					/*
					 * Move point to drop site. May have passed another point.
					 */
					autoTextPoint newPoint = Data_copy (point);
					newPoint -> number = xWC;   // move point to drop site
					textTier -> points. removeItem (iDraggedPoint);
					textTier -> points. addItem_move (newPoint.move());
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
	FunctionEditor_marksChanged (me, true);
	Editor_broadcastDataChanged (me);
}

bool structTextGridEditor :: v_click (double xclick, double yWC, bool shiftKeyPressed) {
	TextGrid grid = (TextGrid) our data;
	double tmin, tmax, x, y;
	long ntiers = grid -> tiers->size, iClickedTier, iClickedInterval, iClickedPoint;
	int clickedLeftBoundary = 0;
	bool nearBoundaryOrPoint, nearCursorCircle, drag = false;
	IntervalTier intervalTier;
	TextTier textTier;
	TextInterval interval = nullptr;
	TextPoint point = nullptr;
	double soundY = _TextGridEditor_computeSoundY (this);
	double tnear;

	/*
	 * In answer to a click in the sound part,
	 * we keep the same tier selected and move the cursor or drag the "yellow" selection.
	 */
	if (yWC > soundY) {   /* Clicked in sound part? */
		if ((our p_spectrogram_show || our p_formant_show) && yWC < 0.5 * (soundY + 1.0)) {
			our d_spectrogram_cursor = our p_spectrogram_viewFrom +
				2.0 * (yWC - soundY) / (1.0 - soundY) * (our p_spectrogram_viewTo - our p_spectrogram_viewFrom);
		}
		our TextGridEditor_Parent :: v_click (xclick, yWC, shiftKeyPressed);
		return FunctionEditor_UPDATE_NEEDED;
	}

	/*
	 * She clicked in the grid part.
	 * We select the tier in which she clicked.
	 */
	iClickedTier = _TextGridEditor_yWCtoTier (this, yWC);

	if (xclick <= our startWindow || xclick >= our endWindow) {
		our selectedTier = iClickedTier;
		return FunctionEditor_UPDATE_NEEDED;
	}

	_TextGridEditor_timeToInterval (this, xclick, iClickedTier, & tmin, & tmax);
	_AnyTier_identifyClass (grid -> tiers->at [iClickedTier], & intervalTier, & textTier);

	/*
	 * Get the time of the nearest boundary or point.
	 */
	tnear = undefined;
	if (intervalTier) {
		iClickedInterval = IntervalTier_timeToIndex (intervalTier, xclick);
		if (iClickedInterval) {
			interval = intervalTier -> intervals.at [iClickedInterval];
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
			our selectedTier = iClickedTier;
			return FunctionEditor_UPDATE_NEEDED;
		}
	} else {
		iClickedPoint = AnyTier_timeToNearestIndex (textTier->asAnyTier(), xclick);
		if (iClickedPoint) {
			point = textTier -> points.at [iClickedPoint];
			tnear = point -> number;
		}
	}
	Melder_assert (! (intervalTier && ! clickedLeftBoundary));

	/*
	 * Where did she click?
	 */
	nearBoundaryOrPoint = ( isdefined (tnear) && fabs (Graphics_dxWCtoMM (our graphics.get(), xclick - tnear)) < 1.5 );
	nearCursorCircle = ( our startSelection == our endSelection && Graphics_distanceWCtoMM (our graphics.get(), xclick, yWC,
		our startSelection, (ntiers + 1 - iClickedTier) * soundY / ntiers - Graphics_dyMMtoWC (our graphics.get(), 1.5)) < 1.5 );

	/*
	 * Find out whether this is a click or a drag.
	 */
	while (Graphics_mouseStillDown (our graphics.get())) {
		Graphics_getMouseLocation (our graphics.get(), & x, & y);
		if (x < our startWindow) x = our startWindow;
		if (x > our endWindow) x = our endWindow;
		if (fabs (Graphics_dxWCtoMM (our graphics.get(), x - xclick)) > 1.5) {
			drag = true;
			break;
		}
	}

	if (nearBoundaryOrPoint) {
		/*
		 * Possibility 1: she clicked near a boundary or point.
		 * Select or drag it.
		 */
		if (intervalTier && (clickedLeftBoundary < 2 || clickedLeftBoundary > intervalTier -> intervals.size)) {
			/*
			 * Ignore click on left edge of first interval or right edge of last interval.
			 */
			our selectedTier = iClickedTier;
		} else if (drag) {
			/*
			 * The tier that has been clicked becomes the new selected tier.
			 * This has to be done before the next Update, i.e. also before do_dragBoundary!
			 */
			our selectedTier = iClickedTier;
			do_dragBoundary (this, tnear, iClickedTier, shiftKeyPressed);
			return FunctionEditor_NO_UPDATE_NEEDED;
		} else {
			/*
			 * If she clicked on an unselected boundary or point, we select it.
			 */
			if (shiftKeyPressed) {
				if (tnear > 0.5 * (our startSelection + our endSelection))
					our endSelection = tnear;
				else
					our startSelection = tnear;
			} else {
				our startSelection = our endSelection = tnear;   // move cursor so that the boundary or point is selected
			}
			our selectedTier = iClickedTier;
		}
	} else if (nearCursorCircle) {
		/*
		 * Possibility 2: she clicked near the cursor circle.
		 * Insert boundary or point. There is no danger that we insert on top of an existing boundary or point,
		 * because we are not 'nearBoundaryOrPoint'.
		 */
		insertBoundaryOrPoint (this, iClickedTier, our startSelection, our startSelection, false);
		our selectedTier = iClickedTier;
		FunctionEditor_marksChanged (this, true);
		Editor_broadcastDataChanged (this);
		if (drag) Graphics_waitMouseUp (our graphics.get());
		return FunctionEditor_NO_UPDATE_NEEDED;
	} else {
		/*
		 * Possibility 3: she clicked in empty space.
		 */
		if (intervalTier) {
			our startSelection = tmin;
			our endSelection = tmax;
		}
		selectedTier = iClickedTier;
	}
	if (drag) Graphics_waitMouseUp (our graphics.get());
	return FunctionEditor_UPDATE_NEEDED;
}

bool structTextGridEditor :: v_clickB (double t, double yWC) {
	double soundY = _TextGridEditor_computeSoundY (this);

	if (yWC > soundY) {   // clicked in sound part?
		if (t < our endWindow) {
			our startSelection = t;
			if (our startSelection > our endSelection) {
				double dummy = our startSelection;
				our startSelection = our endSelection;
				our endSelection = dummy;
			}
			return FunctionEditor_UPDATE_NEEDED;
		} else {
			return structTimeSoundEditor :: v_clickB (t, yWC);
		}
	}
	int itier = _TextGridEditor_yWCtoTier (this, yWC);
	double tmin, tmax;
	_TextGridEditor_timeToInterval (this, t, itier, & tmin, & tmax);
	our startSelection = t - tmin < tmax - t ? tmin : tmax;   // to nearest boundary
	if (our startSelection > our endSelection) {
		double dummy = our startSelection;
		our startSelection = our endSelection;
		our endSelection = dummy;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

bool structTextGridEditor :: v_clickE (double t, double yWC) {
	double soundY = _TextGridEditor_computeSoundY (this);

	if (yWC > soundY) {   // clicked in sound part?
		our endSelection = t;
		if (our startSelection > our endSelection) {
			double dummy = our startSelection;
			our startSelection = our endSelection;
			our endSelection = dummy;
		}
		return FunctionEditor_UPDATE_NEEDED;
	}
	int itier = _TextGridEditor_yWCtoTier (this, yWC);
	double tmin, tmax;
	_TextGridEditor_timeToInterval (this, t, itier, & tmin, & tmax);
	our endSelection = t - tmin < tmax - t ? tmin : tmax;
	if (our startSelection > our endSelection) {
		double dummy = our startSelection;
		our startSelection = our endSelection;
		our endSelection = dummy;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

void structTextGridEditor :: v_clickSelectionViewer (double xWC, double yWC) {
	TextGrid grid = (TextGrid) our data;
	int rowNumber = 1 + (int) ((1.0-yWC) * 12.0);
	int columnNumber = 1 + (int) (xWC * 10.0);
	if (rowNumber < 1 || rowNumber > 12) return;
	if (columnNumber < 1 || columnNumber > 10) return;
	const char32 *character = characters [rowNumber-1] [columnNumber-1];
	character += str32len (character) - 1;
	if (our text) {
		long first = 0, last = 0;
		char32 *oldText = GuiText_getStringAndSelectionPosition (our text, & first, & last);
		static MelderString newText { };
		MelderString_empty (& newText);
		MelderString_ncopy (& newText, oldText, first);
		MelderString_append (& newText, character);
		MelderString_append (& newText, oldText + last);
		Melder_free (oldText);
		if (our selectedTier) {
			IntervalTier intervalTier;
			TextTier textTier;
			_AnyTier_identifyClass (grid -> tiers->at [our selectedTier], & intervalTier, & textTier);
			if (intervalTier) {
				long selectedInterval = getSelectedInterval (this);
				if (selectedInterval) {
					TextInterval interval = intervalTier -> intervals.at [selectedInterval];
					TextInterval_setText (interval, newText.string);

					our suppressRedraw = true;   // prevent valueChangedCallback from redrawing
					trace (U"setting new text ", newText.string);
					GuiText_setString (text, newText.string);
					GuiText_setSelection (text, first + 1, first + 1);
					our suppressRedraw = false;

					FunctionEditor_redraw (this);
					Editor_broadcastDataChanged (this);
				}
			} else {
				long selectedPoint = getSelectedPoint (this);
				if (selectedPoint) {
					TextPoint point = textTier -> points.at [selectedPoint];
					Melder_free (point -> mark);
					if (str32spn (newText.string, U" \n\t") != str32len (newText.string))   // any visible characters?
					point -> mark = Melder_dup_f (newText.string);

					our suppressRedraw = true;   // prevent valueChangedCallback from redrawing
					trace (U"setting new text ", newText.string);
					GuiText_setString (text, newText.string);
					GuiText_setSelection (text, first + 1, first + 1);
					our suppressRedraw = false;

					FunctionEditor_redraw (this);
					Editor_broadcastDataChanged (this);
				}
			}
		}
	}
}

void structTextGridEditor :: v_play (double tmin, double tmax) {
	long numberOfChannels = d_longSound.data ? d_longSound.data -> numberOfChannels : d_sound.data -> ny;
	long numberOfMuteChannels = 0;
	bool *muteChannels = d_sound . muteChannels;
	for (long i = 1; i <= numberOfChannels; i ++) {
		if (muteChannels [i]) {
			numberOfMuteChannels ++;
		}
	}
	long numberOfChannelsToPlay = numberOfChannels - numberOfMuteChannels;
	if (numberOfChannelsToPlay == 0) {
		Melder_throw (U"Please select at least one channel to play.");
	}
	if (d_longSound.data) {
		if (numberOfMuteChannels > 0) {
			autoSound part = LongSound_extractPart (d_longSound.data, tmin, tmax, 1);
			autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
			MixingMatrix_muteAndActivateChannels (thee.get(), muteChannels);
			Sound_and_MixingMatrix_playPart (part.get(), thee.get(), tmin, tmax, theFunctionEditor_playCallback, this);
		} else {
			LongSound_playPart (d_longSound.data, tmin, tmax, theFunctionEditor_playCallback, this);
		}
	} else {
		if (numberOfMuteChannels > 0) {
			autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
			MixingMatrix_muteAndActivateChannels (thee.get(), muteChannels);
			Sound_and_MixingMatrix_playPart (d_sound.data, thee.get(), tmin, tmax, theFunctionEditor_playCallback, this);
			
		} else {
			Sound_playPart ((Sound) d_sound.data, tmin, tmax, theFunctionEditor_playCallback, this);
		}
	}
}

void structTextGridEditor :: v_updateText () {
	TextGrid grid = (TextGrid) our data;
	const char32 *newText = U"";
	trace (U"selected tier ", our selectedTier);
	if (our selectedTier) {
		IntervalTier intervalTier;
		TextTier textTier;
		_AnyTier_identifyClass (grid -> tiers->at [selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			long iinterval = IntervalTier_timeToIndex (intervalTier, our startSelection);
			if (iinterval) {
				TextInterval interval = intervalTier -> intervals.at [iinterval];
				if (interval -> text) {
					newText = interval -> text;
				}
			}
		} else {
			long ipoint = AnyTier_hasPoint (textTier->asAnyTier(), our startSelection);
			if (ipoint) {
				TextPoint point = textTier -> points.at [ipoint];
				if (point -> mark) {
					newText = point -> mark;
				}
			}
		}
	}
	//Melder_casual ("v_updateText in editor %ld %ls %d", this, name, (int) suppressRedraw);
	if (our text) {
		our suppressRedraw = true;   // prevent valueChangedCallback from redrawing
		trace (U"setting new text ", newText);
		GuiText_setString (text, newText);
		long cursor = str32len (newText);   // at end
		GuiText_setSelection (text, cursor, cursor);
		our suppressRedraw = false;
	}
}

void structTextGridEditor :: v_prefs_addFields (EditorCommand cmd) {
	UiField radio;
	NATURAL (U"Font size (points)", default_fontSize ())
	OPTIONMENU_ENUM (U"Text alignment in intervals", kGraphics_horizontalAlignment, kGraphics_horizontalAlignment_DEFAULT)
	OPTIONMENU (U"The symbols %#_^ in labels", default_useTextStyles () + 1)
		OPTION (U"are shown as typed")
		OPTION (U"mean italic/bold/sub/super")
	OPTIONMENU (U"With the shift key, you drag", default_shiftDragMultiple () + 1)
		OPTION (U"a single boundary")
		OPTION (U"multiple boundaries")
	OPTIONMENU_ENUM (U"Show number of", kTextGridEditor_showNumberOf, kTextGridEditor_showNumberOf_DEFAULT)
	OPTIONMENU_ENUM (U"Paint intervals green whose label...", kMelder_string, kMelder_string_DEFAULT)
	SENTENCE (U"...the text", default_greenString ())
}
void structTextGridEditor :: v_prefs_setValues (EditorCommand cmd) {
	SET_INTEGER (U"The symbols %#_^ in labels", p_useTextStyles + 1)
	SET_INTEGER (U"Font size", p_fontSize)
	SET_ENUM (U"Text alignment in intervals", kGraphics_horizontalAlignment, p_alignment)
	SET_INTEGER (U"With the shift key, you drag", p_shiftDragMultiple + 1)
	SET_ENUM (U"Show number of", kTextGridEditor_showNumberOf, p_showNumberOf)
	SET_ENUM (U"Paint intervals green whose label...", kMelder_string, p_greenMethod)
	SET_STRING (U"...the text", p_greenString)
}
void structTextGridEditor :: v_prefs_getValues (EditorCommand cmd) {
	pref_useTextStyles () = p_useTextStyles = GET_INTEGER (U"The symbols %#_^ in labels") - 1;
	pref_fontSize () = p_fontSize = GET_INTEGER (U"Font size");
	pref_alignment () = p_alignment = GET_ENUM (kGraphics_horizontalAlignment, U"Text alignment in intervals");
	pref_shiftDragMultiple () = p_shiftDragMultiple = GET_INTEGER (U"With the shift key, you drag") - 1;
	pref_showNumberOf () = p_showNumberOf = GET_ENUM (kTextGridEditor_showNumberOf, U"Show number of");
	pref_greenMethod () = p_greenMethod = GET_ENUM (kMelder_string, U"Paint intervals green whose label...");
	pref_str32cpy2 (pref_greenString (), p_greenString, GET_STRING (U"...the text"));
	FunctionEditor_redraw (this);
}

void structTextGridEditor :: v_createMenuItems_view_timeDomain (EditorMenu menu) {
	TextGridEditor_Parent :: v_createMenuItems_view_timeDomain (menu);
	EditorMenu_addCommand (menu, U"Select previous tier", GuiMenu_OPTION | GuiMenu_UP_ARROW, menu_cb_SelectPreviousTier);
	EditorMenu_addCommand (menu, U"Select next tier", GuiMenu_OPTION | GuiMenu_DOWN_ARROW, menu_cb_SelectNextTier);
	EditorMenu_addCommand (menu, U"Select previous interval", GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_SelectPreviousInterval);
	EditorMenu_addCommand (menu, U"Select next interval", GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_SelectNextInterval);
	EditorMenu_addCommand (menu, U"Extend-select left", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_ExtendSelectPreviousInterval);
	EditorMenu_addCommand (menu, U"Extend-select right", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_ExtendSelectNextInterval);
}

void structTextGridEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	if (our v_hasAnalysis () && our p_spectrogram_show && (our d_longSound.data || our d_sound.data)) {
		double soundY = _TextGridEditor_computeSoundY (this), soundY2 = 0.5 * (1.0 + soundY);
		//Graphics_highlight (our graphics.get(), left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_highlight (our graphics.get(), left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_highlight (our graphics.get(), left, right, bottom, top);
	}
}

void structTextGridEditor :: v_unhighlightSelection (double left, double right, double bottom, double top) {
	if (our v_hasAnalysis () && our p_spectrogram_show && (our d_longSound.data || our d_sound.data)) {
		double soundY = _TextGridEditor_computeSoundY (this), soundY2 = 0.5 * (1.0 + soundY);
		//Graphics_unhighlight (our graphics.get(), left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_unhighlight (our graphics.get(), left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_unhighlight (our graphics.get(), left, right, bottom, top);
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
	EditorMenu_addCommand (menu, U"Draw visible pitch contour and TextGrid...", 0, menu_cb_DrawTextGridAndPitch);
}

void structTextGridEditor :: v_updateMenuItems_file () {
	TextGridEditor_Parent :: v_updateMenuItems_file ();
	GuiThing_setSensitive (extractSelectedTextGridPreserveTimesButton, our endSelection > our startSelection);
	GuiThing_setSensitive (extractSelectedTextGridTimeFromZeroButton,  our endSelection > our startSelection);
}

/********** EXPORTED **********/

void TextGridEditor_init (TextGridEditor me, const char32 *title, TextGrid grid, Sampled sound, bool ownSound, SpellingChecker spellingChecker, const char *callbackSocket)
{
	my spellingChecker = spellingChecker;   // set in time
	my callbackSocket = Melder_strdup (callbackSocket);

	TimeSoundAnalysisEditor_init (me, title, grid, sound, ownSound);

	my selectedTier = 1;
	my v_updateText ();   // to reflect changed tier selection
	if (my endWindow - my startWindow > 30.0) {
		my endWindow = my startWindow + 30.0;
		if (my startWindow == my tmin)
			my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
		FunctionEditor_marksChanged (me, false);
	}
	if (spellingChecker)
		GuiText_setSelection (my text, 0, 0);
	if (sound && sound -> xmin == 0.0 && grid -> xmin != 0.0 && grid -> xmax > sound -> xmax)
		Melder_warning (U"The time domain of the TextGrid (starting at ",
			Melder_fixed (grid -> xmin, 6), U" seconds) does not overlap with that of the sound "
			U"(which starts at 0 seconds).\nIf you want to repair this, you can select the TextGrid "
			U"and choose “Shift times to...” from the Modify menu "
			U"to shift the starting time of the TextGrid to zero.");
}

autoTextGridEditor TextGridEditor_create (const char32 *title, TextGrid grid, Sampled sound, bool ownSound, SpellingChecker spellingChecker, const char *callbackSocket) {
	try {
		autoTextGridEditor me = Thing_new (TextGridEditor);
		TextGridEditor_init (me.get(), title, grid, sound, ownSound, spellingChecker, callbackSocket);
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid window not created.");
	}
}

/* End of file TextGridEditor.cpp */
