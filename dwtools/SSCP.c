/* SSCP.c
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
 djmw 20010614 Covariance_difference: corrected bug in calculation of 
 	trace (A B^-1) that caused chisq values to be completely unreliable.
 djmw 20010628 TableOfReal_to_SSCP: skip error-return when nrows < 2, 
	just issue warning.
 djmw 20010906 TableOfReal_to_SSCP.
 djmw 20020212 +getEllipse(s)BoundingBoxCoordinates.
 djmw 20020313 corrected SSCP_Eigen_project.
 djmw 20020327 Moved SSCP_and_Eigen_project to Eigen_and_SSCP.c.
 djmw 20020418 Removed some causes for compiler warnings.
 djmw 20020430 Changed explicit calculation of SSCP to svd in
 		TableOfReal_to_SSCP.
 djmw 20030703 Replaced NUMincompletebeta with gsl_sf_beta_inc.
 djmw 20030801 SSCPs_drawConcentrationEllipses extra label argument.
 djmw 20030825 Replaced gsl_sf_beta_inc with NUMincompletebeta.
 djmw 20031104 Added SSCP_to_CCA.
 djmw 20031117 Added SSCP_extractCentroid.
 djmw 20031127 Added Covariance_and_TableOfReal_extractDistanceQuantileRange.
 djmw 20040211 Better warnings in TableOfReal_to_SSCPs_byLabel for single cases.
 djmw 20040214 Fixed some compiler warnings.
 djmw 20040219 SSCP_getTraceFraction added.
 djmw 20040617 SSCP(s)_drawConcentrationEllipse(s) draw with reverse axes possible.
 	(not yet supported by commands in Picture window like 'One mark bottom...' because of reversed axes)!
 djmw 20060202 Removed a bug in TableOfReal_to_SSCP that could crash Praat (if nrows < ncols).
 djmw 20060503 Covariance_getSignificanceOfMeansDifference: set probability = 0 if
 	var_pooled = 0 and paired.
 djmw 20060811 Removed bug in SSCP_and_TableOfReal_to_MahalanobisDistances that caused column labels always to be copied.
 djmw 20061021 printf expects %ld for 'long int'
 djmw 20061214 Corrected possible integer overflow in ellipseScalefactor.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20071016 To Melder_error<n>
 djmw 20071202 Melder_warning<n>
 djmw 20080122 float -> double
*/

#include "SSCP.h"
#include "Eigen.h"
#include "NUMclapack.h"
#include "NUMlapack.h"
#include "NUM2.h"
#include "SVD.h"

#include "oo_DESTROY.h"
#include "SSCP_def.h"
#include "oo_COPY.h"
#include "SSCP_def.h"
#include "oo_EQUAL.h"
#include "SSCP_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SSCP_def.h"
#include "oo_WRITE_TEXT.h"
#include "SSCP_def.h"
#include "oo_READ_TEXT.h"
#include "SSCP_def.h"
#include "oo_WRITE_BINARY.h"
#include "SSCP_def.h"
#include "oo_READ_BINARY.h"
#include "SSCP_def.h"
#include "oo_DESCRIPTION.h"
#include "SSCP_def.h"

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))
#define TOVEC(x) (&(x) - 1)

class_methods (SSCP, TableOfReal)
	class_method_local (SSCP, destroy)
	class_method_local (SSCP, description)
	class_method_local (SSCP, copy)
	class_method_local (SSCP, equal)
	class_method_local (SSCP, canWriteAsEncoding)
	class_method_local (SSCP, writeText)
	class_method_local (SSCP, readText)
	class_method_local (SSCP, writeBinary)
	class_method_local (SSCP, readBinary)
class_methods_end

/*
	Calculate scale factor by which sqrt(eigenvalue) has to
	be multiplied to obtain the length of an ellipse axis. 
*/
static double ellipseScalefactor (SSCP me, double scale, int confidence)
{
	long n = SSCP_getNumberOfObservations (me);

	if (confidence)
	{
		long p = my numberOfColumns;
		double f;
	
		if (n - p < 1) return -1;
		/* D.E. Johnson (1998), Applied Multivariate methods, page 410 */
		f = NUMinvFisherQ (1 - scale, p, n - p);
		scale = 2 * sqrt (f * p * (n - 1) / (((double) n) * (n - p)));
	}
	else
	{
		scale *= 2 / sqrt (n - 1);
	}
	return scale;	
}

static void getEllipseBoundingBoxCoordinates (SSCP me, double scale, int confidence,
	double *xmin, double *xmax, double *ymin, double *ymax)
{
	double a, b, cs, sn, width, height;
	double lscale = ellipseScalefactor (me, scale, confidence);
	
	NUMeigencmp22 (my data[1][1], my data[1][2], my data[2][2], &a, &b, &cs, &sn);
	NUMgetEllipseBoundingBox (sqrt (a), sqrt (b), cs, & width, & height);
	
	*xmin = my centroid[1] - lscale * width / 2;
	*xmax = *xmin + lscale * width;
	*ymin = my centroid[2] - lscale * height / 2;
	*ymax = *ymin + lscale * height;	
}

static void getEllipsesBoundingBoxCoordinates (SSCPs me, double scale, int confidence,
	double *xmin, double *xmax, double *ymin, double *ymax)
{
	long i;
	
	*xmin = *ymin = 1e38;
	*xmax = *ymax = - *xmin;
	
	for (i = 1; i <= my size; i++)
	{
		SSCP s = my item[i];
		double xmn, xmx, ymn, ymx;
		getEllipseBoundingBoxCoordinates (s, scale, confidence, &xmn, &xmx, &ymn, &ymx);
		if (xmn < *xmin) *xmin = xmn;
		if (xmx > *xmax) *xmax = xmx;
		if (ymn < *ymin) *ymin = ymn;
		if (ymx > *ymax) *ymax = ymx;
	}
}

static SSCP _SSCP_extractTwoDimensions (SSCP me, long d1, long d2)
{
	SSCP thee;
		
	if (! (thee = SSCP_create (2))) return NULL;
	
	thy data [1][1] = my data [d1][d1];
	thy data [2][2] = my data [d2][d2];
	thy data [2][1] = thy data [1][2] = my data [d1][d2];
	thy centroid[1] = my centroid[d1];
	thy centroid[2] = my centroid[d2];
	thy numberOfObservations = my numberOfObservations;
	
	return thee;
}

