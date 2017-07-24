/* praat_actions.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016,2017 Paul Boersma
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

#include "praatP.h"
#include "praat_script.h"
#include "longchar.h"
#include "machine.h"
#include "GuiP.h"

#define BUTTON_LEFT  -240
#define BUTTON_RIGHT -5

static OrderedOf <structPraat_Command> theActions;
static GuiMenu praat_writeMenu;
static GuiMenuItem praat_writeMenuSeparator;
static GuiForm praat_form;
static bool actionsInvisible = false;

static void fixSelectionSpecification (ClassInfo *class1, int *n1, ClassInfo *class2, int *n2, ClassInfo *class3, int *n3) {
/*
 * Function:
 *	sort the specification pairs *class(i), *n(i) according to class name, with null classes at the end.
 * Postconditions:
 *	if (*class2) !! *class1;
 *	if (*class3) !! *class2;
 *	(*class1) -> className <= (*class2) -> className <= (*class3) -> className;
 * Usage:
 *	Called by praat_addAction () and praat_removeAction ().
 */

	/* Fix unusual input bubblewise. */

	if (! *class1 && *class2) { *class1 = *class2; *n1 = *n2; *class2 = nullptr; *n2 = 0; }
	if (! *class2 && *class3) { *class2 = *class3; *n2 = *n3; *class3 = nullptr; *n3 = 0;
		if (! *class1 && *class2) { *class1 = *class2; *n1 = *n2; *class2 = nullptr; *n2 = 0; } }

	/* Now: if *class3, then *class2, and if *class2, then *class1.
	 * Bubble-sort the input by class name.
	 */
	if (*class2 && str32cmp ((*class1) -> className, (*class2) -> className) > 0) {
		ClassInfo helpClass1 = *class1; *class1 = *class2; *class2 = helpClass1;
		int helpN1 = *n1; *n1 = *n2; *n2 = helpN1;
	}
	if (*class3 && str32cmp ((*class2) -> className, (*class3) -> className) > 0) {
		ClassInfo helpClass2 = *class2; *class2 = *class3; *class3 = helpClass2;
		int helpN2 = *n2; *n2 = *n3; *n3 = helpN2;
		if (str32cmp ((*class1) -> className, (*class2) -> className) > 0) {
			ClassInfo helpClass1 = *class1; *class1 = *class2; *class2 = helpClass1;
			int helpN1 = *n1; *n1 = *n2; *n2 = helpN1;
		}
	}
}

static long lookUpMatchingAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, ClassInfo class4, const char32 *title) {
/*
 * An action command is fully specified by its environment (the selected classes) and its title.
 * Precondition:
 *	class1, class2, and class3 must be in sorted order.
 */
	for (long i = 1; i <= theActions.size; i ++) {
		Praat_Command action = theActions.at [i];
		if (class1 == action -> class1 && class2 == action -> class2 &&
		    class3 == action -> class3 && class4 == action -> class4 &&
		    title && action -> title && str32equ (action -> title, title)) return i;
	}
	return 0;   // not found
}

void praat_addAction1_ (ClassInfo class1, int n1,
	const char32 *title, const char32 *after, unsigned long flags, UiCallback callback, const char32 *nameOfCallback)
{ praat_addAction4_ (class1, n1, nullptr, 0, nullptr, 0, nullptr, 0, title, after, flags, callback, nameOfCallback); }

void praat_addAction2_ (ClassInfo class1, int n1, ClassInfo class2, int n2,
	const char32 *title, const char32 *after, unsigned long flags, UiCallback callback, const char32 *nameOfCallback)
{ praat_addAction4_ (class1, n1, class2, n2, nullptr, 0, nullptr, 0, title, after, flags, callback, nameOfCallback); }

void praat_addAction3_ (ClassInfo class1, int n1, ClassInfo class2, int n2, ClassInfo class3, int n3,
	const char32 *title, const char32 *after, unsigned long flags, UiCallback callback, const char32 *nameOfCallback)
{ praat_addAction4_ (class1, n1, class2, n2, class3, n3, nullptr, 0, title, after, flags, callback, nameOfCallback); }

