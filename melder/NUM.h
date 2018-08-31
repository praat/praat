#pragma once
/* NUM.h
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

/*
	The following functions are called in inline functions,
	so they have to be declared first.
*/

double NUMinner_ (constVEC x, constVEC y);
void NUM_sum_mean (constVEC x, double *p_sum, double *p_mean) noexcept;
void NUM_sum_mean_sumsq_variance_stdev (constVEC x,
		double *p_sum, double *p_mean, double *p_sumsq, double *p_variance, double *p_stdev) noexcept;
void NUM_sum_mean_sumsq_variance_stdev (constMAT x, integer columnNumber,
		double *p_sum, double *p_mean, double *p_sumsq, double *p_variance, double *p_stdev) noexcept;

inline static double NUMsum (constVEC x) noexcept {
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
	NUM_sum_mean (x, & sum, nullptr);
	return sum;
}

/*
	From here on, the functions appear in alphabetical order.
*/

double NUMcenterOfGravity (constVEC x) noexcept;

double NUMcolumnSum (constMAT x, integer columnNumber);

inline static bool NUMequal (constVEC x, constVEC y) {
	integer n = x.size;
	if (y.size != n)
		return false;
	for (integer i = 1; i <= n; i ++) {
		if (x [i] != y [i])
			return false;
	}
	return true;
}

inline static bool NUMequal (constMAT x, constMAT y) {
	return NUMequal (asVEC (x), asVEC (y));
}

inline static bool NUMequal (constSTRVEC x, constSTRVEC y) {
	integer n = x.size;
	if (y.size != n)
		return false;
	for (integer i = 1; i <= n; i ++) {
		if (! Melder_equ (x [i], y [i]))
			return false;
	}
	return true;
}

inline static double NUMextremum (constVEC vec) {
	double extremum = 0.0;
	for (integer i = 1; i <= vec.size; i ++)
		if (fabs (vec [i]) > extremum) extremum = fabs (vec [i]);
	return extremum;
}

inline static double NUMextremum (constMAT mat) {
	return NUMextremum (asVEC (mat));
}

inline static double NUMinner (constVEC x, constVEC y) {
	integer n = x.size;
	Melder_require (y.size == n,
		U"inner(): the two vectors should have equal length, not, ", x.size, U" and ", y.size, U".");
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
	return NUMinner_ (x, y);
}

inline static integer NUMlength (conststring32 str) {
	return str32len (str);
}

inline static double NUMlog2 (double x) {
	return log (x) * NUMlog2e;
}

inline static double NUMmean (constVEC x) noexcept {
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
	NUM_sum_mean (x, nullptr, & mean);
	return mean;
}

double NUMnorm (constVEC x, double power) noexcept;

inline static double NUMnorm (constMAT x, double power) noexcept {
	return NUMnorm (asVEC (x), power);
}

integer NUMnumberOfTokens (conststring32 str);

/*
	Return zero for non-positive base.
*/
inline static double NUMpow (double base, double exponent) {
	return base <= 0.0 ? 0.0 : pow (base, exponent);
}

inline static double NUMrowSum (constMAT x, integer rowNumber) noexcept {
	Melder_assert (rowNumber > 0 && rowNumber <= x.nrow);
	return NUMsum ({ x [rowNumber], x.ncol });
}

inline static double NUMsqrt (double x) {
	#if defined (_WIN32)
		if (x < 0.0) return undefined;
	#endif
	return sqrt (x);
}

double NUMstdev (constVEC x) noexcept;

inline static double NUMsum (constMAT x) noexcept {
	return NUMsum (asVEC (x));
}

double NUMsumsq (constVEC x) noexcept;

double NUMvariance (constVEC x) noexcept;

/* End of file NUM.h */