static SSCPs _SSCPs_extractTwoDimensions (SSCPs me, long d1, long d2)
{
	SSCPs thee;
	long i;
	
	if (! (thee = SSCPs_create ())) return NULL;
	for (i=1; i <= my size; i++)
	{
		SSCP t = _SSCP_extractTwoDimensions (my item[i], d1, d2);
		Thing_setName (t, Thing_getName (my item[i]));
		if (! t || ! Collection_addItem (thee, t)) break;
	}
	if (Melder_hasError ()) forget (thee);
	return thee;
}

static void _SSCP_drawTwoDimensionalEllipse (SSCP me, Graphics g, double scale,
	int fontSize)
{
	double a, angle, b, cs, sn, twoPi = 2 * NUMpi; 
	long nsteps = 100;
	double angle_inc = twoPi / nsteps; 
	double *x = NULL, *y = NULL; long i;
	wchar_t *name;
	
	x = NUMdvector (0, nsteps);
	if (x == NULL) return; 
	y = NUMdvector (0, nsteps);
	if (y == NULL) goto end;

	/*
		Get principal axes and orientation for the ellipse by performing the
		eigen decomposition of a symmetric 2-by-2 matrix.
		Principal axes are a and b with eigenvector/orientation (cs, sn).
	*/
	
	NUMeigencmp22 (my data[1][1], my data[1][2], my data[2][2], &a, &b, &cs, &sn);

	/*
		1. Take sqrt to get units of 'std_dev'
	*/
	
	a = scale * sqrt (a) / 2; b = scale * sqrt (b) / 2;
	x[nsteps] = x[0] = my centroid[1] + cs * a;
	y[nsteps] = y[0] = my centroid[2] + sn * a;
	for (angle = 0, i = 1; i < nsteps; i++, angle += angle_inc)
	{
		double xc = a * cos (angle);
		double yc = b * sin (angle);
		double xt = xc * cs - yc * sn;
		y[i] = my centroid[2] + xc * sn + yc * cs;
		x[i] = my centroid[1] + xt;	
	}
	Graphics_polyline (g, nsteps + 1, x, y);
	if (fontSize > 0 && (name = Thing_getName (me)))
	{
		int oldFontSize = Graphics_inqFontSize (g);
		Graphics_setFontSize (g, fontSize);
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, my centroid[1], my centroid[2], name);
		Graphics_setFontSize (g, oldFontSize);
	}
end:
	NUMdvector_free (x, 0); 
	NUMdvector_free (y, 0);
}

static SSCP _SSCP_toTwoDimensions (SSCP me, double *v1, double *v2)
{
	SSCP thee; 
	long dimension = my numberOfRows, i, j, k, m; 
	double *vec[3];

	if (! (thee = SSCP_create (2))) return NULL;
	
	vec[1] = v1; vec[2] = v2;
	for (i=1; i <= 2; i++)
	{
		for (j=i; j <= 2; j++)
		{
			for (k=1; k <= dimension; k++)
			{
				for (m=1; m <= dimension; m++)
				{
					thy data[i][j] += vec[i][k] * my data[k][m] * vec[j][m];
				}
			}
			thy data[j][i] = thy data[i][j];
		}
		for (m=1; m <= dimension; m++) thy centroid[i] += my centroid[m] * vec[i][m];
	}
	thy numberOfObservations = SSCP_getNumberOfObservations (me);
 	return thee;
}

int SSCP_init (I, long dimension)
{
	iam (SSCP);
	if (! TableOfReal_init (me, dimension, dimension) ||
		! (my centroid = NUMdvector (1, dimension))) return 0;
	return 1;
}

SSCP SSCP_create (long dimension)
{
	SSCP me = new (SSCP);
	
	if (! me || ! SSCP_init (me, dimension)) forget (me);
	return me;
}

double SSCP_getConcentrationEllipseArea(I, double scale, int confidence, long d1, long d2)
{
	iam (SSCP); SSCP thee;
	double a, b, cs, sn;
	long p = my numberOfRows;
	
	if (d1 < 1 || d1 > p || d2 < 1 || d2 > p || d1 == d2) return 0;

	if ((thee = _SSCP_extractTwoDimensions (me, d1, d2)) == NULL) return 0;
		
	scale = ellipseScalefactor (thee, scale, confidence);
	
	if (scale < 0) return 0;
	
	NUMeigencmp22 (thy data[1][1], thy data[1][2], thy data[2][2], &a, &b, &cs, &sn);

	/*
		1. Take sqrt to get units of 'std_dev'
	*/
	
	a = scale * sqrt (a) / 2; b = scale * sqrt (b) / 2;

	forget (thee);
	return NUMpi * a * b;
} 
/* 
void SSCP_and_TableOfReal_drawMahalanobisDistances (I, thou, Graphics g, long rowb, long rowe, double ymin, 
	double ymax, double chiSqFraction, int garnish)
{
}*/

double SSCP_getFractionVariation(I, long from, long to)
{
	iam (SSCP);
	double sum = 0, trace = 0;
	long i, n = my numberOfRows;
	
	if (from < 1 || from > to || to > n) return NUMundefined;
	
	for (i = 1; i <= n; i++)
	{
		trace += my data[i][i];
		if (i >= from && i <= to) sum += my data[i][i];
	}
	return trace > 0 ? sum / trace : NUMundefined;
}

void SSCP_drawConcentrationEllipse (SSCP me, Graphics g, double scale, 
	int confidence, long d1, long d2, double xmin, double xmax, 
	double ymin, double ymax, int garnish)
{
	SSCP thee;
	double xmn, xmx, ymn, ymx;
	long p = my numberOfColumns;
	
	if (d1 < 1 || d1 > p || d2 < 1 || d2 > p || d1 == d2) return;
	
	if (! (thee = _SSCP_extractTwoDimensions (me, d1, d2))) return;
	
	getEllipseBoundingBoxCoordinates (thee, scale, confidence, &xmn, &xmx, &ymn, &ymx);
		
	if (xmax == xmin)
	{
		xmin = xmn; xmax = xmx;
	}
	
	if (ymax == ymin)
	{
		ymin = ymn; ymax = ymx;
	}
	
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	
	scale = ellipseScalefactor (thee, scale, confidence);
	if (scale < 0) return;
	_SSCP_drawTwoDimensionalEllipse (thee, g, scale, 0);
	
	Graphics_unsetInner (g);
	if (garnish)
	{
		Graphics_drawInnerBox (g);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_marksBottom (g, 2, 1, 1, 0);
	}
	forget (thee);
}

