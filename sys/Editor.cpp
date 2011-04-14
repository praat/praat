/* Editor.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2002/03/22 Editor_setPublish2Callback
 * pb 2005/09/01 do not add a "(cannot) undo" button if there is no data to save
 * pb 2007/06/10 wchar_t
 * pb 2007/09/02 form/ok/do_pictureWindow
 * pb 2007/09/08 createMenuItems_file and createMenuItems_edit
 * pb 2007/09/19 info
 * pb 2007/12/05 enums
 * pb 2008/03/20 split off Help menu
 * sdk 2008/03/24 GTK
 * pb 2009/01/18 arguments to UiForm callbacks
 * pb 2009/08/19 allow editor windows without menu bar (requested by DemoEditor)
 * fb 2010/02/23 GTK
 * pb 2010/07/29 removed GuiWindow_show
 * pb 2011/04/06
 */

#include <time.h>
#include "ScriptEditor.h"
#include "machine.h"
#include "EditorM.h"
#include "praat_script.h"
#include "Preferences.h"

#include "enums_getText.h"
#include "Editor_enums.h"
#include "enums_getValue.h"
#include "Editor_enums.h"

/********** PREFERENCES **********/

static struct {
	struct {
		bool eraseFirst;
		enum kEditor_writeNameAtTop writeNameAtTop;
	} picture;
} preferences;

void Editor_prefs (void) {
	Preferences_addBool (L"Editor.picture.eraseFirst", & preferences.picture.eraseFirst, true);
	Preferences_addEnum (L"Editor.picture.writeNameAtTop", & preferences.picture.writeNameAtTop, kEditor_writeNameAtTop, DEFAULT);
}

/********** class EditorCommand **********/

static void classEditorCommand_destroy (I) {
	iam (EditorCommand);
	Melder_free (my itemTitle);
	Melder_free (my script);
	forget (my dialog);
	inherited (EditorCommand) destroy (me);
}

class_methods (EditorCommand, Thing) {
	class_method_local (EditorCommand, destroy)
	class_methods_end
}

/********** class EditorMenu **********/

#define EditorMenu_members Thing_members \
	Any editor; \
	const wchar_t *menuTitle; \
	GuiObject menuWidget; \
	Ordered commands;
#define EditorMenu_methods Thing_methods
class_create_opaque (EditorMenu, Thing);

static void classEditorMenu_destroy (I) {
	iam (EditorMenu);
	Melder_free (my menuTitle);
	forget (my commands);
	inherited (EditorCommand) destroy (me);
}

class_methods (EditorMenu, Thing) {
	class_method_local (EditorMenu, destroy)
	class_methods_end
}

/********** functions **********/

static void commonCallback (GUI_ARGS) {
	GUI_IAM (EditorCommand);
	#if gtk
		if (G_OBJECT_TYPE (w) == GTK_TYPE_RADIO_MENU_ITEM && ! gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (w))) {
			return;
		}
	#endif
	if (my editor && ((Editor) my editor) -> methods -> scriptable && ! wcsstr (my itemTitle, L"...")) {
		UiHistory_write (L"\n");
		UiHistory_write (my itemTitle);
	}
	if (! my commandCallback (my editor, me, NULL, NULL, NULL)) Melder_flushError (NULL);
}

GuiObject EditorMenu_addCommand (EditorMenu menu, const wchar_t *itemTitle, long flags,
	int (*commandCallback) (Any editor_me, EditorCommand cmd, UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter))
{
	EditorCommand me = Thing_new (EditorCommand);
	my editor = menu -> editor;
	my menu = menu;
	if ((my itemTitle = Melder_wcsdup_e (itemTitle)) == NULL) { forget (me); return NULL; }
	my itemWidget =
		commandCallback == NULL ? GuiMenu_addSeparator (menu -> menuWidget) :
		flags & Editor_HIDDEN ? NULL :
		GuiMenu_addItem (menu -> menuWidget, itemTitle, flags, commonCallback, me);
	Collection_addItem (menu -> commands, me);
	my commandCallback = commandCallback;
	return my itemWidget;
}

