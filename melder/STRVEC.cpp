/* STRVEC.cpp
 *
 * Copyright (C) 2006,2007,2009,2011,2012,2015-2020 Paul Boersma
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

#if defined (_WIN32)
	#include "winport_on.h"
	#include <windows.h>
	#include "winport_off.h"
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
#endif

#include "melder.h"

static autoSTRVEC fileOrFolderNames_STRVEC (conststring32 path /* cattable */, bool wantDirectories) {
	#if defined (_WIN32)
		try {
			char32 searchPath [kMelder_MAXPATH+1];
			int len = str32len (path);
			bool hasAsterisk = !! str32chr (path, U'*');
			bool endsInSeparator = ( len != 0 && path [len - 1] == U'\\' );
			autoSTRVEC strings;
			Melder_sprint (searchPath, kMelder_MAXPATH+1, path, hasAsterisk || endsInSeparator ? U"" : U"\\", hasAsterisk ? U"" : U"*");
			WIN32_FIND_DATAW findData;
			HANDLE searchHandle = FindFirstFileW (Melder_peek32toW_fileSystem (searchPath), & findData);
			if (searchHandle != INVALID_HANDLE_VALUE) {
				do {
					if ((! wantDirectories && (findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
						|| (wantDirectories && (findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
					{
						if (findData. cFileName [0] != L'.')
							strings. append (Melder_peekWto32 (findData. cFileName));
					}
				} while (FindNextFileW (searchHandle, & findData));
				FindClose (searchHandle);
			}
			sort_STRVEC_inout (strings.get());
			return strings;
		} catch (MelderError) {
			throw;
		}
	#else   // use stat
		DIR *d = nullptr;
		try {
			/*
				Parse the path.
				This can be either a directory name such as "/Users/paul/sounds"
				or a wildcarded path such as "/Users/paul/sounds/h*.wav".
				Example: in "/Users/paul/sounds/h*llo.*av",
				the search directory is "/Users/paul/sounds",
				the left environment is "h", the middle environment is "llo.", and the right environment is "av".
			*/
			autoMelderString searchDirectory, left, middle, right, filePath;
			MelderString_copy (& searchDirectory, path);
			char32 *asterisk1 = str32chr (searchDirectory. string, U'*');
			char32 *asterisk2 = str32rchr (searchDirectory. string, U'*');
			if (asterisk1) {
				/*
					The path is a wildcarded path.
				*/
				*asterisk1 = U'\0';
				*asterisk2 = U'\0';
				searchDirectory. length = asterisk1 - searchDirectory. string;   // probably superfluous, but correct
				char32 *lastSlash = str32rchr (searchDirectory. string, Melder_DIRECTORY_SEPARATOR);
				if (lastSlash) {
					*lastSlash = U'\0';   // this fixes searchDirectory
					searchDirectory. length = lastSlash - searchDirectory. string;   // probably superfluous, but correct
					MelderString_copy (& left, lastSlash + 1);
				} else {
					MelderString_copy (& left, searchDirectory. string);   // quickly save...
					MelderString_empty (& searchDirectory);   // ...before destruction
				}
				if (asterisk1 != asterisk2) {
					MelderString_copy (& middle, asterisk1 + 1);
				}
				MelderString_copy (& right, asterisk2 + 1);
			} else {
				/*
					We're finished. No asterisk, hence the path is a directory name.
				*/
			}
			char buffer8 [kMelder_MAXPATH+1];
			Melder_32to8_fileSystem_inplace (searchDirectory. string, buffer8);
			d = opendir (buffer8 [0] ? buffer8 : ".");
			if (! d)
				Melder_throw (U"Cannot open directory ", searchDirectory. string, U".");
			//Melder_casual (U"opened");
			autoSTRVEC strings;
			struct dirent *entry;
			while (!! (entry = readdir (d))) {
				MelderString_copy (& filePath, searchDirectory. string [0] ? searchDirectory. string : U".");
				MelderString_appendCharacter (& filePath, Melder_DIRECTORY_SEPARATOR);
				char32 buffer32 [kMelder_MAXPATH+1];
				Melder_8bitFileRepresentationToStr32_inplace (entry -> d_name, buffer32);
				MelderString_append (& filePath, buffer32);
				//Melder_casual (U"read ", filePath. string);
				Melder_32to8_fileSystem_inplace (filePath. string, buffer8);
				struct stat stats;
				if (stat (buffer8, & stats) != 0) {
					//Melder_throw (U"Cannot look at file ", filePath. string, U".");
					//stats. st_mode = -1L;
				}
				//Melder_casual (U"statted ", filePath. string);
				//Melder_casual (U"file ", filePath. string, U" mode ", stats. st_mode / 4096);
				if ((! wantDirectories && S_ISREG (stats. st_mode)) || (wantDirectories && S_ISDIR (stats. st_mode))) {
					Melder_8bitFileRepresentationToStr32_inplace (entry -> d_name, buffer32);
					int64 length = str32len (buffer32);
					integer numberOfMatchedCharacters = 0;
					bool doesTheLeftMatch = true;
					if (left. length != 0) {
						doesTheLeftMatch = str32nequ (buffer32, left. string, left. length);
						if (doesTheLeftMatch)
							numberOfMatchedCharacters = left.length;
					}
					bool doesTheMiddleMatch = true;
					if (middle. length != 0) {
						char32 *position = str32str (buffer32 + numberOfMatchedCharacters, middle. string);
						doesTheMiddleMatch = !! position;
						if (doesTheMiddleMatch)
							numberOfMatchedCharacters = position - buffer32 + middle.length;
					}
					bool doesTheRightMatch = true;
					if (right. length != 0) {
						int64 startOfRight = length - right. length;
						doesTheRightMatch = startOfRight >= numberOfMatchedCharacters &&
							str32equ (buffer32 + startOfRight, right. string);
					}
					if (buffer32 [0] != U'.' && doesTheLeftMatch && doesTheMiddleMatch && doesTheRightMatch)
						strings. append (buffer32);
				}
			}
			closedir (d);
			sort_STRVEC_inout (strings.get());
			return strings;
		} catch (MelderError) {
			if (d)
				closedir (d);   // "finally"
			throw;
		}
	#endif
}
autoSTRVEC fileNames_STRVEC (conststring32 path /* cattable */) {
	return fileOrFolderNames_STRVEC (path, false);
}
autoSTRVEC folderNames_STRVEC (conststring32 path /* cattable */) {
	return fileOrFolderNames_STRVEC (path, true);
}

autoSTRVEC readLinesFromFile_STRVEC (MelderFile file) {
	autoMelderReadText text = MelderReadText_createFromFile (file);
	int64 numberOfLines = MelderReadText_getNumberOfLines (text.get());
	if (numberOfLines == 0)
		return autoSTRVEC ();
	autoSTRVEC result = autoSTRVEC (numberOfLines);
	for (integer iline = 1; iline <= numberOfLines; iline ++) {
		const mutablestring32 line = MelderReadText_readLine (text.get());
		result [iline] = Melder_dup (line);
	}
	return result;
}

autoSTRVEC splitByWhitespace_STRVEC (conststring32 string) {
	if (! string)
		return autoSTRVEC();   // accept null pointer input
	integer n = NUMnumberOfTokens (string);
	if (n == 0)
		return autoSTRVEC();
	autoSTRVEC result (n);

	integer itoken = 0;
	const char32 *p = & string [0];
	for (;;) {
		Melder_skipHorizontalOrVerticalSpace (& p);
		if (*p == U'\0')
			break;
		const char32 *beginOfInk = p;
		p ++;   // step over first nonspace
		p = Melder_findEndOfInk (p);
		integer numberOfCharacters = p - beginOfInk;
		autostring32 token (numberOfCharacters);
		str32ncpy (token.get(), beginOfInk, numberOfCharacters);
		result [++ itoken] = token.move();
	}
	return result;
}

/* End of file STRVEC.cpp */
