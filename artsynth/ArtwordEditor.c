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
	int i;
	XmListDeleteAllItems (my list);
	for (i = 1; i <= a -> numberOfTargets; i ++) {
		XmString item;
		char itemText [100];
		sprintf (itemText, "%8g  %8g", (double) a -> times [i], (double) a -> targets [i]);
		item = XmStringCreateSimple (itemText);
		XmListAddItem (my list, item, i);
		XmStringFree (item);
	}
	Graphics_updateWs (my graphics);
}

MOTIF_CALLBACK (cb_removeTarget)
	iam (ArtwordEditor);
	Artword artword = my data;
	int *position_list;
	int i, position_count;
	if (XmListGetSelectedPos (my list, & position_list, & position_count)) {
		for (i = position_count - 1; i >= 0; i --)
			Artword_removeTarget (artword, my feature, position_list [i]);
		XtFree ((XtPointer) position_list);
		updateList (me);
		Editor_broadcastChange (me);
	}
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_addTarget)
	iam (ArtwordEditor);
	Artword artword = my data;
	wchar_t *timeText = GuiText_getStringW (my time);
	double tim = Melder_atofW (timeText);
	wchar_t *valueText = GuiText_getStringW (my value);
	double value = Melder_atofW (valueText);
	XmString item;
	char itemText [100];
	ArtwordData a = & artword -> data [my feature];
	int i = 1, oldCount = a -> numberOfTargets;
	Melder_free (timeText);
	Melder_free (valueText);
	Artword_setTarget (artword, my feature, tim, value);

	/* Optimization instead of "updateList (me)". */

	if (tim < 0) tim = 0;
	if (tim > artword -> totalTime) tim = artword -> totalTime;
	while (tim != a -> times [i]) i ++;
	sprintf (itemText, "%8g %8g", (double) tim, (double) value);
	item = XmStringCreateSimple (itemText);
	if (a -> numberOfTargets == oldCount)
		XmListReplaceItemsPos (my list, & item, 1, i);
	else
		XmListAddItem (my list, item, i);
	XmStringFree (item);
	Graphics_updateWs (my graphics);
	Editor_broadcastChange (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_toggle)
	iam (ArtwordEditor);
	int i = 0;
	while (w != my button [i]) i ++;
	my feature = i;
	updateList (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_draw)
	iam (ArtwordEditor);
	Artword artword = my data;
	#ifdef UNIX
		if (((XmDrawingAreaCallbackStruct *) call) -> event -> xexpose. count) return;
	#endif
	Graphics_clearWs (my graphics);
	Artword_draw (artword, my graphics, my feature, TRUE);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_click)
	iam (ArtwordEditor);
	MotifEvent event = MotifEvent_fromCallData (call);
	if (! MotifEvent_isButtonPressedEvent (event)) return;
	Artword artword = my data;
	Graphics_setWindow (my graphics, 0, artword -> totalTime, -1.0, 1.0);
	Graphics_setInner (my graphics);
	int x = MotifEvent_x (event), y = MotifEvent_y (event);
	double xWC, yWC;
	Graphics_DCtoWC (my graphics, x, y, & xWC, & yWC);
	Graphics_unsetInner (my graphics);
	GuiText_setStringW (my time, Melder_fixed (xWC, 6));
	GuiText_setStringW (my value, Melder_fixed (yWC, 6));
MOTIF_CALLBACK_END

static void dataChanged (I) {
	iam (ArtwordEditor);
	updateList (me);
	Graphics_updateWs (my graphics);
}

static void createChildren (I) {
	iam (ArtwordEditor);
	Widget button, scrolled;
	int dy = Machine_getMenuBarHeight ();
	XtVaCreateManagedWidget ("Targets:", xmLabelGadgetClass, my dialog,
		XmNx, 40, XmNy, dy + 3, XmNwidth, 60, NULL);
	XtVaCreateManagedWidget ("Times:", xmLabelGadgetClass, my dialog,
		XmNx, 5, XmNy, dy + 20, XmNwidth, 60, NULL);
	XtVaCreateManagedWidget ("Values:", xmLabelGadgetClass, my dialog,
		XmNx, 80, XmNy, dy + 20, XmNwidth, 60, NULL);
	scrolled = XmCreateScrolledWindow (my dialog, "listWindow", NULL, 0);
	XtVaSetValues (scrolled, XmNy, dy + 40, XmNwidth, 140, XmNheight, 300, NULL);
	my list = XtVaCreateManagedWidget
		("list", xmListWidgetClass, scrolled,
		 XmNvisibleItemCount, 20 /*Artword_maximumNumberOfTargets*/,
		 XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
	XtManageChild (scrolled);

	button = XtVaCreateManagedWidget
		("Remove target", xmPushButtonGadgetClass, my dialog,
		 XmNx, 10, XmNy, dy + 410, XmNwidth, 120, NULL);
	XtAddCallback (button, XmNactivateCallback, cb_removeTarget, (XtPointer) me);

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

	button = XtVaCreateManagedWidget
		("Add target", xmPushButtonGadgetClass, my dialog,
		 XmNx, 240, XmNy, dy + 410, XmNwidth, 120, NULL);
	XtAddCallback (button, XmNactivateCallback, cb_addTarget, (XtPointer) me);
	XtVaSetValues (my dialog, XmNdefaultButton, button, NULL);

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
