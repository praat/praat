/* SSCP.cpp
 *
 * Copyright (C) 1993-2012 David Weenink
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
  djmw 20110304 Thing_new
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

#undef MAX
#undef MIN
#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))
#define TOVEC(x) (&(x) - 1)

Thing_implement (SSCP, TableOfReal, 0);

void structSSCP :: v_info () {
	structTableOfReal :: v_info ();
	double zmin, zmax;
	NUMmatrix_extrema<double> (data, 1, numberOfRows, 1, numberOfColumns, &zmin, &zmax);
	MelderInfo_writeLine (L"Minimum value: ", Melder_double (zmin));
	MelderInfo_writeLine (L"Maximum value: ", Melder_double (zmax));
}

/*
	Calculate scale factor by which sqrt(eigenvalue) has to
	be multiplied to obtain the length of an ellipse axis.
*/
static double ellipseScalefactor (I, double scale, int confidence) {
	iam (SSCP);
	long n = SSCP_getNumberOfObservations (me);

	if (confidence) {
		long p = my numberOfColumns;
		double f;

		if (n - p < 1) {
			return -1;
		}
		/* D.E. Johnson (1998), Applied Multivariate methods, page 410 */
		f = NUMinvFisherQ (1 - scale, p, n - p);
		scale = 2 * sqrt (f * p * (n - 1) / ( ( (double) n) * (n - p)));
	} else {
		// very ugly, temporary hack
		scale *= 2 / (scale < 0 ? -1 : sqrt (n - 1));
	}
	return scale;
}

static void getEllipseBoundingBoxCoordinates (SSCP me, double scale, int confidence,
        double *xmin, double *xmax, double *ymin, double *ymax) {
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
        double *xmin, double *xmax, double *ymin, double *ymax) {
	*xmin = *ymin = 1e38;
	*xmax = *ymax = - *xmin;

	for (long i = 1; i <= my size; i++) {
		SSCP s = (SSCP) my item[i];
		double xmn, xmx, ymn, ymx;
		getEllipseBoundingBoxCoordinates (s, scale, confidence, &xmn, &xmx, &ymn, &ymx);
		if (xmn < *xmin) {
			*xmin = xmn;
		}
		if (xmx > *xmax) {
			*xmax = xmx;
		}
		if (ymn < *ymin) {
			*ymin = ymn;
		}
		if (ymx > *ymax) {
			*ymax = ymx;
		}
	}
}

static SSCP _SSCP_extractTwoDimensions (SSCP me, long d1, long d2) {
	autoSSCP thee = SSCP_create (2);
	if (my numberOfRows == 1) { // diagonal
		thy data [1][1] = my data [1][d1];
		thy data [2][2] = my data [1][d2];
	} else {
		thy data [1][1] = my data [d1][d1];
		thy data [2][2] = my data [d2][d2];
		thy data [2][1] = thy data [1][2] = my data [d1][d2];
	}
	thy centroid[1] = my centroid[d1];
	thy centroid[2] = my centroid[d2];
	thy numberOfObservations = my numberOfObservations;

	TableOfReal_setColumnLabel (thee.peek(), 1, my columnLabels[d1]);
	TableOfReal_setColumnLabel (thee.peek(), 2, my columnLabels[d2]);
	TableOfReal_setRowLabel (thee.peek(), 1, my columnLabels[d1]);
	TableOfReal_setRowLabel (thee.peek(), 2, my columnLabels[d2]);
	return thee.transfer();
}

SSCPs SSCPs_extractTwoDimensions (SSCPs me, long d1, long d2) {
	try {
		autoSSCPs thee = SSCPs_create ();
		for (long i = 1; i <= my size; i++) {
			autoSSCP t = _SSCP_extractTwoDimensions ( (SSCP) my item[i], d1, d2);
			Thing_setName (t.peek(), Thing_getName ( (Thing) my item[i]));
			Collection_addItem (thee.peek(), t.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": cannot extract two dimensions.");
	}
}

static void _SSCP_drawTwoDimensionalEllipse (SSCP me, Graphics g, double scale, int fontSize) {
	long nsteps = 100;
	wchar_t *name;

	autoNUMvector<double> x (0L, nsteps);
	autoNUMvector<double> y (0L, nsteps);

	// Get principal axes and orientation for the ellipse by performing the
	// eigen decomposition of a symmetric 2-by-2 matrix.
	// Principal axes are a and b with eigenvector/orientation (cs, sn).

	double a, b, cs, sn;
	NUMeigencmp22 (my data[1][1], my data[1][2], my data[2][2], &a, &b, &cs, &sn);

	// 1. Take sqrt to get units of 'std_dev'

	a = scale * sqrt (a) / 2;
	b = scale * sqrt (b) / 2;
	x[nsteps] = x[0] = my centroid[1] + cs * a;
	y[nsteps] = y[0] = my centroid[2] + sn * a;
	double angle = 0;
	double angle_inc = NUM2pi / nsteps;
	for (long i = 1; i < nsteps; i++, angle += angle_inc) {
		double xc = a * cos (angle);
		double yc = b * sin (angle);
		double xt = xc * cs - yc * sn;
		y[i] = my centroid[2] + xc * sn + yc * cs;
		x[i] = my centroid[1] + xt;
	}
	Graphics_polyline (g, nsteps + 1, x.peek(), y.peek());
	if (fontSize > 0 && (name = Thing_getName (me))) {
		int oldFontSize = Graphics_inqFontSize (g);
		Graphics_setFontSize (g, fontSize);
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, my centroid[1], my centroid[2], name);
		Graphics_setFontSize (g, oldFontSize);
	}
}

SSCP SSCP_toTwoDimensions (I, double *v1, double *v2) {
	iam (SSCP);
	try {
		double *vec[3];
		autoSSCP thee = SSCP_create (2);

		// Projection P of S on v1 and v2 (given matrix V' with 2 rows) is P = V'SV
		// P[i][j] = sum(k) sum(m) V'[i][k]*S[k][m]*V[m][j] = V'[i][k]*S[k][m]*V'[j][m]

		// For the new centroids cnew[i] = sum(m) V'[i][m]*c[m]

		vec[1] = v1; vec[2] = v2;
		if (my numberOfRows == 1) { // 1xn diagonal matrix
			for (long k = 1; k <= my numberOfColumns; k++) {
				thy data[1][1] += v1[k] * my data[1][k] * v1[k];
			}
			for (long k = 1; k <= my numberOfColumns; k++) {
				thy data[1][2] += v1[k] * my data[1][k] * v2[k];
			}
			for (long k = 1; k <= my numberOfColumns; k++) {
				thy data[2][2] += v2[k] * my data[1][k] * v2[k];
			}
			thy data[2][1] = thy data[1][2];
		} else {
			for (long i = 1; i <= 2; i++) {
				for (long j = i; j <= 2; j++) {
					double sum = 0;
					for (long k = 1; k <= my numberOfRows; k++) {
						for (long m = 1; m <= my numberOfRows; m++) {
							sum += vec[i][k] * my data[k][m] * vec[j][m];
						}
					}
					thy data[j][i] = thy data[i][j] = sum;
				}
			}
		}

		// centroids

		for (long m = 1; m <= my numberOfColumns; m++) {
			thy centroid[1] += v1[m] * my centroid[m];
		}
		for (long m = 1; m <= my numberOfColumns; m++) {
			thy centroid[2] += v2[m] * my centroid[m];
		}

		thy numberOfObservations = SSCP_getNumberOfObservations (me);

		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": cannot extract two dimensions.");
	}
}

void SSCP_init (I, long dimension, long storage) {
	iam (SSCP);
	TableOfReal_init (me, storage, dimension);
	my centroid = NUMvector<double> (1, dimension);
}

SSCP SSCP_create (long dimension) {
	try {
		autoSSCP me = Thing_new (SSCP);
		SSCP_init (me.peek(), dimension, dimension);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("SSCP not created.");
	}
}

