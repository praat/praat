#pragma once
/* MAT.h
 *
 * Copyright (C) 2017-2020 Paul Boersma
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
inline autoMAT add_MAT (constMATVU const& x, double number) {
	autoMAT result = raw_MAT (x.nrow, x.ncol);
	result.all()  <<=  x  +  number;
	return result;
}

struct TypeMATmultiply_MAT_NUM          { constMATVU const& x; double number; };
inline TypeMATmultiply_MAT_NUM operator* (constMATVU const& x, double number) { return { x, number }; }
inline TypeMATmultiply_MAT_NUM operator* (double number, constMATVU const& x) { return { x, number }; }
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
inline autoMAT multiply_MAT (constMATVU const& x, double number) {
	autoMAT result = raw_MAT (x.nrow, x.ncol);
	result.all()  <<=  x  *  number;
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
inline autoMAT subtract_MAT (constMATVU const& x, double number) {
	autoMAT result = raw_MAT (x.nrow, x.ncol);
	result.all()  <<=  x  -  number;
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
inline autoMAT subtract_MAT (double number, constMATVU const& x) {
	autoMAT result = raw_MAT (x.nrow, x.ncol);
	result.all()  <<=  number  -  x;
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
inline autoMAT add_MAT (constMATVU const& x, constVECVU const& y) {
	autoMAT result = raw_MAT (x.nrow, x.ncol);
	result.all()  <<=  x  +  y;
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
inline autoMAT multiply_MAT (constMATVU const& x, constVECVU const& y) {
	autoMAT result = raw_MAT (x.nrow, x.ncol);
	result.all()  <<=  x  *  y;
	return result;
}

struct TypeMATsubtract_MAT_VEC          { constMATVU const& x; constVECVU const& y; };
inline TypeMATsubtract_MAT_VEC operator- (constMATVU const& x, constVECVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATsubtract_MAT_VEC const& expr) noexcept { \
		Melder_assert (expr.x.nrow == target.nrow); \
		Melder_assert (expr.x.ncol == target.ncol); \
		Melder_assert (expr.x.ncol == expr.y.size); \
		for (integer irow = 1; irow <= expr.x.nrow; irow ++) \
			for (integer icol = 1; icol <= expr.x.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] [icol] - expr.y [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT subtract_MAT (constMATVU const& x, constVECVU const& y) {
	autoMAT result = raw_MAT (x.nrow, x.ncol);
	result.all()  <<=  x  -  y;
	return result;
}

struct TypeMATadd_VEC_MAT          { constVECVU const& x; constMATVU const& y; };
inline TypeMATadd_VEC_MAT operator+ (constVECVU const& x, constMATVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATadd_VEC_MAT const& expr) noexcept { \
		Melder_assert (expr.y.nrow == expr.x.size); \
		Melder_assert (expr.y.nrow == target.nrow); \
		Melder_assert (expr.y.ncol == target.ncol); \
		for (integer irow = 1; irow <= target.nrow; irow ++) \
			for (integer icol = 1; icol <= target.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] + expr.y [irow] [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT add_MAT (constVECVU const& x, constMATVU const& y) {
	autoMAT result = raw_MAT (y.nrow, y.ncol);
	result.all()  <<=  x  +  y;
	return result;
}

struct TypeMATmultiply_VEC_MAT          { constVECVU const& x; constMATVU const& y; };
inline TypeMATmultiply_VEC_MAT operator* (constVECVU const& x, constMATVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATmultiply_VEC_MAT const& expr) noexcept { \
		Melder_assert (expr.y.nrow == expr.x.size); \
		Melder_assert (expr.y.nrow == target.nrow); \
		Melder_assert (expr.y.ncol == target.ncol); \
		for (integer irow = 1; irow <= target.nrow; irow ++) \
			for (integer icol = 1; icol <= target.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] * expr.y [irow] [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT multiply_MAT (constVECVU const& x, constMATVU const& y) {
	autoMAT result = raw_MAT (y.nrow, y.ncol);
	result.all()  <<=  x  *  y;
	return result;
}

struct TypeMATsubtract_VEC_MAT          { constVECVU const& x; constMATVU const& y; };
inline TypeMATsubtract_VEC_MAT operator- (constVECVU const& x, constMATVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (MATVU const& target, TypeMATsubtract_VEC_MAT const& expr) noexcept { \
		Melder_assert (expr.y.nrow == expr.x.size); \
		Melder_assert (expr.y.nrow == target.nrow); \
		Melder_assert (expr.y.ncol == target.ncol); \
		for (integer irow = 1; irow <= target.nrow; irow ++) \
			for (integer icol = 1; icol <= target.ncol; icol ++) \
				target [irow] [icol] op expr.x [irow] - expr.y [irow] [icol]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoMAT subtract_MAT (constVECVU const& x, constMATVU const& y) {
	autoMAT result = raw_MAT (y.nrow, y.ncol);
	result.all()  <<=  x  -  y;
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
inline autoMAT add_MAT (constMATVU const& x, constMATVU const& y) {
	autoMAT result = raw_MAT (x.nrow, x.ncol);
	result.all()  <<=  x  +  y;
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
inline autoMAT multiply_MAT (constMATVU const& x, constMATVU const& y) {
	autoMAT result = raw_MAT (x.nrow, x.ncol);
	result.all()  <<=  x  *  y;
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
inline autoMAT subtract_MAT (constMATVU const& x, constMATVU const& y) {
	autoMAT result = raw_MAT (x.nrow, x.ncol);
	result.all()  <<=  x  -  y;
	return result;
}

/*
	Make the average of each column zero.
		a[i][j] -= a[.][j]
*/
extern void centreEachColumn_MAT_inout (MATVU const& x) noexcept;

