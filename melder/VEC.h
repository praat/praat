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

inline void VECadd_inplace (VEC x, double addend) {
	for (integer i = 1; i <= x.size; i ++)
		x [i] += addend;
}
inline void VECadd_inplace (VEC x, constVEC y) {
	Melder_assert (y.size == x.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] += y [i];
}
inline void VECadd_preallocated (VEC target, constVEC x, double addend) {
	Melder_assert (x.size == target.size);
	for (integer i = 1; i <= x.size; i ++)
		target [i] = x [i] + addend;
}
inline autoVEC VECadd (constVEC x, double addend) {
	autoVEC result = VECraw (x.size);
	VECadd_preallocated (result.get(), x, addend);
	return result;
}
inline void VECadd_preallocated (VEC target, constVEC x, constVEC y) {
	Melder_assert (x.size == target.size);
	Melder_assert (y.size == x.size);
	for (integer i = 1; i <= x.size; i ++)
		target [i] = x [i] + y [i];
}
inline autoVEC VECadd (constVEC x, constVEC y) {
	autoVEC result = VECraw (x.size);
	VECadd_preallocated (result.get(), x, y);
	return result;
}

inline void VECcentre_inplace (VEC x, double *out_mean = nullptr) {
	double xmean = NUMmean (x);
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= xmean;
	if (out_mean)
		*out_mean = xmean;
}

inline void VECcolumn_preallocated (VEC target, constMAT source, integer columnNumber) {
	Melder_assert (source.nrow == target.size);
	Melder_assert (columnNumber >= 1 && columnNumber <= source.ncol);
	for (integer irow = 1; irow <= target.size; irow ++)
		target [irow] = source [irow] [columnNumber];
}

extern void VECmul_preallocated (VEC target, constVEC vec, constMAT mat);
extern void VECmul_preallocated (VEC target, constMAT mat, constVEC vec);
extern autoVEC VECmul (constVEC vec, constMAT mat);
extern autoVEC VECmul (constMAT mat, constVEC vec);

inline void VECmultiply_inplace (VEC x, double factor) {
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

extern void VECsort_inplace (VEC x);

inline void VECsubtract_inplace (VEC x, double number) {
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= number;
}
inline void VECsubtractReversed_inplace (VEC x, double number) {
	for (integer i = 1; i <= x.size; i ++)
		x [i] = number - x [i];
}
inline void VECsubtract_inplace (VEC x, constVEC y) {
	Melder_assert (x.size == y.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= y [i];
}
inline void VECsubtractReversed_inplace (VEC x, constVEC y) {
	Melder_assert (x.size == y.size);
	for (integer i = 1; i <= x.size; i ++)
		x [i] = y [i] - x [i];
}
inline autoVEC VECsubtract (constVEC x, double y) {
	autoVEC result = VECraw (x.size);
	for (integer i = 1; i <= x.size; i ++)
		result [i] = x [i] - y;
	return result;
}
inline autoVEC VECsubtract (double x, constVEC y) {
	autoVEC result = VECraw (y.size);
	for (integer i = 1; i <= y.size; i ++)
		result [i] = x - y [i];
	return result;
}
inline autoVEC VECsubtract (constVEC x, constVEC y) {
	Melder_assert (x.size == y.size);
	autoVEC result = VECraw (x.size);
	for (integer i = 1; i <= x.size; i ++)
		result [i] = x [i] - y [i];
	return result;
}

inline autoVEC VECsumPerRow (constMAT x) {
	autoVEC result = VECraw (x.nrow);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		result [irow] = NUMrowSum (x, irow);
	return result;
}

inline autoVEC VECsumPerColumn (constMAT x) {
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
