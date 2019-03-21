#pragma once
/* MAT.h
 *
 * Copyright (C) 2017-2019 Paul Boersma
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

#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, double number) noexcept { \
		integer mindim = ( target.rowStride < target.colStride ? 1 : 2 ); \
		if (mindim == 1) \
			for (integer icol = 1; icol <= target.ncol; icol ++) \
				for (integer irow = 1; irow <= target.nrow; irow ++) \
					target [irow] [icol] op number; \
		else \
			for (integer irow = 1; irow <= target.nrow; irow ++) \
				for (integer icol = 1; icol <= target.ncol; icol ++) \
					target [irow] [icol] op number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION

#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, constMATVU const& x) { \
		Melder_assert (target.nrow == x.nrow); \
		Melder_assert (target.ncol == x.ncol); \
		integer mindim = ( target.rowStride < target.colStride ? 1 : 2 ); \
		if (mindim == 1) \
			for (integer icol = 1; icol <= target.ncol; icol ++) \
				for (integer irow = 1; irow <= target.nrow; irow ++) \
					target [irow] [icol] op x [irow] [icol]; \
		else \
			for (integer irow = 1; irow <= target.nrow; irow ++) \
				for (integer icol = 1; icol <= target.ncol; icol ++) \
					target [irow] [icol] op x [irow] [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION

#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, constVECVU const& x) { \
		Melder_assert (target.ncol == x.size); \
		integer mindim = ( target.rowStride < target.colStride ? 1 : 2 ); \
		if (mindim == 1) \
			for (integer icol = 1; icol <= target.ncol; icol ++) \
				for (integer irow = 1; irow <= target.nrow; irow ++) \
					target [irow] [icol] op x [icol]; \
		else \
			for (integer irow = 1; irow <= target.nrow; irow ++) \
				for (integer icol = 1; icol <= target.ncol; icol ++) \
					target [irow] [icol] op x [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION

struct TypeMATadd_MAT_NUM          { constMATVU const& x; double number; };
inline TypeMATadd_MAT_NUM operator+ (constMATVU const& x, double number) { return { x, number }; }
inline TypeMATadd_MAT_NUM operator+ (double number, constMATVU const& x) { return { x, number }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATadd_MAT_NUM const& expr) noexcept { \
		Melder_assert (expr.x.nrow == target.nrow); \
		Melder_assert (expr.x.ncol == target.ncol); \
		for (integer irow = 1; irow <= expr.x.nrow; irow ++) \
			for (integer icol = 1; icol <= expr.x.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] [icol] + expr.number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT newMATadd (constMATVU const& x, double number) {
	autoMAT result = newMATraw (x.nrow, x.ncol);
	result.all() <<= x  +  number;
	return result;
}

struct TypeMATsubtract_MAT_NUM          { constMATVU const& x; double number; };
inline TypeMATsubtract_MAT_NUM operator- (constMATVU const& x, double number) { return { x, number }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATsubtract_MAT_NUM const& expr) noexcept { \
		Melder_assert (expr.x.nrow == target.nrow); \
		Melder_assert (expr.x.ncol == target.ncol); \
		for (integer irow = 1; irow <= expr.x.nrow; irow ++) \
			for (integer icol = 1; icol <= expr.x.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] [icol] - expr.number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT newMATsubtract (constMATVU const& x, double number) {
	autoMAT result = newMATraw (x.nrow, x.ncol);
	result.all() <<= x  -  number;
	return result;
}

struct TypeMATsubtract_NUM_MAT          { double number; constMATVU const& x; };
inline TypeMATsubtract_NUM_MAT operator- (double number, constMATVU const& x) { return { number, x }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATsubtract_NUM_MAT const& expr) noexcept { \
		Melder_assert (expr.x.nrow == target.nrow); \
		Melder_assert (expr.x.ncol == target.ncol); \
		for (integer irow = 1; irow <= expr.x.nrow; irow ++) \
			for (integer icol = 1; icol <= expr.x.ncol; icol ++) \
				target [irow] [icol] op expr.number - expr.x [irow] [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT newMATsubtract (double number, constMATVU const& x) {
	autoMAT result = newMATraw (x.nrow, x.ncol);
	result.all() <<= number  -  x;
	return result;
}

struct TypeMATmultiply_MAT_NUM          { constMATVU const& x; double number; };
inline TypeMATmultiply_MAT_NUM operator* (constMATVU const& x, double number) { return { x, number }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATmultiply_MAT_NUM const& expr) noexcept { \
		Melder_assert (expr.x.nrow == target.nrow); \
		Melder_assert (expr.x.ncol == target.ncol); \
		for (integer irow = 1; irow <= expr.x.nrow; irow ++) \
			for (integer icol = 1; icol <= expr.x.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] [icol] * expr.number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT newMATmultiply (constMATVU const& x, double number) {
	autoMAT result = newMATraw (x.nrow, x.ncol);
	result.all() <<= x  *  number;
	return result;
}

struct TypeMATadd_MAT_VEC          { constMATVU const& x; constVECVU const& y; };
inline TypeMATadd_MAT_VEC operator+ (constMATVU const& x, constVECVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATadd_MAT_VEC const& expr) noexcept { \
		Melder_assert (expr.x.nrow == target.nrow); \
		Melder_assert (expr.x.ncol == target.ncol); \
		Melder_assert (expr.x.ncol == expr.y.size); \
		for (integer irow = 1; irow <= expr.x.nrow; irow ++) \
			for (integer icol = 1; icol <= expr.x.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] [icol] + expr.y [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT newMATadd (constMATVU const& x, constVECVU const& y) {
	autoMAT result = newMATraw (x.nrow, x.ncol);
	result.all() <<= x  +  y;
	return result;
}

struct TypeMATmultiply_MAT_VEC          { constMATVU const& x; constVECVU const& y; };
inline TypeMATmultiply_MAT_VEC operator* (constMATVU const& x, constVECVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATmultiply_MAT_VEC const& expr) noexcept { \
		Melder_assert (expr.x.nrow == target.nrow); \
		Melder_assert (expr.x.ncol == target.ncol); \
		Melder_assert (expr.x.ncol == expr.y.size); \
		for (integer irow = 1; irow <= expr.x.nrow; irow ++) \
			for (integer icol = 1; icol <= expr.x.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] [icol] * expr.y [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT newMATmultiply (constMATVU const& x, constVECVU const& y) {
	autoMAT result = newMATraw (x.nrow, x.ncol);
	result.all() <<= x  *  y;
	return result;
}

struct TypeMATadd_MAT_MAT          { constMATVU const& x; constMATVU const& y; };
inline TypeMATadd_MAT_MAT operator+ (constMATVU const& x, constMATVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATadd_MAT_MAT const& expr) noexcept { \
		Melder_assert (expr.x.nrow == target.nrow); \
		Melder_assert (expr.x.ncol == target.ncol); \
		Melder_assert (expr.x.nrow == expr.y.nrow); \
		Melder_assert (expr.x.ncol == expr.y.ncol); \
		for (integer irow = 1; irow <= expr.x.nrow; irow ++) \
			for (integer icol = 1; icol <= expr.x.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] [icol] + expr.y [irow] [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT newMATadd (constMATVU const& x, constMATVU const& y) {
	autoMAT result = newMATraw (x.nrow, x.ncol);
	result.all() <<= x  +  y;
	return result;
}

struct TypeMATsubtract_MAT_MAT          { constMATVU const& x; constMATVU const& y; };
inline TypeMATsubtract_MAT_MAT operator- (constMATVU const& x, constMATVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATsubtract_MAT_MAT const& expr) noexcept { \
		Melder_assert (expr.x.nrow == target.nrow); \
		Melder_assert (expr.x.ncol == target.ncol); \
		Melder_assert (expr.x.nrow == expr.y.nrow); \
		Melder_assert (expr.x.ncol == expr.y.ncol); \
		for (integer irow = 1; irow <= expr.x.nrow; irow ++) \
			for (integer icol = 1; icol <= expr.x.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] [icol] - expr.y [irow] [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT newMATsubtract (constMATVU const& x, constMATVU const& y) {
	autoMAT result = newMATraw (x.nrow, x.ncol);
	result.all() <<= x  -  y;
	return result;
}

struct TypeMATmultiply_MAT_MAT          { constMATVU const& x; constMATVU const& y; };
inline TypeMATmultiply_MAT_MAT operator* (constMATVU const& x, constMATVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATmultiply_MAT_MAT const& expr) noexcept { \
		Melder_assert (expr.x.nrow == target.nrow); \
		Melder_assert (expr.x.ncol == target.ncol); \
		Melder_assert (expr.x.nrow == expr.y.nrow); \
		Melder_assert (expr.x.ncol == expr.y.ncol); \
		for (integer irow = 1; irow <= expr.x.nrow; irow ++) \
			for (integer icol = 1; icol <= expr.x.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] [icol] * expr.y [irow] [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT newMATmultiply (constMATVU const& x, constMATVU const& y) {
	autoMAT result = newMATraw (x.nrow, x.ncol);
	result.all() <<= x  *  y;
	return result;
}

/*
	Make the average of each column zero.
		a[i][j] -= a[.][j]
*/
extern void MATcentreEachColumn_inplace (MATVU const& x) noexcept;

