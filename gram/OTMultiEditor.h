#ifndef _OTMultiEditor_h_
#define _OTMultiEditor_h_
/* OTMultiEditor.h
 *
 * Copyright (C) 2005-2011,2012 Paul Boersma
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

Thing_define (OTMultiEditor, HyperPage) {
	// new data:
	public:
		const wchar_t *form1, *form2;
		GuiText form1Text, form2Text;
		long selectedConstraint;
		bool d_constraintsAreDrawnVertically;
	// overridden methods:
		virtual bool v_editable () { return true; }
		virtual void v_createChildren ();
		virtual void v_createMenus ();
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual void v_draw ();
		virtual int v_goToPage (const wchar_t *title);
};

OTMultiEditor OTMultiEditor_create (const wchar_t *title, OTMulti grammar);

/* End of file OTMultiEditor.h */
#endif
