/* Editor.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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

void structEditorCommand :: v_destroy () {
	Melder_free (our itemTitle);
	Melder_free (our script);
	EditorCommand_Parent :: v_destroy ();
}

/********** class EditorMenu **********/

Thing_implement (EditorMenu, Thing, 0);

void structEditorMenu :: v_destroy () {
	Melder_free (our menuTitle);
	forget (our commands);
	EditorMenu_Parent :: v_destroy ();
}

/********** functions **********/

static void commonCallback (EditorCommand me, GuiMenuItemEvent /* event */) {
	if (my d_editor && my d_editor -> v_scriptable () && ! str32str (my itemTitle, U"...")) {
		UiHistory_write (U"\n");
		UiHistory_write_colonize (my itemTitle);
	}
	try {
		my commandCallback (my d_editor, me, nullptr, 0, nullptr, nullptr, nullptr);
	} catch (MelderError) {
		if (! Melder_hasError (U"Script exited.")) {
			Melder_appendError (U"Menu command \"", my itemTitle, U"\" not completed.");
		}
		Melder_flushError ();
	}
}

GuiMenuItem EditorMenu_addCommand (EditorMenu me, const char32 *itemTitle /* cattable */, long flags, EditorCommandCallback commandCallback)
{
	autoEditorCommand thee = Thing_new (EditorCommand);
	thy d_editor = my d_editor;
	thy menu = me;
	thy itemTitle = Melder_dup (itemTitle);
	thy itemWidget =
		! commandCallback ? GuiMenu_addSeparator (my menuWidget) :
		flags & Editor_HIDDEN ? nullptr :
		GuiMenu_addItem (my menuWidget, itemTitle, flags, commonCallback, thee.peek());   // DANGLE BUG: me can be killed by Collection_addItem(), but EditorCommand::destroy doesn't remove the item
	thy commandCallback = commandCallback;
	GuiMenuItem result = thy itemWidget;
	Collection_addItem_move (my commands, thee.move());
	return result;
}

/*GuiObject EditorCommand_getItemWidget (EditorCommand me) { return my itemWidget; }*/

EditorMenu Editor_addMenu (Editor me, const char32 *menuTitle, long flags) {
	autoEditorMenu thee = Thing_new (EditorMenu);
	thy d_editor = me;
	thy menuTitle = Melder_dup (menuTitle);
	thy menuWidget = GuiMenu_createInWindow (my d_windowForm, menuTitle, flags);
	thy commands = Ordered_create ();
	EditorMenu result = thee.peek();
	Collection_addItem_move (my menus, thee.move());
	return result;
}

/*GuiObject EditorMenu_getMenuWidget (EditorMenu me) { return my menuWidget; }*/

GuiMenuItem Editor_addCommand (Editor me, const char32 *menuTitle, const char32 *itemTitle, long flags, EditorCommandCallback commandCallback)
{
	try {
		long numberOfMenus = my menus -> size;
		for (long imenu = 1; imenu <= numberOfMenus; imenu ++) {
			EditorMenu menu = (EditorMenu) my menus -> item [imenu];
			if (str32equ (menuTitle, menu -> menuTitle))
				return EditorMenu_addCommand (menu, itemTitle, flags, commandCallback);
		}
		Melder_throw (U"Menu \"", menuTitle, U"\" does not exist.");
	} catch (MelderError) {
		Melder_throw (U"Command \"", itemTitle, U"\" not inserted in menu \"", menuTitle, U".");
	}
}

static void Editor_scriptCallback (Editor me, EditorCommand cmd, UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter) {
	(void) sendingForm;
	(void) narg;
	(void) args;
	(void) sendingString;
	(void) interpreter;
	DO_RunTheScriptFromAnyAddedEditorCommand (me, cmd -> script);
}

GuiMenuItem Editor_addCommandScript (Editor me, const char32 *menuTitle, const char32 *itemTitle, long flags,
	const char32 *script)
{
	long numberOfMenus = my menus -> size;
	for (long imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = (EditorMenu) my menus -> item [imenu];
		if (str32equ (menuTitle, menu -> menuTitle)) {
			autoEditorCommand cmd = Thing_new (EditorCommand);
			cmd -> d_editor = me;
			cmd -> menu = menu;
			cmd -> itemTitle = Melder_dup_f (itemTitle);
			cmd -> itemWidget = script == nullptr ? GuiMenu_addSeparator (menu -> menuWidget) :
				GuiMenu_addItem (menu -> menuWidget, itemTitle, flags, commonCallback, cmd.peek());   // DANGLE BUG
			cmd -> commandCallback = Editor_scriptCallback;
			if (str32len (script) == 0) {
				cmd -> script = Melder_dup_f (U"");
			} else {
				structMelderFile file = { 0 };
				Melder_relativePathToFile (script, & file);
				cmd -> script = Melder_dup_f (Melder_fileToPath (& file));
			}
			GuiMenuItem result = cmd -> itemWidget;
			Collection_addItem_move (menu -> commands, cmd.move());
			return result;
		}
	}
	Melder_warning (
		U"Menu \"", menuTitle, U"\" does not exist.\n"
		U"Command \"", itemTitle, U"\" not inserted in menu \"", menuTitle, U".\n"
		U"To fix this, go to Praat->Preferences->Buttons->Editors, and remove the script from this menu.\n"
		U"You may want to install the script in a different menu.");
	return nullptr;
}

