/* praat_actions.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2004/04/01 second-level menus
 */

#include "praatP.h"
#include "praat_script.h"
#include "longchar.h"
#include "machine.h"

#if defined (_WIN32)
	#define BUTTON_WIDTH  190
#elif defined (macintosh)
	#define BUTTON_WIDTH  190
#endif

#define praat_MAXNUM_LOOSE_COMMANDS  5000
static long theNumberOfActions = 0;
static struct structPraat_Command *theActions;
static Widget praat_writeMenuTitle, praat_writeMenu, praat_writeMenuSeparator;
static Widget praat_dynamicMenu, praat_dynamicMenuWindow;

static void fixSelectionSpecification (void **class1, int *n1, void **class2, int *n2, void **class3, int *n3) {
/*
 * Function:
 *	sort the specification pairs *class(i), *n(i) according to class name, with NULL classes at the end.
 * Postconditions:
 *	if (*class2 != NULL) *class1 != NULL;
 *	if (*class3 != NULL) *class2 != NULL;
 *	(*class1) -> _className <= (*class2) -> _className <= (*class3) -> _className;
 * Usage:
 *	Called by praat_addAction () and praat_removeAction ().
 */
	void *helpClass;
	int helpN;

	/* Fix unusual input bubblewise. */

	if (*class1 == NULL && *class2 != NULL) { *class1 = *class2; *n1 = *n2; *class2 = NULL; *n2 = 0; }
	if (*class2 == NULL && *class3 != NULL) { *class2 = *class3; *n2 = *n3; *class3 = NULL; *n3 = 0;
		if (*class1 == NULL && *class2 != NULL) { *class1 = *class2; *n1 = *n2; *class2 = NULL; *n2 = 0; } }

	/* Now: if *class3, then *class2, and if *class2, then *class1.
	 * Bubble-sort the input by class name.
	 */
	if (*class2 && strcmp (((Data_Table) *class1) -> _className, ((Data_Table) *class2) -> _className) > 0) {
		helpClass = *class1; *class1 = *class2; *class2 = helpClass;
		helpN = *n1; *n1 = *n2; *n2 = helpN;
	}
	if (*class3 && strcmp (((Data_Table) *class2) -> _className, ((Data_Table) *class3) -> _className) > 0) {
		helpClass = *class2; *class2 = *class3; *class3 = helpClass;
		helpN = *n2; *n2 = *n3; *n3 = helpN;
		if (strcmp (((Data_Table) *class1) -> _className, ((Data_Table) *class2) -> _className) > 0) {
			helpClass = *class1; *class1 = *class2; *class2 = helpClass;
			helpN = *n1; *n1 = *n2; *n2 = helpN;
		}
	}
}

static int lookUpMatchingAction (void *class1, void *class2, void *class3, void *class4, const char *title) {
/*
 * An action command is fully specified by its environment (the selected classes) and its title.
 * Precondition:
 *	class1, class2, and class3 must be in sorted order.
 */
	int i;
	for (i = 1; i <= theNumberOfActions; i ++)
		if (class1 == theActions [i]. class1 && class2 == theActions [i]. class2 &&
		    class3 == theActions [i]. class3 && class4 == theActions [i]. class4 &&
		    title && theActions [i]. title && strequ (theActions [i]. title, title)) return i;
	return 0;   /* Not found. */
}

void praat_addAction (void *class1, int n1, void *class2, int n2, void *class3, int n3,
	const char *title, const char *after, unsigned long flags, int (*callback) (Any, void *))
{ praat_addAction4 (class1, n1, class2, n2, class3, n3, NULL, 0, title, after, flags, callback); }

void praat_addAction1 (void *class1, int n1,
	const char *title, const char *after, unsigned long flags, int (*callback) (Any, void *))
{ praat_addAction4 (class1, n1, NULL, 0, NULL, 0, NULL, 0, title, after, flags, callback); }

void praat_addAction2 (void *class1, int n1, void *class2, int n2,
	const char *title, const char *after, unsigned long flags, int (*callback) (Any, void *))
{ praat_addAction4 (class1, n1, class2, n2, NULL, 0, NULL, 0, title, after, flags, callback); }

