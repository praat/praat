/* FunctionEditor.cpp
 *
 * Copyright (C) 1992-2020 Paul Boersma
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

#define maximumScrollBarValue  2000000000
#define RELATIVE_PAGE_INCREMENT  0.8
#define SCROLL_INCREMENT_FRACTION  20
#define space 30
#define MARGIN 107
#define BOTTOM_MARGIN  2
#define TOP_MARGIN  3
#define TEXT_HEIGHT  50
#define BUTTON_X  3
#define BUTTON_WIDTH  40
#define BUTTON_SPACING  8

#include "prefs_define.h"
#include "FunctionEditor_prefs.h"
#include "prefs_install.h"
#include "FunctionEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "FunctionEditor_prefs.h"

namespace {
	constexpr integer THE_MAXIMUM_GROUP_SIZE = 100;
	integer theGroupSize = 0;
	FunctionEditor theGroupMembers [1 + THE_MAXIMUM_GROUP_SIZE];
}

static void drawWhileDragging (FunctionEditor me, double x1, double x2);

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
	const double slider_size = std::max (1.0, (my endWindow - my startWindow) / (my tmax - my tmin) * maximumScrollBarValue - 1.0);
	const double value = std::min (std::max (1.0, (my startWindow - my tmin) / (my tmax - my tmin) * maximumScrollBarValue + 1.0), maximumScrollBarValue - slider_size);
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
			if (my pref_synchronizedZoomAndScroll ()) {
				thy startWindow = my startWindow;
				thy endWindow = my endWindow;
			}
			thy startSelection = my startSelection;
			thy endSelection = my endSelection;
			FunctionEditor_updateText (thee);
			updateScrollBar (thee);
			Graphics_updateWs (thy graphics.get());
		}
	}
}

static void drawNow (FunctionEditor me) {
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
	VECsort_inplace (VEC (& my marker [1], my numberOfMarkers));

	/*
		Update rectangles.
	*/

	for (integer i = 0; i < 8; i++)
		my rect [i]. left = my rect [i]. right = 0;

	/*
		0: rectangle for total.
	*/
	my rect [0]. left = my functionViewerLeft + ( leftFromWindow ? 0 : MARGIN );
	my rect [0]. right = my functionViewerRight - ( rightFromWindow ? 0 : MARGIN );
	my rect [0]. bottom = BOTTOM_MARGIN;
	my rect [0]. top = BOTTOM_MARGIN + space;

	/*
		1: rectangle for visible part.
	*/
	my rect [1]. left = my functionViewerLeft + MARGIN;
	my rect [1]. right = my functionViewerRight - MARGIN;
	my rect [1]. bottom = BOTTOM_MARGIN + space;
	my rect [1]. top = BOTTOM_MARGIN + space * ( my numberOfMarkers > 1 ? 2 : 3 );

	/*
		2: rectangle for left from visible part.
	*/
	if (leftFromWindow) {
		my rect [2]. left = my functionViewerLeft;
		my rect [2]. right = my functionViewerLeft + MARGIN;
		my rect [2]. bottom = BOTTOM_MARGIN + space;
		my rect [2]. top = BOTTOM_MARGIN + space * 2;
	}

	/*
		3: rectangle for right from visible part.
	*/
	if (rightFromWindow) {
		my rect [3]. left = my functionViewerRight - MARGIN;
		my rect [3]. right = my functionViewerRight;
		my rect [3]. bottom = BOTTOM_MARGIN + space;
		my rect [3]. top = BOTTOM_MARGIN + space * 2;
	}

	/*
		4, 5, 6: rectangles between markers visible in visible part.
	*/
	if (my numberOfMarkers > 1) {
		const double window = my endWindow - my startWindow;
		for (integer i = 1; i <= my numberOfMarkers; i ++) {
			my rect [3 + i]. left = i == 1 ? my functionViewerLeft + MARGIN : my functionViewerLeft + MARGIN + (my functionViewerRight - my functionViewerLeft - MARGIN * 2) *
				(my marker [i - 1] - my startWindow) / window;
			my rect [3 + i]. right = my functionViewerLeft + MARGIN + (my functionViewerRight - my functionViewerLeft - MARGIN * 2) *
				(my marker [i] - my startWindow) / window;
			my rect [3 + i]. bottom = BOTTOM_MARGIN + space * 2;
			my rect [3 + i]. top = BOTTOM_MARGIN + space * 3;
		}
	}
	
	if (selectionIsNonempty) {
		const double window = my endWindow - my startWindow;
		const double left =
			my startSelection == my startWindow ? my functionViewerLeft + MARGIN :
			my startSelection == my tmin ? my functionViewerLeft :
			my startSelection < my startWindow ? my functionViewerLeft + MARGIN * 0.3 :
			my startSelection < my endWindow ? my functionViewerLeft + MARGIN + (my functionViewerRight - my functionViewerLeft - MARGIN * 2) * (my startSelection - my startWindow) / window :
			my startSelection == my endWindow ? my functionViewerRight - MARGIN : my functionViewerRight - MARGIN * 0.7;
		const double right =
			my endSelection < my startWindow ? my functionViewerLeft + MARGIN * 0.7 :
			my endSelection == my startWindow ? my functionViewerLeft + MARGIN :
			my endSelection < my endWindow ? my functionViewerLeft + MARGIN + (my functionViewerRight - my functionViewerLeft - MARGIN * 2) * (my endSelection - my startWindow) / window :
			my endSelection == my endWindow ? my functionViewerRight - MARGIN :
			my endSelection < my tmax ? my functionViewerRight - MARGIN * 0.3 : my functionViewerRight;
		my rect [7]. left = left;
		my rect [7]. right = right;
		my rect [7]. bottom = my height - space - TOP_MARGIN;
		my rect [7]. top = my height - TOP_MARGIN;
	}

	/*
		Be responsive: update the markers now.
	*/
	Graphics_setViewport (my graphics.get(), my functionViewerLeft, my functionViewerRight, 0.0, my height);
	Graphics_setWindow (my graphics.get(), my functionViewerLeft, my functionViewerRight, 0.0, my height);
	Graphics_setColour (my graphics.get(), Melder_WINDOW_BACKGROUND_COLOUR);
	Graphics_fillRectangle (my graphics.get(), my functionViewerLeft + MARGIN, my selectionViewerRight - MARGIN, my height - (TOP_MARGIN + space), my height);
	Graphics_fillRectangle (my graphics.get(), my functionViewerLeft, my functionViewerLeft + MARGIN, BOTTOM_MARGIN + ( leftFromWindow ? space * 2 : 0 ), my height);
	Graphics_fillRectangle (my graphics.get(), my functionViewerRight - MARGIN, my functionViewerRight, BOTTOM_MARGIN + ( rightFromWindow ? space * 2 : 0 ), my height);
	if (my p_showSelectionViewer) {
		Graphics_setViewport (my graphics.get(), my selectionViewerLeft, my selectionViewerRight, 0.0, my height);
		Graphics_setWindow (my graphics.get(), my selectionViewerLeft, my selectionViewerRight, 0.0, my height);
		Graphics_fillRectangle (my graphics.get(), my selectionViewerLeft, my selectionViewerLeft + MARGIN, BOTTOM_MARGIN, my height);
		Graphics_fillRectangle (my graphics.get(), my selectionViewerRight - MARGIN, my selectionViewerRight, BOTTOM_MARGIN, my height);
		Graphics_fillRectangle (my graphics.get(), my selectionViewerLeft + MARGIN, my selectionViewerRight - MARGIN, 0, BOTTOM_MARGIN + space * 3);
	}
	Graphics_setGrey (my graphics.get(), 0.0);
	#if defined (macintosh)
		Graphics_line (my graphics.get(), my functionViewerLeft, 2.0, my selectionViewerRight, 2.0);
		Graphics_line (my graphics.get(), my functionViewerLeft, my height - 2.0, my selectionViewerRight, my height - 2.0);
	#endif

	Graphics_setViewport (my graphics.get(), my functionViewerLeft, my functionViewerRight, 0.0, my height);
	Graphics_setWindow (my graphics.get(), my functionViewerLeft, my functionViewerRight, 0.0, my height);
	Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
	for (integer i = 0; i < 8; i ++) {
		const double left = my rect [i]. left, right = my rect [i]. right;
		if (left < right)
			Graphics_button (my graphics.get(), left, right, my rect [i]. bottom, my rect [i]. top);
	}
	double verticalCorrection = my height / (my height - 111.0 + 11.0);
	#ifdef _WIN32
		verticalCorrection *= 1.5;
	#endif
	for (integer i = 0; i < 8; i ++) {
		const double left = my rect [i]. left, right = my rect [i]. right;
		const double bottom = my rect [i]. bottom, top = my rect [i]. top;
		if (left < right) {
			const char *format = my v_format_long ();
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
			} else if (format == my v_format_long ()) {
				snprintf (text8, 100, my v_format_short (), value);
				text = Melder_8to32 (text8);
				if (Graphics_textWidth (my graphics.get(), text.get()) < right - left)
					Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
			} else {
				snprintf (text8, 100, my v_format_long (), value);
				text = Melder_8to32 (text8);
				if (Graphics_textWidth (my graphics.get(), text.get()) < right - left) {
						Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
				} else {
					snprintf (text8, 100, my v_format_short (), my endSelection - my startSelection);
					text = Melder_8to32 (text8);
					if (Graphics_textWidth (my graphics.get(), text.get()) < right - left)
						Graphics_text (my graphics.get(), 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text.get());
				}
			}
		}
	}

	Graphics_setViewport (my graphics.get(), my functionViewerLeft + MARGIN, my functionViewerRight - MARGIN, 0.0, my height);
	Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, 0.0, my height);
	/*Graphics_setColour (my graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (my graphics.get(), my startWindow, my endWindow, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));*/
	Graphics_setColour (my graphics.get(), Melder_BLACK);
	Graphics_rectangle (my graphics.get(), my startWindow, my endWindow, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));

	/*
		Red marker text.
	*/
	Graphics_setColour (my graphics.get(), Melder_RED);
	if (cursorIsVisible) {
		Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (my graphics.get(), my startSelection, my height - (TOP_MARGIN + space) - verticalCorrection,
			Melder_fixed (my startSelection, my v_fixedPrecision_long ()));
	}
	if (startIsVisible && selectionIsNonempty) {
		Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (my graphics.get(), my startSelection, my height - (TOP_MARGIN + space/2) - verticalCorrection,
			Melder_fixed (my startSelection, my v_fixedPrecision_long ()));
	}
	if (endIsVisible && selectionIsNonempty) {
		Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (my graphics.get(), my endSelection, my height - (TOP_MARGIN + space/2) - verticalCorrection,
			Melder_fixed (my endSelection, my v_fixedPrecision_long ()));
	}
	Graphics_setColour (my graphics.get(), Melder_BLACK);

	/*
		To reduce flashing, give our descendants the opportunity to prepare their data.
	*/
	my v_prepareDraw ();

	/*
		Start of inner drawing.
	*/
	Graphics_setViewport (my graphics.get(), my functionViewerLeft + MARGIN, my functionViewerRight - MARGIN, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));

	my v_draw ();
	Graphics_setViewport (my graphics.get(), my functionViewerLeft + MARGIN, my functionViewerRight - MARGIN, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));

	/*
		Red dotted marker lines.
	*/
	Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Melder_RED);
	Graphics_setLineType (my graphics.get(), Graphics_DOTTED);
	double bottom = my v_getBottomOfSoundAndAnalysisArea ();
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
		const double left = std::max (my startSelection, my startWindow);
		const double right = std::min (my endSelection, my endWindow);
		my v_highlightSelection (left, right, 0.0, 1.0);
	}

	/*
		Draw the selection part.
	*/
	if (my p_showSelectionViewer) {
		Graphics_setViewport (my graphics.get(), my selectionViewerLeft + MARGIN, my selectionViewerRight - MARGIN, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));
		my v_drawSelectionViewer ();
	}

	/*
		End of inner drawing.
	*/
	Graphics_flushWs (my graphics.get());
	Graphics_setViewport (my graphics.get(), my functionViewerLeft, my selectionViewerRight, 0.0, my height);
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
	FunctionEditor_Parent :: v_destroy ();
}

