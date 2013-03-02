#ifndef _TextEditor_h_
#define _TextEditor_h_
/* TextEditor.h
 *
 * Copyright (C) 1997-2011,2012,2013 Paul Boersma
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

#include "Editor.h"

Thing_define (TextEditor, Editor) {
	// new data:
	public:
		structMelderFile file;
		GuiText textWidget;
		UiForm openDialog, saveDialog, printDialog, findDialog;
		int dirty;
		GuiDialog dirtyNewDialog, dirtyOpenDialog, dirtyCloseDialog;
		GuiMenuItem fontSizeButton_10, fontSizeButton_12, fontSizeButton_14, fontSizeButton_18, fontSizeButton_24;
	// functions:
		void init (const wchar_t *initialText);
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_nameChanged ();
		virtual void v_goAway ();
		virtual void v_createChildren ();
		virtual void v_createMenus ();
		virtual bool v_hasQueryMenu () { return false; }
	// new methods:
		virtual bool v_fileBased () { return true; };   // if true, have New, Open..., Save; if false, have Clear
		virtual void v_clear () { }
	// preferences:
		#include "TextEditor_prefs.h"
};

TextEditor TextEditor_create (
	const wchar_t *initialText   // may be NULL
);

void TextEditor_showOpen (TextEditor me);

/* End of file TextEditor.h */
#endif
