/* FunctionEditor.c
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
 * pb 2002/05/18 Mach
 * pb 2002/07/16 GPL
 * pb 2004/02/15 highlight methods
 * pb 2005/01/11 getBottomOfSoundAndAnalysisArea
 * pb 2005/09/23 interface update
 * pb 2005/12/07 scrollStep
 * pb 2006/12/18 better info
 * pb 2006/12/21 thicker moving cursor
 * pb 2007/06/10 wchar_t
 */

#include "FunctionEditor.h"
#include "machine.h"
#include "Preferences.h"
#include "EditorM.h"

#define maximumScrollBarValue  2000000000
#define RELATIVE_PAGE_INCREMENT  0.8
#define SCROLL_INCREMENT_FRACTION  20
#if defined (UNIX)
	#define space 30
	#define margin  107
#elif defined (macintosh)
	#define space 23
	#define margin  83
#else
	#define space 23
	#define margin  83
#endif
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
	int groupWindow;
	double arrowScrollStep;
} prefs =
#ifdef UNIX
	{ 700, 440, TRUE, 0.05 };
#else
	{ 600, 340, TRUE, 0.05 };
#endif

#define maxGroup 100
static int nGroup = 0;
static FunctionEditor group [1 + maxGroup];

static int group_equalDomain (double tmin, double tmax) {
	int i;
	if (nGroup == 0) return 1;
	for (i = 1; i <= maxGroup; i ++)
		if (group [i])
			return tmin == group [i] -> tmin && tmax == group [i] -> tmax;
	return 0;   /* Should not occur. */
}

static void updateScrollBar (FunctionEditor me) {
/* We cannot call this immediately after creation. */
	int slider_size = (my endWindow - my startWindow) /
		(my tmax - my tmin) * maximumScrollBarValue - 1, increment, page_increment;
	int value = (my startWindow - my tmin) / (my tmax - my tmin) *
		maximumScrollBarValue + 1;
	if (slider_size < 1) slider_size = 1;
	if (value > maximumScrollBarValue - slider_size)
		value = maximumScrollBarValue - slider_size;
	if (value < 1) value = 1;
	XtVaSetValues (my scrollBar, XmNmaximum, maximumScrollBarValue, NULL);
	increment = slider_size / SCROLL_INCREMENT_FRACTION + 1;
	page_increment = RELATIVE_PAGE_INCREMENT * slider_size + 1;
	XmScrollBarSetValues (my scrollBar, value, slider_size, increment, page_increment, False);
}

static void updateGroup (FunctionEditor me) {
	int i;
	if (! my group) return;
	for (i = 1; i <= maxGroup; i ++) if (group [i] && group [i] != me) {
		FunctionEditor thee = group [i];
		if (prefs.groupWindow) {
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
	int i;
	int leftFromWindow = my startWindow > my tmin;
	int rightFromWindow = my endWindow < my tmax;
	int cursorVisible = my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow;
	int selection = my endSelection > my startSelection;
	int beginVisible, endVisible;
	double verticalCorrection, bottom;
	wchar_t text [100];

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

	for (i = 0; i < 8; i++) my rect [i]. left = my rect [i]. right = 0;

	/* 0: rectangle for total. */

	my rect [0]. left = leftFromWindow ? 0 : margin;
	my rect [0]. right = my width - (rightFromWindow ? 0 : margin);
	my rect [0]. bottom = BOTTOM_MARGIN;
	my rect [0]. top = BOTTOM_MARGIN + space;

	/* 1: rectangle for visible part. */

	my rect [1]. left = margin;
	my rect [1]. right = my width - margin;
	my rect [1]. bottom = BOTTOM_MARGIN + space;
	my rect [1]. top = BOTTOM_MARGIN + space * (my numberOfMarkers > 1 ? 2 : 3);

	/* 2: rectangle for left from visible part. */

	if (leftFromWindow) {
		my rect [2]. left = 0.0;
		my rect [2]. right = margin;
		my rect [2]. bottom = BOTTOM_MARGIN + space;
		my rect [2]. top = BOTTOM_MARGIN + space * 2;
	}

	/* 3: rectangle for right from visible part. */

	if (rightFromWindow) {
		my rect [3]. left = my width - margin;
		my rect [3]. right = my width;
		my rect [3]. bottom = BOTTOM_MARGIN + space;
		my rect [3]. top = BOTTOM_MARGIN + space * 2;
	}

	/* 4, 5, 6: rectangles between markers visible in visible part. */

	if (my numberOfMarkers > 1) {
		double window = my endWindow - my startWindow;
		for (i = 1; i <= my numberOfMarkers; i ++) {
			my rect [3 + i]. left = i == 1 ? margin : margin + (my width - margin * 2) *
				(my marker [i - 1] - my startWindow) / window;
			my rect [3 + i]. right = margin + (my width - margin * 2) *
				(my marker [i] - my startWindow) / window;
			my rect [3 + i]. bottom = BOTTOM_MARGIN + space * 2;
			my rect [3 + i]. top = BOTTOM_MARGIN + space * 3;
		}
	}
	
	if (selection) {
		double window = my endWindow - my startWindow;
		double left =
			my startSelection == my startWindow ? margin :
			my startSelection == my tmin ? 0.0 :
			my startSelection < my startWindow ? margin * 0.3 :
			my startSelection < my endWindow ? margin + (my width - margin * 2) * (my startSelection - my startWindow) / window :
			my startSelection == my endWindow ? my width - margin : my width - margin * 0.7;
		double right =
			my endSelection < my startWindow ? margin * 0.7 :
			my endSelection == my startWindow ? margin :
			my endSelection < my endWindow ? margin + (my width - margin * 2) * (my endSelection - my startWindow) / window :
			my endSelection == my endWindow ? my width - margin :
			my endSelection < my tmax ? my width - margin * 0.3 : my width;
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
	Graphics_setGrey (my graphics, 0.75);
	Graphics_fillRectangle (my graphics, margin, my width - margin, my height - TOP_MARGIN - space, my height);
	Graphics_fillRectangle (my graphics, 0, margin, BOTTOM_MARGIN + ( leftFromWindow ? space * 2 : 0 ), my height);
	Graphics_fillRectangle (my graphics, my width - margin, my width, BOTTOM_MARGIN + ( rightFromWindow ? space * 2 : 0 ), my height);
	Graphics_setGrey (my graphics, 0.0);
	#if defined (macintosh)
		Graphics_line (my graphics, 0, 2, my width, 2);
		Graphics_line (my graphics, 0, my height - 2, my width, my height - 2);
	#endif

	Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
	for (i = 0; i < 8; i ++) {
		double left = my rect [i]. left, right = my rect [i]. right;
		if (left < right)
			Graphics_button (my graphics, left, right, my rect [i]. bottom, my rect [i]. top);
	}
	verticalCorrection = my height / (my height - 111 + 11.0);
	#ifdef _WIN32
		verticalCorrection *= 1.5;
	#endif
	for (i = 0; i < 8; i ++) {
		double left = my rect [i]. left, right = my rect [i]. right;
		double bottom = my rect [i]. bottom, top = my rect [i]. top;
		if (left < right) {
			const wchar_t *format = our format_long;
			double value = NUMundefined, inverseValue = 0.0;
			switch (i) {
				case 0: format = our format_totalDuration, value = my tmax - my tmin; break;
				case 1: format = our format_window, value = my endWindow - my startWindow;
					/*
					 * Window domain text.
					 */	
					Graphics_setColour (my graphics, Graphics_BLUE);
					Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_HALF);
					Graphics_printfW (my graphics, left, 0.5 * (bottom + top) - verticalCorrection, our format_long, my startWindow);
					Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
					Graphics_printfW (my graphics, right, 0.5 * (bottom + top) - verticalCorrection, our format_long, my endWindow);
					Graphics_setColour (my graphics, Graphics_BLACK);
					Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
				break;
				case 2: value = my startWindow - my tmin; break;
				case 3: value = my tmax - my endWindow; break;
				case 4: value = my marker [1] - my startWindow; break;
				case 5: value = my marker [2] - my marker [1]; break;
				case 6: value = my marker [3] - my marker [2]; break;
				case 7: format = our format_selection, value = my endSelection - my startSelection, inverseValue = 1 / value; break;
			}
			swprintf (text, 100, format, value, inverseValue);
			if (Graphics_textWidthW (my graphics, text) < right - left) {
				Graphics_textW (my graphics, 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text);
			} else if (format == our format_long) {
				swprintf (text, 100, our format_short, value);
				if (Graphics_textWidthW (my graphics, text) < right - left)
					Graphics_textW (my graphics, 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text);
			} else {
				swprintf (text, 100, our format_long, value);
				if (Graphics_textWidthW (my graphics, text) < right - left) {
						Graphics_textW (my graphics, 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text);
				} else {
					swprintf (text, 100, our format_short, my endSelection - my startSelection);
					if (Graphics_textWidthW (my graphics, text) < right - left)
						Graphics_textW (my graphics, 0.5 * (left + right), 0.5 * (bottom + top) - verticalCorrection, text);
				}
			}
		}
	}

	Graphics_setViewport (my graphics, margin, my width - margin, 0, my height);
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
		Graphics_printfW (my graphics, my startSelection, my height - (TOP_MARGIN + space) - verticalCorrection, our format_long, my startSelection);
	}
	if (beginVisible && selection) {
		Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
		Graphics_printfW (my graphics, my startSelection, my height - (TOP_MARGIN + space/2) - verticalCorrection, our format_long, my startSelection);
	}
	if (endVisible && selection) {
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_HALF);
		Graphics_printfW (my graphics, my endSelection, my height - (TOP_MARGIN + space/2) - verticalCorrection, our format_long, my endSelection);
	}
	Graphics_setColour (my graphics, Graphics_BLACK);

	/*
	 * To reduce flashing, give our descendants the opportunity to prepare their data.
	 */
	our prepareDraw (me);

	/*
	 * Start of inner drawing.
	 */
	Graphics_setViewport (my graphics, margin, my width - margin, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));

	our draw (me);
	Graphics_setViewport (my graphics, margin, my width - margin, BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));

	/*
	 * Red dashed marker lines.
	 */
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0.0, 1.0);
	Graphics_setColour (my graphics, Graphics_RED);
	Graphics_setLineType (my graphics, Graphics_DOTTED);
	bottom = our getBottomOfSoundAndAnalysisArea (me);
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
		our highlightSelection (me, left, right, 0.0, 1.0);
	}

	/*
	 * End of inner drawing.
	 */
	Graphics_setViewport (my graphics, 0, my width, 0, my height);
}

