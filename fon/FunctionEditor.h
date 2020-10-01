#ifndef _FunctionEditor_h_
#define _FunctionEditor_h_
/* FunctionEditor.h
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

#include "Editor.h"
#include "Graphics.h"
#include "Function.h"

struct FunctionEditor_picture {
	/* KEEP IN SYNC WITH PREFS. */
	bool garnish;
};

Thing_define (FunctionEditor, Editor) {
	/*
		Inherited attributes:
			data: must be a Function.
	*/
	Function & function() { return * reinterpret_cast <Function *> (& our data); }

	/*
		Subclasses may change the following attributes,
		but have to respect the invariants,
		and have to call FunctionEditor_marksChanged ()
		immediately after making those changes.
		Invariants:
			tmin <= startWindow < endWindow <= tmax;
			tmin <= startSelection <= endSelection <= tmax;
	*/
	double tmin, tmax, startWindow, endWindow;
	double startSelection, endSelection;   // markers

	autoGraphics graphics;   // used in the 'draw' method
	void draw ();
	/*
		The Normalized Device cordinates are in "pixelettes", which are a bit smaller than pixels.
		The purpose of this is optimal look and feel.
		The extent to which a pixelette is smaller than a pixel depends on the size of the drawing area:
		for smaller drawing areas, texts are a bit more cramped in their rectangles
		than for larger drawing areas.
	*/
	double width_pxlt, height_pxlt;   // size of drawing area in pixelettes
private:
	double functionViewerLeft, functionViewerRight;   // location of function viewer in pixelettes
	double selectionViewerLeft, selectionViewerRight;   // location of selection viewer in pixelettes
public:
	void updateGeometry (int width_pixels, int height_pixels) {
		Graphics_setWsViewport (our graphics.get(), 0.0, width_pixels, 0.0, height_pixels);
		our width_pxlt = width_pixels + 21;   // the +21 means that the horizontal margin becomes a tiny bit larger when the window grows
		our height_pxlt = height_pixels + 111;   // the +111 means that the vertical margins become a bit larger when the window grows
		Graphics_setWsWindow (our graphics.get(), 0.0, our width_pxlt, 0.0, our height_pxlt);
		//my viewAllAsPixelettes ();
		/*
			Put the function viewer at the left and the selection viewer at the right.
		*/
		our functionViewerLeft = 0;
		our functionViewerRight = ( our p_showSelectionViewer ? our width_pxlt * (2.0/3.0) : our width_pxlt );
		our selectionViewerLeft = our functionViewerRight;
		our selectionViewerRight = our width_pxlt;
	}
	bool isInSelectionViewer (double x_pxlt) const {
		return x_pxlt > our selectionViewerLeft;
	}
	void viewAllAsPixelettes () const {
		Graphics_setViewport (our graphics.get(), 0.0, our width_pxlt, 0.0, our height_pxlt);
		Graphics_setWindow (our graphics.get(), 0.0, our width_pxlt, 0.0, our height_pxlt);
	}
	void viewFunctionViewerAsPixelettes () const {
		Graphics_setViewport (our graphics.get(), our functionViewerLeft, our functionViewerRight, 0.0, our height_pxlt);
		Graphics_setWindow (our graphics.get(), our functionViewerLeft, our functionViewerRight, 0.0, our height_pxlt);
	}
	void viewSelectionViewerAsPixelettes () const {
		Graphics_setViewport (our graphics.get(), our selectionViewerLeft, our selectionViewerRight, 0.0, our height_pxlt);
		Graphics_setWindow (our graphics.get(), our selectionViewerLeft, our selectionViewerRight, 0.0, our height_pxlt);
	}
	constexpr static double space = 30.0;
	constexpr static double MARGIN = 107.0;
	constexpr static double BOTTOM_MARGIN = 2.0;
	constexpr static double TOP_MARGIN = 3.0;
	double dataLeft_pxlt () const { return our functionViewerLeft + our MARGIN; }
	double dataRight_pxlt () const { return our functionViewerRight - our MARGIN; }
	double dataBottom_pxlt () const { return our BOTTOM_MARGIN + our space * 3; }
	double dataTop_pxlt () const { return our height_pxlt - (TOP_MARGIN + space); }
	void viewDataAsWorldByFraction () const {
		Graphics_setViewport (our graphics.get(), dataLeft_pxlt(), dataRight_pxlt(), dataBottom_pxlt(), dataTop_pxlt());
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
	}
	void viewTallDataAsWorldByFraction () const {
		Graphics_setViewport (our graphics.get(), our dataLeft_pxlt(), our dataRight_pxlt(), 0.0, our height_pxlt);
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, our height_pxlt);
	}
	constexpr static double SELECTION_VIEWER_MARGIN = 0.0;
	void viewInnerSelectionViewerAsFractionByFraction () const {
		Graphics_setViewport (our graphics.get(), our selectionViewerLeft + our MARGIN, our selectionViewerRight - our MARGIN,
				our BOTTOM_MARGIN + our space * 3, our height_pxlt - (our TOP_MARGIN + our space));
		Graphics_setViewport (our graphics.get(),
			our selectionViewerLeft + our SELECTION_VIEWER_MARGIN, our selectionViewerRight - our SELECTION_VIEWER_MARGIN,
			our SELECTION_VIEWER_MARGIN, our height_pxlt - SELECTION_VIEWER_MARGIN
		);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	}

	GuiText text;   // optional text at top
	bool clickWasModifiedByShiftKey;   // information for drag-and-drop and for start of play
	bool duringPlay;
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
	virtual void v_drawSelectionViewer () { }
	virtual void v_drawRealTimeSelectionViewer (double /* time */) { }
	virtual void v_prepareDraw () { }   // for less flashing
	virtual conststring32 v_domainName () { return U"time"; }
	virtual conststring32 v_selectionViewerName () { return U"selection viewer"; }
	virtual conststring32 v_format_domain () { return U"Time domain:"; }
	virtual const char *v_format_short () { return u8"%.3f"; }
	virtual const char *v_format_long () { return u8"%f"; }
	virtual conststring32 v_format_units_long () { return U"seconds"; }
	virtual conststring32 v_format_units_short () { return U"s"; }
	virtual const char *v_format_totalDuration () { return u8"Total duration %f seconds"; }
	virtual const char *v_format_window () { return u8"Visible part %f seconds"; }
	virtual const char *v_format_selection () { return u8"%f (%.3f / s)"; }
	virtual int v_fixedPrecision_long () { return 6; }
	virtual bool v_hasText () { return false; }
	virtual void v_play (double /* startTime */, double /* endTime */) { }
	virtual bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction);
		/*
			Message: "they clicked in the data part of the window, or in the left or right margin."
			'event' is the mouse event, with still relevant info on phase and modifier keys;
			'x_world' is the time (or another world unit);
			'y_fraction' is a value between 0.0 (bottom) and 1.0 (top);
			Behaviour of structFunctionEditor::v_mouseInWideDataView ():
				moves the cursor to 'x_world', drags to create a selection, or extends the selection.
		*/
	virtual void v_clickSelectionViewer (double x_fraction, double y_fraction);
	virtual int v_playCallback (int phase, double startTime, double endTime, double currentTime);
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
	virtual double v_getBottomOfSoundArea () { return 0.0; }
	virtual double v_getBottomOfSoundAndAnalysisArea () { return 0.0; }
	virtual void v_form_pictureSelection (EditorCommand);
	virtual void v_ok_pictureSelection (EditorCommand);
	virtual void v_do_pictureSelection (EditorCommand);

    #include "FunctionEditor_prefs.h"
};