/*
	Make the average of each row zero.
		a[i][j] -= a[i][.]
*/
extern void centreEachRow_MAT_inout (MATVU const& x) noexcept;

/*
	Make the average of every column and every row zero.
		a[i][j] += - a[i][.] - a[.][j] + a[.][.]
*/
extern void doubleCentre_MAT_inout (MATVU const& x) noexcept;

extern void mtm_MAT_out (MATVU const& target, constMATVU const& x) noexcept;
inline autoMAT mtm_MAT (constMATVU const& x) {
	autoMAT result = raw_MAT (x.ncol, x.ncol);
	mtm_MAT_out (result.get(), x);
	return result;
}

/*
	Precise matrix multiplication, using pairwise summation.
*/
extern void _mul_MAT_out (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept;
inline void mul_MAT_out  (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.ncol == y.nrow);
	_mul_MAT_out (target, x, y);
}
inline autoMAT mul_MAT (constMATVU const& x, constMATVU const& y) {
	autoMAT result = raw_MAT (x.nrow, y.ncol);
	mul_MAT_out (result.all(), x, y);
	return result;
}
/*
	Faster multiplication of large matrices,
	which allocates new matrices to make x.colStride and y.rowStride 1
	(unless they are already 1).
	Because of the use of malloc, this function may not be thread-safe.
*/
extern void _mul_forceAllocation_MAT_out (MATVU const& target, constMATVU x, constMATVU y);
inline void mul_forceAllocation_MAT_out  (MATVU const& target, constMATVU x, constMATVU y) {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.ncol == y.nrow);
	_mul_forceAllocation_MAT_out (target, x, y);
}
inline autoMAT newMATmul_forceAllocation (constMATVU const& x, constMATVU const& y) {
	autoMAT result = raw_MAT (x.nrow, y.ncol);
	mul_forceAllocation_MAT_out (result.all(), x, y);
	return result;
}
/*
	The faster of mul_forceAllocation_MAT_out and mul_MAT_out.
	Because of the use of malloc, this function may not be thread-safe.
*/
extern void _mul_allowAllocation_MAT_out (MATVU const& target, constMATVU x, constMATVU y);
inline void mul_allowAllocation_MAT_out  (MATVU const& target, constMATVU x, constMATVU y) {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.ncol == y.nrow);
	_mul_allowAllocation_MAT_out (target, x, y);
}
inline autoMAT mul_allowAllocation_MAT (constMATVU const& x, constMATVU const& y) {
	autoMAT result = raw_MAT (x.nrow, y.ncol);
	mul_allowAllocation_MAT_out (result.all(), x, y);
	return result;
}
/*
	Rough matrix multiplication, using an in-cache inner loop if that is faster.
*/
extern void _mul_fast_MAT_out (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept;
inline void mul_fast_MAT_out  (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept {
	Melder_assert (target.nrow == x.nrow);
	Melder_assert (target.ncol == y.ncol);
	Melder_assert (x.ncol == y.nrow);
	_mul_fast_MAT_out (target, x, y);
}
inline autoMAT mul_fast_MAT (constMATVU const& x, constMATVU const& y) {
	autoMAT result = raw_MAT (x.nrow, y.ncol);
	mul_fast_MAT_out (result.all(), x, y);
	return result;
}
void MATmul_forceMetal_ (MATVU const& target, constMATVU const& x, constMATVU const& y);
void MATmul_forceOpenCL_ (MATVU const& target, constMATVU const& x, constMATVU const& y);

void outer_MAT_out (MATVU const& target, constVECVU const& x, constVECVU const& y);
extern autoMAT outer_MAT (constVECVU const& x, constVECVU const& y);

extern autoMAT peaks_MAT (constVECVU const& x, bool includeEdges, int interpolate, bool sortByHeight);

void power_MAT_out (MATVU const& target, constMATVU const& mat, double power);
inline autoMAT power_MAT (constMATVU const& mat, double power) {
	autoMAT result = raw_MAT (mat.nrow, mat.ncol);
	power_MAT_out (result.all(), mat, power);
	return result;
}

inline void randomGauss_MAT_out (MATVU const& target, double mu, double sigma) noexcept {
	for (integer irow = 1; irow <= target.nrow; irow ++)
		for (integer icol = 1; icol <= target.ncol; icol ++)
			target [irow] [icol] = NUMrandomGauss (mu, sigma);
}
inline autoMAT randomGauss_MAT (integer nrow, integer ncol, double mu, double sigma) {
	autoMAT result = raw_MAT (nrow, ncol);
	randomGauss_MAT_out (result.all(), mu, sigma);
	return result;
}
inline autoMAT randomGauss_MAT (constMATVU const& model, double mu, double sigma) {
	autoMAT result = raw_MAT (model.nrow, model.ncol);
	randomGauss_MAT_out (result.all(), mu, sigma);
	return result;
}

inline void randomUniform_MAT_out (MATVU const& target, double lowest, double highest) noexcept {
	for (integer irow = 1; irow <= target.nrow; irow ++)
		for (integer icol = 1; icol <= target.ncol; icol ++)
			target [irow] [icol] = NUMrandomUniform (lowest, highest);
}
inline autoMAT randomUniform_MAT (integer nrow, integer ncol, double lowest, double highest) {
	autoMAT result = raw_MAT (nrow, ncol);
	randomUniform_MAT_out (result.all(), lowest, highest);
	return result;
}
inline autoMAT randomUniform_MAT (constMATVU const& model, double lowest, double highest) {
	autoMAT result = raw_MAT (model.nrow, model.ncol);
	randomUniform_MAT_out (result.all(), lowest, highest);
	return result;
}

inline void sin_MAT_inout (MATVU const& mat) noexcept {
	for (integer irow = 1; irow <= mat.nrow; irow ++)
		for (integer icol = 1; icol <= mat.ncol; icol ++)
			mat [irow] [icol] = sin (mat [irow] [icol]);
}

inline void subtractReversed_MAT_inout (MATVU const& x, double number) noexcept {
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] = number - x [irow] [icol];
}
inline void subtractReversed_MAT_inout (MATVU const& x, constMATVU const& y) noexcept {
	Melder_assert (y.nrow == x.nrow && y.ncol == x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			x [irow] [icol] = y [irow] [icol] - x [irow] [icol];
}

inline void transpose_mustBeSquare_MAT_inout (MATVU const& x) noexcept {
	Melder_assert (x.nrow == x.ncol);
	integer n = x.nrow;
	for (integer i = 1; i < n; i ++)
		for (integer j = i + 1; j <= n; j ++)
			std::swap (x [i] [j], x [j] [i]);
}
inline void transpose_MAT_out (MATVU const& target, constMATVU const& x) noexcept {
	Melder_assert (x.nrow == target.ncol && x.ncol == target.nrow);
	for (integer irow = 1; irow <= target.nrow; irow ++)
		for (integer icol = 1; icol <= target.ncol; icol ++)
			target [irow] [icol] = x [icol] [irow];
}
inline autoMAT transpose_MAT (constMATVU const& x) {
	autoMAT result = raw_MAT (x.ncol, x.nrow);
	transpose_MAT_out (result.get(), x);
	return result;
}

/* End of file MAT.h */
