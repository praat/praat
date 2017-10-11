/* Matrix_and_Polygon.cpp
 *
 * Copyright (C) 1992-2012,2015,2016,2017 Paul Boersma
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

/*
 * pb 1995/11/07
 * pb 2002/07/16 GPL
 * pb 2008/01/19 double
 * pb 2011/06/04 C++
 */

#include "Matrix_and_Polygon.h"

autoPolygon Matrix_to_Polygon (Matrix me) {
	try {
		if (my nx != 2 && my ny != 2)
			Melder_throw (U"The Matrix should have exactly 2 rows or columns.");
		autoPolygon thee;
		if (my ny == 2) {
			thee = Polygon_create (my nx);
			NUMvector_copyElements (my z [1], thy x, 1, my nx);
			NUMvector_copyElements (my z [2], thy y, 1, my nx);
		} else {
			thee = Polygon_create (my ny);
			for (integer i = 1; i <= my ny; i ++) {
				thy x [i] = my z [i] [1];
				thy y [i] = my z [i] [2];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Polygon.");
	}
}

autoMatrix Polygon_to_Matrix (Polygon me) {
	try {
		autoMatrix thee = Matrix_create (1.0, my numberOfPoints, my numberOfPoints, 1.0, 1.0, 1.0, 2.0, 2, 1.0, 1.0);
		NUMvector_copyElements (my x, thy z [1], 1, my numberOfPoints);
		NUMvector_copyElements (my y, thy z [2], 1, my numberOfPoints);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

/* End of file Matrix_and_Polygon.cpp */
