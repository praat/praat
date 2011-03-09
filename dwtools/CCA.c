/* CCA.c
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
 djmw 20020402 GPL header
 djme 20021008 removed SVD_sort
 djmw 20031023 Removed one argument from CCA_and_TableOfReal_scores
 djmw 20031106 Removed bug from CCA_and_TableOfReal_scores
 djmw 20031221 Removed bug: CCA_and_TableOfReal_scores (wrong dimensions to Eigen_project_into).
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20081119 Check in TableOfReal_to_CCA if TableOfReal_areAllCellsDefined
*/

#include "CCA_and_Correlation.h"
#include "NUM2.h"
#include "NUMlapack.h"
#include "SVD.h"
#include "Strings_extensions.h"
#include "TableOfReal_extensions.h"
#include "Eigen_and_TableOfReal.h"

#include "oo_DESTROY.h"
#include "CCA_def.h"
#include "oo_COPY.h"
#include "CCA_def.h"
#include "oo_EQUAL.h"
#include "CCA_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "CCA_def.h"
#include "oo_WRITE_TEXT.h"
#include "CCA_def.h"
#include "oo_WRITE_BINARY.h"
#include "CCA_def.h"
#include "oo_READ_TEXT.h"
#include "CCA_def.h"
#include "oo_READ_BINARY.h"
#include "CCA_def.h"
#include "oo_DESCRIPTION.h"
#include "CCA_def.h"

static void info (I)
{
	iam (CCA);
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of coefficients: ", Melder_integer (my numberOfCoefficients));
	MelderInfo_writeLine2 (L"ny: ", Melder_integer (my y -> dimension));
	MelderInfo_writeLine2 (L"nx: ", Melder_integer (my x -> dimension));
}

class_methods (CCA, Data)
	class_method_local (CCA, destroy)
	class_method_local (CCA, equal)
	class_method_local (CCA, canWriteAsEncoding)
	class_method_local (CCA, copy)
	class_method (info)
	class_method_local (CCA, readText)
	class_method_local (CCA, readBinary)
	class_method_local (CCA, writeText)
	class_method_local (CCA, writeBinary)
	class_method_local (CCA, description)
class_methods_end

CCA CCA_create (long numberOfCoefficients, long ny, long nx)
{
	CCA me = Thing_new (CCA);
	
	if (me == NULL) return NULL;
	my numberOfCoefficients = numberOfCoefficients;
	my yLabels = Thing_new (Strings);
	my xLabels = Thing_new (Strings);
	if ((my xLabels == NULL) || (my yLabels == NULL) ||
		((my y = Eigen_create (numberOfCoefficients, ny)) == NULL) ||
		((my x = Eigen_create (numberOfCoefficients, nx)) == NULL)) 
			forget (me);
	return me;
}

void CCA_drawEigenvector (CCA me, Graphics g, int x_or_y, long ivec, long first, long last,
	double ymin, double ymax, int weigh, double size_mm, const wchar_t *mark,	int connect, int garnish)
{
	Eigen e = my x; 
	Strings labels = my xLabels;
	if (x_or_y == 1)
	{
		e = my y; labels = my yLabels;
	}	
	Eigen_drawEigenvector (e, g, ivec, first, last, ymin, ymax, weigh, size_mm, mark,
		connect, labels -> strings, garnish);
}

double CCA_getEigenvectorElement (CCA me, int x_or_y, long ivec, long element)
{
	Eigen e = x_or_y == 1 ? my y : my x;
	return Eigen_getEigenvectorElement (e, ivec, element);
}

