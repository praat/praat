/* praat_menuCommands.c
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
 * pb 2002/03/18 Mach
 * pb 2005/08/22 renamed Control menu to "Praat"
 */

#include "praatP.h"
#include "praat_script.h"

#define praat_MAXNUM_FIXED_COMMANDS  1000
static long theNumberOfCommands = 0;
static struct structPraat_Command *theCommands;

void praat_menuCommands_init (void) {
	theCommands = Melder_calloc (praat_MAXNUM_FIXED_COMMANDS + 1, sizeof (struct structPraat_Command));
}

static int compareMenuCommands (const void *void_me, const void *void_thee) {
	praat_Command me = (praat_Command) void_me, thee = (praat_Command) void_thee;
	int compare;
	if (my window) {
		if (! thy window) return 1;
		compare = strcmp (my window, thy window);
		if (compare) return compare;
	} else if (thy window) return -1;
	if (my menu) {
		if (! thy menu) return 1;
		compare = strcmp (my menu, thy menu);
		if (compare) return compare;
	} else if (thy menu) return -1;
	if (my sortingTail < thy sortingTail) return -1;
	return 1;
}

void praat_sortMenuCommands (void) {
	long i;
	for (i = 1; i <= theNumberOfCommands; i ++)
		theCommands [i]. sortingTail = i;
	qsort (& theCommands [1], theNumberOfCommands, sizeof (struct structPraat_Command), compareMenuCommands);
}

static int lookUpMatchingMenuCommand (const char *window, const char *menu, const char *title) {
/*
 * A menu command is fully specified by its environment (window + menu) and its title.
 */
	int i;
	for (i = 1; i <= theNumberOfCommands; i ++) {
		const char *tryWindow = theCommands [i]. window;
		const char *tryMenu = theCommands [i]. menu;
		const char *tryTitle = theCommands [i]. title;
		if ((window == tryWindow || (window && tryWindow && strequ (window, tryWindow))) &&
		    (menu == tryMenu || (menu && tryMenu && strequ (menu, tryMenu))) &&
		    (title == tryTitle || (title && tryTitle && strequ (title, tryTitle)))) return i;
	}
	return 0;   /* Not found. */
}

MOTIF_CALLBACK (cb_menu)
/*
 *	Convert a Motif callback into a Ui callback, and catch modifier keys and special mouse buttons.
 *	Call that callback!
 *	Catch the error queue for menu commands without dots (...).
 */
	int (*callback) (Any, void *) = (int (*) (Any, void *)) void_me;
	int i;
#if defined (macintosh)
	XEvent *event = (XEvent *) call;
	enum { cmdKey = 256, shiftKey = 512, optionKey = 2048, controlKey = 4096 };
	int modified = event -> what == mouseDown &&
		(event -> modifiers & (cmdKey | shiftKey | optionKey | controlKey)) != 0;
#elif defined (_WIN32)
	int modified = FALSE;
#else
	XButtonPressedEvent *event = (XButtonPressedEvent *) ((XmDrawingAreaCallbackStruct *) call) -> event;
	int modified = event -> type == ButtonPress &&
		((event -> state & (ShiftMask | ControlMask | Mod1Mask)) != 0 || event -> button == Button2 || event -> button == Button3);
#endif
	Melder_assert (callback != NULL);
	for (i = 1; i <= theNumberOfCommands; i ++) {
		praat_Command me = & theCommands [i];
		if (my callback == callback) {
			if (my title && ! strequ (my title, "History to script..."))
					UiHistory_write ("\n%s", my title);
			if (! callback (NULL, (XtPointer) modified))
				Melder_flushError ("Command not executed.");
			praat_updateSelection (); return;
		}
		if (my callback == DO_RunTheScriptFromAnyAddedMenuCommand && my script == (void *) void_me) {
			UiHistory_write ("\nexecute");
			if (! DO_RunTheScriptFromAnyAddedMenuCommand ((char *) my script, NULL))
				Melder_flushError ("Script not executed.");
			praat_updateSelection (); return;
		}
	}
MOTIF_CALLBACK_END

static Widget windowMenuToWidget (const char *window, const char *menu) {
	return
		strequ (window, "Picture") ? praat_picture_resolveMenu (menu) :
		strequ (window, "Objects") ? praat_objects_resolveMenu (menu) : NULL;
}

