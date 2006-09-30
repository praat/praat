/* praat.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 */

#include "melder.h"
#include "NUMmachar.h"
#include "gsl_errno.h"
#include <ctype.h>
#include <stdarg.h>
#if defined (UNIX) || defined (__MACH__)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <signal.h>
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

#define EDITOR  praat.list [IOBJECT]. editors

#if defined (_WIN32)
	#define LIST_WIDTH 220
	#define BUTTON_WIDTH 190
	#define WINDOW_HEIGHT 560
#elif defined (macintosh)
	#define LIST_WIDTH 220
	#define BUTTON_WIDTH 190
	#define WINDOW_HEIGHT 560
#else
	#define LIST_WIDTH 229
	#define BUTTON_WIDTH 120
#endif

struct structPraat praat;
struct PraatP praatP;
static int doingCommandLineInterface;
static char programName [64];
static structMelderDir homeDir;
/*
 * praatDirectory: preferences and buttons files.
 *    Unix:   /u/miep/.myProg-dir   (without slash)
 *    Windows 95 and 98:   C:\WINDOWS\MyProg
 *    Windows 2000 and XP:   \\myserver\myshare\Miep\MyProg
 *                     or:   C:\Documents and settings\Miep\MyProg
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs
 *    Mac 9:   Macintosh HD:System Folder:Preferences:MyProg Preferences
 */
static structMelderDir praatDir;
/*
 * prefsFileName: preferences file.
 *    Unix:   /u/miep/.myProg-dir/prefs
 *    Windows 95 and 98:   C:\WINDOWS\MyProg\Preferences.ini
 *    Windows 2000 and XP:   \\myserver\myshare\Miep\MyProg\Preferences.ini
 *                     or:   C:\Documents and settings\Miep\MyProg\Preferences.ini
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Prefs
 *    Mac 9:   Macintosh HD:System Folder:Preferences:MyProg Preferences:Prefs
 */
static structMelderFile prefsFile;
/*
 * buttonsFileName: buttons file.
 *    Unix:   /u/miep/.myProg-dir/buttons
 *    Windows 95 and 98:   C:\WINDOWS\MyProg\Buttons.ini
 *    Windows 2000 and XP:   \\myserver\myshare\Miep\MyProg\Buttons.ini
 *                     or:   C:\Documents and settings\Miep\MyProg\Buttons.ini
 *    Mac X:   /Users/Miep/Library/Preferences/MyProg Prefs/Buttons
 *    Mac 9:   Macintosh HD:System Folder:Preferences:MyProg Preferences:Buttons
 */
static structMelderFile buttonsFile;
#if defined (UNIX)
	static structMelderFile pidFile;   /* Like /u/miep/.myProg-dir/pid */
	static structMelderFile messageFile;   /* Like /u/miep/.myProg-dir/message */
#elif defined (_WIN32)
	static structMelderFile messageFile;   /* Like C:\Windows\myProg\Message.txt */
#endif

static Widget praatList_objects;
static int praat_totalBeingCreated;


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

char * praat_getNameOfSelected (void *voidklas, int inplace) {
	Data_Table klas = (Data_Table) voidklas;
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
	return inplace ?
		Melder_errorp ("No %s #%d selected.", klas ? klas -> _className : "object", inplace) :
		 Melder_errorp ("No %s selected.", klas ? klas -> _className : "object");
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
	praat.totalSelection -= 1;
	if (! Melder_backgrounding) XmListDeselectPos (praatList_objects, IOBJECT);
}

void praat_deselectAll (void) { int IOBJECT; WHERE (1) praat_deselect (IOBJECT); }

void praat_select (int IOBJECT) {
	if (SELECTED) return;
	SELECTED = TRUE;
	praat.totalSelection += 1;
	if (! Melder_backgrounding) {  /* Trick required, or the other items will be deselected. */
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
	return praat.list [result]. object;
}

Any praat_onlyObject_generic (void *klas) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED && Thing_subclass (CLASS, klas)) { result = IOBJECT; found += 1; }
	if (found != 1) return NULL;
	return praat.list [result]. object;
}

praat_Object praat_onlyScreenObject (void) {
	int IOBJECT, result = 0, found = 0;
	WHERE (SELECTED) { result = IOBJECT; found += 1; }
	if (found != 1) Melder_fatal ("praat_onlyScreenObject: found %d objects instead of 1.", found);
	return & praat.list [result];
}

char *praat_name (int IOBJECT) { return strchr (FULL_NAME, ' ') + 1; }

