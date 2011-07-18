#ifndef _OTGrammarEditor_h_
#define _OTGrammarEditor_h_
/* OTGrammar.h
 *
 * Copyright (C) 1997-2011 Paul Boersma
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
#include "OTGrammar.h"

Thing_declare1cpp (OTGrammarEditor);
struct structOTGrammarEditor : public structHyperPage {
	// new data:
		long selected;
	// overridden methods:
		bool v_editable () { return true; }
		void v_createMenus ();
		void v_createHelpMenuItems (EditorMenu menu);
};
#define OTGrammarEditor__methods(Klas) HyperPage__methods(Klas)
Thing_declare2cpp (OTGrammarEditor, HyperPage);

OTGrammarEditor OTGrammarEditor_create (GuiObject parent, const wchar *title, OTGrammar ot);

/* End of file OTGrammarEditor.h */
#endif
