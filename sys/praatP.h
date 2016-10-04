/* praatP.h
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016 Paul Boersma
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

#include "praat.h"

void praat_addActionScript (const char32 *className1, int n1, const char32 *className2, int n2, const char32 *className3, int n3,
	const char32 *title, const char32 *after, int depth, const char32 *script);
/* No strings may be null; unspecify them by giving an empty string. 'title' and 'script' are deep-copied. */
void praat_removeAction_classNames (const char32 *className1, const char32 *className2,
	const char32 *className3, const char32 *title);
/* No arguments may be null; unspecify them by giving an empty string. */
/* 'title' is deep-copied. */
void praat_hideAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const char32 *title);
void praat_hideAction_classNames (const char32 *className1, const char32 *className2,
	const char32 *className3, const char32 *title);
void praat_showAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const char32 *title);
void praat_showAction_classNames (const char32 *className1, const char32 *className2,
	const char32 *className3, const char32 *title);
void praat_sortActions ();

void praat_addMenuCommandScript (const char32 *window, const char32 *menu, const char32 *title,
	const char32 *after, int depth, const char32 *script);
/* All strings are deep-copied and may not be null; unspecify them by giving an empty string. */
/*
	For the Praat objects window:
	'window' is "Objects", 'menu' is "Praat", "New", "Open", "Help", "Preferences", "Goodies", or "Technical".
	For the Praat picture window:
	'window' is "Picture", 'menu' is "File", "Edit", "Margins", "World", "Select", "Pen", "Font", or "Help".
*/
void praat_hideMenuCommand (const char32 *window, const char32 *menu, const char32 *title);
void praat_showMenuCommand (const char32 *window, const char32 *menu, const char32 *title);
void praat_saveMenuCommands (MelderString *buffer);
#define praat_addFixedButtonCommand(p,t,c,x,y)  praat_addFixedButtonCommand_ (p, t, c, U"" #c, x, y)
void praat_addFixedButtonCommand_ (GuiForm parent, const char32 *title, UiCallback callback, const char32 *nameOfCallback, int x, int y);
void praat_sensitivizeFixedButtonCommand (const char32 *title, int sensitive);
void praat_sortMenuCommands ();

#define praat_MAXNUM_MENUS 20   /* Maximum number of added New, Open, Save, or Help menus. */
#define praat_MAXNUM_FILE_TYPE_RECOGNIZERS 50   /* File types recognizable by 'Read from file...'. */

Thing_define (Praat_Command, Thing) {
	ClassInfo class1, class2, class3, class4;   // selected classes
	int32 n1, n2, n3, n4;   // number of selected objects of each class; 0 means "any number"
	const char32 *title;   // button text = command text
	UiCallback callback;   // multi-purpose
		/* If both UiCallback::sendingForm and sendingString are null, this routine is an activate callback;
			you should directly execute the command, or call UiForm_do(dialog) if you need arguments;
			UiForm_do will call this routine again with sendingForm = dialog. */
		/* If sendingForm exists, this routine is an ok callback,
			and you should execute the command. */
		/* If sendingString exists (apparently from a command file),
			UiForm_parseString should be called, which will call this routine again with sendingForm. */
		/* All of these things are normally taken care of by the macros defined in praat.h. */
	const char32 *nameOfCallback;
	signed char
		visible,   // do the selected classes match class1, class2, class3 and class4?
		executable,   // is the command actually executable? I.e. isn't the button greyed out?
		depth,   // 0 = command in main menu, 1 = command in submenu, 2 = command in submenu of submenu
		hidden,   // this can change when Praat is running, even from the start-up file
		toggled,   // did the hiddenness change when Praat was running? The factory value for "hidden" is (hidden != toggled)
		phase,
		unhidable,
		attractive,
		noApi,   // do not include in a library API ("View & Edit", help commands...)
		forceApi;   // include in a library API even if this button is hidden by default ("Record Sound (fixed time)...")
	int32 deprecationYear;
	GuiThing button;
	const char32 *window, *menu;
	const char32 *script;   // if 'callback' equals DO_RunTheScriptFromAnyAddedMenuCommand
	const char32 *after;   // title of previous command, often null; if starting with an asterisk (deprecation), then a reference to the replacement
	int32 uniqueID;   // for sorting the added commands
	int32 sortingTail;
};

