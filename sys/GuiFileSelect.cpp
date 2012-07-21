/* GuiFileSelect.cpp
 *
 * Copyright (C) 2010-2011,2012 Paul Boersma
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
 * pb 2010/07/26 split off from UiFile.c
 * pb 2010/11/27 GuiFileSelect_getDirectoryName ()
 * pb 2011/04/06 C++
 * pb 2011/07/05 C++
 * pb 2012/07/08 GTK's file opener shows working directory (on first time) or last-used directory (later)
 */

#include "Gui.h"
#ifdef _WIN32
	#include <Shlobj.h>
#endif

SortedSetOfString GuiFileSelect_getInfileNames (GuiObject parent, const wchar *title, bool allowMultipleFiles) {
	autoSortedSetOfString me = SortedSetOfString_create ();
	#if gtk
		(void) parent;
		static structMelderDir dir;
		GuiObject dialog = gtk_file_chooser_dialog_new (Melder_peekWcsToUtf8 (title), NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
		gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), allowMultipleFiles);
		if (MelderDir_isNull (& dir))   // first time?
			Melder_getDefaultDir (& dir);
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), Melder_peekWcsToUtf8 (Melder_dirToPath (& dir)));
		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
			char *infolderName_utf8 = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (dialog));
			if (infolderName_utf8 != NULL) {
				wchar_t *infolderName = Melder_peekUtf8ToWcs (infolderName_utf8);   // dangle
				Melder_pathToDir (infolderName, & dir);
				g_free (infolderName_utf8);
			}
			GSList *infileNames_list = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (dialog));
			for (GSList *element = infileNames_list; element != NULL; element = g_slist_next (element)) {
				char *infileName_utf8 = (char *) element -> data;
				my addString (Melder_peekUtf8ToWcs (infileName_utf8));
				g_free (infileName_utf8);
			}
			g_slist_free (infileNames_list);
		}
		gtk_widget_destroy (GTK_WIDGET (dialog));
	#elif defined (macintosh)
		(void) parent;
		OSStatus err;
		NavDialogRef dialogRef;
		NavDialogCreationOptions dialogOptions;
		NavGetDefaultDialogCreationOptions (& dialogOptions);
		dialogOptions. optionFlags |= kNavDontAutoTranslate;
		//dialogOptions. windowTitle = (CFStringRef) Melder_peekWcsToCfstring (title);
		if (! allowMultipleFiles) dialogOptions. optionFlags &= ~ kNavAllowMultipleFiles;
		err = NavCreateChooseFileDialog (& dialogOptions, NULL, NULL, NULL, NULL, NULL, & dialogRef);
		if (err == noErr) {
			NavReplyRecord reply;
			[(NSOpenPanel *) dialogRef setTitle: (NSString *) Melder_peekWcsToCfstring (title)];
			NavDialogRun (dialogRef);
			err = NavDialogGetReply (dialogRef, & reply);
			if (err == noErr && reply. validRecord) {
				long numberOfSelectedFiles;
				AECountItems (& reply. selection, & numberOfSelectedFiles);
				for (int ifile = 1; ifile <= numberOfSelectedFiles; ifile ++) {
					AEKeyword keyWord;
					DescType typeCode;
					Size actualSize = 0;
					FSRef machFile;
					structMelderFile file;
					if ((err = AEGetNthPtr (& reply. selection, ifile, typeFSRef, & keyWord, & typeCode, & machFile, sizeof (FSRef), & actualSize)) == noErr)
						Melder_machToFile (& machFile, & file);
					my addString (Melder_fileToPath (& file));
				}
				NavDisposeReply (& reply);
			}
			NavDialogDispose (dialogRef);
		}
	#elif defined (_WIN32)
		static OPENFILENAMEW openFileName, dummy;
		static wchar_t fullFileName [3000+2];
		ZeroMemory (& openFileName, sizeof (OPENFILENAMEW));
		openFileName. lStructSize = sizeof (OPENFILENAMEW);
		openFileName. hwndOwner = parent ? (HWND) XtWindow (parent) : NULL;
		openFileName. hInstance = NULL;
		openFileName. lpstrFilter = L"All Files\0*.*\0";
		ZeroMemory (fullFileName, (3000+2) * sizeof (wchar_t));
		openFileName. lpstrCustomFilter = NULL;
		openFileName. nMaxCustFilter = 0;
		openFileName. lpstrFile = fullFileName;
		openFileName. nMaxFile = 3000;
		openFileName. lpstrFileTitle = NULL;
		openFileName. nMaxFileTitle = 0;
		openFileName. lpstrInitialDir = NULL;
		openFileName. lpstrTitle = title;
		openFileName. Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY
			| (allowMultipleFiles ? OFN_ALLOWMULTISELECT : 0);
		openFileName. lpstrDefExt = NULL;
		openFileName. lpfnHook = NULL;
		openFileName. lpTemplateName = NULL;
		openFileName. pvReserved = NULL;
		openFileName. dwReserved = 0;
		openFileName. FlagsEx = 0;
		OSVERSIONINFO osVersionInfo;
		ZeroMemory (& osVersionInfo, sizeof (OSVERSIONINFO));
		osVersionInfo. dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		GetVersionEx (& osVersionInfo);
		if (GetOpenFileNameW (& openFileName)) {
			int firstFileNameLength = wcslen (fullFileName);
			if (fullFileName [firstFileNameLength + 1] == '\0') {
				/*
				 * The user selected one file.
				 */
				my addString (fullFileName);
			} else {
				/*
				 * The user selected multiple files.
				 * 'fullFileName' is a directory name; the file names follow.
				 */
				structMelderDir dir;
				Melder_pathToDir (fullFileName, & dir);
				for (const wchar_t *p = & fullFileName [firstFileNameLength + 1]; *p != '\0'; p += wcslen (p) + 1) {
					structMelderFile file;
					MelderDir_getFile (& dir, p, & file);
					my addString (Melder_fileToPath (& file));
				}
			}
		}
	#endif
	return me.transfer();
}