double SSCP_getConcentrationEllipseArea (I, double scale, int confidence, long d1, long d2) {
	iam (SSCP);
	long p = my numberOfRows;

	if (d1 < 1 || d1 > p || d2 < 1 || d2 > p || d1 == d2) {
		Melder_throw ("Incorrect axes.");
	}
	autoSSCP thee = _SSCP_extractTwoDimensions (me, d1, d2);
	scale = ellipseScalefactor (thee.peek(), scale, confidence);
	if (scale < 0) {
		Melder_throw ("Invalid scale factor.");
	}

	double a, b, cs, sn;
	NUMeigencmp22 (thy data[1][1], thy data[1][2], thy data[2][2], &a, &b, &cs, &sn);

	// 1. Take sqrt to get units of 'std_dev'

	a = scale * sqrt (a) / 2; b = scale * sqrt (b) / 2;
	return NUMpi * a * b;
}

double SSCP_getFractionVariation (I, long from, long to) {
	iam (SSCP);
	long n = my numberOfRows;

	if (from < 1 || from > to || to > n) {
		return NUMundefined;
	}

	double sum = 0, trace = 0;
	for (long i = 1; i <= n; i++) {
		trace += my numberOfRows == 1 ? my data[1][i] : my data[i][i];
		if (i >= from && i <= to) {
			sum += my numberOfRows == 1 ? my data[1][i] : my data[i][i];
		}
	}
	return trace > 0 ? sum / trace : NUMundefined;
}

void SSCP_drawConcentrationEllipse (SSCP me, Graphics g, double scale,
                                    int confidence, long d1, long d2, double xmin, double xmax,
                                    double ymin, double ymax, int garnish) {
	long p = my numberOfColumns;

	if (d1 < 1 || d1 > p || d2 < 1 || d2 > p || d1 == d2) {
		Melder_throw ("Incorrect axes.");
	}

	autoSSCP thee = _SSCP_extractTwoDimensions (me, d1, d2);

	double xmn, xmx, ymn, ymx;
	getEllipseBoundingBoxCoordinates (thee.peek(), scale, confidence, &xmn, &xmx, &ymn, &ymx);

	if (xmax == xmin) {
		xmin = xmn; xmax = xmx;
	}

	if (ymax == ymin) {
		ymin = ymn; ymax = ymx;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);

	scale = ellipseScalefactor (thee.peek(), scale, confidence);
	if (scale < 0) {
		Melder_throw ("Invalid scale factor.");
	}
	_SSCP_drawTwoDimensionalEllipse (thee.peek(), g, scale, 0);

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_marksBottom (g, 2, 1, 1, 0);
	}
}

void SSCP_setNumberOfObservations (I, double numberOfObservations) {
	iam (SSCP);
	my numberOfObservations = numberOfObservations;
}

double SSCP_getNumberOfObservations (I) {
	iam (SSCP);
	return my numberOfObservations;
}

double SSCP_getDegreesOfFreedom (I) {
	iam (SSCP);
	return my numberOfObservations - 1;
}

double SSCP_getTotalVariance (I) {
	iam (SSCP);
	double trace = 0;
	for (long i = 1; i <= my numberOfColumns; i++) {
		trace += my numberOfRows == 1 ? my data[1][i] : my data[i][i];
	}
	return trace;
}

double SSCP_getCumulativeContributionOfComponents (I, long from, long to) {
	iam (SSCP);

	double sum = NUMundefined;
	if (to == 0) {
		to = my numberOfRows;
	}
	if (from > 0 && to <= my numberOfRows && from <= to) {
		sum = SSCP_getTotalVariance (me);
		double partial = 0;
		for (long i = from; i <= to; i++) {
			partial += my numberOfRows == 1 ? my data[1][i] : my data[i][i];
		}
		if (sum > 0) {
			sum = partial / sum;
		}
	}
	return sum;
}

/* For nxn matrix only ! */
void Covariance_and_PCA_generateOneVector (Covariance me, PCA thee, double *vec, double *buf) {
	// Generate the multi-normal vector elements N(0,sigma)

	for (long j = 1; j <= my numberOfColumns; j++) {
		buf[j] = NUMrandomGauss (0, sqrt (thy eigenvalues[j]));
	}

	// Rotate back

	for (long j = 1; j <= my numberOfColumns; j++) {
		vec[j] = 0;
		for (long k = 1; k <= my numberOfColumns; k++) {
			vec[j] += buf[k] * thy eigenvectors[k][j];
		}
	}

	// Restore the centroid

	for (long j = 1; j <= my numberOfColumns; j++) {
		vec[j] += my centroid[j];
	}
}

TableOfReal Covariance_to_TableOfReal_randomSampling (Covariance me, long numberOfData) {
	try {
		if (numberOfData <= 0) {
			numberOfData = my numberOfObservations;
		}
		autoPCA pca = SSCP_to_PCA (me);
		autoTableOfReal thee = TableOfReal_create (numberOfData, my numberOfColumns);
		autoNUMvector<double> buf (1, my numberOfColumns);

		for (long i = 1; i <= numberOfData; i++) {
			Covariance_and_PCA_generateOneVector (me, pca.peek(), thy data[i], buf.peek());
		}

		NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, my numberOfColumns);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not random sampled.");
	}
}

SSCP TableOfReal_to_SSCP (I, long rowb, long rowe, long colb, long cole) {
	iam (TableOfReal);
	try {
		TableOfReal_areAllCellsDefined (me, rowb, rowe, colb, cole);

		if (rowb == 0 && rowe == 0) {
			rowb = 1; rowe = my numberOfRows;
		} else if (rowe < rowb || rowb < 1 || rowe > my numberOfRows) {
			Melder_throw ("Invalid row number.");
		}

		if (colb == 0 && cole == 0) {
			colb = 1; cole = my numberOfColumns;
		} else if (cole < colb || colb < 1 || cole > my numberOfColumns) {
			Melder_throw ("Invalid column number.");
		}

		long m = rowe - rowb + 1; /* # rows */
		long n = cole - colb + 1; /* # columns */

		if (m < n) Melder_warning (L"The SSCP will not have \n"
			"full dimensionality. This may be a problem in following analysis steps. \n"
			"(The number of data points was less than the number of variables.)");

		autoSSCP thee = SSCP_create (n);
		autoNUMmatrix<double> v (1, m, 1, n);

		long nvalidrows = 0;
		for (long i = 1; i <= m; i++) {
			nvalidrows++;
			for (long j = 1; j <= n; j++) {
				v[i][j] = my data[rowb + i - 1][colb + j - 1];
			}
		}

		NUMcentreColumns (v.peek(), 1, m, 1, n, thy centroid);

		SSCP_setNumberOfObservations (thee.peek(), m);

		// sum of squares and cross products = T'T

		for (long i = 1; i <= n; i++) {
			for (long j = i; j <= n; j++) {
				double t = 0;
				for (long k = 1; k <= m; k++) {
					t += v[k][i] * v[k][j];
				}
				thy data[i][j] = thy data[j][i] = t;
			}
		}

		NUMstrings_copyElements (TOVEC (my columnLabels[colb]), thy columnLabels, 1, n);
		NUMstrings_copyElements (thy columnLabels, thy rowLabels, 1, n);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": SSCP not created.");
	}
}

TableOfReal SSCP_and_TableOfReal_extractDistanceQuantileRange (SSCP me, thou,
        double qlow, double qhigh) {
	try {
		thouart (TableOfReal);
		autoCovariance cov = SSCP_to_Covariance (me, 1);
		autoTableOfReal him = Covariance_and_TableOfReal_extractDistanceQuantileRange (cov.peek(), thee, qlow, qhigh);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no distance quantile ranges created.");
	}
}

