/* GuiWindow.cpp
 *
 * Copyright (C) 1993-2012,2013 Paul Boersma, 2013 Tom Naughton
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
 * pb 2004/01/07 this file separated from Gui.c
 * pb 2004/02/12 don't trust window modification feedback on MacOS 9
 * pb 2004/04/06 GuiWindow_drain separated from XmUpdateDisplay
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2007/06/19 wchar_t
 * pb 2007/12/30 extraction
 * pb 2010/07/29 removed GuiWindow_show
 * pb 2011/04/06 C++
 * pb 2012/08/30 Cocoa
 */

#include "GuiP.h"
#include "UnicodeData.h"
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
		return TRUE;
	}
	static gboolean _GuiWindow_goAwayCallback (GuiObject widget, GdkEvent *event, gpointer void_me) {
		(void) widget;
		iam (GuiWindow);
		if (my d_goAwayCallback != NULL) {
			my d_goAwayCallback (my d_goAwayBoss);
		}
		return TRUE;
	}
	static gboolean _GuiWindow_resizeCallback (GuiObject widget, GtkAllocation *allocation, gpointer void_me) {
		(void) widget;
		iam (GuiWindow);
		trace ("fixed received size allocation: (%ld, %ld), %ld x %ld.", (long) allocation -> x, (long) allocation -> y, (long) allocation -> width, (long) allocation -> height);
		if (allocation -> width != my d_width || allocation -> height != my d_height) {
			trace ("user changed the size of the window?");
			/*
			 * Apparently, GTK sends the size allocation message both to the shell and to its fixed-container child.
			 * we could capture the message either from the shell or from the fixed; we choose to do it from the fixed.
			 */
			Melder_assert (GTK_IS_FIXED (widget));
			/*
			 * We move and resize all the children of the fixed.
			 */
			GList *children = GTK_FIXED (widget) -> children;
			for (GList *l = g_list_first (children); l != NULL; l = g_list_next (l)) {
				GtkFixedChild *listElement = (GtkFixedChild *) l -> data;
				GtkWidget *childWidget = listElement -> widget;
				Melder_assert (childWidget);
				Thing_cast (GuiThing, child, _GuiObject_getUserData (childWidget));
				if (child) {
					GuiControl control = NULL;
					if (Thing_member (child, classGuiControl)) {
						control = static_cast <GuiControl> (child);
					} else if (Thing_member (child, classGuiMenu)) {
						Thing_cast (GuiMenu, menu, child);
						control = menu -> d_cascadeButton;
					}
					if (control) {
						/*
						 * Move and resize.
						 */
						trace ("moving child of class %ls", Thing_className (control));
						int left = control -> d_left, right = control -> d_right, top = control -> d_top, bottom = control -> d_bottom;
						if (left   <  0) left   += allocation -> width;   // this replicates structGuiControl :: v_positionInForm ()
						if (right  <= 0) right  += allocation -> width;
						if (top    <  0) top    += allocation -> height;
						if (bottom <= 0) bottom += allocation -> height;
						trace ("moving child to (%d,%d)", left, top);
						gtk_fixed_move (GTK_FIXED (widget), GTK_WIDGET (childWidget), left, top);
						gtk_widget_set_size_request (GTK_WIDGET (childWidget), right - left, bottom - top);
						trace ("moved child of class %ls", Thing_className (control));
					}
				}
			}
			my d_width = allocation -> width;
			my d_height = allocation -> height;
			gtk_widget_set_size_request (GTK_WIDGET (widget), allocation -> width, allocation -> height);
		}
		trace ("end");
		return FALSE;
	}
#elif cocoa
	@implementation GuiCocoaWindow {
		GuiWindow d_userData;
	}
	- (void) dealloc {   // override
		GuiWindow me = d_userData;
		my d_cocoaWindow = NULL;   // this is already under destruction, so undangle
		forget (me);
		trace ("deleting a window");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiWindow));
		d_userData = static_cast <GuiWindow> (userData);
	}
	@end
	@interface GuiCocoaWindowDelegate : NSObject <NSWindowDelegate> { } @end
	@implementation GuiCocoaWindowDelegate {
	}
	- (BOOL) windowShouldClose: (id) sender {
		GuiCocoaWindow *widget = (GuiCocoaWindow *) sender;
		GuiWindow me = (GuiWindow) [widget userData];
		if (my d_goAwayCallback != NULL) {
			trace ("calling goAwayCallback)");
			my d_goAwayCallback (my d_goAwayBoss);
		} else {
			trace ("hiding window");
			[widget orderOut: nil];
		}
		return FALSE;
	}
	@end
	static GuiCocoaWindowDelegate *theGuiCocoaWindowDelegate;
#elif motif
	static void _GuiMotifWindow_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiWindow);
		if (my d_xmMenuBar) {
		}
		//Melder_casual ("destroying window widget");
		forget (me);
	}
	static void _GuiMotifWindow_goAwayCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiWindow);
		if (my d_goAwayCallback != NULL) {
			my d_goAwayCallback (my d_goAwayBoss);
		}
	}
#endif

