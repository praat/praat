#pragma once
/* NUM.h
 *
 * Copyright (C) 2017-2019 Paul Boersma
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
	const integer numberOfCells = x.nrow * x.ncol;
	return numberOfCells == 0;   // note: a matrix with 0 rows and 6 columns is a valid empty matrix, to which e.g. a row can be added
}
template <typename T>
bool NUMisEmpty (constmatrix<T> const& x) noexcept {
	const integer numberOfCells = x.nrow * x.ncol;
	return numberOfCells == 0;   // note: a matrix with 0 rows and 6 columns is a valid empty matrix, to which e.g. a row can be added
}
template <typename T>
bool NUMisEmpty (matrixview<T> const& x) noexcept {
	const integer numberOfCells = x.nrow * x.ncol;
	return numberOfCells == 0;   // note: a matrix with 0 rows and 6 columns is a valid empty matrix, to which e.g. a row can be added
}
template <typename T>
bool NUMisEmpty (constmatrixview<T> const& x) noexcept {
	const integer numberOfCells = x.nrow * x.ncol;
	return numberOfCells == 0;   // note: a matrix with 0 rows and 6 columns is a valid empty matrix, to which e.g. a row can be added
}

inline MelderRealRange NUMextrema (const constVECVU& vec) {
	if (NUMisEmpty (vec))
		return { undefined, undefined };
	double minimum = vec [1], maximum = minimum;
	for (integer i = 2; i <= vec.size; i ++) {
		const double value = vec [i];
		if (value < minimum) minimum = value;
		if (value > maximum) maximum = value;
	}
	return { minimum, maximum };
}
inline MelderRealRange NUMextrema (const constMATVU& mat) {
	if (NUMisEmpty (mat))
		return { undefined, undefined };
	double minimum = mat [1] [1], maximum = minimum;
	for (integer irow = 1; irow <= mat.nrow; irow ++) {
		for (integer icol = 1; icol <= mat.ncol; icol ++) {
			const double value = mat [irow] [icol];
			if (value < minimum)
				minimum = value;
			if (value > maximum)
				maximum = value;
		}
	}
	return { minimum, maximum };
}
inline MelderIntegerRange NUMextrema (const constINTVECVU& vec) {
	if (NUMisEmpty (vec))
		return { INTEGER_MIN, INTEGER_MAX };
	integer minimum = vec [1], maximum = minimum;
	for (integer i = 2; i <= vec.size; i ++) {
		const integer value = vec [i];
		if (value < minimum)
			minimum = value;
		if (value > maximum)
			maximum = value;
	}
	return { minimum, maximum };
}

/*
	From here on, the functions appear in alphabetical order.
*/

extern double NUMcenterOfGravity (constVECVU const& x) noexcept;

inline bool NUMdefined (constVECVU const& vec) noexcept {
	for (integer i = 1; i <= vec.size; i ++)
		if (isundef (vec [i]))
			return false;
	return true;
}
inline bool NUMdefined (constMATVU const& mat) noexcept {
	for (integer irow = 1; irow <= mat.nrow; irow ++)
		for (integer icol = 1; icol <= mat.ncol; icol ++)
			if (isundef (mat [irow] [icol]))
				return false;
	return true;
}

inline bool NUMequal (double x, double y) {
	/*
		Any defined value is unequal to --undefined--,
		but any undefined value (inf or NaN) *is* equal to --undefined--.
		This is different from how "==" works in C, hence the following complication.
	*/
	return x == y || isundef (x) && isundef (y);
}
inline bool NUMequal (integer x, integer y) {
	return x == y;
}
inline bool NUMequal (bool x, bool y) {
	return x == y;
}
inline bool NUMequal (byte x, byte y) {
	return x == y;
}
inline bool NUMequal (dcomplex x, dcomplex y) {
	return x.real() == y.real() && x.imag() == y.imag(); // 
}
template <typename T>
bool NUMequal (constvector<T> const& x, constvector<T> const& y) noexcept {
	const integer n = x.size;
	if (y.size != n)
		return false;
	for (integer i = 1; i <= n; i ++)
		if (! NUMequal (x [i], y [i]))
			return false;
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
	const integer nrow = x.nrow, ncol = x.ncol;
	if (y.nrow != nrow || y.ncol != ncol)
		return false;
	for (integer irow = 1; irow <= nrow; irow ++)
		for (integer icol = 1; icol <= ncol; icol ++)
			if (! NUMequal (x [irow] [icol], y [irow] [icol]))
				return false;
	return true;
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
	const integer n = x.size;
	if (y.size != n)
		return false;
	for (integer i = 1; i <= n; i ++)
		if (! Melder_equ (x [i], y [i]))
			return false;
	return true;
}

