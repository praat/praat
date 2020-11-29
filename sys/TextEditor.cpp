/* TextEditor.cpp
 *
 * Copyright (C) 1997-2020 Paul Boersma, 2010 Franz Brausse
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

#include "TextEditor.h"
#include "machine.h"
#include "../kar/longchar.h"
#include "EditorM.h"
#include "../kar/UnicodeData.h"

Thing_implement (TextEditor, Editor, 0);

#include "prefs_define.h"
#include "TextEditor_prefs.h"
#include "prefs_install.h"
#include "TextEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "TextEditor_prefs.h"

static CollectionOf <structTextEditor> theReferencesToAllOpenTextEditors;

/***** TextEditor methods *****/

void structTextEditor :: v_destroy () noexcept {
	our openDialog.reset();   // don't delay till delete
	our saveDialog.reset();   // don't delay till delete
	theReferencesToAllOpenTextEditors. undangleItem (this);
	TextEditor_Parent :: v_destroy ();
}

void structTextEditor :: v_nameChanged () {
	if (v_fileBased ()) {
		bool dirtinessAlreadyShown = GuiWindow_setDirty (our windowForm, our dirty);
		static MelderString windowTitle;
		if (our name [0] == U'\0') {
			MelderString_copy (& windowTitle, U"(untitled");
			if (dirty && ! dirtinessAlreadyShown)
				MelderString_append (& windowTitle, U", modified");
			MelderString_append (& windowTitle, U")");
		} else {
			MelderString_copy (& windowTitle, U"File ", MelderFile_messageName (& our file));
			if (dirty && ! dirtinessAlreadyShown)
				MelderString_append (& windowTitle, U" (modified)");
		}
		GuiShell_setTitle (our windowForm, windowTitle.string);
		//MelderString_copy (& windowTitle, our dirty && ! dirtinessAlreadyShown ? U"*" : U"", our name [0] == U'\0' ? U"(untitled)" : MelderFile_name (& our file));
	} else {
		TextEditor_Parent :: v_nameChanged ();
	}
}

static void openDocument (TextEditor me, MelderFile file) {
	for (integer ieditor = 1; ieditor <= theReferencesToAllOpenTextEditors.size; ieditor ++) {
		TextEditor editor = theReferencesToAllOpenTextEditors.at [ieditor];
		if (editor != me && MelderFile_equal (file, & editor -> file)) {
			Editor_raise (editor);
			/*
				Destruction alarm!
				When we combine the destruction of an object with the presentation of a message,
				we shall always follow the "build message -- destroy -- show message" paradigm.
				Actually, in this case this is not only safe, but also crucial,
				because at the time of writing (2019-04-28) the owner of `file` is owned by `me`,
				so that destroying `me` would dangle `file`.
			*/
			Melder_appendError (U"Text file ", file, U" is already open.");
			forget (me);
			Melder_flushError ();
			return;
		}
	}
	autostring32 text = MelderFile_readText (file);
	GuiText_setString (my textWidget, text.get());
	/*
	 * GuiText_setString has invoked the changeCallback,
	 * which has set `my dirty` to `true`. Fix this.
	 */
	my dirty = false;
	MelderFile_copy (file, & my file);
	Thing_setName (me, Melder_fileToPath (file));
}

static void newDocument (TextEditor me) {
	GuiText_setString (my textWidget, U"");   // implicitly sets my dirty to `true`
	my dirty = false;
	if (my v_fileBased ())
		Thing_setName (me, U"");
}

static void saveDocument (TextEditor me, MelderFile file) {
	autostring32 text = GuiText_getString (my textWidget);
	MelderFile_writeText (file, text.get(), Melder_getOutputEncoding ());
	my dirty = false;
	MelderFile_copy (file, & my file);
	if (my v_fileBased ())
		Thing_setName (me, Melder_fileToPath (file));
}

static void closeDocument (TextEditor me) {
	forget (me);
}

static void cb_open_ok (UiForm sendingForm, integer /* narg */, Stackel /* args */, conststring32 /* sendingString */,
	Interpreter /* interpreter */, conststring32 /* invokingButtonTitle */, bool /* modified */, void *void_me)
{
	iam (TextEditor);
	MelderFile file = UiFile_getFile (sendingForm);
	openDocument (me, file);
}