TableOfReal Covariance_and_TableOfReal_mahalanobis (Covariance me, thou, bool useTableCentroid) {
	try {
		thouart (TableOfReal);

		autoTableOfReal him = TableOfReal_create (thy numberOfRows, 1);
		autoNUMvector<double> centroid (NUMvector_copy (my centroid, 1, thy numberOfColumns), 1);
		autoNUMmatrix<double> covari (NUMmatrix_copy (my data, 1, my numberOfRows, 1, my numberOfRows), 1, 1);

		// Mahalanobis distance calculation. S = L.L' -> S**-1 = L**-1' . L**-1
		// (x-m)'S**-1 (x-m) = (x-m)'L**-1' . L**-1. (x-m) =
		//	(L**-1.(x-m))' . (L**-1.(x-m))

		// Get inverse of covari in lower triangular part.
		double lndet;
		NUMlowerCholeskyInverse (covari.peek(), my numberOfRows, &lndet);

		if (useTableCentroid) {
			for (long icol = 1; icol <= thy numberOfColumns; icol++) {
				double mean = 0;
				for (long irow = 1; irow <= thy numberOfRows; irow++) {
					mean += thy data[irow][icol];
				}
				centroid[icol] = mean / thy numberOfRows;
			}
		}

		for (long k = 1; k <= thy numberOfRows; k++) {
			his data[k][1] = sqrt (NUMmahalanobisDistance_chi (covari.peek(), thy data[k], centroid.peek(), my numberOfRows, my numberOfRows));
			if (thy rowLabels[k] != 0) {
				TableOfReal_setRowLabel (him.peek(), k, thy rowLabels[k]);
			}
		}
		TableOfReal_setColumnLabel (him.peek(), 1, L"d");
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, "no Mahalanobis distances created.");
	}
}


TableOfReal Covariance_and_TableOfReal_extractDistanceQuantileRange (Covariance me, thou, double qlow, double qhigh) {
	try {
		thouart (TableOfReal);
		autoTableOfReal him = Covariance_and_TableOfReal_mahalanobis (me, thee, false);

		double low = TableOfReal_getColumnQuantile (him.peek(), 1, qlow);
		double high = TableOfReal_getColumnQuantile (him.peek(), 1, qhigh);

		// Count the number filtered.
		// nsel = (qhigh - qlow) * nrows is sometimes one off

		long nsel = 0;
		for (long i = 1; i <= thy numberOfRows; i++) {
			if (low <= his data[i][1] && his data[i][1] < high) {
				nsel++;
			}
		}

		if (nsel < 1) {
			Melder_throw ("Not enough data in quantile interval.");
		}

		autoTableOfReal r = TableOfReal_create (nsel, thy numberOfColumns);
		NUMstrings_copyElements (thy columnLabels, r -> columnLabels, 1, thy numberOfColumns);
		//((r -> columnLabels = NUMstrings_copy (thy columnLabels, 1, thy numberOfColumns)) == NULL)) goto end;

		long k = 0;
		for (long i = 1; i <= thy numberOfRows; i++) {
			if (low <= his data[i][1] && his data[i][1] < high) {
				k++;
				TableOfReal_copyOneRowWithLabel (thee, r.peek(), i, k);
			}
		}
		return r.transfer();
	} catch (MelderError) {
		Melder_throw ("TableOfReal with distancequantile range not created.");
	}
}


Covariance TableOfReal_to_Covariance (I) {
	iam (TableOfReal);
	try {
		autoSSCP sscp = TableOfReal_to_SSCP (me, 0, 0, 0, 0);
		autoCovariance thee = SSCP_to_Covariance (sscp.peek(), 1);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": covariances not created.");
	}
}

Correlation TableOfReal_to_Correlation (I) {
	iam (TableOfReal);
	try {
		autoSSCP sscp = TableOfReal_to_SSCP (me, 0, 0, 0, 0);
		autoCorrelation thee = SSCP_to_Correlation (sscp.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": correlations not created.");
	}
}

Correlation TableOfReal_to_Correlation_rank (I) {
	iam (TableOfReal);
	try {
		autoTableOfReal t = TableOfReal_rankColumns (me);
		autoCorrelation thee = TableOfReal_to_Correlation (t.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": rank correlations not created.");
	}
}

SSCPs TableOfReal_to_SSCPs_byLabel (I) {
	iam (TableOfReal);
	try {
		autoSSCPs thee = SSCPs_create ();
		autoTableOfReal mew = TableOfReal_sortOnlyByRowLabels (me);

		const wchar_t *label = mew -> rowLabels[1];
		Melder_warningOff ();
		long numberOfCases = my numberOfRows, ncols = my numberOfColumns;
		long lastrow = 0, ngroups = 0, nsingular = 0, index = 1;
		for (long i = 2; i <= numberOfCases; i++) {
			long nrows = 0;
			const wchar_t *li = mew -> rowLabels[i];
			if (Melder_wcscmp (li, label) != 0) {
				// current label different from previous one(s)
				nrows = i - index; lastrow = i - 1;
			} else if (i == numberOfCases) {
				// current (last) label is same as previous
				nrows = i - index + 1; lastrow = i;
			} else {
				// next one
				continue;
			}
			// We found a new group
			ngroups++;
			if (nrows > 1) { // We need at least two rows for an SSCP
				if (nrows < ncols) {
					nsingular++;
				}
				autoSSCP t = TableOfReal_to_SSCP (mew.peek(), index, lastrow, 0, 0);
				if (! (label = mew -> rowLabels[index])) {
					label = L"?";
				}
				Thing_setName (t.peek(), label);
				Collection_addItem (thee.peek(), t.transfer());
			}
			label = li; index = i;
		}
		if (lastrow != numberOfCases) {
			ngroups++;
		}
		Melder_warningOn ();
		if (nsingular > 0 || thy size != ngroups) {
			long notIncluded = ngroups - thy size;
			Melder_warning (Melder_integer (ngroups), L" different groups detected: ", Melder_integer (nsingular + notIncluded),
			                L" group(s) with less rows than columns (of which ", Melder_integer (notIncluded), L" with only one row).");
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": SSCP not created from labels.");
	}
}

PCA SSCP_to_PCA (I) {
	iam (SSCP);
	try {
		double **data = my data;
		autoNUMmatrix<double> adata;
		autoPCA thee = PCA_create (my numberOfColumns, my numberOfColumns);

		if (my numberOfRows == 1) { // 1xn matrix -> nxn
			// ugly hack
			adata.reset (1, my numberOfColumns, 1, my numberOfColumns);
			for (long i = 1; i <= my numberOfColumns; i++) {
				data[i][i] = my data[1][i];
			}
			data = adata.peek();
		}
		NUMstrings_copyElements (my columnLabels, thy labels, 1, my numberOfColumns);
		Eigen_initFromSymmetricMatrix (thee.peek(), data, my numberOfColumns);
		NUMvector_copyElements (my centroid, thy centroid, 1, my numberOfColumns);
		PCA_setNumberOfObservations (thee.peek(), my numberOfObservations);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": PCA not created.");
	}
}

void SSCP_setValue (I, long row, long col, double value) {
	iam (SSCP);
	if (col < 0 || col > my numberOfColumns) {
		Melder_throw ("Illegal column number.");
	}
	if (row < 0 || row > my numberOfColumns) {
		Melder_throw ("Illegal row number.");    // ! yes numberOfColumns
	}
	if (row == col && value <= 0) {
		Melder_throw ("Diagonal element must always be a positive number.");
	}
	if (my numberOfRows == 1) { // diagonal
		if (row != col) {
			Melder_throw ("Row and column number must be equal for a diagonal matrix.");
		}
		my data[1][row] = value;
	} else {
		if (row != col && (fabs (value) > my data[row][row] || fabs (value) > my data[row][row])) Melder_throw
			("The off-diagonal cannot be larger than the diagonal values. Input diagonal elements first, or change this value.");
		my data[row][col] = my data[col][row] = value;
	}
}

void SSCP_setCentroid (I, long component, double value) {
	iam (SSCP);
	if (component < 1 || component > my numberOfColumns) {
		Melder_throw ("Illegal component number.");
	}
	my centroid[component] = value;
}

