/* praat.cpp
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
static int doingCommandLineInterface;
static char programName [64];
static structMelderDir homeDir = { { 0 } };
/*
 * praatDirectory: preferences and buttons files.
 *    Unix:   /u/miep/.myProg-dir   (without slash)
 *    Windows XP/Vista/7:   \\myserver\myshare\Miep\MyProg
 *                    or:   C:\Documents and settings\Miep\MyProg
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs
 */
extern structMelderDir praatDir;
structMelderDir praatDir = { { 0 } };
/*
 * prefsFile: preferences file.
 *    Unix:   /u/miep/.myProg-dir/prefs5
 *    Windows XP/Vista/7:   \\myserver\myshare\Miep\MyProg\Preferences5.ini
 *                       or:   C:\Documents and settings\Miep\MyProg\Preferences5.ini
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Prefs5
 */
static structMelderFile prefsFile = { 0 };
/*
 * buttonsFile: buttons file.
 *    Unix:   /u/miep/.myProg-dir/buttons
 *    Windows XP/Vista/7:   \\myserver\myshare\Miep\MyProg\Buttons5.ini
 *                    or:   C:\Documents and settings\Miep\MyProg\Buttons5.ini
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Buttons5
 */
static structMelderFile buttonsFile = { 0 };
#if defined (UNIX)
	static structMelderFile pidFile = { 0 };   /* Like /u/miep/.myProg-dir/pid */
	static structMelderFile messageFile = { 0 };   /* Like /u/miep/.myProg-dir/message */
#elif defined (_WIN32)
	static structMelderFile messageFile = { 0 };   /* Like C:\Windows\myProg\Message.txt */
#endif
/*
 * tracingFile: tracing file.
 *    Unix:   /u/miep/.myProg-dir/tracing
 *    Windows XP/Vista/7:   \\myserver\myshare\Miep\MyProg\Tracing.txt
 *                    or:   C:\Documents and settings\Miep\MyProg\Tracing.txt
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
		Melder_throw ("No ", klas ? klas -> className : L"object", " #", inplace, " selected.");
	} else {
		Melder_throw ("No ", klas ? klas -> className : L"object", " selected.");
	}
	return 0;
}

wchar_t * praat_getNameOfSelected (ClassInfo klas, int inplace) {
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
		Melder_throw ("No ", klas ? klas -> className : L"object", " #", inplace, " selected.");
	} else {
		Melder_throw ("No ", klas ? klas -> className : L"object", " selected.");
	}
	return 0;   // Failure.
}

int praat_selection (ClassInfo klas) {
	if (klas == NULL) return theCurrentPraatObjects -> totalSelection;
	long readableClassId = klas -> sequentialUniqueIdOfReadableClass;
	if (readableClassId == 0) Melder_fatal ("No sequential unique ID for class %ls.", klas -> className);
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
		praatList_objects -> f_deselectItem (IOBJECT);
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
	if (readableClassId == 0) Melder_fatal ("No sequential unique ID for class %ls.", object -> classInfo -> className);
	theCurrentPraatObjects -> numberOfSelected [readableClassId] += 1;
	if (! theCurrentPraatApplication -> batch && ! Melder_backgrounding) {
		praatList_objects -> f_selectItem (IOBJECT);
	}
}

void praat_selectAll (void) { int IOBJECT; WHERE (1) praat_select (IOBJECT); }

void praat_list_background (void) {
	int IOBJECT;
	WHERE (SELECTED) praatList_objects -> f_deselectItem (IOBJECT);
}
void praat_list_foreground (void) {
	int IOBJECT;
	WHERE (SELECTED) {
		praatList_objects -> f_selectItem (IOBJECT);
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
	if (found != 1) Melder_fatal ("praat_onlyScreenObject: found %d objects instead of 1.", found);
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

wchar_t *praat_name (int IOBJECT) { return wcschr (FULL_NAME, ' ') + 1; }

void praat_write_do (Any dia, const wchar_t *extension) {
	int IOBJECT, found = 0;
	Data data = NULL;
	static MelderString defaultFileName = { 0 };
	MelderString_empty (& defaultFileName);
	WHERE (SELECTED) { if (! data) data = (Data) OBJECT; found += 1; }
	if (found == 1) {
		MelderString_append (& defaultFileName, data -> name);
		if (defaultFileName.length > 50) { defaultFileName.string [50] = '\0'; defaultFileName.length = 50; }
		MelderString_append (& defaultFileName, L".", extension ? extension : Thing_className (data));
	} else if (extension == NULL) {
		MelderString_append (& defaultFileName, L"praat.Collection");
	} else {
		MelderString_append (& defaultFileName, L"praat.", extension);
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

void praat_cleanUpName (wchar_t *name) {
	/*
	 * Replaces spaces and special characters by underscores.
	 */
	for (; *name; name ++) {
		#if 1
			if (wcschr (L" ,.:;\\/()[]{}~`\'<>*&^%#@!?$\"|", *name)) *name = '_';
		#else
			if (! iswalnum (*name) && *name != '-' && *name != '+') *name = '_';
		#endif
	}
}

/***** objects + commands *****/

