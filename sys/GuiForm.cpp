/* GuiForm.cpp
 *
 * Copyright (C) 1993-2012 Paul Boersma
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

Thing_implement (GuiForm, GuiControl, 0);

#if gtk
	static void _guiGtkForm_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiForm);
		trace ("destroying GuiForm %p", me);
		forget (me);
	}
#elif cocoa
#elif motif
	static void _guiMotifForm_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiForm);
		forget (me);
	}
#endif

GuiForm GuiForm_createInScrolledWindow (GuiScrolledWindow parent)
{
	GuiForm me = Thing_new (GuiForm);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		my d_widget = gtk_fixed_new ();
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (parent -> d_widget), GTK_WIDGET (my d_widget));
	#elif cocoa
	#elif motif
		//my d_widget = XmCreateRowColumn (parent -> d_widget, "menu", NULL, 0);
		my d_widget = XmCreateForm (parent -> d_widget, "menu", NULL, 0);
	#endif
	my f_show ();

	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkForm_destroyCallback), me);
	#elif cocoa
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifForm_destroyCallback, me);
	#endif

	return me;
}

/* End of file GuiForm.cpp */