void praat_addAction3 (void *class1, int n1, void *class2, int n2, void *class3, int n3,
	const char *title, const char *after, unsigned long flags, int (*callback) (Any, void *))
{ praat_addAction4 (class1, n1, class2, n2, class3, n3, NULL, 0, title, after, flags, callback); }

void praat_addAction4 (void *class1, int n1, void *class2, int n2, void *class3, int n3, void *class4, int n4,
	const char *title, const char *after, unsigned long flags, int (*callback) (Any, void *))
{
	int i, position;
	int depth = flags, unhidable = FALSE, hidden = FALSE, key = 0;
	unsigned long motifFlags = 0;
	if (flags > 7) {
		depth = ((flags & 0x00070000) >> 16);
		unhidable = (flags & praat_UNHIDABLE) != 0;
		hidden = (flags & praat_HIDDEN) != 0 && ! unhidable;
		key = flags & 0x000000FF;
		motifFlags = key ? flags & 0x002007FF : flags & 0x00000700;
	}
	fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);

	if (callback && ! title) {
		Melder_flushError ("praat_addAction: command with callback has no title. Classes: %s %s %s.",
			class1 ? ((Data_Table) class1) -> _className : "",
			class2 ? ((Data_Table) class2) -> _className : "",
			class3 ? ((Data_Table) class3) -> _className : "",
			class4 ? ((Data_Table) class4) -> _className : "");
		return;
	}

	if (! class1) {
		Melder_flushError ("praat_addAction: command \"%s\" has no first class.", title);
		return;
	}

	/* Determine the position of the new command.
	 */
	if (after) {   /* Search for existing command with same selection. */
		int found = lookUpMatchingAction (class1, class2, class3, class4, after);
		if (found) {
			position = found + 1;   /* After 'after'. */
		} else {
			Melder_flushError ("praat_addAction: the command \"%s\" cannot be put after \"%s\",\n"
				"because the latter command does not exist.", title, after);
			return;
		}
	} else {
		position = theNumberOfActions + 1;   /* At end. */
	}

	/* Increment the command area.
	 */
	if (theNumberOfActions >= praat_MAXNUM_LOOSE_COMMANDS)
		Melder_flushError ("praat_addAction: too many loose commands.");
	theNumberOfActions += 1;

	/* Make room for insertion.
	 */
	for (i = theNumberOfActions; i > position; i --) theActions [i] = theActions [i - 1];
	memset (& theActions [position], 0, sizeof (struct structPraat_Command));

	/* Insert new command.
	 */
	theActions [position]. class1 = class1;
	theActions [position]. n1 = n1;
	theActions [position]. class2 = class2;
	theActions [position]. n2 = n2;
	theActions [position]. class3 = class3;
	theActions [position]. n3 = n3;
	theActions [position]. class4 = class4;
	theActions [position]. n4 = n4;
	theActions [position]. title = title;
	theActions [position]. depth = depth;
	theActions [position]. callback = callback;   /* NULL for a separator. */
	theActions [position]. button = NULL;
	theActions [position]. script = NULL;
	theActions [position]. hidden = hidden;
	theActions [position]. unhidable = unhidable;
}

static void deleteDynamicMenu (void) {
	if (praatP.phase != praat_HANDLING_EVENTS) return;
	if (praat_dynamicMenu) {
		int i;
		XtDestroyWidget (praat_dynamicMenu);
		praat_dynamicMenu = NULL;
		for (i = 1; i <= theNumberOfActions; i ++)
			theActions [i]. button = NULL;
	}
	if (praat_writeMenu) {
		XtDestroyWidget (praat_writeMenu);
		praat_writeMenuSeparator = NULL;
		praat_writeMenu = XmCreatePulldownMenu (praatP.menuBar, "Write", NULL, 0);
		XtVaSetValues (praat_writeMenuTitle, XmNsubMenuId, praat_writeMenu, NULL);
	}
}

static void updateDynamicMenu (void) {
	if (praatP.phase != praat_HANDLING_EVENTS) return;
	praat_sortActions ();
	deleteDynamicMenu ();
	praat_show ();
}