CCA SSCP_to_CCA (I, long ny) {
	iam (SSCP);
	try {
		char upper = 'L', diag = 'N';
		long info;

		if (ny < 1 || ny >= my numberOfRows) {
			Melder_throw ("ny < 1 || ny >= my numberOfRows");
		}
		if (my numberOfRows == 1) {
			Melder_throw ("Matrix is diagonal.");
		}

		long m = my numberOfRows, nx = m - ny, xy_interchanged = nx < ny, yof = 0, xof = ny;
		if (xy_interchanged) {
			yof = ny; xof = 0;
			nx = ny; ny = m - nx;
		}

		autoCCA thee = Thing_new (CCA);
		autoNUMmatrix<double> sxx (1, nx, 1, nx);
		autoNUMmatrix<double> syy (1, ny, 1, ny);
		autoNUMmatrix<double> syx (1, ny, 1, nx);

		// Copy Syy and Sxx into upper part of syy and sxx matrices.

		for (long i = 1; i <= ny; i++) {
			for (long j = i; j <= ny; j++) {
				syy[i][j] = my data[yof + i][yof + j];
			}
		}
		for (long i = 1; i <= nx; i++) {
			for (long j = i; j <= nx; j++) {
				sxx[i][j] = my data[xof + i][xof + j];
			}
		}

		for (long i = 1; i <= nx; i++) {
			for (long j = 1; j <= ny; j++) {
				syx[i][j] = my data[yof + i][xof + j];
			}
		}

		// Cholesky decomposition: Syy = Uy'*Uy and Sxx = Ux'*Ux.
		// (Pretend as if colum-major storage)

		(void) NUMlapack_dpotf2 (&upper, &ny, &syy[1][1], &ny, &info);
		if (info != 0) Melder_throw ("The leading minor of order ", info, " is not positive definite, and the "
			                             "factorization of Syy could not be completed.");

		(void) NUMlapack_dpotf2 (&upper, &nx, &sxx[1][1], &nx, &info);
		if (info != 0) Melder_throw ("The leading minor of order ", info, " is not positive definite, and the "
			                             "factorization of Sxx could not be completed.");

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
			They are now in the form (A'A - lambda B'B) x = 0 and both can be solved with the GSVD.
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

		autoNUMmatrix<double> a (1, nx, 1, ny);

		// Uxi = inverse(Ux)

		(void) NUMlapack_dtrti2 (&upper, &diag, &nx, &sxx[1][1], &nx, &info);
		if (info != 0) {
			Melder_throw ("Error in inverse for Sxx.");
		}

		// Prepare Uxi' * Syx' = (Syx * Uxi)'

		for (long i = 1; i <= ny; i++) {
			for (long j = 1; j <= nx; j++) {
				double t = 0;
				for (long k = 1; k <= j; k++) {
					t += syx[i][k] * sxx[k][j];
				}
				a[j][i] = t;
			}
		}

		autoGSVD gsvd = GSVD_create_d (a.peek(), nx, ny, syy.peek(), ny);
		autoNUMmatrix<double> ri (NUMmatrix_copy (gsvd -> r, 1, gsvd -> numberOfColumns, 1, gsvd -> numberOfColumns), 1, 1);
		thy y = Eigen_create (gsvd -> numberOfColumns, gsvd -> numberOfColumns);
		thy x = Eigen_create (thy y -> numberOfEigenvalues, nx);

		// Get X=Q*R**-1

		(void) NUMlapack_dtrti2 (&upper, &diag, &gsvd -> numberOfColumns, &ri[1][1], &gsvd -> numberOfColumns, &info);
		if (info != 0) {
			Melder_throw ("Error in inverse for R.");
		}

		for (long i = 1; i <= gsvd -> numberOfColumns; i++) {
			double t = gsvd -> d1[i] / gsvd -> d2[i];
			thy y -> eigenvalues[i] = t * t;
			for (long j = 1; j <= gsvd -> numberOfColumns; j++) {
				t = 0;
				for (long k = 1; k <= j; k++) {
					t += gsvd -> q[i][k] * ri[k][j];
				}
				thy y ->eigenvectors[j][i] = t;
			}
		}

		NUMnormalizeRows (thy y -> eigenvectors, thy y -> numberOfEigenvalues, thy y -> numberOfEigenvalues, 1);

		thy numberOfCoefficients = thy y -> numberOfEigenvalues;
		thy numberOfObservations = my numberOfObservations;

		// x = Sxx**-1 * Syx' * y

		for (long i = 1; i <= thy numberOfCoefficients; i++) {
			double *evecy = thy y -> eigenvectors[i];
			double *evecx = thy x -> eigenvectors[i];
			for (long j = 1; j <= nx; j++) {
				double t = 0;
				for (long k = j; k <= nx; k++) {
					for (long l = 1; l <= nx; l++) {
						for (long n = 1; n <= ny; n++) {
							t += sxx[j][k] * sxx[l][k] * syx[n][l] * evecy[n];
						}
					}
				}
				evecx[j] = t;
			}
		}

		NUMnormalizeRows (thy x -> eigenvectors, thy x -> numberOfEigenvalues, nx, 1);

		if (ny < nx) {
			Eigen t = thy x;
			thy x = thy y; thy y = t;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": CCA not created.");
	}
}

/************ SSCPs ***********************************************/

Thing_implement (SSCPs, Ordered, 0);

SSCPs SSCPs_create () {
	try {
		autoSSCPs me = Thing_new (SSCPs);
		Ordered_init (me.peek(), classSSCP, 10);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("SSCPs not created.");
	}
}

SSCP SSCPs_to_SSCP_pool (SSCPs me) {
	try {
		autoSSCP thee = Data_copy ( (SSCP) my item[1]);

		for (long k = 2; k <= my size; k++) {
			SSCP t = (SSCP) my item[k];
			long no = t -> numberOfObservations;
			if (t -> numberOfRows != thy numberOfRows) {
				Melder_throw ("Unequal dimensions (", k, ").");
			}

			thy numberOfObservations += no;

			// Sum the sscp's and weigh the centroid.

			for (long i = 1; i <= thy numberOfRows; i++) { // if 1xn
				for (long j = 1; j <= thy numberOfColumns; j++) {
					thy data[i][j] += t -> data[i][j];
				}
			}

			for (long j = 1; j <= thy numberOfRows; j++) {
				thy centroid[j] += no * t -> centroid[j];
			}
		}

		for (long i = 1; i <= thy numberOfRows; i++) {
			thy centroid[i] /= thy numberOfObservations;
		}

		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not pooled.");
	}
}

void SSCPs_getHomegeneityOfCovariances_box (SSCPs me, double *probability, double *chisq, long *ndf) {
	*probability = 0; *chisq = 0; *ndf = 0;

	autoSSCP pooled = SSCPs_to_SSCP_pool (me);
	long p = pooled -> numberOfColumns;
	double ln_determinant, inv = 0, sum = 0, g = my size;
	for (long i = 1; i <= g; i++) {
		SSCP t = (SSCP) my item[i];
		double ni = t -> numberOfObservations - 1;
		NUMdeterminant_cholesky (t -> data, p, &ln_determinant);

		// Box-test is for covariance matrices -> scale determinant.

		ln_determinant -= p * log (ni);
		sum += ni; inv += 1 / ni;
		*chisq -= ni * ln_determinant;
	}

	NUMdeterminant_cholesky (pooled -> data, p, &ln_determinant);
	ln_determinant -= p * log (pooled -> numberOfObservations - g);
	*chisq += sum * ln_determinant;

	*chisq *= 1.0 - (inv - 1 / sum) * (2 * p * p + 3 * p - 1) / (6 * (p + 1) * (g - 1));
	*ndf = (g - 1) * p * (p + 1) / 2;
	*probability =  NUMchiSquareQ (*chisq, *ndf);
}


