/* AnyTextGridEditor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#include "AnyTextGridEditor.h"
#include "EditorM.h"
#include "SoundEditor.h"
#include "Sound_and_MixingMatrix.h"
#include "Sound_and_Spectrogram.h"
#include "TextGrid_Sound.h"
#include "SpeechSynthesizer_and_TextGrid.h"

Thing_implement (AnyTextGridEditor, TimeSoundAnalysisEditor, 0);

#include "Prefs_define.h"
#include "AnyTextGridEditor_prefs.h"
#include "Prefs_install.h"
#include "AnyTextGridEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "AnyTextGridEditor_prefs.h"

void structAnyTextGridEditor :: v1_info () {
	AnyTextGridEditor_Parent :: v1_info ();
	MelderInfo_writeLine (U"Selected tier: ", our textGridArea() -> selectedTier);
	MelderInfo_writeLine (U"TextGrid uses text styles: ", our textGridArea() -> instancePref_useTextStyles());
	MelderInfo_writeLine (U"TextGrid font size: ", our textGridArea() -> instancePref_fontSize());
	MelderInfo_writeLine (U"TextGrid alignment: ", kGraphics_horizontalAlignment_getText (our textGridArea() -> instancePref_alignment()));
}

/********** UTILITIES **********/

static double _TextGridEditor_computeSoundY (AnyTextGridEditor me) {
	const integer numberOfTiers = my textGrid() -> tiers->size;
	const bool showAnalysis = (
		my soundAnalysisArea -> instancePref_spectrogram_show() ||
		my soundAnalysisArea -> instancePref_pitch_show() ||
		my soundAnalysisArea -> instancePref_intensity_show() ||
		my soundAnalysisArea -> instancePref_formant_show())
		&& my soundOrLongSound()
	;
	const integer numberOfVisibleChannels =
		my soundOrLongSound() ? Melder_clippedRight (my soundOrLongSound() -> ny, 8_integer) : 1;
	return my soundOrLongSound() ? numberOfTiers / (2.0 * numberOfVisibleChannels + numberOfTiers * (showAnalysis ? 1.8 : 1.3)) : 1.0;
}

static integer _TextGridEditor_y_fraction_local_toTier (AnyTextGridEditor me, double y_fraction_local) {
	const integer numberOfTiers = my textGrid() -> tiers->size;
	integer tierNumber = numberOfTiers - Melder_ifloor (y_fraction_local * (double) numberOfTiers);
	Melder_clip (1_integer, & tierNumber, numberOfTiers);
	return tierNumber;
}

static void _TextGridEditor_timeToInterval (AnyTextGridEditor me, double t, integer tierNumber,
	double *out_tmin, double *out_tmax)
{
	Melder_assert (isdefined (t));
	const Function tier = my textGrid() -> tiers->at [tierNumber];
	IntervalTier intervalTier;
	TextTier textTier;
	AnyTextGridTier_identifyClass (tier, & intervalTier, & textTier);
	if (intervalTier) {
		integer iinterval = IntervalTier_timeToIndex (intervalTier, t);
		if (iinterval == 0) {
			if (t < my tmin) {
				iinterval = 1;
			} else {
				iinterval = intervalTier -> intervals.size;
			}
		}
		Melder_assert (iinterval >= 1);
		Melder_assert (iinterval <= intervalTier -> intervals.size);
		const TextInterval interval = intervalTier -> intervals.at [iinterval];
		*out_tmin = interval -> xmin;
		*out_tmax = interval -> xmax;
	} else {
		const integer n = textTier -> points.size;
		if (n == 0) {
			*out_tmin = my tmin;
			*out_tmax = my tmax;
		} else {
			integer ipointleft = AnyTier_timeToLowIndex (textTier->asAnyTier(), t);
			*out_tmin = ( ipointleft == 0 ? my tmin : textTier -> points.at [ipointleft] -> number );
			*out_tmax = ( ipointleft == n ? my tmax : textTier -> points.at [ipointleft + 1] -> number );
		}
	}
	Melder_clipLeft (my tmin, out_tmin);
	Melder_clipRight (out_tmax, my tmax);
}

/********** METHODS **********/

/*
 * The main invariant of the TextGridEditor is that the selected interval
 * always has the cursor in it, and that the cursor always selects an interval
 * if the selected tier is an interval tier.
 */

/***** FILE MENU *****/

static void CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_preserveTimes (AnyTextGridEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (my endSelection <= my startSelection)
			Melder_throw (U"No selection.");
		autoTextGrid result = TextGrid_extractPart (my textGrid(), my startSelection, my endSelection, true);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_timeFromZero (AnyTextGridEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		if (my endSelection <= my startSelection)
			Melder_throw (U"No selection.");
		autoTextGrid result = TextGrid_extractPart (my textGrid(), my startSelection, my endSelection, false);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

void structAnyTextGridEditor :: v_createMenuItems_extract (EditorMenu menu) {
	AnyTextGridEditor_Parent :: v_createMenuItems_extract (menu);
	extractSelectedTextGridPreserveTimesButton = EditorMenu_addCommand (menu, U"Extract selected TextGrid (preserve times)", 0,
			CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_preserveTimes);
	extractSelectedTextGridTimeFromZeroButton = EditorMenu_addCommand (menu, U"Extract selected TextGrid (time from 0)", 0,
			CONVERT_DATA_TO_ONE__ExtractSelectedTextGrid_timeFromZero);
}

static void menu_cb_DrawVisibleTextGrid (AnyTextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible TextGrid", nullptr)
		my textGridArea() -> v_form_pictureWindow (cmd);
		my textGridArea() -> v_form_pictureMargins (cmd);
		my textGridArea() -> v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my textGridArea() -> default_picture_garnish())
	EDITOR_OK
		my textGridArea() -> v_ok_pictureWindow (cmd);
		my textGridArea() -> v_ok_pictureMargins (cmd);
		my textGridArea() -> v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my textGridArea() -> classPref_picture_garnish())
	EDITOR_DO
		my textGridArea() -> v_do_pictureWindow (cmd);
		my textGridArea() -> v_do_pictureMargins (cmd);
		my textGridArea() -> v_do_pictureSelection (cmd);
		my textGridArea() -> setClassPref_picture_garnish (garnish);
		Editor_openPraatPicture (me);
		TextGrid_Sound_draw (my textGrid(), nullptr, my pictureGraphics,
				my startWindow, my endWindow, true, my textGridArea() -> instancePref_useTextStyles(), garnish);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_DrawVisibleSoundAndTextGrid (AnyTextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw visible sound and TextGrid", nullptr)
		my textGridArea() -> v_form_pictureWindow (cmd);
		my textGridArea() -> v_form_pictureMargins (cmd);
		my textGridArea() -> v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my textGridArea() -> default_picture_garnish())
	EDITOR_OK
		my textGridArea() -> v_ok_pictureWindow (cmd);
		my textGridArea() -> v_ok_pictureMargins (cmd);
		my textGridArea() -> v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my textGridArea() -> classPref_picture_garnish())
	EDITOR_DO
		my textGridArea() -> v_do_pictureWindow (cmd);
		my textGridArea() -> v_do_pictureMargins (cmd);
		my textGridArea() -> v_do_pictureSelection (cmd);
		my textGridArea() -> setClassPref_picture_garnish (garnish);
		Editor_openPraatPicture (me);
		{// scope
			autoSound sound = my longSound() ?
				LongSound_extractPart (my longSound(), my startWindow, my endWindow, true) :
				Sound_extractPart (my sound(), my startWindow, my endWindow,
						kSound_windowShape::RECTANGULAR, 1.0, true);
			TextGrid_Sound_draw (my textGrid(), sound.get(), my pictureGraphics,
					my startWindow, my endWindow, true, my textGridArea() -> instancePref_useTextStyles(), garnish);
		}
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

void structAnyTextGridEditor :: v_createMenuItems_draw (EditorMenu menu) {
	AnyTextGridEditor_Parent :: v_createMenuItems_draw (menu);
	EditorMenu_addCommand (menu, U"Draw visible TextGrid...", 0, menu_cb_DrawVisibleTextGrid);
	if (our soundOrLongSound())
		EditorMenu_addCommand (menu, U"Draw visible sound and TextGrid...", 0, menu_cb_DrawVisibleSoundAndTextGrid);
}

/***** EDIT MENU *****/

#ifndef macintosh
static void menu_cb_Cut (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_cut (my textArea);
}
static void menu_cb_Copy (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_copy (my textArea);
}
static void menu_cb_Paste (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_paste (my textArea);
}
static void menu_cb_Erase (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_remove (my textArea);
}
#endif

static void menu_cb_ConvertToBackslashTrigraphs (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Convert to Backslash Trigraphs");
	TextGrid_convertToBackslashTrigraphs (my textGrid());
	Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_updateText()
	//FunctionEditor_updateText (me); TRY OUT 2022-07-23
	Editor_broadcastDataChanged (me);
}

static void menu_cb_ConvertToUnicode (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Convert to Unicode");
	TextGrid_convertToUnicode (my textGrid());
	Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_updateText()
	//FunctionEditor_updateText (me); TRY OUT 2022-07-23
	Editor_broadcastDataChanged (me);
}

/***** QUERY MENU *****/

static void QUERY_DATA_FOR_REAL__GetStartingPointOfInterval (AnyTextGridEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		checkTierSelection (my textGridArea().get(), U"query the starting point of an interval");
		const Function anyTier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
		Melder_require (anyTier -> classInfo == classIntervalTier,
			U"The selected tier is not an interval tier.");
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		const double result = ( iinterval < 1 || iinterval > tier -> intervals.size ? undefined :
				tier -> intervals.at [iinterval] -> xmin );
	QUERY_DATA_FOR_REAL_END (U" seconds")
}

static void QUERY_DATA_FOR_REAL__GetEndPointOfInterval (AnyTextGridEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_REAL
		checkTierSelection (my textGridArea().get(), U"query the end point of an interval");
		const Function anyTier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
		Melder_require (anyTier -> classInfo == classIntervalTier,
			U"The selected tier is not an interval tier.");
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		const double result = ( iinterval < 1 || iinterval > tier -> intervals.size ? undefined :
				tier -> intervals.at [iinterval] -> xmax );
	QUERY_DATA_FOR_REAL_END (U" seconds")
}

static void QUERY_DATA_FOR_STRING__GetLabelOfInterval (AnyTextGridEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	QUERY_DATA_FOR_STRING
		checkTierSelection (my textGridArea().get(), U"query the label of an interval");
		const Function anyTier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
		Melder_require (anyTier -> classInfo == classIntervalTier,
			U"The selected tier is not an interval tier.");
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		const conststring32 result = ( iinterval < 1 || iinterval > tier -> intervals.size ? U"" :
				tier -> intervals.at [iinterval] -> text.get() );
	QUERY_DATA_FOR_STRING_END
}

/***** VIEW MENU *****/

