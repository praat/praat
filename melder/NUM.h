#pragma once
/* NUM.h
 *
 * Copyright (C) 1992-2024 Paul Boersma
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

inline bool NUMisEmpty (constSTRVEC const& x) noexcept {
	return x.size == 0;
}

extern MelderRealRange NUMextrema_e (constVECVU const& vec);
extern MelderRealRange NUMextrema_u (constVECVU const& vec) noexcept;
extern MelderRealRange NUMextrema_e (constMATVU const& mat);
extern MelderRealRange NUMextrema_u (constMATVU const& mat) noexcept;
extern MelderIntegerRange NUMextrema_e (constINTVECVU const& vec);
extern MelderIntegerRange NUMextrema_u (constINTVECVU const& vec) noexcept;

/*
	From here on, the functions appear in alphabetical order.
*/

extern double NUMcenterOfGravity (constVECVU const& x) noexcept;

inline integer NUMcountDefined (constVECVU const& vec) noexcept {
	integer result = 0;
	for (integer i = 1; i <= vec.size; i ++)
		if (isdefined (vec [i]))
			result += 1;
	return result;
}
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
	return x == y || (isundef (x) && isundef (y));
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

inline double NUMextremum_e (constVECVU const& vec) {
	if (NUMisEmpty (vec))
		Melder_throw (U"Cannot determine the extremum of an empty vector.");
	double extremum = 0.0;
	for (integer i = 1; i <= vec.size; i ++) {
		const double value = fabs (vec [i]);
		if (isundef (value))
			Melder_throw (U"Cannot determine the extremum of a vector: element ", i, U" is undefined.");
		if (value > extremum)
			extremum = value;
	}
	return extremum;
}
inline double NUMextremum_u (constVECVU const& vec) noexcept {
	if (NUMisEmpty (vec))
		return undefined;
	double extremum = 0.0;
	for (integer i = 1; i <= vec.size; i ++) {
		const double value = fabs (vec [i]);
		if (isundef (value))
			return undefined;
		if (value > extremum)
			extremum = value;
	}
	return extremum;
}
inline double NUMextremum_e (constMATVU const& mat) {
	const MelderRealRange range = NUMextrema_e (mat);
	return std::max (fabs (range.min), fabs (range.max));
}
inline double NUMextremum_u (constMATVU const& mat) {
	const MelderRealRange range = NUMextrema_u (mat);
	if (isundef (range))
		return undefined;
	return std::max (fabs (range.min), fabs (range.max));
}

inline integer NUMfindFirst (constSTRVEC const& strvec, conststring32 str) {
	for (integer i = 1; i <= strvec.size; i ++)
		if (Melder_equ (strvec [i], str))
			return i;
	return 0;
}
inline integer NUMfindFirst_caseInsensitive (constSTRVEC const& strvec, conststring32 str) {
	for (integer i = 1; i <= strvec.size; i ++)
		if (Melder_equ_caseInsensitive (strvec [i], str))
			return i;
	return 0;
}
inline integer NUMfindLast (constSTRVEC const& strvec, conststring32 str) {
	for (integer i = strvec.size; i >= 1; i --)
		if (Melder_equ (strvec [i], str))
			return i;
	return 0;
}
inline integer NUMfindLast_caseInsensitive (constSTRVEC const& strvec, conststring32 str) {
	for (integer i = strvec.size; i >= 1; i --)
		if (Melder_equ (strvec [i], str))
			return i;
	return 0;
}

extern double NUMinner (constVECVU const& x, constVECVU const& y) noexcept;

inline bool NUMisSquare (constMATVU const& x) noexcept {
	return x.nrow == x.ncol;
}

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

inline double NUMlength (conststring32 str) noexcept {
	return double (Melder_length (str));
}

inline double NUMlog2 (double x) noexcept {
	return log (x) * NUMlog2e;
}