static void cb_showOpen (EditorCommand cmd) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	if (! my openDialog)
		my openDialog = UiInfile_create (my windowForm, U"Open", cb_open_ok, me, nullptr, nullptr, false);
	UiInfile_do (my openDialog.get());
}

static void cb_saveAs_ok (UiForm sendingForm, integer /* narg */, Stackel /* args */, conststring32 /* sendingString */,
	Interpreter /* interpreter */, conststring32 /* invokingButtonTitle */, bool /* modified */, void *void_me)
{
	iam (TextEditor);
	MelderFile file = UiFile_getFile (sendingForm);
	saveDocument (me, file);
}

static void menu_cb_saveAs (TextEditor me, EDITOR_ARGS_DIRECT) {
	if (! my saveDialog)
		my saveDialog = UiOutfile_create (my windowForm, U"Save", cb_saveAs_ok, me, nullptr, nullptr);
	char32 defaultName [300];
	Melder_sprint (defaultName,300, ! my v_fileBased () ? U"info.txt" : my name [0] ? MelderFile_name (& my file) : U"");
	UiOutfile_do (my saveDialog.get(), defaultName);
}

static void gui_button_cb_saveAndOpen (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyOpenDialog);
	if (my name [0]) {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError ();
			return;
		}
		cb_showOpen (cmd);
	} else {
		menu_cb_saveAs (me, cmd, nullptr, 0, nullptr, nullptr, nullptr);
	}
}

static void gui_button_cb_cancelOpen (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyOpenDialog);
}

static void gui_button_cb_discardAndOpen (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyOpenDialog);
	cb_showOpen (cmd);
}

static void menu_cb_open (TextEditor me, EDITOR_ARGS_CMD) {
	if (my dirty) {
		if (! my dirtyOpenDialog) {
			int buttonWidth = 120, buttonSpacing = 20;
			my dirtyOpenDialog = GuiDialog_create (my windowForm,
				150, 70,
				Gui_LEFT_DIALOG_SPACING + 3 * buttonWidth + 2 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				U"Text changed", nullptr, nullptr, GuiDialog_MODAL);
			GuiLabel_createShown (my dirtyOpenDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				U"The text has changed! Save changes?", 0);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (my dirtyOpenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Discard & Open", gui_button_cb_discardAndOpen, cmd, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyOpenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Cancel", gui_button_cb_cancelOpen, cmd, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyOpenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Save & Open", gui_button_cb_saveAndOpen, cmd, 0);
		}
		GuiThing_show (my dirtyOpenDialog);
	} else {
		cb_showOpen (cmd);
	}
}

static void gui_button_cb_saveAndNew (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyNewDialog);
	if (my name [0]) {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError ();
			return;
		}
		newDocument (me);
	} else {
		menu_cb_saveAs (me, cmd, nullptr, 0, nullptr, nullptr, nullptr);
	}
}

static void gui_button_cb_cancelNew (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyNewDialog);
}

static void gui_button_cb_discardAndNew (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyNewDialog);
	newDocument (me);
}

static void menu_cb_new (TextEditor me, EDITOR_ARGS_CMD) {
	if (my v_fileBased () && my dirty) {
		if (! my dirtyNewDialog) {
			int buttonWidth = 120, buttonSpacing = 20;
			my dirtyNewDialog = GuiDialog_create (my windowForm,
				150, 70, Gui_LEFT_DIALOG_SPACING + 3 * buttonWidth + 2 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
					Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				U"Text changed", nullptr, nullptr, GuiDialog_MODAL);
			GuiLabel_createShown (my dirtyNewDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				U"The text has changed! Save changes?", 0);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (my dirtyNewDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Discard & New", gui_button_cb_discardAndNew, cmd, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyNewDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Cancel", gui_button_cb_cancelNew, cmd, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyNewDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Save & New", gui_button_cb_saveAndNew, cmd, 0);
		}
		GuiThing_show (my dirtyNewDialog);
	} else {
		newDocument (me);
	}
}

static void gui_button_cb_cancelReopen (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyReopenDialog);
}

