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
	asvector (x) += addend;
}
inline MAT operator+= (MAT x, double addend) {
	asvector (x) += addend;
	return x;
}
inline void MATadd_inplace (MAT x, constMAT y) {
	////VECadd_inplace (asvector (x), asvector (y));
	asvector (x) += asvector (y);
}
inline MAT operator+= (MAT x, constMAT y) {
	////VECadd_inplace (asvector (x), asvector (y));
	asvector (x) += asvector (y);
	return x;
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
extern void MATcentreEachColumn_inplace (MAT x, double centres [] = nullptr);

/*
	Make the average of each row zero.
		a[i][j] -= a[i][.]
*/
extern void MATcentreEachRow_inplace (MAT x);

/*
	Make the average of every column and every row zero.
		a[i][j] += - a[i][.] - a[.][j] + a[.][.]
*/
extern void MATdoubleCentre_inplace (MAT x);

/*
	Precise matrix multiplication, using pairwise summation.
	The speed is 6,0.66,0.48,1.69 ns per multiply-add
	for x.nrow = x.ncol = y.nrow = y.ncol = 1,10,100,1000.
	For large matrices this is a bit slow.
*/
extern void MATmul_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y);
inline void MATmul_preallocated  (const MAT& target, const constMAT& x, const constMAT& y) {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.ncol == y.nrow);
	MATmul_preallocated_ (target, x, y);
}
inline autoMAT MATmul (const constMAT& x, const constMAT& y) {
	autoMAT result = MATraw (x.nrow, y.ncol);
	MATmul_preallocated (result, x, y);
	return result;
}
/*
	Rough matrix multiplication, using an in-cache inner loop.
	The speed is 10,0.76,0.32,0.34 ns per multiply-add
	for x.nrow = x.ncol = y.nrow = y.ncol = 1,10,100,1000.
*/
extern void MATmul_fast_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y);
inline void MATmul_fast_preallocated  (const MAT& target, const constMAT& x, const constMAT& y) {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.ncol == y.nrow);
	MATmul_fast_preallocated_ (target, x, y);
}
inline autoMAT MATmul_fast (const constMAT& x, const constMAT& y) {
	autoMAT result = MATraw (x.nrow, y.ncol);
	MATmul_fast_preallocated (result, x, y);
	return result;
}

/*
	Target := X . Y'
	Pairwise summation.
	The speed is 9,0.85,0.51,0.52 ns per multiply-add
	for x.nrow = x.ncol = y.nrow = y.col = 1,10,100,1000.
*/
extern void MATmul_nt_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y);
inline void MATmul_nt_preallocated  (const MAT& target, const constMAT& x, const constMAT& y) {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.nrow);
	Melder_assert (x.ncol == y.ncol);
	MATmul_nt_preallocated_ (target, x, y);
}
inline autoMAT MATmul_nt (constMAT x, constMAT y) {
	autoMAT result = MATraw (x.nrow, y.nrow);
	MATmul_nt_preallocated (result, x, y);
	return result;
}

/*
	Target := X' . Y
	Pairwise summation.
	The speed is 12,0.88,0.60,15.1 ns per multiply-add
	for x.nrow = x.ncol = y.nrow = y.col = 1,10,100,1000.
	For large matrices this is very slow.
*/
extern void MATmul_tn_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y);
inline void MATmul_tn_preallocated  (const MAT& target, const constMAT& x, const constMAT& y) {
	Melder_assert (target.nrow == x.ncol);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.nrow == y.nrow);
	MATmul_tn_preallocated_ (target, x, y);
}
inline autoMAT MATmul_tn (const constMAT& x, const constMAT& y) { // Z = X'.Y
	autoMAT result = MATraw (x.ncol, y.ncol);
	MATmul_tn_preallocated (result, x, y);
	return result;
}
/*
	Rough matrix multiplication, using an in-cache inner loop.
	The speed is 11,0.79,0.32,0.37 ns per multiply-add
	for x.nrow = x.ncol = y.nrow = y.ncol = 1,10,100,1000.
*/
extern void MATmul_tn_fast_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y);
inline void MATmul_tn_fast_preallocated  (const MAT& target, const constMAT& x, const constMAT& y) {
	Melder_assert (target.nrow == x.ncol);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.nrow == y.nrow);
	MATmul_tn_fast_preallocated (target, x, y);
}
inline autoMAT MATmul_tn_fast (const constMAT& x, const constMAT& y) {
	autoMAT result = MATraw (x.ncol, y.ncol);
	MATmul_tn_fast_preallocated (result, x, y);
	return result;
}

