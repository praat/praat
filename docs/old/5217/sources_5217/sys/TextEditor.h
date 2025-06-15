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
 * pb 2011/03/02
 */

#ifndef _Editor_h_
	#include "Editor.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

#define TextEditor__parents(Klas) Editor__parents(Klas) Thing_inherit (Klas, Editor)
Thing_declare1 (TextEditor);

#define TextEditor__members(Klas) Editor__members(Klas) \
	structMelderFile file; \
	GuiObject textWidget; \
	Any openDialog, saveDialog, printDialog, findDialog; \
	int dirty, fontSize; \
	GuiObject dirtyNewDialog, dirtyOpenDialog, dirtyCloseDialog; \
	GuiObject fontSizeButton_10, fontSizeButton_12, fontSizeButton_14, fontSizeButton_18, fontSizeButton_24;
#define TextEditor__methods(Klas) Editor__methods(Klas) \
	bool fileBased; \
	void (*clear) (Klas me);
Thing_declare2 (TextEditor, Editor);

int TextEditor_init (TextEditor me, GuiObject parent, const wchar_t *initialText);
TextEditor TextEditor_create (GuiObject parent, const wchar_t *initialText);
	/* 'initalText' may be NULL. */
void TextEditor_showOpen (TextEditor me);

void TextEditor_prefs (void);

#ifdef __cplusplus
	}
#endif

/* End of file TextEditor.h */
#endif
