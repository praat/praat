/* TableEditor.cpp
 *
 * Copyright (C) 2006-2011,2013,2015,2016,2017 Paul Boersma
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

#include "TableEditor.h"
#include "machine.h"
#include "EditorM.h"

Thing_implement (TableEditor, Editor, 0);

#include "prefs_define.h"
#include "TableEditor_prefs.h"
#include "prefs_install.h"
#include "TableEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "TableEditor_prefs.h"

#define SIZE_INCHES  40

/********** EDITOR METHODS **********/

void structTableEditor :: v_destroy () noexcept {
	TableEditor_Parent :: v_destroy ();
}

void structTableEditor :: v_info () {
	our TableEditor_Parent :: v_info ();
	MelderInfo_writeLine (U"Table uses text styles: ", our p_useTextStyles);
	//MelderInfo_writeLine (U"Table font size: ", our p_fontSize);
}

static void updateVerticalScrollBar (TableEditor me) {
	Table table = static_cast<Table> (my data);
	GuiScrollBar_set (my verticalScrollBar, undefined, table -> rows.size + 1, my topRow, undefined, undefined, undefined);
}

static void updateHorizontalScrollBar (TableEditor me) {
	Table table = static_cast<Table> (my data);
	GuiScrollBar_set (my horizontalScrollBar, undefined, table -> numberOfColumns + 1, my leftColumn, undefined, undefined, undefined);
}

void structTableEditor :: v_dataChanged () {
	Table table = static_cast<Table> (our data);
	if (topRow > table -> rows.size) topRow = table -> rows.size;
	if (leftColumn > table -> numberOfColumns) leftColumn = table -> numberOfColumns;
	updateVerticalScrollBar (this);
	updateHorizontalScrollBar (this);
	Graphics_updateWs (our graphics.get());
}

/********** FILE MENU **********/

static void menu_cb_preferences (TableEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"TableEditor preferences", nullptr);
		OPTIONMENU (U"The symbols %#_^ in labels", my default_useTextStyles () + 1)
			OPTION (U"are shown as typed")
			OPTION (U"mean italic/bold/sub/super")
	EDITOR_OK
		SET_INTEGER (U"The symbols %#_^ in labels", my p_useTextStyles + 1)
	EDITOR_DO
		my pref_useTextStyles () = my p_useTextStyles = GET_INTEGER (U"The symbols %#_^ in labels") - 1;
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

/********** EDIT MENU **********/

#ifndef macintosh
static void menu_cb_Cut (TableEditor me, EDITOR_ARGS_DIRECT) {   // BUG: why only on Mac?
	GuiText_cut (my text);
}
static void menu_cb_Copy (TableEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_copy (my text);
}
static void menu_cb_Paste (TableEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_paste (my text);
}
static void menu_cb_Erase (TableEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_remove (my text);
}
#endif

/********** VIEW MENU **********/

/********** HELP MENU **********/

