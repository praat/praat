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

/*
	From here on alphabetical order.
*/

inline void MATadd_inplace (MAT x, double addend) {
	VECadd_inplace (asvector (x), addend);
}
inline void MATadd_inplace (MAT x, constMAT y) {
	VECadd_inplace (asvector (x), asvector (y));
}
inline void MATadd_preallocated (MAT target, constMAT x, double addend) {
	Melder_assert (x.nrow == target.nrow && x.ncol == target.ncol);
	VECadd_preallocated (asvector (target), asvector (x), addend);
}
inline autoMAT MATadd (constMAT x, double addend) {
	auto result = MATraw (x.nrow, x.ncol);
	MATadd_preallocated (result.get(), x, addend);
	return result;
}
inline void MATadd_preallocated (MAT target, constMAT x, constMAT y) {
	Melder_assert (x.nrow == target.nrow && x.ncol == target.ncol);
	Melder_assert (y.nrow == x.nrow && y.ncol == x.ncol);
	VECadd_preallocated (asvector (target), asvector (x), asvector (y));
}
inline autoMAT MATadd (constMAT x, constMAT y) {
	auto result = MATraw (x.nrow, x.ncol);
	MATadd_preallocated (result.get(), x, y);
	return result;
}

/*
	Make the average of each column zero.
		a[i][j] -= a[.][j]
*/
void MATcentreEachColumn_inplace (MAT x, double centres [] = nullptr);

/*
	Make the average of each row zero.
		a[i][j] -= a[i][.]
*/
void MATcentreEachRow_inplace (MAT x);

/*
	Make the average of every column and every row zero.
		a[i][j] += - a[i][.] - a[.][j] + a[.][.]
*/
void MATdoubleCentre_inplace (MAT x);

inline void MATmultiply_inplace (MAT x, double factor) {
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] *= factor;
}

autoMAT MATouter (constVEC x, constVEC y);

autoMAT MATpeaks (constVEC x, bool includeEdges, int interpolate, bool sortByHeight);

inline autoMAT MATrandomGauss (integer nrow, integer ncol, double mu, double sigma) {
	auto result = MATraw (nrow, ncol);
	for (integer irow = 1; irow <= nrow; irow ++)
		for (integer icol = 1; icol <= ncol; icol ++)
			result [irow] [icol] = NUMrandomGauss (mu, sigma);
	return result;
}

inline void MATsubtract_inplace (MAT x, double number) {
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] -= number;
	}
}
inline void MATsubtractReversed_inplace (MAT x, double number) {
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] = number - x [irow] [icol];
	}
}
inline void MATsubtract_inplace (MAT x, constMAT y) {
	Melder_assert (y.nrow == x.nrow && y.ncol == x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] -= y [irow] [icol];
	}
}
inline void MATsubtractReversed_inplace (MAT x, constMAT y) {
	Melder_assert (y.nrow == x.nrow && y.ncol == x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] = y [irow] [icol] - x [irow] [icol];
	}
}
inline autoMAT MATsubtract (constMAT x, double y) {
	auto result = MATraw (x.nrow, x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			result [irow] [icol] = x [irow] [icol] - y;
	}
	return result;
}
inline autoMAT MATsubtract (double x, constMAT y) {
	auto result = MATraw (y.nrow, y.ncol);
	for (integer irow = 1; irow <= y.nrow; irow ++) {
		for (integer icol = 1; icol <= y.ncol; icol ++)
			result [irow] [icol] = x - y [irow] [icol];
	}
	return result;
}
inline autoMAT MATsubtract (constMAT x, constMAT y) {
	Melder_assert (y.nrow == x.nrow && y.ncol == x.ncol);
	auto result = MATraw (x.nrow, x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++)
			result [irow] [icol] = x [irow] [icol] - y [irow] [icol];
	}
	return result;
}

inline void MATtranspose_preallocated (MAT target, constMAT x) {
	Melder_assert (x.nrow == target.nrow && x.ncol == target.ncol);
	for (integer irow = 1; irow <= target.nrow; irow ++)
		for (integer icol = 1; icol <= target.ncol; icol ++)
			target [irow] [icol] = x [icol] [irow];
}

inline autoMAT MATtranspose (constMAT x) {
	autoMAT result = MATraw (x.ncol, x.nrow);
	MATtranspose_preallocated (result.get(), x);
	return result;
}

/* End of file MAT.h */
