#ifndef _Manual_h_
#define _Manual_h_
/* Manual.h
 *
 * Copyright (C) 1996-2011 Paul Boersma
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
 * pb 2011/07/15
 */

#include "HyperPage.h"
#include "ManPages.h"

Thing_declare1cpp (Manual);
struct structManual : public structHyperPage {
	// data:
		long path, numberOfParagraphs;
		struct structManPage_Paragraph *paragraphs;
		GuiObject searchText;
		GuiObject homeButton, recordButton, playButton, publishButton;
		int numberOfMatches;
		long matches [1 + 20], fromPage, toPage;
		int suppressLinksHither;
		wchar *printPagesStartingWith;
	// overridden methods:
		void v_destroy ();
		bool v_scriptable () { return false; }
		void v_createChildren ();
		void v_createMenus ();
		bool v_hasQueryMenu () { return false; }
		void v_createHelpMenuItems (EditorMenu menu);
};
#define Manual__methods(Klas) HyperPage__methods(Klas)
Thing_declare2cpp (Manual, HyperPage);

void Manual_init (Manual me, GuiObject parent, const wchar *title, Data data);
Manual Manual_create (GuiObject parent, const wchar *title, Data data);

void Manual_search (Manual me, const wchar *query);

/* End of file Manual.h */
#endif

