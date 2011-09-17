/* FunctionEditor.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "FunctionEditor.h"
#include "machine.h"
#include "Preferences.h"
#include "EditorM.h"

Thing_implement (FunctionEditor, Editor, 0);

#define maximumScrollBarValue  2000000000
#define RELATIVE_PAGE_INCREMENT  0.8
#define SCROLL_INCREMENT_FRACTION  20
#define space 30
#define MARGIN 107
#define BOTTOM_MARGIN  2
#define TOP_MARGIN  3
#define TEXT_HEIGHT  50
#ifdef macintosh
	#define BUTTON_X  3
	#define BUTTON_WIDTH  40
	#define BUTTON_SPACING  8
#else
	#define BUTTON_X  1
	#define BUTTON_WIDTH  30
	#define BUTTON_SPACING  4
#endif

static struct {
	int shellWidth, shellHeight;
	bool synchronizedZoomAndScroll;
	double arrowScrollStep;
	struct { bool drawSelectionTimes, drawSelectionHairs; } picture;
} preferences;

void FunctionEditor_prefs (void) {
	Preferences_addInt (L"FunctionEditor.shellWidth", & preferences.shellWidth, 700);
	Preferences_addInt (L"FunctionEditor.shellHeight", & preferences.shellHeight, 440);
	Preferences_addBool (L"FunctionEditor.synchronizedZoomAndScroll", & preferences.synchronizedZoomAndScroll, true);
	Preferences_addDouble (L"FunctionEditor.arrowScrollStep", & preferences.arrowScrollStep, 0.05);   // BUG: seconds?
	Preferences_addBool (L"FunctionEditor.picture.drawSelectionTimes", & preferences.picture.drawSelectionTimes, true);
	Preferences_addBool (L"FunctionEditor.picture.drawSelectionHairs", & preferences.picture.drawSelectionHairs, true);
}

#define maxGroup 100
static int nGroup = 0;
static FunctionEditor theGroup [1 + maxGroup];

static int group_equalDomain (double tmin, double tmax) {
	if (nGroup == 0) return 1;
	for (int i = 1; i <= maxGroup; i ++)
		if (theGroup [i])
			return tmin == theGroup [i] -> tmin && tmax == theGroup [i] -> tmax;
	return 0;   // should not occur
}

static void updateScrollBar (FunctionEditor me) {
/* We cannot call this immediately after creation. */
	int slider_size = (my endWindow - my startWindow) / (my tmax - my tmin) * maximumScrollBarValue - 1;
	int increment, page_increment;
	int value = (my startWindow - my tmin) / (my tmax - my tmin) * maximumScrollBarValue + 1;
	if (slider_size < 1) slider_size = 1;
	if (value > maximumScrollBarValue - slider_size)
		value = maximumScrollBarValue - slider_size;
	if (value < 1) value = 1;
	#if motif
		XtVaSetValues (my scrollBar, XmNmaximum, maximumScrollBarValue, NULL);
	#endif
	increment = slider_size / SCROLL_INCREMENT_FRACTION + 1;
	page_increment = RELATIVE_PAGE_INCREMENT * slider_size + 1;
	#if gtk
		GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (my scrollBar));
		adj -> page_size = slider_size;
		gtk_adjustment_set_value (adj, value);
		gtk_adjustment_changed (adj);
		gtk_range_set_increments (GTK_RANGE (my scrollBar), increment, page_increment);
	#elif motif
		XmScrollBarSetValues (my scrollBar, value, slider_size, increment, page_increment, False);
	#endif
}

static void updateGroup (FunctionEditor me) {
	if (! my group) return;
	for (int i = 1; i <= maxGroup; i ++) if (theGroup [i] && theGroup [i] != me) {
		FunctionEditor thee = theGroup [i];
		if (preferences.synchronizedZoomAndScroll) {
			thy startWindow = my startWindow;
			thy endWindow = my endWindow;
		}
		thy startSelection = my startSelection;
		thy endSelection = my endSelection;
		FunctionEditor_updateText (thee);
		updateScrollBar (thee);
		Graphics_updateWs (thy graphics);
	}
}