static void do_selectAdjacentTier (AnyTextGridEditor me, bool previous) {
	const integer n = my textGrid() -> tiers->size;
	if (n >= 2) {
		my textGridArea() -> selectedTier = ( previous ?
				my textGridArea() -> selectedTier > 1 ? my textGridArea() -> selectedTier - 1 : n :
				my textGridArea() -> selectedTier < n ? my textGridArea() -> selectedTier + 1 : 1 );
		_TextGridEditor_timeToInterval (me, my startSelection, my textGridArea() -> selectedTier, & my startSelection, & my endSelection);
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_SelectPreviousTier (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentTier (me, true);
}

static void menu_cb_SelectNextTier (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentTier (me, false);
}

static void do_selectAdjacentInterval (AnyTextGridEditor me, bool previous, bool shift) {
	IntervalTier intervalTier;
	TextTier textTier;
	if (my textGridArea() -> selectedTier < 1 || my textGridArea() -> selectedTier > my textGrid() -> tiers->size)
		return;
	AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my textGridArea() -> selectedTier], & intervalTier, & textTier);
	if (intervalTier) {
		const integer n = intervalTier -> intervals.size;
		if (n >= 2) {
			integer iinterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
			if (shift) {
				const integer binterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
				integer einterval = IntervalTier_timeToIndex (intervalTier, my endSelection);
				if (my endSelection == intervalTier -> xmax)
					einterval ++;
				if (binterval < iinterval && einterval > iinterval + 1) {
					const TextInterval interval = intervalTier -> intervals.at [iinterval];
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
				} else if (previous) {
					if (einterval > iinterval + 1) {
						if (einterval <= n + 1) {
							const TextInterval interval = intervalTier -> intervals.at [einterval - 1];
							my endSelection = interval -> xmin;
						}
					} else if (binterval > 1) {
						const TextInterval interval = intervalTier -> intervals.at [binterval - 1];
						my startSelection = interval -> xmin;
					}
				} else {
					if (binterval < iinterval) {
						if (binterval > 0) {
							const TextInterval interval = intervalTier -> intervals.at [binterval];
							my startSelection = interval -> xmax;
						}
					} else if (einterval <= n) {
						const TextInterval interval = intervalTier -> intervals.at [einterval];
						my endSelection = interval -> xmax;
					}
				}
			} else {
				iinterval = ( previous ?
						iinterval > 1 ? iinterval - 1 : n :
						iinterval < n ? iinterval + 1 : 1 );
				const TextInterval interval = intervalTier -> intervals.at [iinterval];
				my startSelection = interval -> xmin;
				my endSelection = interval -> xmax;
			}
			Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToView()
			FunctionEditor_scrollToView (me, iinterval == n ? my startSelection : iinterval == 1 ? my endSelection : (my startSelection + my endSelection) / 2);
		}
	} else {
		const integer n = textTier -> points.size;
		if (n >= 2) {
			integer ipoint = AnyTier_timeToHighIndex (textTier->asAnyTier(), my startSelection);
			ipoint = ( previous ?
					ipoint > 1 ? ipoint - 1 : n :
					ipoint < n ? ipoint + 1 : 1 );
			const TextPoint point = textTier -> points.at [ipoint];
			my startSelection = my endSelection = point -> number;
			Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToView()
			FunctionEditor_scrollToView (me, my startSelection);
		}
	}
}

static void menu_cb_SelectPreviousInterval (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, true, false);
}

static void menu_cb_SelectNextInterval (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, false, false);
}

static void menu_cb_ExtendSelectPreviousInterval (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, true, true);
}

static void menu_cb_ExtendSelectNextInterval (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_selectAdjacentInterval (me, false, true);
}

/***** PITCH MENU *****/

static void menu_cb_DrawTextGridAndPitch (AnyTextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Draw TextGrid and Pitch separately", nullptr)
		my textGridArea() -> v_form_pictureWindow (cmd);
		LABEL (U"TextGrid:")
		BOOLEAN (showBoundariesAndPoints, U"Show boundaries and points", my textGridArea() -> default_picture_showBoundaries ());
		LABEL (U"Pitch:")
		BOOLEAN (speckle, U"Speckle", my textGridArea() -> default_picture_pitch_speckle ());
		my textGridArea() -> v_form_pictureMargins (cmd);
		my textGridArea() -> v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", my textGridArea() -> default_picture_garnish ());
	EDITOR_OK
		my textGridArea() -> v_ok_pictureWindow (cmd);
		SET_BOOLEAN (showBoundariesAndPoints, my textGridArea() -> classPref_picture_showBoundaries())
		SET_BOOLEAN (speckle, my textGridArea() -> classPref_picture_pitch_speckle())
		my textGridArea() -> v_ok_pictureMargins (cmd);
		my textGridArea() -> v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my textGridArea() -> classPref_picture_garnish())
	EDITOR_DO
		my textGridArea() -> v_do_pictureWindow (cmd);
		my textGridArea() -> setClassPref_picture_showBoundaries (showBoundariesAndPoints);   // set prefs even if analyses are missing (it would be annoying not to)
		my textGridArea() -> setClassPref_picture_pitch_speckle (speckle);
		my textGridArea() -> v_do_pictureMargins (cmd);
		my textGridArea() -> v_do_pictureSelection (cmd);
		my textGridArea() -> setClassPref_picture_garnish (garnish);
		SoundAnalysisArea_haveVisiblePitch (my soundAnalysisArea.get());
		Editor_openPraatPicture (me);
		const double pitchFloor_hidden = Function_convertStandardToSpecialUnit (my soundAnalysisArea -> d_pitch.get(),
				my soundAnalysisArea -> instancePref_pitch_floor(), Pitch_LEVEL_FREQUENCY, (int) my soundAnalysisArea -> instancePref_pitch_unit());
		const double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (my soundAnalysisArea -> d_pitch.get(),
				my soundAnalysisArea -> instancePref_pitch_ceiling(), Pitch_LEVEL_FREQUENCY, (int) my soundAnalysisArea -> instancePref_pitch_unit());
		const double pitchFloor_overt = Function_convertToNonlogarithmic (my soundAnalysisArea -> d_pitch.get(),
				pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, (int) my soundAnalysisArea -> instancePref_pitch_unit());
		const double pitchCeiling_overt = Function_convertToNonlogarithmic (my soundAnalysisArea -> d_pitch.get(),
				pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, (int) my soundAnalysisArea -> instancePref_pitch_unit());
		const double pitchViewFrom_overt = ( my soundAnalysisArea -> instancePref_pitch_viewFrom() < my soundAnalysisArea -> instancePref_pitch_viewTo() ? my soundAnalysisArea -> instancePref_pitch_viewFrom() : pitchFloor_overt );
		const double pitchViewTo_overt = ( my soundAnalysisArea -> instancePref_pitch_viewFrom() < my soundAnalysisArea -> instancePref_pitch_viewTo() ? my soundAnalysisArea -> instancePref_pitch_viewTo() : pitchCeiling_overt );
		TextGrid_Pitch_drawSeparately (my textGrid(), my soundAnalysisArea -> d_pitch.get(), my pictureGraphics, my startWindow, my endWindow,
			pitchViewFrom_overt, pitchViewTo_overt, showBoundariesAndPoints, my textGridArea() -> instancePref_useTextStyles(), garnish,
			speckle, my soundAnalysisArea -> instancePref_pitch_unit()
		);
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

/***** INTERVAL MENU *****/

static void insertBoundaryOrPoint (AnyTextGridEditor me, integer itier, double t1, double t2, bool insertSecond) {
	const integer numberOfTiers = my textGrid() -> tiers->size;
	if (itier < 1 || itier > numberOfTiers)
		Melder_throw (U"No tier ", itier, U".");
	IntervalTier intervalTier;
	TextTier textTier;
	AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [itier], & intervalTier, & textTier);
	Melder_assert (t1 <= t2);

	if (intervalTier) {
		autoTextInterval rightNewInterval, midNewInterval;
		const bool t1IsABoundary = IntervalTier_hasTime (intervalTier, t1);
		const bool t2IsABoundary = IntervalTier_hasTime (intervalTier, t2);
		if (t1 == t2 && t1IsABoundary)
			Melder_throw (U"Cannot add a boundary at ", Melder_fixed (t1, 6), U" seconds, because there is already a boundary there.");
		if (t1IsABoundary && t2IsABoundary)
			Melder_throw (U"Cannot add boundaries at ", Melder_fixed (t1, 6), U" and ", Melder_fixed (t2, 6), U" seconds, because there are already boundaries there.");
		const integer iinterval = IntervalTier_timeToIndex (intervalTier, t1);
		const integer iinterval2 = t1 == t2 ? iinterval : IntervalTier_timeToIndex (intervalTier, t2);
		if (iinterval == 0 || iinterval2 == 0)
			Melder_throw (U"The selection is outside the time domain of the intervals.");
		const integer correctedIinterval2 = ( t2IsABoundary && iinterval2 == intervalTier -> intervals.size ? iinterval2 + 1 : iinterval2 );
		if (correctedIinterval2 > iinterval + 1 || (correctedIinterval2 > iinterval && ! t2IsABoundary))
			Melder_throw (U"The selection straddles a boundary.");
		const TextInterval interval = intervalTier -> intervals.at [iinterval];

		if (t1 == t2) {
			Editor_save (me, U"Add boundary");
		} else {
			Editor_save (me, U"Add interval");
		}

		if (itier == my textGridArea() -> selectedTier) {
			/*
				Divide up the label text into left, mid and right, depending on where the text selection is.
			*/
			integer left, right;
			autostring32 text = GuiText_getStringAndSelectionPosition (my textArea, & left, & right);
			const bool wholeTextIsSelected = ( right - left == str32len (text.get()) );
			rightNewInterval = TextInterval_create (t2, interval -> xmax, text.get() + right);
			text [right] = U'\0';
			midNewInterval = TextInterval_create (t1, t2, text.get() + left);
			if (! wholeTextIsSelected || t1 != t2)
				text [left] = U'\0';
			TextInterval_setText (interval, text.get());
		} else {
			/*
				Move the text to the left of the boundary.
			*/
			rightNewInterval = TextInterval_create (t2, interval -> xmax, U"");
			midNewInterval = TextInterval_create (t1, t2, U"");
		}
		if (t1IsABoundary) {
			/*
				Merge mid with left interval.
			*/
			if (interval -> xmin != t1)
				Melder_fatal (U"Boundary unequal: ", interval -> xmin, U" versus ", t1, U".");
			interval -> xmax = t2;
			TextInterval_setText (interval, Melder_cat (interval -> text.get(), midNewInterval -> text.get()));
		} else if (t2IsABoundary) {
			/*
				Merge mid and right interval.
			*/
			if (interval -> xmax != t2)
				Melder_fatal (U"Boundary unequal: ", interval -> xmax, U" versus ", t2, U".");
			interval -> xmax = t1;
			Melder_assert (rightNewInterval -> xmin == t2);
			Melder_assert (rightNewInterval -> xmax == t2);
			rightNewInterval -> xmin = t1;
			TextInterval_setText (rightNewInterval.get(), Melder_cat (midNewInterval -> text.get(), rightNewInterval -> text.get()));
		} else {
			interval -> xmax = t1;
			if (t1 != t2)
				intervalTier -> intervals.addItem_move (midNewInterval.move());
		}
		intervalTier -> intervals.addItem_move (rightNewInterval.move());
		if (insertSecond && numberOfTiers >= 2 && t1 == t2) {
			/*
				Find the last time before t on another tier.
			*/
			double tlast = interval -> xmin;
			for (integer jtier = 1; jtier <= numberOfTiers; jtier ++) {
				if (jtier != itier) {
					double startInterval, endInterval;
					_TextGridEditor_timeToInterval (me, t1, jtier, & startInterval, & endInterval);
					if (startInterval > tlast)
						tlast = startInterval;
				}
			}
			if (tlast > interval -> xmin && tlast < t1) {
				autoTextInterval newInterval = TextInterval_create (tlast, t1, U"");
				interval -> xmax = tlast;
				intervalTier -> intervals.addItem_move (newInterval.move());
			}
		}
	} else {
		Melder_assert (isdefined (t1));
		if (AnyTier_hasPoint (textTier->asAnyTier(), t1))
			Melder_throw (U"Cannot add a point at ", Melder_fixed (t1, 6), U" seconds, because there is already a point there.");

		Editor_save (me, U"Add point");

		autoTextPoint newPoint = TextPoint_create (t1, U"");
		textTier -> points. addItem_move (newPoint.move());
	}
	my startSelection = my endSelection = t1;
}

