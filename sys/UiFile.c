/* UiFile.c
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
 * pb 2002/03/07 GPL
 * pb 2006/08/10 Windows: turned file selector into a modal dialog box
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2007/02/12 worked around a bug in Windows XP that caused Praat to crash
                 when the user moved the mouse pointer over a file in the Desktop of the second file selector
                 that was raised in Praat. The workaround is to temporarily disable file info tips.
 * pb 2007/03/23 new Editor API
 */

#if defined (macintosh)
	#define DIALOG_WIDTH  310
	#define LABEL_WIDTH  150
	#define FIELD_X  160
	#define FIELD_WIDTH  150
#else
	#define DIALOG_WIDTH  410
	#define LABEL_WIDTH  200
	#define FIELD_X  210
	#define FIELD_WIDTH  200
#endif

#if defined (UNIX)
	#include <dirent.h>
	#include <sys/stat.h>
#endif

#if defined (_WIN32)
	#include <shlobj.h>
#endif

#include <ctype.h>
#include "longchar.h"
#include "Gui.h"
#include "Collection.h"
#include "UiP.h"
#include "Editor.h"
#include "Graphics.h"   /* colours. */
#include "machine.h"

#ifdef macintosh
	#define DIRECTORY_TERM  "Folder:"
#else
	#define DIRECTORY_TERM  "Directory:"
#endif

#if 0
/***** class SimpleDrive *****/

#define SimpleDrive_members Thing_members \
	structMelderDir dir;
#define SimpleDrive_methods Thing_methods
class_create (SimpleDrive, Thing)

class_methods (SimpleDrive, Thing)
class_methods_end

/***** class SimpleDir *****/

#define SimpleDir_members Thing_members \
	structMelderDir dir;
#define SimpleDir_methods Thing_methods
class_create (SimpleDir, Thing)

class_methods (SimpleDir, Thing)
class_methods_end

/***** class SimpleFile *****/

#define SimpleFile_members Thing_members \
	structMelderFile file;
#define SimpleFile_methods Thing_methods
class_create (SimpleFile, Thing)

class_methods (SimpleFile, Thing)
class_methods_end

/***** class SortedSetOfDrive *****/

#define SortedSetOfDrive_members SortedSet_members
#define SortedSetOfDrive_methods SortedSet_methods
class_create (SortedSetOfDrive, SortedSet)

static int classSortedSetOfDrive_compare (I, thou) {
	iam (SimpleDrive); thouart (SimpleDrive);
	return strcmp (my name, thy name);
}

class_methods (SortedSetOfDrive, SortedSet)
	class_method_local (SortedSetOfDrive, compare)
class_methods_end

/***** class SortedSetOfDir *****/

#define SortedSetOfDir_members SortedSet_members
#define SortedSetOfDir_methods SortedSet_methods
class_create (SortedSetOfDir, SortedSet)

static int classSortedSetOfDir_compare (I, thou) {
	iam (SimpleDir); thouart (SimpleDir);
	return strcmp (my name, thy name);
}

class_methods (SortedSetOfDir, SortedSet)
	class_method_local (SortedSetOfDir, compare)
class_methods_end

/***** class SortedSetOfFile *****/

#define SortedSetOfFile_members SortedSet_members
#define SortedSetOfFile_methods SortedSet_methods
class_create (SortedSetOfFile, SortedSet)

static int classSortedSetOfFile_compare (I, thou) {
	iam (SimpleFile); thouart (SimpleFile);
	return strcmp (MelderFile_name (& my file), MelderFile_name (& thy file));
}

class_methods (SortedSetOfFile, SortedSet)
	class_method_local (SortedSetOfFile, compare)
class_methods_end

/***** class UiFileSelector *****/

#define UiFileSelector_members UiField_members \
	int direction;   /* 1 = open, 2 = save */ \
	Widget dirLabel, fileLabel, dirList, fileList, fileText; \
	SortedSetOfDrive drives; \
	SortedSetOfDir dirs; \
	SortedSetOfFile files; \
	Ordered hierarchy; \
	char defaultValue [256]; \
	structMelderDir dir; \
	structMelderFile value;
#define UiFileSelector_methods UiField_methods
class_create_opaque (UiFileSelector, UiField)

static void classUiFileSelector_setDefault (I) { iam (UiFileSelector);
	if (my direction == 2) {
		if (my defaultValue [0]) {
			XmTextFieldSetString (my fileText, my defaultValue);
		}
	}
}

static int classUiFileSelector_widgetToValue (I) { iam (UiFileSelector);
	int nSelected, *selected = NULL;   /* This superfluous initialization makes CodeWarrior happy. */
	if (my direction == 1) {
		if (! XmListGetSelectedPos (my fileList, & selected, & nSelected)) {
			return Melder_error ("No file selected.");
		} else {
			long ifile;
			if (nSelected > 1) {
				XtFree ((char *) selected);
				return Melder_error ("More than one file selected.");
			}
			ifile = selected [0];
			if (ifile < 1 || ifile > my files -> size) {
				XtFree ((char *) selected);
				return Melder_error ("File name #%ld out of array bounds. Should not ocur.", ifile);
			} else {
				SimpleFile simpleFile = my files -> item [ifile];
				my value = simpleFile -> file;
			}
			XtFree ((char *) selected);
		}
	} else if (my direction == 2) {
		char *fileName = XmTextFieldGetString (my fileText);
		MelderDir_getFile (& my dir, fileName, & my value);
		XtFree (fileName);
		XtFree ((char *) selected);
		if (MelderFile_exists (& my value)) return 2;   /* Special value. */
	}
	return 1;
}

