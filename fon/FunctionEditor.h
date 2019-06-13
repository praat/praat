#ifndef _FunctionEditor_h_
#define _FunctionEditor_h_
/* FunctionEditor.h
 *
 * Copyright (C) 1992-2011,2012,2013,2014,2015,2017 Paul Boersma
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

#include "Editor.h"
#include "Graphics.h"
#include "Function.h"

struct FunctionEditor_picture {
	/* KEEP IN SYNC WITH PREFS. */
	bool garnish;
};

Thing_define (FunctionEditor, Editor) {
	/* Subclass may change the following attributes, */
	/* but has to respect the invariants, */
	/* and has to call FunctionEditor_marksChanged () */
	/* immediately after making the changes. */
	double tmin, tmax, startWindow, endWindow;
	double startSelection, endSelection;   // markers
		/* These attributes are all expressed in seconds. Invariants: */
		/*    tmin <= startWindow < endWindow <= tmax; */
		/*    tmin <= (startSelection, endSelection) <= tmax; */

	autoGraphics graphics;   // used in the 'draw' method
	int functionViewerLeft, functionViewerRight;   // size of drawing areas in pixels
	int selectionViewerLeft, selectionViewerRight;   // size of drawing areas in pixels
	int height;   // size of drawing areas in pixels
	GuiText text;   // optional text at top
	int shiftKeyPressed;   // information for the 'play' method
	bool playingCursor, playingSelection;   // information for end of play
	struct FunctionEditor_picture picture;

	/* Private: */
	GuiDrawingArea drawingArea;
	GuiScrollBar scrollBar;
	GuiCheckButton groupButton;
	GuiObject bottomArea;
	bool group, enableUpdates;
	int nrect;
	struct { double left, right, bottom, top; } rect [8];
	double marker [1 + 3], playCursor, startZoomHistory, endZoomHistory;
	int numberOfMarkers;

	void v_destroy () noexcept
		override;
	void v_info ()
		override;
	void v_createMenus ()
		override;
	void v_createMenuItems_file (EditorMenu)
		override;
	void v_createMenuItems_query (EditorMenu)
		override;
	void v_createChildren ()
		override;
	void v_createHelpMenuItems (EditorMenu)
		override;
	void v_dataChanged ()
		override;

	virtual void v_draw () { }
		/*
		 * Message: "draw your part of the data between startWindow and endWindow."
		 */
	virtual void v_drawSelectionViewer () { }
	virtual void v_drawRealTimeSelectionViewer (int /* phase */, double /* time */) { }
	virtual void v_prepareDraw () { }   // for less flashing
	virtual conststring32 v_format_domain () { return U"Time domain:"; }
	virtual const char *v_format_short () { return u8"%.3f"; }
	virtual const char *v_format_long () { return u8"%f"; }
	virtual conststring32 v_format_units () { return U"seconds"; }
	virtual const char *v_format_totalDuration () { return u8"Total duration %f seconds"; }
	virtual const char *v_format_window () { return u8"Visible part %f seconds"; }
	virtual const char *v_format_selection () { return u8"%f (%.3f / s)"; }
	virtual int v_fixedPrecision_long () { return 6; }
	virtual bool v_hasText () { return false; }
	virtual void v_play (double /* timeFrom */, double /* timeTo */) { }
		/*
		 * Message: "the user clicked in one of the rectangles above or below the data window."
		 */
	virtual bool v_click (double xWC, double yWC, bool shiftKeyPressed);
		/*
		 * Message: "the user clicked in data window with the left mouse button."
		 * 'xWC' is the time;
		 * 'yWC' is a value between 0.0 (bottom) and 1.0 (top);
		 * 'shiftKeyPressed' flags if the Shift key was held down during the click.
		 * Constraints:
		 *    Return FunctionEditor_UPDATE_NEEDED if you want a window update, i.e.,
		 *    if your 'click' moves the cursor or otherwise changes the appearance of the data.
		 *    Return FunctionEditor_NO_UPDATE_NEEDED if you do not want a window update, e.g.,
		 *    if your 'click' method just 'plays' something or puts a dialog on the screen.
		 *    In the latter case, the 'ok' callback of the dialog should
		 *    call FunctionEditor_marksChanged if necessary.
		 * Behaviour of FunctionEditor::click ():
		 *    moves the cursor to 'xWC', drags to create a selection, or extends the selection.
		 */
	virtual void v_clickSelectionViewer (double xWC, double yWC);
	virtual bool v_clickB (double xWC, double yWC);
	virtual bool v_clickE (double xWC, double yWC);
	virtual int v_playCallback (int phase, double tmin, double tmax, double t);
	virtual void v_updateText () { }
	virtual void v_prefs_addFields (EditorCommand) { }
	virtual void v_prefs_setValues (EditorCommand) { }
	virtual void v_prefs_getValues (EditorCommand) { }
	virtual void v_createMenuItems_file_draw (EditorMenu) { }
	virtual void v_createMenuItems_file_extract (EditorMenu) { }
	virtual void v_createMenuItems_file_write (EditorMenu) { }
	virtual void v_createMenuItems_view (EditorMenu);
	virtual void v_createMenuItems_view_timeDomain (EditorMenu);
	virtual void v_createMenuItems_view_audio (EditorMenu);
	virtual void v_highlightSelection (double left, double right, double bottom, double top);
	virtual void v_unhighlightSelection (double left, double right, double bottom, double top);
	virtual double v_getBottomOfSoundArea () { return 0.0; }
	virtual double v_getBottomOfSoundAndAnalysisArea () { return 0.0; }
	virtual void v_form_pictureSelection (EditorCommand);
	virtual void v_ok_pictureSelection (EditorCommand);
	virtual void v_do_pictureSelection (EditorCommand);

    #include "FunctionEditor_prefs.h"
};

