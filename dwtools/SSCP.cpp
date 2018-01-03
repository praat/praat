/* SSCP.cpp
 *
 * Copyright (C) 1993-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20010614 Covariance_difference: corrected bug in calculation of
 	trace (A B^-1) that caused chisq values to be completely unreliable.
 djmw 20010628 TableOfReal_to_SSCP: skip error-return when nrows < 2,
	just issue warning.
 djmw 20010906 TableOfReal_to_SSCP.
 djmw 20020212 +getEllipse(s)BoundingBoxCoordinates.
 djmw 20020313 corrected SSCP_Eigen_project.
 djmw 20020327 Moved SSCP_Eigen_project to Eigen_SSCP.c.
 djmw 20020418 Removed some causes for compiler warnings.
 djmw 20020430 Changed explicit calculation of SSCP to svd in
 		TableOfReal_to_SSCP.
 djmw 20030703 Replaced NUMincompletebeta with gsl_sf_beta_inc.
 djmw 20030801 SSCPs_drawConcentrationEllipses extra label argument.
 djmw 20030825 Replaced gsl_sf_beta_inc with NUMincompletebeta.
 djmw 20031104 Added SSCP_to_CCA.
 djmw 20031117 Added SSCP_extractCentroid.
 djmw 20031127 Added Covariance_TableOfReal_extractDistanceQuantileRange.
 djmw 20040211 Better warnings in TableOfReal_to_SSCPs_byLabel for single cases.
 djmw 20040214 Fixed some compiler warnings.
 djmw 20040219 SSCP_getTraceFraction added.
 djmw 20040617 SSCP(s)_drawConcentrationEllipse(s) draw with reverse axes possible.
 	(not yet supported by commands in Picture window like 'One mark bottom...' because of reversed axes)!
 djmw 20060202 Removed a bug in TableOfReal_to_SSCP that could crash Praat (if nrows < ncols).
 djmw 20060503 Covariance_getSignificanceOfMeansDifference: set probability = 0 if
 	var_pooled = 0 and paired.
 djmw 20060811 Removed bug in SSCP_TableOfReal_to_MahalanobisDistances that caused column labels always to be copied.
 djmw 20061214 Corrected possible integer overflow in ellipseScalefactor.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20071016 To Melder_error<n>
 djmw 20071202 Melder_warning<n>
 djmw 20080122 float -> double
 djmw 20081119 TableOfReal_to_SSCP check if numbers are defined
 djmw 20090617 TableOfReal_to_SSCPs_byLabel better warnings for singular cases.
 djmw 20090629 +Covariances_getMultivariateCentroidDifference, Covariances_equality.
 djmw 20100106 +Covariance_TableOfReal_mahalanobis.
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
Thing_implement (SSCPList, TableOfRealList, 0);
Thing_implement (Covariance, SSCP, 0);
Thing_implement (CovarianceList, SSCPList, 0);
Thing_implement (Correlation, SSCP, 0);

void structSSCP :: v_info () {
	structTableOfReal :: v_info ();
	double zmin, zmax;
	NUMmatrix_extrema<double> (data, 1, numberOfRows, 1, numberOfColumns, &zmin, &zmax);
	MelderInfo_writeLine (U"Minimum value: ", zmin);
	MelderInfo_writeLine (U"Maximum value: ", zmax);
}

/*
	Calculate scale factor by which sqrt(eigenvalue) has to
	be multiplied to obtain the length of an ellipse axis.
*/
double SSCP_getEllipseScalefactor (SSCP me, double scale, bool confidence) {
	integer n = Melder_ifloor (SSCP_getNumberOfObservations (me));

	if (confidence) {
		integer p = my numberOfColumns;
		double f;

		if (n - p < 1) {
			return -1.0;
		}
		/* D.E. Johnson (1998), Applied Multivariate methods, page 410 */
		f = NUMinvFisherQ (1.0 - scale, p, n - p);
		scale = 2.0 * sqrt (f * p * (n - 1) / ( ((double) n) * (n - p)));
	} else {
		// very ugly, temporary hack
		scale *= 2.0 / (scale < 0.0 ? -1.0 : sqrt (n - 1));
	}
	return scale;
}

static void getEllipseBoundingBoxCoordinates (SSCP me, double scale, bool confidence, double *xmin, double *xmax, double *ymin, double *ymax) {
	double a, b, cs, sn, width, height;
	double lscale = SSCP_getEllipseScalefactor (me, scale, confidence);

	NUMeigencmp22 (my data [1] [1], my data [1] [2], my data [2] [2], &a, &b, &cs, &sn);
	NUMgetEllipseBoundingBox (sqrt (a), sqrt (b), cs, & width, & height);

	*xmin = my centroid [1] - lscale * width / 2.0;
	*xmax = *xmin + lscale * width;
	*ymin = my centroid [2] - lscale * height / 2.0;
	*ymax = *ymin + lscale * height;
}

void SSCPList_getEllipsesBoundingBoxCoordinates (SSCPList me, double scale, bool confidence, double *p_xmin, double *p_xmax, double *p_ymin, double *p_ymax) {
	double xmin = 1e308, xmax = -xmin, ymin = xmin, ymax = -ymin;

	for (integer i = 1; i <= my size; i ++) {
		SSCP s = my at [i];
		double xmn, xmx, ymn, ymx;
		getEllipseBoundingBoxCoordinates (s, scale, confidence, &xmn, &xmx, &ymn, &ymx);
		if (xmn < xmin) {
			xmin = xmn;
		}
		if (xmx > xmax) {
			xmax = xmx;
		}
		if (ymn < ymin) {
			ymin = ymn;
		}
		if (ymx > ymax) {
			ymax = ymx;
		}
	}
	if (p_xmin) {
		*p_xmin = xmin;
	}
	if (p_xmax) {
		*p_xmax = xmax;
	}
	if (p_ymin) {
		*p_ymin = ymin;
	}
	if (p_ymax) {
		*p_ymax = ymax;
	}
}

static autoSSCP _SSCP_extractTwoDimensions (SSCP me, integer d1, integer d2) {
	autoSSCP thee = SSCP_create (2);
	if (my numberOfRows == 1) { // diagonal
		thy data [1] [1] = my data [1] [d1];
		thy data [2] [2] = my data [1] [d2];
	} else {
		thy data [1] [1] = my data [d1] [d1];
		thy data [2] [2] = my data [d2] [d2];
		thy data [2] [1] = thy data [1] [2] = my data [d1] [d2];
	}
	thy centroid [1] = my centroid [d1];
	thy centroid [2] = my centroid [d2];
	thy numberOfObservations = my numberOfObservations;

	TableOfReal_setColumnLabel (thee.get(), 1, my columnLabels [d1]);
	TableOfReal_setColumnLabel (thee.get(), 2, my columnLabels [d2]);
	TableOfReal_setRowLabel (thee.get(), 1, my columnLabels [d1]);
	TableOfReal_setRowLabel (thee.get(), 2, my columnLabels [d2]);
	return thee;
}

