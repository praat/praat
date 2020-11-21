/* Editor.cpp
 *
 * Copyright (C) 1992-2020 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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

#include <time.h>
#include "ScriptEditor.h"
#include "machine.h"
#include "EditorM.h"
#include "praat_script.h"
#include "sendsocket.h"

#include "enums_getText.h"
#include "Editor_enums.h"
#include "enums_getValue.h"
#include "Editor_enums.h"

Thing_implement (Editor, Thing, 0);

#include "prefs_define.h"
#include "Editor_prefs.h"
#include "prefs_install.h"
#include "Editor_prefs.h"
#include "prefs_copyToInstance.h"
#include "Editor_prefs.h"

/********** class EditorCommand **********/

Thing_implement (EditorCommand, Thing, 0);

/********** class EditorMenu **********/

Thing_implement (EditorMenu, Thing, 0);

/********** functions **********/

static void commonCallback (EditorCommand me, GuiMenuItemEvent /* event */) {
	if (my d_editor && my d_editor -> v_scriptable () && ! str32str (my itemTitle.get(), U"...")) {
		UiHistory_write (U"\n");
		UiHistory_write_colonize (my itemTitle.get());
	}
	try {
		my commandCallback (my d_editor, me, nullptr, 0, nullptr, nullptr, nullptr);
	} catch (MelderError) {
		if (! Melder_hasError (U"Script exited."))
			Melder_appendError (U"Menu command \"", my itemTitle.get(), U"\" not completed.");
		Melder_flushError ();
	}
}

GuiMenuItem EditorMenu_addCommand (EditorMenu me, conststring32 itemTitle /* cattable */, uint32 flags, EditorCommandCallback commandCallback)
{
	autoEditorCommand thee = Thing_new (EditorCommand);
	thy d_editor = my d_editor;
	thy menu = me;
	thy itemTitle = Melder_dup (itemTitle);
	thy itemWidget =
			! commandCallback ? GuiMenu_addSeparator (my menuWidget) :
			flags & Editor_HIDDEN ? nullptr :
			GuiMenu_addItem (my menuWidget, itemTitle, flags, commonCallback, thee.get());   // DANGLE BUG: me can be killed by Collection_addItem(), but EditorCommand::destroy doesn't remove the item
	thy commandCallback = commandCallback;
	GuiMenuItem result = thy itemWidget;
	my commands. addItem_move (thee.move());
	return result;
}

/*GuiObject EditorCommand_getItemWidget (EditorCommand me) { return my itemWidget; }*/

EditorMenu Editor_addMenu (Editor me, conststring32 menuTitle, uint32 flags) {
	autoEditorMenu thee = Thing_new (EditorMenu);
	thy d_editor = me;
	thy menuTitle = Melder_dup (menuTitle);
	thy menuWidget = GuiMenu_createInWindow (my windowForm, menuTitle, flags);
	return my menus. addItem_move (thee.move());
}

/*GuiObject EditorMenu_getMenuWidget (EditorMenu me) { return my menuWidget; }*/

GuiMenuItem Editor_addCommand (Editor me, conststring32 menuTitle, conststring32 itemTitle, uint32 flags, EditorCommandCallback commandCallback)
{
	try {
		integer numberOfMenus = my menus.size;
		for (integer imenu = 1; imenu <= numberOfMenus; imenu ++) {
			EditorMenu menu = my menus.at [imenu];
			if (str32equ (menuTitle, menu -> menuTitle.get()))
				return EditorMenu_addCommand (menu, itemTitle, flags, commandCallback);
		}
		Melder_throw (U"Menu \"", menuTitle, U"\" does not exist.");
	} catch (MelderError) {
		Melder_throw (U"Command \"", itemTitle, U"\" not inserted in menu \"", menuTitle, U".");
	}
}

static void Editor_scriptCallback (Editor me, EditorCommand cmd, UiForm /* sendingForm */,
	integer /* narg */, Stackel /* args */, conststring32 /* sendingString */, Interpreter /* interpreter */)
{
	DO_RunTheScriptFromAnyAddedEditorCommand (me, cmd -> script.get());
}