void praat_newWithFile (Data me, const wchar_t *myName, MelderFile file) {
	int IOBJECT, ieditor;   // must be local: praat_new can be called from within a loop!!!
	if (me == NULL)
		Melder_throw ("No object was put into the list.");
	/*
	 * If my class is Collection, I'll have to be unpacked.
	 */
	if (my classInfo == classCollection) {
		Collection list = (Collection) me;
		try {
			for (long idata = 1; idata <= list -> size; idata ++) {
				Data object = (Data) list -> item [idata];
				const wchar_t *name = object -> name ? object -> name : myName;
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

	MelderString name = { 0 }, givenName = { 0 };
	if (myName && myName [0]) {
		MelderString_copy (& givenName, myName);
		/*
		 * Remove extension.
		 */
		wchar_t *p = wcsrchr (givenName.string, '.');
		if (p) *p = '\0';
		praat_cleanUpName (givenName.string);
	} else {
		MelderString_copy (& givenName, my name && my name [0] ? my name : L"untitled");
	}
	MelderString_append (& name, Thing_className (me), L" ", givenName.string);

	if (theCurrentPraatObjects -> n == praat_MAXNUM_OBJECTS) {
		forget (me);
		Melder_throw ("The Object Window cannot contain more than ", praat_MAXNUM_OBJECTS, " objects. You could remove some objects.");
	}
		
	IOBJECT = ++ theCurrentPraatObjects -> n;
	Melder_assert (FULL_NAME == NULL);
	FULL_NAME = Melder_wcsdup_f (name.string);   // all right to crash if out of memory
	++ theCurrentPraatObjects -> uniqueId;

	if (! theCurrentPraatApplication -> batch) {   // put a new object on the screen, at the bottom of the list
		MelderString listName = { 0 };
		MelderString_append (& listName, Melder_integer (theCurrentPraatObjects -> uniqueId), L". ", name.string);
		praatList_objects -> f_insertItem (listName.string, theCurrentPraatObjects -> n);
		MelderString_free (& listName);
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
	MelderString_free (& givenName);
	MelderString_free (& name);
}

static MelderString thePraatNewName = { 0 };
void praat_newWithFile (Data me, const wchar_t *s1, const wchar_t *s2, MelderFile file) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2);
	praat_newWithFile (me, thePraatNewName.string, file);
}
void praat_newWithFile (Data me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, MelderFile file) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2, s3);
	praat_newWithFile (me, thePraatNewName.string, file);
}
void praat_newWithFile (Data me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, MelderFile file) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2, s3, s4);
	praat_newWithFile (me, thePraatNewName.string, file);
}
void praat_newWithFile (Data me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, MelderFile file) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2, s3, s4, s5);
	praat_newWithFile (me, thePraatNewName.string, file);
}
void praat_new (Data me, const wchar_t *s1) {
	praat_newWithFile (me, s1, NULL);
}
void praat_new (Data me, const wchar_t *s1, const wchar_t *s2) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2, s3);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2, s3, s4);
	return praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2, s3, s4, s5);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2, s3, s4, s5, s6);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2, s3, s4, s5, s6, s7);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2, s3, s4, s5, s6, s7, s8);
	praat_new (me, thePraatNewName.string);
}
void praat_new (Data me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	MelderString_empty (& thePraatNewName);
	MelderString_append (& thePraatNewName, s1, s2, s3, s4, s5, s6, s7, s8, s9);
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
	long numberOfSelected, *selected = praatList_objects -> f_getSelectedPositions (& numberOfSelected);
	if (selected != NULL) {
		for (long iselected = 1; iselected <= numberOfSelected; iselected ++) {
			IOBJECT = selected [iselected];
			SELECTED = TRUE;
			long readableClassId = ((Thing) theCurrentPraatObjects -> list [IOBJECT]. object) -> classInfo -> sequentialUniqueIdOfReadableClass;
			theCurrentPraatObjects -> numberOfSelected [readableClassId] ++;
			Melder_assert (theCurrentPraatObjects -> numberOfSelected [readableClassId] > 0);
			UiHistory_write (first ? L"\nselectObject (\"" : L"\nplusObject (\"");
			UiHistory_write_expandQuotes (FULL_NAME);
			UiHistory_write (L"\")");
			first = FALSE;
			theCurrentPraatObjects -> totalSelection += 1;
		}
		NUMvector_free <long> (selected, 1);
	}
	praat_show ();
}

void praat_list_renameAndSelect (int position, const wchar_t *name) {
	if (! theCurrentPraatApplication -> batch) {
		praatList_objects -> f_replaceItem (name, position);   // void if name equal
		if (! Melder_backgrounding)
			praatList_objects -> f_selectItem (position);
	}
}

/***** objects *****/

void praat_name2 (wchar_t *name, ClassInfo klas1, ClassInfo klas2) {
	int i1 = 1;
	while (theCurrentPraatObjects -> list [i1]. selected == 0 || theCurrentPraatObjects -> list [i1]. klas != klas1) i1 ++;
	int i2 = 1;
	while (theCurrentPraatObjects -> list [i2]. selected == 0 || theCurrentPraatObjects -> list [i2]. klas != klas2) i2 ++;
	wchar_t *name1 = wcschr (theCurrentPraatObjects -> list [i1]. name, ' ') + 1;
	wchar_t *name2 = wcschr (theCurrentPraatObjects -> list [i2]. name, ' ') + 1;
	if (wcsequ (name1, name2))
		wcscpy (name, name1);
	else
		swprintf (name, 200, L"%ls_%ls", name1, name2);
}

void praat_removeObject (int i) {
	int j, ieditor;
	praat_remove (i);   /* Dangle. */
	for (j = i; j < theCurrentPraatObjects -> n; j ++)
		theCurrentPraatObjects -> list [j] = theCurrentPraatObjects -> list [j + 1];   /* Undangle but create second references. */
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. name = NULL;   /* Undangle or remove second reference. */
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. object = NULL;   /* Undangle or remove second reference. */
	theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. selected = 0;
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. editors [ieditor] = NULL;   /* Undangle or remove second reference. */
	MelderFile_setToNull (& theCurrentPraatObjects -> list [theCurrentPraatObjects -> n]. file);   /* Undangle or remove second reference. */
	-- theCurrentPraatObjects -> n;
	if (! theCurrentPraatApplication -> batch) {
		praatList_objects -> f_deleteItem (i);
	}
}

