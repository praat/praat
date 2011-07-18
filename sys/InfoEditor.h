#ifndef _InfoEditor_h_
#define _InfoEditor_h_
/* InfoEditor.h
 *
 * Copyright (C) 2004-2011 Paul Boersma
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

#include "TextEditor.h"

Thing_declare1cpp (InfoEditor);
struct structInfoEditor : public structTextEditor {
	// overridden methods:
		void v_destroy ();
		bool fileBased () { return false; }
		void clear ();
		bool v_scriptable () { return false; }
};
#define InfoEditor__methods(Klas) TextEditor__methods(Klas)
Thing_declare2cpp (InfoEditor, TextEditor);

/* End of file InfoEditor.h */
#endif
