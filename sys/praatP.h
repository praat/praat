/* praatP.h
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2003/10/03 praat_executeFromFile(AndArguments)
 * pb 2004/11/09 topBar
 * pb 2004/11/16 shorter praat_Command (with signed chars)
 * pb 2005/05/07 inManual
 * pb 2005/08/22
 */

#include "praat.h"

int praat_addActionScript (const char *className1, int n1, const char *className2, int n2, const char *className3, int n3,
	const char *title, const char *after, int depth, const char *script);
/* No strings may be NULL; unspecify them by giving an empty string. 'title' and 'script' are deep-copied. */
int praat_removeAction_classNames (const char *className1, const char *className2,
	const char *className3, const char *title);
/* No arguments may be NULL; unspecify them by giving an empty string. */
/* 'title' is deep-copied. */
int praat_hideAction (void *class1, void *class2, void *class3, const char *title);
int praat_hideAction_classNames (const char *className1, const char *className2,
	const char *className3, const char *title);
int praat_showAction (void *class1, void *class2, void *class3, const char *title);
int praat_showAction_classNames (const char *className1, const char *className2,
	const char *className3, const char *title);
void praat_sortActions (void);

int praat_addMenuCommandScript (const char *window, const char *menu, const char *title,
	const char *after, int depth, const char *script);
/* All strings are deep-copied and may not be NULL; unspecify them by giving an empty string. */
/*
	For the Praat objects window:
	'window' is "Objects", 'menu' is "Praat", "New", "Read", "Help", "Preferences", or "Goodies".
	For the Praat picture window:
	'window' is "Picture", 'menu' is "File", "Edit", "Margins", "World", "Select", "Pen", "Font", or "Help".
*/
int praat_hideMenuCommand (const char *window, const char *menu, const char *title);
int praat_showMenuCommand (const char *window, const char *menu, const char *title);
void praat_saveMenuCommands (FILE *f);
void praat_addFixedButtonCommand (Widget parent, const char *title, int (*callback) (Any, void *),
	int fromLeft, int fromBottom);
void praat_sensitivizeFixedButtonCommand (const char *title, int sensitive);
void praat_sortMenuCommands (void);

#define praat_MAXNUM_MENUS 20   /* Maximum number of added Read, Write, New, or Help menus. */
#define praat_MAXNUM_FILE_TYPE_RECOGNIZERS 50   /* File types recognizable by 'Read from file...'. */

typedef struct structPraat_Command {
	void *class1, *class2, *class3, *class4;   /* Selected classes. */
	short n1, n2, n3, n4;   /* Number of selected objects of each class; 0 means "any number" */
	const char *title;   /* Button text = command text. */
	int (*callback) (Any sender, void *closure);   /* Multi-purpose. */
		/* If sender is NULL, this routine is an activate callback;
			you should directly execute the command, or call UiForm_do(dialog) if you need arguments;
			UiForm_do will call this routine again with sender = dialog. */
		/* If sender is the UiForm of UiFile dialog, this routine is an ok callback,
			and you should execute the command. */
		/* If sender is a string (apparently from a command file),
			UiForm_parseString should be called, which will call this routine again with sender = dialog. */
		/* All of these things are normally taken care of by the macros defined in praat.h. */
	signed char
		visible,   /* Selected classes match class1, class2, and class3? */
		executable,   /* Command actually executable? Button not grey? */
		depth,   /* 1 = subcommand */
		hidden,
		toggled,
		phase,
		unhidable;
	Widget button;
	const char *window, *menu;
	const char *script;   /* If 'callback' equals DO_RunTheScriptFromAnyAddedMenuCommand. */
	const char *after;   /* Title of previous command, often NULL. */
	long uniqueID;   /* For sorting the added commands. */
	long sortingTail;
} *praat_Command;

#define praat_STARTING_UP  1
#define praat_READING_BUTTONS  2
#define praat_HANDLING_EVENTS  3

long praat_getIdOfSelected (void *voidklas, int inplace);
char * praat_getNameOfSelected (void *voidklas, int inplace);

int praat_selection (void *klas);
	/* How many objects of this class (excluding subclasses) are currently selected? */
int praat_selectionGeneric (void *klas);
	/* How many objects of this class (including subclasses) are currently selected? */

/* Used by praat.c; defined in praat_picture.c.
*/
void praat_picture_init (void);   /* Sets praat.graphics. */
void praat_picture_exit (void);

void praat_picture_prefs (void);
void praat_picture_prefsChanged (void);
/* Praat shell tells Picture window that preferences have been changed
   (after reading the prefs file).
   Picture window will update the font menu.
*/
Widget praat_picture_resolveMenu (const char *menu);
void praat_picture_background (void);
void praat_picture_foreground (void);


/* The following routines are a bit private (used by praat_script.c). */
/* If you must call them, follow them by praat_show (). */
void praat_deselect (int i);
void praat_deselectAll (void);
void praat_select (int i);
void praat_selectAll (void);

void praat_list_background (void);
void praat_list_foreground (void);   /* Updates the list of objects after backgrounding. */
void praat_background (void);
void praat_foreground (void);
Editor praat_findEditorFromString (const char *string);

void praat_showLogo (int autoPopDown);

/* Communication with praat_menuCommands.c: */
void praat_menuCommands_init (void);
void praat_menuCommands_exit (void);
int praat_doMenuCommand (const char *command, const char *arguments);   /* 0 = not found or error */
long praat_getNumberOfMenuCommands (void);
praat_Command praat_getMenuCommand (long i);

/* Communication with praat_actions.c: */
void praat_actions_show (void);
void praat_actions_createWriteMenu (Widget bar);
void praat_actions_init (void);   /* Creates space for action commands. */
void praat_actions_createDynamicMenu (Widget form, int leftOffset);
void praat_saveAddedActions (FILE *f);
int praat_doAction (const char *command, const char *arguments);   /* 0 = not found or error */
long praat_getNumberOfActions (void);   /* For ButtonEditor. */
praat_Command praat_getAction (long i);   /* For ButtonEditor. */

/* Communication with praat_statistics.c: */
void praat_statistics_prefs (void);   /* At init time. */
void praat_statistics_prefsChanged (void);   /* After reading prefs file. */
void praat_statistics_exit (void);   /* At exit time. */
void praat_memoryInfo (void);

/* Communication with praat_objectMenus.c: */
Widget praat_objects_resolveMenu (const char *menu);
void praat_addFixedButtons (Widget form);
void praat_addMenus (Widget bar);
void praat_addMenus2 (void);

void praat_list_renameAndSelect (int position, const char *name);

extern struct PraatP {
	int dontUsePictureWindow;   /* See praat_dontUsePictureWindow (). */
	char *title;
	Widget topBar, menuBar;
	int phase, inManual;
	Any editor;   /* Scripting environment. */
} praatP;

/* End of file praatP.h */
