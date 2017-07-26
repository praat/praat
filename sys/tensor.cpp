/* tensor.cpp
 *
 * Copyright (C) 2017 Paul Boersma
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

#include "tensor.h"

static double **nummat_create (int64 nrow, int64 ncol) {
	try {
		/*
		 * Allocate room for the row pointers.
		 */
		double **result;
		for (;;) {
			result = reinterpret_cast <double **> (_Melder_malloc_f (nrow * (int64) sizeof (double *)));   // assume that all pointers have the same size
			result -= 1;
			if (result) break;   // this will normally succeed at the first try
			(void) Melder_realloc_f (result + 1, 1);   // make "sure" that the second try will succeed
		}
		/*
		 * Allocate room for the cells.
		 * The first row pointer points to below the first cell.
		 */
		for (;;) {
			try {
				result [1] = reinterpret_cast <double *> (_Melder_malloc (nrow * ncol * (int64) sizeof (double)));
			} catch (MelderError) {
				result += 1;
				Melder_free (result);   // free the row pointers
				throw MelderError ();
			}
			result [1] -= 1;
			if (result [1]) break;   // this will normally succeed at the first try
			(void) Melder_realloc_f (result [1] + 1, 1);   // make "sure" that the second try will succeed
		}
		for (long irow = 2; irow <= nrow; irow ++) result [irow] = result [irow - 1] + ncol;
		return result;
	} catch (MelderError) {
		Melder_throw (U"Numeric matrix not created.");
	}
}

nummat numvecs_outer_nummat (numvec x, numvec y) {
	double **result = nummat_create (x.size, y.size);
	for (long irow = 1; irow <= x.size; irow ++) {
		for (long icol = 1; icol <= y.size; icol ++) {
			result [irow] [icol] = x.at [irow] * y.at [icol];
		}
	}
	return { result, x.size, y.size };
}

/* End of file tensor.cpp */
