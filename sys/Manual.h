#ifndef _Manual_h_
#define _Manual_h_
/* Manual.h
 *
 * Copyright (C) 1996-2012,2015 Paul Boersma
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "HyperPage.h"
#include "ManPages.h"

Thing_define (Manual, HyperPage) {
	long path, numberOfParagraphs;
	struct structManPage_Paragraph *paragraphs;
	GuiText searchText;
	GuiButton homeButton, recordButton, playButton, publishButton;
	int numberOfMatches;
	long matches [1 + 20], fromPage, toPage;
	int suppressLinksHither;
	char32 *printPagesStartingWith;

	bool v_scriptable ()
		override { return false; }
	void v_createChildren ()
		override;
	void v_createMenus ()
		override;
	bool v_hasQueryMenu ()
		override { return false; }
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_draw ()
		override;
	void v_defaultHeaders (EditorCommand cmd)
		override;
	long v_getNumberOfPages ()
		override;
	long v_getCurrentPageNumber ()
		override;
	int v_goToPage (const char32 *title)
		override;
	void v_goToPage_i (long pageNumber)
		override;
	bool v_hasHistory ()
		override { return true; }
	bool v_isOrdered ()
		override { return true; }
};

void Manual_init (Manual me, const char32 *title, Daata data, bool ownData);
autoManual Manual_create (const char32 *title, Daata data, bool ownData);

void Manual_search (Manual me, const char32 *query);

/* End of file Manual.h */
#endif

