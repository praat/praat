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

static nummat raw_nummat (int64 nrow, int64 ncol) {
	try {
		nummat result;
		result.at = NUMmatrix <double> (1, nrow, 1, ncol, false);
		result.nrow = nrow;
		result.ncol = ncol;
		return result;
	} catch (MelderError) {
		Melder_throw (U"Numeric matrix not created.");
	}
}

nummat numvecs_outer_nummat (numvec x, numvec y) {
	nummat result = raw_nummat (x.size, y.size);
	for (long irow = 1; irow <= x.size; irow ++) {
		for (long icol = 1; icol <= y.size; icol ++) {
			result.at [irow] [icol] = x.at [irow] * y.at [icol];
		}
	}
	return result;
}

/* End of file tensor.cpp */
