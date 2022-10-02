/* Editor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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

Thing_implement (Editor, DataGui, 0);

#include "Prefs_define.h"
#include "Editor_prefs.h"
#include "Prefs_install.h"
#include "Editor_prefs.h"
#include "Prefs_copyToInstance.h"
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
		my commandCallback (my sender, me, nullptr, 0, nullptr, nullptr, nullptr);
	} catch (MelderError) {
		if (! Melder_hasError (U"Script exited."))
			Melder_appendError (U"Menu command \"", my itemTitle.get(), U"\" not completed.");
		Melder_flushError ();
	}
}

static GuiMenuItem DataGuiMenu_addCommand_ (EditorMenu me, conststring32 itemTitle /* cattable */, uint32 flags,
	DataGuiCommandCallback commandCallback, DataGui optionalSender)
{
	autoEditorCommand thee = Thing_new (EditorCommand);
	thy d_editor = my d_editor;
	thy sender = ( optionalSender ? optionalSender : thy d_editor );
	thy menu = me;
	const bool titleIsHeader = Melder_stringMatchesCriterion (itemTitle, kMelder_string::ENDS_WITH, U":", true);
	if (titleIsHeader) {
		flags |= GuiMenu_UNDERLINED;
		if (itemTitle [0] == U'-' && itemTitle [1] == U' ') {
			GuiMenu_addSeparator (my menuWidget);
			itemTitle += 2;
		}
	}
	const int depth = (flags & GuiMenu_DEPTH_3) >> 16;   // the maximum depth in editor windows is 3
	thy itemTitle = Melder_dup (itemTitle);
	if (depth > 0) {
		/*
			bikeshed choices for indented menu items
		*/
		[[maybe_unused]] constexpr conststring32 space = U"      ";   // minimalist
		[[maybe_unused]] constexpr conststring32 fourDots = U"\u205E   ";   // not evenly dispersed
		[[maybe_unused]] constexpr conststring32 twoDots = U"\u205A   ";   // dispersion OKish
		[[maybe_unused]] constexpr conststring32 oneDot = U"·   ";   // dispersion good, not too prominent, not really thin
		[[maybe_unused]] constexpr conststring32 hyphenationPoint = U"\u2027   ";   // xx
		[[maybe_unused]] constexpr conststring32 pipe = U"|   ";   // not long enough, prominent
		[[maybe_unused]] constexpr conststring32 boxDrawingLightVertical = U"\u2502   ";   // not long enough, prominent
		[[maybe_unused]] constexpr conststring32 boxDrawingLightQuadrupleDashVertical = U"\u250A   ";   // not long enough, but nicely thin
		itemTitle = Melder_cat (space, itemTitle);
	}
	if (! commandCallback)
		flags |= GuiMenu_INSENSITIVE;
	thy itemWidget =
		titleIsHeader ? GuiMenu_addItem (my menuWidget, itemTitle, flags, nullptr, nullptr) :
		! commandCallback ? GuiMenu_addSeparator (my menuWidget) :
		flags & GuiMenu_HIDDEN ? nullptr :
		GuiMenu_addItem (my menuWidget, itemTitle, flags, commonCallback, thee.get())
	;   // DANGLE BUG: me can be killed by Collection_addItem(), but EditorCommand::destroy doesn't remove the item
	thy commandCallback = commandCallback;
	GuiMenuItem result = thy itemWidget;
	my commands. addItem_move (thee.move());
	return result;
}
GuiMenuItem DataGuiMenu_addCommand (EditorMenu me, conststring32 itemTitle /* cattable */, uint32 flags,
	DataGuiCommandCallback commandCallback, DataGui optionalSender)
{
	if (flags < 3)   // the maximum depth in editor windows is 3
		flags *= GuiMenu_DEPTH_1;   // turn 1..3 into GuiMenu_DEPTH_1..GuiMenu_DEPTH_3, because the flags are ORed below
	const char32 *pSeparator = str32str (itemTitle, U" || ");
	if (! pSeparator)
		return DataGuiMenu_addCommand_ (me, itemTitle, flags, commandCallback, optionalSender);
	integer positionOfSeparator = pSeparator - itemTitle;
	static MelderString string;
	MelderString_copy (& string, itemTitle);
	char32 *pTitle = & string. string [0];
	GuiMenuItem result = nullptr;
	do {
		pTitle [positionOfSeparator] = U'\0';
		GuiMenuItem menuItem = DataGuiMenu_addCommand_ (me, pTitle, flags, commandCallback, optionalSender);
		if (menuItem)
			result = menuItem;   // only the first
		pTitle += positionOfSeparator + 4;   // step past " || "
		pSeparator = str32str (pTitle, U" || ");
		if (pSeparator)
			positionOfSeparator = pSeparator - pTitle;
		flags |= GuiMenu_HIDDEN;
	} while (pSeparator);
	(void) DataGuiMenu_addCommand_ (me, pTitle, flags, commandCallback, optionalSender);
	return result;
}
GuiMenuItem EditorMenu_addCommand (EditorMenu me, conststring32 itemTitle /* cattable */, uint32 flags,
	EditorCommandCallback commandCallback)
{
	return DataGuiMenu_addCommand (me, itemTitle, flags, commandCallback.get(), nullptr);
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

static EditorMenu findMenu (Editor me, conststring32 menuTitle) {
	const integer numberOfMenus = my menus.size;
	for (integer imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = my menus.at [imenu];
		if (str32equ (menu -> menuTitle.get(), menuTitle))
			return menu;
	}
	return nullptr;
}
static GuiMenuItem EditorMenu_addCommandScript (EditorMenu me, conststring32 itemTitle, uint32 flags, conststring32 script) {
	autoEditorCommand cmd = Thing_new (EditorCommand);
	cmd -> d_editor = my d_editor;
	cmd -> sender = my d_editor;
	cmd -> menu = me;
	cmd -> itemTitle = Melder_dup (itemTitle);
	cmd -> itemWidget = script == nullptr ? GuiMenu_addSeparator (my menuWidget) :
		GuiMenu_addItem (my menuWidget, itemTitle, flags, commonCallback, cmd.get());   // DANGLE BUG
	cmd -> commandCallback = Editor_scriptCallback;
	if (script [0] == U'\0') {
		cmd -> script = Melder_dup (U"");
	} else {
		structMelderFile file { };
		Melder_relativePathToFile (script, & file);
		cmd -> script = Melder_dup (Melder_fileToPath (& file));
	}
	GuiMenuItem result = cmd -> itemWidget;
	my commands. addItem_move (cmd.move());
	return result;
}
GuiMenuItem Editor_addCommandScript (Editor me, conststring32 menuTitle, conststring32 itemTitle, uint32 flags,
	conststring32 script)
{
	EditorMenu menu = findMenu (me, menuTitle);
	if (menu)
		return EditorMenu_addCommandScript (menu, itemTitle, flags, script);
	/*
		Try again, in case the menu title is obsolete.
	*/
	conststring32 alternativeMenuTitle =
		str32equ (menuTitle, U"Spectrum") ? U"Spectrogram" :
		str32equ (menuTitle, U"View") || str32equ (menuTitle, U"Select") ?	U"Time" :   // or "Play", but we cannot know
		str32equ (menuTitle, U"Formant") ? U"Formants" :
		str32equ (menuTitle, U"Query") ? U"Edit" :   // not appropriate, but better than nothing
		nullptr;
	if (alternativeMenuTitle) {
		EditorMenu alternativeMenu = findMenu (me, alternativeMenuTitle);
		if (alternativeMenu) {
			GuiMenuItem menuItem = EditorMenu_addCommandScript (alternativeMenu, itemTitle, flags, script);
			static bool warningGiven = false;
			if (! warningGiven) {
				warningGiven = true;
				Melder_warning (U"The menu \"", menuTitle, U"\" no longer exists. The command \"", itemTitle,
						U"\" has been installed in the menu \"", alternativeMenuTitle, U"\" instead. You could consider updating the script \"", script, U"\".");
			}
			return menuItem;
		} // else issue the original warning
	}
	Melder_warning (
		U"Menu \"", menuTitle, U"\" does not exist.\n"
		U"Command \"", itemTitle, U"\" not inserted in menu \"", menuTitle, U"\".\n"
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
				command -> commandCallback (command -> sender, command, nullptr, narg, args, arguments, interpreter);
				return;
			}
		}
	}
	Melder_throw (U"Command not available in ", my classInfo -> className, U".");
}

