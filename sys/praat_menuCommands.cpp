/* praat_menuCommands.cpp
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
		compare = wcscmp (my window, thy window);
		if (compare) return compare;
	} else if (thy window) return -1;
	if (my menu) {
		if (! thy menu) return 1;
		compare = wcscmp (my menu, thy menu);
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

static long lookUpMatchingMenuCommand (const wchar_t *window, const wchar_t *menu, const wchar_t *title) {
/*
 * A menu command is fully specified by its environment (window + menu) and its title.
 */
	for (long i = 1; i <= theNumberOfCommands; i ++) {
		const wchar_t *tryWindow = theCommands [i]. window;
		const wchar_t *tryMenu = theCommands [i]. menu;
		const wchar_t *tryTitle = theCommands [i]. title;
		if ((window == tryWindow || (window && tryWindow && wcsequ (window, tryWindow))) &&
		    (menu == tryMenu || (menu && tryMenu && wcsequ (menu, tryMenu))) &&
		    (title == tryTitle || (title && tryTitle && wcsequ (title, tryTitle)))) return i;
	}
	return 0;   // not found
}

static void do_menu (I, unsigned long modified) {
	void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *) = (void (*) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *)) void_me;
	Melder_assert (callback != NULL);
	for (long i = 1; i <= theNumberOfCommands; i ++) {
		praat_Command me = & theCommands [i];
		if (my callback == callback) {
			if (my title != NULL && ! wcsstr (my title, L"...")) {
				UiHistory_write (L"\ndo (\"");
				UiHistory_write_expandQuotes (my title);
				UiHistory_write (L"\")");
			}
			try {
				callback (NULL, 0, NULL, NULL, NULL, my title, modified, NULL);
			} catch (MelderError) {
				Melder_error_ ("Command \"", my title, "\" not executed.");
				Melder_flushError (NULL);
			}
			praat_updateSelection (); return;
		}
		if (my callback == DO_RunTheScriptFromAnyAddedMenuCommand && my script == (void *) void_me) {
			if (my title != NULL && ! wcsstr (my title, L"...")) {
				UiHistory_write (L"\nexecute ");
				UiHistory_write (my script);
			} else {
				UiHistory_write (L"\nexecute \"");
				UiHistory_write (my script);
				UiHistory_write (L"\"");
			}
			try {
				DO_RunTheScriptFromAnyAddedMenuCommand (NULL, 0, NULL, my script, NULL, NULL, false, NULL);
			} catch (MelderError) {
				Melder_error_ ("Command \"", my title, "\" not executed.");
				Melder_flushError (NULL);
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

static GuiMenu windowMenuToWidget (const wchar_t *window, const wchar_t *menu) {
	return
		wcsequ (window, L"Picture") ? praat_picture_resolveMenu (menu) :
		wcsequ (window, L"Objects") ? praat_objects_resolveMenu (menu) : NULL;
}

GuiMenuItem praat_addMenuCommand (const wchar_t *window, const wchar_t *menu, const wchar_t *title,
	const wchar_t *after, unsigned long flags, void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *))
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
		Melder_error_ ("praat_addMenuCommand: command with callback has no title. Window \"", window, "\", menu \"", menu, "\".");
		Melder_flushError (NULL);
		return NULL;
	}

	/* Determine the position of the new command.
	 */
	if (after) {   /* Search for existing command with same selection. */
		long found = lookUpMatchingMenuCommand (window, menu, after);
		if (found) {
			position = found + 1;   // after 'after'
		} else {
			Melder_error_ ("praat_addMenuCommand: the command \"", title, "\" cannot be put after \"", after, "\",\n"
				"in the menu \"", menu, "\" in the window \"", window, "\"\n"
				"because the latter command does not exist.");
			Melder_flushError (NULL);
			return NULL;
		}
	} else {
		position = theNumberOfCommands + 1;   // at end
	}

	/* Increment the command area.
	 */
	if (theNumberOfCommands >= praat_MAXNUM_FIXED_COMMANDS) {
		Melder_flushError ("praat_addMenuCommand: too many menu commands.");
		return NULL;
	}
	theNumberOfCommands += 1;

	/* Make room for insertion.
	 */
	for (long i = theNumberOfCommands; i > position; i --) theCommands [i] = theCommands [i - 1];
	memset (& theCommands [position], 0, sizeof (struct structPraat_Command));

	trace ("insert new command \"%ls\"", title);
	theCommands [position]. window = Melder_wcsdup_f (window);
	theCommands [position]. menu = Melder_wcsdup_f (menu);
	theCommands [position]. title = Melder_wcsdup_f (title);
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
							Melder_fatal ("No button for %ls/%ls/%ls.", window, menu, title);
						Thing_cast (GuiMenuItem, parentButton_as_GuiMenuItem, theCommands [parentPosition]. button);
						parentMenu = parentButton_as_GuiMenuItem -> d_menu;
					}
					break;
				}
			}
			if (! parentMenu) parentMenu = windowMenuToWidget (window, menu);   // fallback: put the command in the window's top menu
		}
		if (! parentMenu) {
			trace ("WARNING: no parent menu for %ls/%ls/%ls.", window, menu, title);
			return NULL;
		}

		/* WHAT TO PUT THERE?
		 */

		if (title == NULL || title [0] == '-') {
			trace ("insert the command as a separator");
			theCommands [position]. button = GuiMenu_addSeparator (parentMenu);
			Melder_assert (theCommands [position]. button != NULL);
		} else if (callback == NULL) {
			trace ("insert the command as a submenu");
			theCommands [position]. button = GuiMenu_createInMenu (parentMenu, title, 0) -> d_menuItem;
			Melder_assert (theCommands [position]. button != NULL);
		} else {
			trace ("insert the command as a normal menu item");
			theCommands [position]. button = GuiMenu_addItem (parentMenu, title, guiFlags, gui_cb_menu, (void *) callback);
			Melder_assert (theCommands [position]. button != NULL);
		}
		if (hidden) theCommands [position]. button -> f_hide ();
	}
	Thing_cast (GuiMenuItem, button_as_GuiMenuItem, theCommands [position]. button);
	return button_as_GuiMenuItem;
}

