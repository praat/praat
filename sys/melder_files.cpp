/* melder_files.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * rvs&pb 2002/03/07 url support
 * pb 2002/03/10 Mach compatibility
 * pb 2003/09/12 MelderFile_getMacType
 * pb 2003/09/14 MelderDir_relativePathToFile
 * pb 2004/09/25 use /tmp as temporary directory
 * pb 2004/10/16 C++ compatible structs
 * pb 2005/11/07 Windows: use %USERPROFILE% rather than %HOMESHARE%%HOMEPATH%
 * rvs&pb 2005/11/18 url support
 * pb 2006/01/21 MelderFile_writeText does not create temporary file
 * pb 2006/08/03 openForWriting
 * rvs 2006/08/12 curl: do not fail on error
 * pb 2006/08/12 check whether unicodeName exists
 * pb 2006/10/28 erased MacOS 9 stuff
 * Erez Volk 2007/05/14 FLAC support
 * pb 2007/05/28 wchar_t
 * pb 2007/06/09 more wchar_t
 * pb 2007/08/12 more wchar_t
 * pb 2007/10/05 FSFindFolder
 * pb 2008/11/01 warn after finding final tabs (not just spaces) in file names
 * pb 2010/12/14 more high Unicode compatibility
 * pb 2011/04/05 C++
 */

#include <cstdio>
using namespace std;

#if defined (UNIX) || defined __MWERKS__
	#include <unistd.h>
#endif
/*#if defined (HPUX)
	#define _INCLUDE_POSIX_SOURCE
	#define _INCLUDE_HPUX_SOURCE
	#define _INCLUDE_XOPEN_SOURCE
#endif*/
#if defined (UNIX)
	#include <sys/stat.h>
#endif
#if defined (CURLPRESENT)
	#include <curl/curl.h>
#endif
#ifdef _WIN32
	#include <windows.h>
#endif
#if defined (macintosh)
	#include "macport_on.h"
	#include <Folders.h>
	#include "macport_off.h"
#endif
#include <errno.h>
#include "melder.h"
#include "flac_FLAC_stream_encoder.h"
#include "abcio.h"

#if defined (macintosh)
	#include <sys/stat.h>
	#define UNIX
	#include <unistd.h>
	#include "UnicodeData.h"
#endif

static wchar_t theShellDirectory [kMelder_MAXPATH+1];
void Melder_rememberShellDirectory (void) {
	structMelderDir shellDir = { { 0 } };
	Melder_getDefaultDir (& shellDir);
	wcscpy (theShellDirectory, Melder_dirToPath (& shellDir));
}
wchar_t * Melder_getShellDirectory (void) {
	return & theShellDirectory [0];
}

void Melder_wcsTo8bitFileRepresentation_inline (const wchar_t *wcs, char *utf8) {
	#if defined (_WIN32)
		int n = wcslen (wcs), i, j;
		for (i = 0, j = 0; i < n; i ++) {
			utf8 [j ++] = wcs [i] <= 255 ? wcs [i] : '?';   // The usual replacement on Windows.
		}
		utf8 [j] = '\0';	
	#elif defined (macintosh)
		/*
			On the Mac, the POSIX path name is stored in canonically decomposed UTF-8 encoding.
			The path is probably in precomposed UTF-32.
			So we first convert to UTF-16, then turn into CFString, then decompose, then convert to UTF-8.
		*/
		UniChar unipath [kMelder_MAXPATH+1];
		long n = wcslen (wcs), n_utf16 = 0;
		for (long i = 0; i < n; i ++) {
			uint32_t kar = wcs [i];
			if (kar <= 0xFFFF) {
				unipath [n_utf16 ++] = kar;   // including null byte
			} else if (kar <= 0x10FFFF) {
				kar -= 0x10000;
				unipath [n_utf16 ++] = 0xD800 | (kar >> 10);
				unipath [n_utf16 ++] = 0xDC00 | (kar & 0x3FF);
			} else {
				unipath [n_utf16 ++] = UNICODE_REPLACEMENT_CHARACTER;
			}
		}
		unipath [n_utf16] = '\0';
		CFStringRef cfpath = CFStringCreateWithCharacters (NULL, unipath, n_utf16);
		CFMutableStringRef cfpath2 = CFStringCreateMutableCopy (NULL, 0, cfpath);
		CFRelease (cfpath);
		CFStringNormalize (cfpath2, kCFStringNormalizationFormD);   // Mac requires decomposed characters.
		CFStringGetCString (cfpath2, (char *) utf8, kMelder_MAXPATH+1, kCFStringEncodingUTF8);   // Mac POSIX requires UTF-8.
		CFRelease (cfpath2);
	#else
		Melder_wcsToUtf8_inline (wcs, utf8);
	#endif
}

