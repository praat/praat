/* praat.cpp
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

#include "melder.h"
#include "NUMmachar.h"
#include <ctype.h>
#include <stdarg.h>
#if defined (UNIX) || defined (macintosh)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <signal.h>
#endif
#include <locale.h>
#ifdef macintosh
	#include "macport_on.h"
    #if useCarbon
        #include <Carbon/Carbon.h>
    #endif
	#include "macport_off.h"
#endif
#if defined (UNIX)
	#include <unistd.h>
#endif

#include "praatP.h"
#include "praat_script.h"
#include "site.h"
#include "machine.h"
#include "Printer.h"
#include "ScriptEditor.h"
#include "Strings_.h"

#if gtk
	#include <gdk/gdkx.h>
#endif

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
static structMelderDir homeDir = { { 0 } };
/*
 * praatDirectory: preferences file, buttons file, message files, tracing file, plugins.
 *    Unix:   /u/miep/.myProg-dir   (without slash)
 *    Windows XP/Vista/7:   \\myserver\myshare\Miep\MyProg
 *                    or:   C:\Users\Miep\MyProg
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs
 */
extern structMelderDir praatDir;
structMelderDir praatDir = { { 0 } };
/*
 * prefsFile: preferences file.
 *    Unix:   /u/miep/.myProg-dir/prefs5
 *    Windows XP/Vista/7:   \\myserver\myshare\Miep\MyProg\Preferences5.ini
 *                       or:   C:\Users\Miep\MyProg\Preferences5.ini
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Prefs5
 */
static structMelderFile prefsFile = { 0 };
/*
 * buttonsFile: buttons file.
 *    Unix:   /u/miep/.myProg-dir/buttons
 *    Windows XP/Vista/7:   \\myserver\myshare\Miep\MyProg\Buttons5.ini
 *                    or:   C:\Users\Miep\MyProg\Buttons5.ini
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Buttons5
 */
static structMelderFile buttonsFile = { 0 };
#if defined (UNIX)
	static structMelderFile pidFile = { 0 };   // like /u/miep/.myProg-dir/pid
	static structMelderFile messageFile = { 0 };   // like /u/miep/.myProg-dir/message
#elif defined (_WIN32)
	static structMelderFile messageFile = { 0 };   // like C:\Users\Miep\myProg\Message.txt
#endif
/*
 * tracingFile: tracing file.
 *    Unix:   /u/miep/.myProg-dir/tracing
 *    Windows XP/Vista/7:   \\myserver\myshare\Miep\MyProg\Tracing.txt
 *                    or:   C:\Users\Miep\MyProg\Tracing.txt
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Tracing.txt
 */
static structMelderFile tracingFile = { 0 };

static GuiList praatList_objects;

/***** selection *****/

long praat_getIdOfSelected (ClassInfo klas, int inplace) {
	int place = inplace, IOBJECT;
	if (place == 0) place = 1;
	if (place > 0) {
		WHERE (SELECTED && (klas == NULL || CLASS == klas)) {
			if (place == 1) return ID;
			place --;
		}
	} else {
		WHERE_DOWN (SELECTED && (klas == NULL || CLASS == klas)) {
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

char32 * praat_getNameOfSelected (ClassInfo klas, int inplace) {
	int place = inplace, IOBJECT;
	if (place == 0) place = 1;
	if (place > 0) {
		WHERE (SELECTED && (klas == NULL || CLASS == klas)) {
			if (place == 1) return klas == NULL ? FULL_NAME : NAME;
			place --;
		}
	} else {
		WHERE_DOWN (SELECTED && (klas == NULL || CLASS == klas)) {
			if (place == -1) return klas == NULL ? FULL_NAME : NAME;
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

int praat_selection (ClassInfo klas) {
	if (klas == NULL) return theCurrentPraatObjects -> totalSelection;
	long readableClassId = klas -> sequentialUniqueIdOfReadableClass;
	if (readableClassId == 0) Melder_fatal (U"No sequential unique ID for class ", klas -> className, U".");
	return theCurrentPraatObjects -> numberOfSelected [readableClassId];
}

void praat_deselect (int IOBJECT) {
	if (! SELECTED) return;
	SELECTED = FALSE;
	theCurrentPraatObjects -> totalSelection -= 1;
	long readableClassId = ((Thing) theCurrentPraatObjects -> list [IOBJECT]. object) -> classInfo -> sequentialUniqueIdOfReadableClass;
	Melder_assert (readableClassId != 0);
	theCurrentPraatObjects -> numberOfSelected [readableClassId] -= 1;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding) {
		GuiList_deselectItem (praatList_objects, IOBJECT);
	}
}

void praat_deselectAll (void) { int IOBJECT; WHERE (1) praat_deselect (IOBJECT); }

void praat_select (int IOBJECT) {
	if (SELECTED) return;
	SELECTED = TRUE;
	theCurrentPraatObjects -> totalSelection += 1;
	Thing object = (Thing) theCurrentPraatObjects -> list [IOBJECT]. object;
	Melder_assert (object != NULL);
	long readableClassId = object -> classInfo -> sequentialUniqueIdOfReadableClass;
	if (readableClassId == 0) Melder_fatal (U"No sequential unique ID for class ", object -> classInfo -> className, U".");
	theCurrentPraatObjects -> numberOfSelected [readableClassId] += 1;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding) {
		GuiList_selectItem (praatList_objects, IOBJECT);
	}
}

void praat_selectAll (void) { int IOBJECT; WHERE (1) praat_select (IOBJECT); }

void praat_list_background (void) {
	int IOBJECT;
	WHERE (SELECTED) GuiList_deselectItem (praatList_objects, IOBJECT);
}
void praat_list_foreground (void) {
	int IOBJECT;
	WHERE (SELECTED) {
		GuiList_selectItem (praatList_objects, IOBJECT);
	}
}

Data praat_onlyObject (ClassInfo klas) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED && CLASS == klas) { result = IOBJECT; found += 1; }
	if (found != 1) return NULL;
	return theCurrentPraatObjects -> list [result]. object;
}

Data praat_firstObject (ClassInfo klas) {
	int IOBJECT;
	LOOP {
		if (CLASS == klas) return theCurrentPraatObjects -> list [IOBJECT]. object;
	}
	return NULL;   // this is often OK
}

Data praat_onlyObject_generic (ClassInfo klas) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED && Thing_subclass ((ClassInfo) CLASS, klas)) { result = IOBJECT; found += 1; }
	if (found != 1) return NULL;
	return theCurrentPraatObjects -> list [result]. object;
}

Data praat_firstObject_generic (ClassInfo klas) {
	int IOBJECT;
	LOOP {
		if (Thing_subclass ((ClassInfo) CLASS, klas)) return theCurrentPraatObjects -> list [IOBJECT]. object;
	}
	return NULL;   // this is often OK
}

praat_Object praat_onlyScreenObject (void) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED) { result = IOBJECT; found += 1; }
	if (found != 1) Melder_fatal (U"praat_onlyScreenObject: found ", found, U" objects instead of 1.");
	return & theCurrentPraatObjects -> list [result];
}

Data praat_firstObject_any () {
	int IOBJECT;
	LOOP {
		return theCurrentPraatObjects -> list [IOBJECT]. object;
	}
	return NULL;   // this is often OK
}

