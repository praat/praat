/* Matrix_and_PointProcess.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 1997/03/29
 * pb 2002/07/16 GPL
 * pb 2011/06/04 C++
 */

#include "Matrix_and_PointProcess.h"

Matrix PointProcess_to_Matrix (PointProcess me) {
	try {
		if (my nt == 0)
			Melder_throw ("No times in PointProcess.");
		autoMatrix thee = Matrix_create (1, my nt, my nt, 1, 1, 1, 1, 1, 1, 1);
		for (long i = 1; i <= my nt; i ++)
			thy z [1] [i] = my t [i];
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

PointProcess Matrix_to_PointProcess (Matrix me) {
	try {
		autoPointProcess thee = PointProcess_create (my z [1] [1], my z [1] [my nx], my nx);
		for (long i = 1; i <= my nx; i ++) {
			PointProcess_addPoint (thee.peek(), my z [1] [i]);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to PointProcess.");
	}
}

/* End of file Matrix_and_PointProcess.cpp */
