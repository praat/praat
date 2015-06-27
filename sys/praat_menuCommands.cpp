/* praat_menuCommands.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015 Paul Boersma
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

#include "praatP.h"
#include "praat_script.h"
#include "GuiP.h"

#define praat_MAXNUM_FIXED_COMMANDS  1000
static long theNumberOfCommands = 0;
static struct structPraat_Command *theCommands;

void praat_menuCommands_init (void) {
	theCommands = Melder_calloc_f (struct structPraat_Command, praat_MAXNUM_FIXED_COMMANDS + 1);
}

static int compareMenuCommands (const void *void_me, const void *void_thee) {
	praat_Command me = (praat_Command) void_me, thee = (praat_Command) void_thee;
	int compare;
	if (my window) {
		if (! thy window) return 1;
		compare = str32cmp (my window, thy window);
		if (compare) return compare;
	} else if (thy window) return -1;
	if (my menu) {
		if (! thy menu) return 1;
		compare = str32cmp (my menu, thy menu);
		if (compare) return compare;
	} else if (thy menu) return -1;
	if (my sortingTail < thy sortingTail) return -1;
	return 1;
}

void praat_sortMenuCommands (void) {
	for (long i = 1; i <= theNumberOfCommands; i ++)
		theCommands [i]. sortingTail = i;
	qsort (& theCommands [1], theNumberOfCommands, sizeof (struct structPraat_Command), compareMenuCommands);
}

static long lookUpMatchingMenuCommand (const char32 *window, const char32 *menu, const char32 *title) {
/*
 * A menu command is fully specified by its environment (window + menu) and its title.
 */
	for (long i = 1; i <= theNumberOfCommands; i ++) {
		const char32 *tryWindow = theCommands [i]. window;
		const char32 *tryMenu = theCommands [i]. menu;
		const char32 *tryTitle = theCommands [i]. title;
		if ((window == tryWindow || (window && tryWindow && str32equ (window, tryWindow))) &&
		    (menu == tryMenu || (menu && tryMenu && str32equ (menu, tryMenu))) &&
		    (title == tryTitle || (title && tryTitle && str32equ (title, tryTitle)))) return i;
	}
	return 0;   // not found
}

static void do_menu (I, unsigned long modified) {
	void (*callback) (UiForm, int, Stackel, const char32 *, Interpreter, const char32 *, bool, void *) = (void (*) (UiForm, int, Stackel, const char32 *, Interpreter, const char32 *, bool, void *)) void_me;
	Melder_assert (callback != NULL);
	for (long i = 1; i <= theNumberOfCommands; i ++) {
		praat_Command me = & theCommands [i];
		if (my callback == callback) {
			if (my title != NULL && ! str32str (my title, U"...")) {
				UiHistory_write (U"\n");
				UiHistory_write_expandQuotes (my title);
			}
			try {
				callback (NULL, 0, NULL, NULL, NULL, my title, modified, NULL);
			} catch (MelderError) {
				Melder_flushError (U"Command \"", my title, U"\" not executed.");
			}
			praat_updateSelection (); return;
		}
		if (my callback == DO_RunTheScriptFromAnyAddedMenuCommand && my script == (void *) void_me) {
			if (my title != NULL && ! str32str (my title, U"...")) {
				UiHistory_write (U"\nexecute ");
				UiHistory_write (my script);
			} else {
				UiHistory_write (U"\nexecute \"");
				UiHistory_write (my script);
				UiHistory_write (U"\"");
			}
			try {
				DO_RunTheScriptFromAnyAddedMenuCommand (NULL, 0, NULL, my script, NULL, NULL, false, NULL);
			} catch (MelderError) {
				Melder_flushError (U"Command \"", my title, U"\" not executed.");
			}
			praat_updateSelection (); return;
		}
	}
}

static void gui_button_cb_menu (I, GuiButtonEvent event) {
	do_menu (void_me, event -> shiftKeyPressed | event -> commandKeyPressed | event -> optionKeyPressed | event -> extraControlKeyPressed);
}