/********** METHODS **********/

static void destroy (I) {
	iam (FunctionEditor);
	Melder_stopPlaying (Melder_IMPLICIT);
	if (my group) {   /* Undangle. */
		int i = 1; while (group [i] != me) { Melder_assert (i < maxGroup); i ++; } group [i] = NULL;
		nGroup --;
	}
	forget (my graphics);
	inherited (FunctionEditor) destroy (me);
}

static void updateText (Any functionEditor) {
	(void) functionEditor;
}

/********** MENU METHOD **********/

FORM (FunctionEditor, cb_prefs, "Preferences", 0);
	BOOLEAN ("Synchronize zoom and scroll", 1)
	POSITIVE ("Arrow scroll step (s)", "0.05")
	our prefs_addFields (me, cmd);
	OK
SET_INTEGER ("Synchronize zoom and scroll", 2 - prefs.groupWindow)
SET_REAL ("Arrow scroll step", my arrowScrollStep)
our prefs_setValues (me, cmd);
DO
	int oldGroupWindow = prefs.groupWindow;
	prefs.groupWindow = 2 - GET_INTEGER ("Synchronize zoom and scroll");
	prefs.arrowScrollStep = my arrowScrollStep = GET_REAL ("Arrow scroll step");
	if (oldGroupWindow == FALSE && prefs.groupWindow == TRUE) {
		updateGroup (me);
	}
	our prefs_getValues (me, cmd);
END

FORM (FunctionEditor, cb_play, "Play", 0);
	REAL ("From", "0.0")
	REAL ("To", "1.0")
	OK
SET_REAL ("From", my startWindow)
SET_REAL ("To", my endWindow)
DO
	Melder_stopPlaying (Melder_IMPLICIT);
	our play (me, GET_REAL ("From"), GET_REAL ("To"));
END

DIRECT (FunctionEditor, cb_playOrStop)
	if (Melder_isPlaying) {
		Melder_stopPlaying (Melder_EXPLICIT);
	} else if (my startSelection < my endSelection) {
		my playingSelection = TRUE;
		our play (me, my startSelection, my endSelection);
	} else {
		my playingCursor = TRUE;
		if (my startSelection == my endSelection && my startSelection > my startWindow && my startSelection < my endWindow)
			our play (me, my startSelection, my endWindow);
		else
			our play (me, my startWindow, my endWindow);
	}
END

DIRECT (FunctionEditor, cb_playWindow)
	Melder_stopPlaying (Melder_IMPLICIT);
	my playingCursor = TRUE;
	our play (me, my startWindow, my endWindow);
END

DIRECT (FunctionEditor, cb_interruptPlaying)
	Melder_stopPlaying (Melder_IMPLICIT);
END

FORM (FunctionEditor, cb_select, "Select", 0);
	REAL ("Start of selection", "0.0")
	REAL ("End of selection", "1.0")
	OK
