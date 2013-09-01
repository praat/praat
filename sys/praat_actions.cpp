/* praat_actions.cpp
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
#include "longchar.h"
#include "machine.h"
#include "GuiP.h"

#define BUTTON_LEFT  -240
#define BUTTON_RIGHT -5

#define praat_MAXNUM_LOOSE_COMMANDS  10000
static long theNumberOfActions = 0;
static struct structPraat_Command *theActions;
static GuiMenu praat_writeMenu;
static GuiMenuItem praat_writeMenuSeparator;
static GuiForm praat_form;
static bool actionsInvisible = false;

static void fixSelectionSpecification (ClassInfo *class1, int *n1, ClassInfo *class2, int *n2, ClassInfo *class3, int *n3) {
/*
 * Function:
 *	sort the specification pairs *class(i), *n(i) according to class name, with NULL classes at the end.
 * Postconditions:
 *	if (*class2 != NULL) *class1 != NULL;
 *	if (*class3 != NULL) *class2 != NULL;
 *	(*class1) -> className <= (*class2) -> className <= (*class3) -> className;
 * Usage:
 *	Called by praat_addAction () and praat_removeAction ().
 */

	/* Fix unusual input bubblewise. */

	if (*class1 == NULL && *class2 != NULL) { *class1 = *class2; *n1 = *n2; *class2 = NULL; *n2 = 0; }
	if (*class2 == NULL && *class3 != NULL) { *class2 = *class3; *n2 = *n3; *class3 = NULL; *n3 = 0;
		if (*class1 == NULL && *class2 != NULL) { *class1 = *class2; *n1 = *n2; *class2 = NULL; *n2 = 0; } }

	/* Now: if *class3, then *class2, and if *class2, then *class1.
	 * Bubble-sort the input by class name.
	 */
	if (*class2 && wcscmp ((*class1) -> className, (*class2) -> className) > 0) {
		ClassInfo helpClass = *class1; *class1 = *class2; *class2 = helpClass;
		int helpN = *n1; *n1 = *n2; *n2 = helpN;
	}
	if (*class3 && wcscmp ((*class2) -> className, (*class3) -> className) > 0) {
		ClassInfo helpClass = *class2; *class2 = *class3; *class3 = helpClass;
		int helpN = *n2; *n2 = *n3; *n3 = helpN;
		if (wcscmp ((*class1) -> className, (*class2) -> className) > 0) {
			ClassInfo helpClass = *class1; *class1 = *class2; *class2 = helpClass;
			int helpN = *n1; *n1 = *n2; *n2 = helpN;
		}
	}
}

static long lookUpMatchingAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, ClassInfo class4, const wchar_t *title) {
/*
 * An action command is fully specified by its environment (the selected classes) and its title.
 * Precondition:
 *	class1, class2, and class3 must be in sorted order.
 */
	for (long i = 1; i <= theNumberOfActions; i ++)
		if (class1 == theActions [i]. class1 && class2 == theActions [i]. class2 &&
		    class3 == theActions [i]. class3 && class4 == theActions [i]. class4 &&
		    title && theActions [i]. title && wcsequ (theActions [i]. title, title)) return i;
	return 0;   /* Not found. */
}

void praat_addAction (ClassInfo class1, int n1, ClassInfo class2, int n2, ClassInfo class3, int n3,
	const wchar_t *title, const wchar_t *after, unsigned long flags, void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *))
{ praat_addAction4 (class1, n1, class2, n2, class3, n3, NULL, 0, title, after, flags, callback); }

void praat_addAction1 (ClassInfo class1, int n1,
	const wchar_t *title, const wchar_t *after, unsigned long flags, void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *))
{ praat_addAction4 (class1, n1, NULL, 0, NULL, 0, NULL, 0, title, after, flags, callback); }

void praat_addAction2 (ClassInfo class1, int n1, ClassInfo class2, int n2,
	const wchar_t *title, const wchar_t *after, unsigned long flags, void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *))
{ praat_addAction4 (class1, n1, class2, n2, NULL, 0, NULL, 0, title, after, flags, callback); }

void praat_addAction3 (ClassInfo class1, int n1, ClassInfo class2, int n2, ClassInfo class3, int n3,
	const wchar_t *title, const wchar_t *after, unsigned long flags, void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *))
{ praat_addAction4 (class1, n1, class2, n2, class3, n3, NULL, 0, title, after, flags, callback); }

