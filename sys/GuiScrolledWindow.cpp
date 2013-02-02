/* GuiScrolledWindow.cpp
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

#include "GuiP.h"

Thing_implement (GuiScrolledWindow, GuiControl, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_scrolledwindow \
		Melder_assert (widget -> widgetClass == xmScrolledWindowWidgetClass); \
		GuiScrolledWindow me = (GuiScrolledWindow) widget -> userData
#else
	#define iam_scrolledwindow \
		GuiScrolledWindow me = (GuiScrolledWindow) _GuiObject_getUserData (widget)
#endif

#if gtk
	static void _GuiGtkScrolledWindow_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiScrolledWindow);
		forget (me);
	}
#elif cocoa
#elif win
	void _GuiWinScrolledWindow_destroy (GuiObject widget) {
		DestroyWindow (widget -> window);
		iam_scrolledwindow;
		forget (me);   // NOTE: my widget is not destroyed here
	}
#elif mac
	void _GuiMacScrolledWindow_destroy (GuiObject widget) {
		iam_scrolledwindow;
		/*
		 * One can get here either via GuiScrolledWindow_create or via GuiList_create,
		 * so we cannot be certain that we have a GuiScrolledWindow!!!
		 * So we purposely have a memory leak here:
		 */
		//forget (me);   // NOTE: my widget is not destroyed here
	}
#endif

GuiScrolledWindow GuiScrolledWindow_create (GuiForm parent, int left, int right, int top, int bottom,
	int horizontalScrollbarPersistence, int verticalScrollbarPersistence, unsigned long flags)
{
	GuiScrolledWindow me = Thing_new (GuiScrolledWindow);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		my d_widget = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (my d_widget),
			horizontalScrollbarPersistence == 0 ? GTK_POLICY_NEVER : horizontalScrollbarPersistence == 1 ? GTK_POLICY_AUTOMATIC : GTK_POLICY_ALWAYS,
			verticalScrollbarPersistence   == 0 ? GTK_POLICY_NEVER : verticalScrollbarPersistence   == 1 ? GTK_POLICY_AUTOMATIC : GTK_POLICY_ALWAYS);
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkScrolledWindow_destroyCallback), me);
	#elif cocoa
	#elif motif
		my d_widget = XmCreateScrolledWindow (parent -> d_widget, "scrolledWindow", NULL, 0);
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		Melder_assert (my classInfo == classGuiScrolledWindow);
		trace ("me = %p, user data = %p", me, my d_widget -> userData);
	#endif
	return me;
}


GuiScrolledWindow GuiScrolledWindow_createShown (GuiForm parent, int left, int right, int top, int bottom,
	int horizontalScrollbarPersistence, int verticalScrollbarPersistence, unsigned long flags)
{
	GuiScrolledWindow me = GuiScrolledWindow_create (parent, left, right, top, bottom, horizontalScrollbarPersistence, verticalScrollbarPersistence, flags);
	my f_show ();
	return me;
}

/* End of file GuiScrolledWindow.cpp */
