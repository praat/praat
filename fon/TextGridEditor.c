/* TextGridEditor.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2002/10/06 improved visibility of dragging
 * pb 2004/04/13 less flashing
 * pb 2005/01/11 better visibility of yellow line
 * pb 2005/03/02 green colouring for matching labels
 * pb 2005/05/05 show number of intervals
 * pb 2005/06/17 enums
 * pb 2005/09/23 interface update
 * pb 2006/12/18 better info
 * pb 2007/03/23 new Editor API
 * Erez Volk & pb 2007/05/17 FLAC support
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/09/02 direct drawing to Picture window
 * pb 2007/09/04 TimeSoundAnalysisEditor
 * pb 2007/09/05 direct drawing to picture window
 * pb 2007/11/30 erased Graphics_printf
 * pb 2007/12/07 enums
 * Erez Volk 2008/03/16 Write selected TextGrid to text file
 * pb 2008/03/17 extract selected TextGrid
 * pb 2008/03/18 renamed: "convert to backslash trigraphs/Unicode"
 * pb 2008/03/20 split off Help menu
 * pb 2008/09/23 info: selectedTier
 * pb 2010/11/10 correctedIinterval2
 */

#include "TextGridEditor.h"
#include "SpellingChecker.h"
#include "Preferences.h"
#include "EditorM.h"
#include "SoundEditor.h"
#include "Sound_and_Spectrogram.h"
#include "TextGrid_Sound.h"

#include "enums_getText.h"
#include "TextGridEditor_enums.h"
#include "enums_getValue.h"
#include "TextGridEditor_enums.h"

#define TextGridEditor__members(Klas) TimeSoundAnalysisEditor__members(Klas) \
	SpellingChecker spellingChecker; \
	long selectedTier; \
	bool useTextStyles, shiftDragMultiple, suppressRedraw; \
	int fontSize; \
	enum kGraphics_horizontalAlignment alignment; \
	wchar_t *findString, greenString [Preferences_STRING_BUFFER_SIZE]; \
	int showNumberOf, greenMethod; \
	GuiObject extractSelectedTextGridPreserveTimesButton, extractSelectedTextGridTimeFromZeroButton, writeSelectedTextGridButton;
#define TextGridEditor__methods(Klas) TimeSoundAnalysisEditor__methods(Klas)
Thing_declare2 (TextGridEditor, TimeSoundAnalysisEditor);

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
		wchar_t greenString [Preferences_STRING_BUFFER_SIZE];
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

static void info (I) {
	iam (TextGridEditor);
	inherited (TextGridEditor) info (TextGridEditor_as_parent (me));
	MelderInfo_writeLine2 (L"Selected tier: ", Melder_integer (my selectedTier));
	MelderInfo_writeLine2 (L"TextGrid uses text styles: ", Melder_boolean (my useTextStyles));
	MelderInfo_writeLine2 (L"TextGrid font size: ", Melder_integer (my fontSize));
	MelderInfo_writeLine2 (L"TextGrid alignment: ", kGraphics_horizontalAlignment_getText (my alignment));
}

/********** UTILITIES **********/

static double _TextGridEditor_computeSoundY (TextGridEditor me) {
	TextGrid grid = my data;
	int ntier = grid -> tiers -> size;
	int showAnalysis = (my spectrogram.show || my pitch.show || my intensity.show || my formant.show) && (my longSound.data || my sound.data);
	return my sound.data || my longSound.data ? ntier / (2.0 + ntier * (showAnalysis ? 1.8 : 1.3)) : 1.0;
}

static void _AnyTier_identifyClass (Data anyTier, IntervalTier *intervalTier, TextTier *textTier) {
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		*intervalTier = (IntervalTier) anyTier;
		*textTier = NULL;
	} else {
		*intervalTier = NULL;
		*textTier = (TextTier) anyTier;
	}
}

static int _TextGridEditor_yWCtoTier (TextGridEditor me, double yWC) {
	TextGrid grid = my data;
	int ntier = grid -> tiers -> size;
	double soundY = _TextGridEditor_computeSoundY (me);
	int itier = ntier - (int) floor (yWC / soundY * (double) ntier);
	if (itier < 1) itier = 1; if (itier > ntier) itier = ntier;
	return itier;
}

static void _TextGridEditor_timeToInterval (TextGridEditor me, double t, int itier, double *tmin, double *tmax) {
	TextGrid grid = my data;
	IntervalTier intervalTier;
	TextTier textTier;
	_AnyTier_identifyClass (grid -> tiers -> item [itier], & intervalTier, & textTier);
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
		interval = intervalTier -> intervals -> item [iinterval];
		*tmin = interval -> xmin;
		*tmax = interval -> xmax;
	} else {
		long n = textTier -> points -> size;
		if (n == 0) {
			*tmin = my tmin;
			*tmax = my tmax;
		} else {
			long ipointleft = AnyTier_timeToLowIndex (textTier, t);
			*tmin = ipointleft == 0 ? my tmin : ((TextPoint) textTier -> points -> item [ipointleft]) -> time;
			*tmax = ipointleft == n ? my tmax : ((TextPoint) textTier -> points -> item [ipointleft + 1]) -> time;
		}
	}
	if (*tmin < my tmin) *tmin = my tmin;   /* Clip by FunctionEditor's time domain. */
	if (*tmax > my tmax) *tmax = my tmax;
}

static int checkTierSelection (TextGridEditor me, const wchar_t *verbPhrase) {
	TextGrid grid = my data;
	if (my selectedTier < 1 || my selectedTier > grid -> tiers -> size)
		return Melder_error3 (L"To ", verbPhrase, L", first select a tier by clicking anywhere inside it.");
	return 1;
}

static long getSelectedInterval (TextGridEditor me) {
	TextGrid grid = my data;
	IntervalTier tier;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	tier = grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> methods == classIntervalTier);
	return IntervalTier_timeToIndex (tier, my startSelection);
}

static long getSelectedLeftBoundary (TextGridEditor me) {
	TextGrid grid = my data;
	IntervalTier tier;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	tier = grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> methods == classIntervalTier);
	return IntervalTier_hasBoundary (tier, my startSelection);
}

static long getSelectedPoint (TextGridEditor me) {
	TextGrid grid = my data;
	TextTier tier;
	Melder_assert (my selectedTier >= 1 || my selectedTier <= grid -> tiers -> size);
	tier = grid -> tiers -> item [my selectedTier];
	Melder_assert (tier -> methods == classTextTier);
	return AnyTier_hasPoint (tier, my startSelection);
}

static void scrollToView (TextGridEditor me, double t) {
	if (t <= my startWindow) {
		FunctionEditor_shift (TextGridEditor_as_FunctionEditor (me), t - my startWindow - 0.618 * (my endWindow - my startWindow));
	} else if (t >= my endWindow) {
		FunctionEditor_shift (TextGridEditor_as_FunctionEditor (me), t - my endWindow + 0.618 * (my endWindow - my startWindow));
	} else {
		FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));
	}
}

/********** METHODS **********/

/*
 * The main invariant of the TextGridEditor is that the selected interval
 * always has the cursor in it, and that the cursor always selects an interval
 * if the selected tier is an interval tier.
 */

static void destroy (I) {
	iam (TextGridEditor);
	forget (my sound.data);
	inherited (TextGridEditor) destroy (TextGridEditor_as_parent (me));
}

/***** FILE MENU *****/

static int menu_cb_ExtractSelectedTextGrid_preserveTimes (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my endSelection <= my startSelection) return Melder_error1 (L"No selection.");
	TextGrid extract = TextGrid_extractPart (my data, my startSelection, my endSelection, true);
	if (! extract) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, extract);
	return 1;
}

static int menu_cb_ExtractSelectedTextGrid_timeFromZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my endSelection <= my startSelection) return Melder_error1 (L"No selection.");
	TextGrid extract = TextGrid_extractPart (my data, my startSelection, my endSelection, false);
	if (! extract) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, extract);
	return 1;
}

static void createMenuItems_file_extract (TextGridEditor me, EditorMenu menu) {
	inherited (TextGridEditor) createMenuItems_file_extract (TextGridEditor_as_parent (me), menu);
	my extractSelectedTextGridPreserveTimesButton =
		EditorMenu_addCommand (menu, L"Extract selected TextGrid (preserve times)", 0, menu_cb_ExtractSelectedTextGrid_preserveTimes);
	my extractSelectedTextGridTimeFromZeroButton =
		EditorMenu_addCommand (menu, L"Extract selected TextGrid (time from zero)", 0, menu_cb_ExtractSelectedTextGrid_timeFromZero);
}

static int menu_cb_WriteSelectionToTextFile (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM_WRITE (L"Write selection to TextGrid text file", 0)
		swprintf (defaultName, 300, L"%ls.TextGrid", ((Thing) my data) -> name);
	EDITOR_DO_WRITE
		TextGrid publish = TextGrid_extractPart (my data, my startSelection, my endSelection, false);
		if (! publish) return 0;
		if (! Data_writeToTextFile (publish, file)) return 0;
		forget (publish);
	EDITOR_END
}

static int menu_cb_WriteToTextFile (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM_WRITE (L"Write to TextGrid text file", 0)
		swprintf (defaultName, 300, L"%ls.TextGrid", ((Thing) my data) -> name);
	EDITOR_DO_WRITE
		if (! Data_writeToTextFile (my data, file)) return 0;
	EDITOR_END
}

static void createMenuItems_file_write (TextGridEditor me, EditorMenu menu) {
	inherited (TextGridEditor) createMenuItems_file_write (TextGridEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"Write TextGrid to text file...", 'S', menu_cb_WriteToTextFile);
	my writeSelectedTextGridButton = EditorMenu_addCommand (menu, L"Write selected TextGrid to text file...", 0, menu_cb_WriteSelectionToTextFile);
}