void praat_addAction4 (ClassInfo class1, int n1, ClassInfo class2, int n2, ClassInfo class3, int n3, ClassInfo class4, int n4,
	const wchar_t *title, const wchar_t *after, unsigned long flags, void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *))
{
	try {
		int depth = flags, unhidable = FALSE, hidden = FALSE, key = 0, attractive = 0;
		unsigned long motifFlags = 0;
		if (flags > 7) {
			depth = ((flags & praat_DEPTH_7) >> 16);
			unhidable = (flags & praat_UNHIDABLE) != 0;
			hidden = (flags & praat_HIDDEN) != 0 && ! unhidable;
			key = flags & 0x000000FF;
			motifFlags = key ? flags & (0x002000FF | GuiMenu_BUTTON_STATE_MASK) : flags & GuiMenu_BUTTON_STATE_MASK;
			attractive = (motifFlags & praat_ATTRACTIVE) != 0;
		}
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);

		if (callback && ! title)
			Melder_throw ("An action command with callback has no title. Classes: ",
				class1 ? class1 -> className : L"", L" ",
				class2 ? class2 -> className : L"", L" ",
				class3 ? class3 -> className : L"", L" ",
				class4 ? class4 -> className : L"", L".");

		if (! class1)
			Melder_throw ("The action command \"", title, "\" has no first class.");

		/*
		 * Determine the position of the new command.
		 */
		long position;
		if (after) {   // search for existing command with same selection
			long found = lookUpMatchingAction (class1, class2, class3, class4, after);
			if (found == 0)
				Melder_throw ("The action command \"", title, "\" cannot be put after \"", after, "\",\n"
					"because the latter command does not exist.");
			position = found + 1;   // after 'after'
		} else {
			position = theNumberOfActions + 1;   // at end
		}

		/*
		 * Increment the command area.
		 */
		if (theNumberOfActions >= praat_MAXNUM_LOOSE_COMMANDS)
			Melder_throw ("Too many action commands (maximum ", praat_MAXNUM_LOOSE_COMMANDS, ").");
		theNumberOfActions += 1;

		/*
		 * Make room for insertion.
		 */
		for (long i = theNumberOfActions; i > position; i --) theActions [i] = theActions [i - 1];
		memset (& theActions [position], 0, sizeof (struct structPraat_Command));

		/*
		 * Insert new command.
		 */
		theActions [position]. class1 = class1;
		theActions [position]. n1 = n1;
		theActions [position]. class2 = class2;
		theActions [position]. n2 = n2;
		theActions [position]. class3 = class3;
		theActions [position]. n3 = n3;
		theActions [position]. class4 = class4;
		theActions [position]. n4 = n4;
		theActions [position]. title = Melder_wcsdup_f (title);
		theActions [position]. depth = depth;
		theActions [position]. callback = callback;   /* NULL for a separator. */
		theActions [position]. button = NULL;
		theActions [position]. script = NULL;
		theActions [position]. hidden = hidden;
		theActions [position]. unhidable = unhidable;
		theActions [position]. attractive = attractive;
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
}

