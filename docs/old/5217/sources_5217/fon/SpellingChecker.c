/* SpellingChecker.c
 *
 * Copyright (C) 1999-2011 Paul Boersma
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
 * pb 2007/06/21 tex
 * pb 2007/08/12 wchar_t
 * pb 2007/10/01 can write as encoding
 * pb 2011/03/03 wide-character WordList
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

class_methods (SpellingChecker, Data) {
	class_method_local (SpellingChecker, description)
	class_method_local (SpellingChecker, destroy)
	class_method_local (SpellingChecker, copy)
	class_method_local (SpellingChecker, equal)
	class_method_local (SpellingChecker, canWriteAsEncoding)
	class_method_local (SpellingChecker, writeText)
	class_method_local (SpellingChecker, writeBinary)
	class_method_local (SpellingChecker, readText)
	class_method_local (SpellingChecker, readBinary)
	class_methods_end
}

SpellingChecker WordList_upto_SpellingChecker (WordList me) {
	SpellingChecker thee = new (SpellingChecker); cherror
	thy wordList = Data_copy (me); cherror
	thy separatingCharacters = Melder_wcsdup_e (L".,;:()\""); cherror
end:
	iferror forget (thee);
	return thee;
}

WordList SpellingChecker_extractWordList (SpellingChecker me) {
	return Data_copy (my wordList);
}

int SpellingChecker_replaceWordList (SpellingChecker me, WordList list) {
	WordList newList = Data_copy (list);
	if (! newList) return 0;
	forget (my wordList);
	my wordList = newList;
	return 1;
}

SortedSetOfString SpellingChecker_extractUserDictionary (SpellingChecker me) {
	if (! my userDictionary)
		return Melder_errorp ("This spelling checker does not contain a user dictionary.");
	return Data_copy (my userDictionary);
}

int SpellingChecker_replaceUserDictionary (SpellingChecker me, SortedSetOfString userDictionary) {
	SortedSetOfString newDict = Data_copy (userDictionary);
	if (! newDict) return 0;
	forget (my userDictionary);
	my userDictionary = newDict;
	return 1;
}

static int startsWithCapital (const wchar_t *word) {
	return isupper (word [0]) || (word [0] == '\\' && isupper (word [1]));
}

int SpellingChecker_isWordAllowed (SpellingChecker me, const wchar_t *word) {
	int wordLength = wcslen (word);
	if (my allowAllWordsContaining && my allowAllWordsContaining [0]) {
		wchar_t *p = & my allowAllWordsContaining [0];
		while (*p) {
			/*
			 * Find next token in list of allowed string parts.
			 */
			wchar_t token [100], *q = & token [0];
			/*
			 * Skip spaces in list.
			 */
			while (*p == ' ') p ++;
			/*
			 * Collect one token string from list.
			 */
			while (*p != '\0' && *p != ' ') {
				*q ++ = *p ++;
			}
			*q = '\0';   /* Trailing null byte. */
			/*
			 * Allow word if it contains this token.
			 */
			if (wcsstr (word, token)) return TRUE;
		}
	}
	if (my allowAllNames) {
		/*
		 * Allow word if it starts with a capital.
		 */
		if (startsWithCapital (word)) {
			return TRUE;
		}
		if (my namePrefixes && my namePrefixes [0]) {
			wchar_t *p = & my namePrefixes [0];
			while (*p) {
				wchar_t token [100], *q = & token [0];
				while (*p == ' ') p ++;
				while (*p != '\0' && *p != ' ') *q ++ = *p ++;
				*q = '\0';   /* Trailing null byte. */
				/*
				 * Allow word if starts with this prefix
				 * and this prefix is followed by a capital.
				 */
				if (wcsstr (word, token) == word && startsWithCapital (word + wcslen (token))) {
					return TRUE;
				}
			}
		}
	} else if (my allowAllAbbreviations && startsWithCapital (word)) {
		const wchar_t *p = & word [0];
		for (;;) {
			if (*p == '\0') return TRUE;
			if (islower (*p)) break;
			p ++;
		}
	}
	if (my allowAllWordsStartingWith && my allowAllWordsStartingWith [0]) {
		wchar_t *p = & my allowAllWordsStartingWith [0];
		while (*p) {
			wchar_t token [100], *q = & token [0];
			int tokenLength;
			while (*p == ' ') p ++;
			while (*p != '\0' && *p != ' ') *q ++ = *p ++;
			*q = '\0';   /* Trailing null byte. */
			tokenLength = wcslen (token);
			if (wordLength >= tokenLength && wcsnequ (token, word, tokenLength)) {
				return TRUE;
			}
		}
	}
	if (my allowAllWordsEndingIn && my allowAllWordsEndingIn [0]) {
		wchar_t *p = & my allowAllWordsEndingIn [0];
		while (*p) {
			wchar_t token [100], *q = & token [0];
			int tokenLength;
			while (*p == ' ') p ++;
			while (*p != '\0' && *p != ' ') *q ++ = *p ++;
			*q = '\0';   /* Trailing null byte. */
			tokenLength = wcslen (token);
			if (wordLength >= tokenLength && wcsnequ (token, word + wordLength - tokenLength, tokenLength)) {
				return TRUE;
			}
		}
	}
	if (WordList_hasWord (my wordList, word))
		return TRUE;
	if (my userDictionary != NULL) {
		if (wcslen (word) > 3333) return FALSE;   /* Superfluous, because WordList_hasWord already checked. But safe. */
		Longchar_genericizeW (word, (wchar_t *) Melder_buffer2);
		if (SortedSetOfString_lookUp (my userDictionary, (wchar_t *) Melder_buffer2) != 0)
			return TRUE;
	}
	return FALSE;
}

int SpellingChecker_addNewWord (SpellingChecker me, const wchar_t *word) {
	wchar_t *generic = NULL;
//start:
	if (! my userDictionary) {
		my userDictionary = SortedSetOfString_create ();
		if (! my userDictionary) return 0;
	}
	generic = Melder_calloc_e (wchar_t, 3 * wcslen (word) + 1); cherror
	Longchar_genericizeW (word, generic);
	SortedSetOfString_add (my userDictionary, generic); cherror
end:
	iferror return 0;
	return 1;
}

static int stringContains (const wchar_t *string, int character) {
	const wchar_t *p = & string [0];
	while (*p) {
		if (*p == character) return TRUE;
		p ++;
	}
	return FALSE;
}

wchar_t * SpellingChecker_nextNotAllowedWord (SpellingChecker me, const wchar_t *sentence, long *start) {
	const wchar_t *p = sentence + *start;
	for (;;) {
		if (*p == '\0') {
			return NULL;   /* All words allowed. */
		} else if (*p == '(' && my allowAllParenthesized) {
			p ++;
			for (;;) {
				if (*p == '\0') {
					return NULL;   /* Everything is parenthesized... */
				} else if (*p == ')') {
					p ++;
					break;
				} else {
					p ++;
				}
			}
		} else if (*p == ' ' || (my separatingCharacters && stringContains (my separatingCharacters, *p))) {
			p ++;
		} else {
			static wchar_t word [100];
			wchar_t *q = & word [0];
			*start = p - sentence;
			for (;;) {
				if (*p == '\0' || *p == ' ' || (my separatingCharacters && stringContains (my separatingCharacters, *p))) {
					*q ++ = '\0';
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
	return NULL;   /* All words allowed. */
}

/* End of file SpellingChecker.c */
