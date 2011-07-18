#ifndef _OTMultiEditor_h_
#define _OTMultiEditor_h_
/* OTMultiEditor.h
 *
 * Copyright (C) 2005-2011 Paul Boersma
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

#include "HyperPage.h"
#include "OTMulti.h"

Thing_declare1cpp (OTMultiEditor);
struct structOTMultiEditor : public structHyperPage {
	// new data:
		const wchar *form1, *form2;
		GuiObject form1Text, form2Text;
		long selectedConstraint;
	// overridden methods:
		bool v_editable () { return true; }
		void v_createChildren ();
		void v_createMenus ();
		void v_createHelpMenuItems (EditorMenu menu);
};
#define OTMultiEditor__methods(Klas) HyperPage__methods(Klas)
Thing_declare2cpp (OTMultiEditor, HyperPage);

OTMultiEditor OTMultiEditor_create (GuiObject parent, const wchar *title, OTMulti grammar);

/* End of file OTMultiEditor.h */
#endif