inline double NUMextremum (constVECVU const& vec) noexcept {
	double extremum = 0.0;
	for (integer i = 1; i <= vec.size; i ++)
		if (fabs (vec [i]) > extremum)
			extremum = fabs (vec [i]);
	return extremum;
}
inline double NUMextremum (constMATVU const& mat) {
	MelderRealRange range = NUMextrema (mat);
	return std::max (fabs (range.min), fabs (range.max));
}

extern double NUMinner (constVECVU const& x, constVECVU const& y) noexcept;

inline bool NUMisSymmetric (constMATVU const& x) noexcept {
	if (x.nrow != x.ncol)
		return false;
	const integer n = x.nrow;
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

inline double NUMmax (constVECVU const& vec) {
	if (NUMisEmpty (vec))
		return undefined;
	double maximum = vec [1];
	for (integer i = 2; i <= vec.size; i ++) {
		const double value = vec [i];
		if (value > maximum)
			maximum = value;
	}
	return maximum;
}
inline integer NUMmax (const constINTVECVU& vec) {
	if (NUMisEmpty (vec))
		return INTEGER_MIN;
	integer maximum = vec [1];
	for (integer i = 2; i <= vec.size; i ++) {
		const integer value = vec [i];
		if (value > maximum)
			maximum = value;
	}
	return maximum;
}
inline double NUMmax (constMATVU const& mat) {
	if (NUMisEmpty (mat))
		return undefined;
	double maximum = NUMmax (mat [1]);
	for (integer irow = 2; irow <= mat.nrow; irow ++) {
		const double value = NUMmax (mat [irow]);
		if (value > maximum)
			maximum = value;
	}
	return maximum;
}

extern double NUMmean (constVECVU const& vec);
extern double NUMmean (constMATVU const& mat) noexcept;

extern MelderGaussianStats NUMmeanStdev (constVECVU const& vec) noexcept;
extern MelderGaussianStats NUMmeanStdev (constMATVU const& mat) noexcept;

inline double NUMmin (constVECVU const& vec) {
	if (NUMisEmpty (vec))
		return undefined;
	double minimum = vec [1];
	for (integer i = 2; i <= vec.size; i ++) {
		const double value = vec [i];
		if (value < minimum)
			minimum = value;
	}
	return minimum;
}
inline integer NUMmin (const constINTVECVU& vec) {
	if (NUMisEmpty (vec))
		return INTEGER_MAX;
	integer minimum = vec [1];
	for (integer i = 2; i <= vec.size; i ++) {
		const integer value = vec [i];
		if (value < minimum)
			minimum = value;
	}
	return minimum;
}
inline double NUMmin (constMATVU const& mat) {
	if (NUMisEmpty (mat))
		return undefined;
	double minimum = NUMmin (mat [1]);
	for (integer irow = 2; irow <= mat.nrow; irow ++) {
		const double value = NUMmin (mat [irow]);
		if (value < minimum)
			minimum = value;
	}
	return minimum;
}

double NUMnorm (constVECVU const& x, double power) noexcept;
double NUMnorm (constMATVU const& x, double power) noexcept;

integer NUMnumberOfTokens (conststring32 str) noexcept;

/*
	Return zero for non-positive base.
*/
inline double NUMpow (double base, double exponent) {
	return base <= 0.0 ? 0.0 : pow (base, exponent);
}

inline double NUMsqrt (double x) {
	#if defined (_WIN32)
		if (x < 0.0) return undefined;
	#endif
	return sqrt (x);
}

extern double NUMstdev (constVECVU const& vec) noexcept;
extern double NUMstdev (constMATVU const& mat) noexcept;

extern double NUMsum (constVECVU const& vec) noexcept;
extern double NUMsum (constMATVU const& mat) noexcept;

extern double NUMsum2 (constVECVU const& vec);
extern double NUMsum2 (constMATVU const& mat);

extern double NUMsumOfSquaredDifferences (constVECVU const& vec, double mean);

extern double NUMsumsq (constVECVU const& vec) noexcept;
extern double NUMsumsq (constMATVU const& mat) noexcept;

extern double NUMvariance (constVECVU const& vec) noexcept;
extern double NUMvariance (constMATVU const& mat) noexcept;

/* End of file NUM.h */