void Editor_setMenuSensitive (Editor me, const char32 *menuTitle, int sensitive) {
	int numberOfMenus = my menus -> size;
	for (int imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = (EditorMenu) my menus -> item [imenu];
		if (str32equ (menuTitle, menu -> menuTitle)) {
			GuiThing_setSensitive (menu -> menuWidget, sensitive);
			return;
		}
	}
}

EditorCommand Editor_getMenuCommand (Editor me, const char32 *menuTitle, const char32 *itemTitle) {
	int numberOfMenus = my menus -> size;
	for (int imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = (EditorMenu) my menus -> item [imenu];
		if (str32equ (menuTitle, menu -> menuTitle)) {
			int numberOfCommands = menu -> commands -> size, icommand;
			for (icommand = 1; icommand <= numberOfCommands; icommand ++) {
				EditorCommand command = (EditorCommand) menu -> commands -> item [icommand];
				if (str32equ (itemTitle, command -> itemTitle))
					return command;
			}
		}
	}
	Melder_throw (U"Command \"", itemTitle, U"\" not found in menu \"", menuTitle, U"\".");
}

void Editor_doMenuCommand (Editor me, const char32 *commandTitle, int narg, Stackel args, const char32 *arguments, Interpreter interpreter) {
	int numberOfMenus = my menus -> size;
	for (int imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = (EditorMenu) my menus -> item [imenu];
		long numberOfCommands = menu -> commands -> size;
		for (long icommand = 1; icommand <= numberOfCommands; icommand ++) {
			EditorCommand command = (EditorCommand) menu -> commands -> item [icommand];
			if (str32equ (commandTitle, command -> itemTitle)) {
				command -> commandCallback (me, command, nullptr, narg, args, arguments, interpreter);
				return;
			}
		}
	}
	Melder_throw (U"Command not available in ", my classInfo -> className, U".");
}

/********** class Editor **********/

void structEditor :: v_destroy () {
	trace (U"enter");
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	/*
	 * The following command must be performed before the shell is destroyed.
	 * Otherwise, we would be forgetting dangling command dialogs here.
	 */
	forget (our menus);
	Editor_broadcastDestruction (this);
	if (our d_windowForm) {
		#if gtk
			if (our d_windowForm -> d_gtkWindow) {
				Melder_assert (GTK_IS_WIDGET (our d_windowForm -> d_gtkWindow));
				gtk_widget_destroy (GTK_WIDGET (our d_windowForm -> d_gtkWindow));
			}
		#elif cocoa
			if (our d_windowForm -> d_cocoaWindow) {
				NSWindow *cocoaWindow = our d_windowForm -> d_cocoaWindow;
				//d_windowForm -> d_cocoaWindow = nullptr;
				[cocoaWindow close];
			}
		#elif motif
			if (our d_windowForm -> d_xmShell) {
				XtDestroyWidget (our d_windowForm -> d_xmShell);
			}
		#endif
	}
	forget (our previousData);
	if (our d_ownData) forget (our data);
	Melder_free (our callbackSocket);
	Editor_Parent :: v_destroy ();
}

void structEditor :: v_info () {
	MelderInfo_writeLine (U"Editor type: ", Thing_className (this));
	MelderInfo_writeLine (U"Editor name: ", our name ? our name : U"<no name>");
	time_t today = time (nullptr);
	MelderInfo_writeLine (U"Date: ", Melder_peek8to32 (ctime (& today)));   // includes a newline
	if (our data) {
		MelderInfo_writeLine (U"Data type: ", our data -> classInfo -> className);
		MelderInfo_writeLine (U"Data name: ", our data -> name);
	}
}

void structEditor :: v_nameChanged () {
	if (our name)
		GuiShell_setTitle (our d_windowForm, our name);
}

void structEditor :: v_saveData () {
	if (! our data) return;
	forget (our previousData);
	our previousData = Data_copy (our data);
}

void structEditor :: v_restoreData () {
	if (our data && our previousData)
		Thing_swap (our data, our previousData);
}

