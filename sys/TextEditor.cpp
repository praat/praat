/* TextEditor.cpp
 *
 * Copyright (C) 1997-2012,2013 Paul Boersma, 2010 Franz Brausse
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

#include "TextEditor.h"
#include "machine.h"
#include "longchar.h"
#include "EditorM.h"
#include "UnicodeData.h"

Thing_implement (TextEditor, Editor, 0);

#include "prefs_define.h"
#include "TextEditor_prefs.h"
#include "prefs_install.h"
#include "TextEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "TextEditor_prefs.h"

static Collection theOpenTextEditors = NULL;

/***** TextEditor methods *****/

void structTextEditor :: v_destroy () {
	forget (openDialog);
	forget (saveDialog);
	forget (printDialog);
	forget (findDialog);
	if (theOpenTextEditors) {
		Collection_undangleItem (theOpenTextEditors, this);
	}
	TextEditor_Parent :: v_destroy ();
}

void structTextEditor :: v_nameChanged () {
	if (v_fileBased ()) {
		bool dirtinessAlreadyShown = d_windowForm -> f_setDirty (dirty);
		static MelderString windowTitle = { 0 };
		MelderString_empty (& windowTitle);
		if (name [0] == '\0') {
			MelderString_append (& windowTitle, L"(untitled");
			if (dirty && ! dirtinessAlreadyShown)
				MelderString_append (& windowTitle, L", modified");
			MelderString_append (& windowTitle, L")");
		} else {
			MelderString_append (& windowTitle, L"File ", MelderFile_messageName (& file));
			if (dirty && ! dirtinessAlreadyShown)
				MelderString_append (& windowTitle, L" (modified)");
		}
		d_windowForm -> f_setTitle (windowTitle.string);
		MelderString_empty (& windowTitle);
		MelderString_append (& windowTitle, dirty && ! dirtinessAlreadyShown ? L"*" : L"", name [0] == '\0' ? L"(untitled)" : MelderFile_name (& file));
	} else {
		TextEditor_Parent :: v_nameChanged ();
	}
}

static void openDocument (TextEditor me, MelderFile file) {
	if (theOpenTextEditors) {
		for (long ieditor = 1; ieditor <= theOpenTextEditors -> size; ieditor ++) {
			TextEditor editor = (TextEditor) theOpenTextEditors -> item [ieditor];
			if (editor != me && MelderFile_equal (file, & editor -> file)) {
				editor -> raise ();
				Melder_error_ ("Text file ", file, " is already open.");
				forget (me);   // don't forget me before Melder_error_, because "file" is owned by one of my dialogs
				Melder_flushError (NULL);
				return;
			}
		}
	}
	autostring text = MelderFile_readText (file);
	my textWidget -> f_setString (text.peek());
	/*
	 * GuiText_setString has invoked the changeCallback,
	 * which has set 'my dirty' to TRUE. Fix this.
	 */
	my dirty = FALSE;
	MelderFile_copy (file, & my file);
	Thing_setName (me, Melder_fileToPath (file));
}

static void newDocument (TextEditor me) {
	my textWidget -> f_setString (L"");   // implicitly sets my dirty to TRUE
	my dirty = FALSE;
	if (my v_fileBased ()) Thing_setName (me, L"");
}

static void saveDocument (TextEditor me, MelderFile file) {
	autostring text = my textWidget -> f_getString ();
	MelderFile_writeText (file, text.peek(), Melder_getOutputEncoding ());
	my dirty = FALSE;
	MelderFile_copy (file, & my file);
	if (my v_fileBased ()) Thing_setName (me, Melder_fileToPath (file));
}

static void closeDocument (TextEditor me) {
	forget (me);
}

static void cb_open_ok (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, I) {
	iam (TextEditor);
	(void) sendingString;
	(void) interpreter;
	(void) invokingButtonTitle;
	(void) modified;
	MelderFile file = UiFile_getFile (sendingForm);
	openDocument (me, file);
}