#if ! defined (_WIN32)
void Melder_8bitFileRepresentationToWcs_inline (const char *path, wchar_t *wpath) {
	#if defined (macintosh)
		CFStringRef cfpath = CFStringCreateWithCString (NULL, path, kCFStringEncodingUTF8);
		Melder_assert (cfpath != 0);
		CFMutableStringRef cfpath2 = CFStringCreateMutableCopy (NULL, 0, cfpath);
		CFRelease (cfpath);
		CFStringNormalize (cfpath2, kCFStringNormalizationFormC);   // Praat requires composed characters.
		long n_utf16 = CFStringGetLength (cfpath2);
		long n_wcs = 0;
		for (long i = 0; i < n_utf16; i ++) {
			uint32_t kar = CFStringGetCharacterAtIndex (cfpath2, i);
			if (kar >= 0xD800 && kar <= 0xDBFF) {
				uint32_t kar2 = CFStringGetCharacterAtIndex (cfpath2, ++ i);
				if (kar2 >= 0xDC00 && kar2 <= 0xDFFF) {
					kar = (((kar & 0x3FF) << 10) | (kar2 & 0x3FF)) + 0x10000;
				} else {
					kar = UNICODE_REPLACEMENT_CHARACTER;
				}
			}
			wpath [n_wcs ++] = kar;
		}
		wpath [n_wcs] = '\0';
		CFRelease (cfpath2);
	#else
		Melder_8bitToWcs_inline_e (path, wpath, kMelder_textInputEncoding_UTF8);
	#endif
}
#endif

#if defined (macintosh)
void Melder_machToFile (void *void_fsref, MelderFile file) {
	FSRef *fsref = (FSRef *) void_fsref;
	char path [kMelder_MAXPATH+1];
	FSRefMakePath (fsref, (unsigned char *) path, kMelder_MAXPATH);   // Decomposed UTF-8.
	Melder_8bitFileRepresentationToWcs_inline (path, file -> path);
}
void Melder_machToDir (void *void_fsref, MelderDir dir) {
	FSRef *fsref = (FSRef *) void_fsref;
	char path [kMelder_MAXPATH+1];
	FSRefMakePath (fsref, (unsigned char *) path, kMelder_MAXPATH);   // Decomposed UTF-8.
	Melder_8bitFileRepresentationToWcs_inline (path, dir -> path);
}
int Melder_fileToMach (MelderFile file, void *void_fsref) {
	char path [kMelder_MAXPATH+1];
	Melder_wcsTo8bitFileRepresentation_inline (file -> path, path);
	OSStatus err = FSPathMakeRef ((unsigned char *) path, (FSRef *) void_fsref, NULL);
	if (err != noErr && err != fnfErr)
		return Melder_error5 (L"Error #", Melder_integer (err), L" translating file name ", file -> path, L".");
	return 1;
}
int Melder_dirToMach (MelderDir dir, void *void_fsref) {
	char path [kMelder_MAXPATH+1];
	Melder_wcsTo8bitFileRepresentation_inline (dir -> path, path);
	OSStatus err = FSPathMakeRef ((unsigned char *) path, (FSRef *) void_fsref, NULL);
	if (err != noErr && err != fnfErr)
		return Melder_error5 (L"Error #", Melder_integer (err), L" translating dir name ", dir -> path, L".");
	return 1;
}
#endif

wchar_t * MelderFile_name (MelderFile file) {
	#if defined (UNIX)
		wchar_t *slash = wcsrchr (file -> path, '/');
		return slash ? slash + 1 : file -> path;
	#elif defined (_WIN32)
		wchar_t *backslash = wcsrchr (file -> path, '\\');
		return backslash ? backslash + 1 : file -> path;
	#endif
}

wchar_t * MelderDir_name (MelderDir dir) {
	#if defined (UNIX)
		wchar_t *slash = wcsrchr (dir -> path, '/');
		return slash ? slash + 1 : dir -> path;
	#elif defined (_WIN32)
		wchar_t *backslash = wcsrchr (dir -> path, '\\');
		return backslash ? backslash + 1 : dir -> path;
	#endif
}

int Melder_pathToDir (const wchar_t *path, MelderDir dir) {
	wcscpy (dir -> path, path);
	return 1;
}

int Melder_pathToFile (const wchar_t *path, MelderFile file) {
	/*
	 * This handles complete path names only.
	 * Unlike Melder_relativePathToFile, this handles Windows file names with slashes in them.
	 *
	 * Used if we know for sure that we have a complete path name,
	 * i.e. if the program determined the name (fileselector, printing, prefs).
	 */
	wcscpy (file -> path, path);
	return 1;
}

