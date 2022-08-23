/* melder_sort.cpp
 *
 * Copyright (C) 1992-2011,2015,2017-2021 Paul Boersma
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

void shuffle_VEC_inout (VECVU const& x) {
	for (integer i = 1; i < x.size; i ++)
		std::swap (x [i], x [NUMrandomInteger (i, x.size)]);
}

void shuffle_INTVEC_inout (INTVECVU const& x) {
	for (integer i = 1; i < x.size; i ++)
		std::swap (x [i], x [NUMrandomInteger (i, x.size)]);
}

void shuffle_STRVEC_inout (STRVEC const& x) {
	for (integer i = 1; i < x.size; i ++)
		std::swap (x [i], x [NUMrandomInteger (i, x.size)]);
}
autoSTRVEC shuffle_STRVEC (STRVEC const& x) {
	autoSTRVEC result = copy_STRVEC (x);
	shuffle_STRVEC_inout (result.get());
	return result;
}

void sort_VEC_inout (VEC const& x) noexcept {
	std::sort (x.begin(), x.end(),
		[] (double first, double last) {
			return first < last;
		}
	);
}

void sort_INTVEC_inout (INTVEC const& x) noexcept {
	std::sort (x.begin(), x.end(),
		[] (integer first, integer last) {
			return first < last;
		}
	);
}

void sort_STRVEC_inout (STRVEC const& array) noexcept {
	std::sort (array.begin(), array.end(),
		[] (conststring32 first, conststring32 last) {
			return str32cmp (first, last) < 0;
		}
	);
}
autoSTRVEC sort_STRVEC (STRVEC const& x) {
	autoSTRVEC result = copy_STRVEC (x);
	sort_STRVEC_inout (result.get());
	return result;
}

double NUMquantile (constVECVU const& a, double factor) noexcept {
	if (a.size < 1)
		return undefined;
	if (a.size == 1)
		return a [1];
	const double place = factor * a.size + 0.5;
	Melder_assert (a.size - 1 >= 1);
	const integer left = Melder_clipped (1_integer, Melder_ifloor (place), a.size - 1);
	const double slope = a [left + 1] - a [left];
	if (slope == 0.0)
		return a [left];   // or a [left + 1], which is the same
	return a [left] + (place - left) * slope;
}

/* End of file melder_sort.cpp */
