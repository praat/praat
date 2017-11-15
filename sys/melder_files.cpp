/* melder_files.cpp
 *
 * Copyright (C) 1992-2008,2010-2017 Paul Boersma, 2013 Tom Naughton
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
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
 * pb 2007/05/28 wchar
 * pb 2007/06/09 more wchar
 * pb 2007/08/12 more wchar
 * pb 2007/10/05 FSFindFolder
 * pb 2008/11/01 warn after finding final tabs (not just spaces) in file names
 * pb 2010/12/14 more high Unicode compatibility
 * pb 2011/04/05 C++
 * pb 2012/10/07 
 */

#if defined (UNIX)
	#include <unistd.h>
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
	#include <CoreFoundation/CoreFoundation.h>
	#include "macport_off.h"
#endif
#include <errno.h>
#include "abcio.h"
#include "melder.h"

//#include "flac_FLAC_stream_encoder.h"
extern "C" int  FLAC__stream_encoder_finish (FLAC__StreamEncoder *);
extern "C" void FLAC__stream_encoder_delete (FLAC__StreamEncoder *);

#if defined (macintosh)
	#include <sys/stat.h>
	#define UNIX
	#include <unistd.h>
	#include "UnicodeData.h"
#endif

static char32 theShellDirectory [kMelder_MAXPATH+1];
void Melder_rememberShellDirectory () {
	structMelderDir shellDir { };
	Melder_getDefaultDir (& shellDir);
	str32cpy (theShellDirectory, Melder_dirToPath (& shellDir));
}
const char32 * Melder_getShellDirectory () {
	return & theShellDirectory [0];
}

void Melder_str32To8bitFileRepresentation_inplace (const char32 *string, char *utf8) {
	#if defined (macintosh)
		/*
			On the Mac, the POSIX path name is stored in canonically decomposed UTF-8 encoding.
			The path is probably in precomposed UTF-32.
			So we first convert to UTF-16, then turn into CFString, then decompose, then convert to UTF-8.
		*/
		UniChar unipath [kMelder_MAXPATH+1];
		int64 n = str32len (string), n_utf16 = 0;
		for (int64 i = 0; i < n; i ++) {
			char32 kar = (char32) string [i];   // change sign (bit 32 is never used)
			if (kar <= 0x00FFFF) {
				unipath [n_utf16 ++] = (UniChar) kar;   // including null byte; guarded truncation
			} else if (kar <= 0x10FFFF) {
				kar -= 0x010000;
				unipath [n_utf16 ++] = (UniChar) (0x00D800 | (kar >> 10));   // correct truncation, because UTF-32 has fewer than 27 bits (in fact it has 21 bits)
				unipath [n_utf16 ++] = (UniChar) (0x00DC00 | (kar & 0x0003FF));
			} else {
				unipath [n_utf16 ++] = UNICODE_REPLACEMENT_CHARACTER;
			}
		}
		unipath [n_utf16] = u'\0';
		CFStringRef cfpath = CFStringCreateWithCharacters (nullptr, unipath, n_utf16);
		CFMutableStringRef cfpath2 = CFStringCreateMutableCopy (nullptr, 0, cfpath);
		CFRelease (cfpath);
		CFStringNormalize (cfpath2, kCFStringNormalizationFormD);   // Mac requires decomposed characters
		CFStringGetCString (cfpath2, (char *) utf8, kMelder_MAXPATH+1, kCFStringEncodingUTF8);   // Mac POSIX requires UTF-8
		CFRelease (cfpath2);
	#elif defined (UNIX) || defined (__CYGWIN__)
		Melder_32to8_inplace (string, utf8);
	#elif defined (_WIN32)
		int n = str32len (string), i, j;
		for (i = 0, j = 0; i < n; i ++) {
			utf8 [j ++] = string [i] <= 255 ? string [i] : '?';   // the usual replacement on Windows
		}
		utf8 [j] = '\0';
	#else
		//#error Unsupported platform.
	#endif
}

