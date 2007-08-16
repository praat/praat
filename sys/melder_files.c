/* melder_files.c
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
 */

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
	#define PtoCstr(p)  (p [p [0] + 1] = '\0', (char *) p + 1)
	#define PfromCstr(p,c)  p [0] = strlen (c), strcpy ((char *) p + 1, c);
#endif
#include <errno.h>
#include "melder.h"
#include "flac_FLAC_stream_encoder.h"
#include "abcio.h"

#if defined (macintosh)
	#include <sys/stat.h>
	#define UNIX
	#include <unistd.h>
#endif

static wchar_t theShellDirectory [256];
void Melder_rememberShellDirectory (void) {
	structMelderDir shellDir = { { 0 } };
	Melder_getDefaultDir (& shellDir);
	wcscpy (theShellDirectory, Melder_dirToPathW (& shellDir));
}
wchar_t * Melder_getShellDirectory (void) {
	return & theShellDirectory [0];
}

void Melder_wcsTo8bitFileRepresentation_inline (const wchar_t *wcs, unsigned char *utf8) {
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
		UniChar unipath [260];
		long n = wcslen (wcs);
		for (long i = 0; i <= n; i ++) {
			unipath [i] = wcs [i];   // Including null byte. BUG: should split into genuine UTF-16.
		}
		CFStringRef cfpath = CFStringCreateWithCharacters (NULL, unipath, n);
		CFMutableStringRef cfpath2 = CFStringCreateMutableCopy (NULL, 0, cfpath);
		CFRelease (cfpath);
		CFStringNormalize (cfpath2, kCFStringNormalizationFormD);   // Mac requires decomposed characters.
		CFStringGetCString (cfpath2, (char *) utf8, 1000, kCFStringEncodingUTF8);   // Mac POSIX requires UTF-8.
		CFRelease (cfpath2);
	#else
		Melder_wcsToUtf8_inline (wcs, utf8);
	#endif
}

void Melder_8bitFileRepresentationToWcs_inline (const unsigned char *path, wchar_t *wpath) {
	#if defined (_WIN32)
		long n = strlen ((char *) path), i, j;
		for (i = 0, j = 0; i < n; i ++) {
			wpath [j ++] = path [i];
		}
		wpath [j] = '\0';
	#elif defined (macintosh)
		long n = strlen ((char *) path);
		CFStringRef cfpath = CFStringCreateWithCString (NULL, (char *) path, kCFStringEncodingUTF8);
		Melder_assert (cfpath != 0);
		CFMutableStringRef cfpath2 = CFStringCreateMutableCopy (NULL, 0, cfpath);
		CFRelease (cfpath);
		CFStringNormalize (cfpath2, kCFStringNormalizationFormC);   // Praat requires composed characters.
		n = CFStringGetLength (cfpath2);
		for (long i = 0; i < n; i ++) {
			wpath [i] = CFStringGetCharacterAtIndex (cfpath2, i);
		}
		wpath [n] = '\0';
		CFRelease (cfpath2);
	#else
		Melder_8bitToWcs_inline (path, wpath, Melder_INPUT_ENCODING_UTF8);
	#endif
}

