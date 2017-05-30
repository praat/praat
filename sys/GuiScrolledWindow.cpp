/* GuiScrolledWindow.cpp
 *
 * Copyright (C) 1993-2011,2012,2015,2016,2017 Paul Boersma, 2013 Tom Naughton
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "GuiP.h"

Thing_implement (GuiScrolledWindow, GuiControl, 0);

#if motif
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
#elif motif
	void _GuiWinScrolledWindow_destroy (GuiObject widget) {
		DestroyWindow (widget -> window);
		iam_scrolledwindow;
		forget (me);   // NOTE: my widget is not destroyed here
	}
#elif cocoa
	@implementation GuiCocoaScrolledWindow {
		GuiScrolledWindow d_userData;
	}
	- (void) dealloc {   // override
		GuiScrolledWindow me = d_userData;
		forget (me);
		trace (U"deleting a scrolled window");
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiScrolledWindow));
		d_userData = static_cast <GuiScrolledWindow> (userData);
	}
	@end
#endif

GuiScrolledWindow GuiScrolledWindow_create (GuiForm parent, int left, int right, int top, int bottom,
	int horizontalScrollbarPersistence, int verticalScrollbarPersistence, uint32 /* flags */)
{
	autoGuiScrolledWindow me = Thing_new (GuiScrolledWindow);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		my d_widget = gtk_scrolled_window_new (nullptr, nullptr);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (my d_widget),
			horizontalScrollbarPersistence == 0 ? GTK_POLICY_NEVER : horizontalScrollbarPersistence == 1 ? GTK_POLICY_AUTOMATIC : GTK_POLICY_ALWAYS,
			verticalScrollbarPersistence   == 0 ? GTK_POLICY_NEVER : verticalScrollbarPersistence   == 1 ? GTK_POLICY_AUTOMATIC : GTK_POLICY_ALWAYS);
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkScrolledWindow_destroyCallback), me.get());
	#elif motif
		(void) horizontalScrollbarPersistence;
		(void) verticalScrollbarPersistence;
		my d_widget = XmCreateScrolledWindow (parent -> d_widget, "scrolledWindow", nullptr, 0);
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		Melder_assert (my classInfo == classGuiScrolledWindow);
		trace (U"me = ", Melder_pointer (me.get()), U", user data = ", Melder_pointer (my d_widget -> userData));
	#elif cocoa
		(void) horizontalScrollbarPersistence;
		(void) verticalScrollbarPersistence;
        GuiCocoaScrolledWindow *scrollView = [[GuiCocoaScrolledWindow alloc] init];
        my d_widget = (GuiObject) scrollView;
        my v_positionInForm (my d_widget, left, right, top, bottom, parent);
        [scrollView setUserData: me.get()];
        [scrollView setHasVerticalScroller:   YES];
        [scrollView setHasHorizontalScroller: YES];
        [scrollView setBackgroundColor: [NSColor lightGrayColor]];
	#endif
	return me.releaseToAmbiguousOwner();
}


GuiScrolledWindow GuiScrolledWindow_createShown (GuiForm parent, int left, int right, int top, int bottom,
	int horizontalScrollbarPersistence, int verticalScrollbarPersistence, uint32 flags)
{
	GuiScrolledWindow me = GuiScrolledWindow_create (parent, left, right, top, bottom, horizontalScrollbarPersistence, verticalScrollbarPersistence, flags);
	GuiThing_show (me);
	return me;
}

/* End of file GuiScrolledWindow.cpp */