#if defined (UNIX)
void Melder_8bitFileRepresentationToStr32_inplace (const char *path8, char32 *path32) {
	#if defined (macintosh)
		CFStringRef cfpath = CFStringCreateWithCString (nullptr, path8, kCFStringEncodingUTF8);
		if (! cfpath) {
			/*
				Probably something wrong, like a disk was disconnected in the meantime.
			*/
			Melder_8to32_inplace (path8, path32, kMelder_textInputEncoding::UTF8);
			Melder_throw (U"Unusual error finding or creating file ", path32, U".");
		}
		CFMutableStringRef cfpath2 = CFStringCreateMutableCopy (nullptr, 0, cfpath);
		CFRelease (cfpath);
		CFStringNormalize (cfpath2, kCFStringNormalizationFormC);   // Praat requires composed characters
		integer n_utf16 = CFStringGetLength (cfpath2);
		integer n_utf32 = 0;
		for (integer i = 0; i < n_utf16; i ++) {
			char32 kar1 = CFStringGetCharacterAtIndex (cfpath2, i);
			if (kar1 >= 0x00D800 && kar1 <= 0x00DBFF) {
				char32 kar2 = (char32) CFStringGetCharacterAtIndex (cfpath2, ++ i);   // convert up
				if (kar2 >= 0x00DC00 && kar2 <= 0x00DFFF) {
					kar1 = (((kar1 & 0x3FF) << 10) | (kar2 & 0x3FF)) + 0x10000;
				} else {
					kar1 = UNICODE_REPLACEMENT_CHARACTER;
				}
			}
			path32 [n_utf32 ++] = kar1;
		}
		path32 [n_utf32] = U'\0';
		CFRelease (cfpath2);
	#else
		Melder_8to32_inplace (path8, path32, kMelder_textInputEncoding::UTF8);
	#endif
}
#endif

const char32 * MelderFile_name (MelderFile file) {
	#if defined (UNIX)
		char32 *slash = str32rchr (file -> path, U'/');
		return slash ? slash + 1 : file -> path;
	#elif defined (_WIN32)
		char32 *backslash = str32rchr (file -> path, U'\\');
		return backslash ? backslash + 1 : file -> path;
	#else
		return nullptr;
	#endif
}

const char32 * MelderDir_name (MelderDir dir) {
	#if defined (UNIX)
		char32 *slash = str32rchr (dir -> path, U'/');
		return slash ? slash + 1 : dir -> path;
	#elif defined (_WIN32)
		char32 *backslash = str32rchr (dir -> path, U'\\');
		return backslash ? backslash + 1 : dir -> path;
	#else
		return nullptr;
	#endif
}

void Melder_pathToDir (const char32 *path, MelderDir dir) {
	str32cpy (dir -> path, path);
}

void Melder_pathToFile (const char32 *path, MelderFile file) {
	/*
	 * This handles complete path names only.
	 *
	 * Used if we know for sure that we have a complete path name,
	 * i.e. if the program determined the name (fileselector, printing, prefs).
	 */
	str32cpy (file -> path, path);
}

void Melder_relativePathToFile (const char32 *path, MelderFile file) {
	/*
	 * This handles complete and partial path names,
	 * and translates slashes to native directory separators.
	 *
	 * Used if we do not know for sure that we have a complete path name,
	 * i.e. if the user determined the name (scripting).
	 */
	#if defined (UNIX)
		/*
		 * We assume that Unix complete path names start with a slash.
		 */
		if (path [0] == U'~' && path [1] == U'/') {
			Melder_sprint (file -> path,kMelder_MAXPATH+1, Melder_peek8to32 (getenv ("HOME")), & path [1]);
		} else if (path [0] == U'/' || str32equ (path, U"<stdout>") || str32str (path, U"://")) {
			str32cpy (file -> path, path);
		} else {
			structMelderDir dir { };
			Melder_getDefaultDir (& dir);   // BUG
			if (dir. path [0] == U'/' && dir. path [1] == U'\0') {
				Melder_sprint (file -> path,kMelder_MAXPATH+1, U"/", path);
			} else {
				Melder_sprint (file -> path,kMelder_MAXPATH+1, dir. path, U"/", path);
			}
		}
	#elif defined (_WIN32)
		/*
		 * We assume that Win32 complete path names look like:
		 *    C:\WINDOWS\CTRL32.DLL
		 *    LPT1:
		 *    \\host\path
		 */
		structMelderDir dir { };
		if (path [0] == U'~' && path [1] == U'/') {
			Melder_getHomeDir (& dir);
			Melder_sprint (file -> path,kMelder_MAXPATH+1, dir. path, & path [1]);
			for (;;) {
				char32 *slash = str32chr (file -> path, U'/');
				if (! slash) break;
				*slash = U'\\';
			}
			return;
		}
		if (str32chr (path, U'/') && ! str32str (path, U"://")) {
			char32 winPath [kMelder_MAXPATH+1];
			Melder_sprint (winPath,kMelder_MAXPATH+1, path);
			for (;;) {
				char32 *slash = str32chr (winPath, U'/');
				if (! slash) break;
				*slash = U'\\';
			}
			Melder_relativePathToFile (winPath, file);
			return;
		}
		if (str32chr (path, U':') || path [0] == U'\\' && path [1] == U'\\' || str32equ (path, U"<stdout>")) {
			Melder_sprint (file -> path,kMelder_MAXPATH+1, path);
		} else {
			Melder_getDefaultDir (& dir);   // BUG
			Melder_sprint (file -> path,kMelder_MAXPATH+1,
				dir. path,
				dir. path [0] != U'\0' && dir. path [str32len (dir. path) - 1] == U'\\' ? U"" : U"\\",
				path);
		}
	#endif
}