Collection praat_getSelectedObjects (void) {
	autoCollection thee = Collection_create (NULL, 10);
	Collection_dontOwnItems (thee.peek());
	int IOBJECT;
	LOOP {
		iam_LOOP (Data);
		Collection_addItem (thee.peek(), me);
	}
	return thee.transfer();
}

char32 *praat_name (int IOBJECT) { return str32chr (FULL_NAME, U' ') + 1; }

void praat_write_do (Any dia, const char32 *extension) {
	int IOBJECT, found = 0;
	Data data = NULL;
	static MelderString defaultFileName { 0 };
	WHERE (SELECTED) { if (! data) data = (Data) OBJECT; found += 1; }
	if (found == 1) {
		MelderString_copy (& defaultFileName, data -> name);
		if (defaultFileName.length > 50) { defaultFileName.string [50] = '\0'; defaultFileName.length = 50; }
		MelderString_append (& defaultFileName, U".", extension ? extension : Thing_className (data));
	} else if (extension == NULL) {
		MelderString_copy (& defaultFileName, U"praat.Collection");
	} else {
		MelderString_copy (& defaultFileName, U"praat.", extension);
	}
	UiOutfile_do (dia, defaultFileName.string);
}

static void removeAllReferencesToEditor (Any editor) {
	int iobject, ieditor;
	/*
	 * Remove all references to this editor.
	 * It may be editing multiple objects.
	 */
	for (iobject = 1; iobject <= theCurrentPraatObjects -> n; iobject ++)
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (theCurrentPraatObjects -> list [iobject]. editors [ieditor] == editor)
				theCurrentPraatObjects -> list [iobject]. editors [ieditor] = NULL;
	if (praatP. editor == editor)
		praatP. editor = NULL;
}

static void praat_remove (int iobject) {
/* Remove the "object" from the list. */
/* Kill everything to do with selection. */
	int ieditor;
	Melder_assert (iobject >= 1 && iobject <= theCurrentPraatObjects -> n);
	if (theCurrentPraatObjects -> list [iobject]. _beingCreated) {
		theCurrentPraatObjects -> list [iobject]. _beingCreated = FALSE;
		theCurrentPraatObjects -> totalBeingCreated --;
	}
	praat_deselect (iobject);

	/*
	 * To prevent synchronization problems, kill editors before killing the data.
	 */
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		Editor editor = (Editor) theCurrentPraatObjects -> list [iobject]. editors [ieditor];   /* Save this one reference. */
		if (editor) {
			removeAllReferencesToEditor (editor);
			forget (editor);
		}
	}
	MelderFile_setToNull (& theCurrentPraatObjects -> list [iobject]. file);
	Melder_free (theCurrentPraatObjects -> list [iobject]. name);
	forget (theCurrentPraatObjects -> list [iobject]. object);
}

void praat_cleanUpName (char32 *name) {
	/*
	 * Replaces spaces and special characters by underscores.
	 */
	for (; *name; name ++) {
		#if 1
			if (str32chr (U" ,.:;\\/()[]{}~`\'<>*&^%#@!?$\"|", *name)) *name = U'_';
		#else
			if (! iswalnum (*name) && *name != '-' && *name != '+') *name = '_';
		#endif
	}
}

/***** objects + commands *****/

void praat_newWithFile (Data me, MelderFile file, const char32 *myName) {
	int IOBJECT, ieditor;   // must be local: praat_new can be called from within a loop!!!
	if (me == NULL)
		Melder_throw (U"No object was put into the list.");
	/*
	 * If my class is Collection, I'll have to be unpacked.
	 */
	if (my classInfo == classCollection) {
		Collection list = (Collection) me;
		try {
			for (long idata = 1; idata <= list -> size; idata ++) {
				Data object = (Data) list -> item [idata];
				const char32 *name = object -> name ? object -> name : myName;
				Melder_assert (name != NULL);
				praat_new (object, name);   // recurse
			}
		} catch (MelderError) {
			list -> size = 0;   // disown
			forget (list);
			throw;
		}
		list -> size = 0;   // disown
		forget (list);
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
		praat_cleanUpName (givenName.string);
	} else {
		MelderString_copy (& givenName, my name && my name [0] ? my name : U"untitled");
	}
	MelderString_append (& name, Thing_className (me), U" ", givenName.string);

	if (theCurrentPraatObjects -> n == praat_MAXNUM_OBJECTS) {
		forget (me);
		Melder_throw (U"The Object Window cannot contain more than ", praat_MAXNUM_OBJECTS, U" objects. You could remove some objects.");
	}
		
	IOBJECT = ++ theCurrentPraatObjects -> n;
	Melder_assert (FULL_NAME == NULL);
	FULL_NAME = Melder_dup_f (name.string);   // all right to crash if out of memory
	++ theCurrentPraatObjects -> uniqueId;

	if (! theCurrentPraatApplication -> batch) {   // put a new object on the screen, at the bottom of the list
		GuiList_insertItem (praatList_objects,
			Melder_cat (theCurrentPraatObjects -> uniqueId, U". ", name.string),
			theCurrentPraatObjects -> n);
	}
	OBJECT = me;
	SELECTED = FALSE;
	CLASS = my classInfo;
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		EDITOR [ieditor] = NULL;
	if (file != NULL) {
		MelderFile_copy (file, & theCurrentPraatObjects -> list [IOBJECT]. file);
	} else {
		MelderFile_setToNull (& theCurrentPraatObjects -> list [IOBJECT]. file);
	}
	ID = theCurrentPraatObjects -> uniqueId;
	theCurrentPraatObjects -> list [IOBJECT]. _beingCreated = TRUE;
	Thing_setName ((Thing) OBJECT, givenName.string);
	theCurrentPraatObjects -> totalBeingCreated ++;
}

static MelderString thePraatNewName { 0 };
void praat_new (Data me) {
	praat_newWithFile (me, NULL, U"");
}
void praat_new (Data me, Melder_1_ARG) {
	praat_newWithFile (me, NULL, Melder_1_ARG_CALL);
}
void praat_new (Data me, Melder_2_ARGS) {
	MelderString_copy (& thePraatNewName, Melder_2_ARGS_CALL);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, Melder_3_ARGS) {
	MelderString_copy (& thePraatNewName, Melder_3_ARGS_CALL);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, Melder_4_ARGS) {
	MelderString_copy (& thePraatNewName, Melder_4_ARGS_CALL);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, Melder_5_ARGS) {
	MelderString_copy (& thePraatNewName, Melder_5_ARGS_CALL);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, Melder_6_ARGS) {
	MelderString_copy (& thePraatNewName, Melder_6_ARGS_CALL);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, Melder_7_ARGS) {
	MelderString_copy (& thePraatNewName, Melder_7_ARGS_CALL);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, Melder_8_ARGS) {
	MelderString_copy (& thePraatNewName, Melder_8_ARGS_CALL);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, Melder_9_ARGS) {
	MelderString_copy (& thePraatNewName, Melder_9_ARGS_CALL);
	praat_new (me, thePraatNewName.string);
}

void praat_updateSelection (void) {
	if (theCurrentPraatObjects -> totalBeingCreated) {
		int IOBJECT;
		praat_deselectAll ();
		WHERE (theCurrentPraatObjects -> list [IOBJECT]. _beingCreated) {
			praat_select (IOBJECT);
			theCurrentPraatObjects -> list [IOBJECT]. _beingCreated = FALSE;
		}
		theCurrentPraatObjects -> totalBeingCreated = 0;
		praat_show ();
	}
}

