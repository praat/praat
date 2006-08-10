/* SpellingChecker.c
 *
 * Copyright (C) 1999-2002 Paul Boersma
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
 * pb 2002/03/12
 * pb 2002/07/16 GPL
 */

#include "SpellingChecker.h"
#include <ctype.h>

#include "oo_DESTROY.h"
#include "SpellingChecker_def.h"
#include "oo_COPY.h"
#include "SpellingChecker_def.h"
#include "oo_EQUAL.h"
#include "SpellingChecker_def.h"
#include "oo_WRITE_ASCII.h"
#include "SpellingChecker_def.h"
#include "oo_WRITE_BINARY.h"
#include "SpellingChecker_def.h"
#include "oo_READ_ASCII.h"
#include "SpellingChecker_def.h"
#include "oo_READ_BINARY.h"
#include "SpellingChecker_def.h"
#include "oo_DESCRIPTION.h"
#include "SpellingChecker_def.h"

#include "longchar.h"

class_methods (SpellingChecker, Data)
	class_method_local (SpellingChecker, description)
	class_method_local (SpellingChecker, destroy)
	class_method_local (SpellingChecker, copy)
	class_method_local (SpellingChecker, equal)
	class_method_local (SpellingChecker, writeAscii)
	class_method_local (SpellingChecker, writeBinary)
	class_method_local (SpellingChecker, readAscii)
	class_method_local (SpellingChecker, readBinary)
class_methods_end

SpellingChecker WordList_upto_SpellingChecker (WordList me) {
	SpellingChecker thee = new (SpellingChecker); cherror
	thy wordList = Data_copy (me); cherror
	thy separatingCharacters = Melder_strdup (".,;:()\"");
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

static int startsWithCapital (const char *word) {
	return isupper (word [0]) || (word [0] == '\\' && isupper (word [1]));
}

int SpellingChecker_isWordAllowed (SpellingChecker me, const char *word) {
	int wordLength = strlen (word);
	if (my allowAllWordsContaining && my allowAllWordsContaining [0]) {
		char *p = & my allowAllWordsContaining [0];
		while (*p) {
			/*
			 * Find next token in list of allowed string parts.
			 */
			char token [100], *q = & token [0];
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
			if (strstr (word, token)) return TRUE;
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
			char *p = & my namePrefixes [0];
			while (*p) {
				char token [100], *q = & token [0];
				while (*p == ' ') p ++;
				while (*p != '\0' && *p != ' ') *q ++ = *p ++;
				*q = '\0';   /* Trailing null byte. */
				/*
				 * Allow word if starts with this prefix
				 * and this prefix is followed by a capital.
				 */
				if (strstr (word, token) == word && startsWithCapital (word + strlen (token))) {
					return TRUE;
				}
			}
		}
	} else if (my allowAllAbbreviations && startsWithCapital (word)) {
		const char *p = & word [0];
		for (;;) {
			if (*p == '\0') return TRUE;
			if (islower (*p)) break;
			p ++;
		}
	}
	if (my allowAllWordsStartingWith && my allowAllWordsStartingWith [0]) {
		char *p = & my allowAllWordsStartingWith [0];
		while (*p) {
			char token [100], *q = & token [0];
			int tokenLength;
			while (*p == ' ') p ++;
			while (*p != '\0' && *p != ' ') *q ++ = *p ++;
			*q = '\0';   /* Trailing null byte. */
			tokenLength = strlen (token);
			if (wordLength >= tokenLength && strnequ (token, word, tokenLength)) {
				return TRUE;
			}
		}
	}
	if (my allowAllWordsEndingIn && my allowAllWordsEndingIn [0]) {
		char *p = & my allowAllWordsEndingIn [0];
		while (*p) {
			char token [100], *q = & token [0];
			int tokenLength;
			while (*p == ' ') p ++;
			while (*p != '\0' && *p != ' ') *q ++ = *p ++;
			*q = '\0';   /* Trailing null byte. */
			tokenLength = strlen (token);
			if (wordLength >= tokenLength && strnequ (token, word + wordLength - tokenLength, tokenLength)) {
				return TRUE;
			}
		}
	}
	if (WordList_hasWord (my wordList, word))
		return TRUE;
	if (my userDictionary != NULL) {
		if (strlen (word) > 3333) return FALSE;   /* Superfluous, because WordList_hasWord already checked. But safe. */
		Longchar_genericize (word, Melder_buffer2);
		if (SortedSetOfString_lookUp (my userDictionary, Melder_buffer2) != 0)
			return TRUE;
	}
	return FALSE;
}

int SpellingChecker_addNewWord (SpellingChecker me, const char *word) {
	char *generic = NULL;
	if (! my userDictionary) {
		my userDictionary = SortedSetOfString_create ();
		if (! my userDictionary) return 0;
	}
	generic = Melder_calloc (1, 3 * strlen (word) + 1);
	if (! generic) goto end;
	Longchar_genericize (word, generic);
	if (! SortedSetOfString_add (my userDictionary, generic)) goto end;
end:
	Melder_free (generic);
	iferror return 0;
	return 1;
}

static int stringContains (const char *string, int character) {
	const char *p = & string [0];
	while (*p) {
		if (*p == character) return TRUE;
		p ++;
	}
	return FALSE;
}

char * SpellingChecker_nextNotAllowedWord (SpellingChecker me, const char *sentence, int *start) {
	const char *p = sentence + *start;
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
			static char word [100];
			char *q = & word [0];
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
