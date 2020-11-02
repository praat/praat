/* GuiDialog.cpp
 *
 * Copyright (C) 1993-2012,2013,2015,2017 Paul Boersma, 2013 Tom Naughton
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
		if (my d_goAwayCallback) {
			my d_goAwayCallback (my d_goAwayBoss);
		}
		return true;   // signal handled (don't destroy dialog)
	}
#elif motif
	static void _GuiMotifDialog_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiDialog);
		forget (me);
	}
	static void _GuiMotifDialog_goAwayCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiDialog);
		if (my d_goAwayCallback) {
			my d_goAwayCallback (my d_goAwayBoss);
		}
	}
#endif

GuiDialog GuiDialog_create (GuiWindow parent, int x, int y, int width, int height,
	conststring32 title, GuiShell_GoAwayCallback goAwayCallback, Thing goAwayBoss, uint32 flags)
{
	autoGuiDialog me = Thing_new (GuiDialog);
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
				gtk_window_set_destroy_with_parent (GTK_WINDOW (my d_gtkWindow), true);
			}
		}
		g_signal_connect (G_OBJECT (my d_gtkWindow), "delete-event",
			goAwayCallback ? G_CALLBACK (_GuiGtkDialog_goAwayCallback) : G_CALLBACK (gtk_widget_hide_on_delete), me.get());
		gtk_window_set_default_size (GTK_WINDOW (my d_gtkWindow), width, height);
		gtk_window_set_modal (GTK_WINDOW (my d_gtkWindow), flags & GuiDialog_MODAL);
		gtk_window_set_resizable (GTK_WINDOW (my d_gtkWindow), false);
		GuiShell_setTitle (me.get(), title);
		//GuiObject vbox = GTK_DIALOG (my d_gtkWindow) -> vbox;
		GuiObject vbox = gtk_dialog_get_content_area (GTK_DIALOG (my d_gtkWindow));
		my d_widget = gtk_fixed_new ();
		_GuiObject_setUserData (my d_widget, me.get());
		gtk_widget_set_size_request (GTK_WIDGET (my d_widget), width, height);
		gtk_container_add (GTK_CONTAINER (vbox /*my d_gtkWindow*/), GTK_WIDGET (my d_widget));
		gtk_widget_show (GTK_WIDGET (my d_widget));
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkDialog_destroyCallback), me.get());
	#elif motif
		my d_xmShell = XmCreateDialogShell (parent -> d_widget, "dialogShell", nullptr, 0);
		XtVaSetValues (my d_xmShell, XmNdeleteResponse, goAwayCallback ? XmDO_NOTHING : XmUNMAP, XmNx, x, XmNy, y, nullptr);
		if (goAwayCallback) {
			XmAddWMProtocolCallback (my d_xmShell, 'delw', _GuiMotifDialog_goAwayCallback, (char *) me.get());
		}
		GuiShell_setTitle (me.get(), title);
		my d_widget = XmCreateForm (my d_xmShell, "dialog", nullptr, 0);
		XtVaSetValues (my d_widget, XmNwidth, (Dimension) width, XmNheight, (Dimension) height, nullptr);
		_GuiObject_setUserData (my d_widget, me.get());
		XtAddCallback (my d_widget, XmNdestroyCallback, _GuiMotifDialog_destroyCallback, me.get());
		XtVaSetValues (my d_widget, XmNdialogStyle,
			(flags & GuiDialog_MODAL) ? XmDIALOG_FULL_APPLICATION_MODAL : XmDIALOG_MODELESS,
			XmNautoUnmanage, False, nullptr);
	#elif cocoa
		(void) parent;
		NSRect rect = { { (CGFloat) x, (CGFloat) y }, { (CGFloat) width, (CGFloat) height } };
		my d_cocoaShell = [[GuiCocoaShell alloc]
			initWithContentRect: rect
			styleMask: NSTitledWindowMask | NSClosableWindowMask
			backing: NSBackingStoreBuffered
			defer: false];
        [my d_cocoaShell   setMinSize: NSMakeSize (500.0, 500.0)];   // BUG: should not be needed
		[my d_cocoaShell   setTitle: (NSString *) Melder_peek32toCfstring (title)];
		//[my d_cocoaShell   makeKeyAndOrderFront: nil];
		my d_widget = (GuiObject) [my d_cocoaShell   contentView];
		[my d_cocoaShell   setUserData: me.get()];
		[my d_cocoaShell   setReleasedWhenClosed: NO];
	#endif
	my d_shell = me.get();
	return me.releaseToAmbiguousOwner();
}

/* End of file GuiDialog.cpp */
