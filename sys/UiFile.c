/* UiFile.c
 *
 * Copyright (C) 1992-2009 Paul Boersma
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
 * pb 2007/05/30 wchar_t
 * pb 2009/01/18 arguments to UiForm callbacks
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

#define UiFile_members Thing_members \
	EditorCommand command; \
	Widget parent, dialog, warning; \
	structMelderFile file; \
	const wchar_t *helpTitle; \
	int (*okCallback) (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, void *closure); \
	void *okClosure; \
	int shiftKeyPressed;
#ifdef macintosh
	#define UiFile_methods Thing_methods
#else
	#define UiFile_methods Thing_methods  void (* ok) (I);
#endif
class_create (UiFile, Thing);

static UiFile currentUiFile = NULL;
	/* Currently open file selector (or the last one opened). */

static void classUiFile_destroy (I) {
	iam (UiFile);
	#ifdef UNIX
		#if motif
			XtUnrealizeWidget (XtParent (my dialog));
		#endif
		GuiObject_destroy (my dialog);
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
#if motif
static void UiFile_ok (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiFile);
	char *fileName;
	int motifBug;
	(void) w;
	XmStringGetLtoR (((XmSelectionBoxCallbackStruct *) call) -> value, (XmStringCharSet) XmSTRING_DEFAULT_CHARSET, & fileName);
	my shiftKeyPressed = ((XButtonPressedEvent *) ((XmSelectionBoxCallbackStruct *) call) -> event) -> state & ShiftMask;
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
		Melder_pathToFile (Melder_peekUtf8ToWcs (fileName), & my file);
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

static void UiFile_init (I, Widget parent, const wchar_t *title) {
	iam (UiFile);
	my parent = parent;
	#ifdef UNIX
		if (my parent) {
			#if gtk
				my dialog = gtk_file_chooser_dialog_new (Melder_peekWcsToUtf8 (title), NULL,
									 GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,
									 GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

				gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (my dialog), TRUE);
			#elif motif
				my dialog = XmCreateFileSelectionDialog (my parent, "FSB dialog", NULL, 0);
			#endif
		}
		if (my dialog) {
			#if motif
				XtVaSetValues (my dialog,
					motif_argXmString (XmNdialogTitle, Melder_peekWcsToUtf8 (title)),
					XmNautoUnmanage, False, XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL, NULL);
				XtVaSetValues (XtParent (my dialog), XmNdeleteResponse, XmUNMAP, NULL);
				XtVaSetValues (XmFileSelectionBoxGetChild (my dialog, XmDIALOG_LIST),
					XmNvisibleItemCount, 20, NULL);
				XtVaSetValues (XmFileSelectionBoxGetChild (my dialog, XmDIALOG_DIR_LIST),
					XmNvisibleItemCount, 20, NULL);
				XtAddCallback (my dialog, XmNokCallback, UiFile_ok, (XtPointer) me);
				XtAddCallback (my dialog, XmNcancelCallback, UiFile_cancel, (XtPointer) me);
				XtAddCallback (my dialog, XmNhelpCallback, UiFile_help, (XtPointer) me);
			#endif
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
		GuiObject_hide (currentUiFile -> dialog);
		currentUiFile = NULL;
	}
}

/********** READING A FILE **********/

#define UiInfile_members UiFile_members
#define UiInfile_methods UiFile_methods
class_create (UiInfile, UiFile);

#if defined (UNIX)
#if motif
static XmString inDirMask;
#endif
	/* Set when users clicks 'OK'. */
	/* Used just before managing dialog. */
static void classUiInfile_ok (I) {
	iam (UiFile);
	#if motif
		if (inDirMask) XmStringFree (inDirMask);
		XtVaGetValues (my dialog, XmNdirMask, & inDirMask, NULL);
	#endif
	structMelderFile file;
	MelderFile_copy (& my file, & file);   // save, because okCallback could destroy me
	if (! my okCallback ((UiForm) me, NULL, NULL, my okClosure)) {
		Melder_error3 (L"File ", MelderFile_messageName (& file), L" not finished.");
		Melder_flushError (NULL);
	} else if (! my shiftKeyPressed) {
		GuiObject_hide (my dialog);
	}
	//my shiftKeyPressed = 0;   // BUG (perhaps), but "me" can have been deleted
	UiHistory_write (L" ");
	UiHistory_write (file. path);
}
#endif

class_methods (UiInfile, UiFile)
	#ifdef UNIX
	class_method_local (UiInfile, ok)
	#endif
class_methods_end

Any UiInfile_create (Widget parent, const wchar_t *title,
	int (*okCallback) (UiForm, const wchar_t *, Interpreter, void *), void *okClosure, const wchar_t *helpTitle)
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
	#if gtk
		if (gtk_dialog_run (GTK_DIALOG (my dialog)) == GTK_RESPONSE_ACCEPT) {
			structMelderFile file;
			char *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (my dialog));
			Melder_pathToFile (Melder_peekUtf8ToWcs (filename), & my file);
			g_free (filename);
			MelderFile_copy (& my file, & file);   // save, because okCallback could destroy me
			if (! my okCallback (me, NULL, NULL, my okClosure)) {
				Melder_error3 (L"File ", MelderFile_messageName (& file), L" not finished.");
				Melder_flushError (NULL);
			}
			UiHistory_write (L" ");
			UiHistory_write (Melder_fileToPath (& file));
			/* TODO: IMHO mag deze code wel gedeeld worden tussen de verschillende platfromen
			 * met een platform afhankelijke run/fetch methode. */
		}
		gtk_widget_hide(my dialog);
	#elif defined (macintosh)
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
				structMelderFile file;
				MelderFile_copy (& my file, & file);   // save, because okCallback could destroy me
				if (! my okCallback ((UiForm) me, NULL, NULL, my okClosure)) {
					Melder_error3 (L"File ", MelderFile_messageName (& file), L" not finished.");
					Melder_flushError (NULL);
				}
				UiHistory_write (L" ");
				UiHistory_write (Melder_fileToPath (& file));
				NavDisposeReply (& reply);
			}
			NavDialogDispose (dialogRef);
		}
	#elif defined (_WIN32)
		static OPENFILENAMEW openFileName, dummy;
		static wchar_t fullFileName [3000+2];
		ZeroMemory (& openFileName, sizeof (OPENFILENAMEW));
		openFileName. lStructSize = sizeof (OPENFILENAMEW);
		openFileName. hwndOwner = my parent ? (HWND) XtWindow (my parent) : NULL;
		openFileName. lpstrFilter = L"All Files\0*.*\0";
		ZeroMemory (fullFileName, (3000+2) * sizeof (wchar_t));
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
		if (GetOpenFileNameW (& openFileName)) {
			#if 0
				MelderInfo_open ();
				for (int i = 0; i < strlen (fullFileName); i ++) {
					char buffer [2];
					buffer [0] = fullFileName [i];
					buffer [1] = 0;
					MelderInfo_writeLine3 (Melder_integer (fullFileName [i]), " ", buffer);
				}
				MelderInfo_close ();
			#endif
			Melder_pathToFile (fullFileName, & my file);
			structMelderFile file;
			MelderFile_copy (& my file, & file);   // save, because okCallback could destroy me
			if (! my okCallback ((UiForm) me, NULL, NULL, my okClosure)) {
				Melder_error3 (L"File ", MelderFile_messageName (& file), L" not finished.");
				Melder_flushError (NULL);
			}
			UiHistory_write (L" ");
			UiHistory_write (Melder_fileToPath (& file));
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
		#if motif
			XmString dirMask;
			if (inDirMask != NULL)
				XtVaSetValues (my dialog, XmNdirMask, inDirMask, NULL);
			XtVaGetValues (my dialog, XmNdirMask, & dirMask, NULL);
			XmFileSelectionDoSearch (my dialog, dirMask);
			XmStringFree (dirMask);
		#endif
		GuiDialog_show (my dialog);
		currentUiFile = (UiFile) me;
	#endif
}

