/* VEC.cpp
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

#include "melder.h"
#include "../dwsys/NUM2.h"   /* for NUMsort2 */

#ifdef macintosh
	#include "macport_on.h"
	#include <Accelerate/Accelerate.h>
	#include "macport_off.h"
#endif

#if defined (macintosh)
void _add_macfast_VEC_out (const VECVU& target, const constVECVU& x, const constVECVU& y) noexcept {
	integer n = target.size;
	vDSP_vaddD (& x [1], x.stride, & y [1], y.stride, & target [1], target.stride, integer_to_uinteger (n));
	/*
		Speed if always vDSP_vaddD:
			//9.3,1.26,0.21, 0.10,0.42,0.70, 1.17,1.97,5.32
		Speed if always explicit loop:
			0.61,1.99,4.32, 4.39,2.61,1.51, 0.85,0.50,0.41
		Combined:
			6.1,1.19,0.23, 0.13,0.41,0.70, 1.19,2.02,2.45
	*/
}
#endif

autoVEC from_to_VEC (double from, double to) {
	const integer numberOfElements = Melder_ifloor (to - from + 1.0);
	if (numberOfElements < 1)
		return autoVEC ();
	autoVEC result = raw_VEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = from + (double) (i - 1);
	return result;
}
autoINTVEC from_to_INTVEC (integer from, integer to) {
	const integer numberOfElements = to - from + 1;
	if (numberOfElements < 1)
		return autoINTVEC ();
	autoINTVEC result = raw_INTVEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = from + (i - 1);
	return result;
}

autoVEC from_to_by_VEC (double from, double to, double by) {
	Melder_require (by != 0.0,
		U"from_to_by#: cannot have a step (“by”) of zero.");
	/*
		The following algorithm works for both positive and negative `by`.
	*/
	const integer numberOfElements = Melder_ifloor ((to - from) / by + 1.0);
	if (numberOfElements < 1)
		return autoVEC ();
	autoVEC result = raw_VEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = from + (double) (i - 1) * by;
	return result;
}
autoINTVEC from_to_by_INTVEC (integer from, integer to, integer by) {
	Melder_require (by != 0,
		U"from_to_by#: cannot have a step (“by”) of zero.");
	/*
		The following algorithm works for both positive and negative `by`.
	*/
	const integer numberOfElements = (to - from) / by + 1;
	if (numberOfElements < 1)
		return autoINTVEC ();
	autoINTVEC result = raw_INTVEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = from + (i - 1) * by;
	return result;
}
/*@praat
	assert from_to_count# (0, 10, 5) = { 0, 2.5, 5, 7.5, 10 }
@*/
autoVEC from_to_count_VEC (double from, double to, integer count) {
	Melder_require (count >= 2,
		U"from_to_count#: cannot have fewer than two elements.");
	autoVEC result = raw_VEC (count);
	const double by = (to - from) / (count - 1);
	for (integer i = 1; i < count; i ++)
		result [i] = from + (double) (i - 1) * by;
	result [count] = to;
	return result;
}
autoINTVEC from_to_count_INTVEC (integer from, integer to, integer count) {
	Melder_require (count >= 2,
		U"from_to_count#: cannot have fewer than two elements.");
	autoINTVEC result = raw_INTVEC (count);
	const integer by = (to - from) / (count - 1);
	for (integer i = 1; i < count; i ++)
		result [i] = from + (i - 1) * by;
	result [count] = to;
	return result;
}

autoVEC between_by_VEC (double from, double to, double by) {
	Melder_require (by != 0.0,
		U"between_by#: cannot have a step (“by”) of zero.");
	/*
		The following algorithm works for both positive and negative `by`.
	*/
	const integer numberOfElements = Melder_ifloor ((to - from) / by + 1.0);
	if (numberOfElements < 1)
		return autoVEC ();
	const double spaceNeeded = (numberOfElements - 1) * by;
	const double spaceOnEdges = (to - from) - spaceNeeded;
	const double first = from + 0.5 * spaceOnEdges;
	autoVEC result = raw_VEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = first + (double) (i - 1) * by;
	return result;
}