void praat_write_do (Any dia, const char *extension) {
	int IOBJECT, found = 0;
	Data data = NULL;
	char defaultFileName [200];
	WHERE (SELECTED) { if (! data) data = OBJECT; found += 1; }
	if (found == 1) {
		sprintf (defaultFileName, "%.50s.%s", data -> name, extension ? extension : Thing_className (data));
	} else if (extension == NULL) {
		sprintf (defaultFileName, "praat.Collection");
	} else {
		sprintf (defaultFileName, "praat.%s", extension);
	}
	UiOutfile_do (dia, defaultFileName);
}

static void removeAllReferencesToEditor (Any editor) {
	int iobject, ieditor;
	/*
	 * Remove all references to this editor.
	 * It may be editing multiple objects.
	 */
	for (iobject = 1; iobject <= praat.n; iobject ++)
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (praat.list [iobject]. editors [ieditor] == editor)
				praat.list [iobject]. editors [ieditor] = NULL;
	if (praatP. editor == editor)
		praatP. editor = NULL;
}

static void praat_remove (int iobject) {
/* Remove the "object" from the list. */
/* Kill everything to do with selection. */
	int ieditor;
	Melder_assert (iobject >= 1 && iobject <= praat.n);
	if (praat.list [iobject]. _beingCreated) {
		praat.list [iobject]. _beingCreated = FALSE;
		praat_totalBeingCreated --;
	}
	praat_deselect (iobject);

	/*
	 * To prevent synchronization problems, kill editors before killing the data.
	 */
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
		Any editor = praat.list [iobject]. editors [ieditor];   /* Save this one reference. */
		if (editor) {
			removeAllReferencesToEditor (editor);
			forget (editor);
		}
	}
	MelderFile_setToNull (& praat.list [iobject]. file);
	Melder_free (praat.list [iobject]. name);
	forget (praat.list [iobject]. object);
}

void praat_cleanUpName (char *name) {
	/*
	 * Replaces spaces and special characters by underscores.
	 */
	for (; *name; name ++) {
		#if defined (_WIN32) || defined (__MACH__)
			if (strchr (" ,.:;\\/()[]{}~`\'<>*&^%#@!?$\"|", *name)) *name = '_';
		#else
			if (! isalnum (*name) && *name != '-' && *name != '+') *name = '_';
		#endif
	}
}

/***** objects + commands *****/

int praat_new (I, const char *format, ...) {
/*
   Add an Object to the List if "me" exists.
   Its name will be the highest available of:
      1. 'myName' (made from 'format' and '...')
      2. my name
      3. "untitled"
*/
	iam (Data);
	char myName [200], name [200], givenName [200];
	static long uniqueID = 0;
	int IOBJECT, ieditor;   /* Must be local: praat_new can be called from within a loop!!! */
	va_list arg;
	va_start (arg, format);
	if (format) {
		vsprintf (Melder_buffer1, format, arg);
		strncpy (myName, Melder_buffer1, 100);
		myName [100] = '\0';
	} else {
		myName [0] = '\0';
	}
	#if defined (__MACH__)
	{
		CFStringRef unicodeName = CFStringCreateWithCString (NULL, myName, kCFStringEncodingUTF8);
		if (unicodeName) {
			CFStringGetCString (unicodeName, myName, 200, kCFStringEncodingMacRoman);
			CFRelease (unicodeName);
		}
	}
	#endif

	if (me == NULL) return Melder_error ("No object was put into the list.");

	/*
	 * If my class is Collection, I'll have to be unpacked.
	 */
	if (my methods == (Any) classCollection) {
		Collection list = (Collection) me;
		int idata, result = 1;
		for (idata = 1; idata <= list -> size; idata ++) {
			Data object = list -> item [idata];
			result &= praat_new (object, "%.50s", object -> name) ? 1 : 0;   /* Recurse. */
		}
		list -> size = 0;   /* Disown. */
		forget (list);
		return result;
	}

	if (myName [0]) {
		char *p;
		strcpy (givenName, myName);
		/*
		 * Remove extension.
		 */
		p = strrchr (givenName, '.');
		if (p) *p = '\0';
		praat_cleanUpName (givenName);
	} else {
		sprintf (givenName, my name && my name [0] ? my name : "untitled");
	}

	sprintf (name, "%s %s", Thing_className (me), givenName);

	if (praat.n == praat_MAXNUM_OBJECTS) {
		forget (me);
		return Melder_error ("The Object Window cannot contain more than %d objects. "
			"You could remove some objects.", praat_MAXNUM_OBJECTS);
	}
		
	IOBJECT = ++ praat.n;
	Melder_assert (FULL_NAME == NULL);
	FULL_NAME = Melder_strdup (name);
	Melder_assert (FULL_NAME != NULL);
	++ uniqueID;

	if (! Melder_batch) {   /* Put a new object on the screen, at the bottom of the list. */
		XmString s = XmStringCreateSimple (name);
		#ifdef UNIX
			XtVaSetValues (praatList_objects, XmNvisibleItemCount, praat.n + 2, NULL);
		#endif

		/* The new item must appear unselected (for the moment).
		 * If we called XmListAddItem () instead, the item would appear selected if there is already
		 * a selected item with the same name.
		 * It is an enigma to me why this should be a problem, but it is,
		 * because the new item would end up UNselected in this case, after praat_updateSelection.
		 * ppgb, 95/10/16
		 */
		XmListAddItemUnselected (praatList_objects, s, praat.n);
		XmStringFree (s);
	}
	OBJECT = me;
	SELECTED = FALSE;
	CLASS = my methods;
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		EDITOR [ieditor] = NULL;
	MelderFile_setToNull (& FILENAME);
	ID = uniqueID;
	praat.list [IOBJECT]. _beingCreated = TRUE;
	Thing_setName (OBJECT, givenName);
	praat_totalBeingCreated ++;
	va_end (arg);
	return IOBJECT;
}