CCA TableOfReal_to_CCA (TableOfReal me, long ny)
{
	CCA thee = NULL;
	SVD svdy = NULL, svdx = NULL, svdc = NULL;
	double fnormy, fnormx, **uy, **vy, **ux, **vx, **uc, **vc;
	double **evecy, **evecx;
	long i, j, q, n = my numberOfRows;
	long numberOfZeroedy, numberOfZeroedx, numberOfZeroedc;
	long numberOfCoefficients;
	long nx = my numberOfColumns - ny;
		
	if (ny < 1 || ny > my numberOfColumns - 1) return Melder_errorp1 (L"Dimension of first part not correct.");

	if (! TableOfReal_areAllCellsDefined (me, 0, 0, 0, 0)) return NULL;

	/*
		The dependent 'part' of the CCA should be the smallest dimension.
	*/
		
	if (ny > nx) return Melder_errorp5 (L"The dimension of the dependent "
		"part (", Melder_integer (ny), L") must be less than or equal to the dimension of the "
		"independent part (", Melder_integer (nx), L").");
	
	if (n < ny) return Melder_errorp2 (L"The number of "
		"observations must be larger then ", Melder_integer (ny));

	/*
		Use svd as (temporary) storage, and copy data
	*/

	svdy = SVD_create (n, ny);
	if (svdy == NULL) goto end;
	svdx = SVD_create (n, nx);
	if (svdx == NULL) goto end;


	for (i = 1; i <= n; i++)
	{
		for (j = 1; j <= ny; j++)
		{
			svdy -> u[i][j] = my data[i][j];
		}
		for (j = 1; j <= nx; j++)
		{
			svdx -> u[i][j] = my data[i][ny + j];
		}
	}

 	uy = svdy -> u; vy = svdy -> v;
 	ux = svdx -> u; vx = svdx -> v;
	fnormy = NUMfrobeniusnorm (n, ny, uy);
	fnormx = NUMfrobeniusnorm (n, nx, ux);
	if (fnormy == 0 || fnormx == 0)
	{
		(void) Melder_errorp1 (L"One of the parts of the table contains only zeros.");
		goto end;
	}

	/*
		Centre the data and svd it.
	*/

	NUMcentreColumns (uy, 1, n, 1, ny, NULL);
	NUMcentreColumns (ux, 1, n, 1, nx, NULL);

	if (! SVD_compute (svdy) || ! SVD_compute (svdx)) goto end;

	numberOfZeroedy = SVD_zeroSmallSingularValues (svdy, 0);
	numberOfZeroedx = SVD_zeroSmallSingularValues (svdx, 0);

	/*
		Form the matrix C = ux' uy (use svd-object storage)
	*/

	svdc = SVD_create (nx, ny);
	if (svdc == NULL)  goto end;
 	uc = svdc -> u; vc = svdc -> v;

	for (i = 1; i <= nx; i++)
	{
		for (j = 1; j <= ny; j++)
		{
			double t = 0;
			for (q = 1; q <= n; q++)
			{
				t += ux[q][i] * uy[q][j];
			}
			uc[i][j] = t;
		}
	}

	if (! SVD_compute (svdc)) goto end;
	numberOfZeroedc = SVD_zeroSmallSingularValues (svdc, 0);
	numberOfCoefficients = ny - numberOfZeroedc;

	thee = CCA_create (numberOfCoefficients, ny, nx);
	if (thee == NULL) goto end;
	thy yLabels = strings_to_Strings (my columnLabels, 1, ny);
	if ( thy yLabels == NULL) goto end;
	thy xLabels = strings_to_Strings (my columnLabels, ny+1, my numberOfColumns);
	if ( thy xLabels == NULL) goto end;
	
	evecy = thy y -> eigenvectors;
	evecx = thy x -> eigenvectors;
	thy numberOfObservations = n;

	/*
		Y = Vy * inv(Dy) * Vc
		X = Vx * inv(Dx) * Uc
		For the eigenvectors we want a row representation:
		colums(Y) = rows(Y') = rows(Vc' * inv(Dy) * Vy')
		colums(X) = rows(X') = rows(Uc' * inv(Dx) * Vx')
		rows(Y') = evecy[i][j] = Vc[k][i] * Vy[j][k] / Dy[k]
		rows(X') = evecx[i][j] = Uc[k][i] * Vx[j][k] / Dx[k]
	*/

	for (i = 1; i <= numberOfCoefficients; i++)
	{
		double ccc = svdc -> d[i];
		thy y -> eigenvalues[i] = thy x -> eigenvalues[i] = ccc * ccc;
		for (j = 1; j <= ny; j++)
		{
			double t = 0;
			for (q = 1; q <= ny - numberOfZeroedy; q++)
			{
				t += vc[q][i] * vy[j][q] / svdy -> d[q];
			}
			evecy[i][j] = t;	
		}
		for (j = 1; j <= nx; j++)
		{
			double t = 0;
			for (q = 1; q <= nx - numberOfZeroedx; q++)
			{
				t += uc[q][i] * vx[j][q] / svdx -> d[q];
			}
			evecx[i][j] = t;	
		}
	}
	
	/*
		Normalize eigenvectors.
	*/
	NUMnormalizeRows (thy y -> eigenvectors, numberOfCoefficients, ny, 1);
	NUMnormalizeRows (thy x -> eigenvectors, numberOfCoefficients, nx, 1);
	
end:

	forget (svdy); 
	forget (svdx); 
	forget (svdc);
	Melder_assert (thy x -> dimension == thy xLabels -> numberOfStrings && 
		thy y -> dimension == thy yLabels -> numberOfStrings);
	if (Melder_hasError ()) forget (thee);
	
	return thee;
}


