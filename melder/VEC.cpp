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

autonumvec VECcopy (numvec x) {
	autonumvec result (x.size, kTensorInitializationType::RAW);
	for (integer i = 1; i <= x.size; i ++) {
		result [i] = x [i];
	}
	return result;
}

inline static double NUMinner_stride_ (numvec x, numvec y, integer stride) {
	if (x.size != y.size) return undefined;
	PAIRWISE_SUM (longdouble, sum, integer, x.size,
		double *xx = & x [0];
		double *yy = & y [1 - stride],
		(++ xx, yy += stride),   // this goes way beyond the confines of y
		(longdouble) *xx * (longdouble) *yy)
	return (double) sum;
}

inline static void mul_inplace (numvec target, numvec vec, nummat mat) {
	for (integer j = 1; j <= mat.ncol; j ++) {
		if ((false)) {
			target [j] = 0.0;
			for (integer i = 1; i <= mat.nrow; i ++) {
				target [j] += vec [i] * mat [i] [j];
			}
		} else {
			target [j] = NUMinner_stride_ (vec, { & mat [1] [j] - 1, mat.nrow }, mat.ncol);
		}
	}
}

inline static void mul_inplace (numvec target, nummat mat, numvec vec) {
	for (integer i = 1; i <= mat.nrow; i ++) {
		if ((false)) {
			target [i] = 0.0;
			for (integer j = 1; j <= vec.size; j ++) {
				target [i] += mat [i] [j] * vec [j];
			}
		} else {
			target [i] = NUMinner (numvec (& mat [i] [1] - 1, mat.ncol), vec);
		}
	}
}

autonumvec VECmul (numvec vec, nummat mat) {
	if (mat.nrow != vec.size) return autonumvec { };
	autonumvec result { mat.ncol, kTensorInitializationType::RAW };
	mul_inplace (result.get(), vec, mat);
	return result;
}

autonumvec VECmul (nummat mat, numvec vec) {
	if (vec.size != mat.ncol) return autonumvec { };
	autonumvec result { mat.nrow, kTensorInitializationType::RAW };
	mul_inplace (result.get(), mat, vec);
	return result;
}

void numvec_sort (numvec x) {
	NUMsort_d (x.size, x.at);
}

/* End of file VEC.cpp */