void praat_addMenuCommandScript (const wchar_t *window, const wchar_t *menu, const wchar_t *title,
	const wchar_t *after, int depth, const wchar_t *script)
{
	try {
		Melder_assert (window && menu && title && after && script);
		if (wcslen (script) && ! wcslen (title))
			Melder_throw ("Command with script has no title. Window \"", window, "\", menu \"", menu, "\".");

		/*
		 * Determine the position of the new command.
		 */
		long position;
		if (wcslen (after)) {   /* Search for existing command with same selection. */
			long found = lookUpMatchingMenuCommand (window, menu, after);
			if (found) {
				position = found + 1;   /* After 'after'. */
			} else {
				/*Melder_throw ("The menu command \"", title, "\" cannot be put after \"", after, "\",\n"
					"in the menu \"", menu, "\" in the window \"", window, "\"\n"
					"because the latter command does not exist.", title, after, menu, window);*/
				position = theNumberOfCommands + 1;   // default: at end
			}
		} else {
			position = theNumberOfCommands + 1;   /* At end. */
		}

		/*
		 * Increment the command area.
		 */
		if (theNumberOfCommands >= praat_MAXNUM_FIXED_COMMANDS)
			Melder_throw ("Too many menu commands (maximum ", praat_MAXNUM_FIXED_COMMANDS, ").");
		theNumberOfCommands += 1;

		/*
		 * Make room for insertion.
		 */
		for (long i = theNumberOfCommands; i > position; i --) theCommands [i] = theCommands [i - 1];
		memset (& theCommands [position], 0, sizeof (struct structPraat_Command));

		/*
		 * Insert new command.
		 */
		theCommands [position]. window = Melder_wcsdup_f (window);
		theCommands [position]. menu = Melder_wcsdup_f (menu);
		theCommands [position]. title = wcslen (title) ? Melder_wcsdup_f (title) : NULL;   /* Allow old-fashioned untitled separators. */
		theCommands [position]. depth = depth;
		theCommands [position]. callback = wcslen (script) ? DO_RunTheScriptFromAnyAddedMenuCommand : NULL;   /* NULL for a separator or cascade button. */
		theCommands [position]. executable = wcslen (script) != 0;
		if (wcslen (script) == 0) {
			theCommands [position]. script = Melder_wcsdup_f (L"");   /* Empty string, which will be needed to signal origin. */
		} else {
			structMelderFile file = { 0 };
			Melder_relativePathToFile (script, & file);
			theCommands [position]. script = Melder_wcsdup_f (Melder_fileToPath (& file));
		}
		theCommands [position]. after = wcslen (after) ? Melder_wcsdup_f (after) : NULL;
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
				parentMenu = windowMenuToWidget (window, menu);   /* Not a subitem: in the top menu. */
			} else {
				for (long parentPosition = position - 1; parentPosition > 0; parentPosition --) {
					if (theCommands [parentPosition]. depth == depth - 1) {
						if (theCommands [parentPosition]. callback == NULL && theCommands [parentPosition]. title != NULL && theCommands [parentPosition]. title [0] != '-') {
							if (theCommands [parentPosition]. button == NULL)
								Melder_fatal ("No button for %ls/%ls/%ls.", window, menu, title);
							Melder_assert (theCommands [parentPosition]. button -> classInfo == classGuiMenuItem);
							parentMenu = (static_cast <GuiMenuItem> (theCommands [parentPosition]. button)) -> d_menu;
						}
						break;
					}
				}
				if (! parentMenu) parentMenu = windowMenuToWidget (window, menu);   /* Fallback: a subitem without a menu title. */
			}
			if (parentMenu) {
				/* WHAT TO PUT THERE?
				 */
				if (title [0] == '\0' || title [0] == '-') {
					theCommands [position]. button = GuiMenu_addSeparator (parentMenu);
				} else if (script [0] == '\0') {
					theCommands [position]. button = GuiMenu_createInMenu (parentMenu, title, 0) -> d_menuItem;
				} else {
					theCommands [position]. button = GuiMenu_addItem (parentMenu, title, 0, gui_cb_menu, (void *) theCommands [position]. script);   /* Not just "script"!! */
				}
			}
		}

		if (praatP.phase >= praat_HANDLING_EVENTS) praat_sortMenuCommands ();
	} catch (MelderError) {
		Melder_throw ("Script menu command not added.");
	}
}

