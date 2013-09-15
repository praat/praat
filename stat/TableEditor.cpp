/* TableEditor.cpp
 *
 * Copyright (C) 2006-2011,2013 Paul Boersma
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

#include "TableEditor.h"
#include "machine.h"
#include "EditorM.h"

Thing_implement (TableEditor, Editor, 0);

#define SIZE_INCHES  40

/********** EDITOR METHODS **********/

void structTableEditor :: v_destroy () {
	forget (graphics);
	TableEditor_Parent :: v_destroy ();
}

static void updateVerticalScrollBar (TableEditor me) {
	Table table = static_cast<Table> (my data);
	my verticalScrollBar -> f_set (NUMundefined, table -> rows -> size + 1, my topRow, NUMundefined, NUMundefined, NUMundefined);
}

static void updateHorizontalScrollBar (TableEditor me) {
	Table table = static_cast<Table> (my data);
	my horizontalScrollBar -> f_set (NUMundefined, table -> numberOfColumns + 1, my leftColumn, NUMundefined, NUMundefined, NUMundefined);
}

void structTableEditor :: v_dataChanged () {
	Table table = static_cast<Table> (data);
	if (topRow > table -> rows -> size) topRow = table -> rows -> size;
	if (leftColumn > table -> numberOfColumns) leftColumn = table -> numberOfColumns;
	updateVerticalScrollBar (this);
	updateHorizontalScrollBar (this);
	Graphics_updateWs (graphics);
}

/********** FILE MENU **********/


/********** EDIT MENU **********/

#ifndef macintosh
static void menu_cb_Cut (EDITOR_ARGS) {   // BUG: why only on Mac?
	EDITOR_IAM (TableEditor);
	my text -> f_cut ();
}
static void menu_cb_Copy (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	my text -> f_copy ();
}
static void menu_cb_Paste (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	my text -> f_paste ();
}
static void menu_cb_Erase (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	my text -> f_remove ();
}
#endif

/********** VIEW MENU **********/

/********** HELP MENU **********/

static void menu_cb_TableEditorHelp (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	Melder_help (L"TableEditor");
}

/********** DRAWING AREA **********/

void structTableEditor :: v_draw () {
	Table table = static_cast<Table> (data);
	double spacing = 2.0;   // millimetres at both edges
	double columnWidth, cellWidth;
	/*
	 * We fit 200 rows in 40 inches, which is 14.4 points per row.
	 */
	long rowmin = topRow, rowmax = rowmin + 197;
	long colmin = leftColumn, colmax = colmin + (kTableEditor_MAXNUM_VISIBLE_COLUMNS - 1);
	if (rowmax > table -> rows -> size) rowmax = table -> rows -> size;
	if (colmax > table -> numberOfColumns) colmax = table -> numberOfColumns;
	Graphics_clearWs (graphics);
	Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);
	Graphics_setWindow (graphics, 0.0, 1.0, rowmin + 197.5, rowmin - 2.5);
	Graphics_setColour (graphics, Graphics_SILVER);
	Graphics_fillRectangle (graphics, 0.0, 1.0, rowmin - 2.5, rowmin - 0.5);
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_line (graphics, 0.0, rowmin - 0.5, 1.0, rowmin - 0.5);
	Graphics_setWindow (graphics, 0.0, Graphics_dxWCtoMM (graphics, 1.0), rowmin + 197.5, rowmin - 2.5);
	/*
	 * Determine the width of the column with the row numbers.
	 */
	columnWidth = Graphics_textWidth (graphics, L"row");
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		cellWidth = Graphics_textWidth (graphics, Melder_integer (irow));
		if (cellWidth > columnWidth) columnWidth = cellWidth;
	}
	columnLeft [0] = columnWidth + 2 * spacing;
	Graphics_setColour (graphics, Graphics_SILVER);
	Graphics_fillRectangle (graphics, 0.0, columnLeft [0], rowmin - 0.5, rowmin + 197.5);
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_line (graphics, columnLeft [0], rowmin - 0.5, columnLeft [0], rowmin + 197.5);
	/*
	 * Determine the width of the columns.
	 */
	for (long icol = colmin; icol <= colmax; icol ++) {
		const wchar_t *columnLabel = table -> columnHeaders [icol]. label;
		columnWidth = Graphics_textWidth (graphics, Melder_integer (icol));
		if (columnLabel == NULL) columnLabel = L"";
		cellWidth = Graphics_textWidth (graphics, columnLabel);
		if (cellWidth > columnWidth) columnWidth = cellWidth;
		for (long irow = rowmin; irow <= rowmax; irow ++) {
			const wchar_t *cell = Table_getStringValue_Assert (table, irow, icol);
			Melder_assert (cell != NULL);
			if (cell [0] == '\0') cell = L"?";
			cellWidth = Graphics_textWidth (graphics, cell);
			if (cellWidth > columnWidth) columnWidth = cellWidth;
		}
		columnRight [icol - colmin] = columnLeft [icol - colmin] + columnWidth + 2 * spacing;
		if (icol < colmax) columnLeft [icol - colmin + 1] = columnRight [icol - colmin];
	}
	/*
	 * Show the row numbers.
	 */
	Graphics_text (graphics, columnLeft [0] / 2, rowmin - 1, L"row");
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		Graphics_text1 (graphics, columnLeft [0] / 2, irow, Melder_integer (irow));
	}
	/*
	 * Show the column labels.
	 */
	for (long icol = colmin; icol <= colmax; icol ++) {
		double mid = (columnLeft [icol - colmin] + columnRight [icol - colmin]) / 2;
		const wchar_t *columnLabel = table -> columnHeaders [icol]. label;
		if (columnLabel == NULL || columnLabel [0] == '\0') columnLabel = L"?";
		Graphics_text1 (graphics, mid, rowmin - 2, Melder_integer (icol));
		Graphics_text (graphics, mid, rowmin - 1, columnLabel);
	}
	/*
	 * Show the cell contents.
	 */
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		for (long icol = colmin; icol <= colmax; icol ++) {
			double mid = (columnLeft [icol - colmin] + columnRight [icol - colmin]) / 2;
			const wchar_t *cell = Table_getStringValue_Assert (table, irow, icol);
			Melder_assert (cell != NULL);
			if (cell [0] == '\0') cell = L"?";
			Graphics_text (graphics, mid, irow, cell);
		}
	}
}