const char32 * Melder_dirToPath (MelderDir dir) {
	return & dir -> path [0];
}

const char32 * Melder_fileToPath (MelderFile file) {
	return & file -> path [0];
}

void MelderFile_copy (MelderFile file, MelderFile copy) {
	str32cpy (copy -> path, file -> path);
}

void MelderDir_copy (MelderDir dir, MelderDir copy) {
	str32cpy (copy -> path, dir -> path);
}

bool MelderFile_equal (MelderFile file1, MelderFile file2) {
	return str32equ (file1 -> path, file2 -> path);
}

bool MelderDir_equal (MelderDir dir1, MelderDir dir2) {
	return str32equ (dir1 -> path, dir2 -> path);
}

void MelderFile_setToNull (MelderFile file) {
	file -> path [0] = U'\0';
}

bool MelderFile_isNull (MelderFile file) {
	return ! file || file -> path [0] == U'\0';
}

void MelderDir_setToNull (MelderDir dir) {
	dir -> path [0] = U'\0';
}

bool MelderDir_isNull (MelderDir dir) {
	return dir -> path [0] == U'\0';
}

void MelderDir_getFile (MelderDir parent, const char32 *fileName, MelderFile file) {
	#if defined (UNIX)
		if (parent -> path [0] == U'/' && parent -> path [1] == U'\0') {
			Melder_sprint (file -> path,kMelder_MAXPATH+1, U"/", fileName);
		} else {
			Melder_sprint (file -> path,kMelder_MAXPATH+1, parent -> path, U"/", fileName);
		}
	#elif defined (_WIN32)
		if (str32rchr (file -> path, U'\\') - file -> path == str32len (file -> path) - 1) {
			Melder_sprint (file -> path,kMelder_MAXPATH+1, parent -> path, fileName);
		} else {
			Melder_sprint (file -> path,kMelder_MAXPATH+1, parent -> path, U"\\", fileName);
		}
	#endif
}

void MelderDir_relativePathToFile (MelderDir dir, const char32 *path, MelderFile file) {
	structMelderDir saveDir { };
	Melder_getDefaultDir (& saveDir);
	Melder_setDefaultDir (dir);
	Melder_relativePathToFile (path, file);
	Melder_setDefaultDir (& saveDir);
}

#ifndef UNIX
static void Melder_getDesktop (MelderDir dir) {
	dir -> path [0] = U'\0';
}
#endif