int theFunctionEditor_playCallback (FunctionEditor me, int phase, double startTime, double endTime, double currentTime);

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
		call this after she changes a view option (font, scaling, hide/show xx)
		or after any of the data have changed. In the latter case, also call Editor_broadcastChange.
	Behaviour:
		we just call Graphics_updateWs (my graphics).
*/

void FunctionEditor_enableUpdates (FunctionEditor me, bool enable);
/*
	Function:
		temporarily disable update event to cause 'draw' messages.
	Usage:
		If you call from your 'draw' method functions that may trigger expose events,
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
		Start cut or paste methods by calling this function,
		as the grouped editors will not be synchronized
		after either of those actions. Worse, the selection
		may get outside the common interval of the editors.
*/

/*
	Some functions to enforce a common look to all function editors.
	The x axis of the window is supposed to have been set to [my startWindow, my endWindow].
	Preconditions:
		default line type, default line width.
	Postconditions:
		default line type, default line width, undefined colour, undefined text alignment.
*/
void FunctionEditor_drawRangeMark (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units, int verticalAlignment);
void FunctionEditor_drawCursorFunctionValue (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units);
void FunctionEditor_insertCursorFunctionValue (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units, double minimum, double maximum);
void FunctionEditor_drawHorizontalHair (FunctionEditor me, double yWC, conststring32 yWC_string, conststring32 units);
void FunctionEditor_drawGridLine (FunctionEditor me, double yWC);

void FunctionEditor_garnish (FunctionEditor me);   // optionally selection times and selection hairs

/* End of file FunctionEditor.h */
#endif