GuiWindow GuiWindow_create (int x, int y, int width, int height,
	const wchar_t *title, void (*goAwayCallback) (void *goAwayBoss), void *goAwayBoss, unsigned long flags)
{
	GuiWindow me = Thing_new (GuiWindow);
	my d_parent = NULL;
	my d_goAwayCallback = goAwayCallback;
	my d_goAwayBoss = goAwayBoss;
	#if gtk
		GuiGtk_initialize ();
		my d_gtkWindow = (GtkWindow *) gtk_window_new (GTK_WINDOW_TOPLEVEL);
		g_signal_connect (G_OBJECT (my d_gtkWindow), "delete-event", goAwayCallback ? G_CALLBACK (_GuiWindow_goAwayCallback) : G_CALLBACK (gtk_widget_hide), me);
		g_signal_connect (G_OBJECT (my d_gtkWindow), "destroy-event", G_CALLBACK (_GuiWindow_destroyCallback), me);

		gtk_window_set_default_size (GTK_WINDOW (my d_gtkWindow), width, height);
		gtk_window_set_policy (GTK_WINDOW (my d_gtkWindow), TRUE, TRUE, FALSE);
		my f_setTitle (title);

		my d_widget = gtk_fixed_new ();
		_GuiObject_setUserData (my d_widget, me);
		gtk_widget_set_size_request (GTK_WIDGET (my d_widget), width, height);
		gtk_container_add (GTK_CONTAINER (my d_gtkWindow), GTK_WIDGET (my d_widget));
		g_signal_connect (G_OBJECT (my d_widget), "size-allocate", G_CALLBACK (_GuiWindow_resizeCallback), me);
	#elif cocoa
		NSRect rect = { { x, y }, { width, height } };
		my d_cocoaWindow = [[GuiCocoaWindow alloc]
			initWithContentRect: rect
			styleMask: NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
			backing: NSBackingStoreBuffered
			defer: false];
        [my d_cocoaWindow setMinSize: NSMakeSize (150.0, 150.0)];
		my f_setTitle (title);
		[my d_cocoaWindow makeKeyAndOrderFront: nil];
		my d_widget = [my d_cocoaWindow contentView];
		_GuiObject_setUserData (my d_cocoaWindow, me);
		if (! theGuiCocoaWindowDelegate) {
			theGuiCocoaWindowDelegate = [[GuiCocoaWindowDelegate alloc] init];
		}
		[my d_cocoaWindow setDelegate: theGuiCocoaWindowDelegate];
	#elif motif
		my d_xmShell = XmCreateShell (NULL, flags & GuiWindow_FULLSCREEN ? "Praatwulgfullscreen" : "Praatwulg", NULL, 0);
		XtVaSetValues (my d_xmShell, XmNdeleteResponse, goAwayCallback ? XmDO_NOTHING : XmUNMAP, NULL);
		XtVaSetValues (my d_xmShell, XmNx, x, XmNy, y, XmNwidth, (Dimension) width, XmNheight, (Dimension) height, NULL);
		if (goAwayCallback) {
			XmAddWMProtocolCallback (my d_xmShell, 'delw', _GuiMotifWindow_goAwayCallback, (char *) me);
		}
		my f_setTitle (title);
		my d_widget = XmCreateForm (my d_xmShell, "dialog", NULL, 0);
		_GuiObject_setUserData (my d_widget, me);
		XtAddCallback (my d_widget, XmNdestroyCallback, _GuiMotifWindow_destroyCallback, me);
		XtVaSetValues (my d_widget, XmNdialogStyle, XmDIALOG_MODELESS, XmNautoUnmanage, False, NULL);
	#endif
	my d_width = width;
	my d_height = height;
	my d_shell = me;
	return me;
}

GuiObject theGuiTopMenuBar;
unsigned long theGuiTopLowAccelerators [8];

void structGuiWindow :: f_addMenuBar () {
	#if gtk
		d_gtkMenuBar = (GtkMenuBar *) gtk_menu_bar_new ();
		_GuiObject_setUserData (d_gtkMenuBar, this);
		this -> v_positionInForm (d_gtkMenuBar, 0, 0, 0, Machine_getMenuBarHeight (), this);   // BUG?
		
		// we need an accelerator group for each window we're creating accelerated menus on
		GuiObject topwin = gtk_widget_get_toplevel (GTK_WIDGET (d_widget));
		Melder_assert (topwin == d_gtkWindow);
		GtkAccelGroup *ag = gtk_accel_group_new ();
		gtk_window_add_accel_group (GTK_WINDOW (topwin), ag);
		// unfortunately, menu-bars don't fiddle with accel-groups, so we need a way
		// to pass it to the sub-menus created upon this bar for their items to have
		// access to the accel-group
		g_object_set_data (G_OBJECT (d_gtkMenuBar), "accel-group", ag);
		gtk_widget_show (GTK_WIDGET (d_gtkMenuBar));
	#elif cocoa
	#elif motif
		if (win || theGuiTopMenuBar) {
			d_xmMenuBar = XmCreateMenuBar (d_widget, "menuBar", NULL, 0);
			XtVaSetValues (d_xmMenuBar, XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
			XtManageChild (d_xmMenuBar);
		} else {
			theGuiTopMenuBar = XmCreateMenuBar (NULL, "menuBar", NULL, 0);
			//XtManageChild (topBar);
		}
	#endif
}

bool structGuiWindow :: f_setDirty (bool dirty) {
	#if gtk
		(void) dirty;
		return false;
	#elif cocoa
		[d_cocoaWindow setDocumentEdited: dirty];
		return true;
	#elif win
		(void) dirty;
		return false;
	#elif mac
		SetWindowModified (d_xmShell -> nat.window.ptr, dirty);
		return true;
	#endif
}

/* End of file GuiWindow.cpp */