void structFunctionEditor :: v_info () {
	FunctionEditor_Parent :: v_info ();
	MelderInfo_writeLine (U"Editor start: ", our tmin, U" ", v_format_units_long ());
	MelderInfo_writeLine (U"Editor end: ", our tmax, U" ", v_format_units_long ());
	MelderInfo_writeLine (U"Window start: ", our startWindow, U" ", v_format_units_long ());
	MelderInfo_writeLine (U"Window end: ", our endWindow, U" ", v_format_units_long ());
	MelderInfo_writeLine (U"Selection start: ", our startSelection, U" ", v_format_units_long ());
	MelderInfo_writeLine (U"Selection end: ", our endSelection, U" ", v_format_units_long ());
	MelderInfo_writeLine (U"Arrow scroll step: ", our p_arrowScrollStep, U" ", v_format_units_long ());
	MelderInfo_writeLine (U"Group: ", group ? U"yes" : U"no");
}

/********** FILE MENU **********/

static void gui_drawingarea_cb_resize (FunctionEditor me, GuiDrawingArea_ResizeEvent event) {
	if (! my graphics)
		return;   // could be the case in the very beginning
	Graphics_setWsViewport (my graphics.get(), 0, event -> width, 0, event -> height);
	int width = event -> width + 21;
	/*
		Put the function viewer at the left and the selection viewer at the right.
	*/
	my functionViewerLeft = 0;
	my functionViewerRight = ( my p_showSelectionViewer ? Melder_ifloor (width * (2.0/3.0)) : width );
	my selectionViewerLeft = my functionViewerRight;
	my selectionViewerRight = width;
	my height = event -> height + 111;
	Graphics_setWsWindow (my graphics.get(), 0.0, width, 0.0, my height);
	Graphics_setViewport (my graphics.get(), 0.0, width, 0.0, my height);
	Graphics_updateWs (my graphics.get());
	/*
		Save the current shell size as the user's preference for a new FunctionEditor.
	*/
	my pref_shellWidth  () = GuiShell_getShellWidth  (my windowForm);
	my pref_shellHeight () = GuiShell_getShellHeight (my windowForm);
}