int praat_addActionScript (const char *className1, int n1, const char *className2, int n2, const char *className3, int n3,
	const char *title, const char *after, int depth, const char *script)
{
	int i, position, found;
	void *class1 = NULL, *class2 = NULL, *class3 = NULL;
	Melder_assert (className1 && className2 && className3 && title && after && script);
	if (strlen (className1) && ! (class1 = Thing_classFromClassName (className1))) return 0;
	if (strlen (className2) && ! (class2 = Thing_classFromClassName (className2))) return 0;
	if (strlen (className3) && ! (class3 = Thing_classFromClassName (className3))) return 0;
	fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);

	if (strlen (script) && ! strlen (title))
		return Melder_error ("praat_addActionScript: command with callback has no title. Classes: %s %s %s.",
			className1, className2, className3);

	if (! strlen (className1))
		return Melder_error ("praat_addActionScript: command \"%s\" has no first class.", title);

	/*
	 * If the button already exists, remove it.
	 */
	found = lookUpMatchingAction (class1, class2, class3, NULL, title);
	if (found) {
		theNumberOfActions --;
		for (i = found; i <= theNumberOfActions; i ++) theActions [i] = theActions [i + 1];
	}

	/* Determine the position of the new command.
	 */
	if (strlen (after)) {   /* Search for existing command with same selection. */
		int found = lookUpMatchingAction (class1, class2, class3, NULL, after);
		if (found) {
			position = found + 1;   /* After 'after'. */
		} else {
			position = theNumberOfActions + 1;   /* At end. */
		}
	} else {
		position = theNumberOfActions + 1;   /* At end. */
	}

	/* Increment the command area.
	 */
	if (theNumberOfActions >= praat_MAXNUM_LOOSE_COMMANDS)
		return Melder_error ("praat_addActionScript: too many loose commands.");
	theNumberOfActions += 1;

	/* Make room for insertion.
	 */
	for (i = theNumberOfActions; i > position; i --) theActions [i] = theActions [i - 1];
	memset (& theActions [position], 0, sizeof (struct structPraat_Command));

	/* Insert new command.
	 */
	theActions [position]. class1 = class1;
	theActions [position]. n1 = n1;
	theActions [position]. class2 = class2;
	theActions [position]. n2 = n2;
	theActions [position]. class3 = class3;
	theActions [position]. n3 = n3;
	theActions [position]. title = strlen (title) ? Melder_strdup (title) : NULL;   /* Allow old-fashioned untitled separators. */
	theActions [position]. depth = depth;
	theActions [position]. callback = strlen (script) ? DO_RunTheScriptFromAnyAddedMenuCommand : NULL;   /* NULL for a separator. */
	theActions [position]. button = NULL;
	theActions [position]. script = strlen (script) ? Melder_strdup (script) : NULL;
	if (strlen (script) == 0) {
		theActions [position]. script = NULL;
	} else {
		structMelderFile file;
		Melder_relativePathToFile (script, & file);
		theActions [position]. script = Melder_strdup (Melder_fileToPath (& file));
	}
	theActions [position]. after = strlen (after) ? Melder_strdup (after) : NULL;
	theActions [position]. phase = praatP.phase;
	if (praatP.phase >= praat_READING_BUTTONS) {
		static long uniqueID = 0;
		theActions [position]. uniqueID = ++ uniqueID;
	}
	updateDynamicMenu ();
	return 1;
}

int praat_removeAction (void *class1, void *class2, void *class3, const char *title) {
	int n1, n2, n3, found, i;
	fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
	found = lookUpMatchingAction (class1, class2, class3, NULL, title);
	if (! found) {
		char actionString [200];
		sprintf (actionString, "%s", ((Data_Table) class1) -> _className);
		if (class2) sprintf (actionString + strlen (actionString), " & %s", ((Data_Table) class2) -> _className);
		if (class3) sprintf (actionString + strlen (actionString), " & %s", ((Data_Table) class3) -> _className);
		sprintf (actionString + strlen (actionString), ": %s", title ? title : "");
		return Melder_error ("(praat_removeAction:) Action command \"%s\" not found.", actionString);
	}
	theNumberOfActions --;
	for (i = found; i <= theNumberOfActions; i ++) theActions [i] = theActions [i + 1];
	return 1;
}

int praat_removeAction_classNames (const char *className1, const char *className2,
	const char *className3, const char *title)
{
	void *class1 = NULL, *class2 = NULL, *class3 = NULL;
	Melder_assert (className1 && className2 && className3 && title);
	if (strlen (className1) && ! (class1 = Thing_classFromClassName (className1))) return 0;
	if (strlen (className2) && ! (class2 = Thing_classFromClassName (className2))) return 0;
	if (strlen (className3) && ! (class3 = Thing_classFromClassName (className3))) return 0;
	if (! praat_removeAction (class1, class2, class3, title)) return 0;
	updateDynamicMenu ();
	return 1;
}

