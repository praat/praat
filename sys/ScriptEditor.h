#ifndef _ScriptEditor_h_
#define _ScriptEditor_h_
/* ScriptEditor.h
 *
 * Copyright (C) 1997-2002 Paul Boersma
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
 * pb 2000/09/21
 * pb 2002/03/07 GPL
 */

#ifndef _Script_h_
	#include "Script.h"
#endif
#ifndef _TextEditor_h_
	#include "TextEditor.h"
#endif
#ifndef _Interpreter_h_
	#include "Interpreter.h"
#endif

#define ScriptEditor_members TextEditor_members \
	char *environmentName; \
	Editor_Table editorClass; \
	Interpreter interpreter; \
	Any argsDialog;
#define ScriptEditor_methods TextEditor_methods
class_create (ScriptEditor, TextEditor)

ScriptEditor ScriptEditor_createFromText (Widget parent, Any editor, const char *initialText);
	/* 'initalText' may be NULL. */
ScriptEditor ScriptEditor_createFromScript (Widget parent, Any voidEditor, Script script);

int ScriptEditors_dirty (void);   /* Are there any modified and unsaved scripts? Ask before quitting the program. */

/* End of file ScriptEditor.h */
#endif
