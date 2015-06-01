/* ScriptEditor.cpp
 *
 * Copyright (C) 1997-2012,2013,2015 Paul Boersma
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

#include "ScriptEditor.h"
#include "longchar.h"
#include "praatP.h"
#include "EditorM.h"

Thing_implement (ScriptEditor, TextEditor, 0);

static Collection theScriptEditors;

int ScriptEditors_dirty (void) {
	if (! theScriptEditors) return FALSE;
	for (long i = 1; i <= theScriptEditors -> size; i ++) {
		ScriptEditor me = (ScriptEditor) theScriptEditors -> item [i];
		if (my dirty) return TRUE;
	}
	return FALSE;
}

void structScriptEditor :: v_destroy () {
	Melder_free (environmentName);
	forget (interpreter);
	forget (argsDialog);
	if (theScriptEditors) Collection_undangleItem (theScriptEditors, this);
	ScriptEditor_Parent :: v_destroy ();
}

void structScriptEditor :: v_nameChanged () {
	bool dirtinessAlreadyShown = GuiWindow_setDirty (d_windowForm, dirty);
	static MelderString buffer = { 0 };
	MelderString_copy (& buffer, name [0] ? L"Script" : L"untitled script");
	if (editorClass)
		MelderString_append (& buffer, L" [", environmentName, L"]");
	if (name [0])
		MelderString_append (& buffer, L" ", MelderFile_messageName (& file));
	if (dirty && ! dirtinessAlreadyShown)
		MelderString_append (& buffer, L" (modified)");
	GuiShell_setTitle (d_windowForm, buffer.string);
}

void structScriptEditor :: v_goAway () {
	if (interpreter -> running) {
		Melder_error_ ("Cannot close the script window while the script is running or paused. Please close or continue the pause or demo window.");
		Melder_flushError (NULL);
	} else {
		ScriptEditor_Parent :: v_goAway ();
	}
}

static void args_ok (UiForm sendingForm, int narg_dummy, Stackel args_dummy, const wchar_t *sendingString_dummy, Interpreter interpreter_dummy, const wchar_t *invokingButtonTitle, bool modified_dummy, I) {
	iam (ScriptEditor);
	(void) narg_dummy;
	(void) args_dummy;
	(void) sendingString_dummy;
	(void) interpreter_dummy;
	(void) invokingButtonTitle;
	(void) modified_dummy;
	autostring32 text = GuiText_getString32 (my textWidget);
	structMelderFile file = { 0 };
	if (my name [0]) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);

	Interpreter_getArgumentsFromDialog (my interpreter, sendingForm);

	autoPraatBackground background;
	if (my name [0]) MelderFile_setDefaultDir (& file);
	Interpreter_run (my interpreter, text.peek());
}

static void args_ok_selectionOnly (UiForm sendingForm, int narg_dummy, Stackel args_dummy, const wchar_t *sendingString_dummy, Interpreter interpreter_dummy, const wchar_t *invokingButtonTitle, bool modified_dummy, I) {
	iam (ScriptEditor);
	(void) narg_dummy;
	(void) args_dummy;
	(void) sendingString_dummy;
	(void) interpreter_dummy;
	(void) invokingButtonTitle;
	(void) modified_dummy;
	autostring32 text = GuiText_getSelection32 (my textWidget);
	if (text.peek() == NULL)
		Melder_throw ("No text is selected any longer.\nPlease reselect or click Cancel.");
	structMelderFile file = { 0 };
	if (my name [0]) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);

	Interpreter_getArgumentsFromDialog (my interpreter, sendingForm);

	autoPraatBackground background;
	if (my name [0]) MelderFile_setDefaultDir (& file);
	Interpreter_run (my interpreter, text.peek());
}

static void menu_cb_run (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	if (my interpreter -> running)
		Melder_throw ("The script is already running (paused). Please close or continue the pause or demo window.");
	autostring32 text = GuiText_getString32 (my textWidget);
	trace ("Running the following script (1):\n%s", Melder_peekStr32ToUtf8 (text.peek()));
	structMelderFile file = { 0 };
	if (my name [0]) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);
	int npar = Interpreter_readParameters (my interpreter, text.peek());
	if (npar) {
		/*
		 * Pop up a dialog box for querying the arguments.
		 */
		forget (my argsDialog);
		my argsDialog = Interpreter_createForm (my interpreter, my d_windowForm, NULL, args_ok, me, false);
		UiForm_do (my argsDialog, false);
	} else {
		autoPraatBackground background;
		if (my name [0]) MelderFile_setDefaultDir (& file);
		trace ("Running the following script (2):\n%s", Melder_peekStr32ToUtf8 (text.peek()));
		Interpreter_run (my interpreter, text.peek());
	}
}