static int classUiFileSelector_stringToValue (I, const char *string) { iam (UiFileSelector);
	if (strlen (string) > 255)
		return Melder_error ("File name too long.");
	if (! Melder_relativePathToFile (string, & my value)) return 0;
	return 1;
}

static void classUiFileSelector_valueToHistory (I, int isLast) { iam (UiFileSelector);
	char *path = Melder_fileToPath (& my value);
	if (isLast == FALSE && (path [0] == '\0' || strchr (path, ' ')))
		UiHistory_write (" \"%s\"", path);
	else
		UiHistory_write (" %s", path);
}

static MelderFile classUiFileSelector_getFile (I) { iam (UiFileSelector);
	return & my value;
}

static void classUiFileSelector_setString (I, const char *string) { iam (UiFileSelector);
	long i;
	for (i = 1; i <= my files -> size; i ++) {
		SimpleFile sfile = my files -> item [i];
		if (strequ (string, MelderFile_name (& sfile -> file))) break;
	}
	if (i > my files -> size)
		Melder_fatal ("The file list `%s' does not contain the file `%s'.", my name, string);
	if (my direction == 1)
		XmListSelectPos (my fileList, i, False);
}

static void Melder_getDesktop (MelderDir dir) {
	dir -> path [0] = '\0';
}


static void getNames (UiFileSelector me) {
	long i;
	Collection_removeAllItems (my files);
	Collection_removeAllItems (my dirs);
	Collection_removeAllItems (my drives);
	Collection_removeAllItems (my hierarchy);
	/*
	 * Make sure that my dir is a valid directory.
	 * On Unix, this has the additional advantage of resolving links.
	 */
	if (MelderDir_isDesktop (& my dir)) {
		#ifdef UNIX
			Melder_setDefaultDir ((MelderDir) "/");   /* BUG */
			Melder_getDefaultDir (& my dir);
		#endif
	} else {
		Melder_setDefaultDir (& my dir);
		Melder_getDefaultDir (& my dir);
	}
	#if defined (UNIX)
	{
		structMelderDir dir, parent;
		DIR *d = opendir (my dir. path);
		int i;
if (! d) return;
		for (;;) {
			structMelderFile file;
			struct stat statBuf;
			struct dirent *entry = readdir (d);
			if (entry == NULL) break;
			MelderDir_getFile (& my dir, entry -> d_name, & file);
			stat (file. path, & statBuf);
			if (S_ISDIR (statBuf. st_mode)) {
				/* The following may give a compiler warning "Array index out of range". Does not hurt. */
				if (entry -> d_name [0] != '.' || entry -> d_name [1] != '\0' && entry -> d_name [1] != '.') {
					SimpleDir simpleDir = new (SimpleDir);
					strcpy (simpleDir -> dir. path, file. path);
					Thing_setName (simpleDir, entry -> d_name);
					Collection_addItem (my dirs, simpleDir);
				}
			} else if (my direction == 1) {
				SimpleFile simpleFile = new (SimpleFile);
				strcpy (simpleFile -> file. path, file. path);
				Thing_setName (simpleFile, entry -> d_name);
				Collection_addItem (my files, simpleFile);
			}
		}
		closedir (d);
		dir = my dir;
		for (i = 1; i <= 30; i ++) {
			SimpleDir simpleDir;
			if (MelderDir_isDesktop (& dir)) break;
			simpleDir = new (SimpleDir);
			simpleDir -> dir = dir;
			Thing_setName (simpleDir, MelderDir_name (& dir));
			Collection_addItem (my hierarchy, simpleDir);
			MelderDir_getParentDir (& dir, & parent);
			dir = parent;
		}
	}
	#endif
	/*
	 * The hierarchy is in reverse order. Fix.
	 */
	for (i = 1; i <= my hierarchy -> size / 2; i ++) {
		long j = my hierarchy -> size + 1 - i;
		Any itemi = my hierarchy -> item [i], itemj = my hierarchy -> item [j];
		Any help = itemi;
		my hierarchy -> item [i] = itemj;
		my hierarchy -> item [j] = help;
	}
}

