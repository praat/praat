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

static char theShellDirectory [256];
void Melder_rememberShellDirectory (void) {
	structMelderDir shellDir = { { 0 } };
	Melder_getDefaultDir (& shellDir);
	strcpy (theShellDirectory, Melder_dirToPath (& shellDir));
}
char * Melder_getShellDirectory (void) {
	return & theShellDirectory [0];
}

static void copyPathToWpath (const char *path, wchar_t *wpath) {
	#if defined (_WIN32)
		int n = strlen (path), i, j;
		for (i = 0, j = 0; i < n; i ++) {
			wpath [j ++] = path [i];
		}
		wpath [j] = '\0';	
	#else
		Melder_utf8ToWcs_inline (path, wpath);
	#endif
}

static void copyWpathToPath (const wchar_t *wpath, char *path) {
	#ifdef _WIN32
		int n = wcslen (wpath), i, j;
		for (i = 0, j = 0; i < n; i ++) {
			path [j ++] = wpath [i] <= 255 ? wpath [i] : '?';   // The usual replacement on Windows.
		}
		path [j] = '\0';	
	#else
		Melder_wcsToUtf8_inline (wpath, path);
	#endif
}

#if defined (macintosh)
void Melder_machToFile (void *void_fsref, MelderFile file) {
	FSRef *fsref = (FSRef *) void_fsref;
	FSRefMakePath (fsref, (unsigned char *) file -> path, 259);   // UTF-8
	#if 0
		CFStringRef string = CFStringCreateWithCString (NULL, file -> path, kCFStringEncodingUTF8);
		long n = CFStringGetLength (string);
		for (int i = 0; i < n; i ++) {
			file -> wpath [i] = CFStringGetCharacterAtIndex (string, i);
		}
		CFRelease (string);
	#else
		copyPathToWpath (file -> path, file -> wpath);
	#endif
}
static void Melder_machToDir (int vRefNum, long dirID, MelderDir dir) {
	FSSpec fspec;
	FSRef fsref;
	FSMakeFSSpec (vRefNum, dirID, NULL, & fspec);
	FSpMakeFSRef (& fspec, & fsref);
	FSRefMakePath (& fsref, (unsigned char *) dir -> path, 259);   // UTF-8
	CFStringRef string = CFStringCreateWithCString (NULL, dir -> path, kCFStringEncodingUTF8);
	long n = CFStringGetLength (string);
	for (int i = 0; i < n; i ++) {
		dir -> wpath [i] = CFStringGetCharacterAtIndex (string, i);
	}
	CFRelease (string);
}
int Melder_fileToMach (MelderFile file, void *void_fsref) {
	OSStatus err = FSPathMakeRef ((const unsigned char *) file -> path, (FSRef *) void_fsref, NULL);
	if (err != noErr && err != fnfErr)
		return Melder_error ("Error #%d translating file name %s.", err, file -> path);
	return 1;
}
int Melder_fileToMac (MelderFile file, void *void_fspec) {
	FSSpec *fspec = (FSSpec *) void_fspec;
	FSRef fsref;
	OSStatus err = FSPathMakeRef ((const unsigned char *) file -> path, & fsref, NULL);
	if (err != noErr && err != fnfErr)
		return Melder_error ("Error #%d translating file name %s.", err, file -> path);
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
		err = FSPathMakeRef ((const unsigned char *) parentDir. path, & parentDirectory, NULL);
		if (err != noErr)
			return Melder_error ("Error #%d translating directory name %s.", err, parentDir. path);
		err = FSGetCatalogInfo (& parentDirectory, kFSCatInfoVolume | kFSCatInfoNodeID, & info, NULL, NULL, NULL);
		if (err != noErr)
			return Melder_error ("Error #%d looking for directory of %s.", err, file -> path);
		/*
			Convert from UTF-8 to MacRoman.
		*/
		unicodeName = CFStringCreateWithCString (NULL, MelderFile_name (file), kCFStringEncodingUTF8);
		CFStringGetCString (unicodeName, romanName, 260, kCFStringEncodingMacRoman);
		CFRelease (unicodeName);
		PfromCstr (pname, romanName);
		err = FSMakeFSSpec (info. volume, info. nodeID, & pname [0], fspec);
		if (err != noErr && err != fnfErr)
			return Melder_error ("Error #%d looking for file %s.", err, file -> path);
	}
	return 1;
}
#endif