void SSCP_setNumberOfObservations (I, double numberOfObservations)
{
	iam (SSCP);
	my numberOfObservations = numberOfObservations;
}

double SSCP_getNumberOfObservations (I)
{
	iam (SSCP);
	return my numberOfObservations;
}

double SSCP_getDegreesOfFreedom (I)
{
	iam (SSCP);
	return my numberOfObservations - 1;
}

double SSCP_getTotalVariance (I)
{
	iam (SSCP); long i; double trace = 0;
	for (i=1; i <= my numberOfRows; i++) trace += my data[i][i];
	return trace;
}

double SSCP_getCumulativeContributionOfComponents (I, long from, long to)
{
	iam (SSCP);
	double sum = NUMundefined, partial;
	long i;
	
	if (to == 0) to = my numberOfRows;
	if (from > 0 && to <= my numberOfRows && from <= to)
	{
		sum = SSCP_getTotalVariance (me);
		for (partial = 0, i = from; i <= to; i++)
		{
			partial += my data[i][i];
		}
		if (sum > 0) sum = partial / sum;
	}
	return sum;
}

TableOfReal Covariance_to_TableOfReal_randomSampling (Covariance me,
	long numberOfData)
{
	TableOfReal thee = NULL; 
	PCA pca = NULL;
	long i, j, k, ncols = my numberOfColumns;
	double *stdev = NULL, *tmp = NULL;

	if (numberOfData <= 0) numberOfData = my numberOfObservations;
	
	if ((pca = SSCP_to_PCA (me)) == NULL) return NULL;
	if ((thee = TableOfReal_create (numberOfData, ncols)) == NULL) goto end;
	if ((stdev = NUMdvector (1, ncols)) == NULL) goto end;
	if ((tmp = NUMdvector (1, ncols)) == NULL) goto end;
	
	for (j = 1;j <= ncols; j++)
	{
		stdev[j] = sqrt (pca -> eigenvalues[j]);
	}
	for (i = 1; i <= numberOfData; i++)
	{
		/*
			Generate the multi-normal vector elements
		*/
		for (j=1; j <= ncols; j++)
		{
			tmp[j] = NUMrandomGauss (0, stdev[j]);
		}
		/*
			Rotate back
		*/
		for (j=1; j <= ncols; j++)
		{
			for (k=1; k <= ncols; k++)
			{
				thy data[i][j] += tmp[k] * pca -> eigenvectors[k][j];
			}
		}
		/*
			Restore the centroid
		*/
		for (j=1; j <= ncols; j++) thy data[i][j] += my centroid[j];
	}
	
	/*
		Set column labels
	*/
	
	NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, ncols);
	
end:
	forget (pca);
	NUMdvector_free (tmp, 1); NUMdvector_free (stdev, 1);
	return thee;
}

SSCP TableOfReal_to_SSCP (I, long rowb, long rowe, long colb, long cole)
{
	iam (TableOfReal);
	SSCP thee = NULL;
	long i, j, k, m, n;
	double **v = NULL;
	
	if (rowb == 0 && rowe == 0)
	{
		rowb = 1; rowe = my numberOfRows;
	}
	else if (rowe < rowb || rowb < 1 || rowe > my numberOfRows) return NULL;
	
	if (colb == 0 && cole == 0)
	{
		colb = 1; cole = my numberOfColumns;
	}
	else if (cole < colb || colb < 1 || cole > my numberOfColumns) return NULL;
	
	m = rowe - rowb + 1; /* # rows */
	n = cole - colb + 1; /* # columns */
	if (m < n) Melder_warning1 (L"TableOfReal_to_SSCP: The SSCP will not have \n"
		"full dimensionality. This may be a problem in following analysis steps. \n"
		"(The number of data points was less than the number of variables.)");
		
	thee = SSCP_create (n);
	if (thee == NULL) goto end;
	v = NUMdmatrix (1, m, 1, n);
	if (v == NULL) goto end;
	
	for (i = 1; i <= m; i++)
	{
		for (j = 1; j <= n; j++)
		{
			v[i][j] = my data[rowb + i - 1][colb + j - 1];
		}
	}
	
	NUMcentreColumns (v, 1, m, 1, n, thy centroid);
	
	SSCP_setNumberOfObservations (thee, m);

	/*
		Covariance = T'T
	*/
	
	for (i = 1; i <= n; i++)
	{
		for (j = i; j <= n; j++)
		{
			double t = 0;
			for (k = 1; k <= m; k++)
			{
				t += v[k][i] * v[k][j];
			}
			thy data[i][j] = thy data[j][i] = t;
		}
	}	
	
	NUMstrings_copyElements (TOVEC(my columnLabels[colb]), thy columnLabels, 1, n);
	NUMstrings_copyElements (thy columnLabels, thy rowLabels, 1, n);
	
end:

	NUMdmatrix_free (v, 1, 1);
	if (Melder_hasError ()) forget (thee);
	return thee; 
}


TableOfReal SSCP_and_TableOfReal_extractDistanceQuantileRange (SSCP me, thou,
	double qlow, double qhigh)
{
	TableOfReal him = NULL; thouart (TableOfReal);
	Covariance cov = SSCP_to_Covariance (me, 1);

	if (cov == NULL) return NULL;
	him = Covariance_and_TableOfReal_extractDistanceQuantileRange (cov, thee, qlow, qhigh);
	forget (cov);
	return him;
}