static void gui_cb_list (void *void_me, GuiListEvent event) {
	(void) event; (void) void_me;
	int IOBJECT, first = TRUE;
	WHERE (SELECTED) {
		SELECTED = FALSE;
		long readableClassId = ((Thing) theCurrentPraatObjects -> list [IOBJECT]. object) -> classInfo -> sequentialUniqueIdOfReadableClass;
		theCurrentPraatObjects -> numberOfSelected [readableClassId] --;
		Melder_assert (theCurrentPraatObjects -> numberOfSelected [readableClassId] >= 0);
	}
	theCurrentPraatObjects -> totalSelection = 0;
	long numberOfSelected;
	long *selected = GuiList_getSelectedPositions (praatList_objects, & numberOfSelected);
	if (selected != NULL) {
		for (long iselected = 1; iselected <= numberOfSelected; iselected ++) {
			IOBJECT = selected [iselected];
			SELECTED = TRUE;
			long readableClassId = ((Thing) theCurrentPraatObjects -> list [IOBJECT]. object) -> classInfo -> sequentialUniqueIdOfReadableClass;
			theCurrentPraatObjects -> numberOfSelected [readableClassId] ++;
			Melder_assert (theCurrentPraatObjects -> numberOfSelected [readableClassId] > 0);
			UiHistory_write (first ? U"\nselectObject: \"" : U"\nplusObject: \"");
			UiHistory_write_expandQuotes (FULL_NAME);
			UiHistory_write (U"\"");
			first = FALSE;
			theCurrentPraatObjects -> totalSelection += 1;
		}
		NUMvector_free <long> (selected, 1);
	}
	praat_show ();
}

void praat_list_renameAndSelect (int position, const char32 *name) {
	if (! theCurrentPraatApplication -> batch) {
		GuiList_replaceItem (praatList_objects, name, position);   // void if name equal
		if (! Melder_backgrounding)
			GuiList_selectItem (praatList_objects, position);
	}
}

/***** objects *****/

void praat_name2 (char32 *name, ClassInfo klas1, ClassInfo klas2) {
	int i1 = 1;
	while (theCurrentPraatObjects -> list [i1]. selected == 0 || theCurrentPraatObjects -> list [i1]. klas != klas1) i1 ++;
	int i2 = 1;
	while (theCurrentPraatObjects -> list [i2]. selected == 0 || theCurrentPraatObjects -> list [i2]. klas != klas2) i2 ++;
	char32 *name1 = str32chr (theCurrentPraatObjects -> list [i1]. name, U' ') + 1;
	char32 *name2 = str32chr (theCurrentPraatObjects -> list [i2]. name, U' ') + 1;
	if (str32equ (name1, name2))
		Melder_sprint (name,200, name1);
	else
		Melder_sprint (name,200, name1, U"_", name2);
}

void praat_removeObject (int i) {
	int j, ieditor;
	praat_remove (i);   /* Dangle. */
	for (j = i; j < theCurrentPraatObjects -> n; j ++)
		theCurrentPraatObjects -> list [j] = theCurrentPraatObjects -> list [j + 1];   // undangle but create second references
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. name = NULL;   // undangle or remove second reference
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. object = NULL;   // undangle or remove second reference
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. selected = 0;
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. editors [ieditor] = NULL;   // undangle or remove second reference
	MelderFile_setToNull (& theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. file);   // undangle or remove second reference
	-- theCurrentPraatObjects -> n;
	if (! theCurrentPraatApplication -> batch) {
		GuiList_deleteItem (praatList_objects, i);
	}
}

static void praat_exit (int exit_code) {
	int IOBJECT;
	#ifdef _WIN32
		if (! theCurrentPraatApplication -> batch) {
			Melder_assert (theCurrentPraatApplication);
			Melder_assert (theCurrentPraatApplication -> topShell);
			Melder_assert (theCurrentPraatApplication -> topShell -> d_xmShell);
			trace (U"destroy the object window");
			XtDestroyWidget (theCurrentPraatApplication -> topShell -> d_xmShell);
		}
	#endif
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
					long pid;
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
				MelderString_append (& buffer, U"# This file is generated automatically when you quit the ", praatP.title, U" program.\n");
				MelderString_append (& buffer, U"# It contains the buttons that you added interactively to the fixed or dynamic menus,\n");
				MelderString_append (& buffer, U"# and the buttons that you hid or showed.\n\n");
				praat_saveMenuCommands (& buffer);
				praat_saveAddedActions (& buffer);
				MelderFile_writeText (& buttonsFile, buffer.string, kMelder_textOutputEncoding_ASCII_THEN_UTF16);
			} catch (MelderError) {
				Melder_clearError ();
			}
		}
	}

	trace (U"flush the file-based objects");
	WHERE_DOWN (! MelderFile_isNull (& theCurrentPraatObjects -> list [IOBJECT]. file)) praat_remove (IOBJECT);
	Melder_files_cleanUp ();   // in case a URL is open

	trace (U"leave the program");
	exit (exit_code);
}

static void cb_Editor_destruction (Editor me, void *closure) {
	(void) closure;
	removeAllReferencesToEditor (me);   // remove reference(s) to moribund Editor
}

static void cb_Editor_dataChanged (Editor me, void *closure) {
	(void) closure;
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
				Editor otherEditor = (Editor) theCurrentPraatObjects -> list [iobject]. editors [ieditor];
				if (otherEditor != NULL && otherEditor != me) {
					Editor_dataChanged (otherEditor);
				}
			}
		}
	}
}

static void cb_Editor_publication (Editor me, void *closure, Data publication) {
/*
   The default publish callback.
   Works nicely if the publisher invents a name.
*/
	(void) me;
	(void) closure;
	try {
		praat_new (publication, U"");
	} catch (MelderError) {
		Melder_flushError ();
	}
	praat_updateSelection ();
}

int praat_installEditor (Editor editor, int IOBJECT) {
	if (editor == NULL) return 0;
	for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		if (EDITOR [ieditor] == NULL) {
			EDITOR [ieditor] = editor;
			Editor_setDestructionCallback (editor, cb_Editor_destruction, NULL);
			Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
			if (! editor -> d_publicationCallback)
				Editor_setPublicationCallback (editor, cb_Editor_publication, NULL);
			return 1;
		}
	}
	forget (editor);
	Melder_throw (U"(praat_installEditor:) Cannot have more than ", praat_MAXNUM_EDITORS, U" editors with one object.");
}

int praat_installEditor2 (Editor editor, int i1, int i2) {
	if (editor == NULL) return 0;
	int ieditor1 = 0;
	for (; ieditor1 < praat_MAXNUM_EDITORS; ieditor1 ++)
		if (theCurrentPraatObjects -> list [i1]. editors [ieditor1] == NULL)
			break;
	int ieditor2 = 0;
	for (; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (theCurrentPraatObjects -> list [i2]. editors [ieditor2] == NULL)
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS) {
		theCurrentPraatObjects -> list [i1]. editors [ieditor1] = theCurrentPraatObjects -> list [i2]. editors [ieditor2] = editor;
		Editor_setDestructionCallback (editor, cb_Editor_destruction, NULL);
		Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
		if (! editor -> d_publicationCallback)
			Editor_setPublicationCallback (editor, cb_Editor_publication, NULL);
	} else {
		forget (editor);
		Melder_throw (U"(praat_installEditor2:) Cannot have more than ", praat_MAXNUM_EDITORS, U" editors with one object.");
	}
	return 1;
}

