/* NUMsort.cpp
 *
 * Copyright (C) 1993-2012 David Weenink
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

void NUMrankColumns (MAT m, integer cb, integer ce) {
	Melder_assert (cb > 0 && cb <= m.ncol);
	Melder_assert (ce > 0 && ce <= m.ncol);
	Melder_assert (cb <= ce);
	autoVEC v = newVECraw (m.nrow);
	autoINTVEC index = newINTVECraw (m.nrow);

	for (integer j = cb; j <= ce; j ++) {
		v.all() <<= m.column (j);
		for (integer i = 1; i <= m.nrow; i ++)
			index [i] = i;
		NUMsortTogether (v.get(), index.get());
		NUMrank (v.get());
		for (integer i = 1; i <= m.nrow; i ++)
			m [index [i]] [j] = v [i];
	}
}

template <class T>
void NUMindexx (const T a[], integer n, integer index[], int (*compare) (void *, void *)) {
	integer ii, imin;
	T min;
	for (integer j = 1; j <= n; j ++) {
		index [j] = j;
	}
	if (n < 2) return;   // Already sorted
	if (n == 2) {
		if (COMPARELT (a [2], a [1])) {
			index [1] = 2; index [2] = 1;
		}
		return;
	} 
	if (n <= 12) {
		for (integer i = 1; i < n; i ++) {
			imin = i;
			min = a [index [imin]];
			for (integer j = i + 1; j <= n; j ++) {
				if (COMPARELT (a[index [j]], min)) {
					imin = j;
					min = a [index [j]];
				}
			}
			ii = index [imin]; index [imin] = index [i]; index [i] = ii;
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
				index [1] = k; break;
			}
		}
		// H3 
		integer j = l;
		for (;;) {
			// H4
			i = j;
			j *= 2;
			if (j > r) {
				break;
			}
			if (j < r && COMPARELT (a [index [j]], a [index [j + 1]])) {
				j++; // H5
			}
			index [i] = index [j]; // H7
		}
		for (;;) { // H8' 
			j = i;
			i = j >> 1;
			// H9'
			if (j == l || COMPARELT (a [k], a [index [i]])) {
				index [j] = k; break;
			}
			index[j] = index[i];
		}
	}
}


#define MACRO_NUMindex(TYPE,n) \
{\
	integer l, r, j, i, ii, k, imin; \
	TYPE min; \
	autoINTVEC index = newINTVECraw (n); \
	for (j = 1; j <= n; j ++) index[j] = j;	\
	if (n < 2) return index;   /* Already sorted. */ \
	if (n == 2) \
	{ \
		if (COMPARELT (a [2], a [1])) \
		{\
			index [1] = 2; index [2] = 1; \
		} \
		return index; \
	} \
	if (n <= 12) \
	{ \
		for (i = 1; i < n; i ++) \
		{ \
			imin = i; \
			min = a [index [imin]]; \
			for (j = i + 1; j <= n; j ++) \
			{\
				if (COMPARELT (a [index [j]], min))\
				{ \
					imin = j; \
					min = a [index [j]]; \
				} \
			} \
			ii = index [imin]; index [imin] = index [i]; index [i] = ii; \
		} \
		return index; \
	} \
	/* H1 */\
	l = n / 2 + 1; \
	r = n; \
	for (;;) /* H2 */\
	{ \
		if (l > 1) \
		{ \
			l --; \
			k = index[l]; \
		} \
		else /* l == 1 */ \
		{ \
			k = index [r]; \
			index [r] = index [1]; \
			r --; \
			if (r == 1) \
			{ \
				index [1] = k; break; \
			} \
		} \
		/* H3 */ \
		j = l; \
		for (;;) \
		{ \
			/* H4 */ \
			i = j; \
			j *= 2; \
			if (j > r) break; \
			if (j < r && COMPARELT (a [index [j]], a [index [j + 1]])) j ++; /* H5 */\
			index [i] = index [j]; /* H7 */\
		} \
		for (;;)  /*H8' */\
		{\
			j = i; \
			i = j >> 1; \
			/* H9' */ \
			if (j == l || COMPARELT (a[k], a[index[i]])) \
			{ \
				index [j] = k; break; \
			} \
			index [j] = index [i]; \
		}\
	} \
	return index; \
}

#define COMPARELT(x,y) ((x) < (y))

autoINTVEC NUMindexx (constVEC a)
MACRO_NUMindex (double, a.size)

//void NUMindexx (const double a[], integer n, integer index[])
//MACRO_NUMindex (double, n)


#undef COMPARELT
#define COMPARELT(x,y) (Melder_cmp (x,y) <  0)
//void NUMindexx_s (char32 **a, integer n, integer index[])
autoINTVEC NUMindexx_s (constSTRVEC a)
MACRO_NUMindex (const char32_t *, a.size)


#undef COMPARELT
#undef MACRO_INDEXX

void NUMsort3 (VEC a, INTVEC iv1, INTVEC iv2, bool descending) {
	Melder_assert (a.size == iv1.size && a.size == iv2.size);
	if (a.size == 1)
		return;
	autoVEC atmp = newVECcopy (a);
	autoINTVEC index = NUMindexx (atmp.get());
	if (descending) {
		for (integer j = 1; j <= a.size / 2; j ++) {
			integer tmp = index [j];
			index [j] = index [a.size - j + 1];
			index [a.size - j + 1] = tmp;
		}
	}
	for (integer j = 1; j <= a.size; j ++)
		a [j] = atmp [index [j]];
	autoINTVEC itmp = newINTVECraw (a.size);
	itmp.all() <<= iv1;
	for (integer j = 1; j <= a.size; j ++)
		iv1 [j] = itmp [index [j]];
	itmp.all() <<= iv2;
	for (integer j = 1; j <= a.size; j ++)
		iv2 [j] = itmp [index [j]];
}

/* End of file NUMsort.cpp */
