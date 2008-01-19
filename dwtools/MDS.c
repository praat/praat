/* MDS.c
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
 djmw 20020408 GPL
 djmw 20020513 Applied TableOfReal_setSequential{Column/Row}Labels
 djmw 20030623 Modified calls to NUMeigensystem_d
 djmw 20040309 Extra tests for empty objects.
 djmw 20061218 Changed to Melder_information<x> format.
 djmw 20071022 Removed unused code.
 djmw 20071106 drawSplines: to wchar_t
*/

#include "SVD.h"
#include "Matrix_extensions.h"
#include "TableOfReal_extensions.h"
#include "MDS.h"
#include "SSCP.h"
#include "PCA.h"

#define TINY 1e-30

/********************** NUMERICAL STUFF **************************************/
	
static void NUMdmatrix_into_vector (double **m, double *v, long r1, long r2,
	long c1, long c2)
{
	long i, j, k = 1;
	
	for(i = r1; i <= r2; i++)
	{
		for (j=c1; j <= c2; j++)
		{
			v[k++] = m[i][j];
		}
	}
}

static void NUMdvector_into_matrix (const double *v, double **m, 
	long r1, long r2, long c1, long c2)
{
	long i, j, k = 1;
	
	for(i = r1; i <= r2; i++)
	{
		for (j=c1; j <= c2; j++)
		{
			m[i][j] = v[k++];
		}
	}
}

static void NUMdmatrix_normalizeRows (double **m, long nr, long nc)
{
	long i, j;
	
	for (i = 1; i <= nr; i++)
	{
		double rowSum = 0;
		for (j = 1; j <= nc; j++)
		{
			rowSum += m[i][j];
		}
		
		if (rowSum != 0)
		{
			for (j=1; j <= nc; j++)
			{
				m[i][j] /= rowSum;
			}
		}
	}
}

static long NUMdmatrix_countZeros (double **m, long nr, long nc)
{
	long i, j, nZeros = 0;
	
	for (i = 1; i <= nr; i++)
	{
		for (j=1; j <= nc; j++)
		{
			if (m[i][j] == 0) nZeros++;
		}
	}
	return nZeros;
}

static int NUMsort3 (double *data, long *iPoint, long *jPoint, long ifrom, 
	long ito, int ascending)
{
	int status = 0;
	long j, n = ito - ifrom + 1, *indx = NULL, *itmp = NULL;
	double *atmp = NULL;
	
	if (ifrom > ito || ifrom < 1) return 0;
	if (n == 1) return 1;
	
	if (! (indx = NUMlvector (ifrom, ito)) ||
		! (atmp = NUMdvector (ifrom, ito)) ||
		! (itmp = NUMlvector (ifrom, ito))) goto end;
	
	NUMindexx_d (data + ifrom - 1, n, indx + ifrom - 1);
		
	if (! ascending)
	{
		for (j = ifrom; j <= ifrom + n / 2; j++)
		{
			long tmp = indx[j];
			indx[j] = indx[ito - j + ifrom]; 
			indx[ito - j + ifrom] = tmp;
		}
	}
	for (j = ifrom; j <= ito; j++) indx[j] += ifrom -1;
	for (j = ifrom; j <= ito; j++) atmp[j] = data[j];
	for (j = ifrom; j <= ito; j++) data[j] = atmp[indx[j]];
	for (j = ifrom; j <= ito; j++) itmp[j] = iPoint[j];
	for (j = ifrom; j <= ito; j++) iPoint[j] = itmp[indx[j]];
	for (j = ifrom; j <= ito; j++) itmp[j] = jPoint[j];
	for (j = ifrom; j <= ito; j++) jPoint[j] = itmp[indx[j]];
	status = 1;
	
end:

	NUMlvector_free (itmp, ifrom);
	NUMdvector_free (atmp, ifrom);
	NUMlvector_free (indx, ifrom);
	return status;	
}

/************ Configurations & Similarity **************************/

Distances Configurations_to_Distances (Configurations me)
{
	Distances thee = NULL;
	long i;
	
	thee = Distances_create ();
	if (thee == NULL) return NULL;
	for (i = 1; i <= my size; i++)
	{
		wchar_t *name = Thing_getName (my item[i]);
		Distance d = Configuration_to_Distance (my item[i]);
		
		if (d == NULL || ! Collection_addItem (thee, d))
		{
			forget (thee); goto end;
		}
		Thing_setName (d, name);
	}
	
end:

	return thee;
}

Similarity Configurations_to_Similarity_cc (Configurations me, Weight weight)
{
	Similarity thee = NULL;
	Distances d = Configurations_to_Distances (me);

	if (d == NULL) return NULL;
	thee = Distances_to_Similarity_cc (d, weight);
	forget (d);
	return thee;
}

Similarity Distances_to_Similarity_cc (Distances me, Weight w)
{
	Similarity thee = NULL;
	Distance x;
	long i, j;
	int no_weight = w == NULL;
	
	if (my size == 0) return NULL;
	
	if (! TablesOfReal_checkDimensions (me))
	{
		return Melder_errorp1 (L"Distances_to_Similarity_cc: "
			"All matrices must have the same dimensions.");
	}
	
	x = my item[1];
	if ((no_weight && ! (w = Weight_create (x -> numberOfRows)))) goto end;
	if (! (thee = Similarity_create (my size))) goto end;
	
	for (i = 1; i <= my size; i++)
	{
		wchar_t *name = Thing_getName (my item[i]);
		thy data[i][i] = 1;
		TableOfReal_setRowLabel (thee, i, name);
		TableOfReal_setColumnLabel (thee, i, name);
		for (j=i+1; j <= my size; j++)
		{
			thy data[i][j] = thy data[j][i] = 
				Distance_Weight_congruenceCoefficient (my item[i], 
					my item[j], w);	
		}
	}
	
end:
	
	if (no_weight) forget (w);
	return thee;
}


/***************** Transformator **********************************************/

static Distance classTransformator_transform (I, MDSVec vec, Distance dist,
	Weight w)
{
	iam (Transformator);
	Distance thee;
	long i;
	(void) w;
	
	thee = Distance_create (my numberOfPoints);
	if (thee == NULL) return NULL;
	
	if (! TableOfReal_copyLabels (dist, thee, 1, 1)) goto end;
	
	/*
		Absolute scaling
	*/
	
	for (i = 1; i <= vec -> nProximities; i++)
	{
		long ii = vec -> iPoint[i], jj = vec -> jPoint[i];
		thy data[ii][jj] = thy data[jj][ii] = vec -> proximity[i];
	}
	
end:

	return thee;
}

class_methods (Transformator, Thing)
	class_method_local (Transformator, transform)
class_methods_end

int Transformator_init (I, long numberOfPoints)
{
	iam (Transformator);
	
	my numberOfPoints = numberOfPoints;
	my normalization = 1;
	return 1;
}

Transformator Transformator_create (long numberOfPoints)
{
	Transformator me = new (Transformator);
	
	if (me == NULL || ! Transformator_init (me, numberOfPoints))
	{
		forget (me); return NULL;
	}
	
	my normalization = 0;
	return me;
}

Distance Transformator_transform (I, MDSVec vec, Distance d, Weight w)
{
	iam (Transformator);
	
	if (my numberOfPoints != vec -> nPoints ||
		my numberOfPoints != d -> numberOfRows ||
		d -> numberOfRows != w -> numberOfRows)
	{
		return Melder_errorp1 (L"Transformator_transform: Dimensions do "
			"not agree.");
	}
	
	return our transform ? our transform (me, vec, d, w) : NULL;
}

static Distance classRatioTransformator_transform (I, MDSVec vec, Distance d, 
	Weight w)
{
	iam (RatioTransformator);
	Distance thee;
	long i;
	double etaSq = 0, rho = 0; 

	if (! (thee = Distance_create (my numberOfPoints)) ||
		! TableOfReal_copyLabels (d, thee, 1, 1)) goto end;

	/*
		Determine ratio (eq. 9.4)
	*/
	
	for (i = 1; i <= vec -> nProximities; i++)
	{
		long ii = vec -> iPoint[i], jj = vec -> jPoint[i];
		double delta_ij = vec -> proximity[i], d_ij = d -> data[ii][jj];
		double tmp = w -> data[ii][jj] * delta_ij * delta_ij;
		etaSq += tmp;
		rho += tmp * d_ij * d_ij;
	}
		
	/*
		transform
	*/
	
	if (etaSq == 0)
	{
		forget (thee);
	}
	else
	{ 
		my ratio = rho / etaSq;
		for (i = 1; i <= vec -> nProximities; i++)
		{
			long ii = vec -> iPoint[i], jj = vec -> jPoint[i];
			thy data[ii][jj] = thy data[jj][ii] = 
				my ratio * vec -> proximity[i];
		}
	}
	
	if (my normalization) Distance_Weight_smacofNormalize (thee, w);
	
end:	
	return thee;
}

class_methods (RatioTransformator, Transformator)
	class_method_local (RatioTransformator, transform)
class_methods_end

RatioTransformator RatioTransformator_create (long numberOfPoints)
{
	RatioTransformator me = new (RatioTransformator);
	
	if (me == NULL ||
		! Transformator_init (me, numberOfPoints)) forget (me);
	return me;
}

static Distance classMonotoneTransformator_transform (I, MDSVec vec, 
	Distance d, Weight w)
{
	iam (MonotoneTransformator);
	Distance thee;
	
	thee = MDSVec_Distance_monotoneRegression (vec, d, my tiesProcessing);
	if (thee && my normalization) Distance_Weight_smacofNormalize (thee, w);
	return thee;
}

class_methods (MonotoneTransformator, Transformator)
	class_method_local (MonotoneTransformator, transform)
class_methods_end

MonotoneTransformator MonotoneTransformator_create (long numberOfPoints)
{
	MonotoneTransformator me = new (MonotoneTransformator);

	if (me == NULL) return NULL;
	
	if (Transformator_init (me, numberOfPoints))
	{
		my tiesProcessing = MDS_PRIMARY_APPROACH;
	}
	else
	{		
		forget (me);
	}
	return me;
}

void MonotoneTransformator_setTiesProcessing (MonotoneTransformator me, 
	int tiesProcessing)
{
	my tiesProcessing = tiesProcessing;
}

static void classISplineTransformator_destroy (I)
{
	iam (ISplineTransformator);
	NUMdvector_free (my b, 1);
	NUMdvector_free (my knot, 1);
	NUMdmatrix_free (my m, 1, 1);
	inherited (ISplineTransformator) destroy (me);
}

