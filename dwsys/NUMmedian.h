#ifndef _NUMmedian_h_
#define _NUMmedian_h_
/* NUMmedian.h
 *
 * Copyright (C) 2024-2025 David Weenink
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

#include "median_of_ninthers.h"
#include "melder.h"
#include "NUM2.h"
#include "ExtendedReal.h"

/*
	Select the k-th largest element from the vector.
	The algorithm is from:
	Alexandrescu, Andrei. Fast deterministic selection, Symposium on experimental algorithms (SEA 2017), pp. 1-18.
*/

namespace num {
	

	template <typename T>
	T& NUMselect (vector<T> const& v, integer kth_asIfSorted) {
		adaptiveQuickselect (v.asArgumentToFunctionThatExpectsZeroBasedArray (), kth_asIfSorted - 1, v.size);
		return v [kth_asIfSorted];
	}
	
	template <typename T>
	T& NUMget_kth (vector<T> const& v, integer kth_asIfSorted) {
		num::NUMselect (v, kth_asIfSorted);
		return v[kth_asIfSorted];
	}

	template <typename T>
	T& NUMmin_e (vector<T> v) {
		if (v.size == 0)
			Melder_throw (U"min_e: cannot determine the minimum of an empty ExtendedReal vector.");
		if (v.size == 1)
			return v [1];
		T *t = & v [1];
		for (integer i = 2; i <=v.size; i ++ )
			if (v [i] < *t)
				t = & v [i];
		return *t;
	}

	template <typename T>
	T& NUMmax_e (vector<T> v) {
		if (v.size == 0)
			Melder_throw (U"max_e: cannot determine the maximum of an empty vector.");
		if (v.size == 1)
			return v [1];
		T *t = & v [1];
		for (integer i = 2; i <=v.size; i ++ )
			if (v [i] > *t)
				t = & v [i];
		return *t;
	}
	
	template <typename T>
	T NUMquantile_e (vector<T> const& v, double factor) {
		if (v.size < 1)
			Melder_throw (U"quantile_e: cannot determine quantile from an emplty vector.");
		if (v.size == 1)
			return v [1];
		const double place = factor * v.size + 0.5;
		const integer left = Melder_clipped (1_integer, Melder_ifloor (place), v.size - 1);
		trace (U"left:", left, U" size:", v.size);
		num::NUMselect (v, left);
		vector<T> highPart = v.part (left + 1, v.size);
		const T min = num::NUMmin_e (highPart);
		const double slope = min - v [left];
		if (slope == 0.0)
			return v [left];   // or a [left + 1], which is the same
		return v [left] + (place - left) * slope;
	}
	
}

/* Only for timing the algorithms */

void timeMedian (void);


#endif /* _NUMmedian_h_ */
