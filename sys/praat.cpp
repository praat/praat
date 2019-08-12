/* praat.cpp
 *
 * Copyright (C) 1992-2019 Paul Boersma
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

#include "melder.h"
#include <ctype.h>
#include <stdarg.h>
#if defined (UNIX) || defined (macintosh)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <signal.h>
#endif
#include <locale.h>
#if defined (UNIX)
	#include <unistd.h>
#endif
#if defined (_WIN32)
	#include <windows.h>
	#include <fcntl.h>
	#include <io.h>
#endif

#include "praatP.h"
#include "praat_script.h"
#include "praat_version.h"
#include "site.h"
#include "machine.h"
#include "Printer.h"
#include "ScriptEditor.h"
#include "Strings_.h"
#include "../kar/UnicodeData.h"
#include "InfoEditor.h"

#if gtk
	#include <gdk/gdkx.h>
#endif

Thing_implement (Praat_Command, Thing, 0);

#define EDITOR  theCurrentPraatObjects -> list [IOBJECT]. editors

#define WINDOW_WIDTH 520
#define WINDOW_HEIGHT 700

structPraatApplication theForegroundPraatApplication;
PraatApplication theCurrentPraatApplication = & theForegroundPraatApplication;
structPraatObjects theForegroundPraatObjects;
PraatObjects theCurrentPraatObjects = & theForegroundPraatObjects;
structPraatPicture theForegroundPraatPicture;
PraatPicture theCurrentPraatPicture = & theForegroundPraatPicture;
struct PraatP praatP;
static char32 programName [64];
static structMelderDir homeDir { };

/*
 * praatDir: a folder containing preferences file, buttons file, message files, tracing file, plugins.
 *    Unix:   /home/miep/.praat-dir   (without slash)
 *    Windows XP/Vista/7/8/10:   \\myserver\myshare\Miep\Praat
 *                         or:   C:\Users\Miep\Praat
 *    MacOS:   /Users/Miep/Library/Preferences/Praat Prefs
 */
extern structMelderDir praatDir;
structMelderDir praatDir { };

/*
 * prefsFile: preferences file.
 *    Unix:   /home/miep/.praat-dir/prefs5
 *    Windows XP/Vista/7/8/10:   \\myserver\myshare\Miep\Praat\Preferences5.ini
 *                         or:   C:\Users\Miep\Praat\Preferences5.ini
 *    MacOS:   /Users/Miep/Library/Preferences/Praat Prefs/Prefs5
 */
static structMelderFile prefsFile { };

/*
 * buttonsFile: buttons file.
 *    Unix:   /home/miep/.praat-dir/buttons
 *    Windows XP/Vista/7/8/10:   \\myserver\myshare\Miep\Praat\Buttons5.ini
 *                         or:   C:\Users\Miep\Praat\Buttons5.ini
 *    MacOS:   /Users/Miep/Library/Preferences/Praat Prefs/Buttons5
 */
static structMelderFile buttonsFile { };

#if defined (UNIX)
	static structMelderFile pidFile { };   // like /home/miep/.praat-dir/pid
	static structMelderFile messageFile { };   // like /home/miep/.praat-dir/message
#elif defined (_WIN32)
	static structMelderFile messageFile { };   // like C:\Users\Miep\Praat\Message.txt
#endif

/*
 * tracingFile: tracing file.
 *    Unix:   /home/miep/.praat-dir/tracing
 *    Windows XP/Vista/7/8/10:   \\myserver\myshare\Miep\Praat\Tracing.txt
 *                         or:   C:\Users\Miep\Praat\Tracing.txt
 *    MacOS:   /Users/Miep/Library/Preferences/Praat Prefs/Tracing.txt
 */
static structMelderFile tracingFile { };

static GuiList praatList_objects;

/***** selection *****/

integer praat_idOfSelected (ClassInfo klas, integer inplace) {
	integer place = inplace, IOBJECT;
	if (place == 0) place = 1;
	if (place > 0) {
		WHERE (SELECTED && (! klas || CLASS == klas)) {
			if (place == 1) return ID;
			place --;
		}
	} else {
		WHERE_DOWN (SELECTED && (! klas || CLASS == klas)) {
			if (place == -1) return ID;
			place ++;
		}
	}
	if (inplace) {
		Melder_throw (U"No ", klas ? klas -> className : U"object", U" #", inplace, U" selected.");
	} else {
		Melder_throw (U"No ", klas ? klas -> className : U"object", U" selected.");
	}
	return 0;
}

autoVEC praat_idsOfAllSelected (ClassInfo klas) {
	autoVEC result (praat_numberOfSelected (klas), kTensorInitializationType::RAW);
	integer selectedObjectNumber = 0, IOBJECT;
	WHERE (SELECTED && (! klas || CLASS == klas)) {
		result.at [++ selectedObjectNumber] = ID;
	}
	return result;
}

char32 * praat_nameOfSelected (ClassInfo klas, integer inplace) {
	integer place = inplace, IOBJECT;
	if (place == 0) place = 1;
	if (place > 0) {
		WHERE (SELECTED && (! klas || CLASS == klas)) {
			if (place == 1) return klas ? NAME : FULL_NAME;
			place --;
		}
	} else {
		WHERE_DOWN (SELECTED && (! klas || CLASS == klas)) {
			if (place == -1) return klas ? NAME : FULL_NAME;
			place ++;
		}
	}
	if (inplace) {
		Melder_throw (U"No ", klas ? klas -> className : U"object", U" #", inplace, U" selected.");
	} else {
		Melder_throw (U"No ", klas ? klas -> className : U"object", U" selected.");
	}
	return 0;   // failure
}

integer praat_numberOfSelected (ClassInfo klas) {
	if (! klas) return theCurrentPraatObjects -> totalSelection;
	integer readableClassId = klas -> sequentialUniqueIdOfReadableClass;
	if (readableClassId == 0) Melder_fatal (U"No sequential unique ID for class ", klas -> className, U".");
	return theCurrentPraatObjects -> numberOfSelected [readableClassId];
}

void praat_deselect (int IOBJECT) {
	if (! SELECTED) return;
	SELECTED = false;
	theCurrentPraatObjects -> totalSelection -= 1;
	integer readableClassId = theCurrentPraatObjects -> list [IOBJECT]. object -> classInfo -> sequentialUniqueIdOfReadableClass;
	Melder_assert (readableClassId != 0);
	theCurrentPraatObjects -> numberOfSelected [readableClassId] -= 1;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding) {
		trace (U"deselecting object ", IOBJECT);
		GuiList_deselectItem (praatList_objects, IOBJECT);
		trace (U"deselected object ", IOBJECT);
	}
}

void praat_deselectAll () { int IOBJECT; WHERE (1) praat_deselect (IOBJECT); }

void praat_select (int IOBJECT) {
	if (SELECTED) return;
	SELECTED = true;
	theCurrentPraatObjects -> totalSelection += 1;
	Thing object = theCurrentPraatObjects -> list [IOBJECT]. object;
	Melder_assert (object);
	integer readableClassId = object -> classInfo -> sequentialUniqueIdOfReadableClass;
	if (readableClassId == 0) Melder_fatal (U"No sequential unique ID for class ", object -> classInfo -> className, U".");
	theCurrentPraatObjects -> numberOfSelected [readableClassId] += 1;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding) {
		GuiList_selectItem (praatList_objects, IOBJECT);
	}
}

void praat_selectAll () { int IOBJECT; WHERE (1) praat_select (IOBJECT); }

void praat_list_background () {
	int IOBJECT;
	WHERE (SELECTED) GuiList_deselectItem (praatList_objects, IOBJECT);
}
void praat_list_foreground () {
	int IOBJECT;
	WHERE (SELECTED) {
		GuiList_selectItem (praatList_objects, IOBJECT);
	}
}

autoCollection praat_getSelectedObjects () {
	autoCollection thee = Collection_create ();
	int IOBJECT;
	LOOP {
		iam_LOOP (Daata);
		thy addItem_ref (me);
	}
	return thee;
}

char32 *praat_name (int IOBJECT) { return str32chr (FULL_NAME, U' ') + 1; }

void praat_write_do (UiForm dia, conststring32 extension) {
	static MelderString defaultFileName { };
	if (extension && str32chr (extension, '.')) {
		/*
			Apparently, the "extension" is a complete file name.
			This should be used as the default file name.
			(This case typically occurs when saving a picture.)
		*/
		MelderString_copy (& defaultFileName, extension);
	} else {
		/*
			Apparently, the "extension" is not a complete file name.
			We are expected to prepend the "extension" with the name of a selected object.
		*/
		int IOBJECT, found = 0;
		Daata data = nullptr;
		WHERE (SELECTED) { if (! data) data = (Daata) OBJECT; found += 1; }
		if (found == 1) {
			MelderString_copy (& defaultFileName, data -> name.get());
			if (defaultFileName.length > 200) { defaultFileName.string [200] = U'\0'; defaultFileName.length = 200; }
			MelderString_append (& defaultFileName, U".", extension ? extension : Thing_className (data));
		} else if (! extension) {
			MelderString_copy (& defaultFileName, U"praat.Collection");
		} else {
			MelderString_copy (& defaultFileName, U"praat.", extension);
		}
	}
	UiOutfile_do (dia, defaultFileName.string);
}

static void removeAllReferencesToMoribundEditor (Editor editor) {
	/*
	 * Remove all references to this editor.
	 * It may be editing multiple objects.
	 */
	for (int iobject = 1; iobject <= theCurrentPraatObjects -> n; iobject ++) {
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
			if (theCurrentPraatObjects -> list [iobject]. editors [ieditor] == editor)
				theCurrentPraatObjects -> list [iobject]. editors [ieditor] = nullptr;
		}
	}
	if (praatP. editor == editor)
		praatP. editor = nullptr;
}

