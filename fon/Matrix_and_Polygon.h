/* Matrix_and_Polygon.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

#include "Matrix.h"
#include "Polygon.h"

autoPolygon Matrix_to_Polygon (Matrix me);
/*
	Function:
		create a Polygon from a Matrix.
	Failures:
		my ny != 2 && my nx != 2;   //  Matrix must have exactly 2 rows or columns.
		Out of memory.
	Postconditions:
		thy numberOfPoints == my nx;
		if (my ny == 2) {
			thy x [...] == my z [1] [...];
			thy y [...] == my z [2] [...];
		} else {
			thy x [...] == my z [...] [1];
			thy y [...] == my z [...] [2];
		}
*/

autoMatrix Polygon_to_Matrix (Polygon me);
/*
	Function:
		create a Matrix from a Polygon.
	Postconditions:
		thy xmin xmax nx dx x1 == 1 my numberOfPoints my numberOfPoints 1 1;
		thy ymin ymax ny dy y1 == 1 2 2 1 1;
		thy z [1] [...] == my x [...];
		thy z [2] [...] == my y [...];
*/

/* End of file Matrix_and_Polygon.h */