/*
	Make the average of each row zero.
		a[i][j] -= a[i][.]
*/
extern void MATcentreEachRow_inplace (MATVU const& x) noexcept;

/*
	Make the average of every column and every row zero.
		a[i][j] += - a[i][.] - a[.][j] + a[.][.]
*/
extern void MATdoubleCentre_inplace (MATVU const& x) noexcept;

extern void MATmtm_preallocated (MATVU const& target, constMATVU const& x) noexcept;
inline autoMAT newMATmtm (constMATVU const& x) {
	autoMAT result = newMATraw (x.ncol, x.ncol);
	MATmtm_preallocated (result.get(), x);
	return result;
}

/*
	Precise matrix multiplication, using pairwise summation.
*/
extern void MATVUmul_ (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept;
inline void MATVUmul  (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.ncol == y.nrow);
	MATVUmul_ (target, x, y);
}
inline autoMAT newMATmul (constMATVU const& x, constMATVU const& y) {
	autoMAT result = newMATraw (x.nrow, y.ncol);
	MATVUmul (result.all(), x, y);
	return result;
}
/*
	Faster multiplication of large matrices,
	which allocates new matrices to make x.colStride and y.rowStride 1
	(unless they are already 1).
	Because of the use of malloc, this function may not be thread-safe.
*/
extern void MATVUmul_forceAllocation_ (MATVU const& target, constMATVU x, constMATVU y);
inline void MATVUmul_forceAllocation  (MATVU const& target, constMATVU x, constMATVU y) {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.ncol == y.nrow);
	MATVUmul_forceAllocation_ (target, x, y);
}
inline autoMAT newMATmul_forceAllocation (constMATVU const& x, constMATVU const& y) {
	autoMAT result = newMATraw (x.nrow, y.ncol);
	MATVUmul_forceAllocation (result.all(), x, y);
	return result;
}
/*
	The faster of MATVUmul_forceAllocation and MATVUmul.
	Because of the use of malloc, this function may not be thread-safe.
*/
extern void MATVUmul_allowAllocation_ (MATVU const& target, constMATVU x, constMATVU y);
inline void MATVUmul_allowAllocation  (MATVU const& target, constMATVU x, constMATVU y) {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.ncol == y.nrow);
	MATVUmul_allowAllocation_ (target, x, y);
}
inline autoMAT newMATmul_allowAllocation (constMATVU const& x, constMATVU const& y) {
	autoMAT result = newMATraw (x.nrow, y.ncol);
	MATVUmul_allowAllocation (result.all(), x, y);
	return result;
}
/*
	Rough matrix multiplication, using an in-cache inner loop if that is faster.
*/
extern void MATVUmul_fast_ (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept;
inline void MATVUmul_fast  (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.ncol == y.nrow);
	MATVUmul_fast_ (target, x, y);
}
inline autoMAT newMATmul_fast (constMATVU const& x, constMATVU const& y) {
	autoMAT result = newMATraw (x.nrow, y.ncol);
	MATVUmul_fast (result.all(), x, y);
	return result;
}
void MATVUmul_forceMetal_ (MATVU const& target, constMATVU const& x, constMATVU const& y);
void MATVUmul_forceOpenCL_ (MATVU const& target, constMATVU const& x, constMATVU const& y);