static void menu_cb_sendBackToCallingProgram (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	if (my data) {
		extern structMelderDir praatDir;
		structMelderFile file = { 0 };
		MelderDir_getFile (& praatDir, U"praat_backToCaller.Data", & file);
		Data_writeToTextFile (my data, & file);
		sendsocket (my callbackSocket, Melder_peek32to8 (my data -> name));
	}
	my v_goAway ();
}

static void menu_cb_close (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	my v_goAway ();
}

static void menu_cb_undo (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	my v_restoreData ();
	if (str32nequ (my undoText, U"Undo", 4)) my undoText [0] = U'R', my undoText [1] = U'e';
	else if (str32nequ (my undoText, U"Redo", 4)) my undoText [0] = U'U', my undoText [1] = U'n';
	else str32cpy (my undoText, U"Undo?");
	#if gtk
		gtk_label_set_label (GTK_LABEL (gtk_bin_get_child (GTK_BIN (my undoButton -> d_widget))), Melder_peek32to8 (my undoText));
	#elif cocoa
		[(GuiCocoaMenuItem *) my undoButton -> d_widget   setTitle: (NSString *) Melder_peek32toCfstring (my undoText)];
	#elif motif
		char *text_utf8 = Melder_peek32to8 (my undoText);
		XtVaSetValues (my undoButton -> d_widget, XmNlabelString, text_utf8, nullptr);
	#endif
	/*
	 * Send a message to myself (e.g., I will redraw myself).
	 */
	my v_dataChanged ();
	/*
	 * Send a message to my boss (e.g., she will notify the others that depend on me).
	 */
	Editor_broadcastDataChanged (me);
}

static void menu_cb_searchManual (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	Melder_search ();
}

static void menu_cb_newScript (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	(void) ScriptEditor_createFromText (me, nullptr);
}

static void menu_cb_openScript (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	autoScriptEditor scriptEditor = ScriptEditor_createFromText (me, nullptr);
	TextEditor_showOpen (scriptEditor.transfer());
}

void structEditor :: v_createMenuItems_file (EditorMenu menu) {
	(void) menu;
}

void structEditor :: v_createMenuItems_edit (EditorMenu menu) {
	if (our data)
		our undoButton = EditorMenu_addCommand (menu, U"Cannot undo", GuiMenu_INSENSITIVE + 'Z', menu_cb_undo);
}

static void menu_cb_settingsReport (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	Thing_info (me);
}

static void menu_cb_info (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	if (my data) Thing_info (my data);
}

void structEditor :: v_createMenuItems_query (EditorMenu menu) {
	v_createMenuItems_query_info (menu);
}

void structEditor :: v_createMenuItems_query_info (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"Editor info", 0, menu_cb_settingsReport);
	EditorMenu_addCommand (menu, U"Settings report", Editor_HIDDEN, menu_cb_settingsReport);
	if (data) {
		EditorMenu_addCommand (menu, Melder_cat (Thing_className (data), U" info"), 0, menu_cb_info);
	}
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

void structEditor :: v_form_pictureWindow (EditorCommand cmd) {
	LABEL (U"", U"Picture window:")
	BOOLEAN (U"Erase first", 1);
}
void structEditor :: v_ok_pictureWindow (EditorCommand cmd) {
	SET_INTEGER (U"Erase first", pref_picture_eraseFirst ());
}
void structEditor :: v_do_pictureWindow (EditorCommand cmd) {
	pref_picture_eraseFirst () = GET_INTEGER (U"Erase first");
}

void structEditor :: v_form_pictureMargins (EditorCommand cmd) {
	UiField radio;
	LABEL (U"", U"Margins:")
	OPTIONMENU_ENUM (U"Write name at top", kEditor_writeNameAtTop, kEditor_writeNameAtTop_DEFAULT);
}
void structEditor :: v_ok_pictureMargins (EditorCommand cmd) {
	SET_ENUM (U"Write name at top", kEditor_writeNameAtTop, pref_picture_writeNameAtTop ());
}
void structEditor :: v_do_pictureMargins (EditorCommand cmd) {
	pref_picture_writeNameAtTop () = GET_ENUM (kEditor_writeNameAtTop, U"Write name at top");
}

static void gui_window_cb_goAway (Editor me) {
	Melder_assert (me);
	Melder_assert (Thing_isa (me, classEditor));
	my v_goAway ();
}

