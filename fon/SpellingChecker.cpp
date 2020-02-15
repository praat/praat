/* SpellingChecker.cpp
 *
 * Copyright (C) 1999-2007,2011,2012,2015-2020 Paul Boersma
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

#include "SpellingChecker.h"

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

#include "../kar/longchar.h"

Thing_implement (SpellingChecker, Daata, 1);

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

static int startsWithCapital (conststring32 word) {
	return Melder_isUpperCaseLetter (word [0]);
}

bool SpellingChecker_isWordAllowed (SpellingChecker me, conststring32 word) {
	integer wordLength = str32len (word);
	if (my allowAllWordsContaining && my allowAllWordsContaining [0]) {
		char32 *p = & my allowAllWordsContaining [0];
		while (*p) {
			/*
			 * Find next token in list of allowed string parts.
			 */
			char32 token [100], *q = & token [0];
			Melder_skipHorizontalOrVerticalSpace (& p);
			/*
			 * Collect one token string from list.
			 */
			while (*p != U'\0' && *p != U' ')
				*q ++ = *p ++;
			*q = U'\0';   // trailing null character
			/*
			 * Allow word if it contains this token.
			 */
			if (str32str (word, token))
				return true;
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
				Melder_skipHorizontalOrVerticalSpace (& p);
				while (*p != U'\0' && *p != U' ') *q ++ = *p ++;
				*q = U'\0';   // trailing null character
				/*
				 * Allow word if starts with this prefix
				 * and this prefix is followed by a capital.
				 */
				if (str32str (word, token) == word && startsWithCapital (word + str32len (token)))
					return true;
			}
		}
	} else if (my allowAllAbbreviations && startsWithCapital (word)) {
		const char32 *p = & word [0];
		for (;;) {
			if (*p == U'\0')
				return true;
			if (Melder_isLowerCaseLetter (*p))
				break;
			p ++;
		}
	}
	if (my allowAllWordsStartingWith && my allowAllWordsStartingWith [0]) {
		const char32 *p = & my allowAllWordsStartingWith [0];
		while (*p) {
			char32 token [100], *q = & token [0];
			Melder_skipHorizontalOrVerticalSpace (& p);
			while (*p != U'\0' && *p != U' ')
				*q ++ = *p ++;
			*q = U'\0';   // trailing null character
			integer tokenLength = str32len (token);
			if (wordLength >= tokenLength && str32nequ (token, word, tokenLength))
				return true;
		}
	}
	if (my allowAllWordsEndingIn && my allowAllWordsEndingIn [0]) {
		const char32 *p = & my allowAllWordsEndingIn [0];
		while (*p) {
			char32 token [100], *q = & token [0];
			Melder_skipHorizontalOrVerticalSpace (& p);
			while (*p != U'\0' && *p != U' ')
				*q ++ = *p ++;
			*q = U'\0';   // trailing null character
			integer tokenLength = str32len (token);
			if (wordLength >= tokenLength && str32nequ (token, word + wordLength - tokenLength, tokenLength))
				return true;
		}
	}
	if (WordList_hasWord (my wordList.get(), word))
		return true;
	if (my userDictionary->size > 0) {
		if (str32len (word) > 3333) return false;   // superfluous, because WordList_hasWord already checked; but safe
		static char32 buffer [3*3333+1];
		Longchar_nativize (word, buffer, false);
		if (my userDictionary -> lookUp (buffer) != 0)
			return true;
	}
	return false;
}

void SpellingChecker_addNewWord (SpellingChecker me, conststring32 word) {
	try {
		Melder_require (word && word [0] != U'\0',
			U"Cannot add empty word.");
		my userDictionary -> addString_copy (word);
	} catch (MelderError) {
		Melder_throw (me, U": word \"", word, U"\" not added.");
	}
}

char32 * SpellingChecker_nextNotAllowedWord (SpellingChecker me, conststring32 sentence, integer *start) {
	const char32 *p = & sentence [*start];
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
		} else if (*p == U' ' || (my separatingCharacters && str32chr (my separatingCharacters.get(), *p))) {
			p ++;
		} else {
			static char32 word [100];
			char32 *q = & word [0];
			*start = p - sentence;
			for (;;) {
				if (*p == U'\0' || *p == U' ' || (my separatingCharacters && str32chr (my separatingCharacters.get(), *p))) {
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
