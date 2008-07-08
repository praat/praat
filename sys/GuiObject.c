/* GuiObject.c
 *
 * Copyright (C) 1993-2008 Paul Boersma
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
 * pb 2007/12/26 abstraction from motif
 * pb 2007/12/28 _GuiObject_position: allow the combination of fixed height and automatic position
 * sdk 2008/03/24 GTK
 * sdk 2008/07/01 GTK get sizes
 */

#include "GuiP.h"
#include "machine.h"
#define my  me ->

static int _Gui_defaultHeight (Widget me) {
	#if motif
	WidgetClass klas = XtClass (me);
	if (klas == xmLabelWidgetClass) return Gui_LABEL_HEIGHT;
	if (klas == xmPushButtonWidgetClass) return Gui_PUSHBUTTON_HEIGHT;
	if (klas == xmTextWidgetClass) return Gui_TEXTFIELD_HEIGHT;
	if (klas == xmToggleButtonWidgetClass) return
		#ifdef UNIX
			Gui_CHECKBUTTON_HEIGHT;   // BUG
		#else
			my isRadioButton ? Gui_RADIOBUTTON_HEIGHT : Gui_CHECKBUTTON_HEIGHT;
		#endif
	#endif
	return 100;
}

void _GuiObject_position (Widget me, int left, int right, int top, int bottom) {
	#if gtk
		// TODO: ...nog even te creatief
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
			if (right > Gui_AUTOMATIC + 3000)
				XtVaSetValues (me, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, NULL);
			else if (right != Gui_AUTOMATIC)
				XtVaSetValues (me, XmNwidth, right - Gui_AUTOMATIC, NULL);
		} else {
			Melder_assert (right <= 0);
			XtVaSetValues (me, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, XmNwidth, right - left, NULL);
		}
		if (top >= 0) {
			if (bottom > 0) {
				XtVaSetValues (me, XmNy, top, XmNheight, bottom - top, NULL);
			} else if (bottom == Gui_AUTOMATIC) {
				XtVaSetValues (me, XmNy, top, XmNheight, _Gui_defaultHeight (me), NULL);
			} else {
				XtVaSetValues (me, XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, top,
					XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, NULL);
			}
		} else if (top == Gui_AUTOMATIC) {
			Melder_assert (bottom <= 0);
			if (bottom > Gui_AUTOMATIC + 3000)
				XtVaSetValues (me, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, XmNheight, _Gui_defaultHeight (me), NULL);
			else if (bottom != Gui_AUTOMATIC)
				XtVaSetValues (me, XmNheight, bottom - Gui_AUTOMATIC, NULL);
		} else {
			Melder_assert (bottom <= 0);
			XtVaSetValues (me, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, XmNheight, bottom - top, NULL);
		}
	#endif
}

void * _GuiObject_getUserData (Widget me) {
	void *userData = NULL;
	#if gtk
		userData = (void *) g_object_get_data (G_OBJECT (me), "praat");
	#else
		XtVaGetValues (me, XmNuserData, & userData, NULL);
	#endif
	return userData;
}

void _GuiObject_setUserData (Widget me, void *userData) {
	#if gtk
		g_object_set_data (G_OBJECT (me), "praat", userData);
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

long GuiObject_getHeight (Widget me) {
	long height = 0;
	#if gtk
		gtk_widget_get_size_request (me, NULL, & height);
	#elif win || mac
		height = my height;
	#elif motif
		Dimension height_motif;
		XtVaGetValues (me, XmNheight, & height_motif, NULL);
		height = height_motif;
	#endif
	return height;
}

long GuiObject_getWidth (Widget me) {
	long width = 0;
	#if gtk
		gtk_widget_get_size_request (me, & width, NULL);
	#elif win || mac
		width = my width;
	#elif motif
		Dimension width_motif;
		XtVaGetValues (me, XmNwidth, & width_motif, NULL);
		width = width_motif;
	#endif
	return width;
}

long GuiObject_getX (Widget me) {
	long x = 0;
	#if gtk
	#elif win || mac
		x = my x;
	#elif motif
		Position x_motif;
		XtVaGetValues (me, XmNx, & x_motif, NULL);
		x = x_motif;
	#endif
	return x;
}

long GuiObject_getY (Widget me) {
	long y = 0;
	#if gtk
	#elif win || mac
		y = my y;
	#elif motif
		Position y_motif;
		XtVaGetValues (me, XmNy, & y_motif, NULL);
		y = y_motif;
	#endif
	return y;
}

void GuiObject_move (Widget me, long x, long y) {
	#if gtk
	#elif motif
		if (x != Gui_AUTOMATIC) {
			if (y != Gui_AUTOMATIC) {
				XtVaSetValues (me, XmNx, (Position) x, XmNy, (Position) y, NULL);   // 64-bit-compatible
			} else {
				XtVaSetValues (me, XmNx, (Position) x, NULL);   // 64-bit-compatible
			}
		} else if (y != Gui_AUTOMATIC) {
			XtVaSetValues (me, XmNy, (Position) y, NULL);   // 64-bit-compatible
		}
	#endif
}

void GuiObject_hide (Widget me) {
	#if gtk
		gtk_widget_hide (GTK_WIDGET (me));
	#else
		XtUnmanageChild (me);
		#if win
			// nothing, because the scrolled window is not a widget
		#elif mac
			if (my widgetClass == xmListWidgetClass) {
				XtUnmanageChild (my parent);   // the containing scrolled window; BUG if created with XmScrolledList?
			}
		#elif motif
			if (XtClass (me) == xmListWidgetClass) {
				XtUnmanageChild (XtParent (me));   // the containing scrolled window; BUG if created with XmScrolledList?
			}
		#endif
	#endif
}

Widget GuiObject_parent (Widget me) {
	#if gtk
		return gtk_widget_get_parent (me);
	#elif win || mac
		return my parent;
	#elif motif
		return XtParent (me);
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
				XtManageChild (GuiObject_parent (me));   // the containing scrolled window; BUG if created with XmScrolledList?
			}
		#endif
	#endif
}

void GuiObject_size (Widget me, long width, long height) {
	#if gtk
	#elif motif
		if (width != Gui_AUTOMATIC) {
			if (height != Gui_AUTOMATIC) {
				XtVaSetValues (me, XmNwidth, (Dimension) width, XmNheight, (Dimension) height, NULL);   // 64-bit-compatible
			} else {
				XtVaSetValues (me, XmNwidth, (Dimension) width, NULL);   // 64-bit-compatible
			}
		} else if (height != Gui_AUTOMATIC) {
			XtVaSetValues (me, XmNheight, (Dimension) height, NULL);   // 64-bit-compatible
		}
	#endif
}

/* End of file GuiObject.c */
