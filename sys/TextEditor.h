#ifndef _TextEditor_h_
#define _TextEditor_h_
/* TextEditor.h
 *
 * Copyright (C) 1997-2005 Paul Boersma
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
 * pb 2005/06/28
 */

#ifndef _Editor_h_
	#include "Editor.h"
#endif

#define TextEditor_members Editor_members \
	structMelderFile file; \
	Widget textWidget; \
	Any openDialog, saveDialog, printDialog, findDialog; \
	int dirty, fontSize; \
	Widget dirtyNewDialog, dirtyOpenDialog, dirtyCloseDialog; \
	Widget fontSizeButton_10, fontSizeButton_12, fontSizeButton_14, fontSizeButton_18, fontSizeButton_24;
#define TextEditor_methods Editor_methods \
	int fileBased; \
	void (*clear) (I);
class_create (TextEditor, Editor)

int TextEditor_init (I, Widget parent, const char *initialText);
TextEditor TextEditor_create (Widget parent, const char *initialText);
	/* 'initalText' may be NULL. */
void TextEditor_showOpen (I);

void TextEditor_prefs (void);

/* End of file TextEditor.h */
#endif
