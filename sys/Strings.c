/* Strings.c
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
 * pb 2002/07/16 GPL
 * pb 2003/07/02 corrected Strings_randomize so that the first element can sometimes go to the first place
 * pb 2004/03/21 Strings_createAsFileList now accepts spaces in directory names on Unix and Mac
 * pb 2004/04/20 the previous thing now done with backslashes rather than double quotes,
 *               because double quotes prevent the expansion of the wildcard asterisk
 * pb 2006/02/14 Strings_createAsDirectoryList for Windows
 * pb 2006/03/08 allow 1,000,000 file names in Strings_createAsFileList
 * pb 2006/09/19 Strings_createAsDirectoryList for Mac and Unix
 * pb 2006/10/04 return fewer errors in Strings_createAsFileList and Strings_createAsDirectoryList
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/10 MelderInfo
 * pb 2007/01/24 Strings_createAsFileList: removed gigantic memory leak
 * pb 2007/01/24 Strings_createAsFileList: used stat instead of platform-specific struct dirent. entry
 * pb 2007/08/10 wchar_t
 */

//#define USE_STAT  1
#ifndef USE_STAT
	#if defined (_WIN32)
		#define USE_STAT  0
	#else
		#define USE_STAT  1
	#endif
#endif

#include "Strings.h"
#include "longchar.h"
#if USE_STAT
	#include <sys/types.h>
	//#define __USE_BSD
	#include <sys/stat.h>
	#include <dirent.h>
#endif
#if defined (_WIN32)
	#include <windows.h>
#endif
#include "oo_DESTROY.h"
#include "Strings_def.h"
#include "oo_COPY.h"
#include "Strings_def.h"
#include "oo_EQUAL.h"
#include "Strings_def.h"
#include "oo_WRITE_TEXT.h"
#include "Strings_def.h"
#include "oo_READ_TEXT.h"
#include "Strings_def.h"
#include "oo_WRITE_BINARY.h"
#include "Strings_def.h"
#include "oo_READ_BINARY.h"
#include "Strings_def.h"
#include "oo_DESCRIPTION.h"
#include "Strings_def.h"

static long Strings_totalLength (Strings me) {
	long totalLength = 0, i;
	for (i = 1; i <= my numberOfStrings; i ++) {
		totalLength += wcslen (my strings [i]);
	}
	return totalLength;
}

static long Strings_maximumLength (Strings me) {
	long maximumLength = 0, i;
	for (i = 1; i <= my numberOfStrings; i ++) {
		long length = wcslen (my strings [i]);
		if (length > maximumLength) {
			maximumLength = length;
		}
	}
	return maximumLength;
}

static void info (I) {
	iam (Strings);
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of strings: ", Melder_integer (my numberOfStrings));
	MelderInfo_writeLine3 (L"Total length: ", Melder_integer (Strings_totalLength (me)), L" characters");
	MelderInfo_writeLine3 (L"Longest string: ", Melder_integer (Strings_maximumLength (me)), L" characters");
}

class_methods (Strings, Data)
	class_method_local (Strings, destroy)
	class_method_local (Strings, description)
	class_method_local (Strings, copy)
	class_method_local (Strings, equal)
	class_method_local (Strings, writeText)
	class_method_local (Strings, readText)
	class_method_local (Strings, writeBinary)
	class_method_local (Strings, readBinary)
	class_method (info)
class_methods_end