void praat_addAction4_ (ClassInfo class1, int n1, ClassInfo class2, int n2, ClassInfo class3, int n3, ClassInfo class4, int n4,
	const char32 *title, const char32 *after, unsigned long flags, UiCallback callback, const char32 *nameOfCallback)
{
	try {
		int depth = flags, key = 0;
		bool unhidable = false, hidden = false, attractive = false;
		unsigned long guiFlags = 0;
		if (flags > 7) {
			depth = ((flags & praat_DEPTH_7) >> 16);
			unhidable = (flags & praat_UNHIDABLE) != 0;
			hidden = (flags & praat_HIDDEN) != 0 && ! unhidable;
			key = flags & 0x000000FF;
			guiFlags = key ? flags & (0x000000FF | GuiMenu_SHIFT | GuiMenu_BUTTON_STATE_MASK) : flags & GuiMenu_BUTTON_STATE_MASK;
			attractive = (guiFlags & praat_ATTRACTIVE) != 0;
		}
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);

		if (callback && ! title)
			Melder_throw (U"An action command with callback has no title. Classes: ",
				class1 ? class1 -> className : U"", U" ",
				class2 ? class2 -> className : U"", U" ",
				class3 ? class3 -> className : U"", U" ",
				class4 ? class4 -> className : U"", U".");

		if (! class1)
			Melder_throw (U"The action command \"", title, U"\" has no first class.");

		/*
		 * Determine the position of the new command.
		 */
		long position;
		if (after && after [0] != U'*') {   // search for existing command with same selection
			long found = lookUpMatchingAction (class1, class2, class3, class4, after);
			if (found == 0)
				Melder_throw (U"The action command \"", title, U"\" cannot be put after \"", after, U"\",\n"
					U"because the latter command does not exist.");
			position = found + 1;   // after 'after'
		} else {
			position = theActions.size + 1;   // at end
		}

		/*
		 * Make new command.
		 */
		autoPraat_Command action = Thing_new (Praat_Command);
		action -> class1 = class1;
		action -> n1 = n1;
		action -> class2 = class2;
		action -> n2 = n2;
		action -> class3 = class3;
		action -> n3 = n3;
		action -> class4 = class4;
		action -> n4 = n4;
		action -> title = Melder_dup_f (title);
		action -> depth = depth;
		action -> callback = callback;   // null for a separator
		action -> nameOfCallback = nameOfCallback;
		action -> button = nullptr;
		action -> script = nullptr;
		action -> hidden = hidden;
		action -> unhidable = unhidable;
		action -> attractive = attractive;

		/*
		 * Insert new command.
		 */
		theActions. addItemAtPosition_move (action.move(), position);
	} catch (MelderError) {
		Melder_flushError ();
	}
}