SSCPs SSCPs_toTwoDimensions (SSCPs me, double *v1, double *v2) {
	try {
		autoSSCPs thee = SSCPs_create ();
		for (long i = 1; i <= my size; i++) {
			autoSSCP t = (SSCP) SSCP_toTwoDimensions (my item[i], v1, v2);
			Thing_setName (t.peek(), Thing_getName ( (Thing) my item[i]));
			Collection_addItem (thee.peek(), t.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not reduced to two dimensions.");
	}
}


void SSCPs_drawConcentrationEllipses (SSCPs me, Graphics g, double scale, int confidence, const wchar_t *label,
                                      long d1, long d2, double xmin, double xmax, double ymin, double ymax, int fontSize, int garnish) {
	SSCP t = (SSCP) my item[1];
	long p = t -> numberOfColumns;

	if (d1 < 1 || d1 > p || d2 < 1 || d2 > p || d1 == d2) {
		Melder_throw ("Incorrect axes.");
	}

	autoSSCPs thee = SSCPs_extractTwoDimensions (me, d1, d2);
	double xmn, xmx, ymn, ymx;
	SSCPs_getEllipsesBoundingBoxCoordinates (thee.peek(), scale, confidence, &xmn, &xmx, &ymn, &ymx);

	if (xmin == xmax) {
		xmin = xmn; xmax = xmx;
	}

	if (ymin == ymax) {
		ymin = ymn; ymax = ymx;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);


	for (long i = 1; i <= thy size; i++) {
		double lscale;
		t = (SSCP) thy item[i];
		lscale = ellipseScalefactor (t, scale, confidence);
		if (lscale < 0) {
			continue;
		}
		if (label == 0 || Melder_wcscmp (label, Thing_getName (t)) == 0) {
			_SSCP_drawTwoDimensionalEllipse (t, g, lscale, fontSize);
		}
	}

	Graphics_unsetInner (g);
	if (garnish) {
		wchar_t text[20];
		t = (SSCP) my item[1];
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		swprintf (text, 20, L"Dimension %ld", d2);
		Graphics_textLeft (g, 1, t -> columnLabels[d2] ? t -> columnLabels[d2] : text);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		swprintf (text, 20, L"Dimension %ld", d1);
		Graphics_textBottom (g, 1, t -> columnLabels[d1] ? t -> columnLabels[d1] : text);
	}
}

TableOfReal SSCP_to_TableOfReal (SSCP me) {
	try {
		autoTableOfReal thee = Thing_new (TableOfReal);
		my structTableOfReal :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not copied.");
	}
}

TableOfReal SSCP_extractCentroid (I) {
	iam (SSCP);
	try {
		long n = my numberOfColumns;

		autoTableOfReal thee = TableOfReal_create (1, n);
		NUMvector_copyElements (my centroid, thy data[1], 1, n);
		thy columnLabels = NUMstrings_copy (my columnLabels, 1, n);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": centroid not extracted.");
	}
}

Thing_implement (Covariance, SSCP, 0);

Thing_implement (Correlation, SSCP, 0);

Covariance Covariance_create (long dimension) {
	try {
		autoCovariance me = Thing_new (Covariance);
		SSCP_init (me.peek(), dimension, dimension);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Covariance not created.");
	}
}

Covariance Covariance_create_reduceStorage (long dimension, long storage) {
	try {
		autoCovariance me = Thing_new (Covariance);
		if (storage <= 0 || storage >= dimension) {
			storage = dimension;
		}
		SSCP_init (me.peek(), dimension, storage);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Reduced storage covariance not created.");
	}
}

Covariance Covariance_createSimple (wchar_t *covariances, wchar_t *centroid, long numberOfObservations) {
	try {
		long dimension, ncovars;
		autoNUMvector<double> centroids (NUMstring_to_numbers (centroid, &dimension), 1);
		autoNUMvector<double> covars (NUMstring_to_numbers (covariances, &ncovars), 1);
		long ncovars_wanted = dimension * (dimension + 1) / 2;
		if (ncovars != ncovars_wanted) Melder_throw ("The number of covariance matrix elements and the number of "
			        "centroid elements are not in concordance. There should be d(d+1)/2 covariance values and d centroid values.");

		autoCovariance me = Covariance_create (dimension);

		// Construct the full covariance matrix from the upper-diagonal elements

		long rowNumber = 1;
		for (long inum = 1; inum <= ncovars_wanted; inum++) {
			long nmissing = (rowNumber - 1) * rowNumber / 2;
			long inumc = inum + nmissing;
			rowNumber = (inumc - 1) / dimension + 1;
			long icol = ( (inumc - 1) % dimension) + 1;
			my data[rowNumber][icol] = my data[icol][rowNumber] = covars[inum];
			if (icol == dimension) {
				rowNumber++;
			}
		}

		// Check if a valid covariance, first check variances then covariances

		for (long irow = 1; irow <= dimension; irow++) {
			if (my data[irow][irow] <= 0) {
				Melder_throw ("The variances, i.e. the diagonal matrix elements, must all be positive numbers.");
			}
		}
		for (long irow = 1; irow <= dimension; irow++) {
			for (long icol = irow + 1; icol <= dimension; icol++) {
				if (fabs (my data[irow][icol] / sqrt (my data[irow][irow] * my data[icol][icol])) > 1) {
					long nmissing = (irow - 1) * irow / 2;
					long inum = (irow - 1) * dimension + icol - nmissing;
					Melder_throw (L"The covariance in cell [", irow, L",", icol, L"], i.e. input item ", inum, L" is too large.");
				}
			}
		}
		for (long inum = 1; inum <= dimension; inum++) {
			my centroid[inum] = centroids[inum];
		}
		my numberOfObservations = numberOfObservations;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Simple Covariance not created.");
	}
}

Correlation Correlation_create (long dimension) {
	try {
		autoCorrelation me = Thing_new (Correlation);
		SSCP_init (me.peek(), dimension, dimension);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Correlation not created.");
	}
}

Covariance SSCP_to_Covariance (SSCP me, long numberOfConstraints) {
	try {
		Melder_assert (numberOfConstraints >= 0);
		autoCovariance thee = Thing_new (Covariance);
		my structSSCP :: v_copy (thee.peek());

		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = i; j <= my numberOfColumns; j++) {
				thy data[j][i] = thy data[i][j] /= (my numberOfObservations - numberOfConstraints);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, "; Covariance not created.");
	}
}

SSCP Covariance_to_SSCP (Covariance me) {
	try {
		autoSSCP thee = Thing_new (SSCP);
		my structSSCP :: v_copy (thee.peek());
		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = i; j <= my numberOfColumns; j++) {
				thy data[j][i] = thy data[i][j] *= (my numberOfObservations - 1);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": SSCP not created.");
	}
}

Correlation SSCP_to_Correlation (I) {
	iam (SSCP);
	try {
		autoCorrelation thee = Thing_new (Correlation);
		my structSSCP :: v_copy (thee.peek());
		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = i; j <= my numberOfColumns; j++) {
				thy data[j][i] = thy data[i][j] /= sqrt (my data[i][i] * my data[j][j]);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Correlation not created.");
	}
}

double SSCP_getLnDeterminant (I) {
	try {
		iam (SSCP);
		double ln_d;
		NUMdeterminant_cholesky (my data, my numberOfRows, &ln_d);
		return ln_d;
	} catch (MelderError) {
		return NUMundefined;
	}
}

static Covariance Covariances_pool (Covariance me, Covariance thee) {
	try {
		if (my numberOfRows != thy numberOfRows || my numberOfColumns != thy numberOfColumns)  Melder_throw
			("Matrices must have equal dimensions.");
		autoSSCPs sscps = SSCPs_create ();
		autoSSCP sscp1 = Covariance_to_SSCP (me);
		Collection_addItem (sscps.peek(), sscp1.transfer());
		autoSSCP sscp2 = Covariance_to_SSCP (thee);
		Collection_addItem (sscps.peek(), sscp2.transfer());
		autoSSCP pool = SSCPs_to_SSCP_pool (sscps.peek());
		autoCovariance him = SSCP_to_Covariance (pool.peek(), 2);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, "not pooled.");
	}
}

static double **productOfSquareMatrices (double **s1, double **s2, long n) {
	autoNUMmatrix<double> r (1, n, 1, n);
	for (long i = 1; i <= n; i++) {
		for (long j = 1; j <= n; j++) {
			double sum = 0;
			for (long k = 1; k <= n; k++) {
				sum += s1[i][k] * s2[k][j];
			}
			r[i][j] = sum;
		}
	}
	return r.transfer();
}

