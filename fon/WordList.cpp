/* WordList.cpp
 *
 * Copyright (C) 1999-2012,2015,2017 Paul Boersma
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

Thing_implement (WordList, Daata, 0);

static integer WordList_count (WordList me) {
	integer n = 0;
	for (char32 *p = my string; *p; p ++) {
		if (*p == '\n') n += 1;
	}
	return n;
}

void structWordList :: v_info () {
	structDaata :: v_info ();
	integer n = WordList_count (this);
	if (! our length) our length = str32len (our string);
	MelderInfo_writeLine (U"Number of words: ", n);
	MelderInfo_writeLine (U"Number of characters: ", length - n);
}

void structWordList :: v_readBinary (FILE *f, int /*formatVersion*/) {
	char32 *current, *p;
	int kar = 0;
	our length = bingeti32 (f);
	if (our length < 0)
		Melder_throw (U"Wrong length ", our length, U".");
	string = Melder_calloc (char32, our length + 1);
	p = current = string;
	if (our length > 0) {
		/*
		 * Read first word.
		 */
		for (;;) {
			if (p - string >= length - 1) break;
			kar = fgetc (f);
			if (kar == EOF)
				Melder_throw (U"Early end of file.");
			if (kar >= 128) break;
			*p ++ = kar;
		}
		*p ++ = '\n';
		/*
		 * Read following words.
		 */
		for (;;) {
			char32 *previous = current;
			int numberOfSame = kar - 128;
			current = p;
			str32ncpy (current, previous, numberOfSame);
			p += numberOfSame;
			for (;;) {
				if (p - string >= length - 1) break;
				kar = fgetc (f);
				if (kar == EOF)
					Melder_throw (U"Early end of file.");
				if (kar >= 128) break;
				*p ++ = kar;
			}
			*p ++ = '\n';
			if (p - string >= length) break;
		}
	}
	*p = '\0';
	if (p - string != length)
		Melder_throw (U"Length in header (", length, U") does not match lenth of string (", (integer) (p - string), U").");
}

void structWordList :: v_writeBinary (FILE *f) {
	integer currentLength, previousLength;
	if (! length) length = str32len (string);
	binputi32 (length, f);
	if (length > 0) {
		char32 *current = string, *kar = current;
		for (kar = current; *kar != U'\n'; kar ++) { }
		currentLength = kar - current;
		for (integer i = 0; i < currentLength; i ++)
			fputc ((int) current [i], f);   // TODO: check
		for (;;) {
			char32 *previous = current, *kar1, *kar2;
			int numberOfSame;
			previousLength = currentLength;
			current = previous + previousLength + 1;
			if (*current == U'\0') break;
			kar1 = previous, kar2 = current;
			while (*kar2 != U'\n' && *kar2 == *kar1) {
				kar1 ++, kar2 ++;
			}
			numberOfSame = kar2 - current;
			if (numberOfSame > 127) numberOfSame = 127;   // clip
			fputc (128 + numberOfSame, f);
			while (*kar2 != U'\n') kar2 ++;
			currentLength = kar2 - current;
			for (integer i = 0; i < currentLength - numberOfSame; i ++)
				fputc ((int) current [numberOfSame + i], f);   // TODO: check
		}
	}
}

autoWordList Strings_to_WordList (Strings me) {
	try {
		integer totalLength = 0;
		/*
		 * Check whether the strings are generic and sorted.
		 */
		for (integer i = 1; i <= my numberOfStrings; i ++) {
			char32 *string = my strings [i], *p;
			for (p = & string [0]; *p; p ++) {
				if (*p > 126)
					Melder_throw (U"String \"", string, U"\" not generic.\nPlease convert to backslash trigraphs first.");
			}
			if (i > 1 && str32cmp (my strings [i - 1], string) > 0) {
				Melder_throw (U"String \"", string, U"\" not sorted.\nPlease sort first.");
			}
			totalLength += str32len (string);
		}
		autoWordList thee = Thing_new (WordList);
		thy length = totalLength + my numberOfStrings;
		thy string = Melder_calloc (char32, thy length + 1);
		/*
		 * Concatenate the strings into the word list.
		 */
		char32 *q = thy string;
		for (integer i = 1; i <= my numberOfStrings; i ++) {
			integer length = str32len (my strings [i]);
			str32cpy (q, my strings [i]);
			q += length;
			*q ++ = '\n';
		}
		*q = U'\0';
		Melder_assert (q - thy string == thy length);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to WordList.");
	}
}

autoStrings WordList_to_Strings (WordList me) {
	try {
		unsigned char *word = (unsigned char *) my string;   // BUG: explain this
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = WordList_count (me);
		if (thy numberOfStrings > 0) {
			thy strings = NUMvector <char32 *> (1, thy numberOfStrings);
		}
		for (integer i = 1; i <= thy numberOfStrings; i ++) {
			unsigned char *kar = word;
			for (; *kar != '\n'; kar ++) { }
			integer length = kar - word;
			thy strings [i] = Melder_calloc (char32, length + 1);
			str32ncpy (thy strings [i], Melder_peek8to32 ((const char *) word), length);
			thy strings [i] [length] = U'\0';
			word += length + 1;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Strings.");
	}
}

static integer gotoStart (WordList me, integer p) {
	if (p <= 0) return 0;
	-- p;
	while (p >= 0 && my string [p] != U'\n') p --;
	return p + 1;
}

static integer gotoNext (WordList me, integer p) {
	if (p >= my length - 1) return my length;
	while (my string [p] != U'\n') p ++;
	return p + 1;
}

static integer gotoPrevious (WordList me, integer p) {
	if (p <= 0) return -1;
	if (my string [-- p] != U'\n') return -1;   // should not occur
	if (p <= 0) return 0;   // if first word is empty
	-- p;   // step from newline
	while (p >= 0 && my string [p] != U'\n') p --;
	return p + 1;
}

static int compare (const char32 *word, const char32 *p) {
	for (;;) {
		if (*word == U'\0') {
			if (*p == U'\n') return 0;
			else return -1;   // word is substring of p
		}
		if (*p == U'\n') return +1;   // p is substring of word
		if (*word < *p) return -1;
		if (*word > *p) return +1;
		word ++, p ++;
	}
	return 0;   // should not occur
}

static char32 buffer [3333+1];

bool WordList_hasWord (WordList me, const char32 *word) {
	if (str32len (word) > 3333) return false;
	Longchar_genericize32 (word, buffer);
	if (! my length) my length = str32len (my string);
	integer p = my length / 2, d = p / 2;
	while (d > 20) {
		p = gotoStart (me, p);
		int cf = compare (buffer, my string + p);
		if (cf == 0) return true;
		if (cf < 0) p -= d; else p += d;
		d /= 2;
	}
	p = gotoStart (me, p);
	int cf = compare (buffer, my string + p);
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