static void do_insertIntervalOnTier (AnyTextGridEditor me, int itier) {
	try {
		insertBoundaryOrPoint (me, itier,
			my duringPlay ? my playCursor : my startSelection,
			my duringPlay ? my playCursor : my endSelection,
			true
		);
		my textGridArea() -> selectedTier = itier;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (me, true);
		Editor_broadcastDataChanged (me);
	} catch (MelderError) {
		Melder_throw (U"Interval not inserted.");
	}
}

static void menu_cb_InsertIntervalOnTier1 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 1); }
static void menu_cb_InsertIntervalOnTier2 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 2); }
static void menu_cb_InsertIntervalOnTier3 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 3); }
static void menu_cb_InsertIntervalOnTier4 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 4); }
static void menu_cb_InsertIntervalOnTier5 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 5); }
static void menu_cb_InsertIntervalOnTier6 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 6); }
static void menu_cb_InsertIntervalOnTier7 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 7); }
static void menu_cb_InsertIntervalOnTier8 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertIntervalOnTier (me, 8); }

static void menu_cb_AlignInterval (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	checkTierSelection (my textGridArea().get(), U"align words");
	const AnyTier tier = static_cast <AnyTier> (my textGrid() -> tiers->at [my textGridArea() -> selectedTier]);
	if (tier -> classInfo != classIntervalTier)
		Melder_throw (U"Alignment works only for interval tiers, whereas tier ", my textGridArea() -> selectedTier, U" is a point tier.\nSelect an interval tier instead.");
	const integer intervalNumber = getSelectedInterval (my textGridArea().get());
	if (! intervalNumber)
		Melder_throw (U"Select an interval first");
	if (! my textGridArea() -> instancePref_align_includeWords() && ! my textGridArea() -> instancePref_align_includePhonemes())
		Melder_throw (U"Nothing to be done.\nPlease switch on \"Include words\" and/or \"Include phonemes\" in the \"Alignment settings\".");
	{// scope
		const autoMelderProgressOff noprogress;
		Editor_save (me, U"Align interval");
		TextGrid_anySound_alignInterval (my textGrid(), my soundOrLongSound(), my textGridArea() -> selectedTier, intervalNumber,
				my textGridArea() -> instancePref_align_language(), my textGridArea() -> instancePref_align_includeWords(), my textGridArea() -> instancePref_align_includePhonemes());
	}
	//FunctionEditor_redraw (me); TRY OUT 2022-06-12
	Editor_broadcastDataChanged (me);
}

static void menu_cb_AlignmentSettings (AnyTextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Alignment settings", nullptr)
		OPTIONMENU (language, U"Language", (int) Strings_findString (espeakdata_languages_names.get(), U"English (Great Britain)"))
		for (integer i = 1; i <= espeakdata_languages_names -> numberOfStrings; i ++) {
			OPTION ((conststring32) espeakdata_languages_names -> strings [i].get());
		}
		BOOLEAN (includeWords,    U"Include words",    my textGridArea() -> default_align_includeWords ())
		BOOLEAN (includePhonemes, U"Include phonemes", my textGridArea() -> default_align_includePhonemes ())
		BOOLEAN (allowSilences,   U"Allow silences",   my textGridArea() -> default_align_allowSilences ())
	EDITOR_OK
		int prefVoice = (int) Strings_findString (espeakdata_languages_names.get(), my textGridArea() -> instancePref_align_language());
		if (prefVoice == 0)
			prefVoice = (int) Strings_findString (espeakdata_languages_names.get(), U"English (Great Britain)");
		SET_OPTION (language, prefVoice)
		SET_BOOLEAN (includeWords, my textGridArea() -> instancePref_align_includeWords())
		SET_BOOLEAN (includePhonemes, my textGridArea() -> instancePref_align_includePhonemes())
		SET_BOOLEAN (allowSilences, my textGridArea() -> instancePref_align_allowSilences())
	EDITOR_DO
		my textGridArea() -> setInstancePref_align_language (espeakdata_languages_names -> strings [language].get());
		my textGridArea() -> setInstancePref_align_includeWords (includeWords);
		my textGridArea() -> setInstancePref_align_includePhonemes (includePhonemes);
		my textGridArea() -> setInstancePref_align_allowSilences (allowSilences);
	EDITOR_END
}

/***** BOUNDARY/POINT MENU *****/

static void menu_cb_RemovePointOrBoundary (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	checkTierSelection (my textGridArea().get(), U"remove a point or boundary");
	const Function anyTier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		const integer selectedLeftBoundary = getSelectedLeftBoundary (my textGridArea().get());
		if (selectedLeftBoundary == 0)
			Melder_throw (U"To remove a boundary, first click on it.");

		Editor_save (me, U"Remove boundary");
		IntervalTier_removeLeftBoundary (tier, selectedLeftBoundary);
	} else {
		const TextTier tier = (TextTier) anyTier;
		const integer selectedPoint = getSelectedPoint (my textGridArea().get());
		if (selectedPoint == 0)
			Melder_throw (U"To remove a point, first click on it.");

		Editor_save (me, U"Remove point");
		tier -> points. removeItem (selectedPoint);
	}
	Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_updateText()
	//FunctionEditor_updateText (me); TRY OUT 2022-07-23
	Editor_broadcastDataChanged (me);
}

static void do_movePointOrBoundary (AnyTextGridEditor me, int where) {
	if (where == 0 && ! my sound())
		return;
	checkTierSelection (my textGridArea().get(), U"move a point or boundary");
	const Function anyTier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		static const conststring32 boundarySaveText [3] { U"Move boundary to zero crossing", U"Move boundary to B", U"Move boundary to E" };
		const integer selectedLeftBoundary = getSelectedLeftBoundary (my textGridArea().get());
		if (selectedLeftBoundary == 0)
			Melder_throw (U"To move a boundary, first click on it.");
		const TextInterval left = tier -> intervals.at [selectedLeftBoundary - 1];
		const TextInterval right = tier -> intervals.at [selectedLeftBoundary];
		const double position = ( where == 1 ? my startSelection : where == 2 ? my endSelection :
				Sound_getNearestZeroCrossing (my sound(), left -> xmax, 1) );   // STEREO BUG
		if (isundef (position))
			Melder_throw (U"There is no zero crossing to move to.");
		if (position <= left -> xmin || position >= right -> xmax)
			Melder_throw (U"Cannot move a boundary past its neighbour.");

		Editor_save (me, boundarySaveText [where]);

		left -> xmax = right -> xmin = my startSelection = my endSelection = position;
	} else {
		TextTier tier = (TextTier) anyTier;
		static const conststring32 pointSaveText [3] { U"Move point to zero crossing", U"Move point to B", U"Move point to E" };
		const integer selectedPoint = getSelectedPoint (my textGridArea().get());
		if (selectedPoint == 0)
			Melder_throw (U"To move a point, first click on it.");
		const TextPoint point = tier -> points.at [selectedPoint];
		const double position = ( where == 1 ? my startSelection : where == 2 ? my endSelection :
				Sound_getNearestZeroCrossing (my sound(), point -> number, 1) );   // STEREO BUG
		if (isundef (position))
			Melder_throw (U"There is no zero crossing to move to.");

		Editor_save (me, pointSaveText [where]);

		point -> number = my startSelection = my endSelection = position;
	}
	Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_marksChanged()
	FunctionEditor_marksChanged (me, true);   // because cursor has moved
	Editor_broadcastDataChanged (me);
}

static void menu_cb_MoveToB (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_movePointOrBoundary (me, 1);
}

static void menu_cb_MoveToE (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_movePointOrBoundary (me, 2);
}

static void menu_cb_MoveToZero (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_movePointOrBoundary (me, 0);
}

static void do_insertOnTier (AnyTextGridEditor me, integer itier) {
	try {
		insertBoundaryOrPoint (me, itier,
			my duringPlay ? my playCursor : my startSelection,
			my duringPlay ? my playCursor : my endSelection,
			false
		);
		my textGridArea() -> selectedTier = itier;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (me, true);
		Editor_broadcastDataChanged (me);
	} catch (MelderError) {
		Melder_throw (U"Boundary or point not inserted.");
	}
}

static void menu_cb_InsertOnSelectedTier (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_insertOnTier (me, my textGridArea() -> selectedTier);
}

static void menu_cb_InsertOnTier1 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 1); }
static void menu_cb_InsertOnTier2 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 2); }
static void menu_cb_InsertOnTier3 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 3); }
static void menu_cb_InsertOnTier4 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 4); }
static void menu_cb_InsertOnTier5 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 5); }
static void menu_cb_InsertOnTier6 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 6); }
static void menu_cb_InsertOnTier7 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 7); }
static void menu_cb_InsertOnTier8 (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) { do_insertOnTier (me, 8); }

static void menu_cb_InsertOnAllTiers (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	const integer saveTier = my textGridArea() -> selectedTier;
	for (integer itier = 1; itier <= my textGrid() -> tiers->size; itier ++)
		do_insertOnTier (me, itier);
	my textGridArea() -> selectedTier = saveTier;   // only if everything went right; otherwise, the tier where something went wrong will stand selected
}

/***** SEARCH MENU *****/

static void findInTier (AnyTextGridEditor me) {
	checkTierSelection (my textGridArea().get(), U"find a text");
	Function anyTier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		integer iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals.size) {
			TextInterval interval = tier -> intervals.at [iinterval];
			conststring32 text = interval -> text.get();
			if (text) {
				const char32 *position = str32str (text, my findString.get());
				if (position) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToView()
					FunctionEditor_scrollToView (me, my startSelection);
					GuiText_setSelection (my textArea, position - text, position - text + str32len (my findString.get()));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals.size)
			Melder_beep ();
	} else {
		TextTier tier = (TextTier) anyTier;
		integer ipoint = AnyTier_timeToLowIndex (tier->asAnyTier(), my startSelection) + 1;
		while (ipoint <= tier -> points.size) {
			const TextPoint point = tier->points.at [ipoint];
			conststring32 text = point -> mark.get();
			if (text) {
				const char32 * const position = str32str (text, my findString.get());
				if (position) {
					my startSelection = my endSelection = point -> number;
					Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToView()
					FunctionEditor_scrollToView (me, point -> number);
					GuiText_setSelection (my textArea, position - text, position - text + str32len (my findString.get()));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points.size)
			Melder_beep ();
	}
}

static void do_find (AnyTextGridEditor me) {
	if (my findString) {
		integer left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my textArea, & left, & right);
		const char32 * const position = str32str (& label [right], my findString.get());   // CRLF BUG?
		if (position) {
			GuiText_setSelection (my textArea, position - label.get(), position - label.get() + str32len (my findString.get()));
		} else {
			findInTier (me);
		}
	}
}