static Distance classISplineTransformator_transform (I, MDSVec vec, 
	Distance dist, Weight w)
{
	iam (ISplineTransformator); 
	Distance thee = NULL;
	double *d = NULL, tol = 1e-6;
	long i, j, itermax = 20, nx = vec -> nProximities;
	long nKnots = my numberOfInteriorKnots + my order + my order + 2;

	thee = Distance_create (dist -> numberOfRows);
	if (thee == NULL) goto end;
	
	if (! TableOfReal_copyLabels (dist, thee, 1, -1)) goto end;
	
	d = NUMdvector (1, nx);
	if (d == NULL) goto end;
		
	for (i=1; i <= nx; i++)
	{
		d[i] = dist -> data [vec -> iPoint[i]] [vec -> jPoint[i]];
	}
	
	/*
		Process knots. Put interior knots at quantiles.
		Guarantee that for each proximity x[i]: knot[j] <= x[i] < knot[j+1] 
	*/
	
	for (i=1; i <= my order+1; i++)
	{
		my knot[i] = vec -> proximity[1];
		my knot[nKnots-i+1] = vec -> proximity[nx] * 1.000001;
	}
	for (i = 1; i <= my numberOfInteriorKnots; i++)
	{
		double fraction = (double) i / (my numberOfInteriorKnots + 1);
		my knot[my order+1+i] = NUMquantile (nx, vec -> proximity, fraction);
	}
	 
	/*
		Calculate data matrix m.
	*/
	
	for (i = 1; i <= nx; i++)
	{
		double y, x = vec -> proximity[i];
		my m[i][1] = 1;
		for (j = 2; j <= my numberOfParameters; j++)
		{
			if (! NUMispline (my knot, nKnots, my order, j - 1, x, &y))
			{
				(void) Melder_error6 (L"classISplineTransformator_transform: "
					"I-spline[,", Melder_integer (j-1), L"], data[", Melder_integer (i), L"d] = ", Melder_double (x));
				goto end;
			}
			my m[i][j] = y;
		}
	}
	
	NUMsolveNonNegativeLeastSquaresRegression (my m, nx, my numberOfParameters, 
		d, tol, itermax, my b);
	
	for (i = 1; i <= nx; i++)
	{
		long ii = vec->iPoint[i], jj = vec->jPoint[i];
		double r = 0;
		
		for (j = 1; j <= my numberOfParameters; j++)
		{
			r += my m[i][j] * my b[j];
		}
		thy data[ii][jj] = thy data[jj][ii] = r;
	}
	
	if (my normalization) Distance_Weight_smacofNormalize (thee, w);
	
end:

	NUMdvector_free (d, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

class_methods (ISplineTransformator, Transformator)
	class_method_local (ISplineTransformator, destroy)
	class_method_local (ISplineTransformator, transform)
class_methods_end


ISplineTransformator ISplineTransformator_create (long numberOfPoints, 
	long numberOfInteriorKnots, long order)
{
	ISplineTransformator me = new (ISplineTransformator);
	long i, numberOfKnots;
	long nData = (numberOfPoints - 1) * numberOfPoints / 2;

	if (me == NULL) return NULL;	
	if (! Transformator_init (me, numberOfPoints)) goto end; 
	
	/*
		1 extra parameter for the intercept.
		2 extra knots for the I-spline.
	*/
	
	my numberOfParameters = numberOfInteriorKnots + order + 1;
	numberOfKnots = numberOfInteriorKnots + order + order + 2;

	my b = NUMdvector (1, my numberOfParameters);
	if (my b == NULL) goto end;
	
	my knot = NUMdvector (1, numberOfKnots);
	if (my knot == NULL) goto end;
	
	my m = NUMdmatrix (1, nData, 1, my numberOfParameters);
	if (my m == NULL) goto end;
	
	for (i = 1; i <= my numberOfParameters; i++)
	{
		my b[i] = NUMrandomUniform (0, 1);
	}
	
	my numberOfInteriorKnots = numberOfInteriorKnots;
	my order = order;

end:

	if (Melder_hasError ()) forget (me);
	return me;
}

/***************** CONTINGENCYTABLE **************************************/

static void classContingencyTable_info (I)
{
	iam (ContingencyTable); 
	long ndf;
    double h, hx, hy, hygx, hxgy, uygx, uxgy, uxy, chisq;

    ContingencyTable_entropies (me, &h, &hx, &hy, &hygx, &hxgy, 
		&uygx, &uxgy, &uxy);
    ContingencyTable_chisq (me, &chisq, &ndf);

	Melder_information2 (L"Number of rows: ", Melder_integer (my numberOfRows));
	Melder_information2 (L"Number of columns: ", Melder_integer (my numberOfColumns));
	Melder_information1 (L"Entropies (y is row variable):");
	Melder_information2 (L"  Total: ", Melder_double (h));
	Melder_information2 (L"  Y: ", Melder_double (hy));
	Melder_information2 (L"  X: ", Melder_double (hx));
	Melder_information2 (L"  Y given x: ", Melder_double (hygx));
	Melder_information2 (L"  X given y: ", Melder_double (hxgy));
	Melder_information2 (L"  Dependency of y on x: ", Melder_double (uygx));
	Melder_information2 (L"  Dependency of x on y: ", Melder_double (uxgy));
	Melder_information2 (L"  Symmetrical dependency: ", Melder_double (uxy));
	Melder_information2 (L"  Chi squared: ", Melder_double (chisq));
	Melder_information2 (L"  Degrees of freedom: ", Melder_integer (ndf));
	Melder_information2 (L"  Probability: ", Melder_double (ContingencyTable_chisqProbability (me)));	
}

class_methods (ContingencyTable, TableOfReal)
	class_method_local (ContingencyTable, info)
class_methods_end

ContingencyTable ContingencyTable_create (long numberOfRows, long numberOfColumns)
{
	ContingencyTable me = new (ContingencyTable);
	if (! me || ! TableOfReal_init (me, numberOfRows, numberOfColumns)) forget (me);
	return me;
}

Configuration ContingencyTable_to_Configuration_ca (ContingencyTable me,
	long numberOfDimensions, int scaling)
{
	Configuration thee = NULL;
	SVD svd = NULL;
	long i, j, nr = my numberOfRows, nc = my numberOfColumns;
	long dimmin = nr < nc ? nr : nc;
	double **h = NULL, *rowsum = NULL, *colsum = NULL, sum = 0;
	
	if (! (h = NUMdmatrix_copy (my data, 1, nr, 1, nc)) ||
		! (rowsum = NUMdvector (1, nr)) ||
		! (colsum = NUMdvector (1, nc)) ||
		! (thee = Configuration_create (nr + nc, numberOfDimensions))) goto end;

	if (numberOfDimensions == 0) numberOfDimensions = dimmin - 1;
	if (numberOfDimensions >= dimmin)
	{
		return Melder_errorp1 (L"ContingencyTable_to_Configuration_ca: Dimension too high.");
	}
	
	/*
		Ref: A. Gifi (1990), Nonlinear Multivariate Analysis, Wiley & Sons,
			reprinted 1996,
		Chapter 8, Multidimensional scaling and Correspondence Analysis.
		Get row and column marginals
	*/
	
    for (i = 1; i <= nr; i++)
    {
    	for (j = 1; j <= nc; j++)
		{
	    	rowsum[i] += my data[i][j];
	    	colsum[j] += my data[i][j];
		}
		if (rowsum[i] <= 0)
		{
			(void) Melder_error3 (L"ContingencyTable_to_Configuration_ca: Empty row: ",	Melder_integer (i), L".");
			goto end;
		}
	    sum += rowsum[i];
	}

	for (j = 1; j <= nc; j++) 
	{
		if (colsum[j] <= 0)
		{
			(void) Melder_error3 (L"ContingencyTable_to_Configuration_ca: Empty column: ", Melder_integer (i), L".");
			goto end;
		}
	}
	
	/*
		Remove trivial singular vectors (Eq. 8.24),
		construct Dr^(-1/2) H Dc^(-1/2) - Dr^(1/2) uu' Dc^(1/2) / N
	*/
	
	for (i = 1; i <= nr; i++)
	{
		for (j = 1; j <= nc; j++)
		{
			double rc = sqrt (rowsum[i] * colsum[j]);
			h[i][j] = h[i][j] / rc - rc / sum;
		}
	}
	
	/*
		Singular value decomposition of h
	*/
	
	if (! (svd = SVD_create_d (h, nr, nc))) goto end;
	(void) SVD_zeroSmallSingularValues (svd, 0);
	
	/*
		Scale row vectors and column vectors to configuration.
	*/
	
	for (j = 1; j <= numberOfDimensions; j++)
	{
		double rootsum = sqrt (sum), xfactor, yfactor, lambda = svd -> d[j];
		if (scaling == 1)
		{
			/*
				Scale row points in the centre of gravity of 
				column points (eq 8.5.a)
			*/
			xfactor = rootsum * lambda;
			yfactor = rootsum;
		}
		else if (scaling == 2)
		{
			/*
				Scale column points in the centre of gravity of 
				row points (8.5.b)
			*/
			xfactor = rootsum;
			yfactor = rootsum * lambda;
		}
		else if (scaling == 3)
		{
			/*
				Treat row and columns symmetrically (8.5.c).
			*/
			xfactor = yfactor = rootsum * sqrt (lambda);
		}
		else
		{
			goto end;
		}
		for (i = 1; i <= nr; i++)
		{
			thy data[i][j] = svd -> u[i][j] * xfactor / sqrt (rowsum[i]);
		}
		for (i = 1; i <= nc; i++)
		{
			thy data[nr+i][j] = svd -> v[i][j] * yfactor / sqrt (colsum[i]);
		}
	}
	
	(void) TableOfReal_setSequentialColumnLabels (thee, 0, 0, NULL, 1, 1);
	(void) NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, nr);
	for (i = 1; i <= nc; i++)
	{
		if (my columnLabels[i])
		{
			TableOfReal_setRowLabel (thee, nr + i, my columnLabels[i]);
		}
	}
	 
end:
	NUMdmatrix_free (h, 1, 1);
	forget (svd);
	NUMdvector_free (rowsum, 1);
	NUMdvector_free (colsum, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

double ContingencyTable_chisqProbability (ContingencyTable me)
{
	double chisq;
	long df;	
	ContingencyTable_chisq (me, &chisq, &df);
	if (chisq == 0 && df == 0) return 0;
	return NUMchiSquareQ (chisq, df);
}

double ContingencyTable_cramersStatistic (ContingencyTable me)
{
	double chisq, sum = 0;
	long i, j, df, nr = my numberOfRows, nc = my numberOfColumns, nmin = nr;
	
	if (nr == 1 || nc == 1) return 0;
	
	for (i = 1; i <= nr; i++)
	{
		for (j = 1; j <= nc; j++)
		{
			sum += my data[i][j];
		}
	}
	
	if (nc < nr) nmin = nc;
	nmin--;
	
	ContingencyTable_chisq (me, &chisq, &df);
	if (chisq == 0 && df == 0) return 0;
	return sqrt (chisq / (sum * nmin));
}

double ContingencyTable_contingencyCoefficient (ContingencyTable me)
{
	double chisq, sum = 0;
	long i, j, df, nr = my numberOfRows, nc = my numberOfColumns;
		
	for (i = 1; i <= nr; i++)
	{
		for (j = 1; j <= nc; j++)
		{
			sum += my data[i][j];
		}
	}
	
	ContingencyTable_chisq (me, &chisq, &df);
	if (chisq == 0 && df == 0) return 0;
	return sqrt (chisq / (chisq + sum));
}

void ContingencyTable_chisq (ContingencyTable me, double *chisq, long *df)
{
	long i, j, nr = my numberOfRows, nc = my numberOfColumns;
	double sum = 0, *rowsum = NULL, *colsum = NULL;

	*chisq = 0; *df = 0;
	if (! (rowsum = NUMdvector (1, nr)) ||
		! (colsum = NUMdvector (1, nc))) goto end;

	/*
		row and column marginals
	*/
	
    for (i = 1; i <= my numberOfRows; i++)
    {
    	for (j = 1; j <= my numberOfColumns; j++)
		{
	    	rowsum[i] += my data[i][j];
	    	colsum[j] += my data[i][j];
		}
	    sum += rowsum[i];
	}

	for (i = 1; i <= my numberOfRows; i++)
	{
		if (rowsum[i] == 0) --nr;
	}
	for (j = 1; j <= my numberOfColumns; j++)
	{
		if (colsum[j] == 0) --nc;
	}
	
	*df = (nr - 1) * (nc - 1);
	for (i = 1; i <= my numberOfRows; i++)
	{
		double expt, tmp;
		if (rowsum[i] == 0) continue;
		for (j = 1; j <= my numberOfColumns; j++)
		{
			if (colsum[j] == 0) continue;
			expt = rowsum[i] * colsum[j] / sum;
			tmp = my data[i][j] - expt;
			*chisq += tmp * tmp / expt;
		}
	}
	
end:

	NUMdvector_free (rowsum, 1);
	NUMdvector_free (colsum, 1);
}

void ContingencyTable_entropies (ContingencyTable me, double *h, double *hx,
	double *hy, double *hygx, double *hxgy, double *uygx, double *uxgy, 
	double *uxy)
{
	long i, j; 
    double *rowsum = NULL, *colsum = NULL, p, sum = 0.0;
    
    *h = *hx = *hy = *hxgy = *hygx = *uygx = *uxgy = *uxy = 0;
    
    if (! (rowsum = NUMdvector (1, my numberOfRows)) ||
		! (colsum = NUMdvector (1, my numberOfColumns))) goto end;
	
	/*
		row and column totals
	*/
	
    for (i = 1; i <= my numberOfRows; i++)
    {
    	for (j = 1; j <= my numberOfColumns; j++)
		{
	    	rowsum[i] += my data[i][j];
	    	colsum[j] += my data[i][j];
		}
		sum += rowsum[i];
	}
	
	/*
		Entropy of x distribution
	*/
    
    for (j = 1; j <= my numberOfColumns; j++)
    {
    	if (colsum[j] > 0)
    	{
    		p = colsum[j] / sum;
    		*hx -= p * NUMlog2 (p);
    	}
    }
    
	/*
		Entropy of y distribution
	*/
    
    for (i = 1; i <= my numberOfRows; i++)
    {
    	if (rowsum[i] > 0)
    	{
    		p = rowsum[i] / sum;
    		*hy -= p * NUMlog2 (p);
    	}
    }
    
    /*
    	Total entropy
    */
    
    for (i = 1; i <= my numberOfRows; i++)
    {
    	for (j = 1; j <= my numberOfColumns; j++)
    	{
			if (my data[i][j] > 0)
			{
				p = my data[i][j] / sum;
				*h -= p * NUMlog2 (p);
			}
		}
	}
	
	/*
		Conditional entropies
	*/
	
    *hygx = *h - *hx;
    *hxgy = *h - *hy;
    *uygx = (*hy - *hygx) / (*hy + TINY);
    *uxgy = (*hx - *hxgy) / (*hx + TINY);
    *uxy = 2.0 * (*hx + *hy - *h) / (*hx + *hy + TINY);
    
end:

    NUMdvector_free (rowsum, 1);
    NUMdvector_free (colsum, 1);
}

/********** CASTS FROM & TO TABLEOFREAL **********************************/

ContingencyTable Confusion_to_ContingencyTable (Confusion me)
{
	ContingencyTable thee = Data_copy (me);
	
	if (thee == NULL) return NULL;
	Thing_overrideClass (thee, classContingencyTable);
	return thee;
}

Dissimilarity TableOfReal_to_Dissimilarity (I)
{
	iam (TableOfReal); 
	Dissimilarity thee;
	
	if (my numberOfRows != my numberOfColumns) return Melder_errorp1
		(L"TableOfReal_to_Dissimilarity: TableOfReal must be a square matrix.");
	if (! TableOfReal_checkPositive (me) ||
		! (thee = Data_copy (me))) return NULL;
	Thing_overrideClass (thee, classDissimilarity);
	return thee;
}

Similarity TableOfReal_to_Similarity (I)
{
	iam (TableOfReal); 
	Similarity thee;
	
	if (my numberOfRows != my numberOfColumns) return Melder_errorp1
		(L"TableOfReal_to_Similarity: TableOfReal must be a square matrix.");
	if (! TableOfReal_checkPositive (me) ||
		! (thee = Data_copy (me))) return NULL;
	Thing_overrideClass (thee, classSimilarity);
	return thee;
}

Distance TableOfReal_to_Distance (I)
{
	iam (TableOfReal); 
	Distance thee;
	
	if (my numberOfRows != my numberOfColumns) return Melder_errorp1
		(L"TableOfReal_to_Distance: TableOfReal must be a square matrix.");
	if (! TableOfReal_checkPositive (me) ||
		! (thee = Data_copy (me))) return NULL;
	Thing_overrideClass (thee, classDistance);
	return thee;
}

Salience TableOfReal_to_Salience (I)
{
	iam (TableOfReal); 
	Salience thee;
	
	if (! TableOfReal_checkPositive (me) ||
		! (thee = Data_copy (me))) return NULL;
	Thing_overrideClass (thee, classSalience);
	return thee;
}

Weight TableOfReal_to_Weight (I)
{
	iam (TableOfReal);
	Weight thee;
	if (! TableOfReal_checkPositive (me) ||
		! (thee = Data_copy (me))) return NULL;
	Thing_overrideClass (thee, classWeight);
	return thee;
}

ScalarProduct TableOfReal_to_ScalarProduct (I)
{
	iam (TableOfReal); ScalarProduct thee;
	if (my numberOfRows != my numberOfColumns) return Melder_errorp1
		(L"TableOfReal_to_ScalarProduct: TableOfReal must be a square matrix.");
	if (! (thee = Data_copy (me))) return NULL;
	Thing_overrideClass (thee, classScalarProduct);
	return thee;
}

ContingencyTable TableOfReal_to_ContingencyTable (I)
{
	iam (TableOfReal); ContingencyTable thee;
	if (! TableOfReal_checkPositive (me) ||
		! (thee = Data_copy (me))) return NULL;
	Thing_overrideClass (thee, classContingencyTable);
	return thee;
}

/**************** Covariance & Correlation to Configuration *****************/

Configuration SSCP_to_Configuration (I, long numberOfDimensions)
{
	iam (SSCP); 
	Configuration thee = NULL; 
	PCA a = NULL; 
	long i, j, k;
	
	if (! (thee = Configuration_create (my numberOfRows, numberOfDimensions)) ||
		! (a = SSCP_to_PCA (me)) ||
		! TableOfReal_setSequentialColumnLabels (thee, 0, 0, NULL, 1, 1)) goto end;
		
    for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = 1; j <= numberOfDimensions; j++)
    	{
    		double s = 0;
			for (k = 1; k <= my numberOfRows; k++)
			{
				s += my data[k][i] * a -> eigenvectors[k][j];
			}
			thy data[i][j] = s;
		}
	}
	
end:

	forget (a);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Configuration Covariance_to_Configuration (Covariance me, 
	long numberOfDimensions)
{
	return SSCP_to_Configuration (me, numberOfDimensions);
}

Configuration Correlation_to_Configuration (Correlation me, 
	long numberOfDimensions)
{
	return SSCP_to_Configuration (me, numberOfDimensions);
}

#if 0
Configuration Covariance_to_Configuration (Covariance me, long numberOfDimensions)
{
	Configuration thee = NULL;
	double **a = NULL;
	long i, j;
	
	if (! (thee = Configuration_create (my nx, numberOfDimensions)) ||
		! (a = NUMdmatrix (1, my nx, 1, my ny)) ||
		! TableOfReal_setSequentialRowLabels (thee, 0, 0, NULL, 1, 1) ||
		! TableOfReal_setSequentialColumnLabels (thee, 0, 0, NULL, 1, 1)) goto end;

	for (i = 1; i <= my nx; i++)
	{
		for (j = 1; j <= my ny; j++) a[i][j] = my z[i][j];
	}
	NUMprincipalComponents_d (a, my nx, numberOfDimensions, thy data);
	
end:

	NUMdmatrix_free (a, 1, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Configuration Correlation_to_Configuration (Correlation me, long numberOfDimensions)
{
	Configuration thee = NULL;
	double **a = NULL;
	long i, j;
	
	if (! (thee = Configuration_create (my nx, numberOfDimensions)) ||
		! (a = NUMdmatrix (1, my nx, 1, my ny)) ||
		! TableOfReal_setSequentialRowLabels (thee, 0, 0, NULL, 1, 1) ||
		! TableOfReal_setSequentialColumnLabels (thee, 0, 0, NULL, 1, 1)) goto end;

	for (i = 1; i <= my nx; i++)
	{
		for (j = 1; j <= my ny; j++) a[i][j] = my z[i][j];
	}
	NUMprincipalComponents_d (a, my nx, numberOfDimensions, thy data);
	
end:

	NUMdmatrix_free (a, 1, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}
#endif

/**************************** Weight *****************************************/

class_methods (Weight, TableOfReal)
class_methods_end

Weight Weight_create (long numberOfPoints)
{
	Weight me = new (Weight); 
	long i, j;
	
	if (! me || ! TableOfReal_init (me, numberOfPoints, numberOfPoints))
	{
		forget (me); return NULL;
	}
	for (i = 1; i <= numberOfPoints; i++)
	{
		for (j = 1; j <= numberOfPoints; j++)
		{
			my data[i][j] = 1;
		}
	}
	return me;
}


/**************** Salience *****************************************/

class_methods (Salience, TableOfReal)
class_methods_end

Salience Salience_create (long numberOfSources, long numberOfDimensions)
{
	Salience me = new (Salience);
	 
	if (! me || ! TableOfReal_init (me, numberOfSources, numberOfDimensions))
	{
		forget (me); return NULL;
	}
	Salience_setDefaults (me);
	return me;
}

long Salience_correctNegatives (Salience me)
{
	long i, j, nNegatives = 0;
	
	/*
		The weights might be negative.
		We correct this by simply making them positive.
	*/
	
	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfColumns; j++)
		{
			if (my data[i][j] < 0) 
			{
				nNegatives++; 
				my data[i][j] = - my data[i][j];
			}
		}
	}
	return nNegatives;
}		

void Salience_setDefaults (Salience me)
{
	long i, j;
	
	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfColumns; j++)
		{
			my data[i][j] = 1 / sqrt (my numberOfColumns);
		}
	}
	for (j = 1; j <= my numberOfColumns; j++)
	{
		wchar_t s[40];
		swprintf (s, 40, L"dimension %ld", j);
		TableOfReal_setColumnLabel (me, j, s);
	}
}

void Salience_draw (Salience me, Graphics g, int ix, int iy, int garnish)
{
	long i, j, nc2, nc1 = ix < iy ? (nc2 = iy, ix) : (nc2 = ix, iy);
	double xmin = 0, xmax = 1, ymin = 0, ymax = 1, wmax = 1;
	
	if (ix < 1 || ix > my numberOfColumns || 
		iy < 1 || iy > my numberOfColumns) return;
		
	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = nc1; j <= nc2; j++)
		{
			if (my data[i][j] > wmax) wmax = my data[i][j];
		}
	}
	xmax = ymax = wmax;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
    Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	
	for (i = 1; i <= my numberOfRows; i++)
	{
		if (my rowLabels[i])
		{
			Graphics_text (g, my data[i][ix], my data[i][iy], my rowLabels[i]);
		}
	}
	
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
	Graphics_line (g, xmin, ymax, xmin, ymin);
	Graphics_line (g, xmin, ymin, xmax, ymin);
	/* Graphics_arc (g, xmin, ymin, xmax - xmin, 0, 90); */
    Graphics_unsetInner (g);
	
	if (garnish)
	{
		if (my columnLabels[ix])
		{
			Graphics_textBottom (g, 0, my columnLabels[ix]);
		}
		if (my columnLabels[iy])
		{
			Graphics_textLeft (g, 0, my columnLabels[iy]);
		}
	}
}

/******** MDSVEC *******************************************/

static void classMDSVec_destroy (I)
{
	iam (MDSVec);
	NUMdvector_free (my proximity, 1);
	NUMlvector_free (my iPoint, 1);
	NUMlvector_free (my jPoint, 1);
	inherited (MDSVec) destroy (me);
}

class_methods (MDSVec, Data)
	class_method_local (MDSVec, destroy)
class_methods_end

MDSVec MDSVec_create (long nPoints)
{
	MDSVec me = new (MDSVec);
	
	if (me == NULL) return NULL;
	
	my nPoints = nPoints;
	my nProximities = nPoints * (nPoints - 1) / 2;
	if (! (my proximity = NUMdvector (1, my nProximities)) ||
		! (my iPoint = NUMlvector (1, my nProximities)) ||
		! (my jPoint = NUMlvector (1, my nProximities))) forget (me);
	return me;
}

MDSVec Dissimilarity_to_MDSVec (Dissimilarity me)
{
	MDSVec thee; int ascending = 1;
	long i, j, k = 0;
	
	if (! (thee = MDSVec_create (my numberOfRows))) return NULL;
	
	for (i = 1; i <= my numberOfRows - 1; i++)
	{
		for (j = i + 1; j <= my numberOfColumns; j++)
		{
			double f = (my data[i][j] + my data[j][i]) / 2;
			if (f > 0)
			{
				k++; 
				thy proximity[k] = f; 
				thy iPoint[k] = i; 
				thy jPoint[k] = j;
			}
		}
	}
	
	thy nProximities = k;		
	if (! NUMsort3 (thy proximity, thy iPoint, thy jPoint, 1, k, ascending))
	{
		forget (thee);
	}
	return thee;
}