/********** WRITING A FILE **********/

#define UiOutfile_members UiFile_members \
	int (*allowExecutionHook) (void *closure); \
	void *allowExecutionClosure;   /* I am owner (see destroy). */
#define UiOutfile_methods UiFile_methods
class_create (UiOutfile, UiFile);

#ifdef UNIX
static void UiFile_ok_ok (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiFile);
	(void) call;
	if (w) GuiObject_hide (w);
	if (! my okCallback ((UiForm) me, NULL, NULL, my okClosure)) {
		Melder_error3 (L"File ", MelderFile_messageName (& my file), L" not finished.");
		Melder_flushError (NULL);
	}
	GuiObject_hide (my dialog);
	UiHistory_write (L" ");
	UiHistory_write (my file. path);
}
#if motif
static XmString outDirMask;
#endif
	/* Set when users clicks 'OK'. */
	/* Used just before managing dialog. */
static void classUiOutfile_ok (I) {
	iam (UiFile);

	#if gtk
		UiFile_ok_ok (NULL, me, NULL);
	#elif motif
		if (outDirMask) XmStringFree (outDirMask);
		XtVaGetValues (my dialog, XmNdirMask, & outDirMask, NULL);
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
		wchar_t *message = Melder_wcscat3 (L"A file with the name ", MelderFile_messageName (& my file),
				L" already exists.\nDo you want to replace it?");
		XtVaSetValues (my warning, motif_argXmString (XmNmessageString, Melder_peekWcsToUtf8 (message)), NULL);
		XtManageChild (my warning);
	} else {
		UiFile_ok_ok (NULL, me, NULL);
	
	}
	#endif
}
#endif