#define Strings_createAsFileOrDirectoryList_TYPE_FILE  0
#define Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY  1
static Strings Strings_createAsFileOrDirectoryList (const wchar_t *path, int type) {
	Strings me = new (Strings);
	#if USE_STAT
		/*
		 * Initialize.
		 */
		DIR *d = NULL;
		MelderStringW filePath = { 0 }, searchDirectory = { 0 }, left = { 0 }, right = { 0 };
		/*
		 * Parse the path.
		 * Example: in /Users/paul/sounds/h*.wav",
		 * the search directory is "/Users/paul/sounds",
		 * the left environment is "h", and the right environment is ".wav".
		 */
		MelderStringW_copyW (& searchDirectory, path);
		wchar_t *asterisk = wcsrchr (searchDirectory. string, '*');
		if (asterisk != NULL) {
			*asterisk = '\0';
			searchDirectory. length = asterisk - searchDirectory. string;   // Probably superfluous, but correct.
			wchar_t *lastSlash = wcsrchr (searchDirectory. string, Melder_DIRECTORY_SEPARATOR);
			if (lastSlash != NULL) {
				*lastSlash = '\0';   // This fixes searchDirectory.
				searchDirectory. length = lastSlash - searchDirectory. string;   // Probably superfluous, but correct.
				MelderStringW_copyW (& left, lastSlash + 1);
			} else {
				MelderStringW_copyW (& left, searchDirectory. string);   /* Quickly save... */
				MelderStringW_empty (& searchDirectory);   /* ...before destruction. */
			}
			MelderStringW_copyW (& right, asterisk + 1);
		}
		d = opendir (Melder_peekWcsToUtf8 (searchDirectory. string [0] ? searchDirectory. string : L"."));
		if (d == NULL) {
			Melder_error3 (L"Cannot open directory ", searchDirectory. string, L".");
			goto end;
		}
		//Melder_casual ("opened");
		my strings = NUMpvector (1, 1000000); cherror
		struct dirent *entry;
		while ((entry = readdir (d)) != NULL) {
			MelderStringW_copyW (& filePath, searchDirectory. string [0] ? searchDirectory. string : L".");
			MelderStringW_appendCharacter (& filePath, Melder_DIRECTORY_SEPARATOR);
			MelderStringW_appendW (& filePath, Melder_peekUtf8ToWcs (entry -> d_name));
			//Melder_casual ("read %s", filePath. string);
			struct stat stats;
			if (stat (Melder_peekWcsToUtf8 (filePath. string), & stats) != 0) {
				Melder_error3 (L"Cannot look at file ", filePath. string, L".");
				goto end;
				//stats. st_mode = -1L;
			}
			//Melder_casual ("statted %s", filePath. string);
			//Melder_casual ("file %s mode %s", filePath. string, Melder_integer (stats. st_mode / 4096));
			if ((type == Strings_createAsFileOrDirectoryList_TYPE_FILE && S_ISREG (stats. st_mode)) ||
				(type == Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY && S_ISDIR (stats. st_mode)))
			{
				wchar_t *fileName = Melder_peekUtf8ToWcs (entry -> d_name);
				unsigned long length = wcslen (fileName);
				if (fileName [0] != '.' &&
					(left. length == 0 || wcsnequ (fileName, left. string, left. length)) &&
					(right. length == 0 || (length >= right. length && wcsequ (fileName + (length - right. length), right. string))))
				{
					my strings [++ my numberOfStrings] = Melder_wcsdup (fileName); cherror
				}
			}
		}
	#elif defined (_WIN32)
		HANDLE searchHandle;
		WIN32_FIND_DATAW findData;
		wchar_t searchPath [300];
		int len = wcslen (path), hasAsterisk = wcschr (path, '*') != NULL, endsInSeparator = len != 0 && path [len - 1] == '\\';
		my strings = NUMpvector (1, 1000000); cherror
		swprintf (searchPath, 300, L"%ls%ls%ls", path, hasAsterisk || endsInSeparator ? L"" : L"\\", hasAsterisk ? L"" : L"*");
		searchHandle = FindFirstFileW (searchPath, & findData);
		if (searchHandle != INVALID_HANDLE_VALUE) {
			do {
				if ((type == Strings_createAsFileOrDirectoryList_TYPE_FILE &&
						(findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				 || (type == Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY && 
						(findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
				{
					my strings [++ my numberOfStrings] = Melder_wcsdup (findData. cFileName); cherror
				}
			} while (FindNextFileW (searchHandle, & findData));
			FindClose (searchHandle);
		}
	#endif
end:
	#if USE_STAT
		if (d) closedir (d);
		MelderStringW_free (& filePath);
		MelderStringW_free (& searchDirectory);
		MelderStringW_free (& left);
		MelderStringW_free (& right);
	#endif
	iferror forget (me);
	return me;
}

Strings Strings_createAsFileList (const wchar_t *path) {
	return Strings_createAsFileOrDirectoryList (path, Strings_createAsFileOrDirectoryList_TYPE_FILE);
}

Strings Strings_createAsDirectoryList (const wchar_t *path) {
	return Strings_createAsFileOrDirectoryList (path, Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY);
}

Strings Strings_readFromRawTextFile (MelderFile file) {
	Strings me = NULL;
	wchar_t *string = MelderFile_readText (file); cherror
	MelderReadString text = { string, string };

	/*
	 * Count number of strings.
	 */
	long n = 0;
	wchar_t *p = & string [0];
	for (; *p != '\0'; p ++) if (*p == '\n') n ++;
	if (p - string > 1 && p [-1] != '\n') n ++;

	/*
	 * Create.
	 */
	me = new (Strings);
	if (n > 0) my strings = NUMpvector (1, n); cherror
	my numberOfStrings = n;

	/*
	 * Read strings.
	 */
	for (long i = 1; i <= n; i ++) {
		wchar_t *line = MelderReadString_readLine (& text); cherror
		my strings [i] = Melder_wcsdup (line); cherror
	}

end:
	Melder_free (string);
	iferror {
		forget (me);
		Melder_error3 (L"(Strings_readFromRawTextFile:) File ", MelderFile_messageNameW (file), L" not read.");
		return NULL;
	}
	return me;
}

int Strings_writeToRawTextFile (Strings me, MelderFile file) {
	MelderStringW buffer = { 0 };
	for (long i = 1; i <= my numberOfStrings; i ++) {
		MelderStringW_append2 (& buffer, my strings [i], L"\n");
	}
	MelderFile_writeText (file, buffer.string);
end:
	MelderStringW_free (& buffer);
	iferror return 0;
	return 1;
}

void Strings_randomize (Strings me) {
	for (long i = 1; i < my numberOfStrings; i ++) {
		long other = NUMrandomInteger (i, my numberOfStrings);
		wchar_t *dummy = my strings [other];
		my strings [other] = my strings [i];
		my strings [i] = dummy;
	}
}

int Strings_genericize (Strings me) {
	wchar_t *buffer = Melder_calloc (wchar_t, Strings_maximumLength (me) * 3 + 1); cherror
	for (long i = 1; i <= my numberOfStrings; i ++) {
		const wchar_t *p = (const wchar_t *) my strings [i];
		while (*p) {
			if (*p > 126) {   /* Backslashes are not converted, i.e. genericize^2 == genericize. */
				wchar_t *newString;
				Longchar_genericizeW (my strings [i], buffer);
				newString = Melder_wcsdup (buffer); cherror
				/*
				 * Replace string only if copying was OK.
				 */
				Melder_free (my strings [i]);
				my strings [i] = newString;
				break;
			}
			p ++;
		}
	}
end:
	Melder_free (buffer);
	iferror return 0;
	return 1;
}

void Strings_sort (Strings me) {
	NUMsort_strW (my numberOfStrings, my strings);
}

/* End of file Strings.c */