static void deleteDynamicMenu () {
	if (praatP.phase != praat_HANDLING_EVENTS) return;
	if (actionsInvisible) return;
	static long numberOfDeletions;
	trace (U"deletion #", ++ numberOfDeletions);
	for (long i = 1; i <= theActions.size; i ++) {
		Praat_Command action = theActions.at [i];
		if (action -> button) {
			trace (U"trying to destroy action ", i, U" of ", theActions.size, U": ", action -> title);
			#if gtk || cocoa
				if (action -> button -> d_parent == praat_form) {
					trace (U"destroy a label or a push button or a cascade button");
					GuiObject_destroy (action -> button -> d_widget);
				} else if (praat_writeMenu && action -> button -> d_parent == praat_writeMenu) {
					trace (U"destroying Save menu item");
					GuiObject_destroy (action -> button -> d_widget);
				}
			#elif motif
				if (action -> button -> classInfo == classGuiButton && action -> button -> d_widget -> subMenuId) {   // a cascade button (not a direct child of the form)?
					trace (U"destroy the xm menu bar; this also destroys the xm button and the xm menu");
					GuiObject_destroy (action -> button -> d_widget -> parent);   // the Motif parent, i.e. not d_parent -> d_widget !
				} else if (action -> button -> d_parent == praat_form) {
					trace (U"destroy a label or a push button");
					GuiObject_destroy (action -> button -> d_widget);
				}
			#endif
			action -> button = nullptr;   // undangle
		}
	}
	if (praat_writeMenu) {
		#if gtk || cocoa
			if (praat_writeMenuSeparator) {
				trace (U"destroy the Save menu separator");
				GuiObject_destroy (praat_writeMenuSeparator -> d_widget);
			}
			//praat_writeMenu -> f_empty ();
		#elif motif
			GuiObject_destroy (praat_writeMenu -> d_xmMenuTitle);
			GuiObject_destroy (praat_writeMenu -> d_widget);
			praat_writeMenu = GuiMenu_createInWindow (praatP.menuBar, U"Save", 0);
		#endif
		praat_writeMenuSeparator = nullptr;   // undangle
	}
	actionsInvisible = true;
}

static void updateDynamicMenu () {
	if (praatP.phase != praat_HANDLING_EVENTS) return;
	praat_sortActions ();
	deleteDynamicMenu ();
	praat_show ();
}

void praat_addActionScript (const char32 *className1, int n1, const char32 *className2, int n2, const char32 *className3, int n3,
	const char32 *title, const char32 *after, int depth, const char32 *script)
{
	try {
		ClassInfo class1 = nullptr, class2 = nullptr, class3 = nullptr;
		Melder_assert (className1 && className2 && className3 && title && after && script);
		if (str32len (className1)) {
			class1 = Thing_classFromClassName (className1, nullptr);
		}
		if (str32len (className2)) {
			class2 = Thing_classFromClassName (className2, nullptr);
		}
		if (str32len (className3)) {
			class3 = Thing_classFromClassName (className3, nullptr);
		}
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);

		if (str32len (script) && ! str32len (title))
			Melder_throw (U"Command with callback has no title. Classes: ", className1, U" ", className2, U" ", className3, U".");

		if (! str32len (className1))
			Melder_throw (U"Command \"", title, U"\" has no first class.");

		/*
		 * If the button already exists, remove it.
		 */
		long found = lookUpMatchingAction (class1, class2, class3, nullptr, title);
		if (found) {
			theActions. removeItem (found);
		}

		/*
		 * Determine the position of the new command.
		 */
		long position;
		if (str32len (after)) {   // search for existing command with same selection
			long found = lookUpMatchingAction (class1, class2, class3, nullptr, after);
			if (found) {
				position = found + 1;   // after 'after'
			} else {
				position = theActions.size + 1;   // at end
			}
		} else {
			position = theActions.size + 1;   // at end
		}

		/*
		 * Create new command.
		 */
		autoPraat_Command action = Thing_new (Praat_Command);
		action -> class1 = class1;
		action -> n1 = n1;
		action -> class2 = class2;
		action -> n2 = n2;
		action -> class3 = class3;
		action -> n3 = n3;
		action -> title = str32len (title) ? Melder_dup_f (title) : nullptr;   // allow old-fashioned untitled separators
		action -> depth = depth;
		action -> callback = str32len (script) ? DO_RunTheScriptFromAnyAddedMenuCommand : nullptr;   // null for a separator
		action -> button = nullptr;
		if (str32len (script) == 0) {
			action -> script = nullptr;
		} else {
			structMelderFile file { };
			Melder_relativePathToFile (script, & file);
			action -> script = Melder_dup_f (Melder_fileToPath (& file));
		}
		action -> after = str32len (after) ? Melder_dup_f (after) : nullptr;
		action -> phase = praatP.phase;
		if (praatP.phase >= praat_READING_BUTTONS) {
			static long uniqueID = 0;
			action -> uniqueID = ++ uniqueID;
		}

		/*
		 * Insert new command.
		 */
		theActions. addItemAtPosition_move (action.move(), position);
		updateDynamicMenu ();
	} catch (MelderError) {
		Melder_throw (U"Praat: script action not added.");
	}
}

