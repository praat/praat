/* GuiDialog.cpp
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
 * pb 2007/12/30
 * fb 2010/02/23 gtk
 * pb 2010/05/29 repaired memory leak; made dialog front on show
 * pb 2010/07/29 removed GuiDialog_show
 * pb 2011/04/06 C++
 */

#include "GuiP.h"

Thing_implement (GuiDialog, GuiShell, 0);

#if gtk
	static void _GuiGtkDialog_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiDialog);
		forget (me);
	}
	static gboolean _GuiGtkDialog_goAwayCallback (GuiObject widget, GdkEvent *event, gpointer void_me) {
		(void) widget;
		(void) event;
		iam (GuiDialog);
		if (my d_goAwayCallback != NULL) {
			my d_goAwayCallback (my d_goAwayBoss);
		}
		return TRUE;   // signal handled (don't destroy dialog)
	}
#elif cocoa
	@interface GuiCocoaDialog : NSWindow
	@end
	@implementation GuiCocoaDialog {
		GuiDialog d_userData;
	}
	- (void) dealloc {   // override
		GuiDialog me = d_userData;
		forget (me);
		Melder_casual ("deleting a dialog");
		[super dealloc];
	}
	- (GuiDialog) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiDialog) userData {
		d_userData = userData;
	}
	@end
#elif motif
	static void _GuiMotifDialog_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiDialog);
		forget (me);
	}
	static void _GuiMotifDialog_goAwayCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiDialog);
		if (my d_goAwayCallback != NULL) {
			my d_goAwayCallback (my d_goAwayBoss);
		}
	}
#endif

GuiDialog GuiDialog_create (GuiWindow parent, int x, int y, int width, int height,
	const wchar_t *title, void (*goAwayCallback) (void *goAwayBoss), void *goAwayBoss, unsigned long flags)
{
	GuiDialog me = Thing_new (GuiDialog);
	my d_parent = parent;
	my d_goAwayCallback = goAwayCallback;
	my d_goAwayBoss = goAwayBoss;
	#if gtk
		my d_gtkWindow = (GtkWindow *) gtk_dialog_new ();
		if (parent) {
			Melder_assert (parent -> d_widget);
			GuiObject toplevel = gtk_widget_get_ancestor (GTK_WIDGET (parent -> d_widget), GTK_TYPE_WINDOW);
			if (toplevel) {
				gtk_window_set_transient_for (GTK_WINDOW (my d_gtkWindow), GTK_WINDOW (toplevel));
				gtk_window_set_destroy_with_parent (GTK_WINDOW (my d_gtkWindow), TRUE);
			}
		}
		g_signal_connect (G_OBJECT (my d_gtkWindow), "delete-event",
			goAwayCallback ? G_CALLBACK (_GuiGtkDialog_goAwayCallback) : G_CALLBACK (gtk_widget_hide_on_delete), me);
		gtk_window_set_default_size (GTK_WINDOW (my d_gtkWindow), width, height);
		gtk_window_set_modal (GTK_WINDOW (my d_gtkWindow), flags & GuiDialog_MODAL);
		my f_setTitle (title);
		GuiObject vbox = GTK_DIALOG (my d_gtkWindow) -> vbox;
		my d_widget = gtk_fixed_new ();
		_GuiObject_setUserData (my d_widget, me);
		gtk_widget_set_size_request (GTK_WIDGET (my d_widget), width, height);
		gtk_container_add (GTK_CONTAINER (vbox /*my d_gtkWindow*/), GTK_WIDGET (my d_widget));
		gtk_widget_show (GTK_WIDGET (my d_widget));
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkDialog_destroyCallback), me);
	#elif cocoa
		(void) parent;
		NSRect rect = { { x, y }, { width, height } };
		NSWindow *nsWindow = [[GuiCocoaDialog alloc]
			initWithContentRect: rect
			styleMask: NSTitledWindowMask | NSClosableWindowMask
			backing: NSBackingStoreBuffered
			defer: false];
        [nsWindow setMinSize: NSMakeSize (500.0, 500.0)];   // BUG: should not be needed
		[nsWindow setTitle: (NSString *) Melder_peekWcsToCfstring (title)];
		//[nsWindow makeKeyAndOrderFront: nil];
		my d_widget = (GuiObject) [nsWindow contentView];
		[(GuiCocoaDialog *) nsWindow setUserData: me];
		[nsWindow setReleasedWhenClosed: NO];
	#elif motif
		my d_xmShell = XmCreateDialogShell (mac ? NULL : parent -> d_widget, "dialogShell", NULL, 0);
		XtVaSetValues (my d_xmShell, XmNdeleteResponse, goAwayCallback ? XmDO_NOTHING : XmUNMAP, XmNx, x, XmNy, y, NULL);
		if (goAwayCallback) {
			XmAddWMProtocolCallback (my d_xmShell, 'delw', _GuiMotifDialog_goAwayCallback, (char *) me);
		}
		my f_setTitle (title);
		my d_widget = XmCreateForm (my d_xmShell, "dialog", NULL, 0);
		XtVaSetValues (my d_widget, XmNwidth, (Dimension) width, XmNheight, (Dimension) height, NULL);
		_GuiObject_setUserData (my d_widget, me);
		XtAddCallback (my d_widget, XmNdestroyCallback, _GuiMotifDialog_destroyCallback, me);
		XtVaSetValues (my d_widget, XmNdialogStyle,
			(flags & GuiDialog_MODAL) ? XmDIALOG_FULL_APPLICATION_MODAL : XmDIALOG_MODELESS,
			XmNautoUnmanage, False, NULL);
	#endif
	my d_shell = me;
	return me;
}

/* End of file GuiDialog.cpp */
