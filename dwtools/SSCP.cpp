/* SSCP.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
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

#include "enums_getText.h"
#include "SSCP_enums.h"
#include "enums_getValue.h"
#include "SSCP_enums.h"

#define TOVEC(x) (&(x) - 1)

Thing_implement (SSCP, TableOfReal, 0);
Thing_implement (SSCPList, TableOfRealList, 0);
Thing_implement (Covariance, SSCP, 0);
Thing_implement (CovarianceList, SSCPList, 0);
Thing_implement (Correlation, SSCP, 0);

void structSSCP :: v_info () {
	structTableOfReal :: v_info ();
	double zmin = NUMmin (our data.all());
	double zmax = NUMmax (our data.all());
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
		if (n - my numberOfColumns < 1) return -1.0;
		// D.E. Johnson (1998), Applied Multivariate methods, page 410
		double f = NUMinvFisherQ (1.0 - scale, my numberOfColumns, n - my numberOfColumns);
		scale = 2.0 * sqrt (f * my numberOfColumns * (n - 1) / ( ((double) n) * (n - my numberOfColumns)));
	} else {
		// very ugly, temporary hack
		scale *= 2.0 / (scale < 0.0 ? -1.0 : sqrt (n - 1));
	}
	return scale;
}

static void getEllipseBoundingBoxCoordinates (SSCP me, double scale, bool confidence, double *xmin, double *xmax, double *ymin, double *ymax) {
	double a, b, cs, width, height;
	NUMeigencmp22 (my data [1] [1], my data [1] [2], my data [2] [2], & a, & b, & cs, nullptr);
	NUMgetEllipseBoundingBox (sqrt (a), sqrt (b), cs, & width, & height);

	double lscale = SSCP_getEllipseScalefactor (me, scale, confidence);
	*xmin = my centroid [1] - lscale * width / 2.0;
	*xmax = *xmin + lscale * width;
	*ymin = my centroid [2] - lscale * height / 2.0;
	*ymax = *ymin + lscale * height;
}

void SSCPList_getEllipsesBoundingBoxCoordinates (SSCPList me, double scale, bool confidence, double *out_xmin, double *out_xmax, double *out_ymin, double *out_ymax) {
	double xmin = 1e308, xmax = -xmin, ymin = xmin, ymax = -ymin;

	for (integer i = 1; i <= my size; i ++) {
		SSCP s = my at [i];
		double xmn, xmx, ymn, ymx;
		getEllipseBoundingBoxCoordinates (s, scale, confidence, & xmn, & xmx, & ymn, & ymx);
		if (xmn < xmin) xmin = xmn;
		if (xmx > xmax) xmax = xmx;
		if (ymn < ymin) ymin = ymn;
		if (ymx > ymax) ymax = ymx;
	}
	if (out_xmin) *out_xmin = xmin;
	if (out_xmax) *out_xmax = xmax;
	if (out_ymin) *out_ymin = ymin;
	if (out_ymax) *out_ymax = ymax;
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

	TableOfReal_setColumnLabel (thee.get(), 1, my columnLabels [d1].get());
	TableOfReal_setColumnLabel (thee.get(), 2, my columnLabels [d2].get());
	TableOfReal_setRowLabel (thee.get(), 1, my columnLabels [d1].get());
	TableOfReal_setRowLabel (thee.get(), 2, my columnLabels [d2].get());
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

void SSCP_drawTwoDimensionalEllipse_inside (SSCP me, Graphics g, double scale, conststring32 label, double fontSize) {
	try {
		integer nsteps = 100;
		autoVEC x = newVECraw (nsteps + 1);
		autoVEC y = newVECraw (nsteps + 1);
		// Get principal axes and orientation for the ellipse by performing the
		// eigen decomposition of a symmetric 2-by-2 matrix.
		// Principal axes are a and b with eigenvector/orientation (cs, sn).

		double a, b, cs, sn;
		NUMeigencmp22 (my data [1] [1], my data [1] [2], my data [2] [2], & a, & b, & cs, & sn);
		// 1. Take sqrt to get units of 'std_dev'

		a = scale * sqrt (a) / 2.0;
		b = scale * sqrt (b) / 2.0;
		x [nsteps + 1] = x [1] = my centroid [1] + cs * a;
		y [nsteps + 1] = y [1] = my centroid [2] + sn * a;
		double angle = 0.0;
		double angle_inc = NUM2pi / nsteps;
		for (integer i = 2; i <= nsteps; i ++, angle += angle_inc) {
			double xc = a * cos (angle);
			double yc = b * sin (angle);
			double xt = xc * cs - yc * sn;
			y [i] = my centroid [2] + xc * sn + yc * cs;
			x [i] = my centroid [1] + xt;
		}
		Graphics_polyline (g, nsteps + 1, & x [1], & y [1]);
		if (label && fontSize > 0) {
			const double oldFontSize = Graphics_inqFontSize (g);
			Graphics_setFontSize (g, fontSize);
			Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
			Graphics_text (g, my centroid [1], my centroid [2], label);
			Graphics_setFontSize (g, oldFontSize);
		}
	} catch (MelderError) {
		Melder_clearError ();
	}
}

autoSSCP SSCP_toTwoDimensions (SSCP me, constVECVU const& v1, constVECVU const& v2) {
	try {
		Melder_assert (v1.size == v2.size && v1.size == my numberOfColumns);
		autoSSCP thee = SSCP_create (2);

		// Projection P of S on v1 and v2 (given matrix V' with 2 rows) is P = V'SV
		// P [i] [j] = sum(k) sum(m) V' [i] [k]*S [k] [m]*V [m] [j] = V' [i] [k]*S [k] [m]*V' [j] [m]

		// For the new centroids cnew [i] = sum(m) V' [i] [m]*c [m]

		if (my numberOfRows == 1) { // 1xn diagonal matrix
			for (integer k = 1; k <= my numberOfColumns; k ++) {
				thy data [1] [1] += v1 [k] * my data [1] [k] * v1 [k];
				thy data [1] [2] += v1 [k] * my data [1] [k] * v2 [k];
				thy data [2] [2] += v2 [k] * my data [1] [k] * v2 [k];
			}
			thy data [2] [1] = thy data [1] [2];
		} else {
			thy data [1] [1] = NUMmul (v1, my data.get(), v1);
			thy data [2] [2] = NUMmul (v2, my data.get(), v2);
			thy data [1] [2] = thy data [2] [1] = NUMmul (v1, my data.get(), v2);
		}

		thy centroid [1] = NUMinner (v1, my centroid.get());
		thy centroid [2] = NUMinner (v2, my centroid.get());

		thy numberOfObservations = SSCP_getNumberOfObservations (me);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot extract two dimensions.");
	}
}

void SSCP_init (SSCP me, integer dimension, kSSCPstorage storage) {
	integer numberOfRows = storage == kSSCPstorage::Diagonal ? 1 : dimension;
	TableOfReal_init (me, numberOfRows, dimension);
	my centroid = newVECzero (dimension);
}

autoSSCP SSCP_create (integer dimension) {
	try {
		autoSSCP me = Thing_new (SSCP);
		SSCP_init (me.get(), dimension, kSSCPstorage::Complete);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SSCP not created.");
	}
}

void SSCP_reset (SSCP me) {
	my data.all() <<= 0.0;
	my centroid.all() <<= 0.0;
	my numberOfObservations = 0;
}

double SSCP_getConcentrationEllipseArea (SSCP me, double scale, bool confidence, integer d1, integer d2) {

	Melder_require (d1 > 0 && d1 <= my numberOfRows && d2 > 0 && d2 <= my numberOfRows && d1 != d2,
		U"Incorrect axes.");

	autoSSCP thee = _SSCP_extractTwoDimensions (me, d1, d2);
	scale = SSCP_getEllipseScalefactor (thee.get(), scale, confidence);
	Melder_require (scale > 0, U"The scale factor should be larger than zero.");

	double a, b;
	NUMeigencmp22 (thy data [1] [1], thy data [1] [2], thy data [2] [2], & a, & b, nullptr, nullptr);

	// 1. Take sqrt to get units of 'std_dev'

	a = scale * sqrt (a) / 2.0;
	b = scale * sqrt (b) / 2.0;
	return NUMpi * a * b;
}

double SSCP_getFractionVariation (SSCP me, integer from, integer to) {
	integer n = my numberOfRows;

	if (from < 1 || from > to || to > n)
		return undefined;

	double sum = 0.0, trace = 0.0;
	for (integer i = 1; i <= n; i ++) {
		trace += my numberOfRows == 1 ? my data [1] [i] : my data [i] [i];
		if (i >= from && i <= to)
			sum += my numberOfRows == 1 ? my data [1] [i] : my data [i] [i];
	}
	return trace > 0.0 ? sum / trace : undefined;
}

void SSCP_drawConcentrationEllipse (SSCP me, Graphics g, double scale, int confidence,
	integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, int garnish)
{
	Melder_require (d1 > 0 && d1 <= my numberOfRows && d2 > 0 && d2 <= my numberOfRows && d1 != d2, U"Incorrect axes.");

	autoSSCP thee = _SSCP_extractTwoDimensions (me, d1, d2);

	double xmn, xmx, ymn, ymx;
	getEllipseBoundingBoxCoordinates (thee.get(), scale, confidence, & xmn, & xmx, & ymn, & ymx);

	if (xmax == xmin) {
		xmin = xmn;
		xmax = xmx;
	}

	if (ymax == ymin) {
		ymin = ymn;
		ymax = ymx;
	}

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);

	scale = SSCP_getEllipseScalefactor (thee.get(), scale, confidence);
	Melder_require (scale > 0, U"The scale factor should be larger than zero.");
	
	SSCP_drawTwoDimensionalEllipse_inside (thee.get(), g, scale, nullptr, 0);

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
	return my numberOfRows == 1 ? NUMsum (my data.row (1)) : NUMtrace (my data.get());
}

double SSCP_getCumulativeContributionOfComponents (SSCP me, integer from, integer to) {
	double ccoc = undefined;
	if (to == 0) to = my numberOfRows;
	if (from > 0 && to <= my numberOfRows && from <= to) {
		double sum = SSCP_getTotalVariance (me);
		if (sum > 0.0) {
			double partial = 0.0;
			for (integer i = from; i <= to; i ++)
				partial += my numberOfRows == 1 ? my data [1] [i] : my data [i] [i];
			ccoc = partial / sum;
		}
	}
	return ccoc;
}

/* For nxn matrix only ! */
void Covariance_PCA_generateOneVector_inline (Covariance me, PCA thee, VECVU vec, VEC buf) {
	// Generate the multi-normal vector elements N(0,sigma)
	Melder_require (thy dimension == my numberOfRows, 
		U"The PCA must have the same dimension as the Covariance.");
	Melder_require (vec.size == buf.size && my numberOfColumns == buf.size, 
		U"The vectors and the PCA must have the same dimension.");
	for (integer j = 1; j <= my numberOfColumns; j ++)
		buf [j] = NUMrandomGauss (0.0, sqrt (thy eigenvalues [j]));

	// Rotate back
	
	VECmul (vec, buf, thy eigenvectors.get());
	vec  +=  my centroid.get();
}

