/* GuiWindow.cpp
 *
 * Copyright (C) 1993-2018,2020 Paul Boersma, 2013 Tom Naughton
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
#include "../kar/UnicodeData.h"
#include "machine.h"
#include <locale.h>

Thing_implement (GuiWindow, GuiShell, 0);

#undef iam
#define iam(x)  x me = (x) void_me

#if gtk
	static gboolean _GuiWindow_destroyCallback (GuiObject widget, GdkEvent *event, gpointer void_me) {
		(void) widget;
		iam (GuiWindow);
		forget (me);
		return true;
	}
	static gboolean _GuiWindow_goAwayCallback (GuiObject widget, GdkEvent *event, gpointer void_me) {
		(void) widget;
		iam (GuiWindow);
		if (my d_goAwayCallback) {
			my d_goAwayCallback (my d_goAwayBoss);
		}
		return true;
	}
	static void _GuiWindow_child_resizeCallback (GtkWidget *childWidget, gpointer data) {
		GtkAllocation *parentAllocation = (GtkAllocation *) data;
		GtkWidget *parentWidget = gtk_widget_get_parent (childWidget);
		Thing_cast (GuiThing, child, _GuiObject_getUserData (childWidget));
		if (child) {
			GuiControl control = nullptr;
			if (Thing_isa (child, classGuiWindow)) {
				control = static_cast <GuiWindow> (child);
				//Melder_casual (U"menu bar");
			} else if (Thing_isa (child, classGuiControl)) {
				control = static_cast <GuiControl> (child);
				//Melder_casual (U"control");
			} else if (Thing_isa (child, classGuiMenu)) {
				Thing_cast (GuiMenu, menu, child);
				control = menu -> d_cascadeButton.get();
				//Melder_casual (U"menu");
			}
			if (control) {
				/*
					Move and resize.
				*/
				trace (U"moving child of class ", Thing_className (control));
				int left = control -> d_left, right = control -> d_right, top = control -> d_top, bottom = control -> d_bottom;
				if (left   <  0) left   += parentAllocation -> width;   // this replicates structGuiControl :: v_positionInForm ()
				if (right  <= 0) right  += parentAllocation -> width;
				if (top    <  0) top    += parentAllocation -> height;
				if (bottom <= 0) bottom += parentAllocation -> height;
				trace (U"moving child to (", left, U",", top, U") with size ", right - left, U" x ", bottom - top, U".");
				GtkAllocation childAllocation { left, top, right - left, bottom - top };
				gtk_widget_size_allocate (GTK_WIDGET (childWidget), & childAllocation);
				trace (U"moved child of class ", Thing_className (control));
			}
		}
	}
	static void _GuiWindow_resizeCallback (GuiObject widget, GtkAllocation *allocation, gpointer void_me) {
		(void) widget;
		iam (GuiWindow);
		trace (U"fixed received size allocation: (", allocation -> x, U", ", allocation -> y,
			U"), ", allocation -> width, U" x ", allocation -> height, U".");
		/*
			Apparently, GTK sends the size allocation message both to the shell and to its fixed-container child.
			we could capture the message either from the shell or from the fixed; we choose to do it from the fixed.
		*/
		Melder_assert (GTK_IS_FIXED (widget));
		/*
			We move and resize all the children of the fixed.
		*/
		gtk_container_foreach (GTK_CONTAINER (widget), _GuiWindow_child_resizeCallback, allocation);
		my d_width = allocation -> width;
		my d_height = allocation -> height;
		trace (U"end");
	}
#elif motif
	static void _GuiMotifWindow_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiWindow);
		if (my d_xmMenuBar) {
		}
		trace (U"destroying window widget");
		forget (me);
	}
	static void _GuiMotifWindow_goAwayCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiWindow);
		if (my d_goAwayCallback) {
			//Melder_casual (U"_GuiMotifWindow_goAwayCallback: before");
			my d_goAwayCallback (my d_goAwayBoss);
			//Melder_casual (U"_GuiMotifWindow_goAwayCallback: after");
		}
	}
#endif

