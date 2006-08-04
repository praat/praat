/* TableEditor.c
 *
 * Copyright (C) 2006 Paul Boersma
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
 */

#include "TableEditor.h"
#include "machine.h"
#include "EditorM.h"

#define MAXNUM_VISIBLE_COLUMNS  100

#define TableEditor_members Editor_members \
	long topRow, leftColumn, selectedRow, selectedColumn; \
	Widget text, drawingArea, horizontalScrollBar, verticalScrollBar; \
	double columnLeft [MAXNUM_VISIBLE_COLUMNS], columnRight [MAXNUM_VISIBLE_COLUMNS]; \
	Graphics graphics;
#define TableEditor_methods Editor_methods \
	void (*draw) (I); \
	int (*click) (I, double xWC, double yWC, int shiftKeyPressed);
class_create_opaque (TableEditor, Editor)

/********** FUNCTIONEDITOR METHODS **********/

static void destroy (I) {
	iam (TableEditor);
	forget (my graphics);
	inherited (TableEditor) destroy (me);
}

static void updateVerticalScrollBar (TableEditor me) {
	Table table = my data;
	/*int value, slider, incr, pincr;
	XmScrollBarGetValues (my verticalScrollBar, & value, & slider, & incr, & pincr);
	XmScrollBarSetValues (my verticalScrollBar, my topRow, slider, incr, pincr, False);*/
	XtVaSetValues (my verticalScrollBar,
		XmNvalue, my topRow, XmNmaximum, table -> rows -> size + 1, NULL);
}

