/* TableEditor.c
 *
 * Copyright (C) 2006-2010 Paul Boersma
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
 * fb 2010/02/24 GTK
 */

#include "TableEditor.h"
#include "machine.h"
#include "EditorM.h"

#define MAXNUM_VISIBLE_COLUMNS  100
#define SIZE_INCHES  40

#define TableEditor__members(Klas) Editor__members(Klas) \
	long topRow, leftColumn, selectedRow, selectedColumn; \
	GuiObject text, drawingArea, horizontalScrollBar, verticalScrollBar; \
	double columnLeft [MAXNUM_VISIBLE_COLUMNS], columnRight [MAXNUM_VISIBLE_COLUMNS]; \
	Graphics graphics;
#define TableEditor__methods(Klas) Editor__methods(Klas) \
	void (*draw) (Klas me); \
	int (*click) (Klas me, double xWC, double yWC, int shiftKeyPressed);
Thing_declare2 (TableEditor, Editor);

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

static void dataChanged (TableEditor me) {
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

static void draw (TableEditor me) {
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

static int click (TableEditor me, double xclick, double yWC, int shiftKeyPressed) {
	Table table = my data;
	return 1;
}

static void gui_text_cb_change (I, GuiTextEvent event) {
	iam (TableEditor);
	(void) event;
	Table table = my data;
	Editor_broadcastChange (TableEditor_as_Editor (me));
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

static void gui_drawingarea_cb_resize (I, GuiDrawingAreaResizeEvent event) {
	iam (TableEditor);
	if (my graphics == NULL) return;
	Graphics_updateWs (my graphics);
}

static void createChildren (TableEditor me) {
	Table table = my data;
	GuiObject form;   /* A form inside a form; needed to keep key presses away from the drawing area. */

	#if gtk
		form = my dialog;
	#elif motif
		form = XmCreateForm (my dialog, "buttons", NULL, 0);
		XtVaSetValues (form,
			XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
			XmNbottomAttachment, XmATTACH_FORM,
			XmNtraversalOn, False,   /* Needed in order to redirect all keyboard input to the text widget. */
			NULL);
	#endif

	/***** Create text field. *****/

	#if gtk
		my text = GuiText_create(NULL, 0, 0, 0, Machine_getTextHeight(), 0);
		gtk_box_pack_start(GTK_BOX(form), my text, FALSE, FALSE, 3);
		GuiObject_show(my text);
	#else
		my text = GuiText_createShown (form, 0, 0, 0, Machine_getTextHeight (), 0);
	#endif
	GuiText_setChangeCallback (my text, gui_text_cb_change, me);

	/***** Create drawing area. *****/
	
	#if gtk
		GuiObject table_container = gtk_table_new(2, 2, FALSE);
		gtk_box_pack_start(GTK_BOX(form), table_container, TRUE, TRUE, 3);
		GuiObject_show(table_container);
		
		my drawingArea = GuiDrawingArea_create(NULL, 0, 0, 0, 0,
			gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, gui_drawingarea_cb_resize, me, 0);
		
		// need to turn off double buffering, otherwise we receive the expose events
		// delayed by one event, see also FunctionEditor.c
		gtk_widget_set_double_buffered(my drawingArea, FALSE);
		
		gtk_table_attach(GTK_TABLE(table_container), my drawingArea, 0, 1, 0, 1,
			GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
		GuiObject_show(my drawingArea);
	#else
		my drawingArea = GuiDrawingArea_createShown (form, 0, - Machine_getScrollBarWidth (),
			Machine_getTextHeight (), - Machine_getScrollBarWidth (),
			gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, NULL, me, 0);
	#endif

	/***** Create horizontal scroll bar. *****/

	#if gtk
		GtkAdjustment *hadj = GTK_ADJUSTMENT(gtk_adjustment_new(1, 1, table->numberOfColumns + 1, 1, 3, 1));
		my horizontalScrollBar = gtk_hscrollbar_new(hadj);
		gtk_table_attach(GTK_TABLE(table_container), my horizontalScrollBar, 0, 1, 1, 2,
			GTK_EXPAND | GTK_FILL, 0, 0, 0);
		GuiObject_show(my horizontalScrollBar);
	#elif motif
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

	#if gtk
		GtkAdjustment *vadj = GTK_ADJUSTMENT(gtk_adjustment_new(1, 1, table->rows->size + 1, 1, 10, 1));
		my verticalScrollBar = gtk_vscrollbar_new(vadj);
		gtk_table_attach(GTK_TABLE(table_container), my verticalScrollBar, 1, 2, 0, 1,
			0, GTK_EXPAND | GTK_FILL, 0, 0);
		GuiObject_show(my verticalScrollBar);
	#elif motif
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

static void createMenus (TableEditor me) {
	inherited (TableEditor) createMenus (TableEditor_as_parent (me));

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

static void createHelpMenuItems (TableEditor me, EditorMenu menu) {
	inherited (TableEditor) createHelpMenuItems (TableEditor_as_parent (me), menu);
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


#if gtk
	#define gui_cb_scroll(name, var) \
		void gui_cb_ ## name ## Scroll(GtkRange *rng, gpointer void_me) { \
			iam(TableEditor); \
			double var = gtk_range_get_value(rng); \
			do
#elif motif
	#define gui_cb_scroll(name, var) \
		void gui_cb_ ## name ## Scroll(GUI_ARGS) { \
			GUI_IAM(TableEditor); \
			int var; \
			{ int slider, incr, pincr; \
			  XmScrollBarGetValues(w, &var, &slider, &incr, &pincr); } \
			do
#endif
#define gui_cb_scroll_end while (0); }

static gui_cb_scroll(horizontal, value) {
	if ((int)value != my leftColumn) {
		my leftColumn = value;
		our draw(me);
	}
} gui_cb_scroll_end

static gui_cb_scroll(vertical, value) {
	if ((int)value != my topRow) {
		my topRow = value;
		our draw (me);
	}
} gui_cb_scroll_end

#if gtk
static gboolean gui_cb_drawing_area_scroll(GuiObject w, GdkEventScroll *event, gpointer void_me) {
	iam(TableEditor);
	double hv = gtk_range_get_value(GTK_RANGE(my horizontalScrollBar));
	double hi = gtk_range_get_adjustment(GTK_RANGE(my horizontalScrollBar))->step_increment;
	double vv = gtk_range_get_value(GTK_RANGE(my verticalScrollBar));
	double vi = gtk_range_get_adjustment(GTK_RANGE(my verticalScrollBar))->step_increment;
	switch (event->direction) {
		case GDK_SCROLL_UP:
			gtk_range_set_value(GTK_RANGE(my verticalScrollBar), vv - vi);
			break;
		case GDK_SCROLL_DOWN:
			gtk_range_set_value(GTK_RANGE(my verticalScrollBar), vv + vi);
			break;
		case GDK_SCROLL_LEFT:
			gtk_range_set_value(GTK_RANGE(my horizontalScrollBar), hv - hi);
			break;
		case GDK_SCROLL_RIGHT:
			gtk_range_set_value(GTK_RANGE(my horizontalScrollBar), hv + hi);
			break;
	}
	return TRUE;
}
#endif

TableEditor TableEditor_create (GuiObject parent, const wchar_t *title, Table table) {
	TableEditor me = new (TableEditor); cherror
	Editor_init (TableEditor_as_parent (me), parent, 0, 0, 700, 500, title, table); cherror
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

	#if gtk
	g_signal_connect(G_OBJECT(my drawingArea), "scroll-event", G_CALLBACK(gui_cb_drawing_area_scroll), me);
	g_signal_connect(G_OBJECT(my horizontalScrollBar), "value-changed", G_CALLBACK(gui_cb_horizontalScroll), me);
	g_signal_connect(G_OBJECT(my verticalScrollBar), "value-changed", G_CALLBACK(gui_cb_verticalScroll), me);
	#elif motif
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