static int menu_cb_DrawVisibleTextGrid (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Draw visible TextGrid", 0)
		our form_pictureWindow (me, cmd);
		our form_pictureMargins (me, cmd);
		our form_pictureSelection (me, cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		our ok_pictureWindow (me, cmd);
		our ok_pictureMargins (me, cmd);
		our ok_pictureSelection (me, cmd);
		SET_INTEGER (L"Garnish", preferences.picture.garnish);
	EDITOR_DO
		our do_pictureWindow (me, cmd);
		our do_pictureMargins (me, cmd);
		our do_pictureSelection (me, cmd);
		preferences.picture.garnish = GET_INTEGER (L"Garnish");
		Editor_openPraatPicture (TextGridEditor_as_Editor (me));
		TextGrid_Sound_draw (my data, NULL, my pictureGraphics, my startWindow, my endWindow, true, my useTextStyles,
			preferences.picture.garnish);
		FunctionEditor_garnish (TextGridEditor_as_FunctionEditor (me));
		Editor_closePraatPicture (TextGridEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_DrawVisibleSoundAndTextGrid (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Draw visible sound and TextGrid", 0)
		our form_pictureWindow (me, cmd);
		our form_pictureMargins (me, cmd);
		our form_pictureSelection (me, cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		our ok_pictureWindow (me, cmd);
		our ok_pictureMargins (me, cmd);
		our ok_pictureSelection (me, cmd);
		SET_INTEGER (L"Garnish", preferences.picture.garnish);
	EDITOR_DO
		our do_pictureWindow (me, cmd);
		our do_pictureMargins (me, cmd);
		our do_pictureSelection (me, cmd);
		preferences.picture.garnish = GET_INTEGER (L"Garnish");
		Editor_openPraatPicture (TextGridEditor_as_Editor (me));
		Sound publish = my longSound.data ?
			LongSound_extractPart (my longSound.data, my startWindow, my endWindow, true) :
			Sound_extractPart (my sound.data, my startWindow, my endWindow, kSound_windowShape_RECTANGULAR, 1.0, true);
		if (! publish) return 0;
		TextGrid_Sound_draw (my data, publish, my pictureGraphics, my startWindow, my endWindow, true, my useTextStyles, preferences.picture.garnish);
		forget (publish);
		FunctionEditor_garnish (TextGridEditor_as_FunctionEditor (me));
		Editor_closePraatPicture (TextGridEditor_as_Editor (me));
	EDITOR_END
}

static void createMenuItems_file_draw (TextGridEditor me, EditorMenu menu) {
	inherited (TextGridEditor) createMenuItems_file_draw (TextGridEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"Draw visible TextGrid...", 0, menu_cb_DrawVisibleTextGrid);
	if (my sound.data || my longSound.data)
		EditorMenu_addCommand (menu, L"Draw visible sound and TextGrid...", 0, menu_cb_DrawVisibleSoundAndTextGrid);
}

/***** EDIT MENU *****/

#ifndef macintosh
static int menu_cb_Cut (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	GuiText_cut (my text);
	return 1;
}
static int menu_cb_Copy (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	GuiText_copy (my text);
	return 1;
}
static int menu_cb_Paste (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	GuiText_paste (my text);
	return 1;
}
static int menu_cb_Erase (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	GuiText_remove (my text);
	return 1;
}
#endif

static int menu_cb_Genericize (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	Editor_save (TextGridEditor_as_Editor (me), L"Convert to Backslash Trigraphs");
	TextGrid_genericize (my data);
	FunctionEditor_updateText (TextGridEditor_as_FunctionEditor (me));
	FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
	Editor_broadcastChange (TextGridEditor_as_Editor (me));
	return 1;
}

static int menu_cb_Nativize (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	Editor_save (TextGridEditor_as_Editor (me), L"Convert to Unicode");
	TextGrid_nativize (my data);
	FunctionEditor_updateText (TextGridEditor_as_FunctionEditor (me));
	FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
	Editor_broadcastChange (TextGridEditor_as_Editor (me));
	return 1;
}

/***** QUERY MENU *****/

static int menu_cb_GetStartingPointOfInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, L"query the starting point of an interval")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		double time = iinterval < 1 || iinterval > tier -> intervals -> size ? NUMundefined :
			((TextInterval) tier -> intervals -> item [iinterval]) -> xmin;
		Melder_informationReal (time, L"seconds");
	} else {
		return Melder_error1 (L"The selected tier is not an interval tier.");
	}
	return 1;
}

static int menu_cb_GetEndPointOfInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, L"query the end point of an interval")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		double time = iinterval < 1 || iinterval > tier -> intervals -> size ? NUMundefined :
			((TextInterval) tier -> intervals -> item [iinterval]) -> xmax;
		Melder_informationReal (time, L"seconds");
	} else {
		return Melder_error1 (L"The selected tier is not an interval tier.");
	}
	return 1;
}

static int menu_cb_GetLabelOfInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, L"query the label of an interval")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection);
		wchar_t *label = iinterval < 1 || iinterval > tier -> intervals -> size ? L"" :
			((TextInterval) tier -> intervals -> item [iinterval]) -> text;
		Melder_information1 (label);
	} else {
		return Melder_error1 (L"The selected tier is not an interval tier.");
	}
	return 1;
}

/***** VIEW MENU *****/

static void do_selectAdjacentTier (TextGridEditor me, int previous) {
	TextGrid grid = my data;
	long n = grid -> tiers -> size;
	if (n >= 2) {
		my selectedTier = previous ?
			my selectedTier > 1 ? my selectedTier - 1 : n :
			my selectedTier < n ? my selectedTier + 1 : 1;
		_TextGridEditor_timeToInterval (me, my startSelection, my selectedTier, & my startSelection, & my endSelection);
		FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));
	}
}

static int menu_cb_SelectPreviousTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentTier (me, TRUE);
	return 1;
}

static int menu_cb_SelectNextTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentTier (me, FALSE);
	return 1;
}

static void do_selectAdjacentInterval (TextGridEditor me, int previous, int shift) {
	TextGrid grid = my data;
	IntervalTier intervalTier;
	TextTier textTier;
	if (my selectedTier < 1 || my selectedTier > grid -> tiers -> size) return;
	_AnyTier_identifyClass (grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);
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
					interval = intervalTier -> intervals -> item [iinterval];
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
				} else if (previous) {
					if (einterval > iinterval + 1) {
						if (einterval <= n + 1) {
							interval = intervalTier -> intervals -> item [einterval - 1];
							my endSelection = interval -> xmin;
						}
					} else if (binterval > 1) {
						interval = intervalTier -> intervals -> item [binterval - 1];
						my startSelection = interval -> xmin;
					}
				} else {
					if (binterval < iinterval) {
						if (binterval > 0) {
							interval = intervalTier -> intervals -> item [binterval];
							my startSelection = interval -> xmax;
						}
					} else if (einterval <= n) {
						interval = intervalTier -> intervals -> item [einterval];
						my endSelection = interval -> xmax;
					}
				}
			} else {
				iinterval = previous ?
					iinterval > 1 ? iinterval - 1 : n :
					iinterval < n ? iinterval + 1 : 1;
				interval = intervalTier -> intervals -> item [iinterval];
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
			point = textTier -> points -> item [ipoint];
			my startSelection = my endSelection = point -> time;
			scrollToView (me, my startSelection);
		}
	}
}

static int menu_cb_SelectPreviousInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentInterval (me, TRUE, FALSE);
	return 1;
}

static int menu_cb_SelectNextInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentInterval (me, FALSE, FALSE);
	return 1;
}

static int menu_cb_ExtendSelectPreviousInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentInterval (me, TRUE, TRUE);
	return 1;
}

static int menu_cb_ExtendSelectNextInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_selectAdjacentInterval (me, FALSE, TRUE);
	return 1;
}

static int menu_cb_MoveBtoZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	double zero = Sound_getNearestZeroCrossing (my sound.data, my startSelection, 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my startSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));
	}
	return 1;
}

static int menu_cb_MoveCursorToZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	double zero = Sound_getNearestZeroCrossing (my sound.data, 0.5 * (my startSelection + my endSelection), 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my startSelection = my endSelection = zero;
		FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));
	}
	return 1;
}

static int menu_cb_MoveEtoZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	double zero = Sound_getNearestZeroCrossing (my sound.data, my endSelection, 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my endSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));
	}
	return 1;
}

/***** PITCH MENU *****/