static void doSearch (UiFileSelector me) {
	long i;
	char name [256+20], *spaces = "                                                               ";
	XmString s;
	if (my direction == 1)
		XmListDeleteAllItems (my fileList);
	XmListDeleteAllItems (my dirList);
	XmUpdateDisplay (my dirList);
	getNames (me);
	#if defined (UNIX) || defined (macintosh)
	#else
		s = XmStringCreateSimple ("Desktop");
		XmListAddItem (my dirList, s, 0);
		XmStringFree (s);
	#endif
	if (MelderDir_isDesktop (& my dir)) {
		for (i = 1; i <= my drives -> size; i ++) {
			SimpleDrive simpleDrive = my drives -> item [i];
			sprintf (name, "   [ %s ]", Thing_getName (simpleDrive));
			s = XmStringCreateSimple (name);
			XmListAddItem (my dirList, s, 0);
			XmStringFree (s);
		}
	} else {
		for (i = 1; i <= my hierarchy -> size; i ++) {
			SimpleDir simpleDir = my hierarchy -> item [i];
			#if defined (UNIX) || defined (macintosh)
				char *dirName = Thing_getName (simpleDir);
				sprintf (name, "%.*s/%s", 3 * (i - 1), spaces, dirName [0] ? dirName : "");
			#elif defined (_WIN32)
				sprintf (name, i == 1 ? "%.*s[ %s ]" : "%.*s\\%s", 3 * i, spaces, Thing_getName (simpleDir));
			#endif
			s = XmStringCreateSimple (name);
			XmListAddItem (my dirList, s, 0);
			XmStringFree (s);
		}
		for (i = 1; i <= my dirs -> size; i ++) {
			SimpleDir simpleDir = my dirs -> item [i];
			#if defined (UNIX) || defined (macintosh)
				sprintf (name, "%.*s/%s", 3 * my hierarchy -> size, spaces, Thing_getName (simpleDir));
			#elif defined (_WIN32)
				sprintf (name, "%.*s\\%s", 3 * (my hierarchy -> size + 1), spaces, Thing_getName (simpleDir));
			#endif
			s = XmStringCreateSimple (name);
			XmListAddItem (my dirList, s, 0);
			XmStringFree (s);
		}
	}
	#if defined (UNIX) || defined (macintosh)
		XmListSelectPos (my dirList, my hierarchy -> size, 0);
	#else
		XmListSelectPos (my dirList, my hierarchy -> size + 1, 0);
	#endif
	if (my direction == 1) for (i = 1; i <= my files -> size; i ++) {
		SimpleFile simpleFile = my files -> item [i];
		s = XmStringCreateSimple (MelderFile_name (& simpleFile -> file));
		XmListAddItem (my fileList, s, 0);
		XmStringFree (s);
	}
}

static void classUiFileSelector_startUp (I) { iam (UiFileSelector);
	Melder_getDefaultDir (& my dir);
	doSearch (me);
}

class_methods (UiFileSelector, UiField)
	class_method_local (UiFileSelector, startUp)
	class_method_local (UiFileSelector, setDefault)
	class_method_local (UiFileSelector, widgetToValue)
	class_method_local (UiFileSelector, stringToValue)
	class_method_local (UiFileSelector, valueToHistory)
	class_method_local (UiFileSelector, getFile)
	class_method_local (UiFileSelector, setString)
class_methods_end

MOTIF_CALLBACK (cb_dirList)
	iam (UiFileSelector);
	int nselected, *selected;
	if (! XmListGetSelectedPos (my dirList, & selected, & nselected)) return;
	if (nselected == 1) {   /* Should always succeed. */
		long iselected = selected [0];
#ifndef UNIX
		if (iselected == 1) {
			Melder_getDesktop (& my dir);
			doSearch (me);
		} else {
			iselected -= 1;
#endif
			if (MelderDir_isDesktop (& my dir) && iselected <= my drives -> size) {
				SimpleDrive simpleDrive = my drives -> item [iselected];
				my dir = simpleDrive -> dir;
				doSearch (me);
			} else if (! MelderDir_isDesktop (& my dir) && iselected <= my hierarchy -> size) {
				SimpleDir simpleDir = my hierarchy -> item [iselected];
				my dir = simpleDir -> dir;
				doSearch (me);
			} else {
				iselected -= my drives -> size + my hierarchy -> size;
				if (iselected <= my dirs -> size) {
					SimpleDir simpleDir = my dirs -> item [iselected];
					my dir = simpleDir -> dir;
					doSearch (me);
				}
			}
#ifndef UNIX
		}
#endif
	}
	XtFree ((char *) selected);
MOTIF_CALLBACK_END