/**
	Remove the "object" from the list,
	killing everything that has to do with the selection.
*/
static void praat_remove (int iobject, bool removeVisibly) {

	Melder_assert (iobject >= 1 && iobject <= theCurrentPraatObjects -> n);
	if (theCurrentPraatObjects -> list [iobject]. isBeingCreated) {
		theCurrentPraatObjects -> list [iobject]. isBeingCreated = false;
		theCurrentPraatObjects -> totalBeingCreated --;
	}
	trace (U"deselect object ", iobject);
	if (removeVisibly)
		praat_deselect (iobject);
	trace (U"deselected object ", iobject);

	/*
	 * To prevent synchronization problems, kill editors before killing the data.
	 */
	for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		Editor editor = theCurrentPraatObjects -> list [iobject]. editors [ieditor];   // save this one reference
		if (editor) {
			trace (U"remove references to editor ", ieditor);
			removeAllReferencesToMoribundEditor (editor);
			trace (U"forget editor ", ieditor);
			if (removeVisibly)
				forget (editor);   // TODO: doesn't this call removeAllReferencesToMoribundEditor() again?
			trace (U"forgeotten editor ", ieditor);
		}
	}
	MelderFile_setToNull (& theCurrentPraatObjects -> list [iobject]. file);
	trace (U"free name");
	theCurrentPraatObjects -> list [iobject]. name. reset();
	trace (U"forget object");
	forget (theCurrentPraatObjects -> list [iobject]. object);   // note: this might save a file-based object to file
	trace (U"forgotten object");
}

void praat_cleanUpName (char32 *name) {
	/*
	 * Replaces spaces and special characters by underscores.
	 */
	for (; *name; name ++) {
		if (str32chr (U" ,.:;\\/()[]{}~`\'<>*&^%#@!?$\"|", *name))
			*name = U'_';
	}
}

/***** objects + commands *****/

static void praat_new_unpackCollection (autoCollection me, const char32* myName) {
	for (integer idata = 1; idata <= my size; idata ++) {
		autoDaata object;
		object. adoptFromAmbiguousOwner ((Daata) my at [idata]);
		my at [idata] = nullptr;   // disown; once the elements are autoThings, the move will handle this
		conststring32 name = object -> name ? object -> name.get() : myName;
		Melder_assert (name);
		praat_new (object.move(), name);   // recurse
	}
}

void praat_newWithFile (autoDaata me, MelderFile file, conststring32 myName) {
	if (! me)
		Melder_throw (U"No object was put into the list.");

	if (my classInfo == classCollection) {
		praat_new_unpackCollection (me.static_cast_move<structCollection>(), myName);
		return;
	}

	autoMelderString name, givenName;
	if (myName && myName [0]) {
		MelderString_copy (& givenName, myName);
		/*
		 * Remove extension.
		 */
		char32 *p = str32rchr (givenName.string, U'.');
		if (p) *p = U'\0';
	} else {
		MelderString_copy (& givenName, my name && my name [0] ? my name.get() : U"untitled");
	}
	praat_cleanUpName (givenName.string);
	MelderString_append (& name, Thing_className (me.get()), U" ", givenName.string);

	if (theCurrentPraatObjects -> n == praat_MAXNUM_OBJECTS) {
		//forget (me);
		Melder_throw (U"The Object Window cannot contain more than ", praat_MAXNUM_OBJECTS, U" objects. You could remove some objects.");
	}
		
	int IOBJECT = ++ theCurrentPraatObjects -> n;
	Melder_assert (FULL_NAME == nullptr);
	theCurrentPraatObjects -> list [IOBJECT]. name = Melder_dup_f (name.string);   // all right to crash if out of memory
	++ theCurrentPraatObjects -> uniqueId;

	if (! theCurrentPraatApplication -> batch) {   // put a new object on the screen, at the bottom of the list
		GuiList_insertItem (praatList_objects,
			Melder_cat (theCurrentPraatObjects -> uniqueId, U". ", name.string),
			theCurrentPraatObjects -> n);
	}
	CLASS = my classInfo;
	OBJECT = me.releaseToAmbiguousOwner();   // FIXME: should be move()
	SELECTED = false;
	for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		EDITOR [ieditor] = nullptr;
	if (file) {
		MelderFile_copy (file, & theCurrentPraatObjects -> list [IOBJECT]. file);
	} else {
		MelderFile_setToNull (& theCurrentPraatObjects -> list [IOBJECT]. file);
	}
	ID = theCurrentPraatObjects -> uniqueId;
	theCurrentPraatObjects -> list [IOBJECT]. isBeingCreated = true;
	Thing_setName (OBJECT, givenName.string);
	theCurrentPraatObjects -> totalBeingCreated ++;
}

static MelderString thePraatNewName { };
void praat_new (autoDaata me) {
	praat_newWithFile (me.move(), nullptr, U"");
}
void praat_new (autoDaata me, const MelderArg& arg) {
	praat_newWithFile (me.move(), nullptr, arg._arg);
}
void praat_new (autoDaata me,
	const MelderArg& arg1, const MelderArg& arg2, const MelderArg& arg3,
	const MelderArg& arg4, const MelderArg& arg5)
{
	MelderString_copy (& thePraatNewName, arg1._arg, arg2._arg, arg3._arg, arg4._arg, arg5._arg);
	praat_new (me.move(), thePraatNewName.string);
}

void praat_updateSelection () {
	if (theCurrentPraatObjects -> totalBeingCreated) {
		int IOBJECT;
		praat_deselectAll ();
		WHERE (theCurrentPraatObjects -> list [IOBJECT]. isBeingCreated) {
			praat_select (IOBJECT);
			theCurrentPraatObjects -> list [IOBJECT]. isBeingCreated = false;
		}
		theCurrentPraatObjects -> totalBeingCreated = 0;
		praat_show ();
	}
}

static void gui_cb_list_selectionChanged (Thing /* boss */, GuiList_SelectionChangedEvent event) {
	Melder_assert (event -> list == praatList_objects);
	int IOBJECT;
	bool first = true;
	WHERE (SELECTED) {
		SELECTED = false;
		integer readableClassId = theCurrentPraatObjects -> list [IOBJECT]. object -> classInfo -> sequentialUniqueIdOfReadableClass;
		theCurrentPraatObjects -> numberOfSelected [readableClassId] --;
		Melder_assert (theCurrentPraatObjects -> numberOfSelected [readableClassId] >= 0);
	}
	theCurrentPraatObjects -> totalSelection = 0;
	integer numberOfSelected;
	integer *selected = GuiList_getSelectedPositions (praatList_objects, & numberOfSelected);
	if (selected) {
		for (integer iselected = 1; iselected <= numberOfSelected; iselected ++) {
			IOBJECT = selected [iselected];
			SELECTED = true;
			integer readableClassId = theCurrentPraatObjects -> list [IOBJECT]. object -> classInfo -> sequentialUniqueIdOfReadableClass;
			theCurrentPraatObjects -> numberOfSelected [readableClassId] ++;
			Melder_assert (theCurrentPraatObjects -> numberOfSelected [readableClassId] > 0);
			UiHistory_write (first ? U"\nselectObject: \"" : U"\nplusObject: \"");
			UiHistory_write_expandQuotes (FULL_NAME);
			UiHistory_write (U"\"");
			first = false;
			theCurrentPraatObjects -> totalSelection += 1;
		}
		NUMvector_free (selected, 1);
	}
	praat_show ();
}

void praat_list_renameAndSelect (int position, conststring32 name) {
	if (! theCurrentPraatApplication -> batch) {
		GuiList_replaceItem (praatList_objects, name, position);   // void if name equal
		if (! Melder_backgrounding)
			GuiList_selectItem (praatList_objects, position);
	}
}

/***** objects *****/

void praat_name2 (char32 *name, ClassInfo klas1, ClassInfo klas2) {
	int i1 = 1;
	while (theCurrentPraatObjects -> list [i1]. isSelected == 0 || theCurrentPraatObjects -> list [i1]. klas != klas1) i1 ++;
	int i2 = 1;
	while (theCurrentPraatObjects -> list [i2]. isSelected == 0 || theCurrentPraatObjects -> list [i2]. klas != klas2) i2 ++;
	char32 *name1 = str32chr (theCurrentPraatObjects -> list [i1]. name.get(), U' ') + 1;
	char32 *name2 = str32chr (theCurrentPraatObjects -> list [i2]. name.get(), U' ') + 1;
	if (str32equ (name1, name2))
		Melder_sprint (name,200, name1);
	else
		Melder_sprint (name,200, name1, U"_", name2);
}

void praat_removeObject (int i) {
	praat_remove (i, true);   // dangle
	for (int j = i; j < theCurrentPraatObjects -> n; j ++)
		theCurrentPraatObjects -> list [j] = std::move (theCurrentPraatObjects -> list [j + 1]);   // undangle but create second references
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. name. reset ();
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. object = nullptr;   // undangle or remove second reference
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. isSelected = 0;
	for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. editors [ieditor] = nullptr;   // undangle or remove second reference
	MelderFile_setToNull (& theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. file);   // undangle or remove second reference
	-- theCurrentPraatObjects -> n;
	if (! theCurrentPraatApplication -> batch) {
		GuiList_deleteItem (praatList_objects, i);
	}
}

