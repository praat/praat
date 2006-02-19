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
 * pb 2004/03/21 createAsFileList now accepts spaces in directory names on Unix and Mac
 * pb 2004/04/20 the previous thing now done with backslashes rather than double quotes,
 *               because double quotes prevent the expansion of the wildcard asterisk
 * pb 2006/02/14 Strings_createAsDirectoryList
 */

#include "Strings.h"
#include "longchar.h"
#if defined (_WIN32)
	#include <windows.h>
#elif defined (macintosh)
	#include <Files.h>
#endif
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
	#if defined (UNIX) || defined (__MACH__)
		FILE *f;
		#if defined (linux) || defined (__MACH__)
			#define LS_COMMAND "/bin/ls "
		#else
			#define LS_COMMAND "/usr/bin/ls "
		#endif
		char command [300], buf [300], *p;
		my strings = NUMpvector (1, 10000); cherror
		sprintf (command, LS_COMMAND "%s", path);
		/*
		 * Prepend all spaces with backslashes.
		 */
		for (p = command + strlen (LS_COMMAND); *p != '\0'; p ++) {
			if (*p == ' ') {
				char *q = command + strlen (command);
				q [1] = '\0';
				while (q - p > 0) { q [0] = q [-1]; q --; }
				q [0] = '\\';
				p ++;
			}
		}
		if ((f = popen (command, "r")) == NULL) {
			forget (me);
			return Melder_errorp ("(Strings_fromFilePath:) Cannot open pipe.");
		}
		while (fgets (buf, 300, f) != NULL) {
			char *newLine = strrchr (buf, '\n');
			char *lastSlash = strrchr (buf, '/');
			if (newLine) *newLine = '\0';
			my strings [++ my numberOfStrings] = Melder_strdup (lastSlash ? lastSlash + 1 : buf); cherror
		}
		pclose (f);
	#elif defined (_WIN32)
		HANDLE searchHandle;
		WIN32_FIND_DATA findData;
		char searchPath [300];
		int len = strlen (path), hasAsterisk = strchr (path, '*') != NULL, endsInSeparator = len != 0 && path [len - 1] == '\\';
		my strings = NUMpvector (1, 10000); cherror
		sprintf (searchPath, "%s%s%s", path, hasAsterisk || endsInSeparator ? "" : "\\", hasAsterisk ? "" : "*");
		searchHandle = FindFirstFile (searchPath, & findData);
		if (searchHandle == INVALID_HANDLE_VALUE) { Melder_error ("Cannot find first file."); goto end; }
		do {
			if (! (findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				my strings [++ my numberOfStrings] = Melder_strdup (findData. cFileName); cherror
			}
		} while (FindNextFile (searchHandle, & findData));
		FindClose (searchHandle);
	#elif defined (macintosh)
		HFileParam pb;
		char *asterisk, left [100], right [100], searchDirectory [256], *lastColon;
		long i, leftLength, rightLength;
		structMelderDir dir;
		my strings = NUMpvector (1, 10000); cherror
		/*
		 * Parse the path.
		 * Example: in "Macintosh HD:sounds:h*.aifc",
		 * the search directory is "Macintosh HD:sounds:",
		 * the left environment is "h", and the right environment is ".aifc".
		 */
		strcpy (searchDirectory, path);
		left [0] = right [0] = '\0';
		asterisk = strrchr (searchDirectory, '*');
		if (asterisk) {
			lastColon = strrchr (searchDirectory, ':');
			/*
			 * Fix search directory.
			 */
			if (lastColon)
				*lastColon = '\0';
			else
				searchDirectory [0] = '\0';
			/*
			 * Get left environment.
			 */
			*asterisk = '\0';
			if (lastColon)
				strcpy (left, lastColon + 1);
			else
				strcpy (left, searchDirectory);
			/*
			 * Get right environment.
			 */
			strcpy (right, asterisk + 1);
		}
		lastColon = strrchr (searchDirectory, ':');
		if (lastColon != NULL && * (lastColon + 1) != '\0')
			strcat (searchDirectory, ":");
		leftLength = strlen (left), rightLength = strlen (right);
		Melder_pathToDir (searchDirectory, & dir); cherror   /* BUG: should be relative */
		pb. ioVRefNum = dir. vRefNum;
		for (i = 1;; i ++) {
			Str255 pFileName;
			char *fileName;
			int length;
			pb. ioDirID = dir. dirID;
			pb. ioFDirIndex = i;
			pFileName [0] = 0;
			pb. ioNamePtr = & pFileName [0];
			if (PBHGetFInfoSync ((HParmBlkPtr) & pb) != noErr) break;
			pFileName [pFileName [0] + 1] = '\0';   /* Add null byte at the end. */
			fileName = (char *) & pFileName [1];   /* Pascal to C string. */
			length = pFileName [0];
			if ((leftLength == 0 || strnequ (fileName, left, leftLength)) &&
					(rightLength == 0 || length >= rightLength && strequ (fileName + (length - rightLength), right)))
				my strings [++ my numberOfStrings] = Melder_strdup (fileName);
		}
	#endif
end:
	iferror forget (me);
	return me;
}

Strings Strings_createAsDirectoryList (const char *path) {
	Strings me = new (Strings);
	#if defined (_WIN32)
		HANDLE searchHandle;
		WIN32_FIND_DATA findData;
		char searchPath [300];
		int len = strlen (path), hasAsterisk = strchr (path, '*') != NULL, endsInSeparator = len != 0 && path [len - 1] == '\\';
		my strings = NUMpvector (1, 10000); cherror
		sprintf (searchPath, "%s%s%s", path, hasAsterisk || endsInSeparator ? "" : "\\", hasAsterisk ? "" : "*");
		searchHandle = FindFirstFile (searchPath, & findData);
		if (searchHandle == INVALID_HANDLE_VALUE) { Melder_error ("Cannot find first file."); goto end; }
		do {
			if ((findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
				my strings [++ my numberOfStrings] = Melder_strdup (findData. cFileName); cherror
			}
		} while (FindNextFile (searchHandle, & findData));
		FindClose (searchHandle);
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