static int menu_cb_DrawTextGridAndPitch (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Draw TextGrid and Pitch separately", 0)
		our form_pictureWindow (me, cmd);
		LABEL (L"", L"TextGrid:")
		BOOLEAN (L"Show boundaries and points", 1);
		LABEL (L"", L"Pitch:")
		BOOLEAN (L"Speckle", 0);
		our form_pictureMargins (me, cmd);
		our form_pictureSelection (me, cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		our ok_pictureWindow (me, cmd);
		SET_INTEGER (L"Show boundaries and points", preferences.picture.showBoundaries);
		SET_INTEGER (L"Speckle", preferences.picture.pitch.speckle);
		our ok_pictureMargins (me, cmd);
		our ok_pictureSelection (me, cmd);
		SET_INTEGER (L"Garnish", preferences.picture.garnish);
	EDITOR_DO
		our do_pictureWindow (me, cmd);
		preferences.picture.showBoundaries = GET_INTEGER (L"Show boundaries and points");
		preferences.picture.pitch.speckle = GET_INTEGER (L"Speckle");
		our do_pictureMargins (me, cmd);
		our do_pictureSelection (me, cmd);
		preferences.picture.garnish = GET_INTEGER (L"Garnish");
		if (! my pitch.show)
			return Melder_error1 (L"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
		if (! my pitch.data) {
			TimeSoundAnalysisEditor_computePitch (TextGridEditor_as_TimeSoundAnalysisEditor (me));
			if (! my pitch.data) return Melder_error1 (L"Cannot compute pitch.");
		}
		Editor_openPraatPicture (TextGridEditor_as_Editor (me));
		double pitchFloor_hidden = ClassFunction_convertStandardToSpecialUnit (classPitch, my pitch.floor, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchCeiling_hidden = ClassFunction_convertStandardToSpecialUnit (classPitch, my pitch.ceiling, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchFloor_overt = ClassFunction_convertToNonlogarithmic (classPitch, pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchCeiling_overt = ClassFunction_convertToNonlogarithmic (classPitch, pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, my pitch.unit);
		double pitchViewFrom_overt = my pitch.viewFrom < my pitch.viewTo ? my pitch.viewFrom : pitchFloor_overt;
		double pitchViewTo_overt = my pitch.viewFrom < my pitch.viewTo ? my pitch.viewTo : pitchCeiling_overt;
		TextGrid_Pitch_drawSeparately (my data, my pitch.data, my pictureGraphics, my startWindow, my endWindow,
			pitchViewFrom_overt, pitchViewTo_overt, GET_INTEGER (L"Show boundaries and points"), my useTextStyles, GET_INTEGER (L"Garnish"),
			GET_INTEGER (L"Speckle"), my pitch.unit);
		FunctionEditor_garnish (TextGridEditor_as_FunctionEditor (me));
		Editor_closePraatPicture (TextGridEditor_as_Editor (me));
	EDITOR_END
}

/***** INTERVAL MENU *****/

static int insertBoundaryOrPoint (TextGridEditor me, int itier, double t1, double t2, bool insertSecond) {
	TextGrid grid = my data;
	IntervalTier intervalTier;
	TextTier textTier;
	int ntiers = grid -> tiers -> size;
	if (itier < 1 || itier > ntiers) return 0;
	_AnyTier_identifyClass (grid -> tiers -> item [itier], & intervalTier, & textTier);
	Melder_assert (t1 <= t2);

	if (intervalTier) {
		TextInterval rightNewInterval = NULL, midNewInterval = NULL;
		bool t1IsABoundary = IntervalTier_hasTime (intervalTier, t1);
		bool t2IsABoundary = IntervalTier_hasTime (intervalTier, t2);
		if (t1 == t2 && t1IsABoundary) {
			Melder_error3 (L"Cannot add a boundary at ", Melder_fixed (t1, 6), L" seconds, because there is already a boundary there.");
			Melder_flushError (NULL);
			return 0;
		} else if (t1IsABoundary && t2IsABoundary) {
			Melder_error5 (L"Cannot add boundaries at ", Melder_fixed (t1, 6), L" and ", Melder_fixed (t2, 6), L" seconds, because there are already boundaries there.");
			Melder_flushError (NULL);
			return 0;
		}
		long iinterval = IntervalTier_timeToIndex (intervalTier, t1);
		//Melder_casual ("iinterval %ld, t = %f", iinterval, t1);
		long iinterval2 = t1 == t2 ? iinterval : IntervalTier_timeToIndex (intervalTier, t2);
		//Melder_casual ("iinterval2 %ld, t = %f", iinterval2, t2);
		if (iinterval == 0 || iinterval2 == 0) {
			return 0;   // selection is outside time domain of intervals
		}
		long correctedIinterval2 = t2IsABoundary && iinterval2 == intervalTier -> intervals -> size ? iinterval2 + 1 : iinterval2;
		if (correctedIinterval2 > iinterval + 1 || (correctedIinterval2 > iinterval && ! t2IsABoundary)) {
			return 0;   // selection straddles a boundary
		}
		TextInterval interval = intervalTier -> intervals -> item [iinterval];

		if (t1 == t2) {
			Editor_save (TextGridEditor_as_Editor (me), L"Add boundary");
		} else {
			Editor_save (TextGridEditor_as_Editor (me), L"Add interval");
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
			TextInterval_setText (interval, Melder_wcscat2 (interval -> text, midNewInterval -> text));
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
			TextInterval_setText (rightNewInterval, Melder_wcscat2 (midNewInterval -> text, rightNewInterval -> text));
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
		if (AnyTier_hasPoint (textTier, t1)) {
			Melder_flushError ("Cannot add a point at %f seconds, because there is already a point there.", t1);
			return 0;
		} 

		Editor_save (TextGridEditor_as_Editor (me), L"Add point");

		newPoint = TextPoint_create (t1, L"");
		Collection_addItem (textTier -> points, newPoint);
	}
	my startSelection = my endSelection = t1;
	return 1;
}

static void do_insertIntervalOnTier (TextGridEditor me, int itier) {
	if (! insertBoundaryOrPoint (me, itier,
		my playingCursor || my playingSelection ? my playCursor : my startSelection,
		my playingCursor || my playingSelection ? my playCursor : my endSelection,
		true)) return;
	my selectedTier = itier;
	FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));
	Editor_broadcastChange (TextGridEditor_as_Editor (me));
}

static int menu_cb_InsertIntervalOnTier1 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 1); return 1; }
static int menu_cb_InsertIntervalOnTier2 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 2); return 1; }
static int menu_cb_InsertIntervalOnTier3 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 3); return 1; }
static int menu_cb_InsertIntervalOnTier4 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 4); return 1; }
static int menu_cb_InsertIntervalOnTier5 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 5); return 1; }
static int menu_cb_InsertIntervalOnTier6 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 6); return 1; }
static int menu_cb_InsertIntervalOnTier7 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 7); return 1; }
static int menu_cb_InsertIntervalOnTier8 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertIntervalOnTier (me, 8); return 1; }

/***** BOUNDARY/POINT MENU *****/

static int menu_cb_RemovePointOrBoundary (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, L"remove a point or boundary")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (! selectedLeftBoundary) return Melder_error1 (L"To remove a boundary, first click on it.");

		Editor_save (TextGridEditor_as_Editor (me), L"Remove boundary");
		IntervalTier_removeLeftBoundary (tier, selectedLeftBoundary); iferror return 0;
	} else {
		TextTier tier = (TextTier) anyTier;
		long selectedPoint = getSelectedPoint (me);
		if (! selectedPoint) return Melder_error1 (L"To remove a point, first click on it.");

		Editor_save (TextGridEditor_as_Editor (me), L"Remove point");
		Collection_removeItem (tier -> points, selectedPoint);
	}
	FunctionEditor_updateText (TextGridEditor_as_FunctionEditor (me));
	FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
	Editor_broadcastChange (TextGridEditor_as_Editor (me));
	return 1;
}

static int do_movePointOrBoundary (TextGridEditor me, int where) {
	double position;
	TextGrid grid = my data;
	Data anyTier;
	if (where == 0 && my sound.data == NULL) return 1;
	if (! checkTierSelection (me, L"move a point or boundary")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		static wchar_t *boundarySaveText [3] = { L"Move boundary to zero crossing", L"Move boundary to B", L"Move boundary to E" };
		TextInterval left, right;
		long selectedLeftBoundary = getSelectedLeftBoundary (me);
		if (! selectedLeftBoundary) return Melder_error1 (L"To move a boundary, first click on it.");
		left = tier -> intervals -> item [selectedLeftBoundary - 1];
		right = tier -> intervals -> item [selectedLeftBoundary];
		position = where == 1 ? my startSelection : where == 2 ? my endSelection :
			Sound_getNearestZeroCrossing (my sound.data, left -> xmax, 1);   // STEREO BUG
		if (position == NUMundefined) return 1;
		if (position <= left -> xmin || position >= right -> xmax)
			{ Melder_beep (); return 0; }

		Editor_save (TextGridEditor_as_Editor (me), boundarySaveText [where]);

		left -> xmax = right -> xmin = my startSelection = my endSelection = position;
	} else {
		TextTier tier = (TextTier) anyTier;
		static wchar_t *pointSaveText [3] = { L"Move point to zero crossing", L"Move point to B", L"Move point to E" };
		TextPoint point;
		long selectedPoint = getSelectedPoint (me);
		if (! selectedPoint) return Melder_error1 (L"To move a point, first click on it.");
		point = tier -> points -> item [selectedPoint];
		position = where == 1 ? my startSelection : where == 2 ? my endSelection :
			Sound_getNearestZeroCrossing (my sound.data, point -> time, 1);   // STEREO BUG
		if (position == NUMundefined) return 1;

		Editor_save (TextGridEditor_as_Editor (me), pointSaveText [where]);

		point -> time = my startSelection = my endSelection = position;
	}
	FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));   /* Because cursor has moved. */
	Editor_broadcastChange (TextGridEditor_as_Editor (me));
	return 1;
}

static int menu_cb_MoveToB (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (! do_movePointOrBoundary (me, 1)) return 0;
	return 1;
}

static int menu_cb_MoveToE (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (! do_movePointOrBoundary (me, 2)) return 0;
	return 1;
}

static int menu_cb_MoveToZero (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (! do_movePointOrBoundary (me, 0)) return 0;
	return 1;
}

static void do_insertOnTier (TextGridEditor me, int itier) {
	if (! insertBoundaryOrPoint (me, itier,
		my playingCursor || my playingSelection ? my playCursor : my startSelection,
		my playingCursor || my playingSelection ? my playCursor : my endSelection,
		false)) return;
	my selectedTier = itier;
	FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));
	Editor_broadcastChange (TextGridEditor_as_Editor (me));
}