static void praat_exit (int exit_code) {
//Melder_setTracing (true);
	int IOBJECT;
	trace (U"destroy the picture window");
	praat_picture_exit ();
	praat_statistics_exit ();   // record total memory use across sessions

	if (! praatP.ignorePreferenceFiles) {
		trace (U"stop receiving messages");
		#if defined (UNIX)
			/*
			 * We are going to delete the process id ("pid") file, if it's ours.
			 */
			if (pidFile. path [0]) {
				try {
					/*
					 * To see whether we own the pid file,
					 * we look into it to see whether its pid equals our pid.
					 * If not, then we are probably living in an old invocation of the program,
					 * and the pid file was written by the latest invocation of the program,
					 * which owns the pid (this means sendpraat can only send to the latest Praat if more than one are open).
					 */
					autofile f = Melder_fopen (& pidFile, "r");
					long_not_integer pid;
					if (fscanf (f, "%ld", & pid) < 1) throw MelderError ();
					f.close (& pidFile);
					if (pid == getpid ()) {   // is the pid in the pid file equal to our pid?
						MelderFile_delete (& pidFile);   // ...then we own the pid file and can delete it
					}
				} catch (MelderError) {
					Melder_clearError ();   // if the pid file is somehow missing or corrupted, we just ignore that
				}
			}
		#endif

		trace (U"save the preferences");
		Melder_assert (str32equ (Melder_double (1.5), U"1.5"));   // refuse to write the preferences if the locale is wrong (even if tracing is on)
		Preferences_write (& prefsFile);

		trace (U"save the script buttons");
		if (! theCurrentPraatApplication -> batch) {
			try {
				autoMelderString buffer;
				MelderString_append (& buffer, U"# Buttons (1).\n");
				MelderString_append (& buffer, U"# This file is generated automatically when you quit the ", praatP.title.get(), U" program.\n");
				MelderString_append (& buffer, U"# It contains the buttons that you added interactively to the fixed or dynamic menus,\n");
				MelderString_append (& buffer, U"# and the buttons that you hid or showed.\n\n");
				praat_saveAddedMenuCommands (& buffer);
				praat_saveToggledMenuCommands (& buffer);
				praat_saveAddedActions (& buffer);
				praat_saveToggledActions (& buffer);
				MelderFile_writeText (& buttonsFile, buffer.string, kMelder_textOutputEncoding::ASCII_THEN_UTF16);
			} catch (MelderError) {
				Melder_clearError ();
			}
		}
	}

	trace (U"flush the file-based objects");
	WHERE_DOWN (! MelderFile_isNull (& theCurrentPraatObjects -> list [IOBJECT]. file)) {
		trace (U"removing object based on file ", & theCurrentPraatObjects -> list [IOBJECT]. file);
		praat_remove (IOBJECT, false);
	}
	Melder_files_cleanUp ();   // in case a URL is open

	trace (U"leave the program");
	praat_menuCommands_exit_optimizeByLeaking ();   // these calls are superflous if subsequently _Exit() is called instead of exit()
	praat_actions_exit_optimizeByLeaking ();
	Preferences_exit_optimizeByLeaking ();
	/*
		OPTIMIZE with an exercise in self-documenting code
	*/
	constexpr bool weWouldLikeToOptimizeExitingSpeed = ((true));
	constexpr bool callingExitTimeDestructorsIsSlow = (true);
	constexpr bool notCallingExitTimeDestructorsCausesCorrectBehaviour = (true);
	constexpr bool weAreReallySureAboutThat = (true);
	constexpr bool weWillUseUnderscoreExitInsteadOfExit =
			weWouldLikeToOptimizeExitingSpeed &&
			callingExitTimeDestructorsIsSlow &&
			notCallingExitTimeDestructorsCausesCorrectBehaviour &&
			weAreReallySureAboutThat;
	if ((weWillUseUnderscoreExitInsteadOfExit)) {
		constexpr bool underscoreExitHasMoreSideEffectsThanJustNotCallingExitTimeDestructors = (true);
		constexpr bool avoidOtherSideEffectsOfUnderscoreExit =
				underscoreExitHasMoreSideEffectsThanJustNotCallingExitTimeDestructors;
		if ((avoidOtherSideEffectsOfUnderscoreExit)) {
			constexpr bool oneSideEffectIsThatOpenOutputFilesAreNotFlushed = true;
			constexpr bool weShouldFlushAllOpenOutputFilesWhoseNonflushingWouldCauseIncorrectBehaviour =
					oneSideEffectIsThatOpenOutputFilesAreNotFlushed;
			if ((weShouldFlushAllOpenOutputFilesWhoseNonflushingWouldCauseIncorrectBehaviour)) {
				constexpr bool stdoutIsOpen = (true);
				constexpr bool stderrIsOpen = (true);
				constexpr bool stdoutIsBufferedByDefault = true;
				constexpr bool stderrIsBufferedByDefault = false;
				constexpr bool weKnowThatSetbufHasNotBeenCalledOnStdout = (false);
				constexpr bool weKnowThatSetbufHasNotBeenCalledOnStderr = (false);
				constexpr bool stdoutHasCertainlyBeenFlushed =
						! stdoutIsBufferedByDefault && weKnowThatSetbufHasNotBeenCalledOnStdout;
				constexpr bool stderrHasCertainlyBeenFlushed =
						! stderrIsBufferedByDefault && weKnowThatSetbufHasNotBeenCalledOnStderr;
				constexpr bool notFlushingStdoutCouldCauseIncorrectBehaviour =
						stdoutIsOpen && ! stdoutHasCertainlyBeenFlushed;
				constexpr bool notFlushingStderrCouldCauseIncorrectBehaviour =
						stderrIsOpen && ! stderrHasCertainlyBeenFlushed;
				constexpr bool shouldFlushStdout = notFlushingStdoutCouldCauseIncorrectBehaviour;
				constexpr bool shouldFlushStderr = notFlushingStderrCouldCauseIncorrectBehaviour;
				if ((shouldFlushStdout))
					fflush (stdout);
				if ((shouldFlushStderr))
					fflush (stderr);
				constexpr bool thereAreOtherOpenFiles = (false);
				constexpr bool thereAreOtherOpenFilesWhoseNonflushingCouldCauseIncorrectBehaviour =
						thereAreOtherOpenFiles;
				if ((! thereAreOtherOpenFilesWhoseNonflushingCouldCauseIncorrectBehaviour)) {}
			}
			constexpr bool thereAreNoOtherSideEffectsBesideNotCallingExitDestructorsAndNotFlushingOpenFiles = (true);
			if ((thereAreNoOtherSideEffectsBesideNotCallingExitDestructorsAndNotFlushingOpenFiles)) {}
		}
		_Exit (exit_code);
	} else {
		exit (exit_code);
	}
}

static void cb_Editor_destruction (Editor me) {
	removeAllReferencesToMoribundEditor (me);
}

static void cb_Editor_dataChanged (Editor me) {
	for (int iobject = 1; iobject <= theCurrentPraatObjects -> n; iobject ++) {
		bool editingThisObject = false;
		/*
		 * Am I editing this object?
		 */
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
			if (theCurrentPraatObjects -> list [iobject]. editors [ieditor] == me) {
				editingThisObject = true;
			}
		}
		if (editingThisObject) {
			/*
			 * Notify all other editors associated with this object.
			 */
			for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor otherEditor = theCurrentPraatObjects -> list [iobject]. editors [ieditor];
				if (otherEditor && otherEditor != me) {
					Editor_dataChanged (otherEditor);
				}
			}
		}
	}
}

static void cb_Editor_publication (Editor /* me */, autoDaata publication) {
/*
   The default publish callback.
   Works nicely if the publisher invents a name.
*/
	try {
		praat_new (publication.move(), U"");
	} catch (MelderError) {
		Melder_flushError ();
	}
	praat_updateSelection ();
}

int praat_installEditor (Editor editor, int IOBJECT) {
	if (! editor) return 0;
	for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		if (! EDITOR [ieditor]) {
			EDITOR [ieditor] = editor;
			Editor_setDestructionCallback (editor, cb_Editor_destruction);
			Editor_setDataChangedCallback (editor, cb_Editor_dataChanged);
			if (! editor -> d_publicationCallback)
				Editor_setPublicationCallback (editor, cb_Editor_publication);
			return 1;
		}
	}
	//forget (editor);
	Melder_throw (U"(praat_installEditor:) Cannot have more than ", praat_MAXNUM_EDITORS, U" editors with one object.");
}

int praat_installEditor2 (Editor editor, int i1, int i2) {
	if (! editor) return 0;
	int ieditor1 = 0;
	for (; ieditor1 < praat_MAXNUM_EDITORS; ieditor1 ++)
		if (! theCurrentPraatObjects -> list [i1]. editors [ieditor1])
			break;
	int ieditor2 = 0;
	for (; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (! theCurrentPraatObjects -> list [i2]. editors [ieditor2])
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS) {
		theCurrentPraatObjects -> list [i1]. editors [ieditor1] = theCurrentPraatObjects -> list [i2]. editors [ieditor2] = editor;
		Editor_setDestructionCallback (editor, cb_Editor_destruction);
		Editor_setDataChangedCallback (editor, cb_Editor_dataChanged);
		if (! editor -> d_publicationCallback)
			Editor_setPublicationCallback (editor, cb_Editor_publication);
	} else {
		//forget (editor);
		Melder_throw (U"(praat_installEditor2:) Cannot have more than ", praat_MAXNUM_EDITORS, U" editors with one object.");
	}
	return 1;
}

int praat_installEditor3 (Editor editor, int i1, int i2, int i3) {
	if (! editor) return 0;
	int ieditor1 = 0;
	for (; ieditor1 < praat_MAXNUM_EDITORS; ieditor1 ++)
		if (! theCurrentPraatObjects -> list [i1]. editors [ieditor1])
			break;
	int ieditor2 = 0;
	for (; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (! theCurrentPraatObjects -> list [i2]. editors [ieditor2])
			break;
	int ieditor3 = 0;
	for (; ieditor3 < praat_MAXNUM_EDITORS; ieditor3 ++)
		if (! theCurrentPraatObjects -> list [i3]. editors [ieditor3])
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS && ieditor3 < praat_MAXNUM_EDITORS) {
		theCurrentPraatObjects -> list [i1]. editors [ieditor1] = theCurrentPraatObjects -> list [i2]. editors [ieditor2] = theCurrentPraatObjects -> list [i3]. editors [ieditor3] = editor;
		Editor_setDestructionCallback (editor, cb_Editor_destruction);
		Editor_setDataChangedCallback (editor, cb_Editor_dataChanged);
		if (! editor -> d_publicationCallback)
			Editor_setPublicationCallback (editor, cb_Editor_publication);
	} else {
		//forget (editor);
		Melder_throw (U"(praat_installEditor3:) Cannot have more than ", praat_MAXNUM_EDITORS, U" editors with one object.");
	}
	return 1;
}

