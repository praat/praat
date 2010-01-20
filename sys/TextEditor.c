/* TextEditor.c
 *
 * Copyright (C) 1997-2010 Paul Boersma
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
 * pb 2004/01/07 use GuiWindow_setDirty
 * pb 2004/01/28 MacOS X: use trick for ensuring dirtiness callback
 * pb 2005/06/28 font size
 * pb 2005/09/01 Undo and Redo buttons
 * pb 2006/08/09 guarded against closing when a file selector is open
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/18 improved info
 * pb 2007/02/15 GuiText_updateChangeCountAfterSave
 * pb 2007/03/23 Go to line: guarded against uninitialized 'right'
 * pb 2007/05/30 save Unicode
 * pb 2007/06/12 more wchar_t
 * pb 2007/08/12 more wchar_t
 * pb 2007/10/05 less char
 * pb 2007/12/05 prefs
 * pb 2007/12/23 Gui
 * pb 2008/01/04 guard against multiple opening of same file
 * pb 2008/03/21 new Editor API
 * pb 2009/01/18 arguments to UiForm callbacks
 * pb 2010/01/20 Reopen from disk
 * pb 2010/01/20 guard against Find Again before Find
 */

#include "TextEditor.h"
#include "machine.h"
#include "longchar.h"
#include "EditorM.h"
#include "Preferences.h"
#include "UnicodeData.h"

static int theTextEditorFontSize;

void TextEditor_prefs (void) {
	Preferences_addInt (L"TextEditor.fontSize", & theTextEditorFontSize, 12);
}

static Collection theOpenTextEditors = NULL;

/***** TextEditor methods *****/

static void classTextEditor_destroy (I) {
	iam (TextEditor);
	forget (my openDialog);
	forget (my saveDialog);
	forget (my printDialog);
	forget (my findDialog);
	if (theOpenTextEditors) {
		Collection_undangleItem (theOpenTextEditors, me);
	}
	inherited (TextEditor) destroy (me);
}

static void classTextEditor_nameChanged (I) {
	iam (TextEditor);
	if (our fileBased) {
		int dirtinessAlreadyShown = GuiWindow_setDirty (my shell, my dirty);
		static MelderString windowTitle = { 0 };
		MelderString_empty (& windowTitle);
		if (my name == NULL) {
			MelderString_append (& windowTitle, L"(untitled");
			if (my dirty && ! dirtinessAlreadyShown) MelderString_append (& windowTitle, L", modified");
			MelderString_append (& windowTitle, L")");
		} else {
			MelderString_append3 (& windowTitle, L"File " UNITEXT_LEFT_DOUBLE_QUOTATION_MARK, MelderFile_messageName (& my file), UNITEXT_RIGHT_DOUBLE_QUOTATION_MARK);
			if (my dirty && ! dirtinessAlreadyShown) MelderString_append (& windowTitle, L" (modified)");
		}
		GuiWindow_setTitle (my shell, windowTitle.string);
		MelderString_empty (& windowTitle);
		MelderString_append2 (& windowTitle, my dirty && ! dirtinessAlreadyShown ? L"*" : L"", my name == NULL ? L"(untitled)" : MelderFile_name (& my file));
		#if motif	
			XtVaSetValues (my shell, XmNiconName, Melder_peekWcsToUtf8 (windowTitle.string), NULL);
		#endif
	} else {
		inherited (TextEditor) nameChanged (me);
	}
}

static int openDocument (TextEditor me, MelderFile file) {
	if (theOpenTextEditors) {
		for (long ieditor = 1; ieditor <= theOpenTextEditors -> size; ieditor ++) {
			TextEditor editor = theOpenTextEditors -> item [ieditor];
			if (editor != me && MelderFile_equal (file, & editor -> file)) {
				Editor_raise (TextEditor_as_Editor (editor));
				Melder_error3 (L"Text file ", MelderFile_messageName (file), L" is already open.");
				forget (me);   // don't forget me before Melder_error, because "file" is owned by one of my dialogs
				return 0;
			}
		}
	}
	wchar_t *text = MelderFile_readText (file);
	if (! text) return 0;
	GuiText_setString (my textWidget, text);
	Melder_free (text);
	/*
	 * GuiText_setString has invoked the changeCallback,
	 * which has set 'my dirty' to TRUE. Fix this.
	 */
	my dirty = FALSE;
	MelderFile_copy (file, & my file);
	Thing_setName (me, Melder_fileToPath (file));
	return 1;
}