int Melder_relativePathToFile (const wchar_t *path, MelderFile file) {
	/*
	 * This handles complete and partial path names,
	 * and translates slashes to native directory separators (unlike Melder_pathToFile).
	 *
	 * Used if we do not know for sure that we have a complete path name,
	 * i.e. if the user determined the name (scripting).
	 */
	#if defined (UNIX)
		/*
		 * We assume that Unix complete path names start with a slash.
		 */
		if (path [0] == '~' && path [1] == '/') {
			swprintf (file -> path, kMelder_MAXPATH+1, L"%ls%ls", Melder_peekUtf8ToWcs (getenv ("HOME")), & path [1]);
		} else if (path [0] == '/' || wcsequ (path, L"<stdout>") || wcsstr (path, L"://")) {
			wcscpy (file -> path, path);
		} else {
			structMelderDir dir = { { 0 } };
			Melder_getDefaultDir (& dir);   /* BUG */
			if (dir. path [0] == '/' && dir. path [1] == '\0') {
				wcscpy (file -> path, L"/");
				wcscat (file -> path, path);
			} else {
				wcscpy (file -> path, dir. path);
				wcscat (file -> path, L"/");
				wcscat (file -> path, path);
			}
		}
	#elif defined (_WIN32)
		/*
		 * We assume that Win32 complete path names look like:
		 *    C:\WINDOWS\CTRL32.DLL
		 *    LPT1:
		 *    \\host\path
		 */
		if (wcschr (path, '/') && ! wcsstr (path, L"://")) {
			wchar_t winPath [kMelder_MAXPATH+1];
			wcscpy (winPath, path);
			for (;;) {
				wchar_t *slash = wcschr (winPath, '/');
				if (slash == NULL) break;
				*slash = '\\';
			}
			return Melder_relativePathToFile (winPath, file);
		}
		if (wcschr (path, ':') || path [0] == '\\' && path [1] == '\\' || wcsequ (path, L"<stdout>")) {
			wcscpy (file -> path, path);
		} else {
			structMelderDir dir = { { 0 } };
			Melder_getDefaultDir (& dir);   /* BUG */
			static MelderString buffer = { 0 };
			MelderString_empty (& buffer);
			MelderString_append3 (& buffer,
				dir. path,
				dir. path [0] != '\0' && dir. path [wcslen (dir. path) - 1] == '\\' ? L"" : L"\\",
				path);
			wcscpy (file -> path, buffer.string);
		}
	#endif
	return 1;
}

wchar_t * Melder_dirToPath (MelderDir dir) {
	return & dir -> path [0];
}

wchar_t * Melder_fileToPath (MelderFile file) {
	return & file -> path [0];
}

void MelderFile_copy (MelderFile file, MelderFile copy) {
	wcscpy (copy -> path, file -> path);
}

void MelderDir_copy (MelderDir dir, MelderDir copy) {
	wcscpy (copy -> path, dir -> path);
}

int MelderFile_equal (MelderFile file1, MelderFile file2) {
	return wcsequ (file1 -> path, file2 -> path);
}

int MelderDir_equal (MelderDir dir1, MelderDir dir2) {
	return wcsequ (dir1 -> path, dir2 -> path);
}

void MelderFile_setToNull (MelderFile file) {
	file -> path [0] = '\0';
}

int MelderFile_isNull (MelderFile file) {
	return file -> path [0] == '\0';
}

void MelderDir_setToNull (MelderDir dir) {
	dir -> path [0] = '\0';
}

int MelderDir_isNull (MelderDir dir) {
	return dir -> path [0] == '\0';
}

void MelderDir_getFile (MelderDir parent, const wchar_t *fileName, MelderFile file) {
	#if defined (UNIX)
		if (parent -> path [0] == '/' && parent -> path [1] == '\0') {
			swprintf (file -> path, kMelder_MAXPATH+1, L"/%ls", fileName);
		} else {
			swprintf (file -> path, kMelder_MAXPATH+1, L"%ls/%ls", parent -> path, fileName);
		}
	#elif defined (_WIN32)
		if (wcsrchr (file -> path, '\\') - file -> path == wcslen (file -> path) - 1) {
			swprintf (file -> path, kMelder_MAXPATH+1, L"%ls%ls", parent -> path, fileName);
		} else {
			swprintf (file -> path, kMelder_MAXPATH+1, L"%ls\\%ls", parent -> path, fileName);
		}
	#endif
}

void MelderDir_relativePathToFile (MelderDir dir, const wchar_t *path, MelderFile file) {
	structMelderDir saveDir = { { 0 } };
	Melder_getDefaultDir (& saveDir);
	Melder_setDefaultDir (dir);
	Melder_relativePathToFile (path, file);
	Melder_setDefaultDir (& saveDir);
}

#ifndef UNIX
static void Melder_getDesktop (MelderDir dir) {
	dir -> path [0] = '\0';
}
#endif