static void praat_exit (int exit_code) {
	int IOBJECT;
	#ifdef _WIN32
		if (! theCurrentPraatApplication -> batch) {
			Melder_assert (theCurrentPraatApplication);
			Melder_assert (theCurrentPraatApplication -> topShell);
			Melder_assert (theCurrentPraatApplication -> topShell -> d_xmShell);
			trace ("destroy the object window");
			XtDestroyWidget (theCurrentPraatApplication -> topShell -> d_xmShell);
		}
	#endif
	trace ("destroy the picture window");
	praat_picture_exit ();
	praat_statistics_exit ();   /* Record total memory use across sessions. */

	trace ("stop receiving messages");
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

	trace ("save the preferences");
	Melder_assert (wcsequ (Melder_double (1.5), L"1.5"));   // refuse to write the preferences if the locale is wrong (even if tracing is on)
	Preferences_write (& prefsFile);

	trace ("save the script buttons");
	if (! theCurrentPraatApplication -> batch) {
		try {
			autoMelderString buffer;
			MelderString_append (& buffer, L"# Buttons (1).\n");
			MelderString_append (& buffer, L"# This file is generated automatically when you quit the ", Melder_peekUtf8ToWcs (praatP.title), L" program.\n");
			MelderString_append (& buffer, L"# It contains the buttons that you added interactively to the fixed or dynamic menus,\n");
			MelderString_append (& buffer, L"# and the buttons that you hid or showed.\n\n");
			praat_saveMenuCommands (& buffer);
			praat_saveAddedActions (& buffer);
			MelderFile_writeText (& buttonsFile, buffer.string, kMelder_textOutputEncoding_ASCII_THEN_UTF16);
		} catch (MelderError) {
			Melder_clearError ();
		}
	}

	trace ("flush the file-based objects");
	WHERE_DOWN (! MelderFile_isNull (& theCurrentPraatObjects -> list [IOBJECT]. file)) praat_remove (IOBJECT);
	Melder_files_cleanUp ();   /* If a URL is open. */

	trace ("leave the program");
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
					otherEditor -> dataChanged ();
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
		praat_new (publication, NULL);
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
	praat_updateSelection ();
}

int praat_installEditor (Editor editor, int IOBJECT) {
	if (editor == NULL) return 0;
	for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		if (EDITOR [ieditor] == NULL) {
			EDITOR [ieditor] = editor;
			editor -> setDestructionCallback (cb_Editor_destruction, NULL);
			editor -> setDataChangedCallback (cb_Editor_dataChanged, NULL);
			if (! editor -> d_publicationCallback) editor -> setPublicationCallback (cb_Editor_publication, NULL);
			return 1;
		}
	}
	forget (editor);
	Melder_throw ("(praat_installEditor:) Cannot have more than ", praat_MAXNUM_EDITORS, " editors with one object.");
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
		editor -> setDestructionCallback (cb_Editor_destruction, NULL);
		editor -> setDataChangedCallback (cb_Editor_dataChanged, NULL);
		if (! editor -> d_publicationCallback) editor -> setPublicationCallback (cb_Editor_publication, NULL);
	} else {
		forget (editor);
		Melder_throw ("(praat_installEditor2:) Cannot have more than ", praat_MAXNUM_EDITORS, " editors with one object.");
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
		editor -> setDestructionCallback (cb_Editor_destruction, NULL);
		editor -> setDataChangedCallback (cb_Editor_dataChanged, NULL);
		if (! editor -> d_publicationCallback) editor -> setPublicationCallback (cb_Editor_publication, NULL);
	} else {
		forget (editor);
		Melder_throw ("(praat_installEditor3:) Cannot have more than ", praat_MAXNUM_EDITORS, " editors with one object.");
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
					Melder_throw ("Cannot view the same object in more than ", praat_MAXNUM_EDITORS, " windows.");
				}
				break;
			}
		}
		Melder_assert (iPraatObject <= theCurrentPraatObjects -> n);   /* An element of the Ordered does not occur in the List of Objects. */
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
						editor -> setDestructionCallback (cb_Editor_destruction, NULL);
						editor -> setDataChangedCallback (cb_Editor_dataChanged, NULL);
						if (! editor -> d_publicationCallback) editor -> setPublicationCallback (cb_Editor_publication, NULL);
						break;
					}
				}
				Melder_assert (ieditor < praat_MAXNUM_EDITORS);   /* We just checked, but nevertheless. */
				break;
			}
		}
		Melder_assert (iPraatObject <= theCurrentPraatObjects -> n);   /* We already checked, but still. */
	}
	return 1;
}

void praat_dataChanged (Any object) {
	/*
	 * This function can be called at error time, which is weird.
	 */
	wchar_t *saveError = NULL;
	bool duringError = Melder_hasError ();
	if (duringError) {
		saveError = Melder_wcsdup_f (Melder_getError ());
		Melder_clearError ();
	}
	int IOBJECT;
	WHERE (OBJECT == object) {
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
			Editor editor = (Editor) EDITOR [ieditor];
			if (editor != NULL) {
				editor -> dataChanged ();
			}
		}
	}
	if (duringError) {
		Melder_error_ (saveError);   // BUG: this appends an empty newline to the original error message
		Melder_free (saveError);   // BUG: who will catch the error?
	}
}

static void helpProc (const wchar_t *query) {
	if (theCurrentPraatApplication -> batch) {
		Melder_flushError ("Cannot view manual from batch.");
		return;
	}
	try {
		Manual_create (query, theCurrentPraatApplication -> manPages, false);
	} catch (MelderError) {
		Melder_flushError ("help: no help on \"%ls\".", query);
	}
}

static int publishProc (void *anything) {
	try {
		praat_new ((Data) anything, NULL);
		praat_updateSelection ();
		return 1;
	} catch (MelderError) {
		Melder_throw ("Not published.");
	}
}