#define praat_STARTING_UP  1
#define praat_READING_BUTTONS  2
#define praat_HANDLING_EVENTS  3

int praat_numberOfSelected (ClassInfo klas);
long praat_idOfSelected (ClassInfo klas, int inplace);
char32 * praat_nameOfSelected (ClassInfo klas, int inplace);

/* Used by praat.cpp; defined in praat_picture.cpp.
*/
void praat_picture_init ();
void praat_picture_exit ();

void praat_picture_prefs ();
void praat_picture_prefsChanged ();
/* Praat shell tells Picture window that preferences have been changed
   (after reading the prefs file).
   Picture window will update the font menu.
*/
GuiMenu praat_picture_resolveMenu (const char32 *menu);
void praat_picture_background ();
void praat_picture_foreground ();


/* The following routines are a bit private (used by praat_script.cpp). */
/* If you must call them, follow them by praat_show (). */
void praat_deselect (int i);
void praat_deselectAll ();
void praat_select (int i);
void praat_selectAll ();

void praat_list_background ();
void praat_list_foreground ();   // updates the list of objects after backgrounding
void praat_background ();
void praat_foreground ();
Editor praat_findEditorFromString (const char32 *string);
Editor praat_findEditorById (long id);

void praat_showLogo (bool autoPopDown);

/* Communication with praat_menuCommands.cpp: */
void praat_menuCommands_init ();
void praat_menuCommands_exit ();
int praat_doMenuCommand (const char32 *command, const char32 *arguments, Interpreter interpreter);   // 0 = not found
int praat_doMenuCommand (const char32 *command, int narg, Stackel args, Interpreter interpreter);   // 0 = not found
long praat_getNumberOfMenuCommands ();
Praat_Command praat_getMenuCommand (long i);

/* Communication with praat_actions.cpp: */
void praat_actions_show ();
void praat_actions_createWriteMenu (GuiWindow window);
void praat_actions_init ();   // creates space for action commands
void praat_actions_createDynamicMenu (GuiWindow window);
void praat_saveAddedActions (MelderString *buffer);
int praat_doAction (const char32 *command, const char32 *arguments, Interpreter interpreter);   // 0 = not found
int praat_doAction (const char32 *command, int narg, Stackel args, Interpreter interpreter);   // 0 = not found
long praat_getNumberOfActions ();   // for ButtonEditor
Praat_Command praat_getAction (long i);   // for ButtonEditor

/* Communication with praat_statistics.cpp: */
void praat_statistics_prefs ();   // at init time
void praat_statistics_prefsChanged ();   // after reading prefs file
void praat_statistics_exit ();   // at exit time
void praat_reportMemoryUse ();
void praat_reportSystemProperties ();
void praat_reportGraphicalProperties ();
void praat_reportIntegerProperties ();
void praat_reportTextProperties ();
void praat_listMenuCommands ();
void praat_listActions ();
void praat_listApiCommands ();

/* Communication with praat_objectMenus.cpp: */
GuiMenu praat_objects_resolveMenu (const char32 *menu);
void praat_addFixedButtons (GuiWindow window);
void praat_addMenus (GuiWindow window);
void praat_addMenus2 ();

void praat_cleanUpName (char32 *name);
void praat_list_renameAndSelect (int position, const char32 *name);

extern struct PraatP {
	int argc;
	char **argv;
	int argumentNumber;
	bool userWantsToOpen;
	bool dontUsePictureWindow;   // see praat_dontUsePictureWindow ()
	bool ignorePreferenceFiles, ignorePlugins;
	bool hasCommandLineInput;
	char32 *title;
	GuiWindow menuBar;
	int phase;
	Editor editor;   // scripting environment
} praatP;

struct autoPraatBackground {
	autoPraatBackground () { praat_background (); }
	~autoPraatBackground () { try { praat_foreground (); } catch (...) { Melder_clearError (); } }
};

/* End of file praatP.h */
