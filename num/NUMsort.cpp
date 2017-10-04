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

#define MACRO_NUMsort(DataType, dataExpression, CounterType, sizeExpression) \
	{/* scope */ \
		DataType *_x = dataExpression; \
		CounterType _n = sizeExpression; \
		if (_n < 2) return;   /* Already sorted. */ \
		/* This `n < 2` step is absent from Press et al.'s implementation,      */ \
		/* which will therefore not terminate on `if (_r == 1)`.                */ \
		/* Knuth's initial assumption is now fulfilled: n >= 2.                 */ \
		if (_n == 2) { \
			if (_x [1] > _x [2]) { DataType _min = _x [2]; _x [2] = _x [1]; _x [1] = _min; } \
		} else if (_n <= 44) { \
			for (CounterType _i = 1; _i < _n; _i ++) { \
				DataType _min = _x [_i]; \
				CounterType _pmin = _i; \
				for (CounterType _j = _i + 1; _j <= _n; _j ++) if (_x [_j] < _min) { \
					_min = _x [_j]; \
					_pmin = _j; \
				} \
				_x [_pmin] = _x [_i]; \
				_x [_i] = _min; \
			} \
		} else { \
			CounterType _l = (_n >> 1) + 1; \
			CounterType _r = _n; \
			for (;;) { \
				DataType _k; \
				if (_l > 1) { \
					_l --; \
					_k = _x [_l]; \
				} else /* _l == 1 */ { \
					_k = _x [_r]; \
					_x [_r] = _x [1]; \
					_r --; \
					if (_r == 1) { _x [1] = _k; return; } \
				} \
				CounterType _j = _l; \
				CounterType _i; \
				for (;;) { \
					_i = _j; \
					_j = _j << 1; \
					if (_j > _r) break; \
					if (_j < _r && _x [_j] < _x [_j + 1]) _j ++; \
					if (_k >= _x [_j]) break; \
					_x [_i] = _x [_j]; \
				} \
				_x [_i] = _k; \
			} \
		} \
	}

void NUMsort_d (integer n, double a []) {
	MACRO_NUMsort (double, a, integer, n)
}

void NUMsort_i (integer n, int a []) {
	MACRO_NUMsort (int, a, integer, n)
}

void NUMsort_integer (integer n, integer a []) {
	MACRO_NUMsort (integer, a, integer, n)
}

void NUMsort_str (integer n, char32 *a []) {
	integer l, r, j, i;
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

void NUMsort_p (integer n, void *a [], int (*compare) (const void *, const void *)) {
	integer l, r, j, i;
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

double NUMquantile (integer n, double a [], double factor) {
	double place = factor * n + 0.5;
	integer left = (integer) floor (place);
	if (n < 1) return 0.0;
	if (n == 1) return a [1];
	if (left < 1) left = 1;
	if (left >= n) left = n - 1;
	if (a [left + 1] == a [left]) return a [left];
	return a [left] + (place - left) * (a [left + 1] - a [left]);
}

/* End of file NUMsort.cpp */