static void menu_cb_runSelection (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	if (my interpreter -> running)
		Melder_throw (L"The script is already running (paused). Please close or continue the pause or demo window.");
	autostring32 text = GuiText_getSelection32 (my textWidget);
	if (text.peek() == NULL)
		Melder_throw ("No text selected.");
	structMelderFile file = { 0 };
	if (my name [0]) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);
	int npar = Interpreter_readParameters (my interpreter, text.peek());
	if (npar) {
		/*
		 * Pop up a dialog box for querying the arguments.
		 */
		forget (my argsDialog);
		my argsDialog = Interpreter_createForm (my interpreter, my d_windowForm, NULL, args_ok_selectionOnly, me, true);
		UiForm_do (my argsDialog, false);
	} else {
		autoPraatBackground background;
		if (my name [0]) MelderFile_setDefaultDir (& file);
		Interpreter_run (my interpreter, text.peek());
	}
}

static void menu_cb_addToMenu (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	EDITOR_FORM (L"Add to menu", L"Add to fixed menu...")
		WORD (L"Window", L"?")
		SENTENCE (L"Menu", L"File")
		SENTENCE (L"Command", L"Do it...")
		SENTENCE (L"After command", L"")
		INTEGER (L"Depth", L"0")
		LABEL (L"", L"Script file:")
		TEXTFIELD (L"Script", L"")
	EDITOR_OK
		if (my editorClass) SET_STRING (L"Window", my editorClass -> className)
		if (my name [0])
			SET_STRING (L"Script", my name)
		else
			SET_STRING (L"Script", L"(please save your script first)")
	EDITOR_DO
		praat_addMenuCommandScript (GET_STRING (L"Window"),
			GET_STRING (L"Menu"), GET_STRING (L"Command"), GET_STRING (L"After command"),
			GET_INTEGER (L"Depth"), GET_STRING (L"Script"));
		praat_show ();
	EDITOR_END
}

static void menu_cb_addToFixedMenu (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	EDITOR_FORM (L"Add to fixed menu", L"Add to fixed menu...");
		RADIO (L"Window", 1)
			RADIOBUTTON (L"Objects")
			RADIOBUTTON (L"Picture")
		SENTENCE (L"Menu", L"New")
		SENTENCE (L"Command", L"Do it...")
		SENTENCE (L"After command", L"")
		INTEGER (L"Depth", L"0")
		LABEL (L"", L"Script file:")
		TEXTFIELD (L"Script", L"")
	EDITOR_OK
		if (my name [0])
			SET_STRING (L"Script", my name)
		else
			SET_STRING (L"Script", L"(please save your script first)")
	EDITOR_DO
		praat_addMenuCommandScript (GET_STRING (L"Window"),
			GET_STRING (L"Menu"), GET_STRING (L"Command"), GET_STRING (L"After command"),
			GET_INTEGER (L"Depth"), GET_STRING (L"Script"));
		praat_show ();
	EDITOR_END
}

static void menu_cb_addToDynamicMenu (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	EDITOR_FORM (L"Add to dynamic menu", L"Add to dynamic menu...")
		WORD (L"Class 1", L"Sound")
		INTEGER (L"Number 1", L"0")
		WORD (L"Class 2", L"")
		INTEGER (L"Number 2", L"0")
		WORD (L"Class 3", L"")
		INTEGER (L"Number 3", L"0")
		SENTENCE (L"Command", L"Do it...")
		SENTENCE (L"After command", L"")
		INTEGER (L"Depth", L"0")
		LABEL (L"", L"Script file:")
		TEXTFIELD (L"Script", L"")
	EDITOR_OK
		if (my name [0])
			SET_STRING (L"Script", my name)
		else
			SET_STRING (L"Script", L"(please save your script first)")
	EDITOR_DO
		praat_addActionScript (GET_STRING (L"Class 1"), GET_INTEGER (L"Number 1"),
			GET_STRING (L"Class 2"), GET_INTEGER (L"Number 2"), GET_STRING (L"Class 3"),
			GET_INTEGER (L"Number 3"), GET_STRING (L"Command"), GET_STRING (L"After command"),
			GET_INTEGER (L"Depth"), GET_STRING (L"Script"));
		praat_show ();
	EDITOR_END
}

static void menu_cb_clearHistory (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	UiHistory_clear ();
}

static void menu_cb_pasteHistory (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	char32 *history = UiHistory_get ();
	if (history == NULL || history [0] == U'\0')
		Melder_throw ("No history.");
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
	wchar_t *text = GuiText_getStringAndSelectionPosition (my textWidget, & first, & last);
	Melder_free (text);
	GuiText_replace (my textWidget, first, last, history);
	GuiText_setSelection (my textWidget, first, first + length);
	GuiText_scrollToSelection (my textWidget);
}

static void menu_cb_expandIncludeFiles (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	structMelderFile file = { 0 };
	autostring32 text = GuiText_getString32 (my textWidget);
	if (my name [0]) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);
	GuiText_setString (my textWidget, text.peek());
}