TableOfReal Covariance_and_TableOfReal_extractDistanceQuantileRange (Covariance me, thou,
	double qlow, double qhigh)
{
	thouart (TableOfReal);
	TableOfReal him = NULL;
	long i, j, k, nrows = thy numberOfRows, n = my numberOfRows, nsel;
	double *d = NULL, *ds = NULL, d2, t, **covari = NULL, lndet, low, high;
	
	if (qlow >= qhigh)
	{
		qlow = 0; qhigh = 1;
	}
	if (qhigh > 1 || qlow < 0) return Melder_errorp1 (L"0 <= lowerQuantile < higherQuantile <= 1.");
	if (n != thy numberOfColumns) return Melder_errorp1 (L"Dimensions");
	if (((d = NUMdvector (1, nrows)) == NULL) ||
		((covari = NUMdmatrix_copy (my data, 1, n, 1, n)) == NULL)) goto end;
		
	/*
		Mahalanobis distance calculation. S = L.L' -> S**-1 = L**-1' . L**-1
		(x-m)'S**-1 (x-m) = (x-m)'L**-1' . L**-1. (x-m) = 
			(L**-1.(x-m))' . (L**-1.(x-m))
		Get inverse of covari in lower triangular part.
	*/
	
	if (! NUMinverse_cholesky (covari, n, &lndet)) goto end;
	
	for (k = 1; k <= nrows; k++)
	{
		for (d2 = 0, i = n; i > 0; i--)
		{
			for (t = 0, j = 1; j <= i; j++)
			{
				t += covari[i][j] * (thy data[k][j] - my centroid[j]);
			}
			d2 += t * t;
		}
		d[k] = sqrt (d2);
	}
	
	if ((ds = NUMdvector_copy (d, 1, nrows)) == NULL) goto end;
	
	NUMsort_d (nrows, ds);
	
	/*
		Get upper and lower quantiles.
	*/
	low  = NUMquantile (nrows, ds, qlow);
	high = NUMquantile (nrows, ds, qhigh);

	/* 
		Count the number filtered.
		nsel = (qhigh - qlow) * nrows is sometimes one off
	*/
	
	for (nsel = 0, i = 1; i <= nrows; i++)
	{
		if (low <= d[i] && d[i] <= high) nsel++;
	}
	
	if (nsel < 1) return Melder_errorp1 (L"Not enough data in quantile interval.");

	him = TableOfReal_create (nsel, thy numberOfColumns);
	if (him == NULL ||
		((his columnLabels = NUMstrings_copy (thy columnLabels, 1,
			thy numberOfColumns)) == NULL)) goto end;


	for (k = 0, i = 1; i <= nrows; i++)
	{
		if (low <= d[i] && d[i] <= high)
		{
			k++;
			if (! TableOfReal_copyOneRowWithLabel (thee, him, i, k)) goto end;
		}
	}

end:

	NUMdvector_free (d, 1); NUMdvector_free (ds, 1);
	NUMdmatrix_free (covari, 1, 1);
	if (Melder_hasError()) forget (him);
	return him;
}

Covariance TableOfReal_to_Covariance (I)
{
	iam (TableOfReal);
	SSCP sscp = NULL;
	Covariance thee = NULL;
	
	sscp = TableOfReal_to_SSCP (me, 0, 0, 0, 0);
	if (sscp == NULL) return NULL;
	thee = SSCP_to_Covariance (sscp, 1);
	forget (sscp);
	return thee;
}

Correlation TableOfReal_to_Correlation (I)
{
	iam (TableOfReal);
	SSCP sscp = NULL;
	Correlation thee = NULL;
	
	sscp = TableOfReal_to_SSCP (me, 0, 0, 0, 0);
	if (sscp == NULL) return NULL;
	thee = SSCP_to_Correlation (sscp);
	forget (sscp);
	return thee;
}

Correlation TableOfReal_to_Correlation_rank (I)
{
	iam (TableOfReal);
	Correlation thee = NULL;
	TableOfReal t = TableOfReal_rankColumns (me);

	if (t == NULL) return NULL;
	thee = TableOfReal_to_Correlation (t);
	forget (t);
	return thee;
}

