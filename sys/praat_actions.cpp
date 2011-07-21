/* praat_actions.cpp
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
 * pb 2004/04/01 second-level menus
 * pb 2006/12/26 theCurrentPraat
 * pb 2007/01/25 width of button list is 50 procent
 * pb 2007/06/10 wchar_t
 * pb 2007/10/17 removed a bug that caused a crash in praat_show/hideAction if class2 or class3 was NULL
 * pb 2007/12/26 Gui
 * pb 2009/01/18 arguments to UiForm callbacks
 * pb 2009/08/03 repaired a memory leak in praat_removeAction
 * pb 2009/12/22 invokingButtonTitle
 * pb 2011/04/05 C++
 */

#include "praatP.h"
#include "praat_script.h"
#include "longchar.h"
#include "machine.h"

#define BUTTON_WIDTH  240

#define praat_MAXNUM_LOOSE_COMMANDS  5000
static long theNumberOfActions = 0;
static struct structPraat_Command *theActions;
static GuiObject praat_writeMenuTitle, praat_writeMenu, praat_writeMenuSeparator;
static GuiObject praat_dynamicMenu, praat_dynamicMenuWindow;

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

	/* Fix unusual input bubblewise. */

	if (*class1 == NULL && *class2 != NULL) { *class1 = *class2; *n1 = *n2; *class2 = NULL; *n2 = 0; }
	if (*class2 == NULL && *class3 != NULL) { *class2 = *class3; *n2 = *n3; *class3 = NULL; *n3 = 0;
		if (*class1 == NULL && *class2 != NULL) { *class1 = *class2; *n1 = *n2; *class2 = NULL; *n2 = 0; } }

	/* Now: if *class3, then *class2, and if *class2, then *class1.
	 * Bubble-sort the input by class name.
	 */
	if (*class2 && wcscmp (((Data_Table) *class1) -> _className, ((Data_Table) *class2) -> _className) > 0) {
		void *helpClass = *class1; *class1 = *class2; *class2 = helpClass;
		int helpN = *n1; *n1 = *n2; *n2 = helpN;
	}
	if (*class3 && wcscmp (((Data_Table) *class2) -> _className, ((Data_Table) *class3) -> _className) > 0) {
		void *helpClass = *class2; *class2 = *class3; *class3 = helpClass;
		int helpN = *n2; *n2 = *n3; *n3 = helpN;
		if (wcscmp (((Data_Table) *class1) -> _className, ((Data_Table) *class2) -> _className) > 0) {
			void *helpClass = *class1; *class1 = *class2; *class2 = helpClass;
			int helpN = *n1; *n1 = *n2; *n2 = helpN;
		}
	}
}

static long lookUpMatchingAction (void *class1, void *class2, void *class3, void *class4, const wchar_t *title) {
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

void praat_addAction (void *class1, int n1, void *class2, int n2, void *class3, int n3,
	const wchar *title, const wchar *after, unsigned long flags, void (*callback) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *))
{ praat_addAction4 (class1, n1, class2, n2, class3, n3, NULL, 0, title, after, flags, callback); }

void praat_addAction1 (void *class1, int n1,
	const wchar *title, const wchar *after, unsigned long flags, void (*callback) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *))
{ praat_addAction4 (class1, n1, NULL, 0, NULL, 0, NULL, 0, title, after, flags, callback); }

void praat_addAction2 (void *class1, int n1, void *class2, int n2,
	const wchar *title, const wchar *after, unsigned long flags, void (*callback) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *))
{ praat_addAction4 (class1, n1, class2, n2, NULL, 0, NULL, 0, title, after, flags, callback); }

void praat_addAction3 (void *class1, int n1, void *class2, int n2, void *class3, int n3,
	const wchar *title, const wchar *after, unsigned long flags, void (*callback) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *))
{ praat_addAction4 (class1, n1, class2, n2, class3, n3, NULL, 0, title, after, flags, callback); }

