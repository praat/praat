/* ScriptEditor.c
 *
 * Copyright (C) 1997-2008 Paul Boersma
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
		ScriptEditor me = theScriptEditors -> item [i];
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
		MelderString_append3 (& buffer, L" " UNITEXT_LEFT_DOUBLE_QUOTATION_MARK, MelderFile_messageNameW (& my file), UNITEXT_RIGHT_DOUBLE_QUOTATION_MARK);
	}
	if (my dirty && ! dirtinessAlreadyShown)
		MelderString_append (& buffer, L" (modified)");
	GuiWindow_setTitle (my shell, buffer.string);
	#if motif
	XtVaSetValues (my shell, XmNiconName, "Script", NULL);
	#endif
}

static int args_ok (Any dia, I) {
	iam (ScriptEditor);
	structMelderFile file = { 0 };
	wchar_t *text = GuiText_getString (my textWidget);
	if (my name) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text);

	Interpreter_getArgumentsFromDialog (my interpreter, dia);

	praat_background ();
	if (my name) MelderFile_setDefaultDir (& file);   /* BUG if two disks have the same name (on Mac). */
	Interpreter_run (my interpreter, text);
	praat_foreground ();
	Melder_free (text);
	iferror return 0;
	return 1;
}

static void run (ScriptEditor me, wchar_t **text) {
	structMelderFile file = { 0 };
	int npar;
	if (my name) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (text);
	iferror { Melder_flushError (NULL); return; }
	npar = Interpreter_readParameters (my interpreter, *text);
	iferror { Melder_flushError (NULL); return; }
	if (npar) {
		/*
		 * Pop up a dialog box for querying the arguments.
		 */
		forget (my argsDialog);
		my argsDialog = Interpreter_createForm (my interpreter, my shell, NULL, args_ok, me);
		UiForm_do (my argsDialog, 0);
	} else {
		praat_background ();
		if (my name) MelderFile_setDefaultDir (& file);   /* BUG if two disks have the same name (on Mac). */
		Interpreter_run (my interpreter, *text);
		praat_foreground ();
		iferror Melder_flushError (NULL);
	}
}

DIRECT (ScriptEditor, cb_go)
	wchar_t *text = GuiText_getString (my textWidget);
	run (me, & text);
	Melder_free (text);
END

DIRECT (ScriptEditor, cb_runSelection)
	wchar_t *text = GuiText_getSelection (my textWidget);
	if (! text) {
		Melder_free (text);
		return Melder_error1 (L"No text selected.");
	}
	run (me, & text);
	Melder_free (text);
END

FORM (ScriptEditor, cb_addToMenu, L"Add to menu", L"Add to fixed menu...");
	WORD (L"Window", L"?")
	SENTENCE (L"Menu", L"File")
	SENTENCE (L"Command", L"Do it...")
	SENTENCE (L"After command", L"")
	INTEGER (L"Depth", L"0")
	LABEL (L"", L"Script file:")
	TEXTFIELD (L"Script", L"")
	OK
if (my editorClass) SET_STRING (L"Window", my editorClass -> _classNameW)
if (my name)
	SET_STRING (L"Script", my name)
else
	SET_STRING (L"Script", L"(please save your script first)")
DO
	if (! praat_addMenuCommandScript (GET_STRING (L"Window"),
		GET_STRING (L"Menu"), GET_STRING (L"Command"), GET_STRING (L"After command"),
		GET_INTEGER (L"Depth"), GET_STRING (L"Script"))) return 0;
	praat_show ();
END

FORM (ScriptEditor, cb_addToFixedMenu, L"Add to fixed menu", L"Add to fixed menu...");
	RADIO (L"Window", 1)
		RADIOBUTTON (L"Objects")
		RADIOBUTTON (L"Picture")
	SENTENCE (L"Menu", L"New")
	SENTENCE (L"Command", L"Do it...")
	SENTENCE (L"After command", L"")
	INTEGER (L"Depth", L"0")
	LABEL (L"", L"Script file:")
	TEXTFIELD (L"Script", L"")
	OK
if (my name)
	SET_STRING (L"Script", my name)
else
	SET_STRING (L"Script", L"(please save your script first)")
DO
	if (! praat_addMenuCommandScript (GET_STRING (L"Window"),
		GET_STRING (L"Menu"), GET_STRING (L"Command"), GET_STRING (L"After command"),
		GET_INTEGER (L"Depth"), GET_STRING (L"Script"))) return 0;
	praat_show ();
END

FORM (ScriptEditor, cb_addToDynamicMenu, L"Add to dynamic menu", L"Add to dynamic menu...")
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
	OK
if (my name)
	SET_STRING (L"Script", my name)
else
	SET_STRING (L"Script", L"(please save your script first)")
DO
	if (! praat_addActionScript (GET_STRING (L"Class 1"), GET_INTEGER (L"Number 1"),
		GET_STRING (L"Class 2"), GET_INTEGER (L"Number 2"), GET_STRING (L"Class 3"),
		GET_INTEGER (L"Number 3"), GET_STRING (L"Command"), GET_STRING (L"After command"),
		GET_INTEGER (L"Depth"), GET_STRING (L"Script"))) return 0;
	praat_show ();
END

DIRECT (ScriptEditor, cb_clearHistory)
	UiHistory_clear ();
END