static double traceOfSquaredMatrixProduct (double **s1, double **s2, long n) {
	// tr ((s1*s2)^2), s1, s2 are symmetric
	autoNUMmatrix<double> m (productOfSquareMatrices (s1, s2, n), 1, 1);
	double trace2 = NUMtrace2 (m.peek(), m.peek(), n);
	return trace2;
}

double Covariance_getProbabilityAtPosition_string (Covariance me, wchar_t *vector) {
	autoNUMvector<double> v (1, my numberOfColumns);
	long i = 0;
	for (wchar_t *token = Melder_firstToken (vector); token != NULL; token = Melder_nextToken ()) {
		v[++i] = Melder_atof (token);
		if (i == my numberOfColumns) {
			break;
		}
	}
	double p = Covariance_getProbabilityAtPosition (me, v.peek());
	return p;
}

double Covariance_getProbabilityAtPosition (Covariance me, double *x) {
	if (my lowerCholesky == 0) {
		SSCP_expandLowerCholesky (me);
	}
	double ln2pid = my numberOfColumns * log (NUM2pi);
	double dsq = NUMmahalanobisDistance_chi (my lowerCholesky, x, my centroid, my numberOfRows, my numberOfColumns);
	double lnN = - 0.5 * (ln2pid + my lnd + dsq);
	double p =  exp (lnN);
	return p;
}

double Covariance_getMarginalProbabilityAtPosition (Covariance me, double *vector, double x) {
	double mu, stdev;
	Covariance_getMarginalDensityParameters (me, vector, &mu, &stdev);
	double dx = (x - mu) / stdev;
	double p = (NUM1_sqrt2pi / stdev) * exp (- 0.5 * dx * dx);
	return p;
}

/* Precondition ||v|| = 1 */
void Covariance_getMarginalDensityParameters (Covariance me, double *v, double *mu, double *stdev) {
	*stdev = *mu = 0;
	if (my numberOfRows == 1) { // 1xn diagonal matrix
		for (long m = 1; m <= my numberOfColumns; m++) {
			*stdev += v[m] * my data[1][m] * v[m];
		}
	} else {
		for (long k = 1; k <= my numberOfRows; k++) {
			for (long m = 1; m <= my numberOfColumns; m++) {
				*stdev += v[k] * my data[k][m] * v[m];
			}
		}
	}
	*stdev = sqrt (*stdev);
	for (long m = 1; m <= my numberOfColumns; m++) {
		*mu += v[m] * my centroid[m];
	}
}

double Covariances_getMultivariateCentroidDifference (Covariance me, Covariance thee, int equalCovariances, double *prob, double *fisher, double *df1, double *df2) {
	long p = my numberOfRows, N = my numberOfObservations + thy numberOfObservations;
	long N1 = my numberOfObservations, n1 = N1 - 1;
	long N2 = thy numberOfObservations, n2 = N2 - 1;

	double dif = *prob = *fisher = NUMundefined;
	*df1 = p;
	*df2 = N - p - 1;
	if (*df2 < 1) {
		Melder_throw ("Not enough observations (", N, ") for this test.");
	}
	if (N1 < p || N2 < p) {
		Melder_throw ("At least one of the covariances has less observations than variables.");
	}

	dif = 0;
	for (long i = 1; i <= p; i++) {
		double dist = my centroid[i] - thy centroid[i];
		dif += dist * dist;
	}
	dif = sqrt (dif);

	if (equalCovariances) {
		// Morrison, page 141
		autoCovariance pool = Covariances_pool (me, thee);
		autoNUMmatrix<double> s (NUMmatrix_copy (my data, 1, p, 1, p), 1, 1);
		double lndet;
		NUMlowerCholeskyInverse (s.peek(), p, &lndet);

		double mahalanobis = NUMmahalanobisDistance_chi (s.peek(), my centroid, thy centroid, p, p);
		double hotelling_tsq = mahalanobis * N1 * N2 / N;
		*fisher = hotelling_tsq * *df2 / ( (N - 2) * *df1);
	} else {
		/* Krishnamoorthy-Yu (2004): Modified Nel and Van der Merwe test

			Hotelling t^2 = (x1-x2)'*S^-1*(x1 -x2) follows nu*p*Fisher(p,nu-p+1)/(nu-p+1)

			Approximate number of degrees of freedom  (their formula 7, page 164)
			nu = (p+p^2)/((1/n1)(tr (S1*S^-1)^2 + (tr(S1*S^-1))^2)) +(1/n2)(tr (S2*S^-1)^2 + (tr(S2*S^-1))^2)))
			the matrices S1 and S2 are the covariance matrices 'my data' and 'thy data' divided by N1 and N2 respectively.
			S is the pooled covar divided by N.
		*/
		autoNUMmatrix<double> s1 (1, p, 1, p);
		autoNUMmatrix<double> s2 (1, p, 1, p);
		autoNUMmatrix<double> s (1, p, 1, p);

		for (long i = 1; i <= p; i++) {
			for (long j = 1; j <= p; j++) {
				s1[i][j] = my data[i][j] / my numberOfObservations;
				s2[i][j] = thy data[i][j] / thy numberOfObservations;
				s[i][j] = s1[i][j] + s2[i][j];
			}
		}
		double lndet;
		NUMlowerCholeskyInverse (s.peek(), p, &lndet);
		// Krishan... formula 2, page 162
		double hotelling_tsq = NUMmahalanobisDistance_chi (s.peek(), my centroid, thy centroid, p, p);

		autoNUMmatrix<double> si (NUMinverseFromLowerCholesky (s.peek(), p), 1, 1);
		double tr_s1sisqr = traceOfSquaredMatrixProduct (s1.peek(), si.peek(), p);
		double tr_s1si = NUMtrace2 (s1.peek(), si.peek(), p);
		double tr_s2sisqr = traceOfSquaredMatrixProduct (s2.peek(), si.peek(), p);
		double tr_s2si = NUMtrace2 (s2.peek(), si.peek(), p);

		double nu = (p + p * p) / ( (tr_s1sisqr + tr_s1si * tr_s1si) / n1 + (tr_s2sisqr + tr_s2si * tr_s2si) / n2);
		*df2 = nu - p + 1;
		*fisher =  hotelling_tsq * (nu - p + 1) / (nu * p);
	}

	*prob = NUMfisherQ (*fisher, *df1, *df2);
	return dif;
}

