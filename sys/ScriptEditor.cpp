/* ScriptEditor.cpp
 *
 * Copyright (C) 1997-2012,2013,2015,2016,2017 Paul Boersma
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

#include "ScriptEditor.h"
#include "longchar.h"
#include "praatP.h"
#include "EditorM.h"

Thing_implement (ScriptEditor, TextEditor, 0);

static CollectionOf <structScriptEditor> theReferencesToAllOpenScriptEditors;

bool ScriptEditors_dirty () {
	for (long i = 1; i <= theReferencesToAllOpenScriptEditors.size; i ++) {
		ScriptEditor me = theReferencesToAllOpenScriptEditors.at [i];
		if (my dirty) return true;
	}
	return false;
}

void structScriptEditor :: v_destroy () noexcept {
	Melder_free (environmentName);
	our argsDialog.reset();   // don't delay till delete
	theReferencesToAllOpenScriptEditors. undangleItem (this);
	ScriptEditor_Parent :: v_destroy ();
}

void structScriptEditor :: v_nameChanged () {
	bool dirtinessAlreadyShown = GuiWindow_setDirty (our windowForm, dirty);
	static MelderString buffer { };
	MelderString_copy (& buffer, name [0] ? U"Script" : U"untitled script");
	if (editorClass)
		MelderString_append (& buffer, U" [", environmentName, U"]");
	if (name [0])
		MelderString_append (& buffer, U" ", MelderFile_messageName (& file));
	if (dirty && ! dirtinessAlreadyShown)
		MelderString_append (& buffer, U" (modified)");
	GuiShell_setTitle (windowForm, buffer.string);
}

void structScriptEditor :: v_goAway () {
	if (interpreter -> running) {
		Melder_flushError (U"Cannot close the script window while the script is running or paused. Please close or continue the pause or demo window.");
	} else {
		ScriptEditor_Parent :: v_goAway ();
	}
}

static void args_ok (UiForm sendingForm, int /* narg */, Stackel /* args */, const char32 * /* sendingString */,
	Interpreter /* interpreter */, const char32 * /* invokingButtonTitle */, bool /* modified */, void *void_me)
{
	iam (ScriptEditor);
	autostring32 text = GuiText_getString (my textWidget);
	structMelderFile file { };
	if (my name [0]) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);

	Interpreter_getArgumentsFromDialog (my interpreter.get(), sendingForm);

	autoPraatBackground background;
	if (my name [0]) MelderFile_setDefaultDir (& file);
	Interpreter_run (my interpreter.get(), text.peek());
}

static void args_ok_selectionOnly (UiForm sendingForm, int /* narg */, Stackel /* args */, const char32 * /* sendingString */,
	Interpreter /* interpreter */, const char32 * /* invokingButtonTitle */, bool /* modified */, void *void_me)
{
	iam (ScriptEditor);
	autostring32 text = GuiText_getSelection (my textWidget);
	if (! text.peek())
		Melder_throw (U"No text is selected any longer.\nPlease reselect or click Cancel.");
	structMelderFile file { };
	if (my name [0]) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);

	Interpreter_getArgumentsFromDialog (my interpreter.get(), sendingForm);

	autoPraatBackground background;
	if (my name [0]) MelderFile_setDefaultDir (& file);
	Interpreter_run (my interpreter.get(), text.peek());
}

static void menu_cb_run (ScriptEditor me, EDITOR_ARGS_DIRECT) {
	if (my interpreter -> running)
		Melder_throw (U"The script is already running (paused). Please close or continue the pause or demo window.");
	autostring32 text = GuiText_getString (my textWidget);
	trace (U"Running the following script (1):\n", text.peek());
	structMelderFile file { };
	if (my name [0]) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);
	int npar = Interpreter_readParameters (my interpreter.get(), text.peek());
	if (npar) {
		/*
		 * Pop up a dialog box for querying the arguments.
		 */
		my argsDialog = autoUiForm (Interpreter_createForm (my interpreter.get(), my windowForm, nullptr, args_ok, me, false));
		UiForm_do (my argsDialog.get(), false);
	} else {
		autoPraatBackground background;
		if (my name [0]) MelderFile_setDefaultDir (& file);
		trace (U"Running the following script (2):\n", text.peek());
		Interpreter_run (my interpreter.get(), text.peek());
	}
}

static void menu_cb_runSelection (ScriptEditor me, EDITOR_ARGS_DIRECT) {
	if (my interpreter -> running)
		Melder_throw (U"The script is already running (paused). Please close or continue the pause or demo window.");
	autostring32 text = GuiText_getSelection (my textWidget);
	if (! text.peek())
		Melder_throw (U"No text selected.");
	structMelderFile file { };
	if (my name [0]) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);
	int npar = Interpreter_readParameters (my interpreter.get(), text.peek());
	if (npar) {
		/*
		 * Pop up a dialog box for querying the arguments.
		 */
		my argsDialog = autoUiForm (Interpreter_createForm (my interpreter.get(), my windowForm, nullptr, args_ok_selectionOnly, me, true));
		UiForm_do (my argsDialog.get(), false);
	} else {
		autoPraatBackground background;
		if (my name [0]) MelderFile_setDefaultDir (& file);
		Interpreter_run (my interpreter.get(), text.peek());
	}
}