DIRECT (ScriptEditor, cb_viewHistory)
	long first = 0, last = 0;
	wchar_t *history = UiHistory_get ();
	long length;
	if (! history || history [0] == '\0')
		return Melder_error1 (L"No history.");
	length = wcslen (history);
	if (history [length - 1] != '\n') {
		UiHistory_write (L"\n");
		history = UiHistory_get ();
		length = wcslen (history);
	}
	if (history [0] == '\n') {
		history ++;
		length --;
	}
	GuiText_getSelectionPosition (my textWidget, & first, & last);
	GuiText_replace (my textWidget, first, last, history);
	#if defined (UNIX) || defined (macintosh)
		GuiText_setSelection (my textWidget, first, first + length);
	#endif
END

DIRECT (ScriptEditor, cb_AboutScriptEditor) Melder_help (L"ScriptEditor"); END
DIRECT (ScriptEditor, cb_ScriptingTutorial) Melder_help (L"Scripting"); END
DIRECT (ScriptEditor, cb_ScriptingExamples) Melder_help (L"Scripting examples"); END
DIRECT (ScriptEditor, cb_PraatScript) Melder_help (L"Praat script"); END
DIRECT (ScriptEditor, cb_FormulasTutorial) Melder_help (L"Formulas"); END
DIRECT (ScriptEditor, cb_TheHistoryMechanism) Melder_help (L"History mechanism"); END
DIRECT (ScriptEditor, cb_InitializationScripts) Melder_help (L"Initialization script"); END
DIRECT (ScriptEditor, cb_AddingToAFixedMenu) Melder_help (L"Add to fixed menu..."); END
DIRECT (ScriptEditor, cb_AddingToADynamicMenu) Melder_help (L"Add to dynamic menu..."); END

static void createMenus (I) {
	iam (ScriptEditor);
	inherited (ScriptEditor) createMenus (me);
	if (my editorClass) {
		Editor_addCommand (me, L"File", L"Add to menu...", 0, cb_addToMenu);
	} else {
		Editor_addCommand (me, L"File", L"Add to fixed menu...", 0, cb_addToFixedMenu);
		Editor_addCommand (me, L"File", L"Add to dynamic menu...", 0, cb_addToDynamicMenu);
	}
	Editor_addCommand (me, L"File", L"-- close --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"-- history --", 0, 0);
	Editor_addCommand (me, L"Edit", L"Clear history", 0, cb_clearHistory);
	Editor_addCommand (me, L"Edit", L"Paste history", 'H', cb_viewHistory);
	Editor_addMenu (me, L"Run", 0);
	Editor_addCommand (me, L"Run", L"Run", 'R', cb_go);
	Editor_addCommand (me, L"Run", L"Run selection", 0, cb_runSelection);
}

static void createHelpMenuItems (I, EditorMenu menu) {
	iam (ScriptEditor);
	inherited (ScriptEditor) createHelpMenuItems (me, menu);
	EditorMenu_addCommand (menu, L"About ScriptEditor", '?', cb_AboutScriptEditor);
	EditorMenu_addCommand (menu, L"Scripting tutorial", 0, cb_ScriptingTutorial);
	EditorMenu_addCommand (menu, L"Scripting examples", 0, cb_ScriptingExamples);
	EditorMenu_addCommand (menu, L"Praat script", 0, cb_PraatScript);
	EditorMenu_addCommand (menu, L"Formulas tutorial", 0, cb_FormulasTutorial);
	EditorMenu_addCommand (menu, L"-- help history --", 0, NULL);
	EditorMenu_addCommand (menu, L"The History mechanism", 0, cb_TheHistoryMechanism);
	EditorMenu_addCommand (menu, L"Initialization scripts", 0, cb_InitializationScripts);
	EditorMenu_addCommand (menu, L"-- help add --", 0, NULL);
	EditorMenu_addCommand (menu, L"Adding to a fixed menu", 0, cb_AddingToAFixedMenu);
	EditorMenu_addCommand (menu, L"Adding to a dynamic menu", 0, cb_AddingToADynamicMenu);
}

class_methods (ScriptEditor, TextEditor) {
	class_method (destroy)
	class_method (nameChanged)
	class_method (createMenus)
	class_method (createHelpMenuItems)
	us -> scriptable = FALSE;
	class_methods_end
}

ScriptEditor ScriptEditor_createFromText (Widget parent, Any voidEditor, const wchar_t *initialText) {
	Editor editor = (Editor) voidEditor;
	ScriptEditor me = new (ScriptEditor);
	if (! me) return NULL;
	if (editor) {
		my environmentName = Melder_wcsdup (editor -> name);
		my editorClass = editor -> methods;
	}
	if (! TextEditor_init (me, parent, initialText)) { forget (me); return NULL; }
	my interpreter = Interpreter_createFromEnvironment (editor);
	if (! theScriptEditors) theScriptEditors = Collection_create (NULL, 10);
	Collection_addItem (theScriptEditors, me);
	return me;
}

ScriptEditor ScriptEditor_createFromScript (Widget parent, Any voidEditor, Script script) {
	if (theScriptEditors) {
		for (long ieditor = 1; ieditor <= theScriptEditors -> size; ieditor ++) {
			TextEditor editor = theScriptEditors -> item [ieditor];
			if (MelderFile_equal (& script -> file, & editor -> file)) {
				Editor_raise (editor);
				Melder_error3 (L"Script ", MelderFile_messageNameW (& script -> file), L" is already open.");
				return NULL;
			}
		}
	}
	ScriptEditor me;
	wchar_t *text = MelderFile_readText (& script -> file);
	if (! text) return NULL;
	me = ScriptEditor_createFromText (parent, voidEditor, text);
	Melder_free (text);
	if (! me) return NULL;
	MelderFile_copy (& script -> file, & my file);
	Thing_setName (me, Melder_fileToPath (& script -> file));
	return me;
}

/* End of file ScriptEditor.c */