static int menu_cb_InsertOnSelectedTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_insertOnTier (me, my selectedTier);
	return 1;
}

static int menu_cb_InsertOnTier1 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 1); return 1; }
static int menu_cb_InsertOnTier2 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 2); return 1; }
static int menu_cb_InsertOnTier3 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 3); return 1; }
static int menu_cb_InsertOnTier4 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 4); return 1; }
static int menu_cb_InsertOnTier5 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 5); return 1; }
static int menu_cb_InsertOnTier6 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 6); return 1; }
static int menu_cb_InsertOnTier7 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 7); return 1; }
static int menu_cb_InsertOnTier8 (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); do_insertOnTier (me, 8); return 1; }

static int menu_cb_InsertOnAllTiers (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = my data;
	int saveTier = my selectedTier, itier;
	for (itier = 1; itier <= grid -> tiers -> size; itier ++) {
		do_insertOnTier (me, itier);
	}
	my selectedTier = saveTier;
	return 1;
}

/***** SEARCH MENU *****/

static int findInTier (TextGridEditor me) {
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, L"find a text")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals -> size) {
			TextInterval interval = tier -> intervals -> item [iinterval];
			wchar_t *text = interval -> text;
			if (text) {
				wchar_t *position = wcsstr (text, my findString);
				if (position) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					scrollToView (me, my startSelection);
					GuiText_setSelection (my text, position - text, position - text + wcslen (my findString));
					return 1;
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
			TextPoint point = tier -> points -> item [ipoint];
			wchar_t *text = point -> mark;
			if (text) {
				wchar_t *position = wcsstr (text, my findString);
				if (position) {
					my startSelection = my endSelection = point -> time;
					scrollToView (me, point -> time);
					GuiText_setSelection (my text, position - text, position - text + wcslen (my findString));
					return 1;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points -> size)
			Melder_beep ();
	}
	return 1;
}

static void do_find (TextGridEditor me) {
	if (my findString) {
		long left, right;
		wchar_t *label = GuiText_getStringAndSelectionPosition (my text, & left, & right);
		wchar_t *position = wcsstr (label + right, my findString);   /* CRLF BUG? */
		if (position) {
			GuiText_setSelection (my text, position - label, position - label + wcslen (my findString));
		} else {
			if (! findInTier (me)) Melder_flushError (NULL);
		}
		Melder_free (label);
	}
}

static int menu_cb_Find (EDITOR_ARGS) {
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

static int menu_cb_FindAgain (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	do_find (me);
	return 1;
}

static int checkSpellingInTier (TextGridEditor me) {
	TextGrid grid = my data;
	Data anyTier;
	if (! checkTierSelection (me, L"check spelling")) return 0;
	anyTier = grid -> tiers -> item [my selectedTier];
	if (anyTier -> methods == (Data_Table) classIntervalTier) {
		IntervalTier tier = (IntervalTier) anyTier;
		long iinterval = IntervalTier_timeToIndex (tier, my startSelection) + 1;
		while (iinterval <= tier -> intervals -> size) {
			TextInterval interval = tier -> intervals -> item [iinterval];
			wchar_t *text = interval -> text;
			if (text) {
				long position = 0;
				wchar_t *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = interval -> xmin;
					my endSelection = interval -> xmax;
					scrollToView (me, my startSelection);
					GuiText_setSelection (my text, position, position + wcslen (notAllowed));
					return 1;
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
			TextPoint point = tier -> points -> item [ipoint];
			wchar_t *text = point -> mark;
			if (text) {
				long position = 0;
				wchar_t *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, text, & position);
				if (notAllowed) {
					my startSelection = my endSelection = point -> time;
					scrollToView (me, point -> time);
					GuiText_setSelection (my text, position, position + wcslen (notAllowed));
					return 1;
				}
			}
			ipoint ++;
		}
		if (ipoint > tier -> points -> size)
			Melder_beep ();
	}
	return 1;
}

static int menu_cb_CheckSpelling (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my spellingChecker) {
		long left, right;
		wchar_t *label = GuiText_getStringAndSelectionPosition (my text, & left, & right);
		long position = right;
		wchar_t *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label, & position);
		if (notAllowed) {
			GuiText_setSelection (my text, position, position + wcslen (notAllowed));
		} else {
			if (! checkSpellingInTier (me)) Melder_flushError (NULL);
		}
		Melder_free (label);
	}
	return 1;
}

static int menu_cb_CheckSpellingInInterval (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my spellingChecker) {
		long left, right;
		wchar_t *label = GuiText_getStringAndSelectionPosition (my text, & left, & right);
		long position = right;
		wchar_t *notAllowed = SpellingChecker_nextNotAllowedWord (my spellingChecker, label, & position);
		if (notAllowed) {
			GuiText_setSelection (my text, position, position + wcslen (notAllowed));
		}
		Melder_free (label);
	}
	return 1;
}

static int menu_cb_AddToUserDictionary (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	if (my spellingChecker) {
		wchar_t *word = GuiText_getSelection (my text);
		SpellingChecker_addNewWord (my spellingChecker, word);
		Melder_free (word);
		iferror return 0;
		if (my dataChangedCallback)
			my dataChangedCallback (me, my dataChangedClosure, my spellingChecker);
	}
	return 1;
}

/***** TIER MENU *****/

static int menu_cb_RenameTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Rename tier", 0)
		SENTENCE (L"Name", L"");
	EDITOR_OK
		TextGrid grid = my data;
		Data tier;
		if (! checkTierSelection (me, L"rename a tier")) return 0;
		tier = grid -> tiers -> item [my selectedTier];
		SET_STRING (L"Name", tier -> name ? tier -> name : L"")
	EDITOR_DO
		wchar_t *newName = GET_STRING (L"Name");
		TextGrid grid = my data;
		Data tier;
		if (! checkTierSelection (me, L"rename a tier")) return 0;
		tier = grid -> tiers -> item [my selectedTier];

		Editor_save (TextGridEditor_as_Editor (me), L"Rename tier");

		Thing_setName (tier, newName);

		FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
		Editor_broadcastChange (TextGridEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_PublishTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid publish = NULL;
//start:
	TextGrid grid = my data;
	checkTierSelection (me, L"publish a tier"); cherror
	if (my publishCallback) {
		Data tier = grid -> tiers -> item [my selectedTier];
		publish = TextGrid_createWithoutTiers (1e30, -1e30); cherror
		TextGrid_add (publish, tier); cherror
		Thing_setName (publish, tier -> name); cherror
		my publishCallback (me, my publishClosure, publish);
	}
end:
	iferror return 0;
	return 1;
}

static int menu_cb_RemoveAllTextFromTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = my data;
	IntervalTier intervalTier;
	TextTier textTier;
	if (! checkTierSelection (me, L"remove all text from a tier")) return 0;
	_AnyTier_identifyClass (grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);

	Editor_save (TextGridEditor_as_Editor (me), L"Remove text from tier");
	if (intervalTier) {
		IntervalTier_removeText (intervalTier);
	} else {
		TextTier_removeText (textTier);
	}

	FunctionEditor_updateText (TextGridEditor_as_FunctionEditor (me));
	FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
	Editor_broadcastChange (TextGridEditor_as_Editor (me));
	return 1;
}

static int menu_cb_RemoveTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	TextGrid grid = my data;
	if (grid -> tiers -> size <= 1) {
		return Melder_error1 (L"Sorry, I refuse to remove the last tier.");
	}
	if (! checkTierSelection (me, L"remove a tier")) return 0;

	Editor_save (TextGridEditor_as_Editor (me), L"Remove tier");
	Collection_removeItem (grid -> tiers, my selectedTier);

	my selectedTier = 1;
	FunctionEditor_updateText (TextGridEditor_as_FunctionEditor (me));
	FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
	Editor_broadcastChange (TextGridEditor_as_Editor (me));
	return 1;
}

