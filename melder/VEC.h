#pragma once
/* VEC.h
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

autoVEC VECcopy (VEC x);

autoVEC VECmul (VEC x, MAT y);
void VECmul_inplace (VEC target, VEC vec, MAT mat);
autoVEC VECmul (MAT x, VEC y);
void VECmul_inplace (VEC target, MAT mat, VEC vec);


inline static void numvec_copyElements_nocheck (VEC from, VEC to) {
	for (integer i = 1; i <= from.size; i ++) {
		to [i] = from [i];
	}
}

inline static void nummat_copyElements_nocheck (MAT from, MAT to) {
	for (integer irow = 1; irow <= from.nrow; irow ++) {
		for (integer icol = 1; icol <= from.ncol; icol ++) {
			to [irow] [icol] = from [irow] [icol];
		}
	}
}

inline static autoVEC add_numvec (VEC x, VEC y) {
	if (x.size != y.size) return autoVEC { };
	autoVEC result (x.size, kTensorInitializationType::RAW);
	for (integer i = 1; i <= x.size; i ++) {
		result [i] = x [i] + y [i];
	}
	return result;
}
inline static autoMAT add_nummat (MAT x, MAT y) {
	if (x.nrow != y.nrow || x.ncol != y.ncol) return autoMAT { };
	autoMAT result (x.nrow, x.ncol, kTensorInitializationType::RAW);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol] + y [irow] [icol];
		}
	}
	return result;
}
inline static autoVEC sub_numvec (VEC x, VEC y) {
	if (x.size != y.size) return autoVEC { };
	autoVEC result (x.size, kTensorInitializationType::RAW);
	for (integer i = 1; i <= x.size; i ++) {
		result [i] = x [i] - y [i];
	}
	return result;
}
inline static autoMAT sub_nummat (MAT x, MAT y) {
	if (x.nrow != y.nrow || x.ncol != y.ncol) return autoMAT { };
	autoMAT result (x.nrow, x.ncol, kTensorInitializationType::RAW);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol] - y [irow] [icol];
		}
	}
	return result;
}

inline static autoVEC to_numvec (integer to) {
	autoVEC result (to, kTensorInitializationType::RAW);
	for (integer i = 1; i <= to; i ++) {
		result [i] = (double) i;
	}
	return result;
}

void VECsort_inplace (VEC x);

/* End of file VEC.h */
