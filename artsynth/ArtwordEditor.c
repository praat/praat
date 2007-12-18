/* ArtwordEditor.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2003/05/19 Melder_atof
 * pb 2007/08/30 include menu bar height
 * pb 2007/12/14 Gui
 */

#include "ArtwordEditor.h"
#include "machine.h"

static void destroy (I) {
	iam (ArtwordEditor);
	forget (my graphics);
	inherited (ArtwordEditor) destroy (me);
}

static void updateList (ArtwordEditor me) {
	Artword artword = my data;
	ArtwordData a = & artword -> data [my feature];
	GuiList_deleteAllItems (my list);
	for (int i = 1; i <= a -> numberOfTargets; i ++) {
		static MelderString itemText = { 0 };
		MelderString_empty (& itemText);
		MelderString_append3 (& itemText, Melder_single (a -> times [i]), L"  ", Melder_single (a -> targets [i]));
		GuiList_insertItem (my list, itemText.string, i);
	}
	Graphics_updateWs (my graphics);
}

static void gui_pushButton_cb_removeTarget (Widget widget, I) {
	(void) widget;
	iam (ArtwordEditor);
	Artword artword = my data;
	long numberOfSelectedPositions, *selectedPositions = GuiList_getSelectedPositions (my list, & numberOfSelectedPositions);
	if (selectedPositions != NULL) {
		for (long ipos = numberOfSelectedPositions; ipos > 0; ipos --)
			Artword_removeTarget (artword, my feature, selectedPositions [ipos]);
	}
	NUMlvector_free (selectedPositions, 1);
	updateList (me);
	Editor_broadcastChange (me);
}

static void gui_pushButton_cb_addTarget (Widget widget, I) {
	(void) widget;
	iam (ArtwordEditor);
	Artword artword = my data;
	wchar_t *timeText = GuiText_getString (my time);
	double tim = Melder_atofW (timeText);
	wchar_t *valueText = GuiText_getString (my value);
	double value = Melder_atofW (valueText);
	ArtwordData a = & artword -> data [my feature];
	int i = 1, oldCount = a -> numberOfTargets;
	Melder_free (timeText);
	Melder_free (valueText);
	Artword_setTarget (artword, my feature, tim, value);

	/* Optimization instead of "updateList (me)". */

	if (tim < 0) tim = 0;
	if (tim > artword -> totalTime) tim = artword -> totalTime;
	while (tim != a -> times [i]) i ++;
	static MelderString itemText = { 0 };
	MelderString_empty (& itemText);
	MelderString_append3 (& itemText, Melder_single (tim), L"  ", Melder_single (value));
	if (a -> numberOfTargets == oldCount) {
		GuiList_replaceItem (my list, itemText.string, i);
	} else {
		GuiList_insertItem (my list, itemText.string, i);
	}
	Graphics_updateWs (my graphics);
	Editor_broadcastChange (me);
}

static void cb_toggle (GUI_ARGS) {
	GUI_IAM (ArtwordEditor);
	int i = 0;
	while (w != my button [i]) i ++;
	my feature = i;
	updateList (me);
}

static void cb_draw (GUI_ARGS) {
	GUI_IAM (ArtwordEditor);
	Artword artword = my data;
	#ifdef UNIX
		if (((XmDrawingAreaCallbackStruct *) call) -> event -> xexpose. count) return;
	#endif
	Graphics_clearWs (my graphics);
	Artword_draw (artword, my graphics, my feature, TRUE);
}

static void cb_click (GUI_ARGS) {
	GUI_IAM (ArtwordEditor);
	GuiEvent event = GuiEvent_fromCallData (call);
	if (! GuiEvent_isButtonPressedEvent (event)) return;
	Artword artword = my data;
	Graphics_setWindow (my graphics, 0, artword -> totalTime, -1.0, 1.0);
	Graphics_setInner (my graphics);
	int x = GuiEvent_x (event), y = GuiEvent_y (event);
	double xWC, yWC;
	Graphics_DCtoWC (my graphics, x, y, & xWC, & yWC);
	Graphics_unsetInner (my graphics);
	GuiText_setString (my time, Melder_fixed (xWC, 6));
	GuiText_setString (my value, Melder_fixed (yWC, 6));
}