static void gui_button_cb_discardAndReopen (EditorCommand cmd, GuiButtonEvent /* event */) {
	TextEditor me = (TextEditor) cmd -> d_editor;
	GuiThing_hide (my dirtyReopenDialog);
	openDocument (me, & my file);
}

static void menu_cb_reopen (TextEditor me, EDITOR_ARGS_CMD) {
	Melder_assert (my v_fileBased());
	if (my name [0] == U'\0') {
		Melder_throw (U"Cannot reopen from disk, because the text has never been saved yet.");
	}
	if (my dirty) {
		if (! my dirtyReopenDialog) {
			int buttonWidth = 250, buttonSpacing = 20;
			my dirtyReopenDialog = GuiDialog_create (my windowForm,
				150, 70, Gui_LEFT_DIALOG_SPACING + 2 * buttonWidth + 1 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
					Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				U"Text changed", nullptr, nullptr, GuiDialog_MODAL);
			GuiLabel_createShown (my dirtyReopenDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				U"The text in the editor contains changes! Reopen nevertheless?", 0);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (my dirtyReopenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Keep visible version", gui_button_cb_cancelReopen, cmd, GuiButton_CANCEL);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (my dirtyReopenDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Replace with version from disk", gui_button_cb_discardAndReopen, cmd, GuiButton_DEFAULT);
		}
		GuiThing_show (my dirtyReopenDialog);
	} else {
		try {
			openDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError ();
			return;
		}
	}
}

static void menu_cb_clear (TextEditor me, EDITOR_ARGS_DIRECT) {
	my v_clear ();
}

static void menu_cb_save (TextEditor me, EDITOR_ARGS_CMD) {
	if (my name [0]) {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError ();
			return;
		}
	} else {
		menu_cb_saveAs (me, cmd, nullptr, 0, nullptr, nullptr, nullptr);
	}
}

static void gui_button_cb_saveAndClose (TextEditor me, GuiButtonEvent /* event */) {
	GuiThing_hide (my dirtyCloseDialog);
	if (my name [0]) {
		try {
			saveDocument (me, & my file);
		} catch (MelderError) {
			Melder_flushError ();
			return;
		}
		closeDocument (me);
	} else {
		menu_cb_saveAs (me, Editor_getMenuCommand (me, U"File", U"Save as..."), nullptr, 0, nullptr, nullptr, nullptr);
	}
}

static void gui_button_cb_cancelClose (TextEditor me, GuiButtonEvent /* event */) {
	GuiThing_hide (my dirtyCloseDialog);
}

static void gui_button_cb_discardAndClose (TextEditor me, GuiButtonEvent /* event */) {
	GuiThing_hide (my dirtyCloseDialog);
	closeDocument (me);
}

void structTextEditor :: v_goAway () {
	if (v_fileBased () && dirty) {
		if (! dirtyCloseDialog) {
			int buttonWidth = 120, buttonSpacing = 20;
			dirtyCloseDialog = GuiDialog_create (our windowForm,
				150, 70, Gui_LEFT_DIALOG_SPACING + 3 * buttonWidth + 2 * buttonSpacing + Gui_RIGHT_DIALOG_SPACING,
					Gui_TOP_DIALOG_SPACING + Gui_TEXTFIELD_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME + 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT,
				U"Text changed", nullptr, nullptr, GuiDialog_MODAL);
			GuiLabel_createShown (dirtyCloseDialog,
				Gui_LEFT_DIALOG_SPACING, - Gui_RIGHT_DIALOG_SPACING,
				Gui_TOP_DIALOG_SPACING, Gui_TOP_DIALOG_SPACING + Gui_LABEL_HEIGHT,
				U"The text has changed! Save changes?", 0);
			int x = Gui_LEFT_DIALOG_SPACING, y = - Gui_BOTTOM_DIALOG_SPACING;
			GuiButton_createShown (dirtyCloseDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Discard & Close", gui_button_cb_discardAndClose, this, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (dirtyCloseDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Cancel", gui_button_cb_cancelClose, this, 0);
			x += buttonWidth + buttonSpacing;
			GuiButton_createShown (dirtyCloseDialog,
				x, x + buttonWidth, y - Gui_PUSHBUTTON_HEIGHT, y,
				U"Save & Close", gui_button_cb_saveAndClose, this, 0);
		}
		if (our dirtyNewDialog)
			GuiThing_hide (our dirtyNewDialog);
		if (our dirtyOpenDialog)
			GuiThing_hide (our dirtyOpenDialog);
		if (our dirtyReopenDialog)
			GuiThing_hide (our dirtyReopenDialog);
		GuiThing_show (dirtyCloseDialog);
	} else {
		closeDocument (this);
	}
}