void praat_removeAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const char32 *title) {
	try {
		int n1, n2, n3;
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
		long found = lookUpMatchingAction (class1, class2, class3, nullptr, title);
		if (! found) {
			Melder_throw (U"Action command \"", class1 -> className,
				class2 ? U" & ": U"", class2 -> className,
				class3 ? U" & ": U"", class3 -> className,
				U": ", title, U"\" not found.");
		}
		theActions. removeItem (found);
	} catch (MelderError) {
		Melder_throw (U"Praat: action not removed.");
	}
}

void praat_removeAction_classNames (const char32 *className1, const char32 *className2,
	const char32 *className3, const char32 *title)
{
	try {
		ClassInfo class1 = nullptr, class2 = nullptr, class3 = nullptr;
		Melder_assert (className1 && className2 && className3 && title);
		if (str32len (className1)) {
			class1 = Thing_classFromClassName (className1, nullptr);
		}
		if (str32len (className2)) {
			class2 = Thing_classFromClassName (className2, nullptr);
		}
		if (str32len (className3)) {
			class3 = Thing_classFromClassName (className3, nullptr);
		}
		praat_removeAction (class1, class2, class3, title);
		updateDynamicMenu ();
	} catch (MelderError) {
		Melder_throw (U"Praat: action not removed.");
	}
}

void praat_hideAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const char32 *title) {
	try {
		int n1, n2, n3;
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
		long found = lookUpMatchingAction (class1, class2, class3, nullptr, title);
		if (! found) {
			Melder_throw (U"Praat: action command \"", class1 ? class1 -> className : nullptr,
				class2 ? U" & ": nullptr, class2 ? class2 -> className : nullptr,
				class3 ? U" & ": nullptr, class3 ? class3 -> className : nullptr,
				U": ", title, U"\" not found.");
		}
		Praat_Command action = theActions.at [found];
		if (! action -> hidden) {
			action -> hidden = true;
			if (praatP.phase >= praat_READING_BUTTONS) action -> toggled = ! action -> toggled;
			updateDynamicMenu ();
		}
	} catch (MelderError) {
		Melder_throw (U"Praat: action not hidden.");
	}
}

void praat_hideAction_classNames (const char32 *className1, const char32 *className2,
	const char32 *className3, const char32 *title)
{
	try {
		ClassInfo class1 = nullptr, class2 = nullptr, class3 = nullptr;
		Melder_assert (className1 && className2 && className3 && title);
		if (str32len (className1)) {
			class1 = Thing_classFromClassName (className1, nullptr);
		}
		if (str32len (className2)) {
			class2 = Thing_classFromClassName (className2, nullptr);
		}
		if (str32len (className3)) {
			class3 = Thing_classFromClassName (className3, nullptr);
		}
		praat_hideAction (class1, class2, class3, title);
	} catch (MelderError) {
		Melder_throw (U"Praat: action not hidden.");
	}
}

void praat_showAction (ClassInfo class1, ClassInfo class2, ClassInfo class3, const char32 *title) {
	try {
		int n1, n2, n3;
		fixSelectionSpecification (& class1, & n1, & class2, & n2, & class3, & n3);
		long found = lookUpMatchingAction (class1, class2, class3, nullptr, title);
		if (! found) {
			Melder_throw (U"Action command \"", class1 ? class1 -> className : nullptr,
				class2 ? U" & ": nullptr, class2 ? class2 -> className : nullptr,
				class3 ? U" & ": nullptr, class3 ? class3 -> className : nullptr,
				U": ", title, U"\" not found.");
		}
		Praat_Command action = theActions.at [found];
		if (action -> hidden) {
			action -> hidden = false;
			if (praatP.phase >= praat_READING_BUTTONS) action -> toggled = ! action -> toggled;
			updateDynamicMenu ();
		}
	} catch (MelderError) {
		Melder_throw (U"Praat: action not shown.");
	}
}

