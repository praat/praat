/* Strings.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 */

#include "Strings.h"
#include "longchar.h"
#if defined (_WIN32)
	#include <windows.h>
#elif defined (macintosh)
	#include <Files.h>
#endif
#include <dirent.h>
#include "oo_DESTROY.h"
#include "Strings_def.h"
#include "oo_COPY.h"
#include "Strings_def.h"
#include "oo_EQUAL.h"
#include "Strings_def.h"
#include "oo_WRITE_ASCII.h"
#include "Strings_def.h"
#include "oo_READ_ASCII.h"
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
		totalLength += strlen (my strings [i]);
	}
	return totalLength;
}

static long Strings_maximumLength (Strings me) {
	long maximumLength = 0, i;
	for (i = 1; i <= my numberOfStrings; i ++) {
		long length = strlen (my strings [i]);
		if (length > maximumLength) {
			maximumLength = length;
		}
	}
	return maximumLength;
}

static void info (I) {
	iam (Strings);
	Melder_info ("%ld strings\nTotal length %ld\nMaximum length %ld",
		my numberOfStrings, Strings_totalLength (me), Strings_maximumLength (me));
}

class_methods (Strings, Data)
	class_method_local (Strings, destroy)
	class_method_local (Strings, description)
	class_method_local (Strings, copy)
	class_method_local (Strings, equal)
	class_method_local (Strings, writeAscii)
	class_method_local (Strings, readAscii)
	class_method_local (Strings, writeBinary)
	class_method_local (Strings, readBinary)
	class_method (info)
class_methods_end

