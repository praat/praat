/* NUMFourier.cpp
 *
 * Copyright (C) 2025 David Weenink
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

#include "melder.h"
#include "NUMFourier.h"

#include "oo_DESTROY.h"
#include "NUMFourierTable_def.h"
#include "oo_COPY.h"
#include "NUMFourierTable_def.h"
#include "oo_EQUAL.h"
#include "NUMFourierTable_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "NUMFourierTable_def.h"
#include "oo_WRITE_TEXT.h"
#include "NUMFourierTable_def.h"
#include "oo_WRITE_BINARY.h"
#include "NUMFourierTable_def.h"
#include "oo_READ_TEXT.h"
#include "NUMFourierTable_def.h"
#include "oo_READ_BINARY.h"
#include "NUMFourierTable_def.h"
#include "oo_DESCRIPTION.h"
#include "NUMFourierTable_def.h"

#define FFT_DATA_TYPE double
#include "NUMfft_core.h"

Thing_implement (NUMFourierTable, Daata, 0);

autoNUMFourierTable NUMFourierTable_create (integer n) {
	try {
		autoNUMFourierTable me = Thing_new (NUMFourierTable);
		my n = n;
		my trigcacheSize = 3 * n;
		my trigcache = zero_VEC (my trigcacheSize);
		my splitcacheSize = 32;
		my splitcache = zero_INTVEC (my splitcacheSize);
		NUMrffti (n, my trigcache.asArgumentToFunctionThatExpectsZeroBasedArray(),
			my splitcache.asArgumentToFunctionThatExpectsZeroBasedArray()
	);

		 return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot create NUMFourierTable.");
	}
}

void NUMforwardRealFastFourierTransform (VEC data) {
	autoNUMFourierTable table = NUMFourierTable_create (data.size);
	NUMfft_forward (table.get(), data);
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
	if (data.size > 1) {
		/*
			To be compatible with old behaviour.
		*/
		double tmp = data [2];
		for (integer i = 2; i < data.size; i ++)
			data [i] = data [i + 1];
		data [data.size] = tmp;
	}
	autoNUMFourierTable table = NUMFourierTable_create (data.size);
	NUMfft_backward (table.get(), data);
}

void NUMfft_forward (NUMFourierTable me, VEC data) {
	if (my n == 1)
		return;
	Melder_assert (my n == data.size);
	drftf1 (my n, data.asArgumentToFunctionThatExpectsZeroBasedArray(),
		my trigcache.asArgumentToFunctionThatExpectsZeroBasedArray(),
		my trigcache.asArgumentToFunctionThatExpectsZeroBasedArray() + my n,
		my splitcache.asArgumentToFunctionThatExpectsZeroBasedArray()
	);
}

void NUMfft_backward (NUMFourierTable me, VEC data) {
	if (my n == 1)
		return;
	Melder_assert (my n == data.size);
	drftb1 (my n, data.asArgumentToFunctionThatExpectsZeroBasedArray(),
		my trigcache.asArgumentToFunctionThatExpectsZeroBasedArray(),
		my trigcache.asArgumentToFunctionThatExpectsZeroBasedArray() + my n,
		my splitcache.asArgumentToFunctionThatExpectsZeroBasedArray()
	);
}


void NUMrealft (VEC data, integer isign) {
	if (isign == 1)
		NUMforwardRealFastFourierTransform (data);
	else
		NUMreverseRealFastFourierTransform (data);
}


/* End of file NUMFourier.cpp */