/*GuiObject EditorCommand_getItemWidget (EditorCommand me) { return my itemWidget; }*/

EditorMenu Editor_addMenu (Any editor, const wchar_t *menuTitle, long flags) {
	EditorMenu me = Thing_new (EditorMenu);
	my editor = editor;
	if (! (my menuTitle = Melder_wcsdup_e (menuTitle))) { forget (me); return NULL; }
	my menuWidget = GuiMenuBar_addMenu (((Editor) editor) -> menuBar, menuTitle, flags);
	Collection_addItem (((Editor) editor) -> menus, me);
	my commands = Ordered_create ();
	return me;
}

/*GuiObject EditorMenu_getMenuWidget (EditorMenu me) { return my menuWidget; }*/

GuiObject Editor_addCommand (Any editor, const wchar_t *menuTitle, const wchar_t *itemTitle, long flags,
	int (*commandCallback) (Any editor_me, EditorCommand cmd, UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter))
{
	Editor me = (Editor) editor;
	int numberOfMenus = my menus -> size, imenu;
	for (imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = (EditorMenu) my menus -> item [imenu];
		if (wcsequ (menuTitle, menu -> menuTitle))
			return EditorMenu_addCommand (menu, itemTitle, flags, commandCallback);
	}
	Melder_error3 (L"(Editor_addCommand:) No menu \"", menuTitle, L"\". Cannot insert command.");
	return NULL;
}

static int Editor_scriptCallback (I, EditorCommand cmd, UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter) {
	iam (Editor);
	(void) sendingForm;
	(void) sendingString;
	(void) interpreter;
	return DO_RunTheScriptFromAnyAddedEditorCommand (me, cmd -> script);
}

GuiObject Editor_addCommandScript (Any editor, const wchar_t *menuTitle, const wchar_t *itemTitle, long flags,
	const wchar_t *script)
{
	Editor me = (Editor) editor;
	int numberOfMenus = my menus -> size, imenu;
	for (imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = (EditorMenu) my menus -> item [imenu];
		if (wcsequ (menuTitle, menu -> menuTitle)) {
			EditorCommand cmd = Thing_new (EditorCommand);
			cmd -> editor = me;
			cmd -> menu = menu;
			cmd -> itemTitle = Melder_wcsdup_f (itemTitle);
			cmd -> itemWidget = script == NULL ? GuiMenu_addSeparator (menu -> menuWidget) :
				GuiMenu_addItem (menu -> menuWidget, itemTitle, flags, commonCallback, cmd);
			Collection_addItem (menu -> commands, cmd);
			cmd -> commandCallback = Editor_scriptCallback;
			if (wcslen (script) == 0) {
				cmd -> script = Melder_wcsdup_f (L"");
			} else {
				structMelderFile file = { 0 };
				Melder_relativePathToFile (script, & file);
				cmd -> script = Melder_wcsdup_f (Melder_fileToPath (& file));
			}
			return cmd -> itemWidget;
		}
	}
	Melder_error3 (L"(Editor_addCommand:) No menu \"", menuTitle, L"\". Cannot insert command.");
	return NULL;
}

void Editor_setMenuSensitive (Any editor, const wchar_t *menuTitle, int sensitive) {
	Editor me = (Editor) editor;
	int numberOfMenus = my menus -> size, imenu;
	for (imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = (EditorMenu) my menus -> item [imenu];
		if (wcsequ (menuTitle, menu -> menuTitle)) {
			GuiObject_setSensitive (menu -> menuWidget, sensitive);
			return;
		}
	}
}

EditorCommand Editor_getMenuCommand (Editor me, const wchar_t *menuTitle, const wchar_t *itemTitle) {
	int numberOfMenus = my menus -> size, imenu;
	for (imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = (EditorMenu) my menus -> item [imenu];
		if (wcsequ (menuTitle, menu -> menuTitle)) {
			int numberOfCommands = menu -> commands -> size, icommand;
			for (icommand = 1; icommand <= numberOfCommands; icommand ++) {
				EditorCommand command = (EditorCommand) menu -> commands -> item [icommand];
				if (wcsequ (itemTitle, command -> itemTitle))
					return command;
			}
		}
	}
	Melder_error5 (L"(Editor_getMenuCommand:) No menu \"", menuTitle, L"\" with item \"", itemTitle, L"\".");
	return NULL;
}

