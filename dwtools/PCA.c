/* PCA.c
 *
 * Principal Component Analysis
 * 
 * Copyright (C) 1993-2007 David Weenink
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
 djmw 20000511 PCA_and_TableOfReal_to_Configuration: added centralize option.
 	(later removed)
 djmw 20020327 PCA_and_TableOfReal_to_Configuration modified internals.
 djmw 20020418 Removed some causes for compiler warnings.
 djmw 20020502 modified call Eigen_and_TableOfReal_project_into.
 djmw 20030324 Added PCA_and_TableOfReal_getFractionVariance.
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20071201 Melder_warning<n>
*/

#include "PCA.h"
#include "NUMlapack.h"
#include "NUM2.h"
#include "TableOfReal_extensions.h"
#include "Eigen_and_SSCP.h"
#include "Eigen_and_TableOfReal.h"
#include "Configuration.h"

#include "oo_DESTROY.h"
#include "PCA_def.h"
#include "oo_COPY.h"
#include "PCA_def.h"
#include "oo_EQUAL.h"
#include "PCA_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "PCA_def.h"
#include "oo_WRITE_TEXT.h"
#include "PCA_def.h"
#include "oo_READ_TEXT.h"
#include "PCA_def.h"
#include "oo_WRITE_BINARY.h"
#include "PCA_def.h"
#include "oo_READ_BINARY.h"
#include "PCA_def.h"
#include "oo_DESCRIPTION.h"
#include "PCA_def.h"

static void info (I) {
	iam (PCA);
	
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of components: ", Melder_integer (my numberOfEigenvalues));
	MelderInfo_writeLine2 (L"Number of dimensions: ", Melder_integer (my dimension));
	MelderInfo_writeLine2 (L"Number of observations: ", Melder_integer (my numberOfObservations));
}

class_methods (PCA, Eigen)
	class_method_local (PCA, destroy)
	class_method_local (PCA, description)
	class_method_local (PCA, copy)
	class_method_local (PCA, equal)
	class_method_local (PCA, canWriteAsEncoding)
	class_method_local (PCA, writeText)
	class_method_local (PCA, readText)
	class_method_local (PCA, writeBinary)
	class_method_local (PCA, readBinary)
	class_method (info)
class_methods_end


PCA PCA_create (long numberOfComponents, long dimension)
{
	PCA me = new (PCA);
	if (! me || ! Eigen_init (me, numberOfComponents, dimension) ||
		! (my labels = NUMvector (sizeof (char *), 1, dimension)) ||
		! (my centroid = NUMdvector (1, dimension))) forget (me);
	return me;
}

void PCA_setNumberOfObservations (PCA me, long numberOfObservations)
{
	my numberOfObservations = numberOfObservations;
}

long PCA_getNumberOfObservations (PCA me)
{
	return my numberOfObservations;
}

void PCA_getEqualityOfEigenvalues (PCA me, long from, long to, int conservative,
	double *probability, double *chisq, long *ndf)
{
	long i, n, r; 
	double sum = 0, sumln = 0;
	
	*probability = 1; 
	*ndf = 0; 
	*chisq = 0;
	
	if ((from > 0 && to == from) || to > my numberOfEigenvalues) return;
	
	if (to <= from)
	{
		from = 1; 
		to = my numberOfEigenvalues;
	}
	for (i = from; i <= to; i++)
	{
		if (my eigenvalues[i] <= 0) break;
		sum += my eigenvalues[i];
		sumln += log (my eigenvalues[i]);
	}
	if (sum == 0) return;
	r = i - from;
	n = my numberOfObservations - 1;
	if (conservative) n -= from + (r * (2 * r + 1) + 2) / (6 * r);
	
	*ndf = r * (r + 1) / 2 - 1;
	*chisq = n * (r * log (sum / r) - sumln);
	*probability = NUMchiSquareQ (*chisq, *ndf);
}

