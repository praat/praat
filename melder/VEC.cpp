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

#ifdef macintosh
	#include "macport_on.h"
	#include <Accelerate/Accelerate.h>
	#include "macport_off.h"
#endif

#if defined (macintosh)
void VECadd_macfast_ (const VEC& target, const constVEC& x, const constVEC& y) noexcept {
	integer n = target.size;
	vDSP_vaddD (& x [1], 1, & y [1], 1, & target [1], 1, integer_to_uinteger (n));
	/*
		Speed if always vDSP_vaddD:
			//9.3,1.26,0.21, 0.10,0.42,0.70, 1.17,1.97,5.32
		Speed if always explicit loop:
			//1.7,0.40,0.23, 0.22,0.37,0.64, 1.11,1.93,5.11
		Combined:
			2.4,0.50,0.20, 0.13,0.44,0.71, 1.17,2.02,2.40
	*/
}
void VECVUadd_macfast_ (const VECVU& target, const constVECVU& x, const constVECVU& y) noexcept {
	integer n = target.size;
	vDSP_vaddD (& x [1], x.stride, & y [1], y.stride, & target [1], target.stride, integer_to_uinteger (n));
	/*
		Speed if always vDSP_vaddD:
			//9.3,1.26,0.21, 0.10,0.42,0.70, 1.17,1.97,5.32
		Speed if always explicit loop:
			1.6,0.43,0.23, 0.22,0.37,0.65, 1.10,1.98,2.28
		Combined:
			6.1,1.19,0.23, 0.13,0.41,0.70, 1.19,2.02,2.45
	*/
}
#endif

void VECmul_preallocated (const VEC& target, const constVEC& vec, const constMAT& mat) noexcept {
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
				(px += 1, py += mat.ncol)   // this goes way beyond the confines of mat
			)
			target [icol] = double (sum);
		}
	}
}

autoVEC newVECmul (const constVEC& vec, const constMAT& mat) noexcept {
	autoVEC result = newVECraw (mat.ncol);
	VECmul_preallocated (result.get(), vec, mat);
	return result;
}

void VECmul_preallocated (const VEC& target, const constMAT& mat, const constVEC& vec) noexcept {
	Melder_assert (vec.size == mat.ncol);
	Melder_assert (target.size == mat.nrow);
	for (integer i = 1; i <= mat.nrow; i ++) {
		if ((false)) {
			target [i] = 0.0;
			for (integer j = 1; j <= vec.size; j ++)
				target [i] += mat [i] [j] * vec [j];
		} else {
			target [i] = NUMinner (constVEC (& mat [i] [1] - 1, mat.ncol), vec);
		}
	}
}

autoVEC newVECmul (const constMAT& mat, const constVEC& vec) noexcept {
	autoVEC result = newVECraw (mat.nrow);
	VECmul_preallocated (result.get(), mat, vec);
	return result;
}

/* End of file VEC.cpp */
