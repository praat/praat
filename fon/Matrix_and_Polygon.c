/* Matrix_and_Polygon.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 */

#include "Matrix_and_Polygon.h"

Polygon Matrix_to_Polygon (I) {
	iam (Matrix);
	Polygon thee;
	if (my nx != 2 && my ny != 2)
		return Melder_errorp ("Matrix_to_Polygon: matrix must have exactly 2 rows or columns.");
	if (my ny == 2) {
		if (! (thee = Polygon_create (my nx))) return NULL;
		NUMfvector_copyElements (my z [1], thy x, 1, my nx);
		NUMfvector_copyElements (my z [2], thy y, 1, my nx);
	} else {
		long i;
		if (! (thee = Polygon_create (my ny))) return NULL;
		for (i = 1; i <= my ny; i ++) {
			thy x [i] = my z [i] [1];
			thy y [i] = my z [i] [2];
		}
	}
	return thee;
}

Matrix Polygon_to_Matrix (I) {
	iam (Polygon);
	Matrix thee = Matrix_create (1, my numberOfPoints, my numberOfPoints, 1, 1, 1, 2, 2, 1, 1);
	if (! thee) return NULL;
	NUMfvector_copyElements (my x, thy z [1], 1, my numberOfPoints);
	NUMfvector_copyElements (my y, thy z [2], 1, my numberOfPoints);
	return thee;
}

/* End of file Matrix_and_Polygon.c */