SSCPs TableOfReal_to_SSCPs_byLabel (I)
{
	iam (TableOfReal);
	TableOfReal mew = NULL;
	SSCPs thee = NULL;
	SSCP t;
	long i, index = 1, numberOfCases = my numberOfRows, nsingle = 0;
	wchar_t *label;
	
	if ((thee = SSCPs_create ()) == NULL) return NULL;

	mew = TableOfReal_sortOnlyByRowLabels (me);
	if (mew == NULL) goto end;
		
	label = mew -> rowLabels[1];
	for (i = 2; i <= numberOfCases; i++)
	{
		wchar_t *li = mew -> rowLabels[i];
		if (li != NULL && li != label && wcscmp (li, label))
		{
			if (i - 1 - index > 0) /* At least two rows? */
			{
				t = TableOfReal_to_SSCP (mew, index, i - 1, 0, 0);
				if (t == NULL || ! Collection_addItem (thee, t)) goto end;
				if (! (label = mew -> rowLabels[index])) label = L"?";
				Thing_setName (t, label);
			}
			else
			{
				nsingle++;
			}
			label = li; index = i;
		}
	}
	if (i - 1 - index > 0) /* At least two rows? */
	{
		t = TableOfReal_to_SSCP (mew, index, numberOfCases, 0, 0);
		if (! t || ! Collection_addItem (thee, t)) goto end;
		if (! (label = mew -> rowLabels[index])) label = L"?";
		Thing_setName (t, label);
	}
	else
	{
		nsingle++;
	}
	if (nsingle > 0)
	{
		nsingle == numberOfCases ? (void) Melder_error3 (L"There are ", Melder_integer (nsingle), L" groups with only one data record. ") : 
			Melder_warning3 (L"There are ", Melder_integer (nsingle), L" groups with only one data record.");
	}
end:

	forget (mew);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

PCA SSCP_to_PCA (I)
{
	iam (SSCP);
	long m = my numberOfRows;
	PCA thee = PCA_create (m, m);

	if (thee == NULL) return NULL;
	
	if (! NUMstrings_copyElements (my rowLabels, thy labels, 1, m) ||
		! Eigen_initFromSymmetricMatrix (thee, my data, m))
	{
		forget (thee);
		return NULL;
	}

	NUMdvector_copyElements (my centroid, thy centroid, 1, m);
	PCA_setNumberOfObservations (thee, my numberOfObservations);
	return thee;
}

CCA SSCP_to_CCA (I, long ny)
{
	iam (SSCP);
	char upper = 'L', diag = 'N';
	long info, i, j, k, l, n, m = my numberOfRows, nx = m - ny;
	long xy_interchanged, yof = 0, xof = ny;
	double **sxx = NULL, **syy = NULL, **syx = NULL, **a = NULL, **ri = NULL;
	GSVD gsvd = NULL; CCA thee = NULL;

	if (ny < 1 || ny >= m) return Melder_errorp1 (L"ny < 1 || ny >= m");
	
	if ((xy_interchanged = nx < ny))
	{
		yof = ny; xof = 0;
		nx = ny; ny = m - nx;
	}
	
	thee = new (CCA);
	if (thee == NULL ||
		((sxx = NUMdmatrix (1, nx, 1, nx)) == NULL) ||
		((syy = NUMdmatrix (1, ny, 1, ny)) == NULL) ||
		((syx = NUMdmatrix (1, ny, 1, nx)) == NULL)) goto end;

	/*
		Copy Syy and Sxx into upper part of syy and sxx matrices.
	*/
	for (i = 1; i <= ny; i++)
	{
		for (j = i; j <= ny; j++)
		{
			syy[i][j] = my data[yof + i][yof + j];
		}
	}
	for (i = 1; i <= nx; i++)
	{
		for (j = i; j <= nx; j++)
		{
			sxx[i][j] = my data[xof + i][xof + j];
		}
	}
	
	for (i = 1; i <= nx; i++)
	{
		for (j = 1; j <= ny; j++)
		{
			syx[i][j] = my data[yof + i][xof + j];
		}
	}
	/*
		Cholesky decomposition: Syy = Uy'*Uy and Sxx = Ux'*Ux.
		(Pretend as if colum-major storage)
	*/
	(void) NUMlapack_dpotf2 (&upper, &ny, &syy[1][1], &ny, &info);
	if (info != 0)
	{
		(void) Melder_error3 (L"The leading minor of order ", Melder_integer (info), L" is not "
			"positive definite, and the factorization of Syy could not be "
			"completed.");
		goto end;
	}
	(void) NUMlapack_dpotf2 (&upper, &nx, &sxx[1][1], &nx, &info);
	if (info != 0)
	{
		(void) Melder_error3 (L"The leading minor of order ", Melder_integer (info), L" is not "
			"positive definite, and the factorization of Sxx could not be "
			"completed.");
		goto end;
	}
	
	/*
		With Cholesky decomps Sxx = Ux'* Ux, Syy = Uy * Uy'
		Sxx**-1 = Uxi * Uxi' and Syy**-1 = Uyi * Uyi', where
		Uxi = Ux**-1 and Uyi = Uy**-1, the equations
		(1)  (Syx * Sxx**-1 * Syx' - lambda Syy) y = 0
		(1') (Syx' * Syy**-1 * Syx - lambda Sxx) x = 0
		can be written as:
		(2)  (Syx  * Uxi * Uxi' * Syx' - lambda Uy' * Uy) y = 0
		(2') (Syx' * Uyi * Uyi' * Syx  - lambda Ux' * Ux) x = 0
		More explicitly as:
		(3)  (Uxi' * Syx')' * (Uxi' * Syx') - lambda Uy' * Uy) y = 0
		(3') (Uyi' * Syx )' * (Uyi' * Syx ) - lambda Ux' * Ux) x = 0
		They are now in the form (A'A - lambda B'B) x = 0 and both can be solved
		with the GSVD.
		However, these equations are not independent. Both have the same
		eigenvalues and given the eigenvectors for one, the eigenvectors for
		the other can be calculated.
		If nx >= ny use eq. (3)
			GSVD (Uxi' * Syx', Uy) gives lambda's and y.
			To get x multiply (1) from the left by Syx'*Syy**-1
			(4) (Syx'*Syy**-1*Syx * Sxx**-1 - lambda ) Syx' * y = 0
			Split off Sxx**-1
			(5) (Syx'*Syy**-1*Syx -lambda Sxx) * Sxx**-1 * Syx' * y = 0
			It follows that x = Sxx**-1 * Syx' * y = Uxi * Uxi' * Sxy * y
		If ny > nx use eq. (3')
			We switch the role of x and y. 
	*/

	a = NUMdmatrix (1, nx, 1, ny);
	if (a == NULL) goto end;
	/*
		Uxi = inverse(Ux)
	*/
	(void) NUMlapack_dtrti2 (&upper, &diag, &nx, &sxx[1][1], &nx, &info);
	if (info != 0)
	{
		(void) Melder_error1 (L"Error in inverse for Sxx.");
		goto end;
	}
	/*
		Prepare Uxi' * Syx' = (Syx * Uxi)'
	*/
	for (i = 1; i <= ny; i++)
	{
		for (j = 1; j <= nx; j++)
		{
			double t = 0;
			for (k = 1; k <= j; k++)
			{
				t += syx[i][k] * sxx[k][j];
			}
			a[j][i] = t;
		}
	}


	if (((gsvd = GSVD_create_d (a, nx, ny, syy, ny)) == NULL) ||
		((ri = NUMdmatrix_copy (gsvd -> r, 1, gsvd -> numberOfColumns, 1, gsvd -> numberOfColumns)) == NULL) ||
		((thy y = Eigen_create (gsvd -> numberOfColumns, gsvd -> numberOfColumns)) == NULL) ||
		((thy x = Eigen_create (thy y -> numberOfEigenvalues, nx)) == NULL)) goto end;
	/*
		Get X=Q*R**-1
	*/

	(void) NUMlapack_dtrti2 (&upper, &diag, &gsvd -> numberOfColumns,
		&ri[1][1], &gsvd -> numberOfColumns, &info);
	if (info != 0)
	{
		(void) Melder_error1 (L"Error in inverse for R.");
		goto end;
	}

	for (i = 1; i <= gsvd -> numberOfColumns; i++)
	{
		double t = gsvd -> d1[i] / gsvd -> d2[i];
		thy y -> eigenvalues[i] = t * t;
		for (j = 1; j <= gsvd -> numberOfColumns; j++)
		{
			t = 0;
			for (k = 1; k <= j; k++)
			{
				t += gsvd -> q[i][k] * ri[k][j];
			}
			thy y ->eigenvectors[j][i] = t;
		}
	}

	NUMnormalizeRows (thy y -> eigenvectors, thy y -> numberOfEigenvalues, thy y -> numberOfEigenvalues, 1);

	thy numberOfCoefficients = thy y -> numberOfEigenvalues;
	thy numberOfObservations = my numberOfObservations;
	/*
		x = Sxx**-1 * Syx' * y
	*/
	for (i = 1; i <= thy numberOfCoefficients; i++)
	{
		double *evecy = thy y -> eigenvectors[i];
		double *evecx = thy x -> eigenvectors[i];
		for (j = 1; j <= nx; j++)
		{
			double t = 0;
			for (k = j; k <= nx; k++)
			{
				for (l = 1; l <= nx; l++)
				{
					for (n = 1; n <= ny; n++)
					{
						t += sxx[j][k] * sxx[l][k] * syx[n][l] * evecy[n];
					}
				}
			}
			evecx[j] = t;
		}
	}

	NUMnormalizeRows (thy x -> eigenvectors, thy x -> numberOfEigenvalues, nx, 1);

	if (ny < nx)
	{
		Eigen t = thy x;
		thy x = thy y; thy y = t;
	}
end:
	forget (gsvd);
	NUMdmatrix_free (sxx, 1, 1);
	NUMdmatrix_free (syy, 1, 1);
	NUMdmatrix_free (syx, 1, 1);
	NUMdmatrix_free (a, 1, 1);
	NUMdmatrix_free (ri, 1, 1);
	if (Melder_hasError()) forget (thee);
	return thee;
}

/************ SSCPs ***********************************************/

class_methods (SSCPs, Ordered)
class_methods_end

SSCPs SSCPs_create (void)
{
	SSCPs me = new (SSCPs);
	if (! me || ! Ordered_init (me, classSSCP, 10)) forget (me);
	return me;
}

SSCP SSCPs_to_SSCP_pool (SSCPs me)
{
	SSCP thee; long i, j, k;

	if (! (thee = Data_copy (my item[1]))) return NULL;
	
	for (k = 2; k <= my size; k++)
	{
		SSCP t = my item[k]; long no = t -> numberOfObservations;
		
		if (t -> numberOfRows != thy numberOfRows)
		{
			forget (thee);
			return Melder_errorp3 (L"SSCPs_sum: unequal dimensions (", Melder_integer (k), L").");
		}
		
		thy numberOfObservations += no;
		
		/*
			Sum the sscp's and weigh the centroid.
		*/
		
		for (i = 1; i <= thy numberOfRows; i++)
		{
			for (j = i; j <= thy numberOfRows; j++)
			{
				thy data[j][i] = thy data[i][j] += t -> data[i][j];
			}
		}
		
		for (j = 1; j <= thy numberOfRows; j++)
		{
			thy centroid[j] += no * t -> centroid[j];
		}	
	}
	
	for (i = 1; i <= thy numberOfRows; i++)
	{
		thy centroid[i] /= thy numberOfObservations;
	}
	
	return thee;
}

int SSCPs_getHomegeneityOfCovariances_box (SSCPs me, double *probability, 
	double *chisq, long *ndf)
{
	SSCP pooled; 
	double ln_determinant, inv, sum, ni;
	long i, g = my size, p;
	
	*probability = 0; *chisq = 0; *ndf = 0;

	if (! (pooled = SSCPs_to_SSCP_pool (me))) return 0;
	
	p = pooled -> numberOfColumns;
	
	for (*chisq = 0, sum = 0, inv = 0, i=1; i <= g; i++)
	{
		SSCP t = my item[i];
		ni = t -> numberOfObservations - 1;
		if (! NUMdeterminant_cholesky (t -> data, p, &ln_determinant)) goto end;
		/*
			Box-test is for covariance matrices -> scale determinant.
		*/
		ln_determinant -= p * log (ni);
		sum += ni; inv += 1 / ni;
		*chisq -= ni * ln_determinant;
	}
	
	if (! NUMdeterminant_cholesky (pooled -> data, p, &ln_determinant)) goto end;
	ln_determinant -= p * log (pooled -> numberOfObservations - g);
	*chisq += sum * ln_determinant;
	
	*chisq *= 1.0 - (inv - 1 / sum) * (2 * p * p + 3 * p - 1) 
		/ (6 * (p + 1) * (g - 1));
	*ndf = (g - 1) * p * (p + 1) / 2;
	*probability =  NUMchiSquareQ (*chisq, *ndf);
end:
	forget (pooled);	
	return ! Melder_hasError ();
}


SSCPs SSCPs_toTwoDimensions (SSCPs me, double *v1, double *v2)
{
	SSCPs thee; 
	long i;
	
	if ((thee = SSCPs_create ()) == NULL) return NULL;
	for (i = 1; i <= my size; i++)
	{
		SSCP t = _SSCP_toTwoDimensions (my item[i], v1, v2);
		Thing_setName (t, Thing_getName (my item[i]));
		if (t == NULL || ! Collection_addItem (thee, t)) break;
	}
	if (Melder_hasError ()) forget (thee);
	return thee;
}


void SSCPs_drawConcentrationEllipses (SSCPs me, Graphics g, double scale,
	int confidence, wchar_t *label, long d1, long d2, double xmin, double xmax,
	double ymin, double ymax, int fontSize, int garnish)
{
	SSCPs thee;
	SSCP t = my item[1];
	double xmn, xmx, ymn, ymx;
	long i, p = t -> numberOfColumns;

	if (d1 < 1 || d1 > p || d2 < 1 || d2 > p || d1 == d2) return;
	
	if (! (thee = _SSCPs_extractTwoDimensions (me, d1, d2))) return;
	getEllipsesBoundingBoxCoordinates (me, scale, confidence, &xmn, &xmx, &ymn, &ymx);
	
	if (xmin == xmax) 
	{
		xmin = xmn; xmax = xmx;
	}
	
	if (ymin == ymax)
	{
		ymin = ymn; ymax = ymx;
	}
	
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	
	
	for (i = 1; i <= thy size; i++)
	{
		double lscale;
		t = thy item[i];		
		lscale = ellipseScalefactor (t, scale, confidence);
		if (lscale < 0) continue;
		if (label == NULL || NUMwcscmp (label, Thing_getName (t)) == 0)
		{
			_SSCP_drawTwoDimensionalEllipse (t, g, lscale, fontSize);
		}
	}
	
	Graphics_unsetInner (g);
	if (garnish)
	{
		wchar_t text[20];
		t = my item[1];
    	Graphics_drawInnerBox (g);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
		swprintf (text, 20, L"Dimension %ld", d2);
    	Graphics_textLeft (g, 1, t -> rowLabels[d2] ? t -> rowLabels[d2] : text);
    	Graphics_marksBottom (g, 2, 1, 1, 0);
		swprintf (text, 20, L"Dimension %ld", d1);
		Graphics_textBottom (g, 1, t -> rowLabels[d1] ? t -> rowLabels[d1] : text);
	}	
	forget (thee);
}

TableOfReal SSCP_to_TableOfReal (SSCP me)
{
	TableOfReal thee = Data_copy (me);
	if (thee == NULL) return NULL;
	Thing_overrideClass (thee, classTableOfReal);
	return thee;
}

TableOfReal SSCP_extractCentroid (I)
{
	iam (SSCP);
	TableOfReal thee;
	long n = my numberOfColumns;

	thee = TableOfReal_create (1, n);
	if (thee == NULL) return NULL;
	NUMdvector_copyElements (my centroid, thy data[1], 1, n);
	thy columnLabels = NUMstrings_copy (my columnLabels, 1, n);
	if (thy columnLabels == NULL) forget (thee);
	return thee;
}

class_methods (Covariance, SSCP)
class_methods_end

class_methods (Correlation, SSCP)
class_methods_end

Covariance Covariance_create (long dimension)
{
	Covariance me = new (Covariance);
	if (me == NULL || ! SSCP_init (me, dimension)) forget (me);
	return me;
}

Correlation Correlation_create (long dimension)
{
	Correlation me = new (Correlation);
	if (me == NULL || ! SSCP_init (me, dimension)) forget (me);
	return me;
}

Covariance SSCP_to_Covariance (SSCP me, long numberOfConstraints)
{
	Covariance thee = Data_copy (me);
	long i, j;
	
	Melder_assert (numberOfConstraints >= 0);
	
	if (thee == NULL) return NULL;
	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = i; j <= my numberOfColumns; j++)
			thy data[j][i] = thy data[i][j] /= 
				(my numberOfObservations - numberOfConstraints);
	}
	Thing_overrideClass (thee, classCovariance);
	return thee;	
}

