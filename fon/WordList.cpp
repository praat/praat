/* WordList.cpp
 *
 * Copyright (C) 1999-2007,2011,2012,2015-2019 Paul Boersma
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
#include "../kar/longchar.h"
#include "Collection.h"

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
#include "oo_WRITE_BINARY.h"
#include "WordList_def.h"
#include "oo_DESCRIPTION.h"
#include "WordList_def.h"

Thing_implement (WordList, Daata, 1);

static integer WordList_count (WordList me) {
	integer numberOfWords = 0;
	for (char32 *p = & my string [0]; *p; p ++) {
		if (*p == U'\n')
			numberOfWords += 1;
	}
	return numberOfWords;
}

void structWordList :: v_info () {
	structDaata :: v_info ();
	integer n = WordList_count (this);
	if (our length == 0)
		our length = str32len (our string.get());
	MelderInfo_writeLine (U"Number of words: ", n);
	MelderInfo_writeLine (U"Number of characters: ", length - n);
}

void structWordList :: v_readBinary (FILE *f, int formatVersion) {
	if (formatVersion <= 0) {
		our length = bingeti32 (f);
		if (our length < 0)
			Melder_throw (U"Wrong length ", our length, U".");
		our string = autostring32 (our length);
		char32 *current = & our string [0], *p = current;
		int kar = 0;
		if (our length > 0) {
			/*
			 * Read first word.
			 */
			for (;;) {
				if (p - & string [0] >= length - 1) break;
				kar = fgetc (f);
				if (kar == EOF)
					Melder_throw (U"Early end of file.");
				if (kar >= 128)
					break;
				*p ++ = (char32) kar;
			}
			*p ++ = U'\n';
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
					if (p - & string [0] >= length - 1) break;
					kar = fgetc (f);
					if (kar == EOF)
						Melder_throw (U"Early end of file.");
					if (kar >= 128)
						break;
					*p ++ = (char32) kar;
				}
				*p ++ = U'\n';
				if (p - & string [0] >= our length)
					break;
			}
		}
		*p = U'\0';
		if (p - & our string [0] != our length)
			Melder_throw (U"Length in header (", our length, U") does not match length of string (", (integer) (p - & our string [0]), U").");
	} else {
		our string = bingetw32 (f);
		our length = str32len (our string.get());
	}
}

static autoStringSet Strings_to_StringSet (Strings me, bool nativize) {
	/*
		Make sure that the strings are normalized, sorted, and unique.
	*/
	autoStringSet you = StringSet_create ();
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		autoSimpleString item = SimpleString_create (my strings [i].get());   // TODO: normalize
		your addItem_unsorted_move (item.move());
	}
	your sort ();
	your unicize ();
	return you;
}

autoWordList Strings_to_WordList (Strings me) {
	try {
		/*
			Check whether the strings are all words.
		*/
		for (integer i = 1; i <= my numberOfStrings; i ++)
			if (Melder_findHorizontalOrVerticalSpace (my strings [i].get()))
				Melder_throw (U"String ", i, U" contains a space and can therefore not be included in a WordList.");

		autoStringSet you = Strings_to_StringSet (me, true);

		integer totalLength = 0;
		for (integer i = 1; i <= your size; i ++)
			totalLength += str32len (your at [i]->string.get()) + 1;   // include trailing newline symbol

		autoWordList him = Thing_new (WordList);
		his length = totalLength;
		his string = autostring32 (his length);
		/*
			Concatenate the strings into the word list.
		*/
		char32 *q = & his string [0];
		for (integer i = 1; i <= your size; i ++) {
			str32cpy (q, your at [i]->string.get());
			q += str32len (your at [i]->string.get());
			*q ++ = U'\n';
		}
		*q = U'\0';
		Melder_assert (q - & his string [0] == his length);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to WordList.");
	}
}

autoStrings WordList_to_Strings (WordList me) {
	try {
		const char32 *word = & my string [0];
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = WordList_count (me);
		if (thy numberOfStrings > 0)
			thy strings = autoSTRVEC (thy numberOfStrings);
		for (integer i = 1; i <= thy numberOfStrings; i ++) {
			const char32 *kar = word;
			for (; *kar != U'\n'; kar ++) { }
			integer length = kar - word;
			thy strings [i] = autostring32 (length);
			str32ncpy (thy strings [i].get(), word, length);
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

static int compare (conststring32 word, conststring32 p) {
	for (;;) {
		if (*word == U'\0') {
			if (*p == U'\n') return 0;
			else return -1;   // word is substring of p
		}
		if (*p == U'\n') return +1;   // p is substring of word
		if (*word < *p) return -1;
		if (*word > *p) return +1;
		word ++;
		p ++;
	}
	return 0;   // should not occur
}

static char32 buffer [3333+1];

bool WordList_hasWord (WordList me, conststring32 word) {
	if (str32len (word) > 3333)
		return false;
	Longchar_nativize (word, buffer, false);
	if (! my length)
		my length = str32len (my string.get());
	integer p = my length / 2, d = p / 2;
	while (d > 20) {
		p = gotoStart (me, p);
		int cf = compare (buffer, my string.get() + p);
		if (cf == 0) return true;
		if (cf < 0) p -= d; else p += d;
		d /= 2;
	}
	p = gotoStart (me, p);
	int cf = compare (buffer, my string.get() + p);
	if (cf == 0) return true;
	if (cf > 0) {
		for (;;) {
			p = gotoNext (me, p);
			if (p >= my length) return false;
			cf = compare (buffer, my string.get() + p);
			if (cf == 0) return true;
			if (cf < 0) return false;
		}
	} else {
		for (;;) {
			p = gotoPrevious (me, p);
			if (p < 0) return false;
			cf = compare (buffer, my string.get() + p);
			if (cf == 0) return true;
			if (cf > 0) return false;
		}
	}
	return false;   // should not occur
}

/* End of file WordList.cpp */
