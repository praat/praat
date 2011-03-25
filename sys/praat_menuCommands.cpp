/* praat_menuCommands.c
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
 * pb 2002/03/18 Mach
 * pb 2005/08/22 renamed Control menu to "Praat"
 * pb 2006/12/26 theCurrentPraat
 * pb 2007/01/26 buttons along top
 * pb 2007/06/09 wchar_t
 * pb 2009/01/17 arguments to UiForm callbacks
 * pb 2011/01/02 GTK: allow submenus even from scripts
 */

#include "praatP.h"
#include "praat_script.h"

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
	return 0;   /* Not found. */
}

static void do_menu (I, unsigned long modified) {
	int (*callback) (UiForm, const wchar_t *, Interpreter, const wchar_t *, bool, void *) = (int (*) (UiForm, const wchar_t *, Interpreter, const wchar_t *, bool, void *)) void_me;
	Melder_assert (callback != NULL);
	for (long i = 1; i <= theNumberOfCommands; i ++) {
		praat_Command me = & theCommands [i];
		if (my callback == callback) {
			if (my title != NULL && ! wcsstr (my title, L"...")) {
				UiHistory_write (L"\n");
				UiHistory_write (my title);
			}
			if (! callback (NULL, NULL, NULL, my title, modified, NULL))
				Melder_flushError ("Command not executed.");
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
			if (! DO_RunTheScriptFromAnyAddedMenuCommand (NULL, my script, NULL, NULL, false, NULL))
				Melder_flushError ("Script not executed.");
			praat_updateSelection (); return;
		}
	}
}

static void gui_button_cb_menu (I, GuiButtonEvent event) {
	do_menu (void_me, event -> shiftKeyPressed | event -> commandKeyPressed | event -> optionKeyPressed | event -> extraControlKeyPressed);
}

static void gui_cb_menu (GUI_ARGS) {
/*
 *	Convert a Motif callback into a Ui callback, and catch modifier keys and special mouse buttons.
 *	Call that callback!
 *	Catch the error queue for menu commands without dots (...).
 */
	(void) w;
#if defined (macintosh)
	EventRecord *event = (EventRecord *) call;
	enum { cmdKey = 256, shiftKey = 512, optionKey = 2048, controlKey = 4096 };
	int modified = event -> what == mouseDown &&
		(event -> modifiers & (cmdKey | shiftKey | optionKey | controlKey)) != 0;
#elif defined (_WIN32)
	int modified = FALSE;
#else
	#if gtk
		// TODO: We kunnen bij GDK events geregistreren om key en button press af te vangen, maar dan
		// wordt dit geen gewoon 'activate' signaal meer.
		int modified = 0;

		// TODO: Dit implementeert het gedrag van Motif, en impliceert dat een toggled Call alleen een
		// 'naar positief' is en niet de terug toggle 'naar negatief'.
		if (G_OBJECT_TYPE(w) == GTK_TYPE_RADIO_MENU_ITEM && !gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(w))) { 
			//g_debug("Gotcha!");
			return;
		}
	#elif motif
		XButtonPressedEvent *event = (XButtonPressedEvent *) ((XmDrawingAreaCallbackStruct *) call) -> event;
		int modified = event -> type == ButtonPress &&
			((event -> state & (ShiftMask | ControlMask | Mod1Mask)) != 0 || event -> button == Button2 || event -> button == Button3);
	#endif
#endif
	do_menu (void_me, modified);
}

static GuiObject windowMenuToWidget (const wchar_t *window, const wchar_t *menu) {
	return
		wcsequ (window, L"Picture") ? praat_picture_resolveMenu (menu) :
		wcsequ (window, L"Objects") ? praat_objects_resolveMenu (menu) : NULL;
}