static void dataChanged (I) {
	iam (ArtwordEditor);
	updateList (me);
	Graphics_updateWs (my graphics);
}

static void createChildren (I) {
	iam (ArtwordEditor);
	int dy = Machine_getMenuBarHeight ();
	XtVaCreateManagedWidget ("Targets:", xmLabelGadgetClass, my dialog,
		XmNx, 40, XmNy, dy + 3, XmNwidth, 60, NULL);
	XtVaCreateManagedWidget ("Times:", xmLabelGadgetClass, my dialog,
		XmNx, 5, XmNy, dy + 20, XmNwidth, 60, NULL);
	XtVaCreateManagedWidget ("Values:", xmLabelGadgetClass, my dialog,
		XmNx, 80, XmNy, dy + 20, XmNwidth, 60, NULL);
	my list = GuiList_create (my dialog, 0, 140, dy + 40, dy + 340, true);
	GuiObject_show (my list);

	GuiButton_createShown (my dialog, 10, 130, dy + 410, Gui_AUTOMATIC,
		L"Remove target", gui_pushButton_cb_removeTarget, me, 0);

	my drawingArea = XtVaCreateManagedWidget
		("drawingArea", xmDrawingAreaWidgetClass, my dialog,
		 XmNx, 170, XmNy, dy + 10,
		 XmNwidth, 300, XmNheight, 300, NULL);

	XtVaCreateManagedWidget ("Time:", xmLabelGadgetClass, my dialog,
		XmNx, 220, XmNy, dy + 340, XmNwidth, 50, NULL);
	my time = XtVaCreateManagedWidget
		("Time", xmTextWidgetClass, my dialog,
		 XmNx, 270, XmNy, dy + 340, XmNwidth, 100, NULL);

	XtVaCreateManagedWidget ("Value:", xmLabelGadgetClass, my dialog,
		XmNx, 220, XmNy, dy + 370, XmNwidth, 50, NULL);
	my value = XtVaCreateManagedWidget
		("Value", xmTextWidgetClass, my dialog,
		 XmNx, 270, XmNy, dy + 370, XmNwidth, 100, NULL);

	GuiButton_createShown (my dialog, 240, 360, dy + 410, Gui_AUTOMATIC,
		L"Add target", gui_pushButton_cb_addTarget, me, GuiButton_DEFAULT);

	my radio = XtVaCreateManagedWidget
		("radioBox", xmRowColumnWidgetClass, my dialog,
		 XmNradioBehavior, True, XmNx, 470, XmNy, dy, NULL);
	for (int i = 1; i <= enumlength (Art_MUSCLE); i ++) {
		my button [i] = XtVaCreateManagedWidget
			(Melder_peekWcsToUtf8 (enumstring (Art_MUSCLE, i)), xmToggleButtonGadgetClass, my radio,
			#if defined (_WIN32) || defined (macintosh)
				XmNheight, 18,
			#endif
				XmNwidth, 160, NULL);
		XtAddCallback (my button [i], XmNvalueChangedCallback, cb_toggle, (XtPointer) me);
	}
	my feature = 1;
	XmToggleButtonGadgetSetState (my button [1], True, False);
}

class_methods (ArtwordEditor, Editor)
	class_method (destroy)
	class_method (dataChanged)
	class_method (createChildren)
class_methods_end

ArtwordEditor ArtwordEditor_create (Widget parent, const wchar_t *title, Artword data) {
	ArtwordEditor me = new (ArtwordEditor);
	if (! me || ! Editor_init (me, parent, 20, 40, 650, 600, title, data))
		return NULL;
	//XtUnmanageChild (my menuBar);
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);

	XtAddCallback (my drawingArea, XmNexposeCallback, cb_draw, (XtPointer) me);
	XtAddCallback (my drawingArea, XmNinputCallback, cb_click, (XtPointer) me);
	updateList (me);
	return me;
}

/* End of file ArtwordEditor.c */
