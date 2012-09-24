/* WordList.cpp
 *
 * Copyright (C) 1999-2012 Paul Boersma
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

/* BUG: not Unicode-savvy */

Thing_implement (WordList, Data, 0);

static long WordList_count (WordList me) {
	long n = 0;
	for (wchar_t *p = my string; *p; p ++) {
		if (*p == '\n') n += 1;
	}
	return n;
}

void structWordList :: v_info () {
	structData :: v_info ();
	long n = WordList_count (this);
	if (! length) length = wcslen (string);
	MelderInfo_writeLine (L"Number of words: ", Melder_integer (n));
	MelderInfo_writeLine (L"Number of characters: ", Melder_integer (length - n));
}

void structWordList :: v_readBinary (FILE *f) {
	wchar_t *current, *p;
	int kar = 0;
	length = bingeti4 (f);
	if (length < 0)
		Melder_throw ("Wrong length ", length, ".");
	string = Melder_calloc (wchar_t, length + 1);
	p = current = string;
	if (length > 0) {
		/*
		 * Read first word.
		 */
		for (;;) {
			if (p - string >= length - 1) break;
			kar = fgetc (f);
			if (kar == EOF)
				Melder_throw (L"Early end of file.");
			if (kar >= 128) break;
			*p ++ = kar;
		}
		*p ++ = '\n';
		/*
		 * Read following words.
		 */
		for (;;) {
			wchar_t *previous = current;
			int numberOfSame = kar - 128;
			current = p;
			wcsncpy (current, previous, numberOfSame);
			p += numberOfSame;
			for (;;) {
				if (p - string >= length - 1) break;
				kar = fgetc (f);
				if (kar == EOF)
					Melder_throw (L"Early end of file.");
				if (kar >= 128) break;
				*p ++ = kar;
			}
			*p ++ = '\n';
			if (p - string >= length) break;
		}
	}
	*p = '\0';
	if (p - string != length)
		Melder_throw ("Length in header (", length, ") does not match lenth of string (", (long) (p - string), ").");
}

void structWordList :: v_writeBinary (FILE *f) {
	long currentLength, previousLength;
	if (! length) length = wcslen (string);
	binputi4 (length, f);
	if (length > 0) {
		wchar_t *current = string, *kar = current;
		for (kar = current; *kar != '\n'; kar ++) { }
		currentLength = kar - current;
		for (long i = 0; i < currentLength; i ++)
			fputc (current [i], f);   // TODO: check
		for (;;) {
			wchar_t *previous = current, *kar1, *kar2;
			int numberOfSame;
			previousLength = currentLength;
			current = previous + previousLength + 1;
			if (*current == '\0') break;
			kar1 = previous, kar2 = current;
			while (*kar2 != '\n' && *kar2 == *kar1) {
				kar1 ++, kar2 ++;
			}
			numberOfSame = kar2 - current;
			if (numberOfSame > 127) numberOfSame = 127;   // clip
			fputc (128 + numberOfSame, f);
			while (*kar2 != '\n') kar2 ++;
			currentLength = kar2 - current;
			for (long i = 0; i < currentLength - numberOfSame; i ++)
				fputc (current [numberOfSame + i], f);   // TODO: check
		}
	}
}

WordList Strings_to_WordList (Strings me) {
	try {
		long totalLength = 0;
		/*
		 * Check whether the strings are generic and sorted.
		 */
		for (long i = 1; i <= my numberOfStrings; i ++) {
			wchar_t *string = my strings [i], *p;
			for (p = & string [0]; *p; p ++) {
				if (*p > 126)
					Melder_throw ("String \"", string, "\" not generic.\nPlease convert to backslash trigraphs first.");
			}
			if (i > 1 && wcscmp (my strings [i - 1], string) > 0) {
				Melder_throw ("String \"", string, L"\" not sorted.\nPlease sort first.");
			}
			totalLength += wcslen (string);
		}
		autoWordList thee = Thing_new (WordList);
		thy length = totalLength + my numberOfStrings;
		thy string = Melder_calloc (wchar_t, thy length + 1);
		/*
		 * Concatenate the strings into the word list.
		 */
		wchar_t *q = thy string;
		for (long i = 1; i <= my numberOfStrings; i ++) {
			long length = wcslen (my strings [i]);
			wcscpy (q, my strings [i]);
			q += length;
			*q ++ = '\n';
		}
		*q = '\0';
		Melder_assert (q - thy string == thy length);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to WordList.");
	}
}

Strings WordList_to_Strings (WordList me) {
	try {
		unsigned char *word = (unsigned char *) my string;
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = WordList_count (me);
		if (thy numberOfStrings > 0) {
			thy strings = NUMvector <wchar_t *> (1, thy numberOfStrings);
		}
		for (long i = 1; i <= thy numberOfStrings; i ++) {
			unsigned char *kar = word;
			for (; *kar != '\n'; kar ++) { }
			long length = kar - word;
			thy strings [i] = Melder_calloc (wchar_t, length + 1);
			wcsncpy (thy strings [i], Melder_peekUtf8ToWcs ((const char *) word), length);
			thy strings [i] [length] = '\0';
			word += length + 1;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Strings.");
	}
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
	if (my string [-- p] != '\n') return -1;   // should not occur
	if (p <= 0) return 0;   // if first word is empty
	-- p;   // step from newline
	while (p >= 0 && my string [p] != '\n') p --;
	return p + 1;
}

static int compare (const wchar_t *word, const wchar_t *p) {
	for (;;) {
		if (*word == '\0') {
			if (*p == '\n') return 0;
			else return -1;   // word is substring of p
		}
		if (*p == '\n') return +1;   // p is substring of word
		if (*word < *p) return -1;
		if (*word > *p) return +1;
		word ++, p ++;
	}
	return 0;   // should not occur
}

static wchar_t buffer [3333+1];

bool WordList_hasWord (WordList me, const wchar_t *word) {
	long p, d;
	int cf;
	if (wcslen (word) > 3333) return false;
	Longchar_genericizeW (word, buffer);
	if (! my length) my length = wcslen (my string);
	p = my length / 2, d = p / 2;
	while (d > 20) {
		p = gotoStart (me, p);
		cf = compare (buffer, my string + p);
		if (cf == 0) return true;
		if (cf < 0) p -= d; else p += d;
		d /= 2;
	}
	p = gotoStart (me, p);
	cf = compare (buffer, my string + p);
	if (cf == 0) return true;
	if (cf > 0) {
		for (;;) {
			p = gotoNext (me, p);
			if (p >= my length) return false;
			cf = compare (buffer, my string + p);
			if (cf == 0) return true;
			if (cf < 0) return false;
		}
	} else {
		for (;;) {
			p = gotoPrevious (me, p);
			if (p < 0) return false;
			cf = compare (buffer, my string + p);
			if (cf == 0) return true;
			if (cf > 0) return false;
		}
	}
	return false;   // should not occur
}

/* End of file WordList.cpp */