SET_REAL ("Start of selection", my startSelection)
SET_REAL ("End of selection", my endSelection)
DO
	my startSelection = GET_REAL ("Start of selection");
	if (my startSelection < my tmin + 1e-12)
		my startSelection = my tmin;
	my endSelection = GET_REAL ("End of selection");
	if (my endSelection > my tmax - 1e-12)
		my endSelection = my tmax;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	our updateText (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
END

DIRECT (FunctionEditor, cb_moveCursorToB)
	my endSelection = my startSelection;
	our updateText (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
END

DIRECT (FunctionEditor, cb_moveCursorToE)
	my startSelection = my endSelection;
	our updateText (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
END

FORM (FunctionEditor, cb_moveCursorTo, "Move cursor to", 0)
	REAL ("Position", "0.0")
	OK
SET_REAL ("Position", 0.5 * (my startSelection + my endSelection))
DO
	double position = GET_REAL ("Position");
	if (position < my tmin + 1e-12) position = my tmin;
	if (position > my tmax - 1e-12) position = my tmax;
	my startSelection = my endSelection = position;
	our updateText (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
END

FORM (FunctionEditor, cb_moveCursorBy, "Move cursor by", 0)
	REAL ("Distance", "0.05")
	OK
DO
	double position = 0.5 * (my startSelection + my endSelection) + GET_REAL ("Distance");
	if (position < my tmin) position = my tmin;
	if (position > my tmax) position = my tmax;
	my startSelection = my endSelection = position;
	our updateText (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
END

FORM (FunctionEditor, cb_moveBby, "Move start of selection by", 0)
	REAL ("Distance", "0.05")
	OK
DO
	double position = my startSelection + GET_REAL ("Distance");
	if (position < my tmin) position = my tmin;
	if (position > my tmax) position = my tmax;
	my startSelection = position;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	our updateText (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
END

FORM (FunctionEditor, cb_moveEby, "Move end of selection by", 0);
	REAL ("Distance", "0.05")
	OK
DO
	double position = my endSelection + GET_REAL ("Distance");
	if (position < my tmin) position = my tmin;
	if (position > my tmax) position = my tmax;
	my endSelection = position;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	our updateText (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
END

DIRECT (FunctionEditor, cb_getCursor) Melder_informationRealW (0.5 * (my startSelection + my endSelection), our format_units); END
DIRECT (FunctionEditor, cb_getB) Melder_informationRealW (my startSelection, our format_units); END
DIRECT (FunctionEditor, cb_getE) Melder_informationRealW (my endSelection, our format_units); END
DIRECT (FunctionEditor, cb_getSelectionDuration) Melder_informationRealW (my endSelection - my startSelection, our format_units); END

void FunctionEditor_shift (I, double shift) {
	iam (FunctionEditor);
	double windowLength = my endWindow - my startWindow;
	Melder_stopPlaying (Melder_IMPLICIT);   /* Quickly, before window changes. */
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

DIRECT (FunctionEditor, cb_pageUp)
	FunctionEditor_shift (me, -RELATIVE_PAGE_INCREMENT * (my endWindow - my startWindow));
END

DIRECT (FunctionEditor, cb_pageDown)
	FunctionEditor_shift (me, +RELATIVE_PAGE_INCREMENT * (my endWindow - my startWindow));
END

static void scrollToView (FunctionEditor me, double t) {
	if (t <= my startWindow) {
		FunctionEditor_shift (me, t - my startWindow - 0.618 * (my endWindow - my startWindow));
	} else if (t >= my endWindow) {
		FunctionEditor_shift (me, t - my endWindow + 0.618 * (my endWindow - my startWindow));
	} else {
		FunctionEditor_marksChanged (me);
	}
}

DIRECT (FunctionEditor, cb_selectEarlier)
	my startSelection -= my arrowScrollStep;
	if (my startSelection < my tmin + 1e-12)
		my startSelection = my tmin;
	my endSelection -= my arrowScrollStep;
	if (my endSelection < my tmin + 1e-12)
		my endSelection = my tmin;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
END

DIRECT (FunctionEditor, cb_selectLater)
	my startSelection += my arrowScrollStep;
	if (my startSelection > my tmax - 1e-12)
		my startSelection = my tmax;
	my endSelection += my arrowScrollStep;
	if (my endSelection > my tmax - 1e-12)
		my endSelection = my tmax;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
END

DIRECT (FunctionEditor, cb_moveBleft)
	my startSelection -= my arrowScrollStep;
	if (my startSelection < my tmin + 1e-12)
		my startSelection = my tmin;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
END

DIRECT (FunctionEditor, cb_moveBright)
	my startSelection += my arrowScrollStep;
	if (my startSelection > my tmax - 1e-12)
		my startSelection = my tmax;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
END

DIRECT (FunctionEditor, cb_moveEleft)
	my endSelection -= my arrowScrollStep;
	if (my endSelection < my tmin + 1e-12)
		my endSelection = my tmin;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
END

DIRECT (FunctionEditor, cb_moveEright)
	my endSelection += my arrowScrollStep;
	if (my endSelection > my tmax - 1e-12)
		my endSelection = my tmax;
	scrollToView (me, 0.5 * (my startSelection + my endSelection));
END

/********** CHILD METHODS **********/

FORM (FunctionEditor, cb_zoom, "Zoom", 0);
	REAL ("From", "0.0")
	REAL ("To", "1.0")
	OK
SET_REAL ("From", my startWindow)
SET_REAL ("To", my endWindow)
DO
	my startWindow = GET_REAL ("From");
	if (my startWindow < my tmin + 1e-12)
		my startWindow = my tmin;
	my endWindow = GET_REAL ("To");
	if (my endWindow > my tmax - 1e-12)
		my endWindow = my tmax;
	our updateText (me);
	updateScrollBar (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
END

MOTIF_CALLBACK (cb_zoomIn)
	iam (FunctionEditor);
	double shift = (my endWindow - my startWindow) / 4;
	my startWindow += shift;
	my endWindow -= shift;
	our updateText (me);
	updateScrollBar (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_zoomOut)
	iam (FunctionEditor);
	double shift = (my endWindow - my startWindow) / 2;
	Melder_stopPlaying (Melder_IMPLICIT);   /* Quickly, before window changes. */
	my startWindow -= shift;
	if (my startWindow < my tmin + 1e-12)
		my startWindow = my tmin;
	my endWindow += shift;
	if (my endWindow > my tmax - 1e-12)
		my endWindow = my tmax;
	our updateText (me);
	updateScrollBar (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_showAll)
	iam (FunctionEditor);
	my startWindow = my tmin;
	my endWindow = my tmax;
	our updateText (me);
	updateScrollBar (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_zoomToSelection)
	iam (FunctionEditor);
	if (my endSelection > my startSelection) {
		my startWindow = my startSelection;
		my endWindow = my endSelection;
		our updateText (me);
		updateScrollBar (me);
		/*Graphics_updateWs (my graphics);*/ drawNow (me);
		updateGroup (me);
	}
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_scroll)
	iam (FunctionEditor);
	int value, slider, incr, pincr;
	double shift;
	XmScrollBarGetValues (w, & value, & slider, & incr, & pincr);
	shift = my tmin + (value - 1) * (my tmax - my tmin) / maximumScrollBarValue - my startWindow;
	if (shift != 0.0) {
		int i;
		my startWindow += shift;
		if (my startWindow < my tmin + 1e-12) my startWindow = my tmin;
		my endWindow += shift;
		if (my endWindow > my tmax - 1e-12) my endWindow = my tmax;
		our updateText (me);
		/*Graphics_clearWs (my graphics);*/
		drawNow (me);   /* Do not wait for expose event. */
		if (! my group || ! prefs.groupWindow) return;
		for (i = 1; i <= maxGroup; i ++) if (group [i] && group [i] != me) {
			group [i] -> startWindow = my startWindow;
			group [i] -> endWindow = my endWindow;
			FunctionEditor_updateText (group [i]);
			updateScrollBar (group [i]);
			Graphics_clearWs (group [i] -> graphics);
			drawNow (group [i]);
		}
	}
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_resize)
	iam (FunctionEditor);
	Dimension width, height, marginWidth = 10, marginHeight = 10, shellWidth, shellHeight;
	XtVaGetValues (w, XmNwidth, & width, XmNheight, & height,
		XmNmarginWidth, & marginWidth, XmNmarginHeight, & marginHeight, NULL);
	Graphics_setWsViewport (my graphics, marginWidth, width - marginWidth, marginHeight, height - marginHeight);
	my width = width - marginWidth - marginWidth + 111;
	my height = height - marginHeight - marginHeight + 111;
	Graphics_setWsWindow (my graphics, 0, my width, 0, my height);
	Graphics_setViewport (my graphics, 0, my width, 0, my height);
	Graphics_updateWs (my graphics);

	/* Save the current shell size as the user's preference for a new FunctionEditor. */

	XtVaGetValues (my shell, XmNwidth, & shellWidth, XmNheight, & shellHeight, NULL);
	prefs.shellWidth = shellWidth;
	prefs.shellHeight = shellHeight;
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_group)
	iam (FunctionEditor);
	int i;
	my group = ! my group;
	if (my group) {
		FunctionEditor thee;
		i = 1; while (group [i]) i ++; group [i] = me;
		if (++ nGroup == 1) { Graphics_updateWs (my graphics); return; }
		i = 1; while (group [i] == NULL || group [i] == me) i ++; thee = group [i];
		if (prefs.groupWindow) {
			my startWindow = thy startWindow;
			my endWindow = thy endWindow;
		}
		my startSelection = thy startSelection;
		my endSelection = thy endSelection;
		if (my tmin > thy tmin || my tmax < thy tmax) {
			if (my tmin > thy tmin) my tmin = thy tmin;
			if (my tmax < thy tmax) my tmax = thy tmax;
			our updateText (me);
			updateScrollBar (me);
			Graphics_updateWs (my graphics);
		} else {
			our updateText (me);
			updateScrollBar (me);
			Graphics_updateWs (my graphics);
			if (my tmin < thy tmin || my tmax > thy tmax)
				for (i = 1; i <= maxGroup; i ++) if (group [i] && group [i] != me) {
					if (my tmin < thy tmin)
						group [i] -> tmin = my tmin;
					if (my tmax > thy tmax)
						group [i] -> tmax = my tmax;
					FunctionEditor_updateText (group [i]);
					updateScrollBar (group [i]);
					Graphics_updateWs (group [i] -> graphics);
				}
		}
	} else {
		i = 1; while (group [i] != me) i ++; group [i] = NULL;
		nGroup --;
		our updateText (me);
		Graphics_updateWs (my graphics);   /* For setting buttons in draw method. */
	}
	if (my group) updateGroup (me);
MOTIF_CALLBACK_END

DIRECT (FunctionEditor, DO_showAll) cb_showAll (0, me, 0); END
DIRECT (FunctionEditor, DO_zoomIn) cb_zoomIn (0, me, 0); END
DIRECT (FunctionEditor, DO_zoomOut) cb_zoomOut (0, me, 0); END
DIRECT (FunctionEditor, DO_zoomToSelection) cb_zoomToSelection (0, me, 0); END

DIRECT (FunctionEditor, DO_intro) Melder_help ("Intro"); END

static void createMenus (I) {
	iam (FunctionEditor);
	inherited (FunctionEditor) createMenus (me);

	Editor_addCommand (me, L"File", L"Preferences...", 0, cb_prefs);
	Editor_addCommand (me, L"File", L"-- after prefs --", 0, 0);

	Editor_addMenu (me, L"Query", 0);
	Editor_addCommand (me, L"Query", L"Get start of selection", 0, cb_getB);
	Editor_addCommand (me, L"Query", L"Get begin of selection", Editor_HIDDEN, cb_getB);
	Editor_addCommand (me, L"Query", L"Get cursor", motif_F6, cb_getCursor);
	Editor_addCommand (me, L"Query", L"Get end of selection", 0, cb_getE);
	Editor_addCommand (me, L"Query", L"Get selection length", 0, cb_getSelectionDuration);

	Editor_addMenu (me, L"View", 0);
	our viewMenuEntries (me);
	Editor_addCommand (me, L"View", our format_domain, motif_INSENSITIVE, cb_prefs /* dummy */);
	Editor_addCommand (me, L"View", L"Zoom...", 0, cb_zoom);
	Editor_addCommand (me, L"View", L"Show all", 'A', DO_showAll);
	Editor_addCommand (me, L"View", L"Zoom in", 'I', DO_zoomIn);
	Editor_addCommand (me, L"View", L"Zoom out", 'O', DO_zoomOut);
	Editor_addCommand (me, L"View", L"Zoom to selection", 'N', DO_zoomToSelection);
	Editor_addCommand (me, L"View", L"Scroll page back", motif_PAGE_UP, cb_pageUp);
	Editor_addCommand (me, L"View", L"Scroll page forward", motif_PAGE_DOWN, cb_pageDown);
	Editor_addCommand (me, L"View", L"-- play --", 0, 0);
	Editor_addCommand (me, L"View", L"Audio:", motif_INSENSITIVE, cb_prefs /* dummy */);
	Editor_addCommand (me, L"View", L"Play...", 0, cb_play);
	Editor_addCommand (me, L"View", L"Play or stop", motif_TAB, cb_playOrStop);
	Editor_addCommand (me, L"View", L"Play window", motif_SHIFT + motif_TAB, cb_playWindow);
	Editor_addCommand (me, L"View", L"Interrupt playing", motif_ESCAPE, cb_interruptPlaying);

	Editor_addMenu (me, L"Select", 0);
	Editor_addCommand (me, L"Select", L"Select...", 0, cb_select);
	Editor_addCommand (me, L"Select", L"Move cursor to start of selection", 0, cb_moveCursorToB);
	Editor_addCommand (me, L"Select", L"Move cursor to begin of selection", Editor_HIDDEN, cb_moveCursorToB);
	Editor_addCommand (me, L"Select", L"Move cursor to end of selection", 0, cb_moveCursorToE);
	Editor_addCommand (me, L"Select", L"Move cursor to...", 0, cb_moveCursorTo);
	Editor_addCommand (me, L"Select", L"Move cursor by...", 0, cb_moveCursorBy);
	Editor_addCommand (me, L"Select", L"Move start of selection by...", 0, cb_moveBby);
	Editor_addCommand (me, L"Select", L"Move begin of selection by...", Editor_HIDDEN, cb_moveBby);
	Editor_addCommand (me, L"Select", L"Move end of selection by...", 0, cb_moveEby);
	/*Editor_addCommand (me, L"Select", L"Move cursor back by half a second", motif_, cb_moveCursorBy);*/
	Editor_addCommand (me, L"Select", L"Select earlier", motif_UP_ARROW, cb_selectEarlier);
	Editor_addCommand (me, L"Select", L"Select later", motif_DOWN_ARROW, cb_selectLater);
	Editor_addCommand (me, L"Select", L"Move start of selection left", motif_SHIFT + motif_UP_ARROW, cb_moveBleft);
	Editor_addCommand (me, L"Select", L"Move begin of selection left", Editor_HIDDEN, cb_moveBleft);
	Editor_addCommand (me, L"Select", L"Move start of selection right", motif_SHIFT + motif_DOWN_ARROW, cb_moveBright);
	Editor_addCommand (me, L"Select", L"Move begin of selection right", Editor_HIDDEN, cb_moveBright);
	Editor_addCommand (me, L"Select", L"Move end of selection left", motif_COMMAND + motif_UP_ARROW, cb_moveEleft);
	Editor_addCommand (me, L"Select", L"Move end of selection right", motif_COMMAND + motif_DOWN_ARROW, cb_moveEright);

	Editor_addCommand (me, L"Help", L"Intro", 0, DO_intro);
}

static void createChildren (I) {
	iam (FunctionEditor);
	Widget form, button;
	int x = BUTTON_X;

	form = XmCreateForm (my dialog, "buttons", NULL, 0);
	XtVaSetValues (form,
		XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
		XmNbottomAttachment, XmATTACH_FORM,
		XmNtraversalOn, False,   /* Needed in order to redirect all keyboard input to the text widget. */
		NULL);

	/***** Create zoom buttons. *****/

	button = XtVaCreateManagedWidget ("all", xmPushButtonWidgetClass, form,
		XmNx, x,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 4,
		XmNheight, Machine_getScrollBarWidth () + 2, XmNwidth, BUTTON_WIDTH, NULL);
	XtAddCallback (button, XmNactivateCallback, cb_showAll, (XtPointer) me);
	button = XtVaCreateManagedWidget ("in", xmPushButtonWidgetClass, form,
		XmNx, x += BUTTON_WIDTH + BUTTON_SPACING,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 4,
		XmNheight, Machine_getScrollBarWidth () + 2, XmNwidth, BUTTON_WIDTH, NULL);
	XtAddCallback (button, XmNactivateCallback, cb_zoomIn, (XtPointer) me);
	button = XtVaCreateManagedWidget ("out", xmPushButtonWidgetClass, form,
		XmNx, x += BUTTON_WIDTH + BUTTON_SPACING,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 4,
		XmNheight, Machine_getScrollBarWidth () + 2, XmNwidth, BUTTON_WIDTH, NULL);
	XtAddCallback (button, XmNactivateCallback, cb_zoomOut, (XtPointer) me);
	button = XtVaCreateManagedWidget ("sel", xmPushButtonWidgetClass, form,
		XmNx, x += BUTTON_WIDTH + BUTTON_SPACING,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 4,
		XmNheight, Machine_getScrollBarWidth () + 2, XmNwidth, BUTTON_WIDTH, NULL);
	XtAddCallback (button, XmNactivateCallback, cb_zoomToSelection, (XtPointer) me);

	/***** Create scroll bar. *****/

	my scrollBar = XtVaCreateManagedWidget ("scrollBar",
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

	/***** Create Group button. *****/

	my groupButton = XtVaCreateManagedWidget ("Group", xmToggleButtonWidgetClass, form,
		XmNrightAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 4,
		XmNheight, Machine_getScrollBarWidth () + 1, XmNwidth, 80, NULL);
	XtAddCallback (my groupButton, XmNvalueChangedCallback, cb_group, (XtPointer) me);
	XmToggleButtonSetState (my groupButton, group_equalDomain (my tmin, my tmax), False);

	/***** Create drawing area. *****/

	my drawingArea = XmCreateDrawingArea (form, "drawingArea", NULL, 0);
	XtVaSetValues (my drawingArea,
		XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, our hasText ? TEXT_HEIGHT : 0,
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, Machine_getScrollBarWidth () + 9,
		NULL);
	XtManageChild (my drawingArea);

	/***** Create optional text field. *****/

	if (our hasText) {
		my text = XmCreateText (form, "text", NULL, 0);
		XtVaSetValues (my text,
			XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
			XmNtopAttachment, XmATTACH_FORM,
			XmNheight, TEXT_HEIGHT,
			XmNwordWrap, True,
			XmNeditMode, XmMULTI_LINE_EDIT,   /* Otherwise, cannot wrap. */
			NULL);
		XtManageChild (my text);
		/*
		 * X Toolkit 4:184,461 says: "you should never call XtSetKeyboardFocus",
		 * "since it interferes with the keyboard traversal code".
		 * That's true, we needed to switch traversal off for 'form' (see above).
		 * But does anyone know of an alternative?
		 * Our simple and natural desire is that all keyboard input shall go to the only text widget
		 * in the window (in our Motif emulator, this is the automatic behaviour).
		 */
		#ifdef UNIX
			XtSetKeyboardFocus (form, my text);
		#endif
	}

	XtManageChild (form);
}

static void dataChanged (I) {
	iam (FunctionEditor);
	Function function = my data;
	Melder_assert (Thing_member (function, classFunction));
	my tmin = function -> xmin;
 	my tmax = function -> xmax;
 	if (my startWindow < my tmin || my startWindow > my tmax) my startWindow = my tmin;
 	if (my endWindow < my tmin || my endWindow > my tmax) my endWindow = my tmax;
 	if (my startWindow >= my endWindow) { my startWindow = my tmin; my endWindow = my tmax; }
 	if (my startSelection < my tmin) my startSelection = my tmin;
 	if (my startSelection > my tmax) my startSelection = my tmax;
 	if (my endSelection < my tmin) my endSelection = my tmin;
 	if (my endSelection > my tmax) my endSelection = my tmax;
	FunctionEditor_marksChanged (me);
}

static void draw (Any functionEditor) {
	(void) functionEditor;
}

static void prepareDraw (Any functionEditor) {
	(void) functionEditor;
}

static void play (Any functionEditor, double tmin, double tmax) {
	(void) functionEditor;
	(void) tmin;
	(void) tmax;
}

static void drawWhileDragging (FunctionEditor me, double x1, double x2) {
	/*
	 * We must draw this within the window, because the window tends to have a white background.
	 * We cannot draw this in the margins, because these tend to be grey, so that Graphics_xorOn does not work properly.
	 * We draw the text twice, because we expect that not ALL of the window is white...
	 */
	double xleft, xright;
	if (x1 > x2) xleft = x2, xright = x1; else xleft = x1, xright = x2;
	Graphics_xorOn (my graphics, Graphics_MAGENTA);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_TOP);
	Graphics_printf (my graphics, xleft, 1.0, "%f", xleft);
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_TOP);
	Graphics_printf (my graphics, xright, 1.0, "%f", xright);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_BOTTOM);
	Graphics_printf (my graphics, xleft, 0.0, "%f", xleft);
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_BOTTOM);
	Graphics_printf (my graphics, xright, 0.0, "%f", xright);
	Graphics_xorOff (my graphics);
}

static int click (I, double xbegin, double ybegin, int shiftKeyPressed) {
	iam (FunctionEditor);
	int drag = FALSE;
	double x = xbegin, y = ybegin, x1, x2;
	/*
	 * The 'anchor' is the point that will stay fixed during dragging.
	 * For instance, if she clicks and drags to the right,
	 * the location at which she originally clicked will be the anchor,
         * even if she later chooses to drag the mouse to the left of it.
	 * Another example: if she shift-clicks near E, B will become (and stay) the anchor.
	 */
	double anchorForDragging;
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0, 1);
	if (shiftKeyPressed) {
		/*
		 * Extend the selection.
		 * We should always end up with a real selection (B < E),
		 * even if we start with the reversed temporal order (E < B).
		 */
		int reversed = my startSelection > my endSelection;
		double firstMark = reversed ? my endSelection : my startSelection;
		double secondMark = reversed ? my startSelection : my endSelection;
		/*
		 * Undraw the old selection.
		 */
		if (my endSelection > my startSelection) {
			/*
			 * Determine the visible part of the old selection.
			 */
			double startVisible = my startSelection > my startWindow ? my startSelection : my startWindow;
			double endVisible = my endSelection < my endWindow ? my endSelection : my endWindow;
			/*
			 * Undraw the visible part of the old selection.
			 */
			if (endVisible > startVisible)
				our unhighlightSelection (me, startVisible, endVisible, 0, 1);
		}
		if (xbegin >= secondMark) {
		 	/*
			 * She clicked right from the second mark (usually E). We move E.
			 */
			my endSelection = xbegin;
			anchorForDragging = my startSelection;
		} else if (xbegin <= firstMark) {
		 	/*
			 * She clicked left from the first mark (usually B). We move B.
			 */
			my startSelection = xbegin;
			anchorForDragging = my endSelection;
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
				my startSelection = firstMark;
				my endSelection = secondMark;
			}
			/*
			 * Move the nearest mark.
			 */
			if (distanceOfClickToFirstMark < distanceOfClickToSecondMark) {
				my startSelection = xbegin;
				anchorForDragging = my endSelection;
			} else {
				my endSelection = xbegin;
				anchorForDragging = my startSelection;
			}
		}
		/*
		 * Draw the new selection.
		 */
		if (my endSelection > my startSelection) {
			/*
			 * Determine the visible part of the new selection.
			 */
			double startVisible = my startSelection > my startWindow ? my startSelection : my startWindow;
			double endVisible = my endSelection < my endWindow ? my endSelection : my endWindow;
			/*
			 * Draw the visible part of the new selection.
			 */
			if (endVisible > startVisible)
				our highlightSelection (me, startVisible, endVisible, 0, 1);
		}
	}
	/*
	 * Find out whether this is a click or a drag.
	 */
	while (Graphics_mouseStillDown (my graphics)) {
		Graphics_getMouseLocation (my graphics, & x, & y);
		if (x < my startWindow) x = my startWindow;
		if (x > my endWindow) x = my endWindow;
		if (fabs (Graphics_dxWCtoMM (my graphics, x - xbegin)) > 1.5) {
			drag = TRUE;
			break;
		}
	}
	if (drag) {
		if (! shiftKeyPressed) {
			anchorForDragging = xbegin;
			/*
			 * We will drag and create a new selection.
			 */
		}
		/*
		 * First undraw the old selection.
		 */
		if (my endSelection > my startSelection) {
			/*
			 * Determine the visible part of the old selection.
			 */
			double startVisible = my startSelection > my startWindow ? my startSelection : my startWindow;
			double endVisible = my endSelection < my endWindow ? my endSelection : my endWindow;
			/*
			 * Undraw the visible part of the old selection.
			 */
			if (endVisible > startVisible)
				our unhighlightSelection (me, startVisible, endVisible, 0, 1);
		}
		/*
		 * Draw the text at least once.
		 */
		/*if (x < my startWindow) x = my startWindow; else if (x > my endWindow) x = my endWindow;*/
		drawWhileDragging (me, anchorForDragging, x);
		/*
		 * Draw the dragged selection at least once.
		 */
		if (x > anchorForDragging) x1 = anchorForDragging, x2 = x; else x1 = x, x2 = anchorForDragging;
		our highlightSelection (me, x1, x2, 0, 1);
		/*
		 * Drag for the new selection.
		 */
		while (Graphics_mouseStillDown (my graphics)) {
			double xold = x, x1, x2;
			Graphics_getMouseLocation (my graphics, & x, & y);
			/*
			 * Clip to the visible window. Ideally, we should perform autoscrolling instead, though...
			 */
			if (x < my startWindow) x = my startWindow; else if (x > my endWindow) x = my endWindow;
			if (x == xold) continue;
			/*
			 * Undraw and redraw the text at the top.
			 */
			drawWhileDragging (me, anchorForDragging, xold);
			/*
			 * Remove previous dragged selection.
			 */
			if (xold > anchorForDragging) x1 = anchorForDragging, x2 = xold; else x1 = xold, x2 = anchorForDragging;
			if (x1 != x2) our unhighlightSelection (me, x1, x2, 0, 1);
			/*
			 * Draw new dragged selection.
			 */
			if (x > anchorForDragging) x1 = anchorForDragging, x2 = x; else x1 = x, x2 = anchorForDragging;
			if (x1 != x2) our highlightSelection (me, x1, x2, 0, 1);
			/*
			 * Redraw the text at the top.
			 */
			drawWhileDragging (me, anchorForDragging, x);
		} ;
		/*
		 * Set the new selection.
		 */
		if (x > anchorForDragging) my startSelection = anchorForDragging, my endSelection = x;
		else my startSelection = x, my endSelection = anchorForDragging;
	} else if (! shiftKeyPressed) {
		/*
		 * Move the cursor to the clicked position.
		 */
		my startSelection = my endSelection = xbegin;
	}
	return FunctionEditor_UPDATE_NEEDED;
}

static int clickB (I, double xWC, double yWC) {
	iam (FunctionEditor);
	(void) yWC;
	my startSelection = xWC;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	return 1;
}

static int clickE (I, double xWC, double yWC) {
	iam (FunctionEditor);
	my endSelection = xWC;
	(void) yWC;
	if (my startSelection > my endSelection) {
		double dummy = my startSelection;
		my startSelection = my endSelection;
		my endSelection = dummy;
	}
	return 1;
}

static void key (Any functionEditor, unsigned char key) {
	(void) functionEditor;
	(void) key;
}

void FunctionEditor_insetViewport (I) {
	iam (FunctionEditor);
	Graphics_setViewport (my graphics, margin, my width - margin,
		BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0, 1);
}

static int playCallback (I, int phase, double tmin, double tmax, double t) {
	/*
	 * This callback will often be called by the Melder workproc during playback.
	 * However, it will sometimes be called by Melder_stopPlaying with phase=3.
	 * This will occur at unpredictable times, perhaps when the LongSound is updated.
	 * So we had better make no assumptions about the current viewport.
	 */
	iam (FunctionEditor);
	double x1NDC, x2NDC, y1NDC, y2NDC;
	(void) tmin;
	Graphics_inqViewport (my graphics, & x1NDC, & x2NDC, & y1NDC, & y2NDC);
	FunctionEditor_insetViewport (me);
	Graphics_xorOn (my graphics, Graphics_MAGENTA);
	/*
	 * Undraw the play cursor at its old location.
	 * BUG: during scrolling, zooming, and exposure, an ugly line may remain.
	 */
	if (phase != 1 && my playCursor >= my startWindow && my playCursor <= my endWindow) {
		Graphics_setLineWidth (my graphics, 3.0);
		Graphics_line (my graphics, my playCursor, 0, my playCursor, 1);
		Graphics_setLineWidth (my graphics, 1.0);
	}
	/*
	 * Draw the play cursor at its new location.
	 */
	if (phase != 3 && t >= my startWindow && t <= my endWindow) {
		Graphics_setLineWidth (my graphics, 3.0);
		Graphics_line (my graphics, t, 0, t, 1);
		Graphics_setLineWidth (my graphics, 1.0);
	}
	Graphics_xorOff (my graphics);
	/*
	 * Usually, there will be an event test after each invocation of this callback,
	 * because the asynchronicity is Melder_INTERRUPTABLE or Melder_ASYNCHRONOUS.
	 * However, if the asynchronicity is just Melder_CALLING_BACK,
	 * there is no event test. Which means: no server round trip.
	 * Which means: no automatic flushing of graphics output.
	 * So: we force the flushing ourselves, lest we see too few moving cursors.
	 */
	Graphics_flushWs (my graphics);
	Graphics_setViewport (my graphics, x1NDC, x2NDC, y1NDC, y2NDC);
	my playCursor = t;
	if (phase == 3) {
		if (t < tmax && Melder_stopWasExplicit ()) {
			if (t > my startSelection && t < my endSelection)
				my startSelection = t;
			else
				my startSelection = my endSelection = t;
			our updateText (me);
			/*Graphics_updateWs (my graphics);*/ drawNow (me);
			updateGroup (me);
		}
		my playingCursor = FALSE;
		my playingSelection = FALSE;
	}
	return 1;
}

static void prefs_addFields (Any functionEditor, EditorCommand cmd) {
	(void) functionEditor;
	(void) cmd;
}
static void prefs_setValues (Any functionEditor, EditorCommand cmd) {
	(void) functionEditor;
	(void) cmd;
}
static void prefs_getValues (Any functionEditor, EditorCommand cmd) {
	(void) functionEditor;
	(void) cmd;
}
static void viewMenuEntries (Any functionEditor) {
	(void) functionEditor;
}

static void highlightSelection (I, double left, double right, double bottom, double top) {
	iam (FunctionEditor);
	Graphics_highlight (my graphics, left, right, bottom, top);
}

static void unhighlightSelection (I, double left, double right, double bottom, double top) {
	iam (FunctionEditor);
	Graphics_unhighlight (my graphics, left, right, bottom, top);
}

static double getBottomOfSoundAndAnalysisArea (I) {
	iam (FunctionEditor);
	(void) me;
	return 0.0;
}

class_methods (FunctionEditor, Editor)
	class_method (destroy)
	class_method (createMenus)
	class_method (createChildren)
	class_method (dataChanged)
	class_method (draw)
	class_method (prepareDraw)
	us -> hasText = FALSE;
	us -> format_domain = L"Time domain:";
	us -> format_short = L"%.3f";
	us -> format_long = L"%f";
	us -> format_units = L"seconds";
	us -> format_totalDuration = L"Total duration %f seconds";
	us -> format_window = L"Visible part %f seconds";
	us -> format_selection = L"%f (%.3f / s)";
	class_method (play)
	class_method (click)
	class_method (clickB)
	class_method (clickE)
	class_method (key)
	class_method (playCallback)
	class_method (updateText)
	class_method (prefs_addFields)
	class_method (prefs_setValues)
	class_method (prefs_getValues)
	class_method (viewMenuEntries)
	class_method (highlightSelection)
	class_method (unhighlightSelection)
	class_method (getBottomOfSoundAndAnalysisArea)
class_methods_end

MOTIF_CALLBACK (cb_draw)
	iam (FunctionEditor);
#ifdef UNIX
	if (((XmDrawingAreaCallbackStruct *) call) -> event -> xexpose. count) return;
#endif
	if (my enableUpdates)
		drawNow (me);
MOTIF_CALLBACK_END

static void cb_buttonPress (FunctionEditor me, MotifEvent event) {
	double xWC, yWC;
	#ifdef UNIX
		int leftHanded = FALSE;
		unsigned char map [4];
		XGetPointerMapping (XtDisplay (my shell), map, 3);
		if (map [0] == 3) leftHanded = TRUE;
	#endif
	my shiftKeyPressed = MotifEvent_shiftKeyPressed (event);
	Graphics_setWindow (my graphics, 0, my width, 0, my height);
	Graphics_DCtoWC (my graphics, MotifEvent_x (event), MotifEvent_y (event), & xWC, & yWC);

	if (yWC > BOTTOM_MARGIN + space * 3 && yWC < my height - (TOP_MARGIN + space)) {   /* In signal region? */
		int needsUpdate;
		Graphics_setViewport (my graphics, margin, my width - margin,
			BOTTOM_MARGIN + space * 3, my height - (TOP_MARGIN + space));
		Graphics_setWindow (my graphics, my startWindow, my endWindow, 0.0, 1.0);
		Graphics_DCtoWC (my graphics, MotifEvent_x (event), MotifEvent_y (event), & xWC, & yWC);
		if (xWC < my startWindow) xWC = my startWindow;
		if (xWC > my endWindow) xWC = my endWindow;
#if defined (macintosh)
		needsUpdate =
			! (event -> modifiers & (optionKey | cmdKey | controlKey)) ?
				our click (me, xWC, yWC, my shiftKeyPressed) :
			event -> modifiers & (optionKey | controlKey) ?
				our clickB (me, xWC, yWC) :
			/* Command key. */
				our clickE (me, xWC, yWC);
#elif defined (_WIN32)
		needsUpdate = our click (me, xWC, yWC, my shiftKeyPressed);
#else
		needsUpdate =
			((XButtonPressedEvent *) event) -> button == Button1   /* Left button? */ ?
				our click (me, xWC, yWC, my shiftKeyPressed) :
			((XButtonPressedEvent *) event) -> button == (leftHanded ? Button3 : Button2)   /* Middle button? */ ?
				our clickB (me, xWC, yWC) :
			/* Right button. */
				our clickE (me, xWC, yWC);
#endif
		if (needsUpdate) our updateText (me);
		Graphics_setViewport (my graphics, 0, my width, 0, my height);
		if (needsUpdate) /*Graphics_updateWs (my graphics);*/ drawNow (me);
		if (needsUpdate) updateGroup (me);
	}
	else   /* Clicked outside signal region? Let us hear it. */
	{
		int i;
		for (i = 0; i < 8; i ++)
			if (xWC > my rect [i]. left && xWC < my rect [i]. right &&
				 yWC > my rect [i]. bottom && yWC < my rect [i]. top)
				switch (i) {
					case 0: our play (me, my tmin, my tmax); break;
					case 1: our play (me, my startWindow, my endWindow); break;
					case 2: our play (me, my tmin, my startWindow); break;
					case 3: our play (me, my endWindow, my tmax); break;
					case 4: our play (me, my startWindow, my marker [1]); break;
					case 5: our play (me, my marker [1], my marker [2]); break;
					case 6: our play (me, my marker [2], my marker [3]); break;
					case 7: our play (me, my startSelection, my endSelection); break;
				}
	}
}

static void cb_keyPress (FunctionEditor me, MotifEvent event)
{
#if defined (macintosh)
	enum { charCodeMask = 0x000000FF };
	unsigned char key = event -> message & charCodeMask;
#elif defined (_WIN32)
	char key = event -> key;
#else
	char key;
	XLookupString (& event -> xkey, & key, 1, NULL, NULL);
#endif
	our key (me, key);
}

MOTIF_CALLBACK (cb_input)
	iam (FunctionEditor);
	MotifEvent event = MotifEvent_fromCallData (call);
	if (MotifEvent_isButtonPressedEvent (event))
		cb_buttonPress (me, event);
	else if (MotifEvent_isKeyPressedEvent (event))
		cb_keyPress (me, event);
MOTIF_CALLBACK_END

int FunctionEditor_init (I, Widget parent, const wchar_t *title, Any data) {
	iam (FunctionEditor);
	my tmin = ((Function) data) -> xmin;   /* Set before adding children (see group button). */
	my tmax = ((Function) data) -> xmax;
	if (! Editor_init (me, parent, 0, 0, prefs.shellWidth, prefs.shellHeight, title, data)) return 0;

	my startWindow = my tmin;
	my endWindow = my tmax;
	my startSelection = my endSelection = 0.5 * (my tmin + my tmax);
	Melder_assert (XtWindow (my drawingArea));
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
Graphics_setFontSize (my graphics, 10);
cb_resize (my drawingArea, (XtPointer) me, 0);

	/* Callbacks only now that "graphics" exists. */   
	XtAddCallback (my drawingArea, XmNexposeCallback, cb_draw, (XtPointer) me);
	XtAddCallback (my drawingArea, XmNinputCallback, cb_input, (XtPointer) me);
	XtAddCallback (my drawingArea, XmNresizeCallback, cb_resize, (XtPointer) me);
	XtAddCallback (my scrollBar, XmNvalueChangedCallback, cb_scroll, (XtPointer) me);
	XtAddCallback (my scrollBar, XmNdragCallback, cb_scroll, (XtPointer) me);
	our updateText (me);
	if (group_equalDomain (my tmin, my tmax))
		cb_group (NULL, (XtPointer) me, NULL);
	my enableUpdates = TRUE;
	my arrowScrollStep = prefs.arrowScrollStep;
	return 1;
}

void FunctionEditor_marksChanged (I) {
	iam (FunctionEditor);
	our updateText (me);
	updateScrollBar (me);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
	updateGroup (me);
}

void FunctionEditor_updateText (I) {
	iam (FunctionEditor);
	our updateText (me);
}

void FunctionEditor_redraw (I) {
	iam (FunctionEditor);
	/*Graphics_updateWs (my graphics);*/ drawNow (me);
}

void FunctionEditor_enableUpdates (I, int enable) {
	iam (FunctionEditor);
	my enableUpdates = enable;
}

void FunctionEditor_ungroup (I) {
	iam (FunctionEditor);
	int i = 1;
	if (! my group) return;
	my group = FALSE;
	XmToggleButtonSetState (my groupButton, False, False);
	while (group [i] != me) i ++;
	group [i] = NULL;
	nGroup --;
	our updateText (me);
	Graphics_updateWs (my graphics);   /* For setting buttons in draw method. */
}

void FunctionEditor_prefs (void) {
	Resources_addInt ("FunctionEditor.shellWidth", & prefs.shellWidth);
	Resources_addInt ("FunctionEditor.shellHeight", & prefs.shellHeight);
	Resources_addInt ("FunctionEditor.groupWindow", & prefs.groupWindow);
	Resources_addDouble ("FunctionEditor.arrowScrollStep", & prefs.arrowScrollStep);
}

void FunctionEditor_drawRangeMark (I, const wchar_t *format, double yWC, int verticalAlignment) {
	iam (FunctionEditor);
	wchar_t text [100];
	double textWidth;
	swprintf (text, 100, format, yWC);
	textWidth = Graphics_textWidthW (my graphics, text) + Graphics_dxMMtoWC (my graphics, 0.5);
	Graphics_setColour (my graphics, Graphics_BLUE);
	Graphics_line (my graphics, my endWindow, yWC, my endWindow + textWidth, yWC);
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, verticalAlignment);
	if (verticalAlignment == Graphics_BOTTOM) yWC -= Graphics_dyMMtoWC (my graphics, 0.5);
	Graphics_textW (my graphics, my endWindow, yWC, text);
}

