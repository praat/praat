/* FunctionEditor.cpp
 *
 * Copyright (C) 1992-2021 Paul Boersma
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

#include "FunctionEditor.h"
#include "machine.h"
#include "EditorM.h"
#include "GuiP.h"

Thing_implement (FunctionEditor, Editor, 0);

#include "prefs_define.h"
#include "FunctionEditor_prefs.h"
#include "prefs_install.h"
#include "FunctionEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "FunctionEditor_prefs.h"
#include "GraphicsP.h"

namespace {
	constexpr double maximumScrollBarValue = 2'000'000'000.0;
	constexpr double RELATIVE_PAGE_INCREMENT = 0.8;
	constexpr double SCROLL_INCREMENT_FRACTION = 20.0;
	constexpr int TEXT_HEIGHT = 50;
	constexpr int BUTTON_X = 3;
	constexpr int BUTTON_WIDTH = 40;
	constexpr int BUTTON_SPACING = 8;

	constexpr integer THE_MAXIMUM_GROUP_SIZE = 100;
	integer theGroupSize = 0;
	FunctionEditor theGroupMembers [1 + THE_MAXIMUM_GROUP_SIZE];
}

static bool group_equalDomain (double tmin, double tmax) {
	if (theGroupSize == 0)
		return true;
	for (integer i = 1; i <= THE_MAXIMUM_GROUP_SIZE; i ++)
		if (theGroupMembers [i])
			return ( tmin == theGroupMembers [i] -> tmin && tmax == theGroupMembers [i] -> tmax );
	return false;   // should not occur
}

static void updateScrollBar (FunctionEditor me) {
/* We cannot call this immediately after creation. */
	const double slider_size = Melder_clippedLeft (1.0, (my endWindow - my startWindow) / (my tmax - my tmin) * maximumScrollBarValue - 1.0);
	const double value = Melder_clipped (1.0, (my startWindow - my tmin) / (my tmax - my tmin) * maximumScrollBarValue + 1.0, maximumScrollBarValue - slider_size);
	const double increment = slider_size / SCROLL_INCREMENT_FRACTION + 1.0;
	const double page_increment = RELATIVE_PAGE_INCREMENT * slider_size + 1.0;
	GuiScrollBar_set (my scrollBar, undefined, maximumScrollBarValue, value, slider_size, increment, page_increment);
}

static void updateGroup (FunctionEditor me) {
	if (! my group)
		return;
	for (integer i = 1; i <= THE_MAXIMUM_GROUP_SIZE; i ++) {
		if (theGroupMembers [i] && theGroupMembers [i] != me) {
			FunctionEditor thee = theGroupMembers [i];
			if (my pref_synchronizedZoomAndScroll()) {
				thy startWindow = my startWindow;
				thy endWindow = my endWindow;
			}
			thy startSelection = my startSelection;
			thy endSelection = my endSelection;
			FunctionEditor_updateText (thee);
			updateScrollBar (thee);
			FunctionEditor_redraw (thee);
		}
	}
}