int praat_installEditorN (Editor editor, DaataList objects) {
	if (! editor) return 0;
	/*
	 * First check whether all objects in the Ordered are also in the List of Objects (Praat crashes if not),
	 * and check whether there is room to add an editor for each.
	 */
	for (integer iOrderedObject = 1; iOrderedObject <= objects->size; iOrderedObject ++) {
		Daata object = objects->at [iOrderedObject];
		integer iPraatObject = 1;
		for (; iPraatObject <= theCurrentPraatObjects -> n; iPraatObject ++) {
			if (object == theCurrentPraatObjects -> list [iPraatObject]. object) {
				int ieditor = 0;
				for (; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (! theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor]) {
						break;
					}
				}
				if (ieditor >= praat_MAXNUM_EDITORS) {
					//forget (editor);
					Melder_throw (U"Cannot view the same object in more than ", praat_MAXNUM_EDITORS, U" windows.");
				}
				break;
			}
		}
		Melder_assert (iPraatObject <= theCurrentPraatObjects -> n);   // an element of the Ordered does not occur in the List of Objects
	}
	/*
	 * There appears to be room for all elements of the Ordered. The editor window can appear. Install the editor in all objects.
	 */
	for (integer iOrderedObject = 1; iOrderedObject <= objects->size; iOrderedObject ++) {
		Daata object = objects->at [iOrderedObject];
		integer iPraatObject = 1;
		for (; iPraatObject <= theCurrentPraatObjects -> n; iPraatObject ++) {
			if (object == theCurrentPraatObjects -> list [iPraatObject]. object) {
				int ieditor = 0;
				for (; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (! theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor]) {
						theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor] = editor;
						Editor_setDestructionCallback (editor, cb_Editor_destruction);
						Editor_setDataChangedCallback (editor, cb_Editor_dataChanged);
						if (! editor -> d_publicationCallback)
							Editor_setPublicationCallback (editor, cb_Editor_publication);
						break;
					}
				}
				Melder_assert (ieditor < praat_MAXNUM_EDITORS);   // we just checked, but nevertheless
				break;
			}
		}
		Melder_assert (iPraatObject <= theCurrentPraatObjects -> n);   // we already checked, but still
	}
	return 1;
}

void praat_dataChanged (Daata object) {
	/*
	 * This function can be called at error time, which is weird.
	 */
	autostring32 saveError;
	bool duringError = Melder_hasError ();
	if (duringError) {
		saveError = Melder_dup_f (Melder_getError ());
		Melder_clearError ();
	}
	int IOBJECT;
	WHERE (OBJECT == object) {
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
			Editor editor = EDITOR [ieditor];
			if (editor) {
				Editor_dataChanged (editor);
			}
		}
	}
	if (duringError) {
		Melder_appendError (saveError.get());   // BUG: this appends an empty newline to the original error message
		// BUG: who will catch the error?
	}
}

static void helpProc (conststring32 query) {
	if (theCurrentPraatApplication -> batch) {
		Melder_flushError (U"Cannot view manual from batch.");
		return;
	}
	try {
		autoManual manual = Manual_create (query, theCurrentPraatApplication -> manPages, false);
		manual.releaseToUser();
	} catch (MelderError) {
		Melder_flushError (U"help: no help on \"", query, U"\".");
	}
}

static int publishProc (autoDaata me) {
	try {
		praat_new (me.move(), U"");
		praat_updateSelection ();
		return 1;
	} catch (MelderError) {
		Melder_throw (U"Not published.");
	}
}

/***** QUIT *****/

FORM (DO_Quit, U"Confirm Quit", U"Quit") {
	MUTABLE_LABEL (label, U"You have objects in your list!")
	OK
{
	char32 prompt [300];
	if (ScriptEditors_dirty ()) {
		if (theCurrentPraatObjects -> n)
			Melder_sprint (prompt,300, U"You have objects and unsaved scripts! Do you still want to quit ", praatP.title.get(), U"?");
		else
			Melder_sprint (prompt,300, U"You have unsaved scripts! Do you still want to quit ", praatP.title.get(), U"?");
		SET_STRING (label, prompt)
	} else if (theCurrentPraatObjects -> n) {
		Melder_sprint (prompt,300, U"You have objects in your list! Do you still want to quit ", praatP.title.get(), U"?");
		SET_STRING (label, prompt)
	} else {
		praat_exit (0);
	}
}
DO
	praat_exit (0);
END }

static void gui_cb_quit (Thing /* me */) {
	DO_Quit (nullptr, 0, nullptr, nullptr, nullptr, nullptr, false, nullptr);
}

void praat_dontUsePictureWindow () { praatP.dontUsePictureWindow = true; }

/********** INITIALIZATION OF THE PRAAT SHELL **********/

#if defined (UNIX)
	static void cb_sigusr1 (int signum) {
		Melder_assert (signum == SIGUSR1);
		#if 0
			gboolean retval;
			g_signal_emit_by_name (GTK_OBJECT (theCurrentPraatApplication -> topShell -> d_gtkWindow), "client-event", nullptr, & retval);
		#else
			#if ALLOW_GDK_DRAWING && ! defined (NO_GRAPHICS)
				GdkEventClient gevent;
				gevent. type = GDK_CLIENT_EVENT;
				gevent. window = GTK_WIDGET (theCurrentPraatApplication -> topShell -> d_gtkWindow) -> window;
				gevent. send_event = 1;
				gevent. message_type = gdk_atom_intern_static_string ("SENDPRAAT");
				gevent. data_format = 8;
				// Melder_casual (U"event put");
				gdk_event_put ((GdkEvent *) & gevent);
			#endif
		#endif
	}
#endif

#if defined (UNIX)
	#if ALLOW_GDK_DRAWING && ! defined (NO_GRAPHICS)
		static gboolean cb_userMessage (GtkWidget /* widget */, GdkEventClient * /* event */, gpointer /* userData */) {
			//Melder_casual (U"client event called");
			autofile f;
			try {
				f.reset (Melder_fopen (& messageFile, "r"));
			} catch (MelderError) {
				Melder_clearError ();
				return true;   // OK
			}
			long_not_integer pid = 0;
			int narg = fscanf (f, "#%ld", & pid);
			f.close (& messageFile);
			{// scope
				autoPraatBackground background;
				try {
					praat_executeScriptFromFile (& messageFile, nullptr);
				} catch (MelderError) {
					Melder_flushError (praatP.title.get(), U": message not completely handled.");
				}
			}
			if (narg && pid) kill (pid, SIGUSR2);
			return true;
		}
	#endif
#elif defined (_WIN32)
	static int cb_userMessage () {
		autoPraatBackground background;
		try {
			praat_executeScriptFromFile (& messageFile, nullptr);
		} catch (MelderError) {
			Melder_flushError (praatP.title.get(), U": message not completely handled.");
		}
		return 0;
	}
	extern "C" char *sendpraat (void *display, const char *programName, long timeOut, const char *text);
	static void cb_openDocument (MelderFile file) {
		char32 text [kMelder_MAXPATH+25];
		/*
			The user dropped a file on the Praat icon,
			or double-clicked a Praat file,
			while Praat is already running.
		*/
		Melder_sprint (text,500, U"Read from file... ", file -> path);
		sendpraat (nullptr, Melder_peek32to8 (praatP.title.get()), 0, Melder_peek32to8 (text));
	}
#elif macintosh
	static int (*theUserMessageCallback) (char32 *message);
	static void mac_setUserMessageCallback (int (*userMessageCallback) (char32 *message)) {
		theUserMessageCallback = userMessageCallback;
	}
	static pascal OSErr mac_processSignal8 (const AppleEvent *theAppleEvent, AppleEvent * /* reply */, long /* handlerRefCon */) {
		static bool duringAppleEvent = false;   // FIXME: may have to be atomic?
		if (! duringAppleEvent) {
			char *buffer;
			Size actualSize;
			duringAppleEvent = true;
			//AEInteractWithUser (kNoTimeOut, nullptr, nullptr);   // use time out of 0 to execute immediately (without bringing to foreground)
			ProcessSerialNumber psn;
			GetCurrentProcess (& psn);
			SetFrontProcess (& psn);
			AEGetParamPtr (theAppleEvent, 1, typeUTF8Text, nullptr, nullptr, 0, & actualSize);
			buffer = (char *) malloc ((size_t) actualSize);
			AEGetParamPtr (theAppleEvent, 1, typeUTF8Text, nullptr, & buffer [0], actualSize, nullptr);
			if (theUserMessageCallback) {
				autostring32 buffer32 = Melder_8to32 (buffer);
				theUserMessageCallback (buffer32.get());
			}
			free (buffer);
			duringAppleEvent = false;
		}
		return noErr;
	}
	static pascal OSErr mac_processSignal16 (const AppleEvent *theAppleEvent, AppleEvent * /* reply */, long /* handlerRefCon */) {
		static bool duringAppleEvent = false;   // FIXME: may have to be atomic?
		if (! duringAppleEvent) {
			char16 *buffer;
			Size actualSize;
			duringAppleEvent = true;
			//AEInteractWithUser (kNoTimeOut, nullptr, nullptr);   // use time out of 0 to execute immediately (without bringing to foreground)
			ProcessSerialNumber psn;
			GetCurrentProcess (& psn);
			SetFrontProcess (& psn);
			AEGetParamPtr (theAppleEvent, 1, typeUTF16ExternalRepresentation, nullptr, nullptr, 0, & actualSize);
			buffer = (char16 *) malloc ((size_t) actualSize);
			AEGetParamPtr (theAppleEvent, 1, typeUTF16ExternalRepresentation, nullptr, & buffer [0], actualSize, nullptr);
			if (theUserMessageCallback) {
				autostring32 buffer32 = Melder_16to32 (buffer);
				theUserMessageCallback (buffer32.get());
			}
			free (buffer);
			duringAppleEvent = false;
		}
		return noErr;
	}
	static int cb_userMessage (char32 *message) {
		autoPraatBackground background;
		try {
			praat_executeScriptFromText (message);
		} catch (MelderError) {
			Melder_flushError (praatP.title.get(), U": message not completely handled.");
		}
		return 0;
	}
	static int cb_quitApplication () {
		DO_Quit (nullptr, 0, nullptr, nullptr, nullptr, nullptr, false, nullptr);
		return 0;
	}
#endif