static void newDocument (TextEditor me) {
	GuiText_setString (my textWidget, L"");   /* Implicitly sets my dirty to TRUE. */
	my dirty = FALSE;
	if (our fileBased) Thing_setName (me, NULL);
}

static int saveDocument (TextEditor me, MelderFile file) {
	wchar_t *text = GuiText_getString (my textWidget);
	if (! MelderFile_writeText (file, text)) { Melder_free (text); return 0; }
	Melder_free (text);
	my dirty = FALSE;
	MelderFile_copy (file, & my file);
	if (our fileBased) Thing_setName (me, Melder_fileToPath (file));
	return 1;
}

static void closeDocument (TextEditor me) {
	forget (me);
}

static int cb_open_ok (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, I) {
	iam (TextEditor);
	(void) sendingString;
	(void) interpreter;
	(void) invokingButtonTitle;
	(void) modified;
	MelderFile file = UiFile_getFile (sendingForm);
	if (! openDocument (me, file)) return 0;
	return 1;
}

static void cb_showOpen (EditorCommand cmd, UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter) {
	TextEditor me = (TextEditor) cmd -> editor;
	(void) sendingForm;
	(void) sendingString;
	(void) interpreter;
	if (! my openDialog)
		my openDialog = UiInfile_create (my dialog, L"Open", cb_open_ok, me, NULL, NULL, false);
	UiInfile_do (my openDialog);
}

static int cb_saveAs_ok (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, I) {
	iam (TextEditor);
	(void) sendingString;
	(void) interpreter;
	(void) invokingButtonTitle;
	(void) modified;
	MelderFile file = UiFile_getFile (sendingForm);
	if (! saveDocument (me, file)) return 0;
	return 1;
}

static int menu_cb_saveAs (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	wchar_t defaultName [300];
	if (! my saveDialog)
		my saveDialog = UiOutfile_create (my dialog, L"Save", cb_saveAs_ok, me, NULL, NULL);
	swprintf (defaultName, 300, ! our fileBased ? L"info.txt" : my name ? MelderFile_name (& my file) : L"");
	UiOutfile_do (my saveDialog, defaultName);
	return 1;
}

static void gui_button_cb_saveAndOpen (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	GuiObject_hide (my dirtyOpenDialog);
	if (my name) {
		if (! saveDocument (me, & my file)) { Melder_flushError (NULL); return; }
		cb_showOpen (cmd, NULL, NULL, NULL);
	} else {
		menu_cb_saveAs (me, cmd, NULL, NULL, NULL);
	}
}

static void gui_button_cb_cancelOpen (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	GuiObject_hide (my dirtyOpenDialog);
}

static void gui_button_cb_discardAndOpen (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	GuiObject_hide (my dirtyOpenDialog);
	cb_showOpen (cmd, NULL, NULL, NULL);
}

