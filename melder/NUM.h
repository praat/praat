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

extern double NUMinner_ (constVEC const& x, constVEC const& y) noexcept;
extern void NUM_sum_mean (constVEC const& x, double *out_sum, double *out_mean) noexcept;
extern void NUM_sum_mean_sumsq_variance_stdev (constVEC const& x,
		double *out_sum, double *out_mean,
		double *out_sumsq, double *out_variance, double *out_stdev) noexcept;
extern void NUM_sum_mean_sumsq_variance_stdev (constMAT const& x, integer columnNumber,
		double *out_sum, double *out_mean,
		double *out_sumsq, double *out_variance, double *out_stdev) noexcept;

inline double NUMsum (constVEC const& x) noexcept {
	integer const n = x.size;
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? 0.0 : n == 1 ? x [1] : x [1] + x [2];
		if (n <= 4) return n == 3 ?
			double (longdouble (x [1]) + longdouble (x [2]) + longdouble (x [3])) :
			double ((longdouble (x [1]) + longdouble (x [2])) + (longdouble (x [3]) + longdouble (x [4])));
		if (n <= 6) return n == 5 ?
			double ((longdouble (x [1]) + longdouble (x [2]) + longdouble (x [3])) + (longdouble (x [4]) + longdouble (x [5]))) :
			double ((longdouble (x [1]) + longdouble (x [2]) + longdouble (x [3])) + (longdouble (x [4]) + longdouble (x [5]) + longdouble (x [6])));
		return n == 7 ?
			double (((longdouble (x [1]) + longdouble (x [2])) + (longdouble (x [3]) + longdouble (x [4]))) + (longdouble (x [5]) + longdouble (x [6]) + longdouble (x [7]))) :
			double (((longdouble (x [1]) + longdouble (x [2])) + (longdouble (x [3]) + longdouble (x [4]))) + ((longdouble (x [5]) + longdouble (x [6])) + (longdouble (x [7]) + longdouble (x [8]))));
	}
	double sum;
	NUM_sum_mean (x, & sum, nullptr);
	return sum;
}

/*
	From here on, the functions appear in alphabetical order.
*/

extern double NUMcenterOfGravity (constVEC const& x) noexcept;

extern double NUMcolumnMean (constMAT const& x, integer columnNumber) noexcept;

extern double NUMcolumnSum (constMAT const& x, integer columnNumber) noexcept;

inline bool NUMdefined (constMAT const& x) noexcept {
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			if (isundef (x [irow] [icol]))
				return false;
	return true;
}

template <typename T>
bool NUMequal (constvector<T> const& x, constvector<T> const& y) noexcept {
	integer const n = x.size;
	if (y.size != n)
		return false;
	for (integer i = 1; i <= n; i ++) {
		if (x [i] != y [i])
			return false;
	}
	return true;
}
template <typename T>
bool NUMequal (vector<T> const& x, constvector<T> const& y) noexcept {
	return NUMequal (constvector<T> (x), y);
}
template <typename T>
bool NUMequal (constvector<T> const& x, vector<T> const& y) noexcept {
	return NUMequal (x, constvector<T> (y));
}
template <typename T>
bool NUMequal (vector<T> const& x, vector<T> const& y) noexcept {
	return NUMequal (constvector<T> (x), constvector<T> (y));
}

template <typename T>
bool NUMequal (constmatrix<T> const& x, constmatrix<T> const& y) noexcept {
	return NUMequal (asvector (x), asvector (y));
}
template <typename T>
bool NUMequal (matrix<T> const& x, constmatrix<T> const& y) noexcept {
	return NUMequal (constmatrix<T> (x), y);
}
template <typename T>
bool NUMequal (constmatrix<T> const& x, matrix<T> const& y) noexcept {
	return NUMequal (x, constmatrix<T> (y));
}
template <typename T>
bool NUMequal (matrix<T> const& x, matrix<T> const& y) noexcept {
	return NUMequal (constmatrix<T> (x), constmatrix<T> (y));
}

inline bool NUMequal (constSTRVEC x, constSTRVEC y) noexcept {
	integer const n = x.size;
	if (y.size != n)
		return false;
	for (integer i = 1; i <= n; i ++) {
		if (! Melder_equ (x [i], y [i]))
			return false;
	}
	return true;
}

