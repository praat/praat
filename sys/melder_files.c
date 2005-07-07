/* melder_files.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * ob 2004/10/16 C++ compatible structs
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
#ifdef _WIN32
	#include <windows.h>
#endif
#ifdef macintosh
	#include "macport_on.h"
	#include <Folders.h>
	#include <Files.h>
	#include <MacErrors.h>
	#include <Script.h>
	#include <FSp_fopen.h>
	#include "macport_off.h"
	#define PtoCstr(p)  (p [p [0] + 1] = '\0', (char *) p + 1)
	#define PfromCstr(p,c)  p [0] = strlen (c), strcpy ((char *) p + 1, c);
#endif
#include <errno.h>
#include "melder.h"

#if defined (__MACH__)
	#include <sys/stat.h>
	#undef macintosh
	#define UNIX
#endif

static char theShellDirectory [256];
void Melder_rememberShellDirectory (void) {
	structMelderDir shellDir;
	Melder_getDefaultDir (& shellDir);
	strcpy (theShellDirectory, Melder_dirToPath (& shellDir));
}
char * Melder_getShellDirectory (void) {
	return & theShellDirectory [0];
}

#ifdef macintosh
void Melder_macToFile (void *void_fspec, MelderFile file) {
	FSSpec *fspec = (FSSpec *) void_fspec;
	file -> vRefNum = fspec -> vRefNum;
	file -> parID = fspec -> parID;
	strcpy (file -> name, PtoCstr (fspec -> name));
}
int Melder_fileToMac (MelderFile file, void *void_fspec) {
	FSSpec *fspec = (FSSpec *) void_fspec;
	Str255 pname;
	OSErr err;
	PfromCstr (pname, file -> name);
	err = FSMakeFSSpec (file -> vRefNum, file -> parID, & pname [0], fspec);
	if (err != noErr && err != fnfErr)
		return Melder_error ("Error #%d looking for file %s.", err, file -> name);
	return 1;
}
#endif
#ifdef __MACH__
void Melder_machToFile (void *void_fsref, MelderFile file) {
	FSRef *fsref = (FSRef *) void_fsref;
	FSRefMakePath (fsref, (unsigned char *) file -> path, 259);
}
static void Melder_machToDir (int vRefNum, long dirID, MelderDir dir) {
	FSSpec fspec;
	FSRef fsref;
	FSMakeFSSpec (vRefNum, dirID, NULL, & fspec);
	FSpMakeFSRef (& fspec, & fsref);
	FSRefMakePath (& fsref, (unsigned char *) dir -> path, 259);
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
		structMelderDir parentDir;
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
	#if defined (macintosh)
		return file -> name;
	#elif defined (UNIX)
		char *slash = strrchr (file -> path, '/');
		return slash ? slash + 1 : file -> path;
	#elif defined (_WIN32)
		char *backslash = strrchr (file -> path, '\\');
		return backslash ? backslash + 1 : file -> path;
	#endif
}

char * MelderDir_name (MelderDir dir) {
	#if defined (macintosh)
		CInfoPBRec catInfo;
		static Str255 pdirName, qdirName;
		static int cycle = 0;
		catInfo. dirInfo.ioFDirIndex = -1;   /* Get directory info. */
		catInfo. dirInfo.ioVRefNum = dir -> vRefNum;   /* In. */
		catInfo. dirInfo.ioDrDirID = dir -> dirID;   /* In. */
		catInfo. dirInfo.ioNamePtr = cycle ? qdirName : pdirName;   /* Out. */
		PBGetCatInfoSync (& catInfo);
		cycle = ! cycle;
		return cycle ? PtoCstr (pdirName) : PtoCstr (qdirName);
	#elif defined (UNIX)
		char *slash = strrchr (dir -> path, '/');
		return slash ? slash + 1 : dir -> path;
	#elif defined (_WIN32)
		char *backslash = strrchr (dir -> path, '\\');
		return backslash ? backslash + 1 : dir -> path;
	#endif
}