void MATouter_preallocated (MATVU const& target, constVECVU const& x, constVECVU const& y);
extern autoMAT newMATouter (constVECVU const& x, constVECVU const& y);

extern autoMAT newMATpeaks (constVECVU const& x, bool includeEdges, int interpolate, bool sortByHeight);

inline void MATrandomGauss_preallocated (MATVU const& target, double mu, double sigma) noexcept {
	for (integer irow = 1; irow <= target.nrow; irow ++)
		for (integer icol = 1; icol <= target.ncol; icol ++)
			target [irow] [icol] = NUMrandomGauss (mu, sigma);
}
inline autoMAT newMATrandomGauss (integer nrow, integer ncol, double mu, double sigma) {
	autoMAT result = newMATraw (nrow, ncol);
	MATrandomGauss_preallocated (result.all(), mu, sigma);
	return result;
}

inline void MATrandomUniform_preallocated (MATVU const& target, double lowest, double highest) noexcept {
	for (integer irow = 1; irow <= target.nrow; irow ++)
		for (integer icol = 1; icol <= target.ncol; icol ++)
			target [irow] [icol] = NUMrandomUniform (lowest, highest);
}
inline autoMAT newMATrandomUniform (integer nrow, integer ncol, double lowest, double highest) {
	autoMAT result = newMATraw (nrow, ncol);
	MATrandomUniform_preallocated (result.all(), lowest, highest);
	return result;
}

