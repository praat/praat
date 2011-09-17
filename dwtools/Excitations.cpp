/* Excitations.cpp
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20090914 getItem modified
 djmw 20110304 Thing_new
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

Thing_implement (Excitations, Ordered, 0);

Excitations Excitations_create (long initialCapacity) {
	try {
		autoExcitations me = Thing_new (Excitations);
		Ordered_init (me.peek(), classExcitation, initialCapacity);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Excitations not created.");
	}
}

Pattern Excitations_to_Pattern (Excitations me, long join) {
	try {
		Melder_assert (my size > 0);
		Matrix m = (Matrix) my item[1];
		if (join < 1) {
			join = 1;
		}
		if ( (my size % join) != 0) {
			Melder_throw ("Number of rows is not a multiple of join.");
		}
		autoPattern thee = Pattern_create (my size / join, join * m -> nx);
		long r = 0, c = 1;
		for (long i = 1; i <= my size; i++) {
			double *z = ( (Matrix) my item[i])->z[1];
			if ( (i - 1) % join == 0) {
				r++;
				c = 1;
			}
			for (long j = 1; j <= m -> nx; j++) {
				thy z[r][c++] = z[j];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Pattern created.");
	}
}

TableOfReal Excitations_to_TableOfReal (Excitations me) {
	try {
		Melder_assert (my size > 0);
		Matrix m = (Matrix) my item[1];
		autoTableOfReal thee = TableOfReal_create (my size, m -> nx);
		for (long i = 1;  i <= my size; i++) {
			double *z = ( (Matrix) my item[i]) -> z[1];
			for (long j = 1; j <= m -> nx; j++) {
				thy data[i][j] = z[j];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no TableOfReal created.");
	}
}

Excitation Excitations_getItem (Excitations me, long item) {
	try {
		if (item < 1 || item > my size) {
			Melder_throw ("Not a valid element number.");
		}
		autoExcitation thee = Data_copy ( (Excitation) my item[item]);
		Thing_setName (thee.peek(), Thing_getName ( (Thing) my item[item]));
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Excitation created.");
	}
}

/* End of file Excitations.cpp */