UiFileSelector UiFileSelector_create (Widget parent, const char *label, int direction, const char *defaultValue) {
	UiFileSelector me = new (UiFileSelector);
	UiField_init (me, label);
	my direction = direction;
	my files = new (SortedSetOfFile); SortedSet_init (my files, classSimpleFile, 100);
	my dirs = new (SortedSetOfDir); SortedSet_init (my dirs, classSimpleDir, 20);
	my drives = new (SortedSetOfDrive); SortedSet_init (my drives, classSimpleDrive, 10);
	my hierarchy = Ordered_create ();
	if (parent) {
		Widget form = XmCreateForm (parent, "UiFileSelector_hor", NULL, 0), dirScrolled, fileScrolled;
		if (direction == 1) {
			XtVaSetValues (form, XmNwidth, DIALOG_WIDTH + 200, NULL);
			my dirLabel = XtVaCreateManagedWidget (DIRECTORY_TERM, xmLabelWidgetClass, form,
				XmNx, 0, XmNy, 0, XmNwidth, LABEL_WIDTH + 100,
				XmNalignment, XmALIGNMENT_BEGINNING, NULL);
			my fileLabel = XtVaCreateManagedWidget ("File:", xmLabelWidgetClass, form,
				XmNx, FIELD_X + 100, XmNy, 0, XmNwidth, FIELD_WIDTH + 100,
				XmNalignment, XmALIGNMENT_BEGINNING, NULL);
		} else if (direction == 2) {
			my dirLabel = XtVaCreateManagedWidget (DIRECTORY_TERM, xmLabelWidgetClass, form,
				XmNx, 0, XmNy, 0, XmNwidth, 60,
				XmNalignment, XmALIGNMENT_BEGINNING, NULL);
		}
		#if defined (macintosh)
		if (direction == 1) {
			dirScrolled = XmCreateScrolledWindow (form, "UiFileIn_dirScrolled", NULL, 0);
			XtVaSetValues (dirScrolled, XmNx, 0, XmNwidth, FIELD_WIDTH + 100,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 18,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 0, NULL);
			my dirList = XmCreateList (dirScrolled, "UiFileIn_dirList", NULL, 0);
			fileScrolled = XmCreateScrolledWindow (form, "UiFileIn_fileScrolled", NULL, 0);
			XtVaSetValues (fileScrolled, XmNx, FIELD_X + 100, XmNwidth, FIELD_WIDTH + 100,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 18,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 0, NULL);
			my fileList = XmCreateList (fileScrolled, "UiFileIn_fileList", NULL, 0);
		} else if (direction == 2) {
			dirScrolled = XmCreateScrolledWindow (form, "UiFileOut_dirScrolled", NULL, 0);
			XtVaSetValues (dirScrolled, XmNx, 0, XmNwidth, DIALOG_WIDTH,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 18, NULL);
			my dirList = XmCreateList (dirScrolled, "UiFilOut_dirList", NULL, 0);
		}
		#elif defined (UNIX)
		if (direction == 1) {
			Arg args [2];
			XtSetArg (args [0], XmNx, FIELD_X + 100);
			XtSetArg (args [1], XmNwidth, FIELD_WIDTH + 100);
			dirScrolled = XmCreateScrolledWindow (form, "UiFileIn_dirScrolled", args, 2);
			XtVaSetValues (dirScrolled, XmNx, 0,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 25,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 0, NULL);
			my dirList = XmCreateList (dirScrolled, "UiFileIn_dirList", NULL, 0);
			XtSetArg (args [0], XmNx, FIELD_X + 100);
			XtSetArg (args [1], XmNwidth, FIELD_WIDTH + 100);
			fileScrolled = XmCreateScrolledWindow (form, "UiFileIn_fileScrolled", args, 2);
			XtVaSetValues (fileScrolled, XmNx, FIELD_X + 100, XmNwidth, FIELD_WIDTH + 100,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 25,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 0, NULL);
			my fileList = XmCreateList (fileScrolled, "UiFileIn_fileList", NULL, 0);
		} else if (direction == 2) {
			dirScrolled = XmCreateScrolledWindow (form, "UiFileOut_dirScrolled", 0, 0);
			XtVaSetValues (dirScrolled,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 0,
				XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 65,
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 0,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, Machine_getTextHeight () + 23, NULL);
			my dirList = XmCreateList (dirScrolled, "UiFileOut_dirList", NULL, 0);
		}
		#elif defined (_WIN32)
			/*my upButton = XtVaCreatedManagedWidget ("Up", xmPushButtonWidgetClass, form, XmNy, 30, NULL);
			my desktopButton = XtVaCreateManagedWidget ("Desktop", xmPushButtonWidgetClass, form, XmNy, 60, NULL);*/
			my dirList = XmCreateList (form, "UiFileIn_dirList", NULL, 0);
			XtVaSetValues (my dirList, XmNx, 0, XmNwidth, FIELD_WIDTH + 100,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 18,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, direction == 1 ? 0 : Machine_getTextHeight () + 3, NULL);
			my fileList = XmCreateList (form, "UiFileIn_fileList", NULL, 0);
			XtVaSetValues (my fileList, XmNx, FIELD_X + 100, XmNwidth, FIELD_WIDTH + 100,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 18,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, direction == 1 ? 0 : Machine_getTextHeight () + 3, NULL);
		#endif
		XtVaSetValues (my dirList, /*XmNwidth, FIELD_WIDTH,*/ XmNselectionPolicy, XmBROWSE_SELECT,
			XmNvisibleItemCount, my direction == 1 ? 20 : 15, NULL);
		if (direction == 1)
			XtVaSetValues (my fileList, /*XmNwidth, FIELD_WIDTH,*/ XmNselectionPolicy, XmBROWSE_SELECT,
				XmNvisibleItemCount, my direction == 1 ? 20 : 15, NULL);
		XtAddCallback (my dirList, XmNbrowseSelectionCallback, cb_dirList, (XtPointer) me);
		XtManageChild (my dirList);
		if (direction == 1)
			XtManageChild (my fileList);
		#ifndef _WIN32
			XtManageChild (dirScrolled);
			if (direction == 1)
				XtManageChild (fileScrolled);
		#endif
		if (direction == 2) {
			Widget hor = XmCreateForm (parent, "UiFileOut_hor", NULL, 0);
			XtVaSetValues (hor, XmNwidth, DIALOG_WIDTH, NULL);
			XtVaCreateManagedWidget ("File:", xmLabelWidgetClass, hor, NULL);
			my fileText = XmCreateTextField (hor, "UiFileSelector_text", NULL, 0);
			XtVaSetValues (my fileText,
				XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 60,
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 0, NULL);
			XtManageChild (my fileText);
			XtManageChild (hor);
		}
		XtManageChild (form);
		our setDefault (me);
	}
	return me;
}
#endif
/**** ****/

#define UiFile_members Thing_members \
	EditorCommand command; \
	Widget parent, dialog, warning; \
	structMelderFile file; \
	const char *helpTitle; \
	int (*okCallback) (Any sender, void *closure); \
	void *okClosure; \
	int shiftKeyPressed;
#ifdef macintosh
	#define UiFile_methods Thing_methods
#else
	#define UiFile_methods Thing_methods  void (* ok) (I);
#endif
class_create (UiFile, Thing)

static UiFile currentUiFile = NULL;
	/* Currently open file selector (or the last one opened). */

static void classUiFile_destroy (I) {
	iam (UiFile);
	#ifdef UNIX
		XtUnrealizeWidget (XtParent (my dialog));
		XtDestroyWidget (my dialog);
	#endif
	if (me == currentUiFile) currentUiFile = NULL;   /* Undangle. */
	inherited (UiFile) destroy (me);
}