static void drawNow (FunctionEditor me) {
	int leftFromWindow = my startWindow > my tmin;
	int rightFromWindow = my endWindow < my tmax;
	int cursorVisible = my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow;
	int selection = my endSelection > my startSelection;
	int beginVisible, endVisible;
	double verticalCorrection, bottom;
	wchar text [100];

	/* Update selection. */

	beginVisible = my startSelection > my startWindow && my startSelection < my endWindow;
	endVisible = my endSelection > my startWindow && my endSelection < my endWindow;

	/* Update markers. */

	my numberOfMarkers = 0;
	if (beginVisible)
		my marker [++ my numberOfMarkers] = my startSelection;
	if (endVisible && my endSelection != my startSelection)
		my marker [++ my numberOfMarkers] = my endSelection;
	my marker [++ my numberOfMarkers] = my endWindow;
	NUMsort_d (my numberOfMarkers, my marker);

	/* Update rectangles. */

	for (int i = 0; i < 8; i++)
		my rect [i]. left = my rect [i]. right = 0;

	/* 0: rectangle for total. */

	my rect [0]. left = leftFromWindow ? 0 : MARGIN;
	my rect [0]. right = my width - (rightFromWindow ? 0 : MARGIN);
	my rect [0]. bottom = BOTTOM_MARGIN;
	my rect [0]. top = BOTTOM_MARGIN + space;

	/* 1: rectangle for visible part. */

	my rect [1]. left = MARGIN;
	my rect [1]. right = my width - MARGIN;
	my rect [1]. bottom = BOTTOM_MARGIN + space;
	my rect [1]. top = BOTTOM_MARGIN + space * (my numberOfMarkers > 1 ? 2 : 3);

	/* 2: rectangle for left from visible part. */

	if (leftFromWindow) {
		my rect [2]. left = 0.0;
		my rect [2]. right = MARGIN;
		my rect [2]. bottom = BOTTOM_MARGIN + space;
		my rect [2]. top = BOTTOM_MARGIN + space * 2;
	}

	/* 3: rectangle for right from visible part. */

	if (rightFromWindow) {
		my rect [3]. left = my width - MARGIN;
		my rect [3]. right = my width;
		my rect [3]. bottom = BOTTOM_MARGIN + space;
		my rect [3]. top = BOTTOM_MARGIN + space * 2;
	}

	/* 4, 5, 6: rectangles between markers visible in visible part. */

	if (my numberOfMarkers > 1) {
		double window = my endWindow - my startWindow;
		for (int i = 1; i <= my numberOfMarkers; i ++) {
			my rect [3 + i]. left = i == 1 ? MARGIN : MARGIN + (my width - MARGIN * 2) *
				(my marker [i - 1] - my startWindow) / window;
			my rect [3 + i]. right = MARGIN + (my width - MARGIN * 2) *
				(my marker [i] - my startWindow) / window;
			my rect [3 + i]. bottom = BOTTOM_MARGIN + space * 2;
			my rect [3 + i]. top = BOTTOM_MARGIN + space * 3;
		}
	}
	
	if (selection) {
		double window = my endWindow - my startWindow;
		double left =
			my startSelection == my startWindow ? MARGIN :
			my startSelection == my tmin ? 0.0 :
			my startSelection < my startWindow ? MARGIN * 0.3 :
			my startSelection < my endWindow ? MARGIN + (my width - MARGIN * 2) * (my startSelection - my startWindow) / window :
			my startSelection == my endWindow ? my width - MARGIN : my width - MARGIN * 0.7;
		double right =
			my endSelection < my startWindow ? MARGIN * 0.7 :
			my endSelection == my startWindow ? MARGIN :
			my endSelection < my endWindow ? MARGIN + (my width - MARGIN * 2) * (my endSelection - my startWindow) / window :
			my endSelection == my endWindow ? my width - MARGIN :
			my endSelection < my tmax ? my width - MARGIN * 0.3 : my width;
		my rect [7]. left = left;
		my rect [7]. right = right;
		my rect [7]. bottom = my height - space - TOP_MARGIN;
		my rect [7]. top = my height - TOP_MARGIN;
	}

	/*
	 * Be responsive: update the markers now.
	 */
	Graphics_setViewport (my graphics, 0, my width, 0, my height);
	Graphics_setWindow (my graphics, 0, my width, 0, my height);
	Graphics_setGrey (my graphics, 0.85);
	Graphics_fillRectangle (my graphics, MARGIN, my width - MARGIN, my height - TOP_MARGIN - space, my height);
	Graphics_fillRectangle (my graphics, 0, MARGIN, BOTTOM_MARGIN + ( leftFromWindow ? space * 2 : 0 ), my height);
	Graphics_fillRectangle (my graphics, my width - MARGIN, my width, BOTTOM_MARGIN + ( rightFromWindow ? space * 2 : 0 ), my height);
	Graphics_setGrey (my graphics, 0.0);
	#if defined (macintosh)
		Graphics_line (my graphics, 0, 2, my width, 2);
		Graphics_line (my graphics, 0, my height - 2, my width, my height - 2);
	#endif

	Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
	for (int i = 0; i < 8; i ++) {
		double left = my rect [i]. left, right = my rect [i]. right;
		if (left < right)
			Graphics_button (my graphics, left, right, my rect [i]. bottom, my rect [i]. top);
	}
	verticalCorrection = my height / (my height - 111 + 11.0);
	#ifdef _WIN32
		verticalCorrection *= 1.5;
	#endif
	for (int i = 0; i < 8; i ++) {
		double left = my rect [i]. left, right = my rect [i]. right;
		double bottom = my rect [i]. bottom, top = my rect [i]. top;
		if (left < right) {
			const wchar *format = my v_format_long ();
			double value = NUMundefined, inverseValue = 0.0;
			switch (i) {
				case 0: format = my v_format_totalDuration (), value = my tmax - my tmin; break;
				case 1: format = my v_format_window (), value = my endWindow - my startWindow;
					/*
					 * Window domain text.
					 */	
					Graphics_setColour (my graphics, Graphics_BLUE);
					Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_HALF);
					Graphics_text1 (my graphics, left, 0.5 * (bottom + top) - verticalCorrection, Melder_fixed (my startWindow, my v_fixedPrecision_long ()));
					Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
					Graphics_text1 (my graphics, right, 0.5 * (bottom + top) - verticalCorrection, Melder_fixed (my endWindow, my v_fixedPrecision_long ()));
					Graphics_setColour (my graphics, Graphics_BLACK);
					Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
				break;
				case 2: value = my startWindow - my tmin; break;
				case 3: value = my tmax - my endWindow; break;
				case 4: value = my marker [1] - my startWindow; break;
				case 5: value = my marker [2] - my marker [1]; break;
				case 6: value = my marker [3] - my marker [2]; break;
				case 7: format = my v_format_selection (), value = my endSelection - my startSelection, inverseValue = 1 / value; break;
			}
			swprintf (text, 100, format, value, inverseValue);
			if (Graphics_textWidth (my graphics, text) < right - left) {
				Graphics_text (my graphics, 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text);
			} else if (format == my v_format_long ()) {
				swprintf (text, 100, my v_format_short (), value);
				if (Graphics_textWidth (my graphics, text) < right - left)
					Graphics_text (my graphics, 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text);
			} else {
				swprintf (text, 100, my v_format_long (), value);
				if (Graphics_textWidth (my graphics, text) < right - left) {
						Graphics_text (my graphics, 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text);
				} else {
					swprintf (text, 100, my v_format_short (), my endSelection - my startSelection);
					if (Graphics_textWidth (my graphics, text) < right - left)
						Graphics_text (my graphics, 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text);
				}
			}
		}
	}

	Graphics_setViewport (my graphics, MARGIN, my width - MARGIN, 0, my height);
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0, my height);
	/*Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, my startWindow, my endWindow, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));*/
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_rectangle (my graphics, my startWindow, my endWindow, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));

	/*
	 * Red marker text.
	 */
	Graphics_setColour (my graphics, Graphics_RED);
	if (cursorVisible) {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text1 (my graphics, my startSelection, my height - (TOP_MARGIN + space) - verticalCorrection, Melder_fixed (my startSelection, my v_fixedPrecision_long ()));
	}
	if (beginVisible && selection) {
		Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
		Graphics_text1 (my graphics, my startSelection, my height - (TOP_MARGIN + space/2) - verticalCorrection, Melder_fixed (my startSelection, my v_fixedPrecision_long ()));
	}
	if (endVisible && selection) {
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_HALF);
		Graphics_text1 (my graphics, my endSelection, my height - (TOP_MARGIN + space/2) - verticalCorrection, Melder_fixed (my endSelection, my v_fixedPrecision_long ()));
	}
	Graphics_setColour (my graphics, Graphics_BLACK);

	/*
	 * To reduce flashing, give our descendants the opportunity to prepare their data.
	 */
	my v_prepareDraw ();

	/*
	 * Start of inner drawing.
	 */
	Graphics_setViewport (my graphics, MARGIN, my width - MARGIN, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));

	my v_draw ();
	Graphics_setViewport (my graphics, MARGIN, my width - MARGIN, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));

	/*
	 * Red dotted marker lines.
	 */
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0.0, 1.0);
	Graphics_setColour (my graphics, Graphics_RED);
	Graphics_setLineType (my graphics, Graphics_DOTTED);
	bottom = my v_getBottomOfSoundAndAnalysisArea ();
	if (cursorVisible)
		Graphics_line (my graphics, my startSelection, bottom, my startSelection, 1.0);
	if (beginVisible)
		Graphics_line (my graphics, my startSelection, bottom, my startSelection, 1.0);
	if (endVisible)
		Graphics_line (my graphics, my endSelection, bottom, my endSelection, 1.0);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_setLineType (my graphics, Graphics_DRAWN);

	/*
	 * Highlight selection.
	 */
	if (selection && my startSelection < my endWindow && my endSelection > my startWindow) {
		double left = my startSelection, right = my endSelection;
		if (left < my startWindow) left = my startWindow;
		if (right > my endWindow) right = my endWindow;
		my v_highlightSelection (left, right, 0.0, 1.0);
	}

	/*
	 * End of inner drawing.
	 */
	Graphics_setViewport (my graphics, 0, my width, 0, my height);
}

/********** METHODS **********/

void structFunctionEditor :: v_destroy () {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	if (group) {   // undangle
		int i = 1; while (theGroup [i] != this) { Melder_assert (i < maxGroup); i ++; } theGroup [i] = NULL;
		nGroup --;
	}
	forget (graphics);
	FunctionEditor_Parent :: v_destroy ();
}

void structFunctionEditor :: v_info () {
	FunctionEditor_Parent :: v_info ();
	MelderInfo_writeLine4 (L"Editor start: ", Melder_double (tmin), L" ", v_format_units ());
	MelderInfo_writeLine4 (L"Editor end: ", Melder_double (tmax), L" ", v_format_units ());
	MelderInfo_writeLine4 (L"Window start: ", Melder_double (startWindow), L" ", v_format_units ());
	MelderInfo_writeLine4 (L"Window end: ", Melder_double (endWindow), L" ", v_format_units ());
	MelderInfo_writeLine4 (L"Selection start: ", Melder_double (startSelection), L" ", v_format_units ());
	MelderInfo_writeLine4 (L"Selection end: ", Melder_double (endSelection), L" ", v_format_units ());
	MelderInfo_writeLine4 (L"Arrow scroll step: ", Melder_double (arrowScrollStep), L" ", v_format_units ());
	MelderInfo_writeLine2 (L"Group: ", group ? L"yes" : L"no");
}

/********** FILE MENU **********/

static void menu_cb_preferences (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	EDITOR_FORM (L"Preferences", 0)
		BOOLEAN (L"Synchronize zoom and scroll", 1)
		POSITIVE (L"Arrow scroll step (s)", L"0.05")
		my v_prefs_addFields (cmd);
	EDITOR_OK
		SET_INTEGER (L"Synchronize zoom and scroll", preferences.synchronizedZoomAndScroll)
		SET_REAL (L"Arrow scroll step", my arrowScrollStep)
		my v_prefs_setValues (cmd);
	EDITOR_DO
		bool oldSynchronizedZoomAndScroll = preferences.synchronizedZoomAndScroll;
		preferences.synchronizedZoomAndScroll = GET_INTEGER (L"Synchronize zoom and scroll");
		preferences.arrowScrollStep = my arrowScrollStep = GET_REAL (L"Arrow scroll step");
		if (! oldSynchronizedZoomAndScroll && preferences.synchronizedZoomAndScroll) {
			updateGroup (me);
		}
		my v_prefs_getValues (cmd);
	EDITOR_END
}

void structFunctionEditor :: v_form_pictureSelection (EditorCommand cmd) {
	BOOLEAN (L"Draw selection times", 1);
	BOOLEAN (L"Draw selection hairs", 1);
}
void structFunctionEditor :: v_ok_pictureSelection (EditorCommand cmd) {
	SET_INTEGER (L"Draw selection times", preferences.picture.drawSelectionTimes);
	SET_INTEGER (L"Draw selection hairs", preferences.picture.drawSelectionHairs);
}
void structFunctionEditor :: v_do_pictureSelection (EditorCommand cmd) {
	preferences.picture.drawSelectionTimes = GET_INTEGER (L"Draw selection times");
	preferences.picture.drawSelectionHairs = GET_INTEGER (L"Draw selection hairs");
}

