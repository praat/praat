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

void sum_mean_sumsq_variance_stdev_scalar (numvec x, real *p_sum, real *p_mean, real *p_sumsq, real *p_variance, real *p_stdev) noexcept;

inline static double sum_scalar (numvec x) {
	if (x.size == 0) return 0.0;
	if (Melder_debug >= 48 && Melder_debug <= 50) {
		real sum;
		sum_mean_sumsq_variance_stdev_scalar (x, & sum, nullptr, nullptr, nullptr, nullptr);
		return sum;
	} else {
		real80 offset = (real80) x [1];
		real80 sumOfDifferences = 0.0;
		for (integer i = 2; i <= x.size; i ++) {
			sumOfDifferences += (real80) (x [i] - offset);
		}
		real80 sum = sumOfDifferences + offset * x.size;
		return (real) sum;
	}
}

inline static double mean_scalar (numvec x) {
	if (x.size == 0) return undefined;
	if (Melder_debug >= 48 && Melder_debug <= 50) {
		real mean;
		sum_mean_sumsq_variance_stdev_scalar (x, nullptr, & mean, nullptr, nullptr, nullptr);
		return mean;
	} else {
		real80 offset = (real80) x [1];
		real80 sumOfDifferences = 0.0;
		for (integer i = 2; i <= x.size; i ++) {
			sumOfDifferences += (real80) (x [i] - offset);
		}
		real80 mean = offset + sumOfDifferences / x.size;
		return (real) mean;
	}
}

double sumsq_scalar (numvec x) noexcept;
double variance_scalar (numvec x) noexcept;
double stdev_scalar (numvec x) noexcept;
double center_scalar (numvec x) noexcept;

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
