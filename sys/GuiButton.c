/* GuiButton.c
 *
 * Copyright (C) 1993-2007 Paul Boersma
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
 * pb & sdk 2007/12/17 gtk
 */

#include "GuiP.h"
#define my  me ->
#define iam(x)  x me = (x) void_me

typedef struct structGuiButton {
	void (*clickedCallback) (Widget widget, void *closure);
	void *clickedClosure;
} *GuiButton;

#if gtk
	static void gtk_GuiButton_destroyCallback (Widget widget, gpointer void_me) {
		(void) widget;
		iam (GuiButton);
		Melder_free (me);
	}
	static void gtk_GuiButton_clickedCallback (Widget widget, gpointer void_me) {
		iam (GuiButton);
		if (my clickedCallback != NULL) {
			my clickedCallback (widget, my clickedClosure);
		}
	}
#else
	static void motif_GuiButton_destroyCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiButton);
		Melder_free (me);
	}
	static void motif_GuiButton_clickedCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) call;
		iam (GuiButton);
		if (my clickedCallback != NULL) {
			my clickedCallback (widget, my clickedClosure);
		}
	}
#endif

Widget GuiButton_create (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*clickedCallback) (Widget widget, void *closure), void *clickedClosure, unsigned long flags)
{
	Widget me = NULL;
	GuiButton guiButton = Melder_calloc (struct structGuiButton, 1);
	guiButton -> clickedCallback = clickedCallback;
	guiButton -> clickedClosure = clickedClosure;
	#if gtk
		me = gtk_button_new_with_label (Melder_peekWcsToUtf8 (buttonText));
		_GuiObject_position (me, left, right, top, bottom);
		gtk_box_pack_start (GTK_BOX (parent), me, TRUE, FALSE, 0);
		g_signal_connect (G_OBJECT (me), "destroy",
				  G_CALLBACK (gtk_GuiButton_destroyCallback), guiButton);
		g_signal_connect (GTK_BUTTON (me), "clicked",
				  G_CALLBACK (gtk_GuiButton_clickedCallback), guiButton);
	#else
		me = XtVaCreateWidget (Melder_peekWcsToUtf8 (buttonText), xmPushButtonWidgetClass, parent, NULL);
		_GuiObject_position (me, left, right, top, bottom);
		_GuiObject_setUserData (me, guiButton);
		XtAddCallback (me, XmNdestroyCallback, motif_GuiButton_destroyCallback, guiButton);
		XtAddCallback (me, XmNactivateCallback, motif_GuiButton_clickedCallback, guiButton);
		if (flags & GuiButton_DEFAULT) {
			XtVaSetValues (parent, XmNdefaultButton, me, NULL);
		}
	#endif
	return me;
}

Widget GuiButton_createShown (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*clickedCallback) (Widget widget, void *closure), void *clickedClosure, unsigned long flags)
{
	Widget me = GuiButton_create (parent, left, right, top, bottom, buttonText, clickedCallback, clickedClosure, flags);
	GuiObject_show (me);
	return me;
}

/* End of file GuiButton.c */
