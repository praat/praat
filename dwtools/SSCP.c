/* SSCP.c
 *
 * Copyright (C) 1993-2010 David Weenink
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
 djmw 20081119 TableOfReal_to_SSCP check if numbers are defined
 djmw 20090617 TableOfReal_to_SSCPs_byLabel better warnings for singular cases.
 djmw 20090629 +Covariances_getMultivariateCentroidDifference, Covariances_equality.
 djmw 20100106 +Covariance_and_TableOfReal_mahalanobis.
 djmw 20101019 Reduced storage Covariance.
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
static double ellipseScalefactor (I, double scale, int confidence)
{
	iam (SSCP);
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
		// very ugly, temporary hack
		scale *= 2 / (scale < 0 ? -1 : sqrt (n - 1));
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

void SSCPs_getEllipsesBoundingBoxCoordinates (SSCPs me, double scale, int confidence,
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
	if (my numberOfRows == 1) // diagonal
	{
		thy data [1][1] = my data [1][d1];
		thy data [2][2] = my data [1][d2];
	}
	else
	{
		thy data [1][1] = my data [d1][d1];
		thy data [2][2] = my data [d2][d2];
		thy data [2][1] = thy data [1][2] = my data [d1][d2];
	}
	thy centroid[1] = my centroid[d1];
	thy centroid[2] = my centroid[d2];
	thy numberOfObservations = my numberOfObservations;

	TableOfReal_setColumnLabel (thee, 1, my columnLabels[d1]);
	TableOfReal_setColumnLabel (thee, 2, my columnLabels[d2]);
	TableOfReal_setRowLabel (thee, 1, my columnLabels[d1]);
	TableOfReal_setRowLabel (thee, 2, my columnLabels[d2]);
	return thee;
}

SSCPs SSCPs_extractTwoDimensions (SSCPs me, long d1, long d2)
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

SSCP SSCP_toTwoDimensions (I, double *v1, double *v2)
{
	iam (SSCP);
	SSCP thee;
	double *vec[3];

	if (! (thee = SSCP_create (2))) return NULL;

	/*
		Projection P of S on v1 and v2 (given matrix V' with 2 rows) is P = V'SV
		P[i][j] = sum(k) sum(m) V'[i][k]*S[k][m]*V[m][j] = V'[i][k]*S[k][m]*V'[j][m]

		For the new centroids cnew[i] = sum(m) V'[i][m]*c[m]
	*/
	vec[1] = v1; vec[2] = v2;
	if (my numberOfRows == 1) // 1xn diagonal matrix
	{
		for (long k = 1; k <= my numberOfColumns; k++) { thy data[1][1] += v1[k] * my data[1][k] * v1[k]; }
		for (long k = 1; k <= my numberOfColumns; k++) { thy data[1][2] += v1[k] * my data[1][k] * v2[k]; }
		for (long k = 1; k <= my numberOfColumns; k++) { thy data[2][2] += v2[k] * my data[1][k] * v2[k]; }
		thy data[2][1] = thy data[1][2];
	}
	else
	{
		for (long i = 1; i <= 2; i++)
		{
			for (long j = i; j <= 2; j++)
			{
				double sum = 0;
				for (long k = 1; k <= my numberOfRows; k++)
				{
					for (long m = 1; m <= my numberOfRows; m++)
					{
						sum += vec[i][k] * my data[k][m] * vec[j][m];
					}
				}
				thy data[j][i] = thy data[i][j] = sum;
			}
		}
	}

	// centroids

	for (long m = 1; m <= my numberOfColumns; m++) thy centroid[1] += v1[m] * my centroid[m];
	for (long m = 1; m <= my numberOfColumns; m++) thy centroid[2] += v2[m] * my centroid[m];

	thy numberOfObservations = SSCP_getNumberOfObservations (me);

 	return thee;
}

int SSCP_init (I, long dimension, long storage)
{
	iam (SSCP);
	if (! TableOfReal_init (me, storage, dimension) ||
		! (my centroid = NUMdvector (1, dimension))) return 0;
	return 1;
}