int praat_installEditor3 (Editor editor, int i1, int i2, int i3) {
	if (! editor) return 0;
	int ieditor1 = 0;
	for (; ieditor1 < praat_MAXNUM_EDITORS; ieditor1 ++)
		if (theCurrentPraatObjects -> list [i1]. editors [ieditor1] == NULL)
			break;
	int ieditor2 = 0;
	for (; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (theCurrentPraatObjects -> list [i2]. editors [ieditor2] == NULL)
			break;
	int ieditor3 = 0;
	for (; ieditor3 < praat_MAXNUM_EDITORS; ieditor3 ++)
		if (theCurrentPraatObjects -> list [i3]. editors [ieditor3] == NULL)
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS && ieditor3 < praat_MAXNUM_EDITORS) {
		theCurrentPraatObjects -> list [i1]. editors [ieditor1] = theCurrentPraatObjects -> list [i2]. editors [ieditor2] = theCurrentPraatObjects -> list [i3]. editors [ieditor3] = editor;
		Editor_setDestructionCallback (editor, cb_Editor_destruction, NULL);
		Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
		if (! editor -> d_publicationCallback)
			Editor_setPublicationCallback (editor, cb_Editor_publication, NULL);
	} else {
		forget (editor);
		Melder_throw (U"(praat_installEditor3:) Cannot have more than ", praat_MAXNUM_EDITORS, U" editors with one object.");
	}
	return 1;
}