/********** class Editor **********/

void structEditor :: v9_destroy () noexcept {
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
	Editor_Parent :: v9_destroy ();
}

void structEditor :: v1_info () {
	// skipping parent classes
	MelderInfo_writeLine (U"Editor type: ", Thing_className (this));
	MelderInfo_writeLine (U"Editor name: ", our name ? our name.get() : U"<no name>");
	time_t today = time (nullptr);
	MelderInfo_writeLine (U"Date: ", Melder_peek8to32 (ctime (& today)));   // includes a newline
	if (our data()) {
		MelderInfo_writeLine (U"Data type: ", our data() -> classInfo -> className);
		MelderInfo_writeLine (U"Data name: ", our data() -> name.get());
	}
}

void structEditor :: v_nameChanged () {
	if (our name)
		GuiShell_setTitle (our windowForm, our name.get());
}

void structEditor :: v_saveData () {
	if (! our data())
		return;
	our previousData = Data_copy (our data());
}

void structEditor :: v_restoreData () {
	if (our data() && our previousData)
		Thing_swap (our data(), our previousData.get());
}

static void menu_cb_sendBackToCallingProgram (Editor me, EDITOR_ARGS_DIRECT) {
	if (my data()) {
		structMelderFile file { };
		MelderDir_getFile (& Melder_preferencesFolder, U"praat_backToCaller.Data", & file);
		Data_writeToTextFile (my data(), & file);
		sendsocket (Melder_peek32to8 (my callbackSocket.get()), Melder_peek32to8 (my data() -> name.get()));
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

void structEditor :: v_createMenuItems_edit (EditorMenu menu) {
	if (our data())
		our undoButton = EditorMenu_addCommand (menu, U"Cannot undo", GuiMenu_INSENSITIVE + 'Z', menu_cb_undo);
}

static void INFO_EDITOR__settingsReport (Editor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_EDITOR
		Thing_info (me);
	INFO_EDITOR_END
}

static void INFO_DATA__info (Editor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		if (my data())
			Thing_info (my data());
	INFO_DATA_END
}

void structEditor :: v_createMenus () {
	v_createMenuItems_prefs (our fileMenu);
	v_createMenuItems_save (our fileMenu);
	if (our v_hasEditMenu ()) {
		our editMenu = Editor_addMenu (this, U"Edit", 0);
		v_createMenuItems_edit (our editMenu);
	}
}

static void gui_window_cb_goAway (Editor me) {
	Melder_assert (me);
	Melder_assert (Thing_isa (me, classEditor));
	my v_goAway ();
}

void praat_addCommandsToEditor (Editor me);
void Editor_init (Editor me, int x, int y, int width, int height, conststring32 title, Daata data) {
	DataGui_init (me, data, true, me);   // I am my own boss! BUG: check editability
	/*
		Zero widths are taken from the preferences.
	*/
	if (width == 0)
		width = my classPref_shellWidth();
	if (height == 0)
		height = my classPref_shellHeight();

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
	my windowForm = GuiWindow_create (left, top, width, height, 450, 350, title,
			gui_window_cb_goAway, me, my v_canFullScreen () ? GuiWindow_FULLSCREEN : 0);
	Thing_setName (me, title);

	/*
		Create menus.
	*/

	if (my v_hasMenuBar ())
		GuiWindow_addMenuBar (my windowForm);

	my v_createChildren ();

	if (my v_hasMenuBar ()) {
		my fileMenu = Editor_addMenu (me, U"File", 0);
		if (my v_canReportSettings ()) {
			EditorMenu_addCommand (my fileMenu, U"Editor info || Settings report", 0, INFO_EDITOR__settingsReport);
			if (my data())
				EditorMenu_addCommand (my fileMenu, Melder_cat (Thing_className (my data()), U" info"), 0, INFO_DATA__info);
		}
		my v_createMenus ();
		EditorMenu helpMenu = Editor_addMenu (me, U"Help", 0);
		my v_createMenuItems_help (helpMenu);
		EditorMenu_addCommand (helpMenu, U"-- search --", 0, nullptr);
		my searchButton = EditorMenu_addCommand (helpMenu, U"Search manual...", 'M', menu_cb_searchManual);
		if (my v_scriptable ()) {
			EditorMenu_addCommand (my fileMenu, U"-- scripting --", 0, 0);
			EditorMenu_addCommand (my fileMenu, U"New editor script", 0, menu_cb_newScript);
			EditorMenu_addCommand (my fileMenu, U"Open editor script...", 0, menu_cb_openScript);
		}
		/*
			Add the scripted commands.
		*/
		praat_addCommandsToEditor (me);
		EditorMenu_addCommand (my fileMenu, U"-- closing --", 0, 0);
		if (my callbackSocket)
			EditorMenu_addCommand (my fileMenu, U"Send back to calling program", 0, menu_cb_sendBackToCallingProgram);
		EditorMenu_addCommand (my fileMenu, U"Close", 'W', menu_cb_close);
	}
	GuiThing_show (my windowForm);
}

void Editor_save (Editor me, conststring32 cattableText) {
	Melder_sprint (my undoText,100, U"Undo ", cattableText);
	my v_saveData ();
	if (! my undoButton)
		return;
	GuiThing_setSensitive (my undoButton, true);
	#if gtk
		gtk_label_set_label (GTK_LABEL (gtk_bin_get_child (GTK_BIN (my undoButton -> d_widget))), Melder_peek32to8 (my undoText));
	#elif motif
		conststring8 text_utf8 = Melder_peek32to8 (my undoText);
		XtVaSetValues (my undoButton -> d_widget, XmNlabelString, text_utf8, nullptr);
	#elif cocoa
		[(GuiCocoaMenuItem *) my undoButton -> d_widget   setTitle: (NSString *) Melder_peek32toCfstring (my undoText)];
	#endif
}

/* End of file Editor.cpp */
