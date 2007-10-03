/* WordList.c
 *
 * Copyright (C) 1999-2007 Paul Boersma
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
 * pb 2006/12/10 MelderInfo
 * pb 2007/10/01 can write as encoding
 */

#include "WordList.h"
#include "longchar.h"

#include "oo_DESTROY.h"
#include "WordList_def.h"
#include "oo_COPY.h"
#include "WordList_def.h"
#include "oo_EQUAL.h"
#include "WordList_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "WordList_def.h"
#include "oo_WRITE_TEXT.h"
#include "WordList_def.h"
#include "oo_READ_TEXT.h"
#include "WordList_def.h"
#include "oo_DESCRIPTION.h"
#include "WordList_def.h"

static long WordList_count (WordList me) {
	long n = 0;
	unsigned char *p;
	for (p = (unsigned char *) my string; *p; p ++) {
		if (*p == '\n') n += 1;
	}
	return n;
}

static void info (I) {
	iam (WordList);
	classData -> info (me);
	long n = WordList_count (me);
	if (! my length) my length = strlen (my string);
	MelderInfo_writeLine2 (L"Number of words: ", Melder_integer (n));
	MelderInfo_writeLine2 (L"Number of characters: ", Melder_integer (my length - n));
}

static int readBinary (I, FILE *f) {
	iam (WordList);
	unsigned char *current, *p;
	int kar = 0;
	my length = bingeti4 (f);
	if (my length < 0)
		return Melder_error ("(WordList::readBinary:) Wrong length %ld.", my length);
	my string = Melder_malloc (char, my length + 1);
	p = current = (unsigned char *) my string;
	if (my length > 0) {
		/*
		 * Read first word.
		 */
		for (;;) {
			if (p - (unsigned char *) my string >= my length - 1) break;
			kar = fgetc (f);
			if (kar == EOF) return Melder_error ("(WordList::readBinary:) Early end of file.");
			if (kar >= 128) break;
			*p ++ = kar;
		}
		*p ++ = '\n';
		/*
		 * Read following words.
		 */
		for (;;) {
			unsigned char *previous = current;
			int numberOfSame = kar - 128;
			current = p;
			strncpy ((char *) current, (const char *) previous, numberOfSame);
			p += numberOfSame;
			for (;;) {
				if (p - (unsigned char *) my string >= my length - 1) break;
				kar = fgetc (f);
				if (kar == EOF) return Melder_error ("(WordList::readBinary:) Early end of file.");
				if (kar >= 128) break;
				*p ++ = kar;
			}
			*p ++ = '\n';
			if (p - (unsigned char *) my string >= my length) break;
		}
	}
	*p = '\0';
	if (p - (unsigned char *) my string != my length) return Melder_error ("(WordList::readBinary:) "
		"Length in header (%ld) does not match string (%ld).", my length, p - (unsigned char *) my string);
	return 1;
}

static int writeBinary (I, FILE *f) {
	iam (WordList);
	long currentLength, previousLength;
	if (! my length) my length = strlen (my string);
	binputi4 (my length, f);
	if (my length > 0) {
		unsigned char *current = (unsigned char *) my string, *kar = current;
		for (kar = current; *kar != '\n'; kar ++) { }
		currentLength = kar - current;
		fwrite (current, 1, currentLength, f);
		for (;;) {
			unsigned char *previous = current, *kar1, *kar2;
			int numberOfSame;
			previousLength = currentLength;
			current = previous + previousLength + 1;
			if (*current == '\0') break;
			kar1 = previous, kar2 = current;
			while (*kar2 != '\n' && *kar2 == *kar1) {
				kar1 ++, kar2 ++;
			}
			numberOfSame = kar2 - current;
			if (numberOfSame > 127) numberOfSame = 127;   /* Clip. */
			fputc (128 + numberOfSame, f);
			while (*kar2 != '\n') kar2 ++;
			currentLength = kar2 - current;
			fwrite (current + numberOfSame, 1, currentLength - numberOfSame, f);
		}
	}
	return 1;
}

class_methods (WordList, Data) {
	class_method (info)
	class_method_local (WordList, description)
	class_method_local (WordList, destroy)
	class_method_local (WordList, copy)
	class_method_local (WordList, equal)
	class_method_local (WordList, canWriteAsEncoding)
	class_method_local (WordList, writeText)
	class_method (writeBinary)
	class_method_local (WordList, readText)
	class_method (readBinary)
	class_methods_end
}