void MelderFile_getParentDir (MelderFile file, MelderDir parent) {
	#if defined (UNIX)
		/*
		 * The parent of /usr/hello.txt is /usr.
		 * The parent of /hello.txt is /.
		 */
		char32 *slash;
		str32cpy (parent -> path, file -> path);
		slash = str32rchr (parent -> path, U'/');
		if (slash) *slash = U'\0';
		if (parent -> path [0] == U'\0') str32cpy (parent -> path, U"/");
	#elif defined (_WIN32)
		/*
		 * The parent of C:\WINDOWS\CTRL.DLL is C:\WINDOWS.
		 * The parent of E:\Praat.exe is E:\.
		 * The parent of \\Swine\Apps\init.txt is \\Swine\Apps.
		 * The parent of \\Swine\init.txt is \\Swine\.   (BUG ?)
		 */
		char32 *colon;
		str32cpy (parent -> path, file -> path);
		colon = str32chr (parent -> path, U':');
		if (colon) {
			char32 *backslash = str32rchr (parent -> path, U'\\');
			if (backslash) {   //   C:\WINDOWS\CTRL.DLL or C:\AUTOEXEC.BAT
				if (backslash - colon == 1) {   //   C:\AUTOEXEC.BAT
					* (backslash + 1) = U'\0';   //   C:\   -   !!! aargh this was a bug after converting this line to line comments
				} else {   //   C:\WINDOWS\CTRL.DLL
					*backslash = U'\0';   //   C:\WINDOWS
				}
			} else {   /* ??? */
				Melder_getDesktop (parent);   // empty string
			}
		} else if (parent -> path [0] == U'\\' && parent -> path [1] == U'\\') {
			char32 *backslash = str32rchr (parent -> path + 2, U'\\');
			if (backslash) {   //   \\Swine\Apps\init.txt or \\Swine\init.txt
				char32 *leftBackslash = str32chr (parent -> path + 2, U'\\');
				if (backslash - leftBackslash == 0) {   //   \\Swine\init.txt
					* (backslash + 1) = U'\0';   //   \\Swine\   -
				} else {   //   \\Swine\Apps\init.txt
					*backslash = U'\0';   //   \\Swine\Apps
				}
			} else {   //   \\init.txt   ???
				Melder_getDesktop (parent);   // empty string
			}
		} else {   // unknown path type
			Melder_getDesktop (parent);   // empty string
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
		char32 *slash;
		str32cpy (parent -> path, dir -> path);
		slash = str32rchr (parent -> path, U'/');
		if (slash) {
			if (slash - parent -> path == 0) {
				if (slash [1] == U'\0') {   // child is "/"
					parent -> path [0] = U'\0';   // parent is ""
				} else {   // child is "/usr"
					slash [1] = '\0';   // parent is "/"
				}
			} else {   // child is "/usr/local"
				*slash = U'\0';   // parent is "/usr"
			}
		} else {
			parent -> path [0] = U'\0';   // some failure; desktop
		}
	#elif defined (_WIN32)
		/*
		 * The parent of C:\WINDOWS is C:\.
		 * The parent of E:\ is the desktop.
		 * The parent of \\Swine\ is the desktop.   (BUG ?)
		 */
		char32 *colon;
		str32cpy (parent -> path, dir -> path);
		colon = str32chr (parent -> path, U':');
		if (colon) {
			int length = str32len (parent -> path);
			char32 *backslash = str32rchr (parent -> path, U'\\');
			if (backslash) {   //   C:\WINDOWS\FONTS or C:\WINDOWS or C:\   - (cannot add a line comment with a backslash)
				if (backslash - parent -> path == length - 1) {   //   C:\   -
					Melder_getDesktop (parent);   // empty string
				} else if (backslash - colon == 1) {   //   C:\WINDOWS
					* (backslash + 1) = U'\0';   //   C:\   -
				} else {   //   C:\WINDOWS\FONTS
					*backslash = U'\0';   //   C:\WINDOWS
				}
			} else {   //   LPT1:   ???
				Melder_getDesktop (parent);   // empty string
			}
		} else if (parent -> path [0] == U'\\' && parent -> path [1] == U'\\') {
			int length = str32len (parent -> path);
			char32 *backslash = str32rchr (parent -> path + 2, U'\\');
			if (backslash) {   //   \\Swine\Apps\Praats or \\Swine\Apps or \\Swine\   -
				if (backslash - parent -> path == length - 1) {   //   \\Swine\   -
					Melder_getDesktop (parent);   // empty string
				} else {   //   \\Swine\Apps\Praats or \\Swine\Apps
					char32 *leftBackslash = str32chr (parent -> path + 2, U'\\');
					if (backslash - leftBackslash == 0) {   //   \\Swine\Apps
						* (backslash + 1) = U'\0';   //   \\Swine\   -
					} else {   //   \\Swine\Apps\Praats
						*backslash = U'\0';   //   \\Swine\Apps
					}
				}
			} else {   //   \\Swine   ???
				Melder_getDesktop (parent);   // empty string
			}
		} else {   // unknown path type.
			Melder_getDesktop (parent);   // empty string
		}
	#endif
}

bool MelderDir_isDesktop (MelderDir dir) {
	return dir -> path [0] == U'\0';
}

void MelderDir_getSubdir (MelderDir parent, const char32 *subdirName, MelderDir subdir) {
	#if defined (UNIX)
		if (parent -> path [0] == U'/' && parent -> path [1] == U'\0') {
			Melder_sprint (subdir -> path,kMelder_MAXPATH+1, U"/", subdirName);
		} else {
			Melder_sprint (subdir -> path,kMelder_MAXPATH+1, parent -> path, U"/", subdirName);
		}
	#elif defined (_WIN32)
		int length = str32len (parent -> path);
		char32 *backslash = str32rchr (parent -> path, U'\\');
		if (backslash && backslash - parent -> path == length - 1) {   //   C:\ or \\Swine\   -
			Melder_sprint (subdir -> path, kMelder_MAXPATH+1, parent -> path, subdirName);
		} else {   //   C:\WINDOWS or \\Swine\Apps or even C:
			Melder_sprint (subdir -> path,kMelder_MAXPATH+1, parent -> path, U"\\", subdirName);
		}
	#endif
}

void Melder_getHomeDir (MelderDir homeDir) {
	#if defined (UNIX)
		char *home = getenv ("HOME");
		str32cpy (homeDir -> path, home ? Melder_peek8to32 (home) : U"/");
	#elif defined (_WIN32)
		WCHAR driveW [kMelder_MAXPATH+1], pathW [kMelder_MAXPATH+1];
		DWORD n = GetEnvironmentVariableW (L"USERPROFILE", pathW, kMelder_MAXPATH+1);
		if (n > kMelder_MAXPATH) Melder_throw (U"Home directory name too long.");
		if (n > 0) {
			Melder_sprint (homeDir -> path,kMelder_MAXPATH+1, Melder_peekWto32 (pathW));
			return;
		}
		n = GetEnvironmentVariableW (L"HOMEDRIVE", driveW, kMelder_MAXPATH+1);
		if (n > kMelder_MAXPATH) Melder_throw (U"Home drive name too long.");
		if (n > 0) {
			GetEnvironmentVariable (L"HOMEPATH", pathW, kMelder_MAXPATH+1);
			Melder_sprint (homeDir -> path,kMelder_MAXPATH+1, Melder_peekWto32 (driveW), Melder_peekWto32 (pathW));
			return;
		}
		MelderDir_setToNull (homeDir);   // Windows 95 and 98: alas
	#endif
}

void Melder_getPrefDir (MelderDir prefDir) {
	#if defined (macintosh)
		structMelderDir homeDir { };
		Melder_getHomeDir (& homeDir);
		Melder_sprint (prefDir -> path,kMelder_MAXPATH+1, homeDir. path, U"/Library/Preferences");
	#elif defined (UNIX)
		/*
		 * Preferences files go into the home directory.
		 */
		Melder_getHomeDir (prefDir);
	#elif defined (_WIN32)
		/*
		 * On Windows 95, preferences files went in the Windows directory.
		 * On shared systems (NT, 2000, XP), preferences files go into the home directory.
		 * TODO: at some point, these files should be moved to HOME\AppData\Roaming\Praat.
		 */
		Melder_getHomeDir (prefDir);
	#endif
}

void Melder_getTempDir (MelderDir tempDir) {
	#if defined (macintosh)
		Melder_sprint (tempDir -> path,kMelder_MAXPATH+1, Melder_peek8to32 (getenv ("TMPDIR")));   // or append /TemporaryItems
		// confstr with _CS_DARWIN_USER_TEMP_DIR
	#else
		(void) tempDir;
	#endif
}

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
	if (MelderFile_isNull (file)) Melder_throw (U"Cannot open null file.");
	if (! Melder_isTracing)
		Melder_assert (str32equ (Melder_double (1.5), U"1.5"));   // check locale settings; because of the required file portability Praat cannot stand "1,5"
	/*
	 * On the Unix-like systems (including MacOS), the path has to be converted to 8-bit characters in UTF-8 encoding.
	 * On MacOS, the characters also have to be decomposed.
	 */
	char utf8path [kMelder_MAXPATH+1];
	Melder_str32To8bitFileRepresentation_inplace (file -> path, utf8path);
	FILE *f;
	file -> openForWriting = ( type [0] == 'w' || type [0] == 'a' || strchr (type, '+') );
	if (str32equ (file -> path, U"<stdout>") && file -> openForWriting) {
		f = stdout;
	#ifdef CURLPRESENT
	} else if (strstr (utf8path, "://") && file -> openForWriting) {
		Melder_assert (type [0] == 'w');   // reject "append" and "random" access
		f = tmpfile ();   // open a temporary file for writing
	} else if (strstr (utf8path, "://") && ! file -> openForWriting) {
		CURLcode CURLreturn;
		CURL *CURLhandle;
		char errorbuffer [CURL_ERROR_SIZE] = "";
		f = tmpfile ();   // open a temporary file for writing
		if (! curl_initialized) {
			CURLreturn = curl_global_init (CURL_GLOBAL_ALL);
			curl_initialized = 1;
		};
		CURLhandle = curl_easy_init ();   // initialize session
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
			Melder_appendError (Melder_peek8to32 (errorbuffer));
			f = nullptr;
		};
		/* Clean up session. */
		curl_easy_cleanup (CURLhandle);
		/* Do something with the file. Why? */
		if (f) rewind (f);
	#endif
	} else {
		#if defined (_WIN32) && ! defined (__CYGWIN__)
			f = _wfopen (Melder_peek32toW (file -> path), Melder_peek32toW (Melder_peek8to32 (type)));
		#else
			f = fopen ((char *) utf8path, type);
		#endif
	}
	if (! f) {
		char32 *path = file -> path;
		Melder_appendError (U"Cannot ", type [0] == 'r' ? U"open" : type [0] == 'a' ? U"append to" : U"create",
			U" file ", file, U".");
		if (path [0] == U'\0')
			Melder_appendError (U"Hint: empty file name.");
		else if (path [0] == U' ' || path [0] == U'\t')
			Melder_appendError (U"Hint: file name starts with a space or tab.");
		else if (path [str32len (path) - 1] == U' ' || path [str32len (path) - 1] == U'\t')
			Melder_appendError (U"Hint: file name ends in a space or tab.");
		else if (str32chr (path, U'\n'))
			Melder_appendError (U"Hint: file name contains a newline symbol.");
		throw MelderError ();
		return nullptr;
	}
	return f;
}

