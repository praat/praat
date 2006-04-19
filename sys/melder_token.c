/* melder_token.c
 *
 * Copyright (C) 2006 Paul Boersma
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
 * pb 2006/04/16
 */

#include "melder.h"

long Melder_countTokens (const char *string) {
	long numberOfTokens = 0;
	const char *p = & string [0];
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

static char *theMelderToken;

char *Melder_firstToken (const char *string) {
	Melder_free (theMelderToken);
	theMelderToken = Melder_strdup (string);
	return strtok (theMelderToken, " \t\n\r");
}

char *Melder_nextToken (void) {
	return strtok (NULL, " \t\n\r");
}

/* End of file melder_token.c */
