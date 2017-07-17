/* GuiThing.cpp
 *
 * Copyright (C) 1993-2012,2013,2015,2017 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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
#include "machine.h"

void structGuiThing :: v_destroy () noexcept {
	GuiThing_Parent :: v_destroy ();
}

void structGuiThing :: v_hide () {
	#if gtk
		GuiObject parent = gtk_widget_get_parent (GTK_WIDGET (d_widget));
		if (parent && GTK_IS_DIALOG (parent)) {   // I am the top GtkFixed of a dialog
			gtk_widget_hide (GTK_WIDGET (parent));
		} else if (parent && GTK_IS_DIALOG (gtk_widget_get_parent (GTK_WIDGET (parent)))) {
			trace (U"hiding a dialog indirectly");
			gtk_widget_hide (GTK_WIDGET (gtk_widget_get_parent (GTK_WIDGET (parent))));
		} else {
			gtk_widget_hide (GTK_WIDGET (d_widget));
		}
	#elif motif
		XtUnmanageChild (d_widget);
		// nothing, because the scrolled window is not a widget
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
	#endif
}

void structGuiThing :: v_setSensitive (bool sensitive) {
	#if gtk
		gtk_widget_set_sensitive (GTK_WIDGET (d_widget), sensitive);
	#elif motif
		XtSetSensitive (d_widget, sensitive);
	#elif cocoa
		if ([(NSObject *) d_widget isKindOfClass: [NSControl class]]) {
			[(NSControl *) d_widget setEnabled: sensitive];
		} else if ([(NSObject *) d_widget isKindOfClass: [NSMenuItem class]]) {
			[(NSMenuItem *) d_widget setEnabled: sensitive];
		}
	#endif
}

void structGuiThing :: v_show () {
	#if gtk
		trace (U"showing widget ", Melder_pointer (d_widget));
		GuiObject parent = gtk_widget_get_parent (GTK_WIDGET (d_widget));
		trace (U"the parent widget is ", Melder_pointer (parent));
		if (GTK_IS_WINDOW (parent)) {
			// I am a window's GtkFixed
			trace (U"showing a window");
			gtk_widget_show (GTK_WIDGET (d_widget));
			gtk_window_present (GTK_WINDOW (parent));
		} else if (GTK_IS_DIALOG (parent)) {
			// I am a dialog's GtkFixed, and therefore automatically shown
			trace (U"showing a dialog");
			gtk_window_present (GTK_WINDOW (parent));
		} else if (GTK_IS_DIALOG (gtk_widget_get_parent (GTK_WIDGET (parent)))) {
			// I am a dialog's GtkFixed, and therefore automatically shown
			trace (U"showing a dialog (indirectly)");
			gtk_window_present (GTK_WINDOW (gtk_widget_get_parent (GTK_WIDGET (parent))));
		} else {
			trace (U"showing a widget that is not a window or dialog");
			gtk_widget_show (GTK_WIDGET (d_widget));
		}
	#elif motif
		XtManageChild (d_widget);
		GuiObject parent = d_widget -> parent;
		if (parent -> widgetClass == xmShellWidgetClass) {
			XMapRaised (XtDisplay (parent), XtWindow (parent));
		}
	#elif cocoa
		if ([(NSObject *) d_widget isKindOfClass: [NSWindow class]]) {
			trace (U"trying to show a window");
			[(NSWindow *) d_widget makeKeyAndOrderFront: nil];
		} else if ([(NSObject *) d_widget isKindOfClass: [NSView class]]) {
			if ((NSView *) d_widget == [[(NSView *) d_widget window] contentView]) {
				trace (U"trying to show a window through its content view");
				[[(NSView *) d_widget window] makeKeyAndOrderFront: nil];
			} else {
				[(NSView *) d_widget setHidden: NO];
			}
		} else {
			[(NSMenuItem *) d_widget setHidden: NO];
		}
	#endif
	trace (U"end");
}

void GuiThing_hide (GuiThing me) {
	my v_hide ();
}

void GuiThing_setSensitive (GuiThing me, bool sensitive) {
	my v_setSensitive (sensitive);
}

void GuiThing_show (GuiThing me) {
	my v_show ();
}

/* End of file GuiThing.cpp */