/********** QUERY MENU **********/

static void menu_cb_getB (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	Melder_informationReal (my startSelection, my v_format_units ());
}
static void menu_cb_getCursor (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	Melder_informationReal (0.5 * (my startSelection + my endSelection), my v_format_units ());
}
static void menu_cb_getE (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	Melder_informationReal (my endSelection, my v_format_units ());
}
static void menu_cb_getSelectionDuration (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	Melder_informationReal (my endSelection - my startSelection, my v_format_units ());
}

/********** VIEW MENU **********/

static void menu_cb_zoom (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	EDITOR_FORM (L"Zoom", 0)
		REAL (L"From", L"0.0")
		REAL (L"To", L"1.0")
	EDITOR_OK
		SET_REAL (L"From", my startWindow)
		SET_REAL (L"To", my endWindow)
	EDITOR_DO
		my startWindow = GET_REAL (L"From");
		if (my startWindow < my tmin + 1e-12)
			my startWindow = my tmin;
		my endWindow = GET_REAL (L"To");
		if (my endWindow > my tmax - 1e-12)
			my endWindow = my tmax;
		my v_updateText ();
		updateScrollBar (me);
		/*Graphics_updateWs (my graphics);*/ drawNow (me);
		updateGroup (me);
	EDITOR_END
}