int Melder_pathToDir (const char *path, MelderDir dir) {
	#if defined (macintosh)
		FSSpec fspec;
		Str255 ppath;
		OSErr err;
		PfromCstr (ppath, path);
		err = FSMakeFSSpec (0, 0, & ppath [0], & fspec);   /* Normalize. */
		if (err != noErr && err != fnfErr) {
			char *colon;
			path = PtoCstr (ppath);
			colon = strrchr (path, ':');
			if (colon) *colon = '\0';
			if (err == nsvErr) {
				colon = strchr (path, ':');
				if (colon) *colon = '\0';
				Melder_error ("Volume \"%s\" not found.", path);
				if (path [0] == '\0')
					Melder_error ("Hint: empty volume name.");
				else if (path [0] == ' ')
					Melder_error ("Hint: volume name begins with a space.");
				else if (path [strlen (path) - 1] == ' ')
					Melder_error ("Hint: volume name ends in a space.");
				return NULL;
			} else if (err == dirNFErr) {
				Melder_error ("Folder \"%s\" not found.", path);
				if (path [0] == '\0')
					Melder_error ("Hint: empty folder name.");
				else if (path [0] == ' ')
					Melder_error ("Hint: folder name begins with a space.");
				else if (path [strlen (path) - 1] == ' ')
					Melder_error ("Hint: folder name ends in a space.");
				return NULL;
			} else {
				return Melder_error ("Error #%d looking for folder %s.", err, path);
			}
		}
		/*
		 * The parID in fspec is the dirID of the target directory's parent.
		 * Therefore: look up by name.
		 */
		dir -> vRefNum = fspec. vRefNum;
		dir -> dirID = fspec. parID;   /* Temporarily. */
		if (! MelderDir_getSubdir (dir, PtoCstr (fspec. name), dir)) return 0;
	#else
		strcpy (dir -> path, path);
	#endif
	return 1;
}