int Editor_doMenuCommand (Editor me, const wchar_t *commandTitle, const wchar_t *arguments, Interpreter interpreter) {
	int numberOfMenus = my menus -> size, imenu;
	for (imenu = 1; imenu <= numberOfMenus; imenu ++) {
		EditorMenu menu = (EditorMenu) my menus -> item [imenu];
		int numberOfCommands = menu -> commands -> size, icommand;
		for (icommand = 1; icommand <= numberOfCommands; icommand ++) {
			EditorCommand command = (EditorCommand) menu -> commands -> item [icommand];
			if (wcsequ (commandTitle, command -> itemTitle)) {
				if (! command -> commandCallback (me, command, NULL, arguments, interpreter))
					return 0;
				return 1;
			}
		}
	}
	return Melder_error3 (L"Command not available in ", our _className, L".");
}

/********** class Editor **********/

static void classEditor_destroy (I) {
	iam (Editor);
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	/*
	 * The following command must be performed before the shell is destroyed.
	 * Otherwise, we would be forgetting dangling command dialogs here.
	 */
	forget (my menus);
	if (my shell) {
		#if gtk
			Melder_assert (GTK_IS_WIDGET (my shell));
			gtk_widget_destroy (my shell);
		#else
			XtDestroyWidget (my shell);
		#endif
	}
	if (my destroyCallback) my destroyCallback (me, my destroyClosure);
	forget (my previousData);
	inherited (Editor) destroy (me);
}

static void classEditor_info (I) {
	iam (Editor);
	MelderInfo_writeLine2 (L"Editor type: ", Thing_className (me));
	MelderInfo_writeLine2 (L"Editor name: ", my name ? my name : L"<no name>");
	time_t today = time (NULL);
	MelderInfo_writeLine2 (L"Date: ", Melder_peekUtf8ToWcs (ctime (& today)));   /* Includes a newline. */
	if (my data) {
		MelderInfo_writeLine2 (L"Data type: ", ((Thing) my data) -> methods -> _className);
		MelderInfo_writeLine2 (L"Data name: ", ((Thing) my data) -> name);
	}
}

static void classEditor_nameChanged (I) {
	iam (Editor);
	if (my name)
		GuiWindow_setTitle (my shell, my name);
}

static void classEditor_goAway (Editor me) { forget (me); }

static void classEditor_save (Editor me) {
	if (! my data) return;
	forget (my previousData);
	my previousData = Data_copy (my data);
}

static void classEditor_restore (Editor me) {
	if (my data && my previousData)   /* Swap contents of my data and my previousData. */
		Thing_swap (my data, my previousData);
}

static int menu_cb_close (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	our goAway (me);
	return 1;
}

static int menu_cb_undo (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	our restore (me);
	if (wcsnequ (my undoText, L"Undo", 4)) my undoText [0] = 'R', my undoText [1] = 'e';
	else if (wcsnequ (my undoText, L"Redo", 4)) my undoText [0] = 'U', my undoText [1] = 'n';
	else wcscpy (my undoText, L"Undo?");
	#if gtk
		gtk_label_set_label (GTK_LABEL(gtk_bin_get_child(GTK_BIN(my undoButton))), Melder_peekWcsToUtf8 (my undoText));
	#elif motif
		char *text_utf8 = Melder_peekWcsToUtf8 (my undoText);
		XtVaSetValues (my undoButton, motif_argXmString (XmNlabelString, text_utf8), NULL);
	#endif
	/*
	 * Send a message to myself (e.g., I will redraw myself).
	 */
	our dataChanged (me);
	/*
	 * Send a message to my boss (e.g., she will notify the others that depend on me).
	 */
	Editor_broadcastChange (me);
	return 1;
}