void Melder_fclose (MelderFile file, FILE *f) {
	if (! f) return;
	#if defined (CURLPRESENT)
 	if (str32str (file -> wpath, U"://") && file -> openForWriting) {
		unsigned char utf8path [kMelder_MAXPATH+1];
		Melder_str32To8bitFileRepresentation_inplace (file -> path, utf8path);
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
		if (str32str (file -> path, U"file://") || str32str (file -> path, U"FILE://")) {
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
			curl_easy_cleanup (CURLhandle);
			f = nullptr;
			Melder_throw (Melder_peek8to32 (errorbuffer), U"\n");
	    };
		/* Clean up session */
		curl_easy_cleanup (CURLhandle);
    }
	#endif
	if (f != stdout && fclose (f) == EOF)
		Melder_throw (U"Error closing file ", file, U".");
}

void Melder_files_cleanUp () {
	#if defined (CURLPRESENT)
		if (curl_initialized) {
			curl_global_cleanup ();
			curl_initialized = 0;
		};
	#endif
}

bool MelderFile_exists (MelderFile file) {
	#if defined (UNIX)
		char utf8path [kMelder_MAXPATH+1];
		Melder_str32To8bitFileRepresentation_inplace (file -> path, utf8path);
		struct stat statistics;
		return ! stat (utf8path, & statistics);
	#else
		try {
			autofile f = Melder_fopen (file, "rb");
			f.close (file);
			return true;
		} catch (MelderError) {
			Melder_clearError ();
			return false;
		}
	#endif
}

