 /* NUMhuber_core.h
 *
 * Copyright (C) 1994-2003 David Weenink
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

/*
 djmw 20030814 First version
*/

int HUBER_MAD (HUBER_DATA_TYPE *x, long n, double *location, int wantlocation, double *mad, HUBER_DATA_TYPE *work)
{
	HUBER_DATA_TYPE *tmp = work;
	long i;

	*mad = NUMundefined;
	if (n < 1) return 0;
	if (n == 1)
	{
		*location = x[1];
		return 0;
	}

	if (work == NULL)
	{
		tmp = HUBER_VECTOR (1, n);
		if (tmp == NULL) return 0;
	}

	for (i = 1; i <= n; i++)
	{
		tmp[i] = x[i];
	}

	if (wantlocation)
	{
		HUBER_SORT (n, tmp);
		*location = HUBER_QUANTILE (n, tmp, 0.5);
	}

	for (i = 1; i <= n; i++)
	{
		tmp[i] = fabs (tmp[i] - *location);
	}

	HUBER_SORT (n, tmp);
	*mad = 1.4826 * HUBER_QUANTILE (n, tmp, 0.5);

	if (work == NULL) HUBER_VECTOR_FREE (tmp, 1);
	return 1;
}

int HUBER_STATISTICS_HUBER (HUBER_DATA_TYPE *x, long n, double *location, int wantlocation,
	double *scale, int wantscale, double k, double tol, HUBER_DATA_TYPE *work)
{
	HUBER_DATA_TYPE *tmp = work, low, high;
	double s0, s1, mu0, mu1, mad;
	double theta = 2 * NUMgaussP (k) - 1;
	double beta = theta + k * k * (1 - theta) - 2 * k * NUMgauss (k);
	long i, n1 = n;

	if (work == NULL)
	{
		tmp = HUBER_VECTOR (1, n);
		if (tmp == NULL) return 0;
	}

	if (! HUBER_MAD (x, n, location, wantlocation, & mad, tmp)) goto end;
    if (wantscale) *scale = mad;
    if (*scale == 0) return Melder_error ("NUMstatistics_huber: scale is zero.");

    mu1 = *location;
    s1 = *scale;

    if (wantlocation) n1 = n - 1;

    do
    {
		mu0 = mu1;
		s0 = s1;

		low  = mu0 - k * s0; 
		high = mu0 + k * s0;

		for (i = 1; i <= n; i++)
		{
			if (x[i] < low)
			{
				tmp[i] = low;
			}
			else if (x[i] > high)
			{
				tmp[i] = high;
			}
			else
			{
				tmp[i] =  x[i];
			}
		}
		if (wantlocation)
		{
			mu1 = 0;
			for (i = 1; i <= n; i++)
			{
				mu1 += tmp[i];
			}
			mu1 /= n;
        }
        if (wantscale)
		{
			s1 = 0;
			for (i = 1; i <= n; i++)
			{
				double dx = tmp[i] - mu1;
				s1 += dx * dx;
			}
			s1 = sqrt (s1 / (n1 * beta));
		}
    } while (fabs (mu0 - mu1) > tol * s0 || fabs (s0 - s1) > tol * s0);

    if (wantlocation) *location = mu1;
    if (wantscale) *scale = s1;

end:
	if (work == NULL) HUBER_VECTOR_FREE (tmp, 1);
	return 1;
}