TableOfReal CCA_and_TableOfReal_scores (CCA me, TableOfReal thee, long numberOfFactors)
{
	TableOfReal him = NULL;
	long n = thy numberOfRows;
	long nx = my x -> dimension, ny = my y -> dimension;

	if (ny + nx != thy numberOfColumns) return Melder_errorp7 (L"The number "
		"of columns in the table (", Melder_integer (thy numberOfColumns), L") does not agree with "
		"the dimensions of the CCA object (ny + nx = ", Melder_integer (ny), L" + ", Melder_integer (nx), L").");

	if (numberOfFactors == 0) numberOfFactors = my numberOfCoefficients;
	if (numberOfFactors < 1 || numberOfFactors > my numberOfCoefficients)
		return Melder_errorp3 (L"The number of factors must be in interval "
			"[1, ", Melder_integer (my numberOfCoefficients), L"].");
	
	him = TableOfReal_create (n, 2 * numberOfFactors);
	if (him == NULL) return NULL;

	if (! NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, thy numberOfRows) ||
		! Eigen_and_TableOfReal_project_into (my y, thee, 1, ny, &him, 1, numberOfFactors) ||
		! Eigen_and_TableOfReal_project_into (my x, thee, ny + 1,
			thy numberOfColumns, &him, numberOfFactors + 1, his numberOfColumns) ||
		! TableOfReal_setSequentialColumnLabels (him, 1, numberOfFactors, L"y_", 1, 1) ||
		! TableOfReal_setSequentialColumnLabels (him, numberOfFactors + 1,
			his numberOfColumns, L"x_", 1, 1)) forget (him);

	return him;
}

TableOfReal CCA_and_TableOfReal_predict (CCA me, TableOfReal thee, long from)
{
	TableOfReal him = NULL;
	long ny = my y -> dimension, nx = my x -> dimension;
	long i, j, k, ncols, nev = my y -> numberOfEigenvalues;
	double *buf = NULL, **v = my y -> eigenvectors;
	double *d = my y -> eigenvalues;

	/* 
		We can only predict when we have the largest dimension as input
		and the number of coefficients equals the dimension of the smallest.
	*/
			
	if (ny != nev) return Melder_errorp1 (L"There are not enough correlations present for prediction.");
	
	if (from == 0) from = 1;
	ncols = thy numberOfColumns - from + 1;
	if (from < 1 || ncols != nx) return Melder_errorp3 (L"The number of columns to analyze must be equal to ",
		Melder_integer (nx), L"."); 
	
	/* ???? dimensions if nx .. ny ?? */
	 
	him = Eigen_and_TableOfReal_project (my x, thee, from, ny);
	if (him == NULL) return NULL;
	
	buf = NUMdvector (1, ny);
	if (buf == NULL) goto end;

	/*
		u = V a -> a = V'u
	*/
		
	for (i = 1; i <= thy numberOfRows; i++)
	{
		NUMdvector_copyElements (his data[i], buf, 1, ny);
		for (j = 1; j <= ny; j++)
		{
			double t = 0;
			for (k = 1; k <= ny; k++)
			{
				t += sqrt (d[k]) * v[k][j] * buf[k];
			}
			his data [i][j] = t;
		}
	}

end:
	NUMdvector_free (buf, 1);
	if (Melder_hasError ()) forget (him);
	return him;
}

TableOfReal CCA_and_TableOfReal_factorLoadings (CCA me, TableOfReal thee)
{
	Correlation c = NULL;
	TableOfReal him = NULL;
	
	c = TableOfReal_to_Correlation (thee);
	if (c == NULL) return NULL;
	him = CCA_and_Correlation_factorLoadings (me, c);
	forget (c);
	return him;
}

double CCA_getCorrelationCoefficient (CCA me, long index)
{
	if (index < 1 || index > my numberOfCoefficients)
		return NUMundefined;
	
	return sqrt (my y -> eigenvalues[index]);
}

void CCA_getZeroCorrelationProbability (CCA me, long index, double *chisq,
	long *ndf, double *probability)
{
	double lambda = 1, *ev = my y -> eigenvalues;
	long i, nev = my y -> numberOfEigenvalues;
	long ny = my y -> dimension, nx = my x -> dimension;
	
	*chisq = *probability = NUMundefined;
	*ndf = 0;
	
	if (index < 1 || index > nev) return;
	
	for (i = index; i <= nev; i++)
	{
		lambda *= (1 - ev[i]);
	}
	*ndf = (ny - index + 1) * (nx - index + 1);
	*chisq = - (my numberOfObservations - (ny + nx + 3) / 2) * log (lambda);
	*probability = NUMchiSquareQ (*chisq, *ndf);
}

/* End of file CCA.c */
