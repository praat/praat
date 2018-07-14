#ifndef _OTGrammarEditor_h_
#define _OTGrammarEditor_h_
/* OTGrammar.h
 *
 * Copyright (C) 1997-2005,2007,2009-2012,2015-2018 Paul Boersma
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

#include "HyperPage.h"
#include "OTGrammar.h"

Thing_define (OTGrammarEditor, HyperPage) {
	integer selected;
	bool d_constraintsAreDrawnVertically;

	bool v_editable ()
		override { return true; }
	void v_createMenus ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_draw ()
		override;
	int v_goToPage (conststring32 title)
		override;
};

autoOTGrammarEditor OTGrammarEditor_create (conststring32 title, OTGrammar ot);

/* End of file OTGrammarEditor.h */
#endif
