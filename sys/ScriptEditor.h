#ifndef _ScriptEditor_h_
#define _ScriptEditor_h_
/* ScriptEditor.h
 *
 * Copyright (C) 1997-2011,2012,2015,2016,2018,2022,2023 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Script.h"
#include "TextEditor.h"
#include "Interpreter.h"

Thing_define (ScriptEditor, TextEditor) {
	/*
		Who owns me?
	*/
	autostring32 optionalOwningEditorClassName;   // set at creation, and never changed after that (last checked 2023-02-25)
	Editor optionalReferenceToOwningEditor;   // set at creation, and may be set to null later on
	bool wasCreatedInAnEditor () const noexcept {
		return !! our optionalOwningEditorClassName;
	}

	autoInterpreter interpreter;
	autoUiForm argsDialog;

	void v9_destroy () noexcept
		override;
	void v_nameChanged ()
		override;
	void v_goAway ()
		override;
	bool v_scriptable ()
		override { return false; }
	void v_createMenus ()
		override;
	void v_createMenuItems_help (EditorMenu menu)
		override;
	conststring32 v_extension () const
		override { return U".praat"; }
};

void ScriptEditor_init (ScriptEditor me,
	Editor optionalEditor,
	conststring32 initialText
);
autoScriptEditor ScriptEditor_createFromText (
	Editor optionalEditor,   // the scripting environment; if null, the scripting environment consists of the global windows
	conststring32 initialText   // may be null
);

autoScriptEditor ScriptEditor_createFromScript_canBeNull (
	Editor optionalEditor,
	autoScript script   // the ScriptEditor will take over ownership, and in turn hand it over to the set of known scripts
);

bool ScriptEditors_dirty ();   // are there any modified and unsaved scripts? Ask before quitting the program.

void ScriptEditor_debug_printAllOpenScriptEditors ();

extern CollectionOf <structScriptEditor> theReferencesToAllOpenScriptEditors;

/* End of file ScriptEditor.h */
#endif
