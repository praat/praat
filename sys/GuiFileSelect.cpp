/* GuiFileSelect.cpp
 *
 * Copyright (C) 2010-2012,2013,2014,2015 Paul Boersma, 2013 Tom Naughton
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

#include "GuiP.h"
#include <locale.h>
#ifdef _WIN32
	#include <Shlobj.h>
#endif

SortedSetOfString GuiFileSelect_getInfileNames (GuiWindow parent, const char32 *title, bool allowMultipleFiles) {
	structMelderDir saveDir = { { 0 } };
	Melder_getDefaultDir (& saveDir);
	autoSortedSetOfString me = SortedSetOfString_create ();
	#if gtk
		(void) parent;
		static structMelderDir dir;
		GuiObject dialog = gtk_file_chooser_dialog_new (Melder_peek32to8 (title), NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
		gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), allowMultipleFiles);
		if (MelderDir_isNull (& dir))   // first time?
			Melder_getDefaultDir (& dir);
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), Melder_peek32to8 (Melder_dirToPath (& dir)));
		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
			char *infolderName_utf8 = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (dialog));
			if (infolderName_utf8 != NULL) {
				char32 *infolderName = Melder_peek8to32 (infolderName_utf8);   // dangle
				Melder_pathToDir (infolderName, & dir);
				g_free (infolderName_utf8);
			}
			GSList *infileNames_list = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (dialog));
			for (GSList *element = infileNames_list; element != NULL; element = g_slist_next (element)) {
				char *infileName_utf8 = (char *) element -> data;
				SortedSetOfString_addString (me.peek(), Melder_peek8to32 (infileName_utf8));
				g_free (infileName_utf8);
			}
			g_slist_free (infileNames_list);
		}
		gtk_widget_destroy (GTK_WIDGET (dialog));
		setlocale (LC_ALL, "C");
	#elif cocoa
		(void) parent;
		NSOpenPanel	*openPanel = [NSOpenPanel openPanel];
		[openPanel setTitle: [NSString stringWithUTF8String: Melder_peek32to8 (title)]];
		[openPanel setAllowsMultipleSelection: allowMultipleFiles];
		[openPanel setCanChooseDirectories: NO];
		if ([openPanel runModal] == NSFileHandlingPanelOKButton) {
			for (NSURL *url in [openPanel URLs]) {
				structMelderFile file = { 0 };
				Melder_8bitFileRepresentationToStr32_inline ([[url path] UTF8String], file. path);   // BUG: unsafe buffer
				SortedSetOfString_addString (me.peek(), file. path);
			}
		}
		setlocale (LC_ALL, "en_US");
	#elif mac
		(void) parent;
		OSStatus err;
		NavDialogRef dialogRef;
		NavDialogCreationOptions dialogOptions;
		NavGetDefaultDialogCreationOptions (& dialogOptions);
		dialogOptions. optionFlags |= kNavDontAutoTranslate;
		//dialogOptions. windowTitle = (CFStringRef) Melder_peek32toCfstring (title);
		if (! allowMultipleFiles) dialogOptions. optionFlags &= ~ kNavAllowMultipleFiles;
		err = NavCreateChooseFileDialog (& dialogOptions, NULL, NULL, NULL, NULL, NULL, & dialogRef);
		if (err == noErr) {
			NavReplyRecord reply;
			[(NSOpenPanel *) dialogRef setTitle: (NSString *) Melder_peek32toCfstring (title)];
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
					structMelderFile file = { 0 };
					if ((err = AEGetNthPtr (& reply. selection, ifile, typeFSRef, & keyWord, & typeCode, & machFile, sizeof (FSRef), & actualSize)) == noErr)
						Melder_machToFile (& machFile, & file);
					SortedSetOfString_addString (me.peek(), Melder_fileToPath (& file));
				}
				NavDisposeReply (& reply);
			}
			NavDialogDispose (dialogRef);
		}
		setlocale (LC_ALL, "en_US");
	#elif win
		static OPENFILENAMEW openFileName, dummy;
		static WCHAR fullFileNameW [3000+2];
		ZeroMemory (& openFileName, sizeof (OPENFILENAMEW));
		openFileName. lStructSize = sizeof (OPENFILENAMEW);
		openFileName. hwndOwner = parent && parent -> d_xmShell ? (HWND) XtWindow (parent -> d_xmShell) : NULL;
		openFileName. hInstance = NULL;
		openFileName. lpstrFilter = L"All Files\0*.*\0";
		ZeroMemory (fullFileNameW, (3000+2) * sizeof (WCHAR));
		openFileName. lpstrCustomFilter = NULL;
		openFileName. nMaxCustFilter = 0;
		openFileName. lpstrFile = fullFileNameW;
		openFileName. nMaxFile = 3000;
		openFileName. lpstrFileTitle = NULL;
		openFileName. nMaxFileTitle = 0;
		openFileName. lpstrInitialDir = NULL;
		openFileName. lpstrTitle = Melder_peek32toW (title);
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
			int firstFileNameLength = wcslen (fullFileNameW);
			if (fullFileNameW [firstFileNameLength + 1] == L'\0') {
				/*
				 * The user selected one file.
				 */
				SortedSetOfString_addString (me.peek(), Melder_peekWto32 (fullFileNameW));
			} else {
				/*
				 * The user selected multiple files.
				 * 'fullFileNameW' is a directory name; the file names follow.
				 */
				structMelderDir dir;
				Melder_pathToDir (Melder_peekWto32 (fullFileNameW), & dir);
				for (const WCHAR *p = & fullFileNameW [firstFileNameLength + 1]; *p != L'\0'; p += wcslen (p) + 1) {
					structMelderFile file { 0 };
					MelderDir_getFile (& dir, Melder_peekWto32 (p), & file);
					SortedSetOfString_addString (me.peek(), Melder_fileToPath (& file));
				}
			}
		}
		setlocale (LC_ALL, "C");
	#endif
	Melder_setDefaultDir (& saveDir);
	return me.transfer();
}

