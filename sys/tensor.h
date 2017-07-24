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

#include "NUM.h"

struct numvec {
	double *at;
	long size;
};

struct nummat {
	double **at;
	long nrow, ncol;
};

inline static double numvec_sum (numvec x) {
	double sum = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		sum += x.at [i];
	}
	return sum;
}

inline static double numvec_mean (numvec x) {
	if (x.size == 0) return NUMundefined;
	double sum = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		sum += x.at [i];
	}
	return sum / x.size;
}

inline static double numvec_stdev (numvec x) {
	if (x.size < 2) return NUMundefined;
	double sum = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		sum += x.at [i];
	}
	double mean = sum / x.size;
	double sum2 = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		double diff = x.at [i] - mean;
		sum2 += diff * diff;
	}
	return sqrt (sum2 / (x.size - 1));
}

inline static double numvec_center (numvec x) {
	double weightedSumOfIndexes = 0.0, sumOfWeights = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		weightedSumOfIndexes += i * x.at [i];
		sumOfWeights += x.at [i];
	}
	return weightedSumOfIndexes / sumOfWeights;
}

inline static double numvec_inner (numvec x, numvec y) {
	if (x.size != y.size) Melder_throw (U"inner: vector sizes are unequal (", x.size, U" and ", y.size, U")");
	double result = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		result += x.at [i] * y.at [i];
	}
	return result;
}



/* End of file tensor.h */
#endif
