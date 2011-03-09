/* NUMfft_d.c
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

/* djmw 20020813 GPL header
	djmw 20040511 Added n>1 test for compatibility with old behaviour.
	djmw 20110308 struct renaming
 */

#include "NUM2.h"
#include "melder.h"

#define my me ->

#define FFT_DATA_TYPE double
#include "NUMfft_core.h"


int NUMforwardRealFastFourierTransform (double *data, long n)
{
	struct structNUMfft_Table table_struct;
	NUMfft_Table table = &table_struct;

	if (!NUMfft_Table_init (table, n))
		return 0;
	NUMfft_forward (table, data);

	if (n > 1)
	{
		/* To be compatible with old behaviour */
		long i;
		double tmp;

		tmp = data[n];
		for (i = n; i > 2; i--)
			data[i] = data[i - 1];
		data[2] = tmp;
	}

	NUMfft_Table_free (table);
	return 1;
}

int NUMreverseRealFastFourierTransform (double *data, long n)
{
	struct structNUMfft_Table table_struct;
	NUMfft_Table table = &table_struct;

	if (n > 1)
	{
		/* To be compatible with old behaviour */
		long i;
		double tmp;

		tmp = data[2];
		for (i = 2; i < n; i++) data[i] = data[i + 1];
		data[n] = tmp;
	}

	if (!NUMfft_Table_init (table, n)) return 0;
	NUMfft_backward (table, data);
	NUMfft_Table_free (table);
	return 1;
}

void NUMfft_forward (NUMfft_Table me, double *data)
{
	if (my n == 1)
		return;
	drftf1 (my n, &data[1], my trigcache, my trigcache + my n, my splitcache);
}

void NUMfft_backward (NUMfft_Table me, double *data)
{
	if (my n == 1)
		return;
	drftb1 (my n, &data[1], my trigcache, my trigcache + my n, my splitcache);
}

int NUMfft_Table_init (NUMfft_Table me, long n)
{
	int status = 0;
	
	my  n = n;
	
	my  trigcache = NUMdvector (0, 3 * n - 1);
	if (my trigcache == NULL) return 0;
	
	my  splitcache = NUMlvector (0, 31);
	if (my splitcache == NULL) goto end;
	
	NUMrffti (n, my trigcache, my splitcache);
	status = 1;

  end:

	if (Melder_hasError ()) NUMfft_Table_free (me);
	
	return status;
}

void NUMfft_Table_free (NUMfft_Table me)
{
	if (me)
	{
		NUMdvector_free (my trigcache, 0);
		NUMlvector_free (my splitcache, 0); 
	}
}

int NUMrealft (double *data, long n, int isign)
{
	return isign == 1 ? NUMforwardRealFastFourierTransform (data, n) :
		NUMreverseRealFastFourierTransform (data, n);
}

/* End of file NUMfft.c */
