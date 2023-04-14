/* NotebookEditor.cpp
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

#include "NotebookEditor.h"
#include "../kar/longchar.h"
#include "praatP.h"
#include "EditorM.h"

Thing_implement (NotebookEditor, TextEditor, 0);

CollectionOf <structNotebookEditor> theReferencesToAllOpenNotebookEditors;

bool NotebookEditors_dirty () {
	for (integer i = 1; i <= theReferencesToAllOpenNotebookEditors.size; i ++) {
		const NotebookEditor me = theReferencesToAllOpenNotebookEditors.at [i];
		if (my dirty)
			return true;
	}
	return false;
}

void structNotebookEditor :: v9_destroy () noexcept {
	theReferencesToAllOpenNotebookEditors. undangleItem (this);
	NotebookEditor_Parent :: v9_destroy ();
}

void structNotebookEditor :: v_nameChanged () {
	/*
		As TextEditor does, we totally ignore the name that our boss wants to give us.
		Instead, we compose the window title from three ingredients:

		(1) whether we are already associated with a file or not;
		(2) if so, the full file path;
		(3) whether our text has been modified (i.e. whether we are "dirty");

		(last checked 2023-03-25)
	*/
	const bool dirtinessAlreadyShown = GuiWindow_setDirty (our windowForm, our dirty);   // (3) on the Mac (last checked 2023-03-25)
	static MelderString buffer;
	MelderString_copy (& buffer, MelderFile_isNull (& our file) ? U"untitled notebook" : U"Notebook");   // (1)
	if (! MelderFile_isNull (& our file))
		MelderString_append (& buffer, U" ", MelderFile_messageName (& our file));   // (2)
	if (our dirty && ! dirtinessAlreadyShown)
		MelderString_append (& buffer, U" (modified)");   // (3) on Windows and Linux (last checked 2023-02-25)
	GuiShell_setTitle (windowForm, buffer.string);
}

void structNotebookEditor :: v_goAway () {
	if (our interpreter -> running)
		Melder_flushError (U"Cannot close the NotebookEditor while the notebook is running or paused. Please close or continue the pause or demo window.");
	else
		NotebookEditor_Parent :: v_goAway ();
}

static void menu_cb_run (NotebookEditor me, EDITOR_ARGS) {
	if (my interpreter -> running)
		Melder_throw (U"The notebook is already running (paused). Please close or continue the pause or demo window.");
	autostring32 text = GuiText_getString (my textWidget);
	if (! Melder_stringMatchesCriterion (text.get(), kMelder_string::STARTS_WITH, U"Praat notebook ", true))
		Melder_throw (U"A Praat notebook should start with the text “Praat notebook ”.");
	if (! MelderFile_isNull (& my file))
		MelderFile_setDefaultDir (& my file);
	Melder_includeIncludeFiles (& text);   // BUG: should do only inside code chunks
	autoPraatBackground background;
	if (! MelderFile_isNull (& my file))
		MelderFile_setDefaultDir (& my file);

	autoMelderReadText readText = MelderReadText_createFromText (text.move());
	autoManPages manPages = ManPages_createFromText (readText.get(), & my file);   // readText can release, because manPages duplicates (last checked 2023-03-25)
	ManPage firstPage = manPages -> pages.at [1];
	autoManual manual = Manual_create (firstPage -> title.get(), manPages.releaseToAmbiguousOwner(), true);
	manual.releaseToUser ();
}

static void menu_cb_runChunk (NotebookEditor me, EDITOR_ARGS) {
	if (my interpreter -> running)
		Melder_throw (U"The notebook is already running (paused). Please close or continue the pause or demo window.");
	autostring32 text = GuiText_getSelection (my textWidget);   // TODO: replace with chunk
	if (! text)
		Melder_throw (U"No text selected.");
	if (! MelderFile_isNull (& my file))
		MelderFile_setDefaultDir (& my file);
	Melder_includeIncludeFiles (& text);
	autoPraatBackground background;
	if (! MelderFile_isNull (& my file))
		MelderFile_setDefaultDir (& my file);
	Interpreter_run (my interpreter.get(), text.get(), true);
}

static void menu_cb_expandIncludeFiles (NotebookEditor me, EDITOR_ARGS) {
	autostring32 text = GuiText_getString (my textWidget);
	if (! MelderFile_isNull (& my file))
		MelderFile_setDefaultDir (& my file);
	Melder_includeIncludeFiles (& text);   // BUG: should do only inside code chunks
	GuiText_setString (my textWidget, text.get());
}

