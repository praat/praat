/* Strings.cpp
 *
 * Copyright (C) 1992-2012,2014,2015,2017 Paul Boersma
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

//#define USE_STAT  1
#ifndef USE_STAT
	#if defined (_WIN32)
		#define USE_STAT  0
	#else
		#define USE_STAT  1
	#endif
#endif

#if USE_STAT
	#include <sys/types.h>
	//#define __USE_BSD
	#include <sys/stat.h>
	#include <dirent.h>
#endif
#if defined (_WIN32)
	#include "winport_on.h"
	#include <windows.h>
	#include "winport_off.h"
#endif

#include "Strings_.h"
#include "longchar.h"

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

Thing_implement (Strings, Daata, 0);

static integer Strings_totalLength (Strings me) {
	integer totalLength = 0;
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		totalLength += str32len (my strings [i]);
	}
	return totalLength;
}

static integer Strings_maximumLength (Strings me) {
	integer maximumLength = 0;
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		integer length = str32len (my strings [i]);
		if (length > maximumLength) {
			maximumLength = length;
		}
	}
	return maximumLength;
}

void structStrings :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of strings: ", numberOfStrings);
	MelderInfo_writeLine (U"Total length: ", Strings_totalLength (this), U" characters");
	MelderInfo_writeLine (U"Longest string: ", Strings_maximumLength (this), U" characters");
}

const char32 * structStrings :: v_getVectorStr (integer icol) {
	if (icol < 1 || icol > our numberOfStrings) return U"";
	char32 *stringValue = strings [icol];
	return stringValue ? stringValue : U"";
}

#define Strings_createAsFileOrDirectoryList_TYPE_FILE  0
#define Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY  1
static autoStrings Strings_createAsFileOrDirectoryList (const char32 *path /* cattable */, int type) {
	#if USE_STAT
		/*
		 * Initialize.
		 */
		DIR *d = nullptr;
		try {
			autoMelderString filePath, searchDirectory, left, right;
			/*
			 * Parse the path.
			 * Example: in /Users/paul/sounds/h*.wav",
			 * the search directory is "/Users/paul/sounds",
			 * the left environment is "h", and the right environment is ".wav".
			 */
			MelderString_copy (& searchDirectory, path);
			char32 *asterisk = str32rchr (searchDirectory. string, '*');
			if (asterisk) {
				*asterisk = '\0';
				searchDirectory. length = asterisk - searchDirectory. string;   // probably superfluous, but correct
				char32 *lastSlash = str32rchr (searchDirectory. string, Melder_DIRECTORY_SEPARATOR);
				if (lastSlash) {
					*lastSlash = '\0';   // This fixes searchDirectory.
					searchDirectory. length = lastSlash - searchDirectory. string;   // probably superfluous, but correct
					MelderString_copy (& left, lastSlash + 1);
				} else {
					MelderString_copy (& left, searchDirectory. string);   // quickly save...
					MelderString_empty (& searchDirectory);   // ...before destruction
				}
				MelderString_copy (& right, asterisk + 1);
			}
			char buffer8 [kMelder_MAXPATH+1];
			Melder_str32To8bitFileRepresentation_inplace (searchDirectory. string, buffer8);
			d = opendir (buffer8 [0] ? buffer8 : ".");
			if (! d)
				Melder_throw (U"Cannot open directory ", searchDirectory. string, U".");
			//Melder_casual (U"opened");
			autoStrings me = Thing_new (Strings);
			struct dirent *entry;
			while (!! (entry = readdir (d))) {
				MelderString_copy (& filePath, searchDirectory. string [0] ? searchDirectory. string : U".");
				MelderString_appendCharacter (& filePath, Melder_DIRECTORY_SEPARATOR);
				char32 buffer32 [kMelder_MAXPATH+1];
				Melder_8bitFileRepresentationToStr32_inplace (entry -> d_name, buffer32);
				MelderString_append (& filePath, buffer32);
				//Melder_casual (U"read ", filePath. string);
				Melder_str32To8bitFileRepresentation_inplace (filePath. string, buffer8);
				struct stat stats;
				if (stat (buffer8, & stats) != 0) {
					//Melder_throw (U"Cannot look at file ", filePath. string, U".");
					//stats. st_mode = -1L;
				}
				//Melder_casual (U"statted ", filePath. string);
				//Melder_casual (U"file ", filePath. string, U" mode ", stats. st_mode / 4096);
				if ((type == Strings_createAsFileOrDirectoryList_TYPE_FILE && S_ISREG (stats. st_mode)) ||
					(type == Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY && S_ISDIR (stats. st_mode)))
				{
					Melder_8bitFileRepresentationToStr32_inplace (entry -> d_name, buffer32);
					int64 length = str32len (buffer32);
					if (buffer32 [0] != U'.' &&
						(left. length == 0 || str32nequ (buffer32, left. string, left. length)) &&
						(right. length == 0 || (length >= right. length && str32equ (buffer32 + (length - right. length), right. string))))
					{
						Strings_insert (me.get(), 0, buffer32);
					}
				}
			}
			closedir (d);
			Strings_sort (me.get());
			return me;
		} catch (MelderError) {
			if (d) closedir (d);   // "finally"
			throw;
		}
	#elif defined (_WIN32)
		try {
			char32 searchPath [kMelder_MAXPATH+1];
			int len = str32len (path);
			bool hasAsterisk = !! str32chr (path, U'*');
			bool endsInSeparator = ( len != 0 && path [len - 1] == U'\\' );
			autoStrings me = Thing_new (Strings);
			Melder_sprint (searchPath, kMelder_MAXPATH+1, path, hasAsterisk || endsInSeparator ? U"" : U"\\", hasAsterisk ? U"" : U"*");
			WIN32_FIND_DATAW findData;
			HANDLE searchHandle = FindFirstFileW (Melder_peek32toW (searchPath), & findData);
			if (searchHandle != INVALID_HANDLE_VALUE) {
				do {
					if ((type == Strings_createAsFileOrDirectoryList_TYPE_FILE &&
							(findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
					 || (type == Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY && 
							(findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
					{
						if (findData. cFileName [0] != L'.') {
							Strings_insert (me.get(), 0, Melder_peekWto32 (findData. cFileName));
						}
					}
				} while (FindNextFileW (searchHandle, & findData));
				FindClose (searchHandle);
			}
			Strings_sort (me.get());
			return me;
		} catch (MelderError) {
			throw;
		}
	#endif
}

autoStrings Strings_createAsFileList (const char32 *path /* cattable */) {
	try {
		return Strings_createAsFileOrDirectoryList (path, Strings_createAsFileOrDirectoryList_TYPE_FILE);
	} catch (MelderError) {
		Melder_throw (U"Strings object not created as file list.");
	}
}

autoStrings Strings_createAsDirectoryList (const char32 *path /* cattable */) {
	try {
		return Strings_createAsFileOrDirectoryList (path, Strings_createAsFileOrDirectoryList_TYPE_DIRECTORY);
	} catch (MelderError) {
		Melder_throw (U"Strings object not created as directory list.");
	}
}

autoStrings Strings_readFromRawTextFile (MelderFile file) {
	try {
		autoMelderReadText text = MelderReadText_createFromFile (file);

		/*
		 * Count number of strings.
		 */
		int64 n = MelderReadText_getNumberOfLines (text.peek());

		/*
		 * Create.
		 */
		autoStrings me = Thing_new (Strings);
		if (n > 0) my strings = NUMvector <char32 *> (1, n);
		my numberOfStrings = n;

		/*
		 * Read strings.
		 */
		for (integer i = 1; i <= n; i ++) {
			char32 *line = MelderReadText_readLine (text.peek());
			my strings [i] = Melder_dup (line);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings not read from raw text file ", file, U".");
	}
}

void Strings_writeToRawTextFile (Strings me, MelderFile file) {
	autoMelderString buffer;
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		MelderString_append (& buffer, my strings [i], U"\n");
	}
	MelderFile_writeText (file, buffer.string, Melder_getOutputEncoding ());
}

void Strings_randomize (Strings me) {
	for (integer i = 1; i < my numberOfStrings; i ++) {
		integer other = NUMrandomInteger (i, my numberOfStrings);
		char32 *dummy = my strings [other];
		my strings [other] = my strings [i];
		my strings [i] = dummy;
	}
}

void Strings_genericize (Strings me) {
	autostring32 buffer = Melder_calloc (char32, Strings_maximumLength (me) * 3 + 1);
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		const char32 *p = (const char32 *) my strings [i];
		while (*p) {
			if (*p > 126) {   // backslashes are not converted, i.e. genericize^2 == genericize
				Longchar_genericize32 (my strings [i], buffer.peek());
				autostring32 newString = Melder_dup (buffer.peek());
				/*
				 * Replace string only if copying was OK.
				 */
				Melder_free (my strings [i]);
				my strings [i] = newString.transfer();
				break;
			}
			p ++;
		}
	}
}

void Strings_nativize (Strings me) {
	autostring32 buffer = Melder_calloc (char32, Strings_maximumLength (me) + 1);
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		Longchar_nativize32 (my strings [i], buffer.peek(), false);
		autostring32 newString = Melder_dup (buffer.peek());
		/*
		 * Replace string only if copying was OK.
		 */
		Melder_free (my strings [i]);
		my strings [i] = newString.transfer();
	}
}

void Strings_sort (Strings me) {
	NUMsort_str (my numberOfStrings, my strings);
}

void Strings_remove (Strings me, integer position) {
	if (position < 1 || position > my numberOfStrings) {
		Melder_throw (U"You supplied a position of ", position, U", but for this string it has to be in the range [1, ", my numberOfStrings, U"].");
	}
	Melder_free (my strings [position]);
	for (integer i = position; i < my numberOfStrings; i ++) {
		my strings [i] = my strings [i + 1];
	}
	my numberOfStrings --;
}

void Strings_replace (Strings me, integer position, const char32 *text) {
	if (position < 1 || position > my numberOfStrings) {
		Melder_throw (U"You supplied a position of ", position, U", but for this string it has to be in the range [1, ", my numberOfStrings, U"].");
	}
	if (Melder_equ (my strings [position], text))
		return;   // nothing to change
	/*
	 * Create without change.
	 */
	autostring32 newString = Melder_dup (text);
	/*
	 * Change without error.
	 */
	Melder_free (my strings [position]);
	my strings [position] = newString.transfer();
}

void Strings_insert (Strings me, integer position, const char32 *text) {
	if (position == 0) {
		position = my numberOfStrings + 1;
	} else if (position < 1 || position > my numberOfStrings + 1) {
		Melder_throw (U"You supplied a position of ", position, U", but for this string it has to be in the range [1, ", my numberOfStrings, U"].");
	}
	/*
	 * Create without change.
	 */
	autostring32 newString = Melder_dup (text);
	autoNUMvector <char32 *> newStrings (1, my numberOfStrings + 1);
	/*
	 * Change without error.
	 */
	for (integer i = 1; i < position; i ++) {
		newStrings [i] = my strings [i];
	}
	newStrings [position] = newString.transfer();
	my numberOfStrings ++;
	for (integer i = position + 1; i <= my numberOfStrings; i ++) {
		newStrings [i] = my strings [i - 1];
	}
	NUMvector_free (my strings, 1);
	my strings = newStrings.transfer();
}

/* End of file Strings.cpp */
