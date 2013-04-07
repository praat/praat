/* praatP.h
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma
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

#include "praat.h"

void praat_addActionScript (const wchar_t *className1, int n1, const wchar_t *className2, int n2, const wchar_t *className3, int n3,
	const wchar_t *title, const wchar_t *after, int depth, const wchar_t *script);
/* No strings may be NULL; unspecify them by giving an empty string. 'title' and 'script' are deep-copied. */
void praat_removeAction_classNames (const wchar_t *className1, const wchar_t *className2,
	const wchar_t *className3, const wchar_t *title);
/* No arguments may be NULL; unspecify them by giving an empty string. */
/* 'title' is deep-copied. */
void praat_hideAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const wchar_t *title);
void praat_hideAction_classNames (const wchar_t *className1, const wchar_t *className2,
	const wchar_t *className3, const wchar_t *title);
void praat_showAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const wchar_t *title);
void praat_showAction_classNames (const wchar_t *className1, const wchar_t *className2,
	const wchar_t *className3, const wchar_t *title);
void praat_sortActions ();

void praat_addMenuCommandScript (const wchar_t *window, const wchar_t *menu, const wchar_t *title,
	const wchar_t *after, int depth, const wchar_t *script);
/* All strings are deep-copied and may not be NULL; unspecify them by giving an empty string. */
/*
	For the Praat objects window:
	'window' is "Objects", 'menu' is "Praat", "New", "Open", "Help", "Preferences", "Goodies", or "Technical".
	For the Praat picture window:
	'window' is "Picture", 'menu' is "File", "Edit", "Margins", "World", "Select", "Pen", "Font", or "Help".
*/
void praat_hideMenuCommand (const wchar_t *window, const wchar_t *menu, const wchar_t *title);
void praat_showMenuCommand (const wchar_t *window, const wchar_t *menu, const wchar_t *title);
void praat_saveMenuCommands (MelderString *buffer);
void praat_addFixedButtonCommand (GuiForm parent, const wchar_t *title, void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *), int x, int y);
void praat_sensitivizeFixedButtonCommand (const wchar_t *title, int sensitive);
void praat_sortMenuCommands ();

#define praat_MAXNUM_MENUS 20   /* Maximum number of added New, Open, Save, or Help menus. */
#define praat_MAXNUM_FILE_TYPE_RECOGNIZERS 50   /* File types recognizable by 'Read from file...'. */

typedef struct structPraat_Command {
	ClassInfo class1, class2, class3, class4;   // selected classes
	short n1, n2, n3, n4;   // number of selected objects of each class; 0 means "any number"
	const wchar_t *title;   // button text = command text
	void (*callback) (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *closure);   // multi-purpose
		/* If both sendingForm and sendingString are NULL, this routine is an activate callback;
			you should directly execute the command, or call UiForm_do(dialog) if you need arguments;
			UiForm_do will call this routine again with sendingForm = dialog. */
		/* If sendingForm exists, this routine is an ok callback,
			and you should execute the command. */
		/* If sendingString exists (apparently from a command file),
			UiForm_parseString should be called, which will call this routine again with sendingForm. */
		/* All of these things are normally taken care of by the macros defined in praat.h. */
	signed char
		visible,   /* Selected classes match class1, class2, and class3? */
		executable,   /* Command actually executable? Button not grey? */
		depth,   /* 1 = subcommand */
		hidden,
		toggled,
		phase,
		unhidable,
		attractive;
	GuiThing button;
	const wchar_t *window, *menu;
	const wchar_t *script;   // if 'callback' equals DO_RunTheScriptFromAnyAddedMenuCommand
	const wchar_t *after;   // title of previous command, often NULL
	long uniqueID;   // for sorting the added commands
	long sortingTail;
} *praat_Command;

#define praat_STARTING_UP  1
#define praat_READING_BUTTONS  2
#define praat_HANDLING_EVENTS  3

long praat_getIdOfSelected (ClassInfo klas, int inplace);
wchar_t * praat_getNameOfSelected (ClassInfo klas, int inplace);

int praat_selection (ClassInfo klas);
	/* How many objects of this class (excluding subclasses) are currently selected? */

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
GuiMenu praat_picture_resolveMenu (const wchar_t *menu);
void praat_picture_background ();
void praat_picture_foreground ();


/* The following routines are a bit private (used by praat_script.c). */
/* If you must call them, follow them by praat_show (). */
void praat_deselect (int i);
void praat_deselectAll ();
void praat_select (int i);
void praat_selectAll ();

void praat_list_background ();
void praat_list_foreground ();   // updates the list of objects after backgrounding
void praat_background ();
void praat_foreground ();
Editor praat_findEditorFromString (const wchar_t *string);

void praat_showLogo (int autoPopDown);

/* Communication with praat_menuCommands.cpp: */
void praat_menuCommands_init ();
void praat_menuCommands_exit ();
int praat_doMenuCommand (const wchar_t *command, const wchar_t *arguments, Interpreter interpreter);   // 0 = not found
int praat_doMenuCommand (const wchar_t *command, int narg, Stackel args, Interpreter interpreter);   // 0 = not found
long praat_getNumberOfMenuCommands ();
praat_Command praat_getMenuCommand (long i);

/* Communication with praat_actions.cpp: */
void praat_actions_show ();
void praat_actions_createWriteMenu (GuiWindow window);
void praat_actions_init ();   // creates space for action commands
void praat_actions_createDynamicMenu (GuiWindow window);
void praat_saveAddedActions (MelderString *buffer);
int praat_doAction (const wchar_t *command, const wchar_t *arguments, Interpreter interpreter);   // 0 = not found
int praat_doAction (const wchar_t *command, int narg, Stackel args, Interpreter interpreter);   // 0 = not found
long praat_getNumberOfActions ();   // for ButtonEditor
praat_Command praat_getAction (long i);   // for ButtonEditor

/* Communication with praat_statistics.cpp: */
void praat_statistics_prefs ();   // at init time
void praat_statistics_prefsChanged ();   // after reading prefs file
void praat_statistics_exit ();   // at exit time
void praat_reportMemoryUse ();
void praat_reportGraphicalProperties ();
void praat_reportIntegerProperties ();
void praat_reportTextProperties ();

/* Communication with praat_objectMenus.cpp: */
GuiMenu praat_objects_resolveMenu (const wchar_t *menu);
void praat_addFixedButtons (GuiWindow window);
void praat_addMenus (GuiWindow window);
void praat_addMenus2 ();

void praat_cleanUpName (wchar_t *name);
void praat_list_renameAndSelect (int position, const wchar_t *name);

extern struct PraatP {
	int dontUsePictureWindow;   // see praat_dontUsePictureWindow ()
	char *title;
	GuiWindow menuBar;
	int phase;
	Editor editor;   // scripting environment
} praatP;

struct autoPraatBackground {
	autoPraatBackground () { praat_background (); }
	~autoPraatBackground () { try { praat_foreground (); } catch (...) { Melder_clearError (); } }
};

/* End of file praatP.h */
