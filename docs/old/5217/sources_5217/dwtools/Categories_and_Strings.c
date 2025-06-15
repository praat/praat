/* Categories_and_Strings.c
 *
 * Copyright (C) 1993-2002 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020315 GPL header
 */

#include "Categories_and_Strings.h"

Strings Categories_to_Strings (Categories me)
{
	Strings thee;
	long i;

	if (my size < 1) return NULL;
	thee = new (Strings);
	if (thee == NULL) return NULL;
	thy strings = NUMpvector (1, my size);
	if (thy strings == NULL) goto end;
	thy numberOfStrings = my size;

	for (i = 1; i <= my size; i++)
	{
		SimpleString s = my item[i];
		thy strings[i] = Melder_wcsdup_e (s -> string);
		if (thy strings[i] == NULL) goto end;
	}
end:
	if (Melder_hasError()) forget (thee);
	return thee;
}

Categories Strings_to_Categories (Strings me)
{
	Categories thee;
	long i;

	if (my numberOfStrings < 1) return NULL;
	thee = new (Categories);
	if (thee == NULL) return NULL;
	if (! Categories_init (thee, my numberOfStrings)) goto end;

	for (i = 1; i <= my numberOfStrings; i++)
	{
		SimpleString s = SimpleString_create (my strings[i]);
		if (s == NULL ||
			! Collection_addItem (thee, s)) goto end;
	}
end:
	if (Melder_hasError()) forget (thee);
	return thee;
}

/* End of file Categories_and_Strings.c */
