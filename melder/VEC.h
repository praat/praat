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

inline void VECadd_inplace (VEC const& x, double addend) noexcept {
	for (integer i = 1; i <= x.size; i ++)
		x [i] += addend;
}
inline void VECadd_inplace (VEC const& x, constVEC const& y) noexcept {
	Melder_assert (y.size == x.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] += y [i];
}
inline void VECadd_preallocated (VEC const& target, constVEC const& x, double addend) noexcept {
	Melder_assert (x.size == target.size);
	for (integer i = 1; i <= x.size; i ++)
		target [i] = x [i] + addend;
}
extern void VECadd_macfast_ (VEC const& target, constVEC const& x, constVEC const& y) noexcept;
inline void VECadd_preallocated (VEC const& target, constVEC const& x, constVEC const& y) noexcept {
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
inline autoVEC VECadd (constVEC const& x, double addend) {
	autoVEC result = VECraw (x.size);
	VECadd_preallocated (result.get(), x, addend);
	return result;
}
inline autoVEC VECadd (constVEC const& x, constVEC const& y) {
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
inline autoVEC VECcolumnMeans (const constMAT& x) {
	autoVEC result = VECraw (x.ncol);
	VECcolumnMeans_preallocated (result.get(), x);
	return result;
}

inline void VECdivide_inplace (VEC const& x, double factor) noexcept {
	for (integer i = 1; i <= x.size; i ++)
		x [i] /= factor;
}
inline void VECdivide_inplace (VEC const& x, constVEC const& y) noexcept {
	Melder_assert (y.size == x.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] /= y [i];
}
inline void VECdivide_preallocated (VEC const& target, constVEC const& x, double factor) noexcept {
	Melder_assert (x.size == target.size);
	for (integer i = 1; i <= x.size; i ++)
		target [i] = x [i] / factor;
}
inline void VECdivide_preallocated (VEC const& target, constVEC const& x, constVEC const& y) noexcept {
	integer n = target.size;
	Melder_assert (x.size == n);
	Melder_assert (y.size == n);
	for (integer i = 1; i <= n; i ++)
		target [i] = x [i] / y [i];
}
inline autoVEC VECdivide (constVEC const& x, double factor) {
	autoVEC result = VECraw (x.size);
	VECdivide_preallocated (result.get(), x, factor);
	return result;
}
inline autoVEC VECdivide (constVEC const& x, constVEC const& y) {
	autoVEC result = VECraw (x.size);
	VECdivide_preallocated (result.get(), x, y);
	return result;
}

extern void VECmul_preallocated (const VEC& target, const constVEC& vec, const constMAT& mat) noexcept;
extern void VECmul_preallocated (const VEC& target, const constMAT& mat, const constVEC& vec) noexcept;
extern autoVEC VECmul (const constVEC& vec, const constMAT& mat) noexcept;
extern autoVEC VECmul (const constMAT& mat, const constVEC& vec) noexcept;

inline void VECmultiply_inplace (VEC const& x, double factor) noexcept {
	for (integer i = 1; i <= x.size; i ++)
		x [i] *= factor;
}
inline void VECmultiply_inplace (VEC const& x, constVEC const& y) noexcept {
	Melder_assert (y.size == x.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] *= y [i];
}
inline void VECmultiply_preallocated (VEC const& target, constVEC const& x, double factor) noexcept {
	Melder_assert (x.size == target.size);
	for (integer i = 1; i <= x.size; i ++)
		target [i] = x [i] * factor;
}
inline void VECmultiply_preallocated (VEC const& target, constVEC const& x, constVEC const& y) noexcept {
	integer n = target.size;
	Melder_assert (x.size == n);
	Melder_assert (y.size == n);
	for (integer i = 1; i <= n; i ++)
		target [i] = x [i] * y [i];
}
inline autoVEC VECmultiply (constVEC const& x, double factor) {
	autoVEC result = VECraw (x.size);
	VECmultiply_preallocated (result.get(), x, factor);
	return result;
}
inline autoVEC VECmultiply (constVEC const& x, constVEC const& y) {
	autoVEC result = VECraw (x.size);
	VECmultiply_preallocated (result.get(), x, y);
	return result;
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

inline void VECsin_inplace (VEC const& x) noexcept {
	for (double& element : x) element = sin (element);
}

extern void VECsort_inplace (VEC const& x) noexcept;

inline void VECsubtract_inplace (VEC const& x, double number) noexcept {
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= number;
}
inline void VECsubtract_inplace (VEC const& x, constVEC const& y) noexcept {
	Melder_assert (x.size == y.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= y [i];
}
inline void VECsubtractReversed_inplace (VEC const& x, double number) noexcept {
	for (integer i = 1; i <= x.size; i ++)
		x [i] = number - x [i];
}
inline void VECsubtractReversed_inplace (VEC const& x, constVEC const& y) noexcept {
	Melder_assert (x.size == y.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] = y [i] - x [i];
}
inline void VECsubtract_preallocated (VEC const& target, constVEC const& x, double number) noexcept {
	Melder_assert (x.size == target.size);
	for (integer i = 1; i <= x.size; i ++)
		target [i] = x [i] - number;
}
inline void VECsubtract_preallocated (VEC const& target, double number, constVEC const& x) noexcept {
	Melder_assert (x.size == target.size);
	for (integer i = 1; i <= x.size; i ++)
		target [i] = number - x [i];
}
inline void VECsubtract_preallocated (VEC const& target, constVEC const& x, constVEC const& y) noexcept {
	integer n = target.size;
	Melder_assert (x.size == n);
	Melder_assert (y.size == n);
	for (integer i = 1; i <= n; i ++)
		target [i] = x [i] - y [i];
}
inline autoVEC VECsubtract (constVEC const& vec, double number) {
	autoVEC result = VECraw (vec.size);
	VECsubtract_preallocated (result.get(), vec, number);
	return result;
}
inline autoVEC VECsubtract (double number, constVEC const& vec) {
	autoVEC result = VECraw (vec.size);
	VECsubtract_preallocated (result.get(), number, vec);
	return result;
}
inline autoVEC VECsubtract (constVEC const& x, constVEC const& y) {
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