static void do_showAll (FunctionEditor me) {
	my startWindow = my tmin;
	my endWindow = my tmax;
	my v_updateText ();
	updateScrollBar (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	if (preferences.synchronizedZoomAndScroll) {
		updateGroup (me);
	}
}

static void gui_button_cb_showAll (I, GuiButtonEvent event) {
	(void) event;
	iam (FunctionEditor);
	do_showAll (me);
}

static void do_zoomIn (FunctionEditor me) {
	double shift = (my endWindow - my startWindow) / 4;
	my startWindow += shift;
	my endWindow -= shift;
	my v_updateText ();
	updateScrollBar (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	if (preferences.synchronizedZoomAndScroll) {
		updateGroup (me);
	}
}

static void gui_button_cb_zoomIn (I, GuiButtonEvent event) {
	(void) event;
	iam (FunctionEditor);
	do_zoomIn (me);
}

static void do_zoomOut (FunctionEditor me) {
	double shift = (my endWindow - my startWindow) / 2;
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   /* Quickly, before window changes. */
	my startWindow -= shift;
	if (my startWindow < my tmin + 1e-12)
		my startWindow = my tmin;
	my endWindow += shift;
	if (my endWindow > my tmax - 1e-12)
		my endWindow = my tmax;
	my v_updateText ();
	updateScrollBar (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	if (preferences.synchronizedZoomAndScroll) {
		updateGroup (me);
	}
}

static void gui_button_cb_zoomOut (I, GuiButtonEvent event) {
	(void) event;
	iam (FunctionEditor);
	do_zoomOut (me);
}

static void do_zoomToSelection (FunctionEditor me) {
	if (my endSelection > my startSelection) {
		my startZoomHistory = my startWindow;   // remember for Zoom Back
		my endZoomHistory = my endWindow;   // remember for Zoom Back
		//Melder_casual ("Zoomed in to %f ~ %f seconds.", my startSelection, my endSelection);
		my startWindow = my startSelection;
		my endWindow = my endSelection;
		my v_updateText ();
		updateScrollBar (me);
		/*Graphics_updateWs (my graphics);*/ drawNow (me);
		if (preferences.synchronizedZoomAndScroll) {
			updateGroup (me);
		}
	}
}

static void gui_button_cb_zoomToSelection (I, GuiButtonEvent event) {
	(void) event;
	iam (FunctionEditor);
	do_zoomToSelection (me);
}

static void do_zoomBack (FunctionEditor me) {
	if (my endZoomHistory > my startZoomHistory) {
		my startWindow = my startZoomHistory;
		my endWindow = my endZoomHistory;
		my v_updateText ();
		updateScrollBar (me);
		/*Graphics_updateWs (my graphics);*/ drawNow (me);
		if (preferences.synchronizedZoomAndScroll) {
			updateGroup (me);
		}
	}
}

static void gui_button_cb_zoomBack (I, GuiButtonEvent event) {
	(void) event;
	iam (FunctionEditor);
	do_zoomBack (me);
}

static void menu_cb_showAll (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	do_showAll (me);
}

static void menu_cb_zoomIn (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	do_zoomIn (me);
}

static void menu_cb_zoomOut (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	do_zoomOut (me);
}

static void menu_cb_zoomToSelection (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	do_zoomToSelection (me);
}

static void menu_cb_zoomBack (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	do_zoomBack (me);
}

static void menu_cb_play (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	EDITOR_FORM (L"Play", 0)
		REAL (L"From", L"0.0")
		REAL (L"To", L"1.0")
	EDITOR_OK
		SET_REAL (L"From", my startWindow)
		SET_REAL (L"To", my endWindow)
	EDITOR_DO
		MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
		my v_play (GET_REAL (L"From"), GET_REAL (L"To"));
	EDITOR_END
}

static void menu_cb_playOrStop (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	if (MelderAudio_isPlaying) {
		MelderAudio_stopPlaying (MelderAudio_EXPLICIT);
	} else if (my startSelection < my endSelection) {
		my playingSelection = true;
		my v_play (my startSelection, my endSelection);
	} else {
		my playingCursor = true;
		if (my startSelection == my endSelection && my startSelection > my startWindow && my startSelection < my endWindow)
			my v_play (my startSelection, my endWindow);
		else
			my v_play (my startWindow, my endWindow);
	}
}

static void menu_cb_playWindow (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	my playingCursor = true;
	my v_play (my startWindow, my endWindow);
}

static void menu_cb_interruptPlaying (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
}

/********** SELECT MENU **********/

static void menu_cb_select (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	EDITOR_FORM (L"Select", 0)
		REAL (L"Start of selection", L"0.0")
		REAL (L"End of selection", L"1.0")
	EDITOR_OK
		SET_REAL (L"Start of selection", my startSelection)
		SET_REAL (L"End of selection", my endSelection)
	EDITOR_DO
		my startSelection = GET_REAL (L"Start of selection");
		if (my startSelection < my tmin + 1e-12)
			my startSelection = my tmin;
		my endSelection = GET_REAL (L"End of selection");
		if (my endSelection > my tmax - 1e-12)
			my endSelection = my tmax;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		my v_updateText ();
		/*Graphics_updateWs (my graphics);*/ drawNow (me);
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveCursorToB (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	my endSelection = my startSelection;
	my v_updateText ();
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
}

static void menu_cb_moveCursorToE (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	my startSelection = my endSelection;
	my v_updateText ();
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
}

static void menu_cb_moveCursorTo (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	EDITOR_FORM (L"Move cursor to", 0)
		REAL (L"Position", L"0.0")
	EDITOR_OK
		SET_REAL (L"Position", 0.5 * (my startSelection + my endSelection))
	EDITOR_DO
		double position = GET_REAL (L"Position");
		if (position < my tmin + 1e-12) position = my tmin;
		if (position > my tmax - 1e-12) position = my tmax;
		my startSelection = my endSelection = position;
		my v_updateText ();
		/*Graphics_updateWs (my graphics);*/ drawNow (me);
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveCursorBy (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	EDITOR_FORM (L"Move cursor by", 0)
		REAL (L"Distance", L"0.05")
	EDITOR_OK
	EDITOR_DO
		double position = 0.5 * (my startSelection + my endSelection) + GET_REAL (L"Distance");
		if (position < my tmin) position = my tmin;
		if (position > my tmax) position = my tmax;
		my startSelection = my endSelection = position;
		my v_updateText ();
		/*Graphics_updateWs (my graphics);*/ drawNow (me);
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveBby (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	EDITOR_FORM (L"Move start of selection by", 0)
		REAL (L"Distance", L"0.05")
	EDITOR_OK
	EDITOR_DO
		double position = my startSelection + GET_REAL (L"Distance");
		if (position < my tmin) position = my tmin;
		if (position > my tmax) position = my tmax;
		my startSelection = position;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		my v_updateText ();
		/*Graphics_updateWs (my graphics);*/ drawNow (me);
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveEby (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	EDITOR_FORM (L"Move end of selection by", 0)
		REAL (L"Distance", L"0.05")
	EDITOR_OK
	EDITOR_DO
		double position = my endSelection + GET_REAL (L"Distance");
		if (position < my tmin) position = my tmin;
		if (position > my tmax) position = my tmax;
		my endSelection = position;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		my v_updateText ();
		/*Graphics_updateWs (my graphics);*/ drawNow (me);
		updateGroup (me);
	EDITOR_END
}

void FunctionEditor_shift (FunctionEditor me, double shift) {
	double windowLength = my endWindow - my startWindow;
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   /* Quickly, before window changes. */
	if (shift < 0.0) {
		my startWindow += shift;
		if (my startWindow < my tmin + 1e-12)
			my startWindow = my tmin;
		my endWindow = my startWindow + windowLength;
		if (my endWindow > my tmax - 1e-12)
			my endWindow = my tmax;
	} else {
		my endWindow += shift;
		if (my endWindow > my tmax - 1e-12)
			my endWindow = my tmax;
		my startWindow = my endWindow - windowLength;
		if (my startWindow < my tmin + 1e-12)
			my startWindow = my tmin;
	}
	FunctionEditor_marksChanged (me);
}

static void menu_cb_pageUp (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	FunctionEditor_shift (me, -RELATIVE_PAGE_INCREMENT * (my endWindow - my startWindow));
	updateGroup (me);
}

static void menu_cb_pageDown (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	FunctionEditor_shift (me, +RELATIVE_PAGE_INCREMENT * (my endWindow - my startWindow));
	updateGroup (me);
}

static void scrollToView (FunctionEditor me, double t) {
	if (t <= my startWindow) {
		FunctionEditor_shift (me, t - my startWindow - 0.618 * (my endWindow - my startWindow));
	} else if (t >= my endWindow) {
		FunctionEditor_shift (me, t - my endWindow + 0.618 * (my endWindow - my startWindow));
	} else {
		FunctionEditor_marksChanged (me);
	}
	updateGroup (me);
}

static void menu_cb_selectEarlier (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	my startSelection -= my arrowScrollStep;
	if (my startSelection < my tmin + 1e-12)
		my startSelection = my tmin;
	my endSelection -= my arrowScrollStep;
	if (my endSelection < my tmin + 1e-12)
		my endSelection = my tmin;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

static void menu_cb_selectLater (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	my startSelection += my arrowScrollStep;
	if (my startSelection > my tmax - 1e-12)
		my startSelection = my tmax;
	my endSelection += my arrowScrollStep;
	if (my endSelection > my tmax - 1e-12)
		my endSelection = my tmax;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

static void menu_cb_moveBleft (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	my startSelection -= my arrowScrollStep;
	if (my startSelection < my tmin + 1e-12)
		my startSelection = my tmin;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

static void menu_cb_moveBright (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	my startSelection += my arrowScrollStep;
	if (my startSelection > my tmax - 1e-12)
		my startSelection = my tmax;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

static void menu_cb_moveEleft (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	my endSelection -= my arrowScrollStep;
	if (my endSelection < my tmin + 1e-12)
		my endSelection = my tmin;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

static void menu_cb_moveEright (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	my endSelection += my arrowScrollStep;
	if (my endSelection > my tmax - 1e-12)
		my endSelection = my tmax;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

/********** GUI CALLBACKS **********/

#if gtk
static void gui_cb_scroll (GtkRange *rng, gpointer void_me) {
	iam (FunctionEditor);
	if (my graphics == NULL) return;   // ignore events during creation
	double value = gtk_range_get_value (GTK_RANGE (rng));
	double shift = my tmin + (value - 1) * (my tmax - my tmin) / maximumScrollBarValue - my startWindow;
	if (shift != 0.0) {
		int i;
		my startWindow += shift;
		if (my startWindow < my tmin + 1e-12) my startWindow = my tmin;
		my endWindow += shift;
		if (my endWindow > my tmax - 1e-12) my endWindow = my tmax;
		my v_updateText ();
		/*Graphics_clearWs (my graphics);*/
		drawNow (me);   /* Do not wait for expose event. */
		if (! my group || ! preferences.synchronizedZoomAndScroll) return;
		for (i = 1; i <= maxGroup; i ++) if (theGroup [i] && theGroup [i] != me) {
			theGroup [i] -> startWindow = my startWindow;
			theGroup [i] -> endWindow = my endWindow;
			FunctionEditor_updateText (theGroup [i]);
			updateScrollBar (theGroup [i]);
			Graphics_clearWs (theGroup [i] -> graphics);
			drawNow (theGroup [i]);
		}
	}
}
#else
static void gui_cb_scroll (GUI_ARGS) {
	GUI_IAM (FunctionEditor);
	if (my graphics == NULL) return;   // ignore events during creation
	int value, slider, incr, pincr;
	XmScrollBarGetValues (w, & value, & slider, & incr, & pincr);
	double shift = my tmin + (value - 1) * (my tmax - my tmin) / maximumScrollBarValue - my startWindow;
	if (shift != 0.0) {
		my startWindow += shift;
		if (my startWindow < my tmin + 1e-12) my startWindow = my tmin;
		my endWindow += shift;
		if (my endWindow > my tmax - 1e-12) my endWindow = my tmax;
		my v_updateText ();
		/*Graphics_clearWs (my graphics);*/
		drawNow (me);   // do not wait for expose event
		if (! my group || ! preferences.synchronizedZoomAndScroll) return;
		for (int i = 1; i <= maxGroup; i ++) if (theGroup [i] && theGroup [i] != me) {
			theGroup [i] -> startWindow = my startWindow;
			theGroup [i] -> endWindow = my endWindow;
			FunctionEditor_updateText (theGroup [i]);
			updateScrollBar (theGroup [i]);
			Graphics_clearWs (theGroup [i] -> graphics);
			drawNow (theGroup [i]);
		}
	}
}
#endif

static void gui_checkbutton_cb_group (I, GuiCheckButtonEvent event) {
	iam (FunctionEditor);
	(void) event;
	int i;
	my group = ! my group;
	if (my group) {
		FunctionEditor thee;
		i = 1; while (theGroup [i]) i ++; theGroup [i] = me;
		if (++ nGroup == 1) { Graphics_updateWs (my graphics); return; }
		i = 1; while (theGroup [i] == NULL || theGroup [i] == me) i ++; thee = theGroup [i];
		if (preferences.synchronizedZoomAndScroll) {
			my startWindow = thy startWindow;
			my endWindow = thy endWindow;
		}
		my startSelection = thy startSelection;
		my endSelection = thy endSelection;
		if (my tmin > thy tmin || my tmax < thy tmax) {
			if (my tmin > thy tmin) my tmin = thy tmin;
			if (my tmax < thy tmax) my tmax = thy tmax;
			my v_updateText ();
			updateScrollBar (me);
			Graphics_updateWs (my graphics);
		} else {
			my v_updateText ();
			updateScrollBar (me);
			Graphics_updateWs (my graphics);
			if (my tmin < thy tmin || my tmax > thy tmax)
				for (i = 1; i <= maxGroup; i ++) if (theGroup [i] && theGroup [i] != me) {
					if (my tmin < thy tmin)
						theGroup [i] -> tmin = my tmin;
					if (my tmax > thy tmax)
						theGroup [i] -> tmax = my tmax;
					FunctionEditor_updateText (theGroup [i]);
					updateScrollBar (theGroup [i]);
					Graphics_updateWs (theGroup [i] -> graphics);
				}
		}
	} else {
		i = 1; while (theGroup [i] != me) i ++; theGroup [i] = NULL;
		nGroup --;
		my v_updateText ();
		Graphics_updateWs (my graphics);   // for setting buttons in draw method
	}
	if (my group) updateGroup (me);
}

static void menu_cb_intro (EDITOR_ARGS) {
	EDITOR_IAM (FunctionEditor);
	Melder_help (L"Intro");
}

void structFunctionEditor :: v_createMenuItems_file (EditorMenu menu) {
	FunctionEditor_Parent :: v_createMenuItems_file (menu);
	EditorMenu_addCommand (menu, L"Preferences...", 0, menu_cb_preferences);
	EditorMenu_addCommand (menu, L"-- after preferences --", 0, 0);
}

void structFunctionEditor :: v_createMenuItems_view_timeDomain (EditorMenu menu) {
	EditorMenu_addCommand (menu, v_format_domain (), GuiMenu_INSENSITIVE, menu_cb_zoom /* dummy */);
	EditorMenu_addCommand (menu, L"Zoom...", 0, menu_cb_zoom);
	EditorMenu_addCommand (menu, L"Show all", 'A', menu_cb_showAll);
	EditorMenu_addCommand (menu, L"Zoom in", 'I', menu_cb_zoomIn);
	EditorMenu_addCommand (menu, L"Zoom out", 'O', menu_cb_zoomOut);
	EditorMenu_addCommand (menu, L"Zoom to selection", 'N', menu_cb_zoomToSelection);
	EditorMenu_addCommand (menu, L"Zoom back", 'B', menu_cb_zoomBack);
	EditorMenu_addCommand (menu, L"Scroll page back", GuiMenu_PAGE_UP, menu_cb_pageUp);
	EditorMenu_addCommand (menu, L"Scroll page forward", GuiMenu_PAGE_DOWN, menu_cb_pageDown);
}

void structFunctionEditor :: v_createMenuItems_view_audio (EditorMenu menu) {
	EditorMenu_addCommand (menu, L"-- play --", 0, 0);
	EditorMenu_addCommand (menu, L"Audio:", GuiMenu_INSENSITIVE, menu_cb_play /* dummy */);
	EditorMenu_addCommand (menu, L"Play...", 0, menu_cb_play);
	EditorMenu_addCommand (menu, L"Play or stop", gtk ? 0 : GuiMenu_TAB, menu_cb_playOrStop);
	EditorMenu_addCommand (menu, L"Play window", gtk ? 0 : GuiMenu_SHIFT + GuiMenu_TAB, menu_cb_playWindow);
	EditorMenu_addCommand (menu, L"Interrupt playing", GuiMenu_ESCAPE, menu_cb_interruptPlaying);
}

void structFunctionEditor :: v_createMenuItems_view (EditorMenu menu) {
	v_createMenuItems_view_timeDomain (menu);
	v_createMenuItems_view_audio (menu);
}

void structFunctionEditor :: v_createMenuItems_query (EditorMenu menu) {
	FunctionEditor_Parent :: v_createMenuItems_query (menu);
	EditorMenu_addCommand (menu, L"-- query selection --", 0, 0);
	EditorMenu_addCommand (menu, L"Get start of selection", 0, menu_cb_getB);
	EditorMenu_addCommand (menu, L"Get begin of selection", Editor_HIDDEN, menu_cb_getB);
	EditorMenu_addCommand (menu, L"Get cursor", GuiMenu_F6, menu_cb_getCursor);
	EditorMenu_addCommand (menu, L"Get end of selection", 0, menu_cb_getE);
	EditorMenu_addCommand (menu, L"Get selection length", 0, menu_cb_getSelectionDuration);
}

void structFunctionEditor :: v_createMenus () {
	FunctionEditor_Parent :: v_createMenus ();
	EditorMenu menu;

	menu = Editor_addMenu (this, L"View", 0);
	v_createMenuItems_view (menu);

	Editor_addMenu (this, L"Select", 0);
	Editor_addCommand (this, L"Select", L"Select...", 0, menu_cb_select);
	Editor_addCommand (this, L"Select", L"Move cursor to start of selection", 0, menu_cb_moveCursorToB);
	Editor_addCommand (this, L"Select", L"Move cursor to begin of selection", Editor_HIDDEN, menu_cb_moveCursorToB);
	Editor_addCommand (this, L"Select", L"Move cursor to end of selection", 0, menu_cb_moveCursorToE);
	Editor_addCommand (this, L"Select", L"Move cursor to...", 0, menu_cb_moveCursorTo);
	Editor_addCommand (this, L"Select", L"Move cursor by...", 0, menu_cb_moveCursorBy);
	Editor_addCommand (this, L"Select", L"Move start of selection by...", 0, menu_cb_moveBby);
	Editor_addCommand (this, L"Select", L"Move begin of selection by...", Editor_HIDDEN, menu_cb_moveBby);
	Editor_addCommand (this, L"Select", L"Move end of selection by...", 0, menu_cb_moveEby);
	/*Editor_addCommand (this, L"Select", L"Move cursor back by half a second", motif_, menu_cb_moveCursorBy);*/
	Editor_addCommand (this, L"Select", L"Select earlier", GuiMenu_UP_ARROW, menu_cb_selectEarlier);
	Editor_addCommand (this, L"Select", L"Select later", GuiMenu_DOWN_ARROW, menu_cb_selectLater);
	Editor_addCommand (this, L"Select", L"Move start of selection left", GuiMenu_SHIFT + GuiMenu_UP_ARROW, menu_cb_moveBleft);
	Editor_addCommand (this, L"Select", L"Move begin of selection left", Editor_HIDDEN, menu_cb_moveBleft);
	Editor_addCommand (this, L"Select", L"Move start of selection right", GuiMenu_SHIFT + GuiMenu_DOWN_ARROW, menu_cb_moveBright);
	Editor_addCommand (this, L"Select", L"Move begin of selection right", Editor_HIDDEN, menu_cb_moveBright);
	Editor_addCommand (this, L"Select", L"Move end of selection left", GuiMenu_COMMAND + GuiMenu_UP_ARROW, menu_cb_moveEleft);
	Editor_addCommand (this, L"Select", L"Move end of selection right", GuiMenu_COMMAND + GuiMenu_DOWN_ARROW, menu_cb_moveEright);
}

void structFunctionEditor :: v_createHelpMenuItems (EditorMenu menu) {
	FunctionEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"Intro", 0, menu_cb_intro);
}

static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (FunctionEditor);
	(void) event;
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	if (my enableUpdates)
		drawNow (me);
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	iam (FunctionEditor);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
if (gtk && event -> type != BUTTON_PRESS) return;
	double xWC, yWC;
	my shiftKeyPressed = event -> shiftKeyPressed;
	Graphics_setWindow (my graphics, 0, my width, 0, my height);
	Graphics_DCtoWC (my graphics, event -> x, event -> y, & xWC, & yWC);

	if (yWC > BOTTOM_MARGIN + space * 3 && yWC < my height - (TOP_MARGIN + space)) {   /* In signal region? */
		int needsUpdate;
		Graphics_setViewport (my graphics, MARGIN, my width - MARGIN,
			BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));
		Graphics_setWindow (my graphics, my startWindow, my endWindow, 0.0, 1.0);
		Graphics_DCtoWC (my graphics, event -> x, event -> y, & xWC, & yWC);
		if (xWC < my startWindow) xWC = my startWindow;
		if (xWC > my endWindow) xWC = my endWindow;
		if (Melder_debug == 24) {
			Melder_casual ("FunctionEditor::gui_drawingarea_cb_click: button %d shift %d option %d command %d control %d",
				event -> button, my shiftKeyPressed, event -> optionKeyPressed, event -> commandKeyPressed, event -> extraControlKeyPressed);
		}
#if defined (macintosh)
		needsUpdate =
			event -> optionKeyPressed || event -> extraControlKeyPressed ? my v_clickB (xWC, yWC) :
			event -> commandKeyPressed ? my v_clickE (xWC, yWC) :
			my v_click (xWC, yWC, my shiftKeyPressed);
#elif defined (_WIN32)
		needsUpdate =
			event -> commandKeyPressed ? my v_clickB (xWC, yWC) :
			event -> optionKeyPressed ? my v_clickE (xWC, yWC) :
			my v_click (xWC, yWC, my shiftKeyPressed);
#else
		needsUpdate =
			event -> commandKeyPressed ? my v_clickB (xWC, yWC) :
			event -> optionKeyPressed ? my v_clickE (xWC, yWC) :
			event -> button == 1 ? my v_click (xWC, yWC, my shiftKeyPressed) :
			event -> button == 2 ? my v_clickB (xWC, yWC) : my v_clickE (xWC, yWC);
#endif
		if (needsUpdate) my v_updateText ();
		Graphics_setViewport (my graphics, 0, my width, 0, my height);
		if (needsUpdate) /*Graphics_updateWs (my graphics);*/ drawNow (me);
		if (needsUpdate) updateGroup (me);
	}
	else   /* Clicked outside signal region? Let us hear it. */
	{
		for (int i = 0; i < 8; i ++) {
			if (xWC > my rect [i]. left && xWC < my rect [i]. right &&
				 yWC > my rect [i]. bottom && yWC < my rect [i]. top)
				switch (i) {
					case 0: my v_play (my tmin, my tmax); break;
					case 1: my v_play (my startWindow, my endWindow); break;
					case 2: my v_play (my tmin, my startWindow); break;
					case 3: my v_play (my endWindow, my tmax); break;
					case 4: my v_play (my startWindow, my marker [1]); break;
					case 5: my v_play (my marker [1], my marker [2]); break;
					case 6: my v_play (my marker [2], my marker [3]); break;
					case 7: my v_play (my startSelection, my endSelection); break;
				}
		}
	}
}

static void gui_drawingarea_cb_key (I, GuiDrawingAreaKeyEvent event) {
	iam (FunctionEditor);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	my v_key (event -> key);
}

static void gui_drawingarea_cb_resize (I, GuiDrawingAreaResizeEvent event) {
	iam (FunctionEditor);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	Graphics_setWsViewport (my graphics, 0, event -> width, 0, event -> height);
	my width = event -> width + 21;
	my height = event -> height + 111;
	Graphics_setWsWindow (my graphics, 0, my width, 0, my height);
	Graphics_setViewport (my graphics, 0, my width, 0, my height);
	#if gtk
		// updateWs() also resizes the cairo clipping context to the new window size
	#endif
	Graphics_updateWs (my graphics);

	/* Save the current shell size as the user's preference for a new FunctionEditor. */

	preferences.shellWidth = GuiObject_getWidth (my d_windowShell);
	preferences.shellHeight = GuiObject_getHeight (my d_windowShell);
}

void structFunctionEditor :: v_createChildren () {
	GuiObject form;
	int x = BUTTON_X;

	#if gtk
		form = d_windowForm;
		GuiObject hctl_box = gtk_hbox_new (FALSE, BUTTON_SPACING);
		gtk_box_pack_end (GTK_BOX (form), GTK_WIDGET (hctl_box), FALSE, FALSE, 0);
		GuiObject leftbtn_box = gtk_hbox_new (TRUE, 3);
		gtk_box_pack_start (GTK_BOX (hctl_box), GTK_WIDGET (leftbtn_box), FALSE, FALSE, 0);

		/***** Create zoom buttons. *****/

		gtk_box_pack_start (GTK_BOX (leftbtn_box),
			GTK_WIDGET (GuiButton_create (NULL, 0, 0, 0, 0, L"all", gui_button_cb_showAll, this, 0)), TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (leftbtn_box),
			GTK_WIDGET (GuiButton_create (NULL, 0, 0, 0, 0, L"in", gui_button_cb_zoomIn, this, 0)), TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (leftbtn_box),
			GTK_WIDGET (GuiButton_create (NULL, 0, 0, 0, 0, L"out", gui_button_cb_zoomOut, this, 0)), TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (leftbtn_box),
			GTK_WIDGET (GuiButton_create (NULL, 0, 0, 0, 0, L"sel", gui_button_cb_zoomToSelection, this, 0)), TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (leftbtn_box),
			GTK_WIDGET (GuiButton_create (NULL, 0, 0, 0, 0, L"bak", gui_button_cb_zoomBack, this, 0)), TRUE, TRUE, 0);

		GuiObject_show (leftbtn_box);
	#elif motif
		form = XmCreateForm (d_windowForm, "buttons", NULL, 0);
		XtVaSetValues (form,
			XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
			XmNbottomAttachment, XmATTACH_FORM,
			XmNtraversalOn, False,   /* Needed in order to redirect all keyboard input to the text widget. */
			NULL);

		/***** Create zoom buttons. *****/

		GuiButton_createShown (form, x, x + BUTTON_WIDTH, -6 - Machine_getScrollBarWidth (), -4,
			L"all", gui_button_cb_showAll, this, 0);
		x += BUTTON_WIDTH + BUTTON_SPACING;
		GuiButton_createShown (form, x, x + BUTTON_WIDTH, -6 - Machine_getScrollBarWidth (), -4,
			L"in", gui_button_cb_zoomIn, this, 0);
		x += BUTTON_WIDTH + BUTTON_SPACING;
		GuiButton_createShown (form, x, x + BUTTON_WIDTH, -6 - Machine_getScrollBarWidth (), -4,
			L"out", gui_button_cb_zoomOut, this, 0);
		x += BUTTON_WIDTH + BUTTON_SPACING;
		GuiButton_createShown (form, x, x + BUTTON_WIDTH, -6 - Machine_getScrollBarWidth (), -4,
			L"sel", gui_button_cb_zoomToSelection, this, 0);
		x += BUTTON_WIDTH + BUTTON_SPACING;
		GuiButton_createShown (form, x, x + BUTTON_WIDTH, -6 - Machine_getScrollBarWidth (), -4,
			L"bak", gui_button_cb_zoomBack, this, 0);
	#endif

	/***** Create scroll bar. *****/

	#if gtk
		GtkObject *adj = gtk_adjustment_new (1, 1, maximumScrollBarValue, 1, 1, maximumScrollBarValue - 1);
		scrollBar = gtk_hscrollbar_new (GTK_ADJUSTMENT (adj));
		g_signal_connect (G_OBJECT (scrollBar), "value-changed", G_CALLBACK (gui_cb_scroll), this);
		GuiObject_show (scrollBar);
		gtk_box_pack_start (GTK_BOX (hctl_box), GTK_WIDGET (scrollBar), TRUE, TRUE, 3);
	#elif motif
		scrollBar = XtVaCreateManagedWidget ("scrollBar",
			xmScrollBarWidgetClass, form,
			XmNorientation, XmHORIZONTAL,
			XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, x += BUTTON_WIDTH + BUTTON_SPACING,
			XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 80 + BUTTON_SPACING,
			XmNbottomAttachment, XmATTACH_FORM,
			XmNheight, Machine_getScrollBarWidth (),
			XmNminimum, 1,
			XmNmaximum, maximumScrollBarValue,
			XmNvalue, 1,
			XmNsliderSize, maximumScrollBarValue - 1,
			NULL);
		XtAddCallback (scrollBar, XmNvalueChangedCallback, gui_cb_scroll, (XtPointer) this);
		XtAddCallback (scrollBar, XmNdragCallback, gui_cb_scroll, (XtPointer) this);
	#endif

	/***** Create Group button. *****/

	#if gtk
		groupButton = GuiCheckButton_create (NULL, 0, 0, 0, 0, L"Group",
			gui_checkbutton_cb_group, this, group_equalDomain (tmin, tmax) ? GuiCheckButton_SET : 0);
		gtk_box_pack_start (GTK_BOX (hctl_box), GTK_WIDGET (groupButton), FALSE, FALSE, 0);
		gtk_widget_show_all (GTK_WIDGET (hctl_box));
	#else
		groupButton = GuiCheckButton_createShown (form, -80, 0, - Machine_getScrollBarWidth () - 5, -4,
			L"Group", gui_checkbutton_cb_group, this, group_equalDomain (tmin, tmax) ? GuiCheckButton_SET : 0);
	#endif

	/***** Create optional text field. *****/

	if (v_hasText ()) {
		#if gtk
			text = GuiText_create (NULL, 0, 0, 0, TEXT_HEIGHT, GuiText_WORDWRAP | GuiText_MULTILINE);
			gtk_box_pack_start (GTK_BOX (form), GTK_WIDGET (text), FALSE, FALSE, 3);
			GuiObject_show (text);
		#else
			text = GuiText_createShown (form, 0, 0, 0, TEXT_HEIGHT, GuiText_WORDWRAP | GuiText_MULTILINE);
		#endif
		/*
		 * X Toolkit 4:184,461 says: "you should never call XtSetKeyboardFocus",
		 * "since it interferes with the keyboard traversal code".
		 * That's true, we needed to switch traversal off for 'form' (see above).
		 * But does anyone know of an alternative?
		 * Our simple and natural desire is that all keyboard input shall go to the only text widget
		 * in the window (in our Motif emulator, this is the automatic behaviour).
		 */
		#if gtk
			gtk_widget_grab_focus (GTK_WIDGET (text));   // BUG: can hardly be correct (the text should grab the focus of the window, not the global focus)
		#elif motif && defined (UNIX)
			XtSetKeyboardFocus (form, text);
		#endif
	}

	/***** Create drawing area. *****/

	#if gtk
		drawingArea = GuiDrawingArea_create (NULL, 0, 0, v_hasText () ? TEXT_HEIGHT : 0, - Machine_getScrollBarWidth () - 9,
			gui_drawingarea_cb_expose, gui_drawingarea_cb_click, gui_drawingarea_cb_key, gui_drawingarea_cb_resize, this, 0);
		
		// turn off double-buffering, otherwise the reaction to the expose-events gets
		// delayed by one event (TODO: figure out, why)
		gtk_widget_set_double_buffered (GTK_WIDGET (drawingArea), FALSE);
		
		// turn off clearing window to background colour before an expose event
		// gtk_widget_set_app_paintable (GTK_WIDGET (drawingArea), FALSE);
		
		gtk_box_pack_start (GTK_BOX (form), GTK_WIDGET (drawingArea), TRUE, TRUE, 0);
		GuiObject_show (drawingArea);
	#else
		drawingArea = GuiDrawingArea_createShown (form, 0, 0, v_hasText () ? TEXT_HEIGHT : 0, - Machine_getScrollBarWidth () - 9,
			gui_drawingarea_cb_expose, gui_drawingarea_cb_click, gui_drawingarea_cb_key, gui_drawingarea_cb_resize, this, 0);
	#endif

	GuiObject_show (form);
}

void structFunctionEditor :: v_dataChanged () {
	Function function = (Function) data;
	Melder_assert (Thing_member (function, classFunction));
	tmin = function -> xmin;
 	tmax = function -> xmax;
 	if (startWindow < tmin || startWindow > tmax) startWindow = tmin;
 	if (endWindow < tmin || endWindow > tmax) endWindow = tmax;
 	if (startWindow >= endWindow) { startWindow = tmin; endWindow = tmax; }
 	if (startSelection < tmin) startSelection = tmin;
 	if (startSelection > tmax) startSelection = tmax;
 	if (endSelection < tmin) endSelection = tmin;
 	if (endSelection > tmax) endSelection = tmax;
	FunctionEditor_marksChanged (this);
}

static void drawWhileDragging (FunctionEditor me, double x1, double x2) {
	/*
	 * We must draw this within the window, because the window tends to have a white background.
	 * We cannot draw this in the margins, because these tend to be grey, so that Graphics_xorOn does not work properly.
	 * We draw the text twice, because we expect that not ALL of the window is white...
	 */
	double xleft, xright;
	if (x1 > x2) xleft = x2, xright = x1; else xleft = x1, xright = x2;
	Graphics_xorOn (my graphics, Graphics_MAROON);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_TOP);
	Graphics_text1 (my graphics, xleft, 1.0, Melder_fixed (xleft, 6));
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_TOP);
	Graphics_text1 (my graphics, xright, 1.0, Melder_fixed (xright, 6));
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_BOTTOM);
	Graphics_text1 (my graphics, xleft, 0.0, Melder_fixed (xleft, 6));
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_BOTTOM);
	Graphics_text1 (my graphics, xright, 0.0, Melder_fixed (xright, 6));
	Graphics_xorOff (my graphics);
}

int structFunctionEditor :: v_click (double xbegin, double ybegin, bool shiftKeyPressed) {
	bool drag = false;
	double x = xbegin, y = ybegin, x1, x2;
	/*
	 * The 'anchor' is the point that will stay fixed during dragging.
	 * For instance, if she clicks and drags to the right,
	 * the location at which she originally clicked will be the anchor,
	 * even if she later chooses to drag the mouse to the left of it.
	 * Another example: if she shift-clicks near E, B will become (and stay) the anchor.
	 */
	double anchorForDragging = xbegin;   // the default (for if the shift key isn't pressed)
	Graphics_setWindow (graphics, startWindow, endWindow, 0, 1);
	if (shiftKeyPressed) {
		/*
		 * Extend the selection.
		 * We should always end up with a real selection (B < E),
		 * even if we start with the reversed temporal order (E < B).
		 */
		bool reversed = startSelection > endSelection;
		double firstMark = reversed ? endSelection : startSelection;
		double secondMark = reversed ? startSelection : endSelection;
		/*
		 * Undraw the old selection.
		 */
		if (endSelection > startSelection) {
			/*
			 * Determine the visible part of the old selection.
			 */
			double startVisible = startSelection > startWindow ? startSelection : startWindow;
			double endVisible = endSelection < endWindow ? endSelection : endWindow;
			/*
			 * Undraw the visible part of the old selection.
			 */
			if (endVisible > startVisible)
				v_unhighlightSelection (startVisible, endVisible, 0, 1);
		}
		if (xbegin >= secondMark) {
		 	/*
			 * She clicked right from the second mark (usually E). We move E.
			 */
			endSelection = xbegin;
			anchorForDragging = startSelection;
		} else if (xbegin <= firstMark) {
		 	/*
			 * She clicked left from the first mark (usually B). We move B.
			 */
			startSelection = xbegin;
			anchorForDragging = endSelection;
		} else {
			/*
			 * She clicked in between the two marks. We move the nearest mark.
			 */
			double distanceOfClickToFirstMark = fabs (xbegin - firstMark);
			double distanceOfClickToSecondMark = fabs (xbegin - secondMark);
			/*
			 * We make sure that the marks are in the unmarked B - E order.
			 */
			if (reversed) {
				/*
				 * Swap B and E.
				 */
				startSelection = firstMark;
				endSelection = secondMark;
			}
			/*
			 * Move the nearest mark.
			 */
			if (distanceOfClickToFirstMark < distanceOfClickToSecondMark) {
				startSelection = xbegin;
				anchorForDragging = endSelection;
			} else {
				endSelection = xbegin;
				anchorForDragging = startSelection;
			}
		}
		/*
		 * Draw the new selection.
		 */
		if (endSelection > startSelection) {
			/*
			 * Determine the visible part of the new selection.
			 */
			double startVisible = startSelection > startWindow ? startSelection : startWindow;
			double endVisible = endSelection < endWindow ? endSelection : endWindow;
			/*
			 * Draw the visible part of the new selection.
			 */
			if (endVisible > startVisible)
				v_highlightSelection (startVisible, endVisible, 0, 1);
		}
	}
	/*
	 * Find out whether this is a click or a drag.
	 */
	while (Graphics_mouseStillDown (graphics)) {
		Graphics_getMouseLocation (graphics, & x, & y);
		if (x < startWindow) x = startWindow;
		if (x > endWindow) x = endWindow;
		if (fabs (Graphics_dxWCtoMM (graphics, x - xbegin)) > 1.5) {
			drag = true;
			break;
		}
	}
	if (drag) {
		/*
		 * First undraw the old selection.
		 */
		if (endSelection > startSelection) {
			/*
			 * Determine the visible part of the old selection.
			 */
			double startVisible = startSelection > startWindow ? startSelection : startWindow;
			double endVisible = endSelection < endWindow ? endSelection : endWindow;
			/*
			 * Undraw the visible part of the old selection.
			 */
			if (endVisible > startVisible)
				v_unhighlightSelection (startVisible, endVisible, 0, 1);
		}
		/*
		 * Draw the text at least once.
		 */
		/*if (x < startWindow) x = startWindow; else if (x > endWindow) x = endWindow;*/
		drawWhileDragging (this, anchorForDragging, x);
		/*
		 * Draw the dragged selection at least once.
		 */
		if (x > anchorForDragging) x1 = anchorForDragging, x2 = x; else x1 = x, x2 = anchorForDragging;
		v_highlightSelection (x1, x2, 0, 1);
		/*
		 * Drag for the new selection.
		 */
		while (Graphics_mouseStillDown (graphics)) {
			double xold = x, x1, x2;
			Graphics_getMouseLocation (graphics, & x, & y);
			/*
			 * Clip to the visible window. Ideally, we should perform autoscrolling instead, though...
			 */
			if (x < startWindow) x = startWindow; else if (x > endWindow) x = endWindow;
			if (x == xold)
				continue;
			/*
			 * Undraw and redraw the text at the top.
			 */
			drawWhileDragging (this, anchorForDragging, xold);
			/*
			 * Remove previous dragged selection.
			 */
			if (xold > anchorForDragging) x1 = anchorForDragging, x2 = xold; else x1 = xold, x2 = anchorForDragging;
			if (x1 != x2) v_unhighlightSelection (x1, x2, 0, 1);
			/*
			 * Draw new dragged selection.
			 */
			if (x > anchorForDragging) x1 = anchorForDragging, x2 = x; else x1 = x, x2 = anchorForDragging;
			if (x1 != x2) v_highlightSelection (x1, x2, 0, 1);
			/*
			 * Redraw the text at the top.
			 */
			drawWhileDragging (this, anchorForDragging, x);
		} ;
		/*
		 * Set the new selection.
		 */
		if (x > anchorForDragging) startSelection = anchorForDragging, endSelection = x;
		else startSelection = x, endSelection = anchorForDragging;
	} else if (! shiftKeyPressed) {
		/*
		 * Move the cursor to the clicked position.
		 */
		startSelection = endSelection = xbegin;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

int structFunctionEditor :: v_clickB (double xWC, double yWC) {
	(void) yWC;
	startSelection = xWC;
	if (startSelection > endSelection) {
		double dummy = startSelection;
		startSelection = endSelection;
		endSelection = dummy;
	}
	return 1;
}

int structFunctionEditor :: v_clickE (double xWC, double yWC) {
	endSelection = xWC;
	(void) yWC;
	if (startSelection > endSelection) {
		double dummy = startSelection;
		startSelection = endSelection;
		endSelection = dummy;
	}
	return 1;
}

void FunctionEditor_insetViewport (FunctionEditor me) {
	Graphics_setViewport (my graphics, MARGIN, my width - MARGIN,
		BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0, 1);
}

int structFunctionEditor :: v_playCallback (int phase, double a_tmin, double a_tmax, double t) {
	/*
	 * This callback will often be called by the Melder workproc during playback.
	 * However, it will sometimes be called by Melder_stopPlaying with phase=3.
	 * This will occur at unpredictable times, perhaps when the LongSound is updated.
	 * So we had better make no assumptions about the current viewport.
	 */
	double x1NDC, x2NDC, y1NDC, y2NDC;
	(void) a_tmin;
	Graphics_inqViewport (graphics, & x1NDC, & x2NDC, & y1NDC, & y2NDC);
	FunctionEditor_insetViewport (this);
	Graphics_xorOn (graphics, Graphics_MAROON);
	/*
	 * Undraw the play cursor at its old location.
	 * BUG: during scrolling, zooming, and exposure, an ugly line may remain.
	 */
	if (phase != 1 && playCursor >= startWindow && playCursor <= endWindow) {
		Graphics_setLineWidth (graphics, 3.0);
		Graphics_line (graphics, playCursor, 0, playCursor, 1);
		Graphics_setLineWidth (graphics, 1.0);
	}
	/*
	 * Draw the play cursor at its new location.
	 */
	if (phase != 3 && t >= startWindow && t <= endWindow) {
		Graphics_setLineWidth (graphics, 3.0);
		Graphics_line (graphics, t, 0, t, 1);
		Graphics_setLineWidth (graphics, 1.0);
	}
	Graphics_xorOff (graphics);
	/*
	 * Usually, there will be an event test after each invocation of this callback,
	 * because the asynchronicity is kMelder_asynchronicityLevel_INTERRUPTABLE or kMelder_asynchronicityLevel_ASYNCHRONOUS.
	 * However, if the asynchronicity is just kMelder_asynchronicityLevel_CALLING_BACK,
	 * there is no event test. Which means: no server round trip.
	 * Which means: no automatic flushing of graphics output.
	 * So: we force the flushing ourselves, lest we see too few moving cursors.
	 */
	Graphics_flushWs (graphics);
	Graphics_setViewport (graphics, x1NDC, x2NDC, y1NDC, y2NDC);
	playCursor = t;
	if (phase == 3) {
		if (t < a_tmax && MelderAudio_stopWasExplicit ()) {
			if (t > startSelection && t < endSelection)
				startSelection = t;
			else
				startSelection = endSelection = t;
			v_updateText ();
			/*Graphics_updateWs (graphics);*/ drawNow (this);
			updateGroup (this);
		}
		playingCursor = false;
		playingSelection = false;
	}
	return 1;
}

int theFunctionEditor_playCallback (void *void_me, int phase, double a_tmin, double a_tmax, double t) {
	iam (FunctionEditor);
	return my v_playCallback (phase, a_tmin, a_tmax, t);
}

void structFunctionEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	Graphics_highlight (graphics, left, right, bottom, top);
}

void structFunctionEditor :: v_unhighlightSelection (double left, double right, double bottom, double top) {
	Graphics_unhighlight (graphics, left, right, bottom, top);
}

void FunctionEditor_init (FunctionEditor me, GuiObject parent, const wchar *title, Function data) {
	my tmin = data -> xmin;   // set before adding children (see group button)
	my tmax = data -> xmax;
	Editor_init (me, parent, 0, 0, preferences.shellWidth, preferences.shellHeight, title, data);

	my startWindow = my tmin;
	my endWindow = my tmax;
	my startSelection = my endSelection = 0.5 * (my tmin + my tmax);
	#if motif
		Melder_assert (XtWindow (my drawingArea));
	#endif
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setFontSize (my graphics, 12);

// This exdents because it's a hack:
struct structGuiDrawingAreaResizeEvent event = { my drawingArea, 0 };
event. width = GuiObject_getWidth (my drawingArea);
event. height = GuiObject_getHeight (my drawingArea);
gui_drawingarea_cb_resize (me, & event);

	my v_updateText ();
	if (group_equalDomain (my tmin, my tmax))
		gui_checkbutton_cb_group (me, NULL);   // BUG: NULL
	my enableUpdates = true;
	my arrowScrollStep = preferences.arrowScrollStep;
}

void FunctionEditor_marksChanged (FunctionEditor me) {
	my v_updateText ();
	updateScrollBar (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
}

void FunctionEditor_updateText (FunctionEditor me) {
	my v_updateText ();
}

void FunctionEditor_redraw (FunctionEditor me) {
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
}

void FunctionEditor_enableUpdates (FunctionEditor me, bool enable) {
	my enableUpdates = enable;
}

void FunctionEditor_ungroup (FunctionEditor me) {
	if (! my group) return;
	my group = false;
	GuiCheckButton_setValue (my groupButton, false);
	int i = 1;
	while (theGroup [i] != me) i ++;
	theGroup [i] = NULL;
	nGroup --;
	my v_updateText ();
	Graphics_updateWs (my graphics);   // for setting buttons in v_draw() method
}

void FunctionEditor_drawRangeMark (FunctionEditor me, double yWC, const wchar_t *yWC_string, const wchar_t *units, int verticalAlignment) {
	static MelderString text = { 0 };
	MelderString_empty (& text);
	MelderString_append (& text, yWC_string, units);
	double textWidth = Graphics_textWidth (my graphics, text.string) + Graphics_dxMMtoWC (my graphics, 0.5);
	Graphics_setColour (my graphics, Graphics_BLUE);
	Graphics_line (my graphics, my endWindow, yWC, my endWindow + textWidth, yWC);
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, verticalAlignment);
	if (verticalAlignment == Graphics_BOTTOM) yWC -= Graphics_dyMMtoWC (my graphics, 0.5);
	Graphics_text (my graphics, my endWindow, yWC, text.string);
}

void FunctionEditor_drawCursorFunctionValue (FunctionEditor me, double yWC, const wchar_t *yWC_string, const wchar_t *units) {
	Graphics_setColour (my graphics, Graphics_CYAN);
	Graphics_line (my graphics, my startWindow, yWC, 0.99 * my startWindow + 0.01 * my endWindow, yWC);
	Graphics_fillCircle_mm (my graphics, 0.5 * (my startSelection + my endSelection), yWC, 1.5);
	Graphics_setColour (my graphics, Graphics_BLUE);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text2 (my graphics, my startWindow, yWC, yWC_string, units);
}

void FunctionEditor_insertCursorFunctionValue (FunctionEditor me, double yWC, const wchar_t *yWC_string, const wchar_t *units, double minimum, double maximum) {
	static MelderString text = { 0 };
	MelderString_empty (& text);
	MelderString_append (& text, yWC_string, units);
	double textX = my endWindow, textY = yWC, textWidth;
	int tooHigh = Graphics_dyWCtoMM (my graphics, maximum - textY) < 5.0;
	int tooLow = Graphics_dyWCtoMM (my graphics, textY - minimum) < 5.0;
	if (yWC < minimum || yWC > maximum) return;
	Graphics_setColour (my graphics, Graphics_CYAN);
	Graphics_line (my graphics, 0.99 * my endWindow + 0.01 * my startWindow, yWC, my endWindow, yWC);
	Graphics_fillCircle_mm (my graphics, 0.5 * (my startSelection + my endSelection), yWC, 1.5);
	if (tooHigh) {
		if (tooLow) textY = 0.5 * (minimum + maximum);
		else textY = maximum - Graphics_dyMMtoWC (my graphics, 5.0);
	} else if (tooLow) {
		textY = minimum + Graphics_dyMMtoWC (my graphics, 5.0);
	}
	textWidth = Graphics_textWidth (my graphics, text.string);
	Graphics_fillCircle_mm (my graphics, my endWindow + textWidth + Graphics_dxMMtoWC (my graphics, 1.5), textY, 1.5);
	Graphics_setColour (my graphics, Graphics_RED);
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text (my graphics, textX, textY, text.string);
}

void FunctionEditor_drawHorizontalHair (FunctionEditor me, double yWC, const wchar_t *yWC_string, const wchar_t *units) {
	Graphics_setColour (my graphics, Graphics_RED);
	Graphics_line (my graphics, my startWindow, yWC, my endWindow, yWC);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text2 (my graphics, my startWindow, yWC, yWC_string, units);
}

void FunctionEditor_drawGridLine (FunctionEditor me, double yWC) {
	Graphics_setColour (my graphics, Graphics_CYAN);
	Graphics_setLineType (my graphics, Graphics_DOTTED);
	Graphics_line (my graphics, my startWindow, yWC, my endWindow, yWC);
	Graphics_setLineType (my graphics, Graphics_DRAWN);
}

void FunctionEditor_garnish (FunctionEditor me) {
	if (preferences.picture.drawSelectionTimes) {
		if (my startSelection >= my startWindow && my startSelection <= my endWindow)
			Graphics_markTop (my pictureGraphics, my startSelection, true, true, false, NULL);
		if (my endSelection != my startSelection && my endSelection >= my startWindow && my endSelection <= my endWindow)
			Graphics_markTop (my pictureGraphics, my endSelection, true, true, false, NULL);
	}
	if (preferences.picture.drawSelectionHairs) {
		if (my startSelection >= my startWindow && my startSelection <= my endWindow)
			Graphics_markTop (my pictureGraphics, my startSelection, false, false, true, NULL);
		if (my endSelection != my startSelection && my endSelection >= my startWindow && my endSelection <= my endWindow)
			Graphics_markTop (my pictureGraphics, my endSelection, false, false, true, NULL);
	}
}

/* End of file FunctionEditor.c */
