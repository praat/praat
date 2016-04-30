/* SpellingChecker.cpp
 *
 * Copyright (C) 1999-2011,2015 Paul Boersma
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "SpellingChecker.h"
#include <ctype.h>

#include "oo_DESTROY.h"
#include "SpellingChecker_def.h"
#include "oo_COPY.h"
#include "SpellingChecker_def.h"
#include "oo_EQUAL.h"
#include "SpellingChecker_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SpellingChecker_def.h"
#include "oo_WRITE_TEXT.h"
#include "SpellingChecker_def.h"
#include "oo_WRITE_BINARY.h"
#include "SpellingChecker_def.h"
#include "oo_READ_TEXT.h"
#include "SpellingChecker_def.h"
#include "oo_READ_BINARY.h"
#include "SpellingChecker_def.h"
#include "oo_DESCRIPTION.h"
#include "SpellingChecker_def.h"

#include "longchar.h"

Thing_implement (SpellingChecker, Daata, 0);

autoSpellingChecker WordList_upto_SpellingChecker (WordList me) {
	try {
		autoSpellingChecker thee = Thing_new (SpellingChecker);
		thy wordList = Data_copy (me);
		thy userDictionary = StringSet_create ();
		thy separatingCharacters = Melder_dup (U".,;:()\"");
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to SpellingChecker.");
	}
}

autoWordList SpellingChecker_extractWordList (SpellingChecker me) {
	return Data_copy (my wordList.get());
}

void SpellingChecker_replaceWordList (SpellingChecker me, WordList list) {
	try {
		my wordList = Data_copy (list);
	} catch (MelderError) {
		Melder_throw (me, U": word list not replaced.");
	}
}

autoStringSet SpellingChecker_extractUserDictionary (SpellingChecker me) {
	try {
		if (my userDictionary->size == 0)
			Melder_throw (U"This spelling checker does not contain a user dictionary.");
		return Data_copy (my userDictionary.get());
	} catch (MelderError) {
		Melder_throw (me, U": user dictionary not extracted.");
	}
}

void SpellingChecker_replaceUserDictionary (SpellingChecker me, StringSet userDictionary) {
	try {
		my userDictionary = Data_copy (userDictionary);
	} catch (MelderError) {
		Melder_throw (me, U": user dictionary not replaced.");
	}
}

static int startsWithCapital (const char32 *word) {
	return iswupper ((int) word [0]) || (word [0] == '\\' && iswupper ((int) word [1]));
}

bool SpellingChecker_isWordAllowed (SpellingChecker me, const char32 *word) {
	int wordLength = str32len (word);
	if (my allowAllWordsContaining && my allowAllWordsContaining [0]) {
		char32 *p = & my allowAllWordsContaining [0];
		while (*p) {
			/*
			 * Find next token in list of allowed string parts.
			 */
			char32 token [100], *q = & token [0];
			/*
			 * Skip spaces in list.
			 */
			while (*p == U' ') p ++;
			/*
			 * Collect one token string from list.
			 */
			while (*p != U'\0' && *p != U' ') {
				*q ++ = *p ++;
			}
			*q = U'\0';   // trailing null character
			/*
			 * Allow word if it contains this token.
			 */
			if (str32str (word, token)) return true;
		}
	}
	if (my allowAllNames) {
		/*
		 * Allow word if it starts with a capital.
		 */
		if (startsWithCapital (word)) {
			return true;
		}
		if (my namePrefixes && my namePrefixes [0]) {
			char32 *p = & my namePrefixes [0];
			while (*p) {
				char32 token [100], *q = & token [0];
				while (*p == U' ') p ++;
				while (*p != U'\0' && *p != U' ') *q ++ = *p ++;
				*q = U'\0';   // trailing null character
				/*
				 * Allow word if starts with this prefix
				 * and this prefix is followed by a capital.
				 */
				if (str32str (word, token) == word && startsWithCapital (word + str32len (token))) {
					return true;
				}
			}
		}
	} else if (my allowAllAbbreviations && startsWithCapital (word)) {
		const char32 *p = & word [0];
		for (;;) {
			if (*p == '\0') return true;
			if (iswlower ((int) *p)) break;
			p ++;
		}
	}
	if (my allowAllWordsStartingWith && my allowAllWordsStartingWith [0]) {
		char32 *p = & my allowAllWordsStartingWith [0];
		while (*p) {
			char32 token [100], *q = & token [0];
			int tokenLength;
			while (*p == U' ') p ++;
			while (*p != U'\0' && *p != U' ') *q ++ = *p ++;
			*q = U'\0';   // trailing null character
			tokenLength = str32len (token);
			if (wordLength >= tokenLength && str32nequ (token, word, tokenLength)) {
				return true;
			}
		}
	}
	if (my allowAllWordsEndingIn && my allowAllWordsEndingIn [0]) {
		char32 *p = & my allowAllWordsEndingIn [0];
		while (*p) {
			char32 token [100], *q = & token [0];
			int tokenLength;
			while (*p == U' ') p ++;
			while (*p != U'\0' && *p != U' ') *q ++ = *p ++;
			*q = U'\0';   // trailing null character
			tokenLength = str32len (token);
			if (wordLength >= tokenLength && str32nequ (token, word + wordLength - tokenLength, tokenLength)) {
				return true;
			}
		}
	}
	if (WordList_hasWord (my wordList.get(), word))
		return true;
	if (my userDictionary->size > 0) {
		if (str32len (word) > 3333) return false;   // superfluous, because WordList_hasWord already checked; but safe
		static char32 buffer [3*3333+1];
		Longchar_genericize32 (word, buffer);
		if (my userDictionary -> lookUp (buffer) != 0)
			return true;
	}
	return false;
}

void SpellingChecker_addNewWord (SpellingChecker me, const char32 *word) {
	try {
		autostring32 generic = Melder_calloc (char32, 3 * str32len (word) + 1);
		Longchar_genericize32 (word, generic.peek());
		my userDictionary -> addString_copy (generic.transfer());
	} catch (MelderError) {
		Melder_throw (me, U": word \"", word, U"\" not added.");
	}
}

char32 * SpellingChecker_nextNotAllowedWord (SpellingChecker me, const char32 *sentence, long *start) {
	const char32 *p = sentence + *start;
	for (;;) {
		if (*p == U'\0') {
			return nullptr;   // all words allowed
		} else if (*p == U'(' && my allowAllParenthesized) {
			p ++;
			for (;;) {
				if (*p == U'\0') {
					return nullptr;   // everything is parenthesized...
				} else if (*p == U')') {
					p ++;
					break;
				} else {
					p ++;
				}
			}
		} else if (*p == U' ' || (my separatingCharacters && str32chr (my separatingCharacters, *p))) {
			p ++;
		} else {
			static char32 word [100];
			char32 *q = & word [0];
			*start = p - sentence;
			for (;;) {
				if (*p == U'\0' || *p == U' ' || (my separatingCharacters && str32chr (my separatingCharacters, *p))) {
					*q ++ = U'\0';
					if (SpellingChecker_isWordAllowed (me, word)) {
						/* Don't increment p (may contain a zero or a parenthesis). */
						break;
					} else {
						return word;
					}
				} else {
					*q ++ = *p ++;
				}
			}
		}
	}
	return nullptr;   // all words allowed
}

/* End of file SpellingChecker.cpp */