char * MelderFile_name (MelderFile file) {
	#if defined (UNIX)
		char *slash = strrchr (file -> path, '/');
		return slash ? slash + 1 : file -> path;
	#elif defined (_WIN32)
		char *backslash = strrchr (file -> path, '\\');
		return backslash ? backslash + 1 : file -> path;
	#endif
}

char * MelderDir_name (MelderDir dir) {
	#if defined (UNIX)
		char *slash = strrchr (dir -> path, '/');
		return slash ? slash + 1 : dir -> path;
	#elif defined (_WIN32)
		char *backslash = strrchr (dir -> path, '\\');
		return backslash ? backslash + 1 : dir -> path;
	#endif
}

int Melder_pathToDir (const char *path, MelderDir dir) {
	strcpy (dir -> path, path);
	copyPathToWpath (dir -> path, dir -> wpath);
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
	strcpy (file -> path, path);
	copyPathToWpath (file -> path, file -> wpath);
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
	copyWpathToPath (file -> wpath, file -> path);
	return 1;
}

int Melder_relativePathToFile (const char *path, MelderFile file) {
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
			sprintf (file -> path, "%s%s", getenv ("HOME"), & path [1]);
		} else if (path [0] == '/' || strequ (path, "<stdout>") || strstr (path, "://")) {
			strcpy (file -> path, path);
		} else {
			structMelderDir dir = { { 0 } };
			char path2 [256];
			strcpy (path2, path);
			MelderFile_nativizePath (path2);
			Melder_getDefaultDir (& dir);   /* BUG */
			if (dir. path [0] == '/' && dir. path [1] == '\0') {
				sprintf (file -> path, "/%s", path2);
			} else {
				sprintf (file -> path, "%s/%s", dir. path, path2);
			}
		}
	#elif defined (_WIN32)
		/*
		 * We assume that Win32 complete path names look like:
		 *    C:\WINDOWS\CTRL32.DLL
		 *    LPT1:
		 *    \\host\path
		 */
		if (strchr (path, '/') && ! strstr (path, "://")) {
			char winPath [260];
			strcpy (winPath, path);
			for (;;) {
				char *slash = strchr (winPath, '/');
				if (slash == NULL) break;
				*slash = '\\';
			}
			return Melder_relativePathToFile (winPath, file);
		}
		if (strchr (path, ':') || path [0] == '\\' && path [1] == '\\' || strequ (path, "<stdout>")) {
			strcpy (file -> path, path);
		} else {
			structMelderDir dir = { { 0 } };
			Melder_getDefaultDir (& dir);   /* BUG */
			if (dir. path [0] != '\0' && dir. path [strlen (dir.path) - 1] == '\\')
				sprintf (file -> path, "%s%s", dir. path, path);
			else
				sprintf (file -> path, "%s\\%s", dir. path, path);
		}
	#endif
	copyPathToWpath (file -> path, file -> wpath);
	return 1;
}

char * Melder_dirToPath (MelderDir dir) {
	return & dir -> path [0];
}

char * Melder_fileToPath (MelderFile file) {
	return & file -> path [0];
}

void MelderFile_copy (MelderFile file, MelderFile copy) {
	strcpy (copy -> path, file -> path);
	wcscpy (copy -> wpath, file -> wpath);
}

void MelderDir_copy (MelderDir dir, MelderDir copy) {
	strcpy (copy -> path, dir -> path);
	wcscpy (copy -> wpath, dir -> wpath);
}