class_methods (UiOutfile, UiFile)
	#ifdef UNIX
	class_method_local (UiOutfile, ok)
	#endif
class_methods_end

#ifdef UNIX
#if motif
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
#endif
Any UiOutfile_create (Widget parent, const wchar_t *title,
	int (*okCallback) (UiForm, const wchar_t *, Interpreter, void *), void *okClosure, const wchar_t *helpTitle)
{
	UiOutfile me = new (UiOutfile);
	my okCallback = okCallback;
	my okClosure = okClosure;
	my helpTitle = helpTitle;
	UiFile_init (me, parent, title);
	#ifdef UNIX
		if (my dialog) {
			#if gtk
				gtk_file_chooser_set_action(GTK_FILE_CHOOSER(my dialog), GTK_FILE_CHOOSER_ACTION_SAVE);
			#elif motif
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
			#endif
		}
	#endif
	my allowExecutionHook = theAllowExecutionHookHint;
	my allowExecutionClosure = theAllowExecutionClosureHint;
	return me;
}

static int commonOutfileCallback (UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter, void *closure) {
	EditorCommand command = (EditorCommand) closure;
	return command -> commandCallback (command -> editor, command, sendingForm, sendingString, interpreter);
}

Any UiOutfile_createE (EditorCommand cmd, const wchar_t *title, const wchar_t *helpTitle) {
	Editor editor = (Editor) cmd -> editor;
	UiOutfile dia = UiOutfile_create (editor -> dialog, title, commonOutfileCallback, cmd, helpTitle);
	dia -> command = cmd;
	return dia;
}

