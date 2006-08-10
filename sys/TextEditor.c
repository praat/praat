/* TextEditor.c
 *
 * Copyright (C) 1997-2006 Paul Boersma
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
 */

#include "TextEditor.h"
#include "machine.h"
#include "longchar.h"
#include "EditorM.h"
#include "Preferences.h"

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
		if (my name == NULL)
			sprintf (Melder_buffer1, "(untitled%s)", my dirty && ! dirtinessAlreadyShown ? ", modified" : "");
		else
			sprintf (Melder_buffer1, "File \\\"l%s\\\"r%s", MelderFile_messageName (& my file), my dirty && ! dirtinessAlreadyShown ? " (modified)" : "");
		Longchar_nativize (Melder_buffer1, Melder_buffer2, TRUE);
		XtVaSetValues (my shell, XmNtitle, Melder_buffer2, NULL);
		if (my name == NULL)
			sprintf (Melder_buffer1, "%s(untitled)", my dirty && ! dirtinessAlreadyShown ? "*" : "");
		else
			sprintf (Melder_buffer1, "%s%s", my dirty && ! dirtinessAlreadyShown ? "*" : "", MelderFile_name (& my file));
		Longchar_nativize (Melder_buffer1, Melder_buffer2, TRUE);
		XtVaSetValues (my shell, XmNiconName, Melder_buffer2, NULL);
	} else {
		inherited (TextEditor) nameChanged (me);
	}
}

static int openDocument (TextEditor me, MelderFile file) {
	char *text = MelderFile_readText (file);
	if (! text) return 0;
	XmTextSetString (my textWidget, text);
	Melder_free (text);
	/*
	 * XmTextSetString has invoked the XmNvalueChangedCallback,
	 * which has set 'my dirty' to TRUE. Fix this.
	 */
	my dirty = FALSE;
	MelderFile_copy (file, & my file);
	Thing_setName (me, Melder_fileToPath (file));
	return 1;
}

static void newDocument (TextEditor me) {
	XmTextSetString (my textWidget, "");   /* Implicitly sets my dirty to TRUE. */
	my dirty = FALSE;
	if (our fileBased) Thing_setName (me, NULL);
}

static int saveDocument (TextEditor me, MelderFile file) {
	char *text = XmTextGetString (my textWidget);
	if (! MelderFile_writeText (file, text)) { XtFree (text); return 0; }
	/*
	 * The following is a TRICK to make sure that MacOS X knows that any following
	 * entered character will introduce a value change in the GuiText object.
	 */
	#if defined (macintosh) && defined (__MACH__)
		XmTextSetString (my textWidget, text);
	#endif
	XtFree (text);
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
		my openDialog = UiInfile_create (my dialog, "Open", cb_open_ok, me, 0);
	UiInfile_do (my openDialog);
}

static int cb_saveAs_ok (Any sender, I) {
	iam (TextEditor);
	MelderFile file = UiFile_getFile (sender);
	if (! saveDocument (me, file)) return 0;
	return 1;
}

DIRECT (TextEditor, cb_saveAs)
	char defaultName [300];
	if (! my saveDialog)
		my saveDialog = UiOutfile_create (my dialog, "Save", cb_saveAs_ok, me, 0);
	sprintf (defaultName, ! our fileBased ? "info.txt" : my name ? MelderFile_name (& my file) : "");
	UiOutfile_do (my saveDialog, defaultName);
END

MOTIF_CALLBACK (cb_saveAndOpen)
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	if (my name) {
		if (! saveDocument (me, & my file)) { Melder_flushError (NULL); return; }
		cb_showOpen (cmd, NULL);
	} else {
		cb_saveAs (cmd, NULL);
	}
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_discardAndOpen)
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	XtUnmanageChild (my dirtyOpenDialog);
	cb_showOpen (cmd, NULL);
MOTIF_CALLBACK_END

DIRECT (TextEditor, cb_open)
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
			XtAddCallback (my dirtyOpenDialog, XmNokCallback, cb_saveAndOpen, cmd);
			XtAddCallback (my dirtyOpenDialog, XmNhelpCallback, cb_discardAndOpen, cmd);
		}
		XtManageChild (my dirtyOpenDialog);
	} else {
		cb_showOpen (cmd, sender);
	}
END