void praat_updateSelection (void) {
	if (praat_totalBeingCreated) {
		int IOBJECT;
		praat_deselectAll ();
		WHERE (praat.list [IOBJECT]. _beingCreated) {
			praat_select (IOBJECT);
			praat.list [IOBJECT]. _beingCreated = FALSE;
		}
		praat_totalBeingCreated = 0;
		/*
		 * The following is a workaround for a bug in LessTif (the free Motif implementation for Linux),
		 * suggested by Stefan Werner of Joensuu on January 26, 1999,
		 * after I had suggested him to insert this line in praat_init (), which did not suffice.
		 */
		#ifdef lesstif
			XtVaSetValues (praatList_objects, XmNwidth, LIST_WIDTH - 14, NULL);
		#endif
		praat_show ();
	}
}

MOTIF_CALLBACK (cb_list)
	int pos, position_count, *position_list, IOBJECT, first = TRUE;
	WHERE (1) SELECTED = FALSE;
	praat.totalSelection = 0;
	if (XmListGetSelectedPos (praatList_objects, & position_list, & position_count)) {
		for (pos = 0; pos < position_count; pos ++) {
			IOBJECT = position_list [pos];
			SELECTED = TRUE;
			UiHistory_write (first ? "\nselect %s" : "\nplus %s", FULL_NAME);
			first = FALSE;
			praat.totalSelection += 1;
		}
		XtFree ((XtPointer) position_list);
	}
	praat_show ();
MOTIF_CALLBACK_END

void praat_list_renameAndSelect (int position, const char *name) {
	if (! Melder_batch) {
		XmString s = XmStringCreateSimple (MOTIF_CONST_CHAR_ARG (name));
		XmListReplaceItemsPos (praatList_objects, & s, 1, position);   /* Void if name equal. */
		if (! Melder_backgrounding)
			XmListSelectPos (praatList_objects, position, False);
		XmStringFree (s);
	}
}

/***** objects *****/

void praat_name2 (char *name, void *klas1, void *klas2) {
	int i1 = 1, i2;
	char *name1, *name2;
	while (praat.list [i1]. selected == 0 || praat.list [i1]. klas != klas1) i1 ++;
	i2 = 1;   /* This late initialization works around a Think C BUG. */
	while (praat.list [i2]. selected == 0 || praat.list [i2]. klas != klas2) i2 ++;
	name1 = strchr (praat.list [i1]. name, ' ') + 1;
	name2 = strchr (praat.list [i2]. name, ' ') + 1;
	if (strequ (name1, name2))
		strcpy (name, name1);
	else
		sprintf (name, "%s_%s", name1, name2);
}

void praat_removeObject (int i) {
	int j, ieditor;
	praat_remove (i);   /* Dangle. */
	for (j = i; j < praat.n; j ++)
		praat.list [j] = praat.list [j + 1];   /* Undangle but create second references. */
	praat.list [praat.n]. name = NULL;   /* Undangle or remove second reference. */
	praat.list [praat.n]. object = NULL;   /* Undangle or remove second reference. */
	praat.list [praat.n]. selected = 0;
	for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
		praat.list [praat.n]. editors [ieditor] = NULL;   /* Undangle or remove second reference. */
	MelderFile_setToNull (& praat.list [praat.n]. file);   /* Undangle or remove second reference. */
	-- praat.n;
	if (! Melder_batch) {
		XmListDeletePos (praatList_objects, i);
		#ifdef UNIX
			XtVaSetValues (praatList_objects, XmNvisibleItemCount, praat.n + 1, NULL);
		#endif
	}
}

