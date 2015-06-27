/* melder_token.cpp
 *
 * Copyright (C) 2006-2011,2015 Paul Boersma
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
 * pb 2006/04/16 created
 * pb 2007/08/10 wchar
 * pb 2007/11/18 moved Melder_get/free/searchToken(s) here
 * pb 2011/04/05 C++
 */

#include "melder.h"
#include "NUM.h"

long Melder_countTokens (const char32 *string) {
	long numberOfTokens = 0;
	const char32 *p = & string [0];
	for (;;) {
		while (*p == U' ' || *p == U'\t' || *p == U'\n' || *p == U'\r') p ++;
		if (*p == U'\0') return numberOfTokens;
		numberOfTokens ++;
		while (*p != U' ' && *p != U'\t' && *p != U'\n' && *p != U'\r') {
			if (*p == U'\0') return numberOfTokens;
			p ++;
		}
	}
	return 0;   // should not occur
}

static char32 *theMelderToken;

char32 *Melder_firstToken (const char32 *string) {
	Melder_free (theMelderToken);
	theMelderToken = Melder_dup_f (string);
	return Melder_tok (theMelderToken, U" \t\n\r");
}

char32 *Melder_nextToken (void) {
	return Melder_tok (NULL, U" \t\n\r");
}

char32 ** Melder_getTokens (const char32 *string, long *n) {
	char32 *token;
	long itoken = 0;
	*n = Melder_countTokens (string);
	if (*n == 0) return NULL;
	autostring32vector result (1, *n);
	for (token = Melder_firstToken (string); token != NULL; token = Melder_nextToken ()) {
		result [++ itoken] = Melder_dup (token);
	}
	return result.transfer();
}

void Melder_freeTokens (char32 ***tokens) {
	NUMvector_free (*tokens, 1);
	*tokens = NULL;
}

long Melder_searchToken (const char32 *string, char32 **tokens, long n) {
	for (long i = 1; i <= n; i ++) {
		if (str32equ (string, tokens [i])) return i;
	}
	return 0;
}

/* End of file melder_token.cpp */