MOTIF_CALLBACK (cb_saveAndNew)
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	if (my name) {
		if (! saveDocument (me, & my file)) { Melder_flushError (NULL); return; }
		newDocument (me);
	} else {
		cb_saveAs (cmd, NULL);
	}
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_discardAndNew)
	EditorCommand cmd = (EditorCommand) void_me;
	TextEditor me = (TextEditor) cmd -> editor;
	XtUnmanageChild (my dirtyNewDialog);
	newDocument (me);
MOTIF_CALLBACK_END

DIRECT (TextEditor, cb_new)
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
			XtAddCallback (my dirtyNewDialog, XmNokCallback, cb_saveAndNew, cmd);
			XtAddCallback (my dirtyNewDialog, XmNhelpCallback, cb_discardAndNew, cmd);
		}
		XtManageChild (my dirtyNewDialog);
	} else {
		newDocument (me);
	}
END

DIRECT (TextEditor, cb_clear)
	our clear (me);
END

DIRECT (TextEditor, cb_save)
	if (my name) {
		if (! saveDocument (me, & my file)) return 0;
	} else {
		cb_saveAs (cmd, NULL);
	}
END

MOTIF_CALLBACK (cb_saveAndClose)
	iam (TextEditor);
	if (my name) {
		if (! saveDocument (me, & my file)) { Melder_flushError (NULL); return; }
		closeDocument (me);
	} else {
		cb_saveAs (Editor_getMenuCommand (me, "File", "Save as..."), NULL);
	}
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_discardAndClose)
	iam (TextEditor);
	XtUnmanageChild (my dirtyCloseDialog);
	closeDocument (me);
MOTIF_CALLBACK_END

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
			XtAddCallback (my dirtyCloseDialog, XmNokCallback, cb_saveAndClose, me);
			XtAddCallback (my dirtyCloseDialog, XmNhelpCallback, cb_discardAndClose, me);
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
	#if defined (UNIX)
		XmTextCut (my textWidget, 0/*((XmAnyCallbackStruct *) call) -> event -> xbutton. time*/);
	#else
		XmTextCut (my textWidget, 0);
	#endif
END

DIRECT (TextEditor, cb_copy)
	#if defined (UNIX)
		XmTextCopy (my textWidget, 0/*((XmAnyCallbackStruct *) call) -> event -> xbutton. time*/);
	#else
		XmTextCopy (my textWidget, 0);
	#endif
END

DIRECT (TextEditor, cb_paste)
	XmTextPaste (my textWidget);
END

DIRECT (TextEditor, cb_erase)
	XmTextRemove (my textWidget);
END

static int getSelectedLines (TextEditor me, long *firstLine, long *lastLine) {
	char *text = XmTextGetString (my textWidget);
	XmTextPosition left, right;
	long i;
	*firstLine = 1;
	if (! XmTextGetSelectionPosition (my textWidget, & left, & right))
		left = right = XmTextGetInsertionPosition (my textWidget);
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
	Melder_assert (left <= (XmTextPosition) strlen (text));
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
	Melder_assert (right <= (XmTextPosition) strlen (text));
	XtFree (text);
	return TRUE;
}

DIRECT (TextEditor, cb_whereAmI)
	long numberOfLinesLeft, numberOfLinesRight;
	if (! getSelectedLines (me, & numberOfLinesLeft, & numberOfLinesRight)) {
		Melder_information ("The cursor is on line %ld.", numberOfLinesLeft);
	} else if (numberOfLinesLeft == numberOfLinesRight) {
		Melder_information ("The selection is on line %ld.", numberOfLinesLeft);
	} else {
		Melder_information ("The selection runs from line %ld to line %ld.", numberOfLinesLeft, numberOfLinesRight);
	}
END

FORM (TextEditor, cb_goToLine, "Go to line", 0)
	NATURAL ("Line", "1")
	OK