#ifndef macintosh
static void classUiFile_ok (I) { (void) void_me; }
#endif

class_methods (UiFile, Thing)
	class_method_local (UiFile, destroy)
	#ifndef macintosh
	class_method_local (UiFile, ok)
	#endif
class_methods_end

#ifdef UNIX
static void UiFile_ok (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiFile);
	char *fileName;
	int motifBug;
	(void) w;
	XmStringGetLtoR (((XmSelectionBoxCallbackStruct *) call) -> value,
				(XmStringCharSet) XmSTRING_DEFAULT_CHARSET, & fileName);
	my shiftKeyPressed = ((XButtonPressedEvent *) ((XmSelectionBoxCallbackStruct *) call) -> event)
		-> state & ShiftMask;
	/* Work around a Motif BUG. */
	/* Activating the 'OK' button with a directory chosen should perform the actions */
	/* of the 'Filter' button. */
	/* This is only the first occurrence of this work-around in this file. */
	motifBug = fileName [strlen (fileName) - 1] == '/';
	if (motifBug) {
		XmString dirMask;
		XtVaGetValues (my dialog, XmNdirMask, & dirMask, NULL);
		XmFileSelectionDoSearch (my dialog, dirMask);
		XmStringFree (dirMask);
	} else {
		strcpy (my file. path, fileName);
		our ok (me);
	}
	XtFree (fileName);
}
static void UiFile_cancel (Widget w, XtPointer void_me, XtPointer call) {
	(void) void_me;
	(void) call;
	XtUnmanageChild (w);
}
static void UiFile_help (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiFile);
	(void) w;
	(void) void_me;
	(void) call;
	if (my helpTitle) Melder_help (my helpTitle);
}
#endif

#ifdef UNIX
/*static int isFile (const char *name) {
	struct stat statBuf;
	stat (name, & statBuf);
	return (S_ISREG (statBuf. st_mode)) != 0;
}
static int isDirectory (const char *name) {
	struct stat statBuf;
	stat (name, & statBuf);
	return (S_ISDIR (statBuf. st_mode)) != 0;
}*/
#endif

static void UiFile_init (I, Widget parent, const char *title) {
	iam (UiFile);
	my parent = parent;
	#ifdef UNIX
		if (my parent) {
			my dialog = XmCreateFileSelectionDialog (my parent, "FSB dialog", NULL, 0);
		}
		if (my dialog) {
			Longchar_nativize (title, Melder_buffer1, TRUE);
			XtVaSetValues (my dialog,
				motif_argXmString (XmNdialogTitle, Melder_buffer1),
				XmNautoUnmanage, False, XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL, NULL);
			XtVaSetValues (XtParent (my dialog), XmNdeleteResponse, XmUNMAP, NULL);
			XtVaSetValues (XmFileSelectionBoxGetChild (my dialog, XmDIALOG_LIST),
				XmNvisibleItemCount, 20, NULL);
			XtVaSetValues (XmFileSelectionBoxGetChild (my dialog, XmDIALOG_DIR_LIST),
				XmNvisibleItemCount, 20, NULL);
			XtAddCallback (my dialog, XmNokCallback, UiFile_ok, (XtPointer) me);
			XtAddCallback (my dialog, XmNcancelCallback, UiFile_cancel, (XtPointer) me);
			XtAddCallback (my dialog, XmNhelpCallback, UiFile_help, (XtPointer) me);
		}
	#endif
	Thing_setName (me, title);
}

MelderFile UiFile_getFile (I) {
	iam (UiFile);
	return & my file;
}

void UiFile_hide (void) {
	if (currentUiFile) {
		XtUnmanageChild (currentUiFile -> dialog);
		currentUiFile = NULL;
	}
}

/********** READING A FILE **********/

#define UiInfile_members UiFile_members
#define UiInfile_methods UiFile_methods
class_create (UiInfile, UiFile)

#if defined (UNIX)
static XmString inDirMask;
	/* Set when users clicks 'OK'. */
	/* Used just before managing dialog. */
static void classUiInfile_ok (I) {
	iam (UiFile);
	#if 1
		if (inDirMask) XmStringFree (inDirMask);
		XtVaGetValues (my dialog, XmNdirMask, & inDirMask, NULL);
	#endif
	if (! my okCallback (me, my okClosure))
		Melder_flushError ("File \"%s\" not finished.", MelderFile_messageName (& my file));
	else if (! my shiftKeyPressed)
		XtUnmanageChild (my dialog);
	my shiftKeyPressed = 0;
	UiHistory_write (" %s", my file. path);
}
#endif

class_methods (UiInfile, UiFile)
	#ifdef UNIX
	class_method_local (UiInfile, ok)
	#endif
class_methods_end

Any UiInfile_create (Widget parent, const char *title,
	int (*okCallback) (Any dia, void *closure), void *okClosure, const char *helpTitle)
{
	UiInfile me = new (UiInfile);
	my okCallback = okCallback;
	my okClosure = okClosure;
	my helpTitle = helpTitle;
	UiFile_init (me, parent, title);
	return me;
}