void praat_hideMenuCommand (const wchar_t *window, const wchar_t *menu, const wchar_t *title) {
	if (theCurrentPraatApplication -> batch || ! window || ! menu || ! title) return;
	long found = lookUpMatchingMenuCommand (window, menu, title);
	if (! found) return;
	praat_Command command = & theCommands [found];
	if (! command -> hidden && ! command -> unhidable) {
		command -> hidden = TRUE;
		if (praatP.phase >= praat_READING_BUTTONS) command -> toggled = ! command -> toggled;
		if (command -> button) command -> button -> f_hide ();
	}
}

void praat_showMenuCommand (const wchar_t *window, const wchar_t *menu, const wchar_t *title) {
	if (theCurrentPraatApplication -> batch || ! window || ! menu || ! title) return;
	long found = lookUpMatchingMenuCommand (window, menu, title);
	if (! found) return;
	praat_Command command = & theCommands [found];
	if (command -> hidden) {
		command -> hidden = FALSE;
		if (praatP.phase >= praat_READING_BUTTONS) command -> toggled = ! command -> toggled;
		if (command -> button) command -> button -> f_show ();
	}
}

void praat_saveMenuCommands (MelderString *buffer) {
	long maxID = 0;
	for (long i = 1; i <= theNumberOfCommands; i ++) if (theCommands [i]. uniqueID > maxID) maxID = theCommands [i]. uniqueID;
	for (long id = 1; id <= maxID; id ++)   /* Sorted. */
		for (long i = 1; i <= theNumberOfCommands; i ++) {
			praat_Command me = & theCommands [i];
			if (my uniqueID == id && ! my hidden && my window && my menu && my title) {
				MelderString_append (buffer, L"Add menu command... \"", my window, L"\" \"", my menu, L"\" \"", my title, L"\" \"");
				MelderString_append (buffer, my after ? my after : L"", L"\" ", Melder_integer (my depth), L" ", my script ? my script : L"", L"\n");
				break;
			}
		}
	for (long i = 1; i <= theNumberOfCommands; i ++) {
		praat_Command me = & theCommands [i];
		if (my toggled && my window && my menu && my title && ! my uniqueID && ! my script)
			MelderString_append (buffer, my hidden ? L"Hide" : L"Show", L" menu command... \"",
				my window, L"\" \"", my menu, L"\" ", my title, L"\n");
	}
}