inline double NUMextremum (constVEC const& vec) noexcept {
	double extremum = 0.0;
	for (integer i = 1; i <= vec.size; i ++)
		if (fabs (vec [i]) > extremum) extremum = fabs (vec [i]);
	return extremum;
}

inline double NUMextremum (constMAT const& mat) noexcept {
	return NUMextremum (asvector (mat));
}

inline double NUMinner (constVEC const& x, constVEC const& y) noexcept {
	integer const n = x.size;
	Melder_assert (y.size == n);
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? 0.0 : n == 1 ? x [1] * y [1] : double (longdouble (x [1]) * longdouble (y [1]) + longdouble (x [2]) * longdouble (y [2]));
		if (n <= 4) return n == 3 ?
			double (longdouble (x [1]) * longdouble (y [1]) + longdouble (x [2]) * longdouble (y [2]) + longdouble (x [3]) * longdouble (y [3])) :
			double ((longdouble (x [1]) * longdouble (y [1]) + longdouble (x [2]) * longdouble (y [2])) + (longdouble (x [3]) * longdouble (y [3]) + longdouble (x [4]) * longdouble (y [4])));
		if (n <= 6) return n == 5 ?
			double ((longdouble (x [1]) * longdouble (y [1]) + longdouble (x [2]) * longdouble (y [2]) + longdouble (x [3]) * longdouble (y [3])) +
					(longdouble (x [4]) * longdouble (y [4]) + longdouble (x [5]) * longdouble (y [5]))) :
			double ((longdouble (x [1]) * longdouble (y [1]) + longdouble (x [2]) * longdouble (y [2]) + longdouble (x [3]) * longdouble (y [3])) +
					(longdouble (x [4]) * longdouble (y [4]) + longdouble (x [5]) * longdouble (y [5]) + longdouble (x [6]) * longdouble (y [6])));
		return n == 7 ?
			double (((longdouble (x [1]) * longdouble (y [1]) + longdouble (x [2]) * longdouble (y [2])) + (longdouble (x [3]) * longdouble (y [3]) + longdouble (x [4]) * longdouble (y [4]))) +
					(longdouble (x [5]) * longdouble (y [5]) + longdouble (x [6]) * longdouble (y [6]) + longdouble (x [7]) * longdouble (y [7]))) :
			double (((longdouble (x [1]) * longdouble (y [1]) + longdouble (x [2]) * longdouble (y [2])) + (longdouble (x [3]) * longdouble (y [3]) + longdouble (x [4]) * longdouble (y [4]))) +
					((longdouble (x [5]) * longdouble (y [5]) + longdouble (x [6]) * longdouble (y [6])) + (longdouble (x [7]) * longdouble (y [7]) + longdouble (x [8]) * longdouble (y [8]))));
	}
	return NUMinner_ (x, y);
}

template <typename T>
bool NUMisEmpty (vector<T> const& x) noexcept {
	return x.size == 0;   // note: testing on x.at is incorrect, because the capacity may be large when the size is 0
}
template <typename T>
bool NUMisEmpty (vectorview<T> const& x) noexcept {
	return x.size == 0;   // note: testing on x.at is incorrect, because the capacity may be large when the size is 0
}
template <typename T>
bool NUMisEmpty (constvector<T> const& x) noexcept {
	return x.size == 0;   // note: testing on x.at is incorrect, because the capacity may be large when the size is 0
}
template <typename T>
bool NUMisEmpty (constvectorview<T> const& x) noexcept {
	return x.size == 0;   // note: testing on x.at is incorrect, because the capacity may be large when the size is 0
}

template <typename T>
bool NUMisEmpty (matrix<T> const& x) noexcept {
	integer const numberOfCells = x.nrow * x.ncol;
	return numberOfCells == 0;   // note: a matrix with 0 rows and 6 columns is a valid empty matrix, to which e.g. a row can be added
}
template <typename T>
bool NUMisEmpty (constmatrix<T> const& x) noexcept {
	integer const numberOfCells = x.nrow * x.ncol;
	return numberOfCells == 0;   // note: a matrix with 0 rows and 6 columns is a valid empty matrix, to which e.g. a row can be added
}
template <typename T>
bool NUMisEmpty (matrixview<T> const& x) noexcept {
	integer const numberOfCells = x.nrow * x.ncol;
	return numberOfCells == 0;   // note: a matrix with 0 rows and 6 columns is a valid empty matrix, to which e.g. a row can be added
}
template <typename T>
bool NUMisEmpty (constmatrixview<T> const& x) noexcept {
	integer const numberOfCells = x.nrow * x.ncol;
	return numberOfCells == 0;   // note: a matrix with 0 rows and 6 columns is a valid empty matrix, to which e.g. a row can be added
}

