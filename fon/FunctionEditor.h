#ifndef _FunctionEditor_h_
#define _FunctionEditor_h_
/* FunctionEditor.h
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

/*
 * pb 2011/07/01
 */

#include "Editor.h"
#include "Graphics.h"
#include "Function.h"

#ifdef __cplusplus
	extern "C" {
#endif

struct FunctionEditor_picture {
	/* KEEP IN SYNC WITH PREFS. */
	bool garnish;
};

Thing_declare1cpp (FunctionEditor);

#define FunctionEditor_UPDATE_NEEDED  1
#define FunctionEditor_NO_UPDATE_NEEDED  0

void FunctionEditor_init (FunctionEditor me, GuiObject parent, const wchar *title, Data data);
/*
	Function:
		creates an Editor with a drawing area, a scroll bar and some buttons.
	Preconditions:
		parent != NULL;
		Thing_member (data, classFunction);
	Postconditions:
		my cursorMenu contains the following entries:
			Move cursor to B
			Move cursor to E
			Move cursor to...
			Move cursor by...
		my beginMenu contains:
			Move B to cursor
			Move B to E
			Move B to...
			Move B by...
		my endMenu contains:
			Move E to cursor
			Move E to B
			Move E to...
			Move E by...
		my drawingArea is attached to the form at all sides,
		my scrollBar only to the bottom, left and right sides.
		The other members are 0.0 or NULL.
		The inheritor should call
			'GuiObject_show (my dialog); GuiObject_show (my shell);'
			before calling FunctionEditor_open (me).
*/ 

void FunctionEditor_marksChanged (FunctionEditor me);
/*
	Function:
		update optional text field, the scroll bar, the drawing area and the buttons,
		from the current total time, window, cursor, and selection,
		and redraw the contents. This will be done for all the editors in the group.
	Usage:
		call this after a change in any of the markers or in the duration of the data.
*/

void FunctionEditor_shift (FunctionEditor me, double shift);
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

void FunctionEditor_prefs (void);

/* Some routines to enforce common look to all function editors. */
/* The x axis of the window is supposed to have been set to [my startWindow, my endWindow]. */
/* Preconditions: default line type, default line width. */
/* Postconditions: default line type, default line width, undefined colour, undefined text alignment. */
void FunctionEditor_drawRangeMark (FunctionEditor me, double yWC, const wchar_t *yWC_string, const wchar_t *units, int verticalAlignment);
void FunctionEditor_drawCursorFunctionValue (FunctionEditor me, double yWC, const wchar_t *yWC_string, const wchar_t *units);
void FunctionEditor_insertCursorFunctionValue (FunctionEditor me, double yWC, const wchar_t *yWC_string, const wchar_t *units, double minimum, double maximum);
void FunctionEditor_drawHorizontalHair (FunctionEditor me, double yWC, const wchar_t *yWC_string, const wchar_t *units);
void FunctionEditor_drawGridLine (FunctionEditor me, double yWC);

void FunctionEditor_insetViewport (FunctionEditor me);

void FunctionEditor_garnish (FunctionEditor me);   // Optionally selection times and selection hairs.

#ifdef __cplusplus
	}

	struct structFunctionEditor : public structEditor {
		/* Subclass may change the following attributes, */
		/* but has to respect the invariants, */
		/* and has to call FunctionEditor_marksChanged () */
		/* immediately after making the changes. */
		double tmin, tmax, startWindow, endWindow;
		double startSelection, endSelection;   // markers
			/* These attributes are all expressed in seconds. Invariants: */
			/*    tmin <= startWindow < endWindow <= tmax; */
			/*    tmin <= (startSelection, endSelection) <= tmax; */
		double arrowScrollStep;

		Graphics graphics;   // used in the 'draw' method
		short width, height;   // size of drawing area in pixels
		GuiObject text;   // optional text at top
		int shiftKeyPressed;   // information for the 'play' method.
		int playingCursor, playingSelection;   // information for end of play
		struct FunctionEditor_picture picture;

		/* Private: */
		GuiObject drawingArea, scrollBar, groupButton, bottomArea;
		bool group, enableUpdates;
		int nrect;
		struct { double left, right, bottom, top; } rect [8];
		double marker [1 + 3], playCursor, startZoomHistory, endZoomHistory;
		int numberOfMarkers;
	};
	#define FunctionEditor__methods(Klas) Editor__methods(Klas) \
		void (*draw) (Klas me); \
		void (*prepareDraw) (Klas me);   /* For less flashing. */ \
		const wchar_t *format_domain, *format_short, *format_long, *format_units, *format_totalDuration, *format_window, *format_selection; \
		int fixedPrecision_long; \
		int hasText; \
		void (*play) (Klas me, double tmin, double tmax); \
		int (*click) (Klas me, double xWC, double yWC, int shiftKeyPressed); \
		int (*clickB) (Klas me, double xWC, double yWC); \
		int (*clickE) (Klas me, double xWC, double yWC); \
		void (*key) (Klas me, unsigned char key); \
		int (*playCallback) (Any me, int phase, double tmin, double tmax, double t); \
		void (*updateText) (Klas me); \
		void (*prefs_addFields) (Klas me, EditorCommand cmd); \
		void (*prefs_setValues) (Klas me, EditorCommand cmd); \
		void (*prefs_getValues) (Klas me, EditorCommand cmd); \
		void (*createMenuItems_file_draw) (Klas me, EditorMenu menu); \
		void (*createMenuItems_file_extract) (Klas me, EditorMenu menu); \
		void (*createMenuItems_file_write) (Klas me, EditorMenu menu); \
		void (*createMenuItems_view) (Klas me, EditorMenu menu); \
		void (*createMenuItems_view_timeDomain) (Klas me, EditorMenu menu); \
		void (*createMenuItems_view_audio) (Klas me, EditorMenu menu); \
		void (*highlightSelection) (Klas me, double left, double right, double bottom, double top); \
		void (*unhighlightSelection) (Klas me, double left, double right, double bottom, double top); \
		double (*getBottomOfSoundAndAnalysisArea) (Klas me); \
		void (*form_pictureSelection) (Klas me, EditorCommand cmd); \
		void (*ok_pictureSelection) (Klas me, EditorCommand cmd); \
		void (*do_pictureSelection) (Klas me, EditorCommand cmd);
	Thing_declare2cpp (FunctionEditor, Editor);

	/*
		Attributes:
			data: must be a Function.

		Methods:

		void draw (I);
			"draw your part of the data between startWindow and endWindow."

		void play (I, double tmin, double tmax);
			"user clicked in one of the rectangles above or below the data window."

		int click (I, double xWC, double yWC, int shiftKeyPressed);
			"user clicked in data window with the left (Mac: only) mouse button."
			'xWC' is the time; 'yWC' is a value between 0.0 (bottom) and 1.0 (top).
			'shiftKeyPressed' flags if the Shift key was held down during the click.
			Return FunctionEditor_UPDATE_NEEDED if you want a window update, i.e.,
				if your 'click' moves the cursor or otherwise changes the appearance of the data.
			Return FunctionEditor_NO_UPDATE_NEEDED if you do not want a window update, e.g.,
				if your 'click' method just 'plays' something or puts a dialog on the screen.
				In the latter case, the 'ok' callback of the dialog should
				call FunctionEditor_marksChanged if necessary.
			FunctionEditor::click moves the cursor to 'xWC', drags to create a selection, 
				or extends the selection.

		int clickB (I, double xWC, double yWC);
			"user clicked in data window with the middle mouse button (Mac: control- or option-click)."
			'xWC' is the time; 'yWC' is a value between 0.0 (bottom) and 1.0 (top).
			For the return value, see the 'click' method.
			FunctionEditor::clickB simply moves the start of the selection (B) to 'xWC',
				with the sole statement 'my startSelection = xWC'.

		int clickE (I, double xWC, double yWC);
			"user clicked in data window with the right mouse button (Mac: command-click)."
			'xWC' is the time; 'yWC' is a value between 0.0 (bottom) and 1.0 (top).
			For the return value, see the 'click' method.
			FunctionEditor::clickB simply moves the end of the selection (E) to 'xWC',
				with the sole statement 'my endSelection = xWC'.

		void key (I, unsigned char key);
			"user typed a key to the data window."
			FunctionEditor::key ignores this message.
	*/

#endif // __cplusplus

/* End of file FunctionEditor.h */
#endif
