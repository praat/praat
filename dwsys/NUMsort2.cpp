/* NUMsort.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
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

#include "NUM2.h"
#include "melder.h"

/*
	NUMsort2 uses heapsort to sort the second array in parallel with the first one.

	Algorithm follows p. 145 and 642 in:
	Donald E. Knuth (1998): The art of computer programming. Third edition. Vol. 3: sorting and searching.
		Boston: Addison-Wesley, printed may 2002.
	Modification: there is no distinction between record and key and
	    Floyd's optimization (page 642) is used.
*/

void MATrankColumns (MAT m, integer cb, integer ce) {
	Melder_assert (cb > 0 && cb <= m.ncol);
	Melder_assert (ce > 0 && ce <= m.ncol);
	Melder_assert (cb <= ce);
	autoVEC v = raw_VEC (m.nrow);
	autoINTVEC index = raw_INTVEC (m.nrow);

	for (integer j = cb; j <= ce; j ++) {
		v.all() <<= m.column (j);
		for (integer i = 1; i <= m.nrow; i ++)
			index [i] = i;
		NUMsortTogether (v.get(), index.get());
		VECrankSorted (v.get());
		for (integer i = 1; i <= m.nrow; i ++)
			m [index [i]] [j] = v [i];
	}
}

template <class T>
void NUMindexx (const T a[], integer n, integer index[], int (*compare) (void *, void *)) {
	T min;
	for (integer j = 1; j <= n; j ++)
		index [j] = j;

	if (n < 2)
		return;   // Already sorted
	if (n == 2) {
		if (COMPARELT (a [2], a [1])) {
			index [1] = 2;
			index [2] = 1;
		}
		return;
	}
	if (n <= 12) {
		for (integer i = 1; i < n; i ++) {
			integer imin = i;
			min = a [index [imin]];
			for (integer j = i + 1; j <= n; j ++) {
				if (COMPARELT (a[index [j]], min)) {
					imin = j;
					min = a [index [j]];
				}
			}
			const integer ii = index [imin];
			index [imin] = index [i];
			index [i] = ii;
		}
		return;
	}
	// H1
	integer l = n / 2 + 1, r = n;
	for (;;) { // H2
		integer k, i;
		if (l > 1) {
			l --;
			k = index [l];
		} else { // l == 1
			k = index [r];
			index [r] = index [1];
			r --;
			if (r == 1) {
				index [1] = k;
				break;
			}
		}
		// H3
		integer j = l;
		for (;;) {
			// H4
			i = j;
			j *= 2;
			if (j > r)
				break;

			if (j < r && COMPARELT (a [index [j]], a [index [j + 1]]))
				j++; // H5

			index [i] = index [j]; // H7
		}
		for (;;) { // H8' 
			j = i;
			i = j >> 1;
			// H9'
			if (j == l || COMPARELT (a [k], a [index [i]])) {
				index [j] = k;
				break;
			}
			index[j] = index[i];
		}
	}
}


#define MACRO_NUMindex(TYPE) \
{ \
	Melder_assert (v.size == index.size); \
	integer l, r, i, j, ii, k, imin; \
	TYPE min; \
	to_INTVEC_out (index); \
	if (v.size < 2) \
		return;   /* Already sorted. */ \
	if (v.size == 2) { \
		if (COMPARELT (v [2], v [1])) { \
			index [1] = 2; \
			index [2] = 1; \
		} \
		return; \
	} \
	if (v.size <= 12) { \
		for (i = 1; i < v.size; i ++) { \
			imin = i; \
			min = v [index [imin]]; \
			for (j = i + 1; j <= v.size; j ++) { \
				if (COMPARELT (v [index [j]], min)) { \
					imin = j; \
					min = v [index [j]]; \
				} \
			} \
			ii = index [imin]; \
			index [imin] = index [i]; \
			index [i] = ii; \
		} \
		return; \
	} \
	/* H1 */ \
	l = v.size / 2 + 1; \
	r = v.size; \
	for (;;) { /* H2 */ \
		if (l > 1) { \
			l --; \
			k = index [l]; \
		} else { /* l == 1 */ \
			k = index [r]; \
			index [r] = index [1]; \
			r --; \
			if (r == 1) { \
				index [1] = k; \
				break; \
			} \
		} \
		/* H3 */ \
		j = l; \
		for (;;) { \
			/* H4 */ \
			i = j; \
			j *= 2; \
			if (j > r) \
				break; \
			if (j < r && COMPARELT (v [index [j]], v [index [j + 1]])) \
				j ++; /* H5 */ \
			index [i] = index [j]; /* H7 */ \
		} \
		for (;;) {  /*H8' */ \
			j = i; \
			i = j >> 1; \
			/* H9' */ \
			if (j == l || COMPARELT (v [k], v[index [i]])) { \
				index [j] = k; \
				break; \
			} \
			index [j] = index [i]; \
		} \
	} \
}

#define COMPARELT(x,y) ((x) < (y))

void INTVECindex (INTVEC const& index, constVEC const& v)
MACRO_NUMindex (double)

//void NUMindexx (const double a[], integer n, integer index[])
//MACRO_NUMindex (double, n)


#undef COMPARELT
#define COMPARELT(x,y) (Melder_cmp (x,y) <  0)
//void NUMindexx_s (char32 **a, integer n, integer index[])
void INTVECindex (INTVEC const& index, constSTRVEC const& v)
MACRO_NUMindex (const char32_t *)


#undef COMPARELT
#undef MACRO_INDEXX

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
	itmp.all() <<= iv1;
	for (integer j = 1; j <= a.size; j ++)
		iv1 [j] = itmp [index [j]];
	itmp.all() <<= iv2;
	for (integer j = 1; j <= a.size; j ++)
		iv2 [j] = itmp [index [j]];
}

/* End of file NUMsort.cpp */