wchar * GuiFileSelect_getOutfileName (GuiObject parent, const wchar *title, const wchar *defaultName) {
	wchar *outfileName = NULL;
	#if gtk
		(void) parent;
		static structMelderFile file;
		GuiObject dialog = gtk_file_chooser_dialog_new (Melder_peekWcsToUtf8 (title), NULL, GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
		if (file. path [0] != '\0') {
			gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), Melder_peekWcsToUtf8 (file. path));
		}
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), Melder_peekWcsToUtf8 (defaultName));
		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
			char *outfileName_utf8 = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			outfileName = Melder_peekUtf8ToWcs (outfileName_utf8);
			g_free (outfileName_utf8);
			Melder_pathToFile (outfileName, & file);
		}
		gtk_widget_destroy (GTK_WIDGET (dialog));
	#elif defined (macintosh)
		(void) parent;
		const wchar *lastSlash = wcsrchr (defaultName, Melder_DIRECTORY_SEPARATOR);
		OSStatus err;
		NavDialogRef dialogRef;
		NavDialogCreationOptions dialogOptions;
		NavGetDefaultDialogCreationOptions (& dialogOptions);
		dialogOptions. windowTitle = (CFStringRef) Melder_peekWcsToCfstring (title);
		//dialogOptions. message = (CFStringRef) Melder_peekWcsToCfstring (title);
		dialogOptions. saveFileName = (CFStringRef) Melder_peekWcsToCfstring (lastSlash ? lastSlash + 1 : defaultName);
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
					CFStringRef outfileName_cf = NavDialogGetSaveFileName (dialogRef);   // "Get", therefore it's not ours.
					/*
					 * machFile contains the directory as e.g. "/" or "/Users/jane"; in the latter (most usual) case, append a slash.
					 */
					char directoryPath_utf8 [1000];
					FSRefMakePath (& machFile, (unsigned char *) directoryPath_utf8, 999);
					if (! (directoryPath_utf8 [0] == '/' && directoryPath_utf8 [1] == '\0'))
						strcat (directoryPath_utf8, "/");
					structMelderFile file;
					Melder_8bitToWcs_inline (directoryPath_utf8, file. path, kMelder_textInputEncoding_UTF8); // BUG throwable
					int dirLength = wcslen (file. path);
					int n = CFStringGetLength (outfileName_cf);
					wchar_t *p = file. path + dirLength;
					for (int i = 0; i < n; i ++, p ++)
						*p = CFStringGetCharacterAtIndex (outfileName_cf, i);
					*p = '\0';
					outfileName = Melder_wcsdup_f (file. path);
				}
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
		openFileName. hwndOwner = parent ? (HWND) XtWindow (parent) : NULL;
		openFileName. lpstrFilter = NULL;   /* like *.txt */
		openFileName. lpstrCustomFilter = customFilter;
		openFileName. nMaxCustFilter = 100;
		openFileName. lpstrFile = fullFileName;
		openFileName. nMaxFile = 300;
		openFileName. lpstrFileTitle = NULL;
		openFileName. lpstrInitialDir = NULL;
		openFileName. lpstrTitle = title;
		openFileName. Flags = OFN_LONGNAMES | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_HIDEREADONLY;
		openFileName. lpstrDefExt = NULL;
		if (GetSaveFileNameW (& openFileName)) {
			outfileName = Melder_wcsdup_f (fullFileName);
		}
	#endif
	return outfileName;
}