static void menu_cb_TableEditorHelp (TableEditor, EDITOR_ARGS_DIRECT) {
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
	if (rowmax > table -> rows.size) rowmax = table -> rows.size;
	if (colmax > table -> numberOfColumns) colmax = table -> numberOfColumns;
	Graphics_clearWs (graphics.get());
	Graphics_setTextAlignment (graphics.get(), Graphics_CENTRE, Graphics_HALF);
	Graphics_setWindow (graphics.get(), 0.0, 1.0, rowmin + 197.5, rowmin - 2.5);
	Graphics_setColour (graphics.get(), Graphics_SILVER);
	Graphics_fillRectangle (graphics.get(), 0.0, 1.0, rowmin - 2.5, rowmin - 0.5);
	Graphics_setColour (graphics.get(), Graphics_BLACK);
	Graphics_line (graphics.get(), 0.0, rowmin - 0.5, 1.0, rowmin - 0.5);
	Graphics_setWindow (graphics.get(), 0.0, Graphics_dxWCtoMM (graphics.get(), 1.0), rowmin + 197.5, rowmin - 2.5);
	/*
	 * Determine the width of the column with the row numbers.
	 */
	columnWidth = Graphics_textWidth (graphics.get(), U"row");
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		cellWidth = Graphics_textWidth (graphics.get(), Melder_integer (irow));
		if (cellWidth > columnWidth) columnWidth = cellWidth;
	}
	columnLeft [0] = columnWidth + 2 * spacing;
	Graphics_setColour (graphics.get(), Graphics_SILVER);
	Graphics_fillRectangle (graphics.get(), 0.0, columnLeft [0], rowmin - 0.5, rowmin + 197.5);
	Graphics_setColour (graphics.get(), Graphics_BLACK);
	Graphics_line (graphics.get(), columnLeft [0], rowmin - 0.5, columnLeft [0], rowmin + 197.5);
	/*
	 * Determine the width of the columns.
	 */
	for (long icol = colmin; icol <= colmax; icol ++) {
		const char32 *columnLabel = table -> columnHeaders [icol]. label;
		columnWidth = Graphics_textWidth (graphics.get(), Melder_integer (icol));
		if (! columnLabel) columnLabel = U"";
		cellWidth = Graphics_textWidth (graphics.get(), columnLabel);
		if (cellWidth > columnWidth) columnWidth = cellWidth;
		for (long irow = rowmin; irow <= rowmax; irow ++) {
			const char32 *cell = Table_getStringValue_Assert (table, irow, icol);
			Melder_assert (cell);
			if (cell [0] == U'\0') cell = U"?";
			cellWidth = Graphics_textWidth (graphics.get(), cell);
			if (cellWidth > columnWidth) columnWidth = cellWidth;
		}
		columnRight [icol - colmin] = columnLeft [icol - colmin] + columnWidth + 2 * spacing;
		if (icol < colmax) columnLeft [icol - colmin + 1] = columnRight [icol - colmin];
	}
	/*
		Text can be "graphic" or not.
	*/
	Graphics_setPercentSignIsItalic (our graphics.get(), our p_useTextStyles);
	Graphics_setNumberSignIsBold (our graphics.get(), our p_useTextStyles);
	Graphics_setCircumflexIsSuperscript (our graphics.get(), our p_useTextStyles);
	Graphics_setUnderscoreIsSubscript (our graphics.get(), our p_useTextStyles);
	/*
	 * Show the row numbers.
	 */
	Graphics_text (graphics.get(), columnLeft [0] / 2, rowmin - 1, U"row");
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		Graphics_text (graphics.get(), columnLeft [0] / 2, irow, irow);
	}
	/*
	 * Show the column labels.
	 */
	for (long icol = colmin; icol <= colmax; icol ++) {
		double mid = (columnLeft [icol - colmin] + columnRight [icol - colmin]) / 2;
		const char32 *columnLabel = table -> columnHeaders [icol]. label;
		if (! columnLabel || columnLabel [0] == U'\0') columnLabel = U"?";
		Graphics_text (graphics.get(), mid, rowmin - 2, icol);
		Graphics_text (graphics.get(), mid, rowmin - 1, columnLabel);
	}
	/*
	 * Show the cell contents.
	 */
	for (long irow = rowmin; irow <= rowmax; irow ++) {
		for (long icol = colmin; icol <= colmax; icol ++) {
			double mid = (columnLeft [icol - colmin] + columnRight [icol - colmin]) / 2;
			const char32 *cell = Table_getStringValue_Assert (table, irow, icol);
			Melder_assert (cell);
			if (cell [0] == U'\0') cell = U"?";
			Graphics_text (graphics.get(), mid, irow, cell);
		}
	}
}

bool structTableEditor :: v_click (double xclick, double yWC, bool shiftKeyPressed) {
	Table table = static_cast<Table> (our data);
	return true;
}

static void gui_text_cb_changed (TableEditor me, GuiTextEvent /* event */) {
	Editor_broadcastDataChanged (me);
}

static void gui_drawingarea_cb_expose (TableEditor me, GuiDrawingArea_ExposeEvent /* event */) {
	if (! my graphics) return;
	my v_draw ();
}

static void gui_drawingarea_cb_click (TableEditor me, GuiDrawingArea_ClickEvent event) {
	if (! my graphics) return;
	double xWC, yWC;
	Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & xWC, & yWC);
	// TODO: implement selection
}

