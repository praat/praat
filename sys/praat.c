/* praat.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2003/03/12 preferences in home directory on shared Windows machines
 * pb 2003/06/19 NUMmachar
 * pb 2003/07/10 GSL initialization
 * pb 2003/10/03 praat-executeFromFile without arguments
 * pb 2004/06/17 made Objects label visible on Unix
 * pb 2004/12/29 removed .praat-user-startUp for Windows (empty file name error)
 * pb 2005/06/28 TextEditor_prefs
 * pb 2005/08/22 renamed Control menu to "Praat"
 * pb 2005/11/18 URL support
 * pb 2006/02/23 corrected callbacks in praat_installEditorN
 * pb 2006/08/07 removed quotes from around file paths in openDocument message
 * pb 2006/09/30 praat_selection () can take NULL as an argument
 * pb 2006/10/28 removed MacOS 9 stuff
 * pb 2006/12/26 theCurrentPraat
 * pb 2007/01/25 width of object list is 50 procent
 * pb 2007/06/10 wchar_t
 * pb 2007/06/16 text encoding prefs
 * pb 2007/08/31 praat_new1-9
 * pb 2007/09/02 include Editor prefs
 * sdk 2008/01/14 GTK
 * pb 2008/02/01 made sure that praat_dataChanged can be called at error time
 * pb 2008/03/13 Windows: better file dropping
 * pb 2008/04/09 removed explicit GSL
 * pb 2008/11/01 praatcon -a
 * pb 2009/01/17 arguments to UiForm callbacks
 * pb 2009/03/17 split up theCurrentPraat into Application, Objects and Picture
 * pb 2009/12/22 invokingButtonTitle
 * pb 2010/05/24 sendpraat for GTK
 */

#include "melder.h"
#include "NUMmachar.h"
#include <ctype.h>
#include <stdarg.h>
#if defined (UNIX) || defined (macintosh)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <signal.h>
	#include <locale.h>
#endif
#if defined (UNIX) || defined __MWERKS__
	#include <unistd.h>
#endif

#include "praatP.h"
#include "praat_script.h"
#include "site.h"
#include "machine.h"
#include "Printer.h"
#include "ScriptEditor.h"
#include "Strings.h"

#if gtk
	#include <gdk/gdkx.h>
#endif

#define EDITOR  theCurrentPraatObjects -> list [IOBJECT]. editors

#define WINDOW_WIDTH 520
#define WINDOW_HEIGHT 630

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
 *    Windows 2000/XP/Vista:   \\myserver\myshare\Miep\MyProg
 *                       or:   C:\Documents and settings\Miep\MyProg
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs
 */
extern structMelderDir praatDir;
structMelderDir praatDir = { { 0 } };
/*
 * prefs5File: preferences file.
 *    Unix:   /u/miep/.myProg-dir/prefs5
 *    Windows 2000/XP/Vista:   \\myserver\myshare\Miep\MyProg\Preferences5.ini
 *                       or:   C:\Documents and settings\Miep\MyProg\Preferences5.ini
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Prefs5
 */
static structMelderFile prefs4File = { 0 }, prefs5File = { 0 };
/*
 * buttons5File: buttons file.
 *    Unix:   /u/miep/.myProg-dir/buttons
 *    Windows 2000/XP/Vista:   \\myserver\myshare\Miep\MyProg\Buttons5.ini
 *                       or:   C:\Documents and settings\Miep\MyProg\Buttons5.ini
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Buttons5
 */
static structMelderFile buttons4File = { 0 }, buttons5File = { 0 };
#if defined (UNIX)
	static structMelderFile pidFile = { 0 };   /* Like /u/miep/.myProg-dir/pid */
	static structMelderFile messageFile = { 0 };   /* Like /u/miep/.myProg-dir/message */
#elif defined (_WIN32)
	static structMelderFile messageFile = { 0 };   /* Like C:\Windows\myProg\Message.txt */
#endif

static Widget praatList_objects;

/***** selection *****/

long praat_getIdOfSelected (void *voidklas, int inplace) {
	Data_Table klas = (Data_Table) voidklas;
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
	return inplace ?
		Melder_error5 (L"No ", klas ? klas -> _className : L"object", L" #", Melder_integer (inplace), L" selected.") :
		Melder_error3 (L"No ", klas ? klas -> _className : L"object", L" selected.");
}

wchar_t * praat_getNameOfSelected (void *voidklas, int inplace) {
	Data_Table klas = (Data_Table) voidklas;
	int place = inplace, IOBJECT;
	if (place == 0) place = 1;
	if (place > 0) {
		WHERE (SELECTED && (klas == NULL || CLASS == klas)) {
			if (place == 1) return klas == NULL ? FULL_NAME : NAMEW;
			place --;
		}
	} else {
		WHERE_DOWN (SELECTED && (klas == NULL || CLASS == klas)) {
			if (place == -1) return klas == NULL ? FULL_NAME : NAMEW;
			place ++;
		}
	}
	if (inplace) {
		Melder_error5 (L"No ", klas ? klas -> _className : L"object", L" #", Melder_integer (inplace), L" selected.");
	} else {
		Melder_error3 (L"No ", klas ? klas -> _className : L"object", L" selected.");
	}
	return 0;   // Failure.
}

int praat_selection (void *klas) {
	if (klas == NULL) return theCurrentPraatObjects -> totalSelection;
	long readableClassId = ((Thing_Table) klas) -> sequentialUniqueIdOfReadableClass;
	if (readableClassId == 0) Melder_fatal ("No sequential unique ID for class %ls.", ((Thing_Table) klas) -> _className);
	return theCurrentPraatObjects -> numberOfSelected [readableClassId];
}

void praat_deselect (int IOBJECT) {
	if (! SELECTED) return;
	SELECTED = FALSE;
	theCurrentPraatObjects -> totalSelection -= 1;
	long readableClassId = ((Thing) theCurrentPraatObjects -> list [IOBJECT]. object) -> methods -> sequentialUniqueIdOfReadableClass;
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
	Thing object = theCurrentPraatObjects -> list [IOBJECT]. object;
	Melder_assert (object != NULL);
	long readableClassId = object -> methods -> sequentialUniqueIdOfReadableClass;
	if (readableClassId == 0) Melder_fatal ("No sequential unique ID for class %ls.", object -> methods -> _className);
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

Any praat_onlyObject (void *klas) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED && CLASS == klas) { result = IOBJECT; found += 1; }
	if (found != 1) return NULL;
	return theCurrentPraatObjects -> list [result]. object;
}