SSCP Covariance_to_SSCP (Covariance me)
{
	SSCP thee = Data_copy (me); 
	long i, j;
	
	if (thee == NULL) return NULL;
	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = i; j <= my numberOfColumns; j++)
			thy data[j][i] = thy data[i][j] *= (my numberOfObservations - 1);
	}
	Thing_overrideClass (thee, classSSCP);
	return thee;	
}

Correlation SSCP_to_Correlation (I)
{
	iam (SSCP); Correlation thee = Data_copy (me); long i, j;
	if (thee == NULL) return NULL;
	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = i; j <= my numberOfColumns; j++)
			thy data[j][i] = thy data[i][j] /= 
				sqrt (my data[i][i] * my data[j][j]);
	}
	Thing_overrideClass (thee, classCorrelation);
	return thee;	
}

double SSCP_getLnDeterminant (I)
{
	iam (SSCP);
	double ln_d;
	return NUMdeterminant_cholesky (my data, my numberOfRows, &ln_d) ?
		ln_d : NUMundefined;
}

int Covariance_difference (Covariance me, Covariance thee, double *prob, 
	double *chisq, long *ndf)
{
	long i, j, p = my numberOfRows;
	long numberOfObservations = my numberOfObservations;
	double **linv = NULL, ln_me, ln_thee, l, trace;
	
	if (my numberOfRows != thy numberOfRows)
	{ 
		return Melder_error1 (L"Covariance_difference: matrices don't have "
			"equal dimensions.");
	}
	if (my numberOfObservations != thy numberOfObservations)
	{
		numberOfObservations = (my numberOfObservations > 
			thy numberOfObservations ?
			thy numberOfObservations : my numberOfObservations) - 1;
		Melder_warning3 (L"Covariance_difference: number of observations of matrices do not agree.\n"
			" The minimum  size (", Melder_integer (numberOfObservations), L") of the two is used.");
	}
	if (numberOfObservations < 2)
	{
		return Melder_error1 (L"Covariance_difference: number of observations too small.");
	}
	
	if (((linv = NUMdmatrix_copy (thy data, 1, p, 1, p)) == NULL) ||
		(! NUMinverse_cholesky (linv, p, & ln_thee)) ||
		! NUMdeterminant_cholesky (my data, p, &ln_me)) goto end;
	/*
		We need trace (A B^-1). We have A and the inverse L^(-1) of the 
		cholesky decomposition L^T L of B in the lower triangle + diagonal.
		Always: tr (A B) = tr (B A)
		tr (A B^-1) = tr (A (L L^T)^-1) = tr (A L^-1 (L^T)^-1)
		trace = sum(i=1..p, j=1..p, l=max(i,j)..p, A[i][j]Lm[l][j]Lm[l][i],
		where Lm = L^(-1)
	*/	
	
	for (trace = 0, i = 1; i <= p; i++)
	{
		for (j = 1; j <= p; j++)
		{
			long l, lp = MAX (j, i);
			for (l = lp; l <= p; l++)
			{
				trace += my data[i][j] * linv[l][j] * linv[l][i];
			}
		}
	}
	
	l = (numberOfObservations - 1) * fabs (ln_thee - ln_me + trace - p);
	*chisq = l * fabs (1 - (2 * p + 1 - 2 / (p + 1)) / 
		(numberOfObservations - 1) / 6);
	*ndf = p * (p + 1) / 2;
	*prob = NUMchiSquareQ (*chisq, *ndf);
	
end:

	NUMdmatrix_free (linv, 1, 1);
	return ! Melder_hasError ();
}

