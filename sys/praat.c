/* praat.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 */

#include "melder.h"
#include "NUMmachar.h"
#include "gsl_errno.h"
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

#define EDITOR  theCurrentPraat -> list [IOBJECT]. editors

#define WINDOW_WIDTH 430
#define WINDOW_HEIGHT 560

structPraat theForegroundPraat;
Praat theCurrentPraat = & theForegroundPraat;
struct PraatP praatP;
static int doingCommandLineInterface;
static char programName [64];
static structMelderDir homeDir = { { 0 } };
/*
 * praatDirectory: preferences and buttons files.
 *    Unix:   /u/miep/.myProg-dir   (without slash)
 *    Windows 95 and 98:   C:\WINDOWS\MyProg
 *    Windows 2000 and XP:   \\myserver\myshare\Miep\MyProg
 *                     or:   C:\Documents and settings\Miep\MyProg
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs
 *    Mac 9:   Macintosh HD:System Folder:Preferences:MyProg Preferences
 */
extern structMelderDir praatDir;
structMelderDir praatDir = { { 0 } };
/*
 * prefsFileName: preferences file.
 *    Unix:   /u/miep/.myProg-dir/prefs
 *    Windows 95 and 98:   C:\WINDOWS\MyProg\Preferences.ini
 *    Windows 2000 and XP:   \\myserver\myshare\Miep\MyProg\Preferences.ini
 *                     or:   C:\Documents and settings\Miep\MyProg\Preferences.ini
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Prefs
 *    Mac 9:   Macintosh HD:System Folder:Preferences:MyProg Preferences:Prefs
 */
static structMelderFile prefsFile = { 0 };
/*
 * buttonsFileName: buttons file.
 *    Unix:   /u/miep/.myProg-dir/buttons
 *    Windows 95 and 98:   C:\WINDOWS\MyProg\Buttons.ini
 *    Windows 2000 and XP:   \\myserver\myshare\Miep\MyProg\Buttons.ini
 *                     or:   C:\Documents and settings\Miep\MyProg\Buttons.ini
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Buttons
 *    Mac 9:   Macintosh HD:System Folder:Preferences:MyProg Preferences:Buttons
 */
static structMelderFile buttonsFile = { 0 };
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
		Melder_error ("No %s #%d selected.", klas ? klas -> _className : "object", inplace) :
		Melder_error ("No %s selected.", klas ? klas -> _className : "object");
}

wchar_t * praat_getNameOfSelected (void *voidklas, int inplace) {
	Data_Table klas = (Data_Table) voidklas;
	int place = inplace, IOBJECT;
	if (place == 0) place = 1;
	if (place > 0) {
		WHERE (SELECTED && (klas == NULL || CLASS == klas)) {
			if (place == 1) return klas == NULL ? FULL_NAMEW : NAMEW;
			place --;
		}
	} else {
		WHERE_DOWN (SELECTED && (klas == NULL || CLASS == klas)) {
			if (place == -1) return klas == NULL ? FULL_NAMEW : NAMEW;
			place ++;
		}
	}
	if (inplace) {
		Melder_error5 (L"No ", klas ? klas -> _classNameW : L"object", L" #", Melder_integer (inplace), L" selected.");
	} else {
		Melder_error3 (L"No ", klas ? klas -> _classNameW : L"object", L" selected.");
	}
	return 0;   // Failure.
}

int praat_selection (void *klas) {
	int result = 0, IOBJECT;
	WHERE (SELECTED && (klas == NULL || CLASS == klas)) result += 1;
	return result;
}

int praat_selectionGeneric (void *klas) {
	int result = 0, IOBJECT;
	WHERE (SELECTED && Thing_subclass (CLASS, klas)) result += 1;
	return result;
}

void praat_deselect (int IOBJECT) {
	if (! SELECTED) return;
	SELECTED = FALSE;
	theCurrentPraat -> totalSelection -= 1;
	if (! theCurrentPraat -> batch && ! Melder_backgrounding) XmListDeselectPos (praatList_objects, IOBJECT);
}

void praat_deselectAll (void) { int IOBJECT; WHERE (1) praat_deselect (IOBJECT); }

void praat_select (int IOBJECT) {
	if (SELECTED) return;
	SELECTED = TRUE;
	theCurrentPraat -> totalSelection += 1;
	if (! theCurrentPraat -> batch && ! Melder_backgrounding) {  /* Trick required, or the other items will be deselected. */
		#ifdef UNIX
			XtVaSetValues (praatList_objects, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
		#endif
		XmListSelectPos (praatList_objects, IOBJECT, False);
		#ifdef UNIX
			XtVaSetValues (praatList_objects, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
		#endif
	}
}

void praat_selectAll (void) { int IOBJECT; WHERE (1) praat_select (IOBJECT); }

void praat_list_background (void) {
	int IOBJECT;
	WHERE (SELECTED) XmListDeselectPos (praatList_objects, IOBJECT);
}
void praat_list_foreground (void) {
	int IOBJECT;
	#ifdef UNIX
		XtVaSetValues (praatList_objects, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
	#endif
	WHERE (SELECTED) XmListSelectPos (praatList_objects, IOBJECT, False);
	#ifdef UNIX
		XtVaSetValues (praatList_objects, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
	#endif
}

Any praat_onlyObject (void *klas) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED && CLASS == klas) { result = IOBJECT; found += 1; }
	if (found != 1) return NULL;
	return theCurrentPraat -> list [result]. object;
}

Any praat_onlyObject_generic (void *klas) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED && Thing_subclass (CLASS, klas)) { result = IOBJECT; found += 1; }
	if (found != 1) return NULL;
	return theCurrentPraat -> list [result]. object;
}