int praat_installEditorN (Editor editor, Ordered objects) {
	long iOrderedObject, iPraatObject;
	if (editor == NULL) return 0;
	/*
	 * First check whether all objects in the Ordered are also in the List of Objects (Praat crashes if not),
	 * and check whether there is room to add an editor for each.
	 */
	for (iOrderedObject = 1; iOrderedObject <= objects -> size; iOrderedObject ++) {
		Data object = (Data) objects -> item [iOrderedObject];
		for (iPraatObject = 1; iPraatObject <= theCurrentPraatObjects -> n; iPraatObject ++) {
			if (object == theCurrentPraatObjects -> list [iPraatObject]. object) {
				int ieditor = 0;
				for (; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor] == NULL) {
						break;
					}
				}
				if (ieditor >= praat_MAXNUM_EDITORS) {
					forget (editor);
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
	for (iOrderedObject = 1; iOrderedObject <= objects -> size; iOrderedObject ++) {
		Data object = (Data) objects -> item [iOrderedObject];
		for (iPraatObject = 1; iPraatObject <= theCurrentPraatObjects -> n; iPraatObject ++) {
			if (object == theCurrentPraatObjects -> list [iPraatObject]. object) {
				int ieditor = 0;
				for (; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor] == NULL) {
						theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor] = editor;
						Editor_setDestructionCallback (editor, cb_Editor_destruction, NULL);
						Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
						if (! editor -> d_publicationCallback)
							Editor_setPublicationCallback (editor, cb_Editor_publication, NULL);
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

void praat_dataChanged (Any object) {
	/*
	 * This function can be called at error time, which is weird.
	 */
	char32 *saveError = NULL;
	bool duringError = Melder_hasError ();
	if (duringError) {
		saveError = Melder_dup_f (Melder_getError ());
		Melder_clearError ();
	}
	int IOBJECT;
	WHERE (OBJECT == object) {
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
			Editor editor = (Editor) EDITOR [ieditor];
			if (editor != NULL) {
				Editor_dataChanged (editor);
			}
		}
	}
	if (duringError) {
		Melder_appendError (saveError);   // BUG: this appends an empty newline to the original error message
		Melder_free (saveError);   // BUG: who will catch the error?
	}
}

static void helpProc (const char32 *query) {
	if (theCurrentPraatApplication -> batch) {
		Melder_flushError (U"Cannot view manual from batch.");
		return;
	}
	try {
		Manual_create (query, theCurrentPraatApplication -> manPages, false);
	} catch (MelderError) {
		Melder_flushError (U"help: no help on \"", query, U"\".");
	}
}

static int publishProc (void *anything) {
	try {
		praat_new ((Data) anything, U"");
		praat_updateSelection ();
		return 1;
	} catch (MelderError) {
		Melder_throw (U"Not published.");
	}
}

/***** QUIT *****/

FORM (Quit, U"Confirm Quit", U"Quit")
	LABEL (U"label", U"You have objects in your list!")
	OK
{
	char32 prompt [300];
	if (ScriptEditors_dirty ()) {
		if (theCurrentPraatObjects -> n)
			Melder_sprint (prompt,300, U"You have objects and unsaved scripts! Do you still want to quit ", praatP.title, U"?");
		else
			Melder_sprint (prompt,300, U"You have unsaved scripts! Do you still want to quit ", praatP.title, U"?");
		SET_STRING (U"label", prompt);
	} else if (theCurrentPraatObjects -> n) {
		Melder_sprint (prompt,300, U"You have objects in your list! Do you still want to quit ", praatP.title, U"?");
		SET_STRING (U"label", prompt);
	} else {
		praat_exit (0);
	}
}
DO
	praat_exit (0);
END

static void gui_cb_quit (void *) {
	DO_Quit (NULL, 0, NULL, NULL, NULL, NULL, false, NULL);
}

void praat_dontUsePictureWindow (void) { praatP.dontUsePictureWindow = true; }

/********** INITIALIZATION OF THE PRAAT SHELL **********/

#if defined (UNIX)
	static void cb_sigusr1 (int signum) {
		Melder_assert (signum == SIGUSR1);
		#if 0
			gboolean retval;
			g_signal_emit_by_name (GTK_OBJECT (theCurrentPraatApplication -> topShell -> d_gtkWindow), "client-event", NULL, & retval);
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
		static gboolean cb_userMessage (GtkWidget widget, GdkEventClient *event, gpointer user_data) {
			(void) widget;
			(void) user_data;
			//Melder_casual (U"client event called");
			autofile f;
			try {
				f.reset (Melder_fopen (& messageFile, "r"));
			} catch (MelderError) {
				Melder_clearError ();
				return true;   // OK
			}
			long pid = 0;
			int narg = fscanf (f, "#%ld", & pid);
			f.close (& messageFile);
			{// scope
				autoPraatBackground background;
				try {
					praat_executeScriptFromFile (& messageFile, NULL);
				} catch (MelderError) {
					Melder_flushError (praatP.title, U": message not completely handled.");
				}
			}
			if (narg && pid) kill (pid, SIGUSR2);
			return true;
		}
	#endif
#elif defined (_WIN32)
	static int cb_userMessage (void) {
		autoPraatBackground background;
		try {
			praat_executeScriptFromFile (& messageFile, NULL);
		} catch (MelderError) {
			Melder_flushError (praatP.title, U": message not completely handled.");
		}
		return 0;
	}
	extern "C" char *sendpraat (void *display, const char *programName, long timeOut, const char *text);
	extern "C" wchar_t *sendpraatW (void *display, const wchar_t *programName, long timeOut, const wchar_t *text);
	static void cb_openDocument (MelderFile file) {
		char32 text [500];
		/*
		 * The user dropped a file on the Praat icon, while Praat is already running.
		 * Windows may have enclosed the path between quotes;
		 * this is especially likely to happen for a path that contains spaces.
		 */
		Melder_sprint (text,500, U"Read from file... ", file -> path);
		sendpraatW (NULL, Melder_peek32toW (praatP.title), 0, Melder_peek32toW (text));
	}
#elif cocoa
	static int (*theUserMessageCallback) (char32 *message);
	static void mac_setUserMessageCallback (int (*userMessageCallback) (char32 *message)) {
		theUserMessageCallback = userMessageCallback;
	}
	static pascal OSErr mac_processSignal8 (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon) {
		static int duringAppleEvent = FALSE;
		(void) reply;
		(void) handlerRefCon;
		if (! duringAppleEvent) {
			char *buffer;
			Size actualSize;
			duringAppleEvent = TRUE;
			//AEInteractWithUser (kNoTimeOut, NULL, NULL);   // use time out of 0 to execute immediately (without bringing to foreground)
			ProcessSerialNumber psn;
			GetCurrentProcess (& psn);
			SetFrontProcess (& psn);
			AEGetParamPtr (theAppleEvent, 1, typeUTF8Text, NULL, NULL, 0, & actualSize);
			buffer = (char *) malloc ((size_t) actualSize);
			AEGetParamPtr (theAppleEvent, 1, typeUTF8Text, NULL, & buffer [0], actualSize, NULL);
			if (theUserMessageCallback) {
				autostring32 buffer32 = Melder_8to32 (buffer);
				theUserMessageCallback (buffer32.peek());
			}
			free (buffer);
			duringAppleEvent = FALSE;
		}
		return noErr;
	}
	static pascal OSErr mac_processSignal16 (const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefCon) {
		static int duringAppleEvent = FALSE;
		(void) reply;
		(void) handlerRefCon;
		if (! duringAppleEvent) {
			char16 *buffer;
			Size actualSize;
			duringAppleEvent = TRUE;
			//AEInteractWithUser (kNoTimeOut, NULL, NULL);   // use time out of 0 to execute immediately (without bringing to foreground)
			ProcessSerialNumber psn;
			GetCurrentProcess (& psn);
			SetFrontProcess (& psn);
			AEGetParamPtr (theAppleEvent, 1, typeUTF16ExternalRepresentation, NULL, NULL, 0, & actualSize);
			buffer = (char16 *) malloc ((size_t) actualSize);
			AEGetParamPtr (theAppleEvent, 1, typeUTF16ExternalRepresentation, NULL, & buffer [0], actualSize, NULL);
			if (theUserMessageCallback) {
				autostring32 buffer32 = Melder_16to32 (buffer);
				theUserMessageCallback (buffer32.peek());
			}
			free (buffer);
			duringAppleEvent = FALSE;
		}
		return noErr;
	}
	static int cb_userMessage (char32 *message) {
		autoPraatBackground background;
		try {
			praat_executeScriptFromText (message);
		} catch (MelderError) {
			Melder_flushError (praatP.title, U": message not completely handled.");
		}
		return 0;
	}
	static int cb_quitApplication (void) {
		DO_Quit (NULL, 0, NULL, NULL, NULL, NULL, false, NULL);
		return 0;
	}
#elif defined (macintosh)
	static int cb_userMessage (char32 *message) {
		autoPraatBackground background;
		try {
			praat_executeScriptFromText (message);
		} catch (MelderError) {
			Melder_flushError (praatP.title, U": message not completely handled.");
		}
		return 0;
	}
	static int cb_quitApplication (void) {
		DO_Quit (NULL, 0, NULL, NULL, NULL, NULL, false, NULL);
		return 0;
	}
#endif

static char32 * thePraatStandAloneScriptText = NULL;

void praat_setStandAloneScriptText (char32 *text) {
	thePraatStandAloneScriptText = text;
}

void praat_init (const char32 *title, unsigned int argc, char **argv) {
	for (unsigned int iarg = 0; iarg < argc; iarg ++) {
		//Melder_casual (U"arg ", iarg, U": <<", Melder_peek8to32 (argv [iarg]), U">>");
	}
	static char32 truncatedTitle [300];   // static because praatP.title will point into it
	#if defined (UNIX)
		setlocale (LC_ALL, "C");
		setenv ("PULSE_LATENCY_MSEC", "1", 0);   // Rafael Laboissiere, August 2014
	#elif defined (_WIN32)
		setlocale (LC_ALL, "C");   // said to be superfluous
	#elif defined (macintosh)
		setlocale (LC_ALL, "en_US");   // required to make swprintf work correctly; the default "C" locale does not do that!
	#endif
	#ifdef macintosh
		SInt32 sys1, sys2, sys3;
		Gestalt ('sys1', & sys1);
		Gestalt ('sys2', & sys2);
		Gestalt ('sys3', & sys3);
		Melder_systemVersion = sys1 * 10000 + sys2 * 100 + sys3;
	#endif
	/*
		Initialize numerical libraries.
	*/
	NUMmachar ();
	NUMinit ();
	Melder_alloc_init ();
	Melder_message_init ();
	/*
		Remember the current directory. Only useful for scripts run from batch.
	*/
	Melder_rememberShellDirectory ();

	/*
	 * Install the preferences of the Praat shell, and set the defaults.
	 */
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

	uint32 iarg_batchName = 1;
	const char32 *unknownCommandLineOption = nullptr;

	#if defined (UNIX) || defined (macintosh) || defined (_WIN32) && defined (CONSOLE_APPLICATION)
		/*
		 * Running the Praat shell from the Unix command line,
		 * or running PRAATCON.EXE from the Windows command prompt:
		 *    <programName> <scriptFileName>
		 */
		while (iarg_batchName < argc && argv [iarg_batchName] [0] == '-') {
			if (strequ (argv [iarg_batchName], "-")) {
				praatP.hasCommandLineInput = true;
			} else if (strequ (argv [iarg_batchName], "-a") || strequ (argv [iarg_batchName], "--ansi")) {
				Melder_consoleIsAnsi = true;
				iarg_batchName += 1;
			} else if (strequ (argv [iarg_batchName], "--no-pref-files")) {
				praatP.ignorePreferenceFiles = true;
				iarg_batchName += 1;
			} else if (strequ (argv [iarg_batchName], "--no-plugins")) {
				praatP.ignorePlugins = true;
				iarg_batchName += 1;
			} else if (strnequ (argv [iarg_batchName], "--pref-dir=", 11)) {
				Melder_pathToDir (Melder_peek8to32 (argv [iarg_batchName] + 11), & praatDir);
				iarg_batchName += 1;
			#if defined (macintosh)
			} else if (strequ (argv [iarg_batchName], "-NSDocumentRevisionsDebugMode")) {
				(void) 0;   // ignore this option, which was added by Xcode
				iarg_batchName += 2;   // jump over the argument, which is usually "YES" (this jump works correctly even if this argument is missing)
			} else if (strnequ (argv [iarg_batchName], "-psn_", 5)) {
				(void) 0;   // ignore this option, which was added by the Finder, perhaps when dragging a file on Praat (Process Serial Number)
				iarg_batchName += 1;
			#endif
			} else {
				unknownCommandLineOption = Melder_8to32 (argv [iarg_batchName]);
				iarg_batchName = UINT32_MAX;   // ignore all other command line options
				break;
			}
		}

		/*
		 * We now figure out the script file name, if there is any.
		 * If there is a script file name, it is next on the command line
		 * (not necessarily *last* on the line, because there may be script arguments after it).
		 */
		if (iarg_batchName < argc) {
			MelderString_copy (& theCurrentPraatApplication -> batchName, Melder_peek8to32 (argv [iarg_batchName]));
			if (praatP.hasCommandLineInput) Melder_throw (U"Cannot have both command line input and a script file.");
		} else {
			MelderString_copy (& theCurrentPraatApplication -> batchName, U"");
		}

		Melder_batch = theCurrentPraatApplication -> batchName.string [0] != U'\0' || thePraatStandAloneScriptText != NULL;

		#if defined (_WIN32) && defined (CONSOLE_APPLICATION)
			if (! Melder_batch) {
				fprintf (stderr, "Usage: PRAATCON <scriptFileName>\n");
				exit (0);
			}
		#endif
		/*
		 * Running the Praat shell from the command line:
		 *    praat -
		 */
		if (praatP.hasCommandLineInput) {
			Melder_batch = true;
		}

		/* Take from 'title' ("myProg 3.2" becomes "myProg") or from command line ("/ifa/praat" becomes "praat"). */
		str32cpy (truncatedTitle, argc && argv [0] [0] ? Melder_peek8to32 (argv [0]) : title && title [0] ? title : U"praat");
		//Melder_fatal (U"<", argv [0], U">");
	#else
		#if defined (_WIN32)
			MelderString_copy (& theCurrentPraatApplication -> batchName,
				argv [3] ? Melder_peek8to32 (argv [3]) : U"");   // the command line
		#endif
		Melder_batch = false;   // PRAAT.EXE on Windows is always interactive
		str32cpy (truncatedTitle, title && title [0] ? title : U"praat");
	#endif
	theCurrentPraatApplication -> batch = Melder_batch;

	/*
	 * Construct a program name like "myProg 3.2" by removing directory path.
	 */
	char32 *p = str32rchr (truncatedTitle, Melder_DIRECTORY_SEPARATOR);
	praatP.title = p ? p + 1 : truncatedTitle;

	/*
	 * Construct a program name like "myProg" for file and directory names.
	 */
	str32cpy (programName, praatP.title);
	if ((p = str32chr (programName, U' ')) != NULL) *p = U'\0';
	#if defined (_WIN32)
		if ((p = str32chr (programName, U'.')) != NULL) *p = U'\0';   // chop off ".exe"
	#endif

	/*
	 * Construct a main-window title like "MyProg 3.2".
	 */
	praatP.title [0] = (char32) toupper ((int) praatP.title [0]);

	/*
	 * Get home directory, e.g. "/home/miep/", or "/Users/miep/", or just "/".
	 */
	Melder_getHomeDir (& homeDir);

	/*
	 * Get the program's private directory (if not yet set by the --prefdir option):
	 *    "/u/miep/.myProg-dir" (Unix)
	 *    "/Users/miep/Library/Preferences/MyProg Prefs" (Macintosh)
	 *    "C:\Users\Miep\MyProg" (Windows)
	 * and construct a preferences-file name and a script-buttons-file name like
	 *    /u/miep/.myProg-dir/prefs5
	 *    /u/miep/.myProg-dir/buttons5
	 * or
	 *    /Users/miep/Library/Preferences/MyProg Prefs/Prefs5
	 *    /Users/miep/Library/Preferences/MyProg Prefs/Buttons5
	 * or
	 *    C:\Users\Miep\MyProg\Preferences5.ini
	 *    C:\Users\Miep\MyProg\Buttons5.ini
	 * Also create names for message and tracing files.
	 */
	if (MelderDir_isNull (& praatDir)) {   // not yet set by the --prefdir option?
		structMelderDir prefParentDir { { 0 } };   // directory under which to store our preferences directory
		Melder_getPrefDir (& prefParentDir);

		/*
		 * Make sure that the program's private directory exists.
		 */
		char32 name [256];
		#if defined (UNIX)
			Melder_sprint (name,256, U".", programName, U"-dir");   // for example .myProg-dir
		#elif defined (macintosh)
			Melder_sprint (name,256, praatP.title, U" Prefs");   // for example MyProg Prefs
		#elif defined (_WIN32)
			Melder_sprint (name,256, praatP.title);   // for example MyProg
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
	#if defined (UNIX)
		if (! Melder_batch) {
			/*
			 * Make sure that the directory /u/miep/.myProg-dir exists,
			 * and write our process id into the pid file.
			 * Messages from "sendpraat" are caught very early this way,
			 * though they will be responded to much later.
			 */
			try {
				autofile f = Melder_fopen (& pidFile, "w");
				fprintf (f, "%ld", (long) getpid ());
				f.close (& pidFile);
			} catch (MelderError) {
				Melder_clearError ();
			}
		}
	#elif defined (_WIN32)
		if (! Melder_batch)
			motif_win_setUserMessageCallback (cb_userMessage);
	#elif defined (macintosh)
		#if useCarbon
			if (! Melder_batch) {
				motif_mac_setUserMessageCallback (cb_userMessage);
				Gui_setQuitApplicationCallback (cb_quitApplication);
			}
		#else
			if (! Melder_batch) {
				mac_setUserMessageCallback (cb_userMessage);
				Gui_setQuitApplicationCallback (cb_quitApplication);
			}
		#endif
	#endif

	/*
	 * Make room for commands.
	 */
	trace (U"initing actions");
	praat_actions_init ();
	trace (U"initing menu commands");
	praat_menuCommands_init ();

	GuiWindow raam = NULL;
	if (Melder_batch) {
		#if defined (UNIX) || defined (macintosh) || defined (_WIN32) && defined (CONSOLE_APPLICATION)
			MelderString_empty (& theCurrentPraatApplication -> batchName);
			for (unsigned int i = iarg_batchName; i < argc; i ++) {
				int needsQuoting = strchr (argv [i], ' ') != NULL && (i == iarg_batchName || i < argc - 1);
				if (i > 1) MelderString_append (& theCurrentPraatApplication -> batchName, U" ");
				if (needsQuoting) MelderString_append (& theCurrentPraatApplication -> batchName, U"\"");
				MelderString_append (& theCurrentPraatApplication -> batchName, Melder_peek8to32 (argv [i]));
				if (needsQuoting) MelderString_append (& theCurrentPraatApplication -> batchName, U"\"");
			}
		#elif defined (_WIN32)
			MelderString_copy (& theCurrentPraatApplication -> batchName, Melder_peek8to32 (argv [3]));
		#endif
	} else {
		trace (U"starting the application");
		Machine_initLookAndFeel (argc, argv);
		/*
		 * Start the application.
		 */
		#if gtk
			trace (U"locale ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
			g_set_application_name (Melder_peek32to8 (title));
			trace (U"locale ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
		#elif cocoa
			//[NSApplication sharedApplication];
			[GuiCocoaApplication sharedApplication];
		#elif defined (_WIN32)
			argv [0] = Melder_32to8 (praatP. title);   // argc == 4
			Gui_setOpenDocumentCallback (cb_openDocument);
			GuiAppInitialize ("Praatwulg", NULL, 0, & argc, argv, NULL, NULL);
		#elif defined (macintosh)
			GuiAppInitialize ("Praatwulg", NULL, 0, & argc, argv, NULL, NULL);
		#endif

		trace (U"creating and installing the Objects window");
		char32 objectWindowTitle [100];
		Melder_sprint (objectWindowTitle,100, praatP.title, U" Objects");
		double x, y;
		trace (U"locale ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
		Gui_getWindowPositioningBounds (& x, & y, NULL, NULL);
		trace (U"locale ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
		theCurrentPraatApplication -> topShell = raam = GuiWindow_create (x + 10, y, WINDOW_WIDTH, WINDOW_HEIGHT, 450, 250,
			objectWindowTitle, gui_cb_quit, NULL, 0);
		trace (U"locale ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
		#if motif
			GuiApp_setApplicationShell (theCurrentPraatApplication -> topShell -> d_xmShell);
		#endif
		trace (U"before objects window shows locale ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
		GuiThing_show (raam);
		trace (U"after objects window shows locale ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
	}
	Thing_recognizeClassesByName (classCollection, classStrings, classManPages, classSortedSetOfString, NULL);
	if (Melder_batch) {
		Melder_backgrounding = true;
		trace (U"adding menus without GUI");
		praat_addMenus (NULL);
		trace (U"adding fixed buttons without GUI");
		praat_addFixedButtons (NULL);
	} else {

		#ifdef macintosh
			#if ! useCarbon
				AEInstallEventHandler (758934755, 0, (AEEventHandlerProcPtr) (mac_processSignal8), 0, false);   // for receiving sendpraat
				AEInstallEventHandler (758934756, 0, (AEEventHandlerProcPtr) (mac_processSignal16), 0, false);   // for receiving sendpraatW
			#endif
			MelderGui_create (raam);   /* BUG: default Melder_assert would call printf recursively!!! */
		#endif
		#if defined (macintosh) && useCarbon
			trace (U"creating the menu bar along the top of the screen (Mac only)");
			GuiWindow_addMenuBar (raam);   // yes, on the Mac we create a menu bar twice: once at the top of the screen, once in the Objects window
		#endif
		trace (U"creating the menu bar in the Objects window");
		GuiWindow_addMenuBar (raam);
		praatP.menuBar = raam;
		praat_addMenus (praatP.menuBar);

		trace (U"creating the object list in the Objects window");
		GuiLabel_createShown (raam, 3, -250, Machine_getMainWindowMenuBarHeight () + 5, Machine_getMainWindowMenuBarHeight () + 5 + Gui_LABEL_HEIGHT, U"Objects:", 0);
		praatList_objects = GuiList_create (raam, 0, -250, Machine_getMainWindowMenuBarHeight () + 26, -100, true, U" Objects ");
		GuiList_setSelectionChangedCallback (praatList_objects, gui_cb_list, 0);
		GuiThing_show (praatList_objects);
		praat_addFixedButtons (raam);

		trace (U"creating the dynamic menu in the Objects window");
		praat_actions_createDynamicMenu (raam);
		trace (U"showing the Objects window");
		GuiThing_show (raam);
	//Melder_fatal (U"stop");
		#if defined (UNIX) && ! defined (NO_GRAPHICS)
			try {
				autofile f = Melder_fopen (& pidFile, "a");
				#if ALLOW_GDK_DRAWING
					fprintf (f, " %ld", (long) GDK_WINDOW_XID (GDK_DRAWABLE (GTK_WIDGET (theCurrentPraatApplication -> topShell -> d_gtkWindow) -> window)));
				#else
					fprintf (f, " %ld", (long) GDK_WINDOW_XID (gtk_widget_get_window (GTK_WIDGET (theCurrentPraatApplication -> topShell -> d_gtkWindow))));
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
		#if ! defined (CONSOLE_APPLICATION) && ! defined (macintosh)
			trace (U"initializing the Gui late");
			MelderGui_create (theCurrentPraatApplication -> topShell);   // Mac: done this earlier
		#endif
		Melder_setHelpProc (helpProc);
	}
	Melder_setPublishProc (publishProc);
	theCurrentPraatApplication -> manPages = ManPages_create ();

	trace (U"creating the Picture window");
	trace (U"before picture window shows: locale is ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
	if (! praatP.dontUsePictureWindow) praat_picture_init ();
	trace (U"after picture window shows: locale is ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));

	if (unknownCommandLineOption) {
		Melder_fatal (U"Unrecognized command line option ", unknownCommandLineOption);
	}
}

static void executeStartUpFile (MelderDir startUpDirectory, const char32 *fileNameHead, const char32 *fileNameTail) {
	char32 name [256];
	Melder_sprint (name,256, fileNameHead, programName, fileNameTail);
	if (! MelderDir_isNull (startUpDirectory)) {   // should not occur on modern systems
		structMelderFile startUp = { 0 };
		MelderDir_getFile (startUpDirectory, name, & startUp);
		if (! MelderFile_readable (& startUp))
			return;   // it's OK if the file doesn't exist
		try {
			praat_executeScriptFromFile (& startUp, NULL);
		} catch (MelderError) {
			Melder_flushError (praatP.title, U": start-up file ", & startUp, U" not completed.");
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
				if ((event -> state & GDK_MODIFIER_MASK) == 0) {
					if (GTK_IS_WINDOW (widget)) {
						GtkWidget *shell = gtk_widget_get_toplevel (GTK_WIDGET (widget));
						trace (U"tab pressed in GTK window ", Melder_pointer (shell));
						void (*tabCallback) (GuiObject, gpointer) = (void (*) (GuiObject, gpointer)) g_object_get_data (G_OBJECT (widget), "tabCallback");
						if (tabCallback) {
							trace (U"a tab callback exists");
							void *tabClosure = g_object_get_data (G_OBJECT (widget), "tabClosure");
							tabCallback (widget, tabClosure);
							return TRUE;
						}
					}
				} else if ((event -> state & GDK_MODIFIER_MASK) == GDK_SHIFT_MASK) {
					if (GTK_IS_WINDOW (widget)) {
						GtkWidget *shell = gtk_widget_get_toplevel (GTK_WIDGET (widget));
						trace (U"shift-tab pressed in GTK window ", Melder_pointer (shell));
						void (*tabCallback) (GuiObject, gpointer) = (void (*) (GuiObject, gpointer)) g_object_get_data (G_OBJECT (widget), "shiftTabCallback");
						if (tabCallback) {
							trace (U"a shift tab callback exists");
							void *tabClosure = g_object_get_data (G_OBJECT (widget), "shiftTabClosure");
							tabCallback (widget, tabClosure);
							return TRUE;
						}
					}
				}
			}
			trace (U"end");
			return FALSE;   // pass event on
		}
	#endif
#endif

void praat_run (void) {
	trace (U"adding menus, second round");
	praat_addMenus2 ();
	trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));

	trace (U"adding the Quit command");
	#if defined (macintosh) && useCarbon
		praat_addMenuCommand (U"Objects", U"Praat", U"Quit", 0, praat_HIDDEN, DO_Quit);   // the Quit command is needed for scripts, not for the GUI
	#else
		praat_addMenuCommand (U"Objects", U"Praat", U"-- quit --", 0, 0, 0);
		praat_addMenuCommand (U"Objects", U"Praat", U"Quit", 0, praat_UNHIDABLE + 'Q', DO_Quit);
	#endif

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
		structMelderDir usrLocal = { { 0 } };
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
		trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
		/* The Praat phase should remain praat_STARTING_UP,
		 * because any added commands must not be included in the buttons file.
		 */
		structMelderFile searchPattern { 0 };
		MelderDir_getFile (& praatDir, U"plugin_*", & searchPattern);
		try {
			autoStrings directoryNames = Strings_createAsDirectoryList (Melder_fileToPath (& searchPattern));
			if (directoryNames -> numberOfStrings > 0) {
				for (long i = 1; i <= directoryNames -> numberOfStrings; i ++) {
					structMelderDir pluginDir = { { 0 } };
					structMelderFile plugin = { 0 };
					MelderDir_getSubdir (& praatDir, directoryNames -> strings [i], & pluginDir);
					MelderDir_getFile (& pluginDir, U"setup.praat", & plugin);
					if (MelderFile_readable (& plugin)) {
						Melder_backgrounding = true;
						try {
							praat_executeScriptFromFile (& plugin, NULL);
						} catch (MelderError) {
							Melder_flushError (praatP.title, U": plugin ", & plugin, U" contains an error.");
						}
						Melder_backgrounding = false;
					}
				}
			}
		} catch (MelderError) {
			Melder_clearError ();   // in case Strings_createAsDirectoryList () threw an error
		}
	}

	Melder_assert (str32equ (Melder_double (1.5), U"1.5"));   // check locale settings; because of the required file portability Praat cannot stand "1,5"
	{ int dummy = 200;
		Melder_assert ((int) (signed char) dummy == -56);   // bingeti1 relies on this
		Melder_assert ((int) (unsigned char) dummy == 200);
		Melder_assert ((double) dummy == 200.0);
		Melder_assert ((double) (signed char) dummy == -56.0);
		Melder_assert ((double) (unsigned char) dummy == 200.0);
	}
	{ uint16 dummy = 40000;
		Melder_assert ((int) (int16_t) dummy == -25536);   // bingeti2 relies on this
		Melder_assert ((short) (int16_t) dummy == -25536);   // bingete2 relies on this
		Melder_assert ((double) dummy == 40000.0);
		Melder_assert ((double) (int16_t) dummy == -25536.0);
	}
	{
		int64 dummy = 1000000000000;
		if (! str32equ (Melder_integer (dummy), U"1000000000000"))
			Melder_fatal (U"The number 1000000000000 is mistaken written on this machine as ", dummy, U".");
	}
	{ uint32_t dummy = 0xffffffff;
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
		Melder_assert (str32str (U"hellogoodbye", U"ogo") != NULL);
		Melder_assert (str32str (U"hellogoodbye", U"oygo") == NULL);
	}

	if (sizeof (off_t) < 8)
		Melder_fatal (U"sizeof(off_t) is less than 8. Compile Praat with -D_FILE_OFFSET_BITS=64.");

	if (Melder_batch) {
		if (thePraatStandAloneScriptText != NULL) {
			try {
				praat_executeScriptFromText (thePraatStandAloneScriptText);
				praat_exit (0);
			} catch (MelderError) {
				Melder_flushError (praatP.title, U": stand-alone script session interrupted.");
				praat_exit (-1);
			}
		} else if (praatP.hasCommandLineInput) {
			try {
				praat_executeCommandFromStandardInput (praatP.title);
				praat_exit (0);
			} catch (MelderError) {
				Melder_flushError (praatP.title, U": command line session interrupted.");
				praat_exit (-1);
			}
		} else {
			try {
				praat_executeScriptFromFileNameWithArguments (theCurrentPraatApplication -> batchName.string);
				praat_exit (0);
			} catch (MelderError) {
				/*
				 * Try to get the error message out; this is a bit complicated...
				 */
				structMelderFile batchFile { 0 };
				try {
					Melder_relativePathToFile (theCurrentPraatApplication -> batchName.string, & batchFile);
				} catch (MelderError) {
					praat_exit (-1);
				}
				#if defined (_WIN32) && ! defined (CONSOLE_APPLICATION)
					MelderGui_create (NULL);
				#endif
				Melder_flushError (praatP.title, U": command file ", & batchFile, U" not completed.");
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
					buttons.reset (MelderFile_readText (& buttonsFile));
				} catch (MelderError) {
					Melder_clearError ();
				}
				if (buttons.peek()) {
					char32 *line = buttons.peek();
					for (;;) {
						char32 *newline = str32chr (line, U'\n');
						if (newline) *newline = U'\0';
						try {
							praat_executeCommand (NULL, line);
						} catch (MelderError) {
							Melder_clearError ();   // ignore this line, but not necessarily the next
						}
						if (newline == NULL) break;
						line = newline + 1;
					}
				}
			}
		}

		trace (U"sorting the commands");
		trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
		praat_sortMenuCommands ();
		praat_sortActions ();

		praatP.phase = praat_HANDLING_EVENTS;

		#if gtk
			//gtk_widget_add_events (G_OBJECT (theCurrentPraatApplication -> topShell), GDK_ALL_EVENTS_MASK);
			trace (U"install GTK key snooper");
			trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
			#if ALLOW_GDK_DRAWING
				g_signal_connect (G_OBJECT (theCurrentPraatApplication -> topShell -> d_gtkWindow), "client-event", G_CALLBACK (cb_userMessage), NULL);
			#endif
			signal (SIGUSR1, cb_sigusr1);
			#if ALLOW_GDK_DRAWING
				gtk_key_snooper_install (theKeySnooper, 0);
			#endif
			trace (U"start the GTK event loop");
			trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, NULL)));
			gtk_main ();
		#elif cocoa
			[NSApp run];
		#elif motif
			#if defined (_WIN32)
				if (theCurrentPraatApplication -> batchName.string [0] != U'\0') {
					char32 text [500];
					/*
					 * The user dropped one or more files on the Praat icon, while Praat was not running yet.
					 * Windows may have enclosed each path between quotes;
					 * this is especially likely to happen for paths that contain spaces (which is usual).
					 */

					char32 *s = theCurrentPraatApplication -> batchName.string;
					for (;;) {
						bool endSeen = false;
						while (*s == U' ' || *s == U'\n') s ++;
						if (*s == '\0') break;
						char32 *path = s;
						if (*s == U'\"') {
							path = ++ s;
							while (*s != U'\"' && *s != U'\0') s ++;
							if (*s == '\0') break;
							Melder_assert (*s == U'\"');
							*s = U'\0';
						} else {
							while (*s != U' ' && *s != U'\n' && *s != U'\0') s ++;
							if (*s == U' ' || *s == U'\n') {
								*s = U'\0';
							} else {
								endSeen = true;
							}
						}
						autostring32 text = Melder_dup (Melder_cat (U"Read from file... ", path));
						try {
							praat_executeScriptFromText (text.peek());
						} catch (MelderError) {
							Melder_flushError ();
						}
						if (endSeen) break;
						s ++;
					}
				}
			#endif
			for (;;) {
				XEvent event;
				GuiNextEvent (& event);
				XtDispatchEvent (& event);
			}
		#endif
	}
}

/* End of file praat.cpp */
