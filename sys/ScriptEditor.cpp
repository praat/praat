/* ScriptEditor.cpp
 *
 * Copyright (C) 1997-2011 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2002/10/14 added scripting examples to Help menu
 * pb 2002/12/05 include
 * pb 2004/01/07 use GuiWindow_setDirty
 * pb 2007/06/12 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2008/03/20 split off Help menu
 * pb 2008/03/21 new Editor API
 * pb 2009/01/18 arguments to UiForm callbacks
 * pb 2009/01/20 pause forms
 * pb 2009/05/07 demo window
 * pb 2010/04/30 command "Expand include files"
 * pb 2011/04/06 C++
 */

#include "ScriptEditor.h"
#include "longchar.h"
#include "praatP.h"
#include "EditorM.h"
#include "UnicodeData.h"

static Collection theScriptEditors;

int ScriptEditors_dirty (void) {
	if (! theScriptEditors) return FALSE;
	for (long i = 1; i <= theScriptEditors -> size; i ++) {
		ScriptEditor me = (ScriptEditor) theScriptEditors -> item [i];
		if (my dirty) return TRUE;
	}
	return FALSE;
}

static void destroy (I) {
	iam (ScriptEditor);
	Melder_free (my environmentName);
	forget (my interpreter);
	forget (my argsDialog);
	if (theScriptEditors) Collection_undangleItem (theScriptEditors, me);
	inherited (ScriptEditor) destroy (me);
}

static void nameChanged (I) {
	iam (ScriptEditor);
	int dirtinessAlreadyShown = GuiWindow_setDirty (my shell, my dirty);
	static MelderString buffer = { 0 };
	MelderString_copy (& buffer, my name ? L"Script" : L"untitled script");
	if (my editorClass) {
		MelderString_append3 (& buffer, L" [", my environmentName, L"]");
	}
	if (my name) {
		MelderString_append3 (& buffer, L" " UNITEXT_LEFT_DOUBLE_QUOTATION_MARK, MelderFile_messageName (& my file), UNITEXT_RIGHT_DOUBLE_QUOTATION_MARK);
	}
	if (my dirty && ! dirtinessAlreadyShown)
		MelderString_append (& buffer, L" (modified)");
	GuiWindow_setTitle (my shell, buffer.string);
	#if motif
	XtVaSetValues (my shell, XmNiconName, "Script", NULL);
	#endif
}

static void goAway (ScriptEditor me) {
	if (my interpreter -> running) {
		Melder_error1 (L"Cannot close the script window while the script is running or paused. Please close or continue the pause or demo window.");
		Melder_flushError (NULL);
		return;
	}
	inherited (ScriptEditor) goAway (me);
}

static int args_ok (UiForm sendingForm, const wchar_t *sendingString_dummy, Interpreter interpreter_dummy, const wchar_t *invokingButtonTitle, bool modified_dummy, I) {
	iam (ScriptEditor);
	(void) sendingString_dummy;
	(void) interpreter_dummy;
	(void) invokingButtonTitle;
	(void) modified_dummy;
	structMelderFile file = { 0 };
	wchar_t *text = GuiText_getString (my textWidget);
	if (my name) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);

	Interpreter_getArgumentsFromDialog (my interpreter, sendingForm);

	praat_background ();
	if (my name) MelderFile_setDefaultDir (& file);
	Interpreter_run (my interpreter, text);
	praat_foreground ();
	Melder_free (text);
	iferror return 0;
	return 1;
}

static void run (ScriptEditor me, wchar_t **text) {
	structMelderFile file = { 0 };
	if (my name) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (text);
	iferror { Melder_flushError (NULL); return; }
	int npar = Interpreter_readParameters (my interpreter, *text);
	iferror { Melder_flushError (NULL); return; }
	if (npar) {
		/*
		 * Pop up a dialog box for querying the arguments.
		 */
		forget (my argsDialog);
		my argsDialog = Interpreter_createForm (my interpreter, my shell, NULL, args_ok, me);
		UiForm_do (my argsDialog, false);
	} else {
		praat_background ();
		if (my name) MelderFile_setDefaultDir (& file);
		Interpreter_run (my interpreter, *text);
		praat_foreground ();
		iferror Melder_flushError (NULL);
	}
}

