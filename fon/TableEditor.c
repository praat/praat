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
 */

#include "TableEditor.h"
#include "machine.h"
#include "EditorM.h"

#define TableEditor_members Editor_members \
	long selectedRow, selectedColumn; \
	Widget text, drawingArea, horizontalScrollBar, verticalScrollBar; \
	Graphics graphics;
#define TableEditor_methods Editor_methods \
	void (*draw) (I); \
	int (*click) (I, double xWC, double yWC, int shiftKeyPressed);
class_create_opaque (TableEditor, Editor)

/********** FUNCTIONEDITOR METHODS **********/

static void destroy (I) {
	iam (TableEditor);
	inherited (TableEditor) destroy (me);
}

static void dataChanged (I) {
	iam (TableEditor);
	Table table = my data;
	inherited (TableEditor) dataChanged (me);   /* Does all the updating. */
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
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, Machine_getScrollBarWidth () + 9,
		NULL);
	XtManageChild (my drawingArea);

	/***** Create horizontal scroll bar. *****/

	my horizontalScrollBar = XtVaCreateManagedWidget ("scrollBar",
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

	my verticalScrollBar = XtVaCreateManagedWidget ("scrollBar",
		xmScrollBarWidgetClass, form,
		XmNorientation, XmVERTICAL,
		XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getTextHeight (),
		XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, Machine_getScrollBarWidth (),
		XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 0,
		XmNwidth, Machine_getScrollBarWidth (),
		XmNminimum, 1,
		XmNmaximum, table -> rows -> size + 2,
		XmNvalue, 1,
		XmNsliderSize, 1,
		XmNincrement, 1,
		XmNpageIncrement, 5,
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
	int value, slider, incr, pincr;
	XmScrollBarGetValues (w, & value, & slider, & incr, & pincr);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_draw)
	iam (TableEditor);
#ifdef UNIX
	if (((XmDrawingAreaCallbackStruct *) call) -> event -> xexpose. count) return;
#endif
	/*if (my enableUpdates)
		drawNow (me);*/
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_input)
	iam (TableEditor);
	MotifEvent event = MotifEvent_fromCallData (call);
	int shiftKeyPressed = MotifEvent_shiftKeyPressed (event);
	double xWC, yWC;
	Melder_assert (MotifEvent_isButtonPressedEvent (event));
	Graphics_DCtoWC (my graphics, MotifEvent_x (event), MotifEvent_y (event), & xWC, & yWC);
MOTIF_CALLBACK_END

TableEditor TableEditor_create (Widget parent, const char *title, Table table) {
	TableEditor me = new (TableEditor); cherror
	Editor_init (me, parent, 0, 0, 700, 500, title, table); cherror
	Melder_assert (XtWindow (my drawingArea));
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setFontSize (my graphics, 10);

	XtAddCallback (my drawingArea, XmNexposeCallback, cb_draw, (XtPointer) me);
	XtAddCallback (my drawingArea, XmNinputCallback, cb_input, (XtPointer) me);
	XtAddCallback (my horizontalScrollBar, XmNvalueChangedCallback, cb_horizontalScroll, (XtPointer) me);
	XtAddCallback (my horizontalScrollBar, XmNdragCallback, cb_horizontalScroll, (XtPointer) me);

end:
	iferror forget (me);
	return me;
}

/* End of file TableEditor.c */