void praat_showAction_classNames (const char32 *className1, const char32 *className2,
	const char32 *className3, const char32 *title)
{
	try {
		ClassInfo class1 = nullptr, class2 = nullptr, class3 = nullptr;
		Melder_assert (className1 && className2 && className3 && title);
		if (str32len (className1)) {
			class1 = Thing_classFromClassName (className1, nullptr);
		}
		if (str32len (className2)) {
			class2 = Thing_classFromClassName (className2, nullptr);
		}
		if (str32len (className3)) {
			class3 = Thing_classFromClassName (className3, nullptr);
		}
		praat_showAction (class1, class2, class3, title);
	} catch (MelderError) {
		Melder_throw (U"Praat: action not shown.");
	}
}

static int compareActions (const void *void_me, const void *void_thee) {
	Praat_Command me = * (Praat_Command *) void_me, thee = * (Praat_Command *) void_thee;
	int compare;
	compare = str32cmp (my class1 -> className, thy class1 -> className);
	if (compare) return my class1 == classDaata ? -1 : thy class1 == classDaata ? 1 : compare;
	if (my class2) {
		if (! thy class2) return 1;
		compare = str32cmp (my class2 -> className, thy class2 -> className);
		if (compare) return compare;
	} else if (thy class2) return -1;
	if (my class3) {
		if (! thy class3) return 1;
		compare = str32cmp (my class3 -> className, thy class3 -> className);
		if (compare) return compare;
	} else if (thy class3) return -1;
	if (my sortingTail < thy sortingTail) return -1;
	return 1;
}

void praat_sortActions () {
	for (long i = 1; i <= theActions.size; i ++) {
		Praat_Command action = theActions.at [i];
		action -> sortingTail = i;
	}
	qsort (& theActions.at [1], theActions.size, sizeof (Praat_Command), compareActions);
}

static const char32 *numberString (int number) {
	return number == 1 ? U"one" : number == 2 ? U"two" : number == 3 ? U"three" : U"any number of";
}
static const char32 *classString (ClassInfo klas) {
	return klas == classDaata ? U"" : klas -> className;
}
static const char32 *objectString (int number) {
	return number == 1 ? U"object" : U"objects";
}
static bool allowExecutionHook (void *closure) {
	UiCallback callback = (UiCallback) closure;
	Melder_assert (sizeof (callback) == sizeof (void *));
	long numberOfMatchingCallbacks = 0, firstMatchingCallback = 0;
	for (long i = 1; i <= theActions.size; i ++) {
		Praat_Command me = theActions.at [i];
		if (my callback == callback) {
			int sel1, sel2 = 0, sel3 = 0, sel4 = 0;
			if (! my class1) Melder_throw (U"No class1???");
			numberOfMatchingCallbacks += 1;
			if (! firstMatchingCallback) firstMatchingCallback = i;
			sel1 = my class1 == classDaata ? theCurrentPraatObjects -> totalSelection : praat_numberOfSelected (my class1);
			if (sel1 == 0) continue;
			if (my class2 && (sel2 = praat_numberOfSelected (my class2)) == 0) continue;
			if (my class3 && (sel3 = praat_numberOfSelected (my class3)) == 0) continue;
			if (my class4 && (sel4 = praat_numberOfSelected (my class4)) == 0) continue;
			if (sel1 + sel2 + sel3 + sel4 != theCurrentPraatObjects -> totalSelection) continue;
			if ((my n1 && sel1 != my n1) || (my n2 && sel2 != my n2) || (my n3 && sel3 != my n3) || (my n4 && sel4 != my n4)) continue;
			return true;   // found a matching action
		}
	}
	if (numberOfMatchingCallbacks == 1) {
		Praat_Command me = theActions.at [firstMatchingCallback];
		Melder_appendError (U"Selection changed! It should be:");
		if (my class1) Melder_appendError (U"   ", numberString (my n1), U" ", classString (my class1), U" ", objectString (my n1));
		if (my class2) Melder_appendError (U"   ", numberString (my n2), U" ", classString (my class2), U" ", objectString (my n2));
		if (my class3) Melder_appendError (U"   ", numberString (my n3), U" ", classString (my class3), U" ", objectString (my n3));
		if (my class4) Melder_appendError (U"   ", numberString (my n4), U" ", classString (my class4), U" ", objectString (my n4));
		throw MelderError ();
	} else {
		Melder_throw (U"Selection changed!");
	}
	return false;
}

