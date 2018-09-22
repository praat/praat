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
			9.3,1.26,0.21, 0.10,0.42,0.70, 1.17,1.97,5.32
			20: 0.71   30: 0.54   40: 0.40   50: 0.30   60: 0.30   70: 0.26   80: 0.23   90: 0.23
			1200: 0.10   1300: 0.12   1400: 0.16   1500: 0.27   1600: 0.33   1700: 0.40
		Speed if always explicit loop:
			1.7,0.40,0.23, 0.22,0.37,0.64, 1.11,1.93,5.11
			20: 0.26   30: 0.29   40: 0.24   50: 0.26   60: 0.24   70: 0.25   80: 0.24   90: 0.24
			1200: 0.22   1300: 0.22   1400: 0.22   1500: 0.28   1600: 0.33   1700: 0.35
		Combined:
			2.4,0.58,0.20, 0.10,0.42,0.68, 1.11,1.93,5.02
			20: 0.30   30: 0.29   40: 0.24   50: 0.26   60: 0.23   70: 0.26   80: 0.23   90: 0.23
			1200: 0.10   1300: 0.12   1400: 0.16   1500: 0.29   1600: 0.34   1700: 0.37
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

autoVEC VECmul (const constVEC& vec, const constMAT& mat) noexcept {
	autoVEC result = VECraw (mat.ncol);
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

autoVEC VECmul (const constMAT& mat, const constVEC& vec) noexcept {
	autoVEC result = VECraw (mat.nrow);
	VECmul_preallocated (result.get(), mat, vec);
	return result;
}

/* End of file VEC.cpp */
