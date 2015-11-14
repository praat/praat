#ifndef _ScriptEditor_h_
#define _ScriptEditor_h_
/* ScriptEditor.h
 *
 * Copyright (C) 1997-2011,2012,2015 Paul Boersma
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

#include "Script.h"
#include "TextEditor.h"
#include "Interpreter.h"

Thing_define (ScriptEditor, TextEditor) {
	char32 *environmentName;
	ClassInfo editorClass;
	Interpreter interpreter;
	autoUiForm argsDialog;

	void v_destroy ()
		override;
	void v_nameChanged ()
		override;
	void v_goAway ()
		override;
	bool v_scriptable ()
		override { return false; }
	void v_createMenus ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;
};

void ScriptEditor_init (ScriptEditor me,
	Editor editor,
	const char32 *initialText
);
ScriptEditor ScriptEditor_createFromText (
	Editor editor,   // the scripting environment; if null, the scripting environment consists of the global windows
	const char32 *initialText   // may be null
);

ScriptEditor ScriptEditor_createFromScript (
	Editor editor,
	Script script
);

bool ScriptEditors_dirty ();   // are there any modified and unsaved scripts? Ask before quitting the program.

/* End of file ScriptEditor.h */
#endif