GuiMenuItem Editor_addCommandScript (Editor me, conststring32 menuTitle, conststring32 itemTitle, uint32 flags,
	conststring32 script)
{
	integer numberOfMenus = my menus.size;
	for (integer imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = my menus.at [imenu];
		if (str32equ (menuTitle, menu -> menuTitle.get())) {
			autoEditorCommand cmd = Thing_new (EditorCommand);
			cmd -> d_editor = me;
			cmd -> menu = menu;
			cmd -> itemTitle = Melder_dup (itemTitle);
			cmd -> itemWidget = script == nullptr ? GuiMenu_addSeparator (menu -> menuWidget) :
				GuiMenu_addItem (menu -> menuWidget, itemTitle, flags, commonCallback, cmd.get());   // DANGLE BUG
			cmd -> commandCallback = Editor_scriptCallback;
			if (script [0] == U'\0') {
				cmd -> script = Melder_dup (U"");
			} else {
				structMelderFile file { };
				Melder_relativePathToFile (script, & file);
				cmd -> script = Melder_dup (Melder_fileToPath (& file));
			}
			GuiMenuItem result = cmd -> itemWidget;
			menu -> commands. addItem_move (cmd.move());
			return result;
		}
	}
	Melder_warning (
		U"Menu \"", menuTitle, U"\" does not exist.\n"
		U"Command \"", itemTitle, U"\" not inserted in menu \"", menuTitle, U".\n"
		U"To fix this, go to Praat->Preferences->Buttons->Editors, and remove the script from this menu.\n"
		U"You may want to install the script in a different menu."
	);
	return nullptr;
}

void Editor_setMenuSensitive (Editor me, conststring32 menuTitle, bool sensitive) {
	integer numberOfMenus = my menus.size;
	for (integer imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = my menus.at [imenu];
		if (str32equ (menuTitle, menu -> menuTitle.get())) {
			GuiThing_setSensitive (menu -> menuWidget, sensitive);
			return;
		}
	}
}

EditorCommand Editor_getMenuCommand (Editor me, conststring32 menuTitle, conststring32 itemTitle) {
	integer numberOfMenus = my menus.size;
	for (int imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = my menus.at [imenu];
		if (str32equ (menuTitle, menu -> menuTitle.get())) {
			integer numberOfCommands = menu -> commands.size, icommand;
			for (icommand = 1; icommand <= numberOfCommands; icommand ++) {
				EditorCommand command = menu -> commands.at [icommand];
				if (str32equ (itemTitle, command -> itemTitle.get()))
					return command;
			}
		}
	}
	Melder_throw (U"Command \"", itemTitle, U"\" not found in menu \"", menuTitle, U"\".");
}

void Editor_doMenuCommand (Editor me, conststring32 commandTitle, integer narg, Stackel args, conststring32 arguments, Interpreter interpreter) {
	integer numberOfMenus = my menus.size;
	for (int imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = my menus.at [imenu];
		integer numberOfCommands = menu -> commands.size;
		for (integer icommand = 1; icommand <= numberOfCommands; icommand ++) {
			EditorCommand command = menu -> commands.at [icommand];
			if (str32equ (commandTitle, command -> itemTitle.get())) {
				command -> commandCallback (me, command, nullptr, narg, args, arguments, interpreter);
				return;
			}
		}
	}
	Melder_throw (U"Command not available in ", my classInfo -> className, U".");
}

/********** class Editor **********/

void structEditor :: v_destroy () noexcept {
	trace (U"enter");
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	/*
		The following command must be performed before the shell is destroyed.
		Otherwise, we would be forgetting dangling command dialogs here.
	*/
	our menus.removeAllItems();

	Editor_broadcastDestruction (this);
	if (our windowForm) {
		#if gtk
			if (our windowForm -> d_gtkWindow) {
				Melder_assert (GTK_IS_WIDGET (our windowForm -> d_gtkWindow));
				gtk_widget_destroy (GTK_WIDGET (our windowForm -> d_gtkWindow));
			}
		#elif motif
			if (our windowForm -> d_xmShell) {
				XtDestroyWidget (our windowForm -> d_xmShell);
			}
		#elif cocoa
			if (our windowForm -> drawingArea) {
				GuiCocoaDrawingArea *cocoaDrawingArea = (GuiCocoaDrawingArea *) our windowForm -> drawingArea -> d_widget;
				if (cocoaDrawingArea)
					[cocoaDrawingArea   setUserData: nullptr];
			}
			if (our windowForm -> d_cocoaShell) {
				NSWindow *cocoaWindow = our windowForm -> d_cocoaShell;
				//our windowForm -> d_cocoaShell = nullptr;
				[cocoaWindow close];   // this should *release* the window as well, because `releasedWhenClosed` is on by default for NSWindows
			}
		#endif
	}
	if (our ownData)
		forget (our data);
	Editor_Parent :: v_destroy ();
}