GuiObject praat_addMenuCommand (const wchar_t *window, const wchar_t *menu, const wchar_t *title,
	const wchar_t *after, unsigned long flags, int (*callback) (UiForm, const wchar_t *, Interpreter, const wchar_t *, bool, void *))
{
	long position;
	int depth = flags, unhidable = FALSE, hidden = FALSE, key = 0;
	unsigned long motifFlags = 0;
	if (flags > 7) {
		depth = ((flags & praat_DEPTH_7) >> 16);
		unhidable = (flags & praat_UNHIDABLE) != 0;
		hidden = (flags & praat_HIDDEN) != 0 && ! unhidable;
		key = flags & 0x000000FF;
		motifFlags = key ? flags & (0x006000FF | GuiMenu_BUTTON_STATE_MASK) : flags & GuiMenu_BUTTON_STATE_MASK;
	}
	if (callback && ! title) {
		Melder_error5 (L"praat_addMenuCommand: command with callback has no title. Window \"", window, L"\", menu \"", menu, L"\".");
		Melder_flushError (NULL);
		return NULL;
	}

	/* Determine the position of the new command.
	 */
	if (after) {   /* Search for existing command with same selection. */
		long found = lookUpMatchingMenuCommand (window, menu, after);
		if (found) {
			position = found + 1;   /* After 'after'. */
		} else {
			Melder_error9 (L"praat_addMenuCommand: the command \"", title, L"\" cannot be put after \"", after, L"\",\n"
				"in the menu \"", menu, L"\" in the window \"", window, L"\"\n"
				"because the latter command does not exist.");
			Melder_flushError (NULL);
			return NULL;
		}
	} else {
		position = theNumberOfCommands + 1;   /* At end. */
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

	/* Insert new command.
	 */
	theCommands [position]. window = Melder_wcsdup_f (window);
	theCommands [position]. menu = Melder_wcsdup_f (menu);
	theCommands [position]. title = Melder_wcsdup_f (title);
	theCommands [position]. depth = depth;
	theCommands [position]. callback = callback;   /* NULL for a separator or cascade button. */
	theCommands [position]. executable = callback != NULL;
	theCommands [position]. script = NULL;
	theCommands [position]. hidden = hidden;
	theCommands [position]. unhidable = unhidable;

	if (! theCurrentPraatApplication -> batch) {
		GuiObject parent = NULL;

		/* WHERE TO PUT IT?
		 * Determine parent menu widget.
		 * This is not going to fail:
		 * if 'depth' is inappropriate, the alleged subitem will be put in the top menu.
		 */
		if (depth == 0) {
			parent = windowMenuToWidget (window, menu);   /* Not a subitem: in the top menu. */
		} else {
			for (long i = position - 1; i > 0; i --) {
				if (theCommands [i]. depth == depth - 1) {
					if (theCommands [i]. callback == NULL && theCommands [i]. title != NULL && theCommands [i]. title [0] != '-')   /* Cascade button? */
						#if gtk
							parent = gtk_menu_item_get_submenu (GTK_MENU_ITEM (theCommands [i]. button));
						#elif motif
							XtVaGetValues (theCommands [i]. button, XmNsubMenuId, & parent, NULL);   /* The relevant menu title. */
						#endif
					break;
				}
			}
			if (! parent) parent = windowMenuToWidget (window, menu);   /* Fallback: a subitem without a menu title. */
		}
if (! parent) return NULL;

		/* WHAT TO PUT THERE?
		 */

		if (title == NULL || title [0] == '-') {
			theCommands [position]. button = GuiMenu_addSeparator (parent);
		} else if (callback == NULL) {
			(void) GuiMenuBar_addMenu2 (parent, title, 0, & theCommands [position]. button);   /* Cascade button with submenu. */
		} else {
			theCommands [position]. button = GuiMenu_addItem (parent, title, motifFlags, gui_cb_menu, (void *) callback);
		}
		if (hidden) GuiObject_hide (theCommands [position]. button);
	}
	return theCommands [position]. button;
}

int praat_addMenuCommandScript (const wchar_t *window, const wchar_t *menu, const wchar_t *title,
	const wchar_t *after, int depth, const wchar_t *script)
{
	long position;

	Melder_assert (window && menu && title && after && script);
	if (wcslen (script) && ! wcslen (title))
		return Melder_error5 (L"praat_addMenuCommand: command with script has no title. Window \"", window, L"\", menu \"", menu, L"\".");

	/* Determine the position of the new command.
	 */
	if (wcslen (after)) {   /* Search for existing command with same selection. */
		long found = lookUpMatchingMenuCommand (window, menu, after);
		if (found) {
			position = found + 1;   /* After 'after'. */
		} else {
			/*return Melder_error ("praat_addMenuCommand: the command \"%ls\" cannot be put after \"%ls\",\n"
				"in the menu \"%ls\" in the window \"%ls\"\n"
				"because the latter command does not exist.", title, after, menu, window);*/
			position = theNumberOfCommands + 1;   /* Default: at end. */
		}
	} else {
		position = theNumberOfCommands + 1;   /* At end. */
	}

	/* Increment the command area.
	 */
	if (theNumberOfCommands >= praat_MAXNUM_FIXED_COMMANDS)
		return Melder_error1 (L"praat_addMenuCommand: too many menu commands.");
	theNumberOfCommands += 1;

	/* Make room for insertion.
	 */
	for (long i = theNumberOfCommands; i > position; i --) theCommands [i] = theCommands [i - 1];
	memset (& theCommands [position], 0, sizeof (struct structPraat_Command));

	/* Insert new command.
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
		GuiObject parent = NULL;

		/* WHERE TO PUT IT?
		 * Determine parent menu widget.
		 * This is not going to fail:
		 * if 'depth' is inappropriate, the alleged subitem will be put in the top menu.
		 */
		if (depth == 0) {
			parent = windowMenuToWidget (window, menu);   /* Not a subitem: in the top menu. */
		} else {
			for (long i = position - 1; i > 0; i --) {
				if (theCommands [i]. depth == depth - 1) {
					if (theCommands [i]. callback == NULL && theCommands [i]. title != NULL && theCommands [i]. title [0] != '-')   /* Cascade button? */
						#if gtk
							parent = gtk_menu_item_get_submenu (GTK_MENU_ITEM (theCommands [i]. button));
						#elif motif
							XtVaGetValues (theCommands [i]. button, XmNsubMenuId, & parent, NULL);   /* The relevant menu title. */
						#endif
					break;
				}
			}
			if (! parent) parent = windowMenuToWidget (window, menu);   /* Fallback: a subitem without a menu title. */
		}
		if (parent) {
			/* WHAT TO PUT THERE?
			 */
			if (title [0] == '\0' || title [0] == '-') {
				theCommands [position]. button = GuiMenu_addSeparator (parent);
			} else if (script [0] == '\0') {
				(void) GuiMenuBar_addMenu2 (parent, title, 0, & theCommands [position]. button);
			} else {
				theCommands [position]. button = GuiMenu_addItem (parent, title, 0, gui_cb_menu, (void *) theCommands [position]. script);   /* Not just "script"!! */
			}
		}
	}

	if (praatP.phase >= praat_HANDLING_EVENTS) praat_sortMenuCommands ();
	return 1;
}