static void deleteDynamicMenu (void) {
	if (praatP.phase != praat_HANDLING_EVENTS) return;
	if (actionsInvisible) return;
	static long numberOfDeletions;
	trace ("deletion #%ld", ++ numberOfDeletions);
	for (int i = 1; i <= theNumberOfActions; i ++) {
		if (theActions [i]. button) {
			trace ("trying to destroy action %d of %d: %ls", i, (int) theNumberOfActions, theActions [i]. title);
			#if gtk || cocoa
				if (theActions [i]. button -> d_parent == praat_form) {
					trace ("destroy a label or a push button or a cascade button");
					GuiObject_destroy (theActions [i]. button -> d_widget);
				} else if (praat_writeMenu && theActions [i]. button -> d_parent == praat_writeMenu) {
					trace ("destroying Save menu item");
					GuiObject_destroy (theActions [i]. button -> d_widget);
				}
			#elif motif
				if (theActions [i]. button -> classInfo == classGuiButton && theActions [i]. button -> d_widget -> subMenuId) {   // a cascade button (not a direct child of the form)?
					trace ("destroy the xm menu bar; this also destroys the xm button and the xm menu");
					GuiObject_destroy (theActions [i]. button -> d_widget -> parent);   // the Motif parent, i.e. not d_parent -> d_widget !
				} else if (theActions [i]. button -> d_parent == praat_form) {
					trace ("destroy a label or a push button");
					GuiObject_destroy (theActions [i]. button -> d_widget);
				}
			#endif
			theActions [i]. button = NULL;
		}
	}
	if (praat_writeMenu) {
		#if gtk || cocoa
			if (praat_writeMenuSeparator) {
				trace ("destroy the Save menu separator");
				GuiObject_destroy (praat_writeMenuSeparator -> d_widget);
			}
			//praat_writeMenu -> f_empty ();
		#elif motif
			GuiObject_destroy (praat_writeMenu -> d_xmMenuTitle);
			GuiObject_destroy (praat_writeMenu -> d_widget);
			praat_writeMenu = GuiMenu_createInWindow (praatP.menuBar, L"Save", 0);
		#endif
		praat_writeMenuSeparator = NULL;
	}
	actionsInvisible = true;
}

static void updateDynamicMenu (void) {
	if (praatP.phase != praat_HANDLING_EVENTS) return;
	praat_sortActions ();
	deleteDynamicMenu ();
	praat_show ();
}

void praat_addActionScript (const wchar_t *className1, int n1, const wchar_t *className2, int n2, const wchar_t *className3, int n3,
	const wchar_t *title, const wchar_t *after, int depth, const wchar_t *script)
{
	try {
		ClassInfo class1 = NULL, class2 = NULL, class3 = NULL;
		Melder_assert (className1 && className2 && className3 && title && after && script);
		if (wcslen (className1)) {
			class1 = Thing_classFromClassName (className1);
		}
		if (wcslen (className2)) {
			class2 = Thing_classFromClassName (className2);
		}
		if (wcslen (className3)) {
			class3 = Thing_classFromClassName (className3);
		}
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);

		if (wcslen (script) && ! wcslen (title))
			Melder_throw ("Command with callback has no title. Classes: ", className1, " ", className2, " ", className3, ".");

		if (! wcslen (className1))
			Melder_throw ("Command \"", title, "\" has no first class.");

		/*
		 * If the button already exists, remove it.
		 */
		long found = lookUpMatchingAction (class1, class2, class3, NULL, title);
		if (found) {
			theNumberOfActions --;
			for (long i = found; i <= theNumberOfActions; i ++) theActions [i] = theActions [i + 1];
		}

		/*
		 * Determine the position of the new command.
		 */
		long position;
		if (wcslen (after)) {   /* Search for existing command with same selection. */
			long found = lookUpMatchingAction (class1, class2, class3, NULL, after);
			if (found) {
				position = found + 1;   // after 'after'
			} else {
				position = theNumberOfActions + 1;   // at end
			}
		} else {
			position = theNumberOfActions + 1;   // at end
		}

		/*
		 * Increment the command area.
		 */
		if (theNumberOfActions >= praat_MAXNUM_LOOSE_COMMANDS)
			Melder_throw ("Too many actions (maximum is ", praat_MAXNUM_LOOSE_COMMANDS, ").");
		theNumberOfActions += 1;

		/*
		 * Make room for insertion.
		 */
		for (long i = theNumberOfActions; i > position; i --) theActions [i] = theActions [i - 1];
		memset (& theActions [position], 0, sizeof (struct structPraat_Command));

		/*
		 * Insert new command.
		 */
		theActions [position]. class1 = class1;
		theActions [position]. n1 = n1;
		theActions [position]. class2 = class2;
		theActions [position]. n2 = n2;
		theActions [position]. class3 = class3;
		theActions [position]. n3 = n3;
		theActions [position]. title = wcslen (title) ? Melder_wcsdup_f (title) : NULL;   // allow old-fashioned untitled separators
		theActions [position]. depth = depth;
		theActions [position]. callback = wcslen (script) ? DO_RunTheScriptFromAnyAddedMenuCommand : NULL;   // NULL for a separator
		theActions [position]. button = NULL;
		if (wcslen (script) == 0) {
			theActions [position]. script = NULL;
		} else {
			structMelderFile file = { 0 };
			Melder_relativePathToFile (script, & file);
			theActions [position]. script = Melder_wcsdup_f (Melder_fileToPath (& file));
		}
		theActions [position]. after = wcslen (after) ? Melder_wcsdup_f (after) : NULL;
		theActions [position]. phase = praatP.phase;
		if (praatP.phase >= praat_READING_BUTTONS) {
			static long uniqueID = 0;
			theActions [position]. uniqueID = ++ uniqueID;
		}
		updateDynamicMenu ();
	} catch (MelderError) {
		Melder_throw ("Praat: script action not added.");
	}
}