void structEditor :: v_info () {
	MelderInfo_writeLine (U"Editor type: ", Thing_className (this));
	MelderInfo_writeLine (U"Editor name: ", our name ? our name.get() : U"<no name>");
	time_t today = time (nullptr);
	MelderInfo_writeLine (U"Date: ", Melder_peek8to32 (ctime (& today)));   // includes a newline
	if (our data) {
		MelderInfo_writeLine (U"Data type: ", our data -> classInfo -> className);
		MelderInfo_writeLine (U"Data name: ", our data -> name.get());
	}
}

void structEditor :: v_nameChanged () {
	if (our name)
		GuiShell_setTitle (our windowForm, our name.get());
}

void structEditor :: v_saveData () {
	if (! our data)
		return;
	our previousData = Data_copy (our data);
}

void structEditor :: v_restoreData () {
	if (our data && our previousData)
		Thing_swap (our data, our previousData.get());
}

static void menu_cb_sendBackToCallingProgram (Editor me, EDITOR_ARGS_DIRECT) {
	if (my data) {
		extern structMelderDir praatDir;
		structMelderFile file { };
		MelderDir_getFile (& praatDir, U"praat_backToCaller.Data", & file);
		Data_writeToTextFile (my data, & file);
		sendsocket (Melder_peek32to8 (my callbackSocket.get()), Melder_peek32to8 (my data -> name.get()));
	}
	my v_goAway ();
}

static void menu_cb_close (Editor me, EDITOR_ARGS_DIRECT) {
	my v_goAway ();
}

static void menu_cb_undo (Editor me, EDITOR_ARGS_DIRECT) {
	my v_restoreData ();
	if (str32nequ (my undoText, U"Undo", 4)) my undoText [0] = U'R', my undoText [1] = U'e';
	else if (str32nequ (my undoText, U"Redo", 4)) my undoText [0] = U'U', my undoText [1] = U'n';
	else str32cpy (my undoText, U"Undo?");
	#if gtk
		gtk_label_set_label (GTK_LABEL (gtk_bin_get_child (GTK_BIN (my undoButton -> d_widget))), Melder_peek32to8 (my undoText));
	#elif motif
		conststring8 text_utf8 = Melder_peek32to8 (my undoText);
		XtVaSetValues (my undoButton -> d_widget, XmNlabelString, text_utf8, nullptr);
	#elif cocoa
		[(GuiCocoaMenuItem *) my undoButton -> d_widget   setTitle: (NSString *) Melder_peek32toCfstring (my undoText)];
	#endif
	/*
		Send a message to myself (e.g., I will redraw myself).
	*/
	my v_dataChanged ();
	/*
		Send a message to my boss (e.g., she will notify the others that depend on me).
	*/
	Editor_broadcastDataChanged (me);
}

static void menu_cb_searchManual (Editor /* me */, EDITOR_ARGS_DIRECT) {
	Melder_search ();
}

static void menu_cb_newScript (Editor me, EDITOR_ARGS_DIRECT) {
	autoScriptEditor scriptEditor = ScriptEditor_createFromText (me, nullptr);
	scriptEditor.releaseToUser();
}

static void menu_cb_openScript (Editor me, EDITOR_ARGS_DIRECT) {
	autoScriptEditor scriptEditor = ScriptEditor_createFromText (me, nullptr);
	TextEditor_showOpen (scriptEditor.get());
	scriptEditor.releaseToUser();
}

void structEditor :: v_createMenuItems_file (EditorMenu /* menu */) {
}

void structEditor :: v_createMenuItems_edit (EditorMenu menu) {
	if (our data)
		our undoButton = EditorMenu_addCommand (menu, U"Cannot undo", GuiMenu_INSENSITIVE + 'Z', menu_cb_undo);
}

static void menu_cb_settingsReport (Editor me, EDITOR_ARGS_DIRECT) {
	Thing_info (me);
}

static void menu_cb_info (Editor me, EDITOR_ARGS_DIRECT) {
	if (my data)
		Thing_info (my data);
}

