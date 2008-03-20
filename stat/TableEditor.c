/* TableEditor.c
 *
 * Copyright (C) 2006-2008 Paul Boersma
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
 * pb 2006/02/11 first version
 * pb 2006/04/22 all cells visible
 * pb 2006/05/11 raised maximum number of visible columns
 * pb 2006/05/11 underscores are not subscripts
 * pb 2006/08/02 correct vertical scroll bar on Windows (has to be called "verticalScrollBar")
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/11/30 erased Graphics_printf
 * pb 2008/03/20 split off Help menu
 */

#include "TableEditor.h"
#include "machine.h"
#include "EditorM.h"

#define MAXNUM_VISIBLE_COLUMNS  100
#define SIZE_INCHES  40

#define TableEditor_members Editor_members \
	long topRow, leftColumn, selectedRow, selectedColumn; \
	Widget text, drawingArea, horizontalScrollBar, verticalScrollBar; \
	double columnLeft [MAXNUM_VISIBLE_COLUMNS], columnRight [MAXNUM_VISIBLE_COLUMNS]; \
	Graphics graphics;
#define TableEditor_methods Editor_methods \
	void (*draw) (I); \
	int (*click) (I, double xWC, double yWC, int shiftKeyPressed);
class_create_opaque (TableEditor, Editor);

/********** EDITOR METHODS **********/

static void destroy (I) {
	iam (TableEditor);
	forget (my graphics);
	inherited (TableEditor) destroy (me);
}

static void updateVerticalScrollBar (TableEditor me) {
	Table table = my data;
	#if motif
	/*int value, slider, incr, pincr;
	XmScrollBarGetValues (my verticalScrollBar, & value, & slider, & incr, & pincr);
	XmScrollBarSetValues (my verticalScrollBar, my topRow, slider, incr, pincr, False);*/
	XtVaSetValues (my verticalScrollBar,
		XmNvalue, my topRow, XmNmaximum, table -> rows -> size + 1, NULL);
	#endif
}

static void updateHorizontalScrollBar (TableEditor me) {
	Table table = my data;
	#if motif
	/*int value, slider, incr, pincr;
	XmScrollBarGetValues (my horizontalScrollBar, & value, & slider, & incr, & pincr);
	XmScrollBarSetValues (my horizontalScrollBar, my topRow, slider, incr, pincr, False);*/
	XtVaSetValues (my horizontalScrollBar,
		XmNvalue, my leftColumn, XmNmaximum, table -> numberOfColumns + 1, NULL);
	#endif
}

static void dataChanged (I) {
	iam (TableEditor);
	Table table = my data;
	if (my topRow > table -> rows -> size) my topRow = table -> rows -> size;
	if (my leftColumn > table -> numberOfColumns) my leftColumn = table -> numberOfColumns;
	updateVerticalScrollBar (me);
	updateHorizontalScrollBar (me);
	Graphics_updateWs (my graphics);
}

/********** FILE MENU **********/


/********** EDIT MENU **********/

#ifndef macintosh
static int menu_cb_Cut (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	GuiText_cut (my text);
	return 1;
}
static int menu_cb_Copy (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	GuiText_copy (my text);
	return 1;
}
static int menu_cb_Paste (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	GuiText_paste (my text);
	return 1;
}
static int menu_cb_Erase (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	GuiText_remove (my text);
	return 1;
}
#endif

/********** VIEW MENU **********/

/********** HELP MENU **********/

static int menu_cb_TableEditorHelp (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	Melder_help (L"TableEditor");
	return 1;
}

/********** DRAWING AREA **********/