int praat_hideMenuCommand (const wchar_t *window, const wchar_t *menu, const wchar_t *title) {
	if (theCurrentPraatApplication -> batch || ! window || ! menu || ! title) return 1;
	long found = lookUpMatchingMenuCommand (window, menu, title);
	if (! found) return 0;
	praat_Command command = & theCommands [found];
	if (! command -> hidden && ! command -> unhidable) {
		command -> hidden = TRUE;
		if (praatP.phase >= praat_READING_BUTTONS) command -> toggled = ! command -> toggled;
		if (command -> button) GuiObject_hide (command -> button);
	}
	return 1;
}

int praat_showMenuCommand (const wchar_t *window, const wchar_t *menu, const wchar_t *title) {
	if (theCurrentPraatApplication -> batch || ! window || ! menu || ! title) return 1;
	long found = lookUpMatchingMenuCommand (window, menu, title);
	if (! found) return 0;
	praat_Command command = & theCommands [found];
	if (command -> hidden) {
		command -> hidden = FALSE;
		if (praatP.phase >= praat_READING_BUTTONS) command -> toggled = ! command -> toggled;
		if (command -> button) GuiObject_show (command -> button);
	}
	return 1;
}

void praat_saveMenuCommands (FILE *f) {
	long maxID = 0;
	for (long i = 1; i <= theNumberOfCommands; i ++) if (theCommands [i]. uniqueID > maxID) maxID = theCommands [i]. uniqueID;
	for (long id = 1; id <= maxID; id ++)   /* Sorted. */
		for (long i = 1; i <= theNumberOfCommands; i ++) {
			praat_Command me = & theCommands [i];
			if (my uniqueID == id && ! my hidden && my window && my menu && my title) {
				fwprintf (f, L"Add menu command... \"%ls\" \"%ls\" \"%ls\" \"%ls\" %d %ls\n",
					my window, my menu, my title, my after ? my after : L"", my depth, my script ? my script : L"");
				break;
			}
		}
	for (long i = 1; i <= theNumberOfCommands; i ++) {
		praat_Command me = & theCommands [i];
		if (my toggled && my window && my menu && my title && ! my uniqueID && ! my script)
			fwprintf (f, L"%ls menu command... \"%ls\" \"%ls\" %ls\n",
				my hidden ? L"Hide" : L"Show", my window, my menu, my title);
	}
}

