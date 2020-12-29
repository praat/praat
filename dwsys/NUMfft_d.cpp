/* NUMfft_d.cpp
 *
 * Copyright (C) 1997-2011 David Weenink, Paul Boersma 2016-2018,2020
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/* djmw 20020813 GPL header
	djmw 20040511 Added n>1 test for compatibility with old behaviour.
	djmw 20110308 struct renaming
 */

#include "NUM2.h"
#include "melder.h"

#define FFT_DATA_TYPE double
#include "NUMfft_core.h"

void NUMforwardRealFastFourierTransform (VEC data) {
	autoNUMfft_Table table;
	NUMfft_Table_init (& table, data.size);
	NUMfft_forward (& table, data);
	if (data.size > 1) {
		/*
			To be compatible with old behaviour.
		*/
		double tmp = data [data.size];
		for (integer i = data.size; i > 2; i --)
			data [i] = data [i - 1];
		data [2] = tmp;
	}
}

void NUMreverseRealFastFourierTransform (VEC data) {
	autoNUMfft_Table table;
	if (data.size > 1) {
		/*
			To be compatible with old behaviour.
		*/
		double tmp = data [2];
		for (integer i = 2; i < data.size; i ++)
			data [i] = data [i + 1];
		data [data.size] = tmp;
	}
	NUMfft_Table_init (& table, data.size);
	NUMfft_backward (& table, data);
}

void NUMfft_forward (NUMfft_Table me, VEC data) {
	if (my n == 1)
		return;
	Melder_assert (my n == data.size);
	drftf1 (my n, data.asArgumentToFunctionThatExpectsZeroBasedArray(),
		my trigcache.asArgumentToFunctionThatExpectsZeroBasedArray(),
		my trigcache.asArgumentToFunctionThatExpectsZeroBasedArray() + my n,
		my splitcache.asArgumentToFunctionThatExpectsZeroBasedArray()
	);
}

void NUMfft_backward (NUMfft_Table me, VEC data) {
	if (my n == 1)
		return;
	Melder_assert (my n == data.size);
	drftb1 (my n, data.asArgumentToFunctionThatExpectsZeroBasedArray(),
		my trigcache.asArgumentToFunctionThatExpectsZeroBasedArray(),
		my trigcache.asArgumentToFunctionThatExpectsZeroBasedArray() + my n,
		my splitcache.asArgumentToFunctionThatExpectsZeroBasedArray()
	);
}

void NUMfft_Table_init (NUMfft_Table me, integer n) {
	my n = n;
	my trigcache = zero_VEC (3 * n);
	my splitcache = zero_INTVEC (32);
	NUMrffti (n, my trigcache.asArgumentToFunctionThatExpectsZeroBasedArray(),
		my splitcache.asArgumentToFunctionThatExpectsZeroBasedArray()
	);
}

void NUMrealft (VEC data, integer isign) {
	if (isign == 1)
		NUMforwardRealFastFourierTransform (data);
	else
		NUMreverseRealFastFourierTransform (data);
}

/* End of file NUMfft.cpp */