char32 * GuiFileSelect_getOutfileName (GuiWindow parent, const char32 *title, const char32 *defaultName) {
	structMelderDir saveDir { { 0 } };
	Melder_getDefaultDir (& saveDir);
	char32 *outfileName = NULL;
	#if gtk
		(void) parent;
		static structMelderFile file;
		GuiObject dialog = gtk_file_chooser_dialog_new (Melder_peek32to8 (title), NULL, GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
		if (file. path [0] != '\0') {
			gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), Melder_peek32to8 (file. path));
		}
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), Melder_peek32to8 (defaultName));
		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
			char *outfileName_utf8 = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			outfileName = Melder_8to32 (outfileName_utf8);
			g_free (outfileName_utf8);
			Melder_pathToFile (outfileName, & file);
		}
		gtk_widget_destroy (GTK_WIDGET (dialog));
		setlocale (LC_ALL, "C");
	#elif cocoa
		(void) parent;
		NSSavePanel	*savePanel = [NSSavePanel savePanel];
		[savePanel setTitle: [NSString stringWithUTF8String: Melder_peek32to8 (title)]];
		//[savePanel setNameFieldStringValue: [NSString stringWithUTF8String: Melder_peek32to8 (defaultName)]];   // from 10.6 on
		if ([savePanel runModalForDirectory: nil
			           file: [NSString stringWithUTF8String: Melder_peek32to8 (defaultName)]   // deprecated 10.6 but needed 10.5
			] == NSFileHandlingPanelOKButton)
		{
			NSString *path = [[savePanel URL] path];
			if (path == nil)
				Melder_throw (U"Don't understand where you want to save (1).");
			const char *outfileName_utf8 = [path UTF8String];
			if (outfileName_utf8 == nullptr)
				Melder_throw (U"Don't understand where you want to save (2).");
			structMelderFile file { 0 };
			Melder_8bitFileRepresentationToStr32_inline (outfileName_utf8, file. path);   // BUG: unsafe buffer
			outfileName = Melder_dup (file. path);
		}
		setlocale (LC_ALL, "en_US");
	#elif mac
		(void) parent;
		const char32 *lastSlash = str32rchr (defaultName, Melder_DIRECTORY_SEPARATOR);
		OSStatus err;
		NavDialogRef dialogRef;
		NavDialogCreationOptions dialogOptions;
		NavGetDefaultDialogCreationOptions (& dialogOptions);
		dialogOptions. windowTitle = (CFStringRef) Melder_peek32toCfstring (title);
		//dialogOptions. message = (CFStringRef) Melder_peek32toCfstring (title);
		dialogOptions. saveFileName = (CFStringRef) Melder_peek32toCfstring (lastSlash ? lastSlash + 1 : defaultName);
		dialogOptions. optionFlags |= kNavNoTypePopup;
		err = NavCreatePutFileDialog (& dialogOptions, 0, 0, NULL, NULL, & dialogRef);
		if (err == noErr) {
			NavReplyRecord reply;
			NavDialogRun (dialogRef);
			err = NavDialogGetReply (dialogRef, & reply);
			if (Melder_debug == 19) {
				Melder_casual (U"err ", err, U" ", reply. validRecord);
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
					structMelderFile file = { 0 };
					Melder_8to32_inline (directoryPath_utf8, file. path, kMelder_textInputEncoding_UTF8);   // BUG throwable
					int dirLength = str32len (file. path);
					int n = CFStringGetLength (outfileName_cf);
					char32 *p = file. path + dirLength;
					for (int i = 0; i < n; i ++, p ++)
						*p = CFStringGetCharacterAtIndex (outfileName_cf, i);
					*p = '\0';
					outfileName = Melder_dup (file. path);
				}
				NavDisposeReply (& reply);
			}
			NavDialogDispose (dialogRef);
		}
		setlocale (LC_ALL, "en_US");
	#elif win
		OPENFILENAMEW openFileName;
		static WCHAR customFilter [100+2];
		static WCHAR fullFileNameW [300+2];
		wcsncpy (fullFileNameW, Melder_peek32toW (defaultName), 300+2);
		fullFileNameW [300+1] = L'\0';
		openFileName. lStructSize = sizeof (OPENFILENAMEW);
		openFileName. hwndOwner = parent && parent -> d_xmShell ? (HWND) XtWindow (parent -> d_xmShell) : NULL;
		openFileName. lpstrFilter = NULL;   // like *.txt
		openFileName. lpstrCustomFilter = customFilter;
		openFileName. nMaxCustFilter = 100;
		openFileName. lpstrFile = fullFileNameW;
		openFileName. nMaxFile = 300;
		openFileName. lpstrFileTitle = NULL;
		openFileName. lpstrInitialDir = NULL;
		openFileName. lpstrTitle = Melder_peek32toW (title);
		openFileName. Flags = OFN_LONGNAMES | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_HIDEREADONLY;
		openFileName. lpstrDefExt = NULL;
		if (GetSaveFileNameW (& openFileName)) {
			outfileName = Melder_Wto32 (fullFileNameW);
		}
		setlocale (LC_ALL, "C");
	#endif
	Melder_setDefaultDir (& saveDir);
	return outfileName;
}