int praat_hideAction (void *class1, void *class2, void *class3, const char *title) {
	int n1, n2, n3, found;
	fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
	found = lookUpMatchingAction (class1, class2, class3, NULL, title);
	if (! found) {
		char actionString [200];
		sprintf (actionString, "%s", ((Data_Table) class1) -> _className);
		if (class2) sprintf (actionString + strlen (actionString), " & %s", ((Data_Table) class2) -> _className);
		if (class3) sprintf (actionString + strlen (actionString), " & %s", ((Data_Table) class3) -> _className);
		sprintf (actionString + strlen (actionString), ": %s", title ? title : "");
		return Melder_error ("(praat_hideAction:) Action command \"%s\" not found.", actionString);
	}
	if (! theActions [found]. hidden) {
		theActions [found]. hidden = TRUE;
		if (praatP.phase >= praat_READING_BUTTONS) theActions [found]. toggled = ! theActions [found]. toggled;
		updateDynamicMenu ();
	}
	return 1;
}

int praat_hideAction_classNames (const char *className1, const char *className2,
	const char *className3, const char *title)
{
	void *class1 = NULL, *class2 = NULL, *class3 = NULL;
	Melder_assert (className1 && className2 && className3 && title);
	if (strlen (className1) && ! (class1 = Thing_classFromClassName (className1))) return 0;
	if (strlen (className2) && ! (class2 = Thing_classFromClassName (className2))) return 0;
	if (strlen (className3) && ! (class3 = Thing_classFromClassName (className3))) return 0;
	if (! praat_hideAction (class1, class2, class3, title)) return 0;
	return 1;
}

int praat_showAction (void *class1, void *class2, void *class3, const char *title) {
	int n1, n2, n3, found;
	fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
	found = lookUpMatchingAction (class1, class2, class3, NULL, title);
	if (! found) {
		char actionString [200];
		sprintf (actionString, "%s", ((Data_Table) class1) -> _className);
		if (class2) sprintf (actionString + strlen (actionString), " & %s", ((Data_Table) class2) -> _className);
		if (class3) sprintf (actionString + strlen (actionString), " & %s", ((Data_Table) class3) -> _className);
		sprintf (actionString + strlen (actionString), ": %s", title ? title : "");
		return Melder_error ("(praat_showAction:) Action command \"%s\" not found.", actionString);
	}
	if (theActions [found]. hidden) {
		theActions [found]. hidden = FALSE;
		if (praatP.phase >= praat_READING_BUTTONS) theActions [found]. toggled = ! theActions [found]. toggled;
		updateDynamicMenu ();
	}
	return 1;
}

int praat_showAction_classNames (const char *className1, const char *className2,
	const char *className3, const char *title)
{
	void *class1 = NULL, *class2 = NULL, *class3 = NULL;
	Melder_assert (className1 && className2 && className3 && title);
	if (strlen (className1) && ! (class1 = Thing_classFromClassName (className1))) return 0;
	if (strlen (className2) && ! (class2 = Thing_classFromClassName (className2))) return 0;
	if (strlen (className3) && ! (class3 = Thing_classFromClassName (className3))) return 0;
	if (! praat_showAction (class1, class2, class3, title)) return 0;
	return 1;
}

static int compareActions (const void *void_me, const void *void_thee) {
	praat_Command me = (praat_Command) void_me, thee = (praat_Command) void_thee;
	int compare;
	compare = strcmp (((Data_Table) my class1) -> _className, ((Data_Table) thy class1) -> _className);
	if (compare) return my class1 == classData ? -1 : thy class1 == classData ? 1 : compare;
	if (my class2) {
		if (! thy class2) return 1;
		compare = strcmp (((Data_Table) my class2) -> _className, ((Data_Table) thy class2) -> _className);
		if (compare) return compare;
	} else if (thy class2) return -1;
	if (my class3) {
		if (! thy class3) return 1;
		compare = strcmp (((Data_Table) my class3) -> _className, ((Data_Table) thy class3) -> _className);
		if (compare) return compare;
	} else if (thy class3) return -1;
	if (my sortingTail < thy sortingTail) return -1;
	return 1;
}

