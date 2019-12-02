/* NUMsort.cpp
 *
 * Copyright (C) 1992-2011,2015,2017-2019 Paul Boersma
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

void VECsort_inplace (VECVU const& x) noexcept {
	std::sort (x.begin(), x.end(),
		[] (double first, double last) {
			return first < last;
		}
	);
}

void INTVECsort_inplace (INTVECVU const& x) noexcept {
	std::sort (x.begin(), x.end(),
		[] (double first, double last) {
			return first < last;
		}
	);
}

void STRVECsort_inplace (STRVEC const& array) {
	std::sort (array.begin(), array.end(),
		[] (conststring32 first, conststring32 last) {
			return str32cmp (first, last) < 0;
		}
	);
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

double NUMquantile (constVECVU const& a, double factor) noexcept {
	double place = factor * a.size + 0.5;
	integer left = (integer) floor (place);
	if (a.size < 1) return 0.0;
	if (a.size == 1) return a [1];
	if (left < 1) left = 1;
	if (left >= a.size) left = a.size - 1;
	if (a [left + 1] == a [left]) return a [left];
	return a [left] + (place - left) * (a [left + 1] - a [left]);
}

/* End of file NUMsort.cpp */