Any praat_onlyObject_generic (void *klas) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED && Thing_subclass (CLASS, klas)) { result = IOBJECT; found += 1; }
	if (found != 1) return NULL;
	return theCurrentPraatObjects -> list [result]. object;
}

praat_Object praat_onlyScreenObject (void) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED) { result = IOBJECT; found += 1; }
	if (found != 1) Melder_fatal ("praat_onlyScreenObject: found %d objects instead of 1.", found);
	return & theCurrentPraatObjects -> list [result];
}

wchar_t *praat_name (int IOBJECT) { return wcschr (FULL_NAME, ' ') + 1; }

void praat_write_do (Any dia, const wchar_t *extension) {
	int IOBJECT, found = 0;
	Data data = NULL;
	static MelderString defaultFileName = { 0 };
	MelderString_empty (& defaultFileName);
	WHERE (SELECTED) { if (! data) data = OBJECT; found += 1; }
	if (found == 1) {
		MelderString_append (& defaultFileName, data -> name);
		if (defaultFileName.length > 50) { defaultFileName.string [50] = '\0'; defaultFileName.length = 50; }
		MelderString_append2 (& defaultFileName, L".", extension ? extension : Thing_className (data));
	} else if (extension == NULL) {
		MelderString_append (& defaultFileName, L"praat.Collection");
	} else {
		MelderString_append2 (& defaultFileName, L"praat.", extension);
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
		Any editor = theCurrentPraatObjects -> list [iobject]. editors [ieditor];   /* Save this one reference. */
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

bool praat_new1 (I, const wchar_t *myName) {
	iam (Data);
	int IOBJECT, ieditor;   /* Must be local: praat_new can be called from within a loop!!! */
	if (me == NULL) return Melder_error1 (L"No object was put into the list.");
	/*
	 * If my class is Collection, I'll have to be unpacked.
	 */
	if (my methods == (Any) classCollection) {
		Collection list = (Collection) me;
		bool result = true;
		for (long idata = 1; idata <= list -> size; idata ++) {
			Data object = list -> item [idata];
			Melder_assert (object -> name != NULL);
			result &= praat_new1 (object, object -> name) ? true : false;   // Recurse.
		}
		list -> size = 0;   // Disown.
		forget (list);
		return result;
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
	MelderString_append3 (& name, Thing_className (me), L" ", givenName.string);

	if (theCurrentPraatObjects -> n == praat_MAXNUM_OBJECTS) {
		forget (me);
		return Melder_error3 (L"The Object Window cannot contain more than ", Melder_integer (praat_MAXNUM_OBJECTS), L" objects. "
			"You could remove some objects.");
	}
		
	IOBJECT = ++ theCurrentPraatObjects -> n;
	Melder_assert (FULL_NAME == NULL);
	FULL_NAME = Melder_wcsdup (name.string);
	Melder_assert (FULL_NAME != NULL);
	++ theCurrentPraatObjects -> uniqueId;

	if (! theCurrentPraatApplication -> batch) {   /* Put a new object on the screen, at the bottom of the list. */
		#ifdef UNIX
			#if motif
				XtVaSetValues (praatList_objects, XmNvisibleItemCount, theCurrentPraatObjects -> n + 2, NULL);
			#endif
		#endif
		MelderString listName = { 0 };
		MelderString_append3 (& listName, Melder_integer (theCurrentPraatObjects -> uniqueId), L". ", name.string);
		GuiList_insertItem (praatList_objects, listName.string, theCurrentPraatObjects -> n);
		MelderString_free (& listName);
	}
	OBJECT = me;
	SELECTED = FALSE;
	CLASS = my methods;
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		EDITOR [ieditor] = NULL;
	MelderFile_setToNull (& theCurrentPraatObjects -> list [IOBJECT]. file);
	ID = theCurrentPraatObjects -> uniqueId;
	theCurrentPraatObjects -> list [IOBJECT]. _beingCreated = TRUE;
	Thing_setName (OBJECT, givenName.string);
	theCurrentPraatObjects -> totalBeingCreated ++;
	MelderString_free (& givenName);
	MelderString_free (& name);
	return true;
}

static MelderString thePraatNewName = { 0 };
bool praat_new2 (I, const wchar_t *s1, const wchar_t *s2) {
	iam (Data);
	MelderString_empty (& thePraatNewName);
	MelderString_append2 (& thePraatNewName, s1, s2);
	return praat_new1 (me, thePraatNewName.string);
}
bool praat_new3 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3) {
	iam (Data);
	MelderString_empty (& thePraatNewName);
	MelderString_append3 (& thePraatNewName, s1, s2, s3);
	return praat_new1 (me, thePraatNewName.string);
}
bool praat_new4 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4) {
	iam (Data);
	MelderString_empty (& thePraatNewName);
	MelderString_append4 (& thePraatNewName, s1, s2, s3, s4);
	return praat_new1 (me, thePraatNewName.string);
}
bool praat_new5 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5) {
	iam (Data);
	MelderString_empty (& thePraatNewName);
	MelderString_append5 (& thePraatNewName, s1, s2, s3, s4, s5);
	return praat_new1 (me, thePraatNewName.string);
}
bool praat_new6 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6) {
	iam (Data);
	MelderString_empty (& thePraatNewName);
	MelderString_append6 (& thePraatNewName, s1, s2, s3, s4, s5, s6);
	return praat_new1 (me, thePraatNewName.string);
}
bool praat_new7 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7) {
	iam (Data);
	MelderString_empty (& thePraatNewName);
	MelderString_append7 (& thePraatNewName, s1, s2, s3, s4, s5, s6, s7);
	return praat_new1 (me, thePraatNewName.string);
}
bool praat_new8 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8) {
	iam (Data);
	MelderString_empty (& thePraatNewName);
	MelderString_append8 (& thePraatNewName, s1, s2, s3, s4, s5, s6, s7, s8);
	return praat_new1 (me, thePraatNewName.string);
}
bool praat_new9 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9) {
	iam (Data);
	MelderString_empty (& thePraatNewName);
	MelderString_append9 (& thePraatNewName, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	return praat_new1 (me, thePraatNewName.string);
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
		long readableClassId = ((Thing) theCurrentPraatObjects -> list [IOBJECT]. object) -> methods -> sequentialUniqueIdOfReadableClass;
		theCurrentPraatObjects -> numberOfSelected [readableClassId] --;
		Melder_assert (theCurrentPraatObjects -> numberOfSelected [readableClassId] >= 0);
	}
	theCurrentPraatObjects -> totalSelection = 0;
	long numberOfSelected, *selected = GuiList_getSelectedPositions (praatList_objects, & numberOfSelected);
	if (selected != NULL) {
		for (long iselected = 1; iselected <= numberOfSelected; iselected ++) {
			IOBJECT = selected [iselected];
			SELECTED = TRUE;
			long readableClassId = ((Thing) theCurrentPraatObjects -> list [IOBJECT]. object) -> methods -> sequentialUniqueIdOfReadableClass;
			theCurrentPraatObjects -> numberOfSelected [readableClassId] ++;
			Melder_assert (theCurrentPraatObjects -> numberOfSelected [readableClassId] > 0);
			UiHistory_write (first ? L"\nselect " : L"\nplus ");
			UiHistory_write (FULL_NAME);
			first = FALSE;
			theCurrentPraatObjects -> totalSelection += 1;
		}
		NUMlvector_free (selected, 1);
	}
	praat_show ();
}