static int checkOneIndex (I, long index)
{
	iam (TableOfReal);
	if (index < 1 || index > my numberOfColumns)
	{
		return Melder_error3 (L"Index must be in interval [1, ", 
			Melder_integer (my numberOfColumns), L"].");
	}
	return 1;
}

static int checkTwoIndices (I, long index1, long index2)
{
	iam (TableOfReal);

	if (index1 < 1 || index1 > my numberOfColumns || index2 < 1 ||
		index2 > my numberOfColumns)
	{
		return Melder_error3 (L"Index must be in interval [1, ", 
			Melder_integer (my numberOfColumns), L"].");
	}
	if (index1 == index2)
	{
		return Melder_error1 (L"Indices must be different.");
	}
	return 1;
}

void Covariance_getSignificanceOfOneMean (Covariance me, long index, double mu,
	double *probability, double *t, double *ndf)
{
	double var;
	*probability = *t = NUMundefined;
	*ndf = my numberOfObservations - 1;

	if (! checkOneIndex (me, index)) return;

	if ((var = my data[index][index]) == 0) return;

	*t = (my centroid[index] - mu) / sqrt (var / my numberOfObservations);
   	*probability = 2 * NUMstudentQ (fabs(*t), *ndf);
}

void Covariance_getSignificanceOfMeansDifference (Covariance me,
	long index1, long index2, double mu, int paired, int equalVariances, 
	double *probability, double *t, double *ndf)
{
	long n = my numberOfObservations;
	double df, var1, var2, var_pooled;

	*probability = *t = NUMundefined;
	*ndf = 2 * (n - 1);
	
	if (! checkTwoIndices (me, index1, index2)) return;
	
	var1 = my data[index1][index1];
	var2 = my data[index2][index2];	

	var_pooled = var1 + var2;
	if (var_pooled == 0)
	{
		Melder_warning1 (L"The pooled variance turned out to be zero. Check your data. ");
		return;
	}
	if (paired)
	{
		var_pooled -= 2 * my data[index1][index2];
		*ndf /= 2;
	}
	
	if (var_pooled == 0) 
	{
		Melder_warning1 (L"The pooled variance with the paired correction turned out to be zero. ");
		*probability = 0;
		return;
	}
	
	*t = (my centroid[index1] - my centroid[index2] - mu) / sqrt (var_pooled/n);
	
	/*
		Return two sided probabilty.
	*/
		
	if (equalVariances)
	{
		*probability = 2 * NUMstudentQ (fabs(*t), *ndf);
	}
	else
	{
		df = (1 + 2 * var1 * var2 / (var1 * var1 + var2 * var2)) * (n - 1);
		*probability = NUMincompleteBeta (df / 2, 0.5, df / (df + (*t) * (*t)));
		*ndf = df;
	}
}