/*
	Target := X' . Y'
	Pairwise summation.
	The speed is 11,1.14,0.71,1.92 ns per multiply-add
	for x.nrow = x.ncol = y.nrow = y.col = 1,10,100,1000.
	For large matrices this is a bit slow.
*/
extern void MATmul_tt_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y);
inline void MATmul_tt_preallocated  (const MAT& target, const constMAT& x, const constMAT& y) {
	Melder_assert (target.nrow == x.ncol);
	Melder_assert (target.ncol == y.nrow);
	Melder_assert (x.nrow == y.ncol);
	MATmul_tt_preallocated_ (target, x, y);
}
inline autoMAT MATmul_tt (const constMAT& x, const constMAT& y) {
	autoMAT result = MATraw (x.ncol, y.nrow);
	MATmul_tt_preallocated (result, x, y);
	return result;
}
/*
	Rough matrix multiplication, using an in-cache inner loop after explicit transposition on the heap.
	The speed is 7,0.87,0.84,0.41 ns per multiply-add
	for x.nrow = x.ncol = y.nrow = y.ncol = 1,10,100,1000.
*/
extern void MATmul_tt_fast_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y);
inline void MATmul_tt_fast_preallocated  (const MAT& target, const constMAT& x, const constMAT& y) {
	Melder_assert (target.nrow == x.ncol);
	Melder_assert (target.ncol == y.nrow);
	Melder_assert (x.nrow == y.ncol);
	MATmul_tt_fast_preallocated_ (target, x, y);
}
inline autoMAT MATmul_tt_fast (const constMAT& x, const constMAT& y) {
	autoMAT result = MATraw (x.ncol, y.nrow);
	MATmul_tt_fast_preallocated (result, x, y);
	return result;
}

inline void MATmultiply_inplace (MAT x, double factor) {
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] *= factor;
}

extern autoMAT MATouter (constVEC x, constVEC y);

extern autoMAT MATpeaks (constVEC x, bool includeEdges, int interpolate, bool sortByHeight);

inline autoMAT MATrandomGauss (integer nrow, integer ncol, double mu, double sigma) {
	auto result = MATraw (nrow, ncol);
	for (integer irow = 1; irow <= nrow; irow ++)
		for (integer icol = 1; icol <= ncol; icol ++)
			result [irow] [icol] = NUMrandomGauss (mu, sigma);
	return result;
}

inline void MATsin_inplace (MAT x) {
	VECsin_inplace (asvector (x));
}

inline void MATsubtract_inplace (MAT x, double number) {
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] -= number;
}
inline void MATsubtractReversed_inplace (MAT x, double number) {
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] = number - x [irow] [icol];
}
inline void MATsubtract_inplace (MAT x, constMAT y) {
	Melder_assert (y.nrow == x.nrow && y.ncol == x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] -= y [irow] [icol];
}
inline void MATsubtractReversed_inplace (MAT x, constMAT y) {
	Melder_assert (y.nrow == x.nrow && y.ncol == x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] = y [irow] [icol] - x [irow] [icol];
}
inline autoMAT MATsubtract (constMAT x, double y) {
	auto result = MATraw (x.nrow, x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			result [irow] [icol] = x [irow] [icol] - y;
	return result;
}
inline autoMAT MATsubtract (double x, constMAT y) {
	auto result = MATraw (y.nrow, y.ncol);
	for (integer irow = 1; irow <= y.nrow; irow ++)
		for (integer icol = 1; icol <= y.ncol; icol ++)
			result [irow] [icol] = x - y [irow] [icol];
	return result;
}
inline autoMAT MATsubtract (constMAT x, constMAT y) {
	Melder_assert (y.nrow == x.nrow && y.ncol == x.ncol);
	auto result = MATraw (x.nrow, x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			result [irow] [icol] = x [irow] [icol] - y [irow] [icol];
	return result;
}

inline void MATtranspose_inplace_mustBeSquare (MAT x) {
	Melder_assert (x.nrow == x.ncol);
	integer n = x.nrow;
	for (integer i = 1; i < n; i ++)
		for (integer j = i + 1; j <= n; j ++)
			std::swap (x [i] [j], x [j] [i]);
}
inline void MATtranspose_preallocated (MAT target, constMAT x) {
	Melder_assert (x.nrow == target.ncol && x.ncol == target.nrow);
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