static void menu_cb_addToMenu (ScriptEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add to menu", U"Add to fixed menu...")
		WORD (U"Window", U"?")
		SENTENCE (U"Menu", U"File")
		SENTENCE (U"Command", U"Do it...")
		SENTENCE (U"After command", U"")
		INTEGER (U"Depth", U"0")
		LABEL (U"", U"Script file:")
		TEXTFIELD (U"Script", U"")
	EDITOR_OK
		if (my editorClass) SET_STRING (U"Window", my editorClass -> className)
		if (my name [0])
			SET_STRING (U"Script", my name)
		else
			SET_STRING (U"Script", U"(please save your script first)")
	EDITOR_DO
		praat_addMenuCommandScript (GET_STRING (U"Window"),
			GET_STRING (U"Menu"), GET_STRING (U"Command"), GET_STRING (U"After command"),
			GET_INTEGER (U"Depth"), GET_STRING (U"Script"));
		praat_show ();
	EDITOR_END
}

static void menu_cb_addToFixedMenu (ScriptEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add to fixed menu", U"Add to fixed menu...");
		RADIO (U"Window", 1)
			RADIOBUTTON (U"Objects")
			RADIOBUTTON (U"Picture")
		SENTENCE (U"Menu", U"New")
		SENTENCE (U"Command", U"Do it...")
		SENTENCE (U"After command", U"")
		INTEGER (U"Depth", U"0")
		LABEL (U"", U"Script file:")
		TEXTFIELD (U"Script", U"")
	EDITOR_OK
		if (my name [0])
			SET_STRING (U"Script", my name)
		else
			SET_STRING (U"Script", U"(please save your script first)")
	EDITOR_DO
		praat_addMenuCommandScript (GET_STRING (U"Window"),
			GET_STRING (U"Menu"), GET_STRING (U"Command"), GET_STRING (U"After command"),
			GET_INTEGER (U"Depth"), GET_STRING (U"Script"));
		praat_show ();
	EDITOR_END
}

static void menu_cb_addToDynamicMenu (ScriptEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add to dynamic menu", U"Add to dynamic menu...")
		WORD (U"Class 1", U"Sound")
		INTEGER (U"Number 1", U"0")
		WORD (U"Class 2", U"")
		INTEGER (U"Number 2", U"0")
		WORD (U"Class 3", U"")
		INTEGER (U"Number 3", U"0")
		SENTENCE (U"Command", U"Do it...")
		SENTENCE (U"After command", U"")
		INTEGER (U"Depth", U"0")
		LABEL (U"", U"Script file:")
		TEXTFIELD (U"Script", U"")
	EDITOR_OK
		if (my name [0])
			SET_STRING (U"Script", my name)
		else
			SET_STRING (U"Script", U"(please save your script first)")
	EDITOR_DO
		praat_addActionScript (GET_STRING (U"Class 1"), GET_INTEGER (U"Number 1"),
			GET_STRING (U"Class 2"), GET_INTEGER (U"Number 2"), GET_STRING (U"Class 3"),
			GET_INTEGER (U"Number 3"), GET_STRING (U"Command"), GET_STRING (U"After command"),
			GET_INTEGER (U"Depth"), GET_STRING (U"Script"));
		praat_show ();
	EDITOR_END
}

static void menu_cb_clearHistory (ScriptEditor /* me */, EDITOR_ARGS_DIRECT) {
	UiHistory_clear ();
}

static void menu_cb_pasteHistory (ScriptEditor me, EDITOR_ARGS_DIRECT) {
	char32 *history = UiHistory_get ();
	if (! history || history [0] == U'\0')
		Melder_throw (U"No history.");
	long length = str32len (history);
	if (history [length - 1] != U'\n') {
		UiHistory_write (U"\n");
		history = UiHistory_get ();
		length = str32len (history);
	}
	if (history [0] == U'\n') {
		history ++;
		length --;
	}
	long first = 0, last = 0;
	char32 *text = GuiText_getStringAndSelectionPosition (my textWidget, & first, & last);
	Melder_free (text);
	GuiText_replace (my textWidget, first, last, history);
	GuiText_setSelection (my textWidget, first, first + length);
	GuiText_scrollToSelection (my textWidget);
}

static void menu_cb_expandIncludeFiles (ScriptEditor me, EDITOR_ARGS_DIRECT) {
	structMelderFile file { };
	autostring32 text = GuiText_getString (my textWidget);
	if (my name [0]) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);
	GuiText_setString (my textWidget, text.peek());
}