int theFunctionEditor_playCallback (FunctionEditor me, int phase, double tmin, double tmax, double t);

/*
	Attributes:
		data: must be a Function.

	int clickB (double xWC, double yWC);
		"user clicked in data window with the middle mouse button (Mac: control- or option-click)."
		'xWC' is the time; 'yWC' is a value between 0.0 (bottom) and 1.0 (top).
		For the return value, see the 'click' method.
		FunctionEditor::clickB simply moves the start of the selection (B) to 'xWC',
			with the sole statement 'my startSelection = xWC'.

	int clickE (double xWC, double yWC);
		"user clicked in data window with the right mouse button (Mac: command-click)."
		'xWC' is the time; 'yWC' is a value between 0.0 (bottom) and 1.0 (top).
		For the return value, see the 'click' method.
		FunctionEditor::clickB simply moves the end of the selection (E) to 'xWC',
			with the sole statement 'my endSelection = xWC'.

	void key (unsigned char key);
		"user typed a key to the data window."
		FunctionEditor::key ignores this message.
*/


#define FunctionEditor_UPDATE_NEEDED  true
#define FunctionEditor_NO_UPDATE_NEEDED  false

void FunctionEditor_init (FunctionEditor me, conststring32 title, Function data);
/*
	Function:
		creates an Editor with a drawing area, a scroll bar and some buttons.
	Postconditions:
		my drawingArea is attached to the form at all sides,
		my scrollBar only to the bottom, left and right sides.
*/ 

void FunctionEditor_marksChanged (FunctionEditor me, bool needsUpdateGroup);
/*
	Function:
		update optional text field, the scroll bar, the drawing area and the buttons,
		from the current total time, window, cursor, and selection,
		and redraw the contents.
		If needsUpdateGroup is true, this will be done for all the editors in the group.
	Usage:
		call this after a change in any of the markers or in the duration of the data.
*/

void FunctionEditor_shift (FunctionEditor me, double shift, bool needsUpdateGroup);
/*
	Function:
		shift (scroll) the window through time, keeping the window length constant.
	Usage:
		call this after a search.
*/

void FunctionEditor_updateText (FunctionEditor me);
/*
	Function:
		update the optional text widget.
	Usage:
		call this after moving the cursor, if that would have to change the text.
		The generic FunctionEditor also calls this if one of the other marks have changed.
	Behaviour:
		we just call the updateText method, which the inheritor will have to modify,
		since FunctionEditor::updateText does nothing.
*/

void FunctionEditor_redraw (FunctionEditor me);
/*
	Function:
		update the drawing area of a single editor.
	Usage:
		calls this after she changes a view option (font, scaling, hide/show xx)
		or after any of the data have changed. In the latter case, also call Editor_broadcastChange.
	Behaviour:
		we just call Graphics_updateWs (my graphics).
*/

void FunctionEditor_enableUpdates (FunctionEditor me, bool enable);
/*
	Function:
		temporarily disable update event to cause 'draw' messages.
	Usage:
		If you call from your 'draw' method routines that may trigger expose events,
		you should bracket those routines between
			FunctionEditor_enableUpdates (me, false);
		and
			FunctionEditor_enableUpdates (me, true);
		This may happen if you call an analysis routine which calls Melder_progress.
*/

void FunctionEditor_ungroup (FunctionEditor me);
/*
	Function:
		force me out of the group.
	Usage:
		Start cut or paste methods by calling this routine,
		as the grouped editors will not be synchronized
		after either of those actions. Worse, the selection
		may get outside the common interval of the editors.
*/

/* Some routines to enforce common look to all function editors. */
/* The x axis of the window is supposed to have been set to [my startWindow, my endWindow]. */
/* Preconditions: default line type, default line width. */
/* Postconditions: default line type, default line width, undefined colour, undefined text alignment. */
void FunctionEditor_drawRangeMark (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units, int verticalAlignment);
void FunctionEditor_drawCursorFunctionValue (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units);
void FunctionEditor_insertCursorFunctionValue (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units, double minimum, double maximum);
void FunctionEditor_drawHorizontalHair (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units);
void FunctionEditor_drawGridLine (FunctionEditor me, double yWC);

void FunctionEditor_garnish (FunctionEditor me);   // Optionally selection times and selection hairs.

/* End of file FunctionEditor.h */
#endif