static void gui_cb_menu (GUI_ARGS) {
	bool modified = event -> shiftKeyPressed || event -> commandKeyPressed || event -> optionKeyPressed || event -> extraControlKeyPressed;
	do_menu (void_me, modified);
}

static GuiMenu windowMenuToWidget (const char32 *window, const char32 *menu) {
	return
		str32equ (window, U"Picture") ? praat_picture_resolveMenu (menu) :
		str32equ (window, U"Objects") ? praat_objects_resolveMenu (menu) : NULL;
}

GuiMenuItem praat_addMenuCommand (const char32 *window, const char32 *menu, const char32 *title /* cattable */,
	const char32 *after, unsigned long flags, void (*callback) (UiForm, int, Stackel, const char32 *, Interpreter, const char32 *, bool, void *))
{
	long position;
	int depth = flags, unhidable = FALSE, hidden = FALSE, key = 0;
	unsigned long guiFlags = 0;
	if (flags > 7) {
		depth = ((flags & praat_DEPTH_7) >> 16);
		unhidable = (flags & praat_UNHIDABLE) != 0;
		hidden = (flags & praat_HIDDEN) != 0 && ! unhidable;
		key = flags & 0x000000FF;
		guiFlags = key ? flags & (0x006000FF | GuiMenu_BUTTON_STATE_MASK) : flags & GuiMenu_BUTTON_STATE_MASK;
	}
	if (callback && ! title) {
		Melder_flushError (U"praat_addMenuCommand: command with callback has no title. Window \"", window, U"\", menu \"", menu, U"\".");
		return NULL;
	}

	/* Determine the position of the new command.
	 */
	if (after) {   /* Search for existing command with same selection. */
		long found = lookUpMatchingMenuCommand (window, menu, after);
		if (found) {
			position = found + 1;   // after 'after'
		} else {
			Melder_flushError (U"praat_addMenuCommand: the command \"", title, U"\" cannot be put after \"", after, U"\",\n"
				U"in the menu \"", menu, U"\" in the window \"", window, U"\"\n"
				U"because the latter command does not exist.");
			return NULL;
		}
	} else {
		position = theNumberOfCommands + 1;   // at end
	}

	/* Increment the command area.
	 */
	if (theNumberOfCommands >= praat_MAXNUM_FIXED_COMMANDS) {
		Melder_flushError (U"praat_addMenuCommand: too many menu commands.");
		return NULL;
	}
	theNumberOfCommands += 1;

	/* Make room for insertion.
	 */
	for (long i = theNumberOfCommands; i > position; i --) theCommands [i] = theCommands [i - 1];
	memset (& theCommands [position], 0, sizeof (struct structPraat_Command));

	theCommands [position]. window = Melder_dup_f (window);
	theCommands [position]. menu = Melder_dup_f (menu);
	theCommands [position]. title = Melder_dup_f (title);
	trace (U"insert new command \"", title, U"\"");
	theCommands [position]. depth = depth;
	theCommands [position]. callback = callback;   // NULL for a separator or cascade button
	theCommands [position]. executable = callback != NULL;
	theCommands [position]. script = NULL;
	theCommands [position]. hidden = hidden;
	theCommands [position]. unhidable = unhidable;

	if (! theCurrentPraatApplication -> batch) {
		GuiMenu parentMenu = NULL;

		/* WHERE TO PUT IT?
		 * Determine parent menu widget.
		 * This is not going to fail:
		 * if 'depth' is inappropriate, the alleged subitem will be put in the top menu.
		 */
		if (depth == 0) {
			/*
			 * Put the new command in the window's top menu.
			 */
			parentMenu = windowMenuToWidget (window, menu);
		} else {
			/*
			 * Put the new command in a submenu.
			 * The supermenu to put the new command in is the first menu that we find when going up.
			 */
			for (long parentPosition = position - 1; parentPosition > 0; parentPosition --) {
				if (theCommands [parentPosition]. depth == depth - 1) {
					/*
					 * We found the supermenu.
					 */
					if (theCommands [parentPosition]. callback == NULL && theCommands [parentPosition]. title != NULL && theCommands [parentPosition]. title [0] != '-') {
						if (theCommands [parentPosition]. button == NULL)
							Melder_fatal (U"No button for ", window, U"/", menu, U"/", title, U".");
						Thing_cast (GuiMenuItem, parentButton_as_GuiMenuItem, theCommands [parentPosition]. button);
						parentMenu = parentButton_as_GuiMenuItem -> d_menu;
					}
					break;
				}
			}
			if (! parentMenu) parentMenu = windowMenuToWidget (window, menu);   // fallback: put the command in the window's top menu
		}
		if (! parentMenu) {
			trace (U"WARNING: no parent menu for ", window, U"/", menu, U"/", title, U".");
			return NULL;
		}

		/* WHAT TO PUT THERE?
		 */

		if (title == NULL || title [0] == U'-') {
			trace (U"insert the command as a separator");
			theCommands [position]. button = GuiMenu_addSeparator (parentMenu);
			Melder_assert (theCommands [position]. button != NULL);
		} else if (callback == NULL) {
			trace (U"insert the command as a submenu");
			theCommands [position]. button = GuiMenu_createInMenu (parentMenu, title, 0) -> d_menuItem;
			Melder_assert (theCommands [position]. button != NULL);
		} else {
			trace (U"insert the command as a normal menu item");
			theCommands [position]. button = GuiMenu_addItem (parentMenu, title, guiFlags, gui_cb_menu, (void *) callback);
			Melder_assert (theCommands [position]. button != NULL);
		}
		if (hidden) GuiThing_hide (theCommands [position]. button);
	}
	Thing_cast (GuiMenuItem, button_as_GuiMenuItem, theCommands [position]. button);
	return button_as_GuiMenuItem;
}

