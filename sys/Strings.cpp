/* Strings.cpp
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
 * pb 2007/10/01 can write as encoding
 * pb 2007/10/01 corrected nativization
 * pb 2007/11/17 getVectorStr
 * pb 2007/12/10 Strings_createAsFileList precomposes characters
 * pb 2011/05/03 Windows: ignore file or directory names starting with '.'
 * pb 2011/05/15 C++
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
#include "oo_CAN_WRITE_AS_ENCODING.h"
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

static const wchar * getVectorStr (I, long icol) {
	iam (Strings);
	wchar_t *stringValue;
	if (icol < 1 || icol > my numberOfStrings) return L"";
	stringValue = my strings [icol];
	return stringValue == NULL ? L"" : stringValue;
}

class_methods (Strings, Data) {
	class_method_local (Strings, destroy)
	class_method_local (Strings, description)
	class_method_local (Strings, copy)
	class_method_local (Strings, equal)
	class_method_local (Strings, canWriteAsEncoding)
	class_method_local (Strings, writeText)
	class_method_local (Strings, readText)
	class_method_local (Strings, writeBinary)
	class_method_local (Strings, readBinary)
	class_method (info)
	class_method (getVectorStr)
	class_methods_end
}

#define Strings_createAsFileOrDirectoryList_TYPE_FILE  0
#define Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY  1
static Strings Strings_createAsFileOrDirectoryList (const wchar_t *path, int type) {
	Strings me = Thing_new (Strings);
	#if USE_STAT
		/*
		 * Initialize.
		 */
		DIR *d = NULL;
		MelderString filePath = { 0 }, searchDirectory = { 0 }, left = { 0 }, right = { 0 };
		/*
		 * Parse the path.
		 * Example: in /Users/paul/sounds/h*.wav",
		 * the search directory is "/Users/paul/sounds",
		 * the left environment is "h", and the right environment is ".wav".
		 */
		MelderString_copy (& searchDirectory, path);
		wchar_t *asterisk = wcsrchr (searchDirectory. string, '*');
		if (asterisk != NULL) {
			*asterisk = '\0';
			searchDirectory. length = asterisk - searchDirectory. string;   // Probably superfluous, but correct.
			wchar_t *lastSlash = wcsrchr (searchDirectory. string, Melder_DIRECTORY_SEPARATOR);
			if (lastSlash != NULL) {
				*lastSlash = '\0';   // This fixes searchDirectory.
				searchDirectory. length = lastSlash - searchDirectory. string;   // Probably superfluous, but correct.
				MelderString_copy (& left, lastSlash + 1);
			} else {
				MelderString_copy (& left, searchDirectory. string);   /* Quickly save... */
				MelderString_empty (& searchDirectory);   /* ...before destruction. */
			}
			MelderString_copy (& right, asterisk + 1);
		}
		char buffer8 [1+kMelder_MAXPATH];
		Melder_wcsTo8bitFileRepresentation_inline (searchDirectory. string, buffer8);
		d = opendir (buffer8 [0] ? buffer8 : ".");
		if (d == NULL) error3 (L"Cannot open directory ", searchDirectory. string, L".")
		//Melder_casual ("opened");
		my strings = NUMvector <wchar *> (1, 1000000); cherror   // TODO
		struct dirent *entry;
		while ((entry = readdir (d)) != NULL) {
			MelderString_copy (& filePath, searchDirectory. string [0] ? searchDirectory. string : L".");
			MelderString_appendCharacter (& filePath, Melder_DIRECTORY_SEPARATOR);
			wchar_t bufferW [1+kMelder_MAXPATH];
			Melder_8bitFileRepresentationToWcs_inline (entry -> d_name, bufferW);
			MelderString_append (& filePath, bufferW);
			//Melder_casual ("read %s", filePath. string);
			Melder_wcsTo8bitFileRepresentation_inline (filePath. string, buffer8);
			struct stat stats;
			if (stat (buffer8, & stats) != 0) {
				error3 (L"Cannot look at file ", filePath. string, L".")
				//stats. st_mode = -1L;
			}
			//Melder_casual ("statted %s", filePath. string);
			//Melder_casual ("file %s mode %s", filePath. string, Melder_integer (stats. st_mode / 4096));
			if ((type == Strings_createAsFileOrDirectoryList_TYPE_FILE && S_ISREG (stats. st_mode)) ||
				(type == Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY && S_ISDIR (stats. st_mode)))
			{
				Melder_8bitFileRepresentationToWcs_inline (entry -> d_name, bufferW);
				unsigned long length = wcslen (bufferW);
				if (bufferW [0] != '.' &&
					(left. length == 0 || wcsnequ (bufferW, left. string, left. length)) &&
					(right. length == 0 || (length >= right. length && wcsequ (bufferW + (length - right. length), right. string))))
				{
					my strings [++ my numberOfStrings] = Melder_wcsdup_e (bufferW); cherror
				}
			}
		}
	#elif defined (_WIN32)
		HANDLE searchHandle;
		WIN32_FIND_DATAW findData;
		wchar_t searchPath [1+kMelder_MAXPATH];
		int len = wcslen (path), hasAsterisk = wcschr (path, '*') != NULL, endsInSeparator = len != 0 && path [len - 1] == '\\';
		my strings = NUMvector <wchar *> (1, 1000000); cherror
		swprintf (searchPath, 1+kMelder_MAXPATH, L"%ls%ls%ls", path, hasAsterisk || endsInSeparator ? L"" : L"\\", hasAsterisk ? L"" : L"*");
		searchHandle = FindFirstFileW (searchPath, & findData);
		if (searchHandle != INVALID_HANDLE_VALUE) {
			do {
				if ((type == Strings_createAsFileOrDirectoryList_TYPE_FILE &&
						(findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				 || (type == Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY && 
						(findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
				{
					if (findData. cFileName [0] != '.') {
						my strings [++ my numberOfStrings] = Melder_wcsdup_e (findData. cFileName); cherror
					}
				}
			} while (FindNextFileW (searchHandle, & findData));
			FindClose (searchHandle);
		}
	#endif
	Strings_sort (me);
end:
	#if USE_STAT
		if (d) closedir (d);
		MelderString_free (& filePath);
		MelderString_free (& searchDirectory);
		MelderString_free (& left);
		MelderString_free (& right);
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
	MelderReadText text = NULL;
	try {
		text = MelderReadText_createFromFile (file);

		/*
		 * Count number of strings.
		 */
		long n = MelderReadText_getNumberOfLines (text);

		/*
		 * Create.
		 */
		autoStrings me = Thing_new (Strings);
		if (n > 0) my strings = NUMvector <wchar *> (1, n);
		my numberOfStrings = n;

		/*
		 * Read strings.
		 */
		for (long i = 1; i <= n; i ++) {
			wchar_t *line = MelderReadText_readLine (text); therror
			my strings [i] = Melder_wcsdup (line);
		}
		MelderReadText_delete (text);
		return me.transfer();
	} catch (MelderError) {
		MelderReadText_delete (text);   // TODO
		rethrowmzero ("Strings not read from raw text file ", MelderFile_messageName (file), ".");
	}
}

int Strings_writeToRawTextFile (Strings me, MelderFile file) {
	MelderString buffer = { 0 };
	for (long i = 1; i <= my numberOfStrings; i ++) {
		MelderString_append2 (& buffer, my strings [i], L"\n");
	}
	MelderFile_writeText (file, buffer.string);
end:
	MelderString_free (& buffer);
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
	wchar_t *buffer = NULL;
//start:
	buffer = Melder_calloc_e (wchar_t, Strings_maximumLength (me) * 3 + 1); cherror
	for (long i = 1; i <= my numberOfStrings; i ++) {
		const wchar_t *p = (const wchar_t *) my strings [i];
		while (*p) {
			if (*p > 126) {   /* Backslashes are not converted, i.e. genericize^2 == genericize. */
				wchar_t *newString;
				Longchar_genericizeW (my strings [i], buffer);
				newString = Melder_wcsdup_e (buffer); cherror
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

int Strings_nativize (Strings me) {
	wchar_t *buffer = NULL;
//start:
	buffer = Melder_calloc_e (wchar_t, Strings_maximumLength (me) + 1); cherror
	for (long i = 1; i <= my numberOfStrings; i ++) {
		Longchar_nativizeW (my strings [i], buffer, false);
		wchar_t *newString = Melder_wcsdup_e (buffer); cherror
		/*
		 * Replace string only if copying was OK.
		 */
		Melder_free (my strings [i]);
		my strings [i] = newString;
	}
end:
	Melder_free (buffer);
	iferror return 0;
	return 1;
}

void Strings_sort (Strings me) {
	NUMsort_str (my numberOfStrings, my strings);
}

void Strings_remove (Strings me, long position) {
	Melder_assert (position >= 1);
	Melder_assert (position <= my numberOfStrings);
	Melder_free (my strings [position]);
	for (long i = position; i < my numberOfStrings; i ++) {
		my strings [i] = my strings [i + 1];
	}
	my numberOfStrings --;
}

int Strings_replace (Strings me, long position, const wchar_t *text) {
	Melder_assert (position >= 1);
	Melder_assert (position <= my numberOfStrings);
	if (Melder_wcsequ (my strings [position], text)) return 1;
	Melder_free (my strings [position]);
	my strings [position] = Melder_wcsdup_e (text); cherror
end:
	iferror return 0;
	return 1;
}

int Strings_insert (Strings me, long position, const wchar_t *text) {
	if (position == 0) position = my numberOfStrings + 1;
	Melder_assert (position >= 1);
	Melder_assert (position <= my numberOfStrings + 1);
	for (long i = my numberOfStrings + 1; i > position; i --) {
		my strings [i] = my strings [i - 1];
	}
	my strings [position] = Melder_wcsdup_e (text); cherror
	my numberOfStrings ++;
end:
	iferror return 0;
	return 1;
}

/* End of file Strings.cpp */