static void menu_cb_Find (AnyTextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Find text", nullptr)
		TEXTFIELD (findString, U"Text", U"", 3)
	EDITOR_OK
	EDITOR_DO
		my findString = Melder_dup (findString);
		do_find (me);
	EDITOR_END
}

static void menu_cb_FindAgain (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	do_find (me);
}

static void checkSpellingInTier (AnyTextGridEditor me) {
	checkTierSelection (my textGridArea().get(), U"check spelling");
	const Function anyTier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
	if (anyTier -> classInfo == classIntervalTier) {
		const IntervalTier tier = (IntervalTier) anyTier;
		integer iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals.size) {
			TextInterval interval = tier -> intervals.at [iinterval];
			conststring32 text = interval -> text.get();
			if (text) {
				integer position = 0;
				conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToView()
					FunctionEditor_scrollToView (me, my startSelection);
					GuiText_setSelection (my textArea, position, position + str32len (notAllowed));
					return;
				}
			}
			iinterval ++;
		}
		if (iinterval > tier -> intervals.size)
			Melder_beep ();
	} else {
		const TextTier tier = (TextTier) anyTier;
		integer ipoint = AnyTier_timeToLowIndex (tier->asAnyTier(), my startSelection) + 1;
		while (ipoint <= tier -> points.size) {
			TextPoint point = tier -> points.at [ipoint];
			conststring32 text = point -> mark.get();
			if (text) {
				integer position = 0;
				conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = my endSelection = point -> number;
					Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_scrollToView()
					FunctionEditor_scrollToView (me, point -> number);
					GuiText_setSelection (my textArea, position, position + str32len (notAllowed));
					return;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points.size)
			Melder_beep ();
	}
}

static void menu_cb_CheckSpelling (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my spellingChecker) {
		integer left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my textArea, & left, & right);
		integer position = right;
		conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.get(), & position);
		if (notAllowed) {
			GuiText_setSelection (my textArea, position, position + str32len (notAllowed));
		} else {
			checkSpellingInTier (me);
		}
	}
}

static void menu_cb_CheckSpellingInInterval (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my spellingChecker) {
		integer left, right;
		autostring32 label = GuiText_getStringAndSelectionPosition (my textArea, & left, & right);
		integer position = right;
		conststring32 notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label.get(), & position);
		if (notAllowed)
			GuiText_setSelection (my textArea, position, position + str32len (notAllowed));
	}
}

static void menu_cb_AddToUserDictionary (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my spellingChecker) {
		const autostring32 word = GuiText_getSelection (my textArea);
		SpellingChecker_addNewWord (my spellingChecker, word.get());
		Editor_broadcastDataChanged (me);
	}
}

/***** TIER MENU *****/

static void menu_cb_RenameTier (AnyTextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Rename tier", nullptr)
		SENTENCE (newName, U"New name", U"");
	EDITOR_OK
		checkTierSelection (my textGridArea().get(), U"rename a tier");
		const Daata tier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
		SET_STRING (newName, tier -> name ? tier -> name.get() : U"")
	EDITOR_DO
		checkTierSelection (my textGridArea().get(), U"rename a tier");
		const Function tier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];

		Editor_save (me, U"Rename tier");

		Thing_setName (tier, newName);

		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void CONVERT_DATA_TO_ONE__PublishTier (AnyTextGridEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		checkTierSelection (my textGridArea().get(), U"publish a tier");
		const Function tier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
		autoTextGrid result = TextGrid_createWithoutTiers (1e30, -1e30);
		TextGrid_addTier_copy (result.get(), tier);
	CONVERT_DATA_TO_ONE_END (tier -> name.get())
}

static void menu_cb_RemoveAllTextFromTier (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	checkTierSelection (my textGridArea().get(), U"remove all text from a tier");
	IntervalTier intervalTier;
	TextTier textTier;
	AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my textGridArea() -> selectedTier], & intervalTier, & textTier);

	Editor_save (me, U"Remove text from tier");
	if (intervalTier)
		IntervalTier_removeText (intervalTier);
	else
		TextTier_removeText (textTier);

	Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_updateText()
	//FunctionEditor_updateText (me); TRY OUT 2022-07-23
	Editor_broadcastDataChanged (me);
}

static void menu_cb_RemoveTier (AnyTextGridEditor me, EDITOR_ARGS_DIRECT) {
	if (my textGrid() -> tiers->size <= 1)
		Melder_throw (U"Sorry, I refuse to remove the last tier.");
	checkTierSelection (my textGridArea().get(), U"remove a tier");

	Editor_save (me, U"Remove tier");
	my textGrid() -> tiers-> removeItem (my textGridArea() -> selectedTier);

	//my textGridArea -> selectedTier = 1; TRY OUT 2022-07-23
	Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_updateText()
	//FunctionEditor_updateText (me); TRY OUT 2022-07-23
	Editor_broadcastDataChanged (me);
}