void MelderFile_getParentDir (MelderFile file, MelderDir parent) {
	#if defined (UNIX)
		/*
		 * The parent of /usr/hello.txt is /usr.
		 * The parent of /hello.txt is /.
		 */
		wchar_t *slash;
		wcscpy (parent -> path, file -> path);
		slash = wcsrchr (parent -> path, '/');
		if (slash) *slash = '\0';
		if (parent -> path [0] == '\0') wcscpy (parent -> path, L"/");
	#elif defined (_WIN32)
		/*
		 * The parent of C:\WINDOWS\CTRL.DLL is C:\WINDOWS.
		 * The parent of E:\Praat.exe is E:\.
		 * The parent of \\Swine\Apps\init.txt is \\Swine\Apps.
		 * The parent of \\Swine\init.txt is \\Swine\.   (BUG ?)
		 */
		wchar_t *colon;
		wcscpy (parent -> path, file -> path);
		colon = wcschr (parent -> path, ':');
		if (colon) {
			wchar_t *backslash = wcsrchr (parent -> path, '\\');
			if (backslash) {   /* C:\WINDOWS\CTRL.DLL or C:\AUTOEXEC.BAT */
				if (backslash - colon == 1) {   /* C:\AUTOEXEC.BAT */
					* (backslash + 1) = '\0';   /* C:\ */
				} else {   /* C:\WINDOWS\CTRL.DLL */
					*backslash = '\0';   /* C:\WINDOWS */
				}
			} else {   /* ??? */
				Melder_getDesktop (parent);   /* empty string */
			}
		} else if (parent -> path [0] == '\\' && parent -> path [1] == '\\') {
			wchar_t *backslash = wcsrchr (parent -> path + 2, '\\');
			if (backslash) {   /* \\Swine\Apps\init.txt or \\Swine\init.txt */
				wchar_t *leftBackslash = wcschr (parent -> path + 2, '\\');
				if (backslash - leftBackslash == 0) {   /* \\Swine\init.txt */
					* (backslash + 1) = '\0';   /* \\Swine\ */
				} else {   /* \\Swine\Apps\init.txt */
					*backslash = '\0';   /* \\Swine\Apps */
				}
			} else {   /* \\init.txt   ??? */
				Melder_getDesktop (parent);   /* empty string */
			}
		} else {   /* Unknown path type. */
			Melder_getDesktop (parent);   /* empty string */
		}
	#endif
}

void MelderDir_getParentDir (MelderDir dir, MelderDir parent) {
	#if defined (UNIX)
		/*
		 * The parent of /usr/local is /usr.
		 * The parent of /usr is /.
		 * The parent of / is "".
		 */
		wchar_t *slash;
		wcscpy (parent -> path, dir -> path);
		slash = wcsrchr (parent -> path, '/');
		if (slash) {
			if (slash - parent -> path == 0) {
				if (slash [1] == '\0') {   /* Child is "/". */
					parent -> path [0] = '\0';   /* Parent is "". */
				} else {   /* Child is "/usr". */
					slash [1] = '\0';   /* Parent is "/". */
				}
			} else {   /* Child is "/usr/local". */
				*slash = '\0';   /* Parent is "/usr". */
			}
		} else {
			parent -> path [0] = '\0';   /* Some failure. Desktop. */
		}
	#elif defined (_WIN32)
		/*
		 * The parent of C:\WINDOWS is C:\.
		 * The parent of E:\ is the desktop.
		 * The parent of \\Swine\ is the desktop.   (BUG ?)
		 */
		wchar_t *colon;
		wcscpy (parent -> path, dir -> path);
		colon = wcschr (parent -> path, ':');
		if (colon) {
			int length = wcslen (parent -> path);
			wchar_t *backslash = wcsrchr (parent -> path, '\\');
			if (backslash) {   /* C:\WINDOWS\FONTS or C:\WINDOWS or C:\ */
				if (backslash - parent -> path == length - 1) {   /* C:\ */
					Melder_getDesktop (parent);   /* empty string */
				} else if (backslash - colon == 1) {   /* C:\WINDOWS */
					* (backslash + 1) = '\0';   /* C:\ */
				} else {   /* C:\WINDOWS\FONTS */
					*backslash = '\0';   /* C:\WINDOWS */
				}
			} else {   /* LPT1:   ??? */
				Melder_getDesktop (parent);   /* empty string */
			}
		} else if (parent -> path [0] == '\\' && parent -> path [1] == '\\') {
			int length = wcslen (parent -> path);
			wchar_t *backslash = wcsrchr (parent -> path + 2, '\\');
			if (backslash) {   /* \\Swine\Apps\Praats or \\Swine\Apps or \\Swine\ */
				if (backslash - parent -> path == length - 1) {   /* \\Swine\ */
					Melder_getDesktop (parent);   /* empty string */
				} else {   /* \\Swine\Apps\Praats or \\Swine\Apps */
					wchar_t *leftBackslash = wcschr (parent -> path + 2, '\\');
					if (backslash - leftBackslash == 0) {   /* \\Swine\Apps */
						* (backslash + 1) = '\0';   /* \\Swine\ */
					} else {   /* \\Swine\Apps\Praats */
						*backslash = '\0';   /* \\Swine\Apps */
					}
				}
			} else {   /* \\Swine   ??? */
				Melder_getDesktop (parent);   /* empty string */
			}
		} else {   /* Unknown path type. */
			Melder_getDesktop (parent);   /* empty string */
		}
	#endif
}

int MelderDir_isDesktop (MelderDir dir) {
	return dir -> path [0] == '\0';
}

