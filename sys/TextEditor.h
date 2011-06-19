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
 * pb 2011/06/16
 */

#include "Editor.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (TextEditor);

#ifdef __cplusplus
	struct structTextEditor : public structEditor {
		structMelderFile file;
		GuiObject textWidget;
		Any openDialog, saveDialog, printDialog, findDialog;
		int dirty, fontSize;
		GuiObject dirtyNewDialog, dirtyOpenDialog, dirtyCloseDialog;
		GuiObject fontSizeButton_10, fontSizeButton_12, fontSizeButton_14, fontSizeButton_18, fontSizeButton_24;
	};
#else
	#define TextEditor__members(Klas) Editor__members(Klas) \
		structMelderFile file; \
		GuiObject textWidget; \
		Any openDialog, saveDialog, printDialog, findDialog; \
		int dirty, fontSize; \
		GuiObject dirtyNewDialog, dirtyOpenDialog, dirtyCloseDialog; \
		GuiObject fontSizeButton_10, fontSizeButton_12, fontSizeButton_14, fontSizeButton_18, fontSizeButton_24;
#endif
#define TextEditor__methods(Klas) Editor__methods(Klas) \
	bool fileBased; \
	void (*clear) (Klas me);
Thing_declare2cpp (TextEditor, Editor);

void TextEditor_init (TextEditor me, GuiObject parent, const wchar_t *initialText);
TextEditor TextEditor_create (GuiObject parent, const wchar_t *initialText);
	/* 'initalText' may be NULL. */
void TextEditor_showOpen (TextEditor me);

void TextEditor_prefs (void);

#ifdef __cplusplus
	}
#endif

/* End of file TextEditor.h */
#endif