void UiOutfile_do (I, const wchar_t *defaultName) {
	iam (UiOutfile);
	#if gtk
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(my dialog), Melder_peekWcsToUtf8(defaultName)); // TODO: Waarom krijg ik hier geen fullpath?
	if (gtk_dialog_run (GTK_DIALOG (my dialog)) == GTK_RESPONSE_ACCEPT) {
		structMelderFile file;
		char *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (my dialog));
		Melder_pathToFile (Melder_peekUtf8ToWcs (filename), & my file);
		g_free (filename);
		MelderFile_copy (& my file, & file);   // save, because okCallback could destroy me
		if (! my okCallback (me, NULL, NULL, my okClosure)) {
			Melder_error3 (L"File ", MelderFile_messageName (& file), L" not finished.");
			Melder_flushError (NULL);
		}
		UiHistory_write (L" ");
		UiHistory_write (Melder_fileToPath (& file));
		/* TODO: IMHO mag deze code wel gedeeld worden tussen de verschillende platfromen
		 * met een platform afhankelijke run/fetch methode. */
	}
	gtk_widget_hide(my dialog);

	#elif defined (macintosh)
		const wchar_t *lastSlash = wcsrchr (defaultName, Melder_DIRECTORY_SEPARATOR);
		OSStatus err;
		NavDialogRef dialogRef;
		NavDialogCreationOptions dialogOptions;
		NavGetDefaultDialogCreationOptions (& dialogOptions);
		dialogOptions. windowTitle = Melder_peekWcsToCfstring (my name);
		dialogOptions. message = Melder_peekWcsToCfstring (my name);
		dialogOptions. saveFileName = Melder_peekWcsToCfstring (lastSlash ? lastSlash + 1 : defaultName);
		dialogOptions. optionFlags |= kNavNoTypePopup;
		err = NavCreatePutFileDialog (& dialogOptions, 0, 0, NULL, NULL, & dialogRef);
		if (err == noErr) {
			NavReplyRecord reply;
			NavDialogRun (dialogRef);
			err = NavDialogGetReply (dialogRef, & reply);
			if (Melder_debug == 19) {
				Melder_casual ("err %d %d", err, reply. validRecord);
			}
			if (err == noErr && reply. validRecord) {
				AEKeyword keyWord;
				DescType typeCode;
				Size actualSize = 0;
				FSRef machFile;
				if ((err = AEGetNthPtr (& reply. selection, 1, typeFSRef, & keyWord, & typeCode, & machFile, sizeof (FSRef), & actualSize)) == noErr) {
					CFStringRef fileName = NavDialogGetSaveFileName (dialogRef);   // "Get", therefore it's not ours.
					/*
					 * machFile contains the directory as e.g. "/" or "/Users/jane"; in the latter (most usual) case, append a slash.
					 */
					char directoryPath [1000];
					FSRefMakePath (& machFile, (unsigned char *) directoryPath, 999);
					if (! (directoryPath [0] == '/' && directoryPath [1] == '\0'))
						strcat (directoryPath, "/");
					Melder_8bitToWcs_inline (directoryPath, my file. path, kMelder_textInputEncoding_UTF8);
					int dirLength = wcslen (my file. path);
					int n = CFStringGetLength (fileName);
					wchar_t *p = my file. path + dirLength;
					for (int i = 0; i < n; i ++, p ++)
						*p = CFStringGetCharacterAtIndex (fileName, i);
					*p = '\0';
				}
				if (! my okCallback ((UiForm) me, NULL, NULL, my okClosure)) {
					Melder_error3 (L"File ", MelderFile_messageName (& my file), L" not finished.");
					Melder_flushError (NULL);
				}
				UiHistory_write (L" ");
				UiHistory_write (Melder_fileToPath (& my file));
				NavDisposeReply (& reply);
			}
			NavDialogDispose (dialogRef);
		}
	#elif defined (_WIN32)
		OPENFILENAMEW openFileName;
		static wchar_t customFilter [100+2];
		static wchar_t fullFileName [300+2];
		long n = wcslen (defaultName);
		for (long i = 0; i <= n; i ++) {
			fullFileName [i] = defaultName [i];
		}
		openFileName. lStructSize = sizeof (OPENFILENAMEW);
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
		if (infoTipsWereVisible || ! extensionsWereVisible) {
			SHELLSTATE state = { 0 };
			state. fShowInfoTip = 0;
			state. fShowExtensions = /*1*/ extensionsWereVisible;
			SHGetSetSettings (& state, SSF_SHOWINFOTIP | SSF_SHOWEXTENSIONS, TRUE);
		}
		if (GetSaveFileNameW (& openFileName)) {
			if (my allowExecutionHook && ! my allowExecutionHook (my allowExecutionClosure)) {
				Melder_flushError ("Dialog `%s' cancelled.", my name);
				return;
			}
			Melder_pathToFile (fullFileName, & my file);
			if (! my okCallback ((UiForm) me, NULL, NULL, my okClosure)) {
				Melder_error3 (L"File ", MelderFile_messageName (& my file), L" not finished.");
				Melder_flushError (NULL);
			}
			UiHistory_write (L" ");
			UiHistory_write (Melder_fileToPath (& my file));
		}
		if (infoTipsWereVisible || ! extensionsWereVisible) {
			SHELLSTATE state = { 0 };
			state. fShowInfoTip = infoTipsWereVisible;
			state. fShowExtensions = extensionsWereVisible;
			SHGetSetSettings (& state, SSF_SHOWINFOTIP | SSF_SHOWEXTENSIONS, TRUE);
		}
	#else
		#if motif
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
		char defaultNameUtf8 [300];
		Melder_wcsTo8bitFileRepresentation_inline (defaultName, defaultNameUtf8);
		strcat (dirSpec, defaultNameUtf8);
		XtVaSetValues (my dialog, motif_argXmString (XmNdirSpec, dirSpec), NULL);
		XmStringFree (xmDirMask);
		XmStringFree (xmDirSpec);
		XtFree (dirMask);
		XtFree (dirSpecc);
		XtManageChild (my dialog);
		XMapRaised (XtDisplay (XtParent (my dialog)), XtWindow (XtParent (my dialog)));
		#endif
		currentUiFile = (UiFile) me;
	#endif
}

/* End of file UiFile.c */