static void menu_cb_AboutNotebookEditor (NotebookEditor, EDITOR_ARGS) { Melder_help (U"NotebookEditor"); }
static void menu_cb_ScriptingTutorial (NotebookEditor, EDITOR_ARGS) { Melder_help (U"Scripting"); }
static void menu_cb_ScriptingExamples (NotebookEditor, EDITOR_ARGS) { Melder_help (U"Scripting examples"); }
static void menu_cb_PraatScript (NotebookEditor, EDITOR_ARGS) { Melder_help (U"Praat script"); }
static void menu_cb_FormulasTutorial (NotebookEditor, EDITOR_ARGS) { Melder_help (U"Formulas"); }
static void menu_cb_Functions (NotebookEditor, EDITOR_ARGS) { Melder_help (U"Functions"); }

void structNotebookEditor :: v_createMenus () {
	NotebookEditor_Parent :: v_createMenus ();
	Editor_addCommand (this, U"File", U"-- close --", 0, nullptr);
	Editor_addCommand (this, U"Convert", U"-- expand --", 0, nullptr);
	Editor_addCommand (this, U"Convert", U"Expand include files", 0, menu_cb_expandIncludeFiles);
	Editor_addMenu (this, U"Run", 0);
	Editor_addCommand (this, U"Run", U"Run", 'R', menu_cb_run);
	Editor_addCommand (this, U"Run", U"Run chunk", 'T', menu_cb_runChunk);
}

void structNotebookEditor :: v_createMenuItems_help (EditorMenu menu) {
	NotebookEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"About NotebookEditor", '?', menu_cb_AboutNotebookEditor);
	EditorMenu_addCommand (menu, U"Scripting tutorial", 0, menu_cb_ScriptingTutorial);
	EditorMenu_addCommand (menu, U"Scripting examples", 0, menu_cb_ScriptingExamples);
	EditorMenu_addCommand (menu, U"Praat script", 0, menu_cb_PraatScript);
	EditorMenu_addCommand (menu, U"Formulas tutorial", 0, menu_cb_FormulasTutorial);
	EditorMenu_addCommand (menu, U"Functions", 0, menu_cb_Functions);
}

void NotebookEditor_init (NotebookEditor me, conststring32 initialText) {
	TextEditor_init (me, initialText);
	my interpreter = Interpreter_createFromEnvironment (nullptr);
	theReferencesToAllOpenNotebookEditors. addItem_ref (me);
}

autoNotebookEditor NotebookEditor_createFromText (conststring32 initialText) {
	try {
		autoNotebookEditor me = Thing_new (NotebookEditor);
		NotebookEditor_init (me.get(), initialText);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Notebook window not created.");
	}
}

autoNotebookEditor NotebookEditor_createFromNotebook_canBeNull (Notebook notebook) {
	try {
		for (integer ieditor = 1; ieditor <= theReferencesToAllOpenNotebookEditors.size; ieditor ++) {
			NotebookEditor editor = theReferencesToAllOpenNotebookEditors.at [ieditor];
			if (MelderFile_equal (& notebook -> file, & editor -> file)) {
				Editor_raise (editor);
				Melder_appendError (U"The notebook ", & notebook -> file, U" is already open and has been moved to the front.");
				if (editor -> dirty)
					Melder_appendError (U"Choose “Reopen from disk” if you want to revert to the old version.");
				Melder_flushError ();
				return autoNotebookEditor();   // safe null
			}
		}
		autostring32 text = MelderFile_readText (& notebook -> file);
		autoNotebookEditor me = NotebookEditor_createFromText (text.get());
		MelderFile_copy (& notebook -> file, & my file);
		Thing_setName (me.get(), nullptr);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Notebook window not created.");
	}
}

void NotebookEditor_debug_printAllOpenNotebookEditors () {
	for (integer ieditor = 1; ieditor <= theReferencesToAllOpenNotebookEditors.size; ieditor ++) {
		NotebookEditor editor = theReferencesToAllOpenNotebookEditors.at [ieditor];
		Melder_casual (U"Open notebook editor #", ieditor, U": <<", & editor -> file, U">>");
	}
}

/* End of file NotebookEditor.cpp */