static void do_menu (Praat_Command me, bool modified) {
	if (my callback == DO_RunTheScriptFromAnyAddedMenuCommand) {
		UiHistory_write (U"\nrunScript: ");
		try {
			DO_RunTheScriptFromAnyAddedMenuCommand (nullptr, 0, nullptr, my script, nullptr, nullptr, false, nullptr);
		} catch (MelderError) {
			Melder_flushError (U"Command \"", my title, U"\" not executed.");
		}
		praat_updateSelection (); return;
	} else {
		if (my title && ! str32str (my title, U"...")) {
			UiHistory_write (U"\n");
			UiHistory_write (my title);
		}
		Ui_setAllowExecutionHook (allowExecutionHook, (void *) my callback);   // BUG: one shouldn't assign a function pointer to a void pointer
		try {
			my callback (nullptr, 0, nullptr, nullptr, nullptr, my title, modified, nullptr);
		} catch (MelderError) {
			Melder_flushError (U"Command \"", my title, U"\" not executed.");
		}
		Ui_setAllowExecutionHook (nullptr, nullptr);
		praat_updateSelection (); return;
	}
}

static void cb_menu (Praat_Command me, GuiMenuItemEvent event) {
	bool modified = event -> shiftKeyPressed || event -> commandKeyPressed || event -> optionKeyPressed || event -> extraControlKeyPressed;
	do_menu (me, modified);
}

static void gui_button_cb_menu (Praat_Command me, GuiButtonEvent event) {
	do_menu (me, event -> shiftKeyPressed | event -> commandKeyPressed | event -> optionKeyPressed | event -> extraControlKeyPressed);
}

