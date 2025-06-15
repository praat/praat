/* NUMfft_f.c
 *
 * Copyright (C) 1997-2008 David Weenink
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

int NUMforwardRealFastFourierTransform_f (float *data, long n)
{
	struct NUMfft_Table_f table_struct;
	NUMfft_Table_f table = &table_struct;

	if (!NUMfft_Table_init_f (table, n)) return 0;
	
	NUMfft_forward_f (table, data);

	if (n > 1)
	{
		/* To be compatible with old behaviour */
		long i;
		float tmp;

		tmp = data[n];
		for (i = n; i > 2; i--)
			data[i] = data[i - 1];
		data[2] = tmp;
	}

	NUMfft_Table_free_f (table);
	return 1;
}

int NUMreverseRealFastFourierTransform_f (float *data, long n)
{
	struct NUMfft_Table_f table_struct;
	NUMfft_Table_f table = &table_struct;

	if (n > 1)
	{
		/* To be compatible with old behaviour */
		long i;
		float tmp;

		tmp = data[2];
		for (i = 2; i < n; i++)
			data[i] = data[i + 1];
		data[n] = tmp;
	}

	if (!NUMfft_Table_init_f (table, n))
		return 0;
	NUMfft_backward_f (table, data);
	NUMfft_Table_free_f (table);
	return 1;
}

void NUMfft_forward_f (NUMfft_Table_f me, float *data)
{
	if (my n == 1)
		return;
	drftf1 (my n, &data[1], my trigcache, my trigcache + my n, my splitcache);
}

void NUMfft_backward_f (NUMfft_Table_f me, float *data)
{
	if (my n == 1)
		return;
	drftb1 (my n, &data[1], my trigcache, my trigcache + my n, my splitcache);
}

int NUMfft_Table_init_f (NUMfft_Table_f me, long n)
{
	int status = 0;
	
	my  n = n;
	
	my  trigcache = NUMfvector (0, 3 * n - 1);
	if (my trigcache == NULL) return 0;
	
	my  splitcache = NUMlvector (0, 31);
	if (my splitcache == NULL) goto end;
	
	NUMrffti (n, my trigcache, my splitcache);
	status = 1;

  end:

	if (Melder_hasError ()) NUMfft_Table_free_f (me);
	
	return status;
}

void NUMfft_Table_free_f (NUMfft_Table_f me)
{
	if (me)
	{
		NUMfvector_free (my trigcache, 0);
		NUMlvector_free (my splitcache, 0); 
	}
}

int NUMrealft_f (float *data, long n, int isign)
{
	return isign == 1 ? NUMforwardRealFastFourierTransform_f (data, n) :
		NUMreverseRealFastFourierTransform_f (data, n);
}

/* End of file NUMfft.c */