Widget praat_addMenuCommand (const char *window, const char *menu, const char *title,
	const char *after, unsigned long flags, int (*callback) (Any, void *))
{
	int i, position;
	int depth = flags, unhidable = FALSE, hidden = FALSE, key = 0;
	unsigned long motifFlags = 0;
	if (flags > 7) {
		depth = ((flags & 0x00070000) >> 16);
		unhidable = (flags & praat_UNHIDABLE) != 0;
		hidden = (flags & praat_HIDDEN) != 0 && ! unhidable;
		key = flags & 0x000000FF;
		motifFlags = key ? flags & 0x006007FF : flags & 0x00000700;
	}
	if (callback && ! title) {
		Melder_flushError ("praat_addMenuCommand: command with callback has no title. Window \"%s\", menu \"%s\".",
			window, menu);
		return NULL;
	}

	/* Determine the position of the new command.
	 */
	if (after) {   /* Search for existing command with same selection. */
		int found = lookUpMatchingMenuCommand (window, menu, after);
		if (found) {
			position = found + 1;   /* After 'after'. */
		} else {
			Melder_flushError ("praat_addMenuCommand: the command \"%s\" cannot be put after \"%s\",\n"
				"in the menu \"%s\" in the window \"%s\"\n"
				"because the latter command does not exist.", title, after, menu, window);
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
	for (i = theNumberOfCommands; i > position; i --) theCommands [i] = theCommands [i - 1];
	memset (& theCommands [position], 0, sizeof (struct structPraat_Command));

	/* Insert new command.
	 */
	theCommands [position]. window = window;
	theCommands [position]. menu = menu;
	theCommands [position]. title = title;
	theCommands [position]. depth = depth;
	theCommands [position]. callback = callback;   /* NULL for a separator or cascade button. */
	theCommands [position]. executable = callback != NULL;
	theCommands [position]. script = NULL;
	theCommands [position]. hidden = hidden;
	theCommands [position]. unhidable = unhidable;

	if (! praat.batch) {
		Widget parent = NULL;

		/* WHERE TO PUT IT?
		 * Determine parent menu widget.
		 * This is not going to fail:
		 * if 'depth' is inappropriate, the alleged subitem will be put in the top menu.
		 */
		if (depth == 0) {
			parent = windowMenuToWidget (window, menu);   /* Not a subitem: in the top menu. */
		} else {
			for (i = position - 1; i > 0; i --) {
				if (theCommands [i]. depth == depth - 1) {
					if (theCommands [i]. callback == NULL && theCommands [i]. title != NULL && theCommands [i]. title [0] != '-')   /* Cascade button? */
						XtVaGetValues (theCommands [i]. button, XmNsubMenuId, & parent, NULL);   /* The relevant menu title. */
					break;
				}
			}
			if (! parent) parent = windowMenuToWidget (window, menu);   /* Fallback: a subitem without a menu title. */
		}
if (! parent) return NULL;

		/* WHAT TO PUT THERE?
		 */

		if (title == NULL || title [0] == '-') {
			theCommands [position]. button = motif_addSeparator (parent);
		} else if (callback == NULL) {
			(void) motif_addMenu2 (parent, title, 0, & theCommands [position]. button);   /* Cascade button with submenu. */
		} else {
			theCommands [position]. button = motif_addItem (parent, title, motifFlags, cb_menu, (void *) callback);
		}
		if (hidden) XtUnmanageChild (theCommands [position]. button);
	}
	return theCommands [position]. button;
}

int praat_addMenuCommandScript (const char *window, const char *menu, const char *title,
	const char *after, int depth, const char *script)
{
	int i, position;

	Melder_assert (window && menu && title && after && script);
	if (strlen (script) && ! strlen (title))
		return Melder_error ("praat_addMenuCommand: command with script has no title. Window \"%s\", menu \"%s\".",
			window, menu);

	/* Determine the position of the new command.
	 */
	if (strlen (after)) {   /* Search for existing command with same selection. */
		int found = lookUpMatchingMenuCommand (window, menu, after);
		if (found) {
			position = found + 1;   /* After 'after'. */
		} else {
			/*return Melder_error ("praat_addMenuCommand: the command \"%s\" cannot be put after \"%s\",\n"
				"in the menu \"%s\" in the window \"%s\"\n"
				"because the latter command does not exist.", title, after, menu, window);*/
			position = theNumberOfCommands + 1;   /* Default: at end. */
		}
	} else {
		position = theNumberOfCommands + 1;   /* At end. */
	}

	/* Increment the command area.
	 */
	if (theNumberOfCommands >= praat_MAXNUM_FIXED_COMMANDS)
		return Melder_error ("praat_addMenuCommand: too many menu commands.");
	theNumberOfCommands += 1;

	/* Make room for insertion.
	 */
	for (i = theNumberOfCommands; i > position; i --) theCommands [i] = theCommands [i - 1];
	memset (& theCommands [position], 0, sizeof (struct structPraat_Command));

	/* Insert new command.
	 */
	theCommands [position]. window = Melder_strdup (window);
	theCommands [position]. menu = Melder_strdup (menu);
	theCommands [position]. title = strlen (title) ? Melder_strdup (title) : NULL;   /* Allow old-fashioned untitled separators. */
	theCommands [position]. depth = depth;
	theCommands [position]. callback = strlen (script) ? DO_RunTheScriptFromAnyAddedMenuCommand : NULL;   /* NULL for a separator or cascade button. */
	theCommands [position]. executable = strlen (script) != 0;
	if (strlen (script) == 0) {
		theCommands [position]. script = Melder_strdup ("");   /* Empty string, which will be needed to signal origin. */
	} else {
		structMelderFile file;
		Melder_relativePathToFile (script, & file);
		theCommands [position]. script = Melder_strdup (Melder_fileToPath (& file));
	}
	theCommands [position]. after = strlen (after) ? Melder_strdup (after) : NULL;
	if (praatP.phase >= praat_READING_BUTTONS) {
		static long uniqueID = 0;
		theCommands [position]. uniqueID = ++ uniqueID;
	}

	if (! praat.batch) {
		Widget parent = NULL;

		/* WHERE TO PUT IT?
		 * Determine parent menu widget.
		 * This is not going to fail:
		 * if 'depth' is inappropriate, the alleged subitem will be put in the top menu.
		 */
		if (depth == 0) {
			parent = windowMenuToWidget (window, menu);   /* Not a subitem: in the top menu. */
		} else {
			for (i = position - 1; i > 0; i --) {
				if (theCommands [i]. depth == depth - 1) {
					if (theCommands [i]. callback == NULL && theCommands [i]. title != NULL && theCommands [i]. title [0] != '-')   /* Cascade button? */
						XtVaGetValues (theCommands [i]. button, XmNsubMenuId, & parent, NULL);   /* The relevant menu title. */
					break;
				}
			}
			if (! parent) parent = windowMenuToWidget (window, menu);   /* Fallback: a subitem without a menu title. */
		}
		if (parent) {
			/* WHAT TO PUT THERE?
			 */
			if (title [0] == '\0' || title [0] == '-') {
				theCommands [position]. button = motif_addSeparator (parent);
			} else if (script [0] == '\0') {
				(void) motif_addMenu2 (parent, title, 0, & theCommands [position]. button);
			} else {
				theCommands [position]. button = motif_addItem (parent, title, 0, cb_menu, (void *) theCommands [position]. script);   /* Not just "script"!! */
			}
		}
	}

	if (praatP.phase >= praat_HANDLING_EVENTS) praat_sortMenuCommands ();
	return 1;
}

int praat_hideMenuCommand (const char *window, const char *menu, const char *title) {
	int found;
	praat_Command command;
	if (praat.batch || ! window || ! menu || ! title) return 1;
	found = lookUpMatchingMenuCommand (window, menu, title);
	if (! found) return 0;
	command = & theCommands [found];
	if (! command -> hidden && ! command -> unhidable) {
		command -> hidden = TRUE;
		if (praatP.phase >= praat_READING_BUTTONS) command -> toggled = ! command -> toggled;
		if (command -> button) XtUnmanageChild (command -> button);
	}
	return 1;
}

int praat_showMenuCommand (const char *window, const char *menu, const char *title) {
	int found;
	praat_Command command;
	if (praat.batch || ! window || ! menu || ! title) return 1;
	found = lookUpMatchingMenuCommand (window, menu, title);
	if (! found) return 0;
	command = & theCommands [found];
	if (command -> hidden) {
		command -> hidden = FALSE;
		if (praatP.phase >= praat_READING_BUTTONS) command -> toggled = ! command -> toggled;
		if (command -> button) XtManageChild (command -> button);
	}
	return 1;
}

void praat_saveMenuCommands (FILE *f) {
	long i, id, maxID = 0;
	for (i = 1; i <= theNumberOfCommands; i ++) if (theCommands [i]. uniqueID > maxID) maxID = theCommands [i]. uniqueID;
	for (id = 1; id <= maxID; id ++)   /* Sorted. */
		for (i = 1; i <= theNumberOfCommands; i ++) {
			praat_Command me = & theCommands [i];
			if (my uniqueID == id && ! my hidden && my window && my menu && my title) {
				fprintf (f, "Add menu command... \"%s\" \"%s\" \"%s\" \"%s\" %d %s\n",
					my window, my menu, my title, my after ? my after : "", my depth, my script ? my script : "");
				break;
			}
		}
	for (i = 1; i <= theNumberOfCommands; i ++) {
		praat_Command me = & theCommands [i];
		if (my toggled && my window && my menu && my title && ! my uniqueID && ! my script)
			fprintf (f, "%s menu command... \"%s\" \"%s\" %s\n",
				my hidden ? "Hide" : "Show", my window, my menu, my title);
	}
}

/***** FIXED BUTTONS *****/

void praat_addFixedButtonCommand (Widget parent, const char *title, int (*callback) (Any, void *),
	int fromLeft, int fromBottom)
{
	praat_Command me = & theCommands [++ theNumberOfCommands];
	my window = Melder_strdup ("Objects");
	my title = title;
	my callback = callback;
	my unhidable = TRUE;
	if (praat.batch)
		my button = NULL;
	else {
		Widget button = my button = XmCreatePushButton (parent, MOTIF_CONST_CHAR_ARG (title), 0, 0);
		XtVaSetValues (button, XmNbottomAttachment, XmATTACH_FORM,
			XmNbottomOffset, fromBottom, XmNx, fromLeft, NULL);
		XtSetSensitive (button, False);
		XtAddCallback (button, XmNactivateCallback, cb_menu, (XtPointer) callback);
		XtManageChild (button);
	}
	my executable = False;
}

void praat_sensitivizeFixedButtonCommand (const char *title, int sensitive) {
	int i;
	for (i = 1; i <= theNumberOfCommands; i ++)
		if (strequ (theCommands [i]. title, title)) break;   /* Search. */
	theCommands [i]. executable = sensitive;
	if (! Melder_backgrounding) XtSetSensitive (theCommands [i]. button, sensitive);
}

int praat_doMenuCommand (const char *command, const char *arguments) {
	long i = 1;
	while (i <= theNumberOfCommands && (! theCommands [i]. executable || ! strequ (theCommands [i]. title, command) ||
		(! strequ (theCommands [i]. window, "Objects") && ! strequ (theCommands [i]. window, "Picture")))) i ++;
	if (i > theNumberOfCommands) return 0;
	if (! theCommands [i]. callback ((Any) arguments, NULL))
		return 0;
	return 1;
}

long praat_getNumberOfMenuCommands (void) { return theNumberOfCommands; }

praat_Command praat_getMenuCommand (long i)
	{ return i < 1 || i > theNumberOfCommands ? NULL : & theCommands [i]; }

void praat_addCommandsToEditor (Editor me) {
	const char *windowName = our _className;
	long i;
	for (i = 1; i <= theNumberOfCommands; i ++) if (strequ (theCommands [i]. window, windowName)) {
		if (! Editor_addCommandScript (me, theCommands [i]. menu, theCommands [i]. title, 0, theCommands [i]. script))
			Melder_flushError ("To fix this, go to Praat:Preferences:Buttons:Editors, "
				"and remove the script from this menu.\n"
				"You may want to install the script in a different menu.");
	}
}

/* End of file praat_menuCommands.c */