inline void MATsin_inplace (MATVU const& mat) noexcept {
	for (integer irow = 1; irow <= mat.nrow; irow ++)
		for (integer icol = 1; icol <= mat.ncol; icol ++)
			mat [irow] [icol] = sin (mat [irow] [icol]);
}

inline void MATsubtractReversed_inplace (MATVU const& x, double number) noexcept {
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] = number - x [irow] [icol];
}
inline void MATsubtractReversed_inplace (MATVU const& x, constMATVU const& y) noexcept {
	Melder_assert (y.nrow == x.nrow && y.ncol == x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] = y [irow] [icol] - x [irow] [icol];
}

inline void MATtranspose_inplace_mustBeSquare (MATVU const& x) noexcept {
	Melder_assert (x.nrow == x.ncol);
	integer n = x.nrow;
	for (integer i = 1; i < n; i ++)
		for (integer j = i + 1; j <= n; j ++)
			std::swap (x [i] [j], x [j] [i]);
}
inline void MATtranspose_preallocated (MATVU const& target, constMATVU const& x) noexcept {
	Melder_assert (x.nrow == target.ncol && x.ncol == target.nrow);
	for (integer irow = 1; irow <= target.nrow; irow ++)
		for (integer icol = 1; icol <= target.ncol; icol ++)
			target [irow] [icol] = x [icol] [irow];
}
inline autoMAT newMATtranspose (constMATVU const& x) {
	autoMAT result = newMATraw (x.ncol, x.nrow);
	MATtranspose_preallocated (result.get(), x);
	return result;
}

/* End of file MAT.h */