long firstLine, lastLine;
getSelectedLines (me, & firstLine, & lastLine);
SET_INTEGER ("Line", firstLine);
DO
	char *text = XmTextGetString (my textWidget);
	long lineToGo = GET_INTEGER ("Line"), currentLine = 1;
	unsigned long left, right;
	if (lineToGo == 1) {
		left = 0;
		for (right = left; text [right] != '\n' && text [right] != '\0'; right ++) { }
	} else {
		for (left = 0; text [left] != '\0'; left ++) {
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
	if (left == strlen (text)) {
		right = left;
	} else if (text [right] == '\n') {
		right ++;
	}
	XtFree (text);
	#if defined (_WIN32)
		left += lineToGo - 1;
		right += lineToGo - 1;
	#endif
	XmTextSetSelection (my textWidget, left, right, 0);
	XmTextShowPosition (my textWidget, left);
END

/***** 'Font' menu *****/

static int theTextEditorFontSize = 12;
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
FORM (TextEditor, cb_fontSize, "Text window: Font size", 0)
	NATURAL ("Font size (points)", "12")
	OK
SET_INTEGER ("Font size", (long) my fontSize);
DO
	setFontSize (me, GET_INTEGER ("Font size"));
END

static void createMenus (I) {
	iam (TextEditor);
	inherited (TextEditor) createMenus (me);
	if (our fileBased) {
		Editor_addCommand (me, "File", "New", 'N', cb_new);
		Editor_addCommand (me, "File", "Open...", 'O', cb_open);
	} else {
		Editor_addCommand (me, "File", "Clear", 'N', cb_clear);
	}
	Editor_addCommand (me, "File", "-- save --", 0, NULL);
	if (our fileBased) {
		Editor_addCommand (me, "File", "Save", 'S', cb_save);
		Editor_addCommand (me, "File", "Save as...", 0, cb_saveAs);
	} else {
		Editor_addCommand (me, "File", "Save as...", 'S', cb_saveAs);
	}
	Editor_addCommand (me, "File", "-- close --", 0, NULL);
	Editor_addCommand (me, "Edit", "Undo", 'Z', cb_undo);
	Editor_addCommand (me, "Edit", "Redo", 'Y', cb_redo);
	Editor_addCommand (me, "Edit", "-- cut copy paste --", 0, NULL);
	Editor_addCommand (me, "Edit", "Cut", 'X', cb_cut);
	Editor_addCommand (me, "Edit", "Copy", 'C', cb_copy);
	Editor_addCommand (me, "Edit", "Paste", 'V', cb_paste);
	Editor_addCommand (me, "Edit", "Erase", 0, cb_erase);
	Editor_addMenu (me, "Search", 0);
	if (our fileBased) Editor_addCommand (me, "Search", "Where am I?", 0, cb_whereAmI);
	Editor_addCommand (me, "Search", "Go to line...", 'L', cb_goToLine);
	#ifdef macintosh
		Editor_addMenu (me, "Font", 0);
		my fontSizeButton_10 = Editor_addCommand (me, "Font", "10", motif_CHECKABLE, cb_10);
		my fontSizeButton_12 = Editor_addCommand (me, "Font", "12", motif_CHECKABLE, cb_12);
		my fontSizeButton_14 = Editor_addCommand (me, "Font", "14", motif_CHECKABLE, cb_14);
		my fontSizeButton_18 = Editor_addCommand (me, "Font", "18", motif_CHECKABLE, cb_18);
		my fontSizeButton_24 = Editor_addCommand (me, "Font", "24", motif_CHECKABLE, cb_24);
		Editor_addCommand (me, "Font", "Font size...", 0, cb_fontSize);
	#endif
}

MOTIF_CALLBACK (cb_valueChanged)
	iam (TextEditor);
	if (! my dirty) {
		my dirty = TRUE;
		our nameChanged (me);
	}
MOTIF_CALLBACK_END

static void createChildren (I) {
	iam (TextEditor);
	my textWidget = GuiText_createScrolled (my dialog, "text", TRUE, Machine_getMenuBarHeight ());
	XtAddCallback (my textWidget, XmNvalueChangedCallback, cb_valueChanged, me);
	XtManageChild (my textWidget);
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
	class_method (clear)
class_methods_end

int TextEditor_init (I, Widget parent, const char *initialText) {
	iam (TextEditor);
	if (! Editor_init (me, parent, 0, 0, 600, 400, NULL, NULL)) return 0;
	setFontSize (me, theTextEditorFontSize);
	if (initialText) {
		XmTextSetString (my textWidget, MOTIF_CONST_CHAR_ARG (initialText));
		my dirty = FALSE;   /* Was set to TRUE in valueChanged callback. */
		Thing_setName (me, NULL);
	}
	return 1;
}

TextEditor TextEditor_create (Widget parent, const char *initialText) {
	TextEditor me = new (TextEditor);
	if (! me || ! TextEditor_init (me, parent, initialText)) { forget (me); return NULL; }
	return me;
}

void TextEditor_showOpen (I) {
	iam (TextEditor);
	cb_showOpen (Editor_getMenuCommand (me, "File", "Open..."), NULL);
}

void TextEditor_prefs (void) {
	Resources_addInt ("TextEditor.fontSize", & theTextEditorFontSize);
}

/* End of file TextEditor.c */