extern double  NUMmin_e                 (constVECVU    const& vec);            // throw  if empty or undefined element
extern double  NUMmin_u                 (constVECVU    const& vec) noexcept;   // undef  if empty or undefined element
extern double  NUMmin_removeUndefined_e (constVECVU    const& vec);            // throw  if no defined elements
extern double  NUMmin_removeUndefined_u (constVECVU    const& vec) noexcept;   // undef  if no defined elements
extern integer NUMmin_e                 (constINTVECVU const& vec);            // throw  if empty
extern integer NUMmin_u                 (constINTVECVU const& vec) noexcept;   // intmax if empty
extern double  NUMmin_e                 (constMATVU    const& mat);            // throw  if empty or undefined element
extern double  NUMmin_u                 (constMATVU    const& mat) noexcept;   // undef  if empty or undefined element
extern double  NUMmin_removeUndefined_e (constMATVU    const& mat);            // throw  if no defined elements
extern double  NUMmin_removeUndefined_u (constMATVU    const& mat) noexcept;   // undef  if no defined elements

extern double  NUMmax_e                 (constVECVU    const& vec);            // throw  if empty or undefined element
extern double  NUMmax_u                 (constVECVU    const& vec) noexcept;   // undef  if empty or undefined element
extern double  NUMmax_removeUndefined_e (constVECVU    const& vec);            // throw  if no defined elements
extern double  NUMmax_removeUndefined_u (constVECVU    const& vec) noexcept;   // undef  if no defined elements
extern integer NUMmax_e                 (constINTVECVU const& vec);            // throw  if empty
extern integer NUMmax_u                 (constINTVECVU const& vec) noexcept;   // intmin if empty
extern double  NUMmax_e                 (constMATVU    const& mat);            // throw  if empty or undefined element
extern double  NUMmax_u                 (constMATVU    const& mat) noexcept;   // undef  if empty or undefined element
extern double  NUMmax_removeUndefined_e (constMATVU    const& mat);            // throw  if no defined elements
extern double  NUMmax_removeUndefined_u (constMATVU    const& mat) noexcept;   // undef  if no defined elements

extern double NUMminimumLength (constSTRVEC const& x) noexcept;
extern double NUMmaximumLength (constSTRVEC const& x) noexcept;

extern double NUMmean (constVECVU const& vec) noexcept;
extern double NUMmean (constMATVU const& mat) noexcept;

extern MelderGaussianStats NUMmeanStdev (constVECVU const& vec) noexcept;
extern MelderGaussianStats NUMmeanStdev (constMATVU const& mat) noexcept;

double NUMnorm (constVECVU const& x, double power) noexcept;
double NUMnorm (constMATVU const& x, double power) noexcept;

integer NUMnumberOfTokens (conststring32 str) noexcept;

/*
	Return zero for non-positive base.
*/
inline double NUMpow (double base, double exponent) {
	return base <= 0.0 ? 0.0 : pow (base, exponent);
}

inline double NUMsqrt_u (const double x) {
	#if defined (_WIN32)
		if (! (x >= 0.0))   // NaN-safe
			return undefined;
	#endif
	return sqrt (x);
}

inline double NUMsqrt_0 (const double x) {
	if (! (x >= 0.0))   // NaN-safe
		return 0.0;
	return sqrt (x);
}

inline double NUMsqrt_e (const double x) {
	Melder_require (x >= 0.0,
		U"You cannot take the square root of a negative number (", x, U").");
	return sqrt (x);
}

inline double NUMsqrt_a (const double x) {
	Melder_assert (x >= 0.0);
	return sqrt (x);
}

extern double NUMstdev (constVECVU const& vec) noexcept;
extern double NUMstdev (constMATVU const& mat) noexcept;

extern double NUMsum (constVECVU const& vec) noexcept;
extern double NUMsum (constMATVU const& mat) noexcept;

extern double NUMsum2 (constVECVU const& vec);
extern double NUMsum2 (constMATVU const& mat);

extern double NUMsumOfSquaredDifferences (constVECVU const& vec, double mean);
extern double NUMcorrelation (constVECVU const& vec1, constVECVU const& vec2);

extern double NUMtotalLength (constSTRVEC const& x);

extern double NUMvariance (constVECVU const& vec) noexcept;
extern double NUMvariance (constMATVU const& mat) noexcept;

/* End of file NUM.h */