int MelderFile_equal (MelderFile file1, MelderFile file2) {
	return wcsequ (file1 -> wpath, file2 -> wpath);
}

int MelderDir_equal (MelderDir dir1, MelderDir dir2) {
	return wcsequ (dir1 -> wpath, dir2 -> wpath);
}

void MelderFile_setToNull (MelderFile file) {
	file -> path [0] = '\0';
	file -> wpath [0] = '\0';
}

int MelderFile_isNull (MelderFile file) {
	return file -> wpath [0] == '\0';
}

void MelderDir_setToNull (MelderDir dir) {
	dir -> path [0] = '\0';
	dir -> wpath [0] = '\0';
}

int MelderDir_isNull (MelderDir dir) {
	return dir -> wpath [0] == '\0';
}

void MelderDir_getFile (MelderDir parent, const char *fileName, MelderFile file) {
	#if defined (UNIX)
		if (parent -> path [0] == '/' && parent -> path [1] == '\0') {
			sprintf (file -> path, "/%s", fileName);
		} else {
			sprintf (file -> path, "%s/%s", parent -> path, fileName);
		}
	#elif defined (_WIN32)
		if (strrchr (file -> path, '\\') - file -> path == strlen (file -> path) - 1) {
			sprintf (file -> path, "%s%s", parent -> path, fileName);
		} else {
			sprintf (file -> path, "%s\\%s", parent -> path, fileName);
		}
	#endif
	copyPathToWpath (file -> path, file -> wpath);
}

void MelderDir_relativePathToFile (MelderDir dir, const char *path, MelderFile file) {
	structMelderDir saveDir = { { 0 } };
	Melder_getDefaultDir (& saveDir);
	Melder_setDefaultDir (dir);
	Melder_relativePathToFile (path, file);
	Melder_setDefaultDir (& saveDir);
}

#ifndef UNIX
static void Melder_getDesktop (MelderDir dir) {
	dir -> path [0] = '\0';
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
	copyWpathToPath (parent -> wpath, parent -> path);
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
	copyWpathToPath (parent -> wpath, parent -> path);
}

int MelderDir_isDesktop (MelderDir dir) {
	return dir -> wpath [0] == '\0';
}

int MelderDir_getSubdir (MelderDir parent, const char *subdirName, MelderDir subdir) {
	#if defined (UNIX)
		if (parent -> path [0] == '/' && parent -> path [1] == '\0') {
			sprintf (subdir -> path, "/%s", subdirName);
		} else {
			sprintf (subdir -> path, "%s/%s", parent -> path, subdirName);
		}
	#elif defined (_WIN32)
		int length = strlen (parent -> path);
		char *backslash = strrchr (parent -> path, '\\');
		if (backslash && backslash - parent -> path == length - 1) {   /* C:\ or \\Swine\ */
			sprintf (subdir -> path, "%s%s", parent -> path, subdirName);
		} else {   /* C:\WINDOWS or \\Swine\Apps or even C: */
			sprintf (subdir -> path, "%s\\%s", parent -> path, subdirName);
		}
	#endif
	copyPathToWpath (subdir -> path, subdir -> wpath);
	return 1;
}

void Melder_getHomeDir (MelderDir homeDir) {
	#if defined (UNIX)
		char *home = getenv ("HOME");
		strcpy (homeDir -> path, home ? home : "/");
	#elif defined (_WIN32)
		/*if (GetEnvironmentVariable ("HOMESHARE", homeDir -> path, 255)) {
			GetEnvironmentVariable ("HOMEPATH", homeDir -> path + strlen (homeDir -> path), 255);*/
		if (GetEnvironmentVariable ("USERPROFILE", homeDir -> path, 255)) {
			;   /* Ready. */
		} else if (GetEnvironmentVariable ("HOMEDRIVE", homeDir -> path, 255)) {
			GetEnvironmentVariable ("HOMEPATH", homeDir -> path + strlen (homeDir -> path), 255 - strlen (homeDir -> path));
		} else {
			MelderDir_setToNull (homeDir);   /* Windows 95 and 98: alas. */
		}
	#endif
	copyPathToWpath (homeDir -> path, homeDir -> wpath);
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
			GetWindowsDirectory (prefDir -> path, 255);
		}
	#endif
	copyPathToWpath (prefDir -> path, prefDir -> wpath);
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
/* 
 * The user-defined write function that handles storing of the result of the
 * URL request on disk. This function can be platform-dependent.
 * ONLY for internal use.
 */