/***** FIXED BUTTONS *****/

void praat_addFixedButtonCommand (GuiForm parent, const wchar_t *title, void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *), int x, int y) {
	praat_Command me = & theCommands [++ theNumberOfCommands];
	my window = Melder_wcsdup_f (L"Objects");
	my title = title;
	my callback = callback;
	my unhidable = TRUE;
	if (theCurrentPraatApplication -> batch) {
		my button = NULL;
	} else {
		GuiThing button = my button = GuiButton_create (parent, x, x + 82, -y - Gui_PUSHBUTTON_HEIGHT, -y,
			title, gui_button_cb_menu, (void *) callback, 0);   // BUG: shouldn't convert a function pointer to a void pointer
		button -> f_setSensitive (false);
		button -> f_show ();
	}
	my executable = false;
}

void praat_sensitivizeFixedButtonCommand (const wchar_t *title, int sensitive) {
	long i = 1;
	for (; i <= theNumberOfCommands; i ++)
		if (wcsequ (theCommands [i]. title, title)) break;   // search
	theCommands [i]. executable = sensitive;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding) theCommands [i]. button -> f_setSensitive (sensitive);
}

int praat_doMenuCommand (const wchar_t *command, const wchar_t *arguments, Interpreter interpreter) {
	long i = 1;
	while (i <= theNumberOfCommands && (! theCommands [i]. executable || ! wcsequ (theCommands [i]. title, command) ||
		(! wcsequ (theCommands [i]. window, L"Objects") && ! wcsequ (theCommands [i]. window, L"Picture")))) i ++;
	if (i > theNumberOfCommands) return 0;
	theCommands [i]. callback (NULL, 0, NULL, arguments, interpreter, command, false, NULL);
	return 1;
}

int praat_doMenuCommand (const wchar_t *command, int narg, Stackel args, Interpreter interpreter) {
	long i = 1;
	while (i <= theNumberOfCommands && (! theCommands [i]. executable || ! wcsequ (theCommands [i]. title, command) ||
		(! wcsequ (theCommands [i]. window, L"Objects") && ! wcsequ (theCommands [i]. window, L"Picture")))) i ++;
	if (i > theNumberOfCommands) return 0;
	theCommands [i]. callback (NULL, narg, args, NULL, interpreter, command, false, NULL);
	return 1;
}

long praat_getNumberOfMenuCommands (void) { return theNumberOfCommands; }

praat_Command praat_getMenuCommand (long i)
	{ return i < 1 || i > theNumberOfCommands ? NULL : & theCommands [i]; }

void praat_addCommandsToEditor (Editor me) {
	const wchar_t *windowName = my classInfo -> className;
	for (long i = 1; i <= theNumberOfCommands; i ++) if (wcsequ (theCommands [i]. window, windowName)) {
		if (! Editor_addCommandScript (me, theCommands [i]. menu, theCommands [i]. title, 0, theCommands [i]. script))
			Melder_flushError ("To fix this, go to Praat->Preferences->Buttons->Editors, "
				"and remove the script from this menu.\n"
				"You may want to install the script in a different menu.");
	}
}

/* End of file praat_menuCommands.cpp */