int MelderDir_getSubdir (MelderDir parent, const wchar_t *subdirName, MelderDir subdir) {
	#if defined (UNIX)
		if (parent -> path [0] == '/' && parent -> path [1] == '\0') {
			swprintf (subdir -> path, kMelder_MAXPATH+1, L"/%ls", subdirName);
		} else {
			swprintf (subdir -> path, kMelder_MAXPATH+1, L"%ls/%ls", parent -> path, subdirName);
		}
	#elif defined (_WIN32)
		int length = wcslen (parent -> path);
		wchar_t *backslash = wcsrchr (parent -> path, '\\');
		if (backslash && backslash - parent -> path == length - 1) {   /* C:\ or \\Swine\ */
			swprintf (subdir -> path, kMelder_MAXPATH+1, L"%ls%ls", parent -> path, subdirName);
		} else {   /* C:\WINDOWS or \\Swine\Apps or even C: */
			swprintf (subdir -> path, kMelder_MAXPATH+1, L"%ls\\%ls", parent -> path, subdirName);
		}
	#endif
	return 1;
}

void Melder_getHomeDir (MelderDir homeDir) {
	#if defined (UNIX)
		char *home = getenv ("HOME");
		wcscpy (homeDir -> path, home ? Melder_peekUtf8ToWcs (home) : L"/");
	#elif defined (_WIN32)
		if (GetEnvironmentVariable (L"USERPROFILE", homeDir -> path, kMelder_MAXPATH)) {
			;   /* Ready. */
		} else if (GetEnvironmentVariableW (L"HOMEDRIVE", homeDir -> path, kMelder_MAXPATH)) {
			GetEnvironmentVariable (L"HOMEPATH", homeDir -> path + wcslen (homeDir -> path), kMelder_MAXPATH - wcslen (homeDir -> path));
		} else {
			MelderDir_setToNull (homeDir);   /* Windows 95 and 98: alas. */
		}
	#endif
}

void Melder_getPrefDir (MelderDir prefDir) {
	#if defined (macintosh)
		FSRef macFileReference;
		FSFindFolder (kOnSystemDisk, kPreferencesFolderType, kCreateFolder, & macFileReference);
		Melder_machToDir (& macFileReference, prefDir);
	#elif defined (UNIX)
		/*
		 * Preferences files go into the home directory.
		 */
		Melder_getHomeDir (prefDir);
	#elif defined (_WIN32)
		/*
		 * On shared systems (NT, 2000, XP), preferences files go into the home directory.
		 * Otherwise (Windows 95 and 98), they go into the Windows directory.
		 */
		Melder_getHomeDir (prefDir);
		if (MelderDir_isNull (prefDir)) {
			GetWindowsDirectoryW (prefDir -> path, kMelder_MAXPATH);
		}
	#endif
}

void Melder_getTempDir (MelderDir tempDir) {
	#if defined (macintosh)
		FSRef macFileReference;
		FSFindFolder (kOnSystemDisk, kTemporaryFolderType, kCreateFolder, & macFileReference);
		Melder_machToDir (& macFileReference, tempDir);
	#else
		(void) tempDir;
	#endif
}

#if defined (macintosh)

static long textCreator = 'PpgB';

void MelderFile_setMacTypeAndCreator (MelderFile file, long fileType, long creator) {
	if (wcsequ (file -> path, L"<stdout>")) return;
	OSStatus err;
	FSRef fsref;
	FSCatalogInfo info;
	Melder_fileToMach (file, & fsref);
	err = FSGetCatalogInfo (& fsref, kFSCatInfoFinderInfo, & info, NULL, NULL, NULL);
	if (err == noErr) {
		((FInfo *) & info. finderInfo) -> fdType = fileType;
		((FInfo *) & info. finderInfo) -> fdCreator = creator ? creator : textCreator;
		FSSetCatalogInfo (& fsref, kFSCatInfoFinderInfo, & info);
	}
}
unsigned long MelderFile_getMacType (MelderFile file) {
	OSStatus err;
	FSRef fsref;
	FSCatalogInfo info;
	Melder_fileToMach (file, & fsref);
	err = FSGetCatalogInfo (& fsref, kFSCatInfoFinderInfo, & info, NULL, NULL, NULL);
	if (err == noErr) {
		return ((FInfo *) & info. finderInfo) -> fdType;
	}
	return 0;
}
#endif

#ifdef CURLPRESENT
static int curl_initialized = 0;
static size_t write_URL_data_to_file (void *buffer, size_t size, size_t nmemb, void *userp) {
	return fwrite (buffer, size, nmemb, userp);
}
static size_t read_URL_data_from_file (void *buffer, size_t size, size_t nmemb, void *userp) {
	return fread (buffer, size, nmemb, userp);
}
#endif

