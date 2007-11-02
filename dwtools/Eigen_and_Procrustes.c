/* Eigen_and_Procrustes.c
 * Copyright (C) 2005 David Weenink
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
 
/* djmw 2004 Initial version */

#include "Eigen_and_Procrustes.h"
#include "Configuration_and_Procrustes.h"
#include "NUM2.h"

Procrustes Eigens_to_Procrustes (I, thou, long evec_from, long evec_to)
{
	iam (Eigen); thouart (Eigen);
	Procrustes him = NULL;
	long i, j, nvectors = evec_to - evec_from + 1; 
	long nmin = my numberOfEigenvalues < thy numberOfEigenvalues ? my numberOfEigenvalues : thy numberOfEigenvalues;
	double **x = NULL, **y = NULL;
	
	if (my dimension != thy dimension) return Melder_errorp1 (L"The eigenvectors must have the same dimension.");

	if (evec_from > evec_to || evec_from < 1 || evec_to > nmin) 
		return Melder_errorp1 (L"Eigenvector range too large.");
	
	x = NUMdmatrix (1, my dimension, 1, nvectors);
	if (x == NULL) return NULL;
	y = NUMdmatrix (1, my dimension, 1, nvectors);
	if (y == NULL) goto end;
	
	for (j = 1; j <= nvectors; j++)
	{
		for (i = 1; i <= my dimension; i++)
		{
			x[i][j] =  my eigenvectors[evec_from+j-1][i];
			y[i][j] = thy eigenvectors[evec_from+j-1][i];	
		}
	}
	
	him = Procrustes_create (nvectors);
	if (him == NULL) goto end;

	if (! NUMProcrustes (x, y, my dimension, nvectors, his r, NULL, NULL)) forget (him);
	
end:

	NUMdmatrix_free (x, 1, 1);
	NUMdmatrix_free (y, 1, 1);
	return him;
}


/* End of file Eigen_and_Procrustes.c */
