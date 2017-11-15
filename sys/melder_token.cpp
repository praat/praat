/* melder_token.cpp
 *
 * Copyright (C) 2006,2007,2009,2011,2012,2015-2017 Paul Boersma
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

#include "melder.h"

integer Melder_countTokens (const char32 *string) {
	integer numberOfTokens = 0;
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

char32 *Melder_nextToken () {
	return Melder_tok (nullptr, U" \t\n\r");
}

char32 ** Melder_getTokens (const char32 *string, integer *n) {
	char32 *token;
	integer itoken = 0;
	*n = Melder_countTokens (string);
	if (*n == 0) return nullptr;
	autostring32vector result (1, *n);
	for (token = Melder_firstToken (string); token != nullptr; token = Melder_nextToken ()) {
		result [++ itoken] = Melder_dup (token);
	}
	return result.transfer();
}

void Melder_freeTokens (char32 ***tokens) {
	NUMvector_free (*tokens, 1);
	*tokens = nullptr;
}

integer Melder_searchToken (const char32 *string, char32 **tokens, integer n) {
	for (integer i = 1; i <= n; i ++) {
		if (str32equ (string, tokens [i])) return i;
	}
	return 0;
}

/* End of file melder_token.cpp */