static void gui_drawingarea_cb_resize (TableEditor me, GuiDrawingArea_ResizeEvent /* event */) {
	if (! my graphics) return;
	Graphics_updateWs (my graphics.get());
}

static void gui_cb_scrollHorizontal (TableEditor me, GuiScrollBarEvent event) {
	int value = GuiScrollBar_getValue (event -> scrollBar);
	if (value != my leftColumn) {
		my leftColumn = value;
		#if cocoa || gtk || motif
			Graphics_updateWs (my graphics.get());   // wait for expose event
		#else
			Graphics_clearWs (my graphics.get());
			my v_draw ();   // do not wait for expose event
		#endif
	}
}

static void gui_cb_scrollVertical (TableEditor me, GuiScrollBarEvent event) {
	int value = GuiScrollBar_getValue (event -> scrollBar);
	if (value != my topRow) {
		my topRow = value;
		#if cocoa || gtk || motif
			Graphics_updateWs (my graphics.get());   // wait for expose event
		#else
			Graphics_clearWs (my graphics.get());
			my v_draw ();   // do not wait for expose event
		#endif
	}
}

void structTableEditor :: v_createChildren () {
	Table table = static_cast<Table> (data);
	int y = Machine_getMenuBarHeight () + 4, scrollWidth = Machine_getScrollBarWidth ();

	our text = GuiText_createShown (our windowForm, 0, 0, y, y + Machine_getTextHeight (), 0);
	GuiText_setChangedCallback (our text, gui_text_cb_changed, this);
	y += Machine_getTextHeight () + 4;

	our drawingArea = GuiDrawingArea_createShown (our windowForm, 0, - scrollWidth, y, - scrollWidth,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, gui_drawingarea_cb_resize, this, 0);

	our verticalScrollBar = GuiScrollBar_createShown (our windowForm, - scrollWidth, 0, y, - scrollWidth,
		1, table -> rows.size + 1, 1, 1, 1, 10, gui_cb_scrollVertical, this, 0);

	our horizontalScrollBar = GuiScrollBar_createShown (our windowForm, 0, - scrollWidth, - scrollWidth, 0,
		1, table -> numberOfColumns + 1, 1, 1, 1, 3, gui_cb_scrollHorizontal, this, GuiScrollBar_HORIZONTAL);

	GuiDrawingArea_setSwipable (our drawingArea, our horizontalScrollBar, our verticalScrollBar);
}

void structTableEditor :: v_createMenus () {
	TableEditor_Parent :: v_createMenus ();

	Editor_addCommand (this, U"File", U"Preferences...", 0, menu_cb_preferences);
	Editor_addCommand (this, U"File", U"-- before scripting --", 0, nullptr);

	#ifndef macintosh
	Editor_addCommand (this, U"Edit", U"-- cut copy paste --", 0, nullptr);
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
		Editor_init (me.get(), 0, 0, 700, 500, title, table);
		#if motif
		Melder_assert (XtWindow (my drawingArea -> d_widget));
		#endif
		my topRow = 1;
		my leftColumn = 1;
		my selectedColumn = 1;
		my selectedRow = 1;
		my graphics = Graphics_create_xmdrawingarea (my drawingArea);
		double size_pixels = SIZE_INCHES * Graphics_getResolution (my graphics.get());
		Graphics_setWsViewport (my graphics.get(), 0.0, size_pixels, 0.0, size_pixels);
		Graphics_setWsWindow (my graphics.get(), 0.0, size_pixels, 0.0, size_pixels);
		Graphics_setViewport (my graphics.get(), 0.0, size_pixels, 0.0, size_pixels);
		Graphics_setFont (my graphics.get(), kGraphics_font_COURIER);
		Graphics_setFontSize (my graphics.get(), 12);
		Graphics_setUnderscoreIsSubscript (my graphics.get(), false);
		Graphics_setAtSignIsLink (my graphics.get(), true);
		return me;
	} catch (MelderError) {
		Melder_throw (U"TableEditor not created.");
	}
}

/* End of file TableEditor.cpp */
