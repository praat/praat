/* CCA_and_Correlation.c
 * 
 * Copyright (C) 1993-2006 David Weenink
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
 djmw 20020525 GPL header.
 djmw 20060323 Stewart-Love redundancy added.
 djmw 20071022 Melder_error<n>
 */

#include "CCA_and_Correlation.h"
#include "NUM2.h"

TableOfReal CCA_and_Correlation_factorLoadings (CCA me, Correlation thee)
{
	TableOfReal him;
	long i, j, k, ny = my y -> dimension, nx = my x -> dimension;
	double t, **evecy = my y -> eigenvectors, **evecx = my x -> eigenvectors;

	if (ny + nx != thy numberOfColumns) return Melder_errorp1 (L"The number "
		"of columns in the Correlation object must equal the sum of the "
		"dimensions in the CCA object");

	him = TableOfReal_create (2 * my numberOfCoefficients, thy numberOfColumns);
	if (him == NULL) return NULL;
	
	if (! NUMstrings_copyElements (thy columnLabels, his columnLabels, 
			1, thy numberOfColumns) ||
		! TableOfReal_setSequentialRowLabels (him, 1, my numberOfCoefficients,
			L"dv", 1, 1) ||
		! TableOfReal_setSequentialRowLabels (him, my numberOfCoefficients + 1,
			2 * my numberOfCoefficients, L"iv", 1, 1))
	{
		forget (him);
		return NULL;
	}

	for (i = 1; i <= thy numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfCoefficients; j++)
		{
			t = 0;
			for (k = 1; k <= ny; k++)
			{
				t += thy data[i][k] * evecy[j][k];
			}
			his data[j][i] = t;
		}
		for (j = 1; j <= my numberOfCoefficients; j++)
		{
			t = 0;
			for (k = 1; k <= nx; k++)
			{
				t += thy data[i][ny + k] * evecx[j][k];
			}
			his data[my numberOfCoefficients + j][i] = t;
		}
	}
	return him;
}

static int _CCA_and_Correlation_check (CCA me, Correlation thee, int canonicalVariate_from, int canonicalVariate_to)
{
	if (my y -> dimension + my x -> dimension != thy numberOfColumns) return Melder_error1
		(L"The number of columns in the Correlation object must equal the sum of the dimensions in the CCA object");
	if (canonicalVariate_to < canonicalVariate_from) return Melder_error1
		(L"The second value in the \"Canonical variate range\" must be equal or larger than the first.");
	if (canonicalVariate_from < 1 || canonicalVariate_to > my numberOfCoefficients) return Melder_error3
		(L"The \"Canonical variate range\" must be within the interval [1, ", Melder_integer (my numberOfCoefficients), L"].");
	return 1;
}

double CCA_and_Correlation_getVarianceFraction (CCA me, Correlation thee, int x_or_y, int canonicalVariate_from, int canonicalVariate_to)
{
	long i, icv, ioffset, j, n;
	double **evec, varianceFraction = 0;

	if (! _CCA_and_Correlation_check (me, thee, canonicalVariate_from, canonicalVariate_to)) return NUMundefined;

	/* For the formulas see:
		William W. Cooley & Paul R. Lohnes (1971), Multivariate data Analysis, John Wiley & Sons, pag 170-...
		varianceFraction = s'.s / n,
		where e.g. for the independent set x:
			s = Rxx . c,
		and Rxx is the correlation matrix of x,
		c is the factor coefficient for x,
		nx is the dimension of x,
		The factor coefficient c is the eigenvector e for x scaled by the st.dev of the component,
		i.e. c = e / sqrt (e'.R.e) (pag 32-33).
		Therefore:
		varianceFraction = s'.s / n = c'Rxx' Rxx c/n = (e'.Rxx' Rxx.e) /(e'.Rxx.e) * 1/n
		(for one can. variate)
	*/
	
	if (x_or_y == 1) /* y: dependent set */
	{
		n = my y -> dimension;
		evec = my y -> eigenvectors;
		ioffset = 0;
	}
	else
	{
		n = my x -> dimension;
		evec = my x -> eigenvectors;
		ioffset = my y -> dimension;
	}
	for (icv = canonicalVariate_from; icv <= canonicalVariate_to; icv++)
	{
		double variance = 0, varianceScaling = 0;

		for (i = 1; i <= n; i++)
		{
			double si = 0;
			for (j = 1; j <= n; j++)
			{
				si += thy data[ioffset+i][ioffset+j] * evec[icv][j]; /* Rxx.e */
			}
			variance += si * si; /* (Rxx.e)'(Rxx.e) =  e'.Rxx'.Rxx.e */
			varianceScaling +=  evec[icv][i] * si; /* e'.Rxx.e*/
		}
		varianceFraction += (variance / varianceScaling) / n;
	}
	
	return varianceFraction;
}

double CCA_and_Correlation_getRedundancy_sl (CCA me, Correlation thee, int x_or_y, int canonicalVariate_from, int canonicalVariate_to)
{
	long icv;
	double redundancy = 0;

	if (! _CCA_and_Correlation_check (me, thee, canonicalVariate_from, canonicalVariate_to)) return NUMundefined;
	
	for (icv = canonicalVariate_from; icv <= canonicalVariate_to; icv++)
	{
		double varianceFraction = CCA_and_Correlation_getVarianceFraction (me, thee, x_or_y, icv, icv);
		if (varianceFraction == NUMundefined) return NUMundefined;
		redundancy += varianceFraction * my y -> eigenvalues[icv];
	}
		
	return redundancy;
}

/* End of file CCA_and_Correlation.h */