void praat_addMenuCommandScript (const char32 *window, const char32 *menu, const char32 *title,
	const char32 *after, int depth, const char32 *script)
{
	try {
		Melder_assert (window && menu && title && after && script);
		if (str32len (script) && ! str32len (title))
			Melder_throw (U"Command with script has no title. Window \"", window, U"\", menu \"", menu, U"\".");

		/*
		 * Determine the position of the new command.
		 */
		long position;
		if (str32len (after)) {   // search for existing command with same selection
			long found = lookUpMatchingMenuCommand (window, menu, after);
			if (found) {
				position = found + 1;   // after 'after'
			} else {
				/*Melder_throw (U"The menu command \"", title, U"\" cannot be put after \"", after, U"\",\n"
					U"in the menu \"", menu, "\" in the window \"", window, U"\"\n"
					U"because the latter command does not exist.");*/
				position = theNumberOfCommands + 1;   // default: at end
			}
		} else {
			position = theNumberOfCommands + 1;   /* At end. */
		}

		/*
		 * Increment the command area.
		 */
		if (theNumberOfCommands >= praat_MAXNUM_FIXED_COMMANDS)
			Melder_throw (U"Too many menu commands (maximum ", praat_MAXNUM_FIXED_COMMANDS, U").");
		theNumberOfCommands += 1;

		/*
		 * Make room for insertion.
		 */
		for (long i = theNumberOfCommands; i > position; i --) theCommands [i] = theCommands [i - 1];
		memset (& theCommands [position], 0, sizeof (struct structPraat_Command));

		/*
		 * Insert new command.
		 */
		theCommands [position]. window = Melder_dup_f (window);
		theCommands [position]. menu = Melder_dup_f (menu);
		theCommands [position]. title = str32len (title) ? Melder_dup_f (title) : NULL;   // allow old-fashioned untitled separators
		theCommands [position]. depth = depth;
		theCommands [position]. callback = str32len (script) ? DO_RunTheScriptFromAnyAddedMenuCommand : NULL;   // NULL for a separator or cascade button
		theCommands [position]. executable = str32len (script) != 0;
		if (str32len (script) == 0) {
			theCommands [position]. script = Melder_dup_f (U"");   // Empty string, which will be needed to signal origin
		} else {
			structMelderFile file = { 0 };
			Melder_relativePathToFile (script, & file);
			theCommands [position]. script = Melder_dup_f (Melder_fileToPath (& file));
		}
		theCommands [position]. after = str32len (after) ? Melder_dup_f (after) : NULL;
		if (praatP.phase >= praat_READING_BUTTONS) {
			static long uniqueID = 0;
			theCommands [position]. uniqueID = ++ uniqueID;
		}

		if (! theCurrentPraatApplication -> batch) {
			GuiMenu parentMenu = NULL;

			/* WHERE TO PUT IT?
			 * Determine parent menu widget.
			 * This is not going to fail:
			 * if 'depth' is inappropriate, the alleged subitem will be put in the top menu.
			 */
			if (depth == 0) {
				parentMenu = windowMenuToWidget (window, menu);   // not a subitem: in the top menu
			} else {
				for (long parentPosition = position - 1; parentPosition > 0; parentPosition --) {
					if (theCommands [parentPosition]. depth == depth - 1) {
						if (theCommands [parentPosition]. callback == NULL && theCommands [parentPosition]. title != NULL && theCommands [parentPosition]. title [0] != '-') {
							if (theCommands [parentPosition]. button == NULL)
								Melder_fatal (U"No button for ", window, U"/", menu, U"/", title, U".");
							Melder_assert (theCommands [parentPosition]. button -> classInfo == classGuiMenuItem);
							parentMenu = (static_cast <GuiMenuItem> (theCommands [parentPosition]. button)) -> d_menu;
						}
						break;
					}
				}
				if (! parentMenu) parentMenu = windowMenuToWidget (window, menu);   // fallback: a subitem without a menu title
			}
			if (parentMenu) {
				/* WHAT TO PUT THERE?
				 */
				if (title [0] == '\0' || title [0] == '-') {
					theCommands [position]. button = GuiMenu_addSeparator (parentMenu);
				} else if (script [0] == '\0') {
					theCommands [position]. button = GuiMenu_createInMenu (parentMenu, title, 0) -> d_menuItem;
				} else {
					theCommands [position]. button = GuiMenu_addItem (parentMenu, title, 0, gui_cb_menu, (void *) theCommands [position]. script);   // not just "script"!!
				}
			}
		}

		if (praatP.phase >= praat_HANDLING_EVENTS) praat_sortMenuCommands ();
	} catch (MelderError) {
		Melder_throw (U"Script menu command not added.");
	}
}