void Covariance_getSignificanceOfOneVariance (Covariance me, long index, 
	double sigmasq, double *probability, double *chisq, long *ndf)
{
	double var;
	*probability = *chisq = NUMundefined;
	*ndf = my numberOfObservations - 1;
			
	if (checkOneIndex (me, index)) return;

	if ((var = my data[index][index]) == 0) return;

	*chisq = *ndf;
	if (sigmasq != 0)
	{ 
		*chisq = *ndf * var / sigmasq;
	}
   	*probability = NUMchiSquareQ (*chisq, *ndf);
}

void Covariance_getSignificanceOfVariancesRatio (Covariance me,
	long index1, long index2, double ratio, double *probability, 
	double *f, long *ndf)
{
	long n = my numberOfObservations;
	double var1, var2, ratio2;
	
	*ndf = n - 1;
	if (! checkTwoIndices (me, index1, index2)) return;
	
	var1 = my data[index1][index1];
	var2 = my data[index2][index2];	
	
	if (var1 == 0 || var2 == 0) return;
	
	*f = ratio2 = (var1 / var2) / ratio;
	if (var2 > var1) ratio2 = (var2 / var1) * ratio;

	*probability = 2 * NUMfisherQ (ratio2, *ndf, *ndf);
	if (*probability > 1) *probability = 2 - *probability;
}

TableOfReal Correlation_confidenceIntervals (Correlation me, 
	double confidenceLevel, long numberOfTests, int method)
{
	TableOfReal thee;
	double z, zf, two_n = 2 * my numberOfObservations;
	long i, j, m_bonferroni = my numberOfRows * (my numberOfRows - 1) / 2;

	if (confidenceLevel <= 0 || confidenceLevel > 1) return Melder_errorp1 (L"Confidence level must be in interval (0 - 1).");

	if (my numberOfObservations < 5) return Melder_errorp1 (L"The number of observations must be greater than 4.");
	
	if (numberOfTests < 0)
	{
		return Melder_errorp1 (L"The \"number of tests\" cannot be less than zero."); 
	}
	else if (numberOfTests == 0)
	{
		numberOfTests = m_bonferroni;
	}

	if (numberOfTests > m_bonferroni)
	{
		Melder_warning1 (L"Correlation_getConfidenceIntervals: \"number of tests\" exceeds the number of elements in the Correlation object.");
	}
	
	thee = TableOfReal_create (my numberOfRows, my numberOfRows);
	if (thee == NULL) return NULL;
	if (! TableOfReal_copyLabels (me, thee, 1, 1))
	{
		forget (thee);
		return NULL;
	}
	
	/*
		Obtain large-sample conservative multiple tests and intervals by the
		Bonferroni inequality and the Fisher z transformation.
		Put upper value of confidence intervals in upper part and lower
		values of confidence intervals in lower part of resulting table. 
	*/
		
	z = NUMinvGaussQ ((1 - confidenceLevel) / (2 * numberOfTests));
	
	zf = z / sqrt (my numberOfObservations - 3);

	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = i + 1; j <= my numberOfRows; j++)
		{
			double rij = my data[i][j], rmin, rmax;
			if (method == 2) 
			{
				/* Fisher's approximation */
				
				double zij = 0.5 * log ((1 + rij) / (1 - rij));
				rmax = tanh (zij + zf);	
				rmin = tanh (zij - zf);
			}
			else if (method == 1)
			{
				/* Ruben's approximation */
				
				double rs = rij / sqrt (1 - rij * rij);
				double a = two_n - 3 - z * z;
				double b = rs * sqrt((two_n - 3) * (two_n - 5));
				double c = (a - 2) * rs *rs - 2 * z * z;
				
				/*
					Solve:  a y^2 - 2b y + c = 0
					q = -0.5((-2b) + sgn(-2b) sqrt((-2b)^2 - 4ac))
					y1 = q/a; y2 = c/q;
				*/
				
				double q, d = sqrt (b * b - a * c);  
								
				if (b > 0) d = - d;
				q = b - d;
				rmin = q / a; rmin /= sqrt (1 + rmin * rmin);
				rmax = c / q; rmax /= sqrt (1 + rmax * rmax);
				if (rmin > rmax)
				{
					double t = rmin; rmin = rmax; rmax = t;
				}
			}
			else
			{
				rmax = rmin = 0;
			}
			thy data[i][j] = rmax;
			thy data[j][i] = rmin;
		}
		thy data[i][i] = 1;
	}
	return thee;
}

void SSCP_testDiagonality_bartlett (SSCP me, long numberOfContraints, 
	double *chisq, double *probability)
{
	Correlation c = SSCP_to_Correlation (me);
	
	*chisq = *probability = NUMundefined;
	
	if (c == NULL) return;
	Correlation_testDiagonality_bartlett (c, numberOfContraints,
		chisq, probability);
	forget (c);
	return; 	
}

/* Morrison, page 118 */
void Correlation_testDiagonality_bartlett (Correlation me, 
	long numberOfContraints, double *chisq, double *probability)
{
	double ln_determinant;
	long p = my numberOfRows;

	*chisq = *probability = NUMundefined;
	
	if (numberOfContraints <= 0) numberOfContraints = 1;
	if (numberOfContraints > my numberOfObservations)
	{
		Melder_warning1 (L"Correlation_testDiagonality_bartlett: number of constraints can not exceed the number of observations.");
		return;
	}
	
	if (! NUMdeterminant_cholesky (my data, p, &ln_determinant)) return;

	*chisq = - ln_determinant * (my numberOfObservations - numberOfContraints
		- (2 * p + 5) / 6);
	
	*probability = NUMchiSquareQ (*chisq, p * (p - 1) / 2);
}

#undef MAX
#undef MIN

/* End of file SSCP.c */