static void updateHorizontalScrollBar (TableEditor me) {
	Table table = my data;
	/*int value, slider, incr, pincr;
	XmScrollBarGetValues (my horizontalScrollBar, & value, & slider, & incr, & pincr);
	XmScrollBarSetValues (my horizontalScrollBar, my topRow, slider, incr, pincr, False);*/
	XtVaSetValues (my horizontalScrollBar,
		XmNvalue, my leftColumn, XmNmaximum, table -> numberOfColumns + 1, NULL);
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

DIRECT (TableEditor, cb_cut)
	XmTextCut (my text, 0);
END

DIRECT (TableEditor, cb_copy)
	XmTextCopy (my text, 0);
END

DIRECT (TableEditor, cb_paste)
	XmTextPaste (my text);
END

DIRECT (TableEditor, cb_erase)
	XmTextRemove (my text);
END

/********** VIEW MENU **********/

/********** HELP MENU **********/

DIRECT (TableEditor, cb_TableEditorHelp) Melder_help ("TableEditor"); END

MOTIF_CALLBACK (cb_textChanged)
	iam (TableEditor);
	Table table = my data;
	Editor_broadcastChange (me);
MOTIF_CALLBACK_END

static void createChildren (I) {
	iam (TableEditor);
	Table table = my data;
	Widget form;   /* A form inside a form; needed to keep key presses away from the drawing area. */

	form = XmCreateForm (my dialog, "buttons", NULL, 0);
	XtVaSetValues (form,
		XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
		XmNbottomAttachment, XmATTACH_FORM,
		XmNtraversalOn, False,   /* Needed in order to redirect all keyboard input to the text widget. */
		NULL);

	/***** Create text field. *****/

	my text = XmCreateText (form, "text", NULL, 0);
	XtVaSetValues (my text,
		XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNheight, Machine_getTextHeight (),
		XmNwordWrap, False,
		XmNeditMode, XmSINGLE_LINE_EDIT,
		NULL);
	XtManageChild (my text);
	#ifdef UNIX
		XtSetKeyboardFocus (form, my text);   /* See FunctionEditor.c for the rationale behind this. */
	#endif
	XtAddCallback (my text, XmNvalueChangedCallback, cb_textChanged, (XtPointer) me);

	/***** Create drawing area. *****/

	my drawingArea = XmCreateDrawingArea (form, "drawingArea", NULL, 0);
	XtVaSetValues (my drawingArea,
		XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getTextHeight (),
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, Machine_getScrollBarWidth (),
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, Machine_getScrollBarWidth (),
		NULL);
	XtManageChild (my drawingArea);

	/***** Create horizontal scroll bar. *****/

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

	/***** Create vertical scroll bar. *****/

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

	XtManageChild (form);
}

static void createMenus (I) {
	iam (TableEditor);
	inherited (TableEditor) createMenus (me);

	Editor_addCommand (me, "Edit", "-- cut copy paste --", 0, NULL);
	Editor_addCommand (me, "Edit", "Cut text", 'X', cb_cut);
	Editor_addCommand (me, "Edit", "Cut", Editor_HIDDEN, cb_cut);
	Editor_addCommand (me, "Edit", "Copy text", 'C', cb_copy);
	Editor_addCommand (me, "Edit", "Copy", Editor_HIDDEN, cb_copy);
	Editor_addCommand (me, "Edit", "Paste text", 'V', cb_paste);
	Editor_addCommand (me, "Edit", "Paste", Editor_HIDDEN, cb_paste);
	Editor_addCommand (me, "Edit", "Erase text", 0, cb_erase);
	Editor_addCommand (me, "Edit", "Erase", Editor_HIDDEN, cb_erase);
	Editor_addCommand (me, "Help", "TableEditor help", '?', cb_TableEditorHelp);
}

/********** DRAWING AREA **********/

static void draw (I) {
	iam (TableEditor);
	Table table = my data;
	double spacing = 2.0;   /* millimetres at both edges */
	char numberBuffer [40];
	double columnWidth, cellWidth;
	long irow, icol;
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
	columnWidth = Graphics_textWidth (my graphics, "row");
	for (irow = rowmin; irow <= rowmax; irow ++) {
		sprintf (numberBuffer, "%ld", irow);
		cellWidth = Graphics_textWidth (my graphics, numberBuffer);
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
	for (icol = colmin; icol <= colmax; icol ++) {
		const char *columnLabel = table -> columnHeaders [icol]. label;
		sprintf (numberBuffer, "%ld", icol);
		columnWidth = Graphics_textWidth (my graphics, numberBuffer);
		if (columnLabel == NULL) columnLabel = "";
		cellWidth = Graphics_textWidth (my graphics, columnLabel);
		if (cellWidth > columnWidth) columnWidth = cellWidth;
		for (irow = rowmin; irow <= rowmax; irow ++) {
			const char *cell = Table_getStringValue (table, irow, icol);
			Melder_assert (cell != NULL);
			if (cell [0] == '\0') cell = "?";
			cellWidth = Graphics_textWidth (my graphics, cell);
			if (cellWidth > columnWidth) columnWidth = cellWidth;
		}
		my columnRight [icol - colmin] = my columnLeft [icol - colmin] + columnWidth + 2 * spacing;
		if (icol < colmax) my columnLeft [icol - colmin + 1] = my columnRight [icol - colmin];
	}
	/*
	 * Show the row numbers.
	 */
	Graphics_text (my graphics, my columnLeft [0] / 2, rowmin - 1, "row");
	for (irow = rowmin; irow <= rowmax; irow ++) {
		Graphics_printf (my graphics, my columnLeft [0] / 2, irow, "%ld", irow);
	}
	for (icol = colmin; icol <= colmax; icol ++) {
		double mid = (my columnLeft [icol - colmin] + my columnRight [icol - colmin]) / 2;
		const char *columnLabel = table -> columnHeaders [icol]. label;
		if (columnLabel == NULL || columnLabel [0] == '\0') columnLabel = "?";
		Graphics_printf (my graphics, mid, rowmin - 2, "%ld", icol);
		Graphics_printf (my graphics, mid, rowmin - 1, "%s", columnLabel);
	}
	for (irow = rowmin; irow <= rowmax; irow ++) {
		for (icol = colmin; icol <= colmax; icol ++) {
			double mid = (my columnLeft [icol - colmin] + my columnRight [icol - colmin]) / 2;
			const char *cell = Table_getStringValue (table, irow, icol);
			Melder_assert (cell != NULL);
			if (cell [0] == '\0') cell = "?";
			Graphics_printf (my graphics, mid, irow, "%s", cell);
		}
	}
}

static int click (I, double xclick, double yWC, int shiftKeyPressed) {
	iam (TableEditor);
	Table table = my data;
	return 1;
}

class_methods (TableEditor, Editor)
	class_method (destroy)
	class_method (dataChanged)
	class_method (createChildren)
	class_method (createMenus)
	class_method (draw)
	class_method (click)
class_methods_end

MOTIF_CALLBACK (cb_horizontalScroll)
	iam (TableEditor);
	int value, slider, incr, pincr;
	XmScrollBarGetValues (w, & value, & slider, & incr, & pincr);
	my leftColumn = value;
	our draw (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_verticalScroll)
	iam (TableEditor);
	int value, slider, incr, pincr;
	XmScrollBarGetValues (w, & value, & slider, & incr, & pincr);
	my topRow = value;
	our draw (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_draw)
	iam (TableEditor);
#ifdef UNIX
	if (((XmDrawingAreaCallbackStruct *) call) -> event -> xexpose. count) return;
#endif
	draw (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_input)
	iam (TableEditor);
	MotifEvent event = MotifEvent_fromCallData (call);
	int shiftKeyPressed = MotifEvent_shiftKeyPressed (event);
	double xWC, yWC;
	if (MotifEvent_isButtonPressedEvent (event)) {
		Graphics_DCtoWC (my graphics, MotifEvent_x (event), MotifEvent_y (event), & xWC, & yWC);
	}
MOTIF_CALLBACK_END

TableEditor TableEditor_create (Widget parent, const char *title, Table table) {
	TableEditor me = new (TableEditor); cherror
	Editor_init (me, parent, 0, 0, 700, 500, title, table); cherror
	Melder_assert (XtWindow (my drawingArea));
	my topRow = 1;
	my leftColumn = 1;
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setWsViewport (my graphics, 0, 3000, 0, 3000);
	Graphics_setWsWindow (my graphics, 0, 3000, 0, 3000);
	Graphics_setViewport (my graphics, 0, 3000, 0, 3000);
	Graphics_setFont (my graphics, Graphics_COURIER);
	Graphics_setFontSize (my graphics, 12);
	Graphics_setUnderscoreIsSubscript (my graphics, FALSE);
	Graphics_setAtSignIsLink (my graphics, TRUE);

	XtAddCallback (my drawingArea, XmNexposeCallback, cb_draw, (XtPointer) me);
	XtAddCallback (my drawingArea, XmNinputCallback, cb_input, (XtPointer) me);
	XtAddCallback (my horizontalScrollBar, XmNvalueChangedCallback, cb_horizontalScroll, (XtPointer) me);
	XtAddCallback (my horizontalScrollBar, XmNdragCallback, cb_horizontalScroll, (XtPointer) me);
	XtAddCallback (my verticalScrollBar, XmNvalueChangedCallback, cb_verticalScroll, (XtPointer) me);
	XtAddCallback (my verticalScrollBar, XmNdragCallback, cb_verticalScroll, (XtPointer) me);

end:
	iferror forget (me);
	return me;
}

/* End of file TableEditor.c */
