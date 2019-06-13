/* praatP.h
 *
 * Copyright (C) 1992-2007,2009-2018 Paul Boersma
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

void praat_addActionScript (conststring32 className1, integer n1, conststring32 className2, integer n2, conststring32 className3, integer n3,
	conststring32 title, conststring32 after, integer depth, conststring32 script);
/* No strings may be null; unspecify them by giving an empty string. 'title' and 'script' are deep-copied. */

void praat_removeAction_classNames (conststring32 className1, conststring32 className2,
	conststring32 className3, conststring32 title);
/* No arguments may be null; unspecify them by giving an empty string. */

void praat_hideAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, conststring32 title);
void praat_hideAction_classNames (conststring32 className1, conststring32 className2,
	conststring32 className3, conststring32 title);
void praat_showAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, conststring32 title);
void praat_showAction_classNames (conststring32 className1, conststring32 className2,
	conststring32 className3, conststring32 title);
void praat_sortActions ();

void praat_addMenuCommandScript (conststring32 window, conststring32 menu, conststring32 title,
	conststring32 after, integer depth, conststring32 script);
/* All strings are deep-copied and may not be null; unspecify them by giving an empty string. */

/*
	For the Praat objects window:
	'window' is "Objects", 'menu' is "Praat", "New", "Open", "Help", "Preferences", "Goodies", or "Technical".
	For the Praat picture window:
	'window' is "Picture", 'menu' is "File", "Edit", "Margins", "World", "Select", "Pen", "Font", or "Help".
*/
void praat_hideMenuCommand (conststring32 window, conststring32 menu, conststring32 title);
void praat_showMenuCommand (conststring32 window, conststring32 menu, conststring32 title);
void praat_saveAddedMenuCommands (MelderString *buffer);
void praat_saveToggledMenuCommands (MelderString *buffer);
#define praat_addFixedButtonCommand(p,t,c,x,y)  praat_addFixedButtonCommand_ (p, t, c, U"" #c, x, y)
void praat_addFixedButtonCommand_ (GuiForm parent, conststring32 title, UiCallback callback, conststring32 nameOfCallback, int x, int y);
void praat_sensitivizeFixedButtonCommand (conststring32 title, bool sensitive);
void praat_sortMenuCommands ();

#define praat_MAXNUM_MENUS 20   /* Maximum number of added New, Open, Save, or Help menus. */
#define praat_MAXNUM_FILE_TYPE_RECOGNIZERS 50   /* File types recognizable by 'Read from file...'. */

Thing_define (Praat_Command, Thing) {
	ClassInfo class1, class2, class3, class4;   // selected classes
	integer n1, n2, n3, n4;   // number of selected objects of each class; 0 means "any number"
	autostring32 title;   // button text = command text
	UiCallback callback;   // multi-purpose
		/* If both UiCallback::sendingForm and sendingString are null, this routine is an activate callback;
			you should directly execute the command, or call UiForm_do(dialog) if you need arguments;
			UiForm_do will call this routine again with sendingForm = dialog. */
		/* If sendingForm exists, this routine is an ok callback,
			and you should execute the command. */
		/* If sendingString exists (apparently from a command file),
			UiForm_parseString should be called, which will call this routine again with sendingForm. */
		/* All of these things are normally taken care of by the macros defined in praat.h. */
	conststring32 nameOfCallback;
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
	integer deprecationYear;
	GuiThing button;
	autostring32 window, menu;
	autostring32 script;   // if 'callback' equals DO_RunTheScriptFromAnyAddedMenuCommand
	autostring32 after;   // title of previous command, often null; if starting with an asterisk (deprecation), then a reference to the replacement
	integer uniqueID;   // for sorting the added commands
	integer sortingTail;
};

#define praat_STARTING_UP  1
#define praat_READING_BUTTONS  2
#define praat_HANDLING_EVENTS  3

