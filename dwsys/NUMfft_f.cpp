/* NUMfft_f.c
 *
 * Copyright (C) 1997-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* djmw 20020701 GPL header
	djmw 20040511 Added n>1 test for compatibility with old behaviour.
*/
#include "NUM2.h"
#include "melder.h"

#define my me ->

#define FFT_DATA_TYPE float
#include "NUMfft_core.h"

void NUMforwardRealFastFourierTransform_f (float *data, integer n) {
	struct NUMfft_Table_f table_struct;
	NUMfft_Table_f table = &table_struct;

	NUMfft_Table_init_f (table, n);

	NUMfft_forward_f (table, data);

	if (n > 1) {
		// To be compatible with old behaviour
		float tmp = data[n];
		for (integer i = n; i > 2; i--) {
			data[i] = data[i - 1];
		}
		data[2] = tmp;
	}

	NUMfft_Table_free_f (table);
}

void NUMreverseRealFastFourierTransform_f (float *data, integer n) {
	struct NUMfft_Table_f table_struct;
	NUMfft_Table_f table = &table_struct;

	if (n > 1) {
		// To be compatible with old behaviour
		float tmp = data[2];
		for (integer i = 2; i < n; i++) {
			data[i] = data[i + 1];
		}
		data[n] = tmp;
	}

	NUMfft_Table_init_f (table, n);
	NUMfft_backward_f (table, data);
	NUMfft_Table_free_f (table);
}

void NUMfft_forward_f (NUMfft_Table_f me, float *data) {
	if (my n == 1) {
		return;
	}
	drftf1 (my n, &data[1], my trigcache, my trigcache + my n, my splitcache);
}

void NUMfft_backward_f (NUMfft_Table_f me, float *data) {
	if (my n == 1) {
		return;
	}
	drftb1 (my n, &data[1], my trigcache, my trigcache + my n, my splitcache);
}

void NUMfft_Table_init_f (NUMfft_Table_f me, integer n) {
	my n = n;
	my trigcache = NUMvector<float> (0, 3 * n - 1);
	my splitcache = NUMvector<integer> (0, 31);
	NUMrffti (n, my trigcache, my splitcache);
}

void NUMfft_Table_free_f (NUMfft_Table_f me) {
	if (me) {
		NUMvector_free (my trigcache, 0);
		NUMvector_free (my splitcache, 0);
	}
}

void NUMrealft_f (float *data, integer n, int isign) {
	isign == 1 ? NUMforwardRealFastFourierTransform_f (data, n) :
	NUMreverseRealFastFourierTransform_f (data, n);
}

/* End of file NUMfft.cpp */