int Melder_pathToFile (const char *path, MelderFile file) {
	/*
	 * This handles complete path names only.
	 * Unlike Melder_relativePathToFile, this handles MacOS 7/8/9 or Windows file names with slashes in it.
	 *
	 * Used if we know for sure that we have a complete path name,
	 * i.e. if the program determined the name (fileselector, printing, prefs).
	 */
	#if defined (macintosh)
		FSSpec fspec;
		Str255 ppath;
		OSErr err;
		PfromCstr (ppath, path);
		err = FSMakeFSSpec (0, 0, & ppath [0], & fspec);   /* Normalize. */
		if (err != noErr && err != fnfErr) {
			char *colon;
			path = PtoCstr (ppath);
			colon = strrchr (path, ':');
			if (colon) *colon = '\0';
			if (err == nsvErr) {
				colon = strchr (path, ':');
				if (colon) *colon = '\0';
				Melder_error ("Volume \"%s\" not found.", path);
				if (path [0] == '\0')
					Melder_error ("Hint: empty volume name.");
				else if (path [0] == ' ')
					Melder_error ("Hint: volume name begins with a space.");
				else if (path [strlen (path) - 1] == ' ')
					Melder_error ("Hint: volume name ends in a space.");
				return NULL;
			} else if (err == dirNFErr) {
				Melder_error ("Folder \"%s\" not found.", path);
				if (path [0] == '\0')
					Melder_error ("Hint: empty folder name.");
				else if (path [0] == ' ')
					Melder_error ("Hint: folder name begins with a space.");
				else if (path [strlen (path) - 1] == ' ')
					Melder_error ("Hint: folder name ends in a space.");
				return NULL;
			} else {
				return Melder_error ("Error #%d looking for folder %s.", err, path);
			}
		}
		Melder_macToFile (& fspec, file);
	#else
		strcpy (file -> path, path);
	#endif
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
	#if defined (macintosh)
		/*
		 * MacOS 7/8/9 complete path names contain one or more colons, but not at the start:
		 *    MacHD:Programs:Praats:Praat
		 * Otherwise, paths are relative:
		 *    Praat               (no colon)
		 *    :Praats:Praat       (a colon at the start)
		 */
		FSSpec fspec;
		Str255 ppath;
		OSErr err;
		char path2 [256];
		if (strchr (path, '/') && ! strchr (path, ':') && path [0] != '/') {
			char macPath [256];
			sprintf (macPath, ":%s", path);
			for (;;) {
				char *slash = strchr (macPath, '/');
				if (slash == NULL) break;
				*slash = ':';
			}
			return Melder_relativePathToFile (macPath, file);
		}
		strcpy (path2, path);
		MelderFile_nativizePath (path2);
		PfromCstr (ppath, path2);
		err = FSMakeFSSpec (0, 0, & ppath [0], & fspec);   /* Normalize. */
		if (err != noErr && err != fnfErr) {
			char *colon;
			path = PtoCstr (ppath);
			colon = strrchr (path, ':');
			if (colon) *colon = '\0';
			if (err == nsvErr) {
				colon = strchr (path, ':');
				if (colon) *colon = '\0';
				Melder_error ("Volume \"%s\" not found.", path);
				if (path [0] == '\0')
					Melder_error ("Hint: empty volume name.");
				else if (path [0] == ' ')
					Melder_error ("Hint: volume name begins with a space.");
				else if (path [strlen (path) - 1] == ' ')
					Melder_error ("Hint: volume name ends in a space.");
				return NULL;
			} else if (err == dirNFErr) {
				Melder_error ("Folder \"%s\" not found.", path);
				if (path [0] == '\0')
					Melder_error ("Hint: empty folder name.");
				else if (path [0] == ' ')
					Melder_error ("Hint: folder name begins with a space.");
				else if (path [strlen (path) - 1] == ' ')
					Melder_error ("Hint: folder name ends in a space.");
				return NULL;
			} else {
				return Melder_error ("Error #%d looking for folder %s.", err, path);
			}
		}
		Melder_macToFile (& fspec, file);
	#elif defined (UNIX)
		/*
		 * We assume that Unix complete path names start with a slash.
		 */
		if (path [0] == '~' && path [1] == '/') {
			sprintf (file -> path, "%s%s", getenv ("HOME"), & path [1]);
		} else if (path [0] == '/' || strequ (path, "<stdout>") || strstr (path, "://")) {
			strcpy (file -> path, path);
		} else {
			structMelderDir dir;
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
		if (strchr (path, ':') || path [0] == '\\' && path [1] == '\\' || strequ (path, "<stdout>") || strstr (path, "://")) {
			strcpy (file -> path, path);
		} else {
			structMelderDir dir;
			char path2 [256];
			if (strchr (path, '/') && ! strchr (path, ':') && ! strchr (path, '\\') && path [0] != '/') {
				char winPath [256];
				strcpy (winPath, path);
				for (;;) {
					char *slash = strchr (winPath, '/');
					if (slash == NULL) break;
					*slash = '\\';
				}
				return Melder_relativePathToFile (winPath, file);
			}
			strcpy (path2, path);
			MelderFile_nativizePath (path2);
			Melder_getDefaultDir (& dir);   /* BUG */
			if (dir. path [0] != '\0' && dir. path [strlen (dir.path) - 1] == '\\')
				sprintf (file -> path, "%s%s", dir. path, path2);
			else
				sprintf (file -> path, "%s\\%s", dir. path, path2);
		}
	#endif
	return 1;
}

#ifdef macintosh
	static int appendDirectoryPath (Str255 ppath, int vRefNum, long dirID) {
		CInfoPBRec catInfo;
		Str255 pdirName;
		OSErr err;
		catInfo. dirInfo.ioFDirIndex = -1;   /* Get directory info. */
		catInfo. dirInfo.ioVRefNum = vRefNum;   /* In. */
		catInfo. dirInfo.ioDrDirID = dirID;   /* In. */
		catInfo. dirInfo.ioNamePtr = pdirName;   /* Out. */
		err = PBGetCatInfoSync (& catInfo);
		if (err != noErr) return 0;
		if (dirID != 2) appendDirectoryPath (ppath, vRefNum, catInfo. dirInfo.ioDrParID);   /* Recurse. */
		memcpy ((char *) & ppath [ppath [0] + 1], (const char *) & pdirName [1], pdirName [0]);
		ppath [0] = ppath [0] + pdirName [0] + 1;
		ppath [ppath [0]] = ':';
		return 1;
	}
#endif
char * Melder_dirToPath (MelderDir dir) {
	#if defined (macintosh)
		static Str255 ppath;
		ppath [0] = 0;
		if (! appendDirectoryPath (ppath, dir -> vRefNum, dir -> dirID)) {
			return "";
		}
		return PtoCstr (ppath);
	#else
		return & dir -> path [0];
	#endif
}

char * Melder_fileToPath (MelderFile file) {
	#if defined (macintosh)
		static Str255 ppath;
		char *path;
		ppath [0] = 0;
		if (! appendDirectoryPath (ppath, file -> vRefNum, file -> parID)) {
			return file -> name;
		}
		path = PtoCstr (ppath);
		strcat (path, file -> name);
		return path;
	#else
		return & file -> path [0];
	#endif
}

void MelderFile_copy (MelderFile file, MelderFile copy) {
	#if defined (macintosh)
		copy -> vRefNum = file -> vRefNum;
		copy -> parID = file -> parID;
		strcpy (copy -> name, file -> name);
	#else
		strcpy (copy -> path, file -> path);
	#endif
}

void MelderDir_copy (MelderDir dir, MelderDir copy) {
	#if defined (macintosh)
		copy -> vRefNum = dir -> vRefNum;
		copy -> dirID = dir -> dirID;
	#else
		strcpy (copy -> path, dir -> path);
	#endif
}

int MelderFile_equal (MelderFile file1, MelderFile file2) {
	#if defined (macintosh)
		return file1 -> vRefNum == file2 -> vRefNum &&
			file1 -> parID == file2 -> parID && strequ (file1 -> name, file2 -> name);
	#else
		return strequ (file1 -> path, file2 -> path);
	#endif
}

int MelderDir_equal (MelderDir dir1, MelderDir dir2) {
	#if defined (macintosh)
		return dir1 -> vRefNum == dir2 -> vRefNum && dir1 -> dirID == dir2 -> dirID;
	#else
		return strequ (dir1 -> path, dir2 -> path);
	#endif
}

void MelderFile_setToNull (MelderFile file) {
	#if defined (macintosh)
		file -> name [0] = '\0';
	#else
		file -> path [0] = '\0';
	#endif
}

int MelderFile_isNull (MelderFile file) {
	#if defined (macintosh)
		return file -> name [0] == '\0';
	#else
		return file -> path [0] == '\0';
	#endif
}

void MelderDir_setToNull (MelderDir dir) {
	#if defined (macintosh)
		dir -> vRefNum = 0;
		dir -> dirID = 0;
	#else
		dir -> path [0] = '\0';
	#endif
}

int MelderDir_isNull (MelderDir dir) {
	#if defined (macintosh)
		return dir -> vRefNum == 0 && dir -> dirID == 0;
	#else
		return dir -> path [0] == '\0';
	#endif
}

void MelderDir_getFile (MelderDir parent, const char *fileName, MelderFile file) {
	#if defined (macintosh)
		file -> vRefNum = parent -> vRefNum;
		file -> parID = parent -> dirID;
		strcpy (file -> name, fileName);
	#elif defined (UNIX)
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
}

void MelderDir_relativePathToFile (MelderDir dir, const char *path, MelderFile file) {
	structMelderDir saveDir;
	Melder_getDefaultDir (& saveDir);
	Melder_setDefaultDir (dir);
	Melder_relativePathToFile (path, file);
	Melder_setDefaultDir (& saveDir);
}

#ifndef UNIX
static void Melder_getDesktop (MelderDir dir) {
	#if defined (macintosh)
		dir -> vRefNum = 0;
		dir -> dirID = 0;
	#else
		dir -> path [0] = '\0';
	#endif
}
#endif

void MelderFile_getParentDir (MelderFile file, MelderDir parent) {
	#if defined (macintosh)
		parent -> vRefNum = file -> vRefNum;
		parent -> dirID = file -> parID;
	#elif defined (UNIX)
		/*
		 * The parent of /usr/hello.txt is /usr.
		 * The parent of /hello.txt is /.
		 */
		char *slash;
		strcpy (parent -> path, file -> path);
		slash = strrchr (parent -> path, '/');
		if (slash) *slash = '\0';
		if (parent -> path [0] == '\0') strcpy (parent -> path, "/");
	#elif defined (_WIN32)
		/*
		 * The parent of C:\WINDOWS\CTRL.DLL is C:\WINDOWS.
		 * The parent of E:\Praat.exe is E:\.
		 * The parent of \\Swine\Apps\init.txt is \\Swine\Apps.
		 * The parent of \\Swine\init.txt is \\Swine\.   (BUG ?)
		 */
		char *colon;
		strcpy (parent -> path, file -> path);
		colon = strchr (parent -> path, ':');
		if (colon) {
			char *backslash = strrchr (parent -> path, '\\');
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
			char *backslash = strrchr (parent -> path + 2, '\\');
			if (backslash) {   /* \\Swine\Apps\init.txt or \\Swine\init.txt */
				char *leftBackslash = strchr (parent -> path + 2, '\\');
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
	#if defined (macintosh)
		if (dir -> dirID == 2) {
			/*
			 * This is the root directory on this volume.
			 * The parent is the desktop.
			 */
			Melder_getDesktop (parent);
		} else {
			CInfoPBRec catInfo;
			Str255 pdirName;
			OSErr err;
			catInfo. dirInfo.ioFDirIndex = -1;   /* Get directory info. */
			catInfo. dirInfo.ioVRefNum = dir -> vRefNum;   /* In. */
			catInfo. dirInfo.ioDrDirID = dir -> dirID;   /* In. */
			catInfo. dirInfo.ioNamePtr = pdirName;   /* Out. */
			err = PBGetCatInfoSync (& catInfo);
			if (err != noErr) {
				Melder_getDesktop (parent);
			} else {
				parent -> vRefNum = dir -> vRefNum;
				parent -> dirID = catInfo. dirInfo.ioDrParID;
			}
		}
	#elif defined (UNIX)
		/*
		 * The parent of /usr/local is /usr.
		 * The parent of /usr is /.
		 * The parent of / is "".
		 */
		char *slash;
		strcpy (parent -> path, dir -> path);
		slash = strrchr (parent -> path, '/');
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
		char *colon;
		strcpy (parent -> path, dir -> path);
		colon = strchr (parent -> path, ':');
		if (colon) {
			int length = strlen (parent -> path);
			char *backslash = strrchr (parent -> path, '\\');
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
			int length = strlen (parent -> path);
			char *backslash = strrchr (parent -> path + 2, '\\');
			if (backslash) {   /* \\Swine\Apps\Praats or \\Swine\Apps or \\Swine\ */
				if (backslash - parent -> path == length - 1) {   /* \\Swine\ */
					Melder_getDesktop (parent);   /* empty string */
				} else {   /* \\Swine\Apps\Praats or \\Swine\Apps */
					char *leftBackslash = strchr (parent -> path + 2, '\\');
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
	#if defined (macintosh)
		return dir -> dirID == 0;
	#elif defined (UNIX)
		return dir -> path [0] == '\0';
	#elif defined (_WIN32)
		return dir -> path [0] == '\0';
	#endif
}

int MelderDir_getSubdir (MelderDir parent, const char *subdirName, MelderDir subdir) {
	#if defined (macintosh)
		Str255 psubdirName;
		CInfoPBRec cpb;
		OSErr err;
		PfromCstr (psubdirName, subdirName);
		cpb. dirInfo.ioNamePtr = & psubdirName [0];
		cpb. dirInfo.ioVRefNum = parent -> vRefNum;
		cpb. dirInfo.ioFDirIndex = 0;   /* IM IV-156 suggests that ioDrDirID will be ignored ?? */
		cpb. dirInfo.ioDrDirID = parent -> dirID;
		err = PBGetCatInfoSync (& cpb);
		if (err != noErr) return 0;
		if ((cpb. hFileInfo. ioFlAttrib & (1 << ioDirFlg)) == 0) return 0;
		subdir -> vRefNum = parent -> vRefNum;
		subdir -> dirID = cpb. dirInfo.ioDrDirID;
	#elif defined (UNIX)
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
	return 1;
}

void Melder_getHomeDir (MelderDir homeDir) {
	#if defined (macintosh)
		MelderDir_setToNull (homeDir);   /* Classic Mac: alas. */
	#elif defined (UNIX)
		char *home = getenv ("HOME");
		strcpy (homeDir -> path, home ? home : "/");
	#elif defined (_WIN32)
		if (GetEnvironmentVariable ("HOMESHARE", homeDir -> path, 255)) {
			GetEnvironmentVariable ("HOMEPATH", homeDir -> path + strlen (homeDir -> path), 255);
		} else if (GetEnvironmentVariable ("HOMEDRIVE", homeDir -> path, 255)) {
			GetEnvironmentVariable ("HOMEPATH", homeDir -> path + strlen (homeDir -> path), 255);
		} else {
			MelderDir_setToNull (homeDir);   /* Windows 95 and 98: alas. */
		}
	#endif
}

void Melder_getPrefDir (MelderDir prefDir) {
	#if defined (macintosh)
		FindFolder (kOnSystemDisk, kPreferencesFolderType, kCreateFolder, & prefDir -> vRefNum, & prefDir -> dirID);
	#elif defined (__MACH__)
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
}

void Melder_getTempDir (MelderDir tempDir) {
	#if defined (macintosh)
		FindFolder (kOnSystemDisk, kTemporaryFolderType, kCreateFolder, & tempDir -> vRefNum, & tempDir -> dirID);
	#elif defined (__MACH__)
		short vRefNum;
		long dirID;
		FindFolder (kOnSystemDisk, kTemporaryFolderType, kCreateFolder, & vRefNum, & dirID);
		Melder_machToDir (vRefNum, dirID, tempDir);
	#else
		(void) tempDir;
	#endif
}

#if defined (macintosh) || defined (__MACH__)

static long textCreator = 'PpgB';

void MelderFile_setMacTypeAndCreator (MelderFile file, long fileType, long creator) {
	#ifdef __MACH__
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
  	#else
  		OSErr err;
		FSSpec fspec;
		FInfo finderInfo;
		Melder_fileToMac (file, & fspec);
		err = FSpGetFInfo (& fspec, & finderInfo);
		if (err == noErr) {
			finderInfo. fdType = fileType;
			finderInfo. fdCreator = creator ? creator : textCreator;
			FSpSetFInfo (& fspec, & finderInfo);
		}
	#endif
}
unsigned long MelderFile_getMacType (MelderFile file) {
	#ifdef __MACH__
		OSStatus err;
		FSRef fsref;
		FSCatalogInfo info;
		Melder_fileToMach (file, & fsref);
		err = FSGetCatalogInfo (& fsref, kFSCatInfoFinderInfo, & info, NULL, NULL, NULL);
		if (err == noErr) {
			return ((FInfo *) & info. finderInfo) -> fdType;
		}
  	#else
  		OSErr err;
		FSSpec fspec;
		FInfo finderInfo;
		Melder_fileToMac (file, & fspec);
		err = FSpGetFInfo (& fspec, & finderInfo);
		if (err == noErr) {
			return finderInfo. fdType;
		}
	#endif
	return 0;
}
#endif

FILE * Melder_fopen (MelderFile file, const char *type) {
	FILE *f;
	#ifdef macintosh
		/*
		 * IM VI:25-7 tells us not to use HSetVol,
		 * i.e. the following should not be used:
			err = HGetVol (NULL, & saveVRefNum, & saveDirID);
			HSetVol (NULL, fspec. vRefNum, fspec. parID);
			f = fopen (PtoCstr (fspec. name), type);
			if (err == noErr) HSetVol (NULL, saveVRefNum, saveDirID);
		 *
		 * Nevertheless, the following does seem to work:
		 */
		#if 0
			structMelderDir defaultDir;
			Melder_getDefaultDir (& defaultDir);
			MelderFile_setDefaultDir (file);
			f = fopen (file -> name, type);
			Melder_setDefaultDir (& defaultDir);
		#else
		/* But we have an alternative (include Fsp_fopen.c in the project): */
			FSSpec fspec;
			if (! Melder_fileToMac (file, & fspec)) return NULL;
			f = FSp_fopen (& fspec, type);
		#endif
	#else
		if (strequ (file -> path, "<stdout>") && strchr (type, 'w')) {
			f = stdout;
		#ifdef CURLPRESENT
 		} else if (strstr (file -> path, "://") && strchr (type, 'r')) {
 			f = praat_getURL (file -> path);
 		#endif
		} else {
			f = fopen (file -> path, type);
		}
	#endif
	if (! f) {
		char *path = Melder_fileToPath (file);
		#ifdef sgi
			Melder_error ("%s.", strerror (errno));
		#endif
		Melder_error ("Cannot %s file \"%.200s\".",
			type [0] == 'r' ? "open" : type [0] == 'a' ? "append to" : "create", MelderFile_messageName (file));
		if (path [0] == '\0')
			Melder_error ("Hint: empty file name.");
		else if (path [0] == ' ')
			Melder_error ("Hint: file name starts with a space.");
		else if (path [strlen (path) - 1] == ' ')
			Melder_error ("Hint: file name ends in a space.");
		return NULL;
	}
	return f;
}

int Melder_fclose (MelderFile file, FILE *f) {
	if (! f) return 1;
	if (f != stdout && fclose (f) == EOF) {
		#ifdef sgi
			Melder_error ("%s", strerror (errno));
		#endif
		return Melder_error ("Error closing file \"%.200s\".", MelderFile_messageName (file));
	}
	return 1;
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
	#if defined (macintosh)
		FSSpec fspec;
		if (! file) return 1;
		if (! Melder_fileToMac (file, & fspec)) return 0;
		FSpDelete (& fspec);
	#else
		if (! file) return 1;
		remove (file -> path);
	#endif
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

char * MelderFile_messageName (MelderFile file) {
	#ifdef __MACH__
		char romanName [260];
		CFStringRef unicodeName = CFStringCreateWithCString (NULL, file -> path, kCFStringEncodingUTF8);
		CFStringGetCString (unicodeName, romanName, 260, kCFStringEncodingMacRoman);
		CFRelease (unicodeName);
		return Melder_asciiMessage (romanName);
	#else
		return Melder_asciiMessage (Melder_fileToPath (file));
	#endif
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

/* BUG: the following two routines should be made system-independent,
   so that we can write DOS files from Unix etc., as determined by a user preference. */

int MelderFile_writeText (MelderFile fs, const char *text) {
	FILE *f;
	/*
	 * If the file exists, we will use a temporary file;
	 * if not, we are going to write to it directly.
	 */
	#if defined (UNIX)
		if (MelderFile_exists (fs)) {
			structMelderFile tempFile;
   			FILE *f;
			char tempPath [256], command [500];
			strcpy (tempPath, "/tmp/edjeXXXXXX");
			mktemp (tempPath);
			Melder_pathToFile (tempPath, & tempFile);
			if (! (f = Melder_fopen (& tempFile, "w")))
				return Melder_error ("Cannot write temporary file.");
			fwrite (text, sizeof (char), strlen (text), f);   /* Not trailing null byte. */
			if (fclose (f))
				return Melder_error ("Error closing temporary file \"%s\".", MelderFile_messageName (& tempFile));
			sprintf (command, "cp %s \"%s\"", tempPath, fs -> path);
			if (system (command)) {
				unlink (tempPath);
				return Melder_error ("Error creating file \"%s\".", MelderFile_messageName (fs));
			}
			unlink (tempPath);
			return 1;
		}
	#elif defined (macintosh)
		/* Use FspExchangeFiles (IM 25-19) and HGetVol (25-37).
		 but note that GetDateTime alone does not work if we write multiple files in one second.
		 We can also look at the code of __temp_file_name in file_io.mac.c. */
	#elif defined (_WIN32)
	#endif
	/*
	 * Default action: just write the file.
	 * Appropriate if the file did not exist, or if we just append,
	 * or for systems for which we do not know how to use temporary files.
	 */
	f = Melder_fopen (fs, "w");
	if (! f) return 0;
	/*
	 * On all systems, the number of bytes written (i.e. the return value of fwrite) equals strlen (text).
	 * On Windows, however, the resulting file length will be greater than this.
	 */
	fwrite (text, sizeof (char), strlen (text), f);   /* Not trailing null byte. */
	if (fclose (f))
		return Melder_error ("Error closing file \"%s\".", MelderFile_messageName (fs));
	MelderFile_setMacTypeAndCreator (fs, 'TEXT', 0);
	return 1;
}

int MelderFile_appendText (MelderFile fs, const char *text) {
	FILE *f = Melder_fopen (fs, "a");
	if (! f) return 0;
	fwrite (text, sizeof (char), strlen (text), f);   /* Not trailing null byte. */
	if (fclose (f)) return Melder_error ("Error closing file \"%s\".", MelderFile_messageName (fs));
	MelderFile_setMacTypeAndCreator (fs, 'TEXT', 0);
	return 1;
}

void Melder_getDefaultDir (MelderDir dir) {
	#if defined (macintosh)
		WDPBRec wdpb;
		OSErr err;
		wdpb. ioNamePtr = NULL;
		err = PBHGetVolSync (& wdpb);
		if (err == noErr) {
			dir -> vRefNum = wdpb. ioWDVRefNum;
			/*
			 * According to IM IV-132, vRefNum is now a volume reference number,
			 * since we never call PBHSetVolSync with a working directory reference number.
			 */
			dir -> dirID = wdpb. ioWDDirID;
		} else {
			/*
			 * There is no default directory.
			 * Go to the root directory of the start-up disk.
			 */
			HSetVol (NULL, dir -> vRefNum = -1, dir -> dirID = 2);
		}
	#else
		getcwd (dir -> path, 256);
	#endif
}

void Melder_setDefaultDir (MelderDir dir) {
	#ifdef macintosh
		structMelderDir defaultDir;
		/*
			A fix. In Mac OS X, HSetVol does not work early.
			This caused relative file names in scripts not to work on first run.
			Preceding it by PBHGetVolSync turns out to solve the problem.
		*/
		Melder_getDefaultDir (& defaultDir);
		HSetVol (NULL, dir -> vRefNum, dir -> dirID);   /* Despite IM VI:25-7. */
	#else
		chdir (dir -> path);
	#endif
}

void MelderFile_setDefaultDir (MelderFile file) {
	structMelderDir dir;
	MelderFile_getParentDir (file, & dir);
	Melder_setDefaultDir (& dir);
}

void MelderFile_nativizePath (char *path) {
	#ifdef macintosh
		/*
		 * myfile --> myfile
		 * mydir/myfile --> :mydir:myfile
		 */
		char *p;
		int relative = FALSE;
		for (p = path; *p; p ++) {
			if (*p == '/') {
				*p = ':';
				relative = TRUE;
			}
		}
		if (relative) {   /* Prepend a colon. */
			p [1] = '\0';
			for (; p - path > 0; p --) p [0] = p [-1];
			p [0] = ':';
		}
	#else
		(void) path;
	#endif
}

int Melder_createDirectory (MelderDir parent, const char *dirName, int mode) {
#ifdef macintosh
	FSSpec fss;
	Str255 pdirName;
	long subdirID = 0;
	OSErr err;
	(void) mode;
	PfromCstr (pdirName, dirName);
	err = FSMakeFSSpec (parent -> vRefNum, parent -> dirID, & pdirName [0], & fss);
	if (err != noErr && err != fnfErr)
		return Melder_error ("Error #%d looking for folder %s.", err, dirName);
	err = FSpDirCreate (& fss, smSystemScript, & subdirID);
	if (err != noErr && err != dupFNErr)   /* Ignore if directory already exists. */
		{ errno = err; return Melder_error ("Cannot create directory \"%s\".", dirName); }
	return 1;
#elif defined (_WIN32)
	structMelderFile fs;
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
	structMelderFile file;
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
	#if defined (macintosh) || defined (__MACH__)
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
		Melder_error ("Cannot seek in file %s.", MelderFile_messageName (me));
		fclose (my filePointer);
		my filePointer = NULL;
	}
}

long MelderFile_tell (MelderFile me) {
	long result = 0;
	if (! my filePointer) return 0;
	if ((result = ftell (my filePointer)) == -1) {
		Melder_error ("Cannot tell in file %s.", MelderFile_messageName (me));
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
	if (! my filePointer) return;
	Melder_fclose (me, my filePointer);
	my filePointer = NULL;
}

/* End of file melder_files.c */