autoTableOfReal Covariance_to_TableOfReal_randomSampling (Covariance me, integer numberOfData) {
	try {
		if (numberOfData <= 0)
			numberOfData = Melder_ifloor (my numberOfObservations);
		autoPCA pca = SSCP_to_PCA (me);
		autoTableOfReal thee = TableOfReal_create (numberOfData, my numberOfColumns);
		autoVEC buf = newVECraw (my numberOfColumns);
		for (integer i = 1; i <= numberOfData; i ++)
			Covariance_PCA_generateOneVector_inline (me, pca.get(), thy data.row (i), buf.get());
		thy columnLabels.all() <<= my columnLabels.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not random sampled.");
	}
}

autoSSCP TableOfReal_to_SSCP (TableOfReal me, integer rowb, integer rowe, integer colb, integer cole) {
	try {
		Melder_require (NUMdefined (my data.get()),
			U"All the table's elements should be defined.");
		fixAndCheckRowRange (& rowb, & rowe, my data.get(), 1);
		fixAndCheckColumnRange (& colb, & cole, my data.get(), 1);
		autoMAT part = newMATpart (my data.get(), rowb, rowe, colb, cole);
		if (part.nrow < part.ncol)
			Melder_warning (U"The selected number of rows (", part.nrow,
				U") is less than the selected number of columns (", part.ncol,
				U").\nThe SSCP will not have full dimensionality. This may be a problem in later analysis steps."
			);
		autoSSCP thee = SSCP_create (part.ncol);
		VECcolumnMeans (thy centroid.get(), part.get());
		part.all()  -=  thy centroid.all();
		SSCP_setNumberOfObservations (thee.get(), part.nrow);
		MATmtm (thy data.get(), part.get());   // sum of squares and cross products = T'T
		for (integer j = 1; j <= part.ncol; j ++) {
			conststring32 label = my columnLabels [colb - 1 + j].get();
			TableOfReal_setColumnLabel (thee.get(), j, label);
			TableOfReal_setRowLabel (thee.get(), j, label);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": SSCP not created.");
	}
}