void praat_addCommandsToEditor (Editor me);
void Editor_init (Editor me, int x, int y, int width, int height, const char32 *title, Daata data) {
	double xmin, ymin, widthmax, heightmax;
	Gui_getWindowPositioningBounds (& xmin, & ymin, & widthmax, & heightmax);
	/*
	 * Negative widths are relative to the whole screen.
	 */
	if (width < 0) width += (int) widthmax;
	if (height < 0) height += (int) heightmax;
	/*
	 * Don't start with a maximized window, because then the user doesn't know what a click on the maximize button means.
	 */
	if (width > (int) widthmax - 100) width = (int) widthmax - 100;
	if (height > (int) heightmax - 100) height = (int) heightmax - 100;
	/*
	 * Make sure that the window has at least a sane size.
	 * Just in case the user made the previous window very small (Praat's FunctionEditor saves the last size),
	 * or the user edited the preferences file (which might save a window size).
	 */
	if (width < 200) width = 200;
	if (height < 150) height = 150;
	/*
	 * Now that the size is right, establish the position.
	 */
	int left, right, top, bottom;
	if (x > 0) {
		/*
		 * Positive x: relative to the left edge of the screen.
		 */
		left = (int) xmin + x;
		right = left + width;
	} else if (x < 0) {
		/*
		 * Negative x: relative to the right edge of the screen.
		 */
		right = (int) xmin + (int) widthmax + x;
		left = right - width;
	} else {
		/*
		 * Zero x: randomize between the left and right edge of the screen.
		 */
		left = NUMrandomInteger ((int) xmin + 4, (int) xmin + (int) widthmax - width - 4);
		right = left + width;
	}
	if (y > 0) {
		/*
		 * Positive y: relative to the top of the screen.
		 */
		top = (int) ymin + y;
		bottom = top + height;
	} else if (y < 0) {
		/*
		 * Negative y: relative to the bottom of the screen.
		 */
		bottom = (int) ymin + (int) heightmax + y;
		top = bottom - height;
	} else {
		/*
		 * Zero y: randomize between the top and bottom of the screen.
		 */
		top = NUMrandomInteger ((int) ymin + 4, (int) ymin + (int) heightmax - height - 4);
		//Melder_casual (ymin, U" ", heightmax, U" ", height, U" ", top);
		bottom = top + height;
	}
	#if defined (macintoshXXX) || gtk
		top += Machine_getTitleBarHeight ();
		bottom += Machine_getTitleBarHeight ();
	#endif
	my d_windowForm = GuiWindow_create (left, top, width, height, 450, 250, title, gui_window_cb_goAway, me, my v_canFullScreen () ? GuiWindow_FULLSCREEN : 0);
	Thing_setName (me, title);
	my data = data;
	my v_copyPreferencesToInstance ();

	/* Create menus. */

	if (my v_hasMenuBar ()) {
		my menus = Ordered_create ();
		GuiWindow_addMenuBar (my d_windowForm);
	}

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
		 * Add the scripted commands.
		 */
		praat_addCommandsToEditor (me);
		if (my callbackSocket)
			Editor_addCommand (me, U"File", U"Send back to calling program", 0, menu_cb_sendBackToCallingProgram);
		Editor_addCommand (me, U"File", U"Close", 'W', menu_cb_close);
	}
	GuiThing_show (my d_windowForm);
}

void Editor_save (Editor me, const char32 *text) {
	my v_saveData ();
	if (! my undoButton) return;
	GuiThing_setSensitive (my undoButton, true);
	Melder_sprint (my undoText,100, U"Undo ", text);
	#if gtk
		gtk_label_set_label (GTK_LABEL (gtk_bin_get_child (GTK_BIN (my undoButton -> d_widget))), Melder_peek32to8 (my undoText));
	#elif cocoa
		[(GuiCocoaMenuItem *) my undoButton -> d_widget   setTitle: (NSString *) Melder_peek32toCfstring (my undoText)];
	#elif motif
		char *text_utf8 = Melder_peek32to8 (my undoText);
		XtVaSetValues (my undoButton -> d_widget, XmNlabelString, text_utf8, nullptr);
	#endif
}

void Editor_openPraatPicture (Editor me) {
	my pictureGraphics = praat_picture_editor_open (my pref_picture_eraseFirst ());
}
void Editor_closePraatPicture (Editor me) {
	if (my data && my pref_picture_writeNameAtTop () != kEditor_writeNameAtTop_NO) {
		Graphics_setNumberSignIsBold (my pictureGraphics, false);
		Graphics_setPercentSignIsItalic (my pictureGraphics, false);
		Graphics_setCircumflexIsSuperscript (my pictureGraphics, false);
		Graphics_setUnderscoreIsSubscript (my pictureGraphics, false);
		Graphics_textTop (my pictureGraphics,
			my pref_picture_writeNameAtTop () == kEditor_writeNameAtTop_FAR,
			my data -> name);
		Graphics_setNumberSignIsBold (my pictureGraphics, true);
		Graphics_setPercentSignIsItalic (my pictureGraphics, true);
		Graphics_setCircumflexIsSuperscript (my pictureGraphics, true);
		Graphics_setUnderscoreIsSubscript (my pictureGraphics, true);
	}
	praat_picture_editor_close ();
}

/* End of file Editor.cpp */