static void menu_cb_AddIntervalTier (AnyTextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add interval tier", nullptr)
		NATURAL (position, U"Position", U"1 (= at top)")
		SENTENCE (name, U"Name", U"")
	EDITOR_OK
		SET_INTEGER_AS_STRING (position, Melder_cat (my textGrid() -> tiers->size + 1, U" (= at bottom)"))
		SET_STRING (name, U"")
	EDITOR_DO
		{// scope
			autoIntervalTier tier = IntervalTier_create (my textGrid() -> xmin, my textGrid() -> xmax);
			Melder_clipRight (& position, my textGrid() -> tiers->size + 1);
			Thing_setName (tier.get(), name);

			Editor_save (me, U"Add interval tier");
			my textGrid() -> tiers -> addItemAtPosition_move (tier.move(), position);
		}

		my textGridArea() -> selectedTier = position;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_updateText()
		//FunctionEditor_updateText (me); TRY OUT 2022-07-23
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_AddPointTier (AnyTextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point tier", nullptr)
		NATURAL (position, U"Position", U"1 (= at top)")
		SENTENCE (name, U"Name", U"");
	EDITOR_OK
		SET_INTEGER_AS_STRING (position, Melder_cat (my textGrid() -> tiers->size + 1, U" (= at bottom)"))
		SET_STRING (name, U"")
	EDITOR_DO
		{// scope
			autoTextTier tier = TextTier_create (my textGrid() -> xmin, my textGrid() -> xmax);
			Melder_clipRight (& position, my textGrid() -> tiers->size + 1);
			Thing_setName (tier.get(), name);

			Editor_save (me, U"Add point tier");
			my textGrid() -> tiers -> addItemAtPosition_move (tier.move(), position);
		}

		my textGridArea() -> selectedTier = position;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_updateText()
		//FunctionEditor_updateText (me); TRY OUT 2022-07-23
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_DuplicateTier (AnyTextGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Duplicate tier", nullptr)
		NATURAL (position, U"Position", U"1 (= at top)")
		SENTENCE (name, U"Name", U"")
	EDITOR_OK
		if (my textGridArea() -> selectedTier != 0) {
			SET_INTEGER (position, my textGridArea() -> selectedTier + 1)
			SET_STRING (name, my textGrid() -> tiers->at [my textGridArea() -> selectedTier] -> name.get())
		}
	EDITOR_DO
		checkTierSelection (my textGridArea().get(), U"duplicate a tier");
		const Function tier = my textGrid() -> tiers->at [my textGridArea() -> selectedTier];
		{// scope
			autoFunction newTier = Data_copy (tier);
			Melder_clipRight (& position, my textGrid() -> tiers->size + 1);
			Thing_setName (newTier.get(), name);

			Editor_save (me, U"Duplicate tier");
			my textGrid() -> tiers -> addItemAtPosition_move (newTier.move(), position);
		}

		my textGridArea() -> selectedTier = position;
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_updateText()
		//FunctionEditor_updateText (me); TRY OUT 2022-07-23
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

/***** HELP MENU *****/

static void menu_cb_TextGridEditorHelp (AnyTextGridEditor, EDITOR_ARGS_DIRECT) { HELP (U"TextGridEditor") }
static void menu_cb_AboutSpecialSymbols (AnyTextGridEditor, EDITOR_ARGS_DIRECT) { HELP (U"Special symbols") }
static void menu_cb_PhoneticSymbols (AnyTextGridEditor, EDITOR_ARGS_DIRECT) { HELP (U"Phonetic symbols") }
static void menu_cb_AboutTextStyles (AnyTextGridEditor, EDITOR_ARGS_DIRECT) { HELP (U"Text styles") }

void structAnyTextGridEditor :: v_createMenus () {
	AnyTextGridEditor_Parent :: v_createMenus ();
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

	Editor_addCommand (this, U"Query", U"-- query interval --", 0, nullptr);
	Editor_addCommand (this, U"Query", U"Get starting point of interval", 0,
			QUERY_DATA_FOR_REAL__GetStartingPointOfInterval);
	Editor_addCommand (this, U"Query", U"Get end point of interval", 0,
			QUERY_DATA_FOR_REAL__GetEndPointOfInterval);
	Editor_addCommand (this, U"Query", U"Get label of interval", 0,
			QUERY_DATA_FOR_STRING__GetLabelOfInterval);

	Editor_addCommand (this, U"Draw", U"-- multiple draw --", 0, nullptr);
	Editor_addCommand (this, U"Draw", U"Draw visible pitch contour and TextGrid...", 0, menu_cb_DrawTextGridAndPitch);

	menu = Editor_addMenu (this, U"Interval", 0);
	if (our soundArea) {
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
	if (our soundArea) {   // BUG: not for LongSounds
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
	EditorMenu_addCommand (menu, U"Extract to list of objects:", 0, nullptr);
	EditorMenu_addCommand (menu, U"Extract entire selected tier", 0,
			CONVERT_DATA_TO_ONE__PublishTier);

	if (our spellingChecker) {
		menu = Editor_addMenu (this, U"Spell", 0);
		EditorMenu_addCommand (menu, U"Check spelling in tier", GuiMenu_COMMAND | GuiMenu_OPTION | 'L', menu_cb_CheckSpelling);
		EditorMenu_addCommand (menu, U"Check spelling in interval", 0, menu_cb_CheckSpellingInInterval);
		EditorMenu_addCommand (menu, U"-- edit lexicon --", 0, nullptr);
		EditorMenu_addCommand (menu, U"Add selected word to user dictionary", 0, menu_cb_AddToUserDictionary);
	}

	if (our soundArea)
		our soundAnalysisArea -> v_createMenus ();   // insert some of the ancestor's menus *after* the TextGrid menus
}

void structAnyTextGridEditor :: v_createMenuItems_help (EditorMenu menu) {
	AnyTextGridEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"TextGridEditor help", '?', menu_cb_TextGridEditorHelp);
	EditorMenu_addCommand (menu, U"About special symbols", 0, menu_cb_AboutSpecialSymbols);
	EditorMenu_addCommand (menu, U"Phonetic symbols", 0, menu_cb_PhoneticSymbols);
	EditorMenu_addCommand (menu, U"About text styles", 0, menu_cb_AboutTextStyles);
}

/***** CHILDREN *****/

static void gui_text_cb_changed (AnyTextGridEditor me, GuiTextEvent /* event */) {
	//Melder_casual (U"gui_text_cb_change 1 in editor ", Melder_pointer (me));
	if (my suppressRedraw) return;   /* Prevent infinite loop if 'draw' method or Editor_broadcastChange calls GuiText_setString. */
	//Melder_casual (U"gui_text_cb_change 2 in editor ", me);
	if (my textGridArea() -> selectedTier) {
		autostring32 text = GuiText_getString (my textArea);
		IntervalTier intervalTier;
		TextTier textTier;
		AnyTextGridTier_identifyClass (my textGrid() -> tiers->at [my textGridArea() -> selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			const integer selectedInterval = getSelectedInterval (my textGridArea().get());
			if (selectedInterval) {
				TextInterval interval = intervalTier -> intervals.at [selectedInterval];
				//Melder_casual (U"gui_text_cb_change 3 in editor ", Melder_pointer (me));
				TextInterval_setText (interval, text.get());
				//Melder_casual (U"gui_text_cb_change 4 in editor ", Melder_pointer (me));
				//FunctionEditor_redraw (me); TRY OUT 2022-06-12
				//Melder_casual (U"gui_text_cb_change 5 in editor ", Melder_pointer (me));
				Editor_broadcastDataChanged (me);
				//Melder_casual (U"gui_text_cb_change 6 in editor ", Melder_pointer (me));
			}
		} else {
			const integer selectedPoint = getSelectedPoint (my textGridArea().get());
			if (selectedPoint) {
				TextPoint point = textTier -> points.at [selectedPoint];
				point -> mark. reset();
				if (Melder_findInk (text.get()))   // any visible characters?
					point -> mark = Melder_dup_f (text.get());
				//FunctionEditor_redraw (me); TRY OUT 2022-06-12
				Editor_broadcastDataChanged (me);
			}
		}
	}
}

void structAnyTextGridEditor :: v_createChildren () {
	AnyTextGridEditor_Parent :: v_createChildren ();
	if (our textArea)
		GuiText_setChangedCallback (our textArea, gui_text_cb_changed, this);
}

void structAnyTextGridEditor :: v1_dataChanged () {
	/*
		Perform a minimal selection change.
		Most changes will involve intervals and boundaries; however, there may also be tier removals.
		Do a simple guess.
	*/
	Melder_clipRight (& our textGridArea() -> selectedTier, our textGrid() -> tiers->size);   // crucial: before v_updateText (bug 2022-07-23)!
	AnyTextGridEditor_Parent :: v1_dataChanged ();   // does all the updating
}

/********** DRAWING AREA **********/

void structAnyTextGridEditor :: v_distributeAreas () {
	if (our soundArea) {
		Melder_assert (our soundOrLongSound());
		Melder_assert (our soundAnalysisArea);
		const bool showAnalysis = (
			our soundAnalysisArea -> instancePref_spectrogram_show() ||
			our soundAnalysisArea -> instancePref_pitch_show() ||
			our soundAnalysisArea -> instancePref_intensity_show() ||
			our soundAnalysisArea -> instancePref_formant_show()
		);
		const integer numberOfTiers = our textGrid() -> tiers->size;
		const integer numberOfVisibleChannels = Melder_clippedRight (our soundOrLongSound() -> ny, 8_integer);
		const double soundY = numberOfTiers / (2.0 * numberOfVisibleChannels +
				numberOfTiers * ( showAnalysis ? 1.8 : 1.3 ));
		our textGridArea() -> setGlobalYRange_fraction (0.0, soundY);
		if (showAnalysis) {
			const double soundY2 = 0.5 * (1.0 + soundY);
			our soundAnalysisArea -> setGlobalYRange_fraction (soundY, soundY2);
			our soundArea -> setGlobalYRange_fraction (soundY2, 1.0);
		} else {
			our soundAnalysisArea -> setGlobalYRange_fraction (soundY, soundY);
			our soundArea -> setGlobalYRange_fraction (soundY, 1.0);
		}
	} else {
		our textGridArea() -> setGlobalYRange_fraction (0.0, 1.0);
	}
}

static void do_drawIntervalTier (AnyTextGridEditor me, IntervalTier tier, integer itier) {
	#if gtk || defined (macintosh)
		constexpr bool platformUsesAntiAliasing = true;
	#else
		constexpr bool platformUsesAntiAliasing = false;
	#endif
	integer x1DC, x2DC, yDC;
	Graphics_WCtoDC (my graphics.get(), my startWindow, 0.0, & x1DC, & yDC);
	Graphics_WCtoDC (my graphics.get(), my endWindow, 0.0, & x2DC, & yDC);
	Graphics_setPercentSignIsItalic (my graphics.get(), my textGridArea() -> instancePref_useTextStyles());
	Graphics_setNumberSignIsBold (my graphics.get(), my textGridArea() -> instancePref_useTextStyles());
	Graphics_setCircumflexIsSuperscript (my graphics.get(), my textGridArea() -> instancePref_useTextStyles());
	Graphics_setUnderscoreIsSubscript (my graphics.get(), my textGridArea() -> instancePref_useTextStyles());

	const integer selectedInterval = ( itier == my textGridArea() -> selectedTier ? getSelectedInterval (my textGridArea().get()) : 0 ), ninterval = tier -> intervals.size;

	/*
		Draw a grey bar and a selection button at the cursor position.
	*/
	if (my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow) {
		/* mutable search */ bool cursorAtBoundary = false;
		for (integer iinterval = 2; iinterval <= ninterval; iinterval ++) {
			const TextInterval interval = tier -> intervals.at [iinterval];
			if (interval -> xmin == my startSelection)
				cursorAtBoundary = true;
		}
		if (! cursorAtBoundary) {
			const double dy = Graphics_dyMMtoWC (my graphics.get(), 1.5);
			Graphics_setGrey (my graphics.get(), 0.8);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), my startSelection, 0.0, my startSelection, 1.0);
			Graphics_setLineWidth (my graphics.get(), 1.0);
			Graphics_setColour (my graphics.get(), Melder_BLUE);
			Graphics_circle_mm (my graphics.get(), my startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics.get(), my textGridArea() -> instancePref_alignment(), Graphics_HALF);
	for (integer iinterval = 1; iinterval <= ninterval; iinterval ++) {
		const TextInterval interval = tier -> intervals.at [iinterval];
		/* mutable clip */ double startInterval = interval -> xmin, endInterval = interval -> xmax;
		Melder_clipLeft (my tmin, & startInterval);
		Melder_clipRight (& endInterval, my tmax);
		if (startInterval >= endInterval)
			continue;
		const bool intervalIsSelected = ( selectedInterval == iinterval );

		/*
			Draw left boundary.
		*/
		if (startInterval >= my startWindow && startInterval <= my endWindow && iinterval > 1) {
			const bool boundaryIsSelected = ( my textGridArea() -> selectedTier == itier && startInterval == my startSelection );
			Graphics_setColour (my graphics.get(), boundaryIsSelected ? Melder_RED : Melder_BLUE);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), startInterval, 0.0, startInterval, 1.0);

			/*
				Show alignment with cursor.
			*/
			if (startInterval == my startSelection) {
				Graphics_setColour (my graphics.get(), Melder_YELLOW);
				Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics.get(), startInterval, 0.0, startInterval, 1.0);
			}
		}
		Graphics_setLineWidth (my graphics.get(), 1.0);

		/*
			Draw label text.
		*/
		if (interval -> text && endInterval >= my startWindow && startInterval <= my endWindow) {
			const double t1 = std::max (my startWindow, startInterval);
			const double t2 = std::min (my endWindow, endInterval);
			Graphics_setColour (my graphics.get(), intervalIsSelected ? Melder_RED : Melder_BLACK);
			Graphics_textRect (my graphics.get(), t1, t2, 0.0, 1.0, interval -> text.get());
			Graphics_setColour (my graphics.get(), Melder_BLACK);
		}

	}
	Graphics_setPercentSignIsItalic (my graphics.get(), true);
	Graphics_setNumberSignIsBold (my graphics.get(), true);
	Graphics_setCircumflexIsSuperscript (my graphics.get(), true);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), true);
}

static void do_drawTextTier (AnyTextGridEditor me, TextTier tier, integer itier) {
	#if gtk || defined (macintosh)
		constexpr bool platformUsesAntiAliasing = true;
	#else
		constexpr bool platformUsesAntiAliasing = false;
	#endif
	const integer npoint = tier -> points.size;
	Graphics_setPercentSignIsItalic (my graphics.get(), my textGridArea() -> instancePref_useTextStyles());
	Graphics_setNumberSignIsBold (my graphics.get(), my textGridArea() -> instancePref_useTextStyles());
	Graphics_setCircumflexIsSuperscript (my graphics.get(), my textGridArea() -> instancePref_useTextStyles());
	Graphics_setUnderscoreIsSubscript (my graphics.get(), my textGridArea() -> instancePref_useTextStyles());

	/*
		Draw a grey bar and a selection button at the cursor position.
	*/
	if (my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow) {
		bool cursorAtPoint = false;
		for (integer ipoint = 1; ipoint <= npoint; ipoint ++) {
			const TextPoint point = tier -> points.at [ipoint];
			if (point -> number == my startSelection)
				cursorAtPoint = true;
		}
		if (! cursorAtPoint) {
			const double dy = Graphics_dyMMtoWC (my graphics.get(), 1.5);
			Graphics_setGrey (my graphics.get(), 0.8);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), my startSelection, 0.0, my startSelection, 1.0);
			Graphics_setLineWidth (my graphics.get(), 1.0);
			Graphics_setColour (my graphics.get(), Melder_BLUE);
			Graphics_circle_mm (my graphics.get(), my startSelection, 1.0 - dy, 3.0);
		}
	}

	Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
	for (integer ipoint = 1; ipoint <= npoint; ipoint ++) {
		const TextPoint point = tier -> points.at [ipoint];
		const double t = point -> number;
		if (t >= my startWindow && t <= my endWindow) {
			const bool pointIsSelected = ( itier == my textGridArea() -> selectedTier && t == my startSelection );
			Graphics_setColour (my graphics.get(), pointIsSelected ? Melder_RED : Melder_BLUE);
			Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 6.0 : 5.0);
			Graphics_line (my graphics.get(), t, 0.0, t, 0.2);
			Graphics_line (my graphics.get(), t, 0.8, t, 1);
			Graphics_setLineWidth (my graphics.get(), 1.0);

			/*
				Wipe out the cursor where the text is going to be.
			*/
			Graphics_setColour (my graphics.get(), Melder_WHITE);
			Graphics_line (my graphics.get(), t, 0.2, t, 0.8);

			/*
				Show alignment with cursor.
			*/
			if (my startSelection == my endSelection && t == my startSelection) {
				Graphics_setColour (my graphics.get(), Melder_YELLOW);
				Graphics_setLineWidth (my graphics.get(), platformUsesAntiAliasing ? 2.0 : 1.0);
				Graphics_line (my graphics.get(), t, 0.0, t, 0.2);
				Graphics_line (my graphics.get(), t, 0.8, t, 1.0);
				Graphics_setLineWidth (my graphics.get(), 1.0);
			}
			Graphics_setColour (my graphics.get(), pointIsSelected ? Melder_RED : Melder_BLUE);
			if (point -> mark)
				Graphics_text (my graphics.get(), t, 0.5, point -> mark.get());
		}
	}
	Graphics_setPercentSignIsItalic (my graphics.get(), true);
	Graphics_setNumberSignIsBold (my graphics.get(), true);
	Graphics_setCircumflexIsSuperscript (my graphics.get(), true);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), true);
}