static int menu_cb_open (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	if (my dirty) {
		if (! my dirtyOpenDialog) {
			Widget form, buttons;
			my dirtyOpenDialog = GuiDialog_create (my shell, 150, 70, 420, Gui_AUTOMATIC, L"Text changed", NULL, NULL, GuiDialog_MODAL);
			#if gtk
				  form = GTK_DIALOG (my dirtyOpenDialog) -> vbox;
				  buttons = GTK_DIALOG (my dirtyOpenDialog) -> action_area;
			#elif motif
				  form = my dirtyOpenDialog;    /* TODO: Kan dit ook met een define? */
				  buttons = my dirtyOpenDialog;
			#endif
			#if gtk
			GuiLabel_createShown (form, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Save changes?", 0);
			GuiButton_createShown (buttons, 10, 130, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Discard & Open", gui_button_cb_discardAndOpen, cmd, 0);
			GuiButton_createShown (buttons, 150, 270, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Cancel", gui_button_cb_cancelOpen, cmd, 0);
			GuiButton_createShown (buttons, 290, 410, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Save & Open", gui_button_cb_saveAndOpen, cmd, 0);
			#elif motif
			form = GuiColumn_createShown (my dirtyOpenDialog, 0);
			GuiLabel_createShown (form, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Save changes?", 0);
			buttons = GuiRow_createShown (form, Gui_HOMOGENEOUS);
			GuiButton_createShown (buttons, 10, 130, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Discard & Open", gui_button_cb_discardAndOpen, cmd, 0);
			GuiButton_createShown (buttons, 150, 270, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Cancel", gui_button_cb_cancelOpen, cmd, 0);
			GuiButton_createShown (buttons, 290, 410, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Save & Open", gui_button_cb_saveAndOpen, cmd, 0);
			#endif
		}
		GuiDialog_show (my dirtyOpenDialog);
	} else {
		cb_showOpen (cmd, sendingForm, sendingString, interpreter);
	}
	return 1;
}

static void gui_button_cb_saveAndNew (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	GuiObject_hide (my dirtyNewDialog);
	if (my name) {
		if (! saveDocument (me, & my file)) { Melder_flushError (NULL); return; }
		newDocument (me);
	} else {
		menu_cb_saveAs (me, cmd, NULL, NULL, NULL);
	}
}

static void gui_button_cb_cancelNew (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	GuiObject_hide (my dirtyNewDialog);
}

static void gui_button_cb_discardAndNew (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	GuiObject_hide (my dirtyNewDialog);
	newDocument (me);
}

static int menu_cb_new (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	if (our fileBased && my dirty) {
		if (! my dirtyNewDialog) {
			my dirtyNewDialog = GuiDialog_create (my shell, 150, 70, 420, Gui_AUTOMATIC, L"Text changed", NULL, NULL, GuiDialog_MODAL);
			Widget column1 = GuiColumn_createShown (my dirtyNewDialog, 0);
			GuiLabel_createShown (column1, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Save changes?", 0);
			Widget row1 = GuiRow_createShown (column1, Gui_HOMOGENEOUS);
			GuiButton_createShown (row1, 10, 130, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Discard & New", gui_button_cb_discardAndNew, cmd, 0);
			GuiButton_createShown (row1, 150, 270, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Cancel", gui_button_cb_cancelNew, cmd, 0);
			GuiButton_createShown (row1, 290, 410, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Save & New", gui_button_cb_saveAndNew, cmd, 0);
		}
		GuiDialog_show (my dirtyNewDialog);
	} else {
		newDocument (me);
	}
	return 1;
}

static int menu_cb_clear (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	our clear (me);
	return 1;
}

static int menu_cb_save (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	if (my name) {
		if (! saveDocument (me, & my file)) return 0;
	} else {
		menu_cb_saveAs (me, cmd, NULL, NULL, NULL);
	}
	return 1;
}

static int menu_cb_reopen (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	if (my name) {
		if (! openDocument (me, & my file)) return 0;
	} else {
		return Melder_error1 (L"Cannot reopen from disk, because the text has never been saved yet.");
	}
	return 1;
}

static void gui_button_cb_saveAndClose (I, GuiButtonEvent event) {
	(void) event;
	iam (TextEditor);
	GuiObject_hide (my dirtyCloseDialog);
	if (my name) {
		if (! saveDocument (me, & my file)) { Melder_flushError (NULL); return; }
		closeDocument (me);
	} else {
		menu_cb_saveAs (me, Editor_getMenuCommand (TextEditor_as_Editor (me), L"File", L"Save as..."), NULL, NULL, NULL);
	}
}

static void gui_button_cb_discardAndClose (I, GuiButtonEvent event) {
	(void) event;
	iam (TextEditor);
	GuiObject_hide (my dirtyCloseDialog);
	closeDocument (me);
}

static void classTextEditor_goAway (TextEditor me) {
	if (our fileBased && my dirty) {
		if (! my dirtyCloseDialog) {
			my dirtyCloseDialog = GuiDialog_create (my shell, 150, 70, 290, Gui_AUTOMATIC, L"Text changed", NULL, NULL, GuiDialog_MODAL);
			Widget column1 = GuiColumn_createShown (my dirtyCloseDialog, 0);
			GuiLabel_createShown (column1, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Save changes?", 0);
			Widget row1 = GuiRow_createShown (column1, Gui_HOMOGENEOUS);
			GuiButton_createShown (row1, 10, 130, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Discard & Close", gui_button_cb_discardAndClose, me, 0);
			GuiButton_createShown (row1, 150, 270, Gui_AUTOMATIC, Gui_AUTOMATIC, L"Save & Close", gui_button_cb_saveAndClose, me, 0);
		}
		GuiDialog_show (my dirtyCloseDialog);
	} else {
		closeDocument (me);
	}
}

static int menu_cb_undo (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	GuiText_undo (my textWidget);
	return 1;
}

static int menu_cb_redo (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	GuiText_redo (my textWidget);
	return 1;
}

static int menu_cb_cut (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	GuiText_cut (my textWidget);  // use ((XmAnyCallbackStruct *) call) -> event -> xbutton. time
	return 1;
}

static int menu_cb_copy (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	GuiText_copy (my textWidget);
	return 1;
}

static int menu_cb_paste (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	GuiText_paste (my textWidget);
	return 1;
}

static int menu_cb_erase (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	GuiText_remove (my textWidget);
	return 1;
}

static int getSelectedLines (TextEditor me, long *firstLine, long *lastLine) {
	long left, right;
	wchar_t *text = GuiText_getStringAndSelectionPosition (my textWidget, & left, & right);
	long i;
	*firstLine = 1;
	/*
	 * Cycle through the text in order to see how many linefeeds we pass.
	 */
	for (i = 0; i < left; i ++) {
		if (text [i] == '\n') {
			(*firstLine) ++;
		}
	}
	Melder_assert (left <= (long) wcslen (text));
	if (left == right) return FALSE;
	*lastLine = *firstLine;
	for (; i < right; i ++) {
		if (text [i] == '\n') {
			(*lastLine) ++;
		}
	}
	Melder_assert (right <= (long) wcslen (text));
	Melder_free (text);
	return TRUE;
}

static wchar_t *theFindString = NULL, *theReplaceString = NULL;
static void do_find (TextEditor me) {
	if (theFindString == NULL) return;   // e.g. when the user does "Find again" before having done any "Find"
	long left, right;
	wchar_t *text = GuiText_getStringAndSelectionPosition (my textWidget, & left, & right);
	wchar_t *location = wcsstr (text + right, theFindString);
	if (location != NULL) {
		long index = location - text;
		GuiText_setSelection (my textWidget, index, index + wcslen (theFindString));
		GuiText_scrollToSelection (my textWidget);
	} else {
		/* Try from the start of the document. */
		location = wcsstr (text, theFindString);
		if (location != NULL) {
			long index = location - text;
			GuiText_setSelection (my textWidget, index, index + wcslen (theFindString));
			GuiText_scrollToSelection (my textWidget);
		} else {
			Melder_beep ();
		}
	}
	Melder_free (text);
}

static void do_replace (TextEditor me) {
	if (theReplaceString == NULL) return;   // e.g. when the user does "Replace again" before having done any "Replace"
	long left, right;
	wchar_t *text = GuiText_getStringAndSelectionPosition (my textWidget, & left, & right);
	wchar_t *selection = GuiText_getSelection (my textWidget);
	if (! Melder_wcsequ (selection, theFindString)) {
		Melder_free (text);
		do_find (me);
		return;
	}
	wchar_t *newText = Melder_calloc (wchar_t, wcslen (text) - wcslen (selection) + wcslen (theReplaceString) + 1);
	wcsncpy (newText, text, left);
	wcscpy (newText + left, theReplaceString);
	wcscpy (newText + left + wcslen (theReplaceString), text + right);
	GuiText_setString (my textWidget, newText);
	GuiText_setSelection (my textWidget, left, left + wcslen (theReplaceString));
	GuiText_scrollToSelection (my textWidget);
	Melder_free (text);
	Melder_free (newText);
}

static int menu_cb_find (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	EDITOR_FORM (L"Find", 0)
		LABEL (L"", L"Find:")
		TEXTFIELD (L"findString", L"")
	EDITOR_OK
		if (theFindString != NULL) SET_STRING (L"findString", theFindString);
	EDITOR_DO
		Melder_free (theFindString);
		theFindString = Melder_wcsdup (GET_STRING (L"findString"));
		do_find (me);
	EDITOR_END
}

static int menu_cb_findAgain (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	do_find (me);
	return 1;
}

static int menu_cb_replace (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	EDITOR_FORM (L"Find", 0)
		LABEL (L"", L"This is a \"slow\" find-and-replace method;")
		LABEL (L"", L"if the selected text is identical to the Find string,")
		LABEL (L"", L"the selected text will be replaced by the Replace string;")
		LABEL (L"", L"otherwise, the next occurrence of the Find string will be selected.")
		LABEL (L"", L"So you typically need two clicks on Apply to get a text replaced.")
		LABEL (L"", L"Find:")
		TEXTFIELD (L"findString", L"")
		LABEL (L"", L"Replace with:")
		TEXTFIELD (L"replaceString", L"")
	EDITOR_OK
		if (theFindString != NULL) SET_STRING (L"findString", theFindString);
		if (theReplaceString != NULL) SET_STRING (L"replaceString", theReplaceString);
	EDITOR_DO
		Melder_free (theFindString);
		theFindString = Melder_wcsdup (GET_STRING (L"findString"));
		Melder_free (theReplaceString);
		theReplaceString = Melder_wcsdup (GET_STRING (L"replaceString"));
		do_replace (me);
	EDITOR_END
}

static int menu_cb_replaceAgain (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	do_replace (me);
	return 1;
}

static int menu_cb_whereAmI (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	long numberOfLinesLeft, numberOfLinesRight;
	if (! getSelectedLines (me, & numberOfLinesLeft, & numberOfLinesRight)) {
		Melder_information3 (L"The cursor is on line ", Melder_integer (numberOfLinesLeft), L".");
	} else if (numberOfLinesLeft == numberOfLinesRight) {
		Melder_information3 (L"The selection is on line ", Melder_integer (numberOfLinesLeft), L".");
	} else {
		Melder_information5 (L"The selection runs from line ", Melder_integer (numberOfLinesLeft),
			L" to line ", Melder_integer (numberOfLinesRight), L".");
	}
	return 1;
}

static int menu_cb_goToLine (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	EDITOR_FORM (L"Go to line", 0)
		NATURAL (L"Line", L"1")
	EDITOR_OK
		long firstLine, lastLine;
		getSelectedLines (me, & firstLine, & lastLine);
		SET_INTEGER (L"Line", firstLine);
	EDITOR_DO
		wchar_t *text = GuiText_getString (my textWidget);
		long lineToGo = GET_INTEGER (L"Line"), currentLine = 1;
		unsigned long left = 0, right = 0;
		if (lineToGo == 1) {
			for (; text [right] != '\n' && text [right] != '\0'; right ++) { }
		} else {
			for (; text [left] != '\0'; left ++) {
				if (text [left] == '\n') {
					currentLine ++;
					if (currentLine == lineToGo) {
						left ++;
						for (right = left; text [right] != '\n' && text [right] != '\0'; right ++) { }
						break;
					}
				}
			}
		}
		if (left == wcslen (text)) {
			right = left;
		} else if (text [right] == '\n') {
			right ++;
		}
		Melder_free (text);
		GuiText_setSelection (my textWidget, left, right);
		GuiText_scrollToSelection (my textWidget);
	EDITOR_END
}

/***** 'Font' menu *****/

static void updateSizeMenu (TextEditor me) {
	#if motif
	if (my fontSizeButton_10) XmToggleButtonGadgetSetState (my fontSizeButton_10, my fontSize == 10, 0);
	if (my fontSizeButton_12) XmToggleButtonGadgetSetState (my fontSizeButton_12, my fontSize == 12, 0);
	if (my fontSizeButton_14) XmToggleButtonGadgetSetState (my fontSizeButton_14, my fontSize == 14, 0);
	if (my fontSizeButton_18) XmToggleButtonGadgetSetState (my fontSizeButton_18, my fontSize == 18, 0);
	if (my fontSizeButton_24) XmToggleButtonGadgetSetState (my fontSizeButton_24, my fontSize == 24, 0);
	#endif
}
static void setFontSize (TextEditor me, int fontSize) {
	GuiText_setFontSize (my textWidget, fontSize);
	theTextEditorFontSize = my fontSize = fontSize;
	updateSizeMenu (me);
}

static int menu_cb_10 (EDITOR_ARGS) { EDITOR_IAM (TextEditor); setFontSize (me, 10); return 1; }
static int menu_cb_12 (EDITOR_ARGS) { EDITOR_IAM (TextEditor); setFontSize (me, 12); return 1; }
static int menu_cb_14 (EDITOR_ARGS) { EDITOR_IAM (TextEditor); setFontSize (me, 14); return 1; }
static int menu_cb_18 (EDITOR_ARGS) { EDITOR_IAM (TextEditor); setFontSize (me, 18); return 1; }
static int menu_cb_24 (EDITOR_ARGS) { EDITOR_IAM (TextEditor); setFontSize (me, 24); return 1; }
static int menu_cb_fontSize (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	EDITOR_FORM (L"Text window: Font size", 0)
		NATURAL (L"Font size (points)", L"12")
	EDITOR_OK
		SET_INTEGER (L"Font size", (long) my fontSize);
	EDITOR_DO
		setFontSize (me, GET_INTEGER (L"Font size"));
	EDITOR_END
}

static void classTextEditor_createMenus (TextEditor me) {
	inherited (TextEditor) createMenus (TextEditor_as_Editor (me));
	if (our fileBased) {
		Editor_addCommand (me, L"File", L"New", 'N', menu_cb_new);
		Editor_addCommand (me, L"File", L"Open...", 'O', menu_cb_open);
		Editor_addCommand (me, L"File", L"Reopen from disk", 0, menu_cb_reopen);
	} else {
		Editor_addCommand (me, L"File", L"Clear", 'N', menu_cb_clear);
	}
	Editor_addCommand (me, L"File", L"-- save --", 0, NULL);
	if (our fileBased) {
		Editor_addCommand (me, L"File", L"Save", 'S', menu_cb_save);
		Editor_addCommand (me, L"File", L"Save as...", 0, menu_cb_saveAs);
	} else {
		Editor_addCommand (me, L"File", L"Save as...", 'S', menu_cb_saveAs);
	}
	Editor_addCommand (me, L"File", L"-- close --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Undo", 'Z', menu_cb_undo);
	Editor_addCommand (me, L"Edit", L"Redo", 'Y', menu_cb_redo);
	Editor_addCommand (me, L"Edit", L"-- cut copy paste --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Cut", 'X', menu_cb_cut);
	Editor_addCommand (me, L"Edit", L"Copy", 'C', menu_cb_copy);
	Editor_addCommand (me, L"Edit", L"Paste", 'V', menu_cb_paste);
	Editor_addCommand (me, L"Edit", L"Erase", 0, menu_cb_erase);
	Editor_addMenu (me, L"Search", 0);
	Editor_addCommand (me, L"Search", L"Find...", 'F', menu_cb_find);
	Editor_addCommand (me, L"Search", L"Find again", 'G', menu_cb_findAgain);
	Editor_addCommand (me, L"Search", L"Replace...", GuiMenu_SHIFT + 'F', menu_cb_replace);
	Editor_addCommand (me, L"Search", L"Replace again", GuiMenu_SHIFT + 'G', menu_cb_replaceAgain);
	Editor_addCommand (me, L"Search", L"-- line --", 0, NULL);
	Editor_addCommand (me, L"Search", L"Where am I?", 0, menu_cb_whereAmI);
	Editor_addCommand (me, L"Search", L"Go to line...", 'L', menu_cb_goToLine);
	#ifdef macintosh
		Editor_addMenu (me, L"Font", 0);
		my fontSizeButton_10 = Editor_addCommand (me, L"Font", L"10", GuiMenu_RADIO_FIRST, menu_cb_10);
		my fontSizeButton_12 = Editor_addCommand (me, L"Font", L"12", GuiMenu_RADIO_NEXT, menu_cb_12);
		my fontSizeButton_14 = Editor_addCommand (me, L"Font", L"14", GuiMenu_RADIO_NEXT, menu_cb_14);
		my fontSizeButton_18 = Editor_addCommand (me, L"Font", L"18", GuiMenu_RADIO_NEXT, menu_cb_18);
		my fontSizeButton_24 = Editor_addCommand (me, L"Font", L"24", GuiMenu_RADIO_NEXT, menu_cb_24);
		Editor_addCommand (me, L"Font", L"Font size...", 0, menu_cb_fontSize);
	#endif
}

static void gui_text_cb_change (I, GuiTextEvent event) {
	(void) event;
	iam (TextEditor);
	if (! my dirty) {
		my dirty = TRUE;
		our nameChanged (me);
	}
}

static void classTextEditor_createChildren (TextEditor me) {
	my textWidget = GuiText_createShown (my dialog, 0, 0, Machine_getMenuBarHeight (), 0, GuiText_SCROLLED);
	GuiText_setChangeCallback (my textWidget, gui_text_cb_change, me);
}

static void classTextEditor_clear (TextEditor me) {
	(void) me;
}

class_methods (TextEditor, Editor) {
	class_method_local (TextEditor, destroy)
	class_method_local (TextEditor, nameChanged)
	class_method_local (TextEditor, goAway)
	class_method_local (TextEditor, createChildren)
	class_method_local (TextEditor, createMenus)
	us -> createMenuItems_query = NULL;
	us -> fileBased = true;
	class_method_local (TextEditor, clear)
	class_methods_end
}

int TextEditor_init (TextEditor me, Widget parent, const wchar_t *initialText) {
	Editor_init (TextEditor_as_parent (me), parent, 0, 0, 600, 400, NULL, NULL); cherror
	setFontSize (me, theTextEditorFontSize);
	if (initialText) {
		GuiText_setString (my textWidget, initialText);
		my dirty = FALSE;   /* Was set to TRUE in valueChanged callback. */
		Thing_setName (me, NULL);
	}
	if (theOpenTextEditors == NULL) {
		theOpenTextEditors = Collection_create (classTextEditor, 100);
	}
	if (theOpenTextEditors != NULL) {
		Collection_addItem (theOpenTextEditors, me);
	}
end:
	iferror return 0;
	return 1;
}

TextEditor TextEditor_create (Widget parent, const wchar_t *initialText) {
	TextEditor me = new (TextEditor);
	if (! me || ! TextEditor_init (me, parent, initialText)) { forget (me); return NULL; }
	return me;
}

void TextEditor_showOpen (TextEditor me) {
	cb_showOpen (Editor_getMenuCommand (TextEditor_as_Editor (me), L"File", L"Open..."), NULL, NULL, NULL);
}

/* End of file TextEditor.c */