praat_Object praat_onlyScreenObject (void) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED) { result = IOBJECT; found += 1; }
	if (found != 1) Melder_fatal ("praat_onlyScreenObject: found %d objects instead of 1.", found);
	return & theCurrentPraat -> list [result];
}

wchar_t *praat_name (int IOBJECT) { return wcschr (FULL_NAMEW, ' ') + 1; }

void praat_write_do (Any dia, const wchar_t *extension) {
	int IOBJECT, found = 0;
	Data data = NULL;
	wchar_t defaultFileName [200];
	WHERE (SELECTED) { if (! data) data = OBJECT; found += 1; }
	if (found == 1) {
		swprintf (defaultFileName, 200, L"%.50ls.%ls", data -> nameW, extension ? extension : Thing_classNameW (data));
	} else if (extension == NULL) {
		swprintf (defaultFileName, 200, L"praat.Collection");
	} else {
		swprintf (defaultFileName, 200, L"praat.%ls", extension);
	}
	UiOutfile_do (dia, defaultFileName);
}

static void removeAllReferencesToEditor (Any editor) {
	int iobject, ieditor;
	/*
	 * Remove all references to this editor.
	 * It may be editing multiple objects.
	 */
	for (iobject = 1; iobject <= theCurrentPraat -> n; iobject ++)
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (theCurrentPraat -> list [iobject]. editors [ieditor] == editor)
				theCurrentPraat -> list [iobject]. editors [ieditor] = NULL;
	if (praatP. editor == editor)
		praatP. editor = NULL;
}

static void praat_remove (int iobject) {
/* Remove the "object" from the list. */
/* Kill everything to do with selection. */
	int ieditor;
	Melder_assert (iobject >= 1 && iobject <= theCurrentPraat -> n);
	if (theCurrentPraat -> list [iobject]. _beingCreated) {
		theCurrentPraat -> list [iobject]. _beingCreated = FALSE;
		theCurrentPraat -> totalBeingCreated --;
	}
	praat_deselect (iobject);

	/*
	 * To prevent synchronization problems, kill editors before killing the data.
	 */
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		Any editor = theCurrentPraat -> list [iobject]. editors [ieditor];   /* Save this one reference. */
		if (editor) {
			removeAllReferencesToEditor (editor);
			forget (editor);
		}
	}
	MelderFile_setToNull (& theCurrentPraat -> list [iobject]. file);
	Melder_free (theCurrentPraat -> list [iobject]. name);
	forget (theCurrentPraat -> list [iobject]. object);
}

void praat_cleanUpName (wchar_t *name) {
	/*
	 * Replaces spaces and special characters by underscores.
	 */
	for (; *name; name ++) {
		#if defined (_WIN32) || defined (macintosh)
			if (wcschr (L" ,.:;\\/()[]{}~`\'<>*&^%#@!?$\"|", *name)) *name = '_';
		#else
			if (! isalnum (*name) && *name != '-' && *name != '+') *name = '_';
		#endif
	}
}

/***** objects + commands *****/

static int _praat_new (Data me, wchar_t *myName) {
	wchar_t name [200], givenName [200];
	int IOBJECT, ieditor;   /* Must be local: praat_new can be called from within a loop!!! */
	static long uniqueID = 0;
	if (me == NULL) return Melder_error ("No object was put into the list.");

	/*
	 * If my class is Collection, I'll have to be unpacked.
	 */
	if (my methods == (Any) classCollection) {
		Collection list = (Collection) me;
		int idata, result = 1;
		for (idata = 1; idata <= list -> size; idata ++) {
			Data object = list -> item [idata];
			Melder_assert (object -> nameW != NULL);
			result &= _praat_new (object, object -> nameW) ? 1 : 0;   /* Recurse. */
		}
		list -> size = 0;   /* Disown. */
		forget (list);
		return result;
	}

	if (myName [0]) {
		wchar_t *p;
		wcscpy (givenName, myName);
		/*
		 * Remove extension.
		 */
		p = wcsrchr (givenName, '.');
		if (p) *p = '\0';
		praat_cleanUpName (givenName);
	} else {
		swprintf (givenName, 200, my nameW && my nameW [0] ? my nameW : L"untitled");
	}

	swprintf (name, 200, L"%ls %ls", Thing_classNameW (me), givenName);

	if (theCurrentPraat -> n == praat_MAXNUM_OBJECTS) {
		forget (me);
		return Melder_error ("The Object Window cannot contain more than %d objects. "
			"You could remove some objects.", praat_MAXNUM_OBJECTS);
	}
		
	IOBJECT = ++ theCurrentPraat -> n;
	Melder_assert (FULL_NAMEW == NULL);
	FULL_NAMEW = Melder_wcsdup (name);
	Melder_assert (FULL_NAMEW != NULL);
	++ uniqueID;

	if (! theCurrentPraat -> batch) {   /* Put a new object on the screen, at the bottom of the list. */
		XmString s = XmStringCreateSimple (Melder_peekWcsToAscii (name));
		#ifdef UNIX
			XtVaSetValues (praatList_objects, XmNvisibleItemCount, theCurrentPraat -> n + 2, NULL);
		#endif

		/* The new item must appear unselected (for the moment).
		 * If we called XmListAddItem () instead, the item would appear selected if there is already
		 * a selected item with the same name.
		 * It is an enigma to me why this should be a problem, but it is,
		 * because the new item would end up UNselected in this case, after praat_updateSelection.
		 * ppgb, 95/10/16
		 */
		XmListAddItemUnselected (praatList_objects, s, theCurrentPraat -> n);
		XmStringFree (s);
	}
	OBJECT = me;
	SELECTED = FALSE;
	CLASS = my methods;
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		EDITOR [ieditor] = NULL;
	MelderFile_setToNull (& FILENAMEW);
	ID = uniqueID;
	theCurrentPraat -> list [IOBJECT]. _beingCreated = TRUE;
	Thing_setNameW (OBJECT, givenName);
	theCurrentPraat -> totalBeingCreated ++;
	return IOBJECT;
}

