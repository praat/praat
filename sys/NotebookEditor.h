#ifndef _NotebookEditor_h_
#define _NotebookEditor_h_
/* NotebookEditor.h
 *
 * Copyright (C) 2023 Paul Boersma
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

#include "ManPages.h"
#include "Notebook.h"
#include "TextEditor.h"
#include "Interpreter.h"

Thing_define (NotebookEditor, TextEditor) {
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
		override { return U".praatnb"; }
};

void NotebookEditor_init (NotebookEditor me,
	conststring32 initialText
);
autoNotebookEditor NotebookEditor_createFromText (
	conststring32 initialText   // may be null
);

autoNotebookEditor NotebookEditor_createFromNotebook_canBeNull (
	Notebook notebook
);

bool NotebookEditors_dirty ();   // are there any modified and unsaved notebooks? Ask before quitting the program.

void NotebookEditor_debug_printAllOpenNotebookEditors ();

extern CollectionOf <structNotebookEditor> theReferencesToAllOpenNotebookEditors;

/* End of file NotebookEditor.h */
#endif