void UiInfile_do (I) {
	iam (UiInfile);
	#if defined (macintosh)
		OSStatus err;
		NavDialogRef dialogRef;
		NavDialogCreationOptions dialogOptions;
		NavGetDefaultDialogCreationOptions (& dialogOptions);
		dialogOptions. optionFlags |= kNavDontAutoTranslate;
		err = NavCreateChooseFileDialog (& dialogOptions, NULL, NULL, NULL, NULL, NULL, & dialogRef);
		if (err == noErr) {
			NavReplyRecord reply;
			NavDialogRun (dialogRef);
			err = NavDialogGetReply (dialogRef, & reply);
			if (err == noErr && reply. validRecord) {
				AEKeyword keyWord;
				DescType typeCode;
				Size actualSize = 0;
				FSRef machFile;
				if ((err = AEGetNthPtr (& reply. selection, 1, typeFSRef, & keyWord, & typeCode, & machFile, sizeof (FSRef), & actualSize)) == noErr)
					Melder_machToFile (& machFile, & my file);
				if (! my okCallback (me, my okClosure))
					Melder_flushError ("File \"%s\" not finished.", MelderFile_messageName (& my file));
				UiHistory_write (" %s", Melder_fileToPath (& my file));
				NavDisposeReply (& reply);
			}
			NavDialogDispose (dialogRef);
		}
	#elif defined (_WIN32)
		static OPENFILENAME openFileName, dummy;
		static TCHAR fullFileName [3000+2];
		ZeroMemory (& openFileName, sizeof (OPENFILENAME));
		openFileName. lStructSize = sizeof (OPENFILENAME);
		openFileName. hwndOwner = my parent ? (HWND) XtWindow (my parent) : NULL;
		openFileName. lpstrFilter = "All Files\0*.*\0";
		ZeroMemory (fullFileName, 3000+2);
		openFileName. lpstrFile = fullFileName;
		openFileName. nMaxFile = 3000;
		openFileName. lpstrTitle = my name;
		openFileName. Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		OSVERSIONINFO osVersionInfo;
		ZeroMemory (& osVersionInfo, sizeof (OSVERSIONINFO));
    	osVersionInfo. dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	    GetVersionEx (& osVersionInfo);
		bool hasFileInfoTipsBug = osVersionInfo. dwMajorVersion == 5, infoTipsWereVisible = false, extensionsWereVisible = false;   // XP-only bug.
		if (hasFileInfoTipsBug) {
			SHELLFLAGSTATE settings = { 0 };
			SHGetSettings (& settings, SSF_SHOWINFOTIP | SSF_SHOWEXTENSIONS);
			infoTipsWereVisible = settings. fShowInfoTip != 0;
			extensionsWereVisible = settings. fShowExtensions != 0;
			if (infoTipsWereVisible | ! extensionsWereVisible) {
				SHELLSTATE state = { 0 };
				state. fShowInfoTip = 0;
				state. fShowExtensions = /*1*/ extensionsWereVisible;
				SHGetSetSettings (& state, SSF_SHOWINFOTIP | SSF_SHOWEXTENSIONS, TRUE);
			}
		}
		if (GetOpenFileName (& openFileName)) {
			Melder_pathToFile (fullFileName, & my file);
			if (! my okCallback (me, my okClosure))
				Melder_flushError ("File \"%s\" not finished.", MelderFile_messageName (& my file));
			UiHistory_write (" %s", Melder_fileToPath (& my file));
		}
		if (hasFileInfoTipsBug) {
			if (infoTipsWereVisible | ! extensionsWereVisible) {
				SHELLSTATE state = { 0 };
				state. fShowInfoTip = infoTipsWereVisible;
				state. fShowExtensions = extensionsWereVisible;
				SHGetSetSettings (& state, SSF_SHOWINFOTIP | SSF_SHOWEXTENSIONS, TRUE);
			}
		}
	#else
		#if 1
			XmString dirMask;
			if (inDirMask != NULL)
				XtVaSetValues (my dialog, XmNdirMask, inDirMask, NULL);
			XtVaGetValues (my dialog, XmNdirMask, & dirMask, NULL);
			XmFileSelectionDoSearch (my dialog, dirMask);
			XmStringFree (dirMask);
		#endif
		XtManageChild (my dialog);
		XMapRaised (XtDisplay (XtParent (my dialog)), XtWindow (XtParent (my dialog)));
		currentUiFile = (UiFile) me;
	#endif
}

/********** WRITING A FILE **********/

#define UiOutfile_members UiFile_members \
	int (*allowExecutionHook) (void *closure); \
	void *allowExecutionClosure;   /* I am owner (see destroy). */
#define UiOutfile_methods UiFile_methods
class_create (UiOutfile, UiFile)

#ifdef UNIX
static void UiFile_ok_ok (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiFile);
	(void) call;
	if (w) XtUnmanageChild (w);
	if (! my okCallback (me, my okClosure))
		Melder_flushError ("File \"%s\" not finished.", MelderFile_messageName (& my file));
	XtUnmanageChild (my dialog);
	UiHistory_write (" %s", my file. path);
}
static XmString outDirMask;
	/* Set when users clicks 'OK'. */
	/* Used just before managing dialog. */
static void classUiOutfile_ok (I) {
	iam (UiFile);
	char message [1000];
	#if 1
		if (outDirMask) XmStringFree (outDirMask);
		XtVaGetValues (my dialog, XmNdirMask, & outDirMask, NULL);
	#endif
	if (MelderFile_exists (& my file)) {
		if (! my warning) {
			my warning = XmCreateWarningDialog (my dialog, "fileExists", NULL, 0);
			XtVaSetValues (my warning,
				motif_argXmString (XmNdialogTitle, "File exists"),
				XmNautoUnmanage, True,
				XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
				motif_argXmString (XmNokLabelString, "Overwrite"), NULL);
			XtUnmanageChild (XmMessageBoxGetChild (my warning, XmDIALOG_HELP_BUTTON));
			XtAddCallback (my warning, XmNokCallback, UiFile_ok_ok, me);
		}
		sprintf (message, "A file with the name \"%s\" already exists.\n"
			"Do you want to replace it?", MelderFile_messageName (& my file));
		XtVaSetValues (my warning, motif_argXmString (XmNmessageString, message), NULL);
		XtManageChild (my warning);
	} else {
		UiFile_ok_ok (NULL, me, NULL);
	}
}
#endif

