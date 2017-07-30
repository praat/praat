/* NUMsort.c
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
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

/*
 * pb 2002/06/24 removed NUMselect
 * pb 2007/08/10 NUMsort_strW
 * pb 2008/01/21 double
 * pb 2011/03/29 C++
 */

#include "melder.h"

/*
	NUMsort uses heapsort.
	J.W.J. Williams (1964). 'xx.'
		Communications of the Association for Computing Machinery 7: 347-348.
	R.W. Floyd (1964). 'xx.'
		Communications of the Association for Computing Machinery 7: 701.

	Algorithm follows p. 145 of:
	Donald E. Knuth (1998): The art of computer programming. Third edition. Vol. 3: sorting and searching.
		Boston: Addison-Wesley.
	Modification: there is no distinction between record and key.
*/

#define MACRO_NUMsort(TYPE)  { \
	long l, r, j, i; \
	TYPE k; \
	if (n < 2) return;   /* Already sorted. */ \
	/* This n<2 step is absent from Press et al.'s implementation, */ \
	/* which will therefore not terminate on if(--ir==1). */ \
	/* Knuth's initial assumption is now fulfilled: n >= 2. */ \
if (n == 2) { \
	if (a [1] > a [2]) { TYPE min = a [2]; a [2] = a [1]; a [1] = min; } \
	return; \
} \
if (n <= 12) { \
	for (i = 1; i < n; i ++) { \
		TYPE min = a [i]; \
		long pmin = i; \
		for (j = i + 1; j <= n; j ++) if (a [j] < min) { \
			min = a [j]; \
			pmin = j; \
		} \
		a [pmin] = a [i]; \
		a [i] = min; \
	} \
	return; \
} \
	l = (n >> 1) + 1; \
	r = n; \
	for (;;) { \
		if (l > 1) { \
			l --; \
			k = a [l]; \
		} else /* l == 1 */ { \
			k = a [r]; \
			a [r] = a [1]; \
			r --; \
			if (r == 1) { a [1] = k; return; } \
		} \
		j = l; \
		for (;;) { \
			i = j; \
			j = j << 1; \
			if (j > r) break; \
			if (j < r && a [j] < a [j + 1]) j ++; \
			if (k >= a [j]) break; \
			a [i] = a [j]; \
		} \
		a [i] = k; \
	} \
}

void NUMsort_d (long n, double a [])
	MACRO_NUMsort (double)

void NUMsort_i (long n, int a [])
	MACRO_NUMsort (int)

void NUMsort_l (long n, long a [])
	MACRO_NUMsort (long)

void NUMsort_str (long n, char32 *a []) {
	long l, r, j, i;
	char32 *k;
	if (n < 2) return;
	l = (n >> 1) + 1;
	r = n;
	for (;;) {
		if (l > 1) {
			l --;
			k = a [l];
		} else { 
			k = a [r];
			a [r] = a [1];
			r --;
			if (r == 1) { a [1] = k; return; }
		}
		j = l;
		for (;;) {
			i = j;
			j = j << 1;
			if (j > r) break;
			if (j < r && str32cmp (a [j], a [j + 1]) < 0) j ++;
			if (str32cmp (k, a [j]) >= 0) break;
			a [i] = a [j];
		}
		a [i] = k;
	}
}

void NUMsort_p (long n, void *a [], int (*compare) (const void *, const void *)) {
	long l, r, j, i;
	void *k;
	if (n < 2) return;
	l = (n >> 1) + 1;
	r = n;
	for (;;) {
		if (l > 1) {
			l --;
			k = a [l];
		} else { 
			k = a [r];
			a [r] = a [1];
			r --;
			if (r == 1) { a [1] = k; return; }
		}
		j = l;
		for (;;) {
			i = j;
			j = j << 1;
			if (j > r) break;
			if (j < r && compare (a [j], a [j + 1]) < 0) j ++;
			if (compare (k, a [j]) >= 0) break;
			a [i] = a [j];
		}
		a [i] = k;
	}
}

double NUMquantile (long n, double a [], double factor) {
	double place = factor * n + 0.5;
	long left = (long) floor (place);
	if (n < 1) return 0.0;
	if (n == 1) return a [1];
	if (left < 1) left = 1;
	if (left >= n) left = n - 1;
	if (a [left + 1] == a [left]) return a [left];
	return a [left] + (place - left) * (a [left + 1] - a [left]);
}

/* End of file NUMsort.cpp */