static void menu_cb_preferences (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Preferences", nullptr)
		BOOLEAN (synchronizeZoomAndScroll, U"Synchronize zoom and scroll", my default_synchronizedZoomAndScroll ())
		BOOLEAN (showSelectionViewer, Melder_cat (U"Show ", my v_selectionViewerName ()), my default_showSelectionViewer ())
		POSITIVE (arrowScrollStep, Melder_cat (U"Arrow scroll step (", my v_format_units_short (), U")"), my default_arrowScrollStep ())
		my v_prefs_addFields (cmd);
	EDITOR_OK
		SET_BOOLEAN (synchronizeZoomAndScroll, my pref_synchronizedZoomAndScroll ())
		SET_BOOLEAN (showSelectionViewer, my pref_showSelectionViewer ())
		SET_REAL (arrowScrollStep, my p_arrowScrollStep)
		my v_prefs_setValues (cmd);
	EDITOR_DO
		bool oldSynchronizedZoomAndScroll = my pref_synchronizedZoomAndScroll ();
		bool oldShowSelectionViewer = my p_showSelectionViewer;
		my pref_synchronizedZoomAndScroll () = synchronizeZoomAndScroll;
		my pref_showSelectionViewer () = my p_showSelectionViewer = showSelectionViewer;
		my pref_arrowScrollStep () = my p_arrowScrollStep = arrowScrollStep;
		if (my p_showSelectionViewer != oldShowSelectionViewer) {
			struct structGuiDrawingArea_ResizeEvent event { my drawingArea, 0, 0 };
			event. width  = GuiControl_getWidth  (my drawingArea);
			event. height = GuiControl_getHeight (my drawingArea);
			gui_drawingarea_cb_resize (me, & event);
		}
		if (! oldSynchronizedZoomAndScroll && my pref_synchronizedZoomAndScroll ())
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
	SET_BOOLEAN (v_form_pictureSelection_drawSelectionTimes, my pref_picture_drawSelectionTimes ())
	SET_BOOLEAN (v_form_pictureSelection_drawSelectionHairs, my pref_picture_drawSelectionHairs ())
}
void structFunctionEditor :: v_do_pictureSelection (EditorCommand cmd) {
	FunctionEditor me = (FunctionEditor) cmd -> d_editor;
	my pref_picture_drawSelectionTimes () = v_form_pictureSelection_drawSelectionTimes;
	my pref_picture_drawSelectionHairs () = v_form_pictureSelection_drawSelectionHairs;
}