integer praat_numberOfSelected (ClassInfo klas);
integer praat_idOfSelected (ClassInfo klas, integer inplace);
autoVEC praat_idsOfAllSelected (ClassInfo klas);
char32 * praat_nameOfSelected (ClassInfo klas, integer inplace);

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
GuiMenu praat_picture_resolveMenu (conststring32 menu);
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
Editor praat_findEditorFromString (conststring32 string);
Editor praat_findEditorById (integer id);

void praat_showLogo (bool autoPopDown);

/* Communication with praat_menuCommands.cpp: */
void praat_menuCommands_init ();
void praat_menuCommands_exit_optimizeByLeaking ();
int praat_doMenuCommand (conststring32 command, conststring32 arguments, Interpreter interpreter);   // 0 = not found
int praat_doMenuCommand (conststring32 command, integer narg, Stackel args, Interpreter interpreter);   // 0 = not found
integer praat_getNumberOfMenuCommands ();
Praat_Command praat_getMenuCommand (integer i);

/* Communication with praat_actions.cpp: */
void praat_actions_show ();
void praat_actions_createWriteMenu (GuiWindow window);
void praat_actions_init ();   // creates space for action commands
void praat_actions_exit_optimizeByLeaking ();
void praat_actions_createDynamicMenu (GuiWindow window);
void praat_saveAddedActions (MelderString *buffer);
void praat_saveToggledActions (MelderString *buffer);
int praat_doAction (conststring32 command, conststring32 arguments, Interpreter interpreter);   // 0 = not found
int praat_doAction (conststring32 command, integer narg, Stackel args, Interpreter interpreter);   // 0 = not found
integer praat_getNumberOfActions ();   // for ButtonEditor
Praat_Command praat_getAction (integer i);   // for ButtonEditor

/* Communication with praat_statistics.cpp: */
void praat_statistics_prefs ();   // at init time
void praat_statistics_prefsChanged ();   // after reading prefs file
void praat_statistics_exit ();   // at exit time
void praat_reportMemoryUse ();
void praat_reportSystemProperties ();
void praat_reportGraphicalProperties ();
void praat_reportIntegerProperties ();
void praat_reportTextProperties ();

/* Communication with praat_objectMenus.cpp: */
GuiMenu praat_objects_resolveMenu (conststring32 menu);
void praat_addFixedButtons (GuiWindow window);
void praat_addMenus (GuiWindow window);
void praat_addMenus2 ();

/* API creation: */
void praat_library_createC (bool isInHeaderFile, bool includeCreateAPI, bool includeReadAPI, bool includeSaveAPI,
	bool includeQueryAPI, bool includeModifyAPI, bool includeToAPI,
	bool includeRecordAPI, bool includePlayAPI,
	bool includeDrawAPI, bool includeHelpAPI, bool includeWindowAPI,
	bool includeDemoAPI);
void praat_menuCommands_writeC (bool isInHeaderFile, bool includeCreateAPI, bool includeReadAPI,
	bool includeRecordAPI, bool includePlayAPI, bool includeDrawAPI, bool includeHelpAPI, bool includeWindowAPI);
void praat_actions_writeC (bool isInHeaderFile, bool includeSaveAPI,
	bool includeQueryAPI, bool includeModifyAPI, bool includeToAPI,
	bool includePlayAPI, bool includeDrawAPI, bool includeHelpAPI, bool includeWindowAPI);

void praat_cleanUpName (char32 *name);
void praat_list_renameAndSelect (int position, conststring32 name);

extern struct PraatP {
	int argc;
	char **argv;
	int argumentNumber;
	bool userWantsToOpen;
	bool dontUsePictureWindow;   // see praat_dontUsePictureWindow ()
	bool ignorePreferenceFiles, ignorePlugins;
	bool hasCommandLineInput;
	autostring32 title;
	GuiWindow menuBar;
	int phase;
	Editor editor;   // scripting environment
} praatP;

struct autoPraatBackground {
	autoPraatBackground () { praat_background (); }
	~autoPraatBackground () { try { praat_foreground (); } catch (...) { Melder_clearError (); } }
};

/* End of file praatP.h */