void structEditor :: v_createMenuItems_query (EditorMenu menu) {
	v_createMenuItems_query_info (menu);
}

void structEditor :: v_createMenuItems_query_info (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"Editor info", 0, menu_cb_settingsReport);
	EditorMenu_addCommand (menu, U"Settings report", Editor_HIDDEN, menu_cb_settingsReport);
	if (data)
		EditorMenu_addCommand (menu, Melder_cat (Thing_className (data), U" info"), 0, menu_cb_info);
}

void structEditor :: v_createMenus () {
	EditorMenu menu = Editor_addMenu (this, U"File", 0);
	v_createMenuItems_file (menu);
	if (v_editable ()) {
		menu = Editor_addMenu (this, U"Edit", 0);
		v_createMenuItems_edit (menu);
	}
	if (v_hasQueryMenu ()) {
		menu = Editor_addMenu (this, U"Query", 0);
		v_createMenuItems_query (menu);
	}
}

BOOLEAN_VARIABLE (v_form_pictureWindow_eraseFirst)
void structEditor :: v_form_pictureWindow (EditorCommand cmd) {
	LABEL (U"Picture window:")
	BOOLEAN_FIELD (v_form_pictureWindow_eraseFirst, U"Erase first", true)
}
void structEditor :: v_ok_pictureWindow (EditorCommand cmd) {
	SET_BOOLEAN (v_form_pictureWindow_eraseFirst, our pref_picture_eraseFirst ())
}
void structEditor :: v_do_pictureWindow (EditorCommand /* cmd */) {
	our pref_picture_eraseFirst () = v_form_pictureWindow_eraseFirst;
}

OPTIONMENU_ENUM_VARIABLE (kEditor_writeNameAtTop, v_form_pictureMargins_writeNameAtTop)
void structEditor :: v_form_pictureMargins (EditorCommand cmd) {
	LABEL (U"Margins:")
	OPTIONMENU_ENUM_FIELD (kEditor_writeNameAtTop, v_form_pictureMargins_writeNameAtTop,
			U"Write name at top", kEditor_writeNameAtTop::DEFAULT)
}
void structEditor :: v_ok_pictureMargins (EditorCommand cmd) {
	SET_ENUM (v_form_pictureMargins_writeNameAtTop, kEditor_writeNameAtTop, pref_picture_writeNameAtTop ())
}
void structEditor :: v_do_pictureMargins (EditorCommand /* cmd */) {
	pref_picture_writeNameAtTop () = v_form_pictureMargins_writeNameAtTop;
}

static void gui_window_cb_goAway (Editor me) {
	Melder_assert (me);
	Melder_assert (Thing_isa (me, classEditor));
	my v_goAway ();
}

