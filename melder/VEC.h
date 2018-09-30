#pragma once
/* VEC.h
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
	From here on alphabetical order.
*/

inline void VECadd_inplace (const VEC& x, double addend) noexcept {
	//for (integer i = 1; i <= x.size; i ++)
	//	x [i] += addend;
	for (double& element : x) element += addend;
}
inline VEC operator+= (const VEC& x, double addend) noexcept {
	//for (integer i = 1; i <= x.size; i ++)
	//	x [i] += addend;
	for (double& element : x) element += addend;
	return x;
}
inline void VECVUadd_inplace (const VECVU& x, const constVECVU& y) noexcept {
	Melder_assert (y.size == x.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] += y [i];
}
inline VEC operator+= (const VEC& x, const constVEC& y) noexcept {
	Melder_assert (y.size == x.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] += y [i];
	return x;
}
inline void VECadd_preallocated (const VEC& target, const constVEC& x, double addend) noexcept {
	Melder_assert (x.size == target.size);
	for (integer i = 1; i <= x.size; i ++)
		target [i] = x [i] + addend;
}inline void VECVUadd (const VECVU& target, const constVECVU& x, double addend) noexcept {
	Melder_assert (x.size == target.size);
	for (integer i = 1; i <= x.size; i ++)
		target [i] = x [i] + addend;
}
inline autoVEC VECadd (const constVEC& x, double addend) {
	autoVEC result = VECraw (x.size);
	VECadd_preallocated (result.get(), x, addend);
	return result;
}
extern void VECadd_macfast_ (const VEC& target, const constVEC& x, const constVEC& y) noexcept;
inline void VECadd_preallocated  (const VEC& target, const constVEC& x, const constVEC& y) noexcept {
	integer n = target.size;
	Melder_assert (x.size == n);
	Melder_assert (y.size == n);
	#if defined (macintosh)
		if (n >= 64)
			return VECadd_macfast_ (target, x, y);
	#endif
	for (integer i = 1; i <= n; i ++)
		target [i] = x [i] + y [i];
}
inline autoVEC VECadd (const constVEC& x, const constVEC& y) {
	autoVEC result = VECraw (x.size);
	VECadd_preallocated (result.get(), x, y);
	return result;
}
extern void VECVUadd_macfast_ (const VECVU& target, const constVECVU& x, const constVECVU& y) noexcept;
inline void VECVUadd (const VECVU& target, const constVECVU& x, const constVECVU& y) noexcept {
	integer n = target.size;
	Melder_assert (x.size == n);
	Melder_assert (y.size == n);
	#if defined (macintoshXXX)
		if (n >= 64)
			return VECVUadd_macfast_ (target, x, y);
	#endif
	for (integer i = 1; i <= n; i ++)
		target [i] = x [i] + y [i];
}

inline void VECcentre_inplace (const VEC& x, double *out_mean = nullptr) noexcept {
	double xmean = NUMmean (x);
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= xmean;
	if (out_mean)
		*out_mean = xmean;
}

inline void VECcolumn_preallocated (const VEC& target, const constMAT& source, integer columnNumber) noexcept {
	Melder_assert (source.nrow == target.size);
	Melder_assert (columnNumber >= 1 && columnNumber <= source.ncol);
	for (integer irow = 1; irow <= target.size; irow ++)
		target [irow] = source [irow] [columnNumber];
}
inline autoVEC VECcolumn (const constMAT& source, integer columnNumber) {
	autoVEC target = VECraw (source.nrow);
	VECcolumn_preallocated (target.get(), source, columnNumber);
	return target;
}

inline void VECcolumnMeans_preallocated (const VEC& target, const constMAT& x) noexcept {
	Melder_assert (target.size == x.ncol);
	for (integer icol = 1; icol <= x.ncol; icol ++)
		target [icol] = NUMcolumnMean (x, icol);
}

extern void VECmul_preallocated (const VEC& target, const constVEC& vec, const constMAT& mat) noexcept;
extern void VECmul_preallocated (const VEC& target, const constMAT& mat, const constVEC& vec) noexcept;
extern autoVEC VECmul (const constVEC& vec, const constMAT& mat) noexcept;
extern autoVEC VECmul (const constMAT& mat, const constVEC& vec) noexcept;

inline void VECmultiply_inplace (const VEC& x, double factor) noexcept {
	for (integer i = 1; i <= x.size; i ++)
		x [i] *= factor;
}

inline autoVEC VECrandomGauss (integer size, double mu, double sigma) {
	autoVEC result = VECraw (size);
	for (integer i = 1; i <= size; i ++)
		result [i] = NUMrandomGauss (mu, sigma);
	return result;
}

inline autoVEC VECrandomUniform (integer size, double lowest, double highest) {
	autoVEC result = VECraw (size);
	for (integer i = 1; i <= size; i ++)
		result [i] = NUMrandomUniform (lowest, highest);
	return result;
}

inline void VECsin_inplace (const VEC& x) noexcept {
	for (double& element : x) element = sin (element);
}

extern void VECsort_inplace (const VEC& x) noexcept;

inline void VECsubtract_inplace (const VEC& x, double number) noexcept {
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= number;
}
inline void VECsubtractReversed_inplace (const VEC& x, double number) noexcept {
	for (integer i = 1; i <= x.size; i ++)
		x [i] = number - x [i];
}
inline void VECsubtract_inplace (const VEC& x, const constVEC& y) noexcept {
	Melder_assert (x.size == y.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= y [i];
}
inline void VECsubtractReversed_inplace (const VEC& x, const constVEC& y) noexcept {
	Melder_assert (x.size == y.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] = y [i] - x [i];
}
inline autoVEC VECsubtract (const constVEC& x, double y) {
	autoVEC result = VECraw (x.size);
	for (integer i = 1; i <= x.size; i ++)
		result [i] = x [i] - y;
	return result;
}
inline autoVEC VECsubtract (double x, const constVEC& y) {
	autoVEC result = VECraw (y.size);
	for (integer i = 1; i <= y.size; i ++)
		result [i] = x - y [i];
	return result;
}
inline autoVEC VECsubtract (const constVEC& x, const constVEC& y) {
	Melder_assert (x.size == y.size);
	autoVEC result = VECraw (x.size);
	for (integer i = 1; i <= x.size; i ++)
		result [i] = x [i] - y [i];
	return result;
}

inline autoVEC VECsumPerRow (const constMAT& x) {
	autoVEC result = VECraw (x.nrow);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		result [irow] = NUMrowSum (x, irow);
	return result;
}

inline autoVEC VECsumPerColumn (const constMAT& x) {
	autoVEC result = VECraw (x.ncol);
	for (integer icol = 1; icol <= x.ncol; icol ++)
		result [icol] = NUMcolumnSum (x, icol);
	return result;
}

inline autoVEC VECto (integer to) {
	autoVEC result = VECraw (to);
	for (integer i = 1; i <= to; i ++)
		result [i] = (double) i;
	return result;
}

/* End of file VEC.h */