void praat_removeAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const wchar_t *title) {
	try {
		int n1, n2, n3;
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
		long found = lookUpMatchingAction (class1, class2, class3, NULL, title);
		if (! found) {
			Melder_throw ("Action command \"", class1 -> className,
				class2 ? L" & ": L"", class2 -> className,
				class3 ? L" & ": L"", class3 -> className,
				": ", title, "\" not found.");
		}
		Melder_free (theActions [found]. title);
		theNumberOfActions --;
		for (long i = found; i <= theNumberOfActions; i ++) theActions [i] = theActions [i + 1];
	} catch (MelderError) {
		Melder_throw ("Praat: action not removed.");
	}
}

void praat_removeAction_classNames (const wchar_t *className1, const wchar_t *className2,
	const wchar_t *className3, const wchar_t *title)
{
	try {
		ClassInfo class1 = NULL, class2 = NULL, class3 = NULL;
		Melder_assert (className1 && className2 && className3 && title);
		if (wcslen (className1)) {
			class1 = Thing_classFromClassName (className1);
		}
		if (wcslen (className2)) {
			class2 = Thing_classFromClassName (className2);
		}
		if (wcslen (className3)) {
			class3 = Thing_classFromClassName (className3);
		}
		praat_removeAction (class1, class2, class3, title);
		updateDynamicMenu ();
	} catch (MelderError) {
		Melder_throw ("Praat: action not removed.");
	}
}

void praat_hideAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const wchar_t *title) {
	try {
		int n1, n2, n3;
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
		long found = lookUpMatchingAction (class1, class2, class3, NULL, title);
		if (! found) {
			Melder_throw ("Praat: action command \"", class1 ? class1 -> className : NULL,
				class2 ? " & ": NULL, class2 ? class2 -> className : NULL,
				class3 ? " & ": NULL, class3 ? class3 -> className : NULL,
				": ", title, "\" not found.");
		}
		if (! theActions [found]. hidden) {
			theActions [found]. hidden = TRUE;
			if (praatP.phase >= praat_READING_BUTTONS) theActions [found]. toggled = ! theActions [found]. toggled;
			updateDynamicMenu ();
		}
	} catch (MelderError) {
		Melder_throw ("Praat: action not hidden.");
	}
}

void praat_hideAction_classNames (const wchar_t *className1, const wchar_t *className2,
	const wchar_t *className3, const wchar_t *title)
{
	try {
		ClassInfo class1 = NULL, class2 = NULL, class3 = NULL;
		Melder_assert (className1 && className2 && className3 && title);
		if (wcslen (className1)) {
			class1 = Thing_classFromClassName (className1);
		}
		if (wcslen (className2)) {
			class2 = Thing_classFromClassName (className2);
		}
		if (wcslen (className3)) {
			class3 = Thing_classFromClassName (className3);
		}
		praat_hideAction (class1, class2, class3, title);
	} catch (MelderError) {
		Melder_throw ("Praat: action not hidden.");
	}
}

void praat_showAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const wchar_t *title) {
	try {
		int n1, n2, n3;
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
		long found = lookUpMatchingAction (class1, class2, class3, NULL, title);
		if (! found) {
			Melder_throw ("Action command \"", class1 ? class1 -> className : NULL,
				class2 ? L" & ": NULL, class2 ? class2 -> className : NULL,
				class3 ? L" & ": NULL, class3 ? class3 -> className : NULL,
				L": ", title, "\" not found.");
		}
		if (theActions [found]. hidden) {
			theActions [found]. hidden = FALSE;
			if (praatP.phase >= praat_READING_BUTTONS) theActions [found]. toggled = ! theActions [found]. toggled;
			updateDynamicMenu ();
		}
	} catch (MelderError) {
		Melder_throw ("Praat: action not shown.");
	}
}