static void draw (I) {
	iam (TableEditor);
	Table table = my data;
	double spacing = 2.0;   /* millimetres at both edges */
	double columnWidth, cellWidth;
	/*
	 * We fit 200 rows in 40 inches, which is 14.4 points per row.
	 */
	long rowmin = my topRow, rowmax = rowmin + 197;
	long colmin = my leftColumn, colmax = colmin + (MAXNUM_VISIBLE_COLUMNS - 1);
	if (rowmax > table -> rows -> size) rowmax = table -> rows -> size;
	if (colmax > table -> numberOfColumns) colmax = table -> numberOfColumns;
	Graphics_clearWs (my graphics);
	Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
	Graphics_setWindow (my graphics, 0.0, 1.0, rowmin + 197.5, rowmin - 2.5);
	Graphics_setColour (my graphics, Graphics_SILVER);
	Graphics_fillRectangle (my graphics, 0.0, 1.0, rowmin - 2.5, rowmin - 0.5);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_line (my graphics, 0.0, rowmin - 0.5, 1.0, rowmin - 0.5);
	Graphics_setWindow (my graphics, 0.0, Graphics_dxWCtoMM (my graphics, 1.0), rowmin + 197.5, rowmin - 2.5);
	/*
	 * Determine the width of the column with the row numbers.
	 */
	columnWidth = Graphics_textWidth (my graphics, L"row");
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		cellWidth = Graphics_textWidth (my graphics, Melder_integer (irow));
		if (cellWidth > columnWidth) columnWidth = cellWidth;
	}
	my columnLeft [0] = columnWidth + 2 * spacing;
	Graphics_setColour (my graphics, Graphics_SILVER);
	Graphics_fillRectangle (my graphics, 0.0, my columnLeft [0], rowmin - 0.5, rowmin + 197.5);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_line (my graphics, my columnLeft [0], rowmin - 0.5, my columnLeft [0], rowmin + 197.5);
	/*
	 * Determine the width of the columns.
	 */
	for (long icol = colmin; icol <= colmax; icol ++) {
		const wchar_t *columnLabel = table -> columnHeaders [icol]. label;
		columnWidth = Graphics_textWidth (my graphics, Melder_integer (icol));
		if (columnLabel == NULL) columnLabel = L"";
		cellWidth = Graphics_textWidth (my graphics, columnLabel);
		if (cellWidth > columnWidth) columnWidth = cellWidth;
		for (long irow = rowmin; irow <= rowmax; irow ++) {
			const wchar_t *cell = Table_getStringValue (table, irow, icol);
			Melder_assert (cell != NULL);
			if (cell [0] == '\0') cell = L"?";
			cellWidth = Graphics_textWidth (my graphics, cell);
			if (cellWidth > columnWidth) columnWidth = cellWidth;
		}
		my columnRight [icol - colmin] = my columnLeft [icol - colmin] + columnWidth + 2 * spacing;
		if (icol < colmax) my columnLeft [icol - colmin + 1] = my columnRight [icol - colmin];
	}
	/*
	 * Show the row numbers.
	 */
	Graphics_text (my graphics, my columnLeft [0] / 2, rowmin - 1, L"row");
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		Graphics_text1 (my graphics, my columnLeft [0] / 2, irow, Melder_integer (irow));
	}
	/*
	 * Show the column labels.
	 */
	for (long icol = colmin; icol <= colmax; icol ++) {
		double mid = (my columnLeft [icol - colmin] + my columnRight [icol - colmin]) / 2;
		const wchar_t *columnLabel = table -> columnHeaders [icol]. label;
		if (columnLabel == NULL || columnLabel [0] == '\0') columnLabel = L"?";
		Graphics_text1 (my graphics, mid, rowmin - 2, Melder_integer (icol));
		Graphics_text (my graphics, mid, rowmin - 1, columnLabel);
	}
	/*
	 * Show the cell contents.
	 */
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		for (long icol = colmin; icol <= colmax; icol ++) {
			double mid = (my columnLeft [icol - colmin] + my columnRight [icol - colmin]) / 2;
			const wchar_t *cell = Table_getStringValue (table, irow, icol);
			Melder_assert (cell != NULL);
			if (cell [0] == '\0') cell = L"?";
			Graphics_text (my graphics, mid, irow, cell);
		}
	}
}

static int click (I, double xclick, double yWC, int shiftKeyPressed) {
	iam (TableEditor);
	Table table = my data;
	return 1;
}

static void gui_text_cb_change (I, GuiTextEvent event) {
	iam (TableEditor);
	(void) event;
	Table table = my data;
	Editor_broadcastChange (me);
}

static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (TableEditor);
	(void) event;
	if (my graphics == NULL) return;
	draw (me);
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	iam (TableEditor);
	if (my graphics == NULL) return;
	double xWC, yWC;
	Graphics_DCtoWC (my graphics, event -> x, event -> y, & xWC, & yWC);
	// TODO: implement selection
}