#if defined (macintosh)
void Melder_machToFile (void *void_fsref, MelderFile file) {
	FSRef *fsref = (FSRef *) void_fsref;
	unsigned char path [1000];
	FSRefMakePath (fsref, path, 999);   // Decomposed UTF-8.
	Melder_8bitFileRepresentationToWcs_inline (path, file -> wpath);
}
static void Melder_machToDir (int vRefNum, long dirID, MelderDir dir) {
	FSSpec fspec;
	FSMakeFSSpec (vRefNum, dirID, NULL, & fspec);
	FSRef fsref;
	FSpMakeFSRef (& fspec, & fsref);
	unsigned char path [1000];
	FSRefMakePath (& fsref, path, 999);   // Decomposed UTF-8.
	Melder_8bitFileRepresentationToWcs_inline (path, dir -> wpath);
}
int Melder_fileToMach (MelderFile file, void *void_fsref) {
	unsigned char path [1000];
	Melder_wcsTo8bitFileRepresentation_inline (file -> wpath, path);
	OSStatus err = FSPathMakeRef (path, (FSRef *) void_fsref, NULL);
	if (err != noErr && err != fnfErr)
		return Melder_error5 (L"Error #", Melder_integer (err), L" translating file name ", file -> wpath, L".");
	return 1;
}
int Melder_fileToMac (MelderFile file, void *void_fspec) {
	unsigned char path [1000];
	Melder_wcsTo8bitFileRepresentation_inline (file -> wpath, path);
	FSRef fsref;
	OSStatus err = FSPathMakeRef (path, & fsref, NULL);
	if (err != noErr && err != fnfErr)
		return Melder_error5 (L"Error #", Melder_integer (err), L" translating file name ", file -> wpath, L".");
	FSSpec *fspec = (FSSpec *) void_fspec;
	err = FSGetCatalogInfo (& fsref, kFSCatInfoNone, NULL, NULL, fspec, NULL);
	if (err != noErr) {
		/*
			File does not exist. Get its parent directory instead.
		*/
		structMelderDir parentDir = { { 0 } };
		char romanName [260];
		CFStringRef unicodeName;
		Str255 pname;
		FSCatalogInfo info;
		FSRef parentDirectory;
		MelderFile_getParentDir (file, & parentDir);
		Melder_wcsTo8bitFileRepresentation_inline (parentDir. wpath, path);
		err = FSPathMakeRef (path, & parentDirectory, NULL);
		if (err != noErr)
			return Melder_error5 (L"Error #", Melder_integer (err), L" translating directory name ", parentDir. wpath, L".");
		err = FSGetCatalogInfo (& parentDirectory, kFSCatInfoVolume | kFSCatInfoNodeID, & info, NULL, NULL, NULL);
		if (err != noErr)
			return Melder_error5 (L"Error #", Melder_integer (err), L" looking for directory of ", file -> wpath, L".");
		/*
			Convert from UTF-8 to MacRoman.
		*/
		unicodeName = CFStringCreateWithCString (NULL, MelderFile_name (file), kCFStringEncodingUTF8);
		CFStringGetCString (unicodeName, romanName, 260, kCFStringEncodingMacRoman);
		CFRelease (unicodeName);
		PfromCstr (pname, romanName);
		err = FSMakeFSSpec (info. volume, info. nodeID, & pname [0], fspec);
		if (err != noErr && err != fnfErr)
			return Melder_error5 (L"Error #", Melder_integer (err), L" looking for file ", file -> wpath, L".");
	}
	return 1;
}
#endif

char * MelderFile_name (MelderFile file) {
	return Melder_peekWcsToAscii (MelderFile_nameW (file));
}

wchar_t * MelderFile_nameW (MelderFile file) {
	#if defined (UNIX)
		wchar_t *slash = wcsrchr (file -> wpath, '/');
		return slash ? slash + 1 : file -> wpath;
	#elif defined (_WIN32)
		wchar_t *backslash = wcsrchr (file -> wpath, '\\');
		return backslash ? backslash + 1 : file -> wpath;
	#endif
}

wchar_t * MelderDir_nameW (MelderDir dir) {
	#if defined (UNIX)
		wchar_t *slash = wcsrchr (dir -> wpath, '/');
		return slash ? slash + 1 : dir -> wpath;
	#elif defined (_WIN32)
		wchar_t *backslash = wcsrchr (dir -> wpath, '\\');
		return backslash ? backslash + 1 : dir -> wpath;
	#endif
}

int Melder_pathToDirW (const wchar_t *path, MelderDir dir) {
	wcscpy (dir -> wpath, path);
	return 1;
}

int Melder_pathToFile (const char *path, MelderFile file) {
	/*
	 * This handles complete path names only.
	 * Unlike Melder_relativePathToFile, this handles Windows file names with slashes in them.
	 *
	 * Used if we know for sure that we have a complete path name,
	 * i.e. if the program determined the name (fileselector, printing, prefs).
	 */
	Melder_8bitFileRepresentationToWcs_inline ((unsigned char *) path, file -> wpath);
	return 1;
}