int praat_new9 (I, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
	const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9)
{
	iam (Data);
	static MelderStringW myName = { 0 };
	MelderStringW_copyW (& myName, s1);
	MelderStringW_appendW (& myName, s2);
	MelderStringW_appendW (& myName, s3);
	MelderStringW_appendW (& myName, s4);
	MelderStringW_appendW (& myName, s5);
	MelderStringW_appendW (& myName, s6);
	MelderStringW_appendW (& myName, s7);
	MelderStringW_appendW (& myName, s8);
	MelderStringW_appendW (& myName, s9);
	return _praat_new (me, myName.string);
}

int praat_new (I, const char *format, ...) {
/*
   Add an Object to the List if "me" exists.
   Its name will be the highest available of:
      1. 'myName' (made from 'format' and '...')
      2. my name
      3. "untitled"
*/
	iam (Data);
	char myName [200];
	va_list arg;
	va_start (arg, format);
	if (format) {
		vsprintf (Melder_buffer1, format, arg);
		strncpy (myName, Melder_buffer1, 100);
		myName [100] = '\0';
	} else {
		myName [0] = '\0';
	}
	va_end (arg);
	#if defined (macintosh)
	{
		CFStringRef unicodeName = CFStringCreateWithCString (NULL, myName, kCFStringEncodingUTF8);
		if (unicodeName) {
			CFStringGetCString (unicodeName, myName, 200, kCFStringEncodingMacRoman);
			CFRelease (unicodeName);
		}
	}
	#endif
	return _praat_new (me, Melder_peekAsciiToWcs (myName));
}

void praat_updateSelection (void) {
	if (theCurrentPraat -> totalBeingCreated) {
		int IOBJECT;
		praat_deselectAll ();
		WHERE (theCurrentPraat -> list [IOBJECT]. _beingCreated) {
			praat_select (IOBJECT);
			theCurrentPraat -> list [IOBJECT]. _beingCreated = FALSE;
		}
		theCurrentPraat -> totalBeingCreated = 0;
		praat_show ();
	}
}

MOTIF_CALLBACK (cb_list)
	int pos, position_count, *position_list, IOBJECT, first = TRUE;
	WHERE (1) SELECTED = FALSE;
	theCurrentPraat -> totalSelection = 0;
	if (XmListGetSelectedPos (praatList_objects, & position_list, & position_count)) {
		for (pos = 0; pos < position_count; pos ++) {
			IOBJECT = position_list [pos];
			SELECTED = TRUE;
			UiHistory_write (first ? L"\nselect " : L"\nplus ");
			UiHistory_write (FULL_NAMEW);
			first = FALSE;
			theCurrentPraat -> totalSelection += 1;
		}
		XtFree ((XtPointer) position_list);
	}
	praat_show ();
MOTIF_CALLBACK_END

void praat_list_renameAndSelect (int position, const wchar_t *name) {
	if (! theCurrentPraat -> batch) {
		XmString s = XmStringCreateSimple (MOTIF_CONST_CHAR_ARG (Melder_peekWcsToAscii (name)));
		XmListReplaceItemsPos (praatList_objects, & s, 1, position);   /* Void if name equal. */
		if (! Melder_backgrounding)
			XmListSelectPos (praatList_objects, position, False);
		XmStringFree (s);
	}
}

/***** objects *****/

void praat_name2 (wchar_t *name, void *klas1, void *klas2) {
	int i1 = 1, i2;
	wchar_t *name1, *name2;
	while (theCurrentPraat -> list [i1]. selected == 0 || theCurrentPraat -> list [i1]. klas != klas1) i1 ++;
	i2 = 1;   /* This late initialization works around a Think C BUG. */
	while (theCurrentPraat -> list [i2]. selected == 0 || theCurrentPraat -> list [i2]. klas != klas2) i2 ++;
	name1 = wcschr (theCurrentPraat -> list [i1]. name, ' ') + 1;
	name2 = wcschr (theCurrentPraat -> list [i2]. name, ' ') + 1;
	if (wcsequ (name1, name2))
		wcscpy (name, name1);
	else
		swprintf (name, 200, L"%ls_%ls", name1, name2);
}

void praat_removeObject (int i) {
	int j, ieditor;
	praat_remove (i);   /* Dangle. */
	for (j = i; j < theCurrentPraat -> n; j ++)
		theCurrentPraat -> list [j] = theCurrentPraat -> list [j + 1];   /* Undangle but create second references. */
	theCurrentPraat -> list [theCurrentPraat -> n]. name = NULL;   /* Undangle or remove second reference. */
	theCurrentPraat -> list [theCurrentPraat -> n]. object = NULL;   /* Undangle or remove second reference. */
	theCurrentPraat -> list [theCurrentPraat -> n]. selected = 0;
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		theCurrentPraat -> list [theCurrentPraat -> n]. editors [ieditor] = NULL;   /* Undangle or remove second reference. */
	MelderFile_setToNull (& theCurrentPraat -> list [theCurrentPraat -> n]. file);   /* Undangle or remove second reference. */
	-- theCurrentPraat -> n;
	if (! theCurrentPraat -> batch) {
		XmListDeletePos (praatList_objects, i);
		#ifdef UNIX
			XtVaSetValues (praatList_objects, XmNvisibleItemCount, theCurrentPraat -> n + 1, NULL);
		#endif
	}
}