bool MelderFile_readable (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		f.close (file);
		return true;
	} catch (MelderError) {
		Melder_clearError ();
		return false;
	}
}

integer MelderFile_length (MelderFile file) {
	#if defined (UNIX)
		char utf8path [kMelder_MAXPATH+1];
		Melder_str32To8bitFileRepresentation_inplace (file -> path, utf8path);
		struct stat statistics;
		if (stat ((char *) utf8path, & statistics) != 0) return -1;
		return statistics. st_size;
	#else
		try {
			autofile f = Melder_fopen (file, "r");
			fseek (f, 0, SEEK_END);
			integer length = ftell (f);
			f.close (file);
			return length;
		} catch (MelderError) {
			Melder_clearError ();
			return -1;
		}
	#endif
}

void MelderFile_delete (MelderFile file) {
	if (! file) return;
	#if defined (UNIX)
		char utf8path [kMelder_MAXPATH+1];
		Melder_str32To8bitFileRepresentation_inplace (file -> path, utf8path);
		remove ((char *) utf8path);
	#elif defined (_WIN32)
		DeleteFile (Melder_peek32toW (file -> path));
	#endif
}

char32 * Melder_peekExpandBackslashes (const char32 *message) {
	static char32 names [11] [kMelder_MAXPATH+1];
	static int index = 0;
	const char32 *from;
	char32 *to;
	if (++ index == 11) index = 0;
	for (from = & message [0], to = & names [index] [0]; *from != '\0'; from ++, to ++) {
		*to = *from;
		if (*from == U'\\') { * ++ to = U'b'; * ++ to = U's'; }
	}
	*to = U'\0';
	return & names [index] [0];
}