autoSSCPList SSCPList_extractTwoDimensions (SSCPList me, integer d1, integer d2) {
	try {
		autoSSCPList thee = SSCPList_create ();
		for (integer i = 1; i <= my size; i ++) {
			autoSSCP t = _SSCP_extractTwoDimensions (my at [i], d1, d2);
			Thing_setName (t.get(), Thing_getName (my at [i]));
			thy addItem_move (t.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract two dimensions.");
	}
}

void SSCP_drawTwoDimensionalEllipse_inside (SSCP me, Graphics g, double scale, const char32 *label, int fontSize) {
	try {
		integer nsteps = 100;
		autoNUMvector <double> x ((integer) 0, nsteps);
		autoNUMvector <double> y ((integer) 0, nsteps);
		// Get principal axes and orientation for the ellipse by performing the
		// eigen decomposition of a symmetric 2-by-2 matrix.
		// Principal axes are a and b with eigenvector/orientation (cs, sn).

		double a, b, cs, sn;
		NUMeigencmp22 (my data [1] [1], my data [1] [2], my data [2] [2], &a, &b, &cs, &sn);
		// 1. Take sqrt to get units of 'std_dev'

		a = scale * sqrt (a) / 2.0;
		b = scale * sqrt (b) / 2.0;
		x [nsteps] = x [0] = my centroid [1] + cs * a;
		y [nsteps] = y [0] = my centroid [2] + sn * a;
		double angle = 0.0;
		double angle_inc = NUM2pi / nsteps;
		for (integer i = 1; i < nsteps; i ++, angle += angle_inc) {
			double xc = a * cos (angle);
			double yc = b * sin (angle);
			double xt = xc * cs - yc * sn;
			y [i] = my centroid [2] + xc * sn + yc * cs;
			x [i] = my centroid [1] + xt;
		}
		Graphics_polyline (g, nsteps + 1, x.peek(), y.peek());
		if (label) {
			int oldFontSize = Graphics_inqFontSize (g);
			Graphics_setFontSize (g, fontSize);
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
			Graphics_text (g, my centroid [1], my centroid [2], label);
			Graphics_setFontSize (g, oldFontSize);
		}
	} catch (MelderError) {
		Melder_clearError ();
	}
}

static void _SSCP_drawTwoDimensionalEllipse (SSCP me, Graphics g, double scale, int fontSize) {
	integer nsteps = 100;
	const char32 *name;

	autoNUMvector <double> x ((integer) 0, nsteps);
	autoNUMvector <double> y ((integer) 0, nsteps);

	// Get principal axes and orientation for the ellipse by performing the
	// eigen decomposition of a symmetric 2-by-2 matrix.
	// Principal axes are a and b with eigenvector/orientation (cs, sn).

	double a, b, cs, sn;
	NUMeigencmp22 (my data [1] [1], my data [1] [2], my data [2] [2], &a, &b, &cs, &sn);

	// 1. Take sqrt to get units of 'std_dev'

	a = scale * sqrt (a) / 2.0;
	b = scale * sqrt (b) / 2.0;
	x [nsteps] = x [0] = my centroid [1] + cs * a;
	y [nsteps] = y [0] = my centroid [2] + sn * a;
	double angle = 0.0;
	double angle_inc = NUM2pi / nsteps;
	for (integer i = 1; i < nsteps; i ++, angle += angle_inc) {
		double xc = a * cos (angle);
		double yc = b * sin (angle);
		double xt = xc * cs - yc * sn;
		y [i] = my centroid [2] + xc * sn + yc * cs;
		x [i] = my centroid [1] + xt;
	}
	Graphics_polyline (g, nsteps + 1, x.peek(), y.peek());
	if (fontSize > 0 && (name = Thing_getName (me)) != nullptr) {
		int oldFontSize = Graphics_inqFontSize (g);
		Graphics_setFontSize (g, fontSize);
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, my centroid [1], my centroid [2], name);
		Graphics_setFontSize (g, oldFontSize);
	}
}

autoSSCP SSCP_toTwoDimensions (SSCP me, double *v1, double *v2) {
	try {
		double *vec [3];
		autoSSCP thee = SSCP_create (2);

		// Projection P of S on v1 and v2 (given matrix V' with 2 rows) is P = V'SV
		// P [i] [j] = sum(k) sum(m) V' [i] [k]*S [k] [m]*V [m] [j] = V' [i] [k]*S [k] [m]*V' [j] [m]

		// For the new centroids cnew [i] = sum(m) V' [i] [m]*c [m]

		vec [1] = v1; vec [2] = v2;
		if (my numberOfRows == 1) { // 1xn diagonal matrix
			for (integer k = 1; k <= my numberOfColumns; k ++) {
				thy data [1] [1] += v1 [k] * my data [1] [k] * v1 [k];
			}
			for (integer k = 1; k <= my numberOfColumns; k ++) {
				thy data [1] [2] += v1 [k] * my data [1] [k] * v2 [k];
			}
			for (integer k = 1; k <= my numberOfColumns; k ++) {
				thy data [2] [2] += v2 [k] * my data [1] [k] * v2 [k];
			}
			thy data [2] [1] = thy data [1] [2];
		} else {
			for (integer i = 1; i <= 2; i ++) {
				for (integer j = i; j <= 2; j ++) {
					double sum = 0;
					for (integer k = 1; k <= my numberOfRows; k ++) {
						for (integer m = 1; m <= my numberOfRows; m ++) {
							sum += vec [i] [k] * my data [k] [m] * vec [j] [m];
						}
					}
					thy data [j] [i] = thy data [i] [j] = sum;
				}
			}
		}

		// centroids

		for (integer m = 1; m <= my numberOfColumns; m ++) {
			thy centroid [1] += v1 [m] * my centroid [m];
		}
		for (integer m = 1; m <= my numberOfColumns; m ++) {
			thy centroid [2] += v2 [m] * my centroid [m];
		}

		thy numberOfObservations = SSCP_getNumberOfObservations (me);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract two dimensions.");
	}
}

void SSCP_init (SSCP me, integer dimension, integer storage) {
	TableOfReal_init (me, storage, dimension);
	my centroid = NUMvector<double> (1, dimension);
}

autoSSCP SSCP_create (integer dimension) {
	try {
		autoSSCP me = Thing_new (SSCP);
		SSCP_init (me.get(), dimension, dimension);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SSCP not created.");
	}
}

double SSCP_getConcentrationEllipseArea (SSCP me, double scale, bool confidence, integer d1, integer d2) {

	Melder_require (d1 > 0 && d1 <= my numberOfRows && d2 > 0 && d2 <= my numberOfRows && d1 != d2,
		U"Incorrect axes.");

	autoSSCP thee = _SSCP_extractTwoDimensions (me, d1, d2);
	scale = SSCP_getEllipseScalefactor (thee.get(), scale, confidence);
	Melder_require (scale > 0, U"The scale factor should be larger than zero.");

	double a, b, cs, sn;
	NUMeigencmp22 (thy data [1] [1], thy data [1] [2], thy data [2] [2], & a, & b, & cs, & sn);

	// 1. Take sqrt to get units of 'std_dev'

	a = scale * sqrt (a) / 2.0;
	b = scale * sqrt (b) / 2.0;
	return NUMpi * a * b;
}

double SSCP_getFractionVariation (SSCP me, integer from, integer to) {
	integer n = my numberOfRows;

	if (from < 1 || from > to || to > n) {
		return undefined;
	}

	double sum = 0.0, trace = 0.0;
	for (integer i = 1; i <= n; i ++) {
		trace += my numberOfRows == 1 ? my data [1] [i] : my data [i] [i];
		if (i >= from && i <= to) {
			sum += my numberOfRows == 1 ? my data [1] [i] : my data [i] [i];
		}
	}
	return trace > 0.0 ? sum / trace : undefined;
}

void SSCP_drawConcentrationEllipse (SSCP me, Graphics g, double scale, int confidence, integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, int garnish) {

	Melder_require (d1 > 0 && d1 <= my numberOfRows && d2 > 0 && d2 <= my numberOfRows && d1 != d2, U"Incorrect axes.");

	autoSSCP thee = _SSCP_extractTwoDimensions (me, d1, d2);

	double xmn, xmx, ymn, ymx;
	getEllipseBoundingBoxCoordinates (thee.get(), scale, confidence, &xmn, &xmx, &ymn, &ymx);

	if (xmax == xmin) {
		xmin = xmn; xmax = xmx;
	}

	if (ymax == ymin) {
		ymin = ymn; ymax = ymx;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);

	scale = SSCP_getEllipseScalefactor (thee.get(), scale, confidence);
	Melder_require (scale > 0, U"The scale factor should be larger than zero.");
	
	_SSCP_drawTwoDimensionalEllipse (thee.get(), g, scale, 0);

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

void SSCP_setNumberOfObservations (SSCP me, double numberOfObservations) {
	my numberOfObservations = numberOfObservations;
}

double SSCP_getNumberOfObservations (SSCP me) {
	return my numberOfObservations;
}

double SSCP_getDegreesOfFreedom (SSCP me) {
	return my numberOfObservations - 1;
}

double SSCP_getTotalVariance (SSCP me) {
	double trace = 0.0;
	for (integer i = 1; i <= my numberOfColumns; i ++) {
		trace += my numberOfRows == 1 ? my data [1] [i] : my data [i] [i];
	}
	return trace;
}

double SSCP_getCumulativeContributionOfComponents (SSCP me, integer from, integer to) {
	double sum = undefined;
	if (to == 0) {
		to = my numberOfRows;
	}
	if (from > 0 && to <= my numberOfRows && from <= to) {
		sum = SSCP_getTotalVariance (me);
		double partial = 0.0;
		for (integer i = from; i <= to; i ++) {
			partial += my numberOfRows == 1 ? my data [1] [i] : my data [i] [i];
		}
		if (sum > 0.0) {
			sum = partial / sum;
		}
	}
	return sum;
}

/* For nxn matrix only ! */
void Covariance_PCA_generateOneVector (Covariance me, PCA thee, double *vec, double *buf) {
	// Generate the multi-normal vector elements N(0,sigma)

	for (integer j = 1; j <= my numberOfColumns; j ++) {
		buf [j] = NUMrandomGauss (0.0, sqrt (thy eigenvalues [j]));
	}

	// Rotate back

	for (integer j = 1; j <= my numberOfColumns; j ++) {
		vec [j] = 0.0;
		for (integer k = 1; k <= my numberOfColumns; k ++) {
			vec [j] += buf [k] * thy eigenvectors [k] [j];
		}
	}

	// Restore the centroid

	for (integer j = 1; j <= my numberOfColumns; j ++) {
		vec [j] += my centroid [j];
	}
}

autoTableOfReal Covariance_to_TableOfReal_randomSampling (Covariance me, integer numberOfData) {
	try {
		if (numberOfData <= 0) {
			numberOfData = Melder_ifloor (my numberOfObservations);
		}
		autoPCA pca = SSCP_to_PCA (me);
		autoTableOfReal thee = TableOfReal_create (numberOfData, my numberOfColumns);
		autoNUMvector<double> buf (1, my numberOfColumns);

		for (integer i = 1; i <= numberOfData; i ++) {
			Covariance_PCA_generateOneVector (me, pca.get(), thy data [i], buf.peek());
		}

		NUMstrings_copyElements (my columnLabels, thy columnLabels, 1, my numberOfColumns);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not random sampled.");
	}
}

autoSSCP TableOfReal_to_SSCP (TableOfReal me, integer rowb, integer rowe, integer colb, integer cole) {
	try {
		Melder_require (! NUMdmatrix_containsUndefinedElements (my data, 1, my numberOfRows, 1, my numberOfColumns),
			U"All the table's elements should be defined.");

		if (rowb == 0 && rowe == 0) {
			rowb = 1;
			rowe = my numberOfRows;
		}
		Melder_require (rowb > 0 && rowb <= rowe && rowe <= my numberOfRows, U"Invalid row number.");

		if (colb == 0 && cole == 0) {
			colb = 1;
			cole = my numberOfColumns;
		}
		Melder_require (colb > 0 && colb <= cole && cole <= my numberOfColumns, U"Invalid column number.");

		integer numberOfRows = rowe - rowb + 1; /* m */
		integer numberOfColumns = cole - colb + 1; /* n */

		if (numberOfRows < numberOfColumns) {
			Melder_warning (U"The SSCP will not have \n"
				"full dimensionality. This may be a problem in following analysis steps. \n"
				"(The number of data points was less than the number of variables.)");
		}
		autoSSCP thee = SSCP_create (numberOfColumns);
		autoNUMmatrix<double> v (1, numberOfRows, 1, numberOfColumns);

		integer nvalidrows = 0;
		for (integer i = 1; i <= numberOfRows; i ++) {
			nvalidrows ++;
			for (integer j = 1; j <= numberOfColumns; j ++) {
				v [i] [j] = my data [rowb + i - 1] [colb + j - 1];
			}
		}

		NUMcentreColumns (v.peek(), 1, numberOfRows, 1, numberOfColumns, thy centroid);

		SSCP_setNumberOfObservations (thee.get(), numberOfRows);

		// sum of squares and cross products = T'T

		for (integer i = 1; i <= numberOfColumns; i ++) {
			for (integer j = i; j <= numberOfColumns; j ++) {
				double t = 0.0;
				for (integer k = 1; k <= numberOfRows; k ++) {
					t += v [k] [i] * v [k] [j];
				}
				thy data [i] [j] = thy data [j] [i] = t;
			}
		}
		for (integer j = 1; j <= numberOfColumns; j ++) {
			char32 *label = my columnLabels [colb + j - 1];
			TableOfReal_setColumnLabel (thee.get(), j, label);
			TableOfReal_setRowLabel (thee.get(), j, label);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": SSCP not created.");
	}
}

autoTableOfReal SSCP_TableOfReal_extractDistanceQuantileRange (SSCP me, TableOfReal thee, double qlow, double qhigh) {
	try {
		autoCovariance cov = SSCP_to_Covariance (me, 1);
		autoTableOfReal him = Covariance_TableOfReal_extractDistanceQuantileRange (cov.get(), thee, qlow, qhigh);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no distance quantile ranges created.");
	}
}

autoTableOfReal Covariance_TableOfReal_mahalanobis (Covariance me, TableOfReal thee, bool useTableCentroid) {
	try {
		autoTableOfReal him = TableOfReal_create (thy numberOfRows, 1);
		autoNUMvector<double> centroid (NUMvector_copy (my centroid, 1, thy numberOfColumns), 1);
		autoNUMmatrix<double> covari (NUMmatrix_copy (my data, 1, my numberOfRows, 1, my numberOfRows), 1, 1);

		/*
			Mahalanobis distance calculation. S = L.L' -> S**-1 = L**-1' . L**-1
				(x-m)'S**-1 (x-m) = (x-m)'L**-1' . L**-1. (x-m) =
				(L**-1.(x-m))' . (L**-1.(x-m))

			Get inverse of covari in lower triangular part.
		*/
		double lndet;
		NUMlowerCholeskyInverse (covari.peek(), my numberOfRows, & lndet);

		if (useTableCentroid) {
			for (integer icol = 1; icol <= thy numberOfColumns; icol ++) {
				double mean = 0.0;
				for (integer irow = 1; irow <= thy numberOfRows; irow ++) {
					mean += thy data [irow] [icol];
				}
				centroid [icol] = mean / thy numberOfRows;
			}
		}

		for (integer k = 1; k <= thy numberOfRows; k ++) {
			his data [k] [1] = sqrt (NUMmahalanobisDistance_chi (covari.peek(), thy data [k], centroid.peek(), my numberOfRows, my numberOfRows));
			if (thy rowLabels [k]) {
				TableOfReal_setRowLabel (him.get(), k, thy rowLabels [k]);
			}
		}
		TableOfReal_setColumnLabel (him.get(), 1, U"d");
		return him;
	} catch (MelderError) {
		Melder_throw (me, U"no Mahalanobis distances created.");
	}
}

autoTableOfReal Covariance_TableOfReal_extractDistanceQuantileRange (Covariance me, TableOfReal thee, double qlow, double qhigh) {
	try {
		autoTableOfReal him = Covariance_TableOfReal_mahalanobis (me, thee, false);

		double low = TableOfReal_getColumnQuantile (him.get(), 1, qlow);
		double high = TableOfReal_getColumnQuantile (him.get(), 1, qhigh);

		// Count the number filtered.
		// nsel = (qhigh - qlow) * nrows is sometimes one off

		integer nsel = 0;
		for (integer i = 1; i <= thy numberOfRows; i ++) {
			if (low <= his data [i] [1] && his data [i] [1] < high) {
				nsel ++;
			}
		}

		Melder_require (nsel > 0, U"Not enough data in quantile interval.");
		
		autoTableOfReal r = TableOfReal_create (nsel, thy numberOfColumns);
		NUMstrings_copyElements (thy columnLabels, r -> columnLabels, 1, thy numberOfColumns);
		//((r -> columnLabels = NUMstrings_copy (thy columnLabels, 1, thy numberOfColumns)) == nullptr)) goto end;

		integer k = 0;
		for (integer i = 1; i <= thy numberOfRows; i ++) {
			if (low <= his data [i] [1] && his data [i] [1] < high) {
				k ++;
				TableOfReal_copyOneRowWithLabel (thee, r.get(), i, k);
			}
		}
		return r;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal with distance quantile range not created.");
	}
}

autoCovariance TableOfReal_to_Covariance (TableOfReal me) {
	try {
		autoSSCP sscp = TableOfReal_to_SSCP (me, 0, 0, 0, 0);
		autoCovariance thee = SSCP_to_Covariance (sscp.get(), 1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": covariances not created.");
	}
}

autoCorrelation TableOfReal_to_Correlation (TableOfReal me) {
	try {
		autoSSCP sscp = TableOfReal_to_SSCP (me, 0, 0, 0, 0);
		autoCorrelation thee = SSCP_to_Correlation (sscp.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": correlations not created.");
	}
}

autoCorrelation TableOfReal_to_Correlation_rank (TableOfReal me) {
	try {
		autoTableOfReal t = TableOfReal_rankColumns (me);
		autoCorrelation thee = TableOfReal_to_Correlation (t.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rank correlations not created.");
	}
}

autoSSCPList TableOfReal_to_SSCPList_byLabel (TableOfReal me) {
	try {
		autoSSCPList thee = SSCPList_create ();
		autoTableOfReal mew = TableOfReal_sortOnlyByRowLabels (me);

		Melder_warningOff ();
		integer lastrow = 0, numberOfMatrices = 0, numberOfSingularMatrices = 0, index = 1;
		const char32 *label = mew -> rowLabels [1];
		for (integer i = 2; i <= my numberOfRows; i ++) {
			integer numberOfRowsInCurrent = 0;
			const char32 *currentLabel = mew -> rowLabels [i];
			if (Melder_cmp (currentLabel, label) != 0) {
				// current label different from previous one(s)
				numberOfRowsInCurrent = i - index;
				lastrow = i - 1;
			} else if (i == my numberOfRows) {
				// current (last) label is same as previous
				numberOfRowsInCurrent = i - index + 1;
				lastrow = i;
			} else {
				// next one
				continue;
			}
			// We found a new group
			         numberOfMatrices ++;
			if (numberOfRowsInCurrent > 1) { // We need at least two rows for an SSCP
				if (numberOfRowsInCurrent < my numberOfColumns) {
					numberOfSingularMatrices ++;
				}
				autoSSCP t = TableOfReal_to_SSCP (mew.get(), index, lastrow, 0, 0);
				if (! (label = mew -> rowLabels [index])) {
					label = U"?";
				}
				Thing_setName (t.get(), label);
				thy addItem_move (t.move());
			}
			label = currentLabel;
			index = i;
		}
		if (lastrow != my numberOfRows) {
			numberOfMatrices ++;
		}
		Melder_warningOn ();
		if (numberOfSingularMatrices > 0 || thy size != numberOfMatrices) {
			integer notIncluded = numberOfMatrices - thy size;
			Melder_warning (numberOfMatrices, U" different groups detected: ", numberOfSingularMatrices + notIncluded,
				U" group(s) with less rows than columns (of which ", notIncluded, U" with only one row).");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": SSCP not created from labels.");
	}
}

autoPCA SSCP_to_PCA (SSCP me) {
	try {
		double **data = my data;
		autoNUMmatrix<double> adata;
		autoPCA thee = PCA_create (my numberOfColumns, my numberOfColumns);

		if (my numberOfRows == 1) { // 1xn matrix -> nxn
			// ugly hack
			adata.reset (1, my numberOfColumns, 1, my numberOfColumns);
			for (integer i = 1; i <= my numberOfColumns; i ++) {
				data [i] [i] = my data [1] [i];
			}
			data = adata.peek();
		}
		NUMstrings_copyElements (my columnLabels, thy labels, 1, my numberOfColumns);
		Eigen_initFromSymmetricMatrix (thee.get(), data, my numberOfColumns);
		NUMvector_copyElements (my centroid, thy centroid, 1, my numberOfColumns);
		PCA_setNumberOfObservations (thee.get(), Melder_ifloor (my numberOfObservations));
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": PCA not created.");
	}
}

void SSCP_setValue (SSCP me, integer row, integer col, double value) {
	Melder_require (col > 0 && col <= my numberOfColumns, U"Illegal column number.");
	Melder_require (row > 0 && row <= my numberOfRows, U"Illegal row number.");
	Melder_require (! (row == col && value <= 0), U"Diagonal element should always be a positive number.");
	
	if (my numberOfRows == 1) { // diagonal
		Melder_require (row == col, U"Row and column number should be equal for a diagonal matrix.");
		my data [1] [row] = value;
	} else {
		Melder_require (!(row != col && (fabs (value) > my data [row] [row] || fabs (value) > my data [row] [row])),
			U"The off-diagonal should not be larger than the diagonal values. Input diagonal elements first, or change this value.");
		my data [row] [col] = my data [col] [row] = value;
	}
}

void SSCP_setCentroid (SSCP me, integer component, double value) {
	Melder_require (component > 0 && component <= my numberOfColumns, U"Component number should not exceed ", my numberOfColumns, U".");
	my centroid [component] = value;
}

autoCCA SSCP_to_CCA (SSCP me, integer ny) {
	try {
		char upper = 'L', diag = 'N';
		integer info;
		Melder_require (ny > 0 && ny < my numberOfRows, U"Invalid split.");
		Melder_require (my numberOfRows > 1, U"Matrix should not be diagonal.");

		integer m = my numberOfRows, nx = m - ny, xy_interchanged = nx < ny, yof = 0, xof = ny;
		if (xy_interchanged) {
			yof = ny; xof = 0;
			nx = ny; ny = m - nx;
		}

		autoCCA thee = Thing_new (CCA);
		autoNUMmatrix<double> sxx (1, nx, 1, nx);
		autoNUMmatrix<double> syy (1, ny, 1, ny);
		autoNUMmatrix<double> syx (1, ny, 1, nx);

		// Copy Syy and Sxx into upper part of syy and sxx matrices.

		for (integer i = 1; i <= ny; i ++) {
			for (integer j = i; j <= ny; j ++) {
				syy [i] [j] = my data [yof + i] [yof + j];
			}
		}
		for (integer i = 1; i <= nx; i ++) {
			for (integer j = i; j <= nx; j ++) {
				sxx [i] [j] = my data [xof + i] [xof + j];
			}
		}

		for (integer i = 1; i <= nx; i ++) {
			for (integer j = 1; j <= ny; j ++) {
				syx [i] [j] = my data [yof + i] [xof + j];
			}
		}

		// Cholesky decomposition: Syy = Uy'*Uy and Sxx = Ux'*Ux.
		// (Pretend as if colum-major storage)

		(void) NUMlapack_dpotf2 (& upper, & ny, & syy [1] [1], & ny, & info);
		if (info != 0) Melder_throw (U"The leading minor of order ", info, U" is not positive definite, and the "
			                             U"factorization of Syy could not be completed.");

		(void) NUMlapack_dpotf2 (& upper, & nx, & sxx [1] [1], & nx, & info);
		if (info != 0) Melder_throw (U"The leading minor of order ", info, U" is not positive definite, and the "
			                             U"factorization of Sxx could not be completed.");

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

		(void) NUMlapack_dtrti2 (& upper, & diag, & nx, & sxx [1] [1], & nx, & info);
		Melder_require (info == 0, U"Error in inverse for Sxx.");

		// Prepare Uxi' * Syx' = (Syx * Uxi)'

		for (integer i = 1; i <= ny; i ++) {
			for (integer j = 1; j <= nx; j ++) {
				real80 t = 0.0;
				for (integer k = 1; k <= j; k ++) {
					t += syx [i] [k] * sxx [k] [j];
				}
				a [j] [i] = (real) t;
			}
		}

		autoGSVD gsvd = GSVD_create_d (a.peek(), nx, ny, syy.peek(), ny);
		autoNUMmatrix<double> ri (NUMmatrix_copy (gsvd -> r, 1, gsvd -> numberOfColumns, 1, gsvd -> numberOfColumns), 1, 1);
		thy y = Eigen_create (gsvd -> numberOfColumns, gsvd -> numberOfColumns);
		thy x = Eigen_create (thy y -> numberOfEigenvalues, nx);

		// Get X=Q*R**-1

		(void) NUMlapack_dtrti2 (&upper, &diag, &gsvd -> numberOfColumns, &ri [1] [1], &gsvd -> numberOfColumns, &info);
		Melder_require (info == 0, U"Error in inverse for R.");
		
		for (integer i = 1; i <= gsvd -> numberOfColumns; i ++) {
			double t = gsvd -> d1 [i] / gsvd -> d2 [i];
			thy y -> eigenvalues [i] = t * t;
			for (integer j = 1; j <= gsvd -> numberOfColumns; j ++) {
				t = 0.0;
				for (integer k = 1; k <= j; k ++) {
					t += gsvd -> q [i] [k] * ri [k] [j];
				}
				thy y -> eigenvectors [j] [i] = t;
			}
		}

		NUMnormalizeRows (thy y -> eigenvectors, thy y -> numberOfEigenvalues, thy y -> numberOfEigenvalues, 1);

		thy numberOfCoefficients = thy y -> numberOfEigenvalues;
		thy numberOfObservations = Melder_ifloor (my numberOfObservations);

		// x = Sxx**-1 * Syx' * y

		for (integer i = 1; i <= thy numberOfCoefficients; i ++) {
			double *evecy = thy y -> eigenvectors [i];
			double *evecx = thy x -> eigenvectors [i];
			for (integer j = 1; j <= nx; j ++) {
				double t = 0.0;
				for (integer k = j; k <= nx; k ++) {
					for (integer l = 1; l <= nx; l ++) {
						for (integer n = 1; n <= ny; n ++) {
							t += sxx [j] [k] * sxx [l] [k] * syx [n] [l] * evecy [n];
						}
					}
				}
				evecx [j] = t;
			}
		}

		NUMnormalizeRows (thy x -> eigenvectors, thy x -> numberOfEigenvalues, nx, 1);

		if (ny < nx) {
			autoEigen t = thy x.move();
			thy x = thy y.move(); thy y = t.move();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": CCA not created.");
	}
}

/************ SSCPList ***********************************************/

autoSSCP SSCPList_to_SSCP_pool (SSCPList me) {
	try {
		autoSSCP thee = Data_copy (my at [1]);

		for (integer k = 2; k <= my size; k ++) {
			SSCP t = my at [k];
			Melder_require (t -> numberOfRows == thy numberOfRows, U"The dimension of item ", k, U" should agree.");
			
			thy numberOfObservations += t -> numberOfObservations;

			// Sum the sscp's and weigh the centroid.

			for (integer i = 1; i <= thy numberOfRows; i ++) {   // if 1xn
				for (integer j = 1; j <= thy numberOfColumns; j ++) {
					thy data [i] [j] += t -> data [i] [j];
				}
			}

			for (integer j = 1; j <= thy numberOfRows; j ++) {
				thy centroid [j] += t -> numberOfObservations * t -> centroid [j];
			}
		}

		for (integer i = 1; i <= thy numberOfRows; i ++) {
			thy centroid [i] /= thy numberOfObservations;
		}
		
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not pooled.");
	}
}

autoCovariance CovarianceList_to_Covariance_pool (CovarianceList me) { // Morrison sec 3.5, page 100
	try {
		autoCovariance thee = Data_copy (my at [1]);
		double scaleFactor = thy numberOfObservations - 1.0;
		
		for (integer i = 1; i <= thy numberOfRows; i ++) {
			for (integer j = 1; j <= thy numberOfColumns; j ++) {
				thy data [i] [j] *= scaleFactor;
			}
		}
		
		for (integer k = 2; k <= my size; k ++) {
			Covariance t = my at [k];
			Melder_require (t -> numberOfRows == thy numberOfRows, U"The dimension of item ", k, U" should agree.");

			thy numberOfObservations += t -> numberOfObservations;

			// Sum the sscp's and weigh the centroid.
			scaleFactor = t -> numberOfObservations - 1.0;
			for (integer i = 1; i <= thy numberOfRows; i ++) { // if 1xn
				for (integer j = 1; j <= thy numberOfColumns; j ++) {
					thy data [i] [j] += scaleFactor * t -> data [i] [j];
				}
			}

			for (integer j = 1; j <= thy numberOfRows; j ++) {
				thy centroid [j] += t -> numberOfObservations * t -> centroid [j];
			}
		}

		for (integer i = 1; i <= thy numberOfRows; i ++) {
			thy centroid [i] /= thy numberOfObservations;
		}
		
		scaleFactor = 1.0 / (thy numberOfObservations - my size);
		for (integer i = 1; i <= thy numberOfRows; i ++) { // if 1xn
			for (integer j = 1; j <= thy numberOfColumns; j ++) {
				thy data [i] [j] *= scaleFactor;
			}
		}

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not pooled.");
	}
}

void SSCPList_getHomegeneityOfCovariances_box (SSCPList me, double *p_prob, double *p_chisq, double *p_df) {
	double chisq = 0.0, df = undefined;

	autoSSCP pooled = SSCPList_to_SSCP_pool (me);
	integer p = pooled -> numberOfColumns;
	double ln_determinant, inv = 0.0, sum = 0.0, g = my size;
	for (integer i = 1; i <= g; i ++) {
		SSCP t = my at [i];
		double ni = t -> numberOfObservations - 1.0;
		ln_determinant = NUMdeterminant_cholesky (t -> data, p);

		// Box-test is for covariance matrices -> scale determinant.

		ln_determinant -= p * log (ni);
		sum += ni;
		inv += 1.0 / ni;
		chisq -= ni * ln_determinant;
	}

	ln_determinant = NUMdeterminant_cholesky (pooled -> data, p);
	ln_determinant -= p * log (pooled -> numberOfObservations - g);
	chisq += sum * ln_determinant;

	chisq *= 1.0 - (inv - 1.0 / sum) * (2.0 * p * p + 3.0 * p - 1.0) / (6.0 * (p + 1) * (g - 1.0));
	df = (g - 1.0) * p * (p + 1) / 2.0;
	if (p_prob) {
		*p_prob = NUMchiSquareQ (chisq, df);
	}
	if (p_chisq) {
		*p_chisq = chisq;
	}
	if (p_df) {
		*p_df = df;
	}
}

autoSSCPList SSCPList_toTwoDimensions (SSCPList me, double v1 [], double v2 []) {
	try {
		autoSSCPList thee = SSCPList_create ();
		for (integer i = 1; i <= my size; i ++) {
			autoSSCP t = SSCP_toTwoDimensions (my at [i], v1, v2);
			Thing_setName (t.get(), Thing_getName (my at [i]));
			thy addItem_move (t.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not reduced to two dimensions.");
	}
}


void SSCPList_drawConcentrationEllipses (SSCPList me, Graphics g, double scale, bool confidence, const char32 *label, integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, int fontSize, bool garnish) {
	SSCP t = my at [1];

	Melder_require (d1 > 0 && d1 <= t -> numberOfColumns && d2 > 0 && d2 <= t -> numberOfColumns && d1 != d2, U"Incorrect axes.");

	autoSSCPList thee = SSCPList_extractTwoDimensions (me, d1, d2);

	/*
		Autowindowing.
	*/
	if (xmin == xmax || ymin == ymax) {
		double boundingBox_xmin, boundingBox_xmax, boundingBox_ymin, boundingBox_ymax;
		SSCPList_getEllipsesBoundingBoxCoordinates (thee.get(), scale, confidence,
			& boundingBox_xmin, & boundingBox_xmax, & boundingBox_ymin, & boundingBox_ymax);
		if (xmin == xmax) {
			xmin = boundingBox_xmin;
			xmax = boundingBox_xmax;
		}
		if (ymin == ymax) {
			ymin = boundingBox_ymin;
			ymax = boundingBox_ymax;
		}
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);


	for (integer i = 1; i <= thy size; i ++) {
		t = thy at [i];
		double lscale = SSCP_getEllipseScalefactor (t, scale, confidence);
		if (lscale < 0.0) {
			continue;
		}
		if (! label || Melder_cmp (label, Thing_getName (t)) == 0) {
			_SSCP_drawTwoDimensionalEllipse (t, g, lscale, fontSize);
		}
	}

	Graphics_unsetInner (g);
	if (garnish) {
		t = my at [1];
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, t -> columnLabels [d2] ? t -> columnLabels [d2] : Melder_cat (U"Dimension ", d2));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textBottom (g, true, t -> columnLabels [d1] ? t -> columnLabels [d1] : Melder_cat (U"Dimension ", d1));
	}
}

autoTableOfReal SSCP_to_TableOfReal (SSCP me) {
	try {
		autoTableOfReal thee = Thing_new (TableOfReal);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not copied.");
	}
}

autoTableOfReal SSCP_extractCentroid (SSCP me) {
	try {
		autoTableOfReal thee = TableOfReal_create (1, my numberOfColumns);
		NUMvector_copyElements (my centroid, thy data [1], 1, my numberOfColumns);
		thy columnLabels = NUMstrings_copy (my columnLabels, 1, my numberOfColumns);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": centroid not extracted.");
	}
}

autoCovariance Covariance_create (integer dimension) {
	try {
		autoCovariance me = Thing_new (Covariance);
		SSCP_init (me.get(), dimension, dimension);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Covariance not created.");
	}
}

autoCovariance Covariance_create_reduceStorage (integer dimension, integer storage) {
	try {
		autoCovariance me = Thing_new (Covariance);
		if (storage <= 0 || storage >= dimension) {
			storage = dimension;
		}
		SSCP_init (me.get(), dimension, storage);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Reduced storage covariance not created.");
	}
}



autoCovariance Covariance_createSimple (char32 *s_covariances, char32 *s_centroid, integer numberOfObservations) {
	try {
		integer dimension, numberOfCovariances;
		autoNUMvector <real> centroid (NUMstring_to_numbers (s_centroid, & dimension), 1);
		autoNUMvector <real> covariances (NUMstring_to_numbers (s_covariances, & numberOfCovariances), 1);
		integer numberOfCovariances_wanted = dimension * (dimension + 1) / 2;
		Melder_require (numberOfCovariances == numberOfCovariances_wanted,
			U"The number of covariance matrix elements and the number of centroid elements should agree. "
			"There should be d(d+1)/2 covariance values and d centroid values.");
		
		autoCovariance me = Covariance_create (dimension);

		// Construct the full covariance matrix from the upper-diagonal elements

		integer rowNumber = 1;
		for (integer inum = 1; inum <= numberOfCovariances; inum ++) {
			integer nmissing = (rowNumber - 1) * rowNumber / 2;
			integer inumc = inum + nmissing;
			rowNumber = (inumc - 1) / dimension + 1;
			integer icol = ((inumc - 1) % dimension) + 1;
			my data [rowNumber] [icol] = my data [icol] [rowNumber] = covariances [inum];
			if (icol == dimension) {
				rowNumber ++;
			}
		}

		// Check if a valid covariance, first check variances then covariances

		for (integer irow = 1; irow <= dimension; irow ++) {
			Melder_require (my data [irow] [irow] > 0.0, U"The diagonal matrix elements should all be positive numbers.");
		}
		for (integer irow = 1; irow <= dimension; irow ++) {
			for (integer icol = irow + 1; icol <= dimension; icol ++) {
				if (fabs (my data [irow] [icol] / sqrt (my data [irow] [irow] * my data [icol] [icol])) > 1) {
					integer nmissing = (irow - 1) * irow / 2;
					integer inum = (irow - 1) * dimension + icol - nmissing;
					Melder_throw (U"The covariance in cell [", irow, U",", icol, U"], i.e. input item ", inum, U" is too large.");
				}
			}
		}
		for (integer inum = 1; inum <= dimension; inum ++) {
			my centroid [inum] = centroid [inum];
		}
		my numberOfObservations = numberOfObservations;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Simple Covariance not created.");
	}
}

autoCorrelation Correlation_createSimple (char32 *s_correlations, char32 *s_centroid, integer numberOfObservations) {
	try {
		integer dimension, numberOfCorrelations;
		autoNUMvector<double> centroids (NUMstring_to_numbers (s_centroid, & dimension), 1);
		autoNUMvector<double> correlations (NUMstring_to_numbers (s_correlations, & numberOfCorrelations), 1);
		integer numberOfCorrelations_wanted = dimension * (dimension + 1) / 2;
		Melder_require (numberOfCorrelations == numberOfCorrelations_wanted,
			U"The number of correlation matrix elements and the number of centroid elements should agree. "
			"There should be d(d+1)/2 correlation values and d centroid values.");

		autoCorrelation me = Correlation_create (dimension);

		// Construct the full correlation matrix from the upper-diagonal elements

		integer rowNumber = 1;
		for (integer inum = 1; inum <= numberOfCorrelations; inum ++) {
			integer nmissing = (rowNumber - 1) * rowNumber / 2;
			integer inumc = inum + nmissing;
			rowNumber = (inumc - 1) / dimension + 1;
			integer icol = ( (inumc - 1) % dimension) + 1;
			my data [rowNumber] [icol] = my data [icol] [rowNumber] = correlations [inum];
			if (icol == dimension) {
				rowNumber ++;
			}
		}

		// Check if a valid correlations, first check variances then covariances

		for (integer irow = 1; irow <= dimension; irow ++) {
			Melder_require (my data [irow] [irow] == 1.0, U"The diagonal matrix elements should all equal 1.0.");
		}
		for (integer irow = 1; irow <= dimension; irow ++) {
			for (integer icol = irow + 1; icol <= dimension; icol ++) {
				if (fabs (my data [irow] [icol]) > 1) {
					integer nmissing = (irow - 1) * irow / 2;
					integer inum = (irow - 1) * dimension + icol - nmissing;
					Melder_throw (U"The correlation in cell [", irow, U",", icol, U"], i.e. input item ", inum, U" should not exceed 1.0.");
				}
			}
		}
		for (integer inum = 1; inum <= dimension; inum ++) {
			my centroid [inum] = centroids [inum];
		}
		my numberOfObservations = numberOfObservations;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Simple Correlation not created.");
	}
}

autoCorrelation Correlation_create (integer dimension) {
	try {
		autoCorrelation me = Thing_new (Correlation);
		SSCP_init (me.get(), dimension, dimension);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Correlation not created.");
	}
}

autoCovariance SSCP_to_Covariance (SSCP me, integer numberOfConstraints) {
	try {
		Melder_assert (numberOfConstraints >= 0);
		autoCovariance thee = Thing_new (Covariance);
		my structSSCP :: v_copy (thee.get());

		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			for (integer icol = irow; icol <= my numberOfColumns; icol ++) {   // a covariance matrix is symmetric
				thy data [icol] [irow] = thy data [irow] [icol] /= my numberOfObservations - numberOfConstraints;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"; Covariance not created.");
	}
}

autoSSCP Covariance_to_SSCP (Covariance me) {
	try {
		autoSSCP thee = Thing_new (SSCP);
		my structSSCP :: v_copy (thee.get());
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			for (integer icol = irow; icol <= my numberOfColumns; icol ++) {
				thy data [icol] [irow] = thy data [irow] [icol] *= my numberOfObservations - 1;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": SSCP not created.");
	}
}

autoCorrelation SSCP_to_Correlation (SSCP me) {
	try {
		autoCorrelation thee = Thing_new (Correlation);
		my structSSCP :: v_copy (thee.get());
		for (integer i = 1; i <= my numberOfRows; i ++) {
			for (integer j = i; j <= my numberOfColumns; j ++) {
				thy data [j] [i] = thy data [i] [j] /= sqrt (my data [i] [i] * my data [j] [j]);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Correlation not created.");
	}
}

double SSCP_getLnDeterminant (SSCP me) {
	try {
		return NUMdeterminant_cholesky (my data, my numberOfRows);
	} catch (MelderError) {
		return undefined;
	}
}

static autoCovariance Covariances_pool (Covariance me, Covariance thee) {
	try {
		Melder_require (my numberOfRows == thy numberOfRows && my numberOfColumns == thy numberOfColumns,
			U"Matrices should have equal dimensions.");
		autoSSCPList sscps = SSCPList_create ();
		autoSSCP sscp1 = Covariance_to_SSCP (me);
		sscps -> addItem_move (sscp1.move());
		autoSSCP sscp2 = Covariance_to_SSCP (thee);
		sscps -> addItem_move (sscp2.move());
		autoSSCP pool = SSCPList_to_SSCP_pool (sscps.get());
		autoCovariance him = SSCP_to_Covariance (pool.get(), 2);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U"not pooled.");
	}
}

static double **productOfSquareMatrices (double **s1, double **s2, integer n) {
	autoNUMmatrix<double> r (1, n, 1, n);
	for (integer i = 1; i <= n; i ++) {
		for (integer j = 1; j <= n; j ++) {
			double sum = 0;
			for (integer k = 1; k <= n; k ++) {
				sum += s1 [i] [k] * s2 [k] [j];
			}
			r [i] [j] = sum;
		}
	}
	return r.transfer();
}

static double traceOfSquaredMatrixProduct (double **s1, double **s2, integer n) {
	// tr ((s1*s2)^2), s1, s2 are symmetric
	autoNUMmatrix<double> m (productOfSquareMatrices (s1, s2, n), 1, 1);
	double trace2 = NUMtrace2 (m.peek(), m.peek(), n);
	return trace2;
}

double Covariance_getProbabilityAtPosition_string (Covariance me, char32 *vector) {
	autoNUMvector<double> v (1, my numberOfColumns);
	integer i = 0;
	for (char32 *token = Melder_firstToken (vector); token != nullptr; token = Melder_nextToken ()) {
		v [++ i] = Melder_atof (token);
		if (i == my numberOfColumns) {
			break;
		}
	}
	double p = Covariance_getProbabilityAtPosition (me, v.peek());
	return p;
}

double Covariance_getProbabilityAtPosition (Covariance me, double x []) {
	if (my lowerCholesky == 0) {
		SSCP_expandLowerCholesky (me);
	}
	double ln2pid = my numberOfColumns * log (NUM2pi);
	double dsq = NUMmahalanobisDistance_chi (my lowerCholesky, x, my centroid, my numberOfRows, my numberOfColumns);
	double lnN = - 0.5 * (ln2pid + my lnd + dsq);
	double p =  exp (lnN);
	return p;
}

double Covariance_getMarginalProbabilityAtPosition (Covariance me, double vector [], double x) {
	double mu, stdev;
	Covariance_getMarginalDensityParameters (me, vector, &mu, &stdev);
	double dx = (x - mu) / stdev;
	double p = (NUM1_sqrt2pi / stdev) * exp (- 0.5 * dx * dx);
	return p;
}

/* Precondition ||v|| = 1 */
void Covariance_getMarginalDensityParameters (Covariance me, double v [], double *p_mu, double *p_stdev) {
	if (p_mu) {
		double mu = 0.0;
		for (integer m = 1; m <= my numberOfColumns; m ++) {
			mu += v [m] * my centroid [m];
		}
		*p_mu = mu;
	}
	if (p_stdev) {
		double stdev = 0;
		if (my numberOfRows == 1) { // 1xn diagonal matrix
			for (integer m = 1; m <= my numberOfColumns; m ++) {
				stdev += v [m] * my data [1] [m] * v [m];
			}
		} else {
			for (integer k = 1; k <= my numberOfRows; k ++) {
				for (integer m = 1; m <= my numberOfColumns; m ++) {
					stdev += v [k] * my data [k] [m] * v [m];
				}
			}
		}
		*p_stdev = sqrt (stdev);
	}
}

double Covariances_getMultivariateCentroidDifference (Covariance me, Covariance thee, int equalCovariances, double *p_prob, double *p_fisher, double *p_df1, double *p_df2) {
	integer p = my numberOfRows, N = Melder_ifloor (my numberOfObservations + thy numberOfObservations);
	integer N1 = Melder_ifloor (my numberOfObservations), n1 = N1 - 1;
	integer N2 = Melder_ifloor (thy numberOfObservations), n2 = N2 - 1;

	double dif = undefined, fisher = undefined;
	double df1 = p, df2 = N - p - 1;
	
	Melder_require (df2 >= 1.0, U"Not enough observations (", N, U") for this test.");
	Melder_require (p >= N1 && p >= N2, U"The number of observations should be larger than the number of variables.");

	dif = 0;
	for (integer i = 1; i <= p; i ++) {
		double dist = my centroid [i] - thy centroid [i];
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
		fisher = hotelling_tsq * df2 / ( (N - 2) * df1);
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

		for (integer i = 1; i <= p; i ++) {
			for (integer j = 1; j <= p; j ++) {
				s1 [i] [j] = my data [i] [j] / my numberOfObservations;
				s2 [i] [j] = thy data [i] [j] / thy numberOfObservations;
				s [i] [j] = s1 [i] [j] + s2 [i] [j];
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
		df2 = nu - p + 1;
		fisher =  hotelling_tsq * (nu - p + 1) / (nu * p);
	}

	if (p_prob) {
		*p_prob = NUMfisherQ (fisher, df1, df2);
	}
	if (p_fisher) {
		*p_fisher = fisher;
	}
	if (p_df1) {
		*p_df1 = df1;
	}
	if (p_df2) {
		*p_df2 = df2;
	}
	return dif;
}

/* Schott 2001 */
void Covariances_equality (CovarianceList me, int method, double *p_prob, double *p_chisq, double *p_df) {
	try {

		integer numberOfMatrices = my size;
		double chisq = undefined, df = undefined;
		Melder_require (numberOfMatrices > 1, U"We need at least two matrices");

		autoCovariance pool = CovarianceList_to_Covariance_pool (me); 
		double ns = pool -> numberOfObservations - my size;
		integer p = pool -> numberOfColumns;
		
		if (method == 1) {
			/* Bartlett (see Morrison page 297)
				The hypothesis H0 : Sigma [1] = .... = Sigma [k] of the equality of the covariance matrices of k p-dimensional
				multinormal populations can be tested against the alternative by a modified generalized likelihood-ratio statistic.
				Let S [i] be the unbiased estimate of Sigma [i] based on n [i] degrees of freedom, where n [i] = N [i]-1 for 
				the usual case of a random sample of N [i] observation vectors from the i-th population. When H0 is true
					S = 1/(sum(i=1..k, n [i])) sum(i=1..k, n [i]*S [i])
				is the pooled estimate of the common covariance matrix. The test statistic is
					M = sum(i=1..k,n [i])*ln|S| - sum(i=1..k, n [i]*ln|S [i]|).
				Box (1949), "A general distribution theory for a class of likelihood criteria", 
				Biomerika, vol 36, pp. 317-346. has shown that if the scale factor
				C^(-1) = 1 - (2p^2+3p-1)/(6(p+1)(k-1)) * (sum(i=1..k, 1/n [i]) - 1 / sum(i=1..k, n [i])) is introduced,
				the quatity M/C is approximately distributed as a chi-squared variate with (k-1)p(p+1)/2 degrees of freedom 
				as the n [i] become large.
				It is well known that this likelihood ratio test is very sensitive to violations of the normality assumption, 
				and so other more robust procedures have been proposed.
			 */
			double lnd;
			try {
				lnd = NUMdeterminant_cholesky (pool -> data, p);
			} catch (MelderError) {
				Melder_throw (U"Pooled covariance matrix is singular.");
			}

			double nsi = 0.0, m = ns * lnd; // First part of eq (3) page 297

			for (integer i = 1; i <= numberOfMatrices; i ++) {
				Covariance ci = my at [i];
				try {
					lnd = NUMdeterminant_cholesky (ci -> data, p);
				} catch (MelderError) {
					Melder_throw (U"Covariance matrix ", i, U" is singular.");
				}
				nsi += 1.0 / (ci -> numberOfObservations - 1);
				m -= (ci -> numberOfObservations - 1) * lnd;  // Last part of eq (3) page 297
			}

			/* Eq (4) page 297 */
			double c1 = 1.0 - (2.0 * p * p + 3.0 * p - 1.0) / (6.0 * (p + 1) * (numberOfMatrices - 1)) * (nsi - 1.0 / ns);

			df = (numberOfMatrices - 1.0) * p * (p + 1) / 2.0;
			chisq = m * c1;
		} else if (method == 2) { // Schott (2001) Wald 1
			// T1 = sum(i=1..k, n [i]/n *tr((S [i]*S^-1)^2)- sum(i=1..k, sum(j=1..k, (n [i]/n)*(n [j]/n) *tr(S [i]*S^-1*S [j]*sS^-1))) =
			//	sum(i=1..k, (ni/n -(ni/n)^2) tr((si*s^-1)^2)
			//	- 2 * sum (i=1..k, sum(j=1..i-1, (ni/n)*(nj/n) *tr(si*s^-1*sj*s^-1)))

			double trace = 0;
			NUMlowerCholeskyInverse (pool -> data, p, nullptr);
			autoNUMmatrix<double> si (NUMinverseFromLowerCholesky (pool -> data, p), 1, 1);
			for (integer i = 1; i <= numberOfMatrices; i ++) {
				Covariance ci = my at [i];
				double ni = ci -> numberOfObservations - 1;
				autoNUMmatrix<double> s1 (productOfSquareMatrices (ci -> data, si.peek(), p), 1, 1);
				double trace_ii = NUMtrace2 (s1.peek(), s1.peek(), p);
				trace += (ni / ns) * (1 - (ni / ns)) * trace_ii;
				for (integer j = i + 1; j <= numberOfMatrices; j ++) {
					Covariance cj = my at [j];
					double nj = cj -> numberOfObservations - 1;
					autoNUMmatrix<double> s2 (productOfSquareMatrices (cj -> data, si.peek(), p), 1, 1);
					double trace_ij = NUMtrace2 (s1.peek(), s2.peek(), p);
					trace -= 2.0 * (ni / ns) * (nj / ns) * trace_ij;
				}
			}
			df = (numberOfMatrices - 1) * p * (p + 1) / 2.0;
			chisq = (ns / 2.0) * trace;
		} else {
			return;
		}
		if (p_prob) {
			*p_prob = NUMchiSquareQ (chisq, df);
		}
		if (p_df) {
			*p_df = df;
		}
		if (p_chisq) {
			*p_chisq = chisq;
		}
	} catch (MelderError) {
		Melder_throw (U"Equality coud not be tested.");
	}
}

void Covariance_difference (Covariance me, Covariance thee, double *p_prob, double *p_chisq, double *p_df) {
	integer p = my numberOfRows;
	integer numberOfObservations = Melder_ifloor (my numberOfObservations);
	double  ln_me, ln_thee;
	double chisq = undefined, df = undefined;
	Melder_require (my numberOfRows == thy numberOfRows, U"Matrices should have equal dimensions.");

	if (my numberOfObservations != thy numberOfObservations) {
		numberOfObservations = Melder_ifloor (my numberOfObservations > thy numberOfObservations ?
		                        thy numberOfObservations : my numberOfObservations) - 1;
		Melder_warning (U"Covariance_difference: number of observations of matrices do not agree.\n"
		                U" The minimum  size (", numberOfObservations, U") of the two is used.");
	}
	Melder_require (numberOfObservations > 1, U"Number of observations too small.");

	autoNUMmatrix<double> linv (NUMmatrix_copy (thy data, 1, p, 1, p), 1, 1);
	NUMlowerCholeskyInverse (linv.peek(), p, & ln_thee);
	ln_me = NUMdeterminant_cholesky (my data, p);

	/*
		We need trace (A B^-1). We have A and the inverse L^(-1) of the
		cholesky decomposition L^T L of B in the lower triangle + diagonal.
		Always: tr (A B) = tr (B A)
		tr (A B^-1) = tr (A (L L^T)^-1) = tr (A L^-1 (L^T)^-1)
		trace = sum(i=1..p, j=1..p, l=max(i,j)..p, A [i] [j]Lm [l] [j]Lm [l] [i],
		where Lm = L^(-1)
	*/

	double trace = 0.0;
	for (integer i = 1; i <= p; i ++) {
		for (integer j = 1; j <= p; j ++) {
			integer lp = MAX (j, i);
			for (integer l = lp; l <= p; l ++) {
				trace += my data [i] [j] * linv [l] [j] * linv [l] [i];
			}
		}
	}

	double l = (numberOfObservations - 1) * fabs (ln_thee - ln_me + trace - p);
	chisq = l * fabs (1.0 - (2.0 * p + 1.0 - 2.0 / (p + 1)) / (numberOfObservations - 1) / 6.0);
	df = p * (p + 1) / 2.0;
	if (p_prob) {
		*p_prob = NUMchiSquareQ (chisq, df);
	}
	if (p_chisq) {
		*p_chisq = chisq;
	}
	if (p_df) {
		*p_df = df;
	}
}

static void checkOneIndex (TableOfReal me, integer index) {
	Melder_require (index > 0 && index <= my numberOfColumns, U"Index should be in interval [1, ", my numberOfColumns, U"].");
}

static void checkTwoIndices (TableOfReal me, integer index1, integer index2) {
	Melder_require (index1 > 0 && index1 <= my numberOfColumns && index2 > 0 && index2 <= my numberOfColumns,
		U"Index should be in interval [1, ", my numberOfColumns, U"].");
	Melder_require (index1 != index2,U"Indices should be different.");
}

void Covariance_getSignificanceOfOneMean (Covariance me, integer index, double mu, double *p_prob, double *p_t, double *p_df) {
	double var = my data [index] [index];
	double prob = undefined, t = undefined, df = my numberOfObservations - 1.0;

	checkOneIndex (me, index);

	if (var > 0.0) {
		t = (my centroid [index] - mu) / sqrt (var / my numberOfObservations);
		if (p_prob) {
			prob = 2.0 * NUMstudentQ (fabs (t), df);
		}
	}
	if (p_prob) {
		*p_prob = prob;
	}
	if (p_t) {
		*p_t = t;
	}
	if (p_df) {
		*p_df = df;
	}
}

void Covariance_getSignificanceOfMeansDifference (Covariance me, integer index1, integer index2, double mu, int paired, int equalVariances, double *p_prob, double *p_t, double *p_df) {
	integer n = Melder_ifloor (my numberOfObservations);

	double prob = undefined, t = undefined;
	double df = 2.0 * (n - 1);

	checkTwoIndices (me, index1, index2);

	double var1 = my data [index1] [index1];
	double var2 = my data [index2] [index2];

	double var_pooled = var1 + var2;
	if (var_pooled == 0.0) {
		Melder_warning (U"The pooled variance turned out to be zero. Check your data.");
		goto end;
	}
	if (paired) {
		var_pooled -= 2.0 * my data [index1] [index2];
		df /= 2.0;
	}

	if (var_pooled == 0.0) {
		Melder_warning (U"The pooled variance with the paired correction turned out to be zero. ");
		prob = 0.0;
		goto end;
	}

	t = (my centroid [index1] - my centroid [index2] - mu) / sqrt (var_pooled / n);

	/*
		Return two sided probabilty.
	*/

	if (equalVariances) {
		prob = 2.0 * NUMstudentQ (fabs (t), df);
	} else {
		df = (1.0 + 2.0 * var1 * var2 / (var1 * var1 + var2 * var2)) * (n - 1);
		prob = NUMincompleteBeta (df / 2.0, 0.5, df / (df + t * t));
	}
end:
	if (p_prob) {
		*p_prob = prob;
	}
	if (p_t) {
		*p_t = t;
	}
	if (p_df) {
		*p_df = df;
	}
}

void Covariance_getSignificanceOfOneVariance (Covariance me, integer index, double sigmasq, double *p_prob, double *p_chisq, double *p_df) {
	double var = my data [index] [index];
	double prob = undefined, chisq = undefined;
	double df = my numberOfObservations - 1.0;

	checkOneIndex (me, index);

	if (var > 0.0) {
		chisq = df;
		if (sigmasq > 0.0) {
			chisq = df * var / sigmasq;
		}
		if (p_prob) {
			prob = NUMchiSquareQ (chisq, df);
		}
	}
	if (p_prob) {
		*p_prob = prob;
	}
	if (p_chisq) {
		*p_chisq = chisq;
	}
	if (p_df) {
		*p_df = df;
	}
}

void Covariance_getSignificanceOfVariancesRatio (Covariance me, integer index1, integer index2, double ratio, double *p_prob, double *p_f, double *p_df) {
	double df = my numberOfObservations - 1.0, prob = undefined, f = undefined;
	checkTwoIndices (me, index1, index2);

	double var1 = my data [index1] [index1];
	double var2 = my data [index2] [index2];

	if (var1 > 0.0 && var2 > 0.0) {
		double ratio2 = (var1 / var2) / ratio;
		f = ratio2;
		if (var2 > var1) {
			ratio2 = (var2 / var1) * ratio;
		}
		if (p_prob) {
			prob = 2.0 * NUMfisherQ (ratio2, df, df);
			if (prob > 1.0) {
				prob = 2.0 - prob;
			}
		}
	}
	if (p_prob) {
		*p_prob = prob;
	}
	if (p_df) {
		*p_df = df;
	}
	if (p_f) {
		*p_f = f;
	}
}

autoTableOfReal Correlation_confidenceIntervals (Correlation me, double confidenceLevel, integer numberOfTests, int method) {
	try {
		integer m_bonferroni = my numberOfRows * (my numberOfRows - 1) / 2;
		Melder_require (confidenceLevel > 0 && confidenceLevel <= 1.0, U"Confidence level should be in interval (0-1).");

		Melder_require (my numberOfObservations > 4, U"The number of observations should be greater than 4.");
		Melder_require (numberOfTests >= 0, U"The \"number of tests\" should not be less than zero.");

		if (numberOfTests == 0) {
			numberOfTests = m_bonferroni;
		}

		if (numberOfTests > m_bonferroni) {
			Melder_warning (U"The \"number of tests\" should not exceed the number of elements in the Correlation object.");
		}

		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfRows);

		TableOfReal_copyLabels (me, thee.get(), 1, 1);


		// Obtain large-sample conservative multiple tests and intervals by the
		// Bonferroni inequality and the Fisher z transformation.
		// Put upper value of confidence intervals in upper part and lower
		// values of confidence intervals in lower part of resulting table.


		double z = NUMinvGaussQ ( (1 - confidenceLevel) / (2.0 * numberOfTests));
		double zf = z / sqrt (my numberOfObservations - 3.0);
		double  two_n = 2.0 * my numberOfObservations;

		for (integer i = 1; i <= my numberOfRows; i ++) {
			for (integer j = i + 1; j <= my numberOfRows; j ++) {
				double rij = my data [i] [j], rmin, rmax;
				if (method == 2) {
					// Fisher's approximation

					double zij = 0.5 * log ( (1 + rij) / (1 - rij));
					rmax = tanh (zij + zf);
					rmin = tanh (zij - zf);
				} else if (method == 1) {
					// Ruben's approximation

					double rs = rij / sqrt (1.0 - rij * rij);
					double a = two_n - 3.0 - z * z;
					double b = rs * sqrt ( (two_n - 3.0) * (two_n - 5.0));
					double c = (a - 2.0) * rs * rs - 2.0 * z * z;

					// Solve:  a y^2 - 2b y + c = 0
					// q = -0.5((-2b) + sgn(-2b) sqrt((-2b)^2 - 4ac))
					// y1 = q/a; y2 = c/q;

					double q, d = sqrt (b * b - a * c);

					if (b > 0) {
						d = - d;
					}
					q = b - d;
					rmin = q / a; rmin /= sqrt (1.0 + rmin * rmin);
					rmax = c / q; rmax /= sqrt (1.0 + rmax * rmax);
					if (rmin > rmax) {
						double t = rmin; rmin = rmax; rmax = t;
					}
				} else {
					rmax = rmin = 0;
				}
				thy data [i] [j] = rmax;
				thy data [j] [i] = rmin;
			}
			thy data [i] [i] = 1;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": confidence intervals not created.");
	}
}

void SSCP_getDiagonality_bartlett (SSCP me, integer numberOfContraints, double *chisq, double *prob, double *df) {
	autoCorrelation c = SSCP_to_Correlation (me);
	Correlation_testDiagonality_bartlett (c.get(), numberOfContraints, chisq, prob, df);
}

/* Morrison, page 118 */
void Correlation_testDiagonality_bartlett (Correlation me, integer numberOfContraints, double *p_chisq, double *p_prob, double *p_df) {
	integer p = my numberOfRows;
	double chisq = undefined, prob = undefined, df = p * (p -1) / 2.0;

	if (numberOfContraints <= 0) {
		numberOfContraints = 1;
	}
	if (numberOfContraints > my numberOfObservations) {
		Melder_warning (U"Correlation_testDiagonality_bartlett: number of constraints cannot exceed the number of observations.");
		return;
	}
	if (my numberOfObservations >= numberOfContraints) {
		double ln_determinant = NUMdeterminant_cholesky (my data, p);
		chisq = - ln_determinant * (my numberOfObservations - numberOfContraints - (2.0 * p + 5.0) / 6.0);
		if (p_prob) {
			prob = NUMchiSquareQ (chisq, df);
		}
	}
	if (p_chisq) {
		*p_chisq = chisq;
	}
	if (p_prob) {
		*p_prob = prob;
	}
	if (p_df) {
		*p_df = df;
	}
}

void SSCP_expand (SSCP me) {

	// A reduced matrix has my numberOfRows < my numberOfColumns.
	// After expansion:
	// my numberOfRows == my numberOfColumns
	// my storageNumberOfRows = my numberOfRows (before)
	// my data (after) = my expansion;
	// my expansion = my data (before)
	// No expansion for a standard matrix or if already expanded and data has not changed!

	if ((my expansionNumberOfRows == 0 && my numberOfRows == my numberOfColumns) ||
	        (my expansionNumberOfRows > 0 && ! my dataChanged)) {
		return;
	}
	if (my expansion == 0) {
		my expansion = NUMmatrix<double> (1, my numberOfColumns, 1, my numberOfColumns);
	}
	for (integer ir = 1; ir <= my numberOfColumns; ir ++) {
		for (integer ic = ir; ic <= my numberOfColumns; ic ++) {
			integer dij = labs (ir - ic);
			my expansion [ir] [ic] = my expansion [ic] [ir] = dij < my numberOfRows ? my data [dij + 1] [ic] : 0.0;
		}
	}
	// Now make 'my data' point to 'my expansion'
	double **tmp = my data; my data = my expansion; my expansion = tmp;
	my expansionNumberOfRows = my numberOfRows;
	my numberOfRows = my numberOfColumns;
	my dataChanged = 0;
}

void SSCP_unExpand (SSCP me) {
	if (my expansionNumberOfRows == 0) {
		return;
	}
	NUMmatrix_free (my data, 1, 1);
	my data = my expansion;
	my expansion = nullptr;
	my numberOfRows = my expansionNumberOfRows;
	my expansionNumberOfRows = 0;
	my dataChanged = 0;
}

void SSCP_expandLowerCholesky (SSCP me) {
	if (! my lowerCholesky) {
		my lowerCholesky = NUMmatrix<double> (1, my numberOfRows, 1, my numberOfColumns);
	}
	if (my numberOfRows == 1) {   // diagonal
		my lnd = 0.0;
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			my lowerCholesky [1] [j] = 1.0 / sqrt (my data [1] [j]);   // inverse is 1/stddev
			my lnd += log (my data [1] [j]);   // diagonal elmnt is variance
		}
	} else {
		for (integer i = 1; i <= my numberOfRows; i ++) {
			for (integer j = i; j <= my numberOfColumns; j ++) {
				my lowerCholesky [j] [i] = my lowerCholesky [i] [j] = my data [i] [j];
			}
		}
		try {
			NUMlowerCholeskyInverse (my lowerCholesky, my numberOfColumns, & (my lnd));
		} catch (MelderError) {
			// singular matrix: arrange a diagonal only inverse.
			my lnd = 0.0;
			for (integer i = 1; i <= my numberOfRows; i ++) {
				for (integer j = i; j <= my numberOfColumns; j ++) {
					my lowerCholesky [i] [j] = my lowerCholesky [j] [i] = ( i == j ? 1.0 / sqrt (my data [i] [i]) : 0.0 );
				}
				my lnd += log (my data [i] [i]);
			}
			my lnd *= 2.0;
		}
	}
}

void SSCP_unExpandLowerCholesky (SSCP me) {
	NUMmatrix_free (my lowerCholesky, 1, 1);
	my lnd = 0.0;
}

void SSCP_expandPCA (SSCP me) {
	if (my pca) {
		my pca.reset();
	}
	my pca = SSCP_to_PCA (me);
}

void SSCP_unExpandPCA (SSCP me) {
	my pca.reset();
}


#undef MAX
#undef MIN

/* End of file SSCP.c */