static void praat_exit (int exit_code) {
	int IOBJECT;
	#ifdef _WIN32
		if (! theCurrentPraat -> batch)
			XtDestroyWidget (theCurrentPraat -> topShell);
	#endif
	praat_picture_exit ();
	praat_statistics_exit ();   /* Record total memory use across sessions. */

	/*
	 * Stop receiving messages.
	 */
	#if defined (UNIX)
		if (pidFile. wpath [0]) {
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
	Resources_write (& prefsFile);
	MelderFile_setMacTypeAndCreator (& prefsFile, 'pref', 'PpgB');

	/*
	 * Save the script buttons.
	 */
	if (! theCurrentPraat -> batch) {
		FILE *f = Melder_fopen (& buttonsFile, "wb");
		if (f) {
			MelderFile_setMacTypeAndCreator (& buttonsFile, 'pref', 'PpgB');
			fwprintf (f, L"\ufeff# Buttons (1).\n");
			fwprintf (f, L"# This file is generated automatically when you quit the %ls program.\n", Melder_peekAsciiToWcs (praatP.title));
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
	WHERE_DOWN (! MelderFile_isNull (& FILENAMEW)) praat_remove (IOBJECT);
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
	for (iobject = 1; iobject <= theCurrentPraat -> n; iobject ++) {
		int editingThisObject = FALSE;
		/*
		 * Am I editing this object?
		 */
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (theCurrentPraat -> list [iobject]. editors [ieditor] == me)
				editingThisObject = TRUE;
		if (editingThisObject) {
			/*
			 * Change the data if needed (unusual but possible).
			 * BUG: DO NOT, because changed object may be second data in editor.
			 */
			/*if (data && ((Data) data) -> methods == ((Data) theCurrentPraat -> list [iobject]. object) -> methods)
				theCurrentPraat -> list [iobject]. object = data;*/
			/*
			 * Notify all other editors associated with this object.
			 */
			for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor otherEditor = theCurrentPraat -> list [iobject]. editors [ieditor];
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
	if (! praat_new (publish, NULL)) { Melder_flushError (NULL); return; }
	praat_updateSelection ();
}

int praat_installEditor (Any editor, int IOBJECT) {
	int ieditor;
	if (! editor) return 0;
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		if (EDITOR [ieditor] == NULL) {
			EDITOR [ieditor] = editor;
			Editor_setDestroyCallback (editor, cb_Editor_destroy, NULL);
			Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
			Editor_setPublishCallback (editor, cb_Editor_publish, NULL);
			return 1;
		}
	}
	forget (editor);
	return Melder_error ("(praat_installEditor:) Cannot have more than %d editors with one object.",
		praat_MAXNUM_EDITORS);
}

int praat_installEditor2 (Any editor, int i1, int i2) {
	int ieditor1 = 0, ieditor2 = 0;
	if (! editor) return 0;
	for (ieditor1 = 0; ieditor1 < praat_MAXNUM_EDITORS; ieditor1 ++)
		if (theCurrentPraat -> list [i1]. editors [ieditor1] == NULL)
			break;
	for (ieditor2 = 0; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (theCurrentPraat -> list [i2]. editors [ieditor2] == NULL)
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS) {
		theCurrentPraat -> list [i1]. editors [ieditor1] = theCurrentPraat -> list [i2]. editors [ieditor2] = editor;
		Editor_setDestroyCallback (editor, cb_Editor_destroy, NULL);
		Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
		Editor_setPublishCallback (editor, cb_Editor_publish, NULL);
	} else {
		forget (editor);
		return Melder_error ("(praat_installEditor2:) Cannot have more than %d editors with one object.",
			praat_MAXNUM_EDITORS);
	}
	return 1;
}

int praat_installEditor3 (Any editor, int i1, int i2, int i3) {
	int ieditor1 = 0, ieditor2 = 0, ieditor3;
	if (! editor) return 0;
	for (ieditor1 = 0; ieditor1 < praat_MAXNUM_EDITORS; ieditor1 ++)
		if (theCurrentPraat -> list [i1]. editors [ieditor1] == NULL)
			break;
	for (ieditor2 = 0; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (theCurrentPraat -> list [i2]. editors [ieditor2] == NULL)
			break;
	for (ieditor3 = 0; ieditor3 < praat_MAXNUM_EDITORS; ieditor3 ++)
		if (theCurrentPraat -> list [i3]. editors [ieditor3] == NULL)
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS && ieditor3 < praat_MAXNUM_EDITORS) {
		theCurrentPraat -> list [i1]. editors [ieditor1] = theCurrentPraat -> list [i2]. editors [ieditor2] = theCurrentPraat -> list [i3]. editors [ieditor3] = editor;
		Editor_setDestroyCallback (editor, cb_Editor_destroy, NULL);
		Editor_setDataChangedCallback (editor, cb_Editor_dataChanged, NULL);
		Editor_setPublishCallback (editor, cb_Editor_publish, NULL);
	} else {
		forget (editor);
		return Melder_error ("(praat_installEditor3:) Cannot have more than %d editors with one object.",
			praat_MAXNUM_EDITORS);
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
		for (iPraatObject = 1; iPraatObject <= theCurrentPraat -> n; iPraatObject ++) {
			if (object == theCurrentPraat -> list [iPraatObject]. object) {
				int ieditor;
				for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (theCurrentPraat -> list [iPraatObject]. editors [ieditor] == NULL) {
						break;
					}
				}
				if (ieditor >= praat_MAXNUM_EDITORS) {
					forget (editor);
					return Melder_error ("Cannot view the same object in more than %d windows.", praat_MAXNUM_EDITORS);
				}
				break;
			}
		}
		Melder_assert (iPraatObject <= theCurrentPraat -> n);   /* An element of the Ordered does not occur in the List of Objects. */
	}
	/*
	 * There appears to be room for all elements of the Ordered. The editor window can appear. Install the editor in all objects.
	 */
	for (iOrderedObject = 1; iOrderedObject <= objects -> size; iOrderedObject ++) {
		Data object = objects -> item [iOrderedObject];
		for (iPraatObject = 1; iPraatObject <= theCurrentPraat -> n; iPraatObject ++) {
			if (object == theCurrentPraat -> list [iPraatObject]. object) {
				int ieditor;
				for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (theCurrentPraat -> list [iPraatObject]. editors [ieditor] == NULL) {
						theCurrentPraat -> list [iPraatObject]. editors [ieditor] = editor;
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
		Melder_assert (iPraatObject <= theCurrentPraat -> n);   /* We already checked, but still. */
	}
	return 1;
}

void praat_dataChanged (Any object) {
	int IOBJECT, ieditor;
	WHERE (OBJECT == object) {
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (EDITOR [ieditor]) Editor_dataChanged (EDITOR [ieditor], object);
	}
}

void praat_clipboardChanged (void *closure, Any clipboard) {
	int iobject, ieditor;
	(void) closure;
	for (iobject = 1; iobject <= theCurrentPraat -> n; iobject ++)
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (theCurrentPraat -> list [iobject]. editors [ieditor])
				Editor_clipboardChanged (theCurrentPraat -> list [iobject]. editors [ieditor], clipboard);
}

static void helpProc (const wchar_t *query) {
	if (theCurrentPraat -> batch) { Melder_flushError ("Cannot view manual from batch."); return; }
	if (! Manual_create (theCurrentPraat -> topShell, query, theCurrentPraat -> manPages))
		Melder_flushError ("help: no help on \"%ls\".", query);   /* Failure. */
}

static int publishProc (void *anything) {
	if (! praat_new (anything, NULL)) return Melder_error ("(Melder_publish:) not published.");
	praat_updateSelection ();
	return 1;
}

Editor praat_findEditorFromString (const wchar_t *string) {
	int iobject, ieditor;
	for (iobject = theCurrentPraat -> n; iobject >= 1; iobject --)
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
			Editor editor = theCurrentPraat -> list [iobject]. editors [ieditor];
			if (editor && wcsequ (editor -> nameW, string)) return editor;
	}
	return NULL;
}

/***** QUIT *****/

FORM (Quit, "Confirm Quit", "Quit")
	LABEL ("label", "You have objects in your list!")
	OK
{
	wchar_t prompt [300];
	if (ScriptEditors_dirty ()) {
		if (theCurrentPraat -> n)
			swprintf (prompt, 300, L"You have objects and unsaved scripts! Do you still want to quit %ls?", Melder_peekAsciiToWcs (praatP.title));
		else
			swprintf (prompt, 300, L"You have unsaved scripts! Do you still want to quit %ls?", Melder_peekAsciiToWcs (praatP.title));
		SET_STRINGW (L"label", prompt);
	} else if (theCurrentPraat -> n) {
		swprintf (prompt, 300, L"You have objects in your list! Do you still want to quit %ls?", Melder_peekAsciiToWcs (praatP.title));
		SET_STRINGW (L"label", prompt);
	} else {
		praat_exit (0);
	}
}
DO
	praat_exit (0);
END

MOTIF_CALLBACK (cb_quit)
	DO_Quit (NULL, NULL);
MOTIF_CALLBACK_END

void praat_dontUsePictureWindow (void) { praatP.dontUsePictureWindow = TRUE; }

/********** INITIALIZATION OF THE PRAAT SHELL **********/

#if defined (UNIX)
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
				Melder_error2 (Melder_peekAsciiToWcs (praatP.title), L": message not completely handled.");
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
		XtAppAddTimeOut (theCurrentPraat -> context, 100, timerProc_userMessage, 0);
	}
#elif defined (_WIN32)
	static int cb_userMessage (void) {
		praat_background ();
		if (! praat_executeScriptFromFile (& messageFile, NULL)) {
			Melder_error2 (Melder_peekAsciiToWcs (praatP.title), L": message not completely handled.");
			Melder_flushError (NULL);
		}
		praat_foreground ();
		return 0;
	}
	extern char *sendpraat (void *display, const char *programName, long timeOut, const char *text);
	extern wchar_t *sendpraatW (void *display, const wchar_t *programName, long timeOut, const wchar_t *text);
	static int cb_openDocument (MelderFile file) {
		wchar_t text [500];
		swprintf (text, 500, L"Read from file... %ls", file -> wpath);
		sendpraatW (NULL, Melder_peekAsciiToWcs (praatP.title), 0, text);
		return 0;
	}
#elif defined (macintosh)
	static int cb_userMessageA (char *messageA) {
		praat_background ();
		wchar_t *message = Melder_8bitToWcs ((unsigned char *) messageA, 0);
		if (! praat_executeScriptFromText (message)) error2 (Melder_peekAsciiToWcs (praatP.title), L": message not completely handled.")
	end:
		Melder_free (message);
		praat_foreground ();
		iferror Melder_flushError (NULL);
		return 0;
	}
	static int cb_userMessageW (wchar_t *message) {
		praat_background ();
		if (! praat_executeScriptFromText (message)) error2 (Melder_peekAsciiToWcs (praatP.title), L": message not completely handled.")
	end:
		praat_foreground ();
		iferror Melder_flushError (NULL);
		return 0;
	}
	static int cb_quitApplication (void) {
		DO_Quit (NULL, NULL);
		return 0;
	}
#endif

#ifdef UNIX
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
	
void praat_init (const char *title, unsigned int argc, char **argv) {
	static char truncatedTitle [300];   /* Static because praatP.title will point into it. */
#ifdef UNIX
	FILE *f;
	setlocale (LC_ALL, "en_US");
	//XtSetLanguageProc (NULL, theXtLanguageProc, NULL);
#endif
	char *p;
	#ifdef macintosh
		Gestalt ('sysv', (long *) & Melder_systemVersion);
	#endif
	/*
		Initialize numerical libraries.
	*/
	NUMmachar ();
	gsl_set_error_handler_off ();
	/*
		Remember the current directory. Only useful for scripts run from batch.
	*/
	Melder_rememberShellDirectory ();
	#if defined (UNIX) || defined (macintosh) || defined (_WIN32) && defined (CONSOLE_APPLICATION)
		/*
		 * Running the Praat shell from the Unix command line,
		 * or running PRAATCON.EXE from the MS-DOS prompt or the NT command line:
		 *    <programName> <scriptFileName>
		 */
		theCurrentPraat -> batchName = argc > 1 && (int) argv [1] [0] != '-' ? Melder_asciiToWcs (argv [1]) : NULL;

		Melder_batch = theCurrentPraat -> batchName != NULL;

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
		if (argc == 2 && (int) argv [1] [0] == '-' && argv [1] [1] == '\0') {
			Melder_batch = TRUE;
			doingCommandLineInterface = TRUE;   /* Read from stdin. */
		}

		/* Take from 'title' ("myProg 3.2" becomes "myProg") or from command line ("/ifa/praat" becomes "praat"). */
		sprintf (truncatedTitle, argc && argv [0] [0] ? argv [0] : title && title [0] ? title : "praat");
	#else
		#if defined (_WIN32)
			theCurrentPraat -> batchName = argv [3] ? Melder_asciiToWcs (argv [3]) : L"";   /* The command line. */
		#endif
		Melder_batch = FALSE;   /* Classic Macintosh and PRAAT.EXE are always interactive. */
		sprintf (truncatedTitle, title && title [0] ? title : "praat");
	#endif
	theCurrentPraat -> batch = Melder_batch;

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
			Melder_createDirectoryW (& prefParentDir, name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		#else
			Melder_createDirectoryW (& prefParentDir, name, 0);
		#endif
		MelderDir_getSubdirW (& prefParentDir, name, & praatDir);
		#if defined (UNIX)
			MelderDir_getFileW (& praatDir, L"prefs", & prefsFile);
			MelderDir_getFileW (& praatDir, L"buttons", & buttonsFile);
			MelderDir_getFileW (& praatDir, L"pid", & pidFile);
			MelderDir_getFileW (& praatDir, L"message", & messageFile);
		#elif defined (_WIN32)
			MelderDir_getFileW (& praatDir, L"Preferences.ini", & prefsFile);
			MelderDir_getFileW (& praatDir, L"Buttons.ini", & buttonsFile);
			MelderDir_getFileW (& praatDir, L"Message.txt", & messageFile);
		#elif defined (macintosh)
			MelderDir_getFileW (& praatDir, L"Prefs", & prefsFile);   /* We invite trouble if we call it Preferences! */
			MelderDir_getFileW (& praatDir, L"Buttons", & buttonsFile);
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
				signal (SIGUSR1, handleMessage);
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
			motif_setQuitApplicationCallback (cb_quitApplication);
		}
	#endif

	/*
	 * Make room for commands.
	 */
	praat_actions_init ();
	praat_menuCommands_init ();

	if (Melder_batch) {
		theCurrentPraat -> batchName = Melder_calloc (wchar_t, 1000);
		#if defined (UNIX) || defined (macintosh) || defined (_WIN32) && defined (CONSOLE_APPLICATION)
		{
			unsigned int i;
			for (i = 1; i < argc; i ++) {
				int needsQuoting = strchr (argv [i], ' ') != NULL && (i == 1 || i < argc - 1);
				if (i > 1) wcscat (theCurrentPraat -> batchName, L" ");
				if (needsQuoting) wcscat (theCurrentPraat -> batchName, L"\"");
				wcscat (theCurrentPraat -> batchName, Melder_utf8ToWcs ((unsigned char *) argv [i]));
				if (needsQuoting) wcscat (theCurrentPraat -> batchName, L"\"");
			}
		}
		#elif defined (_WIN32)
			wcscpy (theCurrentPraat -> batchName, Melder_peekAsciiToWcs (argv [3]));
		#endif
	} else {
		char objectWindowTitle [100];
		Machine_initLookAndFeel (argc, argv);
		#ifdef _WIN32
			argv [0] = & praatP. title [0];   /* argc == 4 */
			motif_setOpenDocumentCallback (cb_openDocument);
		#endif
		theCurrentPraat -> topShell = XtVaAppInitialize (& theCurrentPraat -> context, "Praatwulg", NULL, 0, & argc, argv, Machine_getXresources (), NULL);
		sprintf (objectWindowTitle, "%s objects", praatP.title);
		XtVaSetValues (theCurrentPraat -> topShell, XmNdeleteResponse, XmDO_NOTHING, XmNtitle, objectWindowTitle, XmNx, 10, NULL);
		#if defined (macintosh) || defined (_WIN32)
			XtVaSetValues (theCurrentPraat -> topShell, XmNheight, WINDOW_HEIGHT, NULL);
		#endif
		#if ! defined (sun4)
		{
			/* Catch Window Manager "Close" and "Quit". */
			Atom atom = XmInternAtom (XtDisplay (theCurrentPraat -> topShell), "WM_DELETE_WINDOW", True);
			XmAddWMProtocols (theCurrentPraat -> topShell, & atom, 1);
			XmAddWMProtocolCallback (theCurrentPraat -> topShell, atom, cb_quit, 0);
		}
		#endif
	}
	Thing_recognizeClassesByName (classCollection, classStrings, classManPages, classSortedSetOfString, NULL);
	if (Melder_batch) {
		Melder_backgrounding = TRUE;
		praat_addMenus (NULL);
		praat_addFixedButtons (NULL);
	} else {
		Widget Raam = NULL;
		#ifndef _WIN32
			Widget listWindow = 0;
		#endif
		#ifdef macintosh
			MelderMotif_create (theCurrentPraat -> context, theCurrentPraat -> topShell);   /* BUG: default Melder_assert would call printf recursively!!! */
		#endif
		Raam = XmCreateForm (theCurrentPraat -> topShell, "raam", NULL, 0);
		#ifdef macintosh
			XtVaSetValues (Raam, XmNwidth, WINDOW_WIDTH, NULL);
			praatP.topBar = motif_addMenuBar (Raam);
			XtManageChild (praatP.topBar);
		#endif
		praatP.menuBar = motif_addMenuBar (Raam);
		praat_addMenus (praatP.menuBar);
		XtManageChild (praatP.menuBar);
		#ifndef UNIX
			XtVaSetValues (Raam, XmNwidth, WINDOW_WIDTH, NULL);
		#endif
		XtVaCreateManagedWidget ("Objects:", xmLabelWidgetClass, Raam,
			XmNx, 3, XmNy, Machine_getMainWindowMenuBarHeight () + 5, NULL);
		#if defined (_WIN32) || defined (macintoshXXX)
			praatList_objects = XmCreateList (Raam, "list", NULL, 0);
			XtVaSetValues (praatList_objects,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMainWindowMenuBarHeight () + 26,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 100,
				XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 200, NULL);
		#elif defined (UNIX)
			praatList_objects = XmCreateScrolledList (Raam, "list", NULL, 0);
			listWindow = XtParent (praatList_objects);
			XtVaSetValues (listWindow,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMainWindowMenuBarHeight () + 26,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 100,
				XmNleftAttachment, XmATTACH_POSITION, XmNleftPosition, 0,
				XmNrightAttachment, XmATTACH_POSITION, XmNrightPosition, 50, NULL);
		#elif defined (macintosh)
			listWindow = XmCreateScrolledWindow (Raam, "listWindow", NULL, 0);
			XtVaSetValues (listWindow,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMainWindowMenuBarHeight () + 26,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 100,
				XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 200, NULL);
			praatList_objects = XmCreateList (listWindow, "list", NULL, 0);
			XtVaSetValues (praatList_objects, XmNwidth, 500, NULL);
		#endif
		XtAddCallback (praatList_objects, XmNextendedSelectionCallback, cb_list, 0);
		XtManageChild (praatList_objects);
		#if ! defined (_WIN32)
			if (listWindow) XtManageChild (listWindow);
		#endif
		praat_addFixedButtons (Raam);
		praat_actions_createDynamicMenu (Raam, 200);
		XtManageChild (Raam);
		XtRealizeWidget (theCurrentPraat -> topShell);
		#ifdef UNIX
			if ((f = Melder_fopen (& pidFile, "a")) != NULL) {
				fprintf (f, " %ld", (long) XtWindow (theCurrentPraat -> topShell));
				fclose (f);
			} else {
				Melder_clearError ();
			}
		#endif
		#ifdef UNIX
			Resources_read (& prefsFile);
		#endif
		#if ! defined (macintosh)
			/* praat_showLogo (TRUE);   /* Mac: later. */
		#endif
		#if ! defined (CONSOLE_APPLICATION) && ! defined (macintosh)
			MelderMotif_create (theCurrentPraat -> context, theCurrentPraat -> topShell);   /* Mac: done this earlier. */
		#endif
		Melder_setHelpProc (helpProc);
	}
	Melder_setPublishProc (publishProc);
	theCurrentPraat -> manPages = ManPages_create ();
	if (! praatP.dontUsePictureWindow) praat_picture_init ();
	#if defined (macintosh)
		if (! Melder_batch) {
			/* praat_showLogo (TRUE);   /* Unix & Windows: earlier. */
		}
	#endif

	/*
	 * Install the preferences of the Praat shell.
	 */
	praat_statistics_prefs ();   /* Number of sessions, memory used... */
	praat_picture_prefs ();   /* Font... */
	HyperPage_prefs ();   /* Font... */
	Site_prefs ();   /* Print command... */
	Melder_audio_prefs ();   /* Use speaker (Sun & HP), output gain (HP)... */
	Melder_textEncoding_prefs ();
	Printer_prefs ();   /* Paper size, printer command... */
	TextEditor_prefs ();   /* Font size... */
}

static void executeStartUpFile (MelderDir startUpDirectory, const wchar_t *fileNameTemplate) {
	FILE *f;
	wchar_t name [256];
	swprintf (name, 256, fileNameTemplate, Melder_peekAsciiToWcs (programName));
	if (! MelderDir_isNull (startUpDirectory)) {   // Should not occur on modern systems.
		structMelderFile startUp = { 0 };
		MelderDir_getFileW (startUpDirectory, name, & startUp);
		if ((f = Melder_fopen (& startUp, "r")) != NULL) {
			fclose (f);
			if (! praat_executeScriptFromFile (& startUp, NULL)) {
				Melder_error4 (Melder_peekAsciiToWcs (praatP.title), L": start-up file \"", MelderFile_messageNameW (& startUp), L"\" not completed.");
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
	if (Melder_systemVersion >= 0x0A00) {
		praat_addMenuCommand ("Objects", "Praat", "Quit", 0, praat_HIDDEN, DO_Quit);
	} else
	#endif
	{
		praat_addMenuCommand ("Objects", "Praat", "-- quit --", 0, 0, 0);
		praat_addMenuCommand ("Objects", "Praat", "Quit", 0, praat_UNHIDABLE + 'Q', DO_Quit);
	}

	/*
	 * Read the preferences file, and notify those who want to be notified of this,
	 * namely, those who already have a window (namely, the Picture window),
	 * and those that regard the start of a new session as a meaningful event
	 * (namely, the session counter and the cross-session memory counter).
	 */
	Resources_read (& prefsFile);
	if (! praatP.dontUsePictureWindow) praat_picture_prefsChanged ();
	praat_statistics_prefsChanged ();

	praatP.phase = praat_STARTING_UP;

	#if defined (UNIX) || defined (macintosh)
		structMelderDir usrLocal = { { 0 } };
		Melder_pathToDirW (L"/usr/local", & usrLocal);
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
	MelderDir_getFileW (& praatDir, L"plugin_*", & searchPattern);
	Strings directoryNames = Strings_createAsDirectoryList (Melder_fileToPathW (& searchPattern));
	if (directoryNames != NULL && directoryNames -> numberOfStrings > 0) {
		for (long i = 1; i <= directoryNames -> numberOfStrings; i ++) {
			structMelderDir pluginDir = { { 0 } };
			structMelderFile plugin = { 0 };
			MelderDir_getSubdirW (& praatDir, directoryNames -> strings [i], & pluginDir);
			MelderDir_getFileW (& pluginDir, L"setup.praat", & plugin);
			if ((f = Melder_fopen (& plugin, "r")) != NULL) {   /* Necessary? */
				fclose (f);
				if (! praat_executeScriptFromFile (& plugin, NULL))
					Melder_flushError ("%s: plugin \"%s\" contains an error.", praatP.title, MelderFile_messageName (& plugin));
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
			if (praat_executeScriptFromFileNameWithArguments (theCurrentPraat -> batchName)) {
				praat_exit (0);
			} else {
				structMelderFile batchFile = { 0 };
				if (! Melder_relativePathToFile (theCurrentPraat -> batchName, & batchFile)) praat_exit (-1);
				#if defined (_WIN32) && ! defined (CONSOLE_APPLICATION)
					MelderMotif_create (NULL, NULL);
				#endif
				Melder_error4 (Melder_peekAsciiToWcs (praatP.title), L": command file \"", MelderFile_messageNameW (& batchFile), L"\" not completed.");
				Melder_flushError (NULL);
				praat_exit (-1);
			}
		}
	} else /* GUI */ {
		praatP.phase = praat_READING_BUTTONS;
		/*
		 * Read the added script buttons. Each line separately: every error should be ignored.
		 */
		wchar_t *buttons = MelderFile_readText (& buttonsFile);
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
		
		#if defined (_WIN32)
			if (theCurrentPraat -> batchName [0]) {
				wchar_t text [500];
				/*
				 * The user dropped a file on the Praat icon, while Praat was not running yet.
				 * Windows may have enclosed the path between quotes;
				 * this is especially likely to happen if the path contains spaces,
				 * which on Windows XP is very usual.
				 */
				swprintf (text, 500, L"Read from file... %ls", theCurrentPraat -> batchName [0] == '\"' ? theCurrentPraat -> batchName + 1 : theCurrentPraat -> batchName);
				if (wcslen (text) > 0 && text [wcslen (text) - 1] == '\"') {
					text [wcslen (text) - 1] = '\0';
				}
				if (! praat_executeScriptFromText (text)) Melder_error (NULL);
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
							Melder_error2 (Melder_peekAsciiToWcs (praatP.title), L": message not completely handled.");
							Melder_flushError (NULL);
						}
						if (narg) kill (pid, SIGUSR2);
					} else {
						Melder_clearError ();
					}
					haveMessage = FALSE;
				}
			#endif
			XtAppNextEvent (theCurrentPraat -> context, & event);
			#if defined (UNIX)
				if (event. type == ClientMessage && event. xclient.send_event && strnequ (& event. xclient.data.b [0], "SENDPRAAT", 9)) {
					if ((f = Melder_fopen (& messageFile, "r")) != NULL) {
						long pid;
						int narg = fscanf (f, "#%ld", & pid);
						fclose (f);
						if (! praat_executeScriptFromFile (& messageFile, NULL)) {
							Melder_error2 (Melder_peekAsciiToWcs (praatP.title), L": message not completely handled.");
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
	}
}

/* End of file praat.c */
