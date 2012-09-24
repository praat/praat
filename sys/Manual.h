#ifndef _Manual_h_
#define _Manual_h_
/* Manual.h
 *
 * Copyright (C) 1996-2012 Paul Boersma
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
#include "ManPages.h"

Thing_define (Manual, HyperPage) {
	// new data:
	public:
		long path, numberOfParagraphs;
		struct structManPage_Paragraph *paragraphs;
		GuiText searchText;
		GuiButton homeButton, recordButton, playButton, publishButton;
		int numberOfMatches;
		long matches [1 + 20], fromPage, toPage;
		int suppressLinksHither;
		wchar_t *printPagesStartingWith;
	// overridden methods:
		virtual bool v_scriptable () { return false; }
		virtual void v_createChildren ();
		virtual void v_createMenus ();
		virtual bool v_hasQueryMenu () { return false; }
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual void v_draw ();
		virtual void v_defaultHeaders (EditorCommand cmd);
		virtual long v_getNumberOfPages ();
		virtual long v_getCurrentPageNumber ();
		virtual int v_goToPage (const wchar_t *title);
		virtual void v_goToPage_i (long pageNumber);
		virtual bool v_hasHistory () { return true; }
		virtual bool v_isOrdered () { return true; }
};

void Manual_init (Manual me, const wchar_t *title, Data data, bool ownData);
Manual Manual_create (const wchar_t *title, Data data, bool ownData);

void Manual_search (Manual me, const wchar_t *query);

/* End of file Manual.h */
#endif