static void praat_exit (int exit_code) {
	int IOBJECT;
	#ifdef _WIN32
		if (! Melder_batch)
			XtDestroyWidget (praat. topShell);
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
	Resources_write (& prefsFile);
	MelderFile_setMacTypeAndCreator (& prefsFile, 'pref', 'PpgB');

	/*
	 * Save the script buttons.
	 */
	if (! Melder_batch) {
		FILE *f = Melder_fopen (& buttonsFile, "w");
		if (f) {
			MelderFile_setMacTypeAndCreator (& buttonsFile, 'pref', 'PpgB');
			fprintf (f, "# Buttons (1).\n");
			fprintf (f, "# This file is generated automatically when you quit the %s program.\n", praatP.title);
			fprintf (f, "# It contains the buttons that you added interactively to the fixed or dynamic menus,\n");
			fprintf (f, "# and the buttons that you hid or showed.\n\n");
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
	WHERE_DOWN (! MelderFile_isNull (& FILENAME)) praat_remove (IOBJECT);
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
	for (iobject = 1; iobject <= praat.n; iobject ++) {
		int editingThisObject = FALSE;
		/*
		 * Am I editing this object?
		 */
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (praat.list [iobject]. editors [ieditor] == me)
				editingThisObject = TRUE;
		if (editingThisObject) {
			/*
			 * Change the data if needed (unusual but possible).
			 * BUG: DO NOT, because changed object may be second data in editor.
			 */
			/*if (data && ((Data) data) -> methods == ((Data) praat.list [iobject]. object) -> methods)
				praat.list [iobject]. object = data;*/
			/*
			 * Notify all other editors associated with this object.
			 */
			for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
				Editor otherEditor = praat.list [iobject]. editors [ieditor];
				if (otherEditor != NULL && otherEditor != me)
					Editor_dataChanged (otherEditor, data);
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
		if (praat.list [i1]. editors [ieditor1] == NULL)
			break;
	for (ieditor2 = 0; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (praat.list [i2]. editors [ieditor2] == NULL)
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS) {
		praat.list [i1]. editors [ieditor1] = praat.list [i2]. editors [ieditor2] = editor;
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
		if (praat.list [i1]. editors [ieditor1] == NULL)
			break;
	for (ieditor2 = 0; ieditor2 < praat_MAXNUM_EDITORS; ieditor2 ++)
		if (praat.list [i2]. editors [ieditor2] == NULL)
			break;
	for (ieditor3 = 0; ieditor3 < praat_MAXNUM_EDITORS; ieditor3 ++)
		if (praat.list [i3]. editors [ieditor3] == NULL)
			break;
	if (ieditor1 < praat_MAXNUM_EDITORS && ieditor2 < praat_MAXNUM_EDITORS && ieditor3 < praat_MAXNUM_EDITORS) {
		praat.list [i1]. editors [ieditor1] = praat.list [i2]. editors [ieditor2]  = praat.list [i3]. editors [ieditor3] = editor;
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
		for (iPraatObject = 1; iPraatObject <= praat.n; iPraatObject ++) {
			if (object == praat.list [iPraatObject]. object) {
				int ieditor;
				for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (praat.list [iPraatObject]. editors [ieditor] == NULL) {
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
		Melder_assert (iPraatObject <= praat.n);   /* An element of the Ordered does not occur in the List of Objects. */
	}
	/*
	 * There appears to be room for all elements of the Ordered. The editor window can appear. Install the editor in all objects.
	 */
	for (iOrderedObject = 1; iOrderedObject <= objects -> size; iOrderedObject ++) {
		Data object = objects -> item [iOrderedObject];
		for (iPraatObject = 1; iPraatObject <= praat.n; iPraatObject ++) {
			if (object == praat.list [iPraatObject]. object) {
				int ieditor;
				for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
					if (praat.list [iPraatObject]. editors [ieditor] == NULL) {
						praat.list [iPraatObject]. editors [ieditor] = editor;
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
		Melder_assert (iPraatObject <= praat.n);   /* We already checked, but still. */
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
	for (iobject = 1; iobject <= praat.n; iobject ++)
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++)
			if (praat.list [iobject]. editors [ieditor])
				Editor_clipboardChanged (praat.list [iobject]. editors [ieditor], clipboard);
}

static void helpProc (const char *query) {
	if (Melder_batch) { Melder_flushError ("Cannot view manual from batch."); return; }
	if (! Manual_create (praat.topShell, query, praat.manPages))
		Melder_flushError ("help: no help on \"%s\".", query);   /* Failure. */
}

static int publishProc (void *anything) {
	if (! praat_new (anything, NULL)) return Melder_error ("(Melder_publish:) not published.");
	praat_updateSelection ();
	return 1;
}

Editor praat_findEditorFromString (const char *string) {
	int iobject, ieditor;
	for (iobject = praat.n; iobject >= 1; iobject --)
		for (ieditor = 0; ieditor < praat_MAXNUM_EDITORS; ieditor ++) {
			Editor editor = praat.list [iobject]. editors [ieditor];
			if (editor && strequ (editor -> name, string)) return editor;
	}
	return NULL;
}

/***** QUIT *****/

FORM (Quit, "Confirm Quit", "Quit")
	LABEL ("label", "You have objects in your list!")
	OK
{
	char prompt [300];
	if (ScriptEditors_dirty ()) {
		if (praat.n)
			sprintf (prompt, "You have objects and unsaved scripts! Do you still want to quit %s?", praatP.title);
		else
			sprintf (prompt, "You have unsaved scripts! Do you still want to quit %s?", praatP.title);
		SET_STRING ("label", prompt);
	} else if (praat.n) {
		sprintf (prompt, "You have objects in your list! Do you still want to quit %s?", praatP.title);
		SET_STRING ("label", prompt);
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
			if (! praat_executeScriptFromFile (& messageFile, NULL))
				Melder_flushError ("%s: message not completely handled.", praatP.title);
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
		XtAppAddTimeOut (praat.context, 100, timerProc_userMessage, 0);
	}
#elif defined (_WIN32)
	static int cb_userMessage (void) {
		if (! praat_executeScriptFromFile (& messageFile, NULL))
			Melder_flushError ("%s: message not completely handled.", praatP.title);
		return 0;
	}
	extern char *sendpraat (void *display, const char *programName, long timeOut, const char *text);
	static int cb_openDocument (MelderFile file) {
		char text [500];
		sprintf (text, "Read from file... %s", file -> path);
		sendpraat (NULL, praatP.title, 0, text);
		return 0;
	}
#elif defined (macintosh)
	static int cb_userMessage (char *message) {
		if (! praat_executeScriptFromText (message))
			Melder_flushError ("%s: message not completely handled.", praatP.title);
		return 0;
	}
	static int cb_quitApplication (void) {
		DO_Quit (NULL, NULL);
		return 0;
	}
#endif

void praat_init (const char *title, unsigned int argc, char **argv) {
	static char truncatedTitle [300];   /* Static because praatP.title will point into it. */
#ifdef UNIX
	FILE *f;
#endif
	char *p;
	#ifdef macintosh
		#ifndef __MACH__
			extern unsigned short Gestalt (unsigned long selector, long *response);
		#endif
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
	#if defined (UNIX) || defined (__MACH__) || defined (_WIN32) && defined (CONSOLE_APPLICATION)
		/*
		 * Running the Praat shell from the Unix command line,
		 * or running PRAATCON.EXE from the MS-DOS prompt or the NT command line:
		 *    <programName> <scriptFileName>
		 */
		praat.batchName = argc > 1 && (int) argv [1] [0] != '-' ? argv [1] : NULL;

		Melder_batch = praat.batchName != NULL;

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
			praat.batchName = argv [3];   /* The command line. */
		#endif
		Melder_batch = FALSE;   /* Classic Macintosh and PRAAT.EXE are always interactive. */
		sprintf (truncatedTitle, title && title [0] ? title : "praat");
	#endif
	praat.batch = Melder_batch;

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
	 *    "Macintosh HD:System Folder:Preferences:MyProg Preferences" (Classic Macintosh)
	 *    "/Users/miep/Library/Preferences/MyProg Prefs" (Mach)
	 *    "C:\Windows\MyProg" (Windows 95)
	 *    "C:\Documents and Settings\Miep\MyProg" (Windows XP)
	 * and construct a preferences-file name and a script-buttons-file name like
	 *    /u/miep/.myProg-dir/prefs   (Unix)
	 *    /u/miep/.myProg-dir/script_buttons
	 * or
	 *    Harde schijf:Systeemmap:Voorkeuren:MyProg Preferences:Prefs
	 *    Harde schijf:Systeemmap:Voorkeuren:MyProg Preferences:Buttons
	 * or
	 *    /Users/miep/Library/Preferences/MyProg Prefs/Prefs
	 *    /Users/miep/Library/Preferences/MyProg Prefs/Buttons
	 * or
	 *    C:\Windows\MyProg\Preferences.ini
	 *    C:\Windows\MyProg\Buttons.ini
	 * On Unix, also create names for process-id and message files.
	 */
	{
		structMelderDir prefParentDir;   /* Directory under which to store our preferences directory. */
		char name [256];
		Melder_getPrefDir (& prefParentDir);
		/*
		 * Make sure that the program's private directory exists.
		 */
		#if defined (UNIX)
			sprintf (name, ".%s-dir", programName);   /* For example .myProg-dir */
		#elif defined (__MACH__)
			sprintf (name, "%s Prefs", praatP.title);   /* For example MyProg Prefs */
		#elif defined (macintosh)
			sprintf (name, "%s Preferences", praatP.title);   /* For example MyProg Preferences */
		#elif defined (_WIN32)
			sprintf (name, "%s", praatP.title);   /* For example MyProg */
		#endif
		#if defined (UNIX) || defined (__MACH__)
			Melder_createDirectory (& prefParentDir, name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		#else
			Melder_createDirectory (& prefParentDir, name, 0);
		#endif
		MelderDir_getSubdir (& prefParentDir, name, & praatDir);
		#if defined (UNIX)
			MelderDir_getFile (& praatDir, "prefs", & prefsFile);
			MelderDir_getFile (& praatDir, "buttons", & buttonsFile);
			MelderDir_getFile (& praatDir, "pid", & pidFile);
			MelderDir_getFile (& praatDir, "message", & messageFile);
		#elif defined (_WIN32)
			MelderDir_getFile (& praatDir, "Preferences.ini", & prefsFile);
			MelderDir_getFile (& praatDir, "Buttons.ini", & buttonsFile);
			MelderDir_getFile (& praatDir, "Message.txt", & messageFile);
		#elif defined (macintosh)
			MelderDir_getFile (& praatDir, "Prefs", & prefsFile);   /* We invite trouble if we call it Preferences! */
			MelderDir_getFile (& praatDir, "Buttons", & buttonsFile);
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
			motif_mac_setUserMessageCallback (cb_userMessage);
			motif_setQuitApplicationCallback (cb_quitApplication);
		}
	#endif

	/*
	 * Make room for commands.
	 */
	praat_actions_init ();
	praat_menuCommands_init ();

	if (Melder_batch) {
		praat.batchName = Melder_calloc (1000, 1);
		#if defined (UNIX) || defined (__MACH__) || defined (_WIN32) && defined (CONSOLE_APPLICATION)
		{
			unsigned int i;
			for (i = 1; i < argc; i ++) {
				int needsQuoting = strchr (argv [i], ' ') != NULL && (i == 1 || i < argc - 1);
				if (i > 1) strcat (praat.batchName, " ");
				if (needsQuoting) strcat (praat.batchName, "\"");
				strcat (praat.batchName, argv [i]);
				if (needsQuoting) strcat (praat.batchName, "\"");
			}
		}
		#elif defined (_WIN32)
			strcpy (praat.batchName, argv [3]);
		#endif
	} else {
		char objectWindowTitle [100];
		Machine_initLookAndFeel (argc, argv);
		#ifdef _WIN32
			argv [0] = & praatP. title [0];   /* argc == 4 */
			motif_setOpenDocumentCallback (cb_openDocument);
		#endif
		praat.topShell = XtVaAppInitialize (& praat.context, "Praatwulg", NULL, 0, & argc, argv, Machine_getXresources (), NULL);
		sprintf (objectWindowTitle, "%s objects", praatP.title);
		XtVaSetValues (praat.topShell, XmNdeleteResponse, XmDO_NOTHING, XmNtitle, objectWindowTitle, XmNx, 10, NULL);
		#if defined (macintosh) || defined (_WIN32)
			XtVaSetValues (praat.topShell, XmNheight, WINDOW_HEIGHT, NULL);
		#endif
		#if ! defined (sun4)
		{
			/* Catch Window Manager "Close" and "Quit". */
			Atom atom = XmInternAtom (XtDisplay (praat.topShell), "WM_DELETE_WINDOW", True);
			XmAddWMProtocols (praat.topShell, & atom, 1);
			XmAddWMProtocolCallback (praat.topShell, atom, cb_quit, 0);
		}
		#endif
	}
	Thing_recognizeClassesByName (classCollection, classManPages, classSortedSetOfString, NULL);
	if (Melder_batch) {
		Melder_backgrounding = TRUE;
		praat_addMenus (NULL);
		praat_addFixedButtons (NULL);
	} else {
		Widget Raam = NULL;
#ifndef _WIN32
		Widget listWindow;
#endif
		#ifdef macintosh
		MelderMotif_create (praat.context, praat.topShell);   /* BUG: default Melder_assert would call printf recursively!!! */
		#endif
		Raam = XmCreateForm (praat.topShell, "raam", NULL, 0);
#ifdef macintosh
		XtVaSetValues (Raam, XmNwidth, LIST_WIDTH + BUTTON_WIDTH, NULL);
		praatP.topBar = motif_addMenuBar (Raam);
		XtManageChild (praatP.topBar);
#endif
		praatP.menuBar = motif_addMenuBar (Raam);
		praat_addMenus (praatP.menuBar);
		XtManageChild (praatP.menuBar);
		#ifndef UNIX
			XtVaSetValues (Raam, XmNwidth, LIST_WIDTH + BUTTON_WIDTH, NULL);
		#endif
		XtVaCreateManagedWidget ("Objects:", xmLabelWidgetClass, Raam,
			XmNx, 3, XmNy, Machine_getMainWindowMenuBarHeight () + 5, NULL);
#ifdef _WIN32
		praatList_objects = XmCreateList (Raam, "list", NULL, 0);
		XtVaSetValues (praatList_objects,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 26,
			XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 100,
			XmNx, -1, XmNwidth, LIST_WIDTH, NULL);
#else
		listWindow = XmCreateScrolledWindow (Raam, "listWindow", NULL, 0);
		#ifdef macintosh
			XtVaSetValues (listWindow,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMainWindowMenuBarHeight () + 26,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 100,
				XmNx, -1, XmNwidth, LIST_WIDTH, NULL);
		#else
			XtVaSetValues (listWindow,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMainWindowMenuBarHeight () + 26,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 80,
				XmNwidth, LIST_WIDTH, NULL);
		#endif
		praatList_objects = XmCreateList (listWindow, "list", NULL, 0);
#endif
		XtAddCallback (praatList_objects, XmNextendedSelectionCallback, cb_list, 0);
		XtManageChild (praatList_objects);
#ifndef _WIN32
		XtManageChild (listWindow);
#endif
		praat_addFixedButtons (Raam);
		praat_actions_createDynamicMenu (Raam, LIST_WIDTH - 1);
		XtManageChild (Raam);
		XtRealizeWidget (praat.topShell);
		#ifdef UNIX
			if ((f = Melder_fopen (& pidFile, "a")) != NULL) {
				fprintf (f, " %ld", (long) XtWindow (praat.topShell));
				fclose (f);
			} else {
				Melder_clearError ();
			}
		#endif
		#ifdef UNIX
			Resources_read (& prefsFile);
		#endif
		#if ! defined (macintosh)
			praat_showLogo (TRUE);   /* Mac: later. */
		#endif
		#if ! defined (CONSOLE_APPLICATION) && ! defined (macintosh)
			MelderMotif_create (praat.context, praat.topShell);   /* Mac: done this earlier. */
		#endif
		Melder_setHelpProc (helpProc);
	}
	Melder_setPublishProc (publishProc);
	praat.manPages = ManPages_create ();
	if (! praatP.dontUsePictureWindow) praat_picture_init ();
	#if defined (macintosh)
		if (! Melder_batch)
			praat_showLogo (TRUE);   /* Unix & Windows: earlier. */
	#endif

	/*
	 * Install the preferences of the Praat shell.
	 */
	praat_statistics_prefs ();   /* Number of sessions, memory used... */
	praat_picture_prefs ();   /* Font... */
	HyperPage_prefs ();   /* Font... */
	Site_prefs ();   /* Print command... */
	Melder_audio_prefs ();   /* Use speaker (Sun & HP), output gain (HP)... */
	Printer_prefs ();   /* Paper size, printer command... */
	TextEditor_prefs ();   /* Font size... */
}

static void executeStartUpFile (MelderDir startUpDirectory, const char *fileNameTemplate) {
	FILE *f;
	char name [256];
	sprintf (name, fileNameTemplate, programName);
	if (! MelderDir_isNull (startUpDirectory)) {   /* Home directory can be null on Windows 95/98. */
		structMelderFile startUp;
		MelderDir_getFile (startUpDirectory, name, & startUp);
		if ((f = Melder_fopen (& startUp, "r")) != NULL) {
			fclose (f);
			if (! praat_executeScriptFromFile (& startUp, NULL))
				Melder_flushError ("%s: start-up file \"%s\" not completed.", praatP.title, MelderFile_messageName (& startUp));
		} else {
			Melder_clearError ();
		}
	} else {
		if ((f = Melder_fopen ((MelderFile) name, "r")) != NULL) {   /* Future BUG. Would already crash on MacOS 7/8/9. */
			fclose (f);
			if (! praat_executeScriptFromFileNameWithArguments (name))
				Melder_flushError ("%s: start-up file \"%s\" not completed.", praatP.title, name);
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

	#if defined (UNIX) || defined (__MACH__)
	{
		structMelderDir usrLocal;
		Melder_pathToDir ("/usr/local", & usrLocal);
		executeStartUpFile (& usrLocal, "%s-startUp");
	}
	#endif
	#if defined (UNIX) || defined (__MACH__)
		executeStartUpFile (& homeDir, ".%s-user-startUp");   /* Not on Windows (empty file name error). */
	#endif
	#if defined (UNIX) || defined (__MACH__) || defined (_WIN32)
		executeStartUpFile (& homeDir, "%s-user-startUp");
	#endif
	#if defined (UNIX) || defined (__MACH__) || defined (_WIN32)
	/*
	 * Plugins.
	 * The Praat phase should remain praat_STARTING_UP,
	 * because any added commands must not be included in the buttons file.
	 */
	{
		structMelderFile searchPattern;
		Strings directoryNames;
		long i;
		MelderDir_getFile (& praatDir, "plugin_*", & searchPattern);
		directoryNames = Strings_createAsDirectoryList (Melder_fileToPath (& searchPattern));
		if (directoryNames != NULL && directoryNames -> numberOfStrings > 0) {
			for (i = 1; i <= directoryNames -> numberOfStrings; i ++) {
				structMelderDir pluginDir;
				structMelderFile plugin;
				FILE *f;
				MelderDir_getSubdir (& praatDir, directoryNames -> strings [i], & pluginDir);
				MelderDir_getFile (& pluginDir, "setup.praat", & plugin);
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
	}
	#endif

	if (Melder_batch) {
		if (doingCommandLineInterface) {
			if (praat_executeCommandFromStandardInput (praatP.title)) {
				praat_exit (0);
			} else {
				Melder_flushError ("%s: command line session interrupted.", praatP.title);
				praat_exit (-1);
			}
		} else {
			if (praat_executeScriptFromFileNameWithArguments (praat.batchName)) {
				praat_exit (0);
			} else {
				structMelderFile batchFile;
				if (! Melder_relativePathToFile (praat.batchName, & batchFile)) praat_exit (-1);
				#if defined (_WIN32) && ! defined (CONSOLE_APPLICATION)
					MelderMotif_create (NULL, NULL);
				#endif
				Melder_flushError ("%s: command file \"%s\" not completed.", praatP.title, MelderFile_messageName (& batchFile));
				praat_exit (-1);
			}
		}
	} else /* GUI */ {
		praatP.phase = praat_READING_BUTTONS;
		/*
		 * Read the added script buttons. Each line separately: every error should be ignored.
		 */
		if ((f = Melder_fopen (& buttonsFile, "r")) != NULL) {
			for (;;) {
				char line [300];
				int length;
				fgets (line, 300, f);
				if (feof (f)) break;
				length = strlen (line);
				if (line [length - 1] == '\n') line [length - 1] = '\0';
				if (! praat_executeCommand (NULL, line)) Melder_clearError ();
			}
			Melder_clearError ();   /* BUG: it seems that praat_executeCommand does not always return 0 if window not found. */
			fclose (f);
		} else {
			Melder_clearError ();
		}

		/*
		 * Sort the commands.
		 */
		praat_sortMenuCommands ();
		praat_sortActions ();

		praatP.phase = praat_HANDLING_EVENTS;
		
		#if defined (_WIN32)
			if (praat.batchName [0]) {
				char text [500];
				/*
				 * The user dropped a file on the Praat icon, while Praat was not running yet.
				 * Windows may have enclosed the path between quotes;
				 * this is especially likely to happen if the path contains spaces,
				 * which on Windows XP is very usual.
				 */
				sprintf (text, "Read from file... %s", praat.batchName [0] == '\"' ? praat.batchName + 1 : praat.batchName);
				if (strlen (text) > 0 && text [strlen (text) - 1] == '\"') {
					text [strlen (text) - 1] = '\0';
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
						if (! praat_executeScriptFromFile (& messageFile, NULL))
							Melder_flushError ("%s: message not completely handled.", praatP.title);
						if (narg) kill (pid, SIGUSR2);
					} else {
						Melder_clearError ();
					}
					haveMessage = FALSE;
				}
			#endif
			XtAppNextEvent (praat.context, & event);
			#if defined (UNIX)
				if (event. type == ClientMessage && event. xclient.send_event && strnequ (& event. xclient.data.b [0], "SENDPRAAT", 9)) {
					if ((f = Melder_fopen (& messageFile, "r")) != NULL) {
						long pid;
						int narg = fscanf (f, "#%ld", & pid);
						fclose (f);
						if (! praat_executeScriptFromFile (& messageFile, NULL))
							Melder_flushError ("%s: message not completely handled.", praatP.title);
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
