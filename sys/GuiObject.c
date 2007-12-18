/* GuiObject.c
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
 * pb 2007/12/18 abstraction from motif
 */

#include "GuiP.h"

static int _Gui_defaultHeight (WidgetClass klas) {
	if (klas == xmPushButtonWidgetClass) return Gui_PUSHBUTTON_HEIGHT;
	return 100;
}

void _GuiObject_position (Widget me, int left, int right, int top, int bottom) {
	#if gtk
	#else
		if (left >= 0) {
			if (right > 0) {
				XtVaSetValues (me, XmNx, left, XmNwidth, right - left, NULL);
			} else if (right == Gui_AUTOMATIC) {
				XtVaSetValues (me, XmNx, left, NULL);
			} else {
				XtVaSetValues (me, XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, left,
					XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, NULL);
			}
		} else if (left == Gui_AUTOMATIC) {
			Melder_assert (right <= 0);
			if (right != Gui_AUTOMATIC)
				XtVaSetValues (me, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, NULL);
		} else {
			Melder_assert (right <= 0);
			XtVaSetValues (me, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, XmNwidth, right - left, NULL);
		}
		if (top >= 0) {
			if (bottom > 0) {
				XtVaSetValues (me, XmNy, top, XmNheight, bottom - top, NULL);
			} else if (bottom == Gui_AUTOMATIC) {
				XtVaSetValues (me, XmNy, top, XmNheight, _Gui_defaultHeight (XtClass (me)), NULL);
			} else {
				XtVaSetValues (me, XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, top,
					XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, NULL);
			}
		} else if (top == Gui_AUTOMATIC) {
			Melder_assert (bottom <= 0);
			if (bottom != Gui_AUTOMATIC)
				XtVaSetValues (me, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, XmNheight, _Gui_defaultHeight (XtClass (me)), NULL);
		} else {
			Melder_assert (bottom <= 0);
			XtVaSetValues (me, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, XmNheight, bottom - top, NULL);
		}
	#endif
}

void * _GuiObject_getUserData (Widget me) {
	void *userData = NULL;
	#if gtk
		userData = (void *) g_object_get_data (G_OBJECT (me), "userData")
	#else
		XtVaGetValues (me, XmNuserData, & userData, NULL);
	#endif
	return userData;
}

void _GuiObject_setUserData (Widget me, void *userData) {
	#if gtk
		g_object_set_data (G_OBJECT (me), "userData", userData);
	#else
		XtVaSetValues (me, XmNuserData, userData, NULL);
	#endif
}

void GuiObject_destroy (Widget me) {
	#if gtk
		gtk_widget_destroy (me);
	#else
		XtDestroyWidget (me);
	#endif
}

void GuiObject_hide (Widget me) {
	#if gtk
		gtk_widget_hide (me);
	#else
		XtUnmanageChild (me);
		#if win
			// nothing, because the scrolled window is not a widget
		#elif mac
			if (my widgetClass == xmListWidgetClass) {
				XtUnmanageChild (XtParent (me));   // the containing scrolled window; BUG if created with XmScrolledList?
			}
		#elif motif
			if (XtClass (me) == xmListWidgetClass) {
				XtUnmanageChild (XtParent (me));   // the containing scrolled window; BUG if created with XmScrolledList?
			}
		#endif
	#endif
}

void GuiObject_setSensitive (Widget me, bool sensitive) {
	#if gtk
		gtk_widget_set_sensitive (me, sensitive); // BUG in GTK+ be careful!
							 // http://bugzilla.gnome.org/show_bug.cgi?id=56070
		gtk_widget_hide (me);
		gtk_widget_show (me);   // BUG: only do these two if visible
	#else
		XtSetSensitive (me, sensitive);
	#endif
}

void GuiObject_show (Widget me) {
	#if gtk
		gtk_widget_show (me);
	#else
		XtManageChild (me);
		#if win
			// nothing, because the scrolled window is not a widget
		#elif mac || motif
			if (XtClass (me) == xmListWidgetClass) {
				XtManageChild (XtParent (me));   // the containing scrolled window; BUG if created with XmScrolledList?
			}
		#endif
	#endif
}

/* End of file GuiObject.c */