static void menu_cb_undo (TextEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_undo (my textWidget);
}

static void menu_cb_redo (TextEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_redo (my textWidget);
}

static void menu_cb_cut (TextEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_cut (my textWidget);  // use ((XmAnyCallbackStruct *) call) -> event -> xbutton. time
}

static void menu_cb_copy (TextEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_copy (my textWidget);
}

static void menu_cb_paste (TextEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_paste (my textWidget);
}

static void menu_cb_erase (TextEditor me, EDITOR_ARGS_DIRECT) {
	GuiText_remove (my textWidget);
}

static bool getSelectedLines (TextEditor me, integer *firstLine, integer *lastLine) {
	integer left, right;
	autostring32 text = GuiText_getStringAndSelectionPosition (my textWidget, & left, & right);
	integer textLength = str32len (text.get());
	Melder_assert (left >= 0);
	Melder_assert (left <= right);
	if (right > textLength)
		Melder_fatal (U"The end of the selection is at position ", right,
			U", which is beyond the end of the text, which is at position ", textLength, U".");
	integer i = 0;
	*firstLine = 1;
	/*
		Cycle through the text in order to see how many linefeeds we pass.
	*/
	for (; i < left; i ++)
		if (text [i] == U'\n')
			(*firstLine) ++;
	if (left == right)
		return false;
	*lastLine = *firstLine;
	for (; i < right; i ++)
		if (text [i] == U'\n')
			(*lastLine) ++;
	return true;
}

static autostring32 theFindString, theReplaceString;
static void do_find (TextEditor me) {
	if (! theFindString)
		return;   // e.g. when the user does "Find again" before having done any "Find"
	integer left, right;
	autostring32 text = GuiText_getStringAndSelectionPosition (my textWidget, & left, & right);
	char32 *location = str32str (& text [right], theFindString.get());
	if (location) {
		integer index = location - text.get();
		GuiText_setSelection (my textWidget, index, index + str32len (theFindString.get()));
		GuiText_scrollToSelection (my textWidget);
		#ifdef _WIN32
			GuiThing_show (my windowForm);
		#endif
	} else {
		/*
			Try from the start of the document.
		*/
		location = str32str (text.get(), theFindString.get());
		if (location) {
			integer index = location - text.get();
			GuiText_setSelection (my textWidget, index, index + str32len (theFindString.get()));
			GuiText_scrollToSelection (my textWidget);
			#ifdef _WIN32
				GuiThing_show (my windowForm);
			#endif
		} else {
			Melder_beep ();
		}
	}
}

static void do_replace (TextEditor me) {
	if (! theReplaceString) return;   // e.g. when the user does "Replace again" before having done any "Replace"
	autostring32 selection = GuiText_getSelection (my textWidget);
	if (! Melder_equ (selection.get(), theFindString.get())) {
		do_find (me);
		return;
	}
	integer left, right;
	autostring32 text = GuiText_getStringAndSelectionPosition (my textWidget, & left, & right);
	GuiText_replace (my textWidget, left, right, theReplaceString.get());
	GuiText_setSelection (my textWidget, left, left + str32len (theReplaceString.get()));
	GuiText_scrollToSelection (my textWidget);
	#ifdef _WIN32
		GuiThing_show (my windowForm);
	#endif
}

