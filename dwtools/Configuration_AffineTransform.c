/* Configuration_AffineTransform.c
 * 
 * Copyright (C) 1993-2005 David Weenink
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
 djmw 20020315 GPL header
 */

#include "Configuration_AffineTransform.h"
#include "Configuration_and_Procrustes.h"
#include "Procrustes.h"
#include "SVD.h"

static void do_steps45 (double **w, double **t, double **c, long n, double *f)
{
	long i, j, k; 
	double d, tct;
	
	/*
		Step 4 || 10: If W'T has negative diagonal elements, 
			multiply corresponding columns in T by -1.
	*/
	
	for (i = 1; i <= n; i++)
	{
		for (d = 0, k = 1; k <= n; k++)
		{
			d += w[k][i] * t[k][i];
		}
		if (d < 0)
		{
			for (k = 1; k <= n; k++)
			{
				t[k][i] = -t[k][i];
			}
		}
	}
	
	/*
		Step 5 & 11: f = tr W'T (Diag (T'CT))^-1/2
	*/
	
	for (*f = 0, i = 1; i <= n; i++)
	{
		for (tct = 0, d = 0, k=1; k <= n; k++) 
		{
			d += w[k][i] * t[k][i];
			for (j = 1; j <= n; j++)
			{
				tct += t[k][i] * c[k][j] * t[j][i];
			}
		}
		if (tct > 0) *f += d / sqrt (tct);
	}	
}

static int NUMmaximizeCongruence (double **b, double **a, long nr, long nc, 
	double **t, long maximumNumberOfIterations, double tolerance)
{
	SVD svd = NULL; 
	long i, j, k, numberOfIterations = 0;
	double **c = NULL, *evec = NULL,  **u = NULL, **w = NULL;
	double checkc, checkw, f, f_old, rho, scale;
	
	Melder_assert (nr > 0 && nc > 0);
	Melder_assert (t);
	
	if (nc == 1)
	{
		t[1][1] = 1; return 1;
	}
	
	if (! (c = NUMdmatrix (1, nc, 1, nc)) ||
		! (w = NUMdmatrix (1, nc, 1, nc)) ||
		! (u = NUMdmatrix (1, nc, 1, nc)) ||
		! (evec = NUMdvector (1, nc)) ||
		! (svd = SVD_create (nc, nc))) goto end;
	
	/*
		Steps 1 & 2: C = A'A and W = A'B
	*/
	
	for (checkc = checkw = 0, i = 1; i <= nc; i++)
	{
		for (j = 1; j <= nc; j++)
		{
			for (k = 1; k <= nr; k++)
			{
				c[i][j] += a[k][i] * a[k][j];
				w[i][j] += a[k][i] * b[k][j];
			}
			checkc += c[i][j]; checkw += w[i][j];
		}
	}
	
	if (checkc == 0 || checkw == 0) 
	{
		(void) Melder_error ("NUMmaximizeCongruence: we cannot rotate a "
			"zero matrix.");
		goto end;
	}
	
	/*
		Scale W by (diag(B'B))^-1/2
	*/
	
	for (j = 1; j <= nc; j++)
	{
		for (scale = 0, k = 1; k <= nr; k++)
		{
			scale += b[k][j] * b[k][j];
		}
		if (scale > 0)
		{
			scale = 1 / sqrt (scale);
		}
		for (i = 1; i <= nc; i++)
		{
			w[i][j] *= scale;
		}
	}
		
	/*
		Step 3: largest eigenvalue of C
	*/
	
	evec[1] = 1;
	if (! NUMdominantEigenvector_d (c, nc, evec, &rho, 1.0e-6)) goto end;
	
	do_steps45 (w, t, c, nc, &f);
		
	do
	{
		for (j = 1; j <= nc; j++)
		{
			double p, q, ct, ww;
			
			/* Step 7.a */	
			
			for (p = 0, k = 1; k <= nc; k++)
			{
				for (i = 1; i <= nc; i++)
				{
					p += t[k][j] * c[k][i] * t[i][j];
				}
			}
			
			/* Step 7.b */
			
			for (q = 0, k = 1; k <= nc; k++)
			{
				q += w[k][j] * t[k][j];
			}
			
			/* Step 7.c */
			
			if (q == 0)
			{
				for (i = 1; i <= nc; i++)
				{
					u[i][j] = 0;
				}
			}
			else
			{
				for (ww = 0, k = 1; k <= nc; k++)
				{
					ww += w[k][j] * w[k][j];
				}
				for (i = 1; i <= nc; i++)
				{
					for (ct = 0, k = 1; k <= nc; k++)
					{
						ct += c[i][k] * t[k][j];
					}
					u[i][j] = (q * (ct - rho * t[i][j]) / p - 2 * ww * t[i][j]
						/ q - w[i][j]) / sqrt (p);
				}
			}
		}
		
		/* Step 8 */
		
		if (! SVD_svd_d (svd, u)) goto end;
		
		/* Step 9 */
		
		for (i = 1; i <= nc; i++)
		{
			for (j = 1; j <= nc; j++)
			{
				for (t[i][j] = 0, k = 1; k <= nc; k++)
				{
					t[i][j] -= svd -> u[i][k] * svd -> v[j][k];
				}
			}
		}
		
		numberOfIterations++;
		f_old = f;
		
		/* Steps 10 & 11 equal steps 4 & 5 */
		
		do_steps45 (w, t, c, nc, &f);
	
	} while (fabs(f_old - f) / f_old > tolerance && 
		numberOfIterations < maximumNumberOfIterations);

end:

	NUMdmatrix_free (c, 1, 1);
	NUMdmatrix_free (w, 1, 1);
	NUMdvector_free (evec, 1);
	NUMdmatrix_free (u, 1, 1);
	forget (svd);
	return ! Melder_hasError();
}

AffineTransform Configurations_to_AffineTransform_congruence (Configuration me,
	Configuration thee, long maximumNumberOfIterations, double tolerance)
{
	Procrustes p = NULL;
	AffineTransform at = NULL;
		
	/*
		Use Procrustes transform to obtain starting configuration.
		(We only need the transformation matrix T.)
	*/

	p = Configurations_to_Procrustes (me, thee, 0);
	if (p == NULL) return NULL;

	if (! NUMmaximizeCongruence (my data, thy data, my numberOfRows,
		p -> n, p -> r, maximumNumberOfIterations, tolerance)) goto end;

	at = AffineTransform_create (p -> n);
	if (at == NULL) goto end;
	
	NUMdmatrix_copyElements (p -> r, at -> r, 1, p -> n, 1, p -> n);

end:

	forget (p);
	if (Melder_hasError()) forget (at);
	return at;
}


Configuration Configuration_and_AffineTransform_to_Configuration 
	(Configuration me, thou)
{
	Configuration him;
	thouart (AffineTransform);
	
	if (my numberOfColumns != thy n) return Melder_errorp
		("Configuration_and_AffineTransform_to_Configuration: dimensions do "
			"not agree.");
			
	him = Data_copy (me);
	if (him == NULL) return NULL;
	
	/*
		Apply transformation YT
	*/
	
	if (your transform) your transform (thee, my data, my numberOfRows,
		his data);
	
	return him;
}


/* End of file Configuration_AffineTransform.c */