static conststring32 thePraatStandAloneScriptText = nullptr;

void praat_setStandAloneScriptText (conststring32 text) {
	thePraatStandAloneScriptText = text;
}

static bool tryToAttachToTheCommandLine ()
{
	bool weHaveSucceeded = false;
	#if defined (_WIN32)
		/*
		 * On Windows, console applications are automatically attached to the command line,
		 * but Praat is always a Windows application instead, so command line attachment
		 * has to be handled explicitly, as here.
		 */
		if (AttachConsole (ATTACH_PARENT_PROCESS)) {   // was Praat called from either a console window or a "system" command?
			weHaveSucceeded = true;
		}
	#else
		weHaveSucceeded = isatty (fileno (stdin)) || isatty (fileno (stdout)) || isatty (fileno (stderr));
		/*
			The result is `true` if Praat was called from a terminal window or some system() commands or Xcode,
			and `false` if Praat was called from the Finder by double-clicking or dropping a file.
			
			FIXME:
			The result is incorrectly `false` if the output is redirected to a file or pipe.
			A proposed improvement is therefore:
				isatty (fileno (stdin)) || isatty (fileno (stdout)) || isatty (fileno (stderr))
			This might be incorrectly false only if all three streams are redirected, but this hasn't been tested yet.
		*/
	#endif
	return weHaveSucceeded;
}

static void setThePraatLocale () {
	#if defined (UNIX)
		setlocale (LC_ALL, "C");
		//setenv ("PULSE_LATENCY_MSEC", "1", 0);   // Rafael Laboissiere, August 2014
	#elif defined (_WIN32)
		setlocale (LC_ALL, "C");   // said to be superfluous
	#elif defined (macintosh)
		setlocale (LC_ALL, "en_US");   // required to make swprintf work correctly; the default "C" locale does not do that!
	#endif
}

static void installPraatShellPreferences () {
	praat_statistics_prefs ();   // number of sessions, memory used...
	praat_picture_prefs ();   // font...
	Graphics_prefs ();
	structEditor     :: f_preferences ();   // erase picture first...
	structHyperPage  :: f_preferences ();   // font...
	Site_prefs ();   // print command...
	Melder_audio_prefs ();   // asynchronicity, silence after...
	Melder_textEncoding_prefs ();
	Printer_prefs ();   // paper size, printer command...
	structTextEditor :: f_preferences ();   // font size...
}

extern "C" void praatlib_init () {
	setThePraatLocale ();   // FIXME: don't use the global locale
	Melder_init ();
	Melder_rememberShellDirectory ();
	installPraatShellPreferences ();   // needed in the library, because this sets the defaults
	praatP.argc = 0;
	praatP.argv = nullptr;
	praatP.argumentNumber = 1;
	Melder_batch = true;
	praatP.userWantsToOpen = false;
	praatP.title = Melder_dup (U"Praatlib");
	theCurrentPraatApplication -> batch = true;
	Melder_getHomeDir (& homeDir);
	praat_actions_init ();
	praat_menuCommands_init ();
	Thing_recognizeClassesByName (classCollection, classStrings, classManPages, classStringSet, nullptr);
	Thing_recognizeClassByOtherName (classStringSet, U"SortedSetOfString");
	Melder_backgrounding = true;
	praat_addMenus (nullptr);
	praat_addFixedButtons (nullptr);
	praat_addMenus2 ();
}

static void injectMessageAndInformationProcs (GuiWindow parent) {
	Gui_injectMessageProcs (parent);
	InfoEditor_injectInformationProc ();
}