WordList Strings_to_WordList (Strings me) {
	WordList thee = NULL;
	long totalLength = 0, i;
	char *q;
	/*
	 * Check whether the strings are generic and sorted.
	 */
	for (i = 1; i <= my numberOfStrings; i ++) {
		wchar_t *string = my strings [i], *p;
		for (p = & string [0]; *p; p ++) {
			if (*p > 126) {
				Melder_error3 (L"(Strings_to_WordList:) String \"", string, L"\" not generic.\nPlease genericize first.");
				return NULL;
			}
		}
		if (i > 1 && wcscmp (my strings [i - 1], string) > 0) {
			Melder_error3 (L"(Strings_to_WordList:) String \"", string, L"\" not sorted.\nPlease sort first.");
			return NULL;
		}
		totalLength += wcslen (string);
	}
	thee = new (WordList); cherror
	thy length = totalLength + my numberOfStrings;
	thy string = Melder_malloc (char, thy length + 1); cherror
	/*
	 * Concatenate the strings into the word list.
	 */
	q = thy string;
	for (i = 1; i <= my numberOfStrings; i ++) {
		long length = wcslen (my strings [i]);
		strcpy (q, Melder_peekWcsToUtf8 (my strings [i]));
		q += length;
		*q ++ = '\n';
	}
	*q = '\0';
	Melder_assert (q - thy string == thy length);
end:
	iferror forget (thee);
	return thee;
}

Strings WordList_to_Strings (WordList me) {
	long i;
	unsigned char *word = (unsigned char *) my string;
	Strings thee = new (Strings); cherror
	thy numberOfStrings = WordList_count (me);
	if (thy numberOfStrings > 0) { thy strings = NUMpvector (1, thy numberOfStrings); cherror }
	for (i = 1; i <= thy numberOfStrings; i ++) {
		unsigned char *kar;
		long length;
		for (kar = word; *kar != '\n'; kar ++) { }
		length = kar - word;
		thy strings [i] = Melder_calloc (wchar_t, length + 1); cherror
		wcsncpy (thy strings [i], Melder_peekUtf8ToWcs ((const char *) word), length);
		thy strings [i] [length] = '\0';
		word += length + 1;
	}
end:
	iferror forget (thee);
	return thee;
}

static long gotoStart (WordList me, long p) {
	if (p <= 0) return 0;
	-- p;
	while (p >= 0 && my string [p] != '\n') p --;
	return p + 1;
}

static long gotoNext (WordList me, long p) {
	if (p >= my length - 1) return my length;
	while (my string [p] != '\n') p ++;
	return p + 1;
}

static long gotoPrevious (WordList me, long p) {
	if (p <= 0) return -1;
	if (my string [-- p] != '\n') return -1;   /* Should not occur. */
	if (p <= 0) return 0;   /* If first word is empty. */
	-- p;   /* Step from newline. */
	while (p >= 0 && my string [p] != '\n') p --;
	return p + 1;
}

static int compare (const char *word, const char *p) {
	for (;;) {
		if (*word == '\0') {
			if (*p == '\n') return 0;
			else return -1;   /* word is substring of p. */
		}
		if (*p == '\n') return +1;   /* p is substring of word. */
		if (*word < *p) return -1;
		if (*word > *p) return +1;
		word ++, p ++;
	}
	return 0;   /* Should not occur. */
}

int WordList_hasWord (WordList me, const char *word) {
	long p, d;
	int cf;
	if (strlen (word) > 3333) return FALSE;
	Longchar_genericize (word, Melder_buffer2);
	if (! my length) my length = strlen (my string);
	p = my length / 2, d = p / 2;
	while (d > 20) {
		p = gotoStart (me, p);
		cf = compare (Melder_buffer2, my string + p);
		if (cf == 0) return TRUE;
		if (cf < 0) p -= d; else p += d;
		d /= 2;
	}
	p = gotoStart (me, p);
	cf = compare (Melder_buffer2, my string + p);
	if (cf == 0) return TRUE;
	if (cf > 0) {
		for (;;) {
			p = gotoNext (me, p);
			if (p >= my length) return FALSE;
			cf = compare (Melder_buffer2, my string + p);
			if (cf == 0) return TRUE;
			if (cf < 0) return FALSE;
		}
	} else {
		for (;;) {
			p = gotoPrevious (me, p);
			if (p < 0) return FALSE;
			cf = compare (Melder_buffer2, my string + p);
			if (cf == 0) return TRUE;
			if (cf > 0) return FALSE;
		}
	}
	return 0;   /* Should not occur. */
}

/* End of file WordList.c */
