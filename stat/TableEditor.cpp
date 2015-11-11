/* TableEditor.cpp
 *
 * Copyright (C) 2006-2011,2013,2015 Paul Boersma
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
	GuiScrollBar_set (my verticalScrollBar, NUMundefined, table -> rows -> size + 1, my topRow, NUMundefined, NUMundefined, NUMundefined);
}

static void updateHorizontalScrollBar (TableEditor me) {
	Table table = static_cast<Table> (my data);
	GuiScrollBar_set (my horizontalScrollBar, NUMundefined, table -> numberOfColumns + 1, my leftColumn, NUMundefined, NUMundefined, NUMundefined);
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
	GuiText_cut (my text);
}
static void menu_cb_Copy (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	GuiText_copy (my text);
}
static void menu_cb_Paste (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	GuiText_paste (my text);
}
static void menu_cb_Erase (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	GuiText_remove (my text);
}
#endif

/********** VIEW MENU **********/

/********** HELP MENU **********/

static void menu_cb_TableEditorHelp (EDITOR_ARGS) {
	EDITOR_IAM (TableEditor);
	Melder_help (U"TableEditor");
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
	columnWidth = Graphics_textWidth (graphics, U"row");
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
		const char32 *columnLabel = table -> columnHeaders [icol]. label;
		columnWidth = Graphics_textWidth (graphics, Melder_integer (icol));
		if (columnLabel == NULL) columnLabel = U"";
		cellWidth = Graphics_textWidth (graphics, columnLabel);
		if (cellWidth > columnWidth) columnWidth = cellWidth;
		for (long irow = rowmin; irow <= rowmax; irow ++) {
			const char32 *cell = Table_getStringValue_Assert (table, irow, icol);
			Melder_assert (cell != NULL);
			if (cell [0] == U'\0') cell = U"?";
			cellWidth = Graphics_textWidth (graphics, cell);
			if (cellWidth > columnWidth) columnWidth = cellWidth;
		}
		columnRight [icol - colmin] = columnLeft [icol - colmin] + columnWidth + 2 * spacing;
		if (icol < colmax) columnLeft [icol - colmin + 1] = columnRight [icol - colmin];
	}
	/*
	 * Show the row numbers.
	 */
	Graphics_text (graphics, columnLeft [0] / 2, rowmin - 1, U"row");
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		Graphics_text (graphics, columnLeft [0] / 2, irow, irow);
	}
	/*
	 * Show the column labels.
	 */
	for (long icol = colmin; icol <= colmax; icol ++) {
		double mid = (columnLeft [icol - colmin] + columnRight [icol - colmin]) / 2;
		const char32 *columnLabel = table -> columnHeaders [icol]. label;
		if (columnLabel == NULL || columnLabel [0] == U'\0') columnLabel = U"?";
		Graphics_text (graphics, mid, rowmin - 2, icol);
		Graphics_text (graphics, mid, rowmin - 1, columnLabel);
	}
	/*
	 * Show the cell contents.
	 */
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		for (long icol = colmin; icol <= colmax; icol ++) {
			double mid = (columnLeft [icol - colmin] + columnRight [icol - colmin]) / 2;
			const char32 *cell = Table_getStringValue_Assert (table, irow, icol);
			Melder_assert (cell != NULL);
			if (cell [0] == U'\0') cell = U"?";
			Graphics_text (graphics, mid, irow, cell);
		}
	}
}

bool structTableEditor :: v_click (double xclick, double yWC, bool shiftKeyPressed) {
	Table table = static_cast<Table> (data);
	return true;
}

static void gui_text_cb_change (I, GuiTextEvent event) {
	iam (TableEditor);
	(void) event;
	Table table = static_cast<Table> (my data);
	Editor_broadcastDataChanged (me);
}

static void gui_drawingarea_cb_expose (TableEditor me, GuiDrawingArea_ExposeEvent /* event */) {
	if (! my graphics) return;
	my v_draw ();
}

static void gui_drawingarea_cb_click (TableEditor me, GuiDrawingArea_ClickEvent event) {
	if (! my graphics) return;
	double xWC, yWC;
	Graphics_DCtoWC (my graphics, event -> x, event -> y, & xWC, & yWC);
	// TODO: implement selection
}

static void gui_drawingarea_cb_resize (TableEditor me, GuiDrawingArea_ResizeEvent /* event */) {
	if (! my graphics) return;
	Graphics_updateWs (my graphics);
}

static void gui_cb_scrollHorizontal (I, GuiScrollBarEvent event) {
	iam (TableEditor);
	int value = GuiScrollBar_getValue (event -> scrollBar);
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
	int value = GuiScrollBar_getValue (event -> scrollBar);
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

	our text = GuiText_createShown (our d_windowForm, 0, 0, y, y + Machine_getTextHeight (), 0);
	GuiText_setChangeCallback (our text, gui_text_cb_change, this);
	y += Machine_getTextHeight () + 4;

	our drawingArea = GuiDrawingArea_createShown (our d_windowForm, 0, - scrollWidth, y, - scrollWidth,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, gui_drawingarea_cb_resize, this, 0);

	our verticalScrollBar = GuiScrollBar_createShown (our d_windowForm, - scrollWidth, 0, y, - scrollWidth,
		1, table -> rows -> size + 1, 1, 1, 1, 10, gui_cb_scrollVertical, this, 0);

	our horizontalScrollBar = GuiScrollBar_createShown (our d_windowForm, 0, - scrollWidth, - scrollWidth, 0,
		1, table -> numberOfColumns + 1, 1, 1, 1, 3, gui_cb_scrollHorizontal, this, GuiScrollBar_HORIZONTAL);

	GuiDrawingArea_setSwipable (our drawingArea, our horizontalScrollBar, our verticalScrollBar);
}

void structTableEditor :: v_createMenus () {
	TableEditor_Parent :: v_createMenus ();

	#ifndef macintosh
	Editor_addCommand (this, U"Edit", U"-- cut copy paste --", 0, NULL);
	Editor_addCommand (this, U"Edit", U"Cut text", 'X', menu_cb_Cut);
	Editor_addCommand (this, U"Edit", U"Cut", Editor_HIDDEN, menu_cb_Cut);
	Editor_addCommand (this, U"Edit", U"Copy text", 'C', menu_cb_Copy);
	Editor_addCommand (this, U"Edit", U"Copy", Editor_HIDDEN, menu_cb_Copy);
	Editor_addCommand (this, U"Edit", U"Paste text", 'V', menu_cb_Paste);
	Editor_addCommand (this, U"Edit", U"Paste", Editor_HIDDEN, menu_cb_Paste);
	Editor_addCommand (this, U"Edit", U"Erase text", 0, menu_cb_Erase);
	Editor_addCommand (this, U"Edit", U"Erase", Editor_HIDDEN, menu_cb_Erase);
	#endif
}

void structTableEditor :: v_createHelpMenuItems (EditorMenu menu) {
	TableEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"TableEditor help", U'?', menu_cb_TableEditorHelp);
}

autoTableEditor TableEditor_create (const char32 *title, Table table) {
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
		Graphics_setUnderscoreIsSubscript (my graphics, false);
		Graphics_setAtSignIsLink (my graphics, true);
		return me;
	} catch (MelderError) {
		Melder_throw (U"TableEditor not created.");
	}
}

/* End of file TableEditor.cpp */
