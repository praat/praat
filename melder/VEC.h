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
	Some functions that are used below.
*/

inline static autoVEC VECraw (integer size) {
	return { size, kTensorInitializationType::RAW };
}
inline static autoVEC VECzero (integer size) {
	return { size, kTensorInitializationType::ZERO };
}

/*
	From here on alphabetical order.
*/

autoVEC VECcopy (constVEC x);

autoVEC VECmul (constVEC vec, constMAT mat);
void VECmul_inplace (VEC target, constVEC vec, constMAT mat);
autoVEC VECmul (constMAT mat, constVEC vec);
void VECmul_inplace (VEC target, constMAT mat, constVEC vec);

inline static void VECadd_inplace (VEC target, double number) {
	for (integer i = 1; i <= target.size; i ++)
		target [i] += number;
}
inline static void VECadd_inplace (VEC target, constVEC vec) {
	for (integer i = 1; i <= target.size; i ++)
		target [i] += vec [i];
}
inline static autoVEC VECadd (constVEC x, double addend) {
	autoVEC result = VECraw (x.size);
	for (integer i = 1; i <= x.size; i ++)
		result [i] = x [i] + addend;
	return result;
}
inline static autoVEC VECadd (constVEC x, constVEC y) {
	if (x.size != y.size) return autoVEC { };
	autoVEC result = VECraw (x.size);
	for (integer i = 1; i <= x.size; i ++)
		result [i] = x [i] + y [i];
	return result;
}

inline static void VECcentre_inplace (VEC x, double *out_mean = nullptr) {
	double xmean = NUMmean (x);
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= xmean;
	if (out_mean)
		*out_mean = xmean;
}

inline static void VECcopy_inplace (VEC x, constVEC source) {
	for (integer i = 1; i <= source.size; i ++)
		x [i] = source [i];
}

inline static void VECmultiply_inplace (VEC x, double factor) {
	for (integer i = 1; i <= x.size; i ++)
		x [i] *= factor;
}

inline static autoVEC VECrandomGauss (integer size, double mu, double sigma) {
	autoVEC result = VECraw (size);
	for (integer i = 1; i <= size; i ++)
		result [i] = NUMrandomGauss (mu, sigma);
	return result;
}

void VECsort_inplace (VEC x);

inline static void VECsubtract_inplace (VEC x, double number) {
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= number;
}
inline static void VECsubtractReversed_inplace (VEC x, double number) {
	for (integer i = 1; i <= x.size; i ++)
		x [i] = number - x [i];
}
inline static void VECsubtract_inplace (VEC x, constVEC y) {
	for (integer i = 1; i <= x.size; i ++)
		x [i] -= y [i];
}
inline static void VECsubtractReversed_inplace (VEC x, constVEC y) {
	for (integer i = 1; i <= x.size; i ++)
		x [i] = y [i] - x [i];
}
inline static autoVEC VECsubtract (constVEC x, double y) {
	autoVEC result = VECraw (x.size);
	for (integer i = 1; i <= x.size; i ++)
		result [i] = x [i] - y;
	return result;
}
inline static autoVEC VECsubtract (double x, constVEC y) {
	autoVEC result = VECraw (y.size);
	for (integer i = 1; i <= y.size; i ++)
		result [i] = x - y [i];
	return result;
}
inline static autoVEC VECsubtract (constVEC x, constVEC y) {
	if (x.size != y.size) return autoVEC { };
	autoVEC result = VECraw (x.size);
	for (integer i = 1; i <= x.size; i ++)
		result [i] = x [i] - y [i];
	return result;
}

inline static autoVEC VECsumPerRow (constMAT x) {
	autoVEC result = VECraw (x.nrow);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		result [irow] = NUMrowSum (x, irow);
	return result;
}

inline static autoVEC VECsumPerColumn (constMAT x) {
	autoVEC result = VECraw (x.ncol);
	for (integer icol = 1; icol <= x.ncol; icol ++)
		result [icol] = NUMcolumnSum (x, icol);
	return result;
}


inline static autoVEC VECto (integer to) {
	autoVEC result = VECraw (to);
	for (integer i = 1; i <= to; i ++)
		result [i] = (double) i;
	return result;
}

/* End of file VEC.h */