void praat_showAction_classNames (const wchar_t *className1, const wchar_t *className2,
	const wchar_t *className3, const wchar_t *title)
{
	try {
		ClassInfo class1 = NULL, class2 = NULL, class3 = NULL;
		Melder_assert (className1 && className2 && className3 && title);
		if (wcslen (className1)) {
			class1 = Thing_classFromClassName (className1);
		}
		if (wcslen (className2)) {
			class2 = Thing_classFromClassName (className2);
		}
		if (wcslen (className3)) {
			class3 = Thing_classFromClassName (className3);
		}
		praat_showAction (class1, class2, class3, title);
	} catch (MelderError) {
		Melder_throw ("Praat: action not shown.");
	}
}

static int compareActions (const void *void_me, const void *void_thee) {
	praat_Command me = (praat_Command) void_me, thee = (praat_Command) void_thee;
	int compare;
	compare = wcscmp (my class1 -> className, thy class1 -> className);
	if (compare) return my class1 == classData ? -1 : thy class1 == classData ? 1 : compare;
	if (my class2) {
		if (! thy class2) return 1;
		compare = wcscmp (my class2 -> className, thy class2 -> className);
		if (compare) return compare;
	} else if (thy class2) return -1;
	if (my class3) {
		if (! thy class3) return 1;
		compare = wcscmp (my class3 -> className, thy class3 -> className);
		if (compare) return compare;
	} else if (thy class3) return -1;
	if (my sortingTail < thy sortingTail) return -1;
	return 1;
}

void praat_sortActions (void) {
	for (long i = 1; i <= theNumberOfActions; i ++)
		theActions [i]. sortingTail = i;
	qsort (& theActions [1], theNumberOfActions, sizeof (struct structPraat_Command), compareActions);
}

static const wchar_t *numberString (int number) {
	return number == 1 ? L"one" : number == 2 ? L"two" : number == 3 ? L"three" : L"any number of";
}
static const wchar_t *classString (ClassInfo klas) {
	return klas == classData ? L"" : klas -> className;
}
static const wchar_t *objectString (int number) {
	return number == 1 ? L"object" : L"objects";
}
static bool allowExecutionHook (void *closure) {
	void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *) = (void (*) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *)) closure;
	Melder_assert (sizeof (callback) == sizeof (void *));
	long numberOfMatchingCallbacks = 0, firstMatchingCallback = 0;
	for (long i = 1; i <= theNumberOfActions; i ++) {
		praat_Command me = & theActions [i];
		if (my callback == callback) {
			int sel1, sel2 = 0, sel3 = 0, sel4 = 0;
			if (! my class1) Melder_throw ("No class1???");
			numberOfMatchingCallbacks += 1;
			if (! firstMatchingCallback) firstMatchingCallback = i;
			sel1 = my class1 == classData ? theCurrentPraatObjects -> totalSelection : praat_selection (my class1);
			if (sel1 == 0) continue;
			if (my class2 && (sel2 = praat_selection (my class2)) == 0) continue;
			if (my class3 && (sel3 = praat_selection (my class3)) == 0) continue;
			if (my class4 && (sel4 = praat_selection (my class4)) == 0) continue;
			if (sel1 + sel2 + sel3 + sel4 != theCurrentPraatObjects -> totalSelection) continue;
			if ((my n1 && sel1 != my n1) || (my n2 && sel2 != my n2) || (my n3 && sel3 != my n3) || (my n4 && sel4 != my n4)) continue;
			return true;   // found a matching action
		}
	}
	if (numberOfMatchingCallbacks == 1) {
		praat_Command me = & theActions [firstMatchingCallback];
		Melder_error_ ("Selection changed! It should be:");
		if (my class1) Melder_error_ ("   ", numberString (my n1), " ", classString (my class1), " ", objectString (my n1));
		if (my class2) Melder_error_ ("   ", numberString (my n2), " ", classString (my class2), " ", objectString (my n2));
		if (my class3) Melder_error_ ("   ", numberString (my n3), " ", classString (my class3), " ", objectString (my n3));
		if (my class4) Melder_error_ ("   ", numberString (my n4), " ", classString (my class4), " ", objectString (my n4));
		throw MelderError ();
	} else {
		Melder_throw ("Selection changed!");
	}
	return false;
}