int Melder_pathToFileW (const wchar_t *path, MelderFile file) {
	/*
	 * This handles complete path names only.
	 * Unlike Melder_relativePathToFile, this handles Windows file names with slashes in them.
	 *
	 * Used if we know for sure that we have a complete path name,
	 * i.e. if the program determined the name (fileselector, printing, prefs).
	 */
	wcscpy (file -> wpath, path);
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
			swprintf (file -> wpath, 256, L"%ls%ls", Melder_peekAsciiToWcs (getenv ("HOME")), & path [1]);
		} else if (path [0] == '/' || wcsequ (path, L"<stdout>") || wcsstr (path, L"://")) {
			wcscpy (file -> wpath, path);
		} else {
			structMelderDir dir = { { 0 } };
			Melder_getDefaultDir (& dir);   /* BUG */
			if (dir. wpath [0] == '/' && dir. wpath [1] == '\0') {
				wcscpy (file -> wpath, L"/");
				wcscat (file -> wpath, path);
			} else {
				wcscpy (file -> wpath, dir.wpath);
				wcscat (file -> wpath, L"/");
				wcscat (file -> wpath, path);
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
			wchar_t winPath [260];
			wcscpy (winPath, path);
			for (;;) {
				wchar_t *slash = wcschr (winPath, '/');
				if (slash == NULL) break;
				*slash = '\\';
			}
			return Melder_relativePathToFile (winPath, file);
		}
		if (wcschr (path, ':') || path [0] == '\\' && path [1] == '\\' || wcsequ (path, L"<stdout>")) {
			wcscpy (file -> wpath, path);
		} else {
			structMelderDir dir = { { 0 } };
			Melder_getDefaultDir (& dir);   /* BUG */
			if (dir. wpath [0] != '\0' && dir. wpath [wcslen (dir. wpath) - 1] == '\\')
				swprintf (file -> wpath, 256, L"%ls%ls", dir. wpath, path);
			else
				swprintf (file -> wpath, 256, L"%ls\\%ls", dir. wpath, path);
		}
	#endif
	return 1;
}

char * Melder_dirToPath (MelderDir dir) {
	#ifdef _WIN32
		return Melder_peekWcsToAscii (Melder_dirToPathW (dir));
	#else
		return Melder_peekWcsToUtf8 (Melder_dirToPathW (dir));
	#endif
}

wchar_t * Melder_dirToPathW (MelderDir dir) {
	return & dir -> wpath [0];
}

char * Melder_fileToPath (MelderFile file) {
	#ifdef _WIN32
		return Melder_peekWcsToAscii (Melder_fileToPathW (file));
	#else
		return Melder_peekWcsToUtf8 (Melder_fileToPathW (file));
	#endif
}

wchar_t * Melder_fileToPathW (MelderFile file) {
	return & file -> wpath [0];
}

void MelderFile_copy (MelderFile file, MelderFile copy) {
	wcscpy (copy -> wpath, file -> wpath);
}

void MelderDir_copy (MelderDir dir, MelderDir copy) {
	wcscpy (copy -> wpath, dir -> wpath);
}

int MelderFile_equal (MelderFile file1, MelderFile file2) {
	return wcsequ (file1 -> wpath, file2 -> wpath);
}

int MelderDir_equal (MelderDir dir1, MelderDir dir2) {
	return wcsequ (dir1 -> wpath, dir2 -> wpath);
}

void MelderFile_setToNull (MelderFile file) {
	file -> wpath [0] = '\0';
}

int MelderFile_isNull (MelderFile file) {
	return file -> wpath [0] == '\0';
}

void MelderDir_setToNull (MelderDir dir) {
	dir -> wpath [0] = '\0';
}

int MelderDir_isNull (MelderDir dir) {
	return dir -> wpath [0] == '\0';
}