void praat_init (conststring32 title, int argc, char **argv)
{
	bool weWereStartedFromTheCommandLine = tryToAttachToTheCommandLine ();

	for (int iarg = 0; iarg < argc; iarg ++) {
		//Melder_casual (U"arg ", iarg, U": <<", Melder_peek8to32 (argv [iarg]), U">>");
	}
	setThePraatLocale ();
	Melder_init ();

	/*
		Remember the current directory. Useful only for scripts run from batch.
	*/
	Melder_rememberShellDirectory ();

	installPraatShellPreferences ();

	praatP.argc = argc;
	praatP.argv = argv;
	praatP.argumentNumber = 1;
	autostring32 unknownCommandLineOption;

	/*
	 * Running Praat from the command line.
	 */
	bool foundTheOpenOption = false, foundTheRunOption = false;
	while (praatP.argumentNumber < argc && argv [praatP.argumentNumber] [0] == '-') {
		if (strequ (argv [praatP.argumentNumber], "-")) {
			praatP.hasCommandLineInput = true;
		} else if (strequ (argv [praatP.argumentNumber], "--open")) {
			foundTheOpenOption = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--run")) {
			foundTheRunOption = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--no-pref-files")) {
			praatP.ignorePreferenceFiles = true;
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--no-plugins")) {
			praatP.ignorePlugins = true;
			praatP.argumentNumber += 1;
		} else if (strnequ (argv [praatP.argumentNumber], "--pref-dir=", 11)) {
			Melder_pathToDir (Melder_peek8to32 (argv [praatP.argumentNumber] + 11), & praatDir);
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "--version")) {
			#define xstr(s) str(s)
			#define str(s) #s
			Melder_information (title, U" " xstr (PRAAT_VERSION_STR) " (" xstr (PRAAT_MONTH) " ", PRAAT_DAY, U" ", PRAAT_YEAR, U")");
			exit (0);
		} else if (strequ (argv [praatP.argumentNumber], "--help")) {
			MelderInfo_open ();
			MelderInfo_writeLine (U"Usage: praat [options] script-file-name [script-arguments]");
			MelderInfo_writeLine (U"Options:");
			MelderInfo_writeLine (U"  --open           regard the command line as files to be opened in the GUI");
			MelderInfo_writeLine (U"  --run            regard the command line as a script to run, with its arguments");
			MelderInfo_writeLine (U"                   (--run is superfluous when you use a Console or Terminal)");
			MelderInfo_writeLine (U"  --no-pref-files  don't read or write the preferences file and the buttons file");
			MelderInfo_writeLine (U"  --no-plugins     don't activate the plugins");
			MelderInfo_writeLine (U"  --pref-dir=DIR   set the preferences directory to DIR");
			MelderInfo_writeLine (U"  --version        print the Praat version");
			MelderInfo_writeLine (U"  --help           print this list of command line options");
			MelderInfo_writeLine (U"  -u, --utf16      use UTF-16LE output encoding, no BOM (the default on Windows)");
			MelderInfo_writeLine (U"  -8, --utf8       use UTF-8 output encoding (the default on MacOS and Linux)");
			MelderInfo_writeLine (U"  -a, --ansi       use ISO Latin-1 output encoding (lossy, hence not recommended)");
			MelderInfo_writeLine (U"                   (on Windows, use -0 or -a when you redirect to a pipe or file)");
			MelderInfo_close ();
			exit (0);
		} else if (strequ (argv [praatP.argumentNumber], "-8") || strequ (argv [praatP.argumentNumber], "--utf8")) {
			MelderConsole::setEncoding (MelderConsole::Encoding::UTF8);
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "-u") || strequ (argv [praatP.argumentNumber], "--utf16")) {
			MelderConsole::setEncoding (MelderConsole::Encoding::UTF16);
			praatP.argumentNumber += 1;
		} else if (strequ (argv [praatP.argumentNumber], "-a") || strequ (argv [praatP.argumentNumber], "--ansi")) {
			MelderConsole::setEncoding (MelderConsole::Encoding::ANSI);
			praatP.argumentNumber += 1;
		#if defined (macintosh)
		} else if (strequ (argv [praatP.argumentNumber], "-NSDocumentRevisionsDebugMode")) {
			(void) 0;   // ignore this option, which was added by Xcode
			praatP.argumentNumber += 2;   // jump over the argument, which is usually "YES" (this jump works correctly even if this argument is missing)
		} else if (strnequ (argv [praatP.argumentNumber], "-psn_", 5)) {
			(void) 0;   // ignore this option, which was added by the Finder, perhaps when dragging a file on Praat (Process Serial Number)
			praatP.argumentNumber += 1;
		#endif
		} else {
			unknownCommandLineOption = Melder_8to32 (argv [praatP.argumentNumber]);
			praatP.argumentNumber = INT32_MAX;   // ignore all other command line options
			break;
		}
	}
	weWereStartedFromTheCommandLine |= foundTheRunOption;   // some external system()-like commands don't make isatty return true, so we have to help

	const bool thereIsAFileNameInTheArgumentList = ( praatP.argumentNumber < argc );
	Melder_batch = weWereStartedFromTheCommandLine && thereIsAFileNameInTheArgumentList && ! foundTheOpenOption;
	const bool fileNamesCameInByDropping = ( thereIsAFileNameInTheArgumentList && ! weWereStartedFromTheCommandLine );   // doesn't happen on the Mac
	praatP.userWantsToOpen = foundTheOpenOption || fileNamesCameInByDropping;

	if (Melder_batch) {
		Melder_assert (praatP.argumentNumber < argc);
		/*
		 * We now get the script file name. It is next on the command line
		 * (not necessarily *last* on the line, because there may be script arguments after it).
		 */
		MelderString_copy (& theCurrentPraatApplication -> batchName, Melder_peek8to32 (argv [praatP.argumentNumber ++]));
		if (praatP.hasCommandLineInput)
			Melder_throw (U"Cannot have both command line input and a script file.");
	} else {
		MelderString_copy (& theCurrentPraatApplication -> batchName, U"");
	}
	//Melder_casual (U"Script file name <<", theCurrentPraatApplication -> batchName.string, U">>");

	Melder_batch |= !! thePraatStandAloneScriptText;

	/*
	 * Running the Praat shell from the command line:
	 *    praat -
	 */
	Melder_batch |= praatP.hasCommandLineInput;

	praatP.title = Melder_dup (title && title [0] != U'\0' ? title : U"Praat");

	theCurrentPraatApplication -> batch = Melder_batch;

	/*
	 * Construct a program name like "Praat" for file and directory names.
	 */
	str32cpy (programName, praatP.title.get());

	/*
	 * Construct a main-window title like "Praat 6.1".
	 */
	programName [0] = Melder_toLowerCase (programName [0]);

	/*
	 * Get home directory, e.g. "/home/miep/", or "/Users/miep/", or just "/".
	 */
	Melder_getHomeDir (& homeDir);

	/*
	 * Get the program's private directory (if not yet set by the --prefdir option):
	 *    "/home/miep/.praat-dir" (Unix)
	 *    "/Users/miep/Library/Preferences/Praat Prefs" (MacOS)
	 *    "C:\Users\Miep\Praat" (Windows)
	 * and construct a preferences-file name and a script-buttons-file name like
	 *    /home/miep/.praat-dir/prefs5
	 *    /home/miep/.praat-dir/buttons5
	 * or
	 *    /Users/miep/Library/Preferences/Praat Prefs/Prefs5
	 *    /Users/miep/Library/Preferences/Praat Prefs/Buttons5
	 * or
	 *    C:\Users\Miep\Praat\Preferences5.ini
	 *    C:\Users\Miep\Praat\Buttons5.ini
	 * Also create names for message and tracing files.
	 */
	if (MelderDir_isNull (& praatDir)) {   // not yet set by the --prefdir option?
		structMelderDir prefParentDir { };   // directory under which to store our preferences directory
		Melder_getPrefDir (& prefParentDir);

		/*
		 * Make sure that the program's private directory exists.
		 */
		char32 name [256];
		#if defined (UNIX)
			Melder_sprint (name,256, U".", programName, U"-dir");   // for example .praat-dir
		#elif defined (macintosh)
			Melder_sprint (name,256, praatP.title.get(), U" Prefs");   // for example Praat Prefs
		#elif defined (_WIN32)
			Melder_sprint (name,256, praatP.title.get());   // for example Praat
		#endif
		try {
			#if defined (UNIX) || defined (macintosh)
				Melder_createDirectory (& prefParentDir, name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			#else
				Melder_createDirectory (& prefParentDir, name, 0);
			#endif
			MelderDir_getSubdir (& prefParentDir, name, & praatDir);
		} catch (MelderError) {
			/*
			 * If we arrive here, the directory could not be created,
			 * and all the files are null. Praat should nevertheless start up.
			 */
			Melder_clearError ();
		}
	}
	if (! MelderDir_isNull (& praatDir)) {
		#if defined (UNIX)
			MelderDir_getFile (& praatDir, U"prefs5", & prefsFile);
			MelderDir_getFile (& praatDir, U"buttons5", & buttonsFile);
			MelderDir_getFile (& praatDir, U"pid", & pidFile);
			MelderDir_getFile (& praatDir, U"message", & messageFile);
			MelderDir_getFile (& praatDir, U"tracing", & tracingFile);
		#elif defined (_WIN32)
			MelderDir_getFile (& praatDir, U"Preferences5.ini", & prefsFile);
			MelderDir_getFile (& praatDir, U"Buttons5.ini", & buttonsFile);
			MelderDir_getFile (& praatDir, U"Message.txt", & messageFile);
			MelderDir_getFile (& praatDir, U"Tracing.txt", & tracingFile);
		#elif defined (macintosh)
			MelderDir_getFile (& praatDir, U"Prefs5", & prefsFile);
			MelderDir_getFile (& praatDir, U"Buttons5", & buttonsFile);
			MelderDir_getFile (& praatDir, U"Tracing.txt", & tracingFile);
		#endif
		Melder_tracingToFile (& tracingFile);
	}

	#if defined (NO_GUI)
		if (! Melder_batch) {
			fprintf (stderr, "A no-GUI edition of Praat cannot be used interactively. "
				"Supply \"--run\" and a script file name on the command line.\n");
			exit (1);
		}
	#endif
	#if defined (UNIX)
		if (! Melder_batch) {
			/*
			 * Make sure that the directory /home/miep/.praat-dir exists,
			 * and write our process id into the pid file.
			 * Messages from "sendpraat" are caught very early this way,
			 * though they will be responded to much later.
			 */
			try {
				autofile f = Melder_fopen (& pidFile, "w");
				fprintf (f, "%s", Melder8_integer (getpid ()));
				f.close (& pidFile);
			} catch (MelderError) {
				Melder_clearError ();
			}
		}
	#elif defined (_WIN32)
		if (! Melder_batch)
			motif_win_setUserMessageCallback (cb_userMessage);
	#elif defined (macintosh)
		if (! Melder_batch) {
			mac_setUserMessageCallback (cb_userMessage);
			Gui_setQuitApplicationCallback (cb_quitApplication);
		}
	#endif

	/*
	 * Make room for commands.
	 */
	trace (U"initing actions");
	praat_actions_init ();
	trace (U"initing menu commands");
	praat_menuCommands_init ();

	GuiWindow raam = nullptr;
	if (Melder_batch) {
		MelderString_empty (& theCurrentPraatApplication -> batchName);
		for (int i = praatP.argumentNumber - 1; i < argc; i ++) {
			if (i >= praatP.argumentNumber) MelderString_append (& theCurrentPraatApplication -> batchName, U" ");
			bool needsQuoting = !! strchr (argv [i], ' ') && (i == praatP.argumentNumber - 1 || i < argc - 1);
			if (needsQuoting) MelderString_append (& theCurrentPraatApplication -> batchName, U"\"");
			MelderString_append (& theCurrentPraatApplication -> batchName, Melder_peek8to32 (argv [i]));
			if (needsQuoting) MelderString_append (& theCurrentPraatApplication -> batchName, U"\"");
		}
	} else {
		trace (U"starting the application");
		Machine_initLookAndFeel (argc, argv);
		/*
		 * Start the application.
		 */
		#if gtk
			trace (U"locale ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
			g_set_application_name (Melder_peek32to8 (title));
			trace (U"locale ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		#elif motif
			argv [0] = Melder_32to8 (praatP. title.get()).transfer();   // argc == 4
			Gui_setOpenDocumentCallback (cb_openDocument);
			GuiAppInitialize ("Praatwulg", argc, argv);
		#elif cocoa
			//[NSApplication sharedApplication];
			[GuiCocoaApplication sharedApplication];
		#endif

		trace (U"creating and installing the Objects window");
		char32 objectWindowTitle [100];
		Melder_sprint (objectWindowTitle,100, praatP.title.get(), U" Objects");
		double x, y;
		trace (U"locale ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		Gui_getWindowPositioningBounds (& x, & y, nullptr, nullptr);
		trace (U"locale ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		theCurrentPraatApplication -> topShell = raam =
			GuiWindow_create (x + 10, y, WINDOW_WIDTH, WINDOW_HEIGHT, 450, 250,
				objectWindowTitle, gui_cb_quit, nullptr, 0);
		trace (U"locale ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		#if motif
			GuiApp_setApplicationShell (theCurrentPraatApplication -> topShell -> d_xmShell);
		#endif
		trace (U"before objects window shows locale ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		GuiThing_show (raam);
		trace (U"after objects window shows locale ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
	}
	Thing_recognizeClassesByName (classCollection, classStrings, classManPages, classStringSet, nullptr);
	Thing_recognizeClassByOtherName (classStringSet, U"SortedSetOfString");
	if (Melder_batch) {
		Melder_backgrounding = true;
		trace (U"adding menus without GUI");
		praat_addMenus (nullptr);
		trace (U"adding fixed buttons without GUI");
		praat_addFixedButtons (nullptr);
	} else {

		#ifdef macintosh
			AEInstallEventHandler (758934755, 0, (AEEventHandlerProcPtr) (mac_processSignal8), 0, false);   // for receiving sendpraat
			injectMessageAndInformationProcs (raam);   // BUG: default Melder_assert would call printf recursively!!!
		#endif
		trace (U"creating the menu bar in the Objects window");
		GuiWindow_addMenuBar (raam);
		praatP.menuBar = raam;
		praat_addMenus (praatP.menuBar);

		trace (U"creating the object list in the Objects window");
		GuiLabel_createShown (raam, 3, -250, Machine_getMainWindowMenuBarHeight () + 5, Machine_getMainWindowMenuBarHeight () + 5 + Gui_LABEL_HEIGHT, U"Objects:", 0);
		praatList_objects = GuiList_create (raam, 0, -250, Machine_getMainWindowMenuBarHeight () + 26, -100, true, U" Objects ");
		GuiList_setSelectionChangedCallback (praatList_objects, gui_cb_list_selectionChanged, nullptr);
		GuiThing_show (praatList_objects);
		praat_addFixedButtons (raam);

		trace (U"creating the dynamic menu in the Objects window");
		praat_actions_createDynamicMenu (raam);
		trace (U"showing the Objects window");
		GuiThing_show (raam);
	//Melder_fatal (U"stop");
		#if defined (UNIX) && ! defined (NO_GUI)
			try {
				autofile f = Melder_fopen (& pidFile, "a");
				#if ALLOW_GDK_DRAWING
					fprintf (f, " %ld", (long_not_integer) GDK_WINDOW_XID (GDK_DRAWABLE (GTK_WIDGET (theCurrentPraatApplication -> topShell -> d_gtkWindow) -> window)));
				#else
					fprintf (f, " %ld", (long_not_integer) GDK_WINDOW_XID (gtk_widget_get_window (GTK_WIDGET (theCurrentPraatApplication -> topShell -> d_gtkWindow))));
				#endif
				f.close (& pidFile);
			} catch (MelderError) {
				Melder_clearError ();
			}
		#endif
		#ifdef UNIX
			if (! praatP.ignorePreferenceFiles) {
				Preferences_read (& prefsFile);
			}
		#endif
		#if ! defined (macintosh)
			trace (U"initializing the Gui late");
			injectMessageAndInformationProcs (theCurrentPraatApplication -> topShell);   // Mac: done this earlier
		#endif
		Melder_setHelpProc (helpProc);
	}
	Data_setPublishProc (publishProc);
	theCurrentPraatApplication -> manPages = ManPages_create ().releaseToAmbiguousOwner();

	trace (U"creating the Picture window");
	trace (U"before picture window shows: locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
	if (! praatP.dontUsePictureWindow) praat_picture_init ();
	trace (U"after picture window shows: locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));

	if (unknownCommandLineOption) {
		Melder_fatal (U"Unrecognized command line option ", unknownCommandLineOption.get());
	}
}

static void executeStartUpFile (MelderDir startUpDirectory, conststring32 fileNameHead, conststring32 fileNameTail) {
	char32 name [256];
	Melder_sprint (name,256, fileNameHead, programName, fileNameTail);
	if (! MelderDir_isNull (startUpDirectory)) {   // should not occur on modern systems
		structMelderFile startUp { };
		MelderDir_getFile (startUpDirectory, name, & startUp);
		if (! MelderFile_readable (& startUp))
			return;   // it's OK if the file doesn't exist
		try {
			praat_executeScriptFromFile (& startUp, nullptr);
		} catch (MelderError) {
			Melder_flushError (praatP.title.get(), U": start-up file ", & startUp, U" not completed.");
		}
	}
}

#if gtk
	#include <gdk/gdkkeysyms.h>
	#if ALLOW_GDK_DRAWING
		static gint theKeySnooper (GtkWidget *widget, GdkEventKey *event, gpointer data) {
			trace (U"keyval ", event -> keyval, U", type ", event -> type);
			if ((event -> keyval == GDK_Tab || event -> keyval == GDK_ISO_Left_Tab) && event -> type == GDK_KEY_PRESS) {
				trace (U"tab key pressed in window ", Melder_pointer (widget));
				constexpr bool theTabKeyShouldWorkEvenIfNumLockIsOn = true;
				constexpr uint32 theProbableNumLockModifierMask = GDK_MOD2_MASK;
				constexpr uint32 modifiersToIgnore = ( theTabKeyShouldWorkEvenIfNumLockIsOn ? theProbableNumLockModifierMask : 0 );
				constexpr uint32 modifiersNotToIgnore = GDK_MODIFIER_MASK & ~ modifiersToIgnore;
				if ((event -> state & modifiersNotToIgnore) == 0) {
					if (GTK_IS_WINDOW (widget)) {
						GtkWidget *shell = gtk_widget_get_toplevel (GTK_WIDGET (widget));
						trace (U"tab pressed in GTK window ", Melder_pointer (shell));
						void (*tabCallback) (GuiObject, gpointer) = (void (*) (GuiObject, gpointer)) g_object_get_data (G_OBJECT (widget), "tabCallback");
						if (tabCallback) {
							trace (U"a tab callback exists");
							void *tabClosure = g_object_get_data (G_OBJECT (widget), "tabClosure");
							tabCallback (widget, tabClosure);
							return true;
						}
					}
				} else if ((event -> state & modifiersNotToIgnore) == GDK_SHIFT_MASK) {
					if (GTK_IS_WINDOW (widget)) {
						GtkWidget *shell = gtk_widget_get_toplevel (GTK_WIDGET (widget));
						trace (U"shift-tab pressed in GTK window ", Melder_pointer (shell));
						void (*tabCallback) (GuiObject, gpointer) = (void (*) (GuiObject, gpointer)) g_object_get_data (G_OBJECT (widget), "shiftTabCallback");
						if (tabCallback) {
							trace (U"a shift tab callback exists");
							void *tabClosure = g_object_get_data (G_OBJECT (widget), "shiftTabClosure");
							tabCallback (widget, tabClosure);
							return true;
						}
					}
				}
			}
			trace (U"end");
			return false;   // pass event on
		}
	#endif
#endif

void praat_run () {
	trace (U"adding menus, second round");
	praat_addMenus2 ();
	trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));

	trace (U"adding the Quit command");
	praat_addMenuCommand (U"Objects", U"Praat", U"-- quit --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Praat", U"Quit", nullptr, praat_UNHIDABLE | 'Q' | praat_NO_API, DO_Quit);

	trace (U"read the preferences file, and notify those who want to be notified of this");
	/* ...namely, those who already have a window (namely, the Picture window),
	 * and those that regard the start of a new session as a meaningful event
	 * (namely, the session counter and the cross-session memory counter).
	 */
	if (! praatP.ignorePreferenceFiles) {
		Preferences_read (& prefsFile);
		if (! praatP.dontUsePictureWindow) praat_picture_prefsChanged ();
		praat_statistics_prefsChanged ();
	}

	praatP.phase = praat_STARTING_UP;

	trace (U"execute start-up file(s)");
	/*
	 * On Unix and the Mac, we try no less than three start-up file names.
	 */
	#if defined (UNIX) || defined (macintosh)
		structMelderDir usrLocal { };
		Melder_pathToDir (U"/usr/local", & usrLocal);
		executeStartUpFile (& usrLocal, U"", U"-startUp");
	#endif
	#if defined (UNIX) || defined (macintosh)
		executeStartUpFile (& homeDir, U".", U"-user-startUp");   // not on Windows (empty file name error)
	#endif
	#if defined (UNIX) || defined (macintosh) || defined (_WIN32)
		executeStartUpFile (& homeDir, U"", U"-user-startUp");
	#endif

	if (! MelderDir_isNull (& praatDir) && ! praatP.ignorePlugins) {
		trace (U"install plug-ins");
		trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		/* The Praat phase should remain praat_STARTING_UP,
		 * because any added commands must not be included in the buttons file.
		 */
		structMelderFile searchPattern { };
		MelderDir_getFile (& praatDir, U"plugin_*", & searchPattern);
		try {
			autoStrings directoryNames = Strings_createAsDirectoryList (Melder_fileToPath (& searchPattern));
			if (directoryNames -> numberOfStrings > 0) {
				for (integer i = 1; i <= directoryNames -> numberOfStrings; i ++) {
					structMelderDir pluginDir { };
					structMelderFile plugin { };
					MelderDir_getSubdir (& praatDir, directoryNames -> strings [i].get(), & pluginDir);
					MelderDir_getFile (& pluginDir, U"setup.praat", & plugin);
					if (MelderFile_readable (& plugin)) {
						Melder_backgrounding = true;
						try {
							praat_executeScriptFromFile (& plugin, nullptr);
						} catch (MelderError) {
							Melder_flushError (praatP.title.get(), U": plugin ", & plugin, U" contains an error.");
						}
						Melder_backgrounding = false;
					}
				}
			}
		} catch (MelderError) {
			Melder_clearError ();   // in case Strings_createAsDirectoryList () threw an error
		}
	}

	/*
		Check the locale, to ensure identical behaviour on all computers.
	*/
	Melder_assert (str32equ (Melder_double (1.5), U"1.5"));   // check locale settings; because of the required file portability Praat cannot stand "1,5"
	Melder_assert (Melder_isHorizontalOrVerticalSpace (' '));
	Melder_assert (Melder_isHorizontalOrVerticalSpace ('\r'));
	Melder_assert (Melder_isHorizontalOrVerticalSpace ('\n'));
	Melder_assert (Melder_isHorizontalOrVerticalSpace ('\t'));
	Melder_assert (Melder_isHorizontalOrVerticalSpace ('\f'));
	Melder_assert (Melder_isHorizontalOrVerticalSpace ('\v'));

	/*
		According to ISO 30112, a non-breaking space is not a space.
		We do not agree, as long as spaces are assumed to be word breakers:
		non-breaking spaces are used to prevent line breaks, not to prevent word breaks.
		For instance, in English it's possible to insert a non-breaking space
		after "e.g." in "...take drastic measures, e.g. pose a ban on...",
		just because otherwise a line would end in a period that does not signal end of sentence;
		this use does not mean that "e.g." and "pose" aren't separate words.
	*/
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_NO_BREAK_SPACE));

	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_OGHAM_SPACE_MARK));   // ISO 30112

	/*
		According to ISO 30112, a Mongolian vowel separator is a space.
		However, this character is used to separate vowels *within* a word,
		so it should not be a word breaker.
		This means that as long as all spaces are assumed to be word breakers,
		this character cannot be a space.
	*/
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_MONGOLIAN_VOWEL_SEPARATOR));

	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_EN_QUAD));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_EM_QUAD));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_EN_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_EM_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_THREE_PER_EM_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_FOUR_PER_EM_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_SIX_PER_EM_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_FIGURE_SPACE));   // questionable
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_PUNCTUATION_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_THIN_SPACE));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_HAIR_SPACE));   // ISO 30112
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ZERO_WIDTH_SPACE));   // questionable
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ZERO_WIDTH_NON_JOINER));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ZERO_WIDTH_JOINER));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ZERO_WIDTH_NO_BREAK_SPACE));   // this is the byte-order mark!
	//Melder_assert (iswspace (UNICODE_LEFT_TO_RIGHT_MARK));
	//Melder_assert (iswspace (UNICODE_RIGHT_TO_LEFT_MARK));
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_LINE_SEPARATOR));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_PARAGRAPH_SEPARATOR));   // ISO 30112
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_NARROW_NO_BREAK_SPACE));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_LEFT_TO_RIGHT_EMBEDDING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_RIGHT_TO_LEFT_EMBEDDING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_POP_DIRECTIONAL_FORMATTING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_LEFT_TO_RIGHT_OVERRIDE));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_RIGHT_TO_LEFT_OVERRIDE));
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_MEDIUM_MATHEMATICAL_SPACE));   // ISO 30112
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_WORD_JOINER));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_FUNCTION_APPLICATION));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_INVISIBLE_TIMES));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_INVISIBLE_SEPARATOR));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_INHIBIT_SYMMETRIC_SWAPPING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ACTIVATE_SYMMETRIC_SWAPPING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_INHIBIT_ARABIC_FORM_SHAPING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_ACTIVATE_ARABIC_FORM_SHAPING));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_NATIONAL_DIGIT_SHAPES));
	Melder_assert (! Melder_isHorizontalOrVerticalSpace (UNICODE_NOMINAL_DIGIT_SHAPES));
	Melder_assert (Melder_isHorizontalOrVerticalSpace (UNICODE_IDEOGRAPHIC_SPACE));   // ISO 30112; occurs on Japanese computers

	{ unsigned char dummy = 200;
		Melder_assert ((int) dummy == 200);
	}
	Melder_assert (integer_abs (-1000) == integer (1000));
	{ int dummy = 200;
		Melder_assert ((int) (signed char) dummy == -56);   // bingeti8 relies on this
		Melder_assert ((int) (unsigned char) dummy == 200);
		Melder_assert ((double) dummy == 200.0);
		Melder_assert ((double) (signed char) dummy == -56.0);
		Melder_assert ((double) (unsigned char) dummy == 200.0);
	}
	{ int64 dummy = 200;
		Melder_assert ((int) (signed char) dummy == -56);
		Melder_assert ((int) (unsigned char) dummy == 200);
		Melder_assert ((double) dummy == 200.0);
		Melder_assert ((double) (signed char) dummy == -56.0);
		Melder_assert ((double) (unsigned char) dummy == 200.0);
	}
	{ uint16 dummy = 40000;
		Melder_assert ((int) (int16) dummy == -25536);   // bingeti16 relies on this
		Melder_assert ((short) (int16) dummy == -25536);   // bingete16 relies on this
		Melder_assert ((integer) dummy == 40000);   // Melder_integer relies on this
		Melder_assert ((double) dummy == 40000.0);
		Melder_assert ((double) (int16) dummy == -25536.0);
	}
	{ unsigned int dummy = 40000;
		Melder_assert ((int) (int16) dummy == -25536);
		Melder_assert ((short) (int16) dummy == -25536);
		Melder_assert ((integer) dummy == 40000);   // Melder_integer relies on this
		Melder_assert ((double) dummy == 40000.0);
		Melder_assert ((double) (int16) dummy == -25536.0);
	}
	{
		int64 dummy = 1000000000000;
		if (! str32equ (Melder_integer (dummy), U"1000000000000"))
			Melder_fatal (U"The number 1000000000000 is mistakenly written on this machine as ", dummy, U".");
	}
	{ uint32 dummy = 0xffffffff;
		Melder_assert ((int64) dummy == 4294967295LL);
		Melder_assert (str32equ (Melder_integer (dummy), U"4294967295"));
		Melder_assert (double (dummy) == 4294967295.0);
	}
	{ double dummy = 3000000000.0;
		Melder_assert ((uint32) dummy == 3000000000);
	}
	{
		Melder_assert (str32len (U"hello") == 5);
		Melder_assert (str32ncmp (U"hellogoodbye", U"hellogee", 6) == 0);
		Melder_assert (str32ncmp (U"hellogoodbye", U"hellogee", 7) > 0);
		Melder_assert (str32str (U"hellogoodbye", U"ogo"));
		Melder_assert (! str32str (U"hellogoodbye", U"oygo"));
	}
	Melder_assert (isundef (undefined));
	Melder_assert (isinf (1.0 / 0.0));
	Melder_assert (isnan (0.0 / 0.0));
	{
		double x = sqrt (-10.0);
		//if (! isnan (x)) printf ("sqrt (-10.0) = %g\n", x);   // -10.0 on Windows
		x = NUMsqrt (-10.0);
		Melder_assert (isundef (x));
	}
	Melder_assert (isdefined (0.0));
	Melder_assert (isdefined (1e300));
	Melder_assert (isundef ((double) 1e320L));
	Melder_assert (isundef (pow (10.0, 330)));
	Melder_assert (isundef (0.0 / 0.0));
	Melder_assert (isundef (1.0 / 0.0));
	Melder_assert (undefined != undefined);
	Melder_assert (undefined - undefined != 0.0);
	Melder_assert ((1.0/0.0) == (1.0/0.0));
	Melder_assert ((1.0/0.0) - (1.0/0.0) != 0.0);
	{
		/*
			Assumptions made in abcio.cpp:
			`frexp()` returns an infinity if its argument is an infinity,
			and not-a-number if its argument is not-a-number.
		*/
		int exponent;
		Melder_assert (isundef (frexp (HUGE_VAL, & exponent)));
		Melder_assert (isundef (frexp (0.0/0.0, & exponent)));
		Melder_assert (isundef (frexp (undefined, & exponent)));
		/*
			The following relies on the facts that:
			- positive infinity is not less than 1.0 (because it is greater than 1.0)
			- NaN is not less than 1.0 (because it is not ordered)
			
			Note: we cannot replace `! (... < 1.0)` with `... >= 1.0`,
			because `! (NaN < 1.0)` is true but `NaN >= 1.0` is false.
		*/
		Melder_assert (! (frexp (HUGE_VAL, & exponent) < 1.0));
		Melder_assert (! (frexp (0.0/0.0, & exponent) < 1.0));
		Melder_assert (! (frexp (undefined, & exponent) < 1.0));
	}
	{
		VEC xn;   // uninitialized
		Melder_assert (! xn.at);
		Melder_assert (xn.size == 0);
		VEC x { };
		Melder_assert (! x.at);
		Melder_assert (x.size == 0);
		constVEC xnc;   // zero-initialized
		Melder_assert (! xnc.at);
		Melder_assert (xnc.size == 0);
		constVEC xc { };
		Melder_assert (! xc.at);
		Melder_assert (xc.size == 0);
		MAT yn;
		Melder_assert (! yn.cells);
		Melder_assert (yn.nrow == 0);
		Melder_assert (yn.ncol == 0);
		MAT y { };
		Melder_assert (! y.cells);
		Melder_assert (y.nrow == 0);
		Melder_assert (y.ncol == 0);
		//autoMAT z {y.at,y.nrow,y.ncol};   // explicit construction not OK
		autoMAT a = autoMAT { };
		Melder_assert (! a.cells);
		Melder_assert (a.nrow == 0);
		Melder_assert (a.ncol == 0);
		//a = z.move();
		//double q [11];
		//autoVEC s { & q [1], 10 };
		//autoVEC b { x };   // explicit construction not OK
		//autoVEC c = x;   // implicit construction not OK
	}
	static_assert (sizeof (float) == 4,
		"sizeof(float) should be 4");
	static_assert (sizeof (double) == 8,
		"sizeof(double) should be 8");
	static_assert (sizeof (longdouble) >= 8,
		"sizeof(longdouble) should be at least 8");   // this can be 8, 12 or 16
	static_assert (sizeof (integer) == sizeof (void *),
		"sizeof(integer) should equal the size of a pointer");
	static_assert (sizeof (off_t) >= 8,
		"sizeof(off_t) is less than 8. Compile Praat with -D_FILE_OFFSET_BITS=64.");

	if (Melder_batch) {
		if (thePraatStandAloneScriptText) {
			try {
				praat_executeScriptFromText (thePraatStandAloneScriptText);
				praat_exit (0);
			} catch (MelderError) {
				Melder_flushError (praatP.title.get(), U": stand-alone script session interrupted.");
				praat_exit (-1);
			}
		} else if (praatP.hasCommandLineInput) {
			try {
				praat_executeCommandFromStandardInput (praatP.title.get());
				praat_exit (0);
			} catch (MelderError) {
				Melder_flushError (praatP.title.get(), U": command line session interrupted.");
				praat_exit (-1);
			}
		} else {
			try {
				//Melder_casual (U"Script <<", theCurrentPraatApplication -> batchName.string, U">>");
				praat_executeScriptFromFileNameWithArguments (theCurrentPraatApplication -> batchName.string);
				praat_exit (0);
			} catch (MelderError) {
				Melder_flushError (praatP.title.get(), U": script command <<",
					theCurrentPraatApplication -> batchName.string, U">> not completed.");
				praat_exit (-1);
			}
		}
	} else /* GUI */ {
		if (! praatP.ignorePreferenceFiles) {
			trace (U"reading the added script buttons");
			/* Each line separately: every error should be ignored.
			 */
			praatP.phase = praat_READING_BUTTONS;
			{// scope
				autostring32 buttons;
				try {
					buttons = MelderFile_readText (& buttonsFile);
				} catch (MelderError) {
					Melder_clearError ();
				}
				if (buttons) {
					char32 *line = buttons.get();
					for (;;) {
						char32 *newline = str32chr (line, U'\n');
						if (newline) *newline = U'\0';
						try {
							praat_executeCommand (nullptr, line);
						} catch (MelderError) {
							Melder_clearError ();   // ignore this line, but not necessarily the next
						}
						if (! newline) break;
						line = newline + 1;
					}
				}
			}
		}

		trace (U"sorting the commands");
		trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		praat_sortMenuCommands ();
		praat_sortActions ();

		praatP.phase = praat_HANDLING_EVENTS;

		if (praatP.userWantsToOpen) {
			for (; praatP.argumentNumber < praatP.argc; praatP.argumentNumber ++) {
				//Melder_casual (U"File to open <<", Melder_peek8to32 (theArgv [iarg]), U">>");
				/*
					The use double-clicked a Praat file,
					or dropped a file on the Praat icon,
					while Praat was not yet running.
				*/
				autostring32 text = Melder_dup (Melder_cat (U"Read from file... ",
															Melder_peek8to32 (praatP.argv [praatP.argumentNumber])));
				try {
					praat_executeScriptFromText (text.get());
				} catch (MelderError) {
					Melder_flushError ();
				}
			}
		}

		#if gtk
			//gtk_widget_add_events (G_OBJECT (theCurrentPraatApplication -> topShell), GDK_ALL_EVENTS_MASK);
			trace (U"install GTK key snooper");
			trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
			#if ALLOW_GDK_DRAWING
				g_signal_connect (G_OBJECT (theCurrentPraatApplication -> topShell -> d_gtkWindow), "client-event",
					G_CALLBACK (cb_userMessage), nullptr);
			#endif
			signal (SIGUSR1, cb_sigusr1);
			#if ALLOW_GDK_DRAWING
				gtk_key_snooper_install (theKeySnooper, 0);
			#endif
			trace (U"start the GTK event loop");
			trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
			gtk_main ();
		#elif motif
			for (;;) {
				XEvent event;
				GuiNextEvent (& event);
				XtDispatchEvent (& event);
			}
		#elif cocoa
			[NSApp run];
		#endif
	}
}

/* End of file praat.cpp */