void praat_list_renameAndSelect (int position, const wchar_t *name) {
	if (! theCurrentPraatApplication -> batch) {
		GuiList_replaceItem (praatList_objects, name, position);   /* Void if name equal. */
		if (! Melder_backgrounding)
			GuiList_selectItem (praatList_objects, position);
	}
}

/***** objects *****/

void praat_name2 (wchar_t *name, void *klas1, void *klas2) {
	int i1 = 1, i2;
	wchar_t *name1, *name2;
	while (theCurrentPraatObjects -> list [i1]. selected == 0 || theCurrentPraatObjects -> list [i1]. klas != klas1) i1 ++;
	i2 = 1;   /* This late initialization works around a Think C BUG. */
	while (theCurrentPraatObjects -> list [i2]. selected == 0 || theCurrentPraatObjects -> list [i2]. klas != klas2) i2 ++;
	name1 = wcschr (theCurrentPraatObjects -> list [i1]. name, ' ') + 1;
	name2 = wcschr (theCurrentPraatObjects -> list [i2]. name, ' ') + 1;
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
		GuiList_deleteItem (praatList_objects, i);
		#ifdef UNIX
			//XtVaSetValues (praatList_objects, XmNvisibleItemCount, theCurrentPraatObjects -> n + 1, NULL);
		#endif
	}
}

static void praat_exit (int exit_code) {
	int IOBJECT;
	#ifdef _WIN32
		if (! theCurrentPraatApplication -> batch)
			XtDestroyWidget (theCurrentPraatApplication -> topShell);
	#endif
	praat_picture_exit ();
	praat_statistics_exit ();   /* Record total memory use across sessions. */

	/*
	 * Stop receiving messages.
	 */
	#if defined (UNIX)
		if (pidFile. path [0]) {
			FILE *f = Melder_fopen (& pidFile, "r");
			if (f) {
				long pid;
				if (fscanf (f, "%ld", & pid) < 1 || pid == getpid ()) {
					fclose (f);
					MelderFile_delete (& pidFile);
				} else {
					fclose (f);   /* Probably second invocation of program. */
				}
			} else {
				Melder_clearError ();
			}
		}
	#endif

	/*
	 * Save the preferences.
	 */
	Preferences_write (& prefs5File);
	MelderFile_setMacTypeAndCreator (& prefs5File, 'pref', 'PpgB');

	/*
	 * Save the script buttons.
	 */
	if (! theCurrentPraatApplication -> batch) {
		FILE *f = Melder_fopen (& buttons5File, "wb");
		if (f) {
			MelderFile_setMacTypeAndCreator (& buttons5File, 'pref', 'PpgB');
			fwprintf (f, L"\ufeff# Buttons (1).\n");
			fwprintf (f, L"# This file is generated automatically when you quit the %ls program.\n", Melder_peekUtf8ToWcs (praatP.title));
			fwprintf (f, L"# It contains the buttons that you added interactively to the fixed or dynamic menus,\n");
			fwprintf (f, L"# and the buttons that you hid or showed.\n\n");
			praat_saveMenuCommands (f);
			praat_saveAddedActions (f);
			fclose (f);
		} else {
			Melder_clearError ();
		}
	}

	/*
	 * Flush the file-based objects.
	 */
	WHERE_DOWN (! MelderFile_isNull (& theCurrentPraatObjects -> list [IOBJECT]. file)) praat_remove (IOBJECT);
	Melder_files_cleanUp ();   /* If a URL is open. */

	/*
	 * Finally, leave the program.
	 */
	exit (exit_code);
}

static void cb_Editor_destroy (I, void *closure) {
	iam (Editor);
	(void) closure;
	removeAllReferencesToEditor (me);   /* Remove reference(s) to moribund Editor. */
}

static void cb_Editor_dataChanged (I, void *closure, Any data) {
	iam (Editor);
	int iobject, ieditor;
	(void) closure;
	for (iobject = 1; iobject <= theCurrentPraatObjects -> n; iobject ++) {
		int editingThisObject = FALSE;
		/*
		 * Am I editing this object?
		 */
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (theCurrentPraatObjects -> list [iobject]. editors [ieditor] == me)
				editingThisObject = TRUE;
		if (editingThisObject) {
			/*
			 * Change the data if needed (unusual but possible).
			 * BUG: DO NOT, because changed object may be second data in editor.
			 */
			/*if (data && ((Data) data) -> methods == ((Data) theCurrentPraatObjects -> list [iobject]. object) -> methods)
				theCurrentPraatObjects -> list [iobject]. object = data;*/
			/*
			 * Notify all other editors associated with this object.
			 */
			for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor otherEditor = theCurrentPraatObjects -> list [iobject]. editors [ieditor];
				if (otherEditor != NULL && otherEditor != me) {
					Editor_dataChanged (otherEditor, data);
				}
			}
		}
	}
}

static void cb_Editor_publish (Any editor, void *closure, Any publish) {
/*
   The default publish callback.
   Works nicely if the publisher invents a name.
*/
	(void) editor;
	(void) closure;
	if (! praat_new1 (publish, NULL)) { Melder_flushError (NULL); return; }
	praat_updateSelection ();
}

int praat_installEditor (Any editor, int IOBJECT) {
	if (editor == NULL) return 0;
	for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		if (EDITOR [ieditor] == NULL) {
			EDITOR [ieditor] = editor;
			Editor_setDestroyCallback (editor, cb_Editor_destroy, NULL);
			Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
			Editor_setPublishCallback (editor, cb_Editor_publish, NULL);
			return 1;
		}
	}
	forget (editor);
	return Melder_error3 (L"(praat_installEditor:) Cannot have more than ", Melder_integer (praat_MAXNUM_EDITORS), L" editors with one object.");
}

