/* TextEditor.c
 *
 * Copyright (C) 1997-2007 Paul Boersma
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

/***** TextEditor methods *****/

static void destroy (I) {
	iam (TextEditor);
	forget (my openDialog);
	forget (my saveDialog);
	forget (my printDialog);
	forget (my findDialog);
	inherited (TextEditor) destroy (me);
}

static void nameChanged (I) {
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
			MelderString_append3 (& windowTitle, L"File " UNITEXT_LEFT_DOUBLE_QUOTATION_MARK, MelderFile_messageNameW (& my file), UNITEXT_RIGHT_DOUBLE_QUOTATION_MARK);
			if (my dirty && ! dirtinessAlreadyShown) MelderString_append (& windowTitle, L" (modified)");
		}
		GuiWindow_setTitle (my shell, windowTitle.string);
		MelderString_empty (& windowTitle);
		MelderString_append2 (& windowTitle, my dirty && ! dirtinessAlreadyShown ? L"*" : L"", my name == NULL ? L"(untitled)" : MelderFile_name (& my file));
		XtVaSetValues (my shell, XmNiconName, Melder_peekWcsToUtf8 (windowTitle.string), NULL);
	} else {
		inherited (TextEditor) nameChanged (me);
	}
}

static int openDocument (TextEditor me, MelderFile file) {
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

static int cb_open_ok (Any sender, I) {
	iam (TextEditor);
	MelderFile file = UiFile_getFile (sender);
	if (! openDocument (me, file)) return 0;
	return 1;
}

static void cb_showOpen (EditorCommand cmd, Any sender) {
	TextEditor me = (TextEditor) cmd -> editor;
	(void) sender;
	if (! my openDialog)
		my openDialog = UiInfile_create (my dialog, L"Open", cb_open_ok, me, 0);
	UiInfile_do (my openDialog);
}

static int cb_saveAs_ok (Any sender, I) {
	iam (TextEditor);
	MelderFile file = UiFile_getFile (sender);
	if (! saveDocument (me, file)) return 0;
	return 1;
}

static int menu_cb_saveAs (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	wchar_t defaultName [300];
	if (! my saveDialog)
		my saveDialog = UiOutfile_create (my dialog, L"Save", cb_saveAs_ok, me, 0);
	swprintf (defaultName, 300, ! our fileBased ? L"info.txt" : my name ? MelderFile_name (& my file) : L"");
	UiOutfile_do (my saveDialog, defaultName);
	return 1;
}

static void gui_cb_saveAndOpen (GUI_ARGS) {
	(void) w; (void) call;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	if (my name) {
		if (! saveDocument (me, & my file)) { Melder_flushError (NULL); return; }
		cb_showOpen (cmd, NULL);
	} else {
		menu_cb_saveAs (me, cmd, NULL);
	}
}

static void gui_cb_discardAndOpen (GUI_ARGS) {
	(void) w; (void) call;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	XtUnmanageChild (my dirtyOpenDialog);
	cb_showOpen (cmd, NULL);
}

static int menu_cb_open (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	if (my dirty) {
		if (! my dirtyOpenDialog) {
			my dirtyOpenDialog = XmCreateMessageDialog (my shell, "dirtyOpenDialog", NULL, 0);
			XtVaSetValues (my dirtyOpenDialog, XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
				XmNautoUnmanage, True, XmNdialogType, XmDIALOG_QUESTION,
				motif_argXmString (XmNdialogTitle, "Text changed"),
				motif_argXmString (XmNmessageString, "Save changes?"),
				motif_argXmString (XmNokLabelString, "Save & Open"),
				motif_argXmString (XmNhelpLabelString, "Discard & Open"),
				NULL);
			XtAddCallback (my dirtyOpenDialog, XmNokCallback, gui_cb_saveAndOpen, cmd);
			XtAddCallback (my dirtyOpenDialog, XmNhelpCallback, gui_cb_discardAndOpen, cmd);
		}
		XtManageChild (my dirtyOpenDialog);
	} else {
		cb_showOpen (cmd, sender);
	}
	return 1;
}

static void gui_cb_saveAndNew (GUI_ARGS) {
	(void) w; (void) call;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	if (my name) {
		if (! saveDocument (me, & my file)) { Melder_flushError (NULL); return; }
		newDocument (me);
	} else {
		menu_cb_saveAs (me, cmd, NULL);
	}
}

static void gui_cb_discardAndNew (GUI_ARGS) {
	(void) w; (void) call;
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	XtUnmanageChild (my dirtyNewDialog);
	newDocument (me);
}

static int menu_cb_new (EDITOR_ARGS) {
	EDITOR_IAM (TextEditor);
	if (our fileBased && my dirty) {
		if (! my dirtyNewDialog) {
			my dirtyNewDialog = XmCreateMessageDialog (my shell, "dirtyNewDialog", NULL, 0);
			XtVaSetValues (my dirtyNewDialog, XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
				XmNautoUnmanage, True, XmNdialogType, XmDIALOG_QUESTION,
				motif_argXmString (XmNdialogTitle, "Text changed"),
				motif_argXmString (XmNmessageString, "Save changes?"),
				motif_argXmString (XmNokLabelString, "Save & New"),
				motif_argXmString (XmNhelpLabelString, "Discard & New"),
				NULL);
			XtAddCallback (my dirtyNewDialog, XmNokCallback, gui_cb_saveAndNew, cmd);
			XtAddCallback (my dirtyNewDialog, XmNhelpCallback, gui_cb_discardAndNew, cmd);
		}
		XtManageChild (my dirtyNewDialog);
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
		menu_cb_saveAs (me, cmd, NULL);
	}
	return 1;
}

static void gui_cb_saveAndClose (GUI_ARGS) {
	GUI_IAM (TextEditor);
	if (my name) {
		if (! saveDocument (me, & my file)) { Melder_flushError (NULL); return; }
		closeDocument (me);
	} else {
		menu_cb_saveAs (me, Editor_getMenuCommand (me, L"File", L"Save as..."), NULL);
	}
}

static void gui_cb_discardAndClose (GUI_ARGS) {
	GUI_IAM (TextEditor);
	XtUnmanageChild (my dirtyCloseDialog);
	closeDocument (me);
}

static void goAway (I) {
	iam (TextEditor);
	if (our fileBased && my dirty) {
		if (! my dirtyCloseDialog) {
			my dirtyCloseDialog = XmCreateMessageDialog (my shell, "dirtyCloseDialog", NULL, 0);
			XtVaSetValues (my dirtyCloseDialog, XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
				XmNautoUnmanage, True, XmNdialogType, XmDIALOG_QUESTION,
				motif_argXmString (XmNdialogTitle, "Text changed"),
				motif_argXmString (XmNmessageString, "Save changes?"),
				motif_argXmString (XmNokLabelString, "Save & Close"),
				motif_argXmString (XmNhelpLabelString, "Discard & Close"),
				NULL);
			XtAddCallback (my dirtyCloseDialog, XmNokCallback, gui_cb_saveAndClose, me);
			XtAddCallback (my dirtyCloseDialog, XmNhelpCallback, gui_cb_discardAndClose, me);
		}
		XtManageChild (my dirtyCloseDialog);
	} else {
		closeDocument (me);
	}
}

DIRECT (TextEditor, cb_undo)
	GuiText_undo (my textWidget);
END

DIRECT (TextEditor, cb_redo)
	GuiText_redo (my textWidget);
END

DIRECT (TextEditor, cb_cut)
	GuiText_cut (my textWidget);  // use ((XmAnyCallbackStruct *) call) -> event -> xbutton. time
END

DIRECT (TextEditor, cb_copy)
	GuiText_copy (my textWidget);
END

DIRECT (TextEditor, cb_paste)
	GuiText_paste (my textWidget);
END

DIRECT (TextEditor, cb_erase)
	GuiText_remove (my textWidget);
END

static int getSelectedLines (TextEditor me, long *firstLine, long *lastLine) {
	wchar_t *text = GuiText_getString (my textWidget);
	long left, right;
	long i;
	*firstLine = 1;
	GuiText_getSelectionPosition (my textWidget, & left, & right);
	/*
	 * Cycle through the text in order to see how many linefeeds we pass.
	 */
	for (i = 0; i < left; i ++) {
		if (text [i] == '\n') {
			(*firstLine) ++;
			#if defined (_WIN32)
				left --, right --;   /* Correction for linefeed/return combinations. */
			#endif
		}
	}
	Melder_assert (left <= (long) wcslen (text));
	if (left == right) return FALSE;
	*lastLine = *firstLine;
	for (; i < right; i ++) {
		if (text [i] == '\n') {
			(*lastLine) ++;
			#if defined (_WIN32)
				left --, right --;   /* Correction for linefeed/return combinations. */
			#endif
		}
	}
	Melder_assert (right <= (long) wcslen (text));
	Melder_free (text);
	return TRUE;
}

DIRECT (TextEditor, cb_whereAmI)
	long numberOfLinesLeft, numberOfLinesRight;
	if (! getSelectedLines (me, & numberOfLinesLeft, & numberOfLinesRight)) {
		Melder_information3 (L"The cursor is on line ", Melder_integer (numberOfLinesLeft), L".");
	} else if (numberOfLinesLeft == numberOfLinesRight) {
		Melder_information3 (L"The selection is on line ", Melder_integer (numberOfLinesLeft), L".");
	} else {
		Melder_information5 (L"The selection runs from line ", Melder_integer (numberOfLinesLeft),
			L" to line ", Melder_integer (numberOfLinesRight), L".");
	}
END

FORM (TextEditor, cb_goToLine, L"Go to line", 0)
	NATURAL (L"Line", L"1")
	OK
long firstLine, lastLine;
getSelectedLines (me, & firstLine, & lastLine);
SET_INTEGER (L"Line", firstLine);
DO
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
	#if defined (_WIN32)
		left += lineToGo - 1;
		right += lineToGo - 1;
	#endif
	GuiText_setSelection (my textWidget, left, right);
	GuiText_scrollToSelection (my textWidget);
END

/***** 'Font' menu *****/

static void updateSizeMenu (TextEditor me) {
	if (my fontSizeButton_10) XmToggleButtonGadgetSetState (my fontSizeButton_10, my fontSize == 10, 0);
	if (my fontSizeButton_12) XmToggleButtonGadgetSetState (my fontSizeButton_12, my fontSize == 12, 0);
	if (my fontSizeButton_14) XmToggleButtonGadgetSetState (my fontSizeButton_14, my fontSize == 14, 0);
	if (my fontSizeButton_18) XmToggleButtonGadgetSetState (my fontSizeButton_18, my fontSize == 18, 0);
	if (my fontSizeButton_24) XmToggleButtonGadgetSetState (my fontSizeButton_24, my fontSize == 24, 0);
}
static void setFontSize (TextEditor me, int fontSize) {
	GuiText_setFontSize (my textWidget, fontSize);
	theTextEditorFontSize = my fontSize = fontSize;
	updateSizeMenu (me);
}

DIRECT (TextEditor, cb_10) setFontSize (me, 10); END
DIRECT (TextEditor, cb_12) setFontSize (me, 12); END
DIRECT (TextEditor, cb_14) setFontSize (me, 14); END
DIRECT (TextEditor, cb_18) setFontSize (me, 18); END
DIRECT (TextEditor, cb_24) setFontSize (me, 24); END
FORM (TextEditor, cb_fontSize, L"Text window: Font size", 0)
	NATURAL (L"Font size (points)", L"12")
	OK
SET_INTEGER (L"Font size", (long) my fontSize);
DO
	setFontSize (me, GET_INTEGER (L"Font size"));
END

static void createMenus (I) {
	iam (TextEditor);
	inherited (TextEditor) createMenus (me);
	if (our fileBased) {
		Editor_addCommand (me, L"File", L"New", 'N', menu_cb_new);
		Editor_addCommand (me, L"File", L"Open...", 'O', menu_cb_open);
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
	Editor_addCommand (me, L"Edit", L"Undo", 'Z', cb_undo);
	Editor_addCommand (me, L"Edit", L"Redo", 'Y', cb_redo);
	Editor_addCommand (me, L"Edit", L"-- cut copy paste --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Cut", 'X', cb_cut);
	Editor_addCommand (me, L"Edit", L"Copy", 'C', cb_copy);
	Editor_addCommand (me, L"Edit", L"Paste", 'V', cb_paste);
	Editor_addCommand (me, L"Edit", L"Erase", 0, cb_erase);
	Editor_addMenu (me, L"Search", 0);
	if (our fileBased) Editor_addCommand (me, L"Search", L"Where am I?", 0, cb_whereAmI);
	Editor_addCommand (me, L"Search", L"Go to line...", 'L', cb_goToLine);
	#ifdef macintosh
		Editor_addMenu (me, L"Font", 0);
		my fontSizeButton_10 = Editor_addCommand (me, L"Font", L"10", GuiMenu_CHECKABLE, cb_10);
		my fontSizeButton_12 = Editor_addCommand (me, L"Font", L"12", GuiMenu_CHECKABLE, cb_12);
		my fontSizeButton_14 = Editor_addCommand (me, L"Font", L"14", GuiMenu_CHECKABLE, cb_14);
		my fontSizeButton_18 = Editor_addCommand (me, L"Font", L"18", GuiMenu_CHECKABLE, cb_18);
		my fontSizeButton_24 = Editor_addCommand (me, L"Font", L"24", GuiMenu_CHECKABLE, cb_24);
		Editor_addCommand (me, L"Font", L"Font size...", 0, cb_fontSize);
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

static void createChildren (I) {
	iam (TextEditor);
	my textWidget = GuiText_createShown (my dialog, 0, 0, Machine_getMenuBarHeight (), 0, GuiText_SCROLLED);
	GuiText_setChangeCallback (my textWidget, gui_text_cb_change, me);
}

static void clear (I) {
	iam (TextEditor);
	(void) me;
}

class_methods (TextEditor, Editor)
	class_method (destroy)
	class_method (nameChanged)
	class_method (goAway)
	class_method (createChildren)
	class_method (createMenus)
	us -> fileBased = TRUE;
	us -> createMenuItems_query = NULL;
	class_method (clear)
class_methods_end

int TextEditor_init (I, Widget parent, const wchar_t *initialText) {
	iam (TextEditor);
	if (! Editor_init (me, parent, 0, 0, 600, 400, NULL, NULL)) return 0;
	setFontSize (me, theTextEditorFontSize);
	if (initialText) {
		GuiText_setString (my textWidget, initialText);
		my dirty = FALSE;   /* Was set to TRUE in valueChanged callback. */
		Thing_setName (me, NULL);
	}
	return 1;
}

TextEditor TextEditor_create (Widget parent, const wchar_t *initialText) {
	TextEditor me = new (TextEditor);
	if (! me || ! TextEditor_init (me, parent, initialText)) { forget (me); return NULL; }
	return me;
}

void TextEditor_showOpen (I) {
	iam (TextEditor);
	cb_showOpen (Editor_getMenuCommand (me, L"File", L"Open..."), NULL);
}

/* End of file TextEditor.c */
