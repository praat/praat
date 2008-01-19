/* Excitations.c
 *
 * Copyright (C) 1993-2007 David Weenink
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
 djmw 20020813 GPL header
 djmw 20071009 wchar_t
 djmw 20071017 Melder_error<n>
*/

#include "Excitations.h"

/*
static int readText (I, FILE *f)
{
	iam (Excitations);
	my itemClass =  classExcitation;
	return inherited (Ordered) readText (me, f);
}

static int readBinary (I, FILE *f)
{
	iam (Excitations);
	my itemClass =  classExcitation;
	return inherited (Ordered) readBinary (me, f);
}
*/
	
class_methods (Excitations, Ordered)
class_methods_end

Excitations Excitations_create (long initialCapacity)
{
	Excitations me = new (Excitations);
	if (! me || ! Ordered_init (me, classExcitation, initialCapacity)) forget (me);
	return me;
}

Pattern Excitations_to_Pattern (Excitations me, long join)
{
    long i, j, r = 0, c = 1; 
	Pattern thee = NULL; 
	Matrix m;

	Melder_assert (my size > 0);
	m = my item[1];
    if (join < 1) join = 1;
	if ((my size % join) != 0) return Melder_errorp1 (L"Excitations_to_Pattern:"
		"number of rows is not a multiple of join."); 
	if (! (thee = Pattern_create (my size / join, join * m->nx))) return thee;
	for (i = 1; i <= my size; i++)
	{
		double *z = ((Matrix) my item[i])->z[1];
		if ((i - 1) % join == 0) { r++; c = 1; }
		for (j = 1; j <= m->nx; j++) thy z[r][c++] = z[j];
	}
    return thee;
}

TableOfReal Excitations_to_TableOfReal (Excitations me)
{
    TableOfReal thee = NULL;
	Matrix m;

	Melder_assert (my size > 0);
	m = my item[1];
	thee = TableOfReal_create (my size, m -> nx);
	if (thee != NULL)
	{
		long i, j;
		for (i = 1;  i <= my size; i++)
		{
			double *z = ((Matrix) my item[i]) -> z[1];
			for (j = 1; j <= m -> nx; j++) thy data[i][j] = z[j];
		}
	}
	return thee;
}

Any Excitations_getItem (Excitations me, long item)
{
	Excitation thee = NULL;
	if (item < 1 || item > my size ||
		! (thee = Data_copy (my item[item]))) return thee;
	Thing_setName (thee, Thing_getName (my item[item]));
	return thee;
}
/* End of file Excitations.c */