/* Schott 2001 */
void Covariances_equality (Collection me, int method, double *prob, double *chisq, double *df) {
	try {
		long nc = my size;
		double  nsi = 0;

		*prob = *chisq = *df = NUMundefined;

		if (nc < 2) {
			Melder_throw ("We need at least two matrices");
		}

		long p = 1, ns = 0;
		for (long i = 1; i <= nc; i++) {
			Covariance ci = (Covariance) my item[i];
			double ni = ci -> numberOfObservations - 1;
			if (i == 1) {
				p = ci -> numberOfRows;
			}
			if (ci -> numberOfRows != p) {
				Melder_throw ("The dimensions of matrix ", i, " differ from the previous one(s).");
			}
			if (ni < p) {
				Melder_throw ("The number of observations in matrix ", i, " is less than the number of variables. ");
			}
			ns += ni; nsi += 1.0 / ni;
		}

		autoNUMmatrix<double> s (1, p, 1, p);

		for (long i = 1; i <= nc; i++) { // pool
			Covariance ci = (Covariance) my item[i];
			double sf = (ci -> numberOfObservations - 1.0) / ns;
			for (long j = 1; j <= p; j++) {
				for (long k = 1; k <= p; k++) {
					s[j][k] += sf * ci -> data[j][k];
				}
			}
		}

		if (method == 1) { // bartlett
			double lnd;
			try {
				NUMdeterminant_cholesky (s.peek(), p, &lnd);
			} catch (MelderError) {
				Melder_throw ("Pooled covariance matrix is singular.");
			}

			double m = ns * lnd;
			for (long i = 1; i <= nc; i++) {
				Covariance ci = (Covariance) my item[i];
				try {
					NUMdeterminant_cholesky (ci -> data, p, &lnd);
				} catch (MelderError) {
					Melder_throw ("Covariance matrix ", i, " is singular.");
				}
				m -= (ci -> numberOfObservations - 1) * lnd;
			}

			double c1 = 1.0 - (2.0 * p * p - 3 * p - 1) / (6.0 * (p + 1) * (nc - 1)) * (nsi - 1 / ns);

			*df = (nc - 1) * p * (p + 1) / 2;
			*chisq = m * c1;
		} else if (method == 2) { // Schott (2001) Wald 1
			// sum(i, ni/n *tr((si*s^-1)^2)- sum(i,sum(j, (ni/n)*(nj/n) *tr(si*s^-1*sj*s^-1))) =
			//	sum(i=1..k, (ni/n -(ni/n)^2) tr((si*s^-1)^2)
			//	- 2 * sum (i=1..k, sum(j=1..i-1, (ni/n)*(nj/n) *tr(si*s^-1*sj*s^-1)))

			double trace = 0;
			NUMlowerCholeskyInverse (s.peek(), p, NULL);
			autoNUMmatrix<double> si (NUMinverseFromLowerCholesky (s.peek(), p), 1, 1);
			for (long i = 1; i <= nc; i++) {
				Covariance ci = (Covariance) my item[i];
				double ni = ci -> numberOfObservations - 1;
				autoNUMmatrix<double> s1 (productOfSquareMatrices (ci -> data, si.peek(), p), 1, 1);
				double trace_ii = NUMtrace2 (s1.peek(), s1.peek(), p);
				trace += (ni / ns) * (1 - (ni / ns)) * trace_ii;
				for (long j = i + 1; j <= nc; j++) {
					Covariance cj = (Covariance) my item[j];
					double nj = cj -> numberOfObservations - 1;
					autoNUMmatrix<double> s2 (productOfSquareMatrices (cj -> data, si.peek(), p), 1, 1);
					double trace_ij = NUMtrace2 (s1.peek(), s2.peek(), p);
					trace -= 2 * (ni / ns) * (nj / ns) * trace_ij;
				}
			}
			*df = (nc - 1) * p * (p + 1) / 2;
			*chisq = (ns / 2) * trace;
		} else {
			return;
		}
		*prob = NUMchiSquareQ (*chisq, *df);
	} catch (MelderError) {
		Melder_throw ("Equality coud not be tested.");
	}
}

void Covariance_difference (Covariance me, Covariance thee, double *prob, double *chisq, long *ndf) {
	long p = my numberOfRows;
	long numberOfObservations = my numberOfObservations;
	double  ln_me, ln_thee;

	if (my numberOfRows != thy numberOfRows) {
		Melder_throw ("Matrices must have equal dimensions.");
	}
	if (my numberOfObservations != thy numberOfObservations) {
		numberOfObservations = (my numberOfObservations > thy numberOfObservations ?
		                        thy numberOfObservations : my numberOfObservations) - 1;
		Melder_warning (L"Covariance_difference: number of observations of matrices do not agree.\n"
		                " The minimum  size (", Melder_integer (numberOfObservations), L") of the two is used.");
	}
	if (numberOfObservations < 2) {
		Melder_throw ("Number of observations too small.");
	}

	autoNUMmatrix<double> linv (NUMmatrix_copy (thy data, 1, p, 1, p), 1, 1);
	NUMlowerCholeskyInverse (linv.peek(), p, & ln_thee);
	NUMdeterminant_cholesky (my data, p, &ln_me);

	/*
		We need trace (A B^-1). We have A and the inverse L^(-1) of the
		cholesky decomposition L^T L of B in the lower triangle + diagonal.
		Always: tr (A B) = tr (B A)
		tr (A B^-1) = tr (A (L L^T)^-1) = tr (A L^-1 (L^T)^-1)
		trace = sum(i=1..p, j=1..p, l=max(i,j)..p, A[i][j]Lm[l][j]Lm[l][i],
		where Lm = L^(-1)
	*/

	double trace = 0;
	for (long i = 1; i <= p; i++) {
		for (long j = 1; j <= p; j++) {
			long lp = MAX (j, i);
			for (long l = lp; l <= p; l++) {
				trace += my data[i][j] * linv[l][j] * linv[l][i];
			}
		}
	}

	double l = (numberOfObservations - 1) * fabs (ln_thee - ln_me + trace - p);
	*chisq = l * fabs (1 - (2 * p + 1 - 2 / (p + 1)) / (numberOfObservations - 1) / 6);
	*ndf = p * (p + 1) / 2;
	*prob = NUMchiSquareQ (*chisq, *ndf);
}

static void checkOneIndex (TableOfReal me, long index) {
	if (index < 1 || index > my numberOfColumns) {
		Melder_throw ("Index must be in interval [1, ", my numberOfColumns, "].");
	}
}

static void checkTwoIndices (TableOfReal me, long index1, long index2) {
	if (index1 < 1 || index1 > my numberOfColumns || index2 < 1 || index2 > my numberOfColumns) {
		Melder_throw ("Index must be in interval [1, ", my numberOfColumns, "].");
	}
	if (index1 == index2) {
		Melder_throw ("Indices must be different.");
	}
}

void Covariance_getSignificanceOfOneMean (Covariance me, long index, double mu,
        double *probability, double *t, double *ndf) {
	double var;
	*probability = *t = NUMundefined;
	*ndf = my numberOfObservations - 1;

	checkOneIndex (me, index);

	if ( (var = my data[index][index]) == 0) {
		return;
	}

	*t = (my centroid[index] - mu) / sqrt (var / my numberOfObservations);
	*probability = 2 * NUMstudentQ (fabs (*t), *ndf);
}

void Covariance_getSignificanceOfMeansDifference (Covariance me,
        long index1, long index2, double mu, int paired, int equalVariances,
        double *probability, double *t, double *ndf) {
	long n = my numberOfObservations;
	double df, var1, var2, var_pooled;

	*probability = *t = NUMundefined;
	*ndf = 2 * (n - 1);

	checkTwoIndices (me, index1, index2);

	var1 = my data[index1][index1];
	var2 = my data[index2][index2];

	var_pooled = var1 + var2;
	if (var_pooled == 0) {
		Melder_warning (L"The pooled variance turned out to be zero. Check your data. ");
		return;
	}
	if (paired) {
		var_pooled -= 2 * my data[index1][index2];
		*ndf /= 2;
	}

	if (var_pooled == 0) {
		Melder_warning (L"The pooled variance with the paired correction turned out to be zero. ");
		*probability = 0;
		return;
	}

	*t = (my centroid[index1] - my centroid[index2] - mu) / sqrt (var_pooled / n);

	/*
		Return two sided probabilty.
	*/

	if (equalVariances) {
		*probability = 2 * NUMstudentQ (fabs (*t), *ndf);
	} else {
		df = (1 + 2 * var1 * var2 / (var1 * var1 + var2 * var2)) * (n - 1);
		*probability = NUMincompleteBeta (df / 2, 0.5, df / (df + (*t) * (*t)));
		*ndf = df;
	}
}

void Covariance_getSignificanceOfOneVariance (Covariance me, long index,
        double sigmasq, double *probability, double *chisq, long *ndf) {
	double var;
	*probability = *chisq = NUMundefined;
	*ndf = my numberOfObservations - 1;

	checkOneIndex (me, index);

	if ( (var = my data[index][index]) == 0) {
		return;
	}

	*chisq = *ndf;
	if (sigmasq != 0) {
		*chisq = *ndf * var / sigmasq;
	}
	*probability = NUMchiSquareQ (*chisq, *ndf);
}

