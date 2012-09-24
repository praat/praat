#ifndef _ScriptEditor_h_
#define _ScriptEditor_h_
/* ScriptEditor.h
 *
 * Copyright (C) 1997-2011,2012 Paul Boersma
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
	// new data:
	public:
		wchar_t *environmentName;
		ClassInfo editorClass;
		Interpreter interpreter;
		UiForm argsDialog;
	// functions:
		void init (Editor editor, const wchar_t *initialText);
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_nameChanged ();
		virtual void v_goAway ();
		virtual bool v_scriptable () { return false; }
		virtual void v_createMenus ();
		virtual void v_createHelpMenuItems (EditorMenu menu);
};

ScriptEditor ScriptEditor_createFromText (
	Editor editor,   // the scripting environment; if NULL, the scripting environment consists of the global windows
	const wchar_t *initialText   // may be NULL
);

ScriptEditor ScriptEditor_createFromScript (
	Editor editor,
	Script script
);

int ScriptEditors_dirty (void);   // are there any modified and unsaved scripts? Ask before quitting the program.

/* End of file ScriptEditor.h */
#endif