GuiWindow GuiWindow_create (int x, int y, int width, int height, int minimumWidth, int minimumHeight,
	conststring32 title /* cattable */, GuiShell_GoAwayCallback goAwayCallback, Thing goAwayBoss, uint32 flags)
{
	autoGuiWindow me = Thing_new (GuiWindow);
	if (Melder_debug == 55)
		Melder_casual (U"\t", Thing_messageNameAndAddress (me.get()), U" init");
	my d_parent = nullptr;
	my d_goAwayCallback = goAwayCallback;
	my d_goAwayBoss = goAwayBoss;
	#if gtk
		(void) flags;
		GuiGtk_initialize ();
		my d_gtkWindow = (GtkWindow *) gtk_window_new (GTK_WINDOW_TOPLEVEL);
		g_signal_connect (G_OBJECT (my d_gtkWindow), "delete-event", goAwayCallback ? G_CALLBACK (_GuiWindow_goAwayCallback) : G_CALLBACK (gtk_widget_hide), me.get());
		g_signal_connect (G_OBJECT (my d_gtkWindow), "destroy-event", G_CALLBACK (_GuiWindow_destroyCallback), me.get());

		gtk_window_set_default_size (GTK_WINDOW (my d_gtkWindow), width, height);
		gtk_window_set_resizable (GTK_WINDOW (my d_gtkWindow), true);
		GuiShell_setTitle (me.get(), title);

		my d_widget = gtk_fixed_new ();
		_GuiObject_setUserData (my d_widget, me.get());
		gtk_widget_set_size_request (GTK_WIDGET (my d_widget), minimumWidth, minimumHeight);
		gtk_container_add (GTK_CONTAINER (my d_gtkWindow), GTK_WIDGET (my d_widget));
		GdkGeometry geometry = { minimumWidth, minimumHeight, 0, 0, 0, 0, 0, 0, 0, 0, GDK_GRAVITY_NORTH_WEST };
		gtk_window_set_geometry_hints (my d_gtkWindow, GTK_WIDGET (my d_widget), & geometry, GDK_HINT_MIN_SIZE);
		g_signal_connect (G_OBJECT (my d_widget), "size-allocate", G_CALLBACK (_GuiWindow_resizeCallback), me.get());
	#elif motif
		my d_xmShell = XmCreateShell (nullptr, flags & GuiWindow_FULLSCREEN ? "Praatwulgfullscreen" : "Praatwulg", nullptr, 0);
		XtVaSetValues (my d_xmShell, XmNdeleteResponse, goAwayCallback ? XmDO_NOTHING : XmUNMAP, nullptr);
		XtVaSetValues (my d_xmShell, XmNx, x, XmNy, y, XmNwidth, (Dimension) width, XmNheight, (Dimension) height, nullptr);
		if (goAwayCallback)
			XmAddWMProtocolCallback (my d_xmShell, 'delw', _GuiMotifWindow_goAwayCallback, (char *) me.get());
		GuiShell_setTitle (me.get(), title);
		my d_widget = XmCreateForm (my d_xmShell, "dialog", nullptr, 0);
		_GuiObject_setUserData (my d_widget, me.get());
		XtAddCallback (my d_widget, XmNdestroyCallback, _GuiMotifWindow_destroyCallback, me.get());
		XtVaSetValues (my d_widget, XmNdialogStyle, XmDIALOG_MODELESS, XmNautoUnmanage, False, nullptr);
	#elif cocoa
		(void) flags;
		NSRect rect = { { static_cast<CGFloat>(x), static_cast<CGFloat>(y) }, { static_cast<CGFloat>(width), static_cast<CGFloat>(height) } };
		my d_cocoaShell = [[GuiCocoaShell alloc]
			initWithContentRect: rect
			styleMask: NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
			backing: NSBackingStoreBuffered
			defer: false];
		if (Melder_debug == 55)
			Melder_casual (U"\t\tGuiCocoaShell-", Melder_pointer (my d_cocoaShell), U" init in ", Thing_messageNameAndAddress (me.get()));
		[my d_cocoaShell setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];
        [my d_cocoaShell setMinSize: NSMakeSize (minimumWidth, minimumHeight)];
		GuiShell_setTitle (me.get(), title);
		[my d_cocoaShell makeKeyAndOrderFront: nil];
		my d_widget = (GuiObject) [my d_cocoaShell   contentView];   // BUG: this d_widget doesn't have the GuiCocoaAny protocol
		_GuiObject_setUserData (my d_cocoaShell, me.get());
		//if (! theGuiCocoaWindowDelegate) {
		//	theGuiCocoaWindowDelegate = [[GuiCocoaWindowDelegate alloc] init];
		//}
		//[my d_cocoaWindow setDelegate: theGuiCocoaWindowDelegate];
	#endif
	my d_width = width;
	my d_height = height;
	my d_shell = me.get();
	return me.releaseToAmbiguousOwner();
}

uinteger theGuiTopLowAccelerators [8];

void GuiWindow_addMenuBar (GuiWindow me) {
	#if gtk
		my d_gtkMenuBar = (GtkMenuBar *) gtk_menu_bar_new ();
		_GuiObject_setUserData (my d_gtkMenuBar, me);
		my v_positionInForm (my d_gtkMenuBar, 0, 0, 0, Machine_getMenuBarHeight (), me);   // BUG?
		
		// we need an accelerator group for each window we're creating accelerated menus on
		GuiObject topwin = gtk_widget_get_toplevel (GTK_WIDGET (my d_widget));
		Melder_assert (topwin == my d_gtkWindow);
		GtkAccelGroup *ag = gtk_accel_group_new ();
		gtk_window_add_accel_group (GTK_WINDOW (topwin), ag);
		// unfortunately, menu-bars don't fiddle with accel-groups, so we need a way
		// to pass it to the sub-menus created upon this bar for their items to have
		// access to the accel-group
		g_object_set_data (G_OBJECT (my d_gtkMenuBar), "accel-group", ag);
		gtk_widget_show (GTK_WIDGET (my d_gtkMenuBar));
	#elif motif
		my d_xmMenuBar = XmCreateMenuBar (my d_widget, "menuBar", nullptr, 0);
		XtVaSetValues (my d_xmMenuBar, XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, nullptr);
		XtManageChild (my d_xmMenuBar);
	#elif cocoa
		(void) me;   // this is handled elsewhere
	#endif
}

bool GuiWindow_setDirty (GuiWindow me, bool dirty) {
	#if gtk
		(void) dirty;
		return false;
	#elif motif
		(void) dirty;
		return false;
	#elif cocoa
		[my d_cocoaShell   setDocumentEdited: dirty];
		return true;
	#else
		(void) dirty;
		return false;
	#endif
}

/* End of file GuiWindow.cpp */
