/* ScriptEditor.cpp
 *
 * Copyright (C) 1997-2005,2007-2018,2020 Paul Boersma
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
#include "../kar/longchar.h"
#include "praatP.h"
#include "EditorM.h"

Thing_implement (ScriptEditor, TextEditor, 0);

static CollectionOf <structScriptEditor> theReferencesToAllOpenScriptEditors;

bool ScriptEditors_dirty () {
	for (integer i = 1; i <= theReferencesToAllOpenScriptEditors.size; i ++) {
		ScriptEditor me = theReferencesToAllOpenScriptEditors.at [i];
		if (my dirty)
			return true;
	}
	return false;
}

void structScriptEditor :: v_destroy () noexcept {
	our argsDialog.reset();   // don't delay till delete
	theReferencesToAllOpenScriptEditors. undangleItem (this);
	ScriptEditor_Parent :: v_destroy ();
}

void structScriptEditor :: v_nameChanged () {
	bool dirtinessAlreadyShown = GuiWindow_setDirty (our windowForm, dirty);
	static MelderString buffer;
	MelderString_copy (& buffer, name [0] ? U"Script" : U"untitled script");
	if (editorClass)
		MelderString_append (& buffer, U" [", environmentName.get(), U"]");
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

static void args_ok (UiForm sendingForm, integer /* narg */, Stackel /* args */, conststring32 /* sendingString */,
	Interpreter /* interpreter */, conststring32 /* invokingButtonTitle */, bool /* modified */, void *void_me)
{
	iam (ScriptEditor);
	autostring32 text = GuiText_getString (my textWidget);
	structMelderFile file { };
	if (my name [0]) {
		Melder_pathToFile (my name.get(), & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);

	Interpreter_getArgumentsFromDialog (my interpreter.get(), sendingForm);

	autoPraatBackground background;
	if (my name [0])
		MelderFile_setDefaultDir (& file);
	Interpreter_run (my interpreter.get(), text.get());
}

static void args_ok_selectionOnly (UiForm sendingForm, integer /* narg */, Stackel /* args */, conststring32 /* sendingString */,
	Interpreter /* interpreter */, conststring32 /* invokingButtonTitle */, bool /* modified */, void *void_me)
{
	iam (ScriptEditor);
	autostring32 text = GuiText_getSelection (my textWidget);
	if (! text)
		Melder_throw (U"No text is selected any longer.\nPlease reselect or click Cancel.");
	structMelderFile file { };
	if (my name [0]) {
		Melder_pathToFile (my name.get(), & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);

	Interpreter_getArgumentsFromDialog (my interpreter.get(), sendingForm);

	autoPraatBackground background;
	if (my name [0]) MelderFile_setDefaultDir (& file);
	Interpreter_run (my interpreter.get(), text.get());
}

static void menu_cb_run (ScriptEditor me, EDITOR_ARGS_DIRECT) {
	bool isObscured = false;
	if (my interpreter -> running)
		Melder_throw (U"The script is already running (paused). Please close or continue the pause or demo window.");
	autostring32 text = GuiText_getString (my textWidget);
	trace (U"Running the following script (1):\n", text.get());
	structMelderFile file { };
	if (my name [0]) {
		Melder_pathToFile (my name.get(), & file);
		MelderFile_setDefaultDir (& file);
	}
	const conststring32 obscuredLabel = U"#!praatObscured";
	if (Melder_stringMatchesCriterion (text.get(), kMelder_string::STARTS_WITH, obscuredLabel, true)) {
		const integer obscuredLabelLength = str32len (obscuredLabel);
		const double fileKey_real = Melder_atof (MelderFile_name (& file));
		const uint64 fileKey = ( isdefined (fileKey_real) ? uint64 (fileKey_real) : 0 );
		char32 *restOfText = & text [obscuredLabelLength];
		uint64 passwordHash = 0;
		if (*restOfText == U'\n') {
			restOfText += 1;   // skip newline
		} else if (*restOfText == U' ') {
			restOfText ++;
			char32 *endOfFirstLine = str32chr (restOfText, U'\n');
			if (! endOfFirstLine)
				Melder_throw (U"Incomplete script.");
			*endOfFirstLine = U'\0';
			passwordHash = NUMhashString (restOfText);
			restOfText = endOfFirstLine + 1;
		} else {
			Melder_throw (U"Unexpected nonspace after #!praatObscured.");
		}
		static uint64 nonsecret = UINT64_C (529857089);
		text = unhex_STR (restOfText, fileKey + nonsecret + passwordHash);
		isObscured = true;
	}
	Melder_includeIncludeFiles (& text);
	const integer npar = Interpreter_readParameters (my interpreter.get(), text.get());
	if (npar) {
		/*
			Pop up a dialog box for querying the arguments.
		*/
		my argsDialog = Interpreter_createForm (my interpreter.get(), my windowForm, nullptr, args_ok, me, false);
		UiForm_do (my argsDialog.get(), false);
	} else {
		autoPraatBackground background;
		if (my name [0])
			MelderFile_setDefaultDir (& file);
		trace (U"Running the following script (2):\n", text.get());
		Interpreter_run (my interpreter.get(), text.get());
	}
}

static void menu_cb_runSelection (ScriptEditor me, EDITOR_ARGS_DIRECT) {
	if (my interpreter -> running)
		Melder_throw (U"The script is already running (paused). Please close or continue the pause or demo window.");
	autostring32 text = GuiText_getSelection (my textWidget);
	if (! text)
		Melder_throw (U"No text selected.");
	structMelderFile file { };
	if (my name [0]) {
		Melder_pathToFile (my name.get(), & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);
	integer npar = Interpreter_readParameters (my interpreter.get(), text.get());
	if (npar) {
		/*
			Pop up a dialog box for querying the arguments.
		*/
		my argsDialog = Interpreter_createForm (my interpreter.get(), my windowForm, nullptr, args_ok_selectionOnly, me, true);
		UiForm_do (my argsDialog.get(), false);
	} else {
		autoPraatBackground background;
		if (my name [0])
			MelderFile_setDefaultDir (& file);
		Interpreter_run (my interpreter.get(), text.get());
	}
}

static void menu_cb_addToMenu (ScriptEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add to menu", U"Add to fixed menu...")
		WORD (window, U"Window", U"?")
		SENTENCE (menu, U"Menu", U"File")
		SENTENCE (command, U"Command", U"Do it...")
		SENTENCE (afterCommand, U"After command", U"")
		INTEGER (depth, U"Depth", U"0")
		TEXTFIELD (scriptFile, U"Script file:", U"")
	EDITOR_OK
		if (my editorClass)
			SET_STRING (window, my editorClass -> className)
		if (my name [0])
			SET_STRING (scriptFile, my name.get())
		else
			SET_STRING (scriptFile, U"(please save your script first)")
	EDITOR_DO
		praat_addMenuCommandScript (window, menu, command, afterCommand, depth, scriptFile);
		praat_show ();
	EDITOR_END
}

static void menu_cb_addToFixedMenu (ScriptEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add to fixed menu", U"Add to fixed menu...");
		RADIOSTR (window, U"Window", 1)
			RADIOBUTTON (U"Objects")
			RADIOBUTTON (U"Picture")
		SENTENCE (menu, U"Menu", U"New")
		SENTENCE (command, U"Command", U"Do it...")
		SENTENCE (afterCommand, U"After command", U"")
		INTEGER (depth, U"Depth", U"0")
		TEXTFIELD (scriptFile, U"Script file:", U"")
	EDITOR_OK
		if (my name [0])
			SET_STRING (scriptFile, my name.get())
		else
			SET_STRING (scriptFile, U"(please save your script first)")
	EDITOR_DO
		praat_addMenuCommandScript (window, menu, command, afterCommand, depth, scriptFile);
		praat_show ();
	EDITOR_END
}

static void menu_cb_addToDynamicMenu (ScriptEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add to dynamic menu", U"Add to dynamic menu...")
		WORD (class1, U"Class 1", U"Sound")
		INTEGER (number1, U"Number 1", U"0")
		WORD (class2, U"Class 2", U"")
		INTEGER (number2, U"Number 2", U"0")
		WORD (class3, U"Class 3", U"")
		INTEGER (number3, U"Number 3", U"0")
		SENTENCE (command, U"Command", U"Do it...")
		SENTENCE (afterCommand, U"After command", U"")
		INTEGER (depth, U"Depth", U"0")
		TEXTFIELD (scriptFile, U"Script file:", U"")
	EDITOR_OK
		if (my name [0])
			SET_STRING (scriptFile, my name.get())
		else
			SET_STRING (scriptFile, U"(please save your script first)")
	EDITOR_DO
		praat_addActionScript (class1, number1, class2, number2, class3, number3, command, afterCommand, depth, scriptFile);
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
	integer length = str32len (history);
	if (history [length - 1] != U'\n') {
		UiHistory_write (U"\n");
		history = UiHistory_get ();
		length = str32len (history);
	}
	if (history [0] == U'\n') {
		history ++;
		length --;
	}
	integer first = 0, last = 0;
	autostring32 text = GuiText_getStringAndSelectionPosition (my textWidget, & first, & last);
	GuiText_replace (my textWidget, first, last, history);
	GuiText_setSelection (my textWidget, first, first + length);
	GuiText_scrollToSelection (my textWidget);
}

static void menu_cb_expandIncludeFiles (ScriptEditor me, EDITOR_ARGS_DIRECT) {
	structMelderFile file { };
	autostring32 text = GuiText_getString (my textWidget);
	if (my name [0]) {
		Melder_pathToFile (my name.get(), & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);
	GuiText_setString (my textWidget, text.get());
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

void ScriptEditor_init (ScriptEditor me, Editor environment, conststring32 initialText) {
	if (environment) {
		my environmentName = Melder_dup (environment -> name.get());
		my editorClass = environment -> classInfo;
	}
	TextEditor_init (me, initialText);
	my interpreter = Interpreter_createFromEnvironment (environment);
	theReferencesToAllOpenScriptEditors. addItem_ref (me);
}

autoScriptEditor ScriptEditor_createFromText (Editor environment, conststring32 initialText) {
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
		for (integer ieditor = 1; ieditor <= theReferencesToAllOpenScriptEditors.size; ieditor ++) {
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
		autoScriptEditor me = ScriptEditor_createFromText (environment, text.get());
		MelderFile_copy (& script -> file, & my file);
		Thing_setName (me.get(), Melder_fileToPath (& script -> file));
		return me;
	} catch (MelderError) {
		Melder_throw (U"Script window not created.");
	}
}

void ScriptEditor_debug_printAllOpenScriptEditors () {
	for (integer ieditor = 1; ieditor <= theReferencesToAllOpenScriptEditors.size; ieditor ++) {
		ScriptEditor editor = theReferencesToAllOpenScriptEditors.at [ieditor];
		Melder_casual (U"Open script editor #", ieditor, U": <<", & editor -> file, U">>");
	}
}

/* End of file ScriptEditor.cpp */