/*********************** MDSVECS *******************************************/


class_methods (MDSVecs, Ordered)
class_methods_end

MDSVecs MDSVecs_create (void)
{
	MDSVecs me = new (MDSVecs);
	if (! me || ! Ordered_init (me, classMDSVec, 10)) forget (me);
	return me;
}

MDSVecs Dissimilarities_to_MDSVecs (Dissimilarities me)
{
	MDSVecs thee = MDSVecs_create (); long i;
	if (! thee) return NULL;
	for (i=1; i <= my size; i++)
	{
		MDSVec him = Dissimilarity_to_MDSVec (my item[i]);
		
		if (! him || ! Collection_addItem (thee, him))
		{
			forget (thee);
			return NULL;
		}
		Thing_setName (him, Thing_getName (my item[i]));
	}
	return thee;
}

#if 0
/********************  PREFERENCE ************************************/

class_methods (Preference, TableOfReal)
class_methods_end

Preference Preference_create (long numberOfRows, long numberOfColumns)
{
	Preference me = new (Preference);
	
	if (me == NULL) return NULL;
	if (! TableOfReal_init (me, numberOfRows, numberOfColumns)) forget (me);
	return me;
}
#endif

/**************************  CONFUSIONS **************************************/

class_methods (Confusions, Proximities)
class_methods_end

Confusions Confusions_create (void)
{
	Confusions me = new (Confusions);
	if (! me || ! Proximities_init (me, classConfusion)) forget (me);
	return me;
}

Confusion Confusions_sum (Confusions me)
{
	Confusion thee = NULL; 
	TableOfReal sum = TablesOfReal_sum (me);
	
	if (! sum || ! (thee = TableOfReal_to_Confusion (sum))) forget (thee);
	forget (sum);
	return thee;
}


/**************************  DISTANCES **************************************/

class_methods (Distances, Proximities)
class_methods_end

Distances Distances_create (void)
{
	Distances me = new (Distances);
	
	if (me == NULL) return NULL;
	if (! Proximities_init (me, classDistance)) forget (me);
	return me;
}


/*****************  SCALARPRODUCT ***************************************/

class_methods (ScalarProduct, TableOfReal)
class_methods_end

ScalarProduct ScalarProduct_create (long numberOfPoints)
{
	ScalarProduct me = new (ScalarProduct);
	
	if (me == NULL) return NULL;
	if (! TableOfReal_init (me, numberOfPoints, numberOfPoints)) forget (me);
	return me;
}


/************* SCALARPRODUCTS **************************************/

class_methods (ScalarProducts, TablesOfReal)
class_methods_end

ScalarProducts ScalarProducts_create (void)
{
	ScalarProducts me = new (ScalarProducts);
	if (! me || ! TablesOfReal_init (me, classScalarProduct)) forget (me);
	return me;
}

/******************  DISSIMILARITY **********************************/

class_methods (Dissimilarity, Proximity)
class_methods_end

Dissimilarity Dissimilarity_create (long numberOfPoints)
{
	Dissimilarity me = new (Dissimilarity);
	if (me == NULL || ! Proximity_init (me, numberOfPoints)) forget (me);
	return me;
}

int Dissimilarity_getAdditiveConstant (I, double *c)
{
	iam (Dissimilarity);
	int status = 0;
	long i, j, nProximities = 0, nPoints = my numberOfRows;
	long nPoints2 = 2 * nPoints;
	double proximity, **wd = NULL, **wdsqrt = NULL, **b = NULL;
	double *eigenvalue = NULL;
	
	/*
		Return c = average dissimilarity in case of failure
	*/
	if (nPoints < 1) 
	{
		*c = NUMundefined;
		return Melder_error1 (L"Dissimilarity_getAdditiveConstant: Matrix part is empty.");
	}
		
	*c = 0;
	for (i = 1; i <= nPoints - 1; i++)
	{
		for (j = i + 1; j <= nPoints; j++)
		{
			proximity = (my data[i][j] + my data[j][i]) / 2;
			if (proximity > 0)
			{
				nProximities++;
				*c += proximity;
			}
		}
	}
	if (nProximities < 1) return 0;
	*c /= nProximities;
	
	if (! (wd = NUMdmatrix (1, nPoints, 1, nPoints)) ||
		! (wdsqrt = NUMdmatrix (1, nPoints, 1, nPoints)) ||
		! (b = NUMdmatrix (1, nPoints2, 1, nPoints2)) ||
		! (eigenvalue = NUMdvector (1, nPoints2))) goto end;
		
	/*
		The matrices D & D1/2 with distances (squared and linear)
	*/
	
	for (i = 1; i <= nPoints - 1; i++)
	{
		for (j = i + 1; j <= nPoints; j++)
		{
			proximity = (my data[i][j] + my data[j][i]) / 2;
			wdsqrt[i][j] = - proximity / 2;
			wd[i][j] = - proximity * proximity / 2;		
		}
	}
	
	NUMdoubleCentre_d (wdsqrt, 1, nPoints, 1, nPoints);
	NUMdoubleCentre_d (wd, 1, nPoints, 1, nPoints);
	
	/*
		Calculate the B matrix according to eq. 6
	*/
	
	for (i = 1; i <= nPoints; i++)
	{
		for (j = 1; j <= nPoints; j++)
		{ 
			b[i][nPoints+j] = 2 * wd[i][j];
			b[nPoints+i][nPoints+j] = -4 * wdsqrt[i][j];
			b[nPoints+i][i] = -1;
		}
	}
	
	/*
		Get eigenvalues and sort them descending
	*/
	
	if (! NUMeigensystem_d (b, nPoints2, NULL, eigenvalue) ||
		eigenvalue[1] <= 0) goto end;
	*c = eigenvalue[1];
	status = 1;
end:
	NUMdvector_free (eigenvalue, 1);
	NUMdmatrix_free (b, 1, 1);
	NUMdmatrix_free (wdsqrt, 1, 1);
	NUMdmatrix_free (wd, 1, 1);
	return status;
}


/***************  DISSIMILARITIES **************************************/

class_methods (Dissimilarities, Proximities)
class_methods_end

Dissimilarities Dissimilarities_create (void)
{
	Dissimilarities me = new (Dissimilarities);
	
	if (me == NULL || ! Proximities_init (me, classDissimilarity)) forget (me);
	
	return me;
}


/*************  SIMILARITY *****************************************/

class_methods (Similarity, Proximity)
class_methods_end

Similarity Similarity_create (long numberOfPoints)
{
	Similarity me = new (Similarity);
	
	if (me == NULL || ! Proximity_init (me, numberOfPoints)) forget (me);
	
	return me;
}

Similarity Confusion_to_Similarity (Confusion me, int normalize, 
	int symmetrizeMethod)
{
	Similarity thee = NULL;
	long i, j, k, nxy = my numberOfColumns;
	
	if (my numberOfColumns != my numberOfRows) return Melder_errorp1
		(L"Confusion_to_Similarity: Confusion must be a square matrix.");
		
	if ((thee = Similarity_create (nxy)) == NULL) goto end;
	
	if (!TableOfReal_copyLabels (me, thee, 1, 1)) goto end;
	
	NUMdmatrix_copyElements (my data, thy data, 1, my numberOfRows,
		1, my numberOfColumns);
		
	if (normalize) NUMdmatrix_normalizeRows (thy data, nxy, nxy);
	if (symmetrizeMethod == 1)
	{
		return thee;
	}
	else if (symmetrizeMethod == 2)
	{
		for (i = 1; i <= nxy-1; i++)
		{
			for (j = i + 1; j <= nxy; j++)
			{
				thy data[i][j] = thy data[j][i] = 
					(thy data[i][j] + thy data[j][i]) / 2;
			}
		}
	}
	else if (symmetrizeMethod == 3)
	{
		/*
			Method Houtgast.
		*/
		double **p = NUMdmatrix_copy (thy data, 1, nxy, 1, nxy);
		if (! p) goto end;
		for (i = 1; i <= nxy; i++)
		{
			for (j = i; j <= nxy; j++)
			{
				double tmp = 0;
				for (k = 1; k <= nxy; k++)
				{
					tmp += p[i][k] < p[j][k] ? p[i][k] : p[j][k];
				}
				thy data[j][i] = thy data[i][j] = tmp;
			}
		}
		NUMdmatrix_free (p, 1, 1);
	}
	
end:

	if (Melder_hasError ()) forget (thee);	
	return thee;
}

Dissimilarity Similarity_to_Dissimilarity (Similarity me, 
	double maximumDissimilarity)
{
	Dissimilarity thee = NULL;
	long i, j, nxy = my numberOfColumns;
	double max = 0;
	
	if ((thee = Dissimilarity_create (nxy)) == NULL) return NULL;
	if (! TableOfReal_copyLabels (me, thee, 1, 1))
	{
		forget (thee);
		return NULL;
	}
	NUMdmatrix_copyElements (my data, thy data, 1, my numberOfRows, 
		1, my numberOfColumns);

	for (i = 1; i <= nxy; i++)
	{
		for (j = 1; j <= nxy; j++)
		{
			if (thy data[i][j] > max) max = thy data[i][j];
		}
	}
	
	if (maximumDissimilarity <= 0) maximumDissimilarity = max;

	if (maximumDissimilarity < max) Melder_warning 
		("Similarity_to_Dissimilarity: Your maximumDissimilarity is "
		"smaller than the maximum similarity. Some data may be lost.");

	for (i = 1; i <= nxy; i++)
	{
		for (j = 1; j <= nxy; j++)
		{
			double d = maximumDissimilarity - thy data[i][j];
			thy data[i][j] = d > 0 ? d : 0;
		}
	}
	return thee;
}

Distance Dissimilarity_to_Distance (Dissimilarity me, int scale)
{
	Distance thee = NULL;
	long i, j;
	double additiveConstant = 0;
	
	if ((thee = Distance_create (my numberOfRows)) == NULL) return NULL;
	if (! TableOfReal_copyLabels (me, thee, 1, 1))
	{
		forget (thee); return NULL;
	}
	if (scale == MDS_ORDINAL && 
		! Dissimilarity_getAdditiveConstant (me, &additiveConstant))
	{
		Melder_warning ("Dissimilarity_to_Distance: could not determine " 
			"\"additive constant\", the average dissimilarity was used as "
			"its value.");
	}
	for (i = 1; i <= my numberOfRows - 1; i++)
	{
		for (j = i + 1; j <= my numberOfColumns; j++)
		{
			double d = 0.5 * (my data[i][j] + my data[j][i]) + additiveConstant;
			thy data[i][j] = thy data[j][i] = d;
		}
	}
	return thee;
}

Weight Dissimilarity_to_Weight (Dissimilarity me)
{
	Weight thee;
	long i, j;
	
	if (((thee = Weight_create (my numberOfRows)) != NULL) &&
		TableOfReal_copyLabels (me, thee, 1, 1))
	{
		for (i = 1; i <= my numberOfRows; i++)
		{
			for (j = i; j <= my numberOfRows; j++)
			{
				if (my data[i][j] > 0) thy data[i][j] = 1;
			}
			thy data[i][i] = 0;
		}
	}
	return thee;
}


Dissimilarity Confusion_to_Dissimilarity_pdf (Confusion me, 
	double minimumConfusionLevel)
{
	Dissimilarity thee = NULL;
	long i, j, nxy = my numberOfColumns;

	if (my numberOfColumns != my numberOfRows)
	{
		return Melder_errorp1 (L"Confusion_to_Dissimilarity_pdf: Must be a square matrix.");
	}
	
	Melder_assert (minimumConfusionLevel > 0);
	
	if ((thee = Dissimilarity_create (nxy)) == NULL) return NULL;
	
	if (! TableOfReal_copyLabels (me, thee, 1, 1))
	{
		forget (thee); return NULL;
	}
	
	NUMdmatrix_copyElements (my data, thy data, 1, my numberOfRows, 
		1, my numberOfColumns);
	
	/*
		Correct "zero" responses.	
	*/
	
	for (i = 1; i <= nxy; i++)
	{
		for (j = 1; j <= nxy; j++)
		{
			if (thy data[i][j] == 0) thy data[i][j] = minimumConfusionLevel;
		}
	}
	
	NUMdmatrix_normalizeRows (thy data, nxy, nxy);

	/*
		Consider the fraction as the fraction overlap between two gaussians 
		with unequal sigmas (1 & s).
		We have two matrix elements p[i][j] && p[j][i]
		
		N (x, m, s) = 1 / (s * sqrt(2 pi)) exp(-((x - m) / s)^2 / 2)
		N1 (x) = N (x, 0, 1)
		N2 (x) = N (x, m, s)
		These two gaussians cross each other at a point X that can be found 
		by solving N1 (x) == N2 (x).
		The solution that is important to us is:
			X = (- m + s sqrt (m^2 - 2 (s^2 - 1) ln(s))) / (s^2 - 1)  (1)
			
		This point X must be the solution of F (X, 0, 1) == p[i][j], where 
			F (x, m, s) = P (x>X, m, s) = Integral (x, infinity, N (x, m, s) dx)
		
		We can solve for m and obtain:
			m = X + s sqrt (X^2 + 2 ln (s))                           (2)
		
		We also have
			Integral (-Infinity, X, N2 (x) dx) == p[j][i];
		By changing integration variables, (x - m) / s = y, we get
			Integral ((x-m)/s, Infinity, N (y, 0, 1) dy) == p[j][i]
		Let this equation result in a value Y, i.e., (X - m) / s = Y  (3)
		
		(2) and (3) together and solve for m:
		
			m = X + Y * exp ((Y * y - X * X) / 2);
			
		Make maximum dissimilarity equal to 4sigma.
	*/

	for (i = 1; i <= nxy; i++)
	{
		for (j = i + 1; j <= nxy; j++)
		{
			double d, x, y;
			if (thy data[i][j] > thy data[j][i])
			{
				x = thy data[j][i]; y = thy data[i][j];
			}
			else
			{
				x = thy data[i][j]; y = thy data[j][i];
			}
			x = NUMinvGaussQ (x);
			y = NUMinvGaussQ (y);
			d = x + y * exp ((y * y - x * x) / 2);
			/* Melder_info ("i, j, x, y, d: %d %d %.17g %.17g %.17g", i, j, x, y, d); */
			thy data[i][j] = thy data [j][i] = d;
		}
	}
	return thee;
}

void Distance_and_Configuration_drawScatterDiagram (Distance me, 
	Configuration him, Graphics g, double xmin, double xmax, double ymin, 
	double ymax, double size_mm, const wchar_t *mark, int garnish)
{
	Distance dist = Configuration_to_Distance (him);
	 
	if (! dist) return;
	
	Proximity_Distance_drawScatterDiagram (me, dist, g, xmin, xmax, ymin, 
		ymax, size_mm, mark, garnish);
	forget (dist);
}

Dissimilarity Distance_to_Dissimilarity (Distance me)
{
	Dissimilarity thee = Dissimilarity_create (my numberOfRows);
	
	if (thee == NULL || ! TableOfReal_copyLabels (me, thee, 1, 1))
	{
		forget (thee); return NULL;
	}
	NUMdmatrix_copyElements (my data, thy data, 1, my numberOfRows, 
		1, my numberOfColumns);
	return thee;
}

Configuration Distance_to_Configuration_torsca (Distance me, 
	int numberOfDimensions)
{
	Configuration thee = NULL;
	ScalarProduct sp = NULL;
	
	if (numberOfDimensions > my numberOfRows)
	{
		return Melder_errorp1 (L"Distance_to_Configuration_torsca: Number of dimensions too high.");
	}
	if (! (sp = Distance_to_ScalarProduct (me, 0))) return NULL;
	if (! (thee = Configuration_create (my numberOfRows, numberOfDimensions)) ||
		! TableOfReal_copyLabels (me, thee, 1, 0) ||
		! NUMprincipalComponents_d (sp->data, my numberOfRows, 
			numberOfDimensions, thy data))
	{
		forget (thee);
	}
	forget (sp);
	return thee;
}

ScalarProduct Distance_to_ScalarProduct (Distance me, int normalize)
{
	ScalarProduct thee = NULL;
	long i, j;
	
	thee = ScalarProduct_create (my numberOfRows);
	if (thee == NULL) return NULL;	
	if (! TableOfReal_copyLabels (me, thee, 1, 1))
	{
		forget (thee); return NULL;
	}
	for (i = 1; i <= my numberOfRows - 1; i++)
	{
		for (j = i + 1; j <= my numberOfColumns; j++)
		{
			/*
				force symmetry by averaging!
			*/
			double d = 0.5 * (my data[i][j] + my data[j][i]);
			thy data[i][j] = thy data[j][i] = - 0.5 * d * d;
		}
	}
	TableOfReal_doubleCentre (thee);
	if (my name) Thing_setName (thee, my name);
	if (normalize) TableOfReal_normalizeTable (thee, 1);
	return thee;
}

/**********  Configuration & ..... ***********************************/


Distance Configuration_to_Distance (Configuration me)
{
	Distance thee;
	long i, j, k;
	
	if ((thee = Distance_create (my numberOfRows)) == NULL) return NULL;
	
	if (! TableOfReal_copyLabels (me, thee, 1, -1))
	{
		forget (thee); return NULL;
	}
	for (i = 1; i <= thy numberOfRows-1; i++)
	{
		for (j = i + 1; j <= thy numberOfColumns; j++)
		{
			double dmax = 0, d = 0;
			
			/*
				first divide distance by maximum to prevent overflow when metric
				is a large number.
				d = (x^n)^(1/n) may overflow if x>1 & n >>1 even if d would
				not overflow!
				metric changed 24/11/97
				my w[k] * pow (|i-j|) instead of pow (my w[k] * |i-j|) 
			*/
			
			for (k = 1; k <= my numberOfColumns; k++)
			{
				double dtmp  = fabs (my data[i][k] - my data[j][k]);
				if (dtmp > dmax) dmax = dtmp;
			}
			if (dmax > 0)
			{
				for (k = 1; k <= my numberOfColumns; k++)
				{
					double arg = fabs (my data[i][k] - my data[j][k]) / dmax;
					d += my w[k] * pow (arg, my metric);
				}
			}
			thy data[i][j] = thy data[j][i] = dmax * pow (d, 1.0 / my metric);
		}
	}
	return thee;	
}

