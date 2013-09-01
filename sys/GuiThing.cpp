/* GuiThing.cpp
 *
 * Copyright (C) 1993-2012,2013 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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
#include "machine.h"

void structGuiThing :: v_destroy () {
	GuiThing_Parent :: v_destroy ();
}

void structGuiThing :: f_hide () {
	v_hide ();
}

void structGuiThing :: v_hide () {
	#if gtk
		GuiObject parent = gtk_widget_get_parent (GTK_WIDGET (d_widget));
		if (parent != NULL && GTK_IS_DIALOG (parent)) {   // I am the top GtkFixed of a dialog
			gtk_widget_hide (GTK_WIDGET (parent));
		} else if (parent != NULL && GTK_IS_DIALOG (gtk_widget_get_parent (GTK_WIDGET (parent)))) {
			trace ("hiding a dialog indirectly");
			gtk_widget_hide (GTK_WIDGET (gtk_widget_get_parent (GTK_WIDGET (parent))));
		} else {
			gtk_widget_hide (GTK_WIDGET (d_widget));
		}
	#elif cocoa
		if ([(NSObject *) d_widget isKindOfClass: [NSWindow class]]) {
			[(NSWindow *) d_widget orderOut: nil];
		} else if ([(NSObject *) d_widget isKindOfClass: [NSView class]]) {
			if ((NSView *) d_widget == [[(NSView *) d_widget window] contentView]) {
				[[(NSView *) d_widget window] orderOut: nil];
			} else {
				[(NSView *) d_widget setHidden: YES];
			}
		} else {
			[(NSMenuItem *) d_widget setHidden: YES];
		}
	#elif win
		XtUnmanageChild (d_widget);
		// nothing, because the scrolled window is not a widget
	#elif mac
		XtUnmanageChild (d_widget);
		if (d_widget -> widgetClass == xmListWidgetClass) {
			XtUnmanageChild (d_widget -> parent);   // the containing scrolled window; BUG if created with XmScrolledList?
		}
	#endif
}

void structGuiThing :: f_setSensitive (bool sensitive) {
	v_setSensitive (sensitive);
}

void structGuiThing :: v_setSensitive (bool sensitive) {
	#if gtk
		gtk_widget_set_sensitive (GTK_WIDGET (d_widget), sensitive);
	#elif cocoa
	#elif motif
		XtSetSensitive (d_widget, sensitive);
	#endif
}

void structGuiThing :: f_show () {
	v_show ();
}

void structGuiThing :: v_show () {
	#if gtk
		trace ("showing widget %p", d_widget);
		GuiObject parent = gtk_widget_get_parent (GTK_WIDGET (d_widget));
		trace ("the parent widget is %p", parent);
		if (GTK_IS_WINDOW (parent)) {
			// I am a window's GtkFixed
			trace ("showing a window");
			gtk_widget_show (GTK_WIDGET (d_widget));
			gtk_window_present (GTK_WINDOW (parent));
		} else if (GTK_IS_DIALOG (parent)) {
			// I am a dialog's GtkFixed, and therefore automatically shown
			trace ("showing a dialog");
			gtk_window_present (GTK_WINDOW (parent));
		} else if (GTK_IS_DIALOG (gtk_widget_get_parent (GTK_WIDGET (parent)))) {
			// I am a dialog's GtkFixed, and therefore automatically shown
			trace ("showing a dialog (indirectly)");
			gtk_window_present (GTK_WINDOW (gtk_widget_get_parent (GTK_WIDGET (parent))));
		} else {
			trace ("showing a widget that is not a window or dialog");
			gtk_widget_show (GTK_WIDGET (d_widget));
		}
	#elif cocoa
		if ([(NSObject *) d_widget isKindOfClass: [NSWindow class]]) {
			trace ("trying to show a window");
			[(NSWindow *) d_widget makeKeyAndOrderFront: nil];
		} else if ([(NSObject *) d_widget isKindOfClass: [NSView class]]) {
			if ((NSView *) d_widget == [[(NSView *) d_widget window] contentView]) {
				trace ("trying to show a window through its content view");
				[[(NSView *) d_widget window] makeKeyAndOrderFront: nil];
			} else {
				[(NSView *) d_widget setHidden: NO];
			}
		} else {
			[(NSMenuItem *) d_widget setHidden: NO];
		}
	#elif motif
		XtManageChild (d_widget);
		GuiObject parent = d_widget -> parent;
		if (parent -> widgetClass == xmShellWidgetClass) {
			XMapRaised (XtDisplay (parent), XtWindow (parent));
		} else if (mac && d_widget -> widgetClass == xmListWidgetClass) {
			XtManageChild (parent);   // the containing scrolled window; BUG if created with XmScrolledList?
		}
	#endif
	trace ("end");
}

/* End of file GuiThing.cpp */