inline bool NUMisSymmetric (constMAT const& x) noexcept {
	if (x.nrow != x.ncol) return false;
	integer const n = x.nrow;
	for (integer irow = 1; irow <= n; irow ++)
		for (integer icol = irow + 1; icol < n; icol ++)
			if (x [irow] [icol] != x [icol] [irow])
				return false;
	return true;
}

inline integer NUMlength (conststring32 str) {
	return str ? str32len (str) : 0;
}

inline double NUMlog2 (double x) {
	return log (x) * NUMlog2e;
}

inline double NUMmax (const constVECVU& vec) {
	if (NUMisEmpty (vec)) return undefined;
	double maximum = vec [1];
	for (integer i = 2; i <= vec.size; i ++) {
		const double value = vec [i];
		if (value > maximum) maximum = value;
	}
	return maximum;
}

inline double NUMmean (constVEC const& x) noexcept {
	integer const n = x.size;
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? undefined : n == 1 ? x [1] : (double) (0.5 * ((longdouble) x [1] + (longdouble) x [2]));
		if (n <= 4) return n == 3 ?
			double ((1.0 / 3.0L) * (longdouble (x [1]) + longdouble (x [2]) + longdouble (x [3]))) :
			double (0.25 * ((longdouble (x [1]) + longdouble (x [2])) + (longdouble (x [3]) + longdouble (x [4]))));
		if (n <= 6) return n == 5 ?
			double ((1.0 / 5.0L) * ((longdouble (x [1]) + longdouble (x [2]) + longdouble (x [3])) + (longdouble (x [4]) + longdouble (x [5])))) :
			double ((1.0 / 6.0L) * ((longdouble (x [1]) + longdouble (x [2]) + longdouble (x [3])) + (longdouble (x [4]) + longdouble (x [5]) + longdouble (x [6]))));
		return n == 7 ?
			double ((1.0 / 7.0L) * (((longdouble (x [1]) + longdouble (x [2])) + (longdouble (x [3]) + longdouble (x [4]))) + (longdouble (x [5]) + longdouble (x [6]) + longdouble (x [7])))) :
			double (0.125 * (((longdouble (x [1]) + longdouble (x [2])) + (longdouble (x [3]) + longdouble (x [4]))) + ((longdouble (x [5]) + longdouble (x [6])) + (longdouble (x [7]) + longdouble (x [8])))));
	}
	double mean;
	NUM_sum_mean (x, nullptr, & mean);
	return mean;
}

inline double NUMmin (const constVECVU& vec) {
	if (NUMisEmpty (vec)) return undefined;
	double minimum = vec [1];
	for (integer i = 2; i <= vec.size; i ++) {
		const double value = vec [i];
		if (value < minimum) minimum = value;
	}
	return minimum;
}

double NUMnorm (constVEC const& x, double power) noexcept;

inline double NUMnorm (constMAT const& x, double power) noexcept {
	return NUMnorm (asvector (x), power);
}

integer NUMnumberOfTokens (conststring32 str) noexcept;

/*
	Return zero for non-positive base.
*/
inline double NUMpow (double base, double exponent) {
	return base <= 0.0 ? 0.0 : pow (base, exponent);
}

inline double NUMrowSum (constMAT const& x, integer rowNumber) noexcept {
	return NUMsum (x.row (rowNumber));
}

inline double NUMsqrt (double x) {
	#if defined (_WIN32)
		if (x < 0.0) return undefined;
	#endif
	return sqrt (x);
}

extern double NUMstdev (constVEC const& x) noexcept;

inline double NUMsum (constMAT const& x) noexcept {
	return NUMsum (asvector (x));
}

extern double NUMsumsq (constVEC const& x) noexcept;

extern double NUMvariance (constVEC const& x) noexcept;

/* End of file NUM.h */