wchar * GuiFileSelect_getDirectoryName (GuiObject parent, const wchar *title) {
	wchar *directoryName = NULL;
	#if gtk
		(void) parent;
		static structMelderFile file;
		GuiObject dialog = gtk_file_chooser_dialog_new (Melder_peekWcsToUtf8 (title), NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, "Choose", GTK_RESPONSE_ACCEPT, NULL);
		if (file. path [0] != '\0') {
			gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), Melder_peekWcsToUtf8 (file. path));
		}
		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
			char *directoryName_utf8 = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			directoryName = Melder_utf8ToWcs (directoryName_utf8);
			g_free (directoryName_utf8);
			Melder_pathToFile (directoryName, & file);
		}
		gtk_widget_destroy (GTK_WIDGET (dialog));
	#elif defined (macintosh)
		(void) parent;
		OSStatus err;
		NavDialogRef dialogRef;
		NavDialogCreationOptions dialogOptions;
		NavGetDefaultDialogCreationOptions (& dialogOptions);
		dialogOptions. windowTitle = (CFStringRef) Melder_peekWcsToCfstring (title);
		dialogOptions. optionFlags |= kNavDontAutoTranslate;
		dialogOptions. optionFlags &= ~ kNavAllowMultipleFiles;
		err = NavCreateChooseFolderDialog (& dialogOptions, NULL, NULL, NULL, & dialogRef);
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
				structMelderFile file;
				if ((err = AEGetNthPtr (& reply. selection, 1, typeFSRef, & keyWord, & typeCode, & machFile, sizeof (FSRef), & actualSize)) == noErr) {
					Melder_machToFile (& machFile, & file);
					directoryName = Melder_wcsdup_f (Melder_fileToPath (& file));
				}
				NavDisposeReply (& reply);
			}
			NavDialogDispose (dialogRef);
		}
	#elif defined (_WIN32)
		static wchar fullFileName [3000+2];
		static bool comInited = false;
		if (! comInited) {
			CoInitializeEx (NULL, COINIT_APARTMENTTHREADED);
			comInited = true;
		}
		static BROWSEINFO info;
		info. hwndOwner = parent ? (HWND) XtWindow (parent) : NULL;
		info. ulFlags = BIF_USENEWUI;
		info. pidlRoot = NULL;   // everything on the computer should be browsable
		info. pszDisplayName = NULL;   // this would only give the bare directory name, not the full path
		info. lpszTitle = title;
		LPITEMIDLIST idList = SHBrowseForFolder (& info);
		SHGetPathFromIDList (idList, fullFileName);
		CoTaskMemFree (idList);
		directoryName = Melder_wcsdup_f (fullFileName);
	#endif
	return directoryName;
}

/* End of file GuiFileSelect.cpp */