/***** QUIT *****/

FORM (Quit, L"Confirm Quit", L"Quit")
	LABEL (L"label", L"You have objects in your list!")
	OK
{
	wchar_t prompt [300];
	if (ScriptEditors_dirty ()) {
		if (theCurrentPraatObjects -> n)
			swprintf (prompt, 300, L"You have objects and unsaved scripts! Do you still want to quit %ls?", Melder_peekUtf8ToWcs (praatP.title));
		else
			swprintf (prompt, 300, L"You have unsaved scripts! Do you still want to quit %ls?", Melder_peekUtf8ToWcs (praatP.title));
		SET_STRING (L"label", prompt);
	} else if (theCurrentPraatObjects -> n) {
		swprintf (prompt, 300, L"You have objects in your list! Do you still want to quit %ls?", Melder_peekUtf8ToWcs (praatP.title));
		SET_STRING (L"label", prompt);
	} else {
		praat_exit (0);
	}
}
DO
	praat_exit (0);
END

static void gui_cb_quit (void *p) {
	DO_Quit (NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL);
}

void praat_dontUsePictureWindow (void) { praatP.dontUsePictureWindow = TRUE; }

/********** INITIALIZATION OF THE PRAAT SHELL **********/

#if defined (UNIX)
	static void cb_sigusr1 (int signum) {
		Melder_assert (signum == SIGUSR1);
		#if 0
			gboolean retval;
			g_signal_emit_by_name (GTK_OBJECT (theCurrentPraatApplication -> topShell -> d_gtkWindow), "client-event", NULL, & retval);
		#else
			GdkEventClient gevent;
			gevent. type = GDK_CLIENT_EVENT;
			gevent. window = GTK_WIDGET (theCurrentPraatApplication -> topShell -> d_gtkWindow) -> window;
			gevent. send_event = 1;
			gevent. message_type = gdk_atom_intern_static_string ("SENDPRAAT");
			gevent. data_format = 8;
			// Melder_casual ("event put");
			gdk_event_put ((GdkEvent *) & gevent);
		#endif
	}
#endif

#if defined (UNIX)
	static gboolean cb_userMessage (GtkWidget widget, GdkEventClient *event, gpointer user_data) {
		(void) widget;
		(void) user_data;
		//Melder_casual ("client event called");
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
				Melder_error_ (Melder_peekUtf8ToWcs (praatP.title), L": message not completely handled.");
				Melder_flushError (NULL);
			}
		}
		if (narg && pid) kill (pid, SIGUSR2);
		return true;
	}
#elif defined (_WIN32)
	static int cb_userMessage (void) {
		autoPraatBackground background;
		try {
			praat_executeScriptFromFile (& messageFile, NULL);
		} catch (MelderError) {
			Melder_error_ (Melder_peekUtf8ToWcs (praatP.title), L": message not completely handled.");
			Melder_flushError (NULL);
		}
		return 0;
	}
	extern "C" char *sendpraat (void *display, const char *programName, long timeOut, const char *text);
	extern "C" wchar_t *sendpraatW (void *display, const wchar_t *programName, long timeOut, const wchar_t *text);
	static void cb_openDocument (MelderFile file) {
		wchar_t text [500];
		wchar_t *s = file -> path;
		swprintf (text, 500, L"Read from file... %ls", s [0] == ' ' && s [1] == '\"' ? s + 2 : s [0] == '\"' ? s + 1 : s);
		long l = wcslen (text);
		if (l > 0 && text [l - 1] == '\"') text [l - 1] = '\0';
		sendpraatW (NULL, Melder_peekUtf8ToWcs (praatP.title), 0, text);
	}
#elif defined (macintosh)
	static int cb_userMessageA (char *messageA) {
		autoPraatBackground background;
		autostring message = Melder_8bitToWcs (messageA, 0);
		try {
			praat_executeScriptFromText (message.peek());
		} catch (MelderError) {
			Melder_error_ (praatP.title, ": message not completely handled.");
			Melder_flushError (NULL);
		}
		return 0;
	}
	static int cb_userMessageW (wchar_t *message) {
		autoPraatBackground background;
		try {
			praat_executeScriptFromText (message);
		} catch (MelderError) {
			Melder_error_ (praatP.title, ": message not completely handled.");
			Melder_flushError (NULL);
		}
		return 0;
	}
	static int cb_quitApplication (void) {
		DO_Quit (NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL);
		return 0;
	}
#endif

static wchar_t * thePraatStandAloneScriptText = NULL;

void praat_setStandAloneScriptText (wchar_t *text) {
	thePraatStandAloneScriptText = text;
}