static int menu_cb_run (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	if (my interpreter -> running)
		Melder_throw ("The script is already running (paused). Please close or continue the pause or demo window.");
	autostring text = GuiText_getString (my textWidget);   // not an autostring (the text pointer can be changed by including include files)
	run (me, & text);
	return 1;
}

static int menu_cb_runSelection (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	if (my interpreter -> running)
		Melder_throw (L"The script is already running (paused). Please close or continue the pause or demo window.");
	autostring text = GuiText_getSelection (my textWidget);
	if (text.peek() == NULL)
		Melder_throw ("No text selected.");
	run (me, & text);
	return 1;
}

static int menu_cb_addToMenu (EDITOR_ARGS) {
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
		if (my editorClass) SET_STRING (L"Window", my editorClass -> _className)
		if (my name)
			SET_STRING (L"Script", my name)
		else
			SET_STRING (L"Script", L"(please save your script first)")
	EDITOR_DO
		praat_addMenuCommandScript (GET_STRING (L"Window"),
			GET_STRING (L"Menu"), GET_STRING (L"Command"), GET_STRING (L"After command"),
			GET_INTEGER (L"Depth"), GET_STRING (L"Script")); cherror
		praat_show ();
	end: (void) 0;
	EDITOR_END
}

static int menu_cb_addToFixedMenu (EDITOR_ARGS) {
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
		if (my name)
			SET_STRING (L"Script", my name)
		else
			SET_STRING (L"Script", L"(please save your script first)")
	EDITOR_DO
		praat_addMenuCommandScript (GET_STRING (L"Window"),
			GET_STRING (L"Menu"), GET_STRING (L"Command"), GET_STRING (L"After command"),
			GET_INTEGER (L"Depth"), GET_STRING (L"Script")); cherror
		praat_show ();
	end: (void) 0;
	EDITOR_END
}

static int menu_cb_addToDynamicMenu (EDITOR_ARGS) {
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
		if (my name)
			SET_STRING (L"Script", my name)
		else
			SET_STRING (L"Script", L"(please save your script first)")
	EDITOR_DO
		praat_addActionScript (GET_STRING (L"Class 1"), GET_INTEGER (L"Number 1"),
			GET_STRING (L"Class 2"), GET_INTEGER (L"Number 2"), GET_STRING (L"Class 3"),
			GET_INTEGER (L"Number 3"), GET_STRING (L"Command"), GET_STRING (L"After command"),
			GET_INTEGER (L"Depth"), GET_STRING (L"Script")); cherror
		praat_show ();
	end: (void) 0;
	EDITOR_END
}

static int menu_cb_clearHistory (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	UiHistory_clear ();
	return 1;
}

static int menu_cb_pasteHistory (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	wchar_t *history = UiHistory_get ();
	if (history == NULL || history [0] == '\0')
		return Melder_error1 (L"No history.");
	long length = wcslen (history);
	if (history [length - 1] != '\n') {
		UiHistory_write (L"\n");
		history = UiHistory_get ();
		length = wcslen (history);
	}
	if (history [0] == '\n') {
		history ++;
		length --;
	}
	long first = 0, last = 0;
	wchar_t *text = GuiText_getStringAndSelectionPosition (my textWidget, & first, & last);
	Melder_free (text);
	GuiText_replace (my textWidget, first, last, history);
	GuiText_setSelection (my textWidget, first, first + length);
	GuiText_scrollToSelection (my textWidget);
	return 1;
}

static int menu_cb_expandIncludeFiles (EDITOR_ARGS) {
	EDITOR_IAM (ScriptEditor);
	structMelderFile file = { 0 };
	wchar_t *text = GuiText_getString (my textWidget);
	if (my name) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text); cherror
	GuiText_setString (my textWidget, text);
end:
	Melder_free (text);
	iferror return 0;
	return 1;
}