static void drawBackgroundAndData (FunctionEditor me) {
	if (Melder_debug == 55)
		Melder_casual (Thing_messageNameAndAddress (me), U" draw");
	const bool leftFromWindow = ( my startWindow > my tmin );
	const bool rightFromWindow = ( my endWindow < my tmax );
	const bool cursorIsVisible = ( my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow );
	const bool selectionIsNonempty = ( my endSelection > my startSelection );

	/*
		Update selection.
	*/
	const bool startIsVisible = ( my startSelection > my startWindow && my startSelection < my endWindow );
	const bool endIsVisible = ( my endSelection > my startWindow && my endSelection < my endWindow );

	/*
		Update markers.
	*/
	my numberOfMarkers = 0;
	if (startIsVisible)
		my marker [++ my numberOfMarkers] = my startSelection;
	if (endIsVisible && my endSelection != my startSelection)
		my marker [++ my numberOfMarkers] = my endSelection;
	my marker [++ my numberOfMarkers] = my endWindow;
	sort_VEC_inout (VEC (& my marker [1], my numberOfMarkers));

	/*
		Update rectangles.
	*/

	for (integer i = 0; i < 8; i++)
		my rect [i]. left = my rect [i]. right = 0;

	/*
		0: rectangle for total.
	*/
	my rect [0]. left = my _functionViewerLeft + ( leftFromWindow ? 0 : my MARGIN );
	my rect [0]. right = my _functionViewerRight - ( rightFromWindow ? 0 : my MARGIN );
	my rect [0]. bottom = my BOTTOM_MARGIN;
	my rect [0]. top = my BOTTOM_MARGIN + my space;

	/*
		1: rectangle for visible part.
	*/
	my rect [1]. left = my _functionViewerLeft + my MARGIN;
	my rect [1]. right = my _functionViewerRight - my MARGIN;
	my rect [1]. bottom = my BOTTOM_MARGIN + my space;
	my rect [1]. top = my BOTTOM_MARGIN + my space * ( my numberOfMarkers > 1 ? 2 : 3 );

	/*
		2: rectangle for left from visible part.
	*/
	if (leftFromWindow) {
		my rect [2]. left = my _functionViewerLeft;
		my rect [2]. right = my _functionViewerLeft + my MARGIN;
		my rect [2]. bottom = my BOTTOM_MARGIN + my space;
		my rect [2]. top = my BOTTOM_MARGIN + my space * 2;
	}

	/*
		3: rectangle for right from visible part.
	*/
	if (rightFromWindow) {
		my rect [3]. left = my _functionViewerRight - my MARGIN;
		my rect [3]. right = my _functionViewerRight;
		my rect [3]. bottom = my BOTTOM_MARGIN + my space;
		my rect [3]. top = my BOTTOM_MARGIN + my space * 2;
	}

	/*
		4, 5, 6: rectangles between markers visible in visible part.
	*/
	if (my numberOfMarkers > 1) {
		const double window = my endWindow - my startWindow;
		for (integer i = 1; i <= my numberOfMarkers; i ++) {
			my rect [3 + i]. left = i == 1 ? my _functionViewerLeft + my MARGIN : my _functionViewerLeft + my MARGIN + (my _functionViewerRight - my _functionViewerLeft - my MARGIN * 2) *
				(my marker [i - 1] - my startWindow) / window;
			my rect [3 + i]. right = my _functionViewerLeft + my MARGIN + (my _functionViewerRight - my _functionViewerLeft - my MARGIN * 2) *
				(my marker [i] - my startWindow) / window;
			my rect [3 + i]. bottom = my BOTTOM_MARGIN + my space * 2;
			my rect [3 + i]. top = my BOTTOM_MARGIN + my space * 3;
		}
	}

	if (selectionIsNonempty) {
		const double window = my endWindow - my startWindow;
		const double left =
			my startSelection == my startWindow ? my _functionViewerLeft + my MARGIN :
			my startSelection == my tmin ? my _functionViewerLeft :
			my startSelection < my startWindow ? my _functionViewerLeft + my MARGIN * 0.3 :
			my startSelection < my endWindow ? my _functionViewerLeft + my MARGIN + (my _functionViewerRight - my _functionViewerLeft - my MARGIN * 2) * (my startSelection - my startWindow) / window :
			my startSelection == my endWindow ? my _functionViewerRight - my MARGIN : my _functionViewerRight - my MARGIN * 0.7;
		const double right =
			my endSelection < my startWindow ? my _functionViewerLeft + my MARGIN * 0.7 :
			my endSelection == my startWindow ? my _functionViewerLeft + my MARGIN :
			my endSelection < my endWindow ? my _functionViewerLeft + my MARGIN + (my _functionViewerRight - my _functionViewerLeft - my MARGIN * 2) * (my endSelection - my startWindow) / window :
			my endSelection == my endWindow ? my _functionViewerRight - my MARGIN :
			my endSelection < my tmax ? my _functionViewerRight - my MARGIN * 0.3 : my _functionViewerRight;
		my rect [7]. left = left;
		my rect [7]. right = right;
		my rect [7]. bottom = my height_pxlt - my space - my TOP_MARGIN;
		my rect [7]. top = my height_pxlt - my TOP_MARGIN;
	}

	/*
		Window background.
	*/
	my viewAllAsPixelettes ();
	Graphics_setColour (my graphics.get(), Melder_WINDOW_BACKGROUND_COLOUR);
	Graphics_fillRectangle (my graphics.get(), my _functionViewerLeft, my _selectionViewerRight, my BOTTOM_MARGIN, my height_pxlt);
	Graphics_setColour (my graphics.get(), Melder_BLACK);

	my viewFunctionViewerAsPixelettes ();
	Graphics_setFont (my graphics.get(), kGraphics_font :: HELVETICA);
	Graphics_setFontSize (my graphics.get(), 12.0);
	Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
	for (integer i = 0; i < 8; i ++) {
		const double left = my rect [i]. left, right = my rect [i]. right;
		if (left < right)
			Graphics_button (my graphics.get(), left, right, my rect [i]. bottom, my rect [i]. top);
	}
	const double verticalCorrection = my height_pxlt / (my height_pxlt - 111.0 + 11.0)
		#ifdef _WIN32
			* 1.5
		#endif
	;
	for (integer i = 0; i < 8; i ++) {
		const double left = my rect [i]. left, right = my rect [i]. right;
		const double bottom = my rect [i]. bottom, top = my rect [i]. top;
		if (left < right) {
			conststring8 format = my v_format_long ();
			double value = undefined, inverseValue = 0.0;
			switch (i) {
				case 0: {
					format = my v_format_totalDuration ();
					value = my tmax - my tmin;
				} break; case 1: {
					format = my v_format_window ();
					value = my endWindow - my startWindow;
					/*
						Window domain text.
					*/
					Graphics_setColour (my graphics.get(), Melder_BLUE);
					Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_HALF);
					Graphics_text (my graphics.get(), left, 0.5 * (bottom + top) - verticalCorrection,
							Melder_fixed (my startWindow, my v_fixedPrecision_long ()));
					Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_HALF);
					Graphics_text (my graphics.get(), right, 0.5 * (bottom + top) - verticalCorrection,
							Melder_fixed (my endWindow, my v_fixedPrecision_long ()));
					Graphics_setColour (my graphics.get(), Melder_BLACK);
					Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
				} break; case 2: {
					value = my startWindow - my tmin;
				} break; case 3: {
					value = my tmax - my endWindow;
				} break; case 4: {
					value = my marker [1] - my startWindow;
				} break; case 5: {
					value = my marker [2] - my marker [1];
				} break; case 6: {
					value = my marker [3] - my marker [2];
				} break; case 7: {
					format = my v_format_selection ();
					value = my endSelection - my startSelection;
					inverseValue = 1.0 / value;
				}
			}
			char text8 [100];
			snprintf (text8, 100, format, value, inverseValue);
			autostring32 text = Melder_8to32 (text8);
			if (Graphics_textWidth (my graphics.get(), text.get()) < right - left) {
				Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
			} else if (format == my v_format_long()) {
				snprintf (text8, 100, my v_format_short(), value);
				text = Melder_8to32 (text8);
				if (Graphics_textWidth (my graphics.get(), text.get()) < right - left)
					Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
			} else {
				snprintf (text8, 100, my v_format_long(), value);
				text = Melder_8to32 (text8);
				if (Graphics_textWidth (my graphics.get(), text.get()) < right - left) {
					Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
				} else {
					snprintf (text8, 100, my v_format_short(), my endSelection - my startSelection);
					text = Melder_8to32 (text8);
					if (Graphics_textWidth (my graphics.get(), text.get()) < right - left)
						Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
				}
			}
		}
	}

	my viewTallDataAsWorldByPixelettes ();

	/*
		Red marker text.
	*/
	Graphics_setColour (my graphics.get(), Melder_RED);
	if (cursorIsVisible) {
		Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (my graphics.get(), my startSelection, my height_pxlt - (my TOP_MARGIN + my space*0.9) - verticalCorrection * 7,
				Melder_fixed (my startSelection, my v_fixedPrecision_long()));
	}
	if (startIsVisible && selectionIsNonempty) {
		Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (my graphics.get(), my startSelection, my height_pxlt - (my TOP_MARGIN + my space/2) - verticalCorrection,
				Melder_fixed (my startSelection, my v_fixedPrecision_long()));
	}
	if (endIsVisible && selectionIsNonempty) {
		Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (my graphics.get(), my endSelection, my height_pxlt - (my TOP_MARGIN + my space/2) - verticalCorrection,
				Melder_fixed (my endSelection, my v_fixedPrecision_long()));
	}
	Graphics_setColour (my graphics.get(), Melder_BLACK);

	/*
		To reduce flashing, give our descendants the opportunity to prepare their data.
	*/
	my v_prepareDraw ();

	/*
		Start of inner drawing.
	*/
	my viewDataAsWorldByFraction ();
	my v_draw ();

	/*
		Red dotted marker lines.
	*/
	my viewDataAsWorldByFraction ();
	Graphics_setColour (my graphics.get(), Melder_RED);
	Graphics_setLineType (my graphics.get(), Graphics_DOTTED);
	const double bottom = my v_getBottomOfSoundAndAnalysisArea ();
	if (cursorIsVisible)
		Graphics_line (my graphics.get(), my startSelection, bottom, my startSelection, 1.0);
	if (startIsVisible)
		Graphics_line (my graphics.get(), my startSelection, bottom, my startSelection, 1.0);
	if (endIsVisible)
		Graphics_line (my graphics.get(), my endSelection, bottom, my endSelection, 1.0);
	Graphics_setColour (my graphics.get(), Melder_BLACK);
	Graphics_setLineType (my graphics.get(), Graphics_DRAWN);

	/*
		Highlight selection.
	*/
	if (selectionIsNonempty && my startSelection < my endWindow && my endSelection > my startWindow) {
		const double left = Melder_clippedLeft (my startWindow, my startSelection);
		const double right = Melder_clippedRight (my endSelection, my endWindow);
		my v_highlightSelection (left, right, 0.0, 1.0);
	}
}

/********** METHODS **********/

void structFunctionEditor :: v_destroy () noexcept {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	if (our group) {   // undangle
		integer i = 1;
		while (theGroupMembers [i] != this) {
			Melder_assert (i < THE_MAXIMUM_GROUP_SIZE);
			i ++;
		}
		theGroupMembers [i] = nullptr;
		theGroupSize --;
	}
	if (Melder_debug == 55)
		Melder_casual (Thing_messageNameAndAddress (this), U" v_destroy");
	FunctionEditor_Parent :: v_destroy ();
}

void structFunctionEditor :: v_info () {
	FunctionEditor_Parent :: v_info ();
	MelderInfo_writeLine (U"Editor start: ", our tmin, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Editor end: ", our tmax, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Window start: ", our startWindow, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Window end: ", our endWindow, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Selection start: ", our startSelection, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Selection end: ", our endSelection, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Arrow scroll step: ", our p_arrowScrollStep, U" ", v_format_units_long());
	MelderInfo_writeLine (U"Group: ", group ? U"yes" : U"no");
}

/********** FILE MENU **********/

static void gui_drawingarea_cb_resize (FunctionEditor me, GuiDrawingArea_ResizeEvent event) {
	if (! my graphics)
		return;   // could be the case in the very beginning
	my updateGeometry (event -> width, event -> height);
	FunctionEditor_redraw (me);
	/*
		Save the current shell size as the user's preference for a new FunctionEditor.
	*/
	my pref_shellWidth()  = GuiShell_getShellWidth  (my windowForm);
	my pref_shellHeight() = GuiShell_getShellHeight (my windowForm);
}

