/* ScriptEditor.c
 *
 * Copyright (C) 1997-2004 Paul Boersma
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
 */

#include "ScriptEditor.h"
#include "longchar.h"
#include "praatP.h"
#include "EditorM.h"

static Collection theScriptEditors;

int ScriptEditors_dirty (void) {
	long i;
	if (! theScriptEditors) return FALSE;
	for (i = 1; i <= theScriptEditors -> size; i ++) {
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
	strcpy (Melder_buffer1, my name ? "Script" : "untitled script");
	if (my editorClass) {
		strcat (Melder_buffer1, " [");
		strcat (Melder_buffer1, my environmentName);
		strcat (Melder_buffer1, "]");
	}
	if (my name) {
		strcat (Melder_buffer1, " \\\"l");
		strcat (Melder_buffer1, MelderFile_messageName (& my file));
		strcat (Melder_buffer1, "\\\"r");
	}
	if (my dirty && ! dirtinessAlreadyShown)
		strcat (Melder_buffer1, " (modified)");
	Longchar_nativize (Melder_buffer1, Melder_buffer2, TRUE);
	XtVaSetValues (my shell, XmNtitle, Melder_buffer2, XmNiconName, "Script", NULL);
}

static int args_ok (Any dia, I) {
	iam (ScriptEditor);
	structMelderFile file;
	char *text = XmTextGetString (my textWidget);
	if (my name) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (& text, 2);

	Interpreter_getArgumentsFromDialog (my interpreter, dia);

	praat_background ();
	if (my name) MelderFile_setDefaultDir (& file);   /* BUG if two disks have the same name (on Mac). */
	Interpreter_run (my interpreter, text);
	praat_foreground ();
	XtFree (text);
	iferror return 0;
	return 1;
}

static void run (ScriptEditor me, char **text) {
	structMelderFile file;
	int npar;
	if (my name) {
		Melder_pathToFile (my name, & file);
		MelderFile_setDefaultDir (& file);
	}
	Melder_includeIncludeFiles (text, 2);
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
	char *text = XmTextGetString (my textWidget);
	run (me, & text);
	XtFree (text);
END

DIRECT (ScriptEditor, cb_runSelection)
	char *text = XmTextGetSelection (my textWidget);
	if (! text) {
		XtFree (text);
		return Melder_error ("No text selected.");
	}
	run (me, & text);
	XtFree (text);
END

FORM (ScriptEditor, cb_addToMenu, "Add to menu", "Add to fixed menu...");
	WORD ("Window", "?")
	SENTENCE ("Menu", "File")
	SENTENCE ("Command", "Do it...")
	SENTENCE ("After command", "")
	INTEGER ("Depth", "0")
	LABEL ("", "Script file:")
	TEXTFIELD ("Script", "")
	OK
if (my editorClass) SET_STRING ("Window", my editorClass -> _className)
if (my name)
	SET_STRING ("Script", my name)
else
	SET_STRING ("Script", "(please save your script first)")
DO
	if (! praat_addMenuCommandScript (GET_STRING ("Window"),
		GET_STRING ("Menu"), GET_STRING ("Command"), GET_STRING ("After command"),
		GET_INTEGER ("Depth"), GET_STRING ("Script"))) return 0;
	praat_show ();
END

FORM (ScriptEditor, cb_addToFixedMenu, "Add to fixed menu", "Add to fixed menu...");
	RADIO ("Window", 1)
		RADIOBUTTON ("Objects")
		RADIOBUTTON ("Picture")
	SENTENCE ("Menu", "New")
	SENTENCE ("Command", "Do it...")
	SENTENCE ("After command", "")
	INTEGER ("Depth", "0")
	LABEL ("", "Script file:")
	TEXTFIELD ("Script", "")
	OK
if (my name)
	SET_STRING ("Script", my name)
else
	SET_STRING ("Script", "(please save your script first)")
DO
	if (! praat_addMenuCommandScript (GET_STRING ("Window"),
		GET_STRING ("Menu"), GET_STRING ("Command"), GET_STRING ("After command"),
		GET_INTEGER ("Depth"), GET_STRING ("Script"))) return 0;
	praat_show ();
END

FORM (ScriptEditor, cb_addToDynamicMenu, "Add to dynamic menu", "Add to dynamic menu...")
	WORD ("Class 1", "Sound")
	INTEGER ("Number 1", "0")
	WORD ("Class 2", "")
	INTEGER ("Number 2", "0")
	WORD ("Class 3", "")
	INTEGER ("Number 3", "0")
	SENTENCE ("Command", "Do it...")
	SENTENCE ("After command", "")
	INTEGER ("Depth", "0")
	LABEL ("", "Script file:")
	TEXTFIELD ("Script", "")
	OK
if (my name)
	SET_STRING ("Script", my name)
else
	SET_STRING ("Script", "(please save your script first)")
DO
	if (! praat_addActionScript (GET_STRING ("Class 1"), GET_INTEGER ("Number 1"),
		GET_STRING ("Class 2"), GET_INTEGER ("Number 2"), GET_STRING ("Class 3"),
		GET_INTEGER ("Number 3"), GET_STRING ("Command"), GET_STRING ("After command"),
		GET_INTEGER ("Depth"), GET_STRING ("Script"))) return 0;
	praat_show ();
END

DIRECT (ScriptEditor, cb_clearHistory)
	UiHistory_clear ();
END

DIRECT (ScriptEditor, cb_viewHistory)
	XmTextPosition first = 0, last = 0;
	char *history = UiHistory_get ();
	long length;
	if (! history || history [0] == '\0')
		return Melder_error ("No history.");
	length = strlen (history);
	if (history [length - 1] != '\n') {
		UiHistory_write ("\n");
		history = UiHistory_get ();
		length = strlen (history);
	}
	if (history [0] == '\n') {
		history ++;
		length --;
	}
	if (! XmTextGetSelectionPosition (my textWidget, & first, & last))
		first = last = XmTextGetInsertionPosition (my textWidget);
	XmTextReplace (my textWidget, first, last, history);
	#if defined (UNIX) || defined (macintosh)
		XmTextSetSelection (my textWidget, first, first + length, 0);
	#endif
END

DIRECT (ScriptEditor, cb_AboutScriptEditor) Melder_help ("ScriptEditor"); END
DIRECT (ScriptEditor, cb_ScriptingTutorial) Melder_help ("Scripting"); END
DIRECT (ScriptEditor, cb_ScriptingExamples) Melder_help ("Scripting examples"); END
DIRECT (ScriptEditor, cb_PraatScript) Melder_help ("Praat script"); END
DIRECT (ScriptEditor, cb_FormulasTutorial) Melder_help ("Formulas"); END
DIRECT (ScriptEditor, cb_TheHistoryMechanism) Melder_help ("History mechanism"); END
DIRECT (ScriptEditor, cb_InitializationScripts) Melder_help ("Initialization script"); END
DIRECT (ScriptEditor, cb_AddingToAFixedMenu) Melder_help ("Add to fixed menu..."); END
DIRECT (ScriptEditor, cb_AddingToADynamicMenu) Melder_help ("Add to dynamic menu..."); END

static void createMenus (I) {
	iam (ScriptEditor);
	inherited (ScriptEditor) createMenus (me);
	if (my editorClass) {
		Editor_addCommand (me, "File", "Add to menu...", 0, cb_addToMenu);
	} else {
		Editor_addCommand (me, "File", "Add to fixed menu...", 0, cb_addToFixedMenu);
		Editor_addCommand (me, "File", "Add to dynamic menu...", 0, cb_addToDynamicMenu);
	}
	Editor_addCommand (me, "File", "-- close --", 0, NULL);
	Editor_addCommand (me, "Edit", "-- history --", 0, 0);
	Editor_addCommand (me, "Edit", "Clear history", 0, cb_clearHistory);
	Editor_addCommand (me, "Edit", "Paste history", 'H', cb_viewHistory);
	Editor_addCommand (me, "Help", "About ScriptEditor", '?', cb_AboutScriptEditor);
	Editor_addCommand (me, "Help", "Scripting tutorial", 0, cb_ScriptingTutorial);
	Editor_addCommand (me, "Help", "Scripting examples", 0, cb_ScriptingExamples);
	Editor_addCommand (me, "Help", "Praat script", 0, cb_PraatScript);
	Editor_addCommand (me, "Help", "Formulas tutorial", 0, cb_FormulasTutorial);
	Editor_addCommand (me, "Help", "-- help history --", 0, NULL);
	Editor_addCommand (me, "Help", "The History mechanism", 0, cb_TheHistoryMechanism);
	Editor_addCommand (me, "Help", "Initialization scripts", 0, cb_InitializationScripts);
	Editor_addCommand (me, "Help", "-- help add --", 0, NULL);
	Editor_addCommand (me, "Help", "Adding to a fixed menu", 0, cb_AddingToAFixedMenu);
	Editor_addCommand (me, "Help", "Adding to a dynamic menu", 0, cb_AddingToADynamicMenu);
	Editor_addMenu (me, "Run", 0);
	Editor_addCommand (me, "Run", "Run", 'R', cb_go);
	Editor_addCommand (me, "Run", "Run selection", 0, cb_runSelection);
}

class_methods (ScriptEditor, TextEditor)
	class_method (destroy)
	class_method (nameChanged)
	class_method (createMenus)
	us -> scriptable = FALSE;
class_methods_end

ScriptEditor ScriptEditor_createFromText (Widget parent, Any voidEditor, const char *initialText) {
	Editor editor = (Editor) voidEditor;
	ScriptEditor me = new (ScriptEditor);
	if (! me) return NULL;
	if (editor) {
		my environmentName = Melder_strdup (editor -> name);
		my editorClass = editor -> methods;
	}
	if (! TextEditor_init (me, parent, initialText)) { forget (me); return NULL; }
	my interpreter = Interpreter_createFromEnvironment (editor);
	if (! theScriptEditors) theScriptEditors = Collection_create (NULL, 10);
	Collection_addItem (theScriptEditors, me);
	return me;
}

ScriptEditor ScriptEditor_createFromScript (Widget parent, Any voidEditor, Script script) {
	ScriptEditor me;
	char *text = MelderFile_readText (& script -> file);
	if (! text) return NULL;
	me = ScriptEditor_createFromText (parent, voidEditor, text);
	Melder_free (text);
	if (! me) return NULL;
	MelderFile_copy (& script -> file, & my file);
	Thing_setName (me, Melder_fileToPath (& script -> file));
	return me;
}

/* End of file ScriptEditor.c */