autoSSCP TableOfReal_to_SSCP_rowWeights (TableOfReal me, integer rowb, integer rowe, integer colb, integer cole, integer weightColumnNumber) {
	try {
		Melder_require (NUMdefined (my data.get()),
			U"All the table's elements should be defined.");
		fixAndCheckRowRange (& rowb, & rowe, my data.get(), 1);
		fixAndCheckColumnRange (& colb, & cole, my data.get(), 1);
		if (weightColumnNumber != 0) 
			Melder_require (weightColumnNumber < colb || weightColumnNumber > cole, 
				U"The weight columns must be outside the selected block.");
		autoMAT part = newMATpart (my data.get(), rowb, rowe, colb, cole);
		if (part.nrow < part.ncol)
			Melder_warning (U"The selected number of data points (", part.nrow,
				U") is less than the selected number of variables (", part.ncol,
				U").\nThe SSCP will not have full dimensionality. This may be a problem in later analysis steps."
			);
		autoSSCP thee = SSCP_create (part.ncol);
		VECcolumnMeans (thy centroid.get(), part.get());
		part.all()  -=  thy centroid.all();
		SSCP_setNumberOfObservations (thee.get(), part.nrow);
		if (weightColumnNumber != 0) {
			autoVEC rowWeights = newVECcolumn (my data.horizontalBand (rowb, rowe), weightColumnNumber);
			MATmtm_weighRows (thy data.get(), part.get(), rowWeights.get());
		} else
			MATmtm (thy data.get(), part.get());   // sum of squares and cross products = T'T
		for (integer j = 1; j <= part.ncol; j ++) {
			conststring32 label = my columnLabels [colb - 1 + j].get();
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
		/*
			ppgb
			ik kom er niet achter of onderstaande de hele vector kopieert of niet;
			in elk geval zijn hier enkele asserts nodig
		*/
		autoVEC centroid = newVECcopy (my centroid.get());
		autoMAT covari = newMATcopy (my data.get());

		/*
			Mahalanobis distance calculation. S = L.L' -> S**-1 = L**-1' . L**-1
				(x-m)'S**-1 (x-m) = (x-m)'L**-1' . L**-1. (x-m) =
				(L**-1.(x-m))' . (L**-1.(x-m))

			Get inverse of covari in lower triangular part.
		*/

		MATlowerCholeskyInverse_inplace (covari.get(), nullptr);

		if (useTableCentroid)
			VECcolumnMeans (centroid.get(), thy data.get());
		for (integer k = 1; k <= thy numberOfRows; k ++) {
			his data [k] [1] = sqrt (NUMmahalanobisDistance (covari.get(), thy data.row (k), centroid.get()));
			if (thy rowLabels [k])
				TableOfReal_setRowLabel (him.get(), k, thy rowLabels [k].get());
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
		for (integer i = 1; i <= thy numberOfRows; i ++)
			if (low <= his data [i] [1] && his data [i] [1] < high)
				nsel ++;

		Melder_require (nsel > 0, U"Not enough data in quantile interval.");
		
		autoTableOfReal r = TableOfReal_create (nsel, thy numberOfColumns);
		r -> columnLabels.all() <<= thy columnLabels.all();

		integer k = 0;
		for (integer i = 1; i <= thy numberOfRows; i ++)
			if (low <= his data [i] [1] && his data [i] [1] < high)
				TableOfReal_copyOneRowWithLabel (thee, r.get(), i, ++ k);
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
		autoTableOfReal t = TableOfReal_rankColumns (me, 1, my numberOfColumns);
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
		conststring32 label = mew -> rowLabels [1].get();
		for (integer i = 2; i <= my numberOfRows; i ++) {
			integer numberOfRowsInCurrent = 0;
			conststring32 currentLabel = mew -> rowLabels [i].get();
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
				if (numberOfRowsInCurrent < my numberOfColumns)
					numberOfSingularMatrices ++;
				autoSSCP t = TableOfReal_to_SSCP (mew.get(), index, lastrow, 0, 0);
				if (! (label = mew -> rowLabels [index].get())) 
					label = U"?";
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
		Melder_assert (my data.ncol == my numberOfColumns);
		autoMAT mat;
		if (my numberOfRows == 1) {
			mat = newMATzero (my numberOfColumns, my numberOfColumns);
			mat.diagonal() <<= my data.row (1); // 1xn matrix -> nxn
		} else if (my data.nrow == my numberOfColumns && my data.ncol == my numberOfColumns)
			mat = newMATcopy (my data.get());
		else
			Melder_throw (me, U": the SSCP has the wrong dimensions.");
		autoPCA thee = PCA_create (my numberOfColumns, my numberOfColumns);
		Eigen_initFromSymmetricMatrix (thee.get(), mat.get());
		thy centroid.all() <<= my centroid.all();
		PCA_setNumberOfObservations (thee.get(), Melder_ifloor (my numberOfObservations));
		thy labels.all() <<= my columnLabels.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": PCA not created.");
	}
}

void SSCP_setValue (SSCP me, integer rowNumber, integer columnNumber, double value) {
	checkColumnNumber (columnNumber, my numberOfColumns);
	checkRowNumber (rowNumber, my numberOfRows);
	Melder_require (! (rowNumber == columnNumber && value <= 0.0), U"Diagonal element should always be a positive number.");
	
	if (my numberOfRows == 1) {   // diagonal
		Melder_require (rowNumber == columnNumber, U"Row and column number should be equal for a diagonal matrix.");
		my data [1] [rowNumber] = value;
	} else {
		Melder_require (! (rowNumber != columnNumber && (fabs (value) > my data [rowNumber] [rowNumber] || fabs (value) > my data [columnNumber] [columnNumber])),
			U"The off-diagonal values should not be larger than the diagonal values. Input diagonal elements first, or change this value.");
		my data [rowNumber] [columnNumber] = my data [columnNumber] [rowNumber] = value;
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

		integer m = my numberOfRows, nx = m - ny;
		bool xy_interchanged = ( nx < ny );
		integer yof = 0, xof = ny;
		if (xy_interchanged) {
			yof = ny; 
			xof = 0;
			nx = ny;
			ny = m - nx;
		}

		// Copy Syy and Sxx into upper part of syy and sxx matrices.

		autoMAT syy = newMATpart (my data.get(), yof, yof + ny - 1, yof, yof + ny - 1);
		autoMAT sxx = newMATpart (my data.get(), xof, xof + nx - 1, xof, xof + nx - 1);
		autoMAT syx = newMATpart (my data.get(), yof, yof + ny - 1, xof, xof + nx - 1);

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

		// Uxi = inverse(Ux)

		(void) NUMlapack_dtrti2 (& upper, & diag, & nx, & sxx [1] [1], & nx, & info);
		Melder_require (info == 0, U"Error in inverse for Sxx.");

		// Prepare Uxi' * Syx' = (Syx * Uxi)'

		autoMAT a = newMATmul (sxx.transpose(), syx.transpose());
		Melder_assert (a.nrow == nx && a.ncol == ny);

		autoGSVD gsvd = GSVD_create (a.get(), syy.get());
		autoMAT ri = newMATcopy (gsvd -> r.get());
		
		autoCCA thee = Thing_new (CCA);

		thy y = Eigen_create (gsvd -> numberOfColumns, gsvd -> numberOfColumns);
		thy x = Eigen_create (thy y -> numberOfEigenvalues, nx);

		// Get X=Q*R**-1

		(void) NUMlapack_dtrti2 (& upper, & diag, & gsvd -> numberOfColumns, &  ri [1] [1], & gsvd -> numberOfColumns, & info);
		Melder_require (info == 0, U"Error in inverse for R.");
		
		for (integer i = 1; i <= gsvd -> numberOfColumns; i ++) {
			double t = gsvd -> d1 [i] / gsvd -> d2 [i];
			thy y -> eigenvalues [i] = t * t;
			for (integer j = 1; j <= gsvd -> numberOfColumns; j ++) {
				longdouble sum = 0.0;
				for (integer k = 1; k <= j; k ++) {
					sum += gsvd -> q [i] [k] * ri [k] [j];
				}
				thy y -> eigenvectors [j] [i] = double (sum);
			}
		}

		MATnormalizeRows_inplace (thy y -> eigenvectors.get(), 2.0, 1.0);

		thy numberOfCoefficients = thy y -> numberOfEigenvalues;
		thy numberOfObservations = Melder_ifloor (my numberOfObservations);

		// x = Sxx**-1 * Syx' * y

		for (integer i = 1; i <= thy numberOfCoefficients; i ++) {
			constVEC evecy = thy y -> eigenvectors.row (i);
			VEC evecx = thy x -> eigenvectors.row (i);
			for (integer j = 1; j <= nx; j ++) {
				longdouble t = 0.0;
				for (integer k = j; k <= nx; k ++) {
					for (integer l = 1; l <= nx; l ++) {
						for (integer n = 1; n <= ny; n ++) {
							t += sxx [j] [k] * sxx [l] [k] * syx [n] [l] * evecy [n];
						}
					}
				}
				evecx [j] = double (t);
			}
		}

		MATnormalizeRows_inplace (thy x -> eigenvectors.get(), 2.0, 1.0);

		if (ny < nx) {
			autoEigen t = thy x.move();
			thy x = thy y.move();
			thy y = t.move();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": CCA not created.");
	}
}

/************ SSCPList ***********************************************/

autoSSCP SSCPList_to_SSCP_pool (SSCPList me) {
	try {
		autoSSCP pool = Data_copy (my at [1]);

		for (integer k = 2; k <= my size; k ++) {
			SSCP t = my at [k];
			Melder_require (t -> numberOfRows == pool -> numberOfRows, U"The dimension of item ", k, U" should agree.");
			
			pool -> numberOfObservations += t -> numberOfObservations;

			// Sum the sscp's and weigh the centroid.
			pool -> data.all()  +=  t -> data.all();
			pool -> centroid.all()  +=  t->centroid.all()  *  t -> numberOfObservations;
		}
		
		pool -> centroid.all()  *=  1.0 / pool -> numberOfObservations;
		
		return pool;
	} catch (MelderError) {
		Melder_throw (me, U": not pooled.");
	}
}

autoCovariance CovarianceList_to_Covariance_within (CovarianceList me) {
	try {
		autoCovariance thee = Data_copy (my at[1]);
		SSCP_reset (thee.get());
		for (integer i = 1; i <= my size; i ++) {
			Covariance covi = my at [i];
			Melder_require (thy numberOfColumns == covi -> numberOfColumns && thy numberOfRows == covi -> numberOfRows, 
				U"The dimensions of item ", i, U" does not conform.");
			thy data.all()  +=  covi -> data.all()  *  (covi -> numberOfObservations - 1.0);
			thy numberOfObservations += covi -> numberOfObservations;
		}
		thy data.all()  *=  1.0 / (thy numberOfObservations - 1.0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Covariance (within) created.");
	}
}

autoCovariance CovarianceList_to_Covariance_between (CovarianceList me) {
	try {
		autoCovariance thee = Data_copy (my at[1]);
		SSCP_reset (thee.get());

		//	First the new centroid,

		for (integer i = 1; i <= my size; i ++) {
			Covariance covi = my at [i];
			Melder_require (thy numberOfColumns == covi -> numberOfColumns && thy numberOfRows == covi -> numberOfRows, 
				U"The dimensions of item ", i, U" does not conform.");
			thy centroid.get()  +=  covi -> centroid.get()  *  covi -> numberOfObservations;
			thy numberOfObservations += covi -> numberOfObservations;
		}
		thy centroid.get()  *=  1.0 / thy numberOfObservations;
		
		autoVEC mean = newVECraw (thy numberOfColumns);
		autoMAT outer = newMATraw (thy numberOfColumns, thy numberOfColumns);
		for (integer i = 1; i <= my size; i ++) {
			Covariance covi = my at [i];
			mean.get() <<= covi -> centroid.get()  -  thy centroid.get();
			MATouter (outer.get(), mean.get(), mean.get());
			outer.get()  *=  covi -> numberOfObservations;
			if (thy numberOfRows == 1) 
				thy data.row(1)  +=  outer.diagonal();
			else
				thy data.get()  +=  outer.get(); // Y += aX
		}
		thy data.get()  *=  1.0 / (thy numberOfObservations - 1.0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Covariance (between) created.");
	}
}

autoCovariance CovarianceList_to_Covariance_pool (CovarianceList me) { // Morrison sec 3.5, page 100
	try {
		autoCovariance thee = Data_copy (my at [1]);
		SSCP_reset (thee.get());
		for (integer i = 1; i <= my size; i ++) {
			Covariance covi = my at [i];
			Melder_require (covi -> numberOfRows == thy numberOfRows, U"The dimension of item ", i, U" should agree.");

			thy numberOfObservations += covi -> numberOfObservations;

			// Sum the sscp's and weigh the centroid.
			for (integer k = 1; k <= thy numberOfRows; k ++) // catch 1xn
				thy data.row (k)  +=  covi -> data.row (k)  *  (covi -> numberOfObservations - 1.0);

			thy centroid.all()  +=  covi -> centroid.all()  *  covi -> numberOfObservations;
		}
		thy centroid.all()  *=  1.0 / thy numberOfObservations;
		thy data.all()  *=  1.0 / (thy numberOfObservations - my size);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not pooled.");
	}
}

void SSCPList_getHomegeneityOfCovariances_box (SSCPList me, double *out_prob, double *out_chisq, double *out_df) {
	double chisq = 0.0, df = undefined;

	autoSSCP pooled = SSCPList_to_SSCP_pool (me);
	integer p = pooled -> numberOfColumns;
	double ln_determinant, inv = 0.0, sum = 0.0, g = my size;
	for (integer i = 1; i <= g; i ++) {
		SSCP t = my at [i];
		double ni = t -> numberOfObservations - 1.0;
		ln_determinant = NUMdeterminant_fromSymmetricMatrix (t -> data.get());

		// Box-test is for covariance matrices -> scale determinant.

		ln_determinant -= p * log (ni);
		sum += ni;
		inv += 1.0 / ni;
		chisq -= ni * ln_determinant;
	}

	ln_determinant = NUMdeterminant_fromSymmetricMatrix (pooled -> data.get());
	ln_determinant -= p * log (pooled -> numberOfObservations - g);
	chisq += sum * ln_determinant;

	chisq *= 1.0 - (inv - 1.0 / sum) * (2.0 * p * p + 3.0 * p - 1.0) / (6.0 * (p + 1) * (g - 1.0));
	df = (g - 1.0) * p * (p + 1) / 2.0;
	
	if (out_prob) *out_prob = NUMchiSquareQ (chisq, df);
	if (out_chisq) *out_chisq = chisq;
	if (out_df) *out_df = df;
}

autoSSCPList SSCPList_toTwoDimensions (SSCPList me, constVECVU const& v1, constVECVU const& v2) {
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


void SSCPList_drawConcentrationEllipses (SSCPList me, Graphics g,
	double scale, bool confidence, conststring32 label,
	integer d1, integer d2, double xmin, double xmax, double ymin, double ymax,
	double fontSize, bool garnish)
{
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
		if (lscale < 0.0)
			continue;
		if (! label || Melder_cmp (label, Thing_getName (t)) == 0)
			SSCP_drawTwoDimensionalEllipse_inside (t, g, lscale, Thing_getName (t), fontSize);
	}

	Graphics_unsetInner (g);
	if (garnish) {
		t = my at [1];
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, t -> columnLabels [d2] ? t -> columnLabels [d2].get() : Melder_cat (U"Dimension ", d2));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textBottom (g, true, t -> columnLabels [d1] ? t -> columnLabels [d1].get() : Melder_cat (U"Dimension ", d1));
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
		thy data.row (1) <<= my centroid.all();
		thy columnLabels.all() <<= my columnLabels.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": centroid not extracted.");
	}
}

autoCovariance Covariance_create (integer dimension) {
	try {
		autoCovariance me = Thing_new (Covariance);
		SSCP_init (me.get(), dimension, kSSCPstorage::Complete);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Covariance not created.");
	}
}

autoCovariance Covariance_create_reduceStorage (integer dimension, kSSCPstorage storage) {
	try {
		autoCovariance me = Thing_new (Covariance);
		SSCP_init (me.get(), dimension, storage);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Reduced storage covariance not created.");
	}
}



autoCovariance Covariance_createSimple (conststring32 s_covariances, conststring32 s_centroid, integer numberOfObservations) {
	try {
		autoVEC centroid = VEC_createFromString (s_centroid);
		autoVEC covariances = VEC_createFromString (s_covariances);
		integer numberOfCovariances_wanted = centroid.size * (centroid.size + 1) / 2;
		Melder_require (covariances.size == numberOfCovariances_wanted,
			U"The number of covariance matrix elements and the number of centroid elements (d) should conform. "
			"There should be d(d+1)/2 covariance values and d centroid values.");
		
		autoCovariance me = Covariance_create (centroid.size);

		// Construct the full covariance matrix from the upper-diagonal elements

		integer rowNumber = 1;
		for (integer inum = 1; inum <= covariances.size; inum ++) {
			integer nmissing = (rowNumber - 1) * rowNumber / 2;
			integer inumc = inum + nmissing;
			rowNumber = (inumc - 1) / centroid.size + 1;
			integer icol = ((inumc - 1) % centroid.size) + 1;
			my data [rowNumber] [icol] = my data [icol] [rowNumber] = covariances [inum];
			if (icol == centroid.size)
				rowNumber ++;
		}

		// Check if a valid covariance, first check variances then covariances

		for (integer irow = 1; irow <= centroid.size; irow ++)
			Melder_require (my data [irow] [irow] > 0.0,
				U"The diagonal matrix elements should all be positive numbers.");

		for (integer irow = 1; irow <= centroid.size; irow ++)
			for (integer icol = irow + 1; icol <= centroid.size; icol ++)
				if (fabs (my data [irow] [icol] / sqrt (my data [irow] [irow] * my data [icol] [icol])) > 1) {
					integer nmissing = (irow - 1) * irow / 2;
					integer inum = (irow - 1) * centroid.size + icol - nmissing;
					Melder_throw (U"The covariance in cell [", irow, U",", icol, U"], i.e. input item ", inum, U" is too large.");
				}
		my centroid.all() <<= centroid.all();
		my numberOfObservations = numberOfObservations;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Simple Covariance not created.");
	}
}

autoCorrelation Correlation_createSimple (conststring32 s_correlations, conststring32 s_centroid, integer numberOfObservations) {
	try {
		autoVEC centroids = VEC_createFromString (s_centroid);
		autoVEC correlations = VEC_createFromString (s_correlations);
		integer numberOfCorrelations_wanted = centroids.size * (centroids.size + 1) / 2;
		Melder_require (correlations.size == numberOfCorrelations_wanted,
			U"The number of correlation matrix elements and the number of centroid elements should agree. "
			"There should be d(d+1)/2 correlation values and d centroid values.");

		autoCorrelation me = Correlation_create (centroids.size);

		// Construct the full correlation matrix from the upper-diagonal elements

		integer rowNumber = 1;
		for (integer inum = 1; inum <= correlations.size; inum ++) {
			integer nmissing = (rowNumber - 1) * rowNumber / 2;
			integer inumc = inum + nmissing;
			rowNumber = (inumc - 1) / centroids.size + 1;
			integer icol = ( (inumc - 1) % centroids.size) + 1;
			my data [rowNumber] [icol] = my data [icol] [rowNumber] = correlations [inum];
			if (icol == centroids.size) {
				rowNumber ++;
			}
		}

		// Check if a valid correlations, first check diagonal then off-diagonals

		for (integer irow = 1; irow <= centroids.size; irow ++) {
			Melder_require (my data [irow] [irow] == 1.0, U"The diagonal matrix elements should all equal 1.0.");
		}
		for (integer irow = 1; irow <= centroids.size; irow ++) {
			for (integer icol = irow + 1; icol <= centroids.size; icol ++) {
				if (fabs (my data [irow] [icol]) > 1) {
					integer nmissing = (irow - 1) * irow / 2;
					integer inum = (irow - 1) * centroids.size + icol - nmissing;
					Melder_throw (U"The correlation in cell [", irow, U",", icol, U"], i.e. input item ", inum, U" should not exceed 1.0.");
				}
			}
		}
		for (integer inum = 1; inum <= centroids.size; inum ++) {
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
		SSCP_init (me.get(), dimension, kSSCPstorage::Complete);
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
			for (integer icol = irow; icol <= my numberOfColumns; icol ++)   // a covariance matrix is symmetric
				thy data [icol] [irow] = thy data [irow] [icol] /= my numberOfObservations - numberOfConstraints;
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
			for (integer icol = irow; icol <= my numberOfColumns; icol ++)
				thy data [icol] [irow] = thy data [irow] [icol] *= my numberOfObservations - 1;
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
			for (integer j = i; j <= my numberOfColumns; j ++)
				thy data [j] [i] = thy data [i] [j] /= sqrt (my data [i] [i] * my data [j] [j]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Correlation not created.");
	}
}

double SSCP_getLnDeterminant (SSCP me) {
	try {
		return NUMdeterminant_fromSymmetricMatrix (my data.get());
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

static double traceOfSquaredMatrixProduct (constMAT const& s1, constMAT const& s2) {
	// tr ((s1*s2)^2), s1, s2 are symmetric
	autoMAT m = newMATmul (s1, s2);
	double trace2 = NUMtrace2 (m.get(), m.get());
	return trace2;
}

double Covariance_getProbabilityAtPosition_string (Covariance me, conststring32 vector_string) {
	autostring32vector vector = newSTRVECtokenize (vector_string);
	autoVEC v = newVECzero (my numberOfColumns);
	for (integer i = 1; i <= vector.size; i ++) {
		v [i] = Melder_atof (vector [i].get());
		if (i == my numberOfColumns)
			break;
	}
	double p = Covariance_getProbabilityAtPosition (me, v.get());
	return p;
}

double Covariance_getProbabilityAtPosition (Covariance me, constVEC x) {
	Melder_require (x.size == my numberOfColumns,
		U"The dimensions of the Covariance and the vector should agree.");
	if (NUMisEmpty (my lowerCholeskyInverse.get()))
		SSCP_expandLowerCholeskyInverse (me);
	double ln2pid = my numberOfColumns * log (NUM2pi);
	double dsq = NUMmahalanobisDistance (my lowerCholeskyInverse.get(), x, my centroid.get());
	double lnN = - 0.5 * (ln2pid + my lnd + dsq);
	double p = exp (lnN);
	return p;
}

double Covariance_getMarginalProbabilityAtPosition (Covariance me, constVECVU const& vector, double x) {
	double mu, stdev;
	Covariance_getMarginalDensityParameters (me, vector, & mu, & stdev);
	double dx = (x - mu) / stdev;
	double p = (NUM1_sqrt2pi / stdev) * exp (- 0.5 * dx * dx);
	return p;
}

/* Precondition ||v|| = 1 */
void Covariance_getMarginalDensityParameters (Covariance me, constVECVU const& v, double *out_mu, double *out_stdev) {
	Melder_assert (v.size == my numberOfColumns);
	if (out_mu)
		*out_mu = NUMinner (v, my centroid.get());
	if (out_stdev) {
		longdouble stdev = 0.0;
		if (my numberOfRows == 1) { // 1xn diagonal matrix
			for (integer m = 1; m <= my numberOfColumns; m ++)
				stdev += v [m] * my data [1] [m] * v [m];
		} else {
			for (integer k = 1; k <= my numberOfRows; k ++)
				for (integer m = 1; m <= my numberOfColumns; m ++)
					stdev += v [k] * my data [k] [m] * v [m];
		}
		*out_stdev = sqrt (double (stdev));
	}
}

double Covariances_getMultivariateCentroidDifference (Covariance me, Covariance thee, int equalCovariances, double *out_prob, double *out_fisher, double *out_df1, double *out_df2) {
	integer p = my numberOfRows, N = Melder_ifloor (my numberOfObservations + thy numberOfObservations);
	integer N1 = Melder_ifloor (my numberOfObservations), n1 = N1 - 1;
	integer N2 = Melder_ifloor (thy numberOfObservations), n2 = N2 - 1;

	double dif = undefined, fisher = undefined;
	double df1 = p, df2 = N - p - 1;
	
	Melder_require (df2 >= 1.0, 
		U"Not enough observations (", N, U") for this test.");
	Melder_require (p >= N1 && p >= N2,
		U"The number of observations should be larger than the number of variables.");

	dif = 0;
	for (integer i = 1; i <= p; i ++) {
		double dist = my centroid [i] - thy centroid [i];
		dif += dist * dist;
	}
	dif = sqrt (dif);

	if (equalCovariances) {
		// Morrison, page 141
		autoCovariance pool = Covariances_pool (me, thee);
		Melder_assert (my data.ncol == p);   // ppgb 20180913
		autoMAT s = newMATcopy (my data.get());
		double lndet;
		MATlowerCholeskyInverse_inplace (s.get(), & lndet);

		double mahalanobis = NUMmahalanobisDistance (s.get(), my centroid.get(), thy centroid.get());
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
		
		autoMAT s1 = newMATraw (p, p), s2 = newMATraw (p, p), s = newMATraw (p, p);

		for (integer i = 1; i <= p; i ++) {
			for (integer j = 1; j <= p; j ++) {
				s1 [i] [j] = my data [i] [j] / my numberOfObservations;
				s2 [i] [j] = thy data [i] [j] / thy numberOfObservations;
				s [i] [j] = s1 [i] [j] + s2 [i] [j];
			}
		}
		double lndet;
		MATlowerCholeskyInverse_inplace (s.get(), & lndet);
		// Krishan... formula 2, page 162
		double hotelling_tsq = NUMmahalanobisDistance (s.get(), my centroid.get(), thy centroid.get());

		autoMAT si = newMATinverse_fromLowerCholeskyInverse (s.get());
		double tr_s1sisqr = traceOfSquaredMatrixProduct (s1.get(), si.get());
		double tr_s1si = NUMtrace2 (s1.get(), si.get());
		double tr_s2sisqr = traceOfSquaredMatrixProduct (s2.get(), si.get());
		double tr_s2si = NUMtrace2 (s2.get(), si.get());

		double nu = (p + p * p) / ( (tr_s1sisqr + tr_s1si * tr_s1si) / n1 + (tr_s2sisqr + tr_s2si * tr_s2si) / n2);
		df2 = nu - p + 1;
		fisher =  hotelling_tsq * (nu - p + 1) / (nu * p);
	}

	if (out_prob) *out_prob = NUMfisherQ (fisher, df1, df2);
	if (out_fisher) *out_fisher = fisher;
	if (out_df1) *out_df1 = df1;
	if (out_df2) *out_df2 = df2;
 
	return dif;
}

/* Schott 2001 */
void Covariances_equality (CovarianceList me, int method, double *out_prob, double *out_chisq, double *out_df) {
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
				lnd = NUMdeterminant_fromSymmetricMatrix (pool -> data.get());
			} catch (MelderError) {
				Melder_throw (U"Pooled covariance matrix is singular.");
			}

			double nsi = 0.0, m = ns * lnd; // First part of eq (3) page 297

			for (integer i = 1; i <= numberOfMatrices; i ++) {
				Covariance ci = my at [i];
				try {
					lnd = NUMdeterminant_fromSymmetricMatrix (ci -> data.get());
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

			double trace = 0.0;
			MATlowerCholeskyInverse_inplace (pool -> data.get(), nullptr);
			autoMAT si = newMATinverse_fromLowerCholeskyInverse (pool -> data.get());
			for (integer i = 1; i <= numberOfMatrices; i ++) {
				Covariance ci = my at [i];
				double ni = ci -> numberOfObservations - 1;
				autoMAT s1 = newMATmul (ci -> data.get(), si.get());
				double trace_ii = NUMtrace2 (s1.get(), s1.get());
				trace += (ni / ns) * (1 - (ni / ns)) * trace_ii;
				for (integer j = i + 1; j <= numberOfMatrices; j ++) {
					Covariance cj = my at [j];
					double nj = cj -> numberOfObservations - 1;
					autoMAT s2 = newMATmul (cj -> data.get(), si.get());
					double trace_ij = NUMtrace2 (s1.get(), s2.get());
					trace -= 2.0 * (ni / ns) * (nj / ns) * trace_ij;
				}
			}
			df = (numberOfMatrices - 1) * p * (p + 1) / 2.0;
			chisq = (ns / 2.0) * trace;
		} else {
			return;
		}
		if (out_prob) *out_prob = NUMchiSquareQ (chisq, df);
		if (out_df)*out_df = df;
		if (out_chisq) *out_chisq = chisq;

	} catch (MelderError) {
		Melder_throw (U"Equality coud not be tested.");
	}
}

void Covariance_difference (Covariance me, Covariance thee, double *out_prob, double *out_chisq, double *out_df) {
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
	Melder_require (numberOfObservations > 1,
		U"Number of observations too small.");
	Melder_assert (thy data.ncol == p);
	autoMAT linv = newMATcopy (thy data.get());
	MATlowerCholeskyInverse_inplace (linv.get(), & ln_thee);
	ln_me = NUMdeterminant_fromSymmetricMatrix (my data.get());

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
			integer lp = std::max (j, i);
			for (integer l = lp; l <= p; l ++)
				trace += my data [i] [j] * linv [l] [j] * linv [l] [i];
		}
	}

	double l = (numberOfObservations - 1) * fabs (ln_thee - ln_me + trace - p);
	chisq = l * fabs (1.0 - (2.0 * p + 1.0 - 2.0 / (p + 1)) / (numberOfObservations - 1) / 6.0);
	df = p * (p + 1) / 2.0;
	
	if (out_prob) *out_prob = NUMchiSquareQ (chisq, df);
	if (out_chisq) *out_chisq = chisq;
	if (out_df) *out_df = df;

}

static void checkOneIndex (TableOfReal me, integer index) {
	Melder_require (index > 0 && index <= my numberOfColumns, U"Index should be in interval [1, ", my numberOfColumns, U"].");
}

static void checkTwoIndices (TableOfReal me, integer index1, integer index2) {
	Melder_require (index1 > 0 && index1 <= my numberOfColumns && index2 > 0 && index2 <= my numberOfColumns,
		U"Index should be in interval [1, ", my numberOfColumns, U"].");
	Melder_require (index1 != index2, 
		U"Indices should be different.");
}

void Covariance_getSignificanceOfOneMean (Covariance me, integer index, double mu, double *out_prob, double *out_t, double *out_df) {
	double var = my data [index] [index];
	double prob = undefined, t = undefined, df = my numberOfObservations - 1.0;

	checkOneIndex (me, index);

	if (var > 0.0) {
		t = (my centroid [index] - mu) / sqrt (var / my numberOfObservations);
		if (out_prob) prob = 2.0 * NUMstudentQ (fabs (t), df);
	}
	
	if (out_prob)*out_prob = prob;
	if (out_t) *out_t = t;
	if (out_df) *out_df = df;

}

void Covariance_getSignificanceOfMeansDifference (Covariance me, integer index1, integer index2, double mu, int paired, int equalVariances, double *out_prob, double *out_t, double *out_df) {
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
	if (out_prob) *out_prob = prob;
	if (out_t) *out_t = t;
	if (out_df) *out_df = df;
}

void Covariance_getSignificanceOfOneVariance (Covariance me, integer index, double sigmasq, double *out_prob, double *out_chisq, double *out_df) {
	double var = my data [index] [index];
	double prob = undefined, chisq = undefined;
	double df = my numberOfObservations - 1.0;

	checkOneIndex (me, index);

	if (var > 0.0) {
		chisq = df;
		if (sigmasq > 0.0) chisq = df * var / sigmasq;
		if (out_prob) prob = NUMchiSquareQ (chisq, df);
	}
	if (out_prob) *out_prob = prob;
	if (out_chisq) *out_chisq = chisq;
	if (out_df) *out_df = df;
}

void Covariance_getSignificanceOfVariancesRatio (Covariance me, integer index1, integer index2, double ratio, double *out_prob, double *out_f, double *out_df) {
	double df = my numberOfObservations - 1.0, prob = undefined, f = undefined;
	checkTwoIndices (me, index1, index2);

	double var1 = my data [index1] [index1];
	double var2 = my data [index2] [index2];

	if (var1 > 0.0 && var2 > 0.0) {
		double ratio2 = (var1 / var2) / ratio;
		f = ratio2;
		if (var2 > var1) ratio2 = (var2 / var1) * ratio;
		if (out_prob) {
			prob = 2.0 * NUMfisherQ (ratio2, df, df);
			if (prob > 1.0) prob = 2.0 - prob;
		}
	}
	if (out_prob) *out_prob = prob;
	if (out_df) *out_df = df;
	if (out_f) *out_f = f;
}

autoTableOfReal Correlation_confidenceIntervals (Correlation me, double confidenceLevel, integer numberOfTests, int method) {
	try {
		integer m_bonferroni = my numberOfRows * (my numberOfRows - 1) / 2;
		Melder_require (confidenceLevel > 0 && confidenceLevel <= 1.0, U"Confidence level should be in interval (0-1).");

		Melder_require (my numberOfObservations > 4, U"The number of observations should be greater than 4.");
		Melder_require (numberOfTests >= 0, U"The \"number of tests\" should not be less than zero.");

		if (numberOfTests == 0)
			numberOfTests = m_bonferroni;
		if (numberOfTests > m_bonferroni)
			Melder_warning (U"The \"number of tests\" should not exceed the number of elements in the Correlation object.");

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

					if (b > 0) d = - d;
					q = b - d;
					rmin = q / a; 
					rmin /= sqrt (1.0 + rmin * rmin);
					rmax = c / q; 
					rmax /= sqrt (1.0 + rmax * rmax);
					if (rmin > rmax) {
						double t = rmin; 
						rmin = rmax; rmax = t;
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

void SSCP_getDiagonality_bartlett (SSCP me, integer numberOfContraints, double *out_chisq, double *out_prob, double *out_df) {
	autoCorrelation c = SSCP_to_Correlation (me);
	Correlation_testDiagonality_bartlett (c.get(), numberOfContraints, out_chisq, out_prob, out_df);
}

/* Morrison, page 118 */
void Correlation_testDiagonality_bartlett (Correlation me, integer numberOfContraints, double *out_chisq, double *out_prob, double *out_df) {
	integer p = my numberOfRows;
	double chisq = undefined, prob = undefined, df = p * (p -1) / 2.0;

	if (numberOfContraints <= 0)
		numberOfContraints = 1;

	if (numberOfContraints > my numberOfObservations) {
		Melder_warning (U"Correlation_testDiagonality_bartlett: number of constraints cannot exceed the number of observations.");
		return;
	}
	if (my numberOfObservations >= numberOfContraints) {
		double ln_determinant = NUMdeterminant_fromSymmetricMatrix (my data.get());
		chisq = - ln_determinant * (my numberOfObservations - numberOfContraints - (2.0 * p + 5.0) / 6.0);
		if (out_prob) {
			prob = NUMchiSquareQ (chisq, df);
		}
	}
	if (out_chisq) *out_chisq = chisq;
	if (out_prob) *out_prob = prob;
	if (out_df) *out_df = df;
}

void SSCP_expand (SSCP me) {

	/* A reduced matrix has my numberOfRows < my numberOfColumns.
		After expansion:
		my numberOfRows == my numberOfColumns
		my storageNumberOfRows = my numberOfRows (before)
		 my data (after) = my expansion;
		my expansion = my data (before)
		 No expansion for a standard matrix or if already expanded and data has not changed!
	*/
	if ((my expansionNumberOfRows == 0 && my numberOfRows == my numberOfColumns) ||
	        (my expansionNumberOfRows > 0 && ! my dataChanged))
		return;

	if (NUMisEmpty (my expansion.get()))
		my expansion = newMATzero (my numberOfColumns, my numberOfColumns);
	for (integer ir = 1; ir <= my numberOfColumns; ir ++)
		for (integer ic = ir; ic <= my numberOfColumns; ic ++) {
			integer dij = labs (ir - ic);
			my expansion [ir] [ic] = my expansion [ic] [ir] = dij < my numberOfRows ? my data [dij + 1] [ic] : 0.0;
		}

	// Now make 'my data' point to 'my expansion'
	std::swap (my data, my expansion);
	my expansionNumberOfRows = my numberOfRows;
	my numberOfRows = my numberOfColumns;
	my dataChanged = 0;
}

void SSCP_unExpand (SSCP me) {
	if (my expansionNumberOfRows == 0)
		return;
	my data = my expansion.move();
	my numberOfRows = my expansionNumberOfRows;
	my expansionNumberOfRows = 0;
	my dataChanged = 0;
}

void SSCP_expandLowerCholeskyInverse (SSCP me) {
	if (NUMisEmpty (my lowerCholeskyInverse.get()))
		my lowerCholeskyInverse = newMATraw (my numberOfColumns, my numberOfColumns);
	if (my numberOfRows == 1) {   // diagonal
		my lnd = 0.0;
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			my lowerCholeskyInverse [1] [j] = 1.0 / sqrt (my data [1] [j]);   // inverse is 1/stddev
			my lnd += log (my data [1] [j]);   // diagonal elmnt is variance
		}
	} else {
		my lowerCholeskyInverse.all() <<= my data.all();
		try {
			MATlowerCholeskyInverse_inplace (my lowerCholeskyInverse.get(), & (my lnd));
		} catch (MelderError) {
			// singular matrix: arrange a diagonal only inverse.
			my lnd = 0.0;
			for (integer i = 1; i <= my numberOfRows; i ++) {
				for (integer j = i; j <= my numberOfColumns; j ++)
					my lowerCholeskyInverse [i] [j] = my lowerCholeskyInverse [j] [i] = (i == j ? 1.0 / sqrt (my data [i] [i]) : 0.0);
				my lnd += log (my data [i] [i]);
			}
			my lnd *= 2.0;
		}
	}
}

void SSCP_unExpandLowerCholesky (SSCP me) {
	my lowerCholeskyInverse.reset();
	my lnd = 0.0;
}

void SSCP_expandPCA (SSCP me) {
	if (my pca)
		my pca.reset();
	my pca = SSCP_to_PCA (me);
}

void SSCP_unExpandPCA (SSCP me) {
	my pca.reset();
}

/* End of file SSCP.c */