static void menu_cb_AboutScriptEditor (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"ScriptEditor"); }
static void menu_cb_ScriptingTutorial (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Scripting"); }
static void menu_cb_ScriptingExamples (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Scripting examples"); }
static void menu_cb_PraatScript (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Praat script"); }
static void menu_cb_FormulasTutorial (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Formulas"); }
static void menu_cb_DemoWindow (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Demo window"); }
static void menu_cb_TheHistoryMechanism (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"History mechanism"); }
static void menu_cb_InitializationScripts (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Initialization script"); }
static void menu_cb_AddingToAFixedMenu (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Add to fixed menu..."); }
static void menu_cb_AddingToADynamicMenu (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Add to dynamic menu..."); }

void structScriptEditor :: v_createMenus () {
	ScriptEditor_Parent :: v_createMenus ();
	if (editorClass) {
		Editor_addCommand (this, L"File", L"Add to menu...", 0, menu_cb_addToMenu);
	} else {
		Editor_addCommand (this, L"File", L"Add to fixed menu...", 0, menu_cb_addToFixedMenu);
		Editor_addCommand (this, L"File", L"Add to dynamic menu...", 0, menu_cb_addToDynamicMenu);
	}
	Editor_addCommand (this, L"File", L"-- close --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"-- history --", 0, 0);
	Editor_addCommand (this, L"Edit", L"Clear history", 0, menu_cb_clearHistory);
	Editor_addCommand (this, L"Edit", L"Paste history", 'H', menu_cb_pasteHistory);
	Editor_addCommand (this, L"Convert", L"-- expand --", 0, 0);
	Editor_addCommand (this, L"Convert", L"Expand include files", 0, menu_cb_expandIncludeFiles);
	Editor_addMenu (this, L"Run", 0);
	Editor_addCommand (this, L"Run", L"Run", 'R', menu_cb_run);
	Editor_addCommand (this, L"Run", L"Run selection", 'T', menu_cb_runSelection);
}

void structScriptEditor :: v_createHelpMenuItems (EditorMenu menu) {
	ScriptEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"About ScriptEditor", '?', menu_cb_AboutScriptEditor);
	EditorMenu_addCommand (menu, L"Scripting tutorial", 0, menu_cb_ScriptingTutorial);
	EditorMenu_addCommand (menu, L"Scripting examples", 0, menu_cb_ScriptingExamples);
	EditorMenu_addCommand (menu, L"Praat script", 0, menu_cb_PraatScript);
	EditorMenu_addCommand (menu, L"Formulas tutorial", 0, menu_cb_FormulasTutorial);
	EditorMenu_addCommand (menu, L"Demo window", 0, menu_cb_DemoWindow);
	EditorMenu_addCommand (menu, L"-- help history --", 0, NULL);
	EditorMenu_addCommand (menu, L"The History mechanism", 0, menu_cb_TheHistoryMechanism);
	EditorMenu_addCommand (menu, L"Initialization scripts", 0, menu_cb_InitializationScripts);
	EditorMenu_addCommand (menu, L"-- help add --", 0, NULL);
	EditorMenu_addCommand (menu, L"Adding to a fixed menu", 0, menu_cb_AddingToAFixedMenu);
	EditorMenu_addCommand (menu, L"Adding to a dynamic menu", 0, menu_cb_AddingToADynamicMenu);
}

void ScriptEditor_init (ScriptEditor me, Editor environment, const wchar_t *initialText) {
	if (environment != NULL) {
		my environmentName = Melder_wcsdup (environment -> name);
		my editorClass = environment -> classInfo;
	}
	TextEditor_init (me, initialText);
	my interpreter = Interpreter_createFromEnvironment (environment);
	if (theScriptEditors == NULL) {
		theScriptEditors = Collection_create (NULL, 10);
		Collection_dontOwnItems (theScriptEditors);
	}
	Collection_addItem (theScriptEditors, me);
}

ScriptEditor ScriptEditor_createFromText (Editor environment, const wchar_t *initialText) {
	try {
		autoScriptEditor me = Thing_new (ScriptEditor);
		ScriptEditor_init (me.peek(), environment, initialText);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Script window not created.");
	}
}

ScriptEditor ScriptEditor_createFromScript (Editor environment, Script script) {
	try {
		if (theScriptEditors) {
			for (long ieditor = 1; ieditor <= theScriptEditors -> size; ieditor ++) {
				ScriptEditor editor = (ScriptEditor) theScriptEditors -> item [ieditor];
				if (MelderFile_equal (& script -> file, & editor -> file)) {
					Editor_raise (editor);
					Melder_error_ ("The script ", & script -> file, " is already open and has been moved to the front.");
					if (editor -> dirty)
						Melder_error_ ("Choose \"Reopen from disk\" if you want to revert to the old version.");
					Melder_flushError (NULL);
					return NULL;   // safe NULL
				}
			}
		}
		autostring text = MelderFile_readText (& script -> file);
		autoScriptEditor me = ScriptEditor_createFromText (environment, text.peek());
		MelderFile_copy (& script -> file, & my file);
		Thing_setName (me.peek(), Melder_fileToPath (& script -> file));
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Script window not created.");
	}
}

/* End of file ScriptEditor.cpp */