static void menu_cb_find (TextEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Find", nullptr)
		TEXTFIELD (findString, U"Find:", U"")
	EDITOR_OK
		if (theFindString) SET_STRING (findString, theFindString.get());
	EDITOR_DO
		theFindString = Melder_dup (findString);
		#ifdef macintosh
			/*
				Perhaps don't use the system-wide Find pasteboard,
				by which other applications can see what you are searching for in Praat's text windows.
				Remember ever showing your app in Xcode to somebody,
				revealing to your onlooker the name of the person you last looked up in your email?
			*/
			// NSPasteboard * theFindPasteBoard = [NSPasteboard pasteboardWithName: NSPasteboardNameFind   create: NO];
			// [theFindPasteBoard ...]
		#endif
		do_find (me);
	EDITOR_END
}

static void menu_cb_findAgain (TextEditor me, EDITOR_ARGS_DIRECT) {
	do_find (me);
}

static void menu_cb_useSelectionForFind (TextEditor me, EDITOR_ARGS_DIRECT) {
	theFindString = GuiText_getSelection (my textWidget);
}

static void menu_cb_replace (TextEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Find", nullptr)
		LABEL (U"This is a \"slow\" find-and-replace method;")
		LABEL (U"if the selected text is identical to the Find string,")
		LABEL (U"the selected text will be replaced by the Replace string;")
		LABEL (U"otherwise, the next occurrence of the Find string will be selected.")
		LABEL (U"So you typically need two clicks on Apply to get a text replaced.")
		TEXTFIELD (findString, U"Find:", U"")
		TEXTFIELD (replaceString, U"Replace with:", U"")
	EDITOR_OK
		if (theFindString) SET_STRING (findString, theFindString.get());
		if (theReplaceString) SET_STRING (replaceString, theReplaceString.get());
	EDITOR_DO
		theFindString = Melder_dup (findString);
		theReplaceString = Melder_dup (replaceString);
		do_replace (me);
	EDITOR_END
}

static void menu_cb_replaceAgain (TextEditor me, EDITOR_ARGS_DIRECT) {
	do_replace (me);
}

static void menu_cb_whereAmI (TextEditor me, EDITOR_ARGS_DIRECT) {
	integer numberOfLinesLeft, numberOfLinesRight;
	if (! getSelectedLines (me, & numberOfLinesLeft, & numberOfLinesRight)) {
		Melder_information (U"The cursor is on line ", numberOfLinesLeft, U".");
	} else if (numberOfLinesLeft == numberOfLinesRight) {
		Melder_information (U"The selection is on line ", numberOfLinesLeft, U".");
	} else {
		Melder_information (U"The selection runs from line ", numberOfLinesLeft, U" to line ", numberOfLinesRight, U".");
	}
}

static void menu_cb_goToLine (TextEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Go to line", nullptr)
		NATURAL (lineToGo, U"Line", U"1")
	EDITOR_OK
		integer firstLine, lastLine;
		getSelectedLines (me, & firstLine, & lastLine);
		SET_INTEGER (lineToGo, firstLine)
	EDITOR_DO
		autostring32 text = GuiText_getString (my textWidget);
		integer currentLine = 1;
		integer left = 0, right = 0;
		if (lineToGo == 1) {
			for (; text [right] != U'\n' && text [right] != U'\0'; right ++) { }
		} else {
			for (; text [left] != U'\0'; left ++) {
				if (text [left] == U'\n') {
					currentLine ++;
					if (currentLine == lineToGo) {
						left ++;
						for (right = left; text [right] != U'\n' && text [right] != U'\0'; right ++) { }
						break;
					}
				}
			}
		}
		if (left == str32len (text.get())) {
			right = left;
		} else if (text [right] == U'\n') {
			right ++;
		}
		GuiText_setSelection (my textWidget, left, right);
		GuiText_scrollToSelection (my textWidget);
	EDITOR_END
}

