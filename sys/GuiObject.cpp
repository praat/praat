/* GuiObject.cpp
 *
 * Copyright (C) 1993-2012 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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

void * _GuiObject_getUserData (GuiObject me) {
	void *userData = NULL;
	#if gtk
		userData = (void *) g_object_get_data (G_OBJECT (me), "praat");
	#elif cocoa
	#elif motif
		XtVaGetValues (me, XmNuserData, & userData, NULL);
	#endif
	return userData;
}

void _GuiObject_setUserData (GuiObject me, void *userData) {
	#if gtk
		g_object_set_data (G_OBJECT (me), "praat", userData);
	#elif cocoa
	#elif motif
		XtVaSetValues (me, XmNuserData, userData, NULL);
	#endif
}

void GuiObject_destroy (GuiObject me) {
	#if gtk
		gtk_widget_destroy (GTK_WIDGET (me));
	#elif cocoa
	#elif motif
		XtDestroyWidget (me);
	#endif
}

/* End of file GuiObject.cpp */