void Proximity_Distance_drawScatterDiagram (I, Distance thee, Graphics g, 
	double xmin, double xmax, double ymin, double ymax, double size_mm, 
	const wchar_t *mark, int garnish)   
{
	iam (Proximity);
	long i, j, n = my numberOfRows * (my numberOfRows - 1) / 2;
	double **x = my data, **y = thy data;
	
	if (n == 0) return;
	if (! TableOfReal_equalLabels (me, thee, 1, 1))
	{
		(void) Melder_error1 (L"Proximity_Distance_drawScatterDiagram: Dimensions and labels must be the same.");
		return;
	}
	if (xmax <= xmin)
	{
		xmin = xmax = x[1][2];
		for (i=1; i <= thy numberOfRows-1; i++)
		{
			for (j=i+1; j <= thy numberOfColumns; j++)
			{
				if (x[i][j] > xmax) xmax = x[i][j];
				else if (x[i][j] < xmin) xmin = x[i][j];
			}
		}
	}	
	if (ymax <= ymin)
	{
		ymin = ymax = y[1][2];
		for (i=1; i <= my numberOfRows -1; i++)
		{
			for (j=i+1; j <= my numberOfColumns; j++)
			{
				if (y[i][j] > ymax) ymax = y[i][j];
				else if (y[i][j] < ymin) ymin = y[i][j];
			}
		}
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	for (i=1; i <= thy numberOfRows-1; i++)
	{
		for (j=i+1; j <= thy numberOfColumns; j++)
		{
			if (x[i][j] >= xmin && x[i][j] <= xmax &&
				y[i][j] >= ymin && y[i][j] <= ymax)
			{
				Graphics_mark (g, x[i][j], y[i][j], size_mm, mark);
			}
		}
	}

	Graphics_unsetInner (g);
	if (garnish)
	{
		Graphics_drawInnerBox (g);
	   	Graphics_textLeft (g, 1, L"Distance");
	   	Graphics_textBottom (g, 1, L"Dissimilarity");
		Graphics_marksBottom (g, 2, 1, 1, 0);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}
 
Distances MDSVecs_Distance_monotoneRegression (MDSVecs me, Distance thee,
	int tiesProcessing)
{
	Distances him = NULL;
	long i;
	
	if ((him = Distances_create ()) == NULL) return NULL;
	
	for (i=1; i <= my size; i++)
	{
		MDSVec vec = my item[i]; 
		Distance fit = NULL;
		if (vec->nPoints != thy numberOfRows ||
			! (fit = MDSVec_Distance_monotoneRegression 
				(vec, thee, tiesProcessing)) ||
			! Collection_addItem (him, fit))
		{
			forget (him);
			return NULL;
		}
	}
	return him;
}

Distance MDSVec_Distance_monotoneRegression (MDSVec me, Distance thee,
	int tiesProcessing)
{
	Distance him = NULL;
	int ascending = 1;
	double *proximity = my proximity, *distance = NULL, *fit = NULL;
	long i, j, *iPoint = my iPoint, *jPoint = my jPoint;
	long nProximities = my nProximities;

	if (thy numberOfRows != my nPoints) return NULL;
	if (! (distance = NUMdvector (1, nProximities)) ||
		! (fit = NUMdvector (1, nProximities)) ||
		! (him = Distance_create (thy numberOfRows)) ||
		! TableOfReal_copyLabels (thee, him, 1, 1)) goto end;
		
	for (i = 1; i <= nProximities; i++)
	{
		distance[i] = thy data[iPoint[i]][jPoint[i]];
	}

	if (tiesProcessing == MDS_PRIMARY_APPROACH || MDS_SECONDARY_APPROACH)
	{
		/*
			Kruskal's primary approach to tie-blocks:
				Sort corresponding distances, with iPoint, and jPoint.
			Kruskal's secondary approach:
				Substitute average distance in each tie block
		*/
		long ib = 1;
		for (i = 2; i <= nProximities; i++)
		{
			if (proximity[i] == proximity[i-1]) continue;
			if (i - ib > 1)
			{ 
				if (tiesProcessing == MDS_PRIMARY_APPROACH && 
					! NUMsort3 (distance, iPoint, jPoint, ib, i-1, ascending)) 
						goto end;
				else if (tiesProcessing == MDS_SECONDARY_APPROACH)
				{
					double mean = 0; long j;
					for (j = ib; j <= i-1; j++)
					{
						mean += distance[j];
					}
					mean /= (i-ib);
					for (j = ib; j <= i-1; j++)
					{
						distance[j] = mean;
					}
				}
			}
			ib = i;
		}
	}
		
	NUMmonotoneRegression_d (distance, nProximities, fit);
	
	/*
		Fill Distance with monotone regressed distances
	*/
	
	for (i = 1; i <= nProximities; i++)
	{
		long ip = iPoint[i], jp = jPoint[i];
		his data[ip][jp] = his data[jp][ip] = fit[i];	
	}
	
	/*
		Make rest of distances equal to the maximum fit.
	*/
	
	for (i = 1; i <= his numberOfRows - 1; i++)
	{
		for (j = i + 1; j <= his numberOfColumns; j++)
		{
			if (! his data[i][j])
			{
				his data[i][j] = his data[j][i] = fit[nProximities];
			}
		}
	}
	
end:

	NUMdvector_free (distance, 1); NUMdvector_free (fit, 1);
	if (Melder_hasError ()) forget (him);
	return him;
}


Distance Dissimilarity_Distance_monotoneRegression (Dissimilarity me, 
	Distance thee, int tiesProcessing)
{
	Distance him = NULL; MDSVec vec = NULL;

	if (thy numberOfRows != my numberOfRows) return NULL;
	if (! (vec = Dissimilarity_to_MDSVec (me))) return NULL;
	him = MDSVec_Distance_monotoneRegression (vec, thee, tiesProcessing);
	forget (vec);
	return him;
}

/*************** class Proximities **************************************/

class_methods (Proximities, TablesOfReal)
class_methods_end

int Proximities_init (I, void *klas)
{
	iam (Proximities);
	if (! me || ! TablesOfReal_init (me, klas)) return 0;
	return 1;
}

Proximities Proximities_create (void)
{
	Proximities me = new (Proximities);
	if (! me || ! Proximities_init (me, classProximity)) forget (me);
	return me;
}


ScalarProducts Distances_to_ScalarProducts (Distances me, int normalize)
{
	ScalarProducts thee; 
	long i;
	
	if (! (thee = ScalarProducts_create ())) return NULL;
	
	for (i = 1; i <= my size; i++)
	{
		ScalarProduct sp = Distance_to_ScalarProduct (my item[i], normalize);
		if (! sp || ! Collection_addItem (thee, sp))
		{
			forget (thee); return NULL;
		}
	}
	return thee;
}

int Distances_to_Configuration_ytl (Distances me, int numberOfDimensions,
	int normalizeScalarProducts, Configuration *out1, Salience *out2)
{
	ScalarProducts sp = NULL; 
	int status = 0;
	
	*out1 = NULL; *out2 = NULL;
	
	sp = Distances_to_ScalarProducts (me, normalizeScalarProducts);
	if (sp == NULL) return 0;
	
	status = ScalarProducts_to_Configuration_ytl (sp, numberOfDimensions, 
		out1, out2);

	forget (sp);
	
	return status;
}

int ScalarProducts_to_Configuration_ytl (ScalarProducts me, 
	int numberOfDimensions, Configuration *out1, Salience *out2)
{
	Salience mdsw = NULL;
	Configuration thee = NULL; 
	ScalarProduct scalarproduct = my item[1];
	long i, j, k, l, m, numberOfSources = my size;
	long nPoints = scalarproduct -> numberOfRows;
	double **pmean = NULL, **a = NULL, **evec = NULL, *eval = NULL, **w;
	double ***ci = NULL, **y = NULL, **yinv = NULL, **cl = NULL, **K = NULL;
	
	if (! (thee = Configuration_create (nPoints, numberOfDimensions)) ||
		! (mdsw = Salience_create (numberOfSources, numberOfDimensions)) ||
		! TableOfReal_copyLabels (my item[1], thee, 1, 0) ||
		! (ci = NUMvector (sizeof(double **), 1, numberOfSources)) ||
		! (cl = NUMdmatrix (1, numberOfDimensions, 1, numberOfDimensions)) ||
		! (pmean = NUMdmatrix (1, nPoints, 1, nPoints)) ||
		! (y = NUMdmatrix (1, nPoints, 1, numberOfDimensions)) ||
		! (yinv = NUMdmatrix (1, numberOfDimensions, 1, nPoints)) ||
		! (a = NUMdmatrix (1, numberOfSources, 1, numberOfSources)) ||
		! (evec = NUMdmatrix (1, numberOfSources, 1, numberOfSources)) ||
		! (eval = NUMdvector (1, numberOfSources)) ||
		! (K = NUMdmatrix (1, numberOfDimensions, 1, numberOfDimensions))) 
			goto end;
	
	*out1 = NULL; *out2 = NULL;
	
	Thing_setName (mdsw, L"ytl"); 
	Thing_setName (thee, L"ytl");
	TableOfReal_labelsFromCollectionItemNames (mdsw, me, 1, 0);
	
	/*
		Determine the average scalar product matrix (Pmean) of
		dimension [1..nPoints][1..nPoints].
	*/
	
	for (i = 1; i <= numberOfSources; i++)
	{
		ScalarProduct sp = my item[i];
		for (j = 1; j <= nPoints; j++)
		{
			for (k = 1; k <= nPoints; k++)
			{
				pmean[j][k] += sp->data[j][k];
			}
		}
	}
	
	if (numberOfSources > 1)
	{
		for (j = 1; j <= nPoints; j++)
		{
			for (k = 1; k <= nPoints; k++)
			{
				pmean[j][k] /= numberOfSources;
			}
		}
	}

	/*
		Up to a rotation K, the initial configuration can be found by 
		extracting the first 'numberOfDimensions' principal components 
		of Pmean.
	

	if (! NUMprincipalComponents_d (pmean, nPoints, numberOfDimensions, y))
		goto end;*/
	if (! NUMdmatrix_into_principalComponents (pmean, nPoints, nPoints, 
		numberOfDimensions, y)) goto end;
		
	NUMdmatrix_copyElements (y, thy data, 1, nPoints, 1, numberOfDimensions);
	
	/*
		We cannot determine weights from only one sp-matrix.
	*/
	
	if (numberOfSources == 1) goto end;

	/*
		Calculate the C[i] matrices 
			[1..numberOfDimensions][1..numberOfDimensions]
		from the P[i] by: C[i] = (y'.y)^-1 . y' . P[i] . y . (y'.y)^-1 ==
								  yinv P[i] yinv'
	*/

	if (! NUMpseudoInverse_d (y, nPoints, numberOfDimensions, yinv, 1e-14))
		goto end;
/*    for (i = 1;i <= nPoints; i++)Melder_info ("yinv[1][%ld] %g",i,y[1][i]);*/
	for (i = 1; i <= numberOfSources; i++)
	{
		ScalarProduct sp = my item[i];
		if (! (ci[i] = NUMdmatrix (1, numberOfDimensions, 1,
			numberOfDimensions))) goto end;
		for (j = 1; j <= numberOfDimensions; j++) 
		{
			for (k = 1; k <= numberOfDimensions; k++)
			{ 
				for (l = 1; l <= nPoints; l++)
				{
					if (yinv[j][l] != 0)
					{
						for (m = 1; m <= nPoints; m++)
						{
							ci[i][j][k] += yinv[j][l] * 
								sp->data[l][m] * yinv[k][m];
						}
					}
				}
			}
		}
	}

	/*
		Calculate the A[1..numberOfSources][1..numberOfSources] matrix 
		by (eq.12):
		a[i][j] = trace (C[i]*C[j]) - trace (C[i]) * trace (C[j]) / 
			numberOfDimensions;
		Get the first eigenvector and form matrix cl from a linear 
		combination of the C[i]'s
	*/

	for (i = 1; i <= numberOfSources; i++)
	{
		for (j = i; j <= numberOfSources; j++)
		{
			a[j][i] = a[i][j] =  NUMtrace2 (ci[i], ci[j], numberOfDimensions)
				- NUMtrace (ci[i], numberOfDimensions) * 
				NUMtrace (ci[j], numberOfDimensions) / numberOfDimensions;
				/* Melder_info ("i, j, a[i][j] %ld %ld %g", i, j, a[i][j]); */
		}
	}
	
	if (! NUMeigensystem_d (a, numberOfSources, evec, eval)) goto end;
	
	for (i = 1; i <= numberOfSources; i++)
	{
		for (j = 1; j <= numberOfDimensions; j++)
		{
			for (k = 1; k <= numberOfDimensions; k++)
			{
				cl[j][k] += ci[i][j][k] * evec[i][1]; /* eq. (7) */
			}
		}
	}
	
	/*
		The rotation K is obtained from the eigenvectors of cl
		Is the following still correct??? eigensystem was not sorted??
	*/

	if (! NUMeigensystem_d (cl, numberOfDimensions, K, NULL)) goto end;

	/*
		Now get the configuration: X = Y.K
	*/

	for (i = 1; i <= nPoints; i++)
	{
		for (j = 1; j <= numberOfDimensions; j++)
		{
			double x = 0;
			for (k = 1; k <= numberOfDimensions; k++)
			{
				x += y[i][k] * K[k][j];
			}
			thy data [i][j] = x;
		}
	}
	
	Configuration_normalize (thee, 0, 1);

	/*
		And finally the weights: W[i] = K' C[i] K   (eq. (5)).
		We are only interested in the diagonal of the resulting matrix W[i].
	*/
	
	w = mdsw -> data;
	for (i = 1; i <= numberOfSources; i++)
	{
		for (j = 1; j <= numberOfDimensions; j++)
		{
			double wt = 0;
			for (k=1; k <= numberOfDimensions; k++)
			{
				if (K[k][j] != 0)
				{
					for (l = 1; l <= numberOfDimensions; l++)
					{
						wt += K[k][j] * ci[i][k][l] * K[l][j];
					}
				}
			}
			w[i][j] = wt;
		}
	}
		
end:

	NUMdmatrix_free (K, 1, 1); NUMdvector_free (eval, 1);
	NUMdmatrix_free (evec, 1, 1); NUMdmatrix_free (a, 1, 1);
	NUMdmatrix_free (yinv, 1, 1); NUMdmatrix_free (y, 1, 1);
	NUMdmatrix_free (pmean, 1, 1); NUMdmatrix_free (cl, 1, 1);

	for (i = 1; i <= numberOfSources; i++)
	{
		NUMdmatrix_free (ci[i], 1, 1);
	}
	NUMvector_free (sizeof(double **), ci, 1);
	if (Melder_hasError ())
	{
		forget (thee); 
		forget (mdsw); 
		return 0;
	}
	
	*out1 = thee; *out2 = mdsw;
	
	return 1;
}

Dissimilarities Distances_to_Dissimilarities (Distances me)
{
	Dissimilarities thee = Dissimilarities_create (); 
	long i;
	
	if (thee == NULL) return NULL;
	
	for (i = 1; i <= my size; i++)
	{
		Dissimilarity him = Distance_to_Dissimilarity (my item[i]);
		wchar_t *name = Thing_getName (my item[i]);
		if (! him || ! Collection_addItem (thee, him))
		{
			forget (thee); 
			return NULL;
		}
		Thing_setName (him, name ? name : L"untitled");
	}
	return thee;
}

Distances Dissimilarities_to_Distances (Dissimilarities me, 
	int measurementLevel)
{
	Distances thee = Distances_create ();
	long i;
	
	if (thee == NULL) return NULL;
	
	for (i = 1; i <= my size; i++)
	{
		Distance him = Dissimilarity_to_Distance (my item[i],
			measurementLevel == MDS_ORDINAL);
		wchar_t *name = Thing_getName (my item[i]);
		if ((him == NULL) || ! Collection_addItem (thee, him))
		{
			forget (thee); return NULL;
		}
		Thing_setName (him, name ? name : L"untitled");
	}
	return thee;
}

/*****************  Kruskal *****************************************/

static int smacof_guttmanTransform (Configuration cx, Configuration cz, 
	Distance disp, Weight weight, double **vplus)
{
	Distance distZ = NULL;
	long i, j, k, l;
	long nPoints = cx -> numberOfRows, nDimensions = cx -> numberOfColumns;
	double **b = NULL, **z = cz -> data;
	
	if (! (b = NUMdmatrix (1, nPoints, 1, nPoints)) ||
		! (distZ = Configuration_to_Distance (cz))) goto end;
	
	/*
		compute B(Z) (eq. 8.25)
	*/
	
	for (i = 1; i <= nPoints; i++)
	{
		double sum = 0;
		for (j = 1; j <= nPoints; j++)
		{
			double dzij = distZ -> data[i][j];
			if (i == j || dzij == 0) continue;
			b[i][j] = - weight -> data[i][j] * disp -> data[i][j] / dzij;
			sum += b[i][j];
		}
		b[i][i] = - sum;
	}
	
	/*
		Guttman transform: Xu = (V+)B(Z)Z (eq. 8.29)
	*/
	
	for (i = 1; i <= nPoints; i++)
	{
		for (j = 1; j <= nDimensions; j++)
		{
			double xij = 0;
			for (k = 1;  k <= nPoints; k++)
			{
				for (l = 1; l <= nPoints; l++)
				{
					xij += vplus[i][k] * b[k][l] * z[l][j];
				}
			}
			cx -> data[i][j] = xij;
		}
	}
	
end:

	NUMdmatrix_free (b, 1, 1);
	forget (distZ);
	return ! Melder_hasError();
}

double Distance_Weight_stress (Distance fit, Distance conf, Weight weight, 
	int type)
{
	double eta_fit, eta_conf, rho, stress = NUMundefined, denum, tmp;
	
	Distance_Weight_rawStressComponents (fit, conf, weight, &eta_fit, 
		&eta_conf, &rho);
	
	/*
		All formula's for stress, except for raw stress, are independent of the
		scale of the configuration, i.e., the distances conf[i][j].
	*/
	
	if (type == MDS_NORMALIZED_STRESS)
	{
		denum = eta_fit * eta_conf;
		if (denum > 0) stress = 1.0 - rho * rho / denum;
	}
	else if (type == MDS_STRESS_1)
	{
		denum = eta_fit * eta_conf;
		if (denum > 0)
		{
			tmp = 1.0 - rho * rho / denum;
			if (tmp > 0) stress = sqrt (tmp);
		}
	}
	else if (type == MDS_STRESS_2)
	{
		double m = 0, wsum = 0, var = 0, **w = weight -> data;
		double **c = conf ->data;
		long i, j, nPoints = conf -> numberOfRows;
		
		/*
			Get average distance
		*/
		
		for (i = 1; i <= nPoints-1; i++)
		{
			for (j = i + 1; j <= nPoints; j++)
			{
				m += w[i][j] * c[i][j];
				wsum += w[i][j];
			}
		}
		m /= wsum;
		if (m > 0)
		{
			/*
				Get variance
			*/
			
			for (i = 1; i <= nPoints-1; i++)
			{
				for (j = i + 1; j <= nPoints; j++)
				{
					double tmp = c[i][j] - m;
					var += w[i][j] * tmp * tmp;
				}
			}
			denum = var * eta_fit;
			if (denum > 0) 
			{
				stress = sqrt ((eta_fit * eta_conf - rho * rho) / denum);
			}
		}
	}
	else if (type == MDS_RAW_STRESS)
	{
		stress = eta_fit + eta_conf - 2 * rho ;
	}
	return stress;
}

void Distance_Weight_rawStressComponents (Distance fit, Distance conf,
	Weight weight, double *eta_fit, double *eta_conf, double *rho)
{
	long i, j, nPoints = conf -> numberOfRows; 
	double wsum = 0;
	
	 *eta_fit = *eta_conf = *rho = 0;
	
	for (i = 1; i <= nPoints - 1; i++)
	{
		double *wi = weight -> data[i];
		double *fiti = fit -> data[i];
		double *confi = conf -> data[i];
		
		for (j = i + 1; j <= nPoints; j++)
		{
			*eta_fit += wi[j] * fiti[j] * fiti[j];
			*eta_conf += wi[j] * confi[j] * confi[j];
			*rho += wi[j] * fiti[j] * confi[j];
			wsum += wi[j];	
		}
	}
}

double Dissimilarity_Configuration_Transformator_Weight_stress (Dissimilarity d,
	Configuration c, Any transformator, Weight w, int type)
{
	Distance cdist = NULL, fit = NULL;
	MDSVec vec = NULL;
	Transformator t = (Transformator) transformator;
	int no_weight = w == NULL;
	long nPoints = d -> numberOfRows;
	double stress = NUMundefined;
	
	if (nPoints < 1 || nPoints != c -> numberOfRows  || nPoints != t -> numberOfPoints ||
		(w && nPoints != w -> numberOfRows)) goto end;
	 
	if ((no_weight && ! (w = Weight_create (nPoints)))) goto end;
		
	if (((cdist = Configuration_to_Distance (c)) == NULL) ||
		((vec = Dissimilarity_to_MDSVec (d)) == NULL) ||
		((fit = Transformator_transform (t, vec, cdist, w)) == NULL)) goto end;
	
	stress = Distance_Weight_stress (fit, cdist, w, type);

end:

	if (no_weight) forget (w);
	forget (cdist);
	forget (vec);
	forget (fit);
	return stress;
}

double Dissimilarity_Configuration_Weight_absolute_stress (Dissimilarity d,
	Configuration c, Weight w, int type)
{
	Transformator t = NULL;
	double stress;
	
	if (! (t = Transformator_create (d -> numberOfRows))) return NUMundefined;
	stress = Dissimilarity_Configuration_Transformator_Weight_stress 
		(d, c,t, w, type);
	forget (t);
	return stress;
}

double Dissimilarity_Configuration_Weight_ratio_stress (Dissimilarity d,
	Configuration c, Weight w, int type)
{
	double stress;
	RatioTransformator t = RatioTransformator_create (d -> numberOfRows);
	
	if (t == NULL) return NUMundefined;
	
	stress = Dissimilarity_Configuration_Transformator_Weight_stress 
		(d, c, t, w, type);
	
	forget (t);
	return stress;
}

double Dissimilarity_Configuration_Weight_interval_stress (Dissimilarity d,
	Configuration c, Weight w, int type)
{
	double stress;
	ISplineTransformator t = ISplineTransformator_create 
		(d -> numberOfRows, 0, 1);
	
	if (t == NULL) return NUMundefined;
		
	stress = Dissimilarity_Configuration_Transformator_Weight_stress 
		(d, c,t, w, type);
	
	forget (t);
	return stress;
}

double Dissimilarity_Configuration_Weight_monotone_stress (Dissimilarity d,
	Configuration c, Weight w, int tiesProcessing, int type)
{
	double stress;
	MonotoneTransformator t = MonotoneTransformator_create (d -> numberOfRows);
	
	if (t == NULL) return NUMundefined;
		
	MonotoneTransformator_setTiesProcessing (t, tiesProcessing);
	stress = Dissimilarity_Configuration_Transformator_Weight_stress 
		(d, c, t, w, type);
	
	forget (t);
	return stress;
}

double Dissimilarity_Configuration_Weight_ispline_stress (Dissimilarity d,
	Configuration c, Weight w, long numberOfInteriorKnots, long order, int type)
{
	double stress;
	ISplineTransformator t = ISplineTransformator_create 
		(d -> numberOfRows, numberOfInteriorKnots, order);

	if (t == NULL) return NUMundefined;

	stress = Dissimilarity_Configuration_Transformator_Weight_stress 
		(d, c,t, w, type);
	
	forget (t);
	return stress;
}

void Distance_Weight_smacofNormalize (Distance me, Weight w)
{
	double sumsq = 0, scale; 
	long i, j;
	
	for (i = 1; i <= my numberOfRows-1; i++)
	{
		double *wi = w -> data[i];
		double *di = my data[i];
		for (j = i + 1; j <= my numberOfRows; j++)
		{
			sumsq += wi[j] * di[j] * di[j];
		}
	}
	scale = sqrt (my numberOfRows * (my numberOfRows - 1) / (2 * sumsq));
	for (i = 1; i <= my numberOfRows-1; i++)
	{
		for (j = i + 1; j <= my numberOfRows; j++)
		{
			my data[j][i] = (my data[i][j] *= scale);
		}
	}
}

double Distance_Weight_congruenceCoefficient (Distance x, Distance y, Weight w)
{
	long i, j, nPoints = x -> numberOfRows;
	double xy = 0, x2 = 0, y2 = 0;
	if (y -> numberOfRows != nPoints || w -> numberOfRows != nPoints) return 0;
	for (i=1; i <= nPoints-1; i++)
	{
		double *xi = x -> data[i];
		double *yi = y -> data[i];
		double *wi = w -> data[i];
		for (j=i+1; j <= nPoints-1; j++)		
		{
			xy += wi[j] * xi[j] * yi[j];
			x2 += wi[j] * xi[j] * xi[j];
			y2 += wi[j] * yi[j] * yi[j];
		}
	}
	return xy / (sqrt (x2) * sqrt (y2));
}

Configuration Dissimilarity_Configuration_Weight_Transformator_smacof
	(Dissimilarity me, Configuration conf, Weight weight, Any transformator,
	double tolerance, long numberOfIterations, int showProgress, double *stress)
{
	Configuration z = NULL;
	MDSVec vec = NULL;
	Distance fit = NULL, cdist = NULL;
	Transformator t = (Transformator) transformator;

	long i, j, iter, nPoints = conf->numberOfRows;
	long nDimensions = conf->numberOfColumns;
	double **v = NULL, **vplus = NULL, **w, tol = 1e-6, stressp = 1e38;
	int no_weight = weight == NULL;

	if (my numberOfRows != nPoints ||
		(!no_weight && weight->numberOfRows != nPoints) ||
		t -> numberOfPoints != nPoints)
	{
		return Melder_errorp1 (L"Dissimilarity_Configuration_Weight_Transformator_smacof: dimensions!!!");
	}

	if ((no_weight && (weight = Weight_create (nPoints)) == NULL) ||
		(v = NUMdmatrix (1, nPoints, 1, nPoints)) == NULL ||
		(vplus = NUMdmatrix (1, nPoints, 1, nPoints)) == NULL ||
		(z = Data_copy (conf)) == NULL ||
		(vec = Dissimilarity_to_MDSVec (me)) == NULL) goto end;
		
	w = weight -> data;
	
	if (showProgress) Melder_progress1 (0.0, L"MDS analysis" );

	/*
		Get V (eq. 8.19). 
	*/
	
	for (i = 1; i <= nPoints; i++)
	{
		double wsum = 0;
		for (j = 1; j <= nPoints; j++)
		{
			if (i == j) continue;
			v[i][j] = - w[i][j];
			wsum += w[i][j];
		}
		v[i][i] = wsum;
	}
	
	/*
		V is row and column centered and therefore: rank(V) <= nPoints-1.
		V^-1 does not exist -> get Moore-Penrose inverse.
	*/
	
	if (! NUMpseudoInverse_d (v, nPoints, nPoints, vplus, tol)) goto end;

	for (iter = 1; iter <= numberOfIterations; iter++)
	{
		if (! (cdist = Configuration_to_Distance (conf))) goto end;
		
		/*
			transform & normalization
		*/
		
		if (! (fit = Transformator_transform (t, vec, cdist, weight))) goto end;
		forget (cdist);
					
		/*
			Make conf the Guttman transform of z
		*/
		
		if (! smacof_guttmanTransform (conf, z, fit, weight, vplus)) goto end;
		
		/*
			Compute stress
		*/
		
		if (! (cdist = Configuration_to_Distance (conf))) goto end;
		
		*stress = Distance_Weight_stress (fit, cdist, weight,
			MDS_NORMALIZED_STRESS);
				
		/*
			Check stop criterium
		*/
		
		if (fabs (*stress - stressp) / stressp < tolerance) break;
		
		/*
			Make Z = X
		*/
		
		NUMdmatrix_copyElements (conf->data, z->data, 1, nPoints, 
			1, nDimensions);
		
		stressp = *stress;
		if (showProgress && ! Melder_progress2 ((double) i /
			(numberOfIterations+1), L"kruskal: stress ", Melder_double (*stress))) break;	
		forget (cdist); forget (fit);
	}
	
end:

	if (showProgress) Melder_progress1 (1.0, NULL);
	
	NUMdmatrix_free (v, 1, 1);
	NUMdmatrix_free (vplus, 1, 1);
	forget (vec); 
	forget (cdist); 
	forget (fit);
	if (no_weight) forget (weight);
	if (Melder_hasError ()) forget (z);
	return z;	
}

Configuration Dissimilarity_Configuration_Weight_Transformator_multiSmacof
	(Dissimilarity me, Configuration conf, Weight w, Any transformator,
	double tolerance, long numberOfIterations, long numberOfRepetitions, 
	int showProgress)
{
	Configuration cresult = NULL, cstart = NULL, cbest = NULL, ctmp;
	int showSingle = showProgress && numberOfRepetitions == 1;
	int showMulti = showProgress && numberOfRepetitions > 1;
	double stress, stressmax = 1e38;
	long i;
	
	if (((cstart = Data_copy (conf)) == NULL) ||
		((cbest = Data_copy (cstart)) == NULL)) goto end;

	if (showMulti) Melder_progress1 (0.0, L"mds many times");
		
	for (i = 1; i <= numberOfRepetitions; i++)
	{
		cresult = Dissimilarity_Configuration_Weight_Transformator_smacof
			(me, cstart, w, transformator, tolerance, numberOfIterations, 
			showSingle, &stress);
		if (cresult == NULL) goto end;
		if (stress < stressmax)
		{
			stressmax = stress;
			ctmp = cbest;
			cbest = cresult;
			cresult = ctmp;
		}
		forget (cresult);
 		Configuration_randomize (cstart);
 		TableOfReal_centreColumns (cstart);
 		
 		if (showMulti && ! Melder_progress3 ((double)i / (numberOfRepetitions+1),
			Melder_integer (i), L" from ", Melder_integer (numberOfRepetitions))) break;
	}
	
end:
 
	if (showMulti) Melder_progress1 (1.0, NULL);
	forget (cstart);
	if (Melder_hasError ()) forget (cbest);
	return cbest;
}

Configuration Dissimilarity_Configuration_Weight_absolute_mds 
	(Dissimilarity dis, Configuration cstart, Weight w, double tolerance, 
	long numberOfIterations,long numberOfRepetitions, int showProgress)
{
	Transformator t = NULL;
	Configuration c = NULL;
	
	t = Transformator_create (dis -> numberOfRows);
	if (t == NULL) return NULL;
	
	c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof 
		(dis, cstart, w, t, tolerance, numberOfIterations, 
		numberOfRepetitions, showProgress); 
	
	forget (t);
	return c;
}

Configuration Dissimilarity_Configuration_Weight_ratio_mds (Dissimilarity dis,
	Configuration cstart, Weight w, double tolerance, long numberOfIterations,
	long numberOfRepetitions, int showProgress)
{
	RatioTransformator t = NULL;
	Configuration c = NULL;

	t = RatioTransformator_create (dis -> numberOfRows);
	if (t == NULL) return NULL;
		
	c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof 
		(dis, cstart, w, t, tolerance, numberOfIterations, 
		numberOfRepetitions, showProgress);
	
	forget (t);
	return c;
}

Configuration Dissimilarity_Configuration_Weight_interval_mds 
	(Dissimilarity dis, Configuration cstart, Weight w, double tolerance, 
	long numberOfIterations, long numberOfRepetitions, int showProgress)
{
	ISplineTransformator t = NULL;
	Configuration c = NULL;
	
	t = ISplineTransformator_create (dis -> numberOfRows, 0, 1);
	if (t == NULL) return NULL;
	
	c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof 
		(dis, cstart, w, t, tolerance, numberOfIterations, 
		numberOfRepetitions, showProgress);
	
	forget (t);
	return c;
}

Configuration Dissimilarity_Configuration_Weight_monotone_mds 
	(Dissimilarity dis, Configuration cstart, Weight w, int tiesProcessing, 
	double tolerance, long numberOfIterations, long numberOfRepetitions, 
	int showProgress)
{
	MonotoneTransformator t = NULL;
	Configuration c = NULL;
	
	t = MonotoneTransformator_create (dis -> numberOfRows);
	if (t == NULL) return NULL;

	MonotoneTransformator_setTiesProcessing (t, tiesProcessing);
	
	c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof 
		(dis, cstart, w, t, tolerance, numberOfIterations, 
		numberOfRepetitions, showProgress);

	forget (t);
	return c;
}

Configuration Dissimilarity_Configuration_Weight_ispline_mds (Dissimilarity me,
	Configuration cstart, Weight w, long numberOfInteriorKnots, long order,
	double tolerance, long numberOfIterations, long numberOfRepetitions, 
	int showProgress)
{
	Configuration c = NULL;
	ISplineTransformator t = ISplineTransformator_create (my numberOfRows, 
		numberOfInteriorKnots, order);
	
	if (t == NULL) return NULL;
	
	c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me,
		cstart, w, t, tolerance, numberOfIterations, numberOfRepetitions,
		showProgress);
	
	forget (t);
	return c;
}


Configuration Dissimilarity_Weight_absolute_mds (Dissimilarity dis, Weight w,
	long numberOfDimensions, double tolerance, long numberOfIterations, 
	long numberOfRepetitions, int showProgress)
{
	Configuration c = NULL, cstart;
	Distance d = Dissimilarity_to_Distance (dis, MDS_ABSOLUTE);
	
	if (d)
	{
		cstart = Distance_to_Configuration_torsca (d, numberOfDimensions);
		if (cstart)
		{
			c = Dissimilarity_Configuration_Weight_absolute_mds 
				(dis, cstart, w, tolerance, numberOfIterations, 
				numberOfRepetitions, showProgress);
			forget (cstart);
		}
		forget (d);
	}
	
	return c;
}

Configuration Dissimilarity_Weight_interval_mds (Dissimilarity dis, Weight w,
	long numberOfDimensions, double tolerance, long numberOfIterations, 
	long numberOfRepetitions, int showProgress)
{
	Configuration c = NULL, cstart;
	Distance d = Dissimilarity_to_Distance (dis, MDS_RATIO);
	
	if (d)
	{
		cstart = Distance_to_Configuration_torsca (d, numberOfDimensions);
		if (cstart)
		{
			c = Dissimilarity_Configuration_Weight_interval_mds 
				(dis, cstart, w, tolerance, numberOfIterations, 
				numberOfRepetitions, showProgress);
			forget (cstart);
		}
		forget (d);
	}
	 	
	return c;
}

Configuration Dissimilarity_Weight_monotone_mds (Dissimilarity dis, Weight w,
	long numberOfDimensions, int tiesProcessing, double tolerance,
	long numberOfIterations, long numberOfRepetitions, int showProgress)
{
	Configuration c = NULL, cstart;
	Distance d = Dissimilarity_to_Distance (dis, MDS_ORDINAL);
	
	if (d)
	{
		cstart = Distance_to_Configuration_torsca (d, numberOfDimensions);
		if (cstart)
		{
			c = Dissimilarity_Configuration_Weight_monotone_mds 
				(dis, cstart, w, tiesProcessing, tolerance, numberOfIterations,
			 	numberOfRepetitions, showProgress);
			forget (cstart);
		}
		forget (d);
	}
	
	return c;
}

Configuration Dissimilarity_Weight_ratio_mds (Dissimilarity dis, Weight w,
	long numberOfDimensions, double tolerance, long numberOfIterations,
	long numberOfRepetitions, int showProgress)
{
	Configuration c = NULL, cstart;
	Distance d = Dissimilarity_to_Distance (dis, MDS_RATIO); 
 
 	if (d)
	{
		cstart = Distance_to_Configuration_torsca (d, numberOfDimensions);
		if (cstart)
		{
			c = Dissimilarity_Configuration_Weight_ratio_mds 
				(dis, cstart, w, tolerance, numberOfIterations, 
				numberOfRepetitions, showProgress);
			forget (cstart);
		}
		forget (d);
	}
	return c;
}

Configuration Dissimilarity_Weight_ispline_mds (Dissimilarity me, Weight w,
	long numberOfDimensions, long numberOfInteriorKnots, long order, 
	double tolerance, long numberOfIterations, long numberOfRepetitions, 
	int showProgress)
{
	Configuration c = NULL, cstart;
	Distance d = Dissimilarity_to_Distance (me, MDS_ORDINAL); 

	if (d)
	{
		cstart = Distance_to_Configuration_torsca (d, numberOfDimensions);
		if (cstart)
		{
			c = Dissimilarity_Configuration_Weight_ispline_mds (me, cstart, w,
				numberOfInteriorKnots, order, tolerance, numberOfIterations, 
				numberOfRepetitions, showProgress);
			forget (cstart);
		}
		forget (d);
	}
		
	return c;
}

/***** classical **/
	
static void MDSVec_Distances_getStressValues (MDSVec me, Distance ddist, 
	Distance dfit, int stress_formula, double *stress, double *s, double *t, 
	double *dbar)
{
	long i, nProximities = my nProximities;
	long *iPoint = my iPoint, *jPoint = my jPoint;
	double **dist = ddist -> data, **fit = dfit -> data;

	*s = *t = *dbar = 0;

	if (stress_formula == 2)
	{
		for (i = 1; i <= nProximities; i++)
		{
			*dbar += dist[iPoint[i]][jPoint[i]];
		}
		*dbar /= nProximities;
	}
	
	for (i = 1; i <= nProximities; i++)
	{
		long ii = iPoint[i], jj = jPoint[i];
		double st = dist[ii][jj] - fit[ii][jj];
		double tt = dist[ii][jj] - *dbar;
		*s += st * st; *t += tt * tt;
	}
	
	*stress = *t > 0 ? sqrt (*s / *t) : 0;
}

static double func (I, const double p[])
{
	iam (Kruskal); 
	MDSVec him = my vec; 
	Distance dist = NULL, fit = NULL;
	double **x = my configuration -> data, s, t, dbar, stress;
	double metric = my configuration -> metric;
	long i, j, numberOfDimensions = my configuration -> numberOfColumns;
	long numberOfPoints = my configuration -> numberOfRows;
	int tiesProcessing = my process == MDS_CONTINUOUS ? 1 : 0;

	/*
		Substitute results of minimizer into configuration and 
		normalize the configuration
	*/

	NUMdvector_into_matrix (p, x, 1, numberOfPoints, 1, numberOfDimensions);
	
	/*
		Normalize
	*/
	
	NUMcentreColumns_d (x, 1, numberOfPoints, 1, numberOfDimensions, NULL);
	NUMnormalize_d (x, numberOfPoints, numberOfDimensions, 
		sqrt (numberOfPoints));

	/*
		Calculate interpoint distances from the configuration
	*/
	
	if (! (dist = Configuration_to_Distance (my configuration))) goto end;

	/*
		Monotone regression
	*/

	if	(! (fit = MDSVec_Distance_monotoneRegression (my vec, dist,
		tiesProcessing))) goto end;

	/*
		Get numerator and denominator of stress
	*/

	MDSVec_Distances_getStressValues (my vec, dist, fit, my stress_formula, 
		&stress, &s, &t, &dbar);
	
	/*
		Gradient calculation. 
	*/

	for (i = 1; i <= numberOfPoints; i++)
	{
		for (j = 1; j <= numberOfDimensions; j++)
		{
			my dx[i][j] = 0;
		}
	}

	/*
		Prevent overflow when stress is small
	*/
			
	if (stress < 1e-6) goto end;
	 
	for (i = 1; i <= his nProximities; i++)
	{
		long ii = my vec -> iPoint[i], jj = my vec -> jPoint[i];
		double g1 = stress * ((dist->data[ii][jj] - fit->data[ii][jj]) / s
			- (dist->data[ii][jj] - dbar) / t);
		for (j = 1; j <= numberOfDimensions; j++)
		{
			double dj = x[ii][j] - x[jj][j];
			double g2 = g1 * pow (fabs (dj) / dist->data[ii][jj], metric - 1);
			if (dj < 0) g2 = -g2;
			my dx[ii][j] += g2; my dx[jj][j] -= g2;
		}
	}
	
	/*
		Increment the number of times this function has been called
	*/
	 
	(my minimizer -> funcCalls)++;
	
end:
	forget (dist); forget (fit);
	return stress;
}

/* Precondition: configuration was not changed since previous call to func */
static void dfunc (I, const double p[], double dp[])
{
	iam (Kruskal); 
	Configuration thee = my configuration; 
	long i, j, k = 1;
	(void) p;
	
	for (i = 1; i <= thy numberOfRows; i++)
	{
		for (j = 1; j <= thy numberOfColumns; j++)
		{
			dp[k++]= my dx[i][j];
		}
	}
}

static void classKruskal_destroy (I)
{
	iam (Kruskal);
	NUMdmatrix_free (my dx, 1, 1);
	forget (my configuration);
	forget (my proximities);
	forget (my vec);
	forget (my minimizer);
	inherited (Kruskal) destroy (me);
}

class_methods (Kruskal, Thing)
	class_method_local (Kruskal, destroy)
class_methods_end

Kruskal Kruskal_create (long numberOfPoints, long numberOfDimensions)
{
	Kruskal me = new (Kruskal);
	
	if (! me ||
		! (my proximities = Proximities_create ()) ||
		! (my configuration = Configuration_create (numberOfPoints, 
			numberOfDimensions)) ||
		! (my dx = NUMdmatrix (1, numberOfPoints, 1, numberOfDimensions))) 
			forget (me);
	return me;		
}

Configuration Dissimilarity_kruskal (Dissimilarity me, long numberOfDimensions, 
	long metric, int tiesProcessing, int stress_formula, double tolerance,
	long numberOfIterations, long numberOfRepetitions)
{
	Distance d = NULL; 
	Configuration thee = NULL, c = NULL; 
	int scale = 1;
	(void) metric;
	
	d = Dissimilarity_to_Distance (me, scale);
	if (d == NULL) return NULL;
	
	c = Distance_to_Configuration_torsca (d, numberOfDimensions);
	if (c == NULL) goto end;
	
	Configuration_normalize (c, 1.0, 0);
	
	thee = Dissimilarity_Configuration_kruskal (me, c, tiesProcessing,
		stress_formula, tolerance, numberOfIterations, numberOfRepetitions);
	 
end:

	forget (d); 
	forget (c);
	return thee;
}

void Dissimilarity_Configuration_drawShepardDiagram (Dissimilarity me, 
	Configuration him, Graphics g, double xmin, double xmax, double ymin, 
	double ymax, double size_mm, const wchar_t *mark, int garnish)
{
	Distance dist = Configuration_to_Distance (him);
	 
	if (dist == NULL) return;
	Proximity_Distance_drawScatterDiagram (me, dist, g, xmin, xmax, 
		ymin, ymax, size_mm, mark, garnish);
	forget (dist);
}

Distance Dissimilarity_Configuration_monotoneRegression 
	(Dissimilarity dissimilarity, Configuration configuration, 
	int tiesProcessing)
{
	Distance  result;
	Distance dist = Configuration_to_Distance (configuration);
	
	if (dist == NULL) return NULL;

	result = Dissimilarity_Distance_monotoneRegression 
		(dissimilarity, dist, tiesProcessing);

	forget (dist);
	return result;
}

Distances Dissimilarities_Configuration_monotoneRegression (Dissimilarities me,
	Configuration configuration, int tiesProcessing)
{
	Distances result = Distances_create (); 
	Distance dist; 
	long i;
	
	if (result == NULL) return NULL;
	
	dist = Configuration_to_Distance (configuration);
	if (dist == NULL) goto end;
	
	for (i = 1; i <= my size; i++)
	{
		Distance d = Dissimilarity_Distance_monotoneRegression 
			(my item[i], dist, tiesProcessing);
		if (d == NULL || ! Collection_addItem (result, d)) goto end;
	}
	
end:

	forget (dist);
	if (Melder_hasError ())	forget (result);
	return result;
}


void Dissimilarity_Configuration_drawMonotoneRegression 
	(Dissimilarity me, Configuration him, Graphics g, int tiesProcessing, 
	double xmin, double xmax, double ymin, double ymax, double size_mm, 
	const wchar_t *mark, int garnish)
{/* obsolete replace by transformator */
	Distance fit = Dissimilarity_Configuration_monotoneRegression 
		(me, him, tiesProcessing);
	if (! fit) return;
	Proximity_Distance_drawScatterDiagram (me, fit, g, xmin, xmax, ymin, ymax, 
		size_mm, mark, garnish);
	forget (fit);
}

void Dissimilarity_Configuration_Weight_drawAbsoluteRegression 
	(Dissimilarity d, Configuration c, Weight w, Graphics g, 
	double xmin, double xmax, double ymin, double ymax, 
	double size_mm, const wchar_t *mark, int garnish)
{
	Distance fit = NULL;
	Transformator t = Transformator_create (d->numberOfRows);
	
	if (t == NULL) return;
	
	fit = Dissimilarity_Configuration_Transformator_Weight_transform 
		(d, c, t, w);
	forget (t);
	
	if (fit)
	{
		Proximity_Distance_drawScatterDiagram 
			(d, fit, g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
		forget (fit);
	}
}

void Dissimilarity_Configuration_Weight_drawRatioRegression (Dissimilarity d,
	Configuration c, Weight w, Graphics g, 
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, const wchar_t *mark, int garnish)
{
	Distance fit = NULL; 
	RatioTransformator t = RatioTransformator_create (d -> numberOfRows);
	
	if (t == NULL) return;
	
	fit = Dissimilarity_Configuration_Transformator_Weight_transform 
		(d, c, t, w);
	forget (t);
	
	if (fit)
	{
		Proximity_Distance_drawScatterDiagram 
			(d, fit, g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
		forget (fit);
	}	
}

void Dissimilarity_Configuration_Weight_drawIntervalRegression (Dissimilarity d,
	Configuration c, Weight w, Graphics g, 
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, const wchar_t *mark, int garnish)
{
	Dissimilarity_Configuration_Weight_drawISplineRegression (d, c, w, g,
		0, 1, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawMonotoneRegression (Dissimilarity d,
	Configuration c, Weight w, Graphics g, int tiesProcessing, 
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, const wchar_t *mark, int garnish)
{
	Distance fit; 
	MonotoneTransformator t = MonotoneTransformator_create (d->numberOfRows);
	
	if (t == NULL) return;
	
	MonotoneTransformator_setTiesProcessing (t, tiesProcessing);
	
	fit = Dissimilarity_Configuration_Transformator_Weight_transform 
		(d, c, t, w);
	forget (t);
	
	if (fit)
	{
		Proximity_Distance_drawScatterDiagram 
			(d, fit, g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
		forget (fit);
	}
}

void Dissimilarity_Configuration_Weight_drawISplineRegression 
	(Dissimilarity d, Configuration c, Weight w, Graphics g,
	long numberOfInternalKnots, long order, double xmin, double xmax, 
	double ymin, double ymax, double size_mm, const wchar_t *mark, int garnish)
{
	Distance fit; 
	ISplineTransformator t = ISplineTransformator_create (d->numberOfRows, 
		numberOfInternalKnots, order);
		
	if (t == NULL) return;
	
	fit = Dissimilarity_Configuration_Transformator_Weight_transform 
		(d, c, t, w);
	forget (t);
	
	if (fit)
	{
		Proximity_Distance_drawScatterDiagram 
			(d, fit, g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
		forget (fit);
	}
}

Distance Dissimilarity_Configuration_Transformator_Weight_transform 
	(Dissimilarity d, Configuration c, Any t, Weight w)
{
	Distance cdist, thee = NULL; 
	int no_weight = w == NULL;

	if (no_weight)
	{
		w = Weight_create (d -> numberOfRows);
		if (w == NULL) return NULL;
	}
	
	cdist = Configuration_to_Distance (c);
	
	if (cdist)
	{
		MDSVec v = Dissimilarity_to_MDSVec (d);
		if (v)
		{
			thee = Transformator_transform (t, v, cdist, w);
			forget (v);
		}
		forget (cdist);
	}
		
	if (no_weight) forget (w);
	
	return thee;
}

double Dissimilarity_Configuration_Weight_Transformator_normalizedStress 
	(Dissimilarity me, Configuration conf, Weight weight, Transformator t)
{
	double stress = NUMundefined;
	Distance cdist = Configuration_to_Distance (conf); 

	if (cdist)
	{
		MDSVec vec = Dissimilarity_to_MDSVec (me);
		if (vec)
		{
			Distance fdist = Transformator_transform (t, vec, cdist, weight);
			if (fdist)
			{
				stress = Distance_Weight_stress (fdist, cdist, weight, 
					MDS_NORMALIZED_STRESS);
				forget (fdist);
			}
			forget (vec);
		}
		forget (cdist);
	}	
	return stress;
}

double Dissimilarity_Configuration_getStress (Dissimilarity me, 
	Configuration him, int tiesProcessing, int stress_formula)
{
	double stress = 1;
	Distance dist = Configuration_to_Distance (him);

	if (dist)
	{
		MDSVec vec = Dissimilarity_to_MDSVec (me);
		if (vec)
		{
			Distance fit = MDSVec_Distance_monotoneRegression 
				(vec, dist, tiesProcessing);
			if (fit)
			{
				double s, t, dbar;
				MDSVec_Distances_getStressValues (vec, dist, fit, 
					stress_formula, &stress, &s, &t, &dbar);
				forget (fit);
			}
			forget (vec);
		}
		forget (dist);
	}	 
	return stress;
}

Configuration Dissimilarity_Configuration_kruskal (Dissimilarity me, 
	Configuration him, int tiesProcessing, int stress_formula, 
	double tolerance, long numberOfIterations, long numberOfRepetitions)
{
	Kruskal thee = NULL; 
	Dissimilarity dissimilarity = NULL; 
	Configuration result = NULL;
	long numberOfCoordinates = my numberOfRows * his numberOfColumns;

	/*
		The Configuration is normalized: each dimension centred + 
			total variance set
	*/

	long numberOfParameters = numberOfCoordinates - his numberOfColumns - 1;
	long numberOfData = my numberOfRows * (my numberOfRows - 1) / 2;
	
	if (numberOfData < numberOfParameters)
	{
		return Melder_errorp1 (L"Dissimilarity_Configuration_kruskal: "
			"The number of data must be larger than number of parameters in the model.");
	}
		
	if (! (thee = Kruskal_create (my numberOfRows, his numberOfColumns))) 
		return NULL;
	
	if (! TableOfReal_copyLabels (me, thy configuration, 1, 0) ||
		! (dissimilarity = Data_copy (me)) ||
		! Collection_addItem (thy proximities, dissimilarity) ||
		! (thy vec = Dissimilarity_to_MDSVec (me))) goto end;
	
	/* create the minimizer */
	
	if (! (thy minimizer = VDSmagtMinimizer_create (numberOfCoordinates, 
		thee, func, dfunc))) goto end;

	NUMdmatrix_into_vector (his data, thy minimizer -> p, 1, his numberOfRows, 
		1, his numberOfColumns);

	thy stress_formula = stress_formula; 
	thy process = tiesProcessing;
	Configuration_setMetric (thy configuration, his metric);

	if (! Minimizer_minimizeManyTimes (thy minimizer, numberOfRepetitions,
    		numberOfIterations, tolerance)) goto end;

	/* call the function to get the best configuration */
	
	(void) func (thee, ((Minimizer)(thy minimizer)) -> p);
	
	result = Data_copy (thy configuration);
	
end:

	forget (thee);
	return result;
}

/************************** INDSCAL **************************************/


/*
	Ten Berge, Kiers & Krijnen (1993), Computational Solutions for the 
	Problem of Negative Saliences and Nonsymmetry in INDSCAL, 
	Journal of Classification 10, 115-124. 
*/

static int indscal_iteration_tenBerge (ScalarProducts zc, Configuration xc, 
	Salience weights)
{
	long nPoints = xc -> numberOfRows, nDimensions = xc -> numberOfColumns;
	long nSources = zc -> size, i, j, k, l, h;
	double **x = xc -> data, **w = weights -> data, **wsih = NULL;
	double *solution = NULL, lambda;

	/*
		tolerance = 1e-4 is nearly optimal for dominant eigenvector estimation.
	*/
	
	double tolerance = 1e-4;
	
	if (! (wsih = NUMdmatrix (1, nPoints, 1, nPoints)) ||
		! (solution = NUMdvector (1, nPoints))) goto end;
		
	for (h = 1; h <= nDimensions; h++)
	{
		Collection sprc = NULL; 
		double mean = 0, scale = 0;
		 
		if (! (sprc = Data_copy (zc))) goto end;
		
		for (k = 1; k <= nPoints; k++)
		{
			for (l = 1; l <= nPoints; l++)
			{
				wsih[k][l] = 0;
			}
		}
		
		for (i = 1; i<= nSources; i++)
		{
			ScalarProduct spr = sprc -> item[i]; double **sih = spr -> data;
			
			/*
				Construct the S[i][h] matrices (eq. 6)
			*/
			
			for (j = 1; j <= nDimensions; j++)
			{
				if (j == h) continue;
				for (k = 1; k <= nPoints; k++)
				{
					for (l = 1; l <= nPoints; l++)
					{
						sih[k][l] -= x[k][j] * x[l][j] * w[i][j];
					}
				}
			}
						
			/*
				the weighted S matrix (eq. 8)
			*/
			
			for (k = 1; k <= nPoints; k++)
			{
				for (l = 1; l <= nPoints; l++)
				{
					wsih[k][l] += w[i][h] * sih[k][l];
				}
			}
		}
		
		/*
			largest eigenvalue of m (nonsymmetric matrix!!) is optimal solution 
			for this dimension
		*/
		
		for (k = 1; k <= nPoints; k++)
		{
			solution[k] = x[k][h];
		}
		
		if (! NUMdominantEigenvector_d (wsih, nPoints, solution, &lambda,
			tolerance))
		{
			 forget (sprc); goto end;
		}
		
		/*
			normalize the solution: centre and x'x = 1
		*/
		
		for (k = 1; k <= nPoints; k++)
		{
			mean += solution[k];
		}
		
		if (mean == 0)
		{
			forget (sprc); continue;
		}
		
		mean /= nPoints;
		
		for (k = 1; k <= nPoints; k++)
		{
			solution[k] -= mean;
			scale += solution[k] * solution[k];
		}
		
		for (k = 1; k <= nPoints; k++)
		{
			x[k][h] = solution[k] / sqrt (scale);
		}
		
		/*
			update weights. Make negative weights zero.
		*/
		
		for (i = 1; i<= nSources; i++) 
		{
			ScalarProduct spr = sprc -> item[i];
			double **sih = spr -> data, wih = 0;
			for (k = 1; k <= nPoints; k++)
			{
				for (l = 1; l <= nPoints; l++)
				{
					wih += x[k][h] * sih[k][l] * x[l][h];
				}
			}
			if (wih < 0) wih = 0;
			w[i][h] = wih;
		}
		
		forget (sprc);
	}
	
end:

	NUMdmatrix_free (wsih, 1, 1);
	NUMdvector_free (solution, 1);
	return ! Melder_hasError ();
}


int ScalarProducts_Configuration_Salience_indscal (ScalarProducts sp, 
	Configuration configuration, Salience weights, double tolerance, 
	long numberOfIterations, int showProgress, Configuration *out1, 
	Salience *out2, double *vaf)
{
	Configuration x = NULL; 
	Salience w = NULL;
	double tol = 1e-6, vafp = 0;
	long i, iter, nZeros = 0, nSources = sp -> size;;
	
	if (! (x = Data_copy (configuration)) ||
		! (w = Data_copy (weights))) goto end;
	
	*out1 = NULL; *out2 = NULL;
		
	if (showProgress) Melder_progress1 (0.0, L"INDSCAL analysis");
	
	/*
		Solve for X, and W matrix via Alternating Least Squares.
	*/
	
	for (iter = 1; iter <= numberOfIterations; iter++)
	{
		if (! indscal_iteration_tenBerge (sp, x, w)) goto end;
		
		/* 
			Goodness of fit and test criterion.
		*/
		
		if (! ScalarProducts_Configuration_Salience_vaf (sp, x, w, vaf)) 
			goto end;
		
		if (*vaf > 1-tol || fabs(*vaf - vafp) /  vafp < tolerance) break;
		vafp = *vaf;
		if (showProgress && 
			! Melder_progress2 ((double) iter / (numberOfIterations+1), 
				L"indscal: vaf ", Melder_double(*vaf))) break;	
	}
	
	/*
		Count number of zero weights
	*/
	
	nZeros = NUMdmatrix_countZeros (w->data, w->numberOfRows, 
		w->numberOfColumns);
	
	/*
		Set labels & names.
	*/
	
	Thing_setName (x, L"indscal"); Thing_setName (w, L"indscal");
	TableOfReal_labelsFromCollectionItemNames (w, sp, 1, 0);
	
	*out1 = x; *out2 = w;
	
	if (showProgress)
	{
		MelderInfo_open ();
		MelderInfo_writeLine4 (L"**************** INDSCAL results on Distances "
			"*******************\n\n", Thing_classNameW (sp), 
			L"number of objects: ",  Melder_integer (nSources));
		for (i = 1; i <= nSources; i++)
		{
			MelderInfo_writeLine2 (L"  ", Thing_getName (sp -> item[i]));
		}
		if (nZeros > 0)
		{
			MelderInfo_writeLine5 (L"WARNING: ", Melder_integer (nZeros),  L" zero weight", 
				(nZeros > 1 ? L"s": L""), L"!");
		}
		MelderInfo_writeLine5 (L"\n\nVariance Accounted For = ", Melder_double (*vaf), 
			L"\nThe optimal configuration was reached in ", 
			Melder_integer ((iter > numberOfIterations ? numberOfIterations : iter)), L" iterations.");
		MelderInfo_close ();
	}
	
end:

	if (showProgress) Melder_progress1 (1.0, NULL);
	
	if (Melder_hasError ())
	{
		forget (x); forget (w); return 0;
	}
			
	return 1;
}

int Distances_Configuration_Salience_indscal (Distances distances, 
	Configuration configuration, Salience weights, int normalizeScalarProducts,
	double tolerance, long numberOfIterations, int showProgress, 
	Configuration *out1, Salience *out2, double *vaf)
{
	ScalarProducts sp = NULL; 
	int status;
	
	*out1 = NULL; *out2 = NULL;
	
	sp = Distances_to_ScalarProducts (distances, normalizeScalarProducts);
	if (sp == NULL) return 0;
	
	status = ScalarProducts_Configuration_Salience_indscal (sp, configuration, 
		weights, tolerance, numberOfIterations, showProgress, out1, out2, vaf);
	
	forget (sp);	
			
	return status;
}


int Dissimilarities_Configuration_Salience_indscal (Dissimilarities dissims,
	Configuration configuration, Salience weights, int tiesProcessing, 
	int normalizeScalarProducts, double tolerance, long numberOfIterations,
	int showProgress, Configuration *out1, Salience *out2, double *vaf)
{
	Configuration x = NULL; 
	Salience w = NULL; 
	ScalarProducts sp = NULL;
	MDSVecs vecs = NULL; 
	Distances distances = NULL;
	double tol = 1e-6, vafp = 0; 
	long i, nZeros = 0, nSources = dissims -> size;;
	
	if (! (x = Data_copy (configuration)) ||
		! (w = Data_copy (weights)) ||
		! (vecs = Dissimilarities_to_MDSVecs (dissims))) goto end;
	
	*out1 = NULL; *out2 = NULL;
		
	if (showProgress) Melder_progress1 (0.0, L"INDSCAL analysis");
		
	for (i = 1; i <= numberOfIterations; i++)
	{
		distances = MDSVecs_Configuration_Salience_monotoneRegression 
			(vecs, x, w, tiesProcessing);
		if (distances == NULL) goto end;
		sp = Distances_to_ScalarProducts (distances, normalizeScalarProducts);
		if (sp == NULL) goto end;
		
		if (! indscal_iteration_tenBerge (sp, x, w)) goto end;
			
		/* 
			Goodness of fit and test criterion.
		*/
		
		if (! Distances_Configuration_Salience_vaf (distances, x, w, 
			normalizeScalarProducts, vaf)) goto end;
				
		forget (distances);
		forget (sp);
		
		if (*vaf > 1-tol || fabs(*vaf - vafp) /  vafp < tolerance) break;
		vafp = *vaf;
		if (showProgress && ! Melder_progress2 ((double) i / 
			(numberOfIterations+1), L"indscal: vaf ", Melder_double (*vaf))) break;	
	}
	
	/*
		Count number of zero weights
	*/
	
	nZeros = NUMdmatrix_countZeros (w -> data, w -> numberOfRows, 
		w -> numberOfColumns);

	/*
		Set labels & names.
	*/
	
	Thing_setName (x, L"indscal_mr"); Thing_setName (w, L"indscal_mr");
	TableOfReal_labelsFromCollectionItemNames (w, dissims, 1, 0);
	
	*out1 = x; *out2 = w;
	
	if (showProgress)
	{
		MelderInfo_open ();
		MelderInfo_writeLine1 (L"**************** INDSCAL with monotone regression *******************");
		MelderInfo_writeLine1  (Thing_classNameW (dissims));
		MelderInfo_writeLine2  (L"Number of objects: ", Melder_integer (nSources));
		for (i = 1; i <= nSources; i++)
		{
			MelderInfo_writeLine2 (L"  ", Thing_getName (dissims -> item[i]));
		}
		if (nZeros > 0)
		{
			MelderInfo_writeLine4 (L"WARNING: ", Melder_integer (nZeros), L" zero weight", (nZeros > 1 ? L"s": L""));
		}
		MelderInfo_writeLine2  (L"Variance Accounted For: ", Melder_double (*vaf));
		MelderInfo_writeLine1 (L"Based on MONOTONE REGRESSION");
		MelderInfo_writeLine2  (L"number of iterations: ", Melder_integer ((i > numberOfIterations ?	numberOfIterations : i)));
		MelderInfo_close ();
	}		
	
end:
	if (showProgress) Melder_progress1 (1.0, NULL);

	forget (sp); 
	forget (vecs); 
	forget (distances);
	if (Melder_hasError ())
	{
		forget (x); forget (w); return 0;
	}
	return 1;
}


int Distances_Configuration_indscal (Distances dists, Configuration conf, 
	int normalizeScalarProducts, double tolerance, long numberOfIterations,
	int showProgress, Configuration *out1, Salience *out2)
{
	Salience w; 
	int status; 
	double vaf;
	
	w = Salience_create (dists -> size, conf -> numberOfColumns);
	if (w == NULL) return 0;
	
	status = Distances_Configuration_Salience_indscal (dists, conf, w,
		normalizeScalarProducts, tolerance, numberOfIterations, showProgress, 
		out1, out2, &vaf);
	
 	forget (w);	
 	
 	return status;
}

Distances MDSVecs_Configuration_Salience_monotoneRegression (MDSVecs vecs, 
	Configuration conf, Salience weights, int tiesProcessing)
{
	Distances distances = NULL; 
	long i, nDimensions = conf->numberOfColumns;
	double *w = NULL;
	
	if (! (w  = NUMdvector_copy (conf->w, 1, nDimensions)) ||
		! (distances = Distances_create ())) goto end;
		
	for (i = 1; i <= vecs->size; i++)
	{
		Distance dist = NULL, dc = NULL;
		
		NUMdvector_copyElements (weights -> data[i], conf -> w, 1, nDimensions);

		if (! (dc = Configuration_to_Distance (conf)) ||
			! (dist = MDSVec_Distance_monotoneRegression (vecs -> item[i], dc, 
				tiesProcessing)) ||
			! Collection_addItem (distances, dist))
		{
			forget (dc); goto end;
		}
		forget (dc);
	}
end:
	Configuration_setDefaultWeights (conf);
	NUMdvector_free (w, 1);
	if (Melder_hasError ()) forget (distances);
	return distances;
}


Salience Distances_Configuration_to_Salience (Distances d, Configuration c, 
	int normalize)
{
	ScalarProducts sp = Distances_to_ScalarProducts (d, normalize);
	Salience w = NULL;
	
	if (sp == NULL) return NULL;
	w = ScalarProducts_Configuration_to_Salience (sp, c); 
	forget (sp);
	return w;
}

Salience ScalarProducts_Configuration_to_Salience (ScalarProducts me, 
	Configuration him)
{
	Salience salience = NULL; 
	Configuration cx = NULL; 
	long nSources = my size, nDimensions =  his numberOfColumns; 

	if (! (salience = Salience_create (nSources, nDimensions)) ||
		! (cx = Data_copy (him)) ||
		! indscal_iteration_tenBerge (me, cx, salience)) goto end;
end:
	forget (cx);
	if (Melder_hasError ()) forget (salience);
	return salience;	
}

Salience Dissimilarities_Configuration_to_Salience (Dissimilarities me, 
	Configuration him, int tiesProcessing, int normalizeScalarProducts)
{
	Salience w = NULL;
	Distances distances = Dissimilarities_Configuration_monotoneRegression 
		(me, him, tiesProcessing); 
	
	if (distances == NULL) return NULL;
	
	w = Distances_Configuration_to_Salience 
		(distances, him, normalizeScalarProducts);
		
	forget (distances);
	return w;
}

int Dissimilarities_Configuration_indscal (Dissimilarities dissims,
	Configuration configuration, int tiesProcessing, 
	int normalizeScalarProducts, double tolerance, long numberOfIterations,
	int showProgress, Configuration *out1, Salience *out2)
{
	Distances distances = NULL; 
	Salience weights = NULL;
	int status = 0; 
	double vaf;
	
	*out1 = NULL; *out2 = NULL;
	
	distances = Dissimilarities_Configuration_monotoneRegression 
		(dissims, configuration, tiesProcessing);
	if (distances )
	{
		weights = Distances_Configuration_to_Salience 
			(distances, configuration, normalizeScalarProducts);
		if (weights)
		{
			status = Dissimilarities_Configuration_Salience_indscal 
				(dissims, configuration, weights, tiesProcessing, 
				normalizeScalarProducts, tolerance, numberOfIterations, 
				showProgress, out1, out2, &vaf);
			forget (weights);
		}
		forget (distances);
	}
 	
 	return status;
}


int Dissimilarities_indscal (Dissimilarities me, long numberOfDimensions, 
	int tiesProcessing, int normalizeScalarProducts, double tolerance, 
	long numberOfIterations, long numberOfRepetitions, int showProgress, 
	Configuration *out1, Salience *out2)
{
	Configuration cstart = NULL, cresult = NULL, cbest = NULL, ctmp = NULL;
	Salience  wstart = NULL, wresult = NULL, wbest = NULL, wtmp = NULL;
	Distances distances = NULL;
	int showSingle = showProgress && numberOfRepetitions == 1;
	int showMulti = showProgress && numberOfRepetitions > 1;
	double vaf, vafmin = 0; 
	long i;
	
	*out1 = NULL; *out2 = NULL;
	
	distances = Dissimilarities_to_Distances (me, MDS_ORDINAL);
	if (distances == NULL) return 0;
	
	if (! Distances_to_Configuration_ytl (distances, numberOfDimensions,
			normalizeScalarProducts, &cstart, &wstart)) goto end;
			
	if (! (cbest = Data_copy (cstart)) || 
		! (wbest = Data_copy (wstart))) goto end;	

	if (showMulti) Melder_progress1 (0.0, L"Indscal many times");
	
	for (i = 1; i <= numberOfRepetitions; i++)
	{		
		if (! Dissimilarities_Configuration_Salience_indscal 
			(me, cstart, wstart, tiesProcessing, normalizeScalarProducts, 
			tolerance, numberOfIterations, showSingle, &cresult,
			&wresult, &vaf)) goto end; 
			
 		if (vaf > vafmin)
 		{
 			vafmin = vaf; 
			ctmp = cbest; cbest = cresult; cresult = ctmp; 
 			wtmp = wbest; wbest = wresult; wresult = wtmp;
 		}
 		forget (cresult);
		forget (wresult); 
 		Configuration_randomize (cstart);
 		Configuration_normalize (cstart, 1.0, 1);
  		Salience_setDefaults (wstart);
  		
 		if (showMulti && ! Melder_progress3 ((double)i / (numberOfRepetitions+1),
			Melder_integer (i), L" from ", Melder_integer (numberOfRepetitions))) break;

	}
	
	*out1 = cbest; *out2 = wbest;
	
end:

	if (showMulti) Melder_progress1 (1.0, NULL);
	
 	forget (cstart); 
	forget (wstart); 
	forget (distances);
	
 	if (Melder_hasError ())
 	{
 		forget (cbest); 
		forget (wbest); 
		return 0;
 	}
 	return 1;	
}


int Distances_indscal (Distances distances, long numberOfDimensions, 
	int normalizeScalarProducts, double tolerance, long numberOfIterations, 
	long numberOfRepetitions, int showProgress,
	Configuration *out1, Salience *out2)
{
	Configuration cstart = NULL, cresult, cbest = NULL, ctmp = NULL;
	Salience  wstart = NULL, wresult, wbest = NULL, wtmp = NULL;
	int showSingle = showProgress && numberOfRepetitions == 1;
	int showMulti = showProgress && numberOfRepetitions > 1;
	double vaf, vafmin = 0; 
	long i;
	
	*out1 = NULL; *out2 = NULL;
	
	if (! Distances_to_Configuration_ytl (distances, numberOfDimensions, 
		normalizeScalarProducts, &cstart, &wstart)) return 0;

	if (! (cbest = Data_copy (cstart)) || 
		! (wbest = Data_copy (wstart))) goto end;	

	if (showMulti) Melder_progress1 (0.0, L"Indscal many times");

	for (i = 1; i <= numberOfRepetitions; i++)
	{		
		if (! Distances_Configuration_Salience_indscal (distances, cstart, 
			wstart, normalizeScalarProducts, tolerance, numberOfIterations, 
			showSingle, &cresult, &wresult, &vaf)) goto end;

 		if (vaf > vafmin)
 		{
 			vafmin = vaf; 
			ctmp = cbest; cbest = cresult; cresult = ctmp; 
 			wtmp = wbest; wbest = wresult; wresult = wtmp;
 		}
 		forget (cresult);
		forget (wresult); 
 		Configuration_randomize (cstart);
 		Configuration_normalize (cstart, 1.0, 1);
  		Salience_setDefaults (wstart);
  		
 		if (showMulti && ! Melder_progress3 ((double)i / (numberOfRepetitions+1),
			Melder_integer (i), L" from ", Melder_integer (numberOfRepetitions))) break;

	}

	*out1 = cbest; *out2 = wbest;
	
end:

	if (showMulti) Melder_progress1 (1.0, NULL);
	
 	forget (cstart); 
	forget (wstart);
	
 	if (Melder_hasError ())
 	{
 		forget (cbest); forget (wbest); return 0;
 	}
 	return 1;	
}

int Dissimilarities_Configuration_Salience_vaf (Dissimilarities me, 
	Configuration thee, Salience him, int tiesProcessing, 
	int normalizeScalarProducts, double *vaf)
{
	Distances distances = Dissimilarities_Configuration_monotoneRegression 
		(me, thee, tiesProcessing);

	if (distances)
	{
		(void) Distances_Configuration_Salience_vaf 
			(distances, thee, him, normalizeScalarProducts, vaf);
		forget (distances);
		return 1;
	}
	
	return 0;
}

int Distances_Configuration_vaf (Distances me, Configuration thee, 
	int normalizeScalarProducts, double *vaf)
{
	Salience w = Distances_Configuration_to_Salience 
		(me, thee, normalizeScalarProducts);
	
	if (w)
	{
		(void) Distances_Configuration_Salience_vaf 
			(me, thee, w, normalizeScalarProducts, vaf);
		forget (w);
		return 1;
	}
	return 0;
}

int Dissimilarities_Configuration_vaf (Dissimilarities me, Configuration thee,
	int tiesProcessing, int normalizeScalarProducts, double *vaf)
{
	Salience w = Dissimilarities_Configuration_to_Salience (me, thee,
		tiesProcessing, normalizeScalarProducts);
	
	if (w)
	{
		(void) Dissimilarities_Configuration_Salience_vaf 
			(me, thee, w, tiesProcessing, normalizeScalarProducts, vaf);
		forget (w);
		return 1;
	}
	return 0;
}

int Distances_Configuration_Salience_vaf (Distances me, Configuration thee, 
	Salience him, int normalizeScalarProducts, double *vaf)
{
	ScalarProducts sp; 
	int status = 0;

	if (my size != his numberOfRows || 
		thy numberOfColumns != his numberOfColumns) return Melder_error1 
		(L"Distances_Configuration_Salience_vaf: dimensions must conform.");

	sp = Distances_to_ScalarProducts (me, normalizeScalarProducts);
	if (sp)
	{
		status = ScalarProducts_Configuration_Salience_vaf(sp, thee, him, vaf);
		forget (sp);
	}
	return status;
}

int ScalarProduct_Configuration_getVariances (ScalarProduct me, 
	Configuration thee, double *varianceExplained, double *varianceTotal)
{
	long j, k; 
	Distance distance = NULL; 
	ScalarProduct fit = NULL;
	
	*varianceExplained = *varianceTotal = 0;
	if (((distance = Configuration_to_Distance (thee)) != NULL) &&
		((fit = Distance_to_ScalarProduct (distance, 0)) != NULL))
	{
		/*
			We don't have to calculate the mean because a ScalarProduct
			is double centred, i.e., mean == 0.
		*/
		for (j=1; j <= my numberOfRows; j++)
		{
			for (k=1; k <= my numberOfColumns; k++)
			{
				double d2 = my data[j][k] - fit -> data[j][k];
				*varianceExplained += d2 * d2;
				*varianceTotal += my data[j][k] * my data[j][k];
			}
		}
	}
	forget (distance); forget (fit);
	return ! Melder_hasError();
}

int ScalarProducts_Configuration_Salience_vaf (ScalarProducts me, 
	Configuration thee, Salience him, double *vaf)
{
	long i, j; 
	double t = 0, n = 0, *w = NULL;

	if (my size != his numberOfRows || 
		thy numberOfColumns != his numberOfColumns)
	{
		return Melder_error1 (L"ScalarProducts_Configuration_Salience_vaf: Dimensions of input objects must conform.");
	}
	
	if (! (w = NUMdvector_copy (thy w, 1, thy numberOfColumns))) return 0;

	for (i = 1; i <= my size; i++)
	{

		ScalarProduct sp = my item[i]; double vare, vart;
		
		if (sp->numberOfRows != thy numberOfRows)
		{
			return Melder_error3 (L"ScalarProducts_Configuration_Salience_vaf: "
				"ScalarProduct ", Melder_integer (i), L" does not match Configuration.");
		}
		
		/*
			weigh configuration before calculating variances
		*/
		
		for (j = 1; j <= thy numberOfColumns; j++)
		{
			thy w[j] = sqrt (his data[i][j]);
		}
				
		if (! (ScalarProduct_Configuration_getVariances (sp, thee, 
			&vare, &vart))) goto end;

		t += vare; n+= vart;
	}
	
	*vaf = n > 0 ? 1.0 - t / n : 0;
	
end:

	NUMdvector_copyElements (w, thy w, 1, thy numberOfColumns);
	NUMdvector_free (w, 1);
	return 1;
}


/********************** Examples *********************************************/

Dissimilarity Dissimilarity_createLetterRExample (double noiseStd)
{
	long i, j;
	Dissimilarity me = NULL; 
	Configuration r = Configuration_createLetterRExample (1);
	
	if (r)
	{
		Distance d = Configuration_to_Distance (r);
		if (d)
		{
			me = Distance_to_Dissimilarity (d);
			if (me)
			{
				Thing_setName (me, L"R");
				for (i = 1; i <= my numberOfRows - 1; i++)
				{
					for (j = i + 1; j <= my numberOfRows; j++)
					{
						double d = my data[i][j];
						my data[j][i] = my data[i][j] = d * d + 5 + 
							NUMrandomUniform (0, noiseStd);
					}
				}
			}
			forget (d);
		}
		forget (r);	
	} 
	return me;
}

Salience Salience_createCarrollWishExample (void)
{
	long i, nSources = 8;
	double wx[9] = {0,   1, 0.866, 0.707, 0.5,   0.1, 0.5, 0.354, 0.1};
	double wy[9] = {0, 0.1, 0.5,   0.707, 0.866,   1, 0.1, 0.354, 0.5};
	wchar_t  *name[] = { L"", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8"};
	Salience me = Salience_create (nSources, 2);
	
	if (me)
	{
		for (i = 1; i <= nSources; i++)
		{
			my data[i][1] = wx[i]; 
			my data[i][2] = wy[i]; 
			TableOfReal_setRowLabel (me, i, name[i]);	
		}
	}
	return me;
}

Collection INDSCAL_createCarrollWishExample (double noiseRange)
{
	Collection me = NULL; 
	Configuration c = NULL; 
	Salience s = NULL;
	long i, j, l, nObjects, nSources = 8;
	
	c = Configuration_createCarrollWishExample ();
	if (c == NULL) return NULL;
	
	s = Salience_createCarrollWishExample ();
	if (s == NULL) goto end;
	
	me = Collection_create (classData, nSources);
	if (me == NULL) goto end;
	
	nObjects = c -> numberOfRows;
	for (l = 1; l <= nSources; l++)
	{
		Distance d = NULL; 
		Dissimilarity dissim = NULL;
		
		c -> w[1] = s -> data[l][1]; 
		c -> w[2] = s -> data[l][2];
		if (! (d = Configuration_to_Distance (c)) ||
			! (dissim = Distance_to_Dissimilarity (d)) ||
			! Collection_addItem (me, dissim))
		{
			forget (d); goto end;
		}
		Thing_setName (dissim, s -> rowLabels[l]);
		for (i = 1; i <= nObjects - 1; i++)
		{
			for (j = i + 1; j <= nObjects; j++)
			{
				dissim -> data[i][j] = (dissim -> data[j][i] += 
					NUMrandomUniform (0, noiseRange));
			}
		}
		forget (d); 
	}
	Thing_setName (me, L"CarrollWish");
end:
	forget (s); 
	forget (c);
	if (Melder_hasError()) forget (me);
	return me;
}

void drawSplines (Graphics g, double low, double high, double ymin, double ymax,
	int type, long order, wchar_t *interiorKnots, int garnish)
{
	long i, j, k = order, numberOfKnots, numberOfInteriorKnots = 0;
	long nSplines, n = 1000;
	double knot[101]; double y[1001];
	wchar_t *start, *end;
	
	if (type == MDS_ISPLINE) k++;
	for (i = 1; i <= k; i++)
	{
		knot[i] = low;
	}
	numberOfKnots = k;
	
	start = interiorKnots;
	while (*start)
	{
		double value = wcstod (start, &end);
		start = end;
		if (value < low || value > high)
		{
			Melder_warning ("drawSplines: knots must be in interval (%f, %f)", low, high);
			return;
		}
		if (numberOfKnots == 100) 
		{
			Melder_warning ("drawSplines: too many knots (101)");
			return;
		}
	    knot[++numberOfKnots] = value;
	}

	numberOfInteriorKnots = numberOfKnots - k;
	for (i = 1; i <= k; i++)
	{
		knot[++numberOfKnots] = high;
	}
	
	nSplines = order + numberOfInteriorKnots;
	
	if (nSplines == 0) return;
	
	Graphics_setWindow (g, low, high, ymin, ymax);
	Graphics_setInner (g);
	for (i = 1; i <= nSplines; i++)
	{
		double x, yx, dx = (high-low) / (n-1);
		for (j = 1; j <= n; j++) 
		{
			x = low + dx * (j - 1);
			if (type == MDS_MSPLINE) 
			{
				(void) NUMmspline (knot, numberOfKnots, order, i, x, &yx);
			}
			else
			{
				(void) NUMispline (knot, numberOfKnots, order, i, x, &yx);
			}
			y[j] = yx < ymin ? ymin : yx > ymax ? ymax : yx;
		}
		Graphics_function (g, y, 1, n, low, high);
	} 
	Graphics_unsetInner (g);
	if (garnish)
	{
		static MelderString ts = { 0 };
		long lastKnot = type == MDS_ISPLINE ? numberOfKnots - 2 : numberOfKnots;
		MelderString_empty (&ts);
		Graphics_drawInnerBox (g);
	   	Graphics_textLeft (g, 0, type == MDS_MSPLINE ? L"\\s{M}\\--spline" : 
			L"\\s{I}\\--spline");
		Graphics_marksTop (g, 2, 1, 1, 0);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
    	if (low <= knot[order])
    	{
    		if (order == 1) MelderString_append (&ts, L"t__1_");
			else if (order == 2) MelderString_append (&ts,  L"{t__1_, t__2_}");
			else MelderString_append3 (&ts, L"{t__1_..t__", Melder_integer (order), L"_}"); 
			Graphics_markBottom (g, low, 0, 0, 0, ts.string);
		}
		for (i = 1; i <= numberOfInteriorKnots; i++)
		{
			if (low <= knot[k+i] && knot[k+i] < high)
			{
				MelderString_empty (&ts);
				MelderString_append3 (&ts, L"t__", Melder_integer (order + i), L"_");
				Graphics_markBottom (g, knot[k+i], 0, 1, 1, ts.string); 
				Graphics_markTop (g, knot[k+i], 1, 0, 0, 0);
			}
		}
		if (knot[lastKnot-order+1] <= high)
		{
			MelderString_empty (&ts);
			if (order == 1)
			{
				MelderString_append3 (&ts, L"t__", Melder_integer (lastKnot), L"_");
			}
			else 
			{
				MelderString_append5 (&ts, L"{t__", Melder_integer (order == 2 ? lastKnot-1 : lastKnot-order+1), L"_, t__", Melder_integer (lastKnot), L"_}");
			}
			Graphics_markBottom (g, high, 0, 0, 0, ts.string);
		}
	}
}


void drawMDSClassRelations (Graphics g)
{
	long i, nBoxes = 6;
	double boxWidth = 0.3, boxWidth2 = boxWidth / 2, boxWidth3 = boxWidth / 3;
	double boxHeight = 0.1, boxHeight2 = boxHeight / 2;
	double boxHeight3 = boxHeight / 3;
	double r_mm = 3, dxt = 0.025, dyt = 0.03;
	double dboxx = 1 - 0.2 - 2 * boxWidth, dboxy = (1 - 4 * boxHeight) / 3;
	double x1, x2, xm, x23, x13, y1, y2, ym, y23, y13;
	double x[7] = {0.0, 0.2, 0.2, 0.7, 0.2, 0.7, 0.2}; /* left */
	double y[7] = {0.0, 0.9, 0.6, 0.6, 0.3, 0.3, 0.0}; /* bottom */
	wchar_t *text[7] = {L"", L"Confusion", L"Dissimilarity  %\\de__%%ij%_",
		L"Similarity", L"Distance  %d__%%ij%_, %d\\'p__%%ij%_", 
		L"ScalarProduct", L"Configuration"};

	Graphics_setWindow (g, -0.05, 1.05, -0.05, 1.05);	
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);	
	for (i=1; i <= nBoxes; i++)
	{
		x2 = x[i] + boxWidth; y2 = y[i] + boxHeight;
		xm = x[i] + boxWidth2; ym = y[i] + boxHeight2;
		Graphics_roundedRectangle (g, x[i], x2, y[i], y2, r_mm);
		Graphics_text (g, xm, ym, text[i]);
	}
	
	Graphics_setLineWidth (g, 2);
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);	

	/*
		Confusion to Dissimilarity
	*/

	xm = x[1] + boxWidth2;
	y2 = y[2] + boxHeight;
	Graphics_arrow (g, xm, y[1], xm, y2);
	Graphics_text (g, xm + dxt, y2 + dboxy / 2, L"pdf"); 

	/*
		Confusion to Similarity
	*/
	
	x1 = x[1] + boxWidth;
	xm = x[3] + boxWidth2;
	ym = y[1] + boxHeight2;
	y2 = y[3] + boxHeight;
	Graphics_line (g, x1, ym, xm, ym);
	Graphics_arrow (g, xm, ym, xm, y2);
	y2 += + dboxy / 2 + dyt / 2;
	Graphics_text (g, xm + dxt, y2, L"average"); 
	y2 -= dyt;
	Graphics_text (g, xm + dxt, y2, L"houtgast"); 

	/*
		Dissimilarity to Similarity
	*/
	
	x1 = x[2] + boxWidth;
	y23 = y[2] + 2 * boxHeight3;
	Graphics_arrow (g, x1, y23, x[3], y23);
	y13 = y[2] + boxHeight3;
	Graphics_arrow (g, x[3], y13, x1, y13);

	/*
		Dissimilarity to Distance
	*/
	
	x13 = x[4] + boxWidth3;
	y1 = y[4] + boxHeight;
	Graphics_arrow (g, x13, y1, x13, y[2]);
	x23 = x[4] + 2 * boxWidth3;
	Graphics_arrow (g, x23, y[2], x23, y1);
	
	x1 = x23 + dxt;
	y1 = y[2] - dyt;
	x2 = 0 + dxt;
	y1 -= dyt;
	Graphics_text (g, x1, y1, L"%d\\'p__%%ij%_ = %\\de__%%ij%_");
	Graphics_text (g, x2, y1, L"absolute");
	y1 -= dyt;
	Graphics_text (g, x1, y1, L"%d\\'p__%%ij%_ = %b\\.c%\\de__%%ij%_");
	Graphics_text (g, x2, y1, L"ratio");
	y1 -= dyt;
	Graphics_text (g, x1, y1, L"%d\\'p__%%ij%_ = %b\\.c%\\de__%%ij%_+%a");
	Graphics_text (g, x2, y1, L"interval");
	y1 -= dyt;
	Graphics_text (g, x1, y1, L"%d\\'p__%%ij%_ = \\s{I}-spline (%\\de__%%ij%_)");
	Graphics_text (g, x2, y1, L"\\s{I}\\--spline");
	y1 -= dyt;
	Graphics_text (g, x1, y1, L"%d\\'p__%%ij%_ = monotone (%\\de__%%ij%_)");
	Graphics_text (g, x2, y1, L"monotone");
	
	/*
		Distance to ScalarProduct
	*/
	
	x1 = x[4] + boxWidth;
	ym = y[4] + boxHeight2;		
	Graphics_arrow (g, x1, ym, x[5], ym);
	
	/*
		Distance to Configuration
	*/
	
	y1 = y[6] + boxHeight;
	Graphics_arrow (g, x13, y1, x13, y[4]);
		
	/*
		ScalarProduct to Configuration
	*/
	
	x13 = x[5] + boxWidth3;
	x23 = x[6] + 2 * boxWidth3;
	y1 = y[5] - dboxy / 2;
	Graphics_line (g, x13, y[5], x13, y1);
	Graphics_line (g, x13, y1, x23, y1);
	Graphics_arrow (g, x23, y1, x23, y[6] + boxHeight);
	x1 = x[6] + boxWidth + dboxx / 2;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_text (g, x1, y1, L"\\s{TORSCA}");
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
	Graphics_text (g, x1, y1, L"\\s{YTL}");
	
	Graphics_setLineType (g, Graphics_DOTTED);
	
	x23 = x[5] + 2 * boxWidth3;
	ym = y[6] + boxHeight2;	
	Graphics_line (g, x23, y[5], x23, ym);
	Graphics_arrow (g, x23, ym, x[6] + boxWidth, ym);
	x1 = x[6] + boxWidth + dboxx / 2 + boxWidth3;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_text (g, x1, ym, L"\\s{INDSCAL}");
	
	/*
		Dissimilarity to Configuration
	*/

	ym = y[2] + boxHeight2;
	y2 = y[6] + boxHeight2;	
	Graphics_line (g, x[2], ym, 0, ym);
	Graphics_line (g, 0, ym, 0, y2);
	Graphics_arrow (g, 0, y2, x[6], y2);
	
	/*
		Restore settings
	*/
	
	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setLineWidth (g, 1);
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
	
}

/* End of file MDS.c */
