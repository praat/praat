/* GuiForm.cpp
 *
 * Copyright (C) 1993-2012,2015,2017 Paul Boersma
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

Thing_implement (GuiForm, GuiControl, 0);

#if gtk
	static void _guiGtkForm_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiForm);
		trace (U"destroying GuiForm ", Melder_pointer (me));
		forget (me);
	}
#elif motif
	static void _guiMotifForm_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiForm);
		forget (me);
	}
#elif cocoa
#endif

GuiForm GuiForm_createInScrolledWindow (GuiScrolledWindow parent)
{
	autoGuiForm me = Thing_new (GuiForm);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		my d_widget = gtk_fixed_new ();
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (parent -> d_widget), GTK_WIDGET (my d_widget));
	#elif motif
		//my d_widget = XmCreateRowColumn (parent -> d_widget, "menu", nullptr, 0);
		my d_widget = XmCreateForm (parent -> d_widget, "menu", nullptr, 0);
	#elif cocoa
	#endif
	GuiThing_show (me.get());

	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkForm_destroyCallback), me.get());
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifForm_destroyCallback, me.get());
	#elif cocoa
	#endif

	return me.releaseToAmbiguousOwner();
}

/* End of file GuiForm.cpp */