static int menu_cb_AddIntervalTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Add interval tier", 0)
		NATURAL (L"Position", L"1 (= at top)")
		SENTENCE (L"Name", L"")
	EDITOR_OK
		TextGrid grid = my data;
		static MelderString text = { 0 };
		MelderString_empty (& text);
		MelderString_append2 (& text, Melder_integer (grid -> tiers -> size + 1), L" (= at bottom)");
		SET_STRING (L"Position", text.string)
		SET_STRING (L"Name", L"")
	EDITOR_DO
		TextGrid grid = my data;
		int position = GET_INTEGER (L"Position");
		wchar_t *name = GET_STRING (L"Name");
		IntervalTier tier = IntervalTier_create (grid -> xmin, grid -> xmax);
		if (! tier) return 0;
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (tier, name);

		Editor_save (TextGridEditor_as_Editor (me), L"Add interval tier");
		Ordered_addItemPos (grid -> tiers, tier, position);

		my selectedTier = position;
		FunctionEditor_updateText (TextGridEditor_as_FunctionEditor (me));
		FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
		Editor_broadcastChange (TextGridEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_AddPointTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Add point tier", 0)
		NATURAL (L"Position", L"1 (= at top)")
		SENTENCE (L"Name", L"");
	EDITOR_OK
		TextGrid grid = my data;
		static MelderString text = { 0 };
		MelderString_empty (& text);
		MelderString_append2 (& text, Melder_integer (grid -> tiers -> size + 1), L" (= at bottom)");
		SET_STRING (L"Position", text.string)
		SET_STRING (L"Name", L"")
	EDITOR_DO
		TextGrid grid = my data;
		int position = GET_INTEGER (L"Position");
		wchar_t *name = GET_STRING (L"Name");
		TextTier tier = TextTier_create (grid -> xmin, grid -> xmax);
		if (! tier) return 0;
		if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (tier, name);

		Editor_save (TextGridEditor_as_Editor (me), L"Add point tier");
		Ordered_addItemPos (grid -> tiers, tier, position);

		my selectedTier = position;
		FunctionEditor_updateText (TextGridEditor_as_FunctionEditor (me));
		FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
		Editor_broadcastChange (TextGridEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_DuplicateTier (EDITOR_ARGS) {
	EDITOR_IAM (TextGridEditor);
	EDITOR_FORM (L"Duplicate tier", 0)
		NATURAL (L"Position", L"1 (= at top)")
		SENTENCE (L"Name", L"")
	EDITOR_OK
		TextGrid grid = my data;
		if (my selectedTier) {
			SET_STRING (L"Position", Melder_integer (my selectedTier + 1))
			SET_STRING (L"Name", ((AnyTier) grid -> tiers -> item [my selectedTier]) -> name)
		}
	EDITOR_DO
		TextGrid grid = my data;
		int position = GET_INTEGER (L"Position");
		wchar_t *name = GET_STRING (L"Name");
		AnyTier tier, newTier;
		if (! checkTierSelection (me, L"duplicate a tier")) return 0;
		tier = grid -> tiers -> item [my selectedTier];
		newTier = Data_copy (tier);
		if (! newTier) return 0;
			if (position > grid -> tiers -> size) position = grid -> tiers -> size + 1;
		Thing_setName (newTier, name);

		Editor_save (TextGridEditor_as_Editor (me), L"Duplicate tier");
		Ordered_addItemPos (grid -> tiers, newTier, position);

		my selectedTier = position;
		FunctionEditor_updateText (TextGridEditor_as_FunctionEditor (me));
		FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
		Editor_broadcastChange (TextGridEditor_as_Editor (me));
	EDITOR_END
}

/***** HELP MENU *****/

static int menu_cb_TextGridEditorHelp (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); Melder_help (L"TextGridEditor"); return 1; }
static int menu_cb_AboutSpecialSymbols (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); Melder_help (L"Special symbols"); return 1; }
static int menu_cb_PhoneticSymbols (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); Melder_help (L"Phonetic symbols"); return 1; }
static int menu_cb_AboutTextStyles (EDITOR_ARGS) { EDITOR_IAM (TextGridEditor); Melder_help (L"Text styles"); return 1; }

static void createMenus (TextGridEditor me) {
	EditorMenu menu;
	inherited (TextGridEditor) createMenus (TextGridEditor_as_parent (me));

	#ifndef macintosh
		Editor_addCommand (me, L"Edit", L"-- cut copy paste --", 0, NULL);
		Editor_addCommand (me, L"Edit", L"Cut text", 'X', menu_cb_Cut);
		Editor_addCommand (me, L"Edit", L"Cut", Editor_HIDDEN, menu_cb_Cut);
		Editor_addCommand (me, L"Edit", L"Copy text", 'C', menu_cb_Copy);
		Editor_addCommand (me, L"Edit", L"Copy", Editor_HIDDEN, menu_cb_Copy);
		Editor_addCommand (me, L"Edit", L"Paste text", 'V', menu_cb_Paste);
		Editor_addCommand (me, L"Edit", L"Paste", Editor_HIDDEN, menu_cb_Paste);
		Editor_addCommand (me, L"Edit", L"Erase text", 0, menu_cb_Erase);
		Editor_addCommand (me, L"Edit", L"Erase", Editor_HIDDEN, menu_cb_Erase);
	#endif
	Editor_addCommand (me, L"Edit", L"-- encoding --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Convert entire TextGrid to backslash trigraphs", 0, menu_cb_Genericize);
	Editor_addCommand (me, L"Edit", L"Genericize entire TextGrid", Editor_HIDDEN, menu_cb_Genericize);
	Editor_addCommand (me, L"Edit", L"Genericize", Editor_HIDDEN, menu_cb_Genericize);
	Editor_addCommand (me, L"Edit", L"Convert entire TextGrid to Unicode", 0, menu_cb_Nativize);
	Editor_addCommand (me, L"Edit", L"Nativize entire TextGrid", Editor_HIDDEN, menu_cb_Nativize);
	Editor_addCommand (me, L"Edit", L"Nativize", Editor_HIDDEN, menu_cb_Nativize);
	Editor_addCommand (me, L"Edit", L"-- search --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Find...", 'F', menu_cb_Find);
	Editor_addCommand (me, L"Edit", L"Find again", 'G', menu_cb_FindAgain);

	if (my sound.data) {
		Editor_addCommand (me, L"Select", L"-- move to zero --", 0, 0);
		Editor_addCommand (me, L"Select", L"Move start of selection to nearest zero crossing", ',', menu_cb_MoveBtoZero);
		Editor_addCommand (me, L"Select", L"Move begin of selection to nearest zero crossing", Editor_HIDDEN, menu_cb_MoveBtoZero);
		Editor_addCommand (me, L"Select", L"Move cursor to nearest zero crossing", '0', menu_cb_MoveCursorToZero);
		Editor_addCommand (me, L"Select", L"Move end of selection to nearest zero crossing", '.', menu_cb_MoveEtoZero);
	}

	Editor_addCommand (me, L"Query", L"-- query interval --", 0, NULL);
	Editor_addCommand (me, L"Query", L"Get starting point of interval", 0, menu_cb_GetStartingPointOfInterval);
	Editor_addCommand (me, L"Query", L"Get end point of interval", 0, menu_cb_GetEndPointOfInterval);
	Editor_addCommand (me, L"Query", L"Get label of interval", 0, menu_cb_GetLabelOfInterval);

	menu = Editor_addMenu (me, L"Interval", 0);
	EditorMenu_addCommand (menu, L"Add interval on tier 1", GuiMenu_COMMAND | '1', menu_cb_InsertIntervalOnTier1);
	EditorMenu_addCommand (menu, L"Add interval on tier 2", GuiMenu_COMMAND | '2', menu_cb_InsertIntervalOnTier2);
	EditorMenu_addCommand (menu, L"Add interval on tier 3", GuiMenu_COMMAND | '3', menu_cb_InsertIntervalOnTier3);
	EditorMenu_addCommand (menu, L"Add interval on tier 4", GuiMenu_COMMAND | '4', menu_cb_InsertIntervalOnTier4);
	EditorMenu_addCommand (menu, L"Add interval on tier 5", GuiMenu_COMMAND | '5', menu_cb_InsertIntervalOnTier5);
	EditorMenu_addCommand (menu, L"Add interval on tier 6", GuiMenu_COMMAND | '6', menu_cb_InsertIntervalOnTier6);
	EditorMenu_addCommand (menu, L"Add interval on tier 7", GuiMenu_COMMAND | '7', menu_cb_InsertIntervalOnTier7);
	EditorMenu_addCommand (menu, L"Add interval on tier 8", GuiMenu_COMMAND | '8', menu_cb_InsertIntervalOnTier8);

	menu = Editor_addMenu (me, L"Boundary", 0);
	/*EditorMenu_addCommand (menu, L"Move to B", 0, menu_cb_MoveToB);
	EditorMenu_addCommand (menu, L"Move to E", 0, menu_cb_MoveToE);*/
	if (my sound.data)
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

	menu = Editor_addMenu (me, L"Tier", 0);
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

	if (my spellingChecker) {
		menu = Editor_addMenu (me, L"Spell", 0);
		EditorMenu_addCommand (menu, L"Check spelling in tier", GuiMenu_COMMAND | GuiMenu_OPTION | 'L', menu_cb_CheckSpelling);
		EditorMenu_addCommand (menu, L"Check spelling in interval", 0, menu_cb_CheckSpellingInInterval);
		EditorMenu_addCommand (menu, L"-- edit lexicon --", 0, NULL);
		EditorMenu_addCommand (menu, L"Add selected word to user dictionary", 0, menu_cb_AddToUserDictionary);
	}

	if (my sound.data || my longSound.data) {
		our createMenus_analysis (me);   // Insert some of the ancestor's menus *after* the TextGrid menus.
	}
}

static void createHelpMenuItems (TextGridEditor me, EditorMenu menu) {
	inherited (TextGridEditor) createHelpMenuItems (TextGridEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"TextGridEditor help", '?', menu_cb_TextGridEditorHelp);
	EditorMenu_addCommand (menu, L"About special symbols", 0, menu_cb_AboutSpecialSymbols);
	EditorMenu_addCommand (menu, L"Phonetic symbols", 0, menu_cb_PhoneticSymbols);
	EditorMenu_addCommand (menu, L"About text styles", 0, menu_cb_AboutTextStyles);
}

/***** CHILDREN *****/

static void gui_text_cb_change (I, GuiTextEvent event) {
	iam (TextGridEditor);
	(void) event;
	TextGrid grid = my data;
	if (my suppressRedraw) return;   /* Prevent infinite loop if 'draw' method calls GuiText_setString. */
	if (my selectedTier) {
		wchar_t *text = GuiText_getString (my text);
		IntervalTier intervalTier;
		TextTier textTier;
		_AnyTier_identifyClass (grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			long selectedInterval = getSelectedInterval (me);
			if (selectedInterval) {
				TextInterval interval = intervalTier -> intervals -> item [selectedInterval];
				TextInterval_setText (interval, text);
				FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
				Editor_broadcastChange (TextGridEditor_as_Editor (me));
			}
		} else {
			long selectedPoint = getSelectedPoint (me);
			if (selectedPoint) {
				TextPoint point = textTier -> points -> item [selectedPoint];
				Melder_free (point -> mark);
				if (wcsspn (text, L" \n\t") != wcslen (text))   /* Any visible characters? */
				point -> mark = Melder_wcsdup_f (text);
				FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
				Editor_broadcastChange (TextGridEditor_as_Editor (me));
			}
		}
		Melder_free (text);
	}
}