void structAnyTextGridEditor :: v_draw () {
	Graphics_Viewport vp1, vp2;
	const integer numberOfTiers = our textGrid() -> tiers->size;
	const enum kGraphics_font oldFont = Graphics_inqFont (our graphics.get());
	const double oldFontSize = Graphics_inqFontSize (our graphics.get());
	const bool showAnalysis = our soundAnalysisArea && (
		our soundAnalysisArea -> instancePref_spectrogram_show() ||
		our soundAnalysisArea -> instancePref_pitch_show() ||
		our soundAnalysisArea -> instancePref_intensity_show() ||
		our soundAnalysisArea -> instancePref_formant_show())
		&& our soundOrLongSound()  // BUG: collapse
	;

	/*
		Draw optional sound.
	*/
	if (our soundOrLongSound() || our soundAnalysisArea && our soundAnalysisArea -> instancePref_pulses_show()) {
		FunctionArea_prepareCanvas (our soundArea.get());
		if (our soundAnalysisArea && our soundAnalysisArea -> instancePref_pulses_show())
			our soundAnalysisArea -> v_draw_analysis_pulses ();
		FunctionArea_drawInside (our soundArea.get());
		if (showAnalysis) {
			FunctionArea_prepareCanvas (our soundAnalysisArea.get());
			our soundAnalysisArea -> v_draw_analysis ();
		}
	}

	/*
		Draw tiers.
	*/
	FunctionArea_prepareCanvas (our textGridArea().get());
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
	for (integer itier = 1; itier <= numberOfTiers; itier ++) {
		const Function anyTier = our textGrid() -> tiers->at [itier];
		const bool tierIsSelected = ( itier == textGridArea() -> selectedTier );
		const bool isIntervalTier = ( anyTier -> classInfo == classIntervalTier );
		vp2 = Graphics_insetViewport (our graphics.get(), 0.0, 1.0,
				1.0 - (double) itier / (double) numberOfTiers,
				1.0 - (double) (itier - 1) / (double) numberOfTiers);
		Graphics_setColour (our graphics.get(), Melder_BLACK);
		if (itier != 1)
			Graphics_line (our graphics.get(), our startWindow, 1.0, our endWindow, 1.0);

		/*
			Show the number and the name of the tier.
		*/
		Graphics_setColour (our graphics.get(), tierIsSelected ? Melder_RED : Melder_BLACK);
		Graphics_setFont (our graphics.get(), oldFont);
		Graphics_setFontSize (our graphics.get(), 14);
		Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (our graphics.get(), our startWindow, 0.5,   tierIsSelected ? U"☞ " : U"", itier);
		Graphics_setFontSize (our graphics.get(), oldFontSize);
		if (anyTier -> name && anyTier -> name [0]) {
			Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT,
					our textGridArea() -> instancePref_showNumberOf() == kTextGridArea_showNumberOf::NOTHING ? Graphics_HALF : Graphics_BOTTOM);
			Graphics_text (our graphics.get(), our endWindow, 0.5, anyTier -> name.get());
		}
		if (our textGridArea() -> instancePref_showNumberOf() != kTextGridArea_showNumberOf::NOTHING) {
			Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_TOP);
			if (our textGridArea() -> instancePref_showNumberOf() == kTextGridArea_showNumberOf::INTERVALS_OR_POINTS) {
				const integer count = ( isIntervalTier ? ((IntervalTier) anyTier) -> intervals.size : ((TextTier) anyTier) -> points.size );
				const integer position = ( itier == our textGridArea() -> selectedTier ? ( isIntervalTier ? getSelectedInterval (our textGridArea().get()) : getSelectedPoint (our textGridArea().get()) ) : 0 );
				if (position)
					Graphics_text (our graphics.get(), our endWindow, 0.5,   U"(", position, U"/", count, U")");
				else
					Graphics_text (our graphics.get(), our endWindow, 0.5,   U"(", count, U")");
			} else {
				Melder_assert (our textGridArea() -> instancePref_showNumberOf() == kTextGridArea_showNumberOf::NONEMPTY_INTERVALS_OR_POINTS);
				integer count = 0;
				if (isIntervalTier) {
					const IntervalTier tier = (IntervalTier) anyTier;
					const integer numberOfIntervals = tier -> intervals.size;
					for (integer iinterval = 1; iinterval <= numberOfIntervals; iinterval ++) {
						const TextInterval interval = tier -> intervals.at [iinterval];
						if (interval -> text && interval -> text [0] != U'\0')
							count ++;
					}
				} else {
					const TextTier tier = (TextTier) anyTier;
					const integer numberOfPoints = tier -> points.size;
					for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
						const TextPoint point = tier -> points.at [ipoint];
						if (point -> mark && point -> mark [0] != U'\0')
							count ++;
					}
				}
				Graphics_text (our graphics.get(), our endWindow, 0.5,   U"(##", count, U"#)");
			}
		}

		Graphics_setColour (our graphics.get(), Melder_BLACK);
		Graphics_setFont (our graphics.get(), kGraphics_font::TIMES);
		Graphics_setFontSize (our graphics.get(), our textGridArea() -> instancePref_fontSize());
		if (isIntervalTier)
			do_drawIntervalTier (this, (IntervalTier) anyTier, itier);
		else
			do_drawTextTier (this, (TextTier) anyTier, itier);
		Graphics_resetViewport (our graphics.get(), vp2);
	}
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_setFont (our graphics.get(), oldFont);
	Graphics_setFontSize (our graphics.get(), oldFontSize);

	if (isdefined (our draggingTime) && hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
		FunctionArea_setViewport (our textGridArea().get());
		Graphics_xorOn (our graphics.get(), Melder_MAROON);
		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			if (our draggingTiers [itier]) {
				const double ymin = 1.0 - (double) itier / numberOfTiers;
				const double ymax = 1.0 - (double) (itier - 1) / numberOfTiers;
				Graphics_setLineWidth (our graphics.get(), 7.0);
				Graphics_line (our graphics.get(), our draggingTime, ymin, our draggingTime, ymax);
			}
		}
		our viewDataAsWorldByFraction ();
		Graphics_setLineWidth (our graphics.get(), 1.0);
		Graphics_line (our graphics.get(), our draggingTime, 0.0, our draggingTime, 1.0);
		Graphics_text (our graphics.get(), our draggingTime, 1.0, Melder_fixed (our draggingTime, 6));
		Graphics_xorOff (our graphics.get());
	}
}

static const conststring32 characters [12] [10] = {
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

	{ U"ʨ", U"ʥ", U"z̊", U"z̥", U"z̤", U"z̰", U"z̪", U"z̩", U"z̝", U"z̞" },
	{ U"ý", U"ȳ", U"ỳ", U"ÿ", U"ỹ", U"o̟", U"o̱", U"o̹", U"o̜", U"t̚" },
};

void structAnyTextGridEditor :: v_drawSelectionViewer () {
	Graphics_setWindow (our graphics.get(), 0.5, 10.5, 0.5, 12.5);
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.5, 10.5, 0.5, 12.5);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_setFont (our graphics.get(), kGraphics_font::TIMES);
	const double pointsPerMillimetre = 72.0 / 25.4;
	const double cellWidth_points = Graphics_dxWCtoMM (our graphics.get(), 1.0) * pointsPerMillimetre;
	const double cellHeight_points = Graphics_dyWCtoMM (our graphics.get(), 1.0) * pointsPerMillimetre;
	const double fontSize = std::min (0.8 * cellHeight_points, 1.2 * cellWidth_points);
	Graphics_setFontSize (our graphics.get(), fontSize);
	Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
	for (integer irow = 1; irow <= 12; irow ++)
		for (integer icol = 1; icol <= 10; icol ++)
			Graphics_text (our graphics.get(), 0.0 + 1.0 * icol, 13.0 - 1.0 * irow, characters [irow-1] [icol-1]);
}