static int menu_cb_AboutScriptEditor (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"ScriptEditor"); return 1; }
static int menu_cb_ScriptingTutorial (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Scripting"); return 1; }
static int menu_cb_ScriptingExamples (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Scripting examples"); return 1; }
static int menu_cb_PraatScript (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Praat script"); return 1; }
static int menu_cb_FormulasTutorial (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Formulas"); return 1; }
static int menu_cb_DemoWindow (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Demo window"); return 1; }
static int menu_cb_TheHistoryMechanism (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"History mechanism"); return 1; }
static int menu_cb_InitializationScripts (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Initialization script"); return 1; }
static int menu_cb_AddingToAFixedMenu (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Add to fixed menu..."); return 1; }
static int menu_cb_AddingToADynamicMenu (EDITOR_ARGS) { EDITOR_IAM (ScriptEditor); Melder_help (L"Add to dynamic menu..."); return 1; }

static void createMenus (ScriptEditor me) {
	inherited (ScriptEditor) createMenus (me);
	if (my editorClass) {
		Editor_addCommand (me, L"File", L"Add to menu...", 0, menu_cb_addToMenu);
	} else {
		Editor_addCommand (me, L"File", L"Add to fixed menu...", 0, menu_cb_addToFixedMenu);
		Editor_addCommand (me, L"File", L"Add to dynamic menu...", 0, menu_cb_addToDynamicMenu);
	}
	Editor_addCommand (me, L"File", L"-- close --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"-- history --", 0, 0);
	Editor_addCommand (me, L"Edit", L"Clear history", 0, menu_cb_clearHistory);
	Editor_addCommand (me, L"Edit", L"Paste history", 'H', menu_cb_pasteHistory);
	Editor_addCommand (me, L"Convert", L"-- expand --", 0, 0);
	Editor_addCommand (me, L"Convert", L"Expand include files", 0, menu_cb_expandIncludeFiles);
	Editor_addMenu (me, L"Run", 0);
	Editor_addCommand (me, L"Run", L"Run", 'R', menu_cb_run);
	Editor_addCommand (me, L"Run", L"Run selection", 'T', menu_cb_runSelection);
}

static void createHelpMenuItems (ScriptEditor me, EditorMenu menu) {
	inherited (ScriptEditor) createHelpMenuItems (me, menu);
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

class_methods (ScriptEditor, TextEditor) {
	class_method (destroy)
	class_method (nameChanged)
	class_method (goAway)
	class_method (createMenus)
	class_method (createHelpMenuItems)
	us -> scriptable = false;
	class_methods_end
}

ScriptEditor ScriptEditor_createFromText (GuiObject parent, Any voidEditor, const wchar_t *initialText) {
	try {
		Editor editor = (Editor) voidEditor;
		autoScriptEditor me = Thing_new (ScriptEditor);
		if (editor != NULL) {
			my environmentName = Melder_wcsdup (editor -> name);
			my editorClass = editor -> methods;
		}
		TextEditor_init (me.peek(), parent, initialText); therror
		my interpreter = Interpreter_createFromEnvironment (editor); therror
		if (theScriptEditors == NULL) {
			theScriptEditors = Collection_create (NULL, 10); therror
			Collection_dontOwnItems (theScriptEditors);
		}
		Collection_addItem (theScriptEditors, me.peek()); therror
		return me.transfer();
	} catch (MelderError) {
		rethrowmzero ("Script window not created.");
	}
}

ScriptEditor ScriptEditor_createFromScript (GuiObject parent, Any voidEditor, Script script) {
	try {
		if (theScriptEditors) {
			for (long ieditor = 1; ieditor <= theScriptEditors -> size; ieditor ++) {
				ScriptEditor editor = (ScriptEditor) theScriptEditors -> item [ieditor];
				if (MelderFile_equal (& script -> file, & editor -> file)) {
					Editor_raise (editor);
					Melder_error3 (L"Script ", MelderFile_messageName (& script -> file), L" is already open.");
					return NULL;
				}
			}
		}
		autostring text = MelderFile_readText (& script -> file);
		autoScriptEditor me = ScriptEditor_createFromText (parent, voidEditor, text.peek());
		MelderFile_copy (& script -> file, & my file);
		Thing_setName (me.peek(), Melder_fileToPath (& script -> file)); therror
		return me.transfer();
	} catch (MelderError) {
		rethrowmzero ("Script window not created.");
	}
}

/* End of file ScriptEditor.cpp */
