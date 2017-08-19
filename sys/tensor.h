#ifndef _tensor_h_
#define _tensor_h_
/* tensor.h
 *
 * Copyright (C) 2017 Paul Boersma
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
#include <math.h>

inline static double sqrt_scalar (double x) {
	#if defined (_WIN32)
		if (x < 0.0) return undefined;
	#endif
	return sqrt (x);
}

void sum_mean_scalar (numvec x, real *p_sum, real *p_mean) noexcept;
void sum_mean_sumsq_variance_stdev_scalar (numvec x, real *p_sum, real *p_mean, real *p_sumsq, real *p_variance, real *p_stdev) noexcept;

inline static real sum_scalar (numvec x) noexcept {
	integer n = x.size;
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? 0.0 : n == 1 ? x [1] : x [1] + x [2];
		if (n <= 4) return n == 3 ?
			(real) ((real80) x [1] + (real80) x [2] + (real80) x [3]) :
			(real) (((real80) x [1] + (real80) x [2]) + ((real80) x [3] + (real80) x [4]));
		if (n <= 6) return n == 5 ?
			(real) (((real80) x [1] + (real80) x [2] + (real80) x [3]) + ((real80) x [4] + (real80) x [5])) :
			(real) (((real80) x [1] + (real80) x [2] + (real80) x [3]) + ((real80) x [4] + (real80) x [5] + (real80) x [6]));
		return n == 7 ?
			(real) ((((real80) x [1] + (real80) x [2]) + ((real80) x [3] + (real80) x [4])) + ((real80) x [5] + (real80) x [6] + (real80) x [7])) :
			(real) ((((real80) x [1] + (real80) x [2]) + ((real80) x [3] + (real80) x [4])) + (((real80) x [5] + (real80) x [6]) + ((real80) x [7] + (real80) x [8])));
	}
	real sum;
	sum_mean_scalar (x, & sum, nullptr);
	return sum;
}

inline static real mean_scalar (numvec x) noexcept {
	integer n = x.size;
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? undefined : n == 1 ? x [1] : (real) (0.5 * ((real80) x [1] + (real80) x [2]));
		if (n <= 4) return n == 3 ?
			(real) ((1.0 / (real80) 3.0) * ((real80) x [1] + (real80) x [2] + (real80) x [3])) :
			(real) (0.25 * (((real80) x [1] + (real80) x [2]) + ((real80) x [3] + (real80) x [4])));
		if (n <= 6) return n == 5 ?
			(real) ((1.0 / (real80) 5.0) * (((real80) x [1] + (real80) x [2] + (real80) x [3]) + ((real80) x [4] + (real80) x [5]))) :
			(real) ((1.0 / (real80) 6.0) * (((real80) x [1] + (real80) x [2] + (real80) x [3]) + ((real80) x [4] + (real80) x [5] + (real80) x [6])));
		return n == 7 ?
			(real) ((1.0 / (real80) 7.0) * ((((real80) x [1] + (real80) x [2]) + ((real80) x [3] + (real80) x [4])) + ((real80) x [5] + (real80) x [6] + (real80) x [7]))) :
			(real) (0.125 * ((((real80) x [1] + (real80) x [2]) + ((real80) x [3] + (real80) x [4])) + (((real80) x [5] + (real80) x [6]) + ((real80) x [7] + (real80) x [8]))));
	}
	real mean;
	sum_mean_scalar (x, nullptr, & mean);
	return mean;
}

real sumsq_scalar (numvec x) noexcept;
real variance_scalar (numvec x) noexcept;
real stdev_scalar (numvec x) noexcept;
real center_scalar (numvec x) noexcept;

inline static double inner_scalar (numvec x, numvec y) {
	if (x.size != y.size) return undefined;
	double result = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		result += x [i] * y [i];
	}
	return result;
}

autonumvec copy_numvec (numvec x);

inline static autonumvec add_numvec (numvec x, numvec y) {
	//if (x.size != y.size) return autonumvec { nullptr, 0, 0 };
	autonumvec result (x.size, false);
	for (long i = 1; i <= x.size; i ++) {
		result [i] = x [i] + y [i];
	}
	return result;
}

autonummat copy_nummat (nummat x);
autonummat outer_nummat (numvec x, numvec y);

autonummat peaks_nummat (numvec x, bool includeEdges, int interpolate, bool sortByHeight);

/* End of file tensor.h */
#endif