static void createChildren (TextGridEditor me) {
	inherited (TextGridEditor) createChildren (TextGridEditor_as_parent (me));
	GuiText_setChangeCallback (my text, gui_text_cb_change, me);
}

static void dataChanged (TextGridEditor me) {
	TextGrid grid = my data;
	/*
	 * Perform a minimal selection change.
	 * Most changes will involve intervals and boundaries; however, there may also be tier removals.
	 * Do a simple guess.
	 */
	if (grid -> tiers -> size < my selectedTier) {
		my selectedTier = grid -> tiers -> size;
	}
	inherited (TextGridEditor) dataChanged (TextGridEditor_as_parent (me));   /* Does all the updating. */
}

/********** DRAWING AREA **********/

static void prepareDraw (TextGridEditor me) {
	if (my longSound.data) {
		LongSound_haveWindow (my longSound.data, my startWindow, my endWindow);
		Melder_clearError ();
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
		TextInterval interval = tier -> intervals -> item [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		if (tmax > my startWindow && tmin < my endWindow) {   /* Interval visible? */
			int selected = iinterval == selectedInterval;
			int labelMatches = Melder_stringMatchesCriterion (interval -> text, my greenMethod, my greenString);
			if (tmin < my startWindow) tmin = my startWindow;
			if (tmax > my endWindow) tmax = my endWindow;
			if (labelMatches) {
				Graphics_setColour (my graphics, Graphics_LIME);
				Graphics_fillRectangle (my graphics, tmin, tmax, 0.0, 1.0);
			}
			if (selected) {
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
			TextInterval interval = tier -> intervals -> item [iinterval];
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
		TextInterval interval = tier -> intervals -> item [iinterval];
		double tmin = interval -> xmin, tmax = interval -> xmax;
		int selected;
		if (tmin < my tmin) tmin = my tmin; if (tmax > my tmax) tmax = my tmax;
		if (tmin >= tmax) continue;
		selected = selectedInterval == iinterval;

		/*
		 * Draw left boundary.
		 */
		if (tmin >= my startWindow && tmin <= my endWindow && iinterval > 1) {
			int selected = ( my selectedTier == itier && tmin == my startSelection );
			Graphics_setColour (my graphics, selected ? Graphics_RED : Graphics_BLUE);
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
			Graphics_setColour (my graphics, selected ? Graphics_RED : Graphics_BLACK);
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
			TextPoint point = tier -> points -> item [ipoint];
			if (point -> time == my startSelection) cursorAtPoint = TRUE;
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
		TextPoint point = tier -> points -> item [ipoint];
		double t = point -> time;
		if (t >= my startWindow && t <= my endWindow) {
			int selected = ( itier == my selectedTier && t == my startSelection );
			Graphics_setColour (my graphics, selected ? Graphics_RED : Graphics_BLUE);
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
			Graphics_setColour (my graphics, selected ? Graphics_RED : Graphics_BLUE);
			if (point -> mark) Graphics_text (my graphics, t, 0.5, point -> mark);
		}
	}
	Graphics_setPercentSignIsItalic (my graphics, TRUE);
	Graphics_setNumberSignIsBold (my graphics, TRUE);
	Graphics_setCircumflexIsSuperscript (my graphics, TRUE);
	Graphics_setUnderscoreIsSubscript (my graphics, TRUE);
}

static void draw (TextGridEditor me) {
	TextGrid grid = my data;
	Graphics_Viewport vp1, vp2;
	long itier, ntier = grid -> tiers -> size;
	enum kGraphics_font oldFont = Graphics_inqFont (my graphics);
	int oldFontSize = Graphics_inqFontSize (my graphics);
	int showAnalysis = (my spectrogram.show || my pitch.show || my intensity.show || my formant.show) && (my longSound.data || my sound.data);
	double soundY = _TextGridEditor_computeSoundY (me), soundY2 = showAnalysis ? 0.5 * (1.0 + soundY) : soundY;

	/*
	 * Draw optional sound.
	 */
	if (my longSound.data || my sound.data) {
		vp1 = Graphics_insetViewport (my graphics, 0.0, 1.0, soundY2, 1.0);
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
		TimeSoundEditor_draw_sound (TextGridEditor_as_TimeSoundEditor (me), -1.0, 1.0);
		Graphics_flushWs (my graphics);
		Graphics_resetViewport (my graphics, vp1);
	}

	/*
	 * Draw tiers.
	 */
	if (my longSound.data || my sound.data) vp1 = Graphics_insetViewport (my graphics, 0.0, 1.0, 0.0, soundY);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0.0, 1.0);
	for (itier = 1; itier <= ntier; itier ++) {
		Data anyTier = grid -> tiers -> item [itier];
		int selected = itier == my selectedTier;
		int isIntervalTier = anyTier -> methods == (Data_Table) classIntervalTier;
		vp2 = Graphics_insetViewport (my graphics, 0.0, 1.0,
			1.0 - (double) itier / (double) ntier,
			1.0 - (double) (itier - 1) / (double) ntier);
		Graphics_setColour (my graphics, Graphics_BLACK);
		if (itier != 1) Graphics_line (my graphics, my startWindow, 1.0, my endWindow, 1.0);

		/*
		 * Show the number and the name of the tier.
		 */
		Graphics_setColour (my graphics, selected ? Graphics_RED : Graphics_BLACK);
		Graphics_setFont (my graphics, oldFont);
		Graphics_setFontSize (my graphics, 14);
		Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
		Graphics_text2 (my graphics, my startWindow, 0.5, selected ? L"\\pf " : L"", Melder_integer (itier));
		Graphics_setFontSize (my graphics, oldFontSize);
		if (anyTier -> name && anyTier -> name [0]) {
			Graphics_setTextAlignment (my graphics, Graphics_LEFT,
				my showNumberOf == kTextGridEditor_showNumberOf_NOTHING ? Graphics_HALF : Graphics_BOTTOM);
			Graphics_text (my graphics, my endWindow, 0.5, anyTier -> name);
		}
		if (my showNumberOf != kTextGridEditor_showNumberOf_NOTHING) {
			Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_TOP);
			if (my showNumberOf == kTextGridEditor_showNumberOf_INTERVALS_OR_POINTS) {
				long count = isIntervalTier ? ((IntervalTier) anyTier) -> intervals -> size : ((TextTier) anyTier) -> points -> size;
				long position = itier == my selectedTier ? ( isIntervalTier ? getSelectedInterval (me) : getSelectedPoint (me) ) : 0;
				if (position) {
					Graphics_text5 (my graphics, my endWindow, 0.5, L"(", Melder_integer (position), L"/", Melder_integer (count), L")");
				} else {
					Graphics_text3 (my graphics, my endWindow, 0.5, L"(", Melder_integer (count), L")");
				}
			} else {
				Melder_assert (kTextGridEditor_showNumberOf_NONEMPTY_INTERVALS_OR_POINTS);
				long count = 0;
				if (isIntervalTier) {
					IntervalTier tier = (IntervalTier) anyTier;
					long ninterval = tier -> intervals -> size, iinterval;
					for (iinterval = 1; iinterval <= ninterval; iinterval ++) {
						TextInterval interval = tier -> intervals -> item [iinterval];
						if (interval -> text != NULL && interval -> text [0] != '\0') {
							count ++;
						}
					}
				} else {
					TextTier tier = (TextTier) anyTier;
					long npoint = tier -> points -> size, ipoint;
					for (ipoint = 1; ipoint <= npoint; ipoint ++) {
						TextPoint point = tier -> points -> item [ipoint];
						if (point -> mark != NULL && point -> mark [0] != '\0') {
							count ++;
						}
					}
				}
				Graphics_text3 (my graphics, my endWindow, 0.5, L"(##", Melder_integer (count), L"#)");
			}
		}

		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_setFont (my graphics, kGraphics_font_TIMES);
		Graphics_setFontSize (my graphics, my fontSize);
		if (isIntervalTier)
			do_drawIntervalTier (me, (IntervalTier) anyTier, itier);
		else
			do_drawTextTier (me, (TextTier) anyTier, itier);
		Graphics_resetViewport (my graphics, vp2);
	}
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_setFont (my graphics, oldFont);
	Graphics_setFontSize (my graphics, oldFontSize);
	if (my longSound.data || my sound.data) Graphics_resetViewport (my graphics, vp1);
	Graphics_flushWs (my graphics);

	if (showAnalysis) {
		vp1 = Graphics_insetViewport (my graphics, 0.0, 1.0, soundY, soundY2);
		our draw_analysis (me);
		Graphics_flushWs (my graphics);
		Graphics_resetViewport (my graphics, vp1);
		/* Draw pulses. */
		if (my pulses.show) {
			vp1 = Graphics_insetViewport (my graphics, 0.0, 1.0, soundY2, 1.0);
			our draw_analysis_pulses (me);
			TimeSoundEditor_draw_sound (TextGridEditor_as_TimeSoundEditor (me), -1.0, 1.0);   /* Second time, partially across the pulses. */
			Graphics_flushWs (my graphics);
			Graphics_resetViewport (my graphics, vp1);
		}
	}
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0.0, 1.0);
	if (my longSound.data || my sound.data) {
		Graphics_line (my graphics, my startWindow, soundY, my endWindow, soundY);
		if (showAnalysis) {
			Graphics_line (my graphics, my startWindow, soundY2, my endWindow, soundY2);
			Graphics_line (my graphics, my startWindow, soundY, my startWindow, soundY2);
			Graphics_line (my graphics, my endWindow, soundY, my endWindow, soundY2);
		}
	}

	/*
	 * Finally, us usual, update the menus.
	 */
	our updateMenuItems_file (me);
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
	TextGrid grid = my data;
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
			_AnyTier_identifyClass (grid -> tiers -> item [itier], & intervalTier, & textTier);
			if (intervalTier) {
				long ibound = IntervalTier_hasBoundary (intervalTier, xbegin);
				if (ibound) {
					TextInterval leftInterval = intervalTier -> intervals -> item [ibound - 1];
					TextInterval rightInterval = intervalTier -> intervals -> item [ibound];
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

	Graphics_xorOn (my graphics, Graphics_MAGENTA);
	Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_BOTTOM);
	do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);
	while (Graphics_mouseStillDown (my graphics)) {
		do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);
		Graphics_getMouseLocation (my graphics, & xWC, & yWC);
		do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);
	}
	do_drawWhileDragging (me, numberOfTiers, selectedTier, xWC, soundY);
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
	if (yWC > 0.0 && yWC < soundY && ! selectedTier [itierDrop]) {   /* Dropped inside an unselected tier? */
		Data anyTierDrop = grid -> tiers -> item [itierDrop];
		if (anyTierDrop -> methods == (Data_Table) classIntervalTier) {
			IntervalTier tierDrop = (IntervalTier) anyTierDrop;
			long ibound;
			for (ibound = 1; ibound < tierDrop -> intervals -> size; ibound ++) {
				TextInterval left = tierDrop -> intervals -> item [ibound];
				if (fabs (Graphics_dxWCtoMM (my graphics, xWC - left -> xmax)) < 1.5) {   /* Near a boundary? */
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
				TextPoint point = tierDrop -> points -> item [ipoint];
				if (fabs (Graphics_dxWCtoMM (my graphics, xWC - point -> time)) < 1.5) {   /* Near a point? */
					/*
					 * Snap to point.
					 */
					xWC = point -> time;
				}
			}
		}
	} else if (xbegin != my startSelection && fabs (Graphics_dxWCtoMM (my graphics, xWC - my startSelection)) < 1.5) {   /* Near the cursor? */
		/*
		 * Snap to cursor.
		 */
		xWC = my startSelection;
	} else if (xbegin != my endSelection && fabs (Graphics_dxWCtoMM (my graphics, xWC - my endSelection)) < 1.5) {   /* Near the cursor? */
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

	Editor_save (TextGridEditor_as_Editor (me), L"Drag");

	for (itier = 1; itier <= numberOfTiers; itier ++) if (selectedTier [itier]) {
		IntervalTier intervalTier;
		TextTier textTier;
		_AnyTier_identifyClass (grid -> tiers -> item [itier], & intervalTier, & textTier);
		if (intervalTier) {
			long ibound, numberOfIntervals = intervalTier -> intervals -> size;
			Any *intervals = intervalTier -> intervals -> item;
			for (ibound = 2; ibound <= numberOfIntervals; ibound ++) {
				TextInterval left = intervals [ibound - 1], right = intervals [ibound];
				if (left -> xmax == xbegin) {   /* Boundary dragged? */
					left -> xmax = right -> xmin = xWC;   /* Move boundary to drop site. */
					break;
				}
			}
		} else {
			long iDraggedPoint = AnyTier_hasPoint (textTier, xbegin);
			if (iDraggedPoint) {
				long dropSiteHasPoint = AnyTier_hasPoint (textTier, xWC);
				if (dropSiteHasPoint) {
					Melder_warning1 (L"Cannot drop point on an existing point.");
				} else {
					TextPoint point = textTier -> points -> item [iDraggedPoint];
					/*
					 * Move point to drop site. May have passed another point.
					 */
					TextPoint newPoint = Data_copy (point);
					newPoint -> time = xWC;   /* Move point to drop site. */
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
	FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));
	Editor_broadcastChange (TextGridEditor_as_Editor (me));
}

static int click (TextGridEditor me, double xclick, double yWC, int shiftKeyPressed) {
	TextGrid grid = my data;
	double tmin, tmax, x, y;
	long ntiers = grid -> tiers -> size, iClickedTier, iClickedInterval, iClickedPoint;
	int clickedLeftBoundary = 0, nearBoundaryOrPoint, nearCursorCircle, drag = FALSE;
	IntervalTier intervalTier;
	TextTier textTier;
	TextInterval interval = NULL;
	TextPoint point = NULL;
	double soundY = _TextGridEditor_computeSoundY (me);
	double tnear;

	/*
	 * In answer to a click in the sound part,
	 * we keep the same tier selected and move the cursor or drag the "yellow" selection.
	 */
	if (yWC > soundY) {   /* Clicked in sound part? */
		if ((my spectrogram.show || my formant.show) && yWC < 0.5 * (soundY + 1.0)) {
			my spectrogram.cursor = my spectrogram.viewFrom +
				2.0 * (yWC - soundY) / (1.0 - soundY) * (my spectrogram.viewTo - my spectrogram.viewFrom);
		}
		inherited (TextGridEditor) click (TextGridEditor_as_parent (me), xclick, yWC, shiftKeyPressed);
		return FunctionEditor_UPDATE_NEEDED;
	}

	if (xclick <= my startWindow || xclick >= my endWindow) {
		return FunctionEditor_NO_UPDATE_NEEDED;
	}

	/*
	 * She clicked in the grid part.
	 * We select the tier in which she clicked.
	 */
	iClickedTier = _TextGridEditor_yWCtoTier (me, yWC);
	_TextGridEditor_timeToInterval (me, xclick, iClickedTier, & tmin, & tmax);
	_AnyTier_identifyClass (grid -> tiers -> item [iClickedTier], & intervalTier, & textTier);

	/*
	 * Get the time of the nearest boundary or point.
	 */
	tnear = NUMundefined;
	if (intervalTier) {
		iClickedInterval = IntervalTier_timeToIndex (intervalTier, xclick);
		if (iClickedInterval) {
			interval = intervalTier -> intervals -> item [iClickedInterval];
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
			my selectedTier = iClickedTier;
			return FunctionEditor_UPDATE_NEEDED;
		}
	} else {
		iClickedPoint = AnyTier_timeToNearestIndex (textTier, xclick);
		if (iClickedPoint) {
			point = textTier -> points -> item [iClickedPoint];
			tnear = point -> time;
		}
	}
	Melder_assert (! (intervalTier && ! clickedLeftBoundary));

	/*
	 * Where did she click?
	 */
	nearBoundaryOrPoint = tnear != NUMundefined && fabs (Graphics_dxWCtoMM (my graphics, xclick - tnear)) < 1.5;
	nearCursorCircle = my startSelection == my endSelection && Graphics_distanceWCtoMM (my graphics, xclick, yWC,
		my startSelection, (ntiers + 1 - iClickedTier) * soundY / ntiers - Graphics_dyMMtoWC (my graphics, 1.5)) < 1.5;

	/*
	 * Find out whether this is a click or a drag.
	 */
	while (Graphics_mouseStillDown (my graphics)) {
		Graphics_getMouseLocation (my graphics, & x, & y);
		if (x < my startWindow) x = my startWindow;
		if (x > my endWindow) x = my endWindow;
		if (fabs (Graphics_dxWCtoMM (my graphics, x - xclick)) > 1.5) {
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
			my selectedTier = iClickedTier;
		} else if (drag) {
			/*
			 * The tier that has been clicked becomes the new selected tier.
			 * This has to be done before the next Update, i.e. also before do_dragBoundary!
			 */
			my selectedTier = iClickedTier;
			do_dragBoundary (me, tnear, iClickedTier, shiftKeyPressed);
			return FunctionEditor_NO_UPDATE_NEEDED;
		} else {
			/*
			 * If she clicked on an unselected boundary or point, we select it.
			 */
			if (shiftKeyPressed) {
				if (tnear > 0.5 * (my startSelection + my endSelection))
					my endSelection = tnear;
				else
					my startSelection = tnear;
			} else {
				my startSelection = my endSelection = tnear;   /* Move cursor so that the boundary or point is selected. */
			}
			my selectedTier = iClickedTier;
		}
	} else if (nearCursorCircle) {
		/*
		 * Possibility 2: she clicked near the cursor circle.
		 * Insert boundary or point. There is no danger that we insert on top of an existing boundary or point,
		 * because we are not 'nearBoundaryOrPoint'.
		 */
		insertBoundaryOrPoint (me, iClickedTier, my startSelection, my startSelection, false);
		my selectedTier = iClickedTier;
		FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));
		Editor_broadcastChange (TextGridEditor_as_Editor (me));
		if (drag) Graphics_waitMouseUp (my graphics);
		return FunctionEditor_NO_UPDATE_NEEDED;
	} else {
		/*
		 * Possibility 3: she clicked in empty space.
		 */
		if (intervalTier) {
			my startSelection = tmin;
			my endSelection = tmax;
		}
		my selectedTier = iClickedTier;
	}
	if (drag) Graphics_waitMouseUp (my graphics);
	return FunctionEditor_UPDATE_NEEDED;
}

static int clickB (TextGridEditor me, double t, double yWC) {
	int itier;
	double tmin, tmax;
	double soundY = _TextGridEditor_computeSoundY (me);

	if (yWC > soundY) {   /* Clicked in sound part? */
		my startSelection = t;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		return FunctionEditor_UPDATE_NEEDED;
	}
	itier = _TextGridEditor_yWCtoTier (me, yWC);
	_TextGridEditor_timeToInterval (me, t, itier, & tmin, & tmax);
	my startSelection = t - tmin < tmax - t ? tmin : tmax;   /* To nearest boundary. */
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

static int clickE (TextGridEditor me, double t, double yWC) {
	int itier;
	double tmin, tmax;
	double soundY = _TextGridEditor_computeSoundY (me);

	if (yWC > soundY) {   /* Clicked in sound part? */
		my endSelection = t;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		return FunctionEditor_UPDATE_NEEDED;
	}
	itier = _TextGridEditor_yWCtoTier (me, yWC);
	_TextGridEditor_timeToInterval (me, t, itier, & tmin, & tmax);
	my endSelection = t - tmin < tmax - t ? tmin : tmax;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

static void play (TextGridEditor me, double tmin, double tmax) {
	if (my longSound.data) {
		LongSound_playPart (my longSound.data, tmin, tmax, our playCallback, me);
	} else if (my sound.data) {
		Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
	}
}

static void updateText (TextGridEditor me) {
	TextGrid grid = my data;
	wchar_t *newText = L"";
	if (my selectedTier) {
		IntervalTier intervalTier;
		TextTier textTier;
		_AnyTier_identifyClass (grid -> tiers -> item [my selectedTier], & intervalTier, & textTier);
		if (intervalTier) {
			long iinterval = IntervalTier_timeToIndex (intervalTier, my startSelection);
			if (iinterval) {
				TextInterval interval = intervalTier -> intervals -> item [iinterval];
				if (interval -> text) {
					newText = interval -> text;
				}
			}
		} else {
			long ipoint = AnyTier_hasPoint (textTier, my startSelection);
			if (ipoint) {
				TextPoint point = textTier -> points -> item [ipoint];
				if (point -> mark) {
					newText = point -> mark;
				}
			}
		}
	}
	my suppressRedraw = TRUE;   /* Prevent valueChangedCallback from redrawing. */
	GuiText_setString (my text, newText);
	long cursor = wcslen (newText);   // at end
	GuiText_setSelection (my text, cursor, cursor);
	my suppressRedraw = FALSE;
}

static void prefs_addFields (TextGridEditor me, EditorCommand cmd) {
	(void) me;
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
static void prefs_setValues (TextGridEditor me, EditorCommand cmd) {
	SET_INTEGER (L"The symbols %#_^ in labels", my useTextStyles + 1)
	SET_INTEGER (L"Font size", my fontSize)
	SET_ENUM (L"Text alignment in intervals", kGraphics_horizontalAlignment, my alignment)
	SET_INTEGER (L"With the shift key, you drag", my shiftDragMultiple + 1)
	SET_ENUM (L"Show number of", kTextGridEditor_showNumberOf, my showNumberOf)
	SET_ENUM (L"Paint intervals green whose label...", kMelder_string, my greenMethod)
	SET_STRING (L"...the text", my greenString)
}
static void prefs_getValues (TextGridEditor me, EditorCommand cmd) {
	preferences.useTextStyles = my useTextStyles = GET_INTEGER (L"The symbols %#_^ in labels") - 1;
	preferences.fontSize = my fontSize = GET_INTEGER (L"Font size");
	preferences.alignment = my alignment = GET_ENUM (kGraphics_horizontalAlignment, L"Text alignment in intervals");
	preferences.shiftDragMultiple = my shiftDragMultiple = GET_INTEGER (L"With the shift key, you drag") - 1;
	preferences.showNumberOf = my showNumberOf = GET_ENUM (kTextGridEditor_showNumberOf, L"Show number of");
	preferences.greenMethod = my greenMethod = GET_ENUM (kMelder_string, L"Paint intervals green whose label...");
	wcsncpy (my greenString, GET_STRING (L"...the text"), Preferences_STRING_BUFFER_SIZE);
	my greenString [Preferences_STRING_BUFFER_SIZE - 1] = '\0';
	wcscpy (preferences.greenString, my greenString);
	FunctionEditor_redraw (TextGridEditor_as_FunctionEditor (me));
}

static void createMenuItems_view_timeDomain (TextGridEditor me, EditorMenu menu) {
	inherited (TextGridEditor) createMenuItems_view_timeDomain (TextGridEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"Select previous tier", GuiMenu_OPTION | GuiMenu_UP_ARROW, menu_cb_SelectPreviousTier);
	EditorMenu_addCommand (menu, L"Select next tier", GuiMenu_OPTION | GuiMenu_DOWN_ARROW, menu_cb_SelectNextTier);
	EditorMenu_addCommand (menu, L"Select previous interval", GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_SelectPreviousInterval);
	EditorMenu_addCommand (menu, L"Select next interval", GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_SelectNextInterval);
	EditorMenu_addCommand (menu, L"Extend-select left", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_LEFT_ARROW, menu_cb_ExtendSelectPreviousInterval);
	EditorMenu_addCommand (menu, L"Extend-select right", GuiMenu_SHIFT | GuiMenu_OPTION | GuiMenu_RIGHT_ARROW, menu_cb_ExtendSelectNextInterval);
}

static void highlightSelection (TextGridEditor me, double left, double right, double bottom, double top) {
	if (my spectrogram.show && (my longSound.data || my sound.data)) {
		TextGrid grid = my data;
		double soundY = grid -> tiers -> size / (2.0 + grid -> tiers -> size * 1.8), soundY2 = 0.5 * (1.0 + soundY);
		Graphics_highlight (my graphics, left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_highlight (my graphics, left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_highlight (my graphics, left, right, bottom, top);
	}
}

static void unhighlightSelection (TextGridEditor me, double left, double right, double bottom, double top) {
	if (my spectrogram.show) {
		TextGrid grid = my data;
		double soundY = grid -> tiers -> size / (2.0 + grid -> tiers -> size * 1.8), soundY2 = 0.5 * (1.0 + soundY);
		Graphics_unhighlight (my graphics, left, right, bottom, soundY * top + (1 - soundY) * bottom);
		Graphics_unhighlight (my graphics, left, right, soundY2 * top + (1 - soundY2) * bottom, top);
	} else {
		Graphics_unhighlight (my graphics, left, right, bottom, top);
	}
}

static double getBottomOfSoundAndAnalysisArea (TextGridEditor me) {
	return _TextGridEditor_computeSoundY (me);
}

static void createMenuItems_pitch_picture (TextGridEditor me, EditorMenu menu) {
	inherited (TextGridEditor) createMenuItems_pitch_picture (TextGridEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"Draw visible pitch contour and TextGrid...", 0, menu_cb_DrawTextGridAndPitch);
}

static void updateMenuItems_file (TextGridEditor me) {
	inherited (TextGridEditor) updateMenuItems_file (TextGridEditor_as_parent (me));
	GuiObject_setSensitive (my writeSelectedTextGridButton, my endSelection > my startSelection);
	GuiObject_setSensitive (my extractSelectedTextGridPreserveTimesButton, my endSelection > my startSelection);
	GuiObject_setSensitive (my extractSelectedTextGridTimeFromZeroButton, my endSelection > my startSelection);
}

class_methods (TextGridEditor, TimeSoundAnalysisEditor) {
	class_method (destroy)
	class_method (info)
	class_method (dataChanged)
	class_method (createChildren)
	class_method (createMenuItems_file_extract)
	class_method (createMenuItems_file_write)
	class_method (createMenuItems_file_draw)
	class_method (createMenus)
	class_method (createHelpMenuItems)
	class_method (prepareDraw)
	class_method (draw)
	us -> hasText = TRUE;
	class_method (click)
	class_method (clickB)
	class_method (clickE)
	/*class_method (key)*/   /* The key method will never be called, because the text widget receives the key presses. */
	class_method (play)
	class_method (updateText)
	class_method (prefs_addFields)
	class_method (prefs_setValues)
	class_method (prefs_getValues)
	class_method (createMenuItems_view_timeDomain)
	class_method (highlightSelection)
	class_method (unhighlightSelection)
	class_method (getBottomOfSoundAndAnalysisArea)
	class_method (createMenuItems_pitch_picture)
	class_method (updateMenuItems_file)
	class_methods_end
}

/********** EXPORTED **********/

TextGridEditor TextGridEditor_create (GuiObject parent, const wchar_t *title, TextGrid grid, Any sound, Any spellingChecker) {
	TextGridEditor me = new (TextGridEditor); cherror
	my spellingChecker = spellingChecker;   // Set in time.

	/*
	 * Include a deep copy of the Sound, owned by the TextGridEditor, or a pointer to the LongSound.
	 */
	if (sound && Thing_member (sound, classSound)) {
		TimeSoundAnalysisEditor_init (TextGridEditor_as_parent (me), parent, title, grid, sound, true); cherror
	} else {
		TimeSoundAnalysisEditor_init (TextGridEditor_as_parent (me), parent, title, grid, sound, false); cherror
	}

	my useTextStyles = preferences.useTextStyles;
	my fontSize = preferences.fontSize;
	my alignment = preferences.alignment;
	my shiftDragMultiple = preferences.shiftDragMultiple;
	my showNumberOf = preferences.showNumberOf;
	my greenMethod = preferences.greenMethod;
	wcscpy (my greenString, preferences.greenString);
	my selectedTier = 1;
	if (my endWindow - my startWindow > 30.0) {
		my endWindow = my startWindow + 30.0;
		if (my startWindow == my tmin)
			my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
		FunctionEditor_marksChanged (TextGridEditor_as_FunctionEditor (me));
	}
	if (spellingChecker != NULL)
		GuiText_setSelection (my text, 0, 0);
end:
	iferror forget (me);
	return me;
}

/* End of file TextGridEditor.c */