FILE * Melder_fopen (MelderFile file, const char *type) {
	/*
	 * On the Unix-like systems (including MacOS), the path has to be converted to 8-bit characters in UTF-8 encoding.
	 * On MacOS, the characters also have to be decomposed.
	 */
	char utf8path [kMelder_MAXPATH+1];
	Melder_wcsTo8bitFileRepresentation_inline (file -> path, utf8path);
	FILE *f;
	file -> openForWriting = type [0] == 'w' || type [0] == 'a' || strchr (type, '+');
	if (wcsequ (file -> path, L"<stdout>") && file -> openForWriting) {
		f = stdout;
	#ifdef CURLPRESENT
	} else if (strstr (utf8path, "://") && file -> openForWriting) {
		Melder_assert (type [0] == 'w');   /* Reject "append" and "random" access. */
		f = tmpfile ();   /* Open a temporary file for writing. */
	} else if (strstr (utf8path, "://") && ! file -> openForWriting) {
		CURLcode CURLreturn;
		CURL *CURLhandle;
		char errorbuffer [CURL_ERROR_SIZE] = "";
		f = tmpfile ();   /* Open a temporary file for writing. */
		if (! curl_initialized) {
			CURLreturn = curl_global_init (CURL_GLOBAL_ALL);
			curl_initialized = 1;
		};
		CURLhandle = curl_easy_init ();   /* Initialize session. */
		/* 
		 * Set up the connection parameters.
		 */
		/* Debugging: Verbose messages */
		/* CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_VERBOSE, 1); */
		/* Do not fail on error. */
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_FAILONERROR, 0);	
		/* Store error messages in a buffer. */
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_ERRORBUFFER, errorbuffer);
		/* The file stream to store the URL. */
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_FILE, f);
		/* The function to write to the file, necessary for Win32.	*/
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_WRITEFUNCTION, write_URL_data_to_file);
		/* The actual URL to handle.	*/
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_URL, utf8path);
		/* Get the URL and write it to the given file. */
		CURLreturn = curl_easy_perform (CURLhandle);
		/* Handle errors. */
		if (CURLreturn) {
			Melder_error ("%s\n", errorbuffer);
			f = NULL;
		};
		/* Clean up session. */
		curl_easy_cleanup (CURLhandle);
		/* Do something with the file. Why? */
		if (f) rewind (f);
	#endif
	} else {
		#ifdef _WIN32
			f = _wfopen (file -> path, Melder_peekUtf8ToWcs (type));
		#else
			f = fopen ((char *) utf8path, type);
		#endif
	}
	if (! f) {
		wchar_t *path = file -> path;
		#ifdef sgi
			Melder_error ("%s.", strerror (errno));
		#endif
		Melder_error5 (L"Cannot ", type [0] == 'r' ? L"open" : type [0] == 'a' ? L"append to" : L"create",
			L" file ", MelderFile_messageName (file), L".");
		if (path [0] == '\0')
			Melder_error1 (L"Hint: empty file name.");
		else if (path [0] == ' ' || path [0] == '\t')
			Melder_error1 (L"Hint: file name starts with a space or tab.");
		else if (path [wcslen (path) - 1] == ' ' || path [wcslen (path) - 1] == '\t')
			Melder_error1 (L"Hint: file name ends in a space or tab.");
		else if (wcschr (path, '\n'))
			Melder_error1 (L"Hint: file name contains a newline symbol.");
		return NULL;
	}
	return f;
}

int Melder_fclose (MelderFile file, FILE *f) {
	if (! f) return 1;
	#if defined (CURLPRESENT)
 	if (wcsstr (file -> wpath, L"://") && file -> openForWriting) {
		unsigned char utf8path [kMelder_MAXPATH+1];
		Melder_wcsTo8bitFileRepresentation_inline (file -> path, utf8path);
		/* Rewind the file. */
		if (f) rewind (f);
		CURLcode CURLreturn;
		CURL *CURLhandle;
		char errorbuffer [CURL_ERROR_SIZE] = "";
		/* Start global init (necessary only ONCE). */
		if (! curl_initialized) {
			CURLreturn = curl_global_init (CURL_GLOBAL_ALL);
			curl_initialized = 1;
		};
		CURLhandle = curl_easy_init ();   /* Initialize session. */
		/* 
		 * Set up the connection parameters.
		 */
		/* Debugging: Verbose messages */
		/* CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_VERBOSE, 1); */
        /* Catch FILE: protocol errors. No solution yet */
		if (wcsstr (file -> path, L"file://") || wcsstr (file -> path, L"FILE://")) {
			CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_VERBOSE, 1);
		}
		/* Do not return Error pages, just fail. */
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_FAILONERROR, 1);	
		/* Store error messages in a buffer. */
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_ERRORBUFFER, errorbuffer);
		/* Send header. */
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_HEADER, 1);
		/* Upload. */
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_UPLOAD, 1);
		/* The actual URL to handle. */
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_URL, utf8path);
		/* The function to write to the peer, necessary for Win32. */
	    CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_READFUNCTION, read_URL_data_from_file);
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_READDATA, f);
		/* Get the URL and write the file to it. */
		CURLreturn = curl_easy_perform (CURLhandle);
		/* Handle errors. */
		if (CURLreturn) {
			Melder_error ("%s\n", errorbuffer);
			f = NULL;
	    };
		/* Clean up session */
		curl_easy_cleanup (CURLhandle);
    }
	#endif
	if (f != stdout && fclose (f) == EOF) {
		#ifdef sgi
			Melder_error ("%s", strerror (errno));
		#endif
		return Melder_error3 (L"Error closing file ", MelderFile_messageName (file), L".");
	}
	return 1;
}