class_methods (UiOutfile, UiFile)
	#ifdef UNIX
	class_method_local (UiOutfile, ok)
	#endif
class_methods_end

#ifdef UNIX
static void defaultAction_cb (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiOutfile);
	XmString dirMask;
	(void) w;
	(void) call;
	XtVaGetValues (my dialog, XmNdirMask, & dirMask, NULL);
	XmFileSelectionDoSearch (my dialog, dirMask);
	XmStringFree (dirMask);
}
#endif
Any UiOutfile_create (Widget parent, const char *title,
	int (*okCallback) (Any dia, void *closure), void *okClosure, const char *helpTitle)
{
	UiOutfile me = new (UiOutfile);
	my okCallback = okCallback;
	my okClosure = okClosure;
	my helpTitle = helpTitle;
	UiFile_init (me, parent, title);
	#ifdef UNIX
		if (my dialog) {
			XtUnmanageChild (XtParent (XmFileSelectionBoxGetChild (my dialog, XmDIALOG_LIST)));
			XtUnmanageChild (XmFileSelectionBoxGetChild (my dialog, XmDIALOG_LIST_LABEL));
			/* Work around a Motif BUG. */
			/* A double-click in the directory list often activated the 'OK' button, */
			/* instead of the 'Filter' button. */
			/* This is the second occurrence of this work-around in this file. */
			XtRemoveAllCallbacks (XmFileSelectionBoxGetChild (my dialog, XmDIALOG_DIR_LIST),
				XmNdefaultActionCallback);
			XtAddCallback (XmFileSelectionBoxGetChild (my dialog, XmDIALOG_DIR_LIST),
				XmNdefaultActionCallback, defaultAction_cb, me);
		}
	#endif
	my allowExecutionHook = theAllowExecutionHookHint;
	my allowExecutionClosure = theAllowExecutionClosureHint;
	return me;
}

static int commonOutfileCallback (Any dia, void *closure) {
	EditorCommand command = (EditorCommand) closure;
	return command -> commandCallback (command -> editor, command, dia);
}

Any UiOutfile_createE (EditorCommand cmd, const char *title, const char *helpTitle) {
	Editor editor = (Editor) cmd -> editor;
	UiOutfile dia = UiOutfile_create (editor -> dialog, title, commonOutfileCallback, cmd, helpTitle);
	dia -> command = cmd;
	return dia;
}

