#ifndef _NUMselect_h_
#define _NUMselect_h_
/* NUMselect.h
 *
 * Copyright (C) 2025 David Weenink
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
#include "../external/num/median_of_ninthers.h"

namespace num {
	template <class T>
	T& NUMselect (vector<T> const& v, integer kth_asIfSorted) {
		adaptiveQuickselect (v.asArgumentToFunctionThatExpectsZeroBasedArray (), kth_asIfSorted - 1, v.size);
		return v [kth_asIfSorted];
	}
	
	template <class T>
	T& NUMget_kth (vector<T> const& v, integer kth_asIfSorted) {
		num::NUMselect (v, kth_asIfSorted);
		return v[kth_asIfSorted];
	}
	
	template <class T>
	T& NUMmin_e (struct vector <T> v) {
		if (v.size == 0)
			Melder_throw (U"NUMmin_e: cannot determine the minimum of an empty ExtendedReal vector.");
		if (v.size == 1)
			return v [1];
		T *min = & v[1];
		for (integer i = 2; i <= v.size; i++)
			if (v[i] < *min)
				min = & v[i];
		return *min;
	}
	
	template <class T>
	T& NUMmax_e (struct vector <T> v) {
		if (v.size == 0)
			Melder_throw (U"max_e: cannot determine the maximum of an empty vector.");
		if (v.size == 1)
			return v [1];
		T *max = & v[1];
		for (integer i = 2; i <= v.size; i++)
			if (v[i] > *max)
				max = & v[i];
		return *max;
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

void timeMedian (void);

#endif /* _NUMselect_h_ */