PCA TableOfReal_to_PCA (I)
{
	iam (TableOfReal); 
	PCA thee;
	double **a = NULL;
	long i, j, m = my numberOfRows, n = my numberOfColumns;
	
	if (m < 2) return Melder_errorp1 (L"There is not enough data to perform a PCA.\n"
		"Your table has less than 2 rows.");

	if (m < n) Melder_warning1 (L"The number of rows in your table is less than the\n"
		"number of columns. ");
	
	if (NUMfrobeniusnorm (m, n, my data) == 0) return Melder_errorp1 
		(L"All values in your table are zero. ");
	
	if (! (thee = new (PCA)) ||
		! (a = NUMdmatrix_copy (my data, 1, m, 1, n)) ||
		! (thy centroid = NUMdvector (1, n))) goto end;
	
	for (j = 1; j <= n; j++)
	{
		double colmean = a[1][j];
		for (i = 2; i <= m; i++)
		{
			colmean += a[i][j];
		}
		colmean /= m;
		for (i = 1; i <= m; i++)
		{
			a[i][j] -= colmean;
		}
		thy centroid[j] = colmean;
	}

	if (! Eigen_initFromSquareRoot (thee, a, m, n) ||
		! (thy labels = NUMvector (sizeof (char *), 1, n))) goto end;
	
	NUMstrings_copyElements (my columnLabels, thy labels, 1, n);
	
	PCA_setNumberOfObservations (thee, m);
		
	/*
		The covariance matrix C = A'A / (N-1). However, we have calculated
		the eigenstructure for A'A. This has no consequences for the 
		eigenvectors, but the eigenvalues have to be divided by (N-1).
	*/
	
	for (i = 1; i <= thy numberOfEigenvalues; i++)
	{
		thy eigenvalues[i] /= (m - 1);
	}
	
end:

	NUMdmatrix_free (a, 1, 1);
	if (Melder_hasError	()) forget (thee);
	return thee;
}

Configuration PCA_and_TableOfReal_to_Configuration (PCA me, thou, 
	long numberOfDimensions)
{
	thouart (TableOfReal); 
	Configuration him; 
		
	if (numberOfDimensions == 0 || numberOfDimensions > my numberOfEigenvalues)
	{
		 numberOfDimensions = my numberOfEigenvalues;
	}
	
	him = Configuration_create (thy numberOfRows, numberOfDimensions);
	if (him == NULL) return NULL;

	if (! Eigen_and_TableOfReal_project_into (me, thee, 1, thy numberOfColumns,
		& him, 1, numberOfDimensions) || 
		! NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, 
			thy numberOfRows) ||
		! TableOfReal_setSequentialColumnLabels (him, 0, 0, L"pc", 1, 1))
			forget (him);
		
	return him;
}

TableOfReal PCA_and_Configuration_to_TableOfReal_reconstruct (PCA me, thou)
{
	thouart (Configuration); 
	TableOfReal him;
	long i, j, k, npc = thy numberOfColumns;
	
	if (thy numberOfColumns > my dimension) return Melder_errorp1 
		(L"PCA_and_Configuration_to_TableOfReal: The dimension of the Configuration must "
			"be less equal the dimension of the PCA.");
			
	if (npc > my numberOfEigenvalues)
	{
		npc = my numberOfEigenvalues;
		Melder_warning ("");
	}

	him = TableOfReal_create (thy numberOfRows, my dimension);
	if (him == NULL) return him;
	if (! NUMstrings_copyElements (my labels, his columnLabels, 1, my dimension))
	{
		forget (him);
		return NULL;
	}
	
	for (i = 1; i <= thy numberOfRows; i++)
	{
		double *hisdata = his data[i];
		/*
		for (j = 1; j <= my dimension; j++)
		{
			hisdata[j] = my centroid[j];
		}
		*/
		for (k = 1; k <= npc; k++)
		{
			double *evec = my eigenvectors[k], pc = thy data[i][k];
			for (j = 1; j <= my dimension; j++)
			{
				hisdata[j] += pc * evec[j];
			}
		}
	}
	
	return him;
}

double PCA_and_TableOfReal_getFractionVariance (PCA me, thou, long from, long to)
{
	thouart (TableOfReal); 
	SSCP s = NULL, sp = NULL;
	double fraction = NUMundefined;
	
	if (from < 1 || from > to || to > thy numberOfColumns) return NUMundefined;

	s = TableOfReal_to_SSCP (thee, 0, 0, 0, 0);
	if (s == NULL) return NUMundefined;
	sp = Eigen_and_SSCP_project (me, s);
	if (sp == NULL) goto end;
	fraction = SSCP_getFractionVariation (sp, from, to);
end:
	forget (s);
	forget (sp);
	return fraction;
}

TableOfReal PCA_to_TableOfReal_reconstruct1 (PCA me, wchar_t *numstring)
{
	TableOfReal him = NULL;
	Configuration c = NULL;
	long npc;
	double *pc = NUMstring_to_numbers (numstring, & npc);
	
	if (pc == NULL) return NULL;
	c = Configuration_create (1, npc);
	if (c != NULL)
	{
		long j;
		for (j = 1; j <= npc; j++)
		{
			c -> data [1][j] = pc[j];
		}
		him = PCA_and_Configuration_to_TableOfReal_reconstruct (me, c);
	}
	
	NUMdvector_free (pc, 1);
	forget (c);
	return him;		
}


/* End of file PCA.c */
