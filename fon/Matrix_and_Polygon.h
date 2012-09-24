/* Matrix_and_Polygon.h
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

#include "Matrix.h"
#include "Polygon.h"

Polygon Matrix_to_Polygon (Matrix me);
/*
	Function:
		create a Polygon from a Matrix; return NULL in case of failure.
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

Matrix Polygon_to_Matrix (Polygon me);
/*
	Function:
		create a Matrix from a Polygon; return NULL if out of memory.
	Postconditions:
		thy xmin xmax nx dx x1 == 1 my numberOfPoints my numberOfPoints 1 1;
		thy ymin ymax ny dy y1 == 1 2 2 1 1;
		thy z [1] [...] == my x [...];
		thy z [2] [...] == my y [...];
*/

/* End of file Matrix_and_Polygon.h */