void praat_actions_show () {
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
			GuiThing_setSensitive (praat_writeMenu, false);
			if (praat_writeMenuSeparator) GuiThing_hide (praat_writeMenuSeparator);
		}

		/* Determine the visibility and sensitivity of all the actions.
		 */
		if (theCurrentPraatObjects -> totalSelection != 0 && ! Melder_backgrounding)
			GuiThing_setSensitive (praat_writeMenu, true);
	}
	for (long i = 1; i <= theActions.size; i ++) {
		Praat_Command action = theActions.at [i];
		int sel1 = 0, sel2 = 0, sel3 = 0, sel4 = 0;
		int n1 = action -> n1, n2 = action -> n2, n3 = action -> n3, n4 = action -> n4;

		/* Clean up from previous selection. */

		action -> visible = false;
		action -> executable = false;

		/* Match the actually selected classes with the selection required for this visibility. */

		if (! action -> class1) continue;   // at least one class selected
		sel1 = action -> class1 == classDaata ? theCurrentPraatObjects -> totalSelection : praat_numberOfSelected (action -> class1);
		if (sel1 == 0) continue;
		if (action -> class2 && (sel2 = praat_numberOfSelected (action -> class2)) == 0) continue;
		if (action -> class3 && (sel3 = praat_numberOfSelected (action -> class3)) == 0) continue;
		if (action -> class4 && (sel4 = praat_numberOfSelected (action -> class4)) == 0) continue;
		if (sel1 + sel2 + sel3 + sel4 != theCurrentPraatObjects -> totalSelection) continue;   // other classes selected? Do not show
		action -> visible = ! action -> hidden;

		/* Match the actually selected objects with the selection required for this action. */

		if (! action -> callback) continue;   // separators are not executable
		if ((n1 && sel1 != n1) || (n2 && sel2 != n2) || (n3 && sel3 != n3) || (n4 && sel4 != n4)) continue;
		action -> executable = true;
	}

	/* Create a new column of buttons in the dynamic menu. */
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding) {
		actionsInvisible = false;
		GuiMenu currentSubmenu1 = nullptr, currentSubmenu2 = nullptr;
		bool writeMenuGoingToSeparate = false;
		int y = Machine_getMenuBarHeight () + 10;
		for (long i = 1; i <= theActions.size; i ++) {   // add buttons or make existing buttons sensitive (executable)
			Praat_Command me = theActions.at [i];
			if (my depth == 0) currentSubmenu1 = nullptr, currentSubmenu2 = nullptr;   // prevent attachment of later deep actions to earlier submenus after removal of label
			if (my depth == 1) currentSubmenu2 = nullptr;   // prevent attachment of later deep actions to earlier submenus after removal of label
			if (! my visible) continue;
			if (my callback) {
				/* Apparently a true command: create a button in the dynamic menu.
				 * If it is a subcommand (depth > 0), put it in the current submenu,
				 * but only if this exists (umbrella against stray submenu specifications).
				 */
				GuiMenu parentMenu = my depth > 1 && currentSubmenu2 ? currentSubmenu2 : my depth > 0 && currentSubmenu1 ? currentSubmenu1 : nullptr;

				if (str32nequ (my title, U"Save ", 5) || str32nequ (my title, U"Write ", 6) || str32nequ (my title, U"Append to ", 10)) {
					parentMenu = praat_writeMenu;
					if (! praat_writeMenuSeparator) {
						if (writeMenuGoingToSeparate)
							praat_writeMenuSeparator = GuiMenu_addSeparator (parentMenu);
						else if (str32equ (my title, U"Save as binary file..."))
							writeMenuGoingToSeparate = true;
					}
				}
				if (parentMenu) {
					my button = GuiMenu_addItem (parentMenu, my title,
						( my executable ? 0 : GuiMenu_INSENSITIVE ),
						cb_menu, me);
				} else {
					my button = GuiButton_createShown (praat_form,
						BUTTON_LEFT, BUTTON_RIGHT, y, y + Gui_PUSHBUTTON_HEIGHT,
						my title, gui_button_cb_menu,
						me,
							( my executable ? 0 : GuiButton_INSENSITIVE ) | ( my attractive ? GuiButton_ATTRACTIVE : 0 ));
					y += Gui_PUSHBUTTON_HEIGHT + BUTTON_VSPACING;
					#if gtk
						/* Dit soort onzin zou eigenlijk in GuiButton moeten */
						gtk_button_set_alignment (GTK_BUTTON (my button -> d_widget), 0.0f, 0.5f);
					#endif
				}
			} else if (i == theActions.size || theActions.at [i + 1] -> depth == 0) {
				/*
				 * Apparently a labelled separator.
				 */
				my button = GuiLabel_createShown (praat_form, BUTTON_LEFT, BUTTON_RIGHT, y, y + Gui_LABEL_HEIGHT, my title, 0);
				y += Gui_LABEL_HEIGHT + BUTTON_VSPACING;
			} else if (! my title || my title [0] == U'-') {
				/*
				 * Apparently a separator in a submenu.
				 */
				if (currentSubmenu2 || currentSubmenu1) {   // these separators are not shown in a flattened menu
					my button = GuiMenu_addSeparator (currentSubmenu2 ? currentSubmenu2 : currentSubmenu1);
					GuiThing_show (my button);
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
					my button = currentSubmenu1 -> d_cascadeButton.get();
				} else {
					currentSubmenu2 = GuiMenu_createInMenu (currentSubmenu1, my title, 0);
					my button = currentSubmenu2 -> d_menuItem.get();
				}
				GuiThing_show (my button);
			}
		}
	}
}

