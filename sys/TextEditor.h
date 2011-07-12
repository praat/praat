#ifndef _TextEditor_h_
#define _TextEditor_h_
/* TextEditor.h
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

/*
 * pb 2011/07/03
 */

#include "Editor.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (TextEditor);

TextEditor TextEditor_create (
	GuiObject parent,
	const wchar *initialText   // may be NULL
);

void TextEditor_showOpen (TextEditor me);

void TextEditor_prefs (void);

#ifdef __cplusplus
	}

	struct structTextEditor : public structEditor {
		structMelderFile file;
		GuiObject textWidget;
		Any openDialog, saveDialog, printDialog, findDialog;
		int dirty, fontSize;
		GuiObject dirtyNewDialog, dirtyOpenDialog, dirtyCloseDialog;
		GuiObject fontSizeButton_10, fontSizeButton_12, fontSizeButton_14, fontSizeButton_18, fontSizeButton_24;
	// functions:
		void init (GuiObject parent, const wchar *initialText);
	// overridden methods:
		void goAway ();
	// methods:
		virtual bool fileBased () { return true; };   // if true, have New, Open..., Save; if false, have Clear
		virtual void clear () { }
	};
	#define TextEditor__methods(Klas) Editor__methods(Klas)
	Thing_declare2cpp (TextEditor, Editor);

#endif // __cplusplus

/* End of file TextEditor.h */
#endif