const char32 * MelderFile_messageName (MelderFile file) {
	return Melder_cat (U_LEFT_DOUBLE_QUOTE, file -> path, U_RIGHT_DOUBLE_QUOTE);   // BUG: is cat allowed here?
}

void Melder_getDefaultDir (MelderDir dir) {
	#if defined (UNIX)
		char path [kMelder_MAXPATH+1];
		getcwd (path, kMelder_MAXPATH+1);
		Melder_8bitFileRepresentationToStr32_inplace (path, dir -> path);
	#elif defined (_WIN32)
		static WCHAR dirPathW [kMelder_MAXPATH+1];
		GetCurrentDirectory (kMelder_MAXPATH+1, dirPathW);
		Melder_sprint (dir -> path,kMelder_MAXPATH+1, Melder_peekWto32 (dirPathW));
	#endif
}

void Melder_setDefaultDir (MelderDir dir) {
	#if defined (UNIX)
		chdir (Melder_peek32to8 (dir -> path));
	#elif defined (_WIN32)
		SetCurrentDirectory (Melder_peek32toW (dir -> path));
	#endif
}

void MelderFile_setDefaultDir (MelderFile file) {
	structMelderDir dir { };
	MelderFile_getParentDir (file, & dir);
	Melder_setDefaultDir (& dir);
}

void Melder_createDirectory (MelderDir parent, const char32 *dirName, int mode) {
#if defined (UNIX)
	structMelderFile file { };
	if (dirName [0] == U'/') {
		Melder_sprint (file. path,kMelder_MAXPATH+1, dirName);   // absolute path
	} else if (parent -> path [0] == U'/' && parent -> path [1] == U'\0') {
		Melder_sprint (file. path,kMelder_MAXPATH+1, U"/", dirName);   // relative path in root directory
	} else {
		Melder_sprint (file. path,kMelder_MAXPATH+1, parent -> path, U"/", dirName);   // relative path
	}
	char utf8path [kMelder_MAXPATH+1];
	Melder_str32To8bitFileRepresentation_inplace (file. path, utf8path);
	if (mkdir (utf8path, mode) == -1 && errno != EEXIST)   // ignore if directory already exists
		Melder_throw (U"Cannot create directory ", & file, U".");
#elif defined (_WIN32)
	structMelderFile file { };
	SECURITY_ATTRIBUTES sa;
	(void) mode;
	sa. nLength = sizeof (SECURITY_ATTRIBUTES);
	sa. lpSecurityDescriptor = nullptr;
	sa. bInheritHandle = false;
	if (str32chr (dirName, U':') || dirName [0] == U'/' && dirName [1] == U'/') {
		Melder_sprint (file. path,kMelder_MAXPATH+1, dirName);   // absolute path
	} else {
		Melder_sprint (file. path,kMelder_MAXPATH+1, parent -> path, U"/", dirName);   // relative path
	}
	if (! CreateDirectoryW (Melder_peek32toW (file. path), & sa) && GetLastError () != ERROR_ALREADY_EXISTS)   // ignore if directory already exists
		Melder_throw (U"Cannot create directory ", & file, U".");
#else
	//#error Unsupported operating system.
#endif
}