/********** QUERY MENU **********/

static void menu_cb_getB (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	Melder_informationReal (my startSelection, my v_format_units_long ());
}
static void menu_cb_getCursor (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	Melder_informationReal (0.5 * (my startSelection + my endSelection), my v_format_units_long ());
}
static void menu_cb_getE (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	Melder_informationReal (my endSelection, my v_format_units_long ());
}
static void menu_cb_getSelectionDuration (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	Melder_informationReal (my endSelection - my startSelection, my v_format_units_long ());
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
		#if SUPPORT_DIRECT_DRAWING
			drawNow (me);
		#else
			Graphics_updateWs (my graphics.get());
		#endif
		updateGroup (me);
	EDITOR_END
}

static void do_showAll (FunctionEditor me) {
	my startWindow = my tmin;
	my endWindow = my tmax;
	my v_updateText ();
	updateScrollBar (me);
	#if SUPPORT_DIRECT_DRAWING
		drawNow (me);
	#else
		Graphics_updateWs (my graphics.get());
	#endif
	if (my pref_synchronizedZoomAndScroll ())
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
	#if SUPPORT_DIRECT_DRAWING
		drawNow (me);
	#else
		Graphics_updateWs (my graphics.get());
	#endif
	if (my pref_synchronizedZoomAndScroll ())
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
	#if SUPPORT_DIRECT_DRAWING
		drawNow (me);
	#else
		Graphics_updateWs (my graphics.get());
	#endif
	if (my pref_synchronizedZoomAndScroll ())
		updateGroup (me);
}

static void gui_button_cb_zoomOut (FunctionEditor me, GuiButtonEvent /*event*/) {
	do_zoomOut (me);
}