void praat_hideMenuCommand (const char32 *window, const char32 *menu, const char32 *title) {
	if (theCurrentPraatApplication -> batch || ! window || ! menu || ! title) return;
	long found = lookUpMatchingMenuCommand (window, menu, title);
	if (! found) return;
	praat_Command command = & theCommands [found];
	if (! command -> hidden && ! command -> unhidable) {
		command -> hidden = TRUE;
		if (praatP.phase >= praat_READING_BUTTONS) command -> toggled = ! command -> toggled;
		if (command -> button) GuiThing_hide (command -> button);
	}
}

void praat_showMenuCommand (const char32 *window, const char32 *menu, const char32 *title) {
	if (theCurrentPraatApplication -> batch || ! window || ! menu || ! title) return;
	long found = lookUpMatchingMenuCommand (window, menu, title);
	if (! found) return;
	praat_Command command = & theCommands [found];
	if (command -> hidden) {
		command -> hidden = FALSE;
		if (praatP.phase >= praat_READING_BUTTONS) command -> toggled = ! command -> toggled;
		if (command -> button) GuiThing_show (command -> button);
	}
}

void praat_saveMenuCommands (MelderString *buffer) {
	long maxID = 0;
	for (long i = 1; i <= theNumberOfCommands; i ++) if (theCommands [i]. uniqueID > maxID) maxID = theCommands [i]. uniqueID;
	for (long id = 1; id <= maxID; id ++)   // sorted
		for (long i = 1; i <= theNumberOfCommands; i ++) {
			praat_Command me = & theCommands [i];
			if (my uniqueID == id && ! my hidden && my window && my menu && my title) {
				MelderString_append (buffer, U"Add menu command... \"", my window, U"\" \"", my menu, U"\" \"", my title, U"\" \"",
					( my after ? my after : U"" ), U"\" ", my depth, U" ", ( my script ? my script : U"" ), U"\n");
				break;
			}
		}
	for (long i = 1; i <= theNumberOfCommands; i ++) {
		praat_Command me = & theCommands [i];
		if (my toggled && my window && my menu && my title && ! my uniqueID && ! my script)
			MelderString_append (buffer, my hidden ? U"Hide" : U"Show", U" menu command... \"",
				my window, U"\" \"", my menu, U"\" ", my title, U"\n");
	}
}