static void menu_cb_AboutScriptEditor (ScriptEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"ScriptEditor"); }
static void menu_cb_ScriptingTutorial (ScriptEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Scripting"); }
static void menu_cb_ScriptingExamples (ScriptEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Scripting examples"); }
static void menu_cb_PraatScript (ScriptEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Praat script"); }
static void menu_cb_FormulasTutorial (ScriptEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Formulas"); }
static void menu_cb_DemoWindow (ScriptEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Demo window"); }
static void menu_cb_TheHistoryMechanism (ScriptEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"History mechanism"); }
static void menu_cb_InitializationScripts (ScriptEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Initialization script"); }
static void menu_cb_AddingToAFixedMenu (ScriptEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Add to fixed menu..."); }
static void menu_cb_AddingToADynamicMenu (ScriptEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Add to dynamic menu..."); }

void structScriptEditor :: v_createMenus () {
	ScriptEditor_Parent :: v_createMenus ();
	if (editorClass) {
		Editor_addCommand (this, U"File", U"Add to menu...", 0, menu_cb_addToMenu);
	} else {
		Editor_addCommand (this, U"File", U"Add to fixed menu...", 0, menu_cb_addToFixedMenu);
		Editor_addCommand (this, U"File", U"Add to dynamic menu...", 0, menu_cb_addToDynamicMenu);
	}
	Editor_addCommand (this, U"File", U"-- close --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"-- history --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Clear history", 0, menu_cb_clearHistory);
	Editor_addCommand (this, U"Edit", U"Paste history", 'H', menu_cb_pasteHistory);
	Editor_addCommand (this, U"Convert", U"-- expand --", 0, nullptr);
	Editor_addCommand (this, U"Convert", U"Expand include files", 0, menu_cb_expandIncludeFiles);
	Editor_addMenu (this, U"Run", 0);
	Editor_addCommand (this, U"Run", U"Run", 'R', menu_cb_run);
	Editor_addCommand (this, U"Run", U"Run selection", 'T', menu_cb_runSelection);
}

void structScriptEditor :: v_createHelpMenuItems (EditorMenu menu) {
	ScriptEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"About ScriptEditor", '?', menu_cb_AboutScriptEditor);
	EditorMenu_addCommand (menu, U"Scripting tutorial", 0, menu_cb_ScriptingTutorial);
	EditorMenu_addCommand (menu, U"Scripting examples", 0, menu_cb_ScriptingExamples);
	EditorMenu_addCommand (menu, U"Praat script", 0, menu_cb_PraatScript);
	EditorMenu_addCommand (menu, U"Formulas tutorial", 0, menu_cb_FormulasTutorial);
	EditorMenu_addCommand (menu, U"Demo window", 0, menu_cb_DemoWindow);
	EditorMenu_addCommand (menu, U"-- help history --", 0, nullptr);
	EditorMenu_addCommand (menu, U"The History mechanism", 0, menu_cb_TheHistoryMechanism);
	EditorMenu_addCommand (menu, U"Initialization scripts", 0, menu_cb_InitializationScripts);
	EditorMenu_addCommand (menu, U"-- help add --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Adding to a fixed menu", 0, menu_cb_AddingToAFixedMenu);
	EditorMenu_addCommand (menu, U"Adding to a dynamic menu", 0, menu_cb_AddingToADynamicMenu);
}

void ScriptEditor_init (ScriptEditor me, Editor environment, const char32 *initialText) {
	if (environment) {
		my environmentName = Melder_dup (environment -> name);
		my editorClass = environment -> classInfo;
	}
	TextEditor_init (me, initialText);
	my interpreter = Interpreter_createFromEnvironment (environment);
	theReferencesToAllOpenScriptEditors. addItem_ref (me);
}

autoScriptEditor ScriptEditor_createFromText (Editor environment, const char32 *initialText) {
	try {
		autoScriptEditor me = Thing_new (ScriptEditor);
		ScriptEditor_init (me.get(), environment, initialText);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Script window not created.");
	}
}

autoScriptEditor ScriptEditor_createFromScript_canBeNull (Editor environment, Script script) {
	try {
		for (long ieditor = 1; ieditor <= theReferencesToAllOpenScriptEditors.size; ieditor ++) {
			ScriptEditor editor = theReferencesToAllOpenScriptEditors.at [ieditor];
			if (MelderFile_equal (& script -> file, & editor -> file)) {
				Editor_raise (editor);
				Melder_appendError (U"The script ", & script -> file, U" is already open and has been moved to the front.");
				if (editor -> dirty)
					Melder_appendError (U"Choose \"Reopen from disk\" if you want to revert to the old version.");
				Melder_flushError ();
				return autoScriptEditor();   // safe null
			}
		}
		autostring32 text = MelderFile_readText (& script -> file);
		autoScriptEditor me = ScriptEditor_createFromText (environment, text.peek());
		MelderFile_copy (& script -> file, & my file);
		Thing_setName (me.get(), Melder_fileToPath (& script -> file));
		return me;
	} catch (MelderError) {
		Melder_throw (U"Script window not created.");
	}
}

void ScriptEditor_debug_printAllOpenScriptEditors () {
	for (long ieditor = 1; ieditor <= theReferencesToAllOpenScriptEditors.size; ieditor ++) {
		ScriptEditor editor = theReferencesToAllOpenScriptEditors.at [ieditor];
		Melder_casual (U"Open script editor #", ieditor, U": <<", & editor -> file, U">>");
	}
}

/* End of file ScriptEditor.cpp */