void UiOutfile_do (I, const char *defaultName) {
	iam (UiOutfile);
	#if defined (macintosh)
		const char *lastSlash = strrchr (defaultName, Melder_DIRECTORY_SEPARATOR);
		OSStatus err;
		NavDialogRef dialogRef;
		NavDialogCreationOptions dialogOptions;
		NavGetDefaultDialogCreationOptions (& dialogOptions);
		dialogOptions. windowTitle = CFStringCreateWithCString (NULL, my name, kCFStringEncodingMacRoman);
		dialogOptions. message = CFStringCreateWithCString (NULL, my name, kCFStringEncodingMacRoman);
		dialogOptions. saveFileName = CFStringCreateWithCString (NULL, lastSlash ? lastSlash + 1 : defaultName, kCFStringEncodingMacRoman);
		dialogOptions. optionFlags |= kNavNoTypePopup;
		err = NavCreatePutFileDialog (& dialogOptions, 0, 0, NULL, NULL, & dialogRef);
		if (dialogOptions. windowTitle) CFRelease (dialogOptions. windowTitle);
		if (dialogOptions. message) CFRelease (dialogOptions. message);
		if (dialogOptions. saveFileName) CFRelease (dialogOptions. saveFileName);
		if (err == noErr) {
			NavReplyRecord reply;
			NavDialogRun (dialogRef);
			err = NavDialogGetReply (dialogRef, & reply);
			if (err == noErr && reply. validRecord) {
				AEKeyword keyWord;
				DescType typeCode;
				Size actualSize = 0;
				/*
					According to Navigation.h, it is the case that in this fourth file selection API,
					the reply record contains the DIRECTORY that the user has chosen.
				*/
				FSRef machFile;
				if ((err = AEGetNthPtr (& reply. selection, 1, typeFSRef, & keyWord, & typeCode, & machFile, sizeof (FSRef), & actualSize)) == noErr) {
					structMelderDir dir;
					CFStringRef fileName = NavDialogGetSaveFileName (dialogRef);
					FSRefMakePath (& machFile, (unsigned char *) dir. path, 259);
					if (dir. path [0] != '/' || dir. path [1] != '\0') strcat (dir. path, "/");
					strcpy (my file. path, dir. path);
					/*
						The POSIX path name is stored in canonically decomposed UTF8 encoding.
						The navigation dialog gives us precomposed Unicode.
						So we first decompose, then convert to UTF-8.
					*/
					if (Melder_debug == 19) {
						int i;
						for (i = 0; i < CFStringGetLength (fileName); i ++) {
							Melder_casual ("UniCode %d %d", i, CFStringGetCharacterAtIndex (fileName, i));
						}
					}
					#if defined (__i386__) && 0
						/* System 10.4 and up. */
						CFStringGetFileSystemRepresentation (fileName,
							& my file. path [strlen (my file. path)],
							260 - strlen (my file. path));
					#else
					{
						CFMutableStringRef string = CFStringCreateMutableCopy (NULL, 0, fileName);
						CFStringNormalize (string, kCFStringNormalizationFormD);
						CFStringGetCString (string,
							& my file. path [strlen (my file. path)],
							260 - strlen (my file. path),
							kCFStringEncodingUTF8);
						CFRelease (string);
					}
					#endif
					if (Melder_debug == 19) {
						for (unsigned long i = 0; i < strlen (my file. path); i ++) {
							Melder_casual ("UTF-8 %d %d %c", i, (unsigned char) my file. path [i], my file. path [i]);
						}
					}
					/* The sample code has fileName released, but the header file says we shall retain it if we want to keep it,
					   so it does not seem to be ours. Since our code crashes if we release fileName, we believe the header file. */
					/* From Navigation.h:
					 *  Result:
					 *    The save file name is a CFStringRef. The string is immutable. The
					 *    client should retain the string if the reference is to be held
					 *    beyond the life of the dialog (standard CF retain/release
					 *    semantics).
					 */
					/*CFRelease (fileName);*/
				}
				if (! my okCallback (me, my okClosure))
					Melder_flushError ("File \"%s\" not finished.", MelderFile_messageName (& my file));
				UiHistory_write (" %s", Melder_fileToPath (& my file));
				NavDisposeReply (& reply);
			}
			NavDialogDispose (dialogRef);
		}
	#elif defined (_WIN32)
		OPENFILENAME openFileName;
		static TCHAR customFilter [100+2];
		static TCHAR fullFileName [300+2];
		strcpy (fullFileName, defaultName);
		openFileName. lStructSize = sizeof (OPENFILENAME);
		openFileName. hwndOwner = my parent ? (HWND) XtWindow (my parent) : NULL;
		openFileName. lpstrFilter = NULL;   /* like *.txt */
		openFileName. lpstrCustomFilter = customFilter;
		openFileName. nMaxCustFilter = 100;
		openFileName. lpstrFile = fullFileName;
		openFileName. nMaxFile = 300;
		openFileName. lpstrFileTitle = NULL;
		openFileName. lpstrInitialDir = NULL;
		openFileName. lpstrTitle = my name;
		openFileName. Flags = OFN_LONGNAMES | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_HIDEREADONLY;
		openFileName. lpstrDefExt = NULL;
		SHELLFLAGSTATE settings = { 0 };
		SHGetSettings (& settings, SSF_SHOWINFOTIP | SSF_SHOWEXTENSIONS);
		bool infoTipsWereVisible = settings. fShowInfoTip != 0;
		bool extensionsWereVisible = settings. fShowExtensions != 0;
		if (infoTipsWereVisible | ! extensionsWereVisible) {
			SHELLSTATE state = { 0 };
			state. fShowInfoTip = 0;
			state. fShowExtensions = /*1*/ extensionsWereVisible;
			SHGetSetSettings (& state, SSF_SHOWINFOTIP | SSF_SHOWEXTENSIONS, TRUE);
		}
		if (GetSaveFileName (& openFileName)) {
			if (my allowExecutionHook && ! my allowExecutionHook (my allowExecutionClosure)) {
				Melder_flushError ("Dialog `%s' cancelled.", my name);
				return;
			}
			Melder_pathToFile (fullFileName, & my file);
			if (! my okCallback (me, my okClosure))
				Melder_flushError ("File \"%s\" not finished.", MelderFile_messageName (& my file));
			UiHistory_write (" %s", Melder_fileToPath (& my file));
		}
		if (infoTipsWereVisible | ! extensionsWereVisible) {
			SHELLSTATE state = { 0 };
			state. fShowInfoTip = infoTipsWereVisible;
			state. fShowExtensions = extensionsWereVisible;
			SHGetSetSettings (& state, SSF_SHOWINFOTIP | SSF_SHOWEXTENSIONS, TRUE);
		}
	#else
		XmString xmDirMask, xmDirSpec;
		char *dirMask, *dirSpecc, dirSpec [1000];
		int length;
		#if 1
			if (outDirMask)
				XtVaSetValues (my dialog, XmNdirMask, outDirMask, NULL);
		#endif
		XtVaGetValues (my dialog, XmNdirMask, & xmDirMask, NULL);
		XtVaGetValues (my dialog, XmNdirSpec, & xmDirSpec, NULL);
		XmStringGetLtoR (xmDirMask, XmSTRING_DEFAULT_CHARSET, & dirMask);
		XmStringGetLtoR (xmDirSpec, XmSTRING_DEFAULT_CHARSET, & dirSpecc);
		#if 1
			XmFileSelectionDoSearch (my dialog, xmDirMask);
		#endif
		strcpy (dirSpec, dirMask);
		length = strlen (dirSpec);
		if (dirSpec [length - 1] == '*') dirSpec [length - 1] = '\0';
		strcat (dirSpec, defaultName);
		XtVaSetValues (my dialog, motif_argXmString (XmNdirSpec, dirSpec), NULL);
		XmStringFree (xmDirMask);
		XtFree (dirMask);
		XtFree (dirSpecc);
		XtManageChild (my dialog);
		XMapRaised (XtDisplay (XtParent (my dialog)), XtWindow (XtParent (my dialog)));
		currentUiFile = (UiFile) me;
	#endif
}

/* End of file UiFile.c */