static void cb_showOpen (EditorCommand cmd, UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	(void) sendingForm;
	(void) sendingString;
	(void) interpreter;
	if (! my openDialog)
		my openDialog = UiInfile_create (my d_windowForm, L"Open", cb_open_ok, me, NULL, NULL, false);
	UiInfile_do (my openDialog);
}

static void cb_saveAs_ok (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, I) {
	iam (TextEditor);
	(void) sendingString;
	(void) interpreter;
	(void) invokingButtonTitle;
	(void) modified;
	MelderFile file = UiFile_getFile (sendingForm);
	saveDocument (me, file);
}

static void menu_cb_saveAs (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	wchar_t defaultName [300];
	if (! my saveDialog)
		my saveDialog = UiOutfile_create (my d_windowForm, L"Save", cb_saveAs_ok, me, NULL, NULL);
	swprintf (defaultName, 300, ! my v_fileBased () ? L"info.txt" : my name [0] ? MelderFile_name (& my file) : L"");
	UiOutfile_do (my saveDialog, defaultName);
}

static void gui_button_cb_saveAndOpen (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> d_editor;
	my dirtyOpenDialog -> f_hide ();
	if (my name [0]) {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError (NULL);
			return;
		}
		cb_showOpen (cmd, NULL, NULL, NULL);
	} else {
		menu_cb_saveAs (me, cmd, NULL, 0, NULL, NULL, NULL);
	}
}

static void gui_button_cb_cancelOpen (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> d_editor;
	my dirtyOpenDialog -> f_hide ();
}

static void gui_button_cb_discardAndOpen (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> d_editor;
	my dirtyOpenDialog -> f_hide ();
	cb_showOpen (cmd, NULL, NULL, NULL);
}

static void menu_cb_open (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	if (my dirty) {
		if (my dirtyOpenDialog == NULL) {
			int buttonWidth = 120, buttonSpacing = 20;
			my dirtyOpenDialog = GuiDialog_create (my d_windowForm,
				150, 70,
				Gui_LEFT_DIALOG_SPACING + 3 * buttonWidth + 2 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				L"Text changed", NULL, NULL, GuiDialog_MODAL);
			GuiLabel_createShown (my dirtyOpenDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				L"The text has changed! Save changes?", 0);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (my dirtyOpenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				L"Discard & Open", gui_button_cb_discardAndOpen, cmd, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyOpenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				L"Cancel", gui_button_cb_cancelOpen, cmd, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyOpenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				L"Save & Open", gui_button_cb_saveAndOpen, cmd, 0);
		}
		my dirtyOpenDialog -> f_show ();
	} else {
		cb_showOpen (cmd, sendingForm, sendingString, interpreter);
	}
}

static void gui_button_cb_saveAndNew (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> d_editor;
	my dirtyNewDialog -> f_hide ();
	if (my name [0]) {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError (NULL);
			return;
		}
		newDocument (me);
	} else {
		menu_cb_saveAs (me, cmd, NULL, 0, NULL, NULL, NULL);
	}
}

static void gui_button_cb_cancelNew (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> d_editor;
	my dirtyNewDialog -> f_hide ();
}

static void gui_button_cb_discardAndNew (I, GuiButtonEvent event) {
	(void) event;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> d_editor;
	my dirtyNewDialog -> f_hide ();
	newDocument (me);
}

static void menu_cb_new (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	if (my v_fileBased () && my dirty) {
		if (! my dirtyNewDialog) {
			int buttonWidth = 120, buttonSpacing = 20;
			my dirtyNewDialog = GuiDialog_create (my d_windowForm,
				150, 70, Gui_LEFT_DIALOG_SPACING + 3 * buttonWidth + 2 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
					Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				L"Text changed", NULL, NULL, GuiDialog_MODAL);
			GuiLabel_createShown (my dirtyNewDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				L"The text has changed! Save changes?", 0);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (my dirtyNewDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				L"Discard & New", gui_button_cb_discardAndNew, cmd, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyNewDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				L"Cancel", gui_button_cb_cancelNew, cmd, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyNewDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				L"Save & New", gui_button_cb_saveAndNew, cmd, 0);
		}
		my dirtyNewDialog -> f_show ();
	} else {
		newDocument (me);
	}
}