void praat_sortActions (void) {
	long i;
	for (i = 1; i <= theNumberOfActions; i ++)
		theActions [i]. sortingTail = i;
	qsort (& theActions [1], theNumberOfActions, sizeof (struct structPraat_Command), compareActions);
}

static const char *numberString (int number) {
	return number == 1 ? "one" : number == 2 ? "two" : number == 3 ? "three" : "any number of";
}
static const char *classString (void *klas) {
	return klas == classData ? "" : ((Data_Table) klas) -> _className;
}
static const char *objectString (int number) {
	return number == 1 ? "object" : "objects";
}
static int allowExecutionHook (void *closure) {
	int (*callback) (Any, void *) = (int (*) (Any, void *)) closure;
	long i, numberOfMatchingCallbacks = 0, firstMatchingCallback = 0;
	for (i = 1; i <= theNumberOfActions; i ++) {
		praat_Command me = & theActions [i];
		if (my callback == callback) {
			int sel1, sel2 = 0, sel3 = 0, sel4 = 0;
			if (! my class1) return Melder_error ("No class1???");
			numberOfMatchingCallbacks += 1;
			if (! firstMatchingCallback) firstMatchingCallback = i;
			sel1 = my class1 == classData ? praat.totalSelection : praat_selection (my class1);
			if (sel1 == 0) continue;
			if (my class2 && (sel2 = praat_selection (my class2)) == 0) continue;
			if (my class3 && (sel3 = praat_selection (my class3)) == 0) continue;
			if (my class4 && (sel4 = praat_selection (my class4)) == 0) continue;
			if (sel1 + sel2 + sel3 + sel4 != praat.totalSelection) continue;
			if ((my n1 && sel1 != my n1) || (my n2 && sel2 != my n2) || (my n3 && sel3 != my n3) || (my n4 && sel4 != my n4)) continue;
			return TRUE;   /* Found a matching action. */
		}
	}
	if (numberOfMatchingCallbacks == 1) {
		praat_Command me = & theActions [firstMatchingCallback];
		Melder_error ("Selection changed! It should be:");
		if (my class1) Melder_error ("   %s %s %s", numberString (my n1), classString (my class1), objectString (my n1));
		if (my class2) Melder_error ("   %s %s %s", numberString (my n2), classString (my class2), objectString (my n2));
		if (my class3) Melder_error ("   %s %s %s", numberString (my n3), classString (my class3), objectString (my n3));
		if (my class4) Melder_error ("   %s %s %s", numberString (my n4), classString (my class4), objectString (my n4));
	} else {
		Melder_error ("Selection changed!");
	}
	return FALSE;
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
	for (i = 1; i <= theNumberOfActions; i ++) {
		praat_Command me = & theActions [i];
		if (my callback == callback) {
			if (my title) UiHistory_write ("\n%s", my title);
			Ui_setAllowExecutionHook (allowExecutionHook, callback);
			if (! callback (NULL, (XtPointer) modified))
				Melder_flushError ("Command not executed.");
			Ui_setAllowExecutionHook (NULL, NULL);
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

void praat_actions_show (void) {
	long i;
	Widget buttons [1000], writeButtons [50];

	/* The selection has changed;
	 * kill the dynamic menu and the write menu.
	 */
	#if defined (macintosh) || defined (_WIN32)
		deleteDynamicMenu ();
	#endif
	if (! Melder_backgrounding) {
		int nbuttons = 0, nwriteButtons = 0;
		XtSetSensitive (praat_writeMenuTitle, False);
		if (praat_writeMenuSeparator) XtUnmanageChild (praat_writeMenuSeparator);
		if (praat_dynamicMenu) XtUnmanageChild (praat_dynamicMenu);
		for (i = theNumberOfActions; i >= 1; i --) {
			praat_Command me = & theActions [i];
			if (! my visible) continue;
			if (my button) {
				if (XtParent (my button) == praat_dynamicMenu)   /* Unmanage only level-1 visible buttons. */
					buttons [nbuttons ++] = my button;
				else if (my title && (strnequ (my title, "Write ", 6) || strnequ (my title, "Append to ", 10)))
					writeButtons [nwriteButtons ++] = my button;
			}
			if (nbuttons) XtUnmanageChildren (buttons, nbuttons);
			if (nwriteButtons) XtUnmanageChildren (writeButtons, nwriteButtons);
		}
		/*
		 * BUG: Despite all these precautions,
		 * creating and removing a lot of objects from a script
		 * still leaves several seconds of flashing after the script finishes.
		 * Events remaining?
		 */
	}

	/* Determine the visibility and sensitivity of all the actions.
	 */
	if (praat.totalSelection != 0 && ! Melder_backgrounding)
		XtSetSensitive (praat_writeMenuTitle, True);
	for (i = 1; i <= theNumberOfActions; i ++) {
		int sel1 = 0, sel2 = 0, sel3 = 0, sel4 = 0;
		int n1 = theActions [i]. n1, n2 = theActions [i]. n2, n3 = theActions [i]. n3, n4 = theActions [i]. n4;

		/* Clean up from previous selection. */

		theActions [i]. visible = FALSE;
		theActions [i]. executable = FALSE;

		/* Match the actually selected classes with the selection required for this visibility. */

		if (! theActions [i]. class1) continue;   /* At least one class selected. */
		sel1 = theActions [i]. class1 == classData ? praat.totalSelection : praat_selection (theActions [i]. class1);
		if (sel1 == 0) continue;
		if (theActions [i]. class2 && (sel2 = praat_selection (theActions [i]. class2)) == 0) continue;
		if (theActions [i]. class3 && (sel3 = praat_selection (theActions [i]. class3)) == 0) continue;
		if (theActions [i]. class4 && (sel4 = praat_selection (theActions [i]. class4)) == 0) continue;
		if (sel1 + sel2 + sel3 + sel4 != praat.totalSelection) continue;   /* Other classes selected? Do not show. */
		theActions [i]. visible = ! theActions [i]. hidden;

		/* Match the actually selected objects with the selection required for this action. */

		if (! theActions [i]. callback) continue;   /* Separators are not executable. */
		if ((n1 && sel1 != n1) || (n2 && sel2 != n2) || (n3 && sel3 != n3) || (n4 && sel4 != n4)) continue;
		theActions [i]. executable = TRUE;
	}

	/* Create a new column of buttons in the dynamic menu. */
	if (! Melder_backgrounding) {
		Widget currentSubmenu1 = NULL, currentSubmenu2 = NULL;
		int writeMenuGoingToSeparate = FALSE;
		int nbuttons = 0, nwriteButtons = 0;
		if (! praat_dynamicMenu)
			praat_dynamicMenu = XmCreateRowColumn (praat_dynamicMenuWindow, "menu", NULL, 0);
		for (i = 1; i <= theNumberOfActions; i ++) {   /* Add buttons or make existing buttons sensitive (executable). */
			praat_Command me = & theActions [i];
			if (my depth == 0) currentSubmenu1 = NULL, currentSubmenu2 = NULL;   /* Prevent attachment of later deep actions to earlier submenus after removal of label. */
			if (my depth == 1) currentSubmenu2 = NULL;   /* Prevent attachment of later deep actions to earlier submenus after removal of label. */
			if (! my visible) continue;
			if (my callback) {
				/* Apparently a true command: create a button in the dynamic menu.
				 * If it is a subcommand (depth > 0), put it in the current submenu,
				 * but only if this exists (umbrella against stray submenu specifications).
				 */
				if (! my button) {
					Widget parent = my depth > 1 && currentSubmenu2 ? currentSubmenu2 : my depth > 0 && currentSubmenu1 ? currentSubmenu1 : praat_dynamicMenu;
					if (strnequ (my title, "Write ", 6) || strnequ (my title, "Append to ", 10)) {
						parent = praat_writeMenu;
						if (! praat_writeMenuSeparator) {
							if (writeMenuGoingToSeparate)
								praat_writeMenuSeparator = motif_addSeparator (parent);
							else if (strequ (my title, "Write to binary file..."))
								writeMenuGoingToSeparate = TRUE;
						}
					}

					/* Create a new push-button widget.
					 * Unfortunately, we cannot use motif_addItem, which would create a gadget.
					 */
					Longchar_nativize (my title, Melder_buffer1, FALSE);
					my button = XmCreatePushButton (parent, Melder_buffer1, NULL, 0);
					#if defined (_WIN32)
						XtVaSetValues (my button, XmNheight, 19, XmNwidth, BUTTON_WIDTH - 20, NULL);
					#elif defined (macintosh)
						#if TARGET_API_MAC_CARBON
							XtVaSetValues (my button, XmNx, 5, XmNheight, 18, XmNwidth, BUTTON_WIDTH - 25, NULL);
						#else
							XtVaSetValues (my button, XmNheight, 18, XmNwidth, BUTTON_WIDTH - 20, NULL);
						#endif
					#endif
					if (my callback == DO_RunTheScriptFromAnyAddedMenuCommand)
						XtAddCallback (my button, XmNactivateCallback, cb_menu, (void *) my script);
					else
						XtAddCallback (my button, XmNactivateCallback, cb_menu, (void *) my callback);
					if (! my executable)
						XtSetSensitive (my button, False);
					XtManageChild (my button);
				} else if (strnequ (my title, "Write ", 6) || strnequ (my title, "Append to ", 10)) {
					if (writeMenuGoingToSeparate) {
						if (! praat_writeMenuSeparator)
							praat_writeMenuSeparator = motif_addSeparator (praat_writeMenu);
						XtManageChild (praat_writeMenuSeparator);
					} else if (strequ (my title, "Write to binary file...")) {
						writeMenuGoingToSeparate = TRUE;
					}
					XtSetSensitive (writeButtons [nwriteButtons++] = my button, my executable);
				} else {
					XtSetSensitive (my button, my executable);
					if (XtParent (my button) == praat_dynamicMenu) buttons [nbuttons++] = my button;
				}
			} else if (i == theNumberOfActions || theActions [i + 1]. depth == 0) {
				/*
				 * Apparently a labelled separator.
				 */
				if (! my button) {
					my button = XmCreateLabelGadget (praat_dynamicMenu, MOTIF_CONST_CHAR_ARG (my title), NULL, 0);
					#if defined (_WIN32)
						XtVaSetValues (my button, XmNheight, 19, NULL);
					#elif defined (macintosh)
						XtVaSetValues (my button, XmNheight, 15, NULL);
					#endif
					XtManageChild (my button);
				} else {
					if (XtParent (my button) == praat_dynamicMenu) buttons [nbuttons++] = my button;
				}
			} else if (my title == NULL || my title [0] == '-') {
				/*
				 * Apparently a separator in a submenu.
				 */
				if (currentSubmenu2 || currentSubmenu1) {   /* These separators are not shown in a flattened menu. */
					if (! my button) {
						my button = XmCreateSeparator (currentSubmenu2 ? currentSubmenu2 : currentSubmenu1, "separator", NULL, 0);
						XtManageChild (my button);
					}
				}
			} else {
				/*
				 * Apparently a submenu.
				 */
				if (! my button) {
					Widget cascadeButton;
					if (my depth == 0) {
						my button = XmCreateMenuBar (praat_dynamicMenu, "dynamicSubmenuBar", 0, 0);
						currentSubmenu1 = motif_addMenu2 (my button, my title, 0, & cascadeButton);
						#if defined (_WIN32)
							XtVaSetValues (cascadeButton, XmNheight, 19, XmNwidth, BUTTON_WIDTH - 24, NULL);
							XtVaSetValues (my button, XmNheight, 21, XmNwidth, BUTTON_WIDTH - 20, NULL);
						#elif defined (macintosh)
							#if TARGET_API_MAC_CARBON
								XtVaSetValues (cascadeButton, XmNheight, 19, XmNwidth, BUTTON_WIDTH - 29, NULL);
								XtVaSetValues (my button, XmNheight, 22, XmNwidth, BUTTON_WIDTH - 25, NULL);
							#else
								XtVaSetValues (cascadeButton, XmNheight, 19, XmNwidth, BUTTON_WIDTH - 24, NULL);
								XtVaSetValues (my button, XmNheight, 22, XmNwidth, BUTTON_WIDTH - 20, NULL);
							#endif
						#endif
					} else {
						currentSubmenu2 = motif_addMenu2 (currentSubmenu1 ? currentSubmenu1 : praat_dynamicMenu, my title, 0, & my button);
					}
					XtManageChild (my button);
				} else {
					if (XtParent (my button) == praat_dynamicMenu) buttons [nbuttons++] = my button;
				}
			}
		}
		if (nbuttons) XtManageChildren (buttons, nbuttons);
		if (nwriteButtons) XtManageChildren (writeButtons, nwriteButtons);
		XtManageChild (praat_dynamicMenu);
	}
}

void praat_actions_createWriteMenu (Widget bar) {
	if (praat.batch) return;
	praat_writeMenuTitle = XtVaCreateManagedWidget ("Write", xmCascadeButtonWidgetClass, bar, NULL);
	XtSetSensitive (praat_writeMenuTitle, False);
	praat_writeMenu = XmCreatePulldownMenu (bar, "Write", NULL, 0);
	XtVaSetValues (praat_writeMenuTitle, XmNsubMenuId, praat_writeMenu, NULL);
}

void praat_actions_init (void) {
	theActions = Melder_calloc (praat_MAXNUM_LOOSE_COMMANDS + 1, sizeof (struct structPraat_Command));
}

void praat_actions_createDynamicMenu (Widget form, int leftOffset) {
	if (praat.batch) return;
	praat_dynamicMenuWindow = XmCreateScrolledWindow (form, "menuWindow", NULL, 0);
	#ifdef macintosh
		XtVaSetValues (praat_dynamicMenuWindow,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMainWindowMenuBarHeight (),
			XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, -1,
			XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, -1,
			XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, leftOffset,
			NULL);
	#else
		XtVaSetValues (praat_dynamicMenuWindow,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
			XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, -3,
			XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, -3,
			XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, leftOffset,
			NULL);
	#endif
	praat_dynamicMenu = XmCreateRowColumn (praat_dynamicMenuWindow, "menu", NULL, 0);
	XtManageChild (praat_dynamicMenu);
	XtManageChild (praat_dynamicMenuWindow);
}

void praat_saveAddedActions (FILE *f) {
	long i, id, maxID = 0;
	for (i = 1; i <= theNumberOfActions; i ++) if (theActions [i]. uniqueID > maxID) maxID = theActions [i]. uniqueID;
	for (id = 1; id <= maxID; id ++)
		for (i = 1; i <= theNumberOfActions; i ++) {
			praat_Command me = & theActions [i];
			if (my uniqueID == id && ! my hidden && my title) {
				fprintf (f, "Add action command... %s %d %s %d %s %d \"%s\" \"%s\" %d %s\n",
					((Data_Table) my class1) -> _className, my n1,
					my class2 ? ((Data_Table) my class2) -> _className : "\"\"", my n2,
					my class3 ? ((Data_Table) my class3) -> _className : "\"\"", my n3,
					my title, my after ? my after : "", my depth, my script ? my script : "");
				break;
			}
		}
	for (i = 1; i <= theNumberOfActions; i ++) {
		praat_Command me = & theActions [i];
		if (my toggled && my title && ! my uniqueID && ! my script)
			fprintf (f, "%s action command... %s %s %s %s\n",
				my hidden ? "Hide" : "Show",
				((Data_Table) my class1) -> _className,
				my class2 ? ((Data_Table) my class2) -> _className : "\"\"",
				my class3 ? ((Data_Table) my class3) -> _className : "\"\"",
				my title);
	}
}

int praat_doAction (const char *command, const char *arguments) {
	long i = 1;
	while (i <= theNumberOfActions && (! theActions [i]. executable || strcmp (theActions [i]. title, command))) i ++;
	if (i > theNumberOfActions) return 0;   /* Not found. */
	if (! theActions [i]. callback ((char *) arguments, NULL))
		return 0;
	return 1;
}

long praat_getNumberOfActions (void) { return theNumberOfActions; }

praat_Command praat_getAction (long i)
	{ return i < 0 || i > theNumberOfActions ? NULL : & theActions [i]; }

void praat_background (void) {
	if (Melder_backgrounding) return;
	deleteDynamicMenu ();
	praat_list_background ();
	Melder_backgrounding = TRUE;
	if (! praatP.dontUsePictureWindow) praat_picture_background ();
}

void praat_foreground (void) {
	if (! Melder_backgrounding) return;
	Melder_backgrounding = FALSE;
	praat_list_foreground ();
	praat_show ();
	if (! praatP.dontUsePictureWindow) praat_picture_foreground ();
}

/* End of file praat_actions.c */
