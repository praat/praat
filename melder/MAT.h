#pragma once
/* MAT.h
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

/*
	Some functions that are included below.
*/

inline static autoMAT MATraw (integer nrow, integer ncol) {
	return { nrow, ncol, kTensorInitializationType::RAW };
}

/*
	From here on alphabetical order.
*/

inline static void MATadd_inplace (MAT x, double number) {
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] += number;
	}
}
inline static void MATadd_inplace (MAT x, constMAT y) {
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] += y [irow] [icol];
	}
}
inline static autoMAT MATadd (constMAT x, double addend) {   // TODO: check whether casting to VEC is faster
	autoMAT result = MATraw (x.nrow, x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			result [irow] [icol] = x [irow] [icol] + addend;
	}
	return result;
}
inline static autoMAT MATadd (constMAT x, constMAT y) {   // TODO: check whether casting to VEC is faster
	if (x.nrow != y.nrow || x.ncol != y.ncol) return autoMAT { };
	autoMAT result = MATraw (x.nrow, x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			result [irow] [icol] = x [irow] [icol] + y [irow] [icol];
	}
	return result;
}

inline static void MATcopy_inplace (MAT x, constMAT source) {
	for (integer irow = 1; irow <= source.nrow; irow ++)
		for (integer icol = 1; icol <= source.ncol; icol ++)
			x [irow] [icol] = source [irow] [icol];
}
autoMAT MATcopy (constMAT x);

inline static void MATmultiply_inplace (MAT x, double factor) {
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] *= factor;
	}
}

autoMAT MATouter (constVEC x, constVEC y);

autoMAT MATpeaks (constVEC x, bool includeEdges, int interpolate, bool sortByHeight);

inline static void MATsubtract_inplace (MAT x, double number) {
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] -= number;
	}
}
inline static void MATsubtractReversed_inplace (MAT x, double number) {
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] = number - x [irow] [icol];
	}
}
inline static void MATsubtract_inplace (MAT x, constMAT y) {
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] -= y [irow] [icol];
	}
}
inline static void MATsubtractReversed_inplace (MAT x, constMAT y) {
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] = y [irow] [icol] - x [irow] [icol];
	}
}
inline static autoMAT MATsubtract (constMAT x, double y) {
	autoMAT result = MATraw (x.nrow, x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			result [irow] [icol] = x [irow] [icol] - y;
	}
	return result;
}
inline static autoMAT MATsubtract (double x, constMAT y) {
	autoMAT result = MATraw (y.nrow, y.ncol);
	for (integer irow = 1; irow <= y.nrow; irow ++) {
		for (integer icol = 1; icol <= y.ncol; icol ++)
			result [irow] [icol] = x - y [irow] [icol];
	}
	return result;
}
inline static autoMAT MATsubtract (constMAT x, constMAT y) {
	if (x.nrow != y.nrow || x.ncol != y.ncol) return autoMAT { };
	autoMAT result = MATraw (x.nrow, x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			result [irow] [icol] = x [irow] [icol] - y [irow] [icol];
	}
	return result;
}

inline static autoMAT MATzero (integer nrow, integer ncol) {
	return { nrow, ncol, kTensorInitializationType::ZERO };
}

/* End of file MAT.h */