int praat_installEditor2 (Any editor, int i1, int i2) {
	int ieditor1 = 0, ieditor2 = 0;
	if (editor == NULL) return 0;
	for (ieditor1 = 0; ieditor1 < praat_MAXNUM_EDITORS; ieditor1 ++)
		if (theCurrentPraatObjects -> list [i1]. editors [ieditor1] == NULL)
			break;
	for (ieditor2 = 0; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (theCurrentPraatObjects -> list [i2]. editors [ieditor2] == NULL)
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS) {
		theCurrentPraatObjects -> list [i1]. editors [ieditor1] = theCurrentPraatObjects -> list [i2]. editors [ieditor2] = editor;
		Editor_setDestroyCallback (editor, cb_Editor_destroy, NULL);
		Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
		Editor_setPublishCallback (editor, cb_Editor_publish, NULL);
	} else {
		forget (editor);
		return Melder_error3 (L"(praat_installEditor2:) Cannot have more than ", Melder_integer (praat_MAXNUM_EDITORS), L" editors with one object.");
	}
	return 1;
}

int praat_installEditor3 (Any editor, int i1, int i2, int i3) {
	int ieditor1 = 0, ieditor2 = 0, ieditor3;
	if (! editor) return 0;
	for (ieditor1 = 0; ieditor1 < praat_MAXNUM_EDITORS; ieditor1 ++)
		if (theCurrentPraatObjects -> list [i1]. editors [ieditor1] == NULL)
			break;
	for (ieditor2 = 0; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (theCurrentPraatObjects -> list [i2]. editors [ieditor2] == NULL)
			break;
	for (ieditor3 = 0; ieditor3 < praat_MAXNUM_EDITORS; ieditor3 ++)
		if (theCurrentPraatObjects -> list [i3]. editors [ieditor3] == NULL)
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS && ieditor3 < praat_MAXNUM_EDITORS) {
		theCurrentPraatObjects -> list [i1]. editors [ieditor1] = theCurrentPraatObjects -> list [i2]. editors [ieditor2] = theCurrentPraatObjects -> list [i3]. editors [ieditor3] = editor;
		Editor_setDestroyCallback (editor, cb_Editor_destroy, NULL);
		Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
		Editor_setPublishCallback (editor, cb_Editor_publish, NULL);
	} else {
		forget (editor);
		return Melder_error3 (L"(praat_installEditor3:) Cannot have more than ", Melder_integer (praat_MAXNUM_EDITORS), L" editors with one object.");
	}
	return 1;
}

