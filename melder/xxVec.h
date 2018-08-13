#ifndef _xxVec_h_
#define _xxVec_h_
/* xxVec.h
 *
 * Copyright (C) 2017,2018 Paul Boersma
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

void sum_mean_scalar (numvec x, double *p_sum, double *p_mean) noexcept;
void sum_mean_sumsq_variance_stdev_scalar (numvec x, double *p_sum, double *p_mean, double *p_sumsq, double *p_variance, double *p_stdev) noexcept;
void sum_mean_sumsq_variance_stdev_scalar (nummat x, integer columnNumber, double *p_sum, double *p_mean, double *p_sumsq, double *p_variance, double *p_stdev) noexcept;

inline static double sum_scalar (numvec x) noexcept {
	integer n = x.size;
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? 0.0 : n == 1 ? x [1] : x [1] + x [2];
		if (n <= 4) return n == 3 ?
			(double) ((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3]) :
			(double) (((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4]));
		if (n <= 6) return n == 5 ?
			(double) (((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3]) + ((longdouble) x [4] + (longdouble) x [5])) :
			(double) (((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3]) + ((longdouble) x [4] + (longdouble) x [5] + (longdouble) x [6]));
		return n == 7 ?
			(double) ((((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4])) + ((longdouble) x [5] + (longdouble) x [6] + (longdouble) x [7])) :
			(double) ((((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4])) + (((longdouble) x [5] + (longdouble) x [6]) + ((longdouble) x [7] + (longdouble) x [8])));
	}
	double sum;
	sum_mean_scalar (x, & sum, nullptr);
	return sum;
}

inline static double mean_scalar (numvec x) noexcept {
	integer n = x.size;
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? undefined : n == 1 ? x [1] : (double) (0.5 * ((longdouble) x [1] + (longdouble) x [2]));
		if (n <= 4) return n == 3 ?
			(double) ((1.0 / (longdouble) 3.0) * ((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3])) :
			(double) (0.25 * (((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4])));
		if (n <= 6) return n == 5 ?
			(double) ((1.0 / (longdouble) 5.0) * (((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3]) + ((longdouble) x [4] + (longdouble) x [5]))) :
			(double) ((1.0 / (longdouble) 6.0) * (((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3]) + ((longdouble) x [4] + (longdouble) x [5] + (longdouble) x [6])));
		return n == 7 ?
			(double) ((1.0 / (longdouble) 7.0) * ((((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4])) + ((longdouble) x [5] + (longdouble) x [6] + (longdouble) x [7]))) :
			(double) (0.125 * ((((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4])) + (((longdouble) x [5] + (longdouble) x [6]) + ((longdouble) x [7] + (longdouble) x [8]))));
	}
	double mean;
	sum_mean_scalar (x, nullptr, & mean);
	return mean;
}

double sumsq_scalar (numvec x) noexcept;
double variance_scalar (numvec x) noexcept;
double stdev_scalar (numvec x) noexcept;
double norm_scalar (numvec x, double power) noexcept;
double center_scalar (numvec x) noexcept;

double _inner_scalar (numvec x, numvec y);
inline static double inner_scalar (numvec x, numvec y) {
	integer n = x.size;
	if (y.size != n) return undefined;
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? 0.0 : n == 1 ? x [1] * y [1] : (double) ((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2]);
		if (n <= 4) return n == 3 ?
			(double) ((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2] + (longdouble) x [3] * (longdouble) y [3]) :
			(double) (((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2]) + ((longdouble) x [3] * (longdouble) y [3] + (longdouble) x [4] * (longdouble) y [4]));
		if (n <= 6) return n == 5 ?
			(double) (((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2] + (longdouble) x [3] * (longdouble) y [3]) + ((longdouble) x [4] * (longdouble) y [4] + (longdouble) x [5] * (longdouble) y [5])) :
			(double) (((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2] + (longdouble) x [3] * (longdouble) y [3]) + ((longdouble) x [4] * (longdouble) y [4] + (longdouble) x [5] * (longdouble) y [5] + (longdouble) x [6] * (longdouble) y [6]));
		return n == 7 ?
			(double) ((((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2]) + ((longdouble) x [3] * (longdouble) y [3] + (longdouble) x [4] * (longdouble) y [4])) + ((longdouble) x [5] * (longdouble) y [5] + (longdouble) x [6] * (longdouble) y [6] + (longdouble) x [7] * (longdouble) y [7])) :
			(double) ((((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2]) + ((longdouble) x [3] * (longdouble) y [3] + (longdouble) x [4] * (longdouble) y [4])) + (((longdouble) x [5] * (longdouble) y [5] + (longdouble) x [6] * (longdouble) y [6]) + ((longdouble) x [7] * (longdouble) y [7] + (longdouble) x [8] * (longdouble) y [8])));
	}
	return _inner_scalar (x, y);
}

autonumvec copy_numvec (numvec x);

inline static bool equal_numvec (numvec x, numvec y) {
	integer n = x.size;
	if (y.size != n) return false;
	for (integer i = 1; i <= x.size; i ++) {
		if (x [i] != y [i]) {
			return false;
		}
	}
	return true;
}

inline static void numvec_copyElements_nocheck (numvec from, numvec to) {
	for (integer i = 1; i <= from.size; i ++) {
		to [i] = from [i];
	}
}

inline static void nummat_copyElements_nocheck (nummat from, nummat to) {
	for (integer irow = 1; irow <= from.nrow; irow ++) {
		for (integer icol = 1; icol <= from.ncol; icol ++) {
			to [irow] [icol] = from [irow] [icol];
		}
	}
}

inline static autonumvec add_numvec (numvec x, numvec y) {
	if (x.size != y.size) return autonumvec { };
	autonumvec result (x.size, kTensorInitializationType::RAW);
	for (integer i = 1; i <= x.size; i ++) {
		result [i] = x [i] + y [i];
	}
	return result;
}
inline static autonummat add_nummat (nummat x, nummat y) {
	if (x.nrow != y.nrow || x.ncol != y.ncol) return autonummat { };
	autonummat result (x.nrow, x.ncol, kTensorInitializationType::RAW);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol] + y [irow] [icol];
		}
	}
	return result;
}
inline static autonumvec sub_numvec (numvec x, numvec y) {
	if (x.size != y.size) return autonumvec { };
	autonumvec result (x.size, kTensorInitializationType::RAW);
	for (integer i = 1; i <= x.size; i ++) {
		result [i] = x [i] - y [i];
	}
	return result;
}
inline static autonummat sub_nummat (nummat x, nummat y) {
	if (x.nrow != y.nrow || x.ncol != y.ncol) return autonummat { };
	autonummat result (x.nrow, x.ncol, kTensorInitializationType::RAW);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol] - y [irow] [icol];
		}
	}
	return result;
}

inline static autonumvec to_numvec (integer to) {
	autonumvec result (to, kTensorInitializationType::RAW);
	for (integer i = 1; i <= to; i ++) {
		result [i] = (double) i;
	}
	return result;
}

autonumvec mul_numvec (numvec x, nummat y);
autonumvec mul_numvec (nummat x, numvec y);

autonummat copy_nummat (nummat x);

inline static numvec as_numvec (nummat x) {
	return numvec (x [1], x.nrow * x.ncol);
}

inline static double norm_scalar (nummat x, double power) noexcept {
	return norm_scalar (as_numvec (x), power);
}

autonummat outer_nummat (numvec x, numvec y);

autonummat peaks_nummat (numvec x, bool includeEdges, int interpolate, bool sortByHeight);

void numvec_sort (numvec x);

/* End of file xxVec.h */
#endif
