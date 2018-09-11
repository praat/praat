/* VEC.cpp
 *
 * Copyright (C) 2017,2018 Paul Boersma
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

#include "melder.h"
#include "../dwsys/NUM2.h"   /* for NUMsort2 */

inline static double inner_stride_ (constVEC x, constVEC y, integer stride) {
	if (x.size != y.size)
		return undefined;
	PAIRWISE_SUM (longdouble, sum, integer, x.size,
		const double *px = & x [1];
		const double *py = & y [1],
		(longdouble) *px * (longdouble) *py,
		(px += 1, py += stride)   // this goes way beyond the confines of y
	)
	return (double) sum;
}

void VECmul_preallocated (VEC target, constVEC vec, constMAT mat) {
	Melder_assert (mat.nrow == vec.size);
	Melder_assert (target.size == mat.ncol);
	for (integer j = 1; j <= mat.ncol; j ++) {
		if ((false)) {
			target [j] = 0.0;
			for (integer i = 1; i <= mat.nrow; i ++)
				target [j] += vec [i] * mat [i] [j];
		} else {
			target [j] = inner_stride_ (vec, constVEC (& mat [1] [j] - 1, mat.nrow), mat.ncol);
		}
	}
}

autoVEC VECmul (constVEC vec, constMAT mat) {
	autoVEC result = VECraw (mat.ncol);
	VECmul_preallocated (result.get(), vec, mat);
	return result;
}

void VECmul_preallocated (VEC target, constMAT mat, constVEC vec) {
	Melder_assert (vec.size == mat.ncol);
	Melder_assert (target.size == mat.nrow);
	for (integer i = 1; i <= mat.nrow; i ++) {
		if ((false)) {
			target [i] = 0.0;
			for (integer j = 1; j <= vec.size; j ++)
				target [i] += mat [i] [j] * vec [j];
		} else {
			target [i] = NUMinner (constVEC (& mat [i] [1] - 1, mat.ncol), vec);
		}
	}
}

autoVEC VECmul (constMAT mat, constVEC vec) {
	autoVEC result = VECraw (mat.nrow);
	VECmul_preallocated (result.get(), mat, vec);
	return result;
}

/* End of file VEC.cpp */