static void menu_cb_clear (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	my v_clear ();
}

static void menu_cb_save (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	if (my name [0]) {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError (NULL);
			return;
		}
	} else {
		menu_cb_saveAs (me, cmd, NULL, 0, NULL, NULL, NULL);
	}
}

static void menu_cb_reopen (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	if (my name [0]) {
		try {
			openDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError (NULL);
			return;
		}
	} else {
		Melder_throw ("Cannot reopen from disk, because the text has never been saved yet.");
	}
}

static void gui_button_cb_saveAndClose (I, GuiButtonEvent event) {
	(void) event;
	iam (TextEditor);
	my dirtyCloseDialog -> f_hide ();
	if (my name [0]) {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError (NULL);
			return;
		}
		closeDocument (me);
	} else {
		menu_cb_saveAs (me, Editor_getMenuCommand (me, L"File", L"Save as..."), NULL, 0, NULL, NULL, NULL);
	}
}

static void gui_button_cb_cancelClose (I, GuiButtonEvent event) {
	(void) event;
	iam (TextEditor);
	my dirtyCloseDialog -> f_hide ();
}

static void gui_button_cb_discardAndClose (I, GuiButtonEvent event) {
	(void) event;
	iam (TextEditor);
	my dirtyCloseDialog -> f_hide ();
	closeDocument (me);
}

