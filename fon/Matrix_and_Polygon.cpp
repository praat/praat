/* Matrix_and_Polygon.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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
 * pb 1995/11/07
 * pb 2002/07/16 GPL
 * pb 2008/01/19 double
 * pb 2011/06/04 C++
 */

#include "Matrix_and_Polygon.h"

Polygon Matrix_to_Polygon (Matrix me) {
	try {
		if (my nx != 2 && my ny != 2)
			Melder_throw ("Matrix must have exactly 2 rows or columns.");
		autoPolygon thee = NULL;
		if (my ny == 2) {
			thee.reset (Polygon_create (my nx));
			NUMvector_copyElements (my z [1], thy x, 1, my nx);
			NUMvector_copyElements (my z [2], thy y, 1, my nx);
		} else {
			thee.reset (Polygon_create (my ny));
			for (long i = 1; i <= my ny; i ++) {
				thy x [i] = my z [i] [1];
				thy y [i] = my z [i] [2];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Polygon.");
	}
}

Matrix Polygon_to_Matrix (Polygon me) {
	try {
		autoMatrix thee = Matrix_create (1, my numberOfPoints, my numberOfPoints, 1, 1, 1, 2, 2, 1, 1);
		NUMvector_copyElements (my x, thy z [1], 1, my numberOfPoints);
		NUMvector_copyElements (my y, thy z [2], 1, my numberOfPoints);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

/* End of file Matrix_and_Polygon.cpp */