void Melder_files_cleanUp (void) {
	#if defined (CURLPRESENT)
		if (curl_initialized) {
			curl_global_cleanup ();
			curl_initialized = 0;
		};
	#endif
}

int MelderFile_exists (MelderFile file) {
	#if defined (UNIX)
		char utf8path [kMelder_MAXPATH+1];
		Melder_wcsTo8bitFileRepresentation_inline (file -> path, utf8path);
		struct stat statistics;
		return ! stat (utf8path, & statistics);
	#else
		FILE *f = Melder_fopen (file, "rb");
		return f ? (fclose (f), TRUE) : (Melder_clearError (), FALSE);
	#endif
}

int MelderFile_readable (MelderFile file) {
	FILE *f = Melder_fopen (file, "r");
	if (! f) { Melder_clearError (); return FALSE; }
	fclose (f);
	return TRUE;
}

long MelderFile_length (MelderFile file) {
	#if defined (UNIX)
		char utf8path [kMelder_MAXPATH+1];
		Melder_wcsTo8bitFileRepresentation_inline (file -> path, utf8path);
		struct stat statistics;
		if (stat ((char *) utf8path, & statistics)) return -1;
		return statistics. st_size;
	#else
		FILE *f = Melder_fopen (file, "r");
		long length;
		if (! f) { Melder_clearError (); return -1; }
		fseek (f, 0, SEEK_END);
		length = ftell (f);
		fclose (f);
		return length;
	#endif
}

int MelderFile_delete (MelderFile file) {
	if (! file) return 1;
	#if defined (_WIN32)
		DeleteFile (file -> path);
	#else
		char utf8path [kMelder_MAXPATH+1];
		Melder_wcsTo8bitFileRepresentation_inline (file -> path, utf8path);
		remove ((char *) utf8path);
	#endif
	return 1;
}

wchar_t * Melder_peekExpandBackslashes (const wchar_t *message) {
	static wchar_t names [11] [kMelder_MAXPATH+1];
	static int index = 0;
	const wchar_t *from;
	wchar_t *to;
	if (++ index == 11) index = 0;
	for (from = & message [0], to = & names [index] [0]; *from != '\0'; from ++, to ++) {
		*to = *from;
		if (*from == '\\') { * ++ to = 'b'; * ++ to = 's'; }
	}
	*to = '\0';
	return & names [index] [0];
}

wchar_t * MelderFile_messageName (MelderFile file) {
	return file -> path;
}

void Melder_getDefaultDir (MelderDir dir) {
	#if defined (_WIN32)
		GetCurrentDirectory (kMelder_MAXPATH, dir -> path);
	#else
		char path [kMelder_MAXPATH+1];
		getcwd (path, kMelder_MAXPATH+1);
		Melder_8bitFileRepresentationToWcs_inline (path, dir -> path);
	#endif
}

void Melder_setDefaultDir (MelderDir dir) {
	#if defined (_WIN32)
		SetCurrentDirectory (dir -> path);
	#else
		chdir (Melder_peekWcsToUtf8 (dir -> path));
	#endif
}

void MelderFile_setDefaultDir (MelderFile file) {
	structMelderDir dir = { { 0 } };
	MelderFile_getParentDir (file, & dir);
	Melder_setDefaultDir (& dir);
}

int Melder_createDirectory (MelderDir parent, const wchar_t *dirName, int mode) {
#if defined (_WIN32)
	structMelderFile file = { 0 };
	SECURITY_ATTRIBUTES sa;
	(void) mode;
	sa. nLength = sizeof (SECURITY_ATTRIBUTES);
	sa. lpSecurityDescriptor = NULL;
	sa. bInheritHandle = FALSE;
	if (wcschr (dirName, ':') || dirName [0] == '/' && dirName [1] == '/') {
		swprintf (file. path, kMelder_MAXPATH+1, L"%ls", dirName);   // absolute path
	} else {
		swprintf (file. path, kMelder_MAXPATH+1, L"%ls/%ls", parent -> path, dirName);   // relative path
	}
	if (! CreateDirectoryW (file. path, & sa) && GetLastError () != ERROR_ALREADY_EXISTS)   /* Ignore if directory already exists. */
		return Melder_error3 (L"Cannot create directory ", MelderFile_messageName (& file), L".");
	return 1;
#else
	structMelderFile file = { 0 };
	if (dirName [0] == '/') {
		swprintf (file. path, kMelder_MAXPATH+1, L"%ls", dirName);   // absolute path
	} else if (parent -> path [0] == '/' && parent -> path [1] == '\0') {
		swprintf (file. path, kMelder_MAXPATH+1, L"/%ls", dirName);   // relative path in root directory
	} else {
		swprintf (file. path, kMelder_MAXPATH+1, L"%ls/%ls", parent -> path, dirName);   // relative path
	}
	char utf8path [kMelder_MAXPATH+1];
	Melder_wcsTo8bitFileRepresentation_inline (file. path, utf8path);
	if (mkdir (utf8path, mode) == -1 && errno != EEXIST)   /* Ignore if directory already exists. */
		return Melder_error3 (L"Cannot create directory ", MelderFile_messageName (& file), L".");
	return 1;
#endif
}

