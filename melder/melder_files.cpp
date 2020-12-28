/* melder_files.cpp
 *
 * Copyright (C) 1992-2008,2010-2020 Paul Boersma, 2013 Tom Naughton
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
#include "melder.h"
#include "../kar/UnicodeData.h"

//#include "flac_FLAC_stream_encoder.h"
extern "C" int  FLAC__stream_encoder_finish (FLAC__StreamEncoder *);
extern "C" void FLAC__stream_encoder_delete (FLAC__StreamEncoder *);

#if defined (macintosh)
	#include <sys/stat.h>
	#define UNIX
	#include <unistd.h>
#endif

static char32 theShellDirectory [kMelder_MAXPATH+1];
void Melder_rememberShellDirectory () {
	structMelderDir shellDir { };
	Melder_getDefaultDir (& shellDir);
	str32cpy (theShellDirectory, Melder_dirToPath (& shellDir));
}
conststring32 Melder_getShellDirectory () {
	return & theShellDirectory [0];
}

#if defined (UNIX)
void Melder_8bitFileRepresentationToStr32_inplace (const char *path8, char32 *path32) {
	#if defined (macintosh)
		CFStringRef cfpath = CFStringCreateWithCString (nullptr, path8, kCFStringEncodingUTF8);
		if (! cfpath) {
			/*
				Probably something wrong, like a disk was disconnected in the meantime.
			*/
			try {
				Melder_8to32_inplace (path8, path32, kMelder_textInputEncoding::UTF8);
			} catch (MelderError) {
				Melder_8to32_inplace (path8, path32, kMelder_textInputEncoding::MACROMAN);   // cannot fail
				Melder_throw (U"Unusual error finding or creating file <<", path32, U">> (MacRoman).");
			}
			Melder_throw (U"Unusual error finding or creating file ", path32, U".");
		}
		CFMutableStringRef cfpath2 = CFStringCreateMutableCopy (nullptr, 0, cfpath);
		CFRelease (cfpath);
		CFStringNormalize (cfpath2, kCFStringNormalizationFormC);   // Praat requires composed characters
		integer n_utf16 = CFStringGetLength (cfpath2);
		integer n_utf32 = 0;
		for (integer i = 0; i < n_utf16; i ++) {
			char32 kar1 = CFStringGetCharacterAtIndex (cfpath2, i);
			if (kar1 >= 0x00'D800 && kar1 <= 0x00'DBFF) {
				char32 kar2 = (char32) CFStringGetCharacterAtIndex (cfpath2, ++ i);   // convert up
				if (kar2 >= 0x00'DC00 && kar2 <= 0x00'DFFF) {
					kar1 = (((kar1 & 0x3FF) << 10) | (kar2 & 0x3FF)) + 0x01'0000;
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

conststring32 MelderFile_name (MelderFile file) {
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

conststring32 MelderDir_name (MelderDir dir) {
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

void Melder_pathToDir (conststring32 path, MelderDir dir) {
	Melder_sprint (dir -> path,kMelder_MAXPATH+1, path);
}

void Melder_pathToFile (conststring32 path, MelderFile file) {
	/*
	 * This handles complete path names only.
	 *
	 * Used if we know for sure that we have a complete path name,
	 * i.e. if the program determined the name (fileselector, printing, prefs).
	 */
	Melder_sprint (file -> path,kMelder_MAXPATH+1, path);
}

void Melder_relativePathToFile (conststring32 path, MelderFile file) {
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
			Melder_sprint (file -> path,kMelder_MAXPATH+1, path);
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
				if (! slash)
					break;
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

conststring32 Melder_dirToPath (MelderDir dir) {
	return & dir -> path [0];
}

conststring32 Melder_fileToPath (MelderFile file) {
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

void MelderDir_getFile (MelderDir parent, conststring32 fileName, MelderFile file) {
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

void MelderDir_relativePathToFile (MelderDir dir, conststring32 path, MelderFile file) {
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
			The parent of /usr/hello.txt is /usr.
			The parent of /hello.txt is /.
		*/
		str32cpy (parent -> path, file -> path);
		char32 *slash = str32rchr (parent -> path, U'/');
		if (slash)
			*slash = U'\0';
		if (parent -> path [0] == U'\0')
			str32cpy (parent -> path, U"/");
	#elif defined (_WIN32)
		/*
			The parent of C:\WINDOWS\CTRL.DLL is C:\WINDOWS.
			The parent of E:\Praat.exe is E:\.
			The parent of \\Swine\Apps\init.txt is \\Swine\Apps.
			The parent of \\Swine\init.txt is \\Swine\.   (BUG ?)
		*/
		str32cpy (parent -> path, file -> path);
		char32 *colon = str32chr (parent -> path, U':');
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
					* (backslash + 1) = U'\0';   //   \\Swine\   -   !!! dear developer, don't delete this hyphen, lest the line ends in a backslash
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
			The parent of /usr/local is /usr.
			The parent of /usr is /.
			The parent of / is "".
		*/
		str32cpy (parent -> path, dir -> path);
		char32 *slash = str32rchr (parent -> path, U'/');
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
			The parent of C:\WINDOWS is C:\.
			The parent of E:\ is the desktop.
			The parent of \\Swine\ is the desktop.   (BUG ?)
		*/
		str32cpy (parent -> path, dir -> path);
		char32 *colon = str32chr (parent -> path, U':');
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

void MelderDir_getSubdir (MelderDir parent, conststring32 subdirName, MelderDir subdir) {
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
		Melder_sprint (homeDir -> path,kMelder_MAXPATH+1, home ? Melder_peek8to32 (home) : U"/");
	#elif defined (_WIN32)
		WCHAR driveW [kMelder_MAXPATH+1], pathW [kMelder_MAXPATH+1];
		DWORD n = GetEnvironmentVariableW (L"USERPROFILE", pathW, kMelder_MAXPATH+1);
		if (n > kMelder_MAXPATH) Melder_throw (U"Home directory name too long.");
		if (n > 0) {
			Melder_sprint (homeDir -> path,kMelder_MAXPATH+1, Melder_peekWto32 (pathW));
			return;
		}
		n = GetEnvironmentVariableW (L"HOMEDRIVE", driveW, kMelder_MAXPATH+1);
		if (n > kMelder_MAXPATH)
			Melder_throw (U"Home drive name too long.");
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
	 * On Windows, the characters have to be precomposed.
	 */
	char utf8path [kMelder_MAXPATH+1];
	Melder_32to8_fileSystem_inplace (file -> path, utf8path);
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
			f = _wfopen (Melder_peek32toW_fileSystem (file -> path), Melder_peek32toW (Melder_peek8to32 (type)));
		#else
			f = fopen ((char *) utf8path, type);
		#endif
	}
	if (! f) {
		char32 *path = file -> path;
		Melder_appendError (
			( errno == EPERM ? U"No permission to " : U"Cannot " ),
			( type [0] == 'r' ? U"open" : type [0] == 'a' ? U"append to" : U"create" ),
			U" file ", file, U"."
		);
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
		Melder_32to8_fileSystem_inplace (file -> path, utf8path);
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
		struct stat statistics;
		return ! stat (Melder_peek32to8_fileSystem (file -> path), & statistics);
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

bool Melder_tryToWriteFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "wb");
		f.close (file);
		return true;
	} catch (MelderError) {
		Melder_clearError ();
		return false;
	}
}

bool Melder_tryToAppendFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "ab");
		f.close (file);
		return true;
	} catch (MelderError) {
		Melder_clearError ();
		return false;
	}
}

integer MelderFile_length (MelderFile file) {
	#if defined (UNIX)
		struct stat statistics;
		if (stat (Melder_peek32to8_fileSystem (file -> path), & statistics) != 0)
			return -1;
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
		remove (Melder_peek32to8_fileSystem (file -> path));
	#elif defined (_WIN32)
		DeleteFile (Melder_peek32toW_fileSystem (file -> path));
	#endif
}

char32 * Melder_peekExpandBackslashes (conststring32 message) {
	static char32 names [11] [kMelder_MAXPATH+1];
	static int index = 0;
	if (++ index == 11) index = 0;
	char32 *to = & names [index] [0];
	for (const char32 *from = & message [0]; *from != '\0'; from ++, to ++) {
		*to = *from;
		if (*from == U'\\') {
			* ++ to = U'b';
			* ++ to = U's';
		}
	}
	*to = U'\0';
	return & names [index] [0];
}

conststring32 MelderFile_messageName (MelderFile file) {
	return Melder_cat (U"“", file -> path, U"”");   // BUG: is cat allowed here?
}

#if defined (UNIX)
	/*
		From macOS 10.15 Catalina on, getcwd() has failed if a part of the path
		is inaccessible, such as when you open a script that is attached to an email message.
	*/
	static structMelderDir theDefaultDir;
#endif

void Melder_getDefaultDir (MelderDir dir) {
	#if defined (UNIX)
		char path [kMelder_MAXPATH+1];
		char *pathResult = getcwd (path, kMelder_MAXPATH+1);
		if (pathResult)
			Melder_8bitFileRepresentationToStr32_inplace (path, dir -> path);
		else if (errno == EPERM)
			str32cpy (dir -> path, theDefaultDir. path);
		else
			Melder_throw (Melder_peek8to32 (strerror (errno)));
		Melder_assert (str32len (dir -> path) <= kMelder_MAXPATH);
	#elif defined (_WIN32)
		static WCHAR dirPathW [kMelder_MAXPATH+1];
		GetCurrentDirectory (kMelder_MAXPATH+1, dirPathW);
		Melder_sprint (dir -> path,kMelder_MAXPATH+1, Melder_peekWto32 (dirPathW));
	#endif
}

void Melder_setDefaultDir (MelderDir dir) {
	#if defined (UNIX)
		chdir (Melder_peek32to8 (dir -> path));
		str32cpy (theDefaultDir. path, dir -> path);
	#elif defined (_WIN32)
		SetCurrentDirectory (Melder_peek32toW_fileSystem (dir -> path));
	#endif
}

void MelderFile_setDefaultDir (MelderFile file) {
	structMelderDir dir { };
	MelderFile_getParentDir (file, & dir);
	Melder_setDefaultDir (& dir);
}

void Melder_createDirectory (MelderDir parent, conststring32 dirName, int mode) {
#if defined (UNIX)
	structMelderFile file { };
	if (dirName [0] == U'/') {
		Melder_sprint (file. path,kMelder_MAXPATH+1, dirName);   // absolute path
	} else if (parent -> path [0] == U'/' && parent -> path [1] == U'\0') {
		Melder_sprint (file. path,kMelder_MAXPATH+1, U"/", dirName);   // relative path in root directory
	} else {
		Melder_sprint (file. path,kMelder_MAXPATH+1, parent -> path, U"/", dirName);   // relative path
	}
	if (mkdir (Melder_peek32to8_fileSystem (file. path), mode) == -1 && errno != EEXIST)   // ignore if directory already exists
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
	if (! CreateDirectoryW (Melder_peek32toW_fileSystem (file. path), & sa) && GetLastError () != ERROR_ALREADY_EXISTS)   // ignore if directory already exists
		Melder_throw (U"Cannot create directory ", & file, U".");
#else
	//#error Unsupported operating system.
#endif
}

static size_t fread_multi (char *buffer, size_t numberOfBytes, FILE *f) {
	off_t offset = 0;
	size_t numberOfBytesRead = 0;
	const size_t chunkSize = 1'000'000'000;
	while (numberOfBytes > chunkSize) {
		size_t numberOfBytesReadInChunk = fread (buffer + offset, sizeof (char), chunkSize, f);
		numberOfBytesRead += numberOfBytesReadInChunk;
		if (numberOfBytesReadInChunk < chunkSize)
			return numberOfBytesRead;
		numberOfBytes -= chunkSize;
		offset += chunkSize;
	}
	size_t numberOfBytesReadInLastChunk = fread (buffer + offset, sizeof (char), numberOfBytes, f);
	numberOfBytesRead += numberOfBytesReadInLastChunk;
	return numberOfBytesRead;
}

autostring32 MelderFile_readText (MelderFile file, autostring8 *string8) {
	try {
		int type = 0;   // 8-bit
		autostring32 text;
		autofile f = Melder_fopen (file, "rb");
		if (fseeko (f, 0, SEEK_END) < 0)
			Melder_throw (U"Cannot count the bytes in the file.");
		Melder_assert (sizeof (off_t) >= 8);
		int64 length = ftello (f);
		rewind (f);
		if (length >= 2) {
			int firstByte = fgetc (f), secondByte = fgetc (f);
			if (firstByte == 0xFE && secondByte == 0xFF) {
				type = 1;   // big-endian 16-bit
			} else if (firstByte == 0xFF && secondByte == 0xFE) {
				type = 2;   // little-endian 16-bit
			} else if (firstByte == 0xEF && secondByte == 0xBB && length >= 3) {
				int thirdByte = fgetc (f);
				if (thirdByte == 0xBF)
					type = -1;   // UTF-8 with BOM
			}
		}
		if (type <= 0) {
			if (type == -1) {
				length -= 3;
				fseeko (f, 3, SEEK_SET);
			} else {
				rewind (f);   // length and type already set correctly.
			}
			autostring8 text8bit (length);
			Melder_assert (text8bit);
			size_t numberOfBytesRead = fread_multi (text8bit.get(), (size_t) length, f);
			Melder_require ((int64) numberOfBytesRead == length,
				U"The file contains ", length, U" bytes",
				type == -1 ? U" after the byte-order mark" : U"",
				U", but we could read only ", numberOfBytesRead, U" of them."
			);
			text8bit [length] = '\0';
			/*
				Count and repair null bytes.
			*/
			if (length > 0) {
				int64 numberOfNullBytes = 0;
				char *q = & text8bit [0];
				for (integer i = 0; i < length; i ++)
					if (text8bit [i] != '\0')
						* (q ++) = text8bit [i];
					else
						numberOfNullBytes ++;
				*q = '\0';
				if (numberOfNullBytes > 0)
					Melder_warning (U"Ignored ", numberOfNullBytes, U" null bytes in text file ", file, U".");
			}
			if (string8) {
				*string8 = text8bit.move();
				(void) Melder_killReturns_inplace (string8->get());
				return autostring32();   // OK
			} else {
				text = Melder_8to32 (text8bit.get(), kMelder_textInputEncoding::UNDEFINED);
			}
		} else {
			length = length / 2 - 1;   // Byte Order Mark subtracted. Length = number of UTF-16 codes
			text = autostring32 (length + 1);
			if (type == 1) {
				for (int64 i = 0; i < length; i ++) {
					char16 kar1 = bingetu16 (f);
					if (kar1 < 0xD800) {
						text [i] = (char32) kar1;   // convert up without sign extension
					} else if (kar1 < 0xDC00) {
						length --;
						char16 kar2 = bingetu16 (f);
						if (kar2 >= 0xDC00 && kar2 <= 0xDFFF) {
							text [i] = (char32) (0x010000 +
									(char32) (((char32) kar1 & 0x0003FF) << 10) +
									(char32)  ((char32) kar2 & 0x0003FF));
						} else {
							text [i] = UNICODE_REPLACEMENT_CHARACTER;
						}
					} else if (kar1 < 0xE000) {
						text [i] = UNICODE_REPLACEMENT_CHARACTER;
					} else {
						text [i] = (char32) kar1;   // convert up without sign extension
					}
				}
			} else {
				for (int64 i = 0; i < length; i ++) {
					char16 kar1 = bingetu16LE (f);
					if (kar1 < 0xD800) {
						text [i] = (char32) kar1;   // convert up without sign extension
					} else if (kar1 < 0xDC00) {
						length --;
						char16 kar2 = bingetu16LE (f);
						if (kar2 >= 0xDC00 && kar2 <= 0xDFFF) {
							text [i] = (char32) (0x01'0000 +
								(char32) (((char32) kar1 & 0x00'03FF) << 10) +
								(char32)  ((char32) kar2 & 0x00'03FF));
						} else {
							text [i] = UNICODE_REPLACEMENT_CHARACTER;
						}
					} else if (kar1 < 0xE000) {
						text [i] = UNICODE_REPLACEMENT_CHARACTER;
					} else if (kar1 <= 0xFFFF) {
						text [i] = (char32) kar1;   // convert up without sign extension
					} else {
						Melder_fatal (U"MelderFile_readText: unsigned short greater than 0xFFFF: should not occur.");
					}
				}
			}
			text [length] = U'\0';
			(void) Melder_killReturns_inplace (text.get());
		}
		f.close (file);
		return text;
	} catch (MelderError) {
		Melder_throw (U"Error reading file ", file, U".");
	}
}

void Melder_fwrite32to8 (conststring32 string, FILE *f) {
	/*
	 * Precondition:
	 *    the string's encoding is UTF-32.
	 * Failure:
	 *    if the precondition does not hold, we don't crash,
	 *    but the characters that are written may be incorrect.
	 */
	for (const char32* p = string; *p != U'\0'; p ++) {
		char32 kar = *p;
		if (kar <= 0x00'007F) {
			#ifdef _WIN32
				if (kar == U'\n')
					fputc (13, f);
			#endif
			fputc ((int) kar, f);   // because fputc wants an int instead of an uint8 (guarded conversion)
		} else if (kar <= 0x00'07FF) {
			fputc (0xC0 | (kar >> 6), f);
			fputc (0x80 | (kar & 0x00'003F), f);
		} else if (kar <= 0x00FFFF) {
			fputc (0xE0 | (kar >> 12), f);
			fputc (0x80 | ((kar >> 6) & 0x00'003F), f);
			fputc (0x80 | (kar & 0x00'003F), f);
		} else {
			fputc (0xF0 | (kar >> 18), f);
			fputc (0x80 | ((kar >> 12) & 0x00'003F), f);
			fputc (0x80 | ((kar >> 6) & 0x00'003F), f);
			fputc (0x80 | (kar & 0x00'003F), f);
		}
	}
}

void MelderFile_writeText (MelderFile file, conststring32 text, kMelder_textOutputEncoding outputEncoding) {
	if (! text)
		text = U"";
	autofile f = Melder_fopen (file, "wb");
	if (outputEncoding == kMelder_textOutputEncoding::UTF8) {
		Melder_fwrite32to8 (text, f);
	} else if ((outputEncoding == kMelder_textOutputEncoding::ASCII_THEN_UTF16 && Melder_isValidAscii (text)) ||
		(outputEncoding == kMelder_textOutputEncoding::ISO_LATIN1_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ISO_LATIN1)))
	{
		#ifdef _WIN32
			#define flockfile(f)  (void) 0
			#define funlockfile(f)  (void) 0
			#define putc_unlocked  putc
		#endif
		flockfile (f);
		integer n = str32len (text);
		for (integer i = 0; i < n; i ++) {
			char32 kar = text [i];
			#ifdef _WIN32
				if (kar == U'\n')
					putc_unlocked (13, f);
			#endif
			putc_unlocked (kar, f);
		}
		funlockfile (f);
	} else {
		binputu16 (0xFEFF, f);   // Byte Order Mark
		integer n = str32len (text);
		for (integer i = 0; i < n; i ++) {
			char32 kar = text [i];
			#ifdef _WIN32
				if (kar == U'\n')
					binputu16 (13, f);
			#endif
			if (kar <= 0x00'FFFF) {
				binputu16 ((char16) kar, f);   // guarded conversion down
			} else if (kar <= 0x10'FFFF) {
				kar -= 0x010000;
				binputu16 (0xD800 | (uint16) (kar >> 10), f);
				binputu16 (0xDC00 | (uint16) ((char16) kar & 0x3ff), f);
			} else {
				binputu16 (UNICODE_REPLACEMENT_CHARACTER, f);
			}
		}
	}
	f.close (file);
}

void MelderFile_appendText (MelderFile file, conststring32 text) {
	if (! text) text = U"";
	autofile f1;
	try {
		f1.reset (Melder_fopen (file, "rb"));
	} catch (MelderError) {
		Melder_clearError ();   // it's OK if the file didn't exist yet...
		MelderFile_writeText (file, text, Melder_getOutputEncoding ());   // because then we just "write"
		return;
	}
	/*
	 * The file already exists and is open. Determine its type.
	 */
	int firstByte = fgetc (f1), secondByte = fgetc (f1);
	f1.close (file);
	int type = 0;
	if (firstByte == 0xfe && secondByte == 0xff) {
		type = 1;   // big-endian 16-bit
	} else if (firstByte == 0xff && secondByte == 0xfe) {
		type = 2;   // little-endian 16-bit
	}
	if (type == 0) {
		kMelder_textOutputEncoding outputEncoding = Melder_getOutputEncoding ();
		if (outputEncoding == kMelder_textOutputEncoding::UTF8) {   // TODO: read as file's encoding
			autofile f2 = Melder_fopen (file, "ab");
			Melder_fwrite32to8 (text, f2);
			f2.close (file);
		} else if ((outputEncoding == kMelder_textOutputEncoding::ASCII_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ASCII))
		    || (outputEncoding == kMelder_textOutputEncoding::ISO_LATIN1_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ISO_LATIN1)))
		{
			/*
			 * Append ASCII or ISOLatin1 text to ASCII or ISOLatin1 file.
			 */
			autofile f2 = Melder_fopen (file, "ab");
			int64 n = str32len (text);
			for (int64 i = 0; i < n; i ++) {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n')
						fputc (13, f2);
				#endif
				fputc ((char8) kar, f2);
			}
			f2.close (file);
		} else {
			/*
			 * Convert to wide character file.
			 */
			autostring32 oldText = MelderFile_readText (file);
			autofile f2 = Melder_fopen (file, "wb");
			binputu16 (0xfeff, f2);
			int64 n = str32len (oldText.get());
			for (int64 i = 0; i < n; i ++) {
				char32 kar = oldText [i];
				#ifdef _WIN32
					if (kar == U'\n')
						binputu16 (13, f2);
				#endif
				if (kar <= 0x00'FFFF) {
					binputu16 ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10'FFFF) {
					kar -= 0x01'0000;
					binputu16 ((uint16) (0x00'D800 | (kar >> 10)), f2);
					binputu16 ((uint16) (0x00'DC00 | (kar & 0x00'03ff)), f2);
				} else {
					binputu16 (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			}
			n = str32len (text);
			for (int64 i = 0; i < n; i ++) {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n')
						binputu16 (13, f2);
				#endif
				if (kar <= 0x00FFFF) {
					binputu16 ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10'FFFF) {
					kar -= 0x01'0000;
					binputu16 ((uint16) (0x00'D800 | (kar >> 10)), f2);
					binputu16 ((uint16) (0x00'DC00 | (kar & 0x00'03ff)), f2);
				} else {
					binputu16 (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			}
			f2.close (file);
		}
	} else {
		autofile f2 = Melder_fopen (file, "ab");
		int64 n = str32len (text);
		for (int64 i = 0; i < n; i ++) {
			if (type == 1) {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n')
						binputu16 (13, f2);
				#endif
				if (kar <= 0x00'FFFF) {
					binputu16 ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10'FFFF) {
					kar -= 0x01'0000;
					binputu16 ((uint16) (0x00'D800 | (kar >> 10)), f2);
					binputu16 ((uint16) (0x00'DC00 | (kar & 0x00'03ff)), f2);
				} else {
					binputu16 (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			} else {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n')
						binputu16LE (13, f2);
				#endif
				if (kar <= 0x00'FFFF) {
					binputu16LE ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10FFFF) {
					kar -= 0x01'0000;
					binputu16LE ((uint16) (0x00'D800 | (kar >> 10)), f2);
					binputu16LE ((uint16) (0x00'DC00 | (kar & 0x00'03ff)), f2);
				} else {
					binputu16LE (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			}
		}
		f2.close (file);
	}
}

/* End of file melder_files.cpp */
