/* CCs_to_DTW.c
 *
 *	Dynamic Time Warp of two CCs.
 * 
 * Copyright (C) 1993-2008 David Weenink
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
 djmw 2001
 djmw 20020315 GPL header
 djmw 20080122 float -> double
 */

#include "CCs_to_DTW.h"

static void regression (I, long frame, double r[], long nr)
{
	iam (CC);
	long i, j, nmin, nrd2 = nr / 2;
	
	/*
		sum(i^2;i=-n..n) = 2n^3/3 + n^2 +n/3 = n (n (2n/3 + 1) + 1/3);
	*/
		
	double sumsq = nrd2 * (nrd2 * (nr /3 + 1) + 1/3);
	
	if (frame <= nrd2 || frame >= my nx - nrd2) return;
	
	for (i = 0; i <= my maximumNumberOfCoefficients; i++)
	{
		r[i] = 0;
	}
	
	nmin = CC_getMinimumNumberOfCoefficients (me, frame - nrd2, frame + nrd2);

	for (i = 0; i <= nmin; i++)
	{
		double ri = 0;
		for (j = -nrd2; j <= nrd2; j++)
		{
			CC_Frame cf = & my frame[frame + j];
			double c = i == 0 ? cf -> c0 : cf -> c[i];
			ri += c * j;
		}
		r[i] = ri /sumsq / my dx;
	}
}


DTW CCs_to_DTW (I, thou, double wc, double wle, double wr, double wer,
	double dtr, int matchStart, int matchEnd, int constraint)
{
	iam (CC); thouart (CC);
	DTW him = NULL;
	double *ri = NULL, *rj = NULL;
	long i, j, k, nr = dtr / my dx, nmax = my maximumNumberOfCoefficients;
	
	if (nmax != thy maximumNumberOfCoefficients)
	{
		return Melder_errorp1 (L"CCs_to_DTW: CC orders must be equal.");
	}
	if (wr != 0 && nr < 2)
	{
		return Melder_errorp1 (L"CCs_to_DTW: Time window for regression "
			"coefficients is too small.");
	}
	if (nr % 2 == 0) nr++;
	if (wr != 0)
	{
		Melder_casual ("CCs_to_DTW: %ld frames used for regression "
			"coefficients.", nr);
	} 

	if (((him = DTW_create (my xmin, my xmax, my nx, my dx, my x1,
			thy xmin, thy xmax, thy nx, thy dx, thy x1)) == NULL) ||
		((ri = NUMdvector (0, nmax)) == NULL) ||
		((rj = NUMdvector (0, nmax)) == NULL)) goto end;

	/*
		Calculate distance matrix
	*/
	
	Melder_progress1 (0.0, L"CCs_to_DTW");
	for (i = 1; i <= my nx; i++)
	{
		CC_Frame fi = & my frame[i];
		
		regression (me, i, ri, nr);
		
		for (j = 1; j <= thy nx; j++)
		{
			CC_Frame fj = & thy frame[j];
			double d, dist = 0, distr = 0;
			
			/*
				Cepstral distance
			*/
			
			if (wc != 0)
			{
				for (k = 1; k <= fj->numberOfCoefficients; k++)
				{
					d = fi -> c[k] - fj -> c[k];
					dist += d * d;
				}
				dist *= wc;
			}
			
			/*
				Log energy distance
			*/
			
			if (wle != 0)
			{
				d = fi -> c0 - fj -> c0;
				dist += wle * d * d;
			}
			
			/*
				Regression distance
			*/
			
			if (wr != 0)
			{
				regression (thee, j, rj, nr);
				for (k = 1; k <= fj -> numberOfCoefficients; k++)
				{
					d = ri[k] - rj[k];
					distr += d * d;
				}
				dist += wr * distr;
			}
			
			/*
				Regression on c[0]: log(energy)
			*/
			
			if (wer != 0)
			{
				if (wr == 0) regression (thee, j, rj, nr);
				d = ri[0] - rj[0];
				dist += wer * d * d;
			}
			
			dist /= wc + wle + wr + wer;
			his z[i][j] = sqrt (dist);	/* prototype along y-direction */
		}
		
		if ((i % 10) == 1 && ! Melder_progress5 (0.999 * i / my nx, L"Calculate distances: frame ", Melder_integer (i), L" from ", Melder_integer (my nx), L".")) goto end;
	}
	
	Melder_progress1 (1.0, NULL);
	DTW_findPath (him, matchStart, matchEnd, constraint);
	
end:

	Melder_progress1 (1.0, NULL);
	NUMdvector_free (ri, 0);
	NUMdvector_free (rj, 0); 	
	if (Melder_hasError ()) forget (him);
	return him;
}

/* End of file CCs_to_DTW.c */
