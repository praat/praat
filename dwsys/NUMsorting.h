#ifndef _NUMsorting_h_
#define _NUMsorting_h_
/* NUMsorting.h
 *
 * Copyright (C) 2022 David Weenink
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

#include "melder.h"
#include "STRVEC.h"

#include "strings_sorting_enums.h"

/*
	Uses heapsort to sort the second array in parallel with the first one.

	Algorithm follows p. 145 and 642 in:
	Donald E. Knuth (1998): The art of computer programming. Third edition. Vol. 3: sorting and searching.
		Boston: Addison-Wesley, printed may 2002.
	Modification: there is no distinction between record and key and
		Floyd's optimization (page 642) is used.
	Sorts (inplace) an array a [1..n] into ascending order using the Heapsort algorithm,
	while making the corresponding rearrangement of the companion
	array b [1..n]. A characteristic of heapsort is that it does not conserve
	the order of equals: e.g., the array 3,1,1,2 will be sorted as 1,1,2,3 and
	it may occur that the first 1 after sorting came from position 3 and the second 
	1 came from position 2.
*/
template<typename T1, typename T2>
void NUMsortTogether (vector<T1> a, vector<T2> b) {
	Melder_assert (a.size == b.size);
	if (a.size < 2) return;   /* Already sorted. */
	if (a.size == 2) {
		if (a [2] < a [1]) {
			std::swap (a [1], a [2]);
			std::swap (b [1], b [2]);
		}
		return;
	}
	if (a.size <= 12) {
		for (integer i = 1; i < a.size; i ++) {
			T1 min = a [i];
			integer imin = i;
			for (integer j = i + 1; j <= a.size; j ++)
				if (a [j] < min) {
					min = a [j];
					imin = j;
				}
			a [imin] = a [i];
			a [i] = min;
			std::swap (b [imin], b [i]);
		}
		return;
	}
	/* H1 Initialize */
	integer l = (a.size >> 1) + 1;
	integer r = a.size;
	for (;;) {
		T1	k;
		T2 kb;
		/* H2 Decrease */
		if (l > 1) {
			l --;
			k = a [l];
			kb = b [l];
		} else /* l == 1 */ {
			k = a [r];
			kb = b [r];
			a [r] = a [1];
			b [r] = b [1];
			r --;
			if (r == 1) {
				a [1] = k;
				b [1] = kb;
				return;
			}
		}
		/* H3 */
		integer i, j = l;
		for (;;) { /* H4 */
			i = j;
			j = j << 1;
			if (j > r) break;
			if (j < r && a [j] < a [j + 1]) j ++; /* H5 */
			/* if (k >= a [j]) break; H6 */
			a [i] = a [j];
			b [i] = b [j]; /* H7 */
		}
		/* a [i] = k; b [i] = kb; H8 */
		for (;;) { /*H8' */
			j = i;
			i = j >> 1;
			/* H9' */
			if (j == l || k <= a [i]) {
				a [j] = k;
				b [j] = kb;
				break;
			}
			a [j] = a [i];
			b [j] = b [i];
		}
	}
}

void VECsort3_inplace (VEC const& a, INTVEC const& iv1, INTVEC const& iv2, bool descending); // TODO template
/* Sort a together with iv1  and iv2 */


/*
	Precondition: index must have been initialised with (1, 2, 3, ... index.size)
	The interpretation of the index is as follows:
	v[index[i]] <= v[index[j]] if i < j,
	therefore if index [i] is 2, 4, 10, ...
	the order is: v[2] <= v[4] <= v[20] <= ...
*/
inline void INTVECindex_inout (INTVEC index, constINTVEC const& v) {
	std::stable_sort (index.begin(), index.end(), [& v] (integer ix, integer iy)
	 {
		return v [ix] < v [iy];
	 });
}

inline void INTVECindex_inout (INTVEC index, constVEC const& v) {
	std::stable_sort (index.begin(), index.end(), [& v] (integer ix, integer iy)
	{
		return v [ix] < v [iy];
	});
}

inline void INTVECindex_inout (INTVEC index, STRVEC const& v) {
	std::stable_sort (index.begin(), index.end(), [& v] (integer ix, integer iy)
	{
		return Melder_cmp (v [ix], v [iy]) < 0;
	});
}

inline void INTVECindex_inout (INTVEC index, constSTRVEC const& v) {
	std::stable_sort (index.begin(), index.end(), [& v] (integer ix, integer iy)
	{
		return Melder_cmp (v [ix], v [iy]) < 0;
	});
}

inline void INTVECindex_numberAware_inout (INTVEC const& index, constSTRVEC const& v) {
	std::stable_sort (index.begin(), index.end(), [& v] (integer ix, integer iy)
	{
		return str32coll_numberAware (v [ix], v [iy]) < 0;
	});
}

inline autoINTVEC newINTVECindex (constVEC const& a) {
	autoINTVEC result = to_INTVEC (a.size);
	INTVECindex_inout (result.get(), a);
	return result;
}

inline autoINTVEC newINTVECindex (constINTVEC const& a) {
	autoINTVEC result = to_INTVEC (a.size);
	INTVECindex_inout (result.get(), a);
	return result;
}

inline autoINTVEC newINTVECindex (constSTRVEC const& a) {
	autoINTVEC result = to_INTVEC (a.size);
	INTVECindex_inout (result.get(), a);
	return result;
}

void MATrankColumns (MAT m, integer cb, integer ce);

/* rank:
 *  Replace content of sorted array by rank number, including midranking of ties.
 *  E.g. The elements {10, 20.1, 20.1, 20.1, 20.1, 30} in array a will be replaced
 *  by {1, 3.5, 3.5, 3.5, 3.5, 4}, respectively. *
 */

inline void VECrankSorted (VECVU const& a) {
	integer jt, j = 1;
	while (j < a.size) {
		for (jt = j + 1; jt <= a.size && a [jt] == a [j]; jt ++) {}
		double rank = (j + jt - 1) * 0.5;
		for (integer i = j; i <= jt - 1; i ++)
			a [i] = rank;
		j = jt;
	}
	if (j == a.size)
		a [a.size] = a.size;
}


template <typename T>
automatrix<T> matrix_sortRows (constmatrix<T> const& m, constINTVECVU const& columnNumbers) {
	automatrix<T> result = newmatrixraw<T> (m.nrow, m.ncol);
	autoINTVEC rows = to_INTVEC (m.nrow);
	std::sort (rows.begin(), rows.end(),
		[&] (integer rowi, integer rowj) -> bool {
			for (integer icol = 1; icol <= columnNumbers.size; icol ++) {
				const integer col = columnNumbers [icol];
				if (m [rowi] [col] < m [rowj] [col])
					return true;
				if (m [rowi] [col] > m [rowj] [col])
					return false;
			}
			return false;
		}
	);
	for (integer irow = 1; irow <= m.nrow; irow ++)
		result.row (irow)  <<=  m.row (rows [irow]);
	return result;
}

inline autoINTMAT sortRows_INTMAT (constINTMAT const& m, constINTVECVU const& columnNumbers) {
	return matrix_sortRows<integer> (m, columnNumbers);
}

inline autoMAT sortRows_MAT (constMAT const& m, constINTVECVU const& columnNumbers) {
	return matrix_sortRows<double> (m, columnNumbers);
}

#endif /* _NUMsorting_h_ */