static void menu_cb_preferences (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Preferences", nullptr)
		BOOLEAN (synchronizeZoomAndScroll, U"Synchronize zoom and scroll", my default_synchronizedZoomAndScroll())
		BOOLEAN (showSelectionViewer, Melder_cat (U"Show ", my v_selectionViewerName()), my default_showSelectionViewer())
		POSITIVE (arrowScrollStep, Melder_cat (U"Arrow scroll step (", my v_format_units_short(), U")"), my default_arrowScrollStep())
		my v_prefs_addFields (cmd);
	EDITOR_OK
		SET_BOOLEAN (synchronizeZoomAndScroll, my pref_synchronizedZoomAndScroll())
		SET_BOOLEAN (showSelectionViewer, my pref_showSelectionViewer ())
		SET_REAL (arrowScrollStep, my p_arrowScrollStep)
		my v_prefs_setValues (cmd);
	EDITOR_DO
		const bool oldSynchronizedZoomAndScroll = my pref_synchronizedZoomAndScroll();
		const bool oldShowSelectionViewer = my p_showSelectionViewer;
		my pref_synchronizedZoomAndScroll() = synchronizeZoomAndScroll;
		my pref_showSelectionViewer() = my p_showSelectionViewer = showSelectionViewer;
		my pref_arrowScrollStep() = my p_arrowScrollStep = arrowScrollStep;
		if (my p_showSelectionViewer != oldShowSelectionViewer)
			my updateGeometry (GuiControl_getWidth  (my drawingArea), GuiControl_getHeight (my drawingArea));
		if (! oldSynchronizedZoomAndScroll && my pref_synchronizedZoomAndScroll())
			updateGroup (me);
		my v_prefs_getValues (cmd);
	EDITOR_END
}

static bool v_form_pictureSelection_drawSelectionTimes;
static bool v_form_pictureSelection_drawSelectionHairs;
void structFunctionEditor :: v_form_pictureSelection (EditorCommand cmd) {
	UiForm_addBoolean (cmd -> d_uiform.get(), & v_form_pictureSelection_drawSelectionTimes, nullptr, U"Draw selection times", true);
	UiForm_addBoolean (cmd -> d_uiform.get(), & v_form_pictureSelection_drawSelectionHairs, nullptr, U"Draw selection hairs", true);
}
void structFunctionEditor :: v_ok_pictureSelection (EditorCommand cmd) {
	FunctionEditor me = (FunctionEditor) cmd -> d_editor;
	SET_BOOLEAN (v_form_pictureSelection_drawSelectionTimes, my pref_picture_drawSelectionTimes())
	SET_BOOLEAN (v_form_pictureSelection_drawSelectionHairs, my pref_picture_drawSelectionHairs())
}
void structFunctionEditor :: v_do_pictureSelection (EditorCommand cmd) {
	FunctionEditor me = (FunctionEditor) cmd -> d_editor;
	my pref_picture_drawSelectionTimes() = v_form_pictureSelection_drawSelectionTimes;
	my pref_picture_drawSelectionHairs() = v_form_pictureSelection_drawSelectionHairs;
}

/********** QUERY MENU **********/

static void menu_cb_getB (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	Melder_informationReal (my startSelection, my v_format_units_long());
}
static void menu_cb_getCursor (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	Melder_informationReal (0.5 * (my startSelection + my endSelection), my v_format_units_long());
}
static void menu_cb_getE (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	Melder_informationReal (my endSelection, my v_format_units_long());
}
static void menu_cb_getSelectionDuration (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	Melder_informationReal (my endSelection - my startSelection, my v_format_units_long());
}

/********** VIEW MENU **********/

static void menu_cb_zoom (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Zoom", nullptr)
		REAL (from, Melder_cat (U"From (", my v_format_units_short(), U")"), U"0.0")
		REAL (to,   Melder_cat (U"To (", my v_format_units_short(), U")"),   U"1.0")
	EDITOR_OK
		SET_REAL (from, my startWindow)
		SET_REAL (to,   my endWindow)
	EDITOR_DO
		Melder_require (to > from,
			U"“to” should be greater than “from”.");
		if (from < my tmin + 1e-12)
			from = my tmin;
		if (to > my tmax - 1e-12)
			to = my tmax;
		Melder_require (to > from,
			U"“to” should be greater than “from”.");
		my startWindow = from;
		my endWindow = to;
		my v_updateText ();
		updateScrollBar (me);
		FunctionEditor_redraw (me);
		updateGroup (me);
	EDITOR_END
}

static void do_showAll (FunctionEditor me) {
	my startWindow = my tmin;
	my endWindow = my tmax;
	my v_updateText ();
	updateScrollBar (me);
	FunctionEditor_redraw (me);
	if (my pref_synchronizedZoomAndScroll())
		updateGroup (me);
}

static void gui_button_cb_showAll (FunctionEditor me, GuiButtonEvent /* event */) {
	do_showAll (me);
}

static void do_zoomIn (FunctionEditor me) {
	const double shift = (my endWindow - my startWindow) / 4.0;
	my startWindow += shift;
	my endWindow -= shift;
	my v_updateText ();
	updateScrollBar (me);
	FunctionEditor_redraw (me);
	if (my pref_synchronizedZoomAndScroll())
		updateGroup (me);
}

static void gui_button_cb_zoomIn (FunctionEditor me, GuiButtonEvent /* event */) {
	do_zoomIn (me);
}

static void do_zoomOut (FunctionEditor me) {
	const double shift = (my endWindow - my startWindow) / 2.0;
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   // quickly, before window changes
	my startWindow -= shift;
	if (my startWindow < my tmin + 1e-12)
		my startWindow = my tmin;
	my endWindow += shift;
	if (my endWindow > my tmax - 1e-12)
		my endWindow = my tmax;
	my v_updateText ();
	updateScrollBar (me);
	FunctionEditor_redraw (me);
	if (my pref_synchronizedZoomAndScroll())
		updateGroup (me);
}

static void gui_button_cb_zoomOut (FunctionEditor me, GuiButtonEvent /*event*/) {
	do_zoomOut (me);
}

static void do_zoomToSelection (FunctionEditor me) {
	if (my endSelection > my startSelection) {
		my startZoomHistory = my startWindow;   // remember for Zoom Back
		my endZoomHistory = my endWindow;   // remember for Zoom Back
		my startWindow = my startSelection;
		my endWindow = my endSelection;
		my v_updateText ();
		updateScrollBar (me);
		FunctionEditor_redraw (me);
		if (my pref_synchronizedZoomAndScroll())
			updateGroup (me);
	}
}

static void gui_button_cb_zoomToSelection (FunctionEditor me, GuiButtonEvent /* event */) {
	do_zoomToSelection (me);
}

static void do_zoomBack (FunctionEditor me) {
	if (my endZoomHistory > my startZoomHistory) {
		my startWindow = my startZoomHistory;
		my endWindow = my endZoomHistory;
		my v_updateText ();
		updateScrollBar (me);
		FunctionEditor_redraw (me);
		if (my pref_synchronizedZoomAndScroll())
			updateGroup (me);
	}
}

static void gui_button_cb_zoomBack (FunctionEditor me, GuiButtonEvent /* event */) {
	do_zoomBack (me);
}

static void menu_cb_showAll (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	do_showAll (me);
}

static void menu_cb_zoomIn (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	do_zoomIn (me);
}

static void menu_cb_zoomOut (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	do_zoomOut (me);
}

static void menu_cb_zoomToSelection (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	do_zoomToSelection (me);
}

static void menu_cb_zoomBack (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	do_zoomBack (me);
}

static void menu_cb_play (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Play", nullptr)
		REAL (from, Melder_cat (U"From (", my v_format_units_short(), U")"), U"0.0")
		REAL (to,   Melder_cat (U"To (", my v_format_units_short(), U")"),   U"1.0")
	EDITOR_OK
		SET_REAL (from, my startWindow)
		SET_REAL (to,   my endWindow)
	EDITOR_DO
		MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
		my v_play (from, to);
	EDITOR_END
}

