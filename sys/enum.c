/* enum.c
 *
 * Copyright (C) 1994-2007 Paul Boersma
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
 * pb 1995/07/12 latest version
 * pb 2002/03/07 GPL
 * pb 2007/07/22 message changed
 */

#include "melder.h"
#include "enum.h"

int enum_search (void *void_enumerated, const char *string) {
	enum_ANY *enumerated = void_enumerated;
	const char **strings = & enumerated -> zero;
	int i;
	for (i = 0; strings [i] != NULL; i ++)
		if (strequ (strings [i], string))
			return i;
	(void) Melder_error ("\"%s\" is not a value of enumerated type \"%s\".",
		string, enumerated -> _type);
	return -1;   /* "<eof>" */
}

/* End of file enum.c */