void praat_addAction4 (void *class1, int n1, void *class2, int n2, void *class3, int n3, void *class4, int n4,
	const wchar *title, const wchar *after, unsigned long flags, void (*callback) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *))
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
				class1 ? ((Data_Table) class1) -> _className : L"", L" ",
				class2 ? ((Data_Table) class2) -> _className : L"", L" ",
				class3 ? ((Data_Table) class3) -> _className : L"", L" ",
				class4 ? ((Data_Table) class4) -> _className : L"", L".");

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
//static int deletions;
//Melder_information1(Melder_integer(++deletions));
	if (praat_dynamicMenu) {
		GuiObject_destroy (praat_dynamicMenu);
		praat_dynamicMenu = NULL;
		for (int i = 1; i <= theNumberOfActions; i ++)
			theActions [i]. button = NULL;
		if (praat_writeMenu) {   // ppgb 20080103: put into praat_dynamicMenu condition
			GuiObject_destroy (praat_writeMenu);

			praat_writeMenuSeparator = NULL;

			// RFC: Beter? Nog beter?
			#if gtk
				praat_writeMenu = gtk_menu_new ();
				gtk_menu_item_set_submenu (GTK_MENU_ITEM (praat_writeMenuTitle), praat_writeMenu);
			#elif motif
				praat_writeMenu = XmCreatePulldownMenu (praatP.menuBar, "Save", NULL, 0);
				XtVaSetValues (praat_writeMenuTitle, XmNsubMenuId, praat_writeMenu, NULL);
			#endif
		}
	}
}

static void updateDynamicMenu (void) {
	if (praatP.phase != praat_HANDLING_EVENTS) return;
	praat_sortActions ();
	deleteDynamicMenu ();
	praat_show ();
}