static int menu_cb_searchManual (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	Melder_search ();
	return 1;
}

static int menu_cb_newScript (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	(void) ScriptEditor_createFromText (my parent, me, NULL); cherror
end:
	iferror return 0;
	return 1;
}

static int menu_cb_openScript (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	ScriptEditor scriptEditor = ScriptEditor_createFromText (my parent, me, NULL); cherror
	TextEditor_showOpen (ScriptEditor_as_TextEditor (scriptEditor));
end:
	iferror return 0;
	return 1;
}

static void classEditor_createMenuItems_file (Editor me, EditorMenu menu) {
	(void) me;
	(void) menu;
}

static void classEditor_createMenuItems_edit (Editor me, EditorMenu menu) {
	(void) me;
	if (my data)
		my undoButton = EditorMenu_addCommand (menu, L"Cannot undo", GuiMenu_INSENSITIVE + 'Z', menu_cb_undo);
}

static int menu_cb_settingsReport (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	Thing_info (me);
	return 1;
}

static int menu_cb_info (EDITOR_ARGS) {
	EDITOR_IAM (Editor);
	if (my data) Thing_info (my data);
	return 1;
}

static void classEditor_createMenuItems_query (Editor me, EditorMenu menu) {
	our createMenuItems_query_info (me, menu);
}

static void classEditor_createMenuItems_query_info (Editor me, EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Editor info", 0, menu_cb_settingsReport);
	EditorMenu_addCommand (menu, L"Settings report", Editor_HIDDEN, menu_cb_settingsReport);
	if (my data) {
		static MelderString title = { 0 };
		MelderString_empty (& title);
		MelderString_append2 (& title, Thing_className (my data), L" info");
		EditorMenu_addCommand (menu, title.string, 0, menu_cb_info);
	}
}

static void classEditor_createMenus (Editor me) {
	EditorMenu menu = Editor_addMenu (me, L"File", 0);
	our createMenuItems_file (me, menu);
	if (our editable) {
		menu = Editor_addMenu (me, L"Edit", 0);
		our createMenuItems_edit (me, menu);
	}
	if (our createMenuItems_query) {
		menu = Editor_addMenu (me, L"Query", 0);
		our createMenuItems_query (me, menu);
	}
}

static void classEditor_createHelpMenuItems (Editor me, EditorMenu menu) {
	(void) me;
	(void) menu;
}

static void classEditor_createChildren (Editor me) {
	(void) me;
}

static void classEditor_dataChanged (Editor me) {
	(void) me;
}

static void classEditor_clipboardChanged (Editor me, Any clipboard) {
	(void) me;
	(void) clipboard;
}

static void classEditor_form_pictureWindow (Editor me, EditorCommand cmd) {
	(void) me;
	LABEL (L"", L"Picture window:")
	BOOLEAN (L"Erase first", 1);
}
static void classEditor_ok_pictureWindow (Editor me, EditorCommand cmd) {
	(void) me;
	SET_INTEGER (L"Erase first", preferences.picture.eraseFirst);
}
static void classEditor_do_pictureWindow (Editor me, EditorCommand cmd) {
	(void) me;
	preferences.picture.eraseFirst = GET_INTEGER (L"Erase first");
}

static void classEditor_form_pictureMargins (Editor me, EditorCommand cmd) {
	(void) me;
	Any radio = 0;
	LABEL (L"", L"Margins:")
	OPTIONMENU_ENUM (L"Write name at top", kEditor_writeNameAtTop, DEFAULT);
}
static void classEditor_ok_pictureMargins (Editor me, EditorCommand cmd) {
	(void) me;
	SET_ENUM (L"Write name at top", kEditor_writeNameAtTop, preferences.picture.writeNameAtTop);
}
static void classEditor_do_pictureMargins (Editor me, EditorCommand cmd) {
	(void) me;
	preferences.picture.writeNameAtTop = GET_ENUM (kEditor_writeNameAtTop, L"Write name at top");
}