static void createChildren (I) {
	iam (TableEditor);
	Table table = my data;
	Widget form;   /* A form inside a form; needed to keep key presses away from the drawing area. */

	#if motif
	form = XmCreateForm (my dialog, "buttons", NULL, 0);
	XtVaSetValues (form,
		XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
		XmNbottomAttachment, XmATTACH_FORM,
		XmNtraversalOn, False,   /* Needed in order to redirect all keyboard input to the text widget. */
		NULL);
	#endif

	/***** Create text field. *****/

	my text = GuiText_createShown (form, 0, 0, 0, Machine_getTextHeight (), 0);
	#ifdef UNIX
		#if motif
		XtSetKeyboardFocus (form, my text);   /* See FunctionEditor.c for the rationale behind this. */
		#endif
	#endif
	GuiText_setChangeCallback (my text, gui_text_cb_change, me);

	/***** Create drawing area. *****/

	my drawingArea = GuiDrawingArea_createShown (form, 0, - Machine_getScrollBarWidth (), Machine_getTextHeight (), - Machine_getScrollBarWidth (),
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, NULL, me, 0);

	/***** Create horizontal scroll bar. *****/

	#if motif
	my horizontalScrollBar = XtVaCreateManagedWidget ("horizontalScrollBar",
		xmScrollBarWidgetClass, form,
		XmNorientation, XmHORIZONTAL,
		XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 0,
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, Machine_getScrollBarWidth (),
		XmNbottomAttachment, XmATTACH_FORM,
		XmNheight, Machine_getScrollBarWidth (),
		XmNminimum, 1,
		XmNmaximum, table -> numberOfColumns + 1,
		XmNvalue, 1,
		XmNsliderSize, 1,
		XmNincrement, 1,
		XmNpageIncrement, 3,
		NULL);
	#endif

	/***** Create vertical scroll bar. *****/

	#if motif
	my verticalScrollBar = XtVaCreateManagedWidget ("verticalScrollBar",
		xmScrollBarWidgetClass, form,
		XmNorientation, XmVERTICAL,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getTextHeight (),
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, Machine_getScrollBarWidth (),
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 0,
		XmNwidth, Machine_getScrollBarWidth (),
		XmNminimum, 1,
		XmNmaximum, table -> rows -> size + 1,
		XmNvalue, 1,
		XmNsliderSize, 1,
		XmNincrement, 1,
		XmNpageIncrement, 10,
		NULL);
	#endif

	GuiObject_show (form);
}

static void createMenus (I) {
	iam (TableEditor);
	inherited (TableEditor) createMenus (me);

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
}

static void createHelpMenuItems (I, EditorMenu menu) {
	iam (TableEditor);
	inherited (TableEditor) createHelpMenuItems (me, menu);
	EditorMenu_addCommand (menu, L"TableEditor help", '?', menu_cb_TableEditorHelp);
}

class_methods (TableEditor, Editor) {
	class_method (destroy)
	class_method (dataChanged)
	class_method (createChildren)
	class_method (createMenus)
	class_method (createHelpMenuItems)
	class_method (draw)
	class_method (click)
class_methods_end }

static void gui_cb_horizontalScroll (GUI_ARGS) {
	GUI_IAM (TableEditor);
	int value, slider, incr, pincr;
	#if motif
	XmScrollBarGetValues (w, & value, & slider, & incr, & pincr);
	#endif
	my leftColumn = value;
	our draw (me);
}

static void gui_cb_verticalScroll (GUI_ARGS) {
	GUI_IAM (TableEditor);
	int value, slider, incr, pincr;
	#if motif
	XmScrollBarGetValues (w, & value, & slider, & incr, & pincr);
	#endif
	my topRow = value;
	our draw (me);
}

TableEditor TableEditor_create (Widget parent, const wchar_t *title, Table table) {
	TableEditor me = new (TableEditor); cherror
	Editor_init (me, parent, 0, 0, 700, 500, title, table); cherror
	#if motif
	Melder_assert (XtWindow (my drawingArea));
	#endif
	my topRow = 1;
	my leftColumn = 1;
	my selectedColumn = 1;
	my selectedRow = 1;
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	double size_pixels = SIZE_INCHES * Graphics_getResolution (my graphics);
	Graphics_setWsViewport (my graphics, 0, size_pixels, 0, size_pixels);
	Graphics_setWsWindow (my graphics, 0, size_pixels, 0, size_pixels);
	Graphics_setViewport (my graphics, 0, size_pixels, 0, size_pixels);
	Graphics_setFont (my graphics, kGraphics_font_COURIER);
	Graphics_setFontSize (my graphics, 12);
	Graphics_setUnderscoreIsSubscript (my graphics, FALSE);
	Graphics_setAtSignIsLink (my graphics, TRUE);

	#if motif
	XtAddCallback (my horizontalScrollBar, XmNvalueChangedCallback, gui_cb_horizontalScroll, (XtPointer) me);
	XtAddCallback (my horizontalScrollBar, XmNdragCallback, gui_cb_horizontalScroll, (XtPointer) me);
	XtAddCallback (my verticalScrollBar, XmNvalueChangedCallback, gui_cb_verticalScroll, (XtPointer) me);
	XtAddCallback (my verticalScrollBar, XmNdragCallback, gui_cb_verticalScroll, (XtPointer) me);
	#endif

end:
	iferror forget (me);
	return me;
}

/* End of file TableEditor.c */