void praat_init (const char *title, unsigned int argc, char **argv) {
	static char truncatedTitle [300];   /* Static because praatP.title will point into it. */
	#if defined (UNIX)
		setlocale (LC_ALL, "C");
	#elif defined (_WIN32)
		setlocale (LC_ALL, "C");   // said to be superfluous
	#elif defined (macintosh)
		setlocale (LC_ALL, "en_US");   // required to make swprintf work correctly; the default "C" locale does not do that!
	#endif
	char *p;
	#ifdef macintosh
		SInt32 macSystemVersion;
		Gestalt ('sysv', & macSystemVersion);
		Melder_systemVersion = macSystemVersion;
	#endif
	/*
		Initialize numerical libraries.
	*/
	NUMmachar ();
	NUMinit ();
	Melder_alloc_init ();
	/*
		Remember the current directory. Only useful for scripts run from batch.
	*/
	Melder_rememberShellDirectory ();

	/*
	 * Install the preferences of the Praat shell, and set the defaults.
	 */
	praat_statistics_prefs ();   // Number of sessions, memory used...
	praat_picture_prefs ();   // Font...
	structEditor     :: f_preferences ();   // Erase picture first...
	structHyperPage  :: f_preferences ();   // Font...
	Site_prefs ();   // Print command...
	Melder_audio_prefs ();   // Use speaker (Sun & HP), output gain (HP)...
	Melder_textEncoding_prefs ();
	Printer_prefs ();   // Paper size, printer command...
	structTextEditor :: f_preferences ();   // Font size...

	unsigned int iarg_batchName = 1;
	#if defined (UNIX) || defined (macintosh) || defined (_WIN32) && defined (CONSOLE_APPLICATION)
		/*
		 * Running the Praat shell from the Unix command line,
		 * or running PRAATCON.EXE from the Windows command prompt:
		 *    <programName> <scriptFileName>
		 */
		if (argc > iarg_batchName
			&& argv [iarg_batchName] [0] == '-'
			&& argv [iarg_batchName] [1] == 'a'
			&& argv [iarg_batchName] [2] == '\0')
		{
			Melder_consoleIsAnsi = true;
			iarg_batchName ++;
		}
		//fprintf (stdout, "Console <%d> <%s>", Melder_consoleIsAnsi, argv [1]);
		bool hasCommandLineInput =
			argc > iarg_batchName
			&& argv [iarg_batchName] [0] == '-'
			&& argv [iarg_batchName] [1] == '\0';
		MelderString_copy (& theCurrentPraatApplication -> batchName,
			hasCommandLineInput ? L""
			: argc > iarg_batchName && argv [iarg_batchName] [0] != '-' /* funny Mac test */ ? Melder_peekUtf8ToWcs (argv [iarg_batchName])
			: L"");

		Melder_batch = theCurrentPraatApplication -> batchName.string [0] != '\0' || thePraatStandAloneScriptText != NULL;

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
		if (hasCommandLineInput) {
			Melder_batch = true;
			doingCommandLineInterface = TRUE;   /* Read from stdin. */
		}

		/* Take from 'title' ("myProg 3.2" becomes "myProg") or from command line ("/ifa/praat" becomes "praat"). */
		strcpy (truncatedTitle, argc && argv [0] [0] ? argv [0] : title && title [0] ? title : "praat");
		//Melder_fatal ("<%s>", argv [0]);
	#else
		#if defined (_WIN32)
			MelderString_copy (& theCurrentPraatApplication -> batchName,
				argv [3] ? Melder_peekUtf8ToWcs (argv [3]) : L"");   /* The command line. */
		#endif
		Melder_batch = false;   // PRAAT.EXE on Windows is always interactive
		strcpy (truncatedTitle, title && title [0] ? title : "praat");
	#endif
	theCurrentPraatApplication -> batch = Melder_batch;

	/*
	 * Construct a program name like "myProg 3.2" by removing directory path.
	 */
	p = strrchr (truncatedTitle, Melder_DIRECTORY_SEPARATOR);
	praatP.title = p ? p + 1 : truncatedTitle;

	/*
	 * Construct a program name like "myProg" for file and directory names.
	 */
	strcpy (programName, praatP.title);
	if ((p = strchr (programName, ' ')) != NULL) *p = '\0';
	#if defined (_WIN32)
		if ((p = strchr (programName, '.')) != NULL) *p = '\0';   /* Chop off ".exe". */
	#endif

	/*
	 * Construct a main-window title like "MyProg 3.2".
	 */
	praatP.title [0] = toupper (praatP.title [0]);

	/*
	 * Get home directory, e.g. "/home/miep/", or "/Users/miep/", or just "/".
	 */
	Melder_getHomeDir (& homeDir);
	/*
	 * Get the program's private directory:
	 *    "/u/miep/.myProg-dir" (Unix)
	 *    "/Users/miep/Library/Preferences/MyProg Prefs" (Macintosh)
	 *    "C:\Documents and Settings\Miep\MyProg" (Windows)
	 * and construct a preferences-file name and a script-buttons-file name like
	 *    /u/miep/.myProg-dir/prefs5
	 *    /u/miep/.myProg-dir/buttons5
	 * or
	 *    /Users/miep/Library/Preferences/MyProg Prefs/Prefs5
	 *    /Users/miep/Library/Preferences/MyProg Prefs/Buttons5
	 * or
	 *    C:\Documents and Settings\Miep\MyProg\Preferences5.ini
	 *    C:\Documents and Settings\Miep\MyProg\Buttons5.ini
	 * On Unix, also create names for process-id and message files.
	 */
	{
		structMelderDir prefParentDir = { { 0 } };   /* Directory under which to store our preferences directory. */
		wchar_t name [256];
		Melder_getPrefDir (& prefParentDir);
		/*
		 * Make sure that the program's private directory exists.
		 */
		#if defined (UNIX)
			swprintf (name, 256, L".%ls-dir", Melder_utf8ToWcs (programName));   /* For example .myProg-dir */
		#elif defined (macintosh)
			swprintf (name, 256, L"%ls Prefs", Melder_utf8ToWcs (praatP.title));   /* For example MyProg Prefs */
		#elif defined (_WIN32)
			swprintf (name, 256, L"%ls", Melder_utf8ToWcs (praatP.title));   /* For example MyProg */
		#endif
		#if defined (UNIX) || defined (macintosh)
			Melder_createDirectory (& prefParentDir, name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		#else
			Melder_createDirectory (& prefParentDir, name, 0);
		#endif
		MelderDir_getSubdir (& prefParentDir, name, & praatDir);
		#if defined (UNIX)
			MelderDir_getFile (& praatDir, L"prefs5", & prefsFile);
			MelderDir_getFile (& praatDir, L"buttons5", & buttonsFile);
			MelderDir_getFile (& praatDir, L"pid", & pidFile);
			MelderDir_getFile (& praatDir, L"message", & messageFile);
			MelderDir_getFile (& praatDir, L"tracing", & tracingFile);
		#elif defined (_WIN32)
			MelderDir_getFile (& praatDir, L"Preferences5.ini", & prefsFile);
			MelderDir_getFile (& praatDir, L"Buttons5.ini", & buttonsFile);
			MelderDir_getFile (& praatDir, L"Message.txt", & messageFile);
			MelderDir_getFile (& praatDir, L"Tracing.txt", & tracingFile);
		#elif defined (macintosh)
			MelderDir_getFile (& praatDir, L"Prefs5", & prefsFile);
			MelderDir_getFile (& praatDir, L"Buttons5", & buttonsFile);
			MelderDir_getFile (& praatDir, L"Tracing.txt", & tracingFile);
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
				motif_mac_setUserMessageCallbackA (cb_userMessageA);
				motif_mac_setUserMessageCallbackW (cb_userMessageW);
				Gui_setQuitApplicationCallback (cb_quitApplication);
			}
		#else
			if (! Melder_batch) {
				Gui_setQuitApplicationCallback (cb_quitApplication);
			}
		#endif
	#endif

	/*
	 * Make room for commands.
	 */
	trace ("initing actions");
	praat_actions_init ();
	trace ("initing menu commands");
	praat_menuCommands_init ();

	GuiWindow raam = NULL;
	if (Melder_batch) {
		#if defined (UNIX) || defined (macintosh) || defined (_WIN32) && defined (CONSOLE_APPLICATION)
			MelderString_empty (& theCurrentPraatApplication -> batchName);
			for (unsigned int i = iarg_batchName; i < argc; i ++) {
				int needsQuoting = strchr (argv [i], ' ') != NULL && (i == iarg_batchName || i < argc - 1);
				if (i > 1) MelderString_append (& theCurrentPraatApplication -> batchName, L" ");
				if (needsQuoting) MelderString_append (& theCurrentPraatApplication -> batchName, L"\"");
				MelderString_append (& theCurrentPraatApplication -> batchName, Melder_peekUtf8ToWcs (argv [i]));
				if (needsQuoting) MelderString_append (& theCurrentPraatApplication -> batchName, L"\"");
			}
		#elif defined (_WIN32)
			MelderString_copy (& theCurrentPraatApplication -> batchName, Melder_peekUtf8ToWcs (argv [3]));
		#endif
	} else {
		trace ("starting the application");
		Machine_initLookAndFeel (argc, argv);
		/*
		 * Start the application.
		 */
		#if gtk
			trace ("locale %s", setlocale (LC_ALL, NULL));
			g_set_application_name (title);
			trace ("locale %s", setlocale (LC_ALL, NULL));
		#elif cocoa
			[NSApplication sharedApplication];
		#elif defined (_WIN32)
			argv [0] = & praatP. title [0];   /* argc == 4 */
			Gui_setOpenDocumentCallback (cb_openDocument);
			GuiAppInitialize ("Praatwulg", NULL, 0, & argc, argv, NULL, NULL);
		#elif defined (macintosh)
			GuiAppInitialize ("Praatwulg", NULL, 0, & argc, argv, NULL, NULL);
		#endif

		trace ("creating and installing the Objects window");
		char objectWindowTitle [100];
		sprintf (objectWindowTitle, "%s Objects", praatP.title);
		double x, y;
		trace ("locale %s", setlocale (LC_ALL, NULL));
		Gui_getWindowPositioningBounds (& x, & y, NULL, NULL);
		trace ("locale %s", setlocale (LC_ALL, NULL));
		theCurrentPraatApplication -> topShell = raam = GuiWindow_create (x + 10, y, WINDOW_WIDTH, WINDOW_HEIGHT,
			Melder_peekUtf8ToWcs (objectWindowTitle), gui_cb_quit, NULL, 0);
		trace ("locale %s", setlocale (LC_ALL, NULL));
		#if motif
			GuiApp_setApplicationShell (theCurrentPraatApplication -> topShell -> d_xmShell);
		#endif
		trace ("before objects window shows locale %s", setlocale (LC_ALL, NULL));
		raam -> f_show ();
		trace ("after objects window shows locale %s", setlocale (LC_ALL, NULL));
	}
	Thing_recognizeClassesByName (classCollection, classStrings, classManPages, classSortedSetOfString, NULL);
	if (Melder_batch) {
		Melder_backgrounding = true;
		trace ("adding menus without GUI");
		praat_addMenus (NULL);
		trace ("adding fixed buttons without GUI");
		praat_addFixedButtons (NULL);
	} else {

		#ifdef macintosh
			MelderGui_create (raam);   /* BUG: default Melder_assert would call printf recursively!!! */
		#endif
		#if defined (macintosh) && useCarbon
			trace ("creating the menu bar along the top of the screen (Mac only)");
			raam -> f_addMenuBar ();   // yes, on the Mac we create a menu bar twice: once at the top of the screen, once in the Objects window
		#endif
		trace ("creating the menu bar in the Objects window");
		raam -> f_addMenuBar ();
		praatP.menuBar = raam;
		praat_addMenus (praatP.menuBar);

		trace ("creating the object list in the Objects window");
		GuiLabel_createShown (raam, 3, -250, Machine_getMainWindowMenuBarHeight () + 5, Machine_getMainWindowMenuBarHeight () + 5 + Gui_LABEL_HEIGHT, L"Objects:", 0);
		praatList_objects = GuiList_create (raam, 0, -250, Machine_getMainWindowMenuBarHeight () + 26, -100, true, L" Objects ");
		praatList_objects -> f_setSelectionChangedCallback (gui_cb_list, 0);
		praatList_objects -> f_show ();
		praat_addFixedButtons (raam);

		trace ("creating the dynamic menu in the Objects window");
		praat_actions_createDynamicMenu (raam);
		trace ("showing the Objects window");
		raam -> f_show ();
	//Melder_fatal ("stop");
		#ifdef UNIX
			try {
				autofile f = Melder_fopen (& pidFile, "a");
				fprintf (f, " %ld", (long) GDK_WINDOW_XID (GDK_DRAWABLE (GTK_WIDGET (theCurrentPraatApplication -> topShell -> d_gtkWindow) -> window)));
				f.close (& pidFile);
			} catch (MelderError) {
				Melder_clearError ();
			}
		#endif
		#ifdef UNIX
			Preferences_read (& prefsFile);
		#endif
		#if ! defined (CONSOLE_APPLICATION) && ! defined (macintosh)
			trace ("initializing the Gui late");
			MelderGui_create (theCurrentPraatApplication -> topShell);   // Mac: done this earlier
		#endif
		Melder_setHelpProc (helpProc);
	}
	Melder_setPublishProc (publishProc);
	theCurrentPraatApplication -> manPages = ManPages_create ();

	trace ("creating the Picture window");
	trace ("before picture window shows: locale is %s", setlocale (LC_ALL, NULL));
	if (! praatP.dontUsePictureWindow) praat_picture_init ();
	trace ("after picture window shows: locale is %s", setlocale (LC_ALL, NULL));
}

static void executeStartUpFile (MelderDir startUpDirectory, const wchar_t *fileNameTemplate) {
	wchar_t name [256];
	swprintf (name, 256, fileNameTemplate, Melder_peekUtf8ToWcs (programName));
	if (! MelderDir_isNull (startUpDirectory)) {   // Should not occur on modern systems.
		structMelderFile startUp = { 0 };
		MelderDir_getFile (startUpDirectory, name, & startUp);
		if (! MelderFile_readable (& startUp))
			return; // it's OK if the file doesn't exist
		try {
			praat_executeScriptFromFile (& startUp, NULL);
		} catch (MelderError) {
			Melder_error_ (praatP.title, ": start-up file ", & startUp, " not completed.");
			Melder_flushError (NULL);
		}
	}
}

#if gtk
	#include <gdk/gdkkeysyms.h>
	static gint theKeySnooper (GtkWidget *widget, GdkEventKey *event, gpointer data) {
		trace ("keyval %ld, type %ld", (long) event -> keyval, (long) event -> type);
		if ((event -> keyval == GDK_Tab || event -> keyval == GDK_ISO_Left_Tab) && event -> type == GDK_KEY_PRESS) {
			trace ("tab key pressed in window %p", widget);
			if (event -> state == 0) {
				if (GTK_IS_WINDOW (widget)) {
					GtkWidget *shell = gtk_widget_get_toplevel (GTK_WIDGET (widget));
					trace ("tab pressed in GTK window %p", shell);
					void (*tabCallback) (GuiObject, gpointer) = (void (*) (GuiObject, gpointer)) g_object_get_data (G_OBJECT (widget), "tabCallback");
					if (tabCallback) {
						trace ("a tab callback exists");
						void *tabClosure = g_object_get_data (G_OBJECT (widget), "tabClosure");
						tabCallback (widget, tabClosure);
						return TRUE;
					}
				}
			} else if (event -> state == GDK_SHIFT_MASK) {   // BUG: 
				if (GTK_IS_WINDOW (widget)) {
					GtkWidget *shell = gtk_widget_get_toplevel (GTK_WIDGET (widget));
					trace ("shift-tab pressed in GTK window %p", shell);
					void (*tabCallback) (GuiObject, gpointer) = (void (*) (GuiObject, gpointer)) g_object_get_data (G_OBJECT (widget), "shiftTabCallback");
					if (tabCallback) {
						trace ("a shift tab callback exists");
						void *tabClosure = g_object_get_data (G_OBJECT (widget), "shiftTabClosure");
						tabCallback (widget, tabClosure);
						return TRUE;
					}
				}
			}
		}
		trace ("end");
		return FALSE;   // pass event on
	}
#endif

void praat_run (void) {
	trace ("adding menus, second round");
	praat_addMenus2 ();
	trace ("locale is %s", setlocale (LC_ALL, NULL));

	trace ("adding the Quit command");
	#if defined (macintosh) && useCarbon
		praat_addMenuCommand (L"Objects", L"Praat", L"Quit", 0, praat_HIDDEN, DO_Quit);   // the Quit command is needed for scripts, not for the GUI
	#else
		praat_addMenuCommand (L"Objects", L"Praat", L"-- quit --", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"Praat", L"Quit", 0, praat_UNHIDABLE + 'Q', DO_Quit);
	#endif

	trace ("read the preferences file, and notify those who want to be notified of this");
	/* ...namely, those who already have a window (namely, the Picture window),
	 * and those that regard the start of a new session as a meaningful event
	 * (namely, the session counter and the cross-session memory counter).
	 */
	Preferences_read (& prefsFile);
	if (! praatP.dontUsePictureWindow) praat_picture_prefsChanged ();
	praat_statistics_prefsChanged ();
			//Melder_error3 (L"batch name <<", theCurrentPraatApplication -> batchName.string, L">>");
			//Melder_flushError (NULL);

	praatP.phase = praat_STARTING_UP;

	trace ("execute start-up file(s)");
	/*
	 * On Unix and the Mac, we try no less than three start-up file names.
	 */
	#if defined (UNIX) || defined (macintosh)
		structMelderDir usrLocal = { { 0 } };
		Melder_pathToDir (L"/usr/local", & usrLocal);
		executeStartUpFile (& usrLocal, L"%ls-startUp");
	#endif
	#if defined (UNIX) || defined (macintosh)
		executeStartUpFile (& homeDir, L".%ls-user-startUp");   // not on Windows (empty file name error)
	#endif
	#if defined (UNIX) || defined (macintosh) || defined (_WIN32)
		executeStartUpFile (& homeDir, L"%ls-user-startUp");
	#endif

	trace ("install plug-ins");
	trace ("locale is %s", setlocale (LC_ALL, NULL));
	/* The Praat phase should remain praat_STARTING_UP,
	 * because any added commands must not be included in the buttons file.
	 */
	structMelderFile searchPattern = { 0 };
	MelderDir_getFile (& praatDir, L"plugin_*", & searchPattern);
	try {
		autoStrings directoryNames = Strings_createAsDirectoryList (Melder_fileToPath (& searchPattern));
		if (directoryNames -> numberOfStrings > 0) {
			for (long i = 1; i <= directoryNames -> numberOfStrings; i ++) {
				structMelderDir pluginDir = { { 0 } };
				structMelderFile plugin = { 0 };
				MelderDir_getSubdir (& praatDir, directoryNames -> strings [i], & pluginDir);
				MelderDir_getFile (& pluginDir, L"setup.praat", & plugin);
				if (MelderFile_readable (& plugin)) {
					try {
						praat_executeScriptFromFile (& plugin, NULL);
					} catch (MelderError) {
						Melder_error_ (praatP.title, ": plugin ", & plugin, " contains an error.");
						Melder_flushError (NULL);
					}
				}
			}
		}
	} catch (MelderError) {
		Melder_clearError ();   // in case Strings_createAsDirectoryList () threw an error
	}

	Melder_assert (wcsequ (Melder_double (1.5), L"1.5"));   // check locale settings; because of the required file portability Praat cannot stand "1,5"
	{ int dummy = 200; Melder_assert ((int) (signed char) dummy == -56); }   // bingeti1 relies on this
	{ int dummy = 200; Melder_assert ((int) (unsigned char) dummy == 200); }
	{ uint16_t dummy = 40000; Melder_assert ((int) (int16_t) dummy == -25536); }   // bingeti2 relies on this
	{ uint16_t dummy = 40000; Melder_assert ((short) (int16_t) dummy == -25536); }   // bingete2 relies on this

	if (Melder_batch) {
		if (thePraatStandAloneScriptText != NULL) {
			try {
				praat_executeScriptFromText (thePraatStandAloneScriptText);
				praat_exit (0);
			} catch (MelderError) {
				Melder_flushError ("%s: stand-alone script session interrupted.", praatP.title);
				praat_exit (-1);
			}
		} else if (doingCommandLineInterface) {
			try {
				praat_executeCommandFromStandardInput (praatP.title);
				praat_exit (0);
			} catch (MelderError) {
				Melder_flushError ("%s: command line session interrupted.", praatP.title);
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
				structMelderFile batchFile = { 0 };
				try {
					Melder_relativePathToFile (theCurrentPraatApplication -> batchName.string, & batchFile);
				} catch (MelderError) {
					praat_exit (-1);
				}
				#if defined (_WIN32) && ! defined (CONSOLE_APPLICATION)
					MelderGui_create (NULL);
				#endif
				Melder_error_ (praatP.title, ": command file ", & batchFile, " not completed.");
				Melder_flushError (NULL);
				praat_exit (-1);
			}
		}
	} else /* GUI */ {
		trace ("reading the added script buttons");
		/* Each line separately: every error should be ignored.
		 */
		praatP.phase = praat_READING_BUTTONS;
		{// scope
			autostring buttons;
			try {
				buttons.reset (MelderFile_readText (& buttonsFile));
			} catch (MelderError) {
				Melder_clearError ();
			}
			if (buttons.peek()) {
				wchar_t *line = buttons.peek();
				for (;;) {
					wchar_t *newline = wcschr (line, '\n');
					if (newline) *newline = '\0';
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

		trace ("sorting the commands");
		trace ("locale is %s", setlocale (LC_ALL, NULL));
		praat_sortMenuCommands ();
		praat_sortActions ();

		praatP.phase = praat_HANDLING_EVENTS;

		#if gtk
			//gtk_widget_add_events (G_OBJECT (theCurrentPraatApplication -> topShell), GDK_ALL_EVENTS_MASK);
			trace ("install GTK key snooper");
			trace ("locale is %s", setlocale (LC_ALL, NULL));
			g_signal_connect (G_OBJECT (theCurrentPraatApplication -> topShell -> d_gtkWindow), "client-event", G_CALLBACK (cb_userMessage), NULL);
			signal (SIGUSR1, cb_sigusr1);
			gtk_key_snooper_install (theKeySnooper, 0);
			trace ("start the GTK event loop");
			trace ("locale is %s", setlocale (LC_ALL, NULL));
			gtk_main ();
		#elif cocoa
			[NSApp run];
		#elif motif
			#if defined (_WIN32)
				if (theCurrentPraatApplication -> batchName.string [0] != '\0') {
					wchar_t text [500];
					/*
					 * The user dropped a file on the Praat icon, while Praat was not running yet.
					 * Windows may have enclosed the path between quotes;
					 * this is especially likely to happen if the path contains spaces (which is usual).
					 * And sometimes, Windows prepends a space before the quote.
					 * Peel all that off.
					 *
					 * BUG: this only works now with single files; it should work with multiple files as well.
					 */
					wchar_t *s = theCurrentPraatApplication -> batchName.string;
					swprintf (text, 500, L"Read from file... %ls", s [0] == ' ' && s [1] == '\"' ? s + 2 : s [0] == '\"' ? s + 1 : s);
					long l = wcslen (text);
					if (l > 0 && text [l - 1] == '\"') text [l - 1] = '\0';
					//Melder_error3 (L"command <<", text, L">>");
					//Melder_flushError (NULL);
					try {
						praat_executeScriptFromText (text);
					} catch (MelderError) {
						Melder_flushError (NULL);
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
