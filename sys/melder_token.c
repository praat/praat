/* melder_token.c
 *
 * Copyright (C) 2006-2007 Paul Boersma
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
 * pb 2007/08/10 wchar_t
 */

#include "melder.h"

long Melder_countTokens (const wchar_t *string) {
	long numberOfTokens = 0;
	const wchar_t *p = & string [0];
	for (;;) {
		while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p ++;
		if (*p == '\0') return numberOfTokens;
		numberOfTokens ++;
		while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') {
			if (*p == '\0') return numberOfTokens;
			p ++;
		}
	}
	return 0;   /* Should not occur. */
}

static wchar_t *theMelderToken, *theMelderTokenLast;

wchar_t *Melder_firstToken (const wchar_t *string) {
	Melder_free (theMelderToken);
	theMelderToken = Melder_wcsdup (string);
	return wcstok (theMelderToken, L" \t\n\r", & theMelderTokenLast);
}

wchar_t *Melder_nextToken (void) {
	return wcstok (NULL, L" \t\n\r", & theMelderTokenLast);
}

/* End of file melder_token.c */