static void do_menu (I, bool modified) {
/*
 *	Convert a Gui callback into a Ui callback, and catch modifier keys and special mouse buttons.
 *	Call that callback!
 *	Catch the error queue for menu commands without dots (...).
 */
	void (*callback) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *) = (void (*) (UiForm, int, Stackel, const wchar_t *, Interpreter, const wchar_t *, bool, void *)) void_me;
	for (long i = 1; i <= theNumberOfActions; i ++) {
		praat_Command me = & theActions [i];
		if (my callback == callback) {
			if (my title != NULL && ! wcsstr (my title, L"...")) {
				UiHistory_write (L"\ndo (\"");
				UiHistory_write_expandQuotes (my title);
				UiHistory_write (L"\")");
			}
			Ui_setAllowExecutionHook (allowExecutionHook, (void *) callback);   // BUG: one shouldn't assign a function pointer to a void pointer
			try {
				callback (NULL, 0, NULL, NULL, NULL, my title, modified, NULL);
			} catch (MelderError) {
				Melder_error_ ("Command \"", my title, "\" not executed.");
				Melder_flushError (NULL);
			}
			Ui_setAllowExecutionHook (NULL, NULL);
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

static void cb_menu (GUI_ARGS) {
	bool modified = event -> shiftKeyPressed || event -> commandKeyPressed || event -> optionKeyPressed || event -> extraControlKeyPressed;
	do_menu (void_me, modified);
}

static void gui_button_cb_menu (I, GuiButtonEvent event) {
	do_menu (void_me, event -> shiftKeyPressed | event -> commandKeyPressed | event -> optionKeyPressed | event -> extraControlKeyPressed);
}

void praat_actions_show (void) {
	#if defined (macintosh)
		const int BUTTON_VSPACING = 8;
	#else
		const int BUTTON_VSPACING = 5;
	#endif
	/*
	 * The selection has changed;
	 * kill the dynamic menu and the write menu.
	 */
	if (! theCurrentPraatApplication -> batch) {
		deleteDynamicMenu ();
		if (! Melder_backgrounding) {
			praat_writeMenu -> f_setSensitive (false);
			if (praat_writeMenuSeparator) praat_writeMenuSeparator -> f_hide ();
		}

		/* Determine the visibility and sensitivity of all the actions.
		 */
		if (theCurrentPraatObjects -> totalSelection != 0 && ! Melder_backgrounding)
			praat_writeMenu -> f_setSensitive (true);
	}
	for (long i = 1; i <= theNumberOfActions; i ++) {
		int sel1 = 0, sel2 = 0, sel3 = 0, sel4 = 0;
		int n1 = theActions [i]. n1, n2 = theActions [i]. n2, n3 = theActions [i]. n3, n4 = theActions [i]. n4;

		/* Clean up from previous selection. */

		theActions [i]. visible = FALSE;
		theActions [i]. executable = FALSE;

		/* Match the actually selected classes with the selection required for this visibility. */

		if (! theActions [i]. class1) continue;   /* At least one class selected. */
		sel1 = theActions [i]. class1 == classData ? theCurrentPraatObjects -> totalSelection : praat_selection (theActions [i]. class1);
		if (sel1 == 0) continue;
		if (theActions [i]. class2 && (sel2 = praat_selection (theActions [i]. class2)) == 0) continue;
		if (theActions [i]. class3 && (sel3 = praat_selection (theActions [i]. class3)) == 0) continue;
		if (theActions [i]. class4 && (sel4 = praat_selection (theActions [i]. class4)) == 0) continue;
		if (sel1 + sel2 + sel3 + sel4 != theCurrentPraatObjects -> totalSelection) continue;   /* Other classes selected? Do not show. */
		theActions [i]. visible = ! theActions [i]. hidden;

		/* Match the actually selected objects with the selection required for this action. */

		if (! theActions [i]. callback) continue;   /* Separators are not executable. */
		if ((n1 && sel1 != n1) || (n2 && sel2 != n2) || (n3 && sel3 != n3) || (n4 && sel4 != n4)) continue;
		theActions [i]. executable = TRUE;
	}

	/* Create a new column of buttons in the dynamic menu. */
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding) {
		actionsInvisible = false;
		GuiMenu currentSubmenu1 = NULL, currentSubmenu2 = NULL;
		int writeMenuGoingToSeparate = FALSE;
		int y = Machine_getMenuBarHeight () + 10;
		for (long i = 1; i <= theNumberOfActions; i ++) {   /* Add buttons or make existing buttons sensitive (executable). */
			praat_Command me = & theActions [i];
			if (my depth == 0) currentSubmenu1 = NULL, currentSubmenu2 = NULL;   /* Prevent attachment of later deep actions to earlier submenus after removal of label. */
			if (my depth == 1) currentSubmenu2 = NULL;   /* Prevent attachment of later deep actions to earlier submenus after removal of label. */
			if (! my visible) continue;
			if (my callback) {
				/* Apparently a true command: create a button in the dynamic menu.
				 * If it is a subcommand (depth > 0), put it in the current submenu,
				 * but only if this exists (umbrella against stray submenu specifications).
				 */
				GuiMenu parentMenu = my depth > 1 && currentSubmenu2 ? currentSubmenu2 : my depth > 0 && currentSubmenu1 ? currentSubmenu1 : NULL;

				if (wcsnequ (my title, L"Save ", 5) || wcsnequ (my title, L"Write ", 6) || wcsnequ (my title, L"Append to ", 10)) {
					parentMenu = praat_writeMenu;
					if (! praat_writeMenuSeparator) {
						if (writeMenuGoingToSeparate)
							praat_writeMenuSeparator = GuiMenu_addSeparator (parentMenu);
						else if (wcsequ (my title, L"Save as binary file..."))
							writeMenuGoingToSeparate = TRUE;
					}
				}
				if (parentMenu) {
					my button = GuiMenu_addItem (parentMenu, my title,
						( my executable ? 0 : GuiMenu_INSENSITIVE ),
						cb_menu,
						my callback == DO_RunTheScriptFromAnyAddedMenuCommand ? (void *) my script : (void *) my callback);
				} else {
					my button = GuiButton_createShown (praat_form,
						BUTTON_LEFT, BUTTON_RIGHT, y, y + Gui_PUSHBUTTON_HEIGHT,
						my title, gui_button_cb_menu,
						my callback == DO_RunTheScriptFromAnyAddedMenuCommand ? (void *) my script : (void *) my callback,
							( my executable ? 0 : GuiButton_INSENSITIVE ) | ( my attractive ? GuiButton_ATTRACTIVE : 0 ));
					y += Gui_PUSHBUTTON_HEIGHT + BUTTON_VSPACING;
					#if gtk
						/* Dit soort onzin zou eigenlijk in GuiButton moeten */
						gtk_button_set_alignment (GTK_BUTTON (my button -> d_widget), 0.0f, 0.5f);
					#endif
				}
			} else if (i == theNumberOfActions || theActions [i + 1]. depth == 0) {
				/*
				 * Apparently a labelled separator.
				 */
				my button = GuiLabel_createShown (praat_form, BUTTON_LEFT, BUTTON_RIGHT, y, y + Gui_LABEL_HEIGHT, my title, 0);
				y += Gui_LABEL_HEIGHT + BUTTON_VSPACING;
			} else if (my title == NULL || my title [0] == '-') {
				/*
				 * Apparently a separator in a submenu.
				 */
				if (currentSubmenu2 || currentSubmenu1) {   /* These separators are not shown in a flattened menu. */
					my button = GuiMenu_addSeparator (currentSubmenu2 ? currentSubmenu2 : currentSubmenu1);
					my button -> f_show ();
				}
			} else {
				/*
				 * Apparently a submenu.
				 */
				if (my depth == 0 || ! currentSubmenu1) {
					currentSubmenu1 = GuiMenu_createInForm (praat_form,
						BUTTON_LEFT, BUTTON_RIGHT, y, y + Gui_PUSHBUTTON_HEIGHT,
						my title, 0);
					y += Gui_PUSHBUTTON_HEIGHT + BUTTON_VSPACING;
					my button = currentSubmenu1 -> d_cascadeButton;
				} else {
					currentSubmenu2 = GuiMenu_createInMenu (currentSubmenu1, my title, 0);
					my button = currentSubmenu2 -> d_menuItem;
				}
				my button -> f_show ();
			}
		}
	}
}

void praat_actions_createWriteMenu (GuiWindow window) {
	if (theCurrentPraatApplication -> batch) return;
	praat_writeMenu = GuiMenu_createInWindow (window, L"Save", GuiMenu_INSENSITIVE);
	#if gtk
		GuiMenu_addSeparator (praat_writeMenu);
	#endif
}

void praat_actions_init (void) {
	theActions = Melder_calloc_f (struct structPraat_Command, 1 + praat_MAXNUM_LOOSE_COMMANDS);
}

void praat_actions_createDynamicMenu (GuiWindow window) {
	if (theCurrentPraatApplication -> batch) return;
	praat_form = window;
}

void praat_saveAddedActions (MelderString *buffer) {
	long maxID = 0;
	for (long iaction = 1; iaction <= theNumberOfActions; iaction ++) {
		if (theActions [iaction]. uniqueID > maxID)
			maxID = theActions [iaction]. uniqueID;
	}
	for (long ident = 1; ident <= maxID; ident ++)
		for (long iaction = 1; iaction <= theNumberOfActions; iaction ++) {
			praat_Command me = & theActions [iaction];
			if (my uniqueID == ident && ! my hidden && my title) {
				MelderString_append (buffer, L"Add action command...");
				MelderString_append (buffer, L" ", my class1 -> className, L" ", Melder_integer (my n1));
				MelderString_append (buffer, L" ", my class2 ? my class2 -> className : L"\"\"", L" ", Melder_integer (my n2));
				MelderString_append (buffer, L" ", my class3 ? my class3 -> className : L"\"\"", L" ", Melder_integer (my n3));
				MelderString_append (buffer, L" \"", my title, L"\" \"", my after ? my after : L"", L"\" ", Melder_integer (my depth));
				MelderString_append (buffer, L" ", my script ? my script : L"", L"\n");
				break;
			}
		}
	for (long iaction = 1; iaction <= theNumberOfActions; iaction ++) {
		praat_Command me = & theActions [iaction];
		if (my toggled && my title && ! my uniqueID && ! my script) {
			MelderString_append (buffer, my hidden ? L"Hide" : L"Show", L" action command...");
			MelderString_append (buffer, L" ", my class1 -> className);
			MelderString_append (buffer, L" ", my class2 ? my class2 -> className : L"\"\"");
			MelderString_append (buffer, L" ", my class3 ? my class3 -> className : L"\"\"");
			MelderString_append (buffer, L" ", my title, L"\n");
		}
	}
}

int praat_doAction (const wchar_t *command, const wchar_t *arguments, Interpreter interpreter) {
	long i = 1;
	while (i <= theNumberOfActions && (! theActions [i]. executable || wcscmp (theActions [i]. title, command))) i ++;
	if (i > theNumberOfActions) return 0;   /* Not found. */
	theActions [i]. callback (NULL, 0, NULL, arguments, interpreter, command, false, NULL);
	return 1;
}

int praat_doAction (const wchar_t *command, int narg, Stackel args, Interpreter interpreter) {
	long i = 1;
	while (i <= theNumberOfActions && (! theActions [i]. executable || wcscmp (theActions [i]. title, command))) i ++;
	if (i > theNumberOfActions) return 0;   /* Not found. */
	theActions [i]. callback (NULL, narg, args, NULL, interpreter, command, false, NULL);
	return 1;
}

long praat_getNumberOfActions (void) { return theNumberOfActions; }

praat_Command praat_getAction (long i)
	{ return i < 0 || i > theNumberOfActions ? NULL : & theActions [i]; }

void praat_background (void) {
	if (Melder_batch) return;
	if (Melder_backgrounding) return;
	deleteDynamicMenu ();
	praat_list_background ();
	Melder_backgrounding = true;
	if (! praatP.dontUsePictureWindow) praat_picture_background ();
}

void praat_foreground (void) {
	if (Melder_batch) return;
	if (! Melder_backgrounding) return;
	Melder_backgrounding = false;
	praat_list_foreground ();
	praat_show ();
	if (! praatP.dontUsePictureWindow) praat_picture_foreground ();
}

/* End of file praat_actions.cpp */