SSCP SSCP_create (long dimension)
{
	SSCP me = new (SSCP);

	if (! me || ! SSCP_init (me, dimension, dimension)) forget (me);
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

double SSCP_getFractionVariation (I, long from, long to)
{
	iam (SSCP);
	double sum = 0, trace = 0;
	long i, n = my numberOfRows;

	if (from < 1 || from > to || to > n) return NUMundefined;

	for (i = 1; i <= n; i++)
	{
		trace += my numberOfRows == 1 ? my data[1][i] : my data[i][i];
		if (i >= from && i <= to) sum += my numberOfRows == 1 ? my data[1][i] : my data[i][i];
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
	iam (SSCP);
	double trace = 0;
	for (long i = 1; i <= my numberOfColumns; i++)
	{
		trace += my numberOfRows == 1 ? my data[1][i] : my data[i][i];
	}
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
			partial += my numberOfRows == 1 ? my data[1][i] : my data[i][i];
		}
		if (sum > 0) sum = partial / sum;
	}
	return sum;
}

/* For nxn matrix only ! */
void Covariance_and_PCA_generateOneVector (Covariance me, PCA thee, double *vec, double *buf)
{
	// Generate the multi-normal vector elements N(0,sigma)

	for (long j = 1; j <= my numberOfColumns; j++) { buf[j] = NUMrandomGauss (0, sqrt (thy eigenvalues[j])); }

	// Rotate back

	for (long j = 1; j <= my numberOfColumns; j++)
	{
		vec[j] = 0;
		for (long k = 1; k <= my numberOfColumns; k++)
		{
			vec[j] += buf[k] * thy eigenvectors[k][j];
		}
	}

	// Restore the centroid

	for (long j = 1; j <= my numberOfColumns; j++) { vec[j] += my centroid[j]; }
}

TableOfReal Covariance_to_TableOfReal_randomSampling (Covariance me, long numberOfData)
{
	TableOfReal thee = NULL;
	PCA pca = NULL;
	double *buf = NULL;

	if (numberOfData <= 0) numberOfData = my numberOfObservations;

	if (((pca = SSCP_to_PCA (me)) == NULL) ||
		((thee = TableOfReal_create (numberOfData, my numberOfColumns)) == NULL) ||
		((buf = NUMdvector (1, my numberOfColumns)) == NULL)) goto end;

	for (long i = 1; i <= numberOfData; i++)
	{
		Covariance_and_PCA_generateOneVector (me, pca, thy data[i], buf);
	}

	NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, my numberOfColumns);

end:
	NUMdvector_free (buf, 1);
	forget (pca);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

TableOfReal Covariance_to_TableOfReal_randomSampling2 (Covariance me,
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
	long i, j, k, m, n, nvalidrows = 0;
	double **v = NULL;

	if (! TableOfReal_areAllCellsDefined (me, rowb, rowe, colb, cole)) return NULL;

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
		nvalidrows++;
		for (j = 1; j <= n; j++)
		{
			v[i][j] = my data[rowb + i - 1][colb + j - 1];
		}
	}

	NUMcentreColumns (v, 1, m, 1, n, thy centroid);

	SSCP_setNumberOfObservations (thee, m);

	/*
		sum of squares and cross products = T'T
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

TableOfReal Covariance_and_TableOfReal_mahalanobis (Covariance me, thou, bool useTableCentroid)
{
	thouart (TableOfReal);
	TableOfReal him = NULL;
	double lndet, **covari = NULL, *centroid = NULL;

	if (((him = TableOfReal_create (thy numberOfRows, 1)) == NULL) ||
		((centroid = NUMdvector_copy (my centroid, 1, thy numberOfColumns)) == NULL) ||
		((covari = NUMdmatrix_copy (my data, 1, my numberOfRows, 1, my numberOfRows)) == NULL)) goto end;

	/*
		Mahalanobis distance calculation. S = L.L' -> S**-1 = L**-1' . L**-1
		(x-m)'S**-1 (x-m) = (x-m)'L**-1' . L**-1. (x-m) =
			(L**-1.(x-m))' . (L**-1.(x-m))
	*/

	// Get inverse of covari in lower triangular part.
	if (! NUMlowerCholeskyInverse (covari, my numberOfRows, &lndet)) goto end;

	if (useTableCentroid)
	{
		for (long icol = 1; icol <= thy numberOfColumns; icol++)
		{
			double mean = 0;
			for (long irow = 1; irow <= thy numberOfRows; irow++) { mean += thy data[irow][icol]; }
			centroid[icol] = mean / thy numberOfRows;
		}
	}

	for (long k = 1; k <= thy numberOfRows; k++)
	{
		his data[k][1] = sqrt (NUMmahalanobisDistance_chi (covari, thy data[k], centroid, my numberOfRows, my numberOfRows));
		if (thy rowLabels[k] != NULL) TableOfReal_setRowLabel (him, k, thy rowLabels[k]);
	}
	TableOfReal_setColumnLabel (him, 1, L"d");