void structTextEditor :: v_goAway () {
	if (v_fileBased () && dirty) {
		if (! dirtyCloseDialog) {
			int buttonWidth = 120, buttonSpacing = 20;
			dirtyCloseDialog = GuiDialog_create (d_windowForm,
				150, 70, Gui_LEFT_DIALOG_SPACING + 3 * buttonWidth + 2 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
					Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				L"Text changed", NULL, NULL, GuiDialog_MODAL);
			GuiLabel_createShown (dirtyCloseDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				L"The text has changed! Save changes?", 0);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (dirtyCloseDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				L"Discard & Close", gui_button_cb_discardAndClose, this, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (dirtyCloseDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				L"Cancel", gui_button_cb_cancelClose, this, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (dirtyCloseDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				L"Save & Close", gui_button_cb_saveAndClose, this, 0);
		}
		dirtyCloseDialog -> f_show ();
	} else {
		closeDocument (this);
	}
}

static void menu_cb_undo (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	my textWidget -> f_undo ();
}

static void menu_cb_redo (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	my textWidget -> f_redo ();
}

static void menu_cb_cut (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	my textWidget -> f_cut ();  // use ((XmAnyCallbackStruct *) call) -> event -> xbutton. time
}

static void menu_cb_copy (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	my textWidget -> f_copy ();
}

static void menu_cb_paste (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	my textWidget -> f_paste ();
}

static void menu_cb_erase (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	my textWidget -> f_remove ();
}

static bool getSelectedLines (TextEditor me, long *firstLine, long *lastLine) {
	long left, right;
	wchar_t *text = my textWidget -> f_getStringAndSelectionPosition (& left, & right);
	long textLength = wcslen (text);
	Melder_assert (left >= 0);
	Melder_assert (left <= right);
	Melder_assert (right <= textLength);
	long i = 0;
	*firstLine = 1;
	/*
	 * Cycle through the text in order to see how many linefeeds we pass.
	 */
	for (; i < left; i ++) {
		if (text [i] == '\n') {
			(*firstLine) ++;
		}
	}
	if (left == right) return false;
	*lastLine = *firstLine;
	for (; i < right; i ++) {
		if (text [i] == '\n') {
			(*lastLine) ++;
		}
	}
	Melder_free (text);
	return true;
}

static wchar_t *theFindString = NULL, *theReplaceString = NULL;
static void do_find (TextEditor me) {
	if (theFindString == NULL) return;   // e.g. when the user does "Find again" before having done any "Find"
	long left, right;
	autostring text = my textWidget -> f_getStringAndSelectionPosition (& left, & right);
	wchar_t *location = wcsstr (& text [right], theFindString);
	if (location != NULL) {
		long index = location - text.peek();
		my textWidget -> f_setSelection (index, index + wcslen (theFindString));
		my textWidget -> f_scrollToSelection ();
		#ifdef _WIN32
			my d_windowForm -> f_show ();
		#endif
	} else {
		/* Try from the start of the document. */
		location = wcsstr (text.peek(), theFindString);
		if (location != NULL) {
			long index = location - text.peek();
			my textWidget -> f_setSelection (index, index + wcslen (theFindString));
			my textWidget -> f_scrollToSelection ();
			#ifdef _WIN32
				my d_windowForm -> f_show ();
			#endif
		} else {
			Melder_beep ();
		}
	}
}

static void do_replace (TextEditor me) {
	if (theReplaceString == NULL) return;   // e.g. when the user does "Replace again" before having done any "Replace"
	autostring selection = my textWidget -> f_getSelection ();
	if (! Melder_wcsequ (selection.peek(), theFindString)) {
		do_find (me);
		return;
	}
	long left, right;
	autostring text = my textWidget -> f_getStringAndSelectionPosition (& left, & right);
	my textWidget -> f_replace (left, right, theReplaceString);
	my textWidget -> f_setSelection (left, left + wcslen (theReplaceString));
	my textWidget -> f_scrollToSelection ();
	#ifdef _WIN32
		my d_windowForm -> f_show ();
	#endif
}

static void menu_cb_find (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	EDITOR_FORM (L"Find", 0)
		LABEL (L"", L"Find:")
		TEXTFIELD (L"findString", L"")
	EDITOR_OK
		if (theFindString != NULL) SET_STRING (L"findString", theFindString);
	EDITOR_DO
		Melder_free (theFindString);
		theFindString = Melder_wcsdup_f (GET_STRING (L"findString"));
		do_find (me);
	EDITOR_END
}

static void menu_cb_findAgain (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	do_find (me);
}

static void menu_cb_replace (EDITOR_ARGS) {
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

static void menu_cb_replaceAgain (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	do_replace (me);
}

static void menu_cb_whereAmI (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	long numberOfLinesLeft, numberOfLinesRight;
	if (! getSelectedLines (me, & numberOfLinesLeft, & numberOfLinesRight)) {
		Melder_information (L"The cursor is on line ", Melder_integer (numberOfLinesLeft), L".");
	} else if (numberOfLinesLeft == numberOfLinesRight) {
		Melder_information (L"The selection is on line ", Melder_integer (numberOfLinesLeft), L".");
	} else {
		Melder_information (L"The selection runs from line ", Melder_integer (numberOfLinesLeft),
			L" to line ", Melder_integer (numberOfLinesRight), L".");
	}
}

static void menu_cb_goToLine (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	EDITOR_FORM (L"Go to line", 0)
		NATURAL (L"Line", L"1")
	EDITOR_OK
		long firstLine, lastLine;
		getSelectedLines (me, & firstLine, & lastLine);
		SET_INTEGER (L"Line", firstLine);
	EDITOR_DO
		autostring text = my textWidget -> f_getString ();
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
		if (left == wcslen (text.peek())) {
			right = left;
		} else if (text [right] == '\n') {
			right ++;
		}
		my textWidget -> f_setSelection (left, right);
		my textWidget -> f_scrollToSelection ();
	EDITOR_END
}

static void menu_cb_convertToCString (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	autostring text = my textWidget -> f_getString ();
	wchar_t buffer [2] = L" ";
	const wchar_t *hex [16] = { L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"A", L"B", L"C", L"D", L"E", L"F" };
	MelderInfo_open ();
	MelderInfo_write (L"\"");
	for (wchar_t *p = & text [0]; *p != '\0'; p ++) {
		if (*p == '\n') {
			MelderInfo_write (L"\\n\"\n\"");
		} else if (*p == '\t') {
			MelderInfo_write (L"   ");
		} else if (*p == '\"') {
			MelderInfo_write (L"\\\"");
		} else if (*p == '\\') {
			MelderInfo_write (L"\\\\");
		} else if (*p < 0 || *p > 127) {
			uint32 kar = *p;
			if (kar <= 0xFFFF) {
				MelderInfo_write (L"\\u", hex [kar >> 12], hex [(kar >> 8) & 0x0000000F], hex [(kar >> 4) & 0x0000000F], hex [kar & 0x0000000F]);
			} else {
				MelderInfo_write (L"\\U", hex [kar >> 28], hex [(kar >> 24) & 0x0000000F], hex [(kar >> 20) & 0x0000000F], hex [(kar >> 16) & 0x0000000F],
					hex [(kar >> 12) & 0x0000000F], hex [(kar >> 8) & 0x0000000F], hex [(kar >> 4) & 0x0000000F], hex [kar & 0x0000000F]);
			}
		} else {
			buffer [0] = *p;
			MelderInfo_write (& buffer [0]);
		}
	}
	MelderInfo_write (L"\"");
	MelderInfo_close ();
}

/***** 'Font' menu *****/

static void updateSizeMenu (TextEditor me) {
	if (my fontSizeButton_10) my fontSizeButton_10 -> f_check (my p_fontSize == 10);
	if (my fontSizeButton_12) my fontSizeButton_12 -> f_check (my p_fontSize == 12);
	if (my fontSizeButton_14) my fontSizeButton_14 -> f_check (my p_fontSize == 14);
	if (my fontSizeButton_18) my fontSizeButton_18 -> f_check (my p_fontSize == 18);
	if (my fontSizeButton_24) my fontSizeButton_24 -> f_check (my p_fontSize == 24);
}
static void setFontSize (TextEditor me, int fontSize) {
	my textWidget -> f_setFontSize (fontSize);
	my pref_fontSize () = my p_fontSize = fontSize;
	updateSizeMenu (me);
}

static void menu_cb_10 (EDITOR_ARGS) { EDITOR_IAM (TextEditor); setFontSize (me, 10); }
static void menu_cb_12 (EDITOR_ARGS) { EDITOR_IAM (TextEditor); setFontSize (me, 12); }
static void menu_cb_14 (EDITOR_ARGS) { EDITOR_IAM (TextEditor); setFontSize (me, 14); }
static void menu_cb_18 (EDITOR_ARGS) { EDITOR_IAM (TextEditor); setFontSize (me, 18); }
static void menu_cb_24 (EDITOR_ARGS) { EDITOR_IAM (TextEditor); setFontSize (me, 24); }
static void menu_cb_fontSize (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	EDITOR_FORM (L"Text window: Font size", 0)
		NATURAL (L"Font size (points)", L"12")
	EDITOR_OK
		SET_INTEGER (L"Font size", (long) my p_fontSize);
	EDITOR_DO
		setFontSize (me, GET_INTEGER (L"Font size"));
	EDITOR_END
}

void structTextEditor :: v_createMenus () {
	TextEditor_Parent :: v_createMenus ();
	if (v_fileBased ()) {
		Editor_addCommand (this, L"File", L"New", 'N', menu_cb_new);
		Editor_addCommand (this, L"File", L"Open...", 'O', menu_cb_open);
		Editor_addCommand (this, L"File", L"Reopen from disk", 0, menu_cb_reopen);
	} else {
		Editor_addCommand (this, L"File", L"Clear", 'N', menu_cb_clear);
	}
	Editor_addCommand (this, L"File", L"-- save --", 0, NULL);
	if (v_fileBased ()) {
		Editor_addCommand (this, L"File", L"Save", 'S', menu_cb_save);
		Editor_addCommand (this, L"File", L"Save as...", 0, menu_cb_saveAs);
	} else {
		Editor_addCommand (this, L"File", L"Save as...", 'S', menu_cb_saveAs);
	}
	Editor_addCommand (this, L"File", L"-- close --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Undo", 'Z', menu_cb_undo);
	Editor_addCommand (this, L"Edit", L"Redo", 'Y', menu_cb_redo);
	Editor_addCommand (this, L"Edit", L"-- cut copy paste --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Cut", 'X', menu_cb_cut);
	Editor_addCommand (this, L"Edit", L"Copy", 'C', menu_cb_copy);
	Editor_addCommand (this, L"Edit", L"Paste", 'V', menu_cb_paste);
	Editor_addCommand (this, L"Edit", L"Erase", 0, menu_cb_erase);
	Editor_addMenu (this, L"Search", 0);
	Editor_addCommand (this, L"Search", L"Find...", 'F', menu_cb_find);
	Editor_addCommand (this, L"Search", L"Find again", 'G', menu_cb_findAgain);
	Editor_addCommand (this, L"Search", L"Replace...", GuiMenu_SHIFT + 'F', menu_cb_replace);
	Editor_addCommand (this, L"Search", L"Replace again", GuiMenu_SHIFT + 'G', menu_cb_replaceAgain);
	Editor_addCommand (this, L"Search", L"-- line --", 0, NULL);
	Editor_addCommand (this, L"Search", L"Where am I?", 0, menu_cb_whereAmI);
	Editor_addCommand (this, L"Search", L"Go to line...", 'L', menu_cb_goToLine);
	Editor_addMenu (this, L"Convert", 0);
	Editor_addCommand (this, L"Convert", L"Convert to C string", 0, menu_cb_convertToCString);
	#if defined (macintosh) || defined (UNIX) || defined (_WIN32)
		Editor_addMenu (this, L"Font", 0);
		Editor_addCommand (this, L"Font", L"Font size...", 0, menu_cb_fontSize);
		fontSizeButton_10 = Editor_addCommand (this, L"Font", L"10", GuiMenu_CHECKBUTTON, menu_cb_10);
		fontSizeButton_12 = Editor_addCommand (this, L"Font", L"12", GuiMenu_CHECKBUTTON, menu_cb_12);
		fontSizeButton_14 = Editor_addCommand (this, L"Font", L"14", GuiMenu_CHECKBUTTON, menu_cb_14);
		fontSizeButton_18 = Editor_addCommand (this, L"Font", L"18", GuiMenu_CHECKBUTTON, menu_cb_18);
		fontSizeButton_24 = Editor_addCommand (this, L"Font", L"24", GuiMenu_CHECKBUTTON, menu_cb_24);
	#endif
}

static void gui_text_cb_change (I, GuiTextEvent event) {
	(void) event;
	iam (TextEditor);
	if (! my dirty) {
		my dirty = TRUE;
		my v_nameChanged ();
	}
}

void structTextEditor :: v_createChildren () {
	textWidget = GuiText_createShown (d_windowForm, 0, 0, Machine_getMenuBarHeight (), 0, GuiText_SCROLLED);
	textWidget -> f_setChangeCallback (gui_text_cb_change, this);
	textWidget -> f_setUndoItem (Editor_getMenuCommand (this, L"Edit", L"Undo") -> itemWidget);
	textWidget -> f_setRedoItem (Editor_getMenuCommand (this, L"Edit", L"Redo") -> itemWidget);
}

void structTextEditor :: init (const wchar_t *initialText) {
	Editor_init (this, 0, 0, 600, 400, L"", NULL);
	setFontSize (this, p_fontSize);
	if (initialText) {
		textWidget -> f_setString (initialText);
		dirty = FALSE;   // was set to TRUE in valueChanged callback
		Thing_setName (this, L"");
	}
	if (theOpenTextEditors == NULL) {
		theOpenTextEditors = Collection_create (classTextEditor, 100);
		Collection_dontOwnItems (theOpenTextEditors);
	}
	if (theOpenTextEditors != NULL) {
		Collection_addItem (theOpenTextEditors, this);
	}
}

TextEditor TextEditor_create (const wchar_t *initialText) {
	try {
		autoTextEditor me = Thing_new (TextEditor);
		my init (initialText);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Text window not created.");
	}
}

void TextEditor_showOpen (TextEditor me) {
	cb_showOpen (Editor_getMenuCommand (me, L"File", L"Open..."), NULL, NULL, NULL);
}

/* End of file TextEditor.cpp */