static void do_zoomToSelection (FunctionEditor me) {
	if (my endSelection > my startSelection) {
		my startZoomHistory = my startWindow;   // remember for Zoom Back
		my endZoomHistory = my endWindow;   // remember for Zoom Back
		trace (U"Zooming in to ", my startSelection, U" ~ ", my endSelection, U" seconds.");
		my startWindow = my startSelection;
		my endWindow = my endSelection;
		trace (U"Zoomed in to ", my startWindow, U" ~ ", my endWindow, U" seconds (1).");
		my v_updateText ();
		trace (U"Zoomed in to ", my startWindow, U" ~ ", my endWindow, U" seconds (2).");
		updateScrollBar (me);
		trace (U"Zoomed in to ", my startWindow, U" ~ ", my endWindow, U" seconds (3).");
		#if SUPPORT_DIRECT_DRAWING
			drawNow (me);
		#else
			Graphics_updateWs (my graphics.get());
		#endif
		if (my pref_synchronizedZoomAndScroll ())
			updateGroup (me);
		trace (U"Zoomed in to ", my startWindow, U" ~ ", my endWindow, U" seconds (4).");
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
		#if SUPPORT_DIRECT_DRAWING
			drawNow (me);
		#else
			Graphics_updateWs (my graphics.get());
		#endif
		if (my pref_synchronizedZoomAndScroll ())
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

static void menu_cb_playOrStop (FunctionEditor me, EDITOR_ARGS_FORM) {
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

static void menu_cb_playWindow (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	my playingCursor = true;
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
		#if SUPPORT_DIRECT_DRAWING
			drawNow (me);
		#else
			Graphics_updateWs (my graphics.get());
		#endif
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
		#if SUPPORT_DIRECT_DRAWING
			drawNow (me);
		#else
			Graphics_updateWs (my graphics.get());
		#endif
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveCursorToStartOfSelection (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	my endSelection = my startSelection;
	my v_updateText ();
	#if SUPPORT_DIRECT_DRAWING
		drawNow (me);
	#else
		Graphics_updateWs (my graphics.get());
	#endif
	updateGroup (me);
}

static void menu_cb_moveCursorToEndOfSelection (FunctionEditor me, EDITOR_ARGS_DIRECT) {
	my startSelection = my endSelection;
	my v_updateText ();
	#if SUPPORT_DIRECT_DRAWING
		drawNow (me);
	#else
		Graphics_updateWs (my graphics.get());
	#endif
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
		#if SUPPORT_DIRECT_DRAWING
			drawNow (me);
		#else
			Graphics_updateWs (my graphics.get());
		#endif
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveCursorBy (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Move cursor by", nullptr)
		REAL (distance, Melder_cat (U"Distance (", my v_format_units_short(), U")"), U"0.05")
	EDITOR_OK
	EDITOR_DO
		double position = 0.5 * (my startSelection + my endSelection) + distance;
		if (position < my tmin)
			position = my tmin;
		if (position > my tmax)
			position = my tmax;
		my startSelection = my endSelection = position;
		my v_updateText ();
		#if SUPPORT_DIRECT_DRAWING
			drawNow (me);
		#else
			Graphics_updateWs (my graphics.get());
		#endif
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveStartOfSelectionBy (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Move start of selection by", nullptr)
		REAL (distance, Melder_cat (U"Distance (", my v_format_units_short(), U")"), U"0.05")
	EDITOR_OK
	EDITOR_DO
		double position = my startSelection + distance;
		if (position < my tmin)
			position = my tmin;
		if (position > my tmax)
			position = my tmax;
		my startSelection = position;
		if (my startSelection > my endSelection)
			std::swap (my startSelection, my endSelection);
		my v_updateText ();
		#if SUPPORT_DIRECT_DRAWING
			drawNow (me);
		#else
			Graphics_updateWs (my graphics.get());
		#endif
		updateGroup (me);
	EDITOR_END
}

static void menu_cb_moveEndOfSelectionBy (FunctionEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Move end of selection by", nullptr)
		REAL (distance, Melder_cat (U"Distance (", my v_format_units_short(), U")"), U"0.05")
	EDITOR_OK
	EDITOR_DO
		double position = my endSelection + distance;
		if (position < my tmin)
			position = my tmin;
		if (position > my tmax)
			position = my tmax;
		my endSelection = position;
		if (my startSelection > my endSelection)
			std::swap (my startSelection, my endSelection);
		my v_updateText ();
		#if SUPPORT_DIRECT_DRAWING
			drawNow (me);
		#else
			Graphics_updateWs (my graphics.get());
		#endif
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
		updateScrollBar (me);
		#if cocoa
			Graphics_updateWs (my graphics.get());
		#else
			/*Graphics_clearWs (my graphics.get());*/
			drawNow (me);   // do not wait for expose event
		#endif
		if (! my group || ! my pref_synchronizedZoomAndScroll ())
			return;
		for (integer i = 1; i <= THE_MAXIMUM_GROUP_SIZE; i ++) {
			if (theGroupMembers [i] && theGroupMembers [i] != me) {
				theGroupMembers [i] -> startWindow = my startWindow;
				theGroupMembers [i] -> endWindow = my endWindow;
				FunctionEditor_updateText (theGroupMembers [i]);
				updateScrollBar (theGroupMembers [i]);
				#if cocoa
					Graphics_updateWs (theGroupMembers [i] -> graphics.get());
				#else
					Graphics_clearWs (theGroupMembers [i] -> graphics.get());
					drawNow (theGroupMembers [i]);
				#endif
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
			Graphics_updateWs (my graphics.get());
			return;
		}
		const integer otherGroupMember = findOtherGroupMember (me);
		const FunctionEditor thee = theGroupMembers [otherGroupMember];
		if (my pref_synchronizedZoomAndScroll ()) {
			my startWindow = thy startWindow;
			my endWindow = thy endWindow;
		}
		my startSelection = thy startSelection;
		my endSelection = thy endSelection;
		if (my tmin > thy tmin || my tmax < thy tmax) {
			if (my tmin > thy tmin)
				my tmin = thy tmin;
			if (my tmax < thy tmax)
				my tmax = thy tmax;
			my v_updateText ();
			updateScrollBar (me);
			Graphics_updateWs (my graphics.get());
		} else {
			my v_updateText ();
			updateScrollBar (me);
			Graphics_updateWs (my graphics.get());
			if (my tmin < thy tmin || my tmax > thy tmax) {
				for (integer imember = 1; imember <= THE_MAXIMUM_GROUP_SIZE; imember ++) {
					if (theGroupMembers [imember] && theGroupMembers [imember] != me) {
						if (my tmin < thy tmin)
							theGroupMembers [imember] -> tmin = my tmin;
						if (my tmax > thy tmax)
							theGroupMembers [imember] -> tmax = my tmax;
						FunctionEditor_updateText (theGroupMembers [imember]);
						updateScrollBar (theGroupMembers [imember]);
						Graphics_updateWs (theGroupMembers [imember] -> graphics.get());
					}
				}
			}
		}
	} else {
		const integer myLocationInGroup = findMeInGroup (me);
		theGroupMembers [myLocationInGroup] = nullptr;
		theGroupSize --;
		my v_updateText ();
		Graphics_updateWs (my graphics.get());   // for setting buttons in draw method
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
	if (my enableUpdates)
		drawNow (me);
}

static void gui_drawingarea_cb_click (FunctionEditor me, GuiDrawingArea_ClickEvent event) {
	if (! my graphics)
		return;   // could be the case in the very beginning
	my shiftKeyPressed = event -> shiftKeyPressed;
	Graphics_setViewport (my graphics.get(), my functionViewerLeft, my selectionViewerRight, 0.0, my height);
	Graphics_setWindow (my graphics.get(), my functionViewerLeft, my selectionViewerRight, 0.0, my height);
	double xWC, yWC;
	Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & xWC, & yWC);

	if (xWC > my selectionViewerLeft)
	{
		Graphics_setViewport (my graphics.get(), my selectionViewerLeft + MARGIN, my selectionViewerRight - MARGIN,
				BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));
		Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & xWC, & yWC);
		my v_clickSelectionViewer (xWC, yWC);
		//my v_updateText ();
		drawNow (me);
		updateGroup (me);
	}
	else if (yWC > BOTTOM_MARGIN + space * 3 && yWC < my height - (TOP_MARGIN + space)) {   // in signal region?
		bool needsUpdate;
		Graphics_setViewport (my graphics.get(), my functionViewerLeft + MARGIN, my functionViewerRight - MARGIN,
				BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));
		Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, 0.0, 1.0);
		Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & xWC, & yWC);
		if (xWC < my startWindow)
			xWC = my startWindow;
		if (xWC > my endWindow)
			xWC = my endWindow;
		if (Melder_debug == 24) {
			Melder_casual (U"FunctionEditor::gui_drawingarea_cb_click:"
				U" button ", event -> button,
				U" shift ", my shiftKeyPressed,
				U" option ", event -> optionKeyPressed,
				U" command ", event -> commandKeyPressed,
				U" control ", event -> extraControlKeyPressed);
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
		if (needsUpdate)
			my v_updateText ();
		Graphics_setViewport (my graphics.get(), my functionViewerLeft, my functionViewerRight, 0.0, my height);
		if (needsUpdate)
			drawNow (me);
		if (needsUpdate)
			updateGroup (me);
	}
	else   // clicked outside signal region? Let us hear it
	{
		try {
			for (integer i = 0; i < 8; i ++) {
				if (xWC > my rect [i]. left && xWC < my rect [i]. right &&
						yWC > my rect [i]. bottom && yWC < my rect [i]. top)
				{
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
		our text = GuiText_createShown (our windowForm, 0, 0,
			Machine_getMenuBarHeight (),
			Machine_getMenuBarHeight () + TEXT_HEIGHT, GuiText_WORDWRAP | GuiText_MULTILINE);
		#if gtk
			Melder_assert (our text -> d_widget);
			gtk_widget_grab_focus (GTK_WIDGET (our text -> d_widget));   // BUG: can hardly be correct (the text should grab the focus of the window, not the global focus)
		#elif cocoa
			Melder_assert ([(NSView *) our text -> d_widget window]);
			//[[(NSView *) our text -> d_widget window] setInitialFirstResponder: (NSView *) our text -> d_widget];
			[[(NSView *) our text -> d_widget window] makeFirstResponder: (NSView *) our text -> d_widget];
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
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, nullptr, gui_drawingarea_cb_resize, this, 0);
	GuiDrawingArea_setSwipable (our drawingArea, our scrollBar, nullptr);
}

void structFunctionEditor :: v_dataChanged () {
	const Function function = (Function) our data;
	Melder_assert (Thing_isa (function, classFunction));
	our tmin = function -> xmin;
 	our tmax = function -> xmax;
 	if (our startWindow < our tmin || our startWindow > our tmax)
 		our startWindow = our tmin;
 	if (our endWindow < our tmin || our endWindow > our tmax)
 		our endWindow = our tmax;
 	if (our startWindow >= our endWindow) {
 		our startWindow = our tmin;
 		our endWindow = our tmax;
	}
 	if (our startSelection < our tmin)
 		our startSelection = our tmin;
 	if (our startSelection > our tmax)
 		our startSelection = our tmax;
 	if (our endSelection < our tmin)
 		our endSelection = our tmin;
 	if (our endSelection > our tmax)
 		our endSelection = our tmax;
	FunctionEditor_marksChanged (this, false);
}

static void drawWhileDragging (FunctionEditor me, double x1, double x2) {
	/*
		We must draw this within the window, because the window tends to have a white background.
		We cannot draw this in the margins, because these tend to be grey, so that Graphics_xorOn does not work properly.
		We draw the text twice, because we expect that not ALL of the window is white...
	*/
	const double xleft  = std::min (x1, x2);
	const double xright = std::max (x1, x2);
	Graphics_xorOn (my graphics.get(), Melder_MAROON);
	Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_TOP);
	Graphics_text (my graphics.get(), xleft, 1.0, Melder_fixed (xleft, 6));
	Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_TOP);
	Graphics_text (my graphics.get(), xright, 1.0, Melder_fixed (xright, 6));
	Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_BOTTOM);
	Graphics_text (my graphics.get(), xleft, 0.0, Melder_fixed (xleft, 6));
	Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_BOTTOM);
	Graphics_text (my graphics.get(), xright, 0.0, Melder_fixed (xright, 6));
	Graphics_setLineType (my graphics.get(), Graphics_DOTTED);
	Graphics_line (my graphics.get(), xleft, 0.0, xleft, 1.0);
	Graphics_line (my graphics.get(), xright, 0.0, xright, 1.0);
	Graphics_setLineType (my graphics.get(), Graphics_DRAWN);
	Graphics_xorOff (my graphics.get());
}

bool structFunctionEditor :: v_click (double xbegin, double ybegin, bool a_shiftKeyPressed) {
	bool drag = false;
	double x = xbegin, y = ybegin;

	/*
		The 'anchor' is the point that will stay fixed during dragging.
		For instance, if the user clicks and drags to the right,
		the location at which she originally clicked will be the anchor,
		even if she later chooses to drag the mouse to the left of it.
		Another example: if she shift-clicks near E, B will become (and stay) the anchor.
	*/

	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);

	double anchorForDragging = xbegin;   // the default (for if the shift key isn't pressed)
	if (a_shiftKeyPressed) {
		/*
			Extend the selection.
			We should always end up with a real selection (B < E),
			even if we start with the reversed temporal order (E < B).
		*/
		const bool reversed = ( our startSelection > our endSelection );
		const double firstMark = ( reversed ? our endSelection : our startSelection );
		const double secondMark = ( reversed ? our startSelection : our endSelection );
		/*
			Undraw the old selection.
		*/
		if (our endSelection > our startSelection) {
			/*
				Determine the visible part of the old selection.
			*/
			const double startVisible = std::max (our startSelection, our startWindow);
			const double endVisible = std::min (our endSelection, our endWindow);
			/*
				Undraw the visible part of the old selection.
			*/
			if (endVisible > startVisible) {
				v_unhighlightSelection (startVisible, endVisible, 0.0, 1.0);
				//Graphics_flushWs (our graphics.get());
			}
		}
		if (xbegin >= secondMark) {
		 	/*
				She clicked right from the second mark (usually E). We move E.
			*/
			our endSelection = xbegin;
			anchorForDragging = our startSelection;
		} else if (xbegin <= firstMark) {
		 	/*
				She clicked left from the first mark (usually B). We move B.
			*/
			our startSelection = xbegin;
			anchorForDragging = our endSelection;
		} else {
			/*
				She clicked in between the two marks. We move the nearest mark.
			*/
			const double distanceOfClickToFirstMark = fabs (xbegin - firstMark);
			const double distanceOfClickToSecondMark = fabs (xbegin - secondMark);
			/*
				We make sure that the marks are in the unmarked B - E order.
			*/
			if (reversed) {
				/*
					Swap B and E.
				*/
				our startSelection = firstMark;
				our endSelection = secondMark;
			}
			/*
				Move the nearest mark.
			*/
			if (distanceOfClickToFirstMark < distanceOfClickToSecondMark) {
				our startSelection = xbegin;
				anchorForDragging = our endSelection;
			} else {
				our endSelection = xbegin;
				anchorForDragging = our startSelection;
			}
		}
		/*
			Draw the new selection.
		*/
		if (our endSelection > our startSelection) {
			/*
				Determine the visible part of the new selection.
			*/
			const double startVisible = std::max (our startSelection, our startWindow);
			const double endVisible = std::min (our endSelection, our endWindow);
			/*
				Draw the visible part of the new selection.
			*/
			if (endVisible > startVisible)
				v_highlightSelection (startVisible, endVisible, 0.0, 1.0);
		}
	}
	/*
		Find out whether this is a click or a drag.
	*/
	while (Graphics_mouseStillDown (our graphics.get())) {
		Graphics_getMouseLocation (our graphics.get(), & x, & y);
		if (x < our startWindow)
			x = our startWindow;
		if (x > our endWindow)
			x = our endWindow;
		if (fabs (Graphics_dxWCtoMM (our graphics.get(), x - xbegin)) > 1.5) {
			drag = true;
			break;
		}
	}
    
	if (drag) {
		/*
			First undraw the old selection.
		*/
		if (our endSelection > our startSelection) {
			/*
				Determine the visible part of the old selection.
			*/
			const double startVisible = std::max (our startSelection, our startWindow);
			const double endVisible = std::min (our endSelection, our endWindow);
			/*
				Undraw the visible part of the old selection.
			*/
			if (endVisible > startVisible)
				v_unhighlightSelection (startVisible, endVisible, 0.0, 1.0);
		}
		/*
			Draw the text at least once.
		*/
		/*if (x < our startWindow) x = our startWindow; else if (x > our endWindow) x = our endWindow;*/
        drawWhileDragging (this, anchorForDragging, x);
		/*
			Draw the dragged selection at least once.
		*/
		{
			double x1, x2;
			if (x > anchorForDragging) {
				x1 = anchorForDragging;
				x2 = x;
			} else {
				x1 = x;
				x2 = anchorForDragging;
			}
			v_highlightSelection (x1, x2, 0.0, 1.0);
		}
		/*
			Drag for the new selection.
		*/

		while (Graphics_mouseStillDown (our graphics.get()))
		{
			double xold = x, x1, x2;
			Graphics_getMouseLocation (our graphics.get(), & x, & y);
			/*
				Clip to the visible window. Ideally, we should perform autoscrolling instead, though...
			*/
			if (x < our startWindow)
				x = our startWindow;
			else if (x > our endWindow)
				x = our endWindow;
            
			if (x == xold)
				continue;
            
			/*
				Undraw previous dragged selection.
			*/
			if (xold > anchorForDragging) {
				x1 = anchorForDragging;
				x2 = xold;
			} else {
				x1 = xold;
				x2 = anchorForDragging;
			}
			if (x1 != x2)
				v_unhighlightSelection (x1, x2, 0.0, 1.0);
			/*
				Undraw the text.
			*/
			drawWhileDragging (this, anchorForDragging, xold);
			/*
				Redraw the text at the new location.
			*/
            drawWhileDragging (this, anchorForDragging, x);
			/*
				Draw new dragged selection.
			*/
			if (x > anchorForDragging) {
				x1 = anchorForDragging;
				x2 = x;
			} else {
				x1 = x;
				x2 = anchorForDragging;
			}
			if (x1 != x2)
				v_highlightSelection (x1, x2, 0.0, 1.0);
        } ;
		/*
			Set the new selection.
		*/
		if (x > anchorForDragging) {
			our startSelection = anchorForDragging;
			our endSelection = x;
		} else {
			our startSelection = x;
			our endSelection = anchorForDragging;
		}
	} else if (! a_shiftKeyPressed) {
		/*
			Move the cursor to the clicked position.
		*/
		our startSelection = our endSelection = xbegin;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

bool structFunctionEditor :: v_clickB (double xWC, double /* yWC */) {
	our startSelection = xWC;
	if (our startSelection > our endSelection)
		std::swap (our startSelection, our endSelection);
	return FunctionEditor_UPDATE_NEEDED;
}

bool structFunctionEditor :: v_clickE (double xWC, double /* yWC */) {
	our endSelection = xWC;
	if (our startSelection > our endSelection)
		std::swap (our startSelection, our endSelection);
	return FunctionEditor_UPDATE_NEEDED;
}

void structFunctionEditor :: v_clickSelectionViewer (double /* xWC */, double /* yWC */) {
}

int structFunctionEditor :: v_playCallback (int phase, double /* a_tmin */, double a_tmax, double t) {
	/*
	 * This callback will often be called by the Melder workproc during playback.
	 * However, it will sometimes be called by Melder_stopPlaying with phase=3.
	 * This will occur at unpredictable times, perhaps when the LongSound is updated.
	 * So we had better make no assumptions about the current viewport.
	 */
	double x1NDC, x2NDC, y1NDC, y2NDC;
	Graphics_inqViewport (our graphics.get(), & x1NDC, & x2NDC, & y1NDC, & y2NDC);
	Graphics_setViewport (our graphics.get(),
			our functionViewerLeft + MARGIN, our functionViewerRight - MARGIN,
			BOTTOM_MARGIN + space * 3, our height - (TOP_MARGIN + space));
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
	Graphics_xorOn (our graphics.get(), Melder_MAROON);
	/*
	 * Undraw the play cursor at its old location.
	 * BUG: during scrolling, zooming, and exposure, an ugly line may remain.
	 */
	if (phase != 1 && playCursor >= our startWindow && playCursor <= our endWindow) {
		Graphics_setLineWidth (our graphics.get(), 3.0);
		Graphics_line (our graphics.get(), playCursor, 0.0, playCursor, 1.0);
		Graphics_setLineWidth (our graphics.get(), 1.0);
	}
	/*
	 * Draw the play cursor at its new location.
	 */
	if (phase != 3 && t >= our startWindow && t <= our endWindow) {
		Graphics_setLineWidth (our graphics.get(), 3.0);
		Graphics_line (our graphics.get(), t, 0.0, t, 1.0);
		Graphics_setLineWidth (our graphics.get(), 1.0);
	}
	Graphics_xorOff (our graphics.get());
	if (our p_showSelectionViewer) {
		Graphics_setViewport (our graphics.get(),
				our selectionViewerLeft + MARGIN, our selectionViewerRight - MARGIN,
				BOTTOM_MARGIN + space * 3, our height - (TOP_MARGIN + space));
		our v_drawRealTimeSelectionViewer (phase, t);
	}
	/*
	 * Usually, there will be an event test after each invocation of this callback,
	 * because the asynchronicity is kMelder_asynchronicityLevel_INTERRUPTABLE or kMelder_asynchronicityLevel_ASYNCHRONOUS.
	 * However, if the asynchronicity is just kMelder_asynchronicityLevel_CALLING_BACK,
	 * there is no event test. Which means: no server round trip.
	 * Which means: no automatic flushing of graphics output.
	 * So: we force the flushing ourselves, lest we see too few moving cursors.
	 *
	 * At the moment, Cocoa seems to require this flushing even if the asynchronicity is kMelder_asynchronicityLevel_ASYNCHRONOUS.
	 */
	Graphics_flushWs (our graphics.get());
	Graphics_setViewport (our graphics.get(), x1NDC, x2NDC, y1NDC, y2NDC);
	playCursor = t;
	if (phase == 3) {
		if (t < a_tmax && MelderAudio_stopWasExplicit ()) {
			if (t > our startSelection && t < our endSelection)
				our startSelection = t;
			else
				our startSelection = our endSelection = t;
			v_updateText ();
			/*Graphics_updateWs (our graphics);*/ drawNow (this);
			updateGroup (this);
		}
		playingCursor = false;
		playingSelection = false;
	}
	return 1;
}

int theFunctionEditor_playCallback (FunctionEditor me, int phase, double a_tmin, double a_tmax, double t) {
	return my v_playCallback (phase, a_tmin, a_tmax, t);
}

void structFunctionEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	Graphics_highlight (our graphics.get(), left, right, bottom, top);
}

void structFunctionEditor :: v_unhighlightSelection (double left, double right, double bottom, double top) {
	Graphics_unhighlight (our graphics.get(), left, right, bottom, top);
}

void FunctionEditor_init (FunctionEditor me, conststring32 title, Function data) {
	my tmin = data -> xmin;   // set before adding children (see group button)
	my tmax = data -> xmax;
	Editor_init (me, 0, 0, my pref_shellWidth (), my pref_shellHeight (), title, data);

	my startWindow = my tmin;
	my endWindow = my tmax;
	my startSelection = my endSelection = 0.5 * (my tmin + my tmax);
	#if motif
		Melder_assert (XtWindow (my drawingArea -> d_widget));
	#endif
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setFontSize (my graphics.get(), 12);

// This exdents because it's a hack:
struct structGuiDrawingArea_ResizeEvent event { my drawingArea, 0, 0 };
event. width  = GuiControl_getWidth  (my drawingArea);
event. height = GuiControl_getHeight (my drawingArea);
gui_drawingarea_cb_resize (me, & event);

	my v_updateText ();
	if (group_equalDomain (my tmin, my tmax))
		gui_checkbutton_cb_group (me, nullptr);   // BUG: nullptr
	my enableUpdates = true;
}

void FunctionEditor_marksChanged (FunctionEditor me, bool needsUpdateGroup) {
	my v_updateText ();
	updateScrollBar (me);
	#if SUPPORT_DIRECT_DRAWING
		drawNow (me);
	#else
		Graphics_updateWs (my graphics.get());
	#endif
	if (needsUpdateGroup)
		updateGroup (me);
}

void FunctionEditor_updateText (FunctionEditor me) {
	my v_updateText ();
}

void FunctionEditor_redraw (FunctionEditor me) {
	#if SUPPORT_DIRECT_DRAWING
		drawNow (me);
	#else
		Graphics_updateWs (my graphics.get());
	#endif
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
	Graphics_updateWs (my graphics.get());   // for setting buttons in v_draw() method
}

void FunctionEditor_drawRangeMark (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units, int verticalAlignment) {
	static MelderString text { };
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
	int tooHigh = Graphics_dyWCtoMM (my graphics.get(), maximum - textY) < 5.0;
	int tooLow = Graphics_dyWCtoMM (my graphics.get(), textY - minimum) < 5.0;
	if (yWC < minimum || yWC > maximum) return;
	Graphics_setColour (my graphics.get(), Melder_CYAN);
	Graphics_line (my graphics.get(), 0.99 * my endWindow + 0.01 * my startWindow, yWC, my endWindow, yWC);
	Graphics_fillCircle_mm (my graphics.get(), 0.5 * (my startSelection + my endSelection), yWC, 1.5);
	if (tooHigh) {
		if (tooLow) textY = 0.5 * (minimum + maximum);
		else textY = maximum - Graphics_dyMMtoWC (my graphics.get(), 5.0);
	} else if (tooLow) {
		textY = minimum + Graphics_dyMMtoWC (my graphics.get(), 5.0);
	}
	static MelderString text { };
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