/*@praat
	assert between_count# (0, 10, 5) = { 1, 3, 5, 7, 9 }
@*/
autoVEC between_count_VEC (double from, double to, integer count) {
	Melder_require (count >= 0,
		U"between_count#: cannot have fewer than zero elements.");
	if (count < 1)
		return autoVEC ();
	const double by = (to - from) / count;
	autoVEC result = raw_VEC (count);
	for (integer i = 1; i <= count; i ++)
		result [i] = from + (double) (i - 0.5) * by;
	return result;
}

void mul_VEC_out (VECVU const& target, constVECVU const& vec, constMATVU const& mat) noexcept {
	Melder_assert (mat.nrow == vec.size);
	Melder_assert (target.size == mat.ncol);
	if ((true)) {
		for (integer icol = 1; icol <= mat.ncol; icol ++) {
			target [icol] = 0.0;
			for (integer irow = 1; irow <= mat.nrow; irow ++)
				target [icol] += vec [irow] * mat [irow] [icol];
		}
	} else if ((false)) {
		for (integer icol = 1; icol <= mat.ncol; icol ++) {
			PAIRWISE_SUM (longdouble, sum, integer, vec.size,
				const double *px = & vec [1];
				const double *py = & mat [1] [icol],
				(longdouble) *px * (longdouble) *py,
				(px += vec.stride, py += mat.rowStride)   // this goes way beyond the confines of mat
			)
			target [icol] = double (sum);
		}
	}
}

autoVEC mul_VEC (constVECVU const& vec, constMATVU const& mat) {
	autoVEC result = raw_VEC (mat.ncol);
	mul_VEC_out (result.get(), vec, mat);
	return result;
}

void mul_VEC_out (VECVU const& target, constMATVU const& mat, constVECVU const& vec) noexcept {
	Melder_assert (vec.size == mat.ncol);
	Melder_assert (target.size == mat.nrow);
	for (integer i = 1; i <= mat.nrow; i ++) {
		if ((false)) {
			target [i] = 0.0;
			for (integer j = 1; j <= vec.size; j ++)
				target [i] += mat [i] [j] * vec [j];
		} else {
			target [i] = NUMinner (mat.row (i), vec);
		}
	}
}

autoVEC mul_VEC (constMATVU const& mat, constVECVU const& vec) {
	autoVEC result = raw_VEC (mat.nrow);
	mul_VEC_out (result.all(), mat, vec);
	return result;
}

void power_VEC_out (VECVU const& target, constVECVU const& vec, double power) {
	if (power == 2.0) {
		for (integer i = 1; i <= target.size; i ++)
			target [i] = vec [i] * vec [i];
	} else if (power < 0.0) {
		if (power == -1.0) {
			for (integer i = 1; i <= target.size; i ++) {
				Melder_require (vec [i] != 0.0,
					U"Cannot raise zero to a negative power.");
				target [i] = 1.0 / vec [i];
			}
		} else if (power == -2.0) {
			for (integer i = 1; i <= target.size; i ++) {
				Melder_require (vec [i] != 0.0,
					U"Cannot raise zero to a negative power.");
				target [i] = 1.0 / (vec [i] * vec [i]);
			}
		} else {
			for (integer i = 1; i <= target.size; i ++) {
				Melder_require (vec [i] != 0.0,
					U"Cannot raise zero to a negative power.");
				target [i] = pow (vec [i], power);
			}
		}
	} else {
		for (integer i = 1; i <= target.size; i ++)
			target [i] = pow (vec [i], power);
	}
}

void to_INTVEC_out (INTVECVU const& x) noexcept {
	for (integer i = 1; i <= x.size; i ++)
		x [i] = i;
}

autoVEC to_VEC (double to) {
	const integer numberOfElements = Melder_ifloor (to);
	autoVEC result = raw_VEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = (double) i;
	return result;
}
autoINTVEC to_INTVEC (integer to) {
	const integer numberOfElements = to;
	autoINTVEC result = raw_INTVEC (numberOfElements);
	for (integer i = 1; i <= numberOfElements; i ++)
		result [i] = i;
	return result;
}

/* End of file VEC.cpp */