bool structAnyTextGridEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event,
	double x_world, double y_fraction_global)
{
	const integer numberOfTiers = our textGrid() -> tiers->size;
	const bool mouseIsInWideSoundPart =
			our soundArea && our soundArea -> y_fraction_globalIsInside (y_fraction_global);
	const bool mouseIsInWideSoundAnalysisPart =
			our soundAnalysisArea && our soundAnalysisArea -> y_fraction_globalIsInside (y_fraction_global);
	const bool mouseIsInWideSoundOrAnalysisPart = mouseIsInWideSoundPart || mouseIsInWideSoundAnalysisPart;
	const bool mouseIsInWideTextGridPart = our textGridArea() -> y_fraction_globalIsInside (y_fraction_global);
	const integer oldSelectedTier = our textGridArea() -> selectedTier;

	constexpr double clickingVicinityRadius_mm = 1.0;
	constexpr double draggingVicinityRadius_mm = clickingVicinityRadius_mm + 0.2;   // must be greater than `clickingVicinityRadius_mm`
	constexpr double droppingVicinityRadius_mm = 1.5;

	if (event -> isClick()) {
		our anchorIsInWideSoundOrAnalysisPart = mouseIsInWideSoundOrAnalysisPart;
		our anchorIsInWideTextGridPart = mouseIsInWideTextGridPart;
	}
	if (mouseIsInWideSoundAnalysisPart) {
		if (our soundAnalysisArea -> instancePref_spectrogram_show() || our soundAnalysisArea -> instancePref_formant_show()) {
			const double y_fraction_withinAnalysesArea = our soundAnalysisArea -> y_fraction_globalToLocal (y_fraction_global);
			our soundAnalysisArea -> d_spectrogram_cursor = y_fraction_withinAnalysesArea * our soundAnalysisArea -> instancePref_spectrogram_viewTo()
					+ (1.0 - y_fraction_withinAnalysesArea) * our soundAnalysisArea -> instancePref_spectrogram_viewFrom();
		}
	}
	if (! our anchorIsInWideTextGridPart)
		return our AnyTextGridEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction_global);
	const double y_fraction_withinTextGridArea = our textGridArea() -> y_fraction_globalToLocal (y_fraction_global);
	const integer mouseTier = _TextGridEditor_y_fraction_local_toTier (this, y_fraction_withinTextGridArea);

	our draggingTime = undefined;   // information to next expose event
	if (event -> isClick()) {
		if (isdefined (our anchorTime))   // sanity check for the fixed order click-drag-drop
			return false;
		Melder_assert (our clickedLeftBoundary == 0);
		Melder_assert (! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce);
		our draggingTiers.reset();
		/*
			The user clicked in the grid part.
			We select the tier in which they clicked.
		*/
		our textGridArea() -> selectedTier = mouseTier;
		double startInterval, endInterval;
		_TextGridEditor_timeToInterval (this, x_world, our textGridArea() -> selectedTier, & startInterval, & endInterval);

		if (event -> isLeftBottomFunctionKeyPressed()) {
			our startSelection = ( x_world - startInterval < endInterval - x_world ? startInterval : endInterval );   // to nearest boundary
			Melder_sort (& our startSelection, & our endSelection);
			return FunctionEditor_UPDATE_NEEDED;
		}
		if (event -> isRightBottomFunctionKeyPressed()) {
			our endSelection = ( x_world - startInterval < endInterval - x_world ? startInterval : endInterval );
			Melder_sort (& our startSelection, & our endSelection);
			return FunctionEditor_UPDATE_NEEDED;
		}

		IntervalTier selectedIntervalTier;
		TextTier selectedTextTier;
		AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [our textGridArea() -> selectedTier], & selectedIntervalTier, & selectedTextTier);

		if (x_world <= our startWindow || x_world >= our endWindow)
			return FunctionEditor_UPDATE_NEEDED;

		/*
			Get the time of the nearest boundary or point.
		*/
		if (selectedIntervalTier) {
			const integer clickedIntervalNumber = IntervalTier_timeToIndex (selectedIntervalTier, x_world);
			const bool theyClickedOutsidetheTimeDomainOfTheIntervals = ( clickedIntervalNumber == 0 );
			if (theyClickedOutsidetheTimeDomainOfTheIntervals)
				return FunctionEditor_UPDATE_NEEDED;
			const TextInterval interval = selectedIntervalTier -> intervals.at [clickedIntervalNumber];
			if (x_world > 0.5 * (interval -> xmin + interval -> xmax)) {
				our anchorTime = interval -> xmax;
				our clickedLeftBoundary = clickedIntervalNumber + 1;
			} else {
				our anchorTime = interval -> xmin;
				our clickedLeftBoundary = clickedIntervalNumber;
			}
		} else {
			const integer clickedPointNumber = AnyTier_timeToNearestIndex (selectedTextTier->asAnyTier(), x_world);
			if (clickedPointNumber != 0) {
				const TextPoint point = selectedTextTier -> points.at [clickedPointNumber];
				our anchorTime = point -> number;
			}
		}
		Melder_assert (! (selectedIntervalTier && our clickedLeftBoundary == 0));

		const bool nearBoundaryOrPoint = ( isdefined (our anchorTime) &&
				fabs (Graphics_dxWCtoMM (our graphics.get(), x_world - our anchorTime)) < 1.5 );
		FunctionArea_setViewport (our textGridArea().get());
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
		const double distanceToCursorCircle = ( our startSelection != our endSelection ? undefined :
			Graphics_distanceWCtoMM (our graphics.get(), x_world, y_fraction_withinTextGridArea,
				our startSelection,
				(numberOfTiers + 1 - our textGridArea() -> selectedTier) / double (numberOfTiers) -
				Graphics_dyMMtoWC (our graphics.get(), 1.5))
		);
		trace (y_fraction_withinTextGridArea, U" ", distanceToCursorCircle);
		const bool nearCursorCircle = ( distanceToCursorCircle < 1.5 );

		if (nearBoundaryOrPoint) {
			/*
				Possibility 1: the user clicked near a boundary or point.
				Perhaps drag it.
			*/
			bool boundaryOrPointIsMovable = true;
			if (selectedIntervalTier) {
				const bool isLeftEdgeOfFirstInterval = ( our clickedLeftBoundary <= 1 );
				const bool isRightEdgeOfLastInterval = ( our clickedLeftBoundary > selectedIntervalTier -> intervals.size );
				boundaryOrPointIsMovable = ! isLeftEdgeOfFirstInterval && ! isRightEdgeOfLastInterval;
			}
			/*
				If the user clicked on an unselected boundary or point, we extend or shrink the selection to it.
			*/
			if (event -> shiftKeyPressed) {
				if (our anchorTime > 0.5 * (our startSelection + our endSelection))
					our endSelection = our anchorTime;
				else
					our startSelection = our anchorTime;
			}
			if (! boundaryOrPointIsMovable) {
				our draggingTime = undefined;
				our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
				our anchorTime = undefined;
				our clickedLeftBoundary = 0;
				return FunctionEditor_UPDATE_NEEDED;
			}
			/*
				Determine the set of selected boundaries and points, and the dragging range.
			*/
			our draggingTiers = zero_BOOLVEC (numberOfTiers);
			our leftDraggingBoundary = our tmin;
			our rightDraggingBoundary = our tmax;
			for (int itier = 1; itier <= numberOfTiers; itier ++) {
				/*
					If the user has pressed the shift key, let her drag all the boundaries and points at this time.
					Otherwise, let her only drag the boundary or point on the clicked tier.
				*/
				if (itier == mouseTier || our clickWasModifiedByShiftKey == our textGridArea() -> instancePref_shiftDragMultiple()) {
					IntervalTier intervalTier;
					TextTier textTier;
					AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [itier], & intervalTier, & textTier);
					if (intervalTier) {
						const integer ibound = IntervalTier_hasBoundary (intervalTier, our anchorTime);
						if (ibound) {
							TextInterval leftInterval = intervalTier -> intervals.at [ibound - 1];
							TextInterval rightInterval = intervalTier -> intervals.at [ibound];
							our draggingTiers [itier] = true;
							/*
								Prevent the user from dragging the boundary past its left or right neighbours on the same tier.
							*/
							Melder_clipLeft (leftInterval -> xmin, & our leftDraggingBoundary);
							Melder_clipRight (& our rightDraggingBoundary, rightInterval -> xmax);
						}
					} else {
						Melder_assert (isdefined (our anchorTime));
						if (AnyTier_hasPoint (textTier->asAnyTier(), our anchorTime)) {
							/*
								Other than with boundaries on interval tiers,
								points on text tiers can be dragged past their neighbours.
							*/
							our draggingTiers [itier] = true;
						}
					}
				}
			}
		} else if (nearCursorCircle) {
			/*
				Possibility 2: the user clicked near the cursor circle.
				Insert boundary or point. There is no danger that we insert on top of an existing boundary or point,
				because we are not 'nearBoundaryOrPoint'.
			*/
			Melder_assert (isdefined (our startSelection));   // precondition of v_updateText()
			if (our textGridArea() -> selectedTier != oldSelectedTier)
				our v_updateText();   // this puts the text of the newly clicked tier into the text area
			insertBoundaryOrPoint (this, mouseTier, our startSelection, our startSelection, false);
			//Melder_assert (isdefined (our startSelection));   // precondition of FunctionEditor_marksChanged()
			//FunctionEditor_marksChanged (this, true);
			Editor_broadcastDataChanged (this);
		} else {
			/*
				Possibility 3: the user clicked in empty space.
				Select the interval, if any.
			*/
			if (selectedIntervalTier) {
				our startSelection = startInterval;
				our endSelection = endInterval;
			}
		}
	} else if (event -> isDrag ()) {
		if (isdefined (our anchorTime) && our draggingTiers.size > 0) {
			our draggingTime = x_world;
			if (! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
				const double distanceToAnchor_mm = fabs (Graphics_dxWCtoMM (our graphics.get(), x_world - our anchorTime));
				if (distanceToAnchor_mm > draggingVicinityRadius_mm)
					our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = true;
			}
		}
	} else if (event -> isDrop ()) {
		if (isundef (our anchorTime) || our draggingTiers.size == 0) {   // TODO: figure out a circumstance under which anchorTime could be undefined
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}
		/*
			If the user shift-clicked, we extend the selection (this already happened during click()).
		*/
		if (event -> shiftKeyPressed && ! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}
		/*
			If the user dropped near an existing boundary in an unselected tier,
			we snap to that mark.
		*/
		const integer itierDrop = _TextGridEditor_y_fraction_local_toTier (this, y_fraction_withinTextGridArea);
		bool droppedOnABoundaryOrPointInsideAnUnselectedTier = false;
		if (x_world > 0.0 && our textGridArea() -> y_fraction_globalIsInside (y_fraction_global) && ! our draggingTiers [itierDrop]) {   // dropped inside an unselected tier?
			const Function anyTierDrop = our textGrid() -> tiers->at [itierDrop];
			if (anyTierDrop -> classInfo == classIntervalTier) {
				const IntervalTier tierDrop = (IntervalTier) anyTierDrop;
				for (integer ibound = 1; ibound < tierDrop -> intervals.size; ibound ++) {
					const TextInterval left = tierDrop -> intervals.at [ibound];
					const double mouseDistanceToBoundary = fabs (Graphics_dxWCtoMM (our graphics.get(), x_world - left -> xmax));
					if (mouseDistanceToBoundary < droppingVicinityRadius_mm) {
						x_world = left -> xmax;   // snap to boundary
						droppedOnABoundaryOrPointInsideAnUnselectedTier = true;
					}
				}
			} else {
				const TextTier tierDrop = (TextTier) anyTierDrop;
				for (integer ipoint = 1; ipoint <= tierDrop -> points.size; ipoint ++) {
					const TextPoint point = tierDrop -> points.at [ipoint];
					const double mouseDistanceToPoint_mm = fabs (Graphics_dxWCtoMM (our graphics.get(), x_world - point -> number));
					if (mouseDistanceToPoint_mm < droppingVicinityRadius_mm) {
						x_world = point -> number;   // snap to point
						droppedOnABoundaryOrPointInsideAnUnselectedTier = true;
					}
				}
			}
		}
		/*
			If the user dropped near the cursor (outside the anchor),
			we snap to the cursor.
		*/
		if (our startSelection == our endSelection && our startSelection != our anchorTime) {
			const double mouseDistanceToCursor = fabs (Graphics_dxWCtoMM (our graphics.get(), x_world - our startSelection));
			if (mouseDistanceToCursor < droppingVicinityRadius_mm)
				x_world = our startSelection;
		}
		/*
			If the user wiggled near the anchor, we snap to the anchor and bail out
			(a boundary has been selected, but nothing has been dragged).
		*/
		if (! our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce && ! droppedOnABoundaryOrPointInsideAnUnselectedTier) {
			our startSelection = our endSelection = our anchorTime;
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}

		/*
			We cannot move a boundary out of the dragging range.
		*/
		if (x_world <= our leftDraggingBoundary || x_world >= our rightDraggingBoundary) {
			Melder_beep ();
			our draggingTime = undefined;
			our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
			our anchorTime = undefined;
			our clickedLeftBoundary = 0;
			return FunctionEditor_UPDATE_NEEDED;
		}

		Editor_save (this, U"Drag");

		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			if (our draggingTiers [itier]) {
				IntervalTier intervalTier;
				TextTier textTier;
				AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [itier], & intervalTier, & textTier);
				if (intervalTier) {
					const integer numberOfIntervals = intervalTier -> intervals.size;
					for (integer ibound = 2; ibound <= numberOfIntervals; ibound ++) {
						TextInterval left = intervalTier -> intervals.at [ibound - 1], right = intervalTier -> intervals.at [ibound];
						if (left -> xmax == our anchorTime) {   // boundary dragged?
							left -> xmax = right -> xmin = x_world;   // move boundary to drop site
							break;
						}
					}
				} else {
					Melder_assert (isdefined (our anchorTime));
					const integer iDraggedPoint = AnyTier_hasPoint (textTier->asAnyTier(), our anchorTime);
					if (iDraggedPoint) {
						Melder_assert (isdefined (x_world));
						const integer dropSiteHasPoint = AnyTier_hasPoint (textTier->asAnyTier(), x_world);
						if (dropSiteHasPoint != 0) {
							Melder_warning (U"Cannot drop point on an existing point.");
						} else {
							const TextPoint point = textTier -> points.at [iDraggedPoint];
							/*
								Move point to drop site. May have passed another point.
							*/
							autoTextPoint newPoint = Data_copy (point);
							newPoint -> number = x_world;   // move point to drop site
							textTier -> points. removeItem (iDraggedPoint);
							textTier -> points. addItem_move (newPoint.move());
						}
					}
				}
			}
		}

		/*
			Select the drop site.
		*/
		our startSelection = our endSelection = x_world;
		Melder_sort (& our startSelection, & our endSelection);
		our draggingTime = undefined;
		our hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
		our anchorTime = undefined;
		our clickedLeftBoundary = 0;
		//Melder_assert (isdefined (our startSelection));   // precondition of FunctionEditor_marksChanged()
		//FunctionEditor_marksChanged (this, true);
		Editor_broadcastDataChanged (this);
	}
	return FunctionEditor_UPDATE_NEEDED;
}