static void menu_cb_convertToCString (TextEditor me, EDITOR_ARGS_DIRECT) {
	autostring32 text = GuiText_getString (my textWidget);
	char32 buffer [2] = U" ";
	const conststring32 hex [16] = { U"0", U"1", U"2", U"3", U"4", U"5", U"6", U"7", U"8", U"9", U"A", U"B", U"C", U"D", U"E", U"F" };
	MelderInfo_open ();
	MelderInfo_write (U"\"");
	for (char32 *p = & text [0]; *p != U'\0'; p ++) {
		char32 kar = *p;
		if (kar == U'\n') {
			MelderInfo_write (U"\\n\"\n\"");
		} else if (kar == U'\t') {
			MelderInfo_write (U"   ");
		} else if (kar == U'\"') {
			MelderInfo_write (U"\\\"");
		} else if (kar == U'\\') {
			MelderInfo_write (U"\\\\");
		} else if (kar > 127) {
			if (kar <= 0x00FFFF) {
				MelderInfo_write (U"\\u", hex [kar >> 12], hex [(kar >> 8) & 0x00'000F], hex [(kar >> 4) & 0x00'000F], hex [kar & 0x00'000F]);
			} else {
				MelderInfo_write (U"\\U", hex [kar >> 28], hex [(kar >> 24) & 0x00'000F], hex [(kar >> 20) & 0x00'000F], hex [(kar >> 16) & 0x00'000F],
					hex [(kar >> 12) & 0x00'000F], hex [(kar >> 8) & 0x00'000F], hex [(kar >> 4) & 0x00'000F], hex [kar & 0x00'000F]);
			}
		} else {
			buffer [0] = *p;
			MelderInfo_write (& buffer [0]);
		}
	}
	MelderInfo_write (U"\"");
	MelderInfo_close ();
}

/***** 'Font' menu *****/

static void updateSizeMenu (TextEditor me) {
	if (my fontSizeButton_10) GuiMenuItem_check (my fontSizeButton_10, my p_fontSize == 10.0);
	if (my fontSizeButton_12) GuiMenuItem_check (my fontSizeButton_12, my p_fontSize == 12.0);
	if (my fontSizeButton_14) GuiMenuItem_check (my fontSizeButton_14, my p_fontSize == 14.0);
	if (my fontSizeButton_18) GuiMenuItem_check (my fontSizeButton_18, my p_fontSize == 18.0);
	if (my fontSizeButton_24) GuiMenuItem_check (my fontSizeButton_24, my p_fontSize == 24.0);
}
static void setFontSize (TextEditor me, double fontSize) {
	GuiText_setFontSize (my textWidget, fontSize);
	my pref_fontSize () = my p_fontSize = fontSize;
	updateSizeMenu (me);
}

static void menu_cb_10 (TextEditor me, EDITOR_ARGS_DIRECT) { setFontSize (me, 10.0); }
static void menu_cb_12 (TextEditor me, EDITOR_ARGS_DIRECT) { setFontSize (me, 12.0); }
static void menu_cb_14 (TextEditor me, EDITOR_ARGS_DIRECT) { setFontSize (me, 14.0); }
static void menu_cb_18 (TextEditor me, EDITOR_ARGS_DIRECT) { setFontSize (me, 18.0); }
static void menu_cb_24 (TextEditor me, EDITOR_ARGS_DIRECT) { setFontSize (me, 24.0); }
static void menu_cb_fontSize (TextEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Text window: Font size", nullptr)
		POSITIVE (fontSize, U"Font size (points)", U"12")
	EDITOR_OK
		SET_REAL (fontSize, my p_fontSize);
	EDITOR_DO
		setFontSize (me, fontSize);
	EDITOR_END
}

static void gui_text_cb_changed (TextEditor me, GuiTextEvent /* event */) {
	if (! my dirty) {
		my dirty = true;
		my v_nameChanged ();
	}
}

void structTextEditor :: v_createChildren () {
	textWidget = GuiText_createShown (our windowForm, 0, 0, Machine_getMenuBarHeight (), 0, GuiText_SCROLLED);
	GuiText_setChangedCallback (textWidget, gui_text_cb_changed, this);
}

void structTextEditor :: v_createMenus () {
	TextEditor_Parent :: v_createMenus ();

	if (v_fileBased ()) {
		Editor_addCommand (this, U"File", U"New", 'N', menu_cb_new);
		Editor_addCommand (this, U"File", U"Open...", 'O', menu_cb_open);
		Editor_addCommand (this, U"File", U"Reopen from disk", GuiMenu_SHIFT | 'O', menu_cb_reopen);
	} else {
		Editor_addCommand (this, U"File", U"Clear", 'N', menu_cb_clear);
	}
	Editor_addCommand (this, U"File", U"-- save --", 0, nullptr);
	if (v_fileBased ()) {
		Editor_addCommand (this, U"File", U"Save", 'S', menu_cb_save);
		Editor_addCommand (this, U"File", U"Save as...", 0, menu_cb_saveAs);
	} else {
		Editor_addCommand (this, U"File", U"Save as...", 'S', menu_cb_saveAs);
	}
	Editor_addCommand (this, U"File", U"-- close --", 0, nullptr);
	GuiText_setUndoItem (textWidget, Editor_addCommand (this, U"Edit", U"Undo", 'Z', menu_cb_undo));
	GuiText_setRedoItem (textWidget, Editor_addCommand (this, U"Edit", U"Redo", 'Y', menu_cb_redo));
	Editor_addCommand (this, U"Edit", U"-- cut copy paste --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Cut", 'X', menu_cb_cut);
	Editor_addCommand (this, U"Edit", U"Copy", 'C', menu_cb_copy);
	Editor_addCommand (this, U"Edit", U"Paste", 'V', menu_cb_paste);
	Editor_addCommand (this, U"Edit", U"Erase", 0, menu_cb_erase);

	Editor_addMenu (this, U"Search", 0);
	Editor_addCommand (this, U"Search", U"Find...", 'F', menu_cb_find);
	Editor_addCommand (this, U"Search", U"Find again", 'G', menu_cb_findAgain);
	Editor_addCommand (this, U"Search", U"Replace...", GuiMenu_SHIFT | 'F', menu_cb_replace);
	Editor_addCommand (this, U"Search", U"Replace again", GuiMenu_SHIFT | 'G', menu_cb_replaceAgain);
	Editor_addCommand (this, U"Search", U"Use selection for find", 'E', menu_cb_useSelectionForFind);
	Editor_addCommand (this, U"Search", U"-- line --", 0, nullptr);
	Editor_addCommand (this, U"Search", U"Where am I?", 0, menu_cb_whereAmI);
	Editor_addCommand (this, U"Search", U"Go to line...", 'L', menu_cb_goToLine);

	Editor_addMenu (this, U"Convert", 0);
	Editor_addCommand (this, U"Convert", U"Convert to C string", 0, menu_cb_convertToCString);

	Editor_addMenu (this, U"Font", 0);
	Editor_addCommand (this, U"Font", U"Font size...", 0, menu_cb_fontSize);
	fontSizeButton_10 = Editor_addCommand (this, U"Font", U"10", GuiMenu_CHECKBUTTON, menu_cb_10);
	fontSizeButton_12 = Editor_addCommand (this, U"Font", U"12", GuiMenu_CHECKBUTTON, menu_cb_12);
	fontSizeButton_14 = Editor_addCommand (this, U"Font", U"14", GuiMenu_CHECKBUTTON, menu_cb_14);
	fontSizeButton_18 = Editor_addCommand (this, U"Font", U"18", GuiMenu_CHECKBUTTON, menu_cb_18);
	fontSizeButton_24 = Editor_addCommand (this, U"Font", U"24", GuiMenu_CHECKBUTTON, menu_cb_24);
}

void TextEditor_init (TextEditor me, conststring32 initialText) {
	Editor_init (me, 0, 0, 600, 400, U"", nullptr);
	setFontSize (me, my p_fontSize);
	if (initialText) {
		GuiText_setString (my textWidget, initialText);
		my dirty = false;   // was set to true in valueChanged callback
		Thing_setName (me, U"");
	}
	theReferencesToAllOpenTextEditors. addItem_ref (me);
}

autoTextEditor TextEditor_create (conststring32 initialText) {
	try {
		autoTextEditor me = Thing_new (TextEditor);
		TextEditor_init (me.get(), initialText);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Text window not created.");
	}
}

void TextEditor_showOpen (TextEditor me) {
	cb_showOpen (Editor_getMenuCommand (me, U"File", U"Open..."));
}

/* End of file TextEditor.cpp */