class_methods (Editor, Thing) {
	class_method_local (Editor, destroy)
	class_method_local (Editor, info)
	class_method_local (Editor, nameChanged)
	class_method_local (Editor, goAway)
	us -> hasMenuBar = true;
	us -> canFullScreen = false;
	us -> editable = true;
	us -> scriptable = true;
	class_method_local (Editor, createMenuItems_file)
	class_method_local (Editor, createMenuItems_edit)
	class_method_local (Editor, createMenuItems_query)
	class_method_local (Editor, createMenuItems_query_info)
	class_method_local (Editor, createMenus)
	class_method_local (Editor, createHelpMenuItems)
	class_method_local (Editor, createChildren)
	class_method_local (Editor, dataChanged)
	class_method_local (Editor, save)
	class_method_local (Editor, restore)
	class_method_local (Editor, clipboardChanged)
	class_method_local (Editor, form_pictureWindow)
	class_method_local (Editor, ok_pictureWindow)
	class_method_local (Editor, do_pictureWindow)
	class_method_local (Editor, form_pictureMargins)
	class_method_local (Editor, ok_pictureMargins)
	class_method_local (Editor, do_pictureMargins)
	class_methods_end
}

static void gui_window_cb_goAway (I) {
	iam (Editor);
	our goAway (me);
}

extern "C" void praat_addCommandsToEditor (Editor me);
int Editor_init (Editor me, GuiObject parent, int x, int y, int width, int height, const wchar_t *title, Any data) {
	#if gtk
		GdkScreen *screen = gdk_screen_get_default ();
		if (parent != NULL) {
			GuiObject parent_win = gtk_widget_get_ancestor (parent, GTK_TYPE_WINDOW);
			if (parent_win != NULL) {
				screen = gtk_window_get_screen (GTK_WINDOW (parent_win));
			}
		}
		int screenWidth = gdk_screen_get_width (screen);
		int screenHeight = gdk_screen_get_height (screen);
	#elif motif
		int screenWidth = WidthOfScreen (DefaultScreenOfDisplay (XtDisplay (parent)));
		int screenHeight = HeightOfScreen (DefaultScreenOfDisplay (XtDisplay (parent)));
	#endif
	int left, right, top, bottom;
	screenHeight -= Machine_getTitleBarHeight ();
	if (width < 0) width += screenWidth;
	if (height < 0) height += screenHeight;
	if (width > screenWidth - 10) width = screenWidth - 10;
	if (height > screenHeight - 10) height = screenHeight - 10;
	if (x > 0) {
		right = (left = x) + width;
	} else if (x < 0) {
		left = (right = screenWidth + x) - width;
	} else { /* Randomize. */
		right = (left = NUMrandomInteger (4, screenWidth - width - 4)) + width;
	}
	if (y > 0) {
		bottom = (top = y) + height;
	} else if (y < 0) {
		top = (bottom = screenHeight + y) - height;
	} else { /* Randomize. */
		bottom = (top = NUMrandomInteger (4, screenHeight - height - 4)) + height;
	}
	#ifndef _WIN32
		top += Machine_getTitleBarHeight ();
		bottom += Machine_getTitleBarHeight ();
	#endif
	my parent = parent;   /* Probably praat.topShell */
	my dialog = GuiWindow_create (parent, left, top, right - left, bottom - top, title, gui_window_cb_goAway, me, our canFullScreen ? GuiWindow_FULLSCREEN : 0);
	if (my dialog == NULL) return 0;
	my shell = GuiObject_parent (my dialog);   /* Note that GuiObject_parent (my shell) will be NULL! */
	//Melder_casual ("my parent %ld my dialog %ld my shell %ld", my parent, my dialog, my shell);
	Thing_setName (me, title);
	my data = data;

	/* Create menus. */

	if (our hasMenuBar) {
		my menus = Ordered_create ();
		my menuBar = Gui_addMenuBar (my dialog);
		our createMenus (me);
		Melder_clearError ();   /* FIXME: to protect against CategoriesEditor */
		EditorMenu helpMenu = Editor_addMenu (me, L"Help", 0);
		our createHelpMenuItems (me, helpMenu);
		EditorMenu_addCommand (helpMenu, L"-- search --", 0, NULL);
		my searchButton = EditorMenu_addCommand (helpMenu, L"Search manual...", 'M', menu_cb_searchManual);
		if (our scriptable) {
			Editor_addCommand (me, L"File", L"New editor script", 0, menu_cb_newScript);
			Editor_addCommand (me, L"File", L"Open editor script...", 0, menu_cb_openScript);
			Editor_addCommand (me, L"File", L"-- after script --", 0, 0);
		}
		/*
		 * Add the scripted commands.
		 */
		praat_addCommandsToEditor (me);
		Editor_addCommand (me, L"File", L"Close", 'W', menu_cb_close);
		GuiObject_show (my menuBar);
	}

	our createChildren (me);
	GuiObject_show (my dialog);
	return 1;
}