/*
 * Routines for wrapping the file pointers.
 */

MelderFile MelderFile_open (MelderFile me) {
	my filePointer = Melder_fopen (me, "rb");
	my openForReading = true;
	return me;
}

char * MelderFile_readLine (MelderFile me) {
	static char *buffer;
	static integer capacity;
	if (! my filePointer) return nullptr;
	if (feof (my filePointer)) return nullptr;
	if (! buffer) {
		buffer = Melder_malloc (char, capacity = 100);
	}
	integer i = 0;
	for (; true; i ++) {
		if (i >= capacity) {
			buffer = (char *) Melder_realloc (buffer, capacity *= 2);
		}
		int c = fgetc (my filePointer);
		if (feof (my filePointer))
			break;
		if (c == '\n') {
			c = fgetc (my filePointer);
			if (feof (my filePointer)) break;   // ignore last empty line (Unix)
			ungetc (c, my filePointer);
			break;   // Unix line separator
		}
		if (c == '\r') {
			c = fgetc (my filePointer);
			if (feof (my filePointer)) break;   // ignore last empty line (Macintosh)
			if (c == '\n') {
				c = fgetc (my filePointer);
				if (feof (my filePointer)) break;   // ignore last empty line (Windows)
				ungetc (c, my filePointer);
				break;   // Windows line separator
			}
			ungetc (c, my filePointer);
			break;   // Macintosh line separator
		}
		buffer [i] = c;
	}
	buffer [i] = '\0';
	return buffer;
}

MelderFile MelderFile_create (MelderFile me) {
	my filePointer = Melder_fopen (me, "wb");
	my openForWriting = true;   // a bit superfluous (will have been set by Melder_fopen)
	return me;
}

void MelderFile_seek (MelderFile me, integer position, int direction) {
	if (! my filePointer) return;
	if (fseek (my filePointer, position, direction)) {
		fclose (my filePointer);
		my filePointer = nullptr;
		Melder_throw (U"Cannot seek in file ", me, U".");
	}
}

integer MelderFile_tell (MelderFile me) {
	if (! my filePointer) return 0;
	integer result = ftell (my filePointer);
	if (result == -1) {
		fclose (my filePointer);
		my filePointer = nullptr;
		Melder_throw (U"Cannot tell in file ", me, U".");
	}
	return result;
}

void MelderFile_rewind (MelderFile me) {
	if (! my filePointer) return;
	rewind (my filePointer);
}

static void _MelderFile_close (MelderFile me, bool mayThrow) {
	if (my outputEncoding == kMelder_textOutputEncoding_FLAC) {
		if (my flacEncoder) {
			FLAC__stream_encoder_finish (my flacEncoder);   // This already calls fclose! BUG: we cannot get any error messages out.
			FLAC__stream_encoder_delete (my flacEncoder);
		}
	} else if (my filePointer) {
		if (mayThrow) {
			Melder_fclose (me, my filePointer);
		} else {
			fclose (my filePointer);
		}
	}
	/* Set everything to zero, except paths (they stay around for error messages and the like). */
	my filePointer = nullptr;
	my openForWriting = my openForReading = false;
	my indent = 0;
	my flacEncoder = nullptr;
}
void MelderFile_close (MelderFile me) {
	_MelderFile_close (me, true);
}
void MelderFile_close_nothrow (MelderFile me) {
	_MelderFile_close (me, false);
}

/* End of file melder_files.cpp */