char32 * GuiFileSelect_getDirectoryName (GuiWindow parent, const char32 *title) {
	structMelderDir saveDir = { { 0 } };
	Melder_getDefaultDir (& saveDir);
	char32 *directoryName = NULL;
	#if gtk
		(void) parent;
		static structMelderFile file;
		GuiObject dialog = gtk_file_chooser_dialog_new (Melder_peek32to8 (title), NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, "Choose", GTK_RESPONSE_ACCEPT, NULL);
		if (file. path [0] != '\0') {
			gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), Melder_peek32to8 (file. path));
		}
		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
			char *directoryName_utf8 = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			directoryName = Melder_8to32 (directoryName_utf8);
			g_free (directoryName_utf8);
			Melder_pathToFile (directoryName, & file);
		}
		gtk_widget_destroy (GTK_WIDGET (dialog));
		setlocale (LC_ALL, "C");
	#elif cocoa
		(void) parent;
		NSOpenPanel	*openPanel = [NSOpenPanel openPanel];
		[openPanel setTitle: [NSString stringWithUTF8String: Melder_peek32to8 (title)]];
		[openPanel setAllowsMultipleSelection: NO];
		[openPanel setCanChooseDirectories: YES];
		[openPanel setCanChooseFiles: NO];
		[openPanel setPrompt: @"Choose"];
		if ([openPanel runModal] == NSFileHandlingPanelOKButton) {
			for (NSURL *url in [openPanel URLs]) {
				const char *directoryName_utf8 = [[url path] UTF8String];
				structMelderDir dir = { { 0 } };
				Melder_8bitFileRepresentationToStr32_inline (directoryName_utf8, dir. path);   // BUG: unsafe buffer
				directoryName = Melder_dup (dir. path);
			}
		}
		setlocale (LC_ALL, "en_US");
	#elif mac
		(void) parent;
		OSStatus err;
		NavDialogRef dialogRef;
		NavDialogCreationOptions dialogOptions;
		NavGetDefaultDialogCreationOptions (& dialogOptions);
		dialogOptions. windowTitle = (CFStringRef) Melder_peek32toCfstring (title);
		dialogOptions. optionFlags |= kNavDontAutoTranslate;
		dialogOptions. optionFlags &= ~ kNavAllowMultipleFiles;
		err = NavCreateChooseFolderDialog (& dialogOptions, NULL, NULL, NULL, & dialogRef);
		if (err == noErr) {
			NavReplyRecord reply;
			NavDialogRun (dialogRef);
			err = NavDialogGetReply (dialogRef, & reply);
			if (Melder_debug == 19) {
				Melder_casual (U"err ", err, U" ", reply. validRecord);
			}
			if (err == noErr && reply. validRecord) {
				AEKeyword keyWord;
				DescType typeCode;
				Size actualSize = 0;
				FSRef machFile;
				structMelderFile file = { 0 };
				if ((err = AEGetNthPtr (& reply. selection, 1, typeFSRef, & keyWord, & typeCode, & machFile, sizeof (FSRef), & actualSize)) == noErr) {
					Melder_machToFile (& machFile, & file);
					directoryName = Melder_dup (Melder_fileToPath (& file));
				}
				NavDisposeReply (& reply);
			}
			NavDialogDispose (dialogRef);
		}
		setlocale (LC_ALL, "en_US");
	#elif win
		static WCHAR fullFileNameW [3000+2];
		static bool comInited = false;
		if (! comInited) {
			CoInitializeEx (NULL, COINIT_APARTMENTTHREADED);
			comInited = true;
		}
		static BROWSEINFO info;
		info. hwndOwner = parent && parent -> d_xmShell ? (HWND) XtWindow (parent -> d_xmShell) : NULL;
		info. ulFlags = BIF_USENEWUI;
		info. pidlRoot = NULL;   // everything on the computer should be browsable
		info. pszDisplayName = NULL;   // this would only give the bare directory name, not the full path
		info. lpszTitle = Melder_peek32toW (title);
		LPITEMIDLIST idList = SHBrowseForFolder (& info);
		SHGetPathFromIDList (idList, fullFileNameW);
		CoTaskMemFree (idList);
		directoryName = Melder_Wto32 (fullFileNameW);
		setlocale (LC_ALL, "C");
	#endif
	Melder_setDefaultDir (& saveDir);
	return directoryName;
}

/* End of file GuiFileSelect.cpp */