void FunctionEditor_drawCursorFunctionValue (I, const wchar_t *format, double yWC) {
	iam (FunctionEditor);
	Graphics_setColour (my graphics, Graphics_CYAN);
	Graphics_line (my graphics, my startWindow, yWC, 0.99 * my startWindow + 0.01 * my endWindow, yWC);
	Graphics_fillCircle_mm (my graphics, 0.5 * (my startSelection + my endSelection), yWC, 1.5);
	Graphics_setColour (my graphics, Graphics_BLUE);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_printfW (my graphics, my startWindow, yWC, format, yWC);
}

void FunctionEditor_insertCursorFunctionValue (I, const wchar_t *format, double yWC, double minimum, double maximum) {
	iam (FunctionEditor);
	wchar_t text [100];
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
	swprintf (text, 100, format, yWC);
	textWidth = Graphics_textWidthW (my graphics, text);
	Graphics_fillCircle_mm (my graphics, my endWindow + textWidth + Graphics_dxMMtoWC (my graphics, 1.5), textY, 1.5);
	Graphics_setColour (my graphics, Graphics_RED);
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_textW (my graphics, textX, textY, text);
}

void FunctionEditor_drawHorizontalHair (I, const wchar_t *format, double yWC) {
	iam (FunctionEditor);
	Graphics_setColour (my graphics, Graphics_RED);
	Graphics_line (my graphics, my startWindow, yWC, my endWindow, yWC);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_printfW (my graphics, my startWindow, yWC, format, yWC);
}

void FunctionEditor_drawGridLine (I, double yWC) {
	iam (FunctionEditor);
	Graphics_setColour (my graphics, Graphics_CYAN);
	Graphics_setLineType (my graphics, Graphics_DOTTED);
	Graphics_line (my graphics, my startWindow, yWC, my endWindow, yWC);
	Graphics_setLineType (my graphics, Graphics_DRAWN);
}

/* End of file FunctionEditor.c */
