/* Matrix_and_Polygon.cpp
 *
 * Copyright (C) 1992-2005,2011,2012,2015-2018,2020 Paul Boersma
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

#include "Matrix_and_Polygon.h"

autoPolygon Matrix_to_Polygon (Matrix me) {
	try {
		Melder_require (my nx == 2 || my ny == 2,
			U"The Matrix should have exactly 2 rows or columns.");
		autoPolygon thee;
		if (my ny == 2) {
			/*
				The matrix has two rows.
				The first row will be interpreted as x values, the second as y values.
			*/
			thee = Polygon_create (my nx);
			thy x.all()  <<=  my z.row (1);
			thy y.all()  <<=  my z.row (2);
		} else {
			/*
				The matrix has two columns.
				The first column will be interpreted as x values, the second as y values.
			*/
			thee = Polygon_create (my ny);
			thy x.all()  <<=  my z.column (1);
			thy y.all()  <<=  my z.column (2);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Polygon.");
	}
}

autoMatrix Polygon_to_Matrix (Polygon me) {
	try {
		/*
			The matrix will have two rows:
			the first column will represent x, the second will represent y.
		*/
		autoMatrix thee = Matrix_create (1.0, my numberOfPoints, my numberOfPoints, 1.0, 1.0, 1.0, 2.0, 2, 1.0, 1.0);
		thy z.row (1)  <<=  my x.get();
		thy z.row (2)  <<=  my y.get();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

/* End of file Matrix_and_Polygon.cpp */
