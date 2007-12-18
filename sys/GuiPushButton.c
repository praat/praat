/* GuiPushButton.c
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
 * pb 2007/12/16 abstraction from motif
 */

#include "GuiP.h"

struct GuiPushButton {
	void (*activateCallback) (Widget widget, void *closure);
	void *activateClosure;
	#if gtk
		// model callback fields?
	#endif
};

#if gtk
	static void gtk_GuiPushButton_destroyCallback (Widget me, gpointer userData) {
		if (userData) Melder_free (userData);
	}
	static void gtk_GuiPushButton_activateCallback (Widget me, gpointer data) {
		// TODO: Wat moet hier nu eigenlijk gebeuren? 	
		// Is dit de functie die in de userdata zet of de knop aan/uit staat?	
	}
#else
	static void motif_GuiPushButton_destroyCallback (Widget me, XtPointer closure, XtPointer call) {
		(void) closure; (void) call;
		struct GuiPushButton *guiPushButton = _GuiObject_getUserData (me);
		Melder_free (guiPushButton);
	}
	static void motif_GuiPushButton_activateCallback (Widget me, XtPointer closure, XtPointer call) {
		(void) closure; (void) call;
		struct GuiPushButton *guiPushButton = _GuiObject_getUserData (me);
		if (guiPushButton -> activateCallback != NULL) {
			guiPushButton -> activateCallback (me, guiPushButton -> activateClosure);
		}
	}
#endif

Widget GuiPushButton_create (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*activateCallback) (Widget widget, void *closure), void *activateClosure, unsigned long flags)
{
	Widget me = NULL;
	struct GuiPushButton *guiPushButton = Melder_calloc (struct GuiPushButton, 1);
	guiPushButton -> activateCallback = activateCallback;
	guiPushButton -> activateClosure = activateClosure;
	#if gtk
		me = gtk_button_new_with_label (Melder_peekWcsToUtf8 (buttonText));   // BUG: is it OK to just peek, or shoudl we copy?
		_GuiObject_position (me, left, right, top, bottom);
		gtk_box_pack_start (GTK_BOX (parent), me, TRUE, FALSE, 0);
		g_signal_connect (G_OBJECT (me), "destroy",
				  G_CALLBACK (gtk_GuiPushButton_destroyCallback), guiPushButton);
		g_signal_connect (GTK_BUTTON (me), "clicked",
				  G_CALLBACK (gtk_GuiPushButton_activateCallback), guiPushButton);
	#else
		me = XtVaCreateWidget (Melder_peekWcsToUtf8 (buttonText), xmPushButtonWidgetClass, parent, NULL);
		_GuiObject_position (me, left, right, top, bottom);
		_GuiObject_setUserData (me, guiPushButton);
		XtAddCallback (me, XmNdestroyCallback, motif_GuiPushButton_destroyCallback, NULL);
		XtAddCallback (me, XmNactivateCallback, motif_GuiPushButton_activateCallback, NULL);
		if (flags & GuiPushButton_DEFAULT) {
			XtVaSetValues (parent, XmNdefaultButton, me, NULL);
		}
	#endif
	return me;
}

Widget GuiPushButton_createShown (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*activateCallback) (Widget widget, void *closure), void *activateClosure, unsigned long flags)
{
	Widget me = GuiPushButton_create (parent, left, right, top, bottom, buttonText, activateCallback, activateClosure, flags);
	GuiObject_show (me);
	return me;
}

/* End of file GuiPushButton.c */