end:
	NUMdmatrix_free (covari, 1, 1);
	NUMdvector_free (centroid, 1);
	if (Melder_hasError ()) forget (him);
	return him;
}


TableOfReal Covariance_and_TableOfReal_extractDistanceQuantileRange (Covariance me, thou,
	double qlow, double qhigh)
{
	thouart (TableOfReal);
	double low, high;
	long nsel = 0, i, k;

	TableOfReal r = NULL, him = Covariance_and_TableOfReal_mahalanobis (me, thee, false);
	if (him == NULL) goto end;

	low = TableOfReal_getColumnQuantile (him, 1, qlow);
	high = TableOfReal_getColumnQuantile (him, 1, qhigh);

	/*
		Count the number filtered.
		nsel = (qhigh - qlow) * nrows is sometimes one off
	*/

	for (i = 1; i <= thy numberOfRows; i++)
	{
		if (low <= his data[i][1] && his data[i][1] < high) nsel++;
	}

	if (nsel < 1) return Melder_errorp1 (L"Not enough data in quantile interval.");

	r = TableOfReal_create (nsel, thy numberOfColumns);
	if (r == NULL ||
		((r -> columnLabels = NUMstrings_copy (thy columnLabels, 1, thy numberOfColumns)) == NULL)) goto end;

	for (k = 0, i = 1; i <= thy numberOfRows; i++)
	{
		if (low <= his data[i][1] && his data[i][1] < high)
		{
			k++;
			if (! TableOfReal_copyOneRowWithLabel (thee, r, i, k)) goto end;
		}
	}

end:
	forget (him);
	if (Melder_hasError ()) forget (r);
	return r;
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
	long i, index = 1, numberOfCases = my numberOfRows, nrows, lastrow = 0, ncols = my numberOfColumns;
	long ngroups = 0, nsingular = 0;
	wchar_t *label;

	if ((thee = SSCPs_create ()) == NULL) return NULL;

	mew = TableOfReal_sortOnlyByRowLabels (me);
	if (mew == NULL) goto end;

	label = mew -> rowLabels[1];
	Melder_warningOff ();
	for (i = 2; i <= numberOfCases; i++)
	{
		nrows = 0;
		wchar_t *li = mew -> rowLabels[i];
		if (li != NULL && li != label && wcscmp (li, label))
		{ // current label different from previous one(s)
			nrows = i - index; lastrow = i - 1;
		}
		else if (i == numberOfCases)
		{ // current (last) label is same as previous
			nrows = i - index + 1; lastrow = i;
		}
		else
		{ // next one
			continue;
		}
		// We found a new group
		ngroups++;
		if (nrows > 1) // We need at least two rows for an SSCP
		{
			if (nrows < ncols) nsingular++;
			t = TableOfReal_to_SSCP (mew, index, lastrow, 0, 0);
			if (t == NULL || ! Collection_addItem (thee, t)) goto end;
			if(! (label = mew -> rowLabels[index])) label = L"?";
			Thing_setName (t, label);
		}
		label = li; index = i;
	}
	if (lastrow != numberOfCases) ngroups++;
	Melder_warningOn ();
	if (nsingular > 0 || thy size != ngroups)
	{
		long notIncluded = ngroups - thy size;
		Melder_warning6 (Melder_integer (ngroups), L" different groups detected: ", Melder_integer (nsingular + notIncluded),
		L" group(s) with less rows than columns (of which ", Melder_integer (notIncluded), L" with only one row).");
	}
end:

	forget (mew);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

PCA SSCP_to_PCA (I)
{
	iam (SSCP);
	double **data = NULL;
	PCA thee = PCA_create (my numberOfColumns, my numberOfColumns);

	if (thee == NULL) return NULL;
	if (my numberOfRows == 1) // 1xn matrix -> nxn
	{
		// ugly hack
		data = NUMdmatrix (1, my numberOfColumns, 1, my numberOfColumns);
		if (data == NULL) goto end;
		for (long i = 1; i <= my numberOfColumns; i++) data[i][i] = my data[1][i];
	}
	else
	{ data = my data; }
	if (! NUMstrings_copyElements (my columnLabels, thy labels, 1, my numberOfColumns) ||
		! Eigen_initFromSymmetricMatrix (thee, data, my numberOfColumns)) goto end;
	NUMdvector_copyElements (my centroid, thy centroid, 1, my numberOfColumns);
	PCA_setNumberOfObservations (thee, my numberOfObservations);
end:
	if (data != my data) NUMdmatrix_free (data, 1, 1);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

int SSCP_setValue (I, long row, long col, double value)
{
	iam (SSCP);
	if (col < 0 || col > my numberOfColumns) return Melder_error1 (L"Illegal column number.");
	if (row < 0 || row > my numberOfColumns) return Melder_error1 (L"Illegal row number."); // ! yes numberOfColumns
	if (row == col && value <= 0) return Melder_error1 (L"Diagonal element must always be a positive number.");
	if (my numberOfRows == 1) // diagonal
	{
		if (row != col)	return Melder_error1 (L"Row and column number must be equal for a diagonal matrix.");
		my data[1][row] = value;
	}
	else
	{
		if (row != col && (fabs(value) > my data[row][row] || fabs(value) > my data[row][row])) return Melder_error1
			(L"The off-diagonal cannot be larger than the diagonal values. Input diagonal elements first, or change this value.");
		my data[row][col] = my data[col][row] = value;
	}
	return 1;
}

int SSCP_setCentroid (I, long component, double value)
{
	iam (SSCP);
	if (component < 1 || component > my numberOfColumns) return Melder_error1 (L"Illegal component number.");
	my centroid[component] = value;
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
	if (my numberOfRows == 1) return Melder_errorp1 (L"Matrix is diagonal.");
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
		SSCP t = my item[k];
		long no = t -> numberOfObservations;

		if (t -> numberOfRows != thy numberOfRows)
		{
			forget (thee);
			return Melder_errorp3 (L"SSCPs_sum: unequal dimensions (", Melder_integer (k), L").");
		}

		thy numberOfObservations += no;

		/*
			Sum the sscp's and weigh the centroid.
		*/

		for (i = 1; i <= thy numberOfRows; i++) // if 1xn
		{
			for (j = 1; j <= thy numberOfColumns; j++)
			{
				thy data[i][j] += t -> data[i][j];
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
		SSCP t = SSCP_toTwoDimensions (my item[i], v1, v2);
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

	if (! (thee = SSCPs_extractTwoDimensions (me, d1, d2))) return;
	SSCPs_getEllipsesBoundingBoxCoordinates (thee, scale, confidence, &xmn, &xmx, &ymn, &ymx);

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
    	Graphics_textLeft (g, 1, t -> columnLabels[d2] ? t -> columnLabels[d2] : text);
    	Graphics_marksBottom (g, 2, 1, 1, 0);
		swprintf (text, 20, L"Dimension %ld", d1);
		Graphics_textBottom (g, 1, t -> columnLabels[d1] ? t -> columnLabels[d1] : text);
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
	if (me == NULL || ! SSCP_init (me, dimension, dimension)) forget (me);
	return me;
}

Covariance Covariance_create_reduceStorage (long dimension, long storage)
{
	Covariance me = new (Covariance);
	if (storage <= 0 || storage >= dimension) storage = dimension;
	if (me == NULL || ! SSCP_init (me, dimension, storage)) forget (me);
	return me;
}

Covariance Covariance_createSimple (long dimension, wchar_t *variances, wchar_t *centroid, long numberOfObservations)
{
	Covariance me = Covariance_create (dimension);
	if (me == NULL) return me;

	long inum = 1;
	for (wchar_t *token = Melder_firstToken (variances); token != NULL && inum <= dimension; token = Melder_nextToken (), inum++)
	{
		double number = Melder_atof (token);
		if (number <= 0) { Melder_error1 (L"Variances must be positive numbers."); goto end; }
		my data[inum][inum] = number;
	}
	inum--;
	for (long i = inum; i <= dimension; i++) { my data[i][i] = my data[inum][inum]; } // repeat last number given

	inum = 1;
	for (wchar_t *token = Melder_firstToken (centroid); token != NULL && inum <= dimension; token = Melder_nextToken (), inum++)
	{
		double number = Melder_atof (token);
		my centroid[inum] = number;
	}
	inum--;
	for (long i = inum; i <= dimension; i++) { my centroid[i] = my centroid[inum]; } // repeat last number given

	my numberOfObservations = numberOfObservations;
end:
	if (Melder_hasError ()) forget (me);
	return me;
}

Correlation Correlation_create (long dimension)
{
	Correlation me = new (Correlation);
	if (me == NULL || ! SSCP_init (me, dimension, dimension)) forget (me);
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
			thy data[j][i] = thy data[i][j] /= (my numberOfObservations - numberOfConstraints);
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

static Covariance Covariances_pool (Covariance me, Covariance thee)
{
	if (my numberOfRows != thy numberOfRows || my numberOfColumns != thy numberOfColumns) return Melder_errorp1
		(L"Matrices must have equal dimensions.");
	SSCPs sscps = SSCPs_create ();
	if (sscps == NULL) goto end;
	SSCP sscp1 = Covariance_to_SSCP (me);
	if (sscp1 == NULL || ! Collection_addItem (sscps, sscp1)) goto end;
	SSCP sscp2 = Covariance_to_SSCP (thee);
	if (sscp2 == NULL || ! Collection_addItem (sscps, sscp2)) goto end;
	SSCP pool = SSCPs_to_SSCP_pool (sscps);
	if (pool == NULL) goto end;
	Covariance him = SSCP_to_Covariance (pool, 2);
	if (him == NULL) goto end;
end:
	forget (sscps); forget (pool);
	if (Melder_hasError ()) forget (him);
	return him;
}

static double **productOfSquareMatrices (double **s1, double **s2, long n)
{
	double **r = NUMdmatrix (1, n, 1, n);
	if (r == NULL) return NULL;
	for (long i = 1; i <= n; i++)
	{
		for (long j = 1; j <= n; j++)
		{
			double sum = 0;
			for (long k = 1; k <= n; k++)
			{
				sum += s1[i][k] * s2[k][j];
			}
			r[i][j] = sum;
		}
	}
	return r;
}

static double traceOfSquaredMatrixProduct (double **s1, double **s2, long n)
{
	// tr ((s1*s2)^2), s1, s2 are symmetric
	double trace2, **m = NULL;
	m = productOfSquareMatrices (s1, s2, n);
	if (m == NULL) return NUMundefined;
	trace2 = NUMtrace2 (m, m, n);
	NUMdmatrix_free (m, 1, 1);
	return trace2;
}


double Covariance_getProbabilityAtPosition_string (Covariance me, wchar_t *vector)
{
	long i = 0;
	double *v = NUMdvector (1, my numberOfColumns);
	if (v == NULL) return NUMundefined;

	for (wchar_t *token = Melder_firstToken (vector); token != NULL; token = Melder_nextToken ())
	{
		v[++i] = Melder_atof (token);
		if (i == my numberOfColumns) break;
	}
	double p = Covariance_getProbabilityAtPosition (me, v);
	NUMdvector_free (v, 1);
	return p;
}

double Covariance_getProbabilityAtPosition (Covariance me, double *x)
{
	if (my lowerCholesky == NULL && ! SSCP_expandLowerCholesky (me)) return NUMundefined;
	double ln2pid = my numberOfColumns * log (NUM2pi);
	double dsq = NUMmahalanobisDistance_chi (my lowerCholesky, x, my centroid, my numberOfRows, my numberOfColumns);
	double lnN = - 0.5 * (ln2pid + my lnd + dsq);
	double p =  exp (lnN);
	return p;
}

double Covariance_getMarginalProbabilityAtPosition (Covariance me, double *vector, double x)
{
	double mu, stdev;
	Covariance_getMarginalDensityParameters (me, vector, &mu, &stdev);
	double dx = (x - mu) / stdev;
	double p = (NUM1_sqrt2pi / stdev) * exp (- 0.5 * dx * dx);
	return p;
}

/* Precondition ||v|| = 1 */
void Covariance_getMarginalDensityParameters (Covariance me, double *v, double *mu, double *stdev)
{
	*stdev = *mu = 0;
	if (my numberOfRows == 1) // 1xn diagonal matrix
	{
		for (long m = 1; m <= my numberOfColumns; m++) { *stdev += v[m] * my data[1][m] * v[m]; }
	}
	else
	{
		for (long k = 1; k <= my numberOfRows; k++)
		{
			for (long m = 1; m <= my numberOfColumns; m++)
			{
				*stdev += v[k] * my data[k][m] * v[m];
			}
		}
	}
	*stdev = sqrt (*stdev);
	for (long m = 1; m <= my numberOfColumns; m++) *mu += v[m] * my centroid[m];
}

double Covariances_getMultivariateCentroidDifference (Covariance me, Covariance thee, int equalCovariances, double *prob, double *fisher, double *df1, double *df2)
{
	long i, j, p = my numberOfRows, N = my numberOfObservations + thy numberOfObservations;
	long N1 = my numberOfObservations, n1 = N1 - 1;
	long N2 = thy numberOfObservations, n2 = N2 - 1;
	double lndet, mahalanobis, hotelling_tsq, dif, **s = NULL;
	dif = *prob = *fisher = NUMundefined;
	*df1 = p;
	*df2 = N - p - 1;
	if (*df2 < 1)
	{
		Melder_error3 (L"Not enough observations (", Melder_integer (N), L") for this test.");
		return dif;
	}
	if (N1 < p || N2 < p)
	{
		Melder_error1 (L"At least one of the covariances has less observations than variables.");
		return dif;
	}

	dif = 0;
	for (i = 1; i <= p; i++)
	{
		double dist = my centroid[i] - thy centroid[i];
		dif += dist * dist;
	}
	dif = sqrt (dif);

	if (equalCovariances)
	{
		/* Morrison, page 141 */
		Covariance pool = Covariances_pool (me, thee);
		if (pool == NULL) return 0;
		s = NUMdmatrix_copy (my data, 1, p, 1, p);
		if (s == NULL || ! NUMlowerCholeskyInverse (s, p, &lndet)) goto end1;

		mahalanobis = NUMmahalanobisDistance_chi (s, my centroid, thy centroid, p, p);
		hotelling_tsq = mahalanobis * N1 * N2 / N;
		*fisher = hotelling_tsq * *df2 / ((N - 2) * *df1);
end1:
		NUMdmatrix_free (s, 1, 1);
		forget (pool);
	}
	else
	{
		/* Krishnamoorthy-Yu (2004): Modified Nel and Van der Merwe test

			Hotelling t^2 = (x1-x2)'*S^-1*(x1 -x2) follows nu*p*Fisher(p,nu-p+1)/(nu-p+1)

			Approximate number of degrees of freedom  (their formula 7, page 164)
			nu = (p+p^2)/((1/n1)(tr (S1*S^-1)^2 + (tr(S1*S^-1))^2)) +(1/n2)(tr (S2*S^-1)^2 + (tr(S2*S^-1))^2)))
			the matrices S1 and S2 are the covariance matrices 'my data' and 'thy data' divided by N1 and N2 respectively.
			S is the pooled covar divided by N.
		*/
		double **s1 = NULL, **s2 = NULL, **si = NULL;
		if (((s1 = NUMdmatrix (1, p, 1, p)) == NULL) || ((s2 = NUMdmatrix (1, p, 1, p)) == NULL) ||
			 ((s = NUMdmatrix (1, p, 1, p)) == NULL) || ((si = NUMdmatrix (1, p, 1, p)) == NULL)) goto end2;

		for (i = 1; i <= p; i++)
		{
			for (j = 1; j <=p; j++)
			{
				s1[i][j] = my data[i][j] / my numberOfObservations;
				s2[i][j] = thy data[i][j] / thy numberOfObservations;
				s[i][j] = s1[i][j] + s2[i][j];
			}
		}

		if (! NUMlowerCholeskyInverse (s, p, &lndet)) goto end1;
		hotelling_tsq= NUMmahalanobisDistance_chi (s, my centroid, thy centroid, p, p); // Krishan... formula 2, page 162

		if (((si = NUMinverseFromLowerCholesky (s, p)) == NULL)) goto end2;
		double tr_s1sisqr = traceOfSquaredMatrixProduct (s1, si, p);
		if (! NUMdefined (tr_s1sisqr)) goto end2;
		double tr_s1si = NUMtrace2 (s1, si, p);
		if (! NUMdefined (tr_s1si)) goto end2;
		double tr_s2sisqr = traceOfSquaredMatrixProduct (s2, si, p);
		if (! NUMdefined (tr_s2sisqr)) goto end2;
		double tr_s2si = NUMtrace2 (s2, si, p);
		if (! NUMdefined (tr_s2si)) goto end2;
		double nu = (p + p * p) / ((tr_s1sisqr + tr_s1si * tr_s1si) / n1 + (tr_s2sisqr + tr_s2si * tr_s2si) / n2);
		*df2 = nu - p + 1;
		*fisher =  hotelling_tsq * (nu - p + 1) / (nu * p);
end2:
		NUMdmatrix_free (s1, 1, 1); NUMdmatrix_free (s2, 1, 1);
		NUMdmatrix_free (s, 1, 1); NUMdmatrix_free (si, 1, 1);
	}

	*prob = NUMfisherQ (*fisher, *df1, *df2);
	return dif;
}

/* Schott 2001 */
void Covariances_equality (Ordered me, int method, double *prob, double *chisq, double *df)
{
	long i, j, k, nc = my size, p = 1;
	double lnd, **s = NULL, m, c1, nsi = 0, ni, nj, ns = 0;
	Covariance ci, cj;

	*prob = *chisq = *df = NUMundefined;

	if (nc < 2) return;

	for (i = 1; i <= nc; i++)
	{
		ci = my item[i];
		ni = ci -> numberOfObservations - 1;
		if (i == 1) p = ci -> numberOfRows;
		if (ci -> numberOfRows != p)
		{
			Melder_error3 (L"The dimensions of matrix ", Melder_integer (i),
			L" differ from the previous one(s).");
			return;
		}
		if (ni < p)
		{
			Melder_error3 (L"The number of observations in matrix ", Melder_integer (i),
			L" is less than the number of variables. ");
			return;
		}
		ns += ni; nsi += 1.0 / ni;
	}

	if ((s = NUMdmatrix (1, p, 1, p)) == NULL) goto end;

	for (i = 1; i <= nc; i++) // pool
	{
		ci = my item[i];
		double sf = (ci -> numberOfObservations - 1.0) / ns;
		for (j = 1; j <= p; j++)
		{
			for (k = 1; k <= p; k++)
			{
				s[j][k] += sf * ci -> data[j][k];
			}
		}
	}

	if (method == 1) // bartlett
	{
		if (! NUMdeterminant_cholesky (s, p, &lnd))
		{
			Melder_error1 (L"Pooled covariance matrix is singular.");
			return;
		}

		m = ns * lnd;
		for (i = 1; i <= nc; i++)
		{
			ci = my item[i];
			if (! NUMdeterminant_cholesky (ci -> data, p, &lnd))
			{
				Melder_error3 (L"Covariance matrix ", Melder_integer (i), L" is singular.");
				return;
			}
			m -= (ci -> numberOfObservations - 1) * lnd;
		}

		c1 = 1.0 - (2.0 * p * p - 3 * p - 1) / (6.0 * (p + 1) * (nc - 1)) * (nsi - 1 / ns);

		*df = (nc - 1) * p * (p + 1) / 2;
		*chisq = m * c1;
	}
	else if (method == 2) // Schott (2001) Wald 1
	{
		/* sum(i, ni/n *tr((si*s^-1)^2)- sum(i,sum(j, (ni/n)*(nj/n) *tr(si*s^-1*sj*s^-1))) =
			sum(i=1..k, (ni/n -(ni/n)^2) tr((si*s^-1)^2)
				- 2 * sum (i=1..k, sum(j=1..i-1, (ni/n)*(nj/n) *tr(si*s^-1*sj*s^-1)))
		*/
		double trace_ii, trace_ij, trace = 0;
		double **si = NULL, **s1 = NULL, **s2 = NULL;
		if (! NUMlowerCholeskyInverse (s, p, NULL)) goto end;
		if (((si = NUMinverseFromLowerCholesky (s, p)) == NULL)) goto end;
		for (i = 1; i <= nc; i++)
		{
			ci = my item[i];
			ni = ci -> numberOfObservations - 1;
			s1 = productOfSquareMatrices (ci -> data, si, p);
			if (s1 != NULL)
			{
				trace_ii = NUMtrace2 (s1, s1, p);
				trace += (ni / ns) * (1 - (ni / ns)) * trace_ii;
				for (j = i + 1; j <= nc; j++)
				{
					cj = my item[j];
					nj = cj -> numberOfObservations - 1;
					s2 = productOfSquareMatrices (cj -> data, si, p);
					if (s2 != NULL)
					{
						trace_ij = NUMtrace2 (s1, s2, p);
						trace -= 2 * (ni / ns) * (nj / ns) * trace_ij;
					}
					NUMdmatrix_free (s2, 1, 1);
					if (Melder_hasError ()) break;
				}
			}
			NUMdmatrix_free (s1, 1, 1);
			if (Melder_hasError ()) break;
		}
		NUMdmatrix_free (si, 1, 1);
		if (Melder_hasError ()) goto end;
		*df = (nc - 1) * p * (p + 1) / 2;
		*chisq = (ns / 2) * trace;
	}
	else return;

	*prob = NUMchiSquareQ (*chisq, *df);
end:
	NUMdmatrix_free (s, 1, 1);
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
		(! NUMlowerCholeskyInverse (linv, p, & ln_thee)) ||
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

	*ndf = n - 1; *probability = *f = NUMundefined;
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

int SSCP_expand (I)
{
	iam(SSCP);
	/*
		A reduced matrix has my numberOfRows < my numberOfColumns.
		After expansion:
		my numberOfRows == my numberOfColumns
		my storageNumberOfRows = my numberOfRows (before)
		my data (after) = my expansion;
		my expansion = my data (before)
		No expansion for a standard matrix or if already expanded and data has not changed!
	*/
	if ((my expansionNumberOfRows == 0 && my numberOfRows == my numberOfColumns) ||
		(my expansionNumberOfRows > 0 && ! my dataChanged)) return 1;
	if (my expansion == NULL &&
		((my expansion = NUMdmatrix (1, my numberOfColumns, 1, my numberOfColumns)) == NULL)) return 0;
	for (long ir = 1; ir <= my numberOfColumns; ir++)
	{
		for (long ic = ir; ic <= my numberOfColumns; ic++)
		{
			long dij = abs (ir - ic);
			my expansion[ir][ic] = my expansion[ic][ir] = dij < my numberOfRows ? my data[dij + 1][ic] : 0;
		}
	}
	// Now make 'my data' point to 'my expansion'
	double **tmp = my data; my data = my expansion; my expansion = tmp;
	my expansionNumberOfRows = my numberOfRows;
	my numberOfRows = my numberOfColumns; // Now forget(me) is save
	my dataChanged = 0;

	if (Melder_hasError ())
	{
		NUMdmatrix_free (my expansion, 1, 1);
		return 0;
	}
	return 1;
}

void SSCP_unExpand (I)
{
	iam(SSCP);
	if (my expansionNumberOfRows == 0) return;
	NUMdmatrix_free (my data, 1, 1);
	my data = my expansion;
	my expansion = NULL;
	my numberOfRows = my expansionNumberOfRows;
	my expansionNumberOfRows = 0;
	my dataChanged = 0;
}

int SSCP_expandLowerCholesky (I)
{
	iam (SSCP);
	if ((my lowerCholesky == NULL) &&
		((my lowerCholesky = NUMdmatrix (1, my numberOfRows, 1, my numberOfColumns)) == NULL)) return 0;
	if (my numberOfRows == 1) // diagonal
	{
		my lnd = 0;
		for (long j = 1; j <= my numberOfColumns; j++)
		{
			my lowerCholesky[1][j] = 1 / sqrt(my data[1][j]); // inverse is 1/stddev
			my lnd += log (my data[1][j]); // diagonal elmnt is variance
		}
	}
	else
	{
		for (long i = 1; i <= my numberOfRows; i++)
			for (long j = i; j <= my numberOfColumns; j++)
			{ my lowerCholesky[j][i] = my lowerCholesky[i][j] = my data[i][j]; }
		if (! NUMlowerCholeskyInverse (my lowerCholesky, my numberOfColumns, &(my lnd)))
		{
			// singular matrix: arrange a diagonal only inverse.
			my lnd = 0;
			for (long i = 1; i <= my numberOfRows; i++)
			{
				for (long j = i; j <= my numberOfColumns; j++)
				{
					my lowerCholesky[i][j] =  my lowerCholesky[j][i] = i == j ? 1. / sqrt (my data[i][i]) : 0;
				}
				my lnd += log (my data[i][i]);
			}
			my lnd *= 2;
		}
	}
	return 1;
}

void SSCP_unExpandLowerCholesky (I)
{
	iam (SSCP);
	NUMdmatrix_free (my lowerCholesky, 1, 1);
	my lnd = 0;
}

int SSCP_expandPCA (I)
{
	iam (SSCP);
	if (my pca != NULL) forget (my pca);
	return (my pca = SSCP_to_PCA (me)) != NULL;
}

void SSCP_unExpandPCA (I)
{
	iam (SSCP);
	forget (my pca);
}


#undef MAX
#undef MIN

/* End of file SSCP.c */