void structAnyTextGridEditor :: v_clickSelectionViewer (double xWC, double yWC) {
	integer rowNumber = Melder_iceiling ((1.0 - yWC) * 12.0);
	integer columnNumber = Melder_iceiling (xWC * 10.0);
	if (rowNumber < 1 || rowNumber > 12)
		return;
	if (columnNumber < 1 || columnNumber > 10)
		return;
	conststring32 character = characters [rowNumber-1] [columnNumber-1];
	character += str32len (character) - 1;
	if (our textArea) {
		integer first = 0, last = 0;
		autostring32 oldText = GuiText_getStringAndSelectionPosition (our textArea, & first, & last);
		static MelderString newText;
		MelderString_empty (& newText);
		MelderString_ncopy (& newText, oldText.get(), first);
		MelderString_append (& newText, character);
		MelderString_append (& newText, oldText.get() + last);
		if (our textGridArea() -> selectedTier != 0) {
			IntervalTier intervalTier;
			TextTier textTier;
			AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [our textGridArea() -> selectedTier], & intervalTier, & textTier);
			if (intervalTier) {
				const integer selectedInterval = getSelectedInterval (our textGridArea().get());
				if (selectedInterval != 0) {
					TextInterval interval = intervalTier -> intervals.at [selectedInterval];
					TextInterval_setText (interval, newText.string);

					our suppressRedraw = true;   // prevent valueChangedCallback from redrawing
					trace (U"setting new text ", newText.string);
					GuiText_setString (our textArea, newText.string);
					GuiText_setSelection (our textArea, first + 1, first + 1);
					our suppressRedraw = false;

					//FunctionEditor_redraw (this); TRY OUT 2022-06-12
					Editor_broadcastDataChanged (this);
				}
			} else {
				const integer selectedPoint = getSelectedPoint (our textGridArea().get());
				if (selectedPoint != 0) {
					TextPoint point = textTier -> points.at [selectedPoint];
					point -> mark. reset();
					if (Melder_findInk (newText.string))   // any visible characters?
						point -> mark = Melder_dup_f (newText.string);

					our suppressRedraw = true;   // prevent valueChangedCallback from redrawing
					trace (U"setting new text ", newText.string);
					GuiText_setString (our textArea, newText.string);
					GuiText_setSelection (our textArea, first + 1, first + 1);
					our suppressRedraw = false;

					//FunctionEditor_redraw (this); TRY OUT 2022-06-12
					Editor_broadcastDataChanged (this);
				}
			}
		}
	}
}

void structAnyTextGridEditor :: v_play (double startTime, double endTime) {
	if (our soundArea)
		SoundArea_play (our soundArea.get(), startTime, endTime);
}

void structAnyTextGridEditor :: v_updateText () {
	conststring32 newText = U"";
	trace (U"selected tier ", our textGridArea() -> selectedTier);
	if (our textGridArea() -> selectedTier != 0) {
		IntervalTier intervalTier;
		TextTier textTier;
		AnyTextGridTier_identifyClass (our textGrid() -> tiers->at [our textGridArea() -> selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			const integer iinterval = IntervalTier_timeToIndex (intervalTier, our startSelection);
			if (iinterval) {
				TextInterval interval = intervalTier -> intervals.at [iinterval];
				if (interval -> text)
					newText = interval -> text.get();
			}
		} else {
			Melder_assert (isdefined (our startSelection));
			const integer ipoint = AnyTier_hasPoint (textTier->asAnyTier(), our startSelection);
			if (ipoint) {
				TextPoint point = textTier -> points.at [ipoint];
				if (point -> mark)
					newText = point -> mark.get();
			}
		}
	}
	if (our textArea) {
		our suppressRedraw = true;   // prevent valueChangedCallback from redrawing
		trace (U"setting new text ", newText);
		GuiText_setString (our textArea, newText);
		const integer cursor = str32len (newText);   // at end
		GuiText_setSelection (our textArea, cursor, cursor);
		our suppressRedraw = false;
	}
}

POSITIVE_VARIABLE (v_prefs_addFields__fontSize)
OPTIONMENU_ENUM_VARIABLE (kGraphics_horizontalAlignment, v_prefs_addFields__textAlignmentInIntervals)
OPTIONMENU_VARIABLE (v_prefs_addFields__useTextStyles)
OPTIONMENU_VARIABLE (v_prefs_addFields__shiftDragMultiple)
OPTIONMENU_ENUM_VARIABLE (kTextGridArea_showNumberOf, v_prefs_addFields__showNumberOf)
OPTIONMENU_ENUM_VARIABLE (kMelder_string, v_prefs_addFields__paintIntervalsGreenWhoseLabel)
SENTENCE_VARIABLE (v_prefs_addFields__theText)
void structAnyTextGridEditor :: v_prefs_addFields (EditorCommand cmd) {
	UiField _radio_;
	POSITIVE_FIELD (v_prefs_addFields__fontSize, U"Font size (points)", our textGridArea() -> default_fontSize())
	OPTIONMENU_ENUM_FIELD (kGraphics_horizontalAlignment, v_prefs_addFields__textAlignmentInIntervals,
			U"Text alignment in intervals", kGraphics_horizontalAlignment::DEFAULT)
	OPTIONMENU_FIELD (v_prefs_addFields__useTextStyles, U"The symbols %#_^ in labels", our textGridArea() -> default_useTextStyles() + 1)
		OPTION (U"are shown as typed")
		OPTION (U"mean italic/bold/sub/super")
	OPTIONMENU_FIELD (v_prefs_addFields__shiftDragMultiple, U"With the shift key, you drag", our textGridArea() -> default_shiftDragMultiple() + 1)
		OPTION (U"a single boundary")
		OPTION (U"multiple boundaries")
	OPTIONMENU_ENUM_FIELD (kTextGridArea_showNumberOf, v_prefs_addFields__showNumberOf,
			U"Show number of", kTextGridArea_showNumberOf::DEFAULT)
	OPTIONMENU_ENUM_FIELD (kMelder_string, v_prefs_addFields__paintIntervalsGreenWhoseLabel,
			U"Paint intervals green whose label...", kMelder_string::DEFAULT)
	SENTENCE_FIELD (v_prefs_addFields__theText, U"...the text", our textGridArea() -> default_greenString())
}
void structAnyTextGridEditor :: v_prefs_setValues (EditorCommand cmd) {
	SET_OPTION (v_prefs_addFields__useTextStyles, our textGridArea() -> instancePref_useTextStyles() + 1)
	SET_REAL (v_prefs_addFields__fontSize, our textGridArea() -> instancePref_fontSize())
	SET_ENUM (v_prefs_addFields__textAlignmentInIntervals, kGraphics_horizontalAlignment, our textGridArea() -> instancePref_alignment())
	SET_OPTION (v_prefs_addFields__shiftDragMultiple, our textGridArea() -> instancePref_shiftDragMultiple() + 1)
	SET_ENUM (v_prefs_addFields__showNumberOf, kTextGridArea_showNumberOf, our textGridArea() -> instancePref_showNumberOf())
	SET_ENUM (v_prefs_addFields__paintIntervalsGreenWhoseLabel, kMelder_string, our textGridArea() -> instancePref_greenMethod())
	SET_STRING (v_prefs_addFields__theText, our textGridArea() -> instancePref_greenString())
}
void structAnyTextGridEditor :: v_prefs_getValues (EditorCommand /* cmd */) {
	our textGridArea() -> setInstancePref_useTextStyles (v_prefs_addFields__useTextStyles - 1);
	our textGridArea() -> setInstancePref_fontSize (v_prefs_addFields__fontSize);
	our textGridArea() -> setInstancePref_alignment (v_prefs_addFields__textAlignmentInIntervals);
	our textGridArea() -> setInstancePref_shiftDragMultiple (v_prefs_addFields__shiftDragMultiple - 1);
	our textGridArea() -> setInstancePref_showNumberOf (v_prefs_addFields__showNumberOf);
	our textGridArea() -> setInstancePref_greenMethod (v_prefs_addFields__paintIntervalsGreenWhoseLabel);
	our textGridArea() -> setInstancePref_greenString (v_prefs_addFields__theText);
}

void structAnyTextGridEditor :: v_createMenuItems_view_timeDomain (EditorMenu menu) {
	AnyTextGridEditor_Parent :: v_createMenuItems_view_timeDomain (menu);
	EditorMenu_addCommand (menu, U"Select previous tier", GuiMenu_OPTION | GuiMenu_UP_ARROW, menu_cb_SelectPreviousTier);
	EditorMenu_addCommand (menu, U"Select next tier", GuiMenu_OPTION | GuiMenu_DOWN_ARROW, menu_cb_SelectNextTier);
	EditorMenu_addCommand (menu, U"Select previous interval", GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_SelectPreviousInterval);
	EditorMenu_addCommand (menu, U"Select next interval", GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_SelectNextInterval);
	EditorMenu_addCommand (menu, U"Extend-select left", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_ExtendSelectPreviousInterval);
	EditorMenu_addCommand (menu, U"Extend-select right", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_ExtendSelectNextInterval);
}

void structAnyTextGridEditor :: v_updateMenuItems () {
	AnyTextGridEditor_Parent :: v_updateMenuItems ();
	GuiThing_setSensitive (extractSelectedTextGridPreserveTimesButton, our endSelection > our startSelection);
	GuiThing_setSensitive (extractSelectedTextGridTimeFromZeroButton,  our endSelection > our startSelection);
}

/********** EXPORTED **********/

void AnyTextGridEditor_init (AnyTextGridEditor me, conststring32 title, TextGrid textGrid,
	SpellingChecker spellingChecker, conststring32 callbackSocket)
{
	my spellingChecker = spellingChecker;   // set before FunctionEditor_init, which may install spellingChecker menus
	my callbackSocket = Melder_dup (callbackSocket);
	FunctionEditor_init (me, title, textGrid);

	my textGridArea() -> selectedTier = 1;
	my draggingTime = undefined;
	Melder_assert (isdefined (my startSelection));   // precondition of v_updateText()
	my v_updateText ();   // to reflect changed tier selection
	if (my endWindow - my startWindow > 30.0) {
		my endWindow = my startWindow + 30.0;
		if (my startWindow == my tmin)
			my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
		Melder_assert (isdefined (my startSelection));   // precondition of FunctionEditor_marksChanged()
		FunctionEditor_marksChanged (me, false);
	}
	if (spellingChecker)
		GuiText_setSelection (my textArea, 0, 0);
	if (my soundOrLongSound() &&
		my soundOrLongSound() -> xmin == 0.0 &&
		my textGrid() -> xmin != 0.0 &&
		my textGrid() -> xmax > my soundOrLongSound() -> xmax
	)
		Melder_warning (U"The time domain of the TextGrid (starting at ",
			Melder_fixed (my textGrid() -> xmin, 6), U" seconds) does not overlap with that of the sound "
			U"(which starts at 0 seconds).\nIf you want to repair this, you can select the TextGrid "
			U"and choose “Shift times to...” from the Modify menu "
			U"to shift the starting time of the TextGrid to zero.");
}

/* End of file AnyTextGridEditor.cpp */