void praat_addCommandsToEditor (Editor me);
void Editor_init (Editor me, int x, int y, int width, int height, conststring32 title, Daata data) {
	double xmin, ymin, widthmax, heightmax;
	Gui_getWindowPositioningBounds (& xmin, & ymin, & widthmax, & heightmax);
	/*
		Negative widths are relative to the whole screen.
	*/
	if (width < 0)
		width += (int) widthmax;
	if (height < 0)
		height += (int) heightmax;
	/*
		Don't start with a maximized window, because then the user doesn't know what a click on the maximize button means.
	*/
	Melder_clipRight (& width, (int) widthmax - 100);
	Melder_clipRight (& height, (int) heightmax - 100);
	/*
		Make sure that the window has at least a sane size,
		just in case the user made the previous window very small (Praat's FunctionEditor saves the last size),
		or the user edited the preferences file (which might save a window size).
	*/
	Melder_clipLeft (200, & width);
	Melder_clipLeft (150, & height);
	/*
		Now that the size is right, establish the position.
	*/
	int left, right, top, bottom;
	if (x > 0) {
		/*
			Positive x: relative to the left edge of the screen.
		*/
		left = (int) xmin + x;
		right = left + width;
	} else if (x < 0) {
		/*
			Negative x: relative to the right edge of the screen.
		*/
		right = (int) xmin + (int) widthmax + x;
		left = right - width;
	} else {
		/*
			Zero x: randomize between the left and right edge of the screen.
		*/
		left = (int) NUMrandomInteger ((int) xmin + 4, (int) xmin + (int) widthmax - width - 4);
		right = left + width;
	}
	if (y > 0) {
		/*
			Positive y: relative to the top of the screen.
		*/
		top = (int) ymin + y;
		bottom = top + height;
	} else if (y < 0) {
		/*
			Negative y: relative to the bottom of the screen.
		*/
		bottom = (int) ymin + (int) heightmax + y;
		top = bottom - height;
	} else {
		/*
			Zero y: randomize between the top and bottom of the screen.
		*/
		top = (int) NUMrandomInteger ((int) ymin + 4, (int) ymin + (int) heightmax - height - 4);
		//Melder_casual (ymin, U" ", heightmax, U" ", height, U" ", top);
		bottom = top + height;
	}
	#if defined (macintoshXXX) || gtk
		top += Machine_getTitleBarHeight ();
		bottom += Machine_getTitleBarHeight ();
	#endif
	my windowForm = GuiWindow_create (left, top, width, height, 450, 350, title, gui_window_cb_goAway, me, my v_canFullScreen () ? GuiWindow_FULLSCREEN : 0);
	Thing_setName (me, title);
	my data = data;
	my v_copyPreferencesToInstance ();

	/*
		Create menus.
	*/

	if (my v_hasMenuBar ())
		GuiWindow_addMenuBar (my windowForm);

	my v_createChildren ();

	if (my v_hasMenuBar ()) {
		my v_createMenus ();
		EditorMenu helpMenu = Editor_addMenu (me, U"Help", 0);
		my v_createHelpMenuItems (helpMenu);
		EditorMenu_addCommand (helpMenu, U"-- search --", 0, nullptr);
		my searchButton = EditorMenu_addCommand (helpMenu, U"Search manual...", 'M', menu_cb_searchManual);
		if (my v_scriptable ()) {
			Editor_addCommand (me, U"File", U"New editor script", 0, menu_cb_newScript);
			Editor_addCommand (me, U"File", U"Open editor script...", 0, menu_cb_openScript);
			Editor_addCommand (me, U"File", U"-- after script --", 0, 0);
		}
		/*
			Add the scripted commands.
		*/
		praat_addCommandsToEditor (me);
		if (my callbackSocket)
			Editor_addCommand (me, U"File", U"Send back to calling program", 0, menu_cb_sendBackToCallingProgram);
		Editor_addCommand (me, U"File", U"Close", 'W', menu_cb_close);
	}
	GuiThing_show (my windowForm);
}

void Editor_save (Editor me, conststring32 text) {
	my v_saveData ();
	if (! my undoButton)
		return;
	GuiThing_setSensitive (my undoButton, true);
	Melder_sprint (my undoText,100, U"Undo ", text);
	#if gtk
		gtk_label_set_label (GTK_LABEL (gtk_bin_get_child (GTK_BIN (my undoButton -> d_widget))), Melder_peek32to8 (my undoText));
	#elif motif
		conststring8 text_utf8 = Melder_peek32to8 (my undoText);
		XtVaSetValues (my undoButton -> d_widget, XmNlabelString, text_utf8, nullptr);
	#elif cocoa
		[(GuiCocoaMenuItem *) my undoButton -> d_widget   setTitle: (NSString *) Melder_peek32toCfstring (my undoText)];
	#endif
}

void Editor_openPraatPicture (Editor me) {
	my pictureGraphics = praat_picture_editor_open (my pref_picture_eraseFirst ());
}
void Editor_closePraatPicture (Editor me) {
	if (my data && my pref_picture_writeNameAtTop () != kEditor_writeNameAtTop::NO_) {
		Graphics_setNumberSignIsBold (my pictureGraphics, false);
		Graphics_setPercentSignIsItalic (my pictureGraphics, false);
		Graphics_setCircumflexIsSuperscript (my pictureGraphics, false);
		Graphics_setUnderscoreIsSubscript (my pictureGraphics, false);
		Graphics_textTop (my pictureGraphics,
			my pref_picture_writeNameAtTop () == kEditor_writeNameAtTop::FAR_,
			my data -> name.get()
		);
		Graphics_setNumberSignIsBold (my pictureGraphics, true);
		Graphics_setPercentSignIsItalic (my pictureGraphics, true);
		Graphics_setCircumflexIsSuperscript (my pictureGraphics, true);
		Graphics_setUnderscoreIsSubscript (my pictureGraphics, true);
	}
	praat_picture_editor_close ();
}

/* End of file Editor.cpp */