/***** FIXED BUTTONS *****/

void praat_addFixedButtonCommand (GuiForm parent, const char32 *title, void (*callback) (UiForm, int, Stackel, const char32 *, Interpreter, const char32 *, bool, void *), int x, int y) {
	praat_Command me = & theCommands [++ theNumberOfCommands];
	my window = Melder_dup_f (U"Objects");
	my title = title;
	my callback = callback;
	my unhidable = TRUE;
	if (theCurrentPraatApplication -> batch) {
		my button = NULL;
	} else {
		GuiThing button = my button = GuiButton_create (parent, x, x + 82, -y - Gui_PUSHBUTTON_HEIGHT, -y,
			title, gui_button_cb_menu, (void *) callback, 0);   // BUG: shouldn't convert a function pointer to a void pointer
		GuiThing_setSensitive (button, false);
		GuiThing_show (button);
	}
	my executable = false;
}

void praat_sensitivizeFixedButtonCommand (const char32 *title, int sensitive) {
	long i = 1;
	for (; i <= theNumberOfCommands; i ++)
		if (str32equ (theCommands [i]. title, title)) break;   // search
	theCommands [i]. executable = sensitive;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding)
		GuiThing_setSensitive (theCommands [i]. button, sensitive);
}

int praat_doMenuCommand (const char32 *command, const char32 *arguments, Interpreter interpreter) {
	long i = 1;
	while (i <= theNumberOfCommands && (! theCommands [i]. executable || ! str32equ (theCommands [i]. title, command) ||
		(! str32equ (theCommands [i]. window, U"Objects") && ! str32equ (theCommands [i]. window, U"Picture")))) i ++;
	if (i > theNumberOfCommands) return 0;
	theCommands [i]. callback (NULL, 0, NULL, arguments, interpreter, command, false, NULL);
	return 1;
}

int praat_doMenuCommand (const char32 *command, int narg, Stackel args, Interpreter interpreter) {
	long i = 1;
	while (i <= theNumberOfCommands && (! theCommands [i]. executable || ! str32equ (theCommands [i]. title, command) ||
		(! str32equ (theCommands [i]. window, U"Objects") && ! str32equ (theCommands [i]. window, U"Picture")))) i ++;
	if (i > theNumberOfCommands) return 0;
	theCommands [i]. callback (NULL, narg, args, NULL, interpreter, command, false, NULL);
	return 1;
}

long praat_getNumberOfMenuCommands (void) { return theNumberOfCommands; }

praat_Command praat_getMenuCommand (long i)
	{ return i < 1 || i > theNumberOfCommands ? NULL : & theCommands [i]; }

void praat_addCommandsToEditor (Editor me) {
	const char32 *windowName = my classInfo -> className;
	for (long i = 1; i <= theNumberOfCommands; i ++) if (str32equ (theCommands [i]. window, windowName)) {
		Editor_addCommandScript (me, theCommands [i]. menu, theCommands [i]. title, 0, theCommands [i]. script);
	}
}

/* End of file praat_menuCommands.cpp */