static size_t write_URL_data_to_file (void *buffer, size_t size, size_t nmemb, void *userp) {
	/* Just use the standard fwrite function (*userp == *stream) */
	return fwrite (buffer, size, nmemb, userp);
}
static size_t read_URL_data_from_file (void *buffer, size_t size, size_t nmemb, void *userp) {
	/* Just use the standard fwrite function (*userp == *stream) */
	return fread (buffer, size, nmemb, userp);
}
#endif

FILE * Melder_fopen (MelderFile file, const char *type) {
	FILE *f;
	file -> openForWriting = type [0] == 'w' || type [0] == 'a' || strchr (type, '+');
	if (strequ (file -> path, "<stdout>") && file -> openForWriting) {
		f = stdout;
	#ifdef CURLPRESENT
	} else if (strstr (file -> path, "://") && file -> openForWriting) {
		Melder_assert (type [0] == 'w');   /* Reject "append" and "random" access. */
		f = tmpfile ();   /* Open a temporary file for writing. */
	} else if (strstr (file -> path, "://") && ! file -> openForWriting) {
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
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_URL, file -> path);
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
			copyWpathToPath (file -> wpath, file -> path);
			f = fopen (file -> path, type);
		#endif
		#ifdef macintosh
		if (f == NULL) {
			/*
			 * Perhaps the file name was sent by a script command.
			 */
			structMelderFile file2 = { { 0 } };
			CFStringRef stringOfFile = CFStringCreateWithCString (NULL, file -> path, kCFStringEncodingMacRoman);
			CFMutableStringRef mutableStringOfFile = CFStringCreateMutableCopy (NULL, 0, stringOfFile);
			CFRelease (stringOfFile);
			CFStringNormalize (mutableStringOfFile, kCFStringNormalizationFormD);
			CFStringGetCString (mutableStringOfFile, file2. path, 260, kCFStringEncodingUTF8);
			CFRelease (mutableStringOfFile);
			f = fopen (file2. path, type);
		}
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
 	if (strstr (file -> path, "://") && file -> openForWriting) {
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
		CURLreturn = curl_easy_setopt (CURLhandle, CURLOPT_URL, file -> path);
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
		return Melder_error ("Error closing file \"%.200s\".", MelderFile_messageName (file));
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
		struct stat statistics;
		return ! stat (file -> path, & statistics);
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
		struct stat statistics;
		if (stat (file -> path, & statistics)) return -1;
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
	remove (file -> path);
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
	#if defined (macintosh)
		char romanName [260];
		CFStringRef unicodeName = CFStringCreateWithCString (NULL, file -> path, kCFStringEncodingUTF8);
		if (unicodeName) {
			CFStringGetCString (unicodeName, romanName, 260, kCFStringEncodingMacRoman);
			CFRelease (unicodeName);
		} else {
			sprintf (romanName, "<<%s>>", Melder_asciiMessage (file -> path));
		}
		return Melder_asciiMessage (romanName);
	#else
		return Melder_asciiMessage (Melder_fileToPath (file));
	#endif
}

wchar_t * MelderFile_messageNameW (MelderFile file) {
	return Melder_peekExpandBackslashes (file -> wpath);
}

char * MelderFile_readText (MelderFile file) {
	char *text;
	long length;
	FILE *f;
	if ((f = Melder_fopen (file, "rb")) == NULL) return NULL;
 	fseek (f, 0, SEEK_END);
 	length = ftell (f);
 	rewind (f);
	text = Melder_malloc (length + 1);
	if (! text) { Melder_fclose (file, f); return NULL; }
	fread (text, sizeof (char), length, f);
	if (! Melder_fclose (file, f)) {
		Melder_free (text);
		return Melder_errorp ("Error reading file \"%s\".", MelderFile_messageName (file));
	}
	text [length] = '\0';
	/*
	 * Convert Mac and DOS files to Unix text.
	 */
	(void) Melder_killReturns_inline (text);
	return text;
}

wchar_t * MelderFile_readTextW (MelderFile file) {
	int type = 0;   // 8-bit
	wchar_t *text = NULL;
	FILE *f = Melder_fopen (file, "rb");
	if (f == NULL) return NULL;
 	fseek (f, 0, SEEK_END);
 	unsigned long length = ftell (f);
 	rewind (f);
	if (length >= 2) {
		int firstByte = fgetc (f), secondByte = fgetc (f);
		if (firstByte == 0xfe && secondByte == 0xff) {
			type = 1;   // big-endian 16-bit
		} else if (firstByte == 0xff && secondByte == 0xfe) {
			type = 2;   // little-endian 16-bit
		}
	}
	if (type == 0) {
		rewind (f);   // length and type already set correctly.
		char *textA = Melder_malloc (length + 1);
		if (! textA) { Melder_fclose (file, f); return NULL; }
		fread (textA, sizeof (char), length, f);
		if (! Melder_fclose (file, f)) {
			Melder_free (textA);
			return Melder_errorp ("Error reading file \"%s\".", MelderFile_messageName (file));
		}
		textA [length] = '\0';
		/*
		 * Convert Mac and DOS files to Unix text.
		 */
		(void) Melder_killReturns_inline (textA);
		text = Melder_asciiToWcs (textA);
		Melder_free (textA);
	} else {
		length = length / 2 - 1;
		text = Melder_malloc ((length + 1) * sizeof (wchar_t));
		if (! text) { Melder_fclose (file, f); return NULL; }
		if (type == 1) {
			for (unsigned long i = 0; i < length; i ++) {
				text [i] = bingetu2 (f);
			}
		} else {
			for (unsigned long i = 0; i < length; i ++) {
				text [i] = bingetu2LE (f);
			}
		}
		if (! Melder_fclose (file, f)) {
			Melder_free (text);
			return Melder_errorp ("Error reading file \"%s\".", MelderFile_messageName (file));
		}
		text [length] = '\0';

	}
	return text;
}

/* BUG: the following two routines should be made system-independent,
   so that we can write DOS files from Unix etc., as determined by a user preference. */

int MelderFile_writeText (MelderFile file, const char *text) {
	FILE *f = Melder_fopen (file, "w");
	if (! f) return 0;
	/*
	 * On all systems, the number of bytes written (i.e. the return value of fwrite) equals strlen (text).
	 * On Windows, however, the resulting file length will be greater than this.
	 */
	fwrite (text, sizeof (char), strlen (text), f);   /* Not trailing null byte. */
	if (fclose (f))
		return Melder_error ("Error closing file \"%s\".", MelderFile_messageName (file));
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

static bool Melder_isValidAscii (const wchar_t *text) {
	for (; *text != '\0'; text ++) {
		if (*text > 127) return false;
	}
	return true;
}

int MelderFile_writeTextW (MelderFile file, const wchar_t *text) {
	FILE *f = Melder_fopen (file, "wb");
	if (! f) return 0;
	if (Melder_isValidAscii (text)) {
		fwrite (Melder_peekWcsToAscii (text), sizeof (char), wcslen (text), f);   /* Not trailing null byte. */
	} else {
		binputu2 (0xfeff, f);
		long n = wcslen (text);
		for (long i = 0; i < n; i ++) {
			binputu2 (text [i], f);
		}
	}
	if (fclose (f)) {
		Melder_error3 (L"Error closing file \"", MelderFile_messageNameW (file), L"\".");
		return 0;
	}
	MelderFile_setMacTypeAndCreator (file, 'TEXT', 0);
	return 1;
}

int MelderFile_appendText (MelderFile fs, const char *text) {
	FILE *f = Melder_fopen (fs, "a");
	if (! f) return 0;
	fwrite (text, sizeof (char), strlen (text), f);   /* Not trailing null byte. */
	if (fclose (f))
		return Melder_error ("Error closing file \"%s\".", MelderFile_messageName (fs));
	MelderFile_setMacTypeAndCreator (fs, 'TEXT', 0);
	return 1;
}

void Melder_getDefaultDir (MelderDir dir) {
	getcwd (dir -> path, 256);
	copyPathToWpath (dir -> path, dir -> wpath);
}

void Melder_setDefaultDir (MelderDir dir) {
	chdir (dir -> path);
}

void MelderFile_setDefaultDir (MelderFile file) {
	structMelderDir dir = { { 0 } };
	MelderFile_getParentDir (file, & dir);
	Melder_setDefaultDir (& dir);
}

void MelderFile_nativizePath (char *path) {
	(void) path;
}

int Melder_createDirectory (MelderDir parent, const char *dirName, int mode) {
#if defined (_WIN32)
	structMelderFile fs = { { 0 } };
	SECURITY_ATTRIBUTES sa;
	(void) mode;
	sa. nLength = sizeof (SECURITY_ATTRIBUTES);
	sa. lpSecurityDescriptor = NULL;
	sa. bInheritHandle = FALSE;
	sprintf (fs. path, "%s\\%s", parent -> path, dirName);
	if (! CreateDirectory (fs. path, & sa) && GetLastError () != ERROR_ALREADY_EXISTS)   /* Ignore if directory already exists. */
		return Melder_error ("Cannot create directory \"%s\".", MelderFile_messageName (& fs));
	return 1;
#else
	structMelderFile file = { { 0 } };
	if (parent -> path [0] == '/' && parent -> path [1] == '\0') {
		sprintf (file. path, "/%s", dirName);
	} else {
		sprintf (file. path, "%s/%s", parent -> path, dirName);
	}
	if (mkdir (file. path, mode) == -1 && errno != EEXIST)   /* Ignore if directory already exists. */
		return Melder_error ("Cannot create directory \"%s\".", MelderFile_messageName (& file));
	return 1;
#endif
}

/*
 * Routines for wrapping the file pointers.
 */

#define my  me ->

void MelderFile_open (MelderFile me) {
	my filePointer = Melder_fopen (me, "rb");
}

char * MelderFile_readLine (MelderFile me) {
	long i;
	static char *buffer;
	static long capacity;
	if (! my filePointer) return NULL;
	if (feof (my filePointer)) return NULL;
	if (! buffer) {
		buffer = Melder_malloc (capacity = 100);
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

void MelderFile_create (MelderFile me, const char *macType, const char *macCreator, const char *winExtension) {
	my filePointer = Melder_fopen (me, "wb");
	if (! my filePointer) return;
	#if defined (macintosh)
		(void) winExtension;
	{
		unsigned long macType_int = macType == NULL && macType [0] == '\0' ? 0 :
			((unsigned int) macType [0] << 24) | ((unsigned int) macType [1] << 16) |
			((unsigned int) macType [2] << 8) | (unsigned int) macType [3];
		unsigned long macCreator_int = macCreator == NULL && macCreator [0] == '\0' ? 0 :
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
	if (my type == Melder_FILETYPE_FLAC) {
	   if (my flacEncoder) {
		   FLAC__stream_encoder_finish (my flacEncoder);
		   FLAC__stream_encoder_delete (my flacEncoder);
	   }
	}
	else if (my filePointer) Melder_fclose (me, my filePointer);
	/* Set everything to zero, except paths (they stay around for error messages and the like). */
	my filePointer = NULL;
	my openForWriting = false;
	my flacEncoder = NULL;
	my type = 0;
}

/* End of file melder_files.c */