static void menu_cb_playOrStop (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	if (MelderAudio_isPlaying) {
		MelderAudio_stopPlaying (MelderAudio_EXPLICIT);
	} else if (my startSelection < my endSelection) {
		my v_play (my startSelection, my endSelection);
	} else {
		if (my startSelection == my endSelection && my startSelection > my startWindow && my startSelection < my endWindow)
			my v_play (my startSelection, my endWindow);
		else
			my v_play (my startWindow, my endWindow);
	}
}

static void menu_cb_playWindow (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	my v_play (my startWindow, my endWindow);
}

static void menu_cb_interruptPlaying (FunctionEditor /* me */, EDITOR_ARGS_DIRECT) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
}

/********** SELECT MENU **********/

static void menu_cb_select (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Select", nullptr)
		REAL (startOfSelection, Melder_cat (U"Start of selection (", my v_format_units_short(), U")"), U"0.0")
		REAL (endOfSelection,   Melder_cat (U"End of selection (",   my v_format_units_short(), U")"), U"1.0")
	EDITOR_OK
		SET_REAL (startOfSelection, my startSelection)
		SET_REAL (endOfSelection,   my endSelection)
	EDITOR_DO
		my startSelection = startOfSelection;
		if (my startSelection < my tmin + 1e-12)
			my startSelection = my tmin;
		my endSelection = endOfSelection;
		if (my endSelection > my tmax - 1e-12)
			my endSelection = my tmax;
		if (my startSelection > my endSelection)
			std::swap (my startSelection, my endSelection);
		my v_updateText ();
		FunctionEditor_redraw (me);
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_widenOrShrinkSelection (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Widen or shrink selection", nullptr)
		POSITIVE (newWidthOfSelection, Melder_cat (U"New width of selection (", my v_format_units_short(), U")"), U"0.3")
		RADIO_ENUM (kGraphics_horizontalAlignment, alignmentWithCurrentSelection, U"Alignment with current selection", kGraphics_horizontalAlignment::CENTRE)
	EDITOR_OK
	EDITOR_DO
		const double currentWidthOfSelection = my endSelection - my startSelection;
		const double degreeOfWidening = newWidthOfSelection - currentWidthOfSelection;
		double newStartOfSelection;
		switch (alignmentWithCurrentSelection) {
			case kGraphics_horizontalAlignment::LEFT: {
				newStartOfSelection = my startSelection;
			} break; case kGraphics_horizontalAlignment::CENTRE: {
				newStartOfSelection = my startSelection - 0.5 * degreeOfWidening;
			} break; case kGraphics_horizontalAlignment::RIGHT: {
				newStartOfSelection = my startSelection - degreeOfWidening;
			} break; case kGraphics_horizontalAlignment::UNDEFINED: {
				Melder_throw (U"Undefined alignment.");
			}
		}
		const double newEndOfSelection = newStartOfSelection + newWidthOfSelection;
		Melder_require (newStartOfSelection >= my tmin,
			U"Widening the selection to ", newWidthOfSelection, U" ", my v_format_units_long(),
			U" would move the start of the selection to ", newStartOfSelection, U" ", my v_format_units_long(),
			U", which lies before the start of the editor’s ", my v_domainName(),
			U" domain, which is at ", my tmin, U" ", my v_format_units_long(), U"."
		);
		Melder_require (newEndOfSelection <= my tmax,
			U"Widening the selection to ", newWidthOfSelection, U" ", my v_format_units_long(),
			U" would move the end of the selection to ", newEndOfSelection, U" ", my v_format_units_long(),
			U", which lies past the end of the editor’s ", my v_domainName(),
			U" domain, which is at ", my tmax, U" ", my v_format_units_long(), U"."
		);
		my startSelection = newStartOfSelection;
		my endSelection = newEndOfSelection;
		my v_updateText ();
		FunctionEditor_redraw (me);
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveCursorToStartOfSelection (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	my endSelection = my startSelection;
	my v_updateText ();
	FunctionEditor_redraw (me);
	updateGroup (me);
}

static void menu_cb_moveCursorToEndOfSelection (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	my startSelection = my endSelection;
	my v_updateText ();
	FunctionEditor_redraw (me);
	updateGroup (me);
}

static void menu_cb_moveCursorTo (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Move cursor to", nullptr)
		REAL (position, Melder_cat (U"Position (", my v_format_units_short(), U")"), U"0.0")
	EDITOR_OK
		SET_REAL (position, 0.5 * (my startSelection + my endSelection))
	EDITOR_DO
		if (position < my tmin + 1e-12)
			position = my tmin;
		if (position > my tmax - 1e-12)
			position = my tmax;
		my startSelection = my endSelection = position;
		my v_updateText ();
		FunctionEditor_redraw (me);
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveCursorBy (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Move cursor by", nullptr)
		REAL (distance, Melder_cat (U"Distance (", my v_format_units_short(), U")"), U"0.05")
	EDITOR_OK
	EDITOR_DO
		const double position = Melder_clipped (my tmin, 0.5 * (my startSelection + my endSelection) + distance, my tmax);
		my startSelection = my endSelection = position;
		my v_updateText ();
		FunctionEditor_redraw (me);
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveStartOfSelectionBy (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Move start of selection by", nullptr)
		REAL (distance, Melder_cat (U"Distance (", my v_format_units_short(), U")"), U"0.05")
	EDITOR_OK
	EDITOR_DO
		my startSelection = Melder_clipped (my tmin, my startSelection + distance, my tmax);
		Melder_sort (& my startSelection, & my endSelection);
		my v_updateText ();
		FunctionEditor_redraw (me);
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveEndOfSelectionBy (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Move end of selection by", nullptr)
		REAL (distance, Melder_cat (U"Distance (", my v_format_units_short(), U")"), U"0.05")
	EDITOR_OK
	EDITOR_DO
		my endSelection = Melder_clipped (my tmin, my endSelection + distance, my tmax);
		Melder_sort (& my startSelection, & my endSelection);
		my v_updateText ();
		FunctionEditor_redraw (me);
		updateGroup (me);
	EDITOR_END
}

void FunctionEditor_shift (FunctionEditor me, double shift, bool needsUpdateGroup) {
	const double windowLength = my endWindow - my startWindow;
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   // quickly, before window changes
	trace (U"shifting by ", shift);
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
	FunctionEditor_marksChanged (me, needsUpdateGroup);
}

static void menu_cb_pageUp (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	FunctionEditor_shift (me, -RELATIVE_PAGE_INCREMENT * (my endWindow - my startWindow), true);
}

static void menu_cb_pageDown (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	FunctionEditor_shift (me, +RELATIVE_PAGE_INCREMENT * (my endWindow - my startWindow), true);
}

static void scrollToView (FunctionEditor me, double t) {
	if (t <= my startWindow) {
		FunctionEditor_shift (me, t - my startWindow - 0.618 * (my endWindow - my startWindow), true);
	} else if (t >= my endWindow) {
		FunctionEditor_shift (me, t - my endWindow + 0.618 * (my endWindow - my startWindow), true);
	} else {
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_selectEarlier (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	my startSelection -= my p_arrowScrollStep;
	if (my startSelection < my tmin + 1e-12)
		my startSelection = my tmin;
	my endSelection -= my p_arrowScrollStep;
	if (my endSelection < my tmin + 1e-12)
		my endSelection = my tmin;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

static void menu_cb_selectLater (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	my startSelection += my p_arrowScrollStep;
	if (my startSelection > my tmax - 1e-12)
		my startSelection = my tmax;
	my endSelection += my p_arrowScrollStep;
	if (my endSelection > my tmax - 1e-12)
		my endSelection = my tmax;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

static void menu_cb_moveStartOfSelectionLeft (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	my startSelection -= my p_arrowScrollStep;
	if (my startSelection < my tmin + 1e-12)
		my startSelection = my tmin;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

static void menu_cb_moveStartOfSelectionRight (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	my startSelection += my p_arrowScrollStep;
	if (my startSelection > my tmax - 1e-12)
		my startSelection = my tmax;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

static void menu_cb_moveEndOfSelectionLeft (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	my endSelection -= my p_arrowScrollStep;
	if (my endSelection < my tmin + 1e-12)
		my endSelection = my tmin;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

static void menu_cb_moveEndOfSelectionRight (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	my endSelection += my p_arrowScrollStep;
	if (my endSelection > my tmax - 1e-12)
		my endSelection = my tmax;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
}

/********** GUI CALLBACKS **********/

static void gui_cb_scroll (FunctionEditor me, GuiScrollBarEvent event) {
	if (! my graphics)
		return;   // ignore events during creation
	const double value = GuiScrollBar_getValue (event -> scrollBar);
	const double shift = my tmin + (value - 1) * (my tmax - my tmin) / maximumScrollBarValue - my startWindow;
	const bool shifted = ( shift != 0.0 );
	const double oldSliderSize = (my endWindow - my startWindow) / (my tmax - my tmin) * maximumScrollBarValue - 1.0;
	const double newSliderSize = GuiScrollBar_getSliderSize (event -> scrollBar);
	bool zoomed = ( newSliderSize != oldSliderSize );
	#if ! cocoa
		zoomed = false;
	#endif
	if (shifted) {
		my startWindow += shift;
		if (my startWindow < my tmin + 1e-12)
			my startWindow = my tmin;
		my endWindow += shift;
		if (my endWindow > my tmax - 1e-12)
			my endWindow = my tmax;
	}
	if (zoomed) {
		const double zoom = (newSliderSize + 1) * (my tmax - my tmin) / maximumScrollBarValue;
		my endWindow = my startWindow + zoom;
		if (my endWindow > my tmax - 1e-12)
			my endWindow = my tmax;
	}
	if (shifted || zoomed) {
		my v_updateText ();
		//updateScrollBar (me);
		FunctionEditor_redraw (me);
		if (! my group || ! my pref_synchronizedZoomAndScroll())
			return;
		for (integer i = 1; i <= THE_MAXIMUM_GROUP_SIZE; i ++) {
			if (theGroupMembers [i] && theGroupMembers [i] != me) {
				theGroupMembers [i] -> startWindow = my startWindow;
				theGroupMembers [i] -> endWindow = my endWindow;
				FunctionEditor_updateText (theGroupMembers [i]);
				updateScrollBar (theGroupMembers [i]);
				FunctionEditor_redraw (theGroupMembers [i]);
			}
		}
	}
}

static integer findEmptySpotInGroup () {
	integer emptySpot = 1;
	while (theGroupMembers [emptySpot])
		emptySpot ++;
	return emptySpot;
}

static integer findMeInGroup (FunctionEditor me) {
	integer positionInGroup = 1;
	while (theGroupMembers [positionInGroup] != me)
		positionInGroup ++;
	return positionInGroup;
}

static integer findOtherGroupMember (FunctionEditor me) {
	integer otherGroupMember = 1;
	while (! theGroupMembers [otherGroupMember] || theGroupMembers [otherGroupMember] == me)
		otherGroupMember ++;
	return otherGroupMember;
}

static void gui_checkbutton_cb_group (FunctionEditor me, GuiCheckButtonEvent /* event */) {
	my group = ! my group;   // toggle
	if (my group) {
		const integer emptySpot = findEmptySpotInGroup ();
		theGroupMembers [emptySpot] = me;
		if (++ theGroupSize == 1) {
			FunctionEditor_redraw (me);
			return;
		}
		const integer otherGroupMember = findOtherGroupMember (me);
		const FunctionEditor thee = theGroupMembers [otherGroupMember];
		if (my pref_synchronizedZoomAndScroll()) {
			my startWindow = thy startWindow;
			my endWindow = thy endWindow;
		}
		my startSelection = thy startSelection;
		my endSelection = thy endSelection;
		if (my tmin > thy tmin || my tmax < thy tmax) {
			Melder_clipRight (& my tmin, thy tmin);
			Melder_clipLeft (thy tmax, & my tmax);
			my v_updateText ();
			updateScrollBar (me);
			FunctionEditor_redraw (me);
		} else {
			my v_updateText ();
			updateScrollBar (me);
			FunctionEditor_redraw (me);
			if (my tmin < thy tmin || my tmax > thy tmax) {
				for (integer imember = 1; imember <= THE_MAXIMUM_GROUP_SIZE; imember ++) {
					if (theGroupMembers [imember] && theGroupMembers [imember] != me) {
						if (my tmin < thy tmin)
							theGroupMembers [imember] -> tmin = my tmin;
						if (my tmax > thy tmax)
							theGroupMembers [imember] -> tmax = my tmax;
						FunctionEditor_updateText (theGroupMembers [imember]);
						updateScrollBar (theGroupMembers [imember]);
						FunctionEditor_redraw (theGroupMembers [imember]);
					}
				}
			}
		}
	} else {
		const integer myLocationInGroup = findMeInGroup (me);
		theGroupMembers [myLocationInGroup] = nullptr;
		theGroupSize --;
		my v_updateText ();
		FunctionEditor_redraw (me);   // for setting buttons in draw method
	}
	if (my group)
		updateGroup (me);
}

static void menu_cb_intro (FunctionEditor /* me */, EDITOR_ARGS_DIRECT) {
	Melder_help (U"Intro");
}

void structFunctionEditor :: v_createMenuItems_file (EditorMenu menu) {
	FunctionEditor_Parent :: v_createMenuItems_file (menu);
	EditorMenu_addCommand (menu, U"Preferences...", 0, menu_cb_preferences);
	EditorMenu_addCommand (menu, U"-- after preferences --", 0, nullptr);
}

void structFunctionEditor :: v_createMenuItems_view_timeDomain (EditorMenu menu) {
	EditorMenu_addCommand (menu, v_format_domain (), GuiMenu_INSENSITIVE, menu_cb_zoom /* dummy */);
	EditorMenu_addCommand (menu, U"Zoom...", 0, menu_cb_zoom);
	EditorMenu_addCommand (menu, U"Show all", 'A', menu_cb_showAll);
	EditorMenu_addCommand (menu, U"Zoom in", 'I', menu_cb_zoomIn);
	EditorMenu_addCommand (menu, U"Zoom out", 'O', menu_cb_zoomOut);
	EditorMenu_addCommand (menu, U"Zoom to selection", 'N', menu_cb_zoomToSelection);
	EditorMenu_addCommand (menu, U"Zoom back", 'B', menu_cb_zoomBack);
	EditorMenu_addCommand (menu, U"Scroll page back", GuiMenu_PAGE_UP, menu_cb_pageUp);
	EditorMenu_addCommand (menu, U"Scroll page forward", GuiMenu_PAGE_DOWN, menu_cb_pageDown);
}

void structFunctionEditor :: v_createMenuItems_view_audio (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"-- play --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Audio:", GuiMenu_INSENSITIVE, menu_cb_play /* dummy */);
	EditorMenu_addCommand (menu, U"Play...", 0, menu_cb_play);
	EditorMenu_addCommand (menu, U"Play or stop", GuiMenu_TAB, menu_cb_playOrStop);
	EditorMenu_addCommand (menu, U"Play window", GuiMenu_SHIFT | GuiMenu_TAB, menu_cb_playWindow);
	EditorMenu_addCommand (menu, U"Interrupt playing", GuiMenu_ESCAPE, menu_cb_interruptPlaying);
}

void structFunctionEditor :: v_createMenuItems_view (EditorMenu menu) {
	v_createMenuItems_view_timeDomain (menu);
	v_createMenuItems_view_audio (menu);
}

void structFunctionEditor :: v_createMenuItems_query (EditorMenu menu) {
	FunctionEditor_Parent :: v_createMenuItems_query (menu);
	EditorMenu_addCommand (menu, U"-- query selection --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Get start of selection", 0, menu_cb_getB);
	EditorMenu_addCommand (menu, U"Get begin of selection", Editor_HIDDEN, menu_cb_getB);
	EditorMenu_addCommand (menu, U"Get cursor", GuiMenu_F6, menu_cb_getCursor);
	EditorMenu_addCommand (menu, U"Get end of selection", 0, menu_cb_getE);
	EditorMenu_addCommand (menu, U"Get selection length", 0, menu_cb_getSelectionDuration);
}

void structFunctionEditor :: v_createMenus () {
	FunctionEditor_Parent :: v_createMenus ();
	EditorMenu menu;

	menu = Editor_addMenu (this, U"View", 0);
	v_createMenuItems_view (menu);

	Editor_addMenu (this, U"Select", 0);
	Editor_addCommand (this, U"Select", U"Select...", 0, menu_cb_select);
	Editor_addCommand (this, U"Select", U"Widen or shrink selection...", 0, menu_cb_widenOrShrinkSelection);
	Editor_addCommand (this, U"Select", U"Move cursor to start of selection", 0, menu_cb_moveCursorToStartOfSelection);
	Editor_addCommand (this, U"Select", U"Move cursor to begin of selection", Editor_HIDDEN, menu_cb_moveCursorToStartOfSelection);
	Editor_addCommand (this, U"Select", U"Move cursor to end of selection", 0, menu_cb_moveCursorToEndOfSelection);
	Editor_addCommand (this, U"Select", U"Move cursor to...", 0, menu_cb_moveCursorTo);
	Editor_addCommand (this, U"Select", U"Move cursor by...", 0, menu_cb_moveCursorBy);
	Editor_addCommand (this, U"Select", U"Move start of selection by...", 0, menu_cb_moveStartOfSelectionBy);
	Editor_addCommand (this, U"Select", U"Move begin of selection by...", Editor_HIDDEN, menu_cb_moveStartOfSelectionBy);
	Editor_addCommand (this, U"Select", U"Move end of selection by...", 0, menu_cb_moveEndOfSelectionBy);
	/*Editor_addCommand (this, U"Select", U"Move cursor back by half a second", motif_, menu_cb_moveCursorBy);*/
	Editor_addCommand (this, U"Select", U"Select earlier", GuiMenu_UP_ARROW, menu_cb_selectEarlier);
	Editor_addCommand (this, U"Select", U"Select later", GuiMenu_DOWN_ARROW, menu_cb_selectLater);
	Editor_addCommand (this, U"Select", U"Move start of selection left", GuiMenu_SHIFT | GuiMenu_UP_ARROW, menu_cb_moveStartOfSelectionLeft);
	Editor_addCommand (this, U"Select", U"Move begin of selection left", Editor_HIDDEN, menu_cb_moveStartOfSelectionLeft);
	Editor_addCommand (this, U"Select", U"Move start of selection right", GuiMenu_SHIFT | GuiMenu_DOWN_ARROW, menu_cb_moveStartOfSelectionRight);
	Editor_addCommand (this, U"Select", U"Move begin of selection right", Editor_HIDDEN, menu_cb_moveStartOfSelectionRight);
	Editor_addCommand (this, U"Select", U"Move end of selection left", GuiMenu_COMMAND | GuiMenu_UP_ARROW, menu_cb_moveEndOfSelectionLeft);
	Editor_addCommand (this, U"Select", U"Move end of selection right", GuiMenu_COMMAND | GuiMenu_DOWN_ARROW, menu_cb_moveEndOfSelectionRight);
}

void structFunctionEditor :: v_createHelpMenuItems (EditorMenu menu) {
	FunctionEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"Intro", 0, menu_cb_intro);
}

static void gui_drawingarea_cb_expose (FunctionEditor me, GuiDrawingArea_ExposeEvent /* event */) {
	if (! my graphics)
		return;   // could be the case in the very beginning
	if (! my enableUpdates)
		return;
	#ifdef WIN32
		if (my duringPlay) {
			GraphicsScreen graphics = (GraphicsScreen) my graphics.get();
			HDC foregroundContext = graphics -> d_gdiGraphicsContext;
			static HDC backgroundContext;
			if (! my backgroundIsUpToDate) {
				/*
					Draw into background.
				*/
				static HBITMAP backgroundBitmap;
				if (backgroundBitmap)
					DeleteObject (backgroundBitmap);
				if (backgroundContext)
					DeleteDC (backgroundContext);
				backgroundContext = CreateCompatibleDC (foregroundContext);
				backgroundBitmap = CreateCompatibleBitmap (foregroundContext, my drawingArea -> d_widget -> width, my drawingArea -> d_widget -> height);
				SelectObject (backgroundContext, backgroundBitmap);
				SetBkMode (backgroundContext, TRANSPARENT);   // not the default!
				SelectPen (backgroundContext, GetStockPen (BLACK_PEN));
				SelectBrush (backgroundContext, GetStockBrush (BLACK_BRUSH));
				SetTextAlign (backgroundContext, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);   // baseline is not the default!
				graphics -> d_gdiGraphicsContext = backgroundContext;
				drawBackgroundAndData (me);
				graphics -> d_gdiGraphicsContext = foregroundContext;
				my backgroundIsUpToDate = true;
			}
			/*
				Copy to foreground.
			*/
			BitBlt (foregroundContext, 0, 0, my drawingArea -> d_widget -> width, my drawingArea -> d_widget -> height, backgroundContext, 0, 0, SRCCOPY);
		} else {
			my backgroundIsUpToDate = false;
			drawBackgroundAndData (me);
		}
	#else
		drawBackgroundAndData (me);
	#endif

	/*
		Draw the running cursor.
	*/
	if (my duringPlay) {
		if (Melder_debug == 53) {
			static integer numberOfRunningCursorsDrawn = 0;
			numberOfRunningCursorsDrawn += 1;
			Melder_casual (U"playing cursor ", numberOfRunningCursorsDrawn);
		}
		my viewDataAsWorldByFraction ();
		Graphics_setColour (my graphics.get(), Melder_BLACK);
		Graphics_setLineWidth (my graphics.get(), 3.0);
		Graphics_xorOn (my graphics.get(), Melder_BLACK);
		Graphics_line (my graphics.get(), my playCursor, 0.0, my playCursor, 1.0);
		Graphics_xorOff (my graphics.get());
		Graphics_setLineWidth (my graphics.get(), 1.0);
	}

	/*
		Draw the selection part.
	*/
	if (my p_showSelectionViewer) {
		my viewInnerSelectionViewerAsFractionByFraction ();
		if (my duringPlay)
			my v_drawRealTimeSelectionViewer (my playCursor);
		else
			my v_drawSelectionViewer ();
	}
}

bool structFunctionEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double mouseTime, double /* mouseY_fraction */) {
	Melder_assert (our startSelection <= our endSelection);
	Melder_clip (our startWindow, & mouseTime, our endWindow);   // WYSIWYG
	static double anchorTime = undefined;
	if (event -> isClick()) {
		/*
			Ignore any click that occurs during a drag,
			such as might occur when the user has both a mouse and a trackpad.
		*/
		if (isdefined (anchorTime))
			return false;
		const double selectedMiddleTime = 0.5 * (our startSelection + our endSelection);
		const bool theyWantToExtendTheCurrentSelectionAtTheLeft =
				(event -> shiftKeyPressed && mouseTime < selectedMiddleTime) || event -> isLeftBottomFunctionKeyPressed();
		const bool theyWantToExtendTheCurrentSelectionAtTheRight =
				(event -> shiftKeyPressed && mouseTime >= selectedMiddleTime) || event -> isRightBottomFunctionKeyPressed();
		if (theyWantToExtendTheCurrentSelectionAtTheLeft) {
			our startSelection = mouseTime;
			anchorTime = our endSelection;
		} else if (theyWantToExtendTheCurrentSelectionAtTheRight) {
			our endSelection = mouseTime;
			anchorTime = our startSelection;
		} else {
			our startSelection = mouseTime;
			our endSelection = mouseTime;
			anchorTime = mouseTime;
		}
		Melder_sort (& our startSelection, & our endSelection);
		Melder_assert (isdefined (anchorTime));
	} else if (event -> isDrag() || event -> isDrop()) {
		/*
			Ignore any drag or drop that happens after a descendant preempted the above click handling.
		*/
		if (isundef (anchorTime))
			return false;
		static bool hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
		if (! hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
			const double distanceToAnchor_mm = fabs (Graphics_dxWCtoMM (our graphics.get(), mouseTime - anchorTime));
			constexpr double vicinityRadius_mm = 1.0;
			if (distanceToAnchor_mm > vicinityRadius_mm)
				hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = true;
		}
		if (hasBeenDraggedBeyondVicinityRadiusAtLeastOnce) {
			our startSelection = std::min (anchorTime, mouseTime);
			our endSelection = std::max (anchorTime, mouseTime);
		}
		if (event -> isDrop()) {
			anchorTime = undefined;
			hasBeenDraggedBeyondVicinityRadiusAtLeastOnce = false;
		}
	}
	return true;
}

void structFunctionEditor :: v_clickSelectionViewer (double /* x_fraction */, double /* y_fraction */) {
}

static void gui_drawingarea_cb_mouse (FunctionEditor me, GuiDrawingArea_MouseEvent event) {
	if (! my graphics)
		return;   // could be the case in the very beginning
	my viewAllAsPixelettes ();
	double x_pxlt, y_pxlt;
	Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & x_pxlt, & y_pxlt);
	static bool anchorIsInSelectionViewer = false;
	static bool anchorIsInWideDataView = false;
	if (event -> isClick()) {
		my clickWasModifiedByShiftKey = event -> shiftKeyPressed;
		anchorIsInSelectionViewer = my isInSelectionViewer (x_pxlt);
		anchorIsInWideDataView = ( y_pxlt > my dataBottom_pxlt() && y_pxlt < my dataTop_pxlt() );
	}
	if (anchorIsInSelectionViewer) {
		my viewInnerSelectionViewerAsFractionByFraction ();
		double x_fraction, y_fraction;
		Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & x_fraction, & y_fraction);
		if (event -> isClick()) {
			my v_clickSelectionViewer (x_fraction, y_fraction);
			//my v_updateText ();
			FunctionEditor_redraw (me);
			updateGroup (me);
		} else;   // no dragging (yet?) in any selection viewer
	} else if (anchorIsInWideDataView) {
		my viewDataAsWorldByFraction ();
		double x_world, y_fraction;
		Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & x_world, & y_fraction);
		my v_mouseInWideDataView (event, x_world, y_fraction);
		my v_updateText ();
		FunctionEditor_redraw (me);
		updateGroup (me);
	} else {   // clicked outside signal region? Let us hear it
		try {
			if (event -> isClick()) {
				for (integer i = 0; i < 8; i ++) {
					if (x_pxlt > my rect [i]. left && x_pxlt < my rect [i]. right && y_pxlt > my rect [i]. bottom && y_pxlt < my rect [i]. top) {
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
			} else;   // no dragging in the play rectangles
		} catch (MelderError) {
			Melder_flushError ();
		}
	}
}

void structFunctionEditor :: v_createChildren () {
	int x = BUTTON_X;

	/***** Create zoom buttons. *****/

	GuiButton_createShown (our windowForm, x, x + BUTTON_WIDTH, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"all", gui_button_cb_showAll, this, 0);
	x += BUTTON_WIDTH + BUTTON_SPACING;
	GuiButton_createShown (our windowForm, x, x + BUTTON_WIDTH, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"in", gui_button_cb_zoomIn, this, 0);
	x += BUTTON_WIDTH + BUTTON_SPACING;
	GuiButton_createShown (our windowForm, x, x + BUTTON_WIDTH, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"out", gui_button_cb_zoomOut, this, 0);
	x += BUTTON_WIDTH + BUTTON_SPACING;
	GuiButton_createShown (our windowForm, x, x + BUTTON_WIDTH, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"sel", gui_button_cb_zoomToSelection, this, 0);
	x += BUTTON_WIDTH + BUTTON_SPACING;
	GuiButton_createShown (our windowForm, x, x + BUTTON_WIDTH, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"bak", gui_button_cb_zoomBack, this, 0);

	/***** Create scroll bar. *****/

	our scrollBar = GuiScrollBar_createShown (our windowForm,
		x += BUTTON_WIDTH + BUTTON_SPACING, -80 - BUTTON_SPACING, -4 - Gui_PUSHBUTTON_HEIGHT, 0,
		1, maximumScrollBarValue, 1, maximumScrollBarValue - 1, 1, 1,
		gui_cb_scroll, this, GuiScrollBar_HORIZONTAL);

	/***** Create Group button. *****/

	our groupButton = GuiCheckButton_createShown (our windowForm, -80, 0, -4 - Gui_PUSHBUTTON_HEIGHT, -4,
		U"Group", gui_checkbutton_cb_group, this, group_equalDomain (our tmin, our tmax) ? GuiCheckButton_SET : 0);

	/***** Create optional text field. *****/

	if (our v_hasText ()) {
		our textArea = GuiText_createShown (our windowForm, 0, 0,
			Machine_getMenuBarHeight (),
			Machine_getMenuBarHeight () + TEXT_HEIGHT, GuiText_WORDWRAP | GuiText_MULTILINE);
		#if gtk
			Melder_assert (our textArea -> d_widget);
			gtk_widget_grab_focus (GTK_WIDGET (our textArea -> d_widget));   // BUG: can hardly be correct (the text should grab the focus of the window, not the global focus)
		#elif cocoa
			Melder_assert ([(NSView *) our textArea -> d_widget window]);
			//[[(NSView *) our textArea -> d_widget window] setInitialFirstResponder: (NSView *) our textArea -> d_widget];
			[[(NSView *) our textArea -> d_widget window] makeFirstResponder: (NSView *) our textArea -> d_widget];
		#endif
	}

	/***** Create drawing area. *****/

	#if cocoa
		int marginBetweenTextAndDrawingAreaToEnsureCorrectUnhighlighting = 3;
	#else
		int marginBetweenTextAndDrawingAreaToEnsureCorrectUnhighlighting = 0;
	#endif
	our drawingArea = GuiDrawingArea_createShown (our windowForm,
		0, 0,
		Machine_getMenuBarHeight () + ( our v_hasText () ? TEXT_HEIGHT + marginBetweenTextAndDrawingAreaToEnsureCorrectUnhighlighting : 0), -8 - Gui_PUSHBUTTON_HEIGHT,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_mouse,
		nullptr, gui_drawingarea_cb_resize, this, 0
	);
	GuiDrawingArea_setSwipable (our drawingArea, our scrollBar, nullptr);
}

void structFunctionEditor :: v_dataChanged () {
	Melder_assert (Thing_isa (our function(), classFunction));
	our tmin = our function() -> xmin;
 	our tmax = our function() -> xmax;
 	if (our startWindow < our tmin || our startWindow > our tmax)
 		our startWindow = our tmin;
 	if (our endWindow < our tmin || our endWindow > our tmax)
 		our endWindow = our tmax;
 	if (our startWindow >= our endWindow) {
 		our startWindow = our tmin;
 		our endWindow = our tmax;
	}
	Melder_clip (our tmin, & our startSelection, our tmax);
	Melder_clip (our tmin, & our endSelection, our tmax);
	FunctionEditor_marksChanged (this, false);
}

int structFunctionEditor :: v_playCallback (int phase, double /* startTime */, double endTime, double currentTime) {
	our playCursor = currentTime;
	if (phase == 1) {
		our duringPlay = true;
		return 1;
	}
	if (phase == 3) {
		our duringPlay = false;
		if (currentTime < endTime && MelderAudio_stopWasExplicit ()) {
			if (currentTime > our startSelection && currentTime < our endSelection)
				our startSelection = currentTime;
			else
				our startSelection = our endSelection = currentTime;
			v_updateText ();
			updateGroup (this);
		}
	}
	if (Melder_debug == 53)
		Melder_casual (U"draining");
	Graphics_updateWs (our graphics.get());   // note: without setting my backgroundIsUpToDate to false
	GuiShell_drain (our windowForm);   // this may not be needed on all platforms (but on Windows it is, at least on 2020-09-21)
	return 1;
}

int theFunctionEditor_playCallback (FunctionEditor me, int phase, double startTime, double endTime, double currentTime) {
	return my v_playCallback (phase, startTime, endTime, currentTime);
}

void structFunctionEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	Graphics_highlight (our graphics.get(), left, right, bottom, top);
}

void FunctionEditor_init (FunctionEditor me, conststring32 title, Function function) {
	if (Melder_debug == 55)
		Melder_casual (Thing_messageNameAndAddress (me), U" init");
	my tmin = function -> xmin;   // set before adding children (see group button)
	my tmax = function -> xmax;
	Editor_init (me, 0, 0, my pref_shellWidth(), my pref_shellHeight(), title, function);

	my startWindow = my tmin;
	my endWindow = my tmax;
	my startSelection = my endSelection = 0.5 * (my tmin + my tmax);
	#if motif
		Melder_assert (XtWindow (my drawingArea -> d_widget));
	#endif
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setFontSize (my graphics.get(), 12);

	my updateGeometry (GuiControl_getWidth (my drawingArea), GuiControl_getHeight (my drawingArea));

	my v_updateText ();
	if (group_equalDomain (my tmin, my tmax))
		gui_checkbutton_cb_group (me, nullptr);   // BUG: nullptr
	my enableUpdates = true;
}

void FunctionEditor_marksChanged (FunctionEditor me, bool needsUpdateGroup) {
	my v_updateText ();
	updateScrollBar (me);
	FunctionEditor_redraw (me);
	if (needsUpdateGroup)
		updateGroup (me);
}

void FunctionEditor_updateText (FunctionEditor me) {
	my v_updateText ();
}

void FunctionEditor_redraw (FunctionEditor me) {
	my backgroundIsUpToDate = false;
	Graphics_updateWs (my graphics.get());
}

void FunctionEditor_enableUpdates (FunctionEditor me, bool enable) {
	my enableUpdates = enable;
}

void FunctionEditor_ungroup (FunctionEditor me) {
	if (! my group)
		return;
	my group = false;
	GuiCheckButton_setValue (my groupButton, false);
	integer i = 1;
	while (theGroupMembers [i] != me)
		i ++;
	theGroupMembers [i] = nullptr;
	theGroupSize --;
	my v_updateText ();
	FunctionEditor_redraw (me);   // for setting buttons in v_draw() method
}

void FunctionEditor_drawRangeMark (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units, int verticalAlignment) {
	static MelderString text;
	MelderString_copy (& text, yWC_string, units);
	double textWidth = Graphics_textWidth (my graphics.get(), text.string) + Graphics_dxMMtoWC (my graphics.get(), 0.5);
	Graphics_setColour (my graphics.get(), Melder_BLUE);
	Graphics_line (my graphics.get(), my endWindow, yWC, my endWindow + textWidth, yWC);
	Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, verticalAlignment);
	if (verticalAlignment == Graphics_BOTTOM)
		yWC -= Graphics_dyMMtoWC (my graphics.get(), 0.5);
	Graphics_text (my graphics.get(), my endWindow, yWC, text.string);
}

void FunctionEditor_drawCursorFunctionValue (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units) {
	Graphics_setColour (my graphics.get(), Melder_CYAN);
	Graphics_line (my graphics.get(), my startWindow, yWC, 0.99 * my startWindow + 0.01 * my endWindow, yWC);
	Graphics_fillCircle_mm (my graphics.get(), 0.5 * (my startSelection + my endSelection), yWC, 1.5);
	Graphics_setColour (my graphics.get(), Melder_BLUE);
	Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_HALF);
	Graphics_text (my graphics.get(), my startWindow, yWC,   yWC_string, units);
}

void FunctionEditor_insertCursorFunctionValue (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units, double minimum, double maximum) {
	double textX = my endWindow, textY = yWC;
	const bool tooHigh = ( Graphics_dyWCtoMM (my graphics.get(), maximum - textY) < 5.0 );
	const bool tooLow = ( Graphics_dyWCtoMM (my graphics.get(), textY - minimum) < 5.0 );
	if (yWC < minimum || yWC > maximum)
		return;
	Graphics_setColour (my graphics.get(), Melder_CYAN);
	Graphics_line (my graphics.get(), 0.99 * my endWindow + 0.01 * my startWindow, yWC, my endWindow, yWC);
	Graphics_fillCircle_mm (my graphics.get(), 0.5 * (my startSelection + my endSelection), yWC, 1.5);
	if (tooHigh) {
		if (tooLow)
			textY = 0.5 * (minimum + maximum);
		else
			textY = maximum - Graphics_dyMMtoWC (my graphics.get(), 5.0);
	} else if (tooLow) {
		textY = minimum + Graphics_dyMMtoWC (my graphics.get(), 5.0);
	}
	static MelderString text;
	MelderString_copy (& text, yWC_string, units);
	double textWidth = Graphics_textWidth (my graphics.get(), text.string);
	Graphics_fillCircle_mm (my graphics.get(), my endWindow + textWidth + Graphics_dxMMtoWC (my graphics.get(), 1.5), textY, 1.5);
	Graphics_setColour (my graphics.get(), Melder_RED);
	Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_HALF);
	Graphics_text (my graphics.get(), textX, textY, text.string);
}

void FunctionEditor_drawHorizontalHair (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units) {
	Graphics_setColour (my graphics.get(), Melder_RED);
	Graphics_line (my graphics.get(), my startWindow, yWC, my endWindow, yWC);
	Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_HALF);
	Graphics_text (my graphics.get(), my startWindow, yWC,   yWC_string, units);
}

void FunctionEditor_drawGridLine (FunctionEditor me, double yWC) {
	Graphics_setColour (my graphics.get(), Melder_CYAN);
	Graphics_setLineType (my graphics.get(), Graphics_DOTTED);
	Graphics_line (my graphics.get(), my startWindow, yWC, my endWindow, yWC);
	Graphics_setLineType (my graphics.get(), Graphics_DRAWN);
}

void FunctionEditor_garnish (FunctionEditor me) {
	if (my pref_picture_drawSelectionTimes ()) {
		if (my startSelection >= my startWindow && my startSelection <= my endWindow)
			Graphics_markTop (my pictureGraphics, my startSelection, true, true, false, nullptr);
		if (my endSelection != my startSelection && my endSelection >= my startWindow && my endSelection <= my endWindow)
			Graphics_markTop (my pictureGraphics, my endSelection, true, true, false, nullptr);
	}
	if (my pref_picture_drawSelectionHairs ()) {
		if (my startSelection >= my startWindow && my startSelection <= my endWindow)
			Graphics_markTop (my pictureGraphics, my startSelection, false, false, true, nullptr);
		if (my endSelection != my startSelection && my endSelection >= my startWindow && my endSelection <= my endWindow)
			Graphics_markTop (my pictureGraphics, my endSelection, false, false, true, nullptr);
	}
}

/* End of file FunctionEditor.cpp */
