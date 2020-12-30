/* SSCP.cpp
 *
 * Copyright (C) 1993-2020 David Weenink
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

#include "Covariance.h"
#include "Correlation.h"
#include "Eigen.h"
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

#include "enums_getText.h"
#include "SSCP_enums.h"
#include "enums_getValue.h"
#include "SSCP_enums.h"

#define TOVEC(x) (&(x) - 1)

Thing_implement (SSCP, TableOfReal, 0);
Thing_implement (SSCPList, TableOfRealList, 0);

void structSSCP :: v_info () {
	structTableOfReal :: v_info ();
	const double zmin = NUMmin (our data.all());
	const double zmax = NUMmax (our data.all());
	MelderInfo_writeLine (U"Minimum value: ", zmin);
	MelderInfo_writeLine (U"Maximum value: ", zmax);
}

/*
	Calculate scale factor by which sqrt(eigenvalue) has to
	be multiplied to obtain the length of an ellipse axis.
*/
double SSCP_getEllipseScalefactor (SSCP me, double scale, bool confidence) {
	const integer n = Melder_ifloor (SSCP_getNumberOfObservations (me));

	if (confidence) {
		if (n - my numberOfColumns < 1)
			return -1.0;
		/*
			D.E. Johnson (1998), Applied Multivariate methods, page 410
		*/
		const double f = NUMinvFisherQ (1.0 - scale, my numberOfColumns, n - my numberOfColumns);
		scale = 2.0 * sqrt (f * my numberOfColumns * (n - 1) / ( ((double) n) * (n - my numberOfColumns)));
	} else {
		/*
			Very ugly, temporary hack
		*/
		scale *= 2.0 / (scale < 0.0 ? -1.0 : sqrt (n - 1));
	}
	return scale;
}

static void getEllipseBoundingBoxCoordinates (SSCP me, double scale, bool confidence, double *out_xmin, double *out_xmax, double *out_ymin, double *out_ymax) {
	double a, b, cs, width, height;
	NUMeigencmp22 (my data [1] [1], my data [1] [2], my data [2] [2], & a, & b, & cs, nullptr);
	NUMgetEllipseBoundingBox (sqrt (a), sqrt (b), cs, & width, & height);

	const double lscale = SSCP_getEllipseScalefactor (me, scale, confidence);
	const double xmin = my centroid [1] - lscale * width / 2.0;
	const double xmax = xmin + lscale * width;
	const double ymin = my centroid [2] - lscale * height / 2.0;
	const double ymax = ymin + lscale * height;
	if (out_xmin)
		*out_xmin = xmin;
	if (out_xmax)
		*out_xmax = xmax;
	if (out_ymin)
		*out_ymin = ymin;
	if (out_ymax)
		*out_ymax = ymax;
}