void MelderDir_getFileW (MelderDir parent, const wchar_t *fileName, MelderFile file) {
	#if defined (UNIX)
		if (parent -> wpath [0] == '/' && parent -> wpath [1] == '\0') {
			swprintf (file -> wpath, 256, L"/%ls", fileName);
		} else {
			swprintf (file -> wpath, 256, L"%ls/%ls", parent -> wpath, fileName);
		}
	#elif defined (_WIN32)
		if (wcsrchr (file -> wpath, '\\') - file -> wpath == wcslen (file -> wpath) - 1) {
			swprintf (file -> wpath, 256, L"%ls%ls", parent -> wpath, fileName);
		} else {
			swprintf (file -> wpath, 256, L"%ls\\%ls", parent -> wpath, fileName);
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
	dir -> wpath [0] = '\0';
}
#endif

void MelderFile_getParentDir (MelderFile file, MelderDir parent) {
	#if defined (UNIX)
		/*
		 * The parent of /usr/hello.txt is /usr.
		 * The parent of /hello.txt is /.
		 */
		wchar_t *slash;
		wcscpy (parent -> wpath, file -> wpath);
		slash = wcsrchr (parent -> wpath, '/');
		if (slash) *slash = '\0';
		if (parent -> wpath [0] == '\0') wcscpy (parent -> wpath, L"/");
	#elif defined (_WIN32)
		/*
		 * The parent of C:\WINDOWS\CTRL.DLL is C:\WINDOWS.
		 * The parent of E:\Praat.exe is E:\.
		 * The parent of \\Swine\Apps\init.txt is \\Swine\Apps.
		 * The parent of \\Swine\init.txt is \\Swine\.   (BUG ?)
		 */
		wchar_t *colon;
		wcscpy (parent -> wpath, file -> wpath);
		colon = wcschr (parent -> wpath, ':');
		if (colon) {
			wchar_t *backslash = wcsrchr (parent -> wpath, '\\');
			if (backslash) {   /* C:\WINDOWS\CTRL.DLL or C:\AUTOEXEC.BAT */
				if (backslash - colon == 1) {   /* C:\AUTOEXEC.BAT */
					* (backslash + 1) = '\0';   /* C:\ */
				} else {   /* C:\WINDOWS\CTRL.DLL */
					*backslash = '\0';   /* C:\WINDOWS */
				}
			} else {   /* ??? */
				Melder_getDesktop (parent);   /* empty string */
			}
		} else if (parent -> wpath [0] == '\\' && parent -> wpath [1] == '\\') {
			wchar_t *backslash = wcsrchr (parent -> wpath + 2, '\\');
			if (backslash) {   /* \\Swine\Apps\init.txt or \\Swine\init.txt */
				wchar_t *leftBackslash = wcschr (parent -> wpath + 2, '\\');
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
		wcscpy (parent -> wpath, dir -> wpath);
		slash = wcsrchr (parent -> wpath, '/');
		if (slash) {
			if (slash - parent -> wpath == 0) {
				if (slash [1] == '\0') {   /* Child is "/". */
					parent -> wpath [0] = '\0';   /* Parent is "". */
				} else {   /* Child is "/usr". */
					slash [1] = '\0';   /* Parent is "/". */
				}
			} else {   /* Child is "/usr/local". */
				*slash = '\0';   /* Parent is "/usr". */
			}
		} else {
			parent -> wpath [0] = '\0';   /* Some failure. Desktop. */
		}
	#elif defined (_WIN32)
		/*
		 * The parent of C:\WINDOWS is C:\.
		 * The parent of E:\ is the desktop.
		 * The parent of \\Swine\ is the desktop.   (BUG ?)
		 */
		wchar_t *colon;
		wcscpy (parent -> wpath, dir -> wpath);
		colon = wcschr (parent -> wpath, ':');
		if (colon) {
			int length = wcslen (parent -> wpath);
			wchar_t *backslash = wcsrchr (parent -> wpath, '\\');
			if (backslash) {   /* C:\WINDOWS\FONTS or C:\WINDOWS or C:\ */
				if (backslash - parent -> wpath == length - 1) {   /* C:\ */
					Melder_getDesktop (parent);   /* empty string */
				} else if (backslash - colon == 1) {   /* C:\WINDOWS */
					* (backslash + 1) = '\0';   /* C:\ */
				} else {   /* C:\WINDOWS\FONTS */
					*backslash = '\0';   /* C:\WINDOWS */
				}
			} else {   /* LPT1:   ??? */
				Melder_getDesktop (parent);   /* empty string */
			}
		} else if (parent -> wpath [0] == '\\' && parent -> wpath [1] == '\\') {
			int length = wcslen (parent -> wpath);
			wchar_t *backslash = wcsrchr (parent -> wpath + 2, '\\');
			if (backslash) {   /* \\Swine\Apps\Praats or \\Swine\Apps or \\Swine\ */
				if (backslash - parent -> wpath == length - 1) {   /* \\Swine\ */
					Melder_getDesktop (parent);   /* empty string */
				} else {   /* \\Swine\Apps\Praats or \\Swine\Apps */
					wchar_t *leftBackslash = wcschr (parent -> wpath + 2, '\\');
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
	return dir -> wpath [0] == '\0';
}

int MelderDir_getSubdirW (MelderDir parent, const wchar_t *subdirName, MelderDir subdir) {
	#if defined (UNIX)
		if (parent -> wpath [0] == '/' && parent -> wpath [1] == '\0') {
			swprintf (subdir -> wpath, 260, L"/%ls", subdirName);
		} else {
			swprintf (subdir -> wpath, 260, L"%ls/%ls", parent -> wpath, subdirName);
		}
	#elif defined (_WIN32)
		int length = wcslen (parent -> wpath);
		wchar_t *backslash = wcsrchr (parent -> wpath, '\\');
		if (backslash && backslash - parent -> wpath == length - 1) {   /* C:\ or \\Swine\ */
			swprintf (subdir -> wpath, 260, L"%ls%ls", parent -> wpath, subdirName);
		} else {   /* C:\WINDOWS or \\Swine\Apps or even C: */
			swprintf (subdir -> wpath, 260, L"%ls\\%ls", parent -> wpath, subdirName);
		}
	#endif
	return 1;
}

void Melder_getHomeDir (MelderDir homeDir) {
	#if defined (UNIX)
		char *home = getenv ("HOME");
		wcscpy (homeDir -> wpath, home ? Melder_peekAsciiToWcs (home) : L"/");
	#elif defined (_WIN32)
		if (GetEnvironmentVariableW (L"USERPROFILE", homeDir -> wpath, 255)) {
			;   /* Ready. */
		} else if (GetEnvironmentVariableW (L"HOMEDRIVE", homeDir -> wpath, 255)) {
			GetEnvironmentVariableW (L"HOMEPATH", homeDir -> wpath + wcslen (homeDir -> wpath), 255 - wcslen (homeDir -> wpath));
		} else {
			MelderDir_setToNull (homeDir);   /* Windows 95 and 98: alas. */
		}
	#endif
}

void Melder_getPrefDir (MelderDir prefDir) {
	#if defined (macintosh)
		short vRefNum;
		long dirID;
		FindFolder (kOnSystemDisk, kPreferencesFolderType, kCreateFolder, & vRefNum, & dirID);
		Melder_machToDir (vRefNum, dirID, prefDir);
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
			GetWindowsDirectoryW (prefDir -> wpath, 255);
		}
	#endif
}

void Melder_getTempDir (MelderDir tempDir) {
	#if defined (macintosh)
		short vRefNum;
		long dirID;
		FindFolder (kOnSystemDisk, kTemporaryFolderType, kCreateFolder, & vRefNum, & dirID);
		Melder_machToDir (vRefNum, dirID, tempDir);
	#else
		(void) tempDir;
	#endif
}

#if defined (macintosh)

static long textCreator = 'PpgB';

void MelderFile_setMacTypeAndCreator (MelderFile file, long fileType, long creator) {
	if (wcsequ (file -> wpath, L"<stdout>")) return;
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
	unsigned char utf8path [1000];
	Melder_wcsTo8bitFileRepresentation_inline (file -> wpath, utf8path);
	FILE *f;
	file -> openForWriting = type [0] == 'w' || type [0] == 'a' || strchr (type, '+');
	if (wcsequ (file -> wpath, L"<stdout>") && file -> openForWriting) {
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
			f = _wfopen (file -> wpath, Melder_peekAsciiToWcs (type));
		#else
			f = fopen ((char *) utf8path, type);
		#endif
	}
	if (! f) {
		wchar_t *path = file -> wpath;
		#ifdef sgi
			Melder_error ("%s.", strerror (errno));
		#endif
		Melder_error5 (L"Cannot ", type [0] == 'r' ? L"open" : type [0] == 'a' ? L"append to" : L"create",
			L" file \"", MelderFile_messageNameW (file), L"\".");
		if (path [0] == '\0')
			Melder_error1 (L"Hint: empty file name.");
		else if (path [0] == ' ')
			Melder_error1 (L"Hint: file name starts with a space.");
		else if (path [wcslen (path) - 1] == ' ')
			Melder_error1 (L"Hint: file name ends in a space.");
		return NULL;
	}
	return f;
}

int Melder_fclose (MelderFile file, FILE *f) {
	if (! f) return 1;
	#if defined (CURLPRESENT)
 	if (wcsstr (file -> wpath, L"://") && file -> openForWriting) {
		unsigned char utf8path [1000];
		Melder_wcsTo8bitFileRepresentation_inline (file -> wpath, utf8path);
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
		if (strstr (file -> path, "file://") || strstr (file -> path, "FILE://")) {
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
		return Melder_error3 (L"Error closing file \"", MelderFile_messageNameW (file), L"\".");
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
		unsigned char utf8path [1000];
		Melder_wcsTo8bitFileRepresentation_inline (file -> wpath, utf8path);
		struct stat statistics;
		return ! stat ((char *) utf8path, & statistics);
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
		unsigned char utf8path [1000];
		Melder_wcsTo8bitFileRepresentation_inline (file -> wpath, utf8path);
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
	unsigned char utf8path [1000];
	Melder_wcsTo8bitFileRepresentation_inline (file -> wpath, utf8path);
	remove ((char *) utf8path);
	return 1;
}

char * Melder_asciiMessage (const char *message) {
	static char names [11] [300];
	static int index = 0;
	const char *from;
	char *to;
	if (++ index == 11) index = 0;
	for (from = & message [0], to = & names [index] [0]; *from != '\0'; from ++, to ++) {
		*to = *from;
		if (*from == '\\') { * ++ to = 'b'; * ++ to = 's'; }
	}
	*to = '\0';
	return & names [index] [0];
}

wchar_t * Melder_peekExpandBackslashes (const wchar_t *message) {
	static wchar_t names [11] [300];
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

char * MelderFile_messageName (MelderFile file) {
	return Melder_peekWcsToAscii (file -> wpath);
}

wchar_t * MelderFile_messageNameW (MelderFile file) {
	return file -> wpath;
}

void Melder_getDefaultDir (MelderDir dir) {
	unsigned char path [1000];
	getcwd ((char *) path, 1000);
	Melder_8bitFileRepresentationToWcs_inline (path, dir -> wpath);
}

void Melder_setDefaultDir (MelderDir dir) {
	chdir (Melder_peekWcsToAscii (dir -> wpath));
}

void MelderFile_setDefaultDir (MelderFile file) {
	structMelderDir dir = { { 0 } };
	MelderFile_getParentDir (file, & dir);
	Melder_setDefaultDir (& dir);
}

int Melder_createDirectoryW (MelderDir parent, const wchar_t *dirName, int mode) {
#if defined (_WIN32)
	structMelderFile file = { 0 };
	SECURITY_ATTRIBUTES sa;
	(void) mode;
	sa. nLength = sizeof (SECURITY_ATTRIBUTES);
	sa. lpSecurityDescriptor = NULL;
	sa. bInheritHandle = FALSE;
	swprintf (file. wpath, 260, L"%ls\\%ls", parent -> wpath, dirName);
	if (! CreateDirectoryW (file. wpath, & sa) && GetLastError () != ERROR_ALREADY_EXISTS)   /* Ignore if directory already exists. */
		return Melder_error3 (L"Cannot create directory \"", MelderFile_messageNameW (& file), L"\".");
	return 1;
#else
	structMelderFile file = { 0 };
	if (parent -> wpath [0] == '/' && parent -> wpath [1] == '\0') {
		swprintf (file. wpath, 260, L"/%ls", dirName);
	} else {
		swprintf (file. wpath, 260, L"%ls/%ls", parent -> wpath, dirName);
	}
	unsigned char utf8path [1000];
	Melder_wcsTo8bitFileRepresentation_inline (file. wpath, utf8path);
	if (mkdir ((char *) utf8path, mode) == -1 && errno != EEXIST)   /* Ignore if directory already exists. */
		return Melder_error3 (L"Cannot create directory \"", MelderFile_messageNameW (& file), L"\".");
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
		buffer = Melder_malloc (char, capacity = 100);
		if (buffer == NULL) {
			Melder_error ("(MelderFile_readLine:) No room even for a string buffer of 100 bytes.");
			fclose (my filePointer);
			my filePointer = NULL;
		}
	}
	for (i = 0; 1; i ++) {
		int c;
		if (i >= capacity && ! (buffer = Melder_realloc (buffer, capacity *= 2))) {
			Melder_error ("(MelderFile_readLine:) No memory to extend string buffer to %ld bytes.", capacity);
			/*
			 * If the buffer overflows the available memory,
			 * half the buffer will also be quite large!
			 * So shrink it.
			 */
			buffer = Melder_realloc (buffer, capacity = 100);
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
		Melder_error3 (L"Cannot seek in file ", MelderFile_messageNameW (me), L".");
		fclose (my filePointer);
		my filePointer = NULL;
	}
}

long MelderFile_tell (MelderFile me) {
	long result = 0;
	if (! my filePointer) return 0;
	if ((result = ftell (my filePointer)) == -1) {
		Melder_error3 (L"Cannot tell in file ", MelderFile_messageNameW (me), L".");
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
	if (my outputEncoding == Melder_OUTPUT_ENCODING_FLAC) {
		if (my flacEncoder) {
			FLAC__stream_encoder_finish (my flacEncoder);   // This already calls fclose! BUG: we cannot get any error messages out.
			FLAC__stream_encoder_delete (my flacEncoder);
		}
	} else if (my filePointer) {
		Melder_fclose (me, my filePointer);
	}
	/* Set everything to zero, except paths (they stay around for error messages and the like). */
	my filePointer = NULL;
	my openForWriting = my openForReading = false;
	my indent = 0;
	my flacEncoder = NULL;
}

/* End of file melder_files.c */
