#ifndef _StringsEditor_h_
#define _StringsEditor_h_
/* StringsEditor.h
 *
 * Copyright (C) 1993-2011,2015 David Weenink & Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Editor.h"
#include "Strings_.h"

Thing_define (StringsEditor, Editor) {
	GuiList list;
	GuiText text;

	void v_destroy () noexcept
		override;
	void v_createChildren ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_dataChanged ()
		override;
};

autoStringsEditor StringsEditor_create (conststring32 title, Strings data);

/* End of file StringsEditor.h */
#endif