void SSCPList_getEllipsesBoundingBoxCoordinates (SSCPList me, double scale, bool confidence, double *out_xmin, double *out_xmax, double *out_ymin, double *out_ymax) {
	MelderExtremaWithInit extremaX, extremaY;
	for (integer i = 1; i <= my size; i ++) {
		const SSCP s = my at [i];
		double xmn, xmx, ymn, ymx;
		getEllipseBoundingBoxCoordinates (s, scale, confidence, & xmn, & xmx, & ymn, & ymx);
		if (xmn < extremaX.min)
			extremaX.min = xmn;
		if (xmx > extremaX.max)
			extremaX.max = xmx;
		if (ymn < extremaY.min)
			extremaY.min = ymn;
		if (ymx > extremaY.max)
			extremaY.max = ymx;
	}
	if (out_xmin)
		*out_xmin = extremaX.min;
	if (out_xmax)
		*out_xmax = extremaX.max;
	if (out_ymin)
		*out_ymin = extremaY.min;
	if (out_ymax)
		*out_ymax = extremaY.max;
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
		constexpr integer nsteps = 100;
		autoVEC x = raw_VEC (nsteps + 1);
		autoVEC y = raw_VEC (nsteps + 1);
		/*
			From the eigenvalues and eigenvectors of the symmetrical sscp matrix we
			can calculate the length and directions of the principal axes of the ellipse.
		*/
		double eval1, eval2, cosine, sine;
		NUMeigencmp22 (my data [1] [1], my data [1] [2], my data [2] [2], & eval1, & eval2, & cosine, & sine);
		/*
			1. Parametrize as standard ellipse with horizontal radius a and vertical radius b and origin at (0,0) as
				x = a cos(phi)
				y = b sin(phi)
		*/
		const double a = scale * sqrt (eval1) / 2.0;
		const double b = scale * sqrt (eval2) / 2.0;
		const double angle_inc = NUM2pi / nsteps;
		for (integer i = 1; i <= nsteps + 1; i ++) {
			const double phi = (i - 1) * angle_inc;
			x [i] = a * cos (phi);
			y [i] = b * sin (phi);
		}
		/*
			2. Rotate x axis to the eigenvector 1 (cosine, sine)
			|x'|   | cosine -sine |   |x|
			|  | = |              | * | |
			|y'|   | sine  cosine |   |y|
		*/
		for (integer i = 1; i <= nsteps + 1; i ++) {
			double xp =  cosine * x [i] -   sine * y [i];			
			y [i]     =    sine * x [i] + cosine * y[i];
			x [i] = xp;
		}
		/*
			3. Translate to the centroid
		*/
		x.get()  +=  my centroid [1];
		y.get()  +=  my centroid [2];
		Graphics_polyline (g, nsteps + 1, & x [1], & y [1]);
		
		if (label && fontSize > 0.0) {
			const double oldFontSize = Graphics_inqFontSize (g);
			Graphics_setFontSize (g, fontSize);
			Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
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
		/*
			Projection P of S on v1 and v2 (given matrix V' with 2 rows) is P = V'SV
			P [i] [j] = sum(k) sum(m) V' [i] [k]*S [k] [m]*V [m] [j] = V' [i] [k]*S [k] [m]*V' [j] [m]

			For the new centroids cnew [i] = sum(m) V' [i] [m]*c [m]
		*/
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
	const integer numberOfRows = storage == kSSCPstorage::DIAGONAL ? 1 : dimension;
	TableOfReal_init (me, numberOfRows, dimension);
	my centroid = zero_VEC (dimension);
}

autoSSCP SSCP_create (integer dimension) {
	try {
		autoSSCP me = Thing_new (SSCP);
		SSCP_init (me.get(), dimension, kSSCPstorage::COMPLETE);
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
	Melder_require (scale > 0,
		U"The scale factor should be larger than zero.");
	double a, b;
	NUMeigencmp22 (thy data [1] [1], thy data [1] [2], thy data [2] [2], & a, & b, nullptr, nullptr);
	/*
		Take sqrt to get units of 'std_dev'
	*/
	a = scale * sqrt (a) / 2.0;
	b = scale * sqrt (b) / 2.0;
	return NUMpi * a * b;
}

double SSCP_getFractionVariation (SSCP me, integer from, integer to) {
	const integer n = my numberOfRows;

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
	integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, bool garnish)
{
	Melder_require (d1 > 0 && d1 <= my numberOfRows && d2 > 0 && d2 <= my numberOfRows && d1 != d2,
		U"Incorrect axes.");

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
	Melder_require (scale > 0,
		U"The scale factor should be larger than zero.");
	
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
	if (to == 0)
		to = my numberOfRows;
	if (from > 0 && to <= my numberOfRows && from <= to) {
		const double sum = SSCP_getTotalVariance (me);
		if (sum > 0.0) {
			double partial = 0.0;
			for (integer i = from; i <= to; i ++)
				partial += my numberOfRows == 1 ? my data [1] [i] : my data [i] [i];
			ccoc = partial / sum;
		}
	}
	return ccoc;
}

autoSSCP TableOfReal_to_SSCP (TableOfReal me, integer rowb, integer rowe, integer colb, integer cole) {
	try {
		Melder_require (NUMdefined (my data.get()),
			U"All the table's elements should be defined.");
		fixAndCheckRowRange (& rowb, & rowe, my data.get(), 1);
		fixAndCheckColumnRange (& colb, & cole, my data.get(), 1);
		autoMAT part = part_MAT (my data.get(), rowb, rowe, colb, cole);
		if (part.nrow < part.ncol)
			Melder_warning (U"The selected number of rows (", part.nrow,
				U") is less than the selected number of columns (", part.ncol,
				U").\nThe SSCP will not have full dimensionality. This may be a problem in later analysis steps."
			);
		autoSSCP thee = SSCP_create (part.ncol);
		columnMeans_VEC_out (thy centroid.get(), part.get());
		part.all()  -=  thy centroid.all();
		SSCP_setNumberOfObservations (thee.get(), part.nrow);
		mtm_MAT_out (thy data.get(), part.get());   // sum of squares and cross products = T'T
		for (integer j = 1; j <= part.ncol; j ++) {
			const conststring32 label = my columnLabels [colb - 1 + j].get();
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
		autoMAT part = part_MAT (my data.get(), rowb, rowe, colb, cole);
		if (part.nrow < part.ncol)
			Melder_warning (U"The selected number of data points (", part.nrow,
				U") is less than the selected number of variables (", part.ncol,
				U").\nThe SSCP will not have full dimensionality. This may be a problem in later analysis steps."
			);
		autoSSCP thee = SSCP_create (part.ncol);
		columnMeans_VEC_out (thy centroid.get(), part.get());
		part.all()  -=  thy centroid.all();
		SSCP_setNumberOfObservations (thee.get(), part.nrow);
		if (weightColumnNumber != 0) {
			autoVEC rowWeights = column_VEC (my data.horizontalBand (rowb, rowe), weightColumnNumber);
			MATmtm_weighRows (thy data.get(), part.get(), rowWeights.get());
		} else
			mtm_MAT_out (thy data.get(), part.get());   // sum of squares and cross products = T'T
		for (integer j = 1; j <= part.ncol; j ++) {
			const conststring32 label = my columnLabels [colb - 1 + j].get();
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
				/*
					Current label different from previous one(s)
				*/
				numberOfRowsInCurrent = i - index;
				lastrow = i - 1;
			} else if (i == my numberOfRows) {
				/*
					Current (last) label is same as previous
				*/
				numberOfRowsInCurrent = i - index + 1;
				lastrow = i;
			} else {
				/*
					Next one
				*/
				continue;
			}
			/*
				We found a new group
			*/
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
		if (lastrow != my numberOfRows)
			numberOfMatrices ++;
		Melder_warningOn ();
		if (numberOfSingularMatrices > 0 || thy size != numberOfMatrices) {
			const integer notIncluded = numberOfMatrices - thy size;
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
			mat = zero_MAT (my numberOfColumns, my numberOfColumns);
			mat.diagonal() <<= my data.row (1); // 1xn matrix -> nxn
		} else if (my data.nrow == my numberOfColumns && my data.ncol == my numberOfColumns)
			mat = copy_MAT (my data.get());
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
	Melder_require (! (rowNumber == columnNumber && value <= 0.0),
		U"Diagonal element should always be a positive number.");
	
	if (my numberOfRows == 1) {   // diagonal
		Melder_require (rowNumber == columnNumber,
			U"Row and column number should be equal for a diagonal matrix.");
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

/************ SSCPList ***********************************************/

autoSSCP SSCPList_to_SSCP_pool (SSCPList me) {
	try {
		autoSSCP pool = Data_copy (my at [1]);

		for (integer k = 2; k <= my size; k ++) {
			const SSCP t = my at [k];
			Melder_require (t -> numberOfRows == pool -> numberOfRows,
				U"The dimension of item ", k, U" should agree.");
			pool -> numberOfObservations += t -> numberOfObservations;
			/*
				Sum the sscp's and weigh the centroid.
			*/
			pool -> data.all()  +=  t -> data.all();
			pool -> centroid.all()  +=  t->centroid.all()  *  t -> numberOfObservations;
		}
		
		pool -> centroid.all()  *=  1.0 / pool -> numberOfObservations;
		
		return pool;
	} catch (MelderError) {
		Melder_throw (me, U": not pooled.");
	}
}

void SSCPList_getHomegeneityOfCovariances_box (SSCPList me, double *out_prob, double *out_chisq, double *out_df) {
	autoSSCP pooled = SSCPList_to_SSCP_pool (me);
	const integer p = pooled -> numberOfColumns;
	double ln_determinant, inv = 0.0, sum = 0.0, chisq = 0.0;
	for (integer i = 1; i <= my size; i ++) {
		const SSCP t = my at [i];
		const double ni = t -> numberOfObservations - 1.0;
		ln_determinant = NUMdeterminant_fromSymmetricMatrix (t -> data.get());
		/*
			Box-test is for covariance matrices -> scale determinant.
		*/
		ln_determinant -= p * log (ni);
		sum += ni;
		inv += 1.0 / ni;
		chisq -= ni * ln_determinant;
	}

	ln_determinant = NUMdeterminant_fromSymmetricMatrix (pooled -> data.get());
	ln_determinant -= p * log (pooled -> numberOfObservations - my size);
	chisq += sum * ln_determinant;

	chisq *= 1.0 - (inv - 1.0 / sum) * (2.0 * p * p + 3.0 * p - 1.0) / (6.0 * (p + 1) * (my size - 1.0));
	const double df = (my size - 1.0) * p * (p + 1) / 2.0;
	
	if (out_prob)
		*out_prob = NUMchiSquareQ (chisq, df);
	if (out_chisq)
		*out_chisq = chisq;
	if (out_df)
		*out_df = df;
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

void SSCPList_drawConcentrationEllipses (SSCPList me, Graphics g, double scale, bool confidence, conststring32 label, integer d1, integer d2, double xmin, double xmax, double ymin, double ymax, double fontSize, bool garnish) {
	const SSCP t = my at [1];

	Melder_require (d1 > 0 && d1 <= t -> numberOfColumns && d2 > 0 && d2 <= t -> numberOfColumns && d1 != d2,
		U"Incorrect axes.");
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
		const SSCP ti = thy at [i];
		const double lscale = SSCP_getEllipseScalefactor (ti, scale, confidence);
		if (lscale < 0.0)
			continue;
		if (! label || Melder_cmp (label, Thing_getName (ti)) == 0)
			SSCP_drawTwoDimensionalEllipse_inside (ti, g, lscale, Thing_getName (ti), fontSize);
	}

	Graphics_unsetInner (g);
	if (garnish) {
		const SSCP t1 = my at [1];
		Graphics_drawInnerBox (g);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, t1 -> columnLabels [d2] ? t1 -> columnLabels [d2].get() : Melder_cat (U"Dimension ", d2));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textBottom (g, true, t1 -> columnLabels [d1] ? t1 -> columnLabels [d1].get() : Melder_cat (U"Dimension ", d1));
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

autoSSCP Covariance_to_SSCP (Covariance me) {
	try {
		autoSSCP thee = Thing_new (SSCP);
		my structSSCP :: v_copy (thee.get());
		for (integer irow = 1; irow <= my numberOfRows; irow ++)
			for (integer icol = irow; icol <= my numberOfColumns; icol ++)
				thy data [icol] [irow] = thy data [irow] [icol] *= my numberOfObservations - 1;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": SSCP not created.");
	}
}

double SSCP_getLnDeterminant (SSCP me) {
	try {
		return NUMdeterminant_fromSymmetricMatrix (my data.get());
	} catch (MelderError) {
		return undefined;
	}
}

void SSCP_getDiagonality_bartlett (SSCP me, integer numberOfContraints, double *out_chisq, double *out_prob, double *out_df) {
	autoCorrelation c = SSCP_to_Correlation (me);
	Correlation_testDiagonality_bartlett (c.get(), numberOfContraints, out_chisq, out_prob, out_df);
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
		my expansion = zero_MAT (my numberOfColumns, my numberOfColumns);
	for (integer ir = 1; ir <= my numberOfColumns; ir ++)
		for (integer ic = ir; ic <= my numberOfColumns; ic ++) {
			const integer dij = integer_abs (ir - ic);
			my expansion [ir] [ic] = ( my expansion [ic] [ir] = dij < my numberOfRows ? my data [dij + 1] [ic] : 0.0 );
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
		my lowerCholeskyInverse = raw_MAT (my numberOfColumns, my numberOfColumns);
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