void Covariance_getSignificanceOfVariancesRatio (Covariance me,
        long index1, long index2, double ratio, double *probability,
        double *f, long *ndf) {
	long n = my numberOfObservations;
	double var1, var2, ratio2;

	*ndf = n - 1; *probability = *f = NUMundefined;
	checkTwoIndices (me, index1, index2);

	var1 = my data[index1][index1];
	var2 = my data[index2][index2];

	if (var1 == 0 || var2 == 0) {
		return;
	}

	*f = ratio2 = (var1 / var2) / ratio;
	if (var2 > var1) {
		ratio2 = (var2 / var1) * ratio;
	}

	*probability = 2 * NUMfisherQ (ratio2, *ndf, *ndf);
	if (*probability > 1) {
		*probability = 2 - *probability;
	}
}

TableOfReal Correlation_confidenceIntervals (Correlation me, double confidenceLevel, long numberOfTests, int method) {
	try {
		long m_bonferroni = my numberOfRows * (my numberOfRows - 1) / 2;

		if (confidenceLevel <= 0 || confidenceLevel > 1) {
			Melder_throw ("Confidence level must be in interval (0-1).");
		}

		if (my numberOfObservations < 5) {
			Melder_throw ("The number of observations must be greater than 4.");
		}

		if (numberOfTests < 0) {
			Melder_throw ("The \"number of tests\" cannot be less than zero.");
		} else if (numberOfTests == 0) {
			numberOfTests = m_bonferroni;
		}

		if (numberOfTests > m_bonferroni) {
			Melder_warning (L"The \"number of tests\" exceeds the number of elements in the Correlation object.");
		}

		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfRows);

		TableOfReal_copyLabels (me, thee.peek(), 1, 1);


		// Obtain large-sample conservative multiple tests and intervals by the
		// Bonferroni inequality and the Fisher z transformation.
		// Put upper value of confidence intervals in upper part and lower
		// values of confidence intervals in lower part of resulting table.


		double z = NUMinvGaussQ ( (1 - confidenceLevel) / (2 * numberOfTests));
		double zf = z / sqrt (my numberOfObservations - 3);
		double  two_n = 2 * my numberOfObservations;

		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = i + 1; j <= my numberOfRows; j++) {
				double rij = my data[i][j], rmin, rmax;
				if (method == 2) {
					// Fisher's approximation

					double zij = 0.5 * log ( (1 + rij) / (1 - rij));
					rmax = tanh (zij + zf);
					rmin = tanh (zij - zf);
				} else if (method == 1) {
					// Ruben's approximation

					double rs = rij / sqrt (1 - rij * rij);
					double a = two_n - 3 - z * z;
					double b = rs * sqrt ( (two_n - 3) * (two_n - 5));
					double c = (a - 2) * rs * rs - 2 * z * z;

					// Solve:  a y^2 - 2b y + c = 0
					// q = -0.5((-2b) + sgn(-2b) sqrt((-2b)^2 - 4ac))
					// y1 = q/a; y2 = c/q;

					double q, d = sqrt (b * b - a * c);

					if (b > 0) {
						d = - d;
					}
					q = b - d;
					rmin = q / a; rmin /= sqrt (1 + rmin * rmin);
					rmax = c / q; rmax /= sqrt (1 + rmax * rmax);
					if (rmin > rmax) {
						double t = rmin; rmin = rmax; rmax = t;
					}
				} else {
					rmax = rmin = 0;
				}
				thy data[i][j] = rmax;
				thy data[j][i] = rmin;
			}
			thy data[i][i] = 1;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": confidence intervals not created.");
	}
}

void SSCP_testDiagonality_bartlett (SSCP me, long numberOfContraints, double *chisq, double *probability) {
	*chisq = *probability = NUMundefined;
	autoCorrelation c = SSCP_to_Correlation (me);
	Correlation_testDiagonality_bartlett (c.peek(), numberOfContraints, chisq, probability);
}

/* Morrison, page 118 */
void Correlation_testDiagonality_bartlett (Correlation me, long numberOfContraints, double *chisq, double *probability) {
	*chisq = *probability = NUMundefined;

	if (numberOfContraints <= 0) {
		numberOfContraints = 1;
	}
	if (numberOfContraints > my numberOfObservations) {
		Melder_warning (L"Correlation_testDiagonality_bartlett: number of constraints cannot exceed the number of observations.");
		return;
	}
	long p = my numberOfRows;
	double ln_determinant;
	NUMdeterminant_cholesky (my data, p, &ln_determinant);

	*chisq = - ln_determinant * (my numberOfObservations - numberOfContraints - (2 * p + 5) / 6);
	*probability = NUMchiSquareQ (*chisq, p * (p - 1) / 2);
}

void SSCP_expand (I) {
	iam (SSCP);

	// A reduced matrix has my numberOfRows < my numberOfColumns.
	// After expansion:
	// my numberOfRows == my numberOfColumns
	// my storageNumberOfRows = my numberOfRows (before)
	// my data (after) = my expansion;
	// my expansion = my data (before)
	// No expansion for a standard matrix or if already expanded and data has not changed!

	if ( (my expansionNumberOfRows == 0 && my numberOfRows == my numberOfColumns) ||
	        (my expansionNumberOfRows > 0 && ! my dataChanged)) {
		return;
	}
	if (my expansion == 0) {
		my expansion = NUMmatrix<double> (1, my numberOfColumns, 1, my numberOfColumns);
	}
	for (long ir = 1; ir <= my numberOfColumns; ir++) {
		for (long ic = ir; ic <= my numberOfColumns; ic++) {
			long dij = abs (ir - ic);
			my expansion[ir][ic] = my expansion[ic][ir] = dij < my numberOfRows ? my data[dij + 1][ic] : 0;
		}
	}
	// Now make 'my data' point to 'my expansion'
	double **tmp = my data; my data = my expansion; my expansion = tmp;
	my expansionNumberOfRows = my numberOfRows;
	my numberOfRows = my numberOfColumns; // Now forget(me) is save
	my dataChanged = 0;
}

void SSCP_unExpand (I) {
	iam (SSCP);
	if (my expansionNumberOfRows == 0) {
		return;
	}
	NUMmatrix_free (my data, 1, 1);
	my data = my expansion;
	my expansion = 0;
	my numberOfRows = my expansionNumberOfRows;
	my expansionNumberOfRows = 0;
	my dataChanged = 0;
}

void SSCP_expandLowerCholesky (I) {
	iam (SSCP);
	if (my lowerCholesky == 0) {
		my lowerCholesky = NUMmatrix<double> (1, my numberOfRows, 1, my numberOfColumns);
	}
	if (my numberOfRows == 1) { // diagonal
		my lnd = 0;
		for (long j = 1; j <= my numberOfColumns; j++) {
			my lowerCholesky[1][j] = 1 / sqrt (my data[1][j]); // inverse is 1/stddev
			my lnd += log (my data[1][j]); // diagonal elmnt is variance
		}
	} else {
		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = i; j <= my numberOfColumns; j++) {
				my lowerCholesky[j][i] = my lowerCholesky[i][j] = my data[i][j];
			}
		}
		try {
			NUMlowerCholeskyInverse (my lowerCholesky, my numberOfColumns, & (my lnd));
		} catch (MelderError) {
			// singular matrix: arrange a diagonal only inverse.
			my lnd = 0;
			for (long i = 1; i <= my numberOfRows; i++) {
				for (long j = i; j <= my numberOfColumns; j++) {
					my lowerCholesky[i][j] =  my lowerCholesky[j][i] = i == j ? 1. / sqrt (my data[i][i]) : 0;
				}
				my lnd += log (my data[i][i]);
			}
			my lnd *= 2;
		}
	}
}

void SSCP_unExpandLowerCholesky (I) {
	iam (SSCP);
	NUMmatrix_free (my lowerCholesky, 1, 1);
	my lnd = 0;
}

void SSCP_expandPCA (I) {
	iam (SSCP);
	if (my pca != 0) {
		forget (my pca);
	}
	my pca = SSCP_to_PCA (me);
}

void SSCP_unExpandPCA (I) {
	iam (SSCP);
	forget (my pca);
}


#undef MAX
#undef MIN

/* End of file SSCP.c 2148*/
