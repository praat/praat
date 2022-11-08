/* NUMsorting.cpp
 *
 * Copyright (C) 1993-2022 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20030121 Initial version
 djmw 20030627 Removed bug in MACRO_NUMindex
 djmw 20120305 Latest modification
*/

#include "NUMsorting.h"
#include "enums_getText.h"
#include "strings_sorting_enums.h"
#include "enums_getValue.h"
#include "strings_sorting_enums.h"

void MATrankColumns (MAT m, integer cb, integer ce) {
	Melder_assert (cb > 0 && cb <= m.ncol);
	Melder_assert (ce > 0 && ce <= m.ncol);
	Melder_assert (cb <= ce);
	autoVEC v = raw_VEC (m.nrow);
	autoINTVEC index = raw_INTVEC (m.nrow);

	for (integer j = cb; j <= ce; j ++) {
		v.all()  <<=  m.column (j);
		for (integer i = 1; i <= m.nrow; i ++)
			index [i] = i;
		NUMsortTogether (v.get(), index.get());
		VECrankSorted (v.get());
		for (integer i = 1; i <= m.nrow; i ++)
			m [index [i]] [j] = v [i];
	}
}

void VECsort3_inplace (VEC const& a, INTVEC const& iv1, INTVEC const& iv2, bool descending) {
	Melder_assert (a.size == iv1.size && a.size == iv2.size);
	if (a.size == 1)
		return;
	autoVEC atmp = copy_VEC (a);
	autoINTVEC index = newINTVECindex (atmp.get());
	if (descending)
		for (integer j = 1; j <= a.size / 2; j ++)
			std::swap (index [j], index [a.size - j + 1]);

	for (integer j = 1; j <= a.size; j ++)
		a [j] = atmp [index [j]];
	autoINTVEC itmp = raw_INTVEC (a.size);
	itmp.all()  <<=  iv1;
	for (integer j = 1; j <= a.size; j ++)
		iv1 [j] = itmp [index [j]];
	itmp.all()  <<=  iv2;
	for (integer j = 1; j <= a.size; j ++)
		iv2 [j] = itmp [index [j]];
}

autoINTMAT sortRows_INTMAT (constINTMAT mat, integer icol) { // TODO test me
	try {
		Melder_require (icol > 0 && icol <= mat.ncol,
			U"The column index is not valid.");
		autoINTVEC colvals = raw_INTVEC (mat.nrow);
		for (integer irow = 1; irow <= mat.nrow; irow ++)
			colvals [irow] = mat [irow] [icol];
		autoINTVEC index = newINTVECindex (colvals.get());
		autoINTMAT result = raw_INTMAT (mat.nrow, mat.ncol);
		for (integer irow = 1; irow <= mat.nrow; irow ++)
			result [index [irow]]  <<= mat [irow];
		return result;
	} catch (MelderError) {
		Melder_throw (U"Matrix not sorted.");
	}
}

/* End of file NUMsort.cpp */
