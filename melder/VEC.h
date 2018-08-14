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

autonumvec VECcopy (numvec x);

autonumvec VECmul (numvec x, nummat y);
void VECmul_inplace (numvec target, numvec vec, nummat mat);
autonumvec VECmul (nummat x, numvec y);
void VECmul_inplace (numvec target, nummat mat, numvec vec);


inline static void numvec_copyElements_nocheck (numvec from, numvec to) {
	for (integer i = 1; i <= from.size; i ++) {
		to [i] = from [i];
	}
}

inline static void nummat_copyElements_nocheck (nummat from, nummat to) {
	for (integer irow = 1; irow <= from.nrow; irow ++) {
		for (integer icol = 1; icol <= from.ncol; icol ++) {
			to [irow] [icol] = from [irow] [icol];
		}
	}
}

inline static autonumvec add_numvec (numvec x, numvec y) {
	if (x.size != y.size) return autonumvec { };
	autonumvec result (x.size, kTensorInitializationType::RAW);
	for (integer i = 1; i <= x.size; i ++) {
		result [i] = x [i] + y [i];
	}
	return result;
}
inline static autonummat add_nummat (nummat x, nummat y) {
	if (x.nrow != y.nrow || x.ncol != y.ncol) return autonummat { };
	autonummat result (x.nrow, x.ncol, kTensorInitializationType::RAW);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol] + y [irow] [icol];
		}
	}
	return result;
}
inline static autonumvec sub_numvec (numvec x, numvec y) {
	if (x.size != y.size) return autonumvec { };
	autonumvec result (x.size, kTensorInitializationType::RAW);
	for (integer i = 1; i <= x.size; i ++) {
		result [i] = x [i] - y [i];
	}
	return result;
}
inline static autonummat sub_nummat (nummat x, nummat y) {
	if (x.nrow != y.nrow || x.ncol != y.ncol) return autonummat { };
	autonummat result (x.nrow, x.ncol, kTensorInitializationType::RAW);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol] - y [irow] [icol];
		}
	}
	return result;
}

inline static autonumvec to_numvec (integer to) {
	autonumvec result (to, kTensorInitializationType::RAW);
	for (integer i = 1; i <= to; i ++) {
		result [i] = (double) i;
	}
	return result;
}

void VECsort_inplace (numvec x);

/* End of file VEC.h */