/***** FIXED BUTTONS *****/

void praat_addFixedButtonCommand (GuiObject parent, const wchar_t *title, int (*callback) (UiForm, const wchar_t *, Interpreter, const wchar_t *, bool, void *), int x, int y) {
	praat_Command me = & theCommands [++ theNumberOfCommands];
	my window = Melder_wcsdup_f (L"Objects");
	my title = title;
	my callback = callback;
	my unhidable = TRUE;
	if (theCurrentPraatApplication -> batch) {
		my button = NULL;
	} else {
		GuiObject button = my button = GuiButton_create (parent, x, x + 82, Gui_AUTOMATIC, -y,
			title, gui_button_cb_menu, (void *) callback, 0);   // BUG: shouldn't convert a function pointer to a void pointer
		GuiObject_setSensitive (button, false);
		GuiObject_show (button);
	}
	my executable = False;
}

void praat_sensitivizeFixedButtonCommand (const wchar_t *title, int sensitive) {
	long i;
	for (i = 1; i <= theNumberOfCommands; i ++)
		if (wcsequ (theCommands [i]. title, title)) break;   /* Search. */
	theCommands [i]. executable = sensitive;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding) GuiObject_setSensitive (theCommands [i]. button, sensitive);
}

int praat_doMenuCommand (const wchar_t *command, const wchar_t *arguments, Interpreter interpreter) {
	long i = 1;
	while (i <= theNumberOfCommands && (! theCommands [i]. executable || ! wcsequ (theCommands [i]. title, command) ||
		(! wcsequ (theCommands [i]. window, L"Objects") && ! wcsequ (theCommands [i]. window, L"Picture")))) i ++;
	if (i > theNumberOfCommands) return 0;
	if (! theCommands [i]. callback (NULL, arguments, interpreter, NULL, false, NULL))
		return 0;
	return 1;
}

long praat_getNumberOfMenuCommands (void) { return theNumberOfCommands; }

praat_Command praat_getMenuCommand (long i)
	{ return i < 1 || i > theNumberOfCommands ? NULL : & theCommands [i]; }

void praat_addCommandsToEditor (Editor me) {
	const wchar_t *windowName = our _className;
	for (long i = 1; i <= theNumberOfCommands; i ++) if (wcsequ (theCommands [i]. window, windowName)) {
		if (! Editor_addCommandScript (me, theCommands [i]. menu, theCommands [i]. title, 0, theCommands [i]. script))
			Melder_flushError ("To fix this, go to Praat:Preferences:Buttons:Editors, "
				"and remove the script from this menu.\n"
				"You may want to install the script in a different menu.");
	}
}

/* End of file praat_menuCommands.c */
