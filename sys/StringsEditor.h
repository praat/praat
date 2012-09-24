#ifndef _StringsEditor_h_
#define _StringsEditor_h_
/* StringsEditor.h
 *
 * Copyright (C) 1993-2011 David Weenink & Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Editor.h"
#include "Strings_.h"

Thing_define (StringsEditor, Editor) {
	// new data:
	public:
		GuiList list;
		GuiText text;
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_createChildren ();
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual void v_dataChanged ();
};

StringsEditor StringsEditor_create (const wchar_t *title, Strings data);

/* End of file StringsEditor.h */
#endif