void praat_addActionScript (const wchar *className1, int n1, const wchar *className2, int n2, const wchar *className3, int n3,
	const wchar *title, const wchar *after, int depth, const wchar *script)
{
	try {
		void *class1 = NULL, *class2 = NULL, *class3 = NULL;
		Melder_assert (className1 && className2 && className3 && title && after && script);
		if (wcslen (className1)) {
			class1 = Thing_classFromClassName (className1); therror
		}
		if (wcslen (className2)) {
			class2 = Thing_classFromClassName (className2); therror
		}
		if (wcslen (className3)) {
			class3 = Thing_classFromClassName (className3); therror
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

void praat_removeAction (void *class1, void *class2, void *class3, const wchar_t *title) {
	try {
		int n1, n2, n3;
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
		long found = lookUpMatchingAction (class1, class2, class3, NULL, title);
		if (! found) {
			Melder_throw ("Action command \"", ((Data_Table) class1) -> _className,
				class2 ? L" & ": L"", ((Data_Table) class2) -> _className,
				class3 ? L" & ": L"", ((Data_Table) class3) -> _className,
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
		void *class1 = NULL, *class2 = NULL, *class3 = NULL;
		Melder_assert (className1 && className2 && className3 && title);
		if (wcslen (className1)) {
			class1 = Thing_classFromClassName (className1); therror
		}
		if (wcslen (className2)) {
			class2 = Thing_classFromClassName (className2); therror
		}
		if (wcslen (className3)) {
			class3 = Thing_classFromClassName (className3); therror
		}
		praat_removeAction (class1, class2, class3, title); therror
		updateDynamicMenu ();
	} catch (MelderError) {
		Melder_throw ("Praat: action not removed.");
	}
}

void praat_hideAction (void *class1, void *class2, void *class3, const wchar_t *title) {
	try {
		int n1, n2, n3;
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
		long found = lookUpMatchingAction (class1, class2, class3, NULL, title);
		if (! found) {
			Melder_throw ("Praat: action command \"", class1 ? ((Data_Table) class1) -> _className : NULL,
				class2 ? " & ": NULL, class2 ? ((Data_Table) class2) -> _className : NULL,
				class3 ? " & ": NULL, class3 ? ((Data_Table) class3) -> _className : NULL,
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
		void *class1 = NULL, *class2 = NULL, *class3 = NULL;
		Melder_assert (className1 && className2 && className3 && title);
		if (wcslen (className1)) {
			class1 = Thing_classFromClassName (className1); therror
		}
		if (wcslen (className2)) {
			class2 = Thing_classFromClassName (className2); therror
		}
		if (wcslen (className3)) {
			class3 = Thing_classFromClassName (className3); therror
		}
		praat_hideAction (class1, class2, class3, title); therror
	} catch (MelderError) {
		Melder_throw ("Praat: action not hidden.");
	}
}

void praat_showAction (void *class1, void *class2, void *class3, const wchar_t *title) {
	try {
		int n1, n2, n3;
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
		long found = lookUpMatchingAction (class1, class2, class3, NULL, title);
		if (! found) {
			Melder_throw ("Action command \"", class1 ? ((Data_Table) class1) -> _className : NULL,
				class2 ? L" & ": NULL, class2 ? ((Data_Table) class2) -> _className : NULL,
				class3 ? L" & ": NULL, class3 ? ((Data_Table) class3) -> _className : NULL,
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
		void *class1 = NULL, *class2 = NULL, *class3 = NULL;
		Melder_assert (className1 && className2 && className3 && title);
		if (wcslen (className1)) {
			class1 = Thing_classFromClassName (className1); therror
		}
		if (wcslen (className2)) {
			class2 = Thing_classFromClassName (className2); therror
		}
		if (wcslen (className3)) {
			class3 = Thing_classFromClassName (className3); therror
		}
		praat_showAction (class1, class2, class3, title); therror
	} catch (MelderError) {
		Melder_throw ("Praat: action not shown.");
	}
}

static int compareActions (const void *void_me, const void *void_thee) {
	praat_Command me = (praat_Command) void_me, thee = (praat_Command) void_thee;
	int compare;
	compare = wcscmp (((Data_Table) my class1) -> _className, ((Data_Table) thy class1) -> _className);
	if (compare) return my class1 == classData ? -1 : thy class1 == classData ? 1 : compare;
	if (my class2) {
		if (! thy class2) return 1;
		compare = wcscmp (((Data_Table) my class2) -> _className, ((Data_Table) thy class2) -> _className);
		if (compare) return compare;
	} else if (thy class2) return -1;
	if (my class3) {
		if (! thy class3) return 1;
		compare = wcscmp (((Data_Table) my class3) -> _className, ((Data_Table) thy class3) -> _className);
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

static const wchar *numberString (int number) {
	return number == 1 ? L"one" : number == 2 ? L"two" : number == 3 ? L"three" : L"any number of";
}
static const wchar *classString (void *klas) {
	return klas == classData ? L"" : ((Data_Table) klas) -> _className;
}
static const wchar *objectString (int number) {
	return number == 1 ? L"object" : L"objects";
}
static int allowExecutionHook (void *closure) {
	void (*callback) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *) = (void (*) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *)) closure;
	Melder_assert (sizeof (callback) == sizeof (void *));
	long numberOfMatchingCallbacks = 0, firstMatchingCallback = 0;
	for (long i = 1; i <= theNumberOfActions; i ++) {
		praat_Command me = & theActions [i];
		if (my callback == callback) {
			int sel1, sel2 = 0, sel3 = 0, sel4 = 0;
			if (! my class1) return Melder_error1 (L"No class1???");
			numberOfMatchingCallbacks += 1;
			if (! firstMatchingCallback) firstMatchingCallback = i;
			sel1 = my class1 == classData ? theCurrentPraatObjects -> totalSelection : praat_selection (my class1);
			if (sel1 == 0) continue;
			if (my class2 && (sel2 = praat_selection (my class2)) == 0) continue;
			if (my class3 && (sel3 = praat_selection (my class3)) == 0) continue;
			if (my class4 && (sel4 = praat_selection (my class4)) == 0) continue;
			if (sel1 + sel2 + sel3 + sel4 != theCurrentPraatObjects -> totalSelection) continue;
			if ((my n1 && sel1 != my n1) || (my n2 && sel2 != my n2) || (my n3 && sel3 != my n3) || (my n4 && sel4 != my n4)) continue;
			return TRUE;   /* Found a matching action. */
		}
	}
	if (numberOfMatchingCallbacks == 1) {
		praat_Command me = & theActions [firstMatchingCallback];
		Melder_error1 (L"Selection changed! It should be:");
		if (my class1) Melder_error6 (L"   ", numberString (my n1), L" ", classString (my class1), L" ", objectString (my n1));
		if (my class2) Melder_error6 (L"   ", numberString (my n2), L" ", classString (my class2), L" ", objectString (my n2));
		if (my class3) Melder_error6 (L"   ", numberString (my n3), L" ", classString (my class3), L" ", objectString (my n3));
		if (my class4) Melder_error6 (L"   ", numberString (my n4), L" ", classString (my class4), L" ", objectString (my n4));
	} else {
		Melder_error1 (L"Selection changed!");
	}
	return FALSE;
}

static void do_menu (I, bool modified) {
/*
 *	Convert a Gui callback into a Ui callback, and catch modifier keys and special mouse buttons.
 *	Call that callback!
 *	Catch the error queue for menu commands without dots (...).
 */
	void (*callback) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *) = (void (*) (UiForm, const wchar *, Interpreter, const wchar *, bool, void *)) void_me;
	for (long i = 1; i <= theNumberOfActions; i ++) {
		praat_Command me = & theActions [i];
		if (my callback == callback) {
			if (my title != NULL && ! wcsstr (my title, L"...")) {
				UiHistory_write (L"\n");
				UiHistory_write (my title);
			}
			Ui_setAllowExecutionHook (allowExecutionHook, (void *) callback);   // BUG: one shouldn't assign a function pointer to a void pointer
			try {
				callback (NULL, NULL, NULL, my title, modified, NULL); therror
			} catch (MelderError) {
				Melder_flushError ("Command not executed.");
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
				DO_RunTheScriptFromAnyAddedMenuCommand (NULL, my script, NULL, NULL, false, NULL); therror
			} catch (MelderError) {
				Melder_flushError ("Script not executed.");
			}
			praat_updateSelection (); return;
		}
	}
}

static void cb_menu (GUI_ARGS) {
	(void) w;
	bool modified = false;
	if (call) {
		#if defined (macintosh)
			EventRecord *event = (EventRecord *) call;
			enum { cmdKey = 256, shiftKey = 512, optionKey = 2048, controlKey = 4096 };
			modified = event -> what == mouseDown &&
				(event -> modifiers & (cmdKey | shiftKey | optionKey | controlKey)) != 0;
		#elif defined (_WIN32)
			modified = FALSE;
		#else
			#if motif
			XButtonPressedEvent *event = (XButtonPressedEvent *) ((XmDrawingAreaCallbackStruct *) call) -> event;
			modified = event -> type == ButtonPress &&
				((event -> state & (ShiftMask | ControlMask | Mod1Mask)) != 0 || event -> button == Button2 || event -> button == Button3);
			#endif
		#endif
	}
	do_menu (void_me, modified);
}

static void gui_button_cb_menu (I, GuiButtonEvent event) {
	do_menu (void_me, event -> shiftKeyPressed | event -> commandKeyPressed | event -> optionKeyPressed | event -> extraControlKeyPressed);
}

void praat_actions_show (void) {
	long i;
	#if motif
		GuiObject buttons [1000], writeButtons [50];
	#endif

	/* The selection has changed;
	 * kill the dynamic menu and the write menu.
	 */
	if (! theCurrentPraatApplication -> batch) {
		#if defined (macintosh) || defined (_WIN32) || 1
			deleteDynamicMenu ();
		#endif
		if (! Melder_backgrounding) {
			#if motif
				int nbuttons = 0, nwriteButtons = 0;
			#endif
			GuiObject_setSensitive (praat_writeMenuTitle, False);
			if (praat_writeMenuSeparator) GuiObject_hide (praat_writeMenuSeparator);
			if (praat_dynamicMenu) GuiObject_hide (praat_dynamicMenu);
			for (i = theNumberOfActions; i >= 1; i --) {
				praat_Command me = & theActions [i];
				if (! my visible) continue;
				if (my button) {
					if (GuiObject_parent (my button) == praat_dynamicMenu)   /* Unmanage only level-1 visible buttons. */
						#if gtk
							GuiObject_hide (my button);
						#elif motif
							buttons [nbuttons ++] = my button;
						#endif
					else if (my title && (wcsnequ (my title, L"Save ", 5) || wcsnequ (my title, L"Write ", 6) || wcsnequ (my title, L"Append to ", 10)))
						#if gtk
							GuiObject_hide (my button);
						#elif motif
							writeButtons [nwriteButtons ++] = my button;
						#endif
				}
				#if motif
					if (nbuttons) XtUnmanageChildren (buttons, nbuttons);   // multiple hide
					if (nwriteButtons) XtUnmanageChildren (writeButtons, nwriteButtons);   // multiple hide
				#endif
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
		if (theCurrentPraatObjects -> totalSelection != 0 && ! Melder_backgrounding)
			GuiObject_setSensitive (praat_writeMenuTitle, True);
	}
	for (i = 1; i <= theNumberOfActions; i ++) {
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
		GuiObject currentSubmenu1 = NULL, currentSubmenu2 = NULL;
		int writeMenuGoingToSeparate = FALSE;
		#if motif
			int nbuttons = 0, nwriteButtons = 0;
		#endif
		if (! praat_dynamicMenu) {
			#if gtk
				praat_dynamicMenu = gtk_vbutton_box_new ();
				gtk_button_box_set_layout (GTK_BUTTON_BOX (praat_dynamicMenu), GTK_BUTTONBOX_START);
				GuiObject viewport = gtk_bin_get_child (GTK_BIN (praat_dynamicMenuWindow));
				gtk_container_add (GTK_CONTAINER (viewport), praat_dynamicMenu);
			#elif motif
				praat_dynamicMenu = XmCreateRowColumn (praat_dynamicMenuWindow, "menu", NULL, 0);
			#endif
		}
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
					GuiObject parent = my depth > 1 && currentSubmenu2 ? currentSubmenu2 : my depth > 0 && currentSubmenu1 ? currentSubmenu1 : praat_dynamicMenu;
					if (wcsnequ (my title, L"Save ", 5) || wcsnequ (my title, L"Write ", 6) || wcsnequ (my title, L"Append to ", 10)) {
						parent = praat_writeMenu;
						if (! praat_writeMenuSeparator) {
							if (writeMenuGoingToSeparate)
								praat_writeMenuSeparator = GuiMenu_addSeparator (parent);
							else if (wcsequ (my title, L"Save as binary file..."))
								writeMenuGoingToSeparate = TRUE;
						}
					}

					if (parent == praat_dynamicMenu) {
						my button = GuiButton_createShown (praat_dynamicMenu,
							#if defined (_WIN32)
								4, BUTTON_WIDTH - 16, Gui_AUTOMATIC, Gui_AUTOMATIC,
							#elif defined (macintosh)
								/*
								 * Keep 5 pixels distance on both sides for shadow.
								 */
								9, BUTTON_WIDTH - 16, Gui_AUTOMATIC, Gui_AUTOMATIC,
							#else
								Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC, Gui_AUTOMATIC,
							#endif
							my title, gui_button_cb_menu,
							my callback == DO_RunTheScriptFromAnyAddedMenuCommand ? (void *) my script : (void *) my callback,
								( my executable ? 0 : GuiButton_INSENSITIVE ) | ( my attractive ? GuiButton_ATTRACTIVE : 0 ));
						#if gtk
							/* Dit soort onzin zou eigenlijk in GuiButton moeten */
							gtk_button_set_alignment (GTK_BUTTON (my button), 0.0f, 0.5f);
						#endif
					} else {
						my button = GuiMenu_addItem (parent, my title,
							( my executable ? 0 : GuiMenu_INSENSITIVE ),
							cb_menu,
							my callback == DO_RunTheScriptFromAnyAddedMenuCommand ? (void *) my script : (void *) my callback);
					}
				} else if (wcsnequ (my title, L"Save ", 5) || wcsnequ (my title, L"Write ", 6) || wcsnequ (my title, L"Append to ", 10)) {
					if (writeMenuGoingToSeparate) {
						if (! praat_writeMenuSeparator)
							praat_writeMenuSeparator = GuiMenu_addSeparator (praat_writeMenu);
						GuiObject_show (praat_writeMenuSeparator);
					} else if (wcsequ (my title, L"Save as binary file...")) {
						writeMenuGoingToSeparate = TRUE;
					}
					#if motif
						writeButtons [nwriteButtons ++] = my button;
					#endif
					GuiObject_setSensitive (my button, my executable);
				} else {
					GuiObject_setSensitive (my button, my executable);
					if (GuiObject_parent (my button) == praat_dynamicMenu)
						#if gtk
							GuiObject_show (my button);
						#elif motif
							buttons [nbuttons ++] = my button;
						#endif
				}
			} else if (i == theNumberOfActions || theActions [i + 1]. depth == 0) {
				/*
				 * Apparently a labelled separator.
				 */
				if (! my button) {
					my button = GuiLabel_createShown (praat_dynamicMenu, 0, BUTTON_WIDTH - 20, Gui_AUTOMATIC, Gui_AUTOMATIC, my title, 0);
				} else {
					if (GuiObject_parent (my button) == praat_dynamicMenu)
						#if gtk
							GuiObject_show (my button);
						#elif motif
							buttons [nbuttons ++] = my button;
						#endif
				}
			} else if (my title == NULL || my title [0] == '-') {
				/*
				 * Apparently a separator in a submenu.
				 */
				if (currentSubmenu2 || currentSubmenu1) {   /* These separators are not shown in a flattened menu. */
					if (! my button) {
						// RFC: Beter?
						#if gtk
							my button = GuiMenu_addSeparator (currentSubmenu2 ? currentSubmenu2 : currentSubmenu1);
						#elif motif
							my button = XmCreateSeparator (currentSubmenu2 ? currentSubmenu2 : currentSubmenu1, "separator", NULL, 0);
						#endif
						GuiObject_show (my button);
					}
				}
			} else {
				/*
				 * Apparently a submenu.
				 */
				if (! my button) {
					if (my depth == 0) {
						#if gtk
							currentSubmenu1 = GuiMenuBar_addMenu3 (praat_dynamicMenu, my title, 0, & my button);
							/* Dit soort onzin zou eigenlijk in GuiButton moeten */
							gtk_button_set_alignment (GTK_BUTTON (my button), 0.0f, 0.5f);
						#elif motif
							GuiObject cascadeButton;
							my button = XmCreateMenuBar (praat_dynamicMenu, "dynamicSubmenuBar", 0, 0);
							currentSubmenu1 = GuiMenuBar_addMenu2 (my button, my title, 0, & cascadeButton);
						#endif

						#if defined (_WIN32)
							GuiObject_size (cascadeButton, BUTTON_WIDTH - 24, 19);
							GuiObject_size (my button, BUTTON_WIDTH - 20, 21);
						#elif defined (macintosh)
							GuiObject_size (cascadeButton, BUTTON_WIDTH - 29, 19);
							GuiObject_size (my button, BUTTON_WIDTH - 25, 22);
						#endif
					} else {
						currentSubmenu2 = GuiMenuBar_addMenu2 (currentSubmenu1 ? currentSubmenu1 : praat_dynamicMenu, my title, 0, & my button);
					}
					GuiObject_show (my button);
				} else {
					if (GuiObject_parent (my button) == praat_dynamicMenu)
						#if gtk
							GuiObject_show (my button);
						#elif motif
							buttons [nbuttons++] = my button;
						#endif
				}
			}
		}
		#if motif
			if (nbuttons) XtManageChildren (buttons, nbuttons);   // multiple show
			if (nwriteButtons) XtManageChildren (writeButtons, nwriteButtons);   // multiple show
		#endif
		GuiObject_show (praat_dynamicMenu);
	}
}

void praat_actions_createWriteMenu (GuiObject bar) {
	if (theCurrentPraatApplication -> batch) return;
	#if gtk
		praat_writeMenu = GuiMenuBar_addMenu2 (bar, L"Save", GuiMenu_INSENSITIVE, & praat_writeMenuTitle);
	#elif motif
		praat_writeMenuTitle = XtVaCreateManagedWidget ("Save", xmCascadeButtonWidgetClass, bar, NULL);
		praat_writeMenu = XmCreatePulldownMenu (bar, "Save", NULL, 0);   // the name is irrelevant
		XtVaSetValues (praat_writeMenuTitle, XmNsubMenuId, praat_writeMenu, NULL);
	#endif
	GuiObject_setSensitive (praat_writeMenuTitle, False);
}

void praat_actions_init (void) {
	theActions = Melder_calloc_f (struct structPraat_Command, 1 + praat_MAXNUM_LOOSE_COMMANDS);
}

void praat_actions_createDynamicMenu (GuiObject form, int width) {
	if (theCurrentPraatApplication -> batch) return;
	// Kan dit bovenstaande niet met een #if constructie?
	// Wat doet dit?
	// Dit maakt de buitenkant van de dynamische knoppenlijst (Sound help, Edit, Draw, Modify...):
	// een scrolledWindow met daarin een kolom.
	#if gtk
		praat_dynamicMenu = gtk_vbutton_box_new ();
		gtk_button_box_set_layout (GTK_BUTTON_BOX (praat_dynamicMenu), GTK_BUTTONBOX_START);

		/* Oh yes, we are lazy */
		praat_dynamicMenuWindow = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (praat_dynamicMenuWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (praat_dynamicMenuWindow), praat_dynamicMenu);
		gtk_container_add (GTK_CONTAINER (form), praat_dynamicMenuWindow);
		gtk_widget_set_size_request (praat_dynamicMenuWindow, width, -1);
	#elif motif
		praat_dynamicMenuWindow = XmCreateScrolledWindow (form, "menuWindow", NULL, 0);
		#if defined (macintosh)
			XtVaSetValues (praat_dynamicMenuWindow,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMainWindowMenuBarHeight (),
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, -1,
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, -1,
				XmNwidth, width,
				NULL);
		#elif defined (UNIX)
			(void) width;
			XtVaSetValues (praat_dynamicMenuWindow,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMainWindowMenuBarHeight (),
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 0,
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 0,
				XmNwidth, width,
				NULL);
		#else
			XtVaSetValues (praat_dynamicMenuWindow,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, -3,
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, -3,
				XmNwidth, width,
				NULL);
		#endif
		praat_dynamicMenu = XmCreateRowColumn (praat_dynamicMenuWindow, "menu", NULL, 0);
	#endif
	GuiObject_show (praat_dynamicMenu);
	GuiObject_show (praat_dynamicMenuWindow);
}

void praat_saveAddedActions (FILE *f) {
	long i, id, maxID = 0;
	for (i = 1; i <= theNumberOfActions; i ++) if (theActions [i]. uniqueID > maxID) maxID = theActions [i]. uniqueID;
	for (id = 1; id <= maxID; id ++)
		for (i = 1; i <= theNumberOfActions; i ++) {
			praat_Command me = & theActions [i];
			if (my uniqueID == id && ! my hidden && my title) {
				fwprintf (f, L"Add action command... %ls %d %ls %d %ls %d \"%ls\" \"%ls\" %d %ls\n",
					((Data_Table) my class1) -> _className, my n1,
					my class2 ? ((Data_Table) my class2) -> _className : L"\"\"", my n2,
					my class3 ? ((Data_Table) my class3) -> _className : L"\"\"", my n3,
					my title, my after ? my after : L"", my depth, my script ? my script : L"");
				break;
			}
		}
	for (i = 1; i <= theNumberOfActions; i ++) {
		praat_Command me = & theActions [i];
		if (my toggled && my title && ! my uniqueID && ! my script)
			fwprintf (f, L"%ls action command... %ls %ls %ls %ls\n",
				my hidden ? L"Hide" : L"Show",
				((Data_Table) my class1) -> _className,
				my class2 ? ((Data_Table) my class2) -> _className : L"\"\"",
				my class3 ? ((Data_Table) my class3) -> _className : L"\"\"",
				my title);
	}
}

int praat_doAction (const wchar *command, const wchar *arguments, Interpreter interpreter) {
	long i = 1;
	while (i <= theNumberOfActions && (! theActions [i]. executable || wcscmp (theActions [i]. title, command))) i ++;
	if (i > theNumberOfActions) return 0;   /* Not found. */
	theActions [i]. callback (NULL, arguments, interpreter, NULL, false, NULL); therror
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

/* End of file praat_actions.c */
