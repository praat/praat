/* GuiScale.cpp
 *
 * Copyright (C) 1993-2011,2012 Paul Boersma
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
 * pb & sdk 2007/12/28 gtk
 * pb 2010/11/28 removed Motif
 * pb 2011/04/06 C++
 */

#include "GuiP.h"

Thing_implement (GuiScale, GuiControl, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_scale \
		Melder_assert (widget -> widgetClass == xmScaleWidgetClass); \
		GuiScale me = (GuiScale) widget -> userData
#else
	#define iam_scale \
		GuiScale me = (GuiScale) _GuiObject_getUserData (widget)
#endif

#if gtk
	static void _GuiGtkScale_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiScale);
		forget (me);
	}
#elif cocoa
#elif win
	void _GuiWinScale_destroy (GuiObject widget) {
		iam_scale;
		DestroyWindow (widget -> window);
		trace ("forgetting a scale or a progress bar");
		forget (me);   // NOTE: my widget is not destroyed here
	}
#elif mac
	void _GuiMacScale_destroy (GuiObject widget) {
		_GuiMac_clipOnParent (widget);
		EraseRect (& widget -> rect);
		GuiMac_clipOff ();
		iam_scale;
		trace ("forgetting a scale or a progress bar");
		forget (me);   // NOTE: my widget is not destroyed here
	}
#endif

GuiScale GuiScale_create (GuiForm parent, int left, int right, int top, int bottom,
	int minimum, int maximum, int value, unsigned long flags)
{
	GuiScale me = Thing_new (GuiScale);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		my d_widget = gtk_hscrollbar_new (NULL);
		gtk_range_set_range (GTK_RANGE (my d_widget), 0, 1000);
		GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (my d_widget));
		adj -> page_size = 150;
		gtk_adjustment_changed (adj);
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkScale_destroyCallback), me);
	#elif cocoa
	#elif motif
		my d_widget = XmCreateScale (parent -> d_widget, "scale", NULL, 0);
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		XtVaSetValues (my d_widget, XmNorientation, XmHORIZONTAL,
			XmNminimum, minimum, XmNmaximum, maximum, XmNvalue, value, //XmNy, 300,
			#ifdef macintosh
				//XmNscaleWidth, 340,
			#endif
			NULL);
	#endif
	return me;
}

GuiScale GuiScale_createShown (GuiForm parent, int left, int right, int top, int bottom,
	int minimum, int maximum, int value, unsigned long flags)
{
	GuiScale me = GuiScale_create (parent, left, right, top, bottom, minimum, maximum, value, flags);
	my f_show ();
	return me;
}

void structGuiScale :: f_setValue (int value) {
	#if gtk
		gtk_range_set_value (GTK_RANGE (d_widget), value);
	#elif cocoa
	#elif motif
		XmScaleSetValue (d_widget, value);
	#endif
}

/* End of file GuiScale.cpp */
