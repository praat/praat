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


/* Temporary repair to make Praat compilable again */

namespace num {
	template <class T>
	T NUMselect (struct vector <T> v, integer k) {
		return v[1];
	}
	
	template <class T>
	T NUMquantile_e (struct vector <T>  v, double factor) {
		return v[1];
	}
	
	template <class T>
	T NUMget_kth (struct vector <T>  v, integer k) {
		return v[1];
	}
	
	template <class T>
	T NUMmin_e (struct vector <T> v) {
		T min = v[1];
		for (integer i = 2; i <= v.size; i++)
			if (v[i] < min)
				min = v[i];
		return min;
	}
	
	template <class T>
	T NUMmax_e (struct vector <T> v) {
		T max = v[1];
		for (integer i = 2; i <= v.size; i++)
			if (v[i] > max)
				max = v[i];
		return max;
	}
}

void timeMedian (void);

#endif /* _NUMselect_h_ */