/*
 * Routines for wrapping the file pointers.
 */

#define my  me ->

void MelderFile_open (MelderFile me) {
	my filePointer = Melder_fopen (me, "rb");
	if (my filePointer == NULL) return;
	my openForReading = true;
}

char * MelderFile_readLine (MelderFile me) {
	long i;
	static char *buffer;
	static long capacity;
	if (! my filePointer) return NULL;
	if (feof (my filePointer)) return NULL;
	if (! buffer) {
		buffer = Melder_malloc_e (char, capacity = 100);
		if (buffer == NULL) {
			Melder_error1 (L"(MelderFile_readLine:) No room even for a string buffer of 100 bytes.");
			fclose (my filePointer);
			my filePointer = NULL;
		}
	}
	for (i = 0; 1; i ++) {
		int c;
		if (i >= capacity && ! (buffer = (char *) Melder_realloc_e (buffer, capacity *= 2))) {
			Melder_error ("(MelderFile_readLine:) No memory to extend string buffer to %ld bytes.", capacity);
			/*
			 * If the buffer overflows the available memory,
			 * half the buffer will also be quite large!
			 * So shrink it.
			 */
			buffer = (char *) Melder_realloc_f (buffer, capacity = 100);
			fclose (my filePointer);
			my filePointer = NULL;
			return NULL;
		}
		c = fgetc (my filePointer);
		if (feof (my filePointer))
			break;
		if (c == '\n') {
			c = fgetc (my filePointer);
			if (feof (my filePointer)) break;   /* Ignore last empty line (Unix). */
			ungetc (c, my filePointer);
			break;   /* Unix line separator. */
		}
		if (c == '\r') {
			c = fgetc (my filePointer);
			if (feof (my filePointer)) break;   /* Ignore last empty line (Macintosh). */
			if (c == '\n') {
				c = fgetc (my filePointer);
				if (feof (my filePointer)) break;   /* Ignore last empty line (Windows). */
				ungetc (c, my filePointer);
				break;   /* Windows line separator. */
			}
			ungetc (c, my filePointer);
			break;   /* Macintosh line separator. */
		}
		buffer [i] = c;
	}
	buffer [i] = '\0';
	return buffer;
}

void MelderFile_create (MelderFile me, const wchar_t *macType, const wchar_t *macCreator, const wchar_t *winExtension) {
	my filePointer = Melder_fopen (me, "wb");
	if (! my filePointer) return;
	my openForWriting = true;   // A bit superfluous (will have been set by Melder_fopen).
	if (my filePointer == stdout) return;
	#if defined (macintosh)
		(void) winExtension;
	{
		unsigned long macType_int = macType == NULL || macType [0] == '\0' ? 0 :
			((unsigned int) macType [0] << 24) | ((unsigned int) macType [1] << 16) |
			((unsigned int) macType [2] << 8) | (unsigned int) macType [3];
		unsigned long macCreator_int = macCreator == NULL || macCreator [0] == '\0' ? 0 :
			((unsigned int) macCreator [0] << 24) | ((unsigned int) macCreator [1] << 16) |
			((unsigned int) macCreator [2] << 8) | (unsigned int) macCreator [3];
		MelderFile_setMacTypeAndCreator (me, macType_int, macCreator_int);
	}
	#elif defined (_WIN32)
		(void) macType;
		(void) macCreator;
		(void) winExtension;   /* BUG */
	#else
		(void) macType;
		(void) macCreator;
		(void) winExtension;
	#endif
}

void MelderFile_seek (MelderFile me, long position, int direction) {
	if (! my filePointer) return;
	if (fseek (my filePointer, position, direction)) {
		Melder_error3 (L"Cannot seek in file ", MelderFile_messageName (me), L".");
		fclose (my filePointer);
		my filePointer = NULL;
	}
}

long MelderFile_tell (MelderFile me) {
	long result = 0;
	if (! my filePointer) return 0;
	if ((result = ftell (my filePointer)) == -1) {
		Melder_error3 (L"Cannot tell in file ", MelderFile_messageName (me), L".");
		fclose (my filePointer);
		my filePointer = NULL;
	}
	return result;
}

void MelderFile_rewind (MelderFile me) {
	if (! my filePointer) return;
	rewind (my filePointer);
}

void MelderFile_close (MelderFile me) {
	if (my outputEncoding == kMelder_textOutputEncoding_FLAC) {
		if (my flacEncoder) {
			FLAC__stream_encoder_finish (my flacEncoder);   // This already calls fclose! BUG: we cannot get any error messages out.
			FLAC__stream_encoder_delete (my flacEncoder);
		}
	} else if (my filePointer != NULL) {
		Melder_fclose (me, my filePointer);
	}
	/* Set everything to zero, except paths (they stay around for error messages and the like). */
	my filePointer = NULL;
	my openForWriting = my openForReading = false;
	my indent = 0;
	my flacEncoder = NULL;
}

/* End of file melder_files.cpp */