void Editor_raise (Editor me) {
	GuiObject_show (my dialog);
}

void Editor_dataChanged (Editor me, Any data) {
	/*if (data) my data = data; BUG */
	(void) data;
	our dataChanged (me);
}

void Editor_clipboardChanged (Editor me, Any data) {
	our clipboardChanged (me, data);
}

void Editor_setDestroyCallback (Editor me, void (*cb) (I, void *closure), void *closure) {
	my destroyCallback = cb;
	my destroyClosure = closure;
}

void Editor_broadcastChange (Editor me) {
	if (my dataChangedCallback)
		my dataChangedCallback (me, my dataChangedClosure, NULL);
}

void Editor_setDataChangedCallback (Editor me, void (*cb) (I, void *closure, Any data), void *closure) {
	my dataChangedCallback = cb;
	my dataChangedClosure = closure;
}

void Editor_setPublishCallback (Editor me, void (*cb) (I, void *closure, Any publish), void *closure) {
	my publishCallback = cb;
	my publishClosure = closure;
}

void Editor_setPublish2Callback (Editor me, void (*cb) (I, void *closure, Any publish1, Any publish2), void *closure) {
	my publish2Callback = cb;
	my publish2Closure = closure;
}

void Editor_save (Editor me, const wchar_t *text) {
	our save (me);
	if (! my undoButton) return;
	GuiObject_setSensitive (my undoButton, True);
	swprintf (my undoText, 100, L"Undo %ls", text);
	#if gtk
		gtk_label_set_label (GTK_LABEL(gtk_bin_get_child(GTK_BIN(my undoButton))), Melder_peekWcsToUtf8 (my undoText));
	#elif motif
		char *text_utf8 = Melder_peekWcsToUtf8 (my undoText);
		XtVaSetValues (my undoButton, motif_argXmString (XmNlabelString, text_utf8), NULL);
	#endif
}

void Editor_openPraatPicture (Editor me) {
	my pictureGraphics = praat_picture_editor_open (preferences.picture.eraseFirst);
}
void Editor_closePraatPicture (Editor me) {
	if (my data != NULL && preferences.picture.writeNameAtTop != kEditor_writeNameAtTop_NO) {
		Graphics_setNumberSignIsBold (my pictureGraphics, false);
		Graphics_setPercentSignIsItalic (my pictureGraphics, false);
		Graphics_setCircumflexIsSuperscript (my pictureGraphics, false);
		Graphics_setUnderscoreIsSubscript (my pictureGraphics, false);
		Graphics_textTop (my pictureGraphics,
			preferences.picture.writeNameAtTop == kEditor_writeNameAtTop_FAR,
			((Data) my data) -> name);
		Graphics_setNumberSignIsBold (my pictureGraphics, true);
		Graphics_setPercentSignIsItalic (my pictureGraphics, true);
		Graphics_setCircumflexIsSuperscript (my pictureGraphics, true);
		Graphics_setUnderscoreIsSubscript (my pictureGraphics, true);
	}
	praat_picture_editor_close ();
}

/* End of file Editor.cpp */