int praat_installEditorN (Any editor, Ordered objects) {
	long iOrderedObject, iPraatObject;
	if (editor == NULL) return 0;
	/*
	 * First check whether all objects in the Ordered are also in the List of Objects (Praat crashes if not),
	 * and check whether there is room to add an editor for each.
	 */
	for (iOrderedObject = 1; iOrderedObject <= objects -> size; iOrderedObject ++) {
		Data object = objects -> item [iOrderedObject];
		for (iPraatObject = 1; iPraatObject <= theCurrentPraatObjects -> n; iPraatObject ++) {
			if (object == theCurrentPraatObjects -> list [iPraatObject]. object) {
				int ieditor;
				for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor] == NULL) {
						break;
					}
				}
				if (ieditor >= praat_MAXNUM_EDITORS) {
					forget (editor);
					return Melder_error3 (L"Cannot view the same object in more than ", Melder_integer (praat_MAXNUM_EDITORS), L" windows.");
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
		Data object = objects -> item [iOrderedObject];
		for (iPraatObject = 1; iPraatObject <= theCurrentPraatObjects -> n; iPraatObject ++) {
			if (object == theCurrentPraatObjects -> list [iPraatObject]. object) {
				int ieditor;
				for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor] == NULL) {
						theCurrentPraatObjects -> list [iPraatObject]. editors [ieditor] = editor;
						Editor_setDestroyCallback (editor, cb_Editor_destroy, NULL);
						Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
						Editor_setPublishCallback (editor, cb_Editor_publish, NULL);
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
	 * This function can be called at error time.
	 */
	wchar_t *saveError = NULL;
	bool duringError = Melder_hasError ();
	if (duringError) {
		saveError = Melder_wcsdup (Melder_getError ());
		Melder_clearError ();
	}
	int IOBJECT, ieditor;
	WHERE (OBJECT == object) {
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (EDITOR [ieditor]) Editor_dataChanged (EDITOR [ieditor], object);
	}
	if (duringError) {
		Melder_error1 (saveError);
		Melder_free (saveError);
	}
}

void praat_clipboardChanged (void *closure, Any clipboard) {
	(void) closure;
	for (int iobject = 1; iobject <= theCurrentPraatObjects -> n; iobject ++)
		for (int ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (theCurrentPraatObjects -> list [iobject]. editors [ieditor])
				Editor_clipboardChanged (theCurrentPraatObjects -> list [iobject]. editors [ieditor], clipboard);
}

static void helpProc (const wchar_t *query) {
	if (theCurrentPraatApplication -> batch) { Melder_flushError ("Cannot view manual from batch."); return; }
	if (! Manual_create (theCurrentPraatApplication -> topShell, query, theCurrentPraatApplication -> manPages))
		Melder_flushError ("help: no help on \"%ls\".", query);   /* Failure. */
}

static int publishProc (void *anything) {
	if (! praat_new1 (anything, NULL)) return Melder_error1 (L"(Melder_publish:) not published.");
	praat_updateSelection ();
	return 1;
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

static void gui_cb_quit (GUI_ARGS) {
	(void) w; (void) void_me; (void) call;
	DO_Quit (NULL, NULL, NULL, NULL, NULL, NULL);
}

#if gtk
static void gui_cb_quit_gtk (void *p) {
  DO_Quit (NULL, NULL, NULL, NULL, NULL, NULL);
}
#endif

void praat_dontUsePictureWindow (void) { praatP.dontUsePictureWindow = TRUE; }

/********** INITIALIZATION OF THE PRAAT SHELL **********/

#if defined (UNIX)
	#if gtk
		static gboolean cb_userMessage (GtkWidget widget, GdkEventClient *event, gpointer user_data) {
			FILE *f;
			(void) widget;
			(void) user_data;
			if ((f = Melder_fopen (& messageFile, "r")) != NULL) {
				long pid;
				int narg = fscanf (f, "#%ld", & pid);
				fclose (f);
				praat_background ();
				if (! praat_executeScriptFromFile (& messageFile, NULL)) {
					Melder_error2 (Melder_peekUtf8ToWcs (praatP.title), L": message not completely handled.");
					Melder_flushError (NULL);
				}
				praat_foreground ();
				if (narg) kill (pid, SIGUSR2);
			} else {
				Melder_clearError ();
			}
			return TRUE;
		}
	#elif motif
		int haveMessage = FALSE;
		static void timerProc_userMessage (XtPointer dummy, XtIntervalId *id) {
			FILE *f;
			(void) dummy;
			(void) id;
			if ((f = Melder_fopen (& messageFile, "r")) != NULL) {
				long pid;
				int narg = fscanf (f, "#%ld", & pid);
				fclose (f);
				praat_background ();
				if (! praat_executeScriptFromFile (& messageFile, NULL)) {
					Melder_error2 (Melder_peekUtf8ToWcs (praatP.title), L": message not completely handled.");
					Melder_flushError (NULL);
				}
				praat_foreground ();
				if (narg) kill (pid, SIGUSR2);
			} else {
				Melder_clearError ();
			}
		}
		static void handleMessage (int message) {
			(void) message;
			signal (SIGUSR1, handleMessage);   /* Keep this handler in the air. */
			haveMessage = TRUE;
			/* Trial: */
			haveMessage = FALSE;
			XtAppAddTimeOut (theCurrentPraatApplication -> context, 100, timerProc_userMessage, 0);
		}
	#endif
#elif defined (_WIN32)
	static int cb_userMessage (void) {
		praat_background ();
		if (! praat_executeScriptFromFile (& messageFile, NULL)) {
			Melder_error2 (Melder_peekUtf8ToWcs (praatP.title), L": message not completely handled.");
			Melder_flushError (NULL);
		}
		praat_foreground ();
		return 0;
	}
	extern char *sendpraat (void *display, const char *programName, long timeOut, const char *text);
	extern wchar_t *sendpraatW (void *display, const wchar_t *programName, long timeOut, const wchar_t *text);
	static int cb_openDocument (MelderFile file) {
		wchar_t text [500];
		wchar_t *s = file -> path;
		swprintf (text, 500, L"Read from file... %ls", s [0] == ' ' && s [1] == '\"' ? s + 2 : s [0] == '\"' ? s + 1 : s);
		long l = wcslen (text);
		if (l > 0 && text [l - 1] == '\"') text [l - 1] = '\0';
		sendpraatW (NULL, Melder_peekUtf8ToWcs (praatP.title), 0, text);
		return 0;
	}
#elif defined (macintosh)
	static int cb_userMessageA (char *messageA) {
		praat_background ();
		wchar_t *message = Melder_8bitToWcs (messageA, 0);
		if (! praat_executeScriptFromText (message)) error2 (Melder_peekUtf8ToWcs (praatP.title), L": message not completely handled.")
	end:
		Melder_free (message);
		praat_foreground ();
		iferror Melder_flushError (NULL);
		return 0;
	}
	static int cb_userMessageW (wchar_t *message) {
		praat_background ();
		if (! praat_executeScriptFromText (message)) error2 (Melder_peekUtf8ToWcs (praatP.title), L": message not completely handled.")
	end:
		praat_foreground ();
		iferror Melder_flushError (NULL);
		return 0;
	}
	static int cb_quitApplication (void) {
		DO_Quit (NULL, NULL, NULL, NULL, NULL, NULL);
		return 0;
	}
#endif

#ifdef UNIX
#if motif
String theXtLanguageProc (Display *display, String xnl, XtPointer client_data) {
	(void) display;
	(void) xnl;
	(void) client_data;
	setlocale (LC_ALL, "en_US.UTF-8");
	if (! XSupportsLocale ()) Melder_casual ("Locale en_US.UTF-8 not supported.");
	XSetLocaleModifiers ("");
	return setlocale (LC_ALL, NULL);
}
#endif
#endif
	
void praat_init (const char *title, unsigned int argc, char **argv) {
	static char truncatedTitle [300];   /* Static because praatP.title will point into it. */
	#if defined (UNIX)
		FILE *f;
		//setlocale (LC_ALL, "en_US");
		#if gtk
			gtk_init (& argc, & argv);
			gtk_set_locale ();
			setlocale (LC_ALL, "en_US.utf8");
		#elif motif
			XtSetLanguageProc (NULL, theXtLanguageProc, NULL);
		#endif
	#elif defined (macintosh)
		setlocale (LC_ALL, "en_US");   // required to make swprintf work correctly; the default "C" locale does not do that!
	#endif
	char *p;
	#ifdef macintosh
		Gestalt ('sysv', (long *) & Melder_systemVersion);
	#endif
	/*
		Initialize numerical libraries.
	*/
	NUMmachar ();
	NUMinit ();
	/*
		Remember the current directory. Only useful for scripts run from batch.
	*/
	Melder_rememberShellDirectory ();

	/*
	 * Install the preferences of the Praat shell, and set the defaults.
	 */
	praat_statistics_prefs ();   // Number of sessions, memory used...
	praat_picture_prefs ();   // Font...
	Editor_prefs ();   // Erase picture first...
	HyperPage_prefs ();   // Font...
	Site_prefs ();   // Print command...
	Melder_audio_prefs ();   // Use speaker (Sun & HP), output gain (HP)...
	Melder_textEncoding_prefs ();
	Printer_prefs ();   // Paper size, printer command...
	TextEditor_prefs ();   // Font size...

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

		Melder_batch = theCurrentPraatApplication -> batchName.string [0] != '\0';

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
			Melder_batch = TRUE;
			doingCommandLineInterface = TRUE;   /* Read from stdin. */
		}

		/* Take from 'title' ("myProg 3.2" becomes "myProg") or from command line ("/ifa/praat" becomes "praat"). */
		strcpy (truncatedTitle, argc && argv [0] [0] ? argv [0] : title && title [0] ? title : "praat");
	#else
		#if defined (_WIN32)
			MelderString_copy (& theCurrentPraatApplication -> batchName,
				argv [3] ? Melder_peekUtf8ToWcs (argv [3]) : L"");   /* The command line. */
		#endif
		Melder_batch = FALSE;   /* PRAAT.EXE on Windows is always interactive. */
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
	 *    "/u/miep/myProg-dir" (Unix)
	 *    "/Users/miep/Library/Preferences/MyProg Prefs" (Macintosh)
	 *    "C:\Windows\MyProg" (Windows 95)
	 *    "C:\Documents and Settings\Miep\MyProg" (Windows XP)
	 * and construct a preferences-file name and a script-buttons-file name like
	 *    /u/miep/.myProg-dir/prefs   (Unix)
	 *    /u/miep/.myProg-dir/script_buttons
	 * or
	 *    /Users/miep/Library/Preferences/MyProg Prefs/Prefs
	 *    /Users/miep/Library/Preferences/MyProg Prefs/Buttons
	 * or
	 *    C:\Windows\MyProg\Preferences.ini
	 *    C:\Windows\MyProg\Buttons.ini
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
			swprintf (name, 256, L".%s-dir", programName);   /* For example .myProg-dir */
		#elif defined (macintosh)
			swprintf (name, 256, L"%s Prefs", praatP.title);   /* For example MyProg Prefs */
		#elif defined (_WIN32)
			swprintf (name, 256, L"%s", praatP.title);   /* For example MyProg */
		#endif
		#if defined (UNIX) || defined (macintosh)
			Melder_createDirectory (& prefParentDir, name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		#else
			Melder_createDirectory (& prefParentDir, name, 0);
		#endif
		MelderDir_getSubdir (& prefParentDir, name, & praatDir);
		#if defined (UNIX)
			MelderDir_getFile (& praatDir, L"prefs", & prefs4File);
			MelderDir_getFile (& praatDir, L"prefs5", & prefs5File);
			MelderDir_getFile (& praatDir, L"buttons", & buttons4File);
			MelderDir_getFile (& praatDir, L"buttons5", & buttons5File);
			MelderDir_getFile (& praatDir, L"pid", & pidFile);
			MelderDir_getFile (& praatDir, L"message", & messageFile);
		#elif defined (_WIN32)
			MelderDir_getFile (& praatDir, L"Preferences.ini", & prefs4File);
			MelderDir_getFile (& praatDir, L"Preferences5.ini", & prefs5File);
			MelderDir_getFile (& praatDir, L"Buttons.ini", & buttons4File);
			MelderDir_getFile (& praatDir, L"Buttons5.ini", & buttons5File);
			MelderDir_getFile (& praatDir, L"Message.txt", & messageFile);
		#elif defined (macintosh)
			MelderDir_getFile (& praatDir, L"Prefs", & prefs4File);   /* We invite trouble if we call it Preferences! */
			MelderDir_getFile (& praatDir, L"Prefs5", & prefs5File);
			MelderDir_getFile (& praatDir, L"Buttons", & buttons4File);
			MelderDir_getFile (& praatDir, L"Buttons5", & buttons5File);
		#endif
	}
	#if defined (UNIX)
		if (! Melder_batch) {
			/*
			 * Make sure that the directory /u/miep/.myProg-dir exists,
			 * and write our process id into the pid file.
			 * Messages from "sendpraat" are caught very early this way,
			 * though they will be responded to much later.
			 */
			if ((f = Melder_fopen (& pidFile, "w")) != NULL) {
				fprintf (f, "%ld", (long) getpid ());
				fclose (f);
				#if motif
					signal (SIGUSR1, handleMessage);
				#endif
			} else {
				Melder_clearError ();
			}
		}
	#elif defined (_WIN32)
		if (! Melder_batch)
			motif_win_setUserMessageCallback (cb_userMessage);
	#elif defined (macintosh)
		if (! Melder_batch) {
			motif_mac_setUserMessageCallbackA (cb_userMessageA);
			motif_mac_setUserMessageCallbackW (cb_userMessageW);
			Gui_setQuitApplicationCallback (cb_quitApplication);
		}
	#endif

	/*
	 * Make room for commands.
	 */
	praat_actions_init ();
	praat_menuCommands_init ();

	Widget raam = NULL;
	if (Melder_batch) {
		#if defined (UNIX) || defined (macintosh) || defined (_WIN32) && defined (CONSOLE_APPLICATION)
			MelderString_empty (& theCurrentPraatApplication -> batchName);
			for (unsigned int i = iarg_batchName; i < argc; i ++) {
				int needsQuoting = strchr (argv [i], ' ') != NULL && (i == iarg_batchName || i < argc - 1);
				if (i > 1) MelderString_append1 (& theCurrentPraatApplication -> batchName, L" ");
				if (needsQuoting) MelderString_append1 (& theCurrentPraatApplication -> batchName, L"\"");
				MelderString_append1 (& theCurrentPraatApplication -> batchName, Melder_peekUtf8ToWcs (argv [i]));
				if (needsQuoting) MelderString_append1 (& theCurrentPraatApplication -> batchName, L"\"");
			}
		#elif defined (_WIN32)
			MelderString_copy (& theCurrentPraatApplication -> batchName, Melder_peekUtf8ToWcs (argv [3]));
		#endif
	} else {
		char objectWindowTitle [100];
		Machine_initLookAndFeel (argc, argv);
		sprintf (objectWindowTitle, "%s Objects", praatP.title);
		#if gtk
			g_set_application_name (title);
			raam = GuiWindow_create (NULL, -1, Gui_AUTOMATIC, -1, 600, Melder_peekUtf8ToWcs (objectWindowTitle), gui_cb_quit_gtk, NULL, 0);
			theCurrentPraatApplication -> topShell = gtk_widget_get_parent (raam);
			theCurrentPraatApplication -> context = g_main_context_default ();
			GuiObject_show (theCurrentPraatApplication -> topShell);
		#else
			#ifdef _WIN32
				argv [0] = & praatP. title [0];   /* argc == 4 */
				Gui_setOpenDocumentCallback (cb_openDocument);
			#endif
			theCurrentPraatApplication -> topShell = XtVaAppInitialize (& theCurrentPraatApplication -> context, "Praatwulg", NULL, 0, & argc, argv, Machine_getXresources (), NULL);
			XtVaSetValues (theCurrentPraatApplication -> topShell, XmNdeleteResponse, XmDO_NOTHING, XmNtitle, objectWindowTitle, XmNx, 10, NULL);
			#if defined (macintosh) || defined (_WIN32)
				XtVaSetValues (theCurrentPraatApplication -> topShell, XmNheight, WINDOW_HEIGHT, NULL);
			#endif
			#if ! defined (sun4)
			{
				/* Catch Window Manager "Close" and "Quit". */
				Atom atom = XmInternAtom (XtDisplay (theCurrentPraatApplication -> topShell), "WM_DELETE_WINDOW", True);
				XmAddWMProtocols (theCurrentPraatApplication -> topShell, & atom, 1);
				XmAddWMProtocolCallback (theCurrentPraatApplication -> topShell, atom, gui_cb_quit, 0);
			}
			#endif
		#endif
	}
	Thing_recognizeClassesByName (classCollection, classStrings, classManPages, classSortedSetOfString, NULL);
	if (Melder_batch) {
		Melder_backgrounding = TRUE;
		praat_addMenus (NULL);
		praat_addFixedButtons (NULL);
	} else {
		Widget Raam = NULL;
		#if gtk
			Widget raHoriz, raLeft; /* I want to have more possibilities for GTK widgets */
		#else
			#define raHoriz Raam 
			#define raLeft Raam 
		#endif

		#ifdef macintosh
			MelderGui_create (theCurrentPraatApplication -> context, theCurrentPraatApplication -> topShell);   /* BUG: default Melder_assert would call printf recursively!!! */
		#endif
		#if gtk
			Raam = raam;
		#elif motif
			Raam = XmCreateForm (theCurrentPraatApplication -> topShell, "raam", NULL, 0);
		#endif
		#ifdef macintosh
			GuiObject_size (Raam, WINDOW_WIDTH, Gui_AUTOMATIC);
			praatP.topBar = Gui_addMenuBar (Raam);
			GuiObject_show (praatP.topBar);
		#endif
		praatP.menuBar = Gui_addMenuBar (Raam);
		praat_addMenus (praatP.menuBar);
		GuiObject_show (praatP.menuBar);

		#ifndef UNIX
			GuiObject_size (Raam, WINDOW_WIDTH, Gui_AUTOMATIC);
		#endif
		#if gtk
			raHoriz = gtk_hpaned_new ();
			gtk_container_add (GTK_CONTAINER (Raam), raHoriz);
			raLeft = gtk_vbox_new (FALSE, 0);
			gtk_container_add (GTK_CONTAINER (raHoriz), raLeft);
		#else
			GuiLabel_createShown (raLeft, 3, -250, Machine_getMainWindowMenuBarHeight () + 5, Gui_AUTOMATIC, L"Objects:", 0);
		#endif
		praatList_objects = GuiList_create (raLeft, 0, -250, Machine_getMainWindowMenuBarHeight () + 26, -100, true, L" Objects ");
		GuiList_setSelectionChangedCallback (praatList_objects, gui_cb_list, 0);
		//XtVaSetValues (praatList_objects, XmNvisibleItemCount, 20, NULL);
		GuiObject_show (praatList_objects);
		praat_addFixedButtons (raLeft);
		praat_actions_createDynamicMenu (raHoriz, 250);
		#if gtk
			GuiObject_show (raLeft);
			GuiObject_show (raHoriz);
		#endif
		GuiObject_show (Raam);
		
		#if gtk
			gtk_widget_show (theCurrentPraatApplication -> topShell);
		#else
			XtRealizeWidget (theCurrentPraatApplication -> topShell);
		#endif
		#ifdef UNIX
			if ((f = Melder_fopen (& pidFile, "a")) != NULL) {
				#if gtk
					fprintf (f, " %ld", (long) GDK_WINDOW_XID (GDK_DRAWABLE (theCurrentPraatApplication -> topShell -> window)));
				#else
					fprintf (f, " %ld", (long) XtWindow (theCurrentPraatApplication -> topShell));
				#endif
				fclose (f);
			} else {
				Melder_clearError ();
			}
		#endif
		#ifdef UNIX
			Preferences_read (MelderFile_readable (& prefs5File) ? & prefs5File : & prefs4File);
		#endif
		#if ! defined (macintosh)
			/* praat_showLogo (TRUE);   /* Mac: later. */
		#endif
		#if ! defined (CONSOLE_APPLICATION) && ! defined (macintosh)
			MelderGui_create (theCurrentPraatApplication -> context, theCurrentPraatApplication -> topShell);   /* Mac: done this earlier. */
		#endif
		Melder_setHelpProc (helpProc);
	}
	Melder_setPublishProc (publishProc);
	theCurrentPraatApplication -> manPages = ManPages_create ();

	if (! praatP.dontUsePictureWindow) praat_picture_init ();
	#if defined (macintosh)
		if (! Melder_batch) {
			/* praat_showLogo (TRUE);   /* Unix & Windows: earlier. */
		}
	#endif
}

static void executeStartUpFile (MelderDir startUpDirectory, const wchar_t *fileNameTemplate) {
	FILE *f;
	wchar_t name [256];
	swprintf (name, 256, fileNameTemplate, Melder_peekUtf8ToWcs (programName));
	if (! MelderDir_isNull (startUpDirectory)) {   // Should not occur on modern systems.
		structMelderFile startUp = { 0 };
		MelderDir_getFile (startUpDirectory, name, & startUp);
		if ((f = Melder_fopen (& startUp, "r")) != NULL) {
			fclose (f);
			if (! praat_executeScriptFromFile (& startUp, NULL)) {
				Melder_error4 (Melder_peekUtf8ToWcs (praatP.title), L": start-up file ", MelderFile_messageName (& startUp), L" not completed.");
				Melder_flushError (NULL);
			}
		} else {
			Melder_clearError ();
		}
	}
}

void praat_run (void) {
	FILE *f;

	praat_addMenus2 ();
	#ifdef macintosh
		praat_addMenuCommand (L"Objects", L"Praat", L"Quit", 0, praat_HIDDEN, DO_Quit);
	#else
		praat_addMenuCommand (L"Objects", L"Praat", L"-- quit --", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"Praat", L"Quit", 0, praat_UNHIDABLE + 'Q', DO_Quit);
	#endif

	/*
	 * Read the preferences file, and notify those who want to be notified of this,
	 * namely, those who already have a window (namely, the Picture window),
	 * and those that regard the start of a new session as a meaningful event
	 * (namely, the session counter and the cross-session memory counter).
	 */
	Preferences_read (MelderFile_readable (& prefs5File) ? & prefs5File : & prefs4File);
	if (! praatP.dontUsePictureWindow) praat_picture_prefsChanged ();
	praat_statistics_prefsChanged ();
			//Melder_error3 (L"batch name <<", theCurrentPraatApplication -> batchName.string, L">>");
			//Melder_flushError (NULL);

	praatP.phase = praat_STARTING_UP;

	#if defined (UNIX) || defined (macintosh)
		structMelderDir usrLocal = { { 0 } };
		Melder_pathToDir (L"/usr/local", & usrLocal);
		executeStartUpFile (& usrLocal, L"%ls-startUp");
	#endif
	#if defined (UNIX) || defined (macintosh)
		executeStartUpFile (& homeDir, L".%ls-user-startUp");   /* Not on Windows (empty file name error). */
	#endif
	#if defined (UNIX) || defined (macintosh) || defined (_WIN32)
		executeStartUpFile (& homeDir, L"%ls-user-startUp");
	#endif
	/*
	 * Plugins.
	 * The Praat phase should remain praat_STARTING_UP,
	 * because any added commands must not be included in the buttons file.
	 */
	structMelderFile searchPattern = { 0 };
	MelderDir_getFile (& praatDir, L"plugin_*", & searchPattern);
	Strings directoryNames = Strings_createAsDirectoryList (Melder_fileToPath (& searchPattern));
	if (directoryNames != NULL && directoryNames -> numberOfStrings > 0) {
		for (long i = 1; i <= directoryNames -> numberOfStrings; i ++) {
			structMelderDir pluginDir = { { 0 } };
			structMelderFile plugin = { 0 };
			MelderDir_getSubdir (& praatDir, directoryNames -> strings [i], & pluginDir);
			MelderDir_getFile (& pluginDir, L"setup.praat", & plugin);
			if ((f = Melder_fopen (& plugin, "r")) != NULL) {   /* Necessary? */
				fclose (f);
				if (! praat_executeScriptFromFile (& plugin, NULL)) {
					Melder_error4 (Melder_peekUtf8ToWcs (praatP.title), L": plugin ", MelderFile_messageName (& plugin), L" contains an error.");
					Melder_flushError (NULL);
				}
			} else {
				Melder_clearError ();
			}
		}
	}
	forget (directoryNames);
	Melder_clearError ();   /* In case Strings_createAsDirectoryList () returned an error. */

	if (Melder_batch) {
		if (doingCommandLineInterface) {
			if (praat_executeCommandFromStandardInput (praatP.title)) {
				praat_exit (0);
			} else {
				Melder_flushError ("%s: command line session interrupted.", praatP.title);
				praat_exit (-1);
			}
		} else {
			if (praat_executeScriptFromFileNameWithArguments (theCurrentPraatApplication -> batchName.string)) {
				praat_exit (0);
			} else {
				structMelderFile batchFile = { 0 };
				if (! Melder_relativePathToFile (theCurrentPraatApplication -> batchName.string, & batchFile)) praat_exit (-1);
				#if defined (_WIN32) && ! defined (CONSOLE_APPLICATION)
					MelderGui_create (NULL, NULL);
				#endif
				Melder_error4 (Melder_peekUtf8ToWcs (praatP.title), L": command file ", MelderFile_messageName (& batchFile), L" not completed.");
				Melder_flushError (NULL);
				praat_exit (-1);
			}
		}
	} else /* GUI */ {
		praatP.phase = praat_READING_BUTTONS;
		/*
		 * Read the added script buttons. Each line separately: every error should be ignored.
		 */
		wchar_t *buttons = MelderFile_readText (MelderFile_readable (& buttons5File) ? & buttons5File : & buttons4File);
		if (buttons == NULL) {
			Melder_clearError ();   // The file does not have to exist yet.
		} else {
			wchar_t *line = buttons;
			for (;;) {
				wchar_t *newline = wcschr (line, '\n');
				if (newline) *newline = '\0';
				if (! praat_executeCommand (NULL, line)) Melder_clearError ();
				if (newline == NULL) break;
				line = newline + 1;
			}
		}

		/*
		 * Sort the commands.
		 */
		praat_sortMenuCommands ();
		praat_sortActions ();

		praatP.phase = praat_HANDLING_EVENTS;
		
		#if gtk
			//gtk_widget_add_events (G_OBJECT (theCurrentPraatApplication -> topShell), GDK_ALL_EVENTS_MASK);
			g_signal_connect (G_OBJECT (theCurrentPraatApplication -> topShell), "client-event", G_CALLBACK (cb_userMessage), NULL);
			gtk_main ();
		#else

		#if defined (_WIN32)
			if (theCurrentPraatApplication -> batchName.string [0] != '\0') {
				wchar_t text [500];
				/*
				 * The user dropped a file on the Praat icon, while Praat was not running yet.
				 * Windows may have enclosed the path between quotes;
				 * this is especially likely to happen if the path contains spaces (which is usual).
				 * And sometimes, Windows prepends a space before the quote.
				 * Peel all that off.
				 */
				wchar_t *s = theCurrentPraatApplication -> batchName.string;
				swprintf (text, 500, L"Read from file... %ls", s [0] == ' ' && s [1] == '\"' ? s + 2 : s [0] == '\"' ? s + 1 : s);
				long l = wcslen (text);
				if (l > 0 && text [l - 1] == '\"') text [l - 1] = '\0';
				//Melder_error3 (L"command <<", text, L">>");
				//Melder_flushError (NULL);
				if (! praat_executeScriptFromText (text)) Melder_error1 (NULL);   // BUG
			}
		#endif

		for (;;) {
			XEvent event;
			#if defined (UNIX)
				if (haveMessage) {
					if ((f = Melder_fopen (& messageFile, "r")) != NULL) {
						long pid;
						int narg = fscanf (f, "#%ld", & pid);
						fclose (f);
						if (! praat_executeScriptFromFile (& messageFile, NULL)) {
							Melder_error2 (Melder_peekUtf8ToWcs (praatP.title), L": message not completely handled.");
							Melder_flushError (NULL);
						}
						if (narg) kill (pid, SIGUSR2);
					} else {
						Melder_clearError ();
					}
					haveMessage = FALSE;
				}
			#endif
			XtAppNextEvent (theCurrentPraatApplication -> context, & event);
			#if defined (UNIX)
				if (event. type == ClientMessage && event. xclient.send_event && strnequ (& event. xclient.data.b [0], "SENDPRAAT", 9)) {
					if ((f = Melder_fopen (& messageFile, "r")) != NULL) {
						long pid;
						int narg = fscanf (f, "#%ld", & pid);
						fclose (f);
						if (! praat_executeScriptFromFile (& messageFile, NULL)) {
							Melder_error2 (Melder_peekUtf8ToWcs (praatP.title), L": message not completely handled.");
							Melder_flushError (NULL);
						}
						if (narg) kill (pid, SIGUSR2);
					} else {
						Melder_clearError ();
					}
				}
			#endif
			XtDispatchEvent (& event);
		}
		#endif
	}
}

/* End of file praat.c */
