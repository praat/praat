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

#define GENERATE_FIVE_TENSOR_FUNCTIONS  \
	GENERATE_ONE_TENSOR_FUNCTION (operator<<=, =) \
	GENERATE_ONE_TENSOR_FUNCTION (operator+=, +=) \
	GENERATE_ONE_TENSOR_FUNCTION (operator-=, -=) \
	GENERATE_ONE_TENSOR_FUNCTION (operator*=, *=) \
	GENERATE_ONE_TENSOR_FUNCTION (operator/=, /=)

#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, double number) noexcept { \
		for (integer i = 1; i <= target.size; i ++) \
			target [i] op number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION

#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, constVECVU const& x) { \
		Melder_assert (target.size == x.size); \
		for (integer i = 1; i <= target.size; i ++) \
			target [i] op x [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION

struct TypeVECadd_VEC_NUM          { constVECVU const& x; double number; };
inline TypeVECadd_VEC_NUM operator+ (constVECVU const& x, double number) { return { x, number }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECadd_VEC_NUM const& expr) noexcept { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.x [i] + expr.number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC newVECadd (constVECVU const& x, double number) {
	autoVEC result = newVECraw (x.size);
	result.all() <<= x  +  number;
	return result;
}

struct TypeVECsubtract_VEC_NUM          { constVECVU const& x; double number; };
inline TypeVECsubtract_VEC_NUM operator- (constVECVU const& x, double number) { return { x, number }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECsubtract_VEC_NUM const& expr) noexcept { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.x [i] - expr.number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC newVECsubtract (constVECVU const& x, double number) {
	autoVEC result = newVECraw (x.size);
	result.all() <<= x  -  number;
	return result;
}

struct TypeVECmultiply_VEC_NUM          { constVECVU const& x; double number; };
inline TypeVECmultiply_VEC_NUM operator* (constVECVU const& x, double number) { return { x, number }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECmultiply_VEC_NUM const& expr) noexcept { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.x [i] * expr.number; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC newVECmultiply (constVECVU const& x, double factor) {
	autoVEC result = newVECraw (x.size);
	result.all() <<= x  *  factor;
	return result;
}

struct TypeVECadd_NUM_VEC          { double number; constVECVU const& x; };
inline TypeVECadd_NUM_VEC operator+ (double number, constVECVU const& x) { return { number, x }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECadd_NUM_VEC const& expr) noexcept { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.number + expr.x [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC newVECadd (double number, constVECVU const& x) {
	autoVEC result = newVECraw (x.size);
	result.all() <<= number  +  x;
	return result;
}

struct TypeVECsubtract_NUM_VEC          { double number; constVECVU const& x; };
inline TypeVECsubtract_NUM_VEC operator- (double number, constVECVU const& x) { return { number, x }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECsubtract_NUM_VEC const& expr) noexcept { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.number - expr.x [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC newVECsubtract (double number, constVECVU const& x) {
	autoVEC result = newVECraw (x.size);
	result.all() <<= number  -  x;
	return result;
}

struct TypeVECmultiply_NUM_VEC          { double number; constVECVU const& x; };
inline TypeVECmultiply_NUM_VEC operator* (double number, constVECVU const& x) { return { number, x }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (VECVU const& target, TypeVECmultiply_NUM_VEC const& expr) noexcept { \
		Melder_assert (expr.x.size == target.size); \
		for (integer i = 1; i <= expr.x.size; i ++) \
			target [i] op expr.number * expr.x [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC newVECmultiply (double number, constVECVU const& x) {
	autoVEC result = newVECraw (x.size);
	result.all() <<= number  *  x;
	return result;
}

struct TypeVECadd_VEC_VEC          { constVECVU const& x; constVECVU const& y; };
inline TypeVECadd_VEC_VEC operator+ (constVECVU const& x, constVECVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (const VECVU& target, TypeVECadd_VEC_VEC expr) noexcept { \
		integer n = target.size; \
		Melder_assert (expr.x.size == n); \
		Melder_assert (expr.y.size == n); \
		for (integer i = 1; i <= n; i ++) \
			target [i] op expr.x [i] + expr.y [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC newVECadd (constVECVU const& x, constVECVU const& y) {
	autoVEC result = newVECraw (x.size);
	result.all() <<= x  +  y;
	return result;
}

struct TypeVECsubtract_VEC_VEC          { constVECVU const& x; constVECVU const& y; };
inline TypeVECsubtract_VEC_VEC operator- (constVECVU const& x, constVECVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (const VECVU& target, TypeVECsubtract_VEC_VEC expr) noexcept { \
		integer n = target.size; \
		Melder_assert (expr.x.size == n); \
		Melder_assert (expr.y.size == n); \
		for (integer i = 1; i <= n; i ++) \
			target [i] op expr.x [i] - expr.y [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC newVECsubtract (constVECVU const& x, constVECVU const& y) {
	autoVEC result = newVECraw (x.size);
	result.all() <<= x  -  y;
	return result;
}

struct TypeVECmultiply_VEC_VEC          { constVECVU const& x; constVECVU const& y; };
inline TypeVECmultiply_VEC_VEC operator* (constVECVU const& x, constVECVU const& y) { return { x, y }; }
#define GENERATE_ONE_TENSOR_FUNCTION(operator, op)  \
	inline void operator (const VECVU& target, TypeVECmultiply_VEC_VEC expr) noexcept { \
		integer n = target.size; \
		Melder_assert (expr.x.size == n); \
		Melder_assert (expr.y.size == n); \
		for (integer i = 1; i <= n; i ++) \
			target [i] op expr.x [i] * expr.y [i]; \
	}
GENERATE_FIVE_TENSOR_FUNCTIONS
#undef GENERATE_ONE_TENSOR_FUNCTION
inline autoVEC newVECmultiply (constVECVU const& x, constVECVU const& y) {
	autoVEC result = newVECraw (x.size);
	result.all() <<= x  *  y;
	return result;
}

extern void VECadd_macfast_ (const VECVU& target, const constVECVU& x, const constVECVU& y) noexcept;
inline void VECadd (const VECVU& target, const constVECVU& x, const constVECVU& y) noexcept {
	integer n = target.size;
	Melder_assert (x.size == n);
	Melder_assert (y.size == n);
	#if defined (macintoshXXX)
		if (n >= 64)
			return VECadd_macfast_ (target, x, y);
	#endif
	for (integer i = 1; i <= n; i ++)
		target [i] = x [i] + y [i];
}

inline void VECcentre_inplace (VECVU const& x, double *out_mean = nullptr) noexcept {
	double xmean = NUMmean (x);
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= xmean;
	if (out_mean)
		*out_mean = xmean;
}

inline autoVEC newVECcolumn (constMATVU const& source, integer columnNumber) {
	autoVEC target = newVECraw (source.nrow);
	target.all() <<= source.column (columnNumber);
	return target;
}

inline void VECcolumnMeans (VECVU const& target, constMATVU const& x) noexcept {
	Melder_assert (target.size == x.ncol);
	for (integer icol = 1; icol <= x.ncol; icol ++)
		target [icol] = NUMmean (x.column (icol));
}
inline autoVEC newVECcolumnMeans (constMATVU const& x) {
	autoVEC result = newVECraw (x.ncol);
	VECcolumnMeans (result.get(), x);
	return result;
}

extern void VECmul (VECVU const& target, constVECVU const& vec, constMATVU const& mat) noexcept;
extern void VECmul (VECVU const& target, constMATVU const& mat, constVECVU const& vec) noexcept;
extern autoVEC newVECmul (constVECVU const& vec, constMATVU const& mat);
extern autoVEC newVECmul (constMATVU const& mat, constVECVU const& vec);

extern void VECpower (VECVU const& target, constVECVU const& vec, double power);
inline autoVEC newVECpower (constVECVU const& vec, double power) {
	autoVEC result = newVECraw (vec.size);
	VECpower (result.all(), vec, power);
	return result;
}

inline void VECrandomGauss (VECVU const& target, double mu, double sigma) noexcept {
	for (integer i = 1; i <= target.size; i ++)
		target [i] = NUMrandomGauss (mu, sigma);
}
inline autoVEC newVECrandomGauss (integer size, double mu, double sigma) {
	autoVEC result = newVECraw (size);
	VECrandomGauss (result.all(), mu, sigma);
	return result;
}
inline autoVEC newVECrandomGauss (constVECVU const& model, double mu, double sigma) {
	autoVEC result = newVECraw (model.size);
	VECrandomGauss (result.all(), mu, sigma);
	return result;
}

inline void VECrandomUniform (VECVU const& target, double lowest, double highest) noexcept {
	for (integer i = 1; i <= target.size; i ++)
		target [i] = NUMrandomUniform (lowest, highest);
}
inline autoVEC newVECrandomUniform (integer size, double lowest, double highest) {
	autoVEC result = newVECraw (size);
	VECrandomUniform (result.all(), lowest, highest);
	return result;
}
inline autoVEC newVECrandomUniform (constVECVU const& model, double lowest, double highest) {
	autoVEC result = newVECraw (model.size);
	VECrandomUniform (result.all(), lowest, highest);
	return result;
}

inline void VECrowInners (VECVU const& target, constMATVU const& x, constMATVU const& y) {
	Melder_assert (y.nrow == x.nrow);
	Melder_assert (y.ncol == x.ncol);
	Melder_assert (target.size == x.nrow);
	for (integer irow = 1; irow <= target.size; irow ++)
		target [irow] = NUMinner (x.row (irow), y.row (irow));
}
inline autoVEC newVECrowInners (constMATVU const& x, constMATVU const& y) {
	autoVEC result = newVECraw (x.nrow);
	VECrowInners (result.all(), x, y);
	return result;
}

inline void VECsin_inplace (VECVU const& vec) noexcept {
	for (integer i = 1; i <= vec.size; i ++)
		vec [i] = sin (vec [i]);
}

extern void VECsort_inplace (VECVU const& x) noexcept;

inline autoVEC newVECrowSums (constMATVU const& x) {
	autoVEC result = newVECraw (x.nrow);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		result [irow] = NUMsum (x.row (irow));
	return result;
}

inline autoVEC newVECcolumnSums (constMATVU const& x) {
	autoVEC result = newVECraw (x.ncol);
	for (integer icol = 1; icol <= x.ncol; icol ++)
		result [icol] = NUMsum (x.column (icol));
	return result;
}

extern autoVEC newVECfrom_to (double from, double to);
extern autoVEC newVECfrom_to_by (double from, double to, double by);
extern autoVEC newVECbetween_by (double from, double to, double by);
extern autoVEC newVECto (double to);

/* End of file VEC.h */