Strings Strings_createAsFileList (const char *path) {
	Strings me = new (Strings);
	#if defined (macintosh) || defined (UNIX)
		char *asterisk, left [100], right [100], searchDirectory [256], *lastSlash;
		long leftLength, rightLength;
		my strings = NUMpvector (1, 1000000); cherror
		/*
		 * Parse the path.
		 * Example: in /Users/paul/sounds/h*.wav",
		 * the search directory is "/Users/paul/sounds",
		 * the left environment is "h", and the right environment is ".wav".
		 */
		strcpy (searchDirectory, path);
		left [0] = right [0] = '\0';
		asterisk = strrchr (searchDirectory, '*');
		if (asterisk) {
			*asterisk = '\0';
			if ((lastSlash = strrchr (searchDirectory, '/')) != NULL) {
				*lastSlash = '\0';   /* This fixes searchDirectory. */
				strcpy (left, lastSlash + 1);
			} else {
				strcpy (left, searchDirectory);   /* Quickly save... */
				searchDirectory [0] = '\0';   /* ...before destruction. */
			}
			strcpy (right, asterisk + 1);
		}
		leftLength = strlen (left), rightLength = strlen (right);
		{
			DIR *d = opendir (searchDirectory [0] ? searchDirectory : ".");
			struct dirent *entry;
			if (d == NULL) { forget (me); return Melder_errorp ("Cannot open directory %s.", searchDirectory); }
			my strings = NUMpvector (1, 1000000); cherror
			while ((entry = readdir (d)) != NULL) {
				#ifndef DT_REG
					#define DT_REG  8
				#endif
				if (entry -> d_type == DT_REG) {
					char *fileName = entry -> d_name;
					int length = strlen (fileName);
					if (fileName [0] != '.' &&
					    (leftLength == 0 || strnequ (entry -> d_name, left, leftLength)) &&
					    (rightLength == 0 || (length >= rightLength && strequ (entry -> d_name + (length - rightLength), right))))
					{
						my strings [++ my numberOfStrings] = Melder_strdup (entry -> d_name); cherror
					}
				}
			}
			closedir (d);
		}
	#elif defined (_WIN32)
		HANDLE searchHandle;
		WIN32_FIND_DATA findData;
		char searchPath [300];
		int len = strlen (path), hasAsterisk = strchr (path, '*') != NULL, endsInSeparator = len != 0 && path [len - 1] == '\\';
		my strings = NUMpvector (1, 1000000); cherror
		sprintf (searchPath, "%s%s%s", path, hasAsterisk || endsInSeparator ? "" : "\\", hasAsterisk ? "" : "*");
		searchHandle = FindFirstFile (searchPath, & findData);
		if (searchHandle != INVALID_HANDLE_VALUE) {
			do {
				if (! (findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					my strings [++ my numberOfStrings] = Melder_strdup (findData. cFileName); cherror
				}
			} while (FindNextFile (searchHandle, & findData));
			FindClose (searchHandle);
		}
	#endif
end:
	iferror forget (me);
	return me;
}

Strings Strings_createAsDirectoryList (const char *path) {
	Strings me = new (Strings);
	#if defined (macintosh) || defined (UNIX)
		char *asterisk, left [100], right [100], searchDirectory [256], *lastSlash;
		long leftLength, rightLength;
		my strings = NUMpvector (1, 1000000); cherror
		/*
		 * Parse the path.
		 * Example: in /Users/paul/sounds/h*.wav",
		 * the search directory is "/Users/paul/sounds",
		 * the left environment is "h", and the right environment is ".wav".
		 */
		strcpy (searchDirectory, path);
		left [0] = right [0] = '\0';
		asterisk = strrchr (searchDirectory, '*');
		if (asterisk) {
			*asterisk = '\0';
			if ((lastSlash = strrchr (searchDirectory, '/')) != NULL) {
				*lastSlash = '\0';   /* This fixes searchDirectory. */
				strcpy (left, lastSlash + 1);
			} else {
				strcpy (left, searchDirectory);   /* Quickly save... */
				searchDirectory [0] = '\0';   /* ...before destruction. */
			}
			strcpy (right, asterisk + 1);
		}
		leftLength = strlen (left), rightLength = strlen (right);
		{
			DIR *d = opendir (searchDirectory [0] ? searchDirectory : ".");
			struct dirent *entry;
			if (d == NULL) { forget (me); return Melder_errorp ("Cannot open directory %s.", searchDirectory); }
			my strings = NUMpvector (1, 1000000); cherror
			while ((entry = readdir (d)) != NULL) {
				#ifndef DT_DIR
					#define DT_DIR  4
				#endif
				if (entry -> d_type == DT_DIR) {
					char *fileName = entry -> d_name;
					int length = strlen (fileName);
					if (fileName [0] != '.' &&
					    (leftLength == 0 || strnequ (entry -> d_name, left, leftLength)) &&
					    (rightLength == 0 || (length >= rightLength && strequ (entry -> d_name + (length - rightLength), right))))
					{
						my strings [++ my numberOfStrings] = Melder_strdup (entry -> d_name); cherror
					}
				}
			}
			closedir (d);
		}
	#elif defined (_WIN32)
		HANDLE searchHandle;
		WIN32_FIND_DATA findData;
		char searchPath [300];
		int len = strlen (path), hasAsterisk = strchr (path, '*') != NULL, endsInSeparator = len != 0 && path [len - 1] == '\\';
		my strings = NUMpvector (1, 1000000); cherror
		sprintf (searchPath, "%s%s%s", path, hasAsterisk || endsInSeparator ? "" : "\\", hasAsterisk ? "" : "*");
		searchHandle = FindFirstFile (searchPath, & findData);
		if (searchHandle != INVALID_HANDLE_VALUE) {
			do {
				if ((findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
					my strings [++ my numberOfStrings] = Melder_strdup (findData. cFileName); cherror
				}
			} while (FindNextFile (searchHandle, & findData));
			FindClose (searchHandle);
		}
	#endif
end:
	iferror forget (me);
	return me;
}

Strings Strings_readFromRawTextFile (MelderFile file) {
	Strings me = NULL;
	long n, i;
	char *line;

	MelderFile_open (file); cherror

	/*
	 * Count number of strings.
	 */
	for (n = 0;; n++) {
		line = MelderFile_readLine (file); cherror
		if (line == NULL) break;
	}

	/*
	 * Create.
	 */
	me = new (Strings);
	if (n > 0) my strings = NUMpvector (1, n); cherror
	my numberOfStrings = n;

	/*
	 * Read strings.
	 */
	MelderFile_rewind (file);
	for (i = 1; i <= n; i ++) {
		line = MelderFile_readLine (file); cherror
		my strings [i] = Melder_strdup (line); cherror
	}

end:
	MelderFile_close (file);
	iferror { forget (me); return Melder_errorp (
		"(Strings_readFromRawTextFile:) File %s not read.", MelderFile_messageName (file)); }
	return me;
}

int Strings_writeToRawTextFile (Strings me, MelderFile fs) {
	long i;
	FILE *f = Melder_fopen (fs, "w"); cherror
	for (i = 1; i <= my numberOfStrings; i ++) {
		fprintf (f, "%s\n", my strings [i]);
	}
end:
	Melder_fclose (fs, f);
	iferror return 0;
	MelderFile_setMacTypeAndCreator (fs, 'TEXT', 0);
	return 1;
}

void Strings_randomize (Strings me) {
	long i;
	for (i = 1; i < my numberOfStrings; i ++) {
		long other = NUMrandomInteger (i, my numberOfStrings);
		char *dummy = my strings [other];
		my strings [other] = my strings [i];
		my strings [i] = dummy;
	}
}

int Strings_genericize (Strings me) {
	long i;
	char *buffer = Melder_malloc (Strings_maximumLength (me) * 3 + 1); cherror
	for (i = 1; i <= my numberOfStrings; i ++) {
		const unsigned char *p = (const unsigned char *) my strings [i];
		while (*p) {
			if (*p > 126) {   /* Backslashes are not converted, i.e. genericize^2 == genericize. */
				char *newString;
				Longchar_genericize (my strings [i], buffer);
				newString = Melder_strdup (buffer); cherror
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
	NUMsort_str (my numberOfStrings, my strings);
}

/* End of file Strings.c */