int structTableEditor :: v_click (double xclick, double yWC, bool shiftKeyPressed) {
	Table table = static_cast<Table> (data);
	return 1;
}

static void gui_text_cb_change (I, GuiTextEvent event) {
	iam (TableEditor);
	(void) event;
	Table table = static_cast<Table> (my data);
	my broadcastDataChanged ();
}

static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (TableEditor);
	(void) event;
	if (my graphics == NULL) return;
	my v_draw ();
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	iam (TableEditor);
	if (my graphics == NULL) return;
	double xWC, yWC;
	Graphics_DCtoWC (my graphics, event -> x, event -> y, & xWC, & yWC);
	// TODO: implement selection
}

static void gui_drawingarea_cb_resize (I, GuiDrawingAreaResizeEvent event) {
	iam (TableEditor);
	if (my graphics == NULL) return;
	Graphics_updateWs (my graphics);
}

static void gui_cb_scrollHorizontal (I, GuiScrollBarEvent event) {
	iam (TableEditor);
	int value = event -> scrollBar -> f_getValue ();
	if (value != my leftColumn) {
		my leftColumn = value;
		#if cocoa || gtk || win
			Graphics_updateWs (my graphics);   // wait for expose event
		#else
			Graphics_clearWs (my graphics);
			my v_draw ();   // do not wait for expose event
		#endif
	}
}

static void gui_cb_scrollVertical (I, GuiScrollBarEvent event) {
	iam (TableEditor);
	int value = event -> scrollBar -> f_getValue ();
	if (value != my topRow) {
		my topRow = value;
		#if cocoa || gtk || win
			Graphics_updateWs (my graphics);   // wait for expose event
		#else
			Graphics_clearWs (my graphics);
			my v_draw ();   // do not wait for expose event
		#endif
	}
}

void structTableEditor :: v_createChildren () {
	Table table = static_cast<Table> (data);
	int y = Machine_getMenuBarHeight () + 4, scrollWidth = Machine_getScrollBarWidth ();

	text = GuiText_createShown (d_windowForm, 0, 0, y, y + Machine_getTextHeight (), 0);
	text -> f_setChangeCallback (gui_text_cb_change, this);
	y += Machine_getTextHeight () + 4;

	drawingArea = GuiDrawingArea_createShown (d_windowForm, 0, - scrollWidth, y, - scrollWidth,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, gui_drawingarea_cb_resize, this, 0);

	verticalScrollBar = GuiScrollBar_createShown (d_windowForm, - scrollWidth, 0, y, - scrollWidth,
		1, table -> rows -> size + 1, 1, 1, 1, 10, gui_cb_scrollVertical, this, 0);

	horizontalScrollBar = GuiScrollBar_createShown (d_windowForm, 0, - scrollWidth, - scrollWidth, 0,
		1, table -> numberOfColumns + 1, 1, 1, 1, 3, gui_cb_scrollHorizontal, this, GuiScrollBar_HORIZONTAL);

	drawingArea -> f_setSwipable (horizontalScrollBar, verticalScrollBar);
}

void structTableEditor :: v_createMenus () {
	TableEditor_Parent :: v_createMenus ();

	#ifndef macintosh
	Editor_addCommand (this, L"Edit", L"-- cut copy paste --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Cut text", 'X', menu_cb_Cut);
	Editor_addCommand (this, L"Edit", L"Cut", Editor_HIDDEN, menu_cb_Cut);
	Editor_addCommand (this, L"Edit", L"Copy text", 'C', menu_cb_Copy);
	Editor_addCommand (this, L"Edit", L"Copy", Editor_HIDDEN, menu_cb_Copy);
	Editor_addCommand (this, L"Edit", L"Paste text", 'V', menu_cb_Paste);
	Editor_addCommand (this, L"Edit", L"Paste", Editor_HIDDEN, menu_cb_Paste);
	Editor_addCommand (this, L"Edit", L"Erase text", 0, menu_cb_Erase);
	Editor_addCommand (this, L"Edit", L"Erase", Editor_HIDDEN, menu_cb_Erase);
	#endif
}

void structTableEditor :: v_createHelpMenuItems (EditorMenu menu) {
	TableEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"TableEditor help", '?', menu_cb_TableEditorHelp);
}

TableEditor TableEditor_create (const wchar_t *title, Table table) {
	try {
		autoTableEditor me = Thing_new (TableEditor);
		Editor_init (me.peek(), 0, 0, 700, 500, title, table);
		#if motif
		Melder_assert (XtWindow (my drawingArea -> d_widget));
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
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("TableEditor not created.");
	}
}

/* End of file TableEditor.cpp */