void praat_actions_createWriteMenu (GuiWindow window) {
	if (theCurrentPraatApplication -> batch) return;
	praat_writeMenu = GuiMenu_createInWindow (window, U"Save", GuiMenu_INSENSITIVE);
	#if gtk
		GuiMenu_addSeparator (praat_writeMenu);
	#endif
}

void praat_actions_init () {
}

void praat_actions_createDynamicMenu (GuiWindow window) {
	if (theCurrentPraatApplication -> batch) return;
	praat_form = window;
}

void praat_saveAddedActions (MelderString *buffer) {
	long maxID = 0;
	for (long iaction = 1; iaction <= theActions.size; iaction ++) {
		Praat_Command action = theActions.at [iaction];
		if (action -> uniqueID > maxID)
			maxID = action -> uniqueID;
	}
	for (long ident = 1; ident <= maxID; ident ++)
		for (long iaction = 1; iaction <= theActions.size; iaction ++) {
			Praat_Command me = theActions.at [iaction];
			if (my uniqueID == ident && ! my hidden && my title) {
				MelderString_append (buffer, U"Add action command...",
					U" ", my class1 -> className, U" ", my n1,
					U" ", ( my class2 ? my class2 -> className : U"\"\"" ), U" ", my n2,
					U" ", ( my class3 ? my class3 -> className : U"\"\"" ), U" ", my n3,
					U" \"", my title, U"\" \"", ( my after ? my after : U"" ), U"\" ", my depth);
				MelderString_append (buffer, U" ", my script ? my script : U"", U"\n");
				break;
			}
		}
	for (long iaction = 1; iaction <= theActions.size; iaction ++) {
		Praat_Command me = theActions.at [iaction];
		if (my toggled && my title && ! my uniqueID && ! my script) {
			MelderString_append (buffer, ( my hidden ? U"Hide" : U"Show" ), U" action command...",
				U" ", my class1 -> className,
				U" ", ( my class2 ? my class2 -> className : U"\"\"" ),
				U" ", ( my class3 ? my class3 -> className : U"\"\"" ),
				U" ", my title, U"\n");
		}
	}
}

int praat_doAction (const char32 *command, const char32 *arguments, Interpreter interpreter) {
	long i = 1;
	while (i <= theActions.size && (! theActions.at [i] -> executable || str32cmp (theActions.at [i] -> title, command))) i ++;
	if (i > theActions.size) return 0;   // not found
	theActions.at [i] -> callback (nullptr, 0, nullptr, arguments, interpreter, command, false, nullptr);
	return 1;
}

int praat_doAction (const char32 *command, int narg, Stackel args, Interpreter interpreter) {
	long i = 1;
	while (i <= theActions.size && (! theActions.at [i] -> executable || str32cmp (theActions.at [i] -> title, command))) i ++;
	if (i > theActions.size) return 0;   // not found
	theActions.at [i] -> callback (nullptr, narg, args, nullptr, interpreter, command, false, nullptr);
	return 1;
}

long praat_getNumberOfActions () { return theActions.size; }

Praat_Command praat_getAction (long i)
	{ return i < 0 || i > theActions.size ? nullptr : theActions.at [i]; }

void praat_background () {
	if (Melder_batch) return;
	if (Melder_backgrounding) return;
	deleteDynamicMenu ();
	praat_list_background ();
	Melder_backgrounding = true;
	if (! praatP.dontUsePictureWindow) praat_picture_background ();
}

void praat_foreground () {
	if (Melder_batch) return;
	if (! Melder_backgrounding) return;
	Melder_backgrounding = false;
	praat_list_foreground ();
	praat_show ();
	if (! praatP.dontUsePictureWindow) praat_picture_foreground ();
}

/* End of file praat_actions.cpp */
