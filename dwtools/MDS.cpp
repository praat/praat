/* MDS.cpp
 *
 * Copyright (C) 1993-2020 David Weenink, 2015,2017 Paul Boersma
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
 djmw 20020408 GPL
 djmw 20020513 Applied TableOfReal_setSequential{Column/Row}Labels
 djmw 20040309 Extra tests for empty objects.
 djmw 20061218 Changed to Melder_information<x> format.
 djmw 20071022 Removed unused code.
 djmw 20071106 drawSplines: to wchar
 djmw 20071201 Melder_warning<n>
 djmw 20071213 Removed Preference.
 djmw 20080724 Thing_classNameW ->Thing_className
  djmw 20110304 Thing_new
*/

#include "SVD.h"
#include "Matrix_extensions.h"
#include "TableOfReal_extensions.h"
#include "MDS.h"
#include "Proximity_and_Distance.h"
#include "SSCP.h"
#include "PCA.h"

#include "enums_getText.h"
#undef _MDS_enums_h_
#include "MDS_enums.h"
#include "enums_getValue.h"
#undef _MDS_enums_h_
#include "MDS_enums.h"

#define TINY 1e-30

Thing_implement (Kruskal, Thing, 0);

Thing_implement (Transformator, Thing, 0);
Thing_implement (RatioTransformator, Transformator, 0);
Thing_implement (MonotoneTransformator, Transformator, 0);
Thing_implement (ISplineTransformator, Transformator, 0);

Thing_implement (Weight, TableOfReal, 0);
Thing_implement (Salience, TableOfReal, 0);
Thing_implement (ScalarProduct, TableOfReal, 0);

Thing_implement (ConfusionList, TableOfRealList, 0);
Thing_implement (ProximityList, TableOfRealList, 0);
Thing_implement (ScalarProductList, TableOfRealList, 0);

Thing_implement (DissimilarityList, ProximityList, 0);
Thing_implement (Similarity, Proximity, 0);


/********************** NUMERICAL STUFF **************************************/


static void MATfromVEC_inplace (MATVU const& m, VECVU v) {
	Melder_assert (m.nrow * m.ncol == v.size);
	integer k = 1;
	for (integer irow = 1; irow <= m.nrow; irow ++)
		for (integer icol = 1; icol <= m.ncol; icol ++)
			m [irow] [icol] = v [k ++];
}

static void MAT_divideRowByRowsum_inplace (MATVU const& m) {
	for (integer i = 1; i <= m.nrow; i ++) {
		const longdouble rowSum = NUMsum (m.row (i));
		if (rowSum != 0.0)
			m.row (i)  *=  double (1.0 / rowSum);
	}
}

static integer NUMcountZeros (constMATVU const& m) {
	integer numberOfZeros = 0;
	for (integer i = 1; i <= m.nrow; i ++)
		for (integer j = 1; j <= m.ncol; j ++)
			if (m [i] [j] == 0.0)
				numberOfZeros ++;
	return numberOfZeros;
}

/************ ConfigurationList & Similarity **************************/

autoDistanceList ConfigurationList_to_DistanceList (ConfigurationList me) {
	try {
		autoDistanceList thee = DistanceList_create ();
		for (integer i = 1; i <= my size; i ++) {
			const Configuration conf = my at [i];
			autoDistance d = Configuration_to_Distance (conf);
			Thing_setName (d.get(), Thing_getName (conf));
			thy addItem_move (d.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"DistanceList not created form Configurations.");
	}
}

autoSimilarity ConfigurationList_to_Similarity_cc (ConfigurationList me, Weight weight) {
	try {
		autoDistanceList d = ConfigurationList_to_DistanceList (me);
		autoSimilarity thee = DistanceList_to_Similarity_cc (d.get(), weight);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Similarity not created form Configurations.");
	}
}

autoSimilarity DistanceList_to_Similarity_cc (DistanceList me, Weight w) {
	try {
		Melder_require (my size > 0,
			U"DistanceList should not be empty.");
		Melder_require (TableOfRealList_haveIdenticalDimensions (my asTableOfRealList()),
			U"All matrices should have the same dimensions.");

		autoWeight aw;
		if (! w) {
			aw = Weight_create (my at [1] -> numberOfRows);
			w = aw.get();
		}

		autoSimilarity thee = Similarity_create (my size);

		for (integer i = 1; i <= my size; i ++) {
			const Distance di = my at [i];
			const conststring32 name = Thing_getName (di);
			TableOfReal_setRowLabel (thee.get(), i, name);
			TableOfReal_setColumnLabel (thee.get(), i, name);
			thy data [i] [i] = 1;
			for (integer j = i + 1; j <= my size; j ++) {
				const Distance dj = my at [j];
				thy data [i] [j] = thy data [j] [i] = Distance_Weight_congruenceCoefficient (di, dj, w);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Similarity not created from DistanceList.");
	}
}


/***************** Transformator **********************************************/

autoDistance structTransformator :: v_transform (MDSVec vec, Distance dist, Weight /* w */) {
	try {
		autoDistance thee = Distance_create (numberOfPoints);
		TableOfReal_copyLabels (dist, thee.get(), 1, 1);

		// Absolute scaling

		for (integer i = 1; i <= vec -> numberOfProximities; i ++) {
			const integer ii = vec -> rowIndex [i];
			const integer jj = vec -> columnIndex [i];
			thy data [ii] [jj] = thy data [jj] [ii] = vec -> proximity [i];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Distance not created.");
	}
}

void Transformator_init (Transformator me, integer numberOfPoints) {
	my numberOfPoints = numberOfPoints;
	my normalization = 1;
}

autoTransformator Transformator_create (integer numberOfPoints) {
	try {
		autoTransformator me = Thing_new (Transformator);
		Transformator_init (me.get(), numberOfPoints);
		my normalization = 0;
		return me;
	} catch (MelderError) {
		Melder_throw (U"No Transformator created.");
	}
}

autoDistance Transformator_transform (Transformator me, MDSVec vec, Distance d, Weight w) {
	try {
		Melder_require (my numberOfPoints == vec -> numberOfPoints && my numberOfPoints == d -> numberOfRows &&
			d -> numberOfRows == w -> numberOfRows,
				U"Dimensions should agree.");
		return my v_transform (vec, d, w);
	} catch (MelderError) {
		Melder_throw (me, U"Distance not created.");
	}
}

autoDistance structRatioTransformator :: v_transform (MDSVec vec, Distance d, Weight w) {
	autoDistance thee = Distance_create (numberOfPoints);
	TableOfReal_copyLabels (d, thee.get(), 1, 1);

	// Determine ratio (eq. 9.4)

	longdouble etaSq = 0.0, rho = 0.0;
	for (integer i = 1; i <= vec -> numberOfProximities; i ++) {
		const integer ii = vec -> rowIndex [i];
		const integer jj = vec -> columnIndex [i];
		const double delta_ij = vec -> proximity [i];
		const double d_ij = d -> data [ii] [jj];
		const double tmp = w -> data [ii] [jj] * delta_ij * delta_ij;
		etaSq += tmp;
		rho += tmp * d_ij * d_ij;
	}

	// transform

	Melder_require (etaSq > 0.0,
		U"Eta squared should not be zero.");
	
	our ratio = rho / etaSq;
	for (integer i = 1; i <= vec -> numberOfProximities; i ++) {
		const integer ii = vec -> rowIndex [i];
		const integer jj = vec -> columnIndex [i];
		thy data [ii] [jj] = thy data [jj] [ii] = our ratio * vec -> proximity [i];
	}

	if (our normalization)
		Distance_Weight_smacofNormalize (thee.get(), w);
	return thee;
}

autoRatioTransformator RatioTransformator_create (integer numberOfPoints) {
	try {
		autoRatioTransformator me = Thing_new (RatioTransformator);
		Transformator_init (me.get(), numberOfPoints);
		return me;
	} catch (MelderError) {
		Melder_throw (U"RatioTransformator not created.");
	}
}

autoDistance structMonotoneTransformator :: v_transform (MDSVec vec, Distance d, Weight w) {
	try {
		autoDistance thee = MDSVec_Distance_monotoneRegression (vec, d, tiesHandling);
		if (normalization)
			Distance_Weight_smacofNormalize (thee.get(), w);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Distance not created.");
	}
}

autoMonotoneTransformator MonotoneTransformator_create (integer numberOfPoints) {
	try {
		autoMonotoneTransformator me = Thing_new (MonotoneTransformator);
		Transformator_init (me.get(), numberOfPoints);
		my tiesHandling = kMDS_TiesHandling::PRIMARY_APPROACH;
		return me;
	} catch (MelderError) {
		Melder_throw (U"MonotoneTransformator not created.");
	}
}

void MonotoneTransformator_setTiesProcessing (MonotoneTransformator me, kMDS_TiesHandling tiesHandling) {
	my tiesHandling = tiesHandling;
}

autoDistance structISplineTransformator :: v_transform (MDSVec vec, Distance dist, Weight w) {
	const double tol = 1e-6;
	const integer itermax = 20, numberOfProximities = vec -> numberOfProximities;
	const integer nKnots = numberOfInteriorKnots + order + order + 2;

	autoDistance thee = Distance_create (dist -> numberOfRows);
	TableOfReal_copyLabels (dist, thee.get(), 1, -1);

	autoVEC d = zero_VEC (numberOfProximities);

	for (integer i = 1; i <= numberOfProximities; i ++)
		d [i] = dist -> data [vec -> rowIndex [i]] [vec -> columnIndex [i]];
	/*
		Process knots. Put interior knots at quantiles.
		Guarantee that for each proximity x [i]: knot [j] <= x [i] < knot [j+1]
	*/
	for (integer i = 1; i <= order + 1; i ++) {
		knot [i] = vec -> proximity [1];
		knot [nKnots - i + 1] = vec -> proximity [numberOfProximities] * 1.000001;
	}
	for (integer i = 1; i <= numberOfInteriorKnots; i ++) {
		const double fraction = (double) i / (numberOfInteriorKnots + 1);
		knot [order + 1 + i] = NUMquantile (vec -> proximity.get(), fraction);
	}
	/*
		Calculate data matrix m.
	*/
	for (integer iprox = 1; iprox <= numberOfProximities; iprox ++) {
		const double x = vec -> proximity [iprox];
		m [iprox] [1] = 1.0;
		double y;
		for (integer ipar = 2; ipar <= numberOfParameters; ipar ++) {
			try {
				y = NUMispline (knot.get(), order, ipar - 1, x);
			} catch (MelderError) {
				Melder_throw (U"I-spline [", ipar - 1, U"], data [", iprox, U"d] = ", x);
			}
			m [iprox] [ipar] = y;
		}
	}

	our b = solveNonnegativeLeastSquaresRegression_VEC (m.get(), d.get(), itermax, tol, 0);

	for (integer iprox = 1; iprox <= numberOfProximities; iprox ++) {
		const integer ii = vec->rowIndex [iprox];
		const integer jj = vec->columnIndex [iprox];
		const double r = NUMinner (m.row (iprox), b.all());
		thy data [ii] [jj] = thy data [jj] [ii] = r;
	}
	if (normalization)
		Distance_Weight_smacofNormalize (thee.get(), w);
	return thee;
}

autoISplineTransformator ISplineTransformator_create (integer numberOfPoints, integer numberOfInteriorKnots, integer order) {
	try {
		autoISplineTransformator me = Thing_new (ISplineTransformator);
		const integer nData = (numberOfPoints - 1) * numberOfPoints / 2;

		Transformator_init (me.get(), numberOfPoints);
		/*
			1 extra parameter for the intercept.
			2 extra knots for the I-spline.
		*/
		my numberOfParameters = numberOfInteriorKnots + order + 1;
		const integer numberOfKnots = numberOfInteriorKnots + order + order + 2;

		my b = raw_VEC (my numberOfParameters);
		my knot = raw_VEC (numberOfKnots);
		my m = zero_MAT (nData, my numberOfParameters);

		for (integer i = 1; i <= my numberOfParameters; i ++)
			my b [i] = NUMrandomUniform (0.0, 1.0);

		my numberOfInteriorKnots = numberOfInteriorKnots;
		my order = order;
		return me;
	} catch (MelderError) {
		Melder_throw (U"ISplineTransformator not created.");
	}
}

/***************** CONTINGENCYTABLE **************************************/

autoConfiguration ContingencyTable_to_Configuration_ca (ContingencyTable me, integer numberOfDimensions, integer scaling) {
	try {
		const integer nrow = my numberOfRows, ncol = my numberOfColumns;
		const integer dimmin = std::min (nrow, ncol);

		autoMAT h = copy_MAT (my data.get());
		autoVEC rowsum = rowSums_VEC (my data.get());
		autoVEC colsum = columnSums_VEC (my data.get());
		autoConfiguration thee = Configuration_create (nrow + ncol, numberOfDimensions);

		if (numberOfDimensions == 0)
			numberOfDimensions = dimmin - 1;
		Melder_require (numberOfDimensions < dimmin,
			U"Dimension should be lower than ", dimmin, U".");
		/*
			Ref: A. Gifi (1990), Nonlinear Multivariate Analysis, Wiley & Sons, reprinted 1996,
				Chapter 8, Multidimensional scaling and Correspondence Analysis.
				
			Get row and column marginals
		*/
		longdouble sum = 0.0;;
		for (integer irow = 1; irow <= nrow; irow ++) {
			Melder_require (rowsum [irow] > 0.0,
				U"Row number ", irow, U" should not be empty.");
			sum += rowsum [irow];
		}

		for (integer icol = 1; icol <= ncol; icol ++)
			Melder_require (colsum [icol] > 0.0,
				U"Column number ", icol, U" should not be empty.");
		/*
			Remove trivial singular vectors (Eq. 8.24),
			construct Dr^(-1/2) H Dc^(-1/2) - Dr^(1/2) uu' Dc^(1/2) / N
		*/
		for (integer irow = 1; irow <= nrow; irow ++) {
			for (integer icol = 1; icol <= ncol; icol ++) {
				const double rc = sqrt (rowsum [irow] * colsum [icol]);
				h [irow] [icol] = h [irow] [icol] / rc - rc / (double) sum;
			}
		}

		autoSVD svd = SVD_createFromGeneralMatrix (h.get());
		SVD_zeroSmallSingularValues (svd.get(), 0);
		/*
			Scale row vectors and column vectors to configuration.
		*/
		for (integer j = 1; j <= numberOfDimensions; j ++) {
			const double rootsum = sqrt ((double) sum), lambda = svd -> d [j];
			double xfactor, yfactor;
			if (scaling == 1) {
				/*
					Scale row points in the centre of gravity of column points (eq 8.5.a)
				*/
				xfactor = rootsum * lambda;
				yfactor = rootsum;
			} else if (scaling == 2) {
				/*
					Scale column points in the centre of gravity of row points (8.5.b)
				*/
				xfactor = rootsum;
				yfactor = rootsum * lambda;
			} else if (scaling == 3) {
				/*
					Treat row and columns symmetrically (8.5.c).
				*/
				xfactor = yfactor = rootsum * sqrt (lambda);
			} else {
				break;
			}
			for (integer irow = 1; irow <= nrow; irow ++)
				thy data [irow] [j] = svd -> u [irow] [j] * xfactor / sqrt (rowsum [irow]);
			for (integer icol = 1; icol <= ncol; icol ++)
				thy data [nrow + icol] [j] = svd -> v [icol] [j] * yfactor / sqrt (colsum [icol]);
		}

		TableOfReal_setSequentialColumnLabels (thee.get(), 0, 0, nullptr, 1, 1);
		thy rowLabels.part (1, nrow)  <<=  my rowLabels.all();
		thy rowLabels.part (nrow + 1, nrow + ncol)  <<=  my columnLabels.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created.");
	}
}

autoDissimilarity TableOfReal_to_Dissimilarity (TableOfReal me) {
	try {
		Melder_require (my numberOfRows == my numberOfColumns,
			U"The TableOfReal should be square.");
		Melder_require (TableOfReal_isNonNegative (me),
			U"No cell in the table should be negative.");
		autoDissimilarity thee = Thing_new (Dissimilarity);
		my structTableOfReal :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Dissimilarity.");
	}
}

autoSimilarity TableOfReal_to_Similarity (TableOfReal me) {
	try {
		Melder_require (my numberOfRows == my numberOfColumns,
			U"The TableOfReal should be square.");
		Melder_require (TableOfReal_isNonNegative (me),
			U"No cell in the table should be negative.");
		autoSimilarity thee = Thing_new (Similarity);
		my structTableOfReal :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Similarity.");
	}
}

autoDistance TableOfReal_to_Distance (TableOfReal me) {
	try {
		Melder_require (my numberOfRows == my numberOfColumns,
			U"The TableOfReal should be square.");
		Melder_require (TableOfReal_isNonNegative (me),
			U"No cell in the table should be negative.");
		autoDistance thee = Thing_new (Distance);
		my structTableOfReal :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Distance.");
	}
}

autoSalience TableOfReal_to_Salience (TableOfReal me) {
	try {
		Melder_require (TableOfReal_isNonNegative (me),
			U"No cell in the table should be negative.");
		autoSalience thee = Thing_new (Salience);
		my structTableOfReal :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Salience.");
	}
}

autoWeight TableOfReal_to_Weight (TableOfReal me) {
	try {
		Melder_require (TableOfReal_isNonNegative (me),
			U"No cell in the table should be negative.");
		autoWeight thee = Thing_new (Weight);
		my structTableOfReal :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Weight.");
	}
}

autoScalarProduct TableOfReal_to_ScalarProduct (TableOfReal me) {
	try {
		Melder_require (my numberOfRows == my numberOfColumns,
			U"TableOfReal should be a square table.");
		autoScalarProduct thee = Thing_new (ScalarProduct);
		my structTableOfReal :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to ScalarProduct.");
	}
}

/**************** Covariance & Correlation to Configuration *****************/

autoConfiguration SSCP_to_Configuration (SSCP me, integer numberOfDimensions) {
	try {
		autoConfiguration thee = Configuration_create (my numberOfRows, numberOfDimensions);
		autoPCA a = SSCP_to_PCA (me);
		TableOfReal_setSequentialColumnLabels (thee.get(), 0, 0, nullptr, 1, 1);

		for (integer i = 1; i <= my numberOfRows; i ++) {
			for (integer j = 1; j <= numberOfDimensions; j ++)
				thy data [i] [j] = NUMinner (my data.column (i), a -> eigenvectors.column (j));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created.");
	}
}

autoConfiguration Covariance_to_Configuration (Covariance me, integer numberOfDimensions) {
	return SSCP_to_Configuration (me, numberOfDimensions);
}

autoConfiguration Correlation_to_Configuration (Correlation me, integer numberOfDimensions) {
	return SSCP_to_Configuration (me, numberOfDimensions);
}

/**************************** Weight *****************************************/

autoWeight Weight_create (integer numberOfPoints) {
	try {
		autoWeight me = Thing_new (Weight);
		TableOfReal_init (me.get(), numberOfPoints, numberOfPoints);
		my data.all()  <<=  1.0;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Weight not created.");
	}
}


/**************** Salience *****************************************/

autoSalience Salience_create (integer numberOfSources, integer numberOfDimensions) {
	try {
		autoSalience me = Thing_new (Salience);
		TableOfReal_init (me.get(), numberOfSources, numberOfDimensions);
		Salience_setDefaults (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Salience not created.");
	}
}

integer Salience_correctNegatives (Salience me) {
	/*
		The weights might be negative.
		We correct this by simply making them positive.
	*/
	integer nNegatives = 0;
	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			if (my data [i] [j] < 0.0) {
				nNegatives ++;
				my data [i] [j] = - my data [i] [j];
			}
		}
	}
	return nNegatives;
}

void Salience_setDefaults (Salience me) {
	my data.all()  <<=  1.0 / sqrt (my numberOfColumns);
	for (integer j = 1; j <= my numberOfColumns; j ++)
		TableOfReal_setColumnLabel (me, j, Melder_cat (U"dimension ", j));
}

void Salience_draw (Salience me, Graphics g, integer ix, integer iy, bool garnish) {
	integer nc2, nc1 = ( ix < iy ? (nc2 = iy, ix) : (nc2 = ix, iy) );

	if (ix < 1 || ix > my numberOfColumns || iy < 1 || iy > my numberOfColumns)
		return;

	double wmax = 1.0;
	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = nc1; j <= nc2; j ++)
			if (my data [i] [j] > wmax)
				wmax = my data [i] [j];
	}
	const double xmin = 0.0, ymin = 0.0;
	double ymax, xmax = ymax = wmax;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);

	for (integer i = 1; i <= my numberOfRows; i ++)
		if (my rowLabels [i])
			Graphics_text (g, my data [i] [ix], my data [i] [iy], my rowLabels [i].get());

	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
	Graphics_line (g, xmin, ymax, xmin, ymin);
	Graphics_line (g, xmin, ymin, xmax, ymin);
	/* Graphics_arc (g, xmin, ymin, xmax - xmin, 0, 90); */
	Graphics_unsetInner (g);

	if (garnish) {
		if (my columnLabels [ix])
			Graphics_textBottom (g, false, my columnLabels [ix].get());
		if (my columnLabels [iy])
			Graphics_textLeft (g, false, my columnLabels [iy].get());
	}
}

/**************************  COonfusionList **************************************/

autoConfusion ConfusionList_sum (ConfusionList me) {
	try {
		autoTableOfReal sum = TableOfRealList_sum (my asTableOfRealList());
		autoConfusion thee = TableOfReal_to_Confusion (sum.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": sum not created.");
	}
}

/*****************  ScalarProduct ***************************************/

autoScalarProduct ScalarProduct_create (integer numberOfPoints) {
	try {
		autoScalarProduct me = Thing_new (ScalarProduct);
		TableOfReal_init (me.get(), numberOfPoints, numberOfPoints);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ScalarProduct not created.");
	}
}

autoSimilarity Confusion_to_Similarity (Confusion me, bool normalize, integer symmetrizeMethod) {
	try {
		Melder_require (my numberOfColumns == my numberOfRows,
			U"Confusion should be a square table.");

		const integer nxy = my numberOfColumns;
		autoSimilarity thee = Similarity_create (nxy);

		TableOfReal_copyLabels (me, thee.get(), 1, 1);

		thy data.all()  <<=  my data.all();

		if (normalize)
			MAT_divideRowByRowsum_inplace (thy data.get());

		if (symmetrizeMethod == 1)
			return thee;

		if (symmetrizeMethod == 2) {   // average data
			for (integer i = 1; i <= nxy - 1; i ++)
				for (integer j = i + 1; j <= nxy; j ++)
					thy data [i] [j] = thy data [j] [i] = (thy data [i] [j] + thy data [j] [i]) / 2;
		} else if (symmetrizeMethod == 3) {   // method Houtgast
			autoMAT p = copy_MAT (thy data.get());
			for (integer i = 1; i <= nxy; i ++) {
				for (integer j = i; j <= nxy; j ++) {
					longdouble tmp = 0;
					for (integer k = 1; k <= nxy; k ++)
						tmp += std::min (p [i] [k], p [j] [k]);
					thy data [j] [i] = thy data [i] [j] = (double) tmp;
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Similarity created.");
	}
}

autoDissimilarity Similarity_to_Dissimilarity (Similarity me, double maximumDissimilarity) {
	try {
		const integer nxy = my numberOfColumns;
		autoDissimilarity thee = Dissimilarity_create (nxy);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		thy data.all()  <<=  my data.all();

		double max = 0.0;
		for (integer i = 1; i <= nxy; i ++) {
			for (integer j = 1; j <= nxy; j ++) {
				if (thy data [i] [j] > max) {
					max = thy data [i] [j];
				}
			}
		}

		if (maximumDissimilarity <= 0.0)
			maximumDissimilarity = max;

		if (maximumDissimilarity < max)
			Melder_warning (U"Your maximumDissimilarity is smaller than the maximum similarity. Some data may be lost.");

		for (integer i = 1; i <= nxy; i ++) {
			for (integer j = 1; j <= nxy; j ++) {
				const double d = maximumDissimilarity - thy data [i] [j];
				thy data [i] [j] = std::max (d, 0.0);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Dissimilarity created.");
	}
}


autoWeight Dissimilarity_to_Weight (Dissimilarity me) {
	try {
		autoWeight thee = Weight_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		for (integer i = 1; i <= my numberOfRows; i ++) {
			for (integer j = i; j <= my numberOfRows; j ++) {
				if (my data [i] [j] > 0.0)
					thy data [i] [j] = 1.0;
			}
			thy data [i] [i] = 0.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Weight created.");
	}
}


autoDissimilarity Confusion_to_Dissimilarity_pdf (Confusion me, double minimumConfusionLevel) {
	try {
		Melder_require (my numberOfColumns == my numberOfRows,
			U"Confusion should be a square table.");
		Melder_require (minimumConfusionLevel > 0.0 && minimumConfusionLevel < 1.0,
			U"The minimum confusion level should be positive and smaller than 1.0.");

		autoDissimilarity thee = Dissimilarity_create (my numberOfColumns);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		thy data.all()  <<=  my data.all();
		/*
			Set all zero responses to the minimumConfusionLevel.
		*/
		for (integer i = 1; i <= my numberOfColumns; i ++)
			for (integer j = 1; j <= my numberOfColumns; j ++)
				if (thy data [i] [j] == 0.0)
					thy data [i] [j] = minimumConfusionLevel;

		MAT_divideRowByRowsum_inplace (thy data.get());

		/*
			Consider the fraction as the fraction overlap between two gaussians with unequal sigmas (1 & s).
			We have two matrix elements p [i] [j] && p [j] [i]

			N (x, m, s) = 1 / (s * sqrt(2 pi)) exp(-((x - m) / s)^2 / 2)
			N1 (x) = N (x, 0, 1)
			N2 (x) = N (x, m, s)
			These two gaussians cross each other at a point X that can be found
			by solving N1 (x) == N2 (x).
			The solution that is important to us is:
				X = (- m + s sqrt (m^2 - 2 (s^2 - 1) ln(s))) / (s^2 - 1)  (1)

			This point X must be the solution of F (X, 0, 1) == p [i] [j], where
				F (x, m, s) = P (x>X, m, s) = Integral (x, infinity, N (x, m, s) dx)

			We can solve for m and obtain:
				m = X + s sqrt (X^2 + 2 ln (s))                           (2)

			We also have
				Integral (-Infinity, X, N2 (x) dx) == p [j] [i];
			By changing integration variables, (x - m) / s = y, we get
				Integral ((x-m)/s, Infinity, N (y, 0, 1) dy) == p [j] [i]
			Let this equation result in a value Y, i.e., (X - m) / s = Y  (3)

			(2) and (3) together and solve for m:

				m = X + Y * exp ((Y * Y - X * X) / 2);

			Make maximum dissimilarity equal to 4sigma.
		*/

		for (integer i = 1; i <= my numberOfColumns; i ++) {
			for (integer j = i + 1; j <= my numberOfColumns; j ++) {
				const double px = std::min (thy data [i] [j], thy data [j] [i]);
				const double py = std::max (thy data [i] [j], thy data [j] [i]);
				const double x = NUMinvGaussQ (px);
				const double y = NUMinvGaussQ (py);
				const double d = x + y * exp ((y * y - x * x) / 2.0);
				thy data [i] [j] = thy data [j] [i] = d;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Dissimilarity created from pdf.");
	}
}

void Distance_Configuration_drawScatterDiagram (Distance me, Configuration him, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, conststring32 mark, bool garnish) {
	autoDistance dist = Configuration_to_Distance (him);
	Proximity_Distance_drawScatterDiagram (me, dist.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

autoConfiguration Distance_to_Configuration_torsca (Distance me, integer numberOfDimensions) {
	try {
		Melder_require (numberOfDimensions <= my numberOfRows,
			U"Number of dimensions should not exceed ", my numberOfRows, U".");
		autoScalarProduct sp = Distance_to_ScalarProduct (me, false);
		autoConfiguration thee = Configuration_create (my numberOfRows, numberOfDimensions);
		TableOfReal_copyLabels (me, thee.get(), 1, 0);
		MAT_asPrincipalComponents_preallocated (thy data.get(), sp -> data.get(), numberOfDimensions);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (torsca method).");
	}
}

autoScalarProduct Distance_to_ScalarProduct (Distance me, bool normalize) {
	try {
		autoScalarProduct thee = ScalarProduct_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		for (integer i = 1; i <= my numberOfRows - 1; i ++) {
			for (integer j = i + 1; j <= my numberOfColumns; j ++) {
				/*
					Force symmetry by averaging!
				*/
				const double d = 0.5 * (my data [i] [j] + my data [j] [i]);
				thy data [i] [j] = thy data [j] [i] = - 0.5 * d * d;
			}
			// thy data [i] [i] = 0.0; // redundant
		}
		TableOfReal_doubleCentre (thee.get());
		
		if (my name)
			Thing_setName (thee.get(), my name.get());
		if (normalize)
			TableOfReal_normalizeTable (thee.get(), 1.0);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no ScalarProduct created.");
	}
}

/**********  Configuration & ..... ***********************************/



void Proximity_Distance_drawScatterDiagram (Proximity me, Distance thee, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double size_mm, conststring32 mark, bool garnish) {
	const integer n = my numberOfRows * (my numberOfRows - 1) / 2;

	if (n == 0)
		return;

	Melder_require (NUMequal (my rowLabels.get(), thy rowLabels.get()) &&
					NUMequal (my columnLabels.get(), thy columnLabels.get()),
		U"The labels should be the same.");
	
	constMAT x = my data.get(), y = thy data.get();
	if (xmax <= xmin) {
		MelderExtremaWithInit extrema;
		for (integer i = 1; i <= thy numberOfRows - 1; i ++)
			for (integer j = i + 1; j <= thy numberOfColumns; j ++)
				extrema.update (x [i] [j]);
		xmin = extrema.min;
		xmax = extrema.max;
	}
	if (ymax <= ymin) {
		MelderExtremaWithInit extrema;
		for (integer i = 1; i <= my numberOfRows - 1; i ++)
			for (integer j = i + 1; j <= my numberOfColumns; j ++)
				extrema.update (y [i] [j]);
		ymin = extrema.min;
		ymax = extrema.max;
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	for (integer i = 1; i <= thy numberOfRows - 1; i ++)
		for (integer j = i + 1; j <= thy numberOfColumns; j ++)
			if (x [i] [j] >= xmin && x [i] [j] <= xmax && y [i] [j] >= ymin && y [i] [j] <= ymax)
				Graphics_mark (g, x [i] [j], y [i] [j], size_mm, mark);

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, true, U"Distance");
		Graphics_textBottom (g, true, U"Dissimilarity");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

autoDistanceList MDSVecList_Distance_monotoneRegression (MDSVecList me, Distance thee, kMDS_TiesHandling tiesHandling) {
	try {
		autoDistanceList him = DistanceList_create ();
		for (integer i = 1; i <= my size; i ++) {
			const MDSVec vec = my at [i];
			Melder_require (vec -> numberOfPoints == thy numberOfRows,
				U"Dimension of MDSVec and Distance should be equal.");
			autoDistance fit = MDSVec_Distance_monotoneRegression (vec, thee, tiesHandling);
			his addItem_move (fit.move());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"No DistanceList created from MDSVecList and Distance.");
	}
}

autoDistance MDSVec_Distance_monotoneRegression (MDSVec me, Distance thee, kMDS_TiesHandling tiesHandling) {
	try {
		Melder_require (my numberOfPoints == thy numberOfColumns,
			U"The dimensions of the Distance and the MDSVec should agree.");
		Melder_require (thy numberOfRows == my numberOfPoints,
			U"Distance and MDSVVec dimensions should agreee.");
		const integer numberOfProximities = my numberOfProximities;
		autoVEC distances = raw_VEC (numberOfProximities);
		autoDistance him = Distance_create (thy numberOfRows);
		TableOfReal_copyLabels (thee, him.get(), 1, 1);

		for (integer i = 1; i <= numberOfProximities; i ++)
			distances [i] = thy data [my rowIndex [i]] [my columnIndex [i]];

		if (tiesHandling == kMDS_TiesHandling::PRIMARY_APPROACH || tiesHandling == kMDS_TiesHandling::SECONDARY_APPROACH) {
			/*
				Kruskal's primary approach to tie-blocks:
					Sort corresponding distances, with rowIndex, and columnIndex.
				Kruskal's secondary approach:
					Substitute average distance in each tie block
			*/
			integer ib = 1;
			for (integer i = 2; i <= numberOfProximities; i ++) {
				if (my proximity [i] == my proximity [i - 1])
					continue;
				if (i - ib > 1) {
					if (tiesHandling == kMDS_TiesHandling::PRIMARY_APPROACH) {
						// all equal
					} else if (tiesHandling == kMDS_TiesHandling::SECONDARY_APPROACH) {
						const double mean = NUMmean (distances.part (ib, i - 1));
						distances.part (ib, i - 1)  <<=  mean;
					}
				}
				ib = i;
			}
		}

		autoVEC fit = newVECmonotoneRegression (distances.get());
		/*
			Fill Distance with monotone regressed distances
		*/
		for (integer i = 1; i <= numberOfProximities; i ++) {
			const integer irow = my rowIndex [i], icol = my columnIndex [i];
			his data [irow] [icol] = his data [icol] [irow] = fit [i];
		}
		/*
			Make rest of distances equal to the maximum fit.
		*/
		for (integer i = 1; i <= his numberOfRows - 1; i ++) {
			for (integer j = i + 1; j <= his numberOfColumns; j ++) {
				if (his data [i] [j] == 0.0)
					his data [i] [j] = his data [j] [i] = fit [numberOfProximities];
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"Distance not created.");
	}
}


autoDistance Dissimilarity_Distance_monotoneRegression (Dissimilarity me, Distance thee, kMDS_TiesHandling tiesHandling) {
	try {
		Melder_require (thy numberOfRows == my numberOfRows, U"Dimensions should agree.");
		autoMDSVec vec = Dissimilarity_to_MDSVec (me);
		autoDistance him = MDSVec_Distance_monotoneRegression (vec.get(), thee, tiesHandling);
		return him;
	} catch (MelderError) {
		Melder_throw (U"Distance not created.");
	}
}

/*************** class Proximities **************************************/

autoScalarProductList DistanceList_to_ScalarProductList (DistanceList me, bool normalize) {
	try {
		autoScalarProductList thee = ScalarProductList_create ();
		for (integer i = 1; i <= my size; i ++) {
			autoScalarProduct sp = Distance_to_ScalarProduct (my at [i], normalize);
			thy addItem_move (sp.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no ScalarProducts created.");
	}
}

void DistanceList_to_Configuration_ytl (DistanceList me, integer numberOfDimensions, integer normalizeScalarProducts, autoConfiguration *out1, autoSalience *out2) {
	try {
		autoScalarProductList sp = DistanceList_to_ScalarProductList (me, normalizeScalarProducts);
		ScalarProductList_to_Configuration_ytl (sp.get(), numberOfDimensions, out1, out2);
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (ytl method).");
	}
}

/*
	Algorithm : F. Young, Y. Takane & R. Lewyckyj (1978), "Three notes on ALSCAL", 
	Psychometrika 43, 433-435.
*/
void ScalarProductList_to_Configuration_ytl (ScalarProductList me, integer numberOfDimensions, autoConfiguration *out1, autoSalience *out2) {
	try {
		const integer numberOfSources = my size;
		Melder_require (numberOfSources > 1,
			U"The number of sources should exceed one.");
		const integer nPoints = my at [1] -> numberOfRows;

		autoConfiguration thee = Configuration_create (nPoints, numberOfDimensions);
		autoSalience mdsw = Salience_create (numberOfSources, numberOfDimensions);
		TableOfReal_copyLabels (my at [1], thee.get(), 1, 0);

		Thing_setName (mdsw.get(), U"ytl");
		Thing_setName (thee.get(), U"ytl");
		TableOfReal_setLabelsFromCollectionItemNames (mdsw.get(), (Collection) me, true, false);   // FIXME cast

		/*
			Determine the average scalar product matrix (Pmean) of dimension [1..nPoints] [1..nPoints].
		*/		
		autoMAT pmean = zero_MAT (nPoints, nPoints);

		for (integer i = 1; i <= numberOfSources; i ++) {
			ScalarProduct sp = my at [i];
			Melder_require (sp -> numberOfRows == nPoints,
				U"The dimension of ScalarProduct ", i, U" does not conform.");
			pmean.all()  +=  sp -> data.all();
		}
		
		pmean.all()  *=  1.0 / numberOfSources;

		/*
			Up to a rotation K, the initial configuration can be found by
			extracting the first 'numberOfDimensions' principal components of Pmean.
		*/
		
		autoMAT y = MAT_asPrincipalComponents (pmean.get (), numberOfDimensions);

		thy data.all()  <<=  y.all();
		
		/*
			Calculate the C [i] matrices [1..numberOfDimensions] [1..numberOfDimensions]
			from the P [i] by: C [i] = (y'.y)^-1 . y' . P [i] . y . (y'.y)^-1 == yinv P [i] yinv'
		*/
		
		autoMAT yinv = newMATpseudoInverse (y.get(), 1e-14);
		autoTEN3 ci = raw_TEN3 (numberOfSources, numberOfDimensions, numberOfDimensions);

		for (integer i = 1; i <= numberOfSources; i ++) {
			const ScalarProduct sp = my at [i];
			MATmul3_XYXt (ci [i], yinv.get(), sp -> data.get()); //yinv.Data.yinv'
		}

		/*
			Calculate the A [1..numberOfSources] [1..numberOfSources] matrix by (eq.12):
			a [i] [j] = trace (C [i]*C [j]) - trace (C [i]) * trace (C [j]) / numberOfDimensions;
			Get the first eigenvector and form matrix cl from a linear combination of the C [i]'s
		*/
		
		autoMAT a = raw_MAT (numberOfSources, numberOfSources);

		for (integer i = 1; i <= numberOfSources; i ++) {
			for (integer j = i; j <= numberOfSources; j ++) {
				a [j] [i] = a [i] [j] =  NUMtrace2 (ci [i], ci [j])
					- NUMtrace (ci [i]) * NUMtrace (ci [j]) / numberOfDimensions;
			}
		}
		
		autoMAT evec;
		
		MAT_getEigenSystemFromSymmetricMatrix (a.get(), & evec, nullptr, false);
		
		autoMAT cl = zero_MAT (numberOfDimensions, numberOfDimensions);
		for (integer i = 1; i <= numberOfSources; i ++) {
			for (integer j = 1; j <= numberOfDimensions; j ++) {
				for (integer k = 1; k <= numberOfDimensions; k ++) {
					cl [j] [k] += ci [i] [j] [k] * evec [i] [1]; /* eq. (7) TODO first column??*/
				}
			}
		}

		/*
			The rotation K is obtained from the eigenvectors of cl
			Is the following still correct??? eigensystem was not sorted??
		*/

		autoMAT K;
		MAT_getEigenSystemFromSymmetricMatrix (cl.get(), & K, nullptr, false);

		mul_MAT_out (thy data.get(), y.get(), K.get()); // Y.K

		Configuration_normalize (thee.get(), 0, true);

		/*
			And finally the weights: W [i] = K' C [i] K   (eq. (5)).
			We are only interested in the diagonal of the resulting matrix W [i].
		*/

		for (integer i = 1; i <= numberOfSources; i ++) {
			for (integer j = 1; j <= numberOfDimensions; j ++) {
				longdouble wt = 0.0;
				for (integer k = 1; k <= numberOfDimensions; k ++) {
					if (K [k] [j] != 0.0) {
						for (integer l = 1; l <= numberOfDimensions; l ++) {
							wt += K [k] [j] * ci [i] [k] [l] * K [l] [j];
						}
					}
				}
				mdsw -> data [i] [j] = (double) wt;
			}
		}
		
		if (out1)
			*out1 = thee.move();
		if (out2)
			*out2 = mdsw.move();
		
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration (ytl) created.");
	}
}

autoDissimilarityList DistanceList_to_DissimilarityList (DistanceList me) {
	try {
		autoDissimilarityList thee = DissimilarityList_create ();
		for (integer i = 1; i <= my size; i ++) {
			const conststring32 name = Thing_getName (my at [i]);
			autoDissimilarity him = Distance_to_Dissimilarity (my at [i]);
			Thing_setName (him.get(), ( name ? name : U"untitled" ));
			thy addItem_move (him.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no DissimilaryList created.");
	}
}

/*****************  Kruskal *****************************************/

static void smacof_guttmanTransform (Configuration cx, Configuration cz, Distance disp, Weight weight, constMAT vplus) {
	const integer nPoints = cx -> numberOfRows, nDimensions = cx -> numberOfColumns;

	autoMAT b = raw_MAT (nPoints, nPoints);
	autoDistance distZ = Configuration_to_Distance (cz);
	/*
		compute B(Z) (eq. 8.25)
	*/
	for (integer i = 1; i <= nPoints; i ++) {
		longdouble sum = 0.0;
		for (integer j = 1; j <= nPoints; j ++) {
			const double dzij = distZ -> data [i] [j];
			if (i == j || dzij == 0.0)
				continue;
			b [i] [j] = - weight -> data [i] [j] * disp -> data [i] [j] / dzij;
			sum += b [i] [j];
		}
		b [i] [i] = - (double) sum;
	}
	/*
		Guttman transform: Xu = (V+)B(Z)Z (eq. 8.29)
	*/
	for (integer i = 1; i <= nPoints; i ++) {
		for (integer j = 1; j <= nDimensions; j ++) {
			longdouble xij = 0.0;
			for (integer k = 1;  k <= nPoints; k ++) {
				for (integer l = 1; l <= nPoints; l ++) {
					xij += vplus [i] [k] * b [k] [l] * cz -> data [l] [j];
				}
			}
			cx -> data [i] [j] = (double) xij;
		}
	}
}

double Distance_Weight_stress (Distance fit, Distance conf, Weight weight, kMDS_stressMeasure stressMeasure) {
	double stress = undefined;

	double eta_fit, eta_conf, rho;
	Distance_Weight_rawStressComponents (fit, conf, weight, & eta_fit, & eta_conf, & rho);

	/*
		All formulas for stress, except for raw stress, are independent of the
		scale of the configuration, i.e., the distances conf [i] [j].
	*/

	if (stressMeasure == kMDS_stressMeasure::NORMALIZED) {
		const double denum = eta_fit * eta_conf;
		if (denum > 0.0)
			stress = 1.0 - rho * rho / denum;
	} else if (stressMeasure == kMDS_stressMeasure::KRUSKAL_1) {
		const double denum = eta_fit * eta_conf;
		if (denum > 0.0) {
			const double tmp = 1.0 - rho * rho / denum;
			if (tmp > 0.0)
				stress = sqrt (tmp);
		}
	} else if (stressMeasure == kMDS_stressMeasure::KRUSKAL_2) {
		/*
			Get average distance.
		*/
		const integer nPoints = conf -> numberOfRows;
		longdouble m = 0.0, wsum = 0.0;
		for (integer i = 1; i <= nPoints - 1; i ++) {
			m += NUMinner (weight -> data.row (i).part (i + 1, nPoints),
					conf -> data.row (i).part (i + 1, nPoints));
			wsum += NUMsum (weight -> data.row (i).part (i + 1, nPoints));
		}
		m /= wsum;
		if (m > 0.0) {
			/*
				Get variance.
			*/
			longdouble var = 0.0;
			for (integer i = 1; i <= nPoints - 1; i ++) {
				for (integer j = i + 1; j <= nPoints; j ++) {
					const longdouble tmp = conf -> data [i] [j] - m;
					var += weight -> data [i] [j] * tmp * tmp;
				}
			}
			const double denum = double (var) * eta_fit;
			if (denum > 0.0)
				stress = sqrt ((eta_fit * eta_conf - rho * rho) / denum);
		}
	} else if (stressMeasure == kMDS_stressMeasure::RAW) {
		stress = eta_fit + eta_conf - 2.0 * rho;
	}
	return stress;
}

void Distance_Weight_rawStressComponents (Distance fit, Distance conf, Weight weight, double *out_etafit, double *out_etaconf, double *out_rho)
{
	const integer nPoints = conf -> numberOfRows;
	longdouble etafit = 0.0, etaconf = 0.0, rho = 0.0;
	for (integer i = 1; i <= nPoints - 1; i ++) {
		constVEC wi = weight -> data.row (i);
		constVEC fiti = fit -> data.row (i);
		constVEC confi = conf -> data.row (i);
		for (integer j = i + 1; j <= nPoints; j ++) {
			etafit += wi [j] * fiti [j] * fiti [j];
			etaconf += wi [j] * confi [j] * confi [j];
			rho += wi [j] * fiti [j] * confi [j];
		}
	}
	if (out_etafit)
		*out_etafit = (double) etafit;
	if (out_etaconf)
		*out_etaconf = (double) etaconf;
	if (out_rho)
		*out_rho = (double) rho;
}

double Dissimilarity_Configuration_Transformator_Weight_stress (Dissimilarity d, Configuration c, Transformator t, Weight w, kMDS_stressMeasure stressMeasure) {
	const integer nPoints = d -> numberOfRows;
	double stress = undefined;
	Melder_require (nPoints > 0 && nPoints == c -> numberOfRows && nPoints == t -> numberOfPoints ||
		(w && nPoints == w -> numberOfRows),
			U"Dimensions should agree.");

	autoWeight aw;
	if (! w) {
		aw = Weight_create (nPoints);
		w = aw.get();
	}
	autoDistance cdist = Configuration_to_Distance (c);
	autoMDSVec mdsvec = Dissimilarity_to_MDSVec (d);
	autoDistance fit = Transformator_transform (t, mdsvec.get(), cdist.get(), w);

	stress = Distance_Weight_stress (fit.get(), cdist.get(), w, stressMeasure);
	return stress;
}

double Dissimilarity_Configuration_Weight_absolute_stress (Dissimilarity d, Configuration c, Weight w, kMDS_stressMeasure stressMeasure) {
	autoTransformator t = Transformator_create (d -> numberOfRows);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.get(), w, stressMeasure);
	return stress;
}

double Dissimilarity_Configuration_Weight_ratio_stress (Dissimilarity d, Configuration c, Weight w, kMDS_stressMeasure stressMeasure) {
	autoRatioTransformator t = RatioTransformator_create (d -> numberOfRows);
	const double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.get(), w, stressMeasure);
	return stress;
}

double Dissimilarity_Configuration_Weight_interval_stress (Dissimilarity d, Configuration c, Weight w, kMDS_stressMeasure stressMeasure) {
	autoISplineTransformator t = ISplineTransformator_create (d -> numberOfRows, 0, 1);
	const double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.get(), w, stressMeasure);
	return stress;
}

double Dissimilarity_Configuration_Weight_monotone_stress (Dissimilarity d, Configuration c, Weight w, kMDS_TiesHandling tiesHandling, kMDS_stressMeasure stressMeasure) {
	autoMonotoneTransformator t = MonotoneTransformator_create (d -> numberOfRows);
	MonotoneTransformator_setTiesProcessing (t.get(), tiesHandling);
	const double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.get(), w, stressMeasure);
	return stress;
}

double Dissimilarity_Configuration_Weight_ispline_stress (Dissimilarity d, Configuration c, Weight w, integer numberOfInteriorKnots, integer order, kMDS_stressMeasure stressMeasure) {
	autoISplineTransformator t = ISplineTransformator_create (d -> numberOfRows, numberOfInteriorKnots, order);
	const double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.get(), w, stressMeasure);
	return stress;
}

void Distance_Weight_smacofNormalize (Distance me, Weight w) {
	longdouble sumsq = 0.0;
	for (integer i = 1; i <= my numberOfRows - 1; i ++)
		for (integer j = i + 1; j <= my numberOfRows; j ++)
			sumsq += w -> data [i] [j] * my data [i] [j] * my data [i] [j];

	const double scale = sqrt (my numberOfRows * (my numberOfRows - 1) / double (2.0 * sumsq));
	my data.all()  *=  scale;
}

double Distance_Weight_congruenceCoefficient (Distance x, Distance y, Weight w) {
	const integer nPoints = x -> numberOfRows;
	if (y -> numberOfRows != nPoints || w -> numberOfRows != nPoints)
		return 0.0;

	double xy = 0.0, x2 = 0.0, y2 = 0.0;
	for (integer i = 1; i <= nPoints - 1; i ++) {
		constVEC xi = x -> data.row (i);
		constVEC yi = y -> data.row (i);
		constVEC wi = w -> data.row (i);
		for (integer j = i + 1; j <= nPoints - 1; j ++) {
			xy += wi [j] * xi [j] * yi [j];
			x2 += wi [j] * xi [j] * xi [j];
			y2 += wi [j] * yi [j] * yi [j];
		}
	}
	return xy / (sqrt (x2) * sqrt (y2));
}

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_smacof (Dissimilarity me, Configuration conf, Weight weight, Transformator t, double tolerance, integer numberOfIterations, bool showProgress, double *out_stress) {
	try {
		const integer nPoints = conf -> numberOfRows;
		Melder_require (my numberOfRows == nPoints && t -> numberOfPoints == nPoints ||
			(weight && weight -> numberOfRows == nPoints),
				U"Dimensions should agree.");

		autoWeight aw;
		if (! weight) {
			aw = Weight_create (nPoints);
			weight = aw.get();
		}
		autoMAT v = raw_MAT (nPoints, nPoints);
		autoConfiguration z = Data_copy (conf);
		autoMDSVec vec = Dissimilarity_to_MDSVec (me);

		if (showProgress)
			Melder_progress (0.0, U"MDS analysis");

		// Get V (eq. 8.19).

		for (integer irow = 1; irow <= nPoints; irow ++) {
			longdouble wsum = 0.0;
			for (integer icol = 1; icol <= nPoints; icol ++) {
				if (irow != icol) {
					v [irow] [icol] = -  weight -> data [irow] [icol];
					wsum +=  weight -> data [irow] [icol];
				}
			}
			v [irow] [irow] = (double) wsum;
		}

		/*
			V is row and column centered and therefore: rank(V) <= nPoints-1.
			V^-1 does not exist -> get Moore-Penrose inverse.
		*/
		constexpr double tol = 1e-6;
		autoMAT vplus = newMATpseudoInverse (v.get(), tol);
		double stressp = 1e308, stress = 0.0;
		for (integer iter = 1; iter <= numberOfIterations; iter ++) {
			autoDistance dist = Configuration_to_Distance (conf);
			/*
				transform & normalization
			*/
			autoDistance fit = Transformator_transform (t, vec.get(), dist.get(), weight);
			/*
				Make conf the Guttman transform of z
			*/
			smacof_guttmanTransform (conf, z.get(), fit.get(), weight, vplus.get());
			/*
				Compute stress
			*/
			autoDistance cdist = Configuration_to_Distance (conf);

			stress = Distance_Weight_stress (fit.get(), cdist.get(), weight, kMDS_stressMeasure::NORMALIZED);
			/*
				Check stop criterium
			*/
			if (fabs (stress - stressp) / stressp < tolerance) break;
			/*
				Make Z = X
			*/
			z -> data.all()  <<=  conf -> data.all();

			stressp = stress;
			if (showProgress)
				Melder_progress ((double) iter / (numberOfIterations + 1), U"kruskal: stress ", stress);
		}
		if (showProgress)
			Melder_progress (1.0);
		if (out_stress)
			*out_stress = stress;
		return z;
	} catch (MelderError) {
		if (showProgress)
			Melder_progress (1.0);
		Melder_throw (me, U": no improved Configuration created (smacof method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_multiSmacof (Dissimilarity me, Configuration conf,  Weight w, Transformator t, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	bool showMulti = showProgress && numberOfRepetitions > 1;
	try {
		const bool showSingle = showProgress && numberOfRepetitions == 1;
		autoConfiguration cstart = Data_copy (conf);
		autoConfiguration  cbest = Data_copy (conf);

		if (showMulti)
			Melder_progress (0.0, U"MDS many times");

		double stress, stressmax = 1e308;
		for (integer i = 1; i <= numberOfRepetitions; i ++) {
			autoConfiguration cresult = Dissimilarity_Configuration_Weight_Transformator_smacof (me, cstart.get(), w, t, tolerance, numberOfIterations, showSingle, & stress);
			if (stress < stressmax) {
				stressmax = stress;
				cbest = cresult.move();
			}
			Configuration_randomize (cstart.get());
			TableOfReal_centreColumns (cstart.get());

			if (showMulti)
				Melder_progress ((double) i / (numberOfRepetitions + 1), i, U" from ", numberOfRepetitions);
		}
		if (showMulti)
			Melder_progress (1.0);
		return cbest;
	} catch (MelderError) {
		if (showMulti)
			Melder_progress (1.0);
		Melder_throw (me, U": no improved Configuration created (smacodf method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_absolute_mds (Dissimilarity me, Configuration cstart, Weight w, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	try {
		autoTransformator t = Transformator_create (my numberOfRows);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.get(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no improved Configuration created (absolute mds method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_ratio_mds (Dissimilarity me, Configuration cstart, Weight w, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	try {
		autoRatioTransformator t = RatioTransformator_create (my numberOfRows);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.get(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no improved Configuration created (ratio mds method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_interval_mds (Dissimilarity me, Configuration cstart, Weight w, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	try {
		autoISplineTransformator t = ISplineTransformator_create (my numberOfRows, 0, 1);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.get(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no improved Configuration created (interval mds method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_monotone_mds (Dissimilarity me, Configuration cstart, Weight w, kMDS_TiesHandling tiesHandling, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	try {
		autoMonotoneTransformator t = MonotoneTransformator_create (my numberOfRows);
		MonotoneTransformator_setTiesProcessing (t.get(), tiesHandling);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.get(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no improved Configuration created (monotone mds method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_ispline_mds (Dissimilarity me, Configuration cstart, Weight w, integer numberOfInteriorKnots, integer order, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	try {
		autoISplineTransformator t = ISplineTransformator_create (my numberOfRows, numberOfInteriorKnots, order);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.get(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no improved Configuration created (ispline mds method).");
	}
}

autoConfiguration Dissimilarity_Weight_absolute_mds (Dissimilarity me, Weight w, integer numberOfDimensions, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, kMDS_AnalysisScale::ABSOLUTE_);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_absolute_mds (me, cstart.get(), w, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (absolute mds method).");
	}
}

autoConfiguration Dissimilarity_Weight_interval_mds (Dissimilarity me, Weight w, integer numberOfDimensions, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, kMDS_AnalysisScale::RATIO);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_interval_mds (me, cstart.get(), w, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (interval mds method).");
	}
}

autoConfiguration Dissimilarity_Weight_monotone_mds (Dissimilarity me, Weight w, integer numberOfDimensions, kMDS_TiesHandling tiesHandling, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, kMDS_AnalysisScale::ORDINAL);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_monotone_mds (me, cstart.get(), w, tiesHandling, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (monotone mds method).");
	}
}

autoConfiguration Dissimilarity_Weight_ratio_mds (Dissimilarity me, Weight w, integer numberOfDimensions, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, kMDS_AnalysisScale::RATIO);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_ratio_mds (me, cstart.get(), w, tolerance,
		    numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (ratio mds method).");
	}
}

autoConfiguration Dissimilarity_Weight_ispline_mds (Dissimilarity me, Weight w, integer numberOfDimensions, integer numberOfInteriorKnots, integer order, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, kMDS_AnalysisScale::ORDINAL);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_ispline_mds (me, cstart.get(), w,
		    numberOfInteriorKnots, order, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (ispline mds method).");
	}
}

/***** classical **/

static void MDSVec_Distances_getStressValues (MDSVec me, Distance ddist, Distance dfit, kMDS_KruskalStress stress_formula, double *out_stress, double *out_s, double *out_t, double *out_dbar) {
	
	longdouble s = 0.0, t = 0.0, dbar = 0.0;

	if (stress_formula == kMDS_KruskalStress::KRUSKAL_1) {
		for (integer i = 1; i <= my numberOfProximities; i ++) {
			dbar += ddist -> data [my rowIndex [i]] [my columnIndex [i]];
		}
		dbar /= my numberOfProximities;
	}

	for (integer i = 1; i <= my numberOfProximities; i ++) {
		const integer ii = my rowIndex [i], jj = my columnIndex [i];
		const double st = ddist -> data [ii] [jj] - dfit -> data [ii] [jj];
		const double tt = ddist -> data [ii] [jj] - dbar;
		s += st * st; 
		t += tt * tt;
	}

	if (out_stress)
		*out_stress = ( t > 0.0 ? sqrt (s / t) : 0.0 );
	if (out_s)
		*out_s = s;
	if (out_t)
		*out_t = t;
	if (out_dbar)
		*out_dbar = dbar;
}

autoKruskal Kruskal_create (integer numberOfPoints, integer numberOfDimensions) {
	try {
		autoKruskal me = Thing_new (Kruskal);
		my configuration = Configuration_create (numberOfPoints, numberOfDimensions);
		my proximities = ProximityList_create ();
		my dx = raw_MAT (numberOfPoints, numberOfDimensions);
		my tiesHandling = kMDS_TiesHandling::PRIMARY_APPROACH;
		my stress_formula = kMDS_KruskalStress::KRUSKAL_1;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Kruskal not created.");
	}
}

autoConfiguration Dissimilarity_to_Configuration_kruskal (Dissimilarity me, integer numberOfDimensions, integer /* metric */, kMDS_TiesHandling tiesHandling, kMDS_KruskalStress stress_formula, double tolerance, integer numberOfIterations, integer numberOfRepetitions) {
	try {
		//autoDistance d = Dissimilarity_to_Distance (me, kMDS_AnalysisScale::RATIO);
		autoDistance d = Dissimilarity_to_Distance (me, kMDS_AnalysisScale::ORDINAL);
		autoConfiguration c = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		Configuration_normalize (c.get(), 1.0, false);
		autoConfiguration thee = Dissimilarity_Configuration_kruskal (me, c.get(), tiesHandling, stress_formula, tolerance, numberOfIterations, numberOfRepetitions);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (kruskal method).");
	}
}

void Dissimilarity_Configuration_drawShepardDiagram (Dissimilarity me, Configuration him, Graphics g,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish) {
	autoDistance dist = Configuration_to_Distance (him);
	Proximity_Distance_drawScatterDiagram (me, dist.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

autoDistance Dissimilarity_Configuration_monotoneRegression (Dissimilarity dissimilarity, Configuration configuration, kMDS_TiesHandling tiesHandling) {
	try {
		autoDistance dist = Configuration_to_Distance (configuration);
		autoDistance result = Dissimilarity_Distance_monotoneRegression (dissimilarity, dist.get(), tiesHandling);
		return result;
	} catch (MelderError) {
		Melder_throw (U"No Distance created (monotone regression).");
	}
}

autoDistanceList DissimilarityList_Configuration_monotoneRegression (DissimilarityList me, Configuration configuration, kMDS_TiesHandling tiesHandling) {
	try {
		autoDistanceList thee = DistanceList_create ();
		autoDistance dist = Configuration_to_Distance (configuration);
		for (integer i = 1; i <= my size; i ++) {
			autoDistance d = Dissimilarity_Distance_monotoneRegression (my at [i], dist.get(), tiesHandling);
			thy addItem_move (d.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No DistanceList created (monotone regression).");
	}
}

void Dissimilarity_Configuration_drawMonotoneRegression (Dissimilarity me, Configuration him, Graphics g, kMDS_TiesHandling tiesHandling, double xmin, double xmax, double ymin, double ymax, double size_mm, conststring32 mark, bool garnish)
{
	/* obsolete replace by transformator */
	autoDistance fit = Dissimilarity_Configuration_monotoneRegression (me, him, tiesHandling);
	Proximity_Distance_drawScatterDiagram (me, fit.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawAbsoluteRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish) {
	autoTransformator t = Transformator_create (d -> numberOfRows);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.get(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawRatioRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish) {
	autoRatioTransformator t = RatioTransformator_create (d -> numberOfRows);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.get(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawIntervalRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish) {
	Dissimilarity_Configuration_Weight_drawISplineRegression (d, c, w, g, 0.0, 1.0, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawMonotoneRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	kMDS_TiesHandling tiesHandling, double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish) {
	autoMonotoneTransformator t = MonotoneTransformator_create (d->numberOfRows);
	MonotoneTransformator_setTiesProcessing (t.get(), tiesHandling);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.get(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawISplineRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
	integer numberOfInternalKnots, integer order, double xmin, double xmax, double ymin, double ymax,
	double size_mm, conststring32 mark, bool garnish) {
	autoISplineTransformator t = ISplineTransformator_create (d->numberOfRows, numberOfInternalKnots, order);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.get(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

autoDistance Dissimilarity_Configuration_Transformator_Weight_transform (Dissimilarity d, Configuration c, Transformator t, Weight w) {
	try {
		autoWeight aw;
		if (! w) {
			aw = Weight_create (d -> numberOfRows);
			w = aw.get();
		}
		autoDistance cdist = Configuration_to_Distance (c);
		autoMDSVec v = Dissimilarity_to_MDSVec (d);
		autoDistance thee = Transformator_transform (t, v.get(), cdist.get(), w);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Distance not created.");
	}
}

double Dissimilarity_Configuration_Weight_Transformator_normalizedStress (Dissimilarity me, Configuration conf, Weight weight, Transformator t) {
	autoDistance cdist = Configuration_to_Distance (conf);
	autoMDSVec vec = Dissimilarity_to_MDSVec (me);
	autoDistance fdist = Transformator_transform (t, vec.get(), cdist.get(), weight);
	const double stress = Distance_Weight_stress (fdist.get(), cdist.get(), weight, kMDS_stressMeasure::NORMALIZED);
	return stress;
}

double Dissimilarity_Configuration_getStress (Dissimilarity me, Configuration him, kMDS_TiesHandling tiesHandling, kMDS_KruskalStress stress_formula) {
	autoDistance dist = Configuration_to_Distance (him);
	autoMDSVec vec = Dissimilarity_to_MDSVec (me);
	autoDistance fit = MDSVec_Distance_monotoneRegression (vec.get(), dist.get(), tiesHandling);
	double stress;
	MDSVec_Distances_getStressValues (vec.get(), dist.get(), fit.get(), stress_formula, & stress, nullptr, nullptr, nullptr);
	return stress;
}

static double func (Daata object, VEC const& p) {
	Kruskal me = (Kruskal) object;
	const MDSVec him = my vec.get();
	const MAT x = my configuration -> data.get();
	double s, t, dbar, stress;
	const integer numberOfDimensions = my configuration -> numberOfColumns;
	const integer numberOfPoints = my configuration -> numberOfRows;
	/*
		Substitute results of minimizer into configuration and
		normalize the configuration
	*/
	MATfromVEC_inplace (x, p);
	centreEachColumn_MAT_inout (x);
	MATnormalize_inplace (x, 2.0, sqrt (numberOfPoints));
	/*
		Calculate interpoint distances from the configuration
	*/
	autoDistance dist = Configuration_to_Distance (my configuration.get());
	/*
		Monotone regression
	*/
	autoDistance fit = MDSVec_Distance_monotoneRegression (my vec.get(), dist.get(), my tiesHandling);
	/*
		Get numerator and denominator of stress
	*/
	MDSVec_Distances_getStressValues (my vec.get(), dist.get(), fit.get(), my stress_formula, & stress, & s, & t, & dbar);
	/*
		Prevent overflow when stress is small
	*/
	if (stress >= 1e-6) {
		my dx.all()  <<=  0.0;
		for (integer i = 1; i <= his numberOfProximities; i ++) {
			const integer ii = my vec -> rowIndex [i], jj = my vec -> columnIndex [i];
			const double g1 = stress * ((dist -> data [ii] [jj] - fit -> data [ii] [jj]) / s - (dist -> data [ii] [jj] - dbar) / t);
			for (integer j = 1; j <= numberOfDimensions; j ++) {
				const double dj = x [ii] [j] - x [jj] [j];
				double g2 = g1 * pow (fabs (dj) / dist -> data [ii] [jj], my configuration -> metric - 1.0);
				if (dj < 0.0)
					g2 = -g2;
				my dx [ii] [j] += g2;
				my dx [jj] [j] -= g2;
			}
		}
	}
	(my minimizer -> numberOfFunctionCalls) ++;
	return stress;
}

/*
	Precondition: configuration was not changed since previous call to func
*/
static void dfunc (Daata object, VEC const& /* p */, VEC const& dp) {
	const Kruskal me = (Kruskal) object;
	const Configuration thee = my configuration.get();

	integer k = 1;
	for (integer i = 1; i <= thy numberOfRows; i ++) {
		for (integer j = 1; j <= thy numberOfColumns; j ++)
			dp [k ++] = my dx [i] [j];
	}
}

autoConfiguration Dissimilarity_Configuration_kruskal (Dissimilarity me, Configuration him, kMDS_TiesHandling tiesHandling, kMDS_KruskalStress stress_formula, double tolerance, integer numberOfIterations, integer numberOfRepetitions) {
	try {
		/*
			The Configuration is normalized: each dimension centred +
			total variance set to 1.0
		*/
		const integer numberOfCoordinates = my numberOfRows * his numberOfColumns;
		const integer numberOfParameters = numberOfCoordinates - his numberOfColumns - 1;
		const integer numberOfData = my numberOfRows * (my numberOfRows - 1) / 2;

		Melder_require (numberOfParameters <= numberOfData,
			U"The number of parameters should not exceed the number of data.");

		autoKruskal thee = Kruskal_create (my numberOfRows, his numberOfColumns);
		TableOfReal_copyLabels (me, thy configuration.get(), 1, 0);
		autoDissimilarity dissimilarity = Data_copy (me);
		thy proximities -> addItem_move (dissimilarity.move());
		thy vec = Dissimilarity_to_MDSVec (me);

		thy minimizer = VDSmagtMinimizer_create (numberOfCoordinates, (Daata) thee.get(), func, dfunc);

		VECchainRows_preallocated (thy minimizer -> p.get(), his data.get());

		thy stress_formula = stress_formula;
		thy tiesHandling = tiesHandling;
		Configuration_setMetric (thy configuration.get(), his metric);

		Minimizer_minimizeManyTimes (thy minimizer.get(), numberOfIterations, numberOfRepetitions, tolerance);

		// call the function to get the best configuration

		(void) func ((Daata) thee.get(), thy minimizer -> p.get());
		return thy configuration.move();
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created.");
	}
}

/************************** INDSCAL **************************************/

/*
	Ten Berge, Kiers & Krijnen (1993), Computational Solutions for the
	Problem of Negative Saliences and Nonsymmetry in INDSCAL, Journal of Classification 10, 115-124.
*/

static void indscal_iteration_tenBerge (ScalarProductList zc, Configuration xc, Salience weights) {
	const integer nPoints = xc -> numberOfRows, nDimensions = xc -> numberOfColumns;
	const integer nSources = zc -> size;

	const double tolerance = 1e-4; // reasonable for dominant eigenvector estimation.
	autoMAT wsih = raw_MAT (nPoints, nPoints);
	autoVEC solution = raw_VEC (nPoints);

	for (integer h = 1; h <= nDimensions; h ++) {
		autoScalarProductList sprc = Data_copy (zc);
		wsih.all()  <<=  0.0;
		for (integer i = 1; i <= nSources; i ++) {
			const ScalarProduct sih = sprc -> at [i];
			/*
				Construct the Sih matrices (eq. 6)
			*/
			for (integer j = 1; j <= nDimensions; j ++) {
				if (j != h) {
					for (integer k = 1; k <= nPoints; k ++)
						for (integer l = 1; l <= nPoints; l ++)
							sih -> data [k] [l] -= xc -> data [k] [j] * xc -> data [l] [j] * weights -> data [i] [j];
				}
			}
			/*
				the weighted S matrix (eq. 8)
			*/
			wsih.all()  +=  sih -> data.all()  *  weights -> data [i] [h];
		}

		solution.all()  <<=  xc -> data.column (h); // initial guess
		/*
			largest eigenvalue of wsih (nonsymmetric matrix!!) is optimal solution for this dimension
		*/
		(void) VECdominantEigenvector_inplace (solution.get(), wsih.get(), tolerance);
		/*
			normalize the solution: centre and x'x = 1
		*/
		double mean;
		centre_VEC_inout (solution.get(), & mean);
		if (mean == 0.0)
			continue;
		VECnormalize_inplace (solution.get(), 2.0, 1.0);

		for (integer k = 1; k <= nPoints; k ++)
			xc -> data [k] [h] = solution [k];
		/*
			update weights. Make negative weights zero.
		*/
		for (integer i = 1; i <= nSources; i ++) {
			const ScalarProduct spr = sprc -> at [i];
			longdouble wih = 0.0;
			for (integer k = 1; k <= nPoints; k ++) {
				for (integer l = 1; l <= nPoints; l ++)
					wih += xc -> data [k] [h] * spr -> data [k] [l] * xc -> data [l] [h];
			}
			if (wih < 0.0)
				wih = 0.0;
			weights -> data [i] [h] = double (wih);
		}
	}
}


void ScalarProductList_Configuration_Salience_indscal (ScalarProductList sp, Configuration configuration, Salience weights, double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out_conf, autoSalience *out_sal, double *out_varianceAccountedFor) {
	try {
		autoConfiguration conf = Data_copy (configuration);
		autoSalience sal = Data_copy (weights);
		/*
			Solve for X, and W matrix via Alternating Least Squares.
		*/
		if (showProgress)
			Melder_progress (0.0, U"INDSCAL analysis");
		double vafp = 0.0, varianceAccountedFor;
		const double tol = 1e-6;
		integer iter;
		for (iter = 1; iter <= numberOfIterations; iter ++) {
			indscal_iteration_tenBerge (sp, conf.get(), sal.get());
			/*
				Goodness of fit and test criterion.
			*/
			ScalarProductList_Configuration_Salience_vaf (sp, conf.get(), sal.get(), & varianceAccountedFor);

			if (varianceAccountedFor > 1.0 - tol || fabs (varianceAccountedFor - vafp) /  vafp < tolerance)
				break;

			vafp = varianceAccountedFor;
			if (showProgress) Melder_progress (iter / (numberOfIterations + 1.0),
				U"indscal: varianceAccountedFor ", varianceAccountedFor);
		}
		const integer numberOfZeros = NUMcountZeros (sal -> data.get());

		if (out_conf) {
			Thing_setName (conf.get(), U"indscal");
			*out_conf = conf.move();
		}
		if (out_sal) {
			Thing_setName (sal.get(), U"indscal");
			TableOfReal_setLabelsFromCollectionItemNames (sal.get(), (Collection) sp, true, false);   // FIXME cast
			*out_sal = sal.move();
		}
		if (out_varianceAccountedFor)
			*out_varianceAccountedFor = varianceAccountedFor;
		if (showProgress) {
			const integer nSources = sp -> size;
			MelderInfo_writeLine (U"**************** INDSCAL results on Distances *******************\n\n",
				Thing_className (sp), U"number of objects: ", nSources);
			for (integer i = 1; i <= nSources; i ++) 
				MelderInfo_writeLine (U"  ", Thing_getName (sp->at [i]));

			if (numberOfZeros > 0)
				MelderInfo_writeLine (U"WARNING: ", numberOfZeros,  U" zero weight", ( numberOfZeros > 1 ? U"s" : U"" ), U"!");

			MelderInfo_writeLine (U"\n\nVariance Accounted For = ", varianceAccountedFor, U"\nThe optimal configuration was reached in ", ( iter > numberOfIterations ? numberOfIterations : iter ), U" iterations.");
			MelderInfo_drain();
		}
		if (showProgress)
			Melder_progress (1.0);
	} catch (MelderError) {
		if (showProgress) {
			Melder_progress (1.0);
		}
		Melder_throw (U"No indscal configuration calculated.");
	}
}

void DistanceList_Configuration_Salience_indscal (DistanceList distances, Configuration configuration, Salience weights, bool normalizeScalarProducts, double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *out_varianceAccountedFor) {
	try {
		autoScalarProductList sp = DistanceList_to_ScalarProductList (distances, normalizeScalarProducts);
		ScalarProductList_Configuration_Salience_indscal (sp.get(), configuration, weights, tolerance, numberOfIterations, showProgress, out1, out2, out_varianceAccountedFor);
	} catch (MelderError) {
		Melder_throw (U"No indscal configuration calculated.");
	}
}

void DissimilarityList_Configuration_Salience_indscal (DissimilarityList dissims, Configuration conf, Salience weights, kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts, double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out_configuration, autoSalience *out_salience, double *out_varianceAccountedFor) {
	try {
		autoConfiguration configuration = Data_copy (conf);
		autoSalience salience = Data_copy (weights);
		autoMDSVecList mdsveclist = DissimilarityList_to_MDSVecList (dissims);

		if (showProgress)
			Melder_progress (0.0, U"INDSCAL analysis");

		const double tol = 1e-6;
		double vafp = 0.0, vaf;
		integer iter;
		for (iter = 1; iter <= numberOfIterations; iter ++) {
			autoDistanceList distances = MDSVecList_Configuration_Salience_monotoneRegression (mdsveclist.get(), configuration.get(), salience.get(), tiesHandling);
			autoScalarProductList sp = DistanceList_to_ScalarProductList (distances.get(), normalizeScalarProducts);

			indscal_iteration_tenBerge (sp.get(), configuration.get(), salience.get());
			/*
				Goodness of fit and test criterion.
			*/
			DistanceList_Configuration_Salience_vaf (distances.get(), configuration.get(), salience.get(), normalizeScalarProducts, & vaf);

			if (vaf > 1.0 - tol || fabs (vaf - vafp) < vafp * tolerance)
				break;
			vafp = vaf;
			if (showProgress)
				Melder_progress ((double) iter / (numberOfIterations + 1), U"indscal: vaf ", vaf);
		}
		integer numberOfZeros = NUMcountZeros (salience -> data.get());
		Thing_setName (configuration.get(), U"indscal_mr");
		Thing_setName (salience.get(), U"indscal_mr");
		TableOfReal_setLabelsFromCollectionItemNames (salience.get(), (Collection) dissims, true, false);   // FIXME cast

		if (out_configuration)
			*out_configuration = configuration.move();
		if (out_salience)
			*out_salience = salience.move();
		if (out_varianceAccountedFor)
			*out_varianceAccountedFor = vaf;

		if (showProgress) {
			const integer nSources = dissims->size;
			MelderInfo_writeLine (U"**************** INDSCAL with monotone regression *******************");
			MelderInfo_writeLine (Thing_className (dissims));
			MelderInfo_writeLine (U"Number of objects: ", nSources);
			for (integer i = 1; i <= nSources; i ++)
				MelderInfo_writeLine (U"  ", Thing_getName (dissims->at [i]));
			if (numberOfZeros > 0)
				MelderInfo_writeLine (U"WARNING: ", numberOfZeros, U" zero weight", ( numberOfZeros > 1 ? U"s" : U"" ));
			MelderInfo_writeLine (U"Variance Accounted For: ", vaf);
			MelderInfo_writeLine (U"Based on MONOTONE REGRESSION");
			MelderInfo_writeLine (U"number of iterations: ", (iter > numberOfIterations ?	numberOfIterations : iter));
			MelderInfo_drain();
		}
		if (showProgress)
			Melder_progress (1.0);
	} catch (MelderError) {
		if (showProgress)
			Melder_progress (1.0);
		Melder_throw (U"No inscal configuration calculated.");
	}
}

void DistanceList_Configuration_indscal (DistanceList dists, Configuration conf, bool normalizeScalarProducts, double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2) {
	try {
		autoSalience w = Salience_create (dists->size, conf -> numberOfColumns);
		double vaf;
		DistanceList_Configuration_Salience_indscal (dists, conf, w.get(), normalizeScalarProducts, tolerance, numberOfIterations, showProgress, out1, out2, & vaf);
	} catch (MelderError) {
		Melder_throw (U"No indscal performed.");
	}
}

autoDistanceList MDSVecList_Configuration_Salience_monotoneRegression (MDSVecList vecs, Configuration conf, Salience weights, kMDS_TiesHandling tiesHandling) {
	try {
		autoVEC w = copy_VEC (conf -> w.get());
		autoDistanceList distances = DistanceList_create ();
		for (integer i = 1; i <= vecs->size; i ++) {
			conf -> w.all()  <<=  weights -> data.row (i);
			autoDistance dc = Configuration_to_Distance (conf);
			autoDistance dist = MDSVec_Distance_monotoneRegression (vecs->at [i], dc.get(), tiesHandling);
			distances -> addItem_move (dist.move());
		}
		Configuration_setDefaultWeights (conf);
		return distances;
	} catch (MelderError) {
		Melder_throw (U"No DistanceList created.");
	}
}

autoSalience DistanceList_Configuration_to_Salience (DistanceList d, Configuration c, bool normalize) {
	try {
		autoScalarProductList sp = DistanceList_to_ScalarProductList (d, normalize);
		autoSalience w = ScalarProductList_Configuration_to_Salience (sp.get(), c);
		return w;
	} catch (MelderError) {
		Melder_throw (U"No Salience created.");
	}
}

autoSalience ScalarProductList_Configuration_to_Salience (ScalarProductList me, Configuration him) {
	try {
		autoSalience salience = Salience_create (my size, his numberOfColumns);
		autoConfiguration cx = Data_copy (him);
		indscal_iteration_tenBerge (me, cx.get(), salience.get());
		return salience;
	} catch (MelderError) {
		Melder_throw (U"No Salience created.");
	}
}

autoSalience DissimilarityList_Configuration_to_Salience (DissimilarityList me, Configuration him, kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts) {
	try {
		autoDistanceList distances = DissimilarityList_Configuration_monotoneRegression (me, him, tiesHandling);
		autoSalience w = DistanceList_Configuration_to_Salience (distances.get(), him, normalizeScalarProducts);
		return w;
	} catch (MelderError) {
		Melder_throw (U"No Salience created.");
	}
}

void DissimilarityList_Configuration_indscal (DissimilarityList dissims, Configuration conf, kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts, double tolerance, integer numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2) {
	try {
		autoDistanceList distances = DissimilarityList_Configuration_monotoneRegression (dissims, conf, tiesHandling);
		autoSalience weights = DistanceList_Configuration_to_Salience (distances.get(), conf, normalizeScalarProducts);
		double vaf;
		DissimilarityList_Configuration_Salience_indscal (dissims, conf, weights.get(), tiesHandling, normalizeScalarProducts, tolerance, numberOfIterations, showProgress, out1, out2, & vaf);
	} catch (MelderError) {
		Melder_throw (U"No indscal performed.");
	}
}

void DissimilarityList_indscal (DissimilarityList me, integer numberOfDimensions, kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress, autoConfiguration *out_conf, autoSalience *out_sal) {
	bool showMulti = showProgress && numberOfRepetitions > 1;
	try {
		const bool showSingle = (showProgress && numberOfRepetitions == 1);

		autoDistanceList distances = DissimilarityList_to_DistanceList (me, kMDS_AnalysisScale::ORDINAL);
		autoConfiguration cstart; autoSalience wstart;
		DistanceList_to_Configuration_ytl (distances.get(), numberOfDimensions, normalizeScalarProducts, & cstart, & wstart);
		autoConfiguration conf = Data_copy (cstart.get());
		autoSalience sal = Data_copy (wstart.get());

		if (showMulti)
			Melder_progress (0.0, U"Indscal many times");

		double vaf, vafmin = 0.0;
		for (integer iter = 1; iter <= numberOfRepetitions; iter ++) {
			autoConfiguration cresult;
			autoSalience wresult;
			DissimilarityList_Configuration_Salience_indscal (me, cstart.get(), wstart.get(), tiesHandling,
				normalizeScalarProducts, tolerance, numberOfIterations, showSingle, & cresult, & wresult, & vaf);
			if (vaf > vafmin) {
				vafmin = vaf;
				conf = cresult.move();
				sal = wresult.move();
			}
			Configuration_randomize (cstart.get());
			Configuration_normalize (cstart.get(), 1.0, true);
			Salience_setDefaults (wstart.get());

			if (showMulti)
				Melder_progress ( (double) iter / (numberOfRepetitions + 1), iter, U" from ", numberOfRepetitions);
		}

		if (out_conf)
			*out_conf = conf.move();
		if (out_sal)
			*out_sal = sal.move();
		if (showMulti)
			Melder_progress (1.0);
		
	} catch (MelderError) {
		if (showMulti) {
			Melder_progress (1.0);
		}
		Melder_throw (me, U": no indscal performed.");
	}
}

void DistanceList_to_Configuration_indscal (DistanceList distances, integer numberOfDimensions, bool normalizeScalarProducts, double tolerance, integer numberOfIterations, integer numberOfRepetitions, bool showProgress, autoConfiguration *out_conf, autoSalience *out_sal) {
	bool showMulti = showProgress && numberOfRepetitions > 1;
	try {
		const bool showSingle = (showProgress && numberOfRepetitions == 1);
		autoConfiguration cstart;
		autoSalience wstart;
		DistanceList_to_Configuration_ytl (distances, numberOfDimensions, normalizeScalarProducts, & cstart, & wstart);
		autoConfiguration conf = Data_copy (cstart.get());
		autoSalience sal = Data_copy (wstart.get());

		if (showMulti)
			Melder_progress (0.0, U"Indscal many times");

		double vaf, vafmin = 0.0;
		for (integer i = 1; i <= numberOfRepetitions; i ++) {
			autoConfiguration cresult;
			autoSalience wresult;
			DistanceList_Configuration_Salience_indscal (distances, cstart.get(), wstart.get(), normalizeScalarProducts,  tolerance, numberOfIterations, showSingle, & cresult, & wresult, & vaf);
			if (vaf > vafmin) {
				vafmin = vaf;
				conf = cresult.move();
				sal = wresult.move();
			}
			Configuration_randomize (cstart.get());
			Configuration_normalize (cstart.get(), 1.0, true);
			Salience_setDefaults (wstart.get());

			if (showMulti)
				Melder_progress ((double) i / (numberOfRepetitions + 1), i, U" from ", numberOfRepetitions);
		}

		if (out_conf)
			*out_conf = conf.move();
		if (out_sal)
			*out_sal = sal.move();
		if (showMulti)
			Melder_progress (1.0);
		
	} catch (MelderError) {
		if (showMulti)
			Melder_progress (1.0);
		Melder_throw (distances, U": no indscal performed.");
	}
}

void DissimilarityList_Configuration_Salience_vaf (DissimilarityList me, Configuration thee, Salience him, kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts, double *out_varianceAccountedFor) {
	autoDistanceList distances = DissimilarityList_Configuration_monotoneRegression (me, thee, tiesHandling);
	DistanceList_Configuration_Salience_vaf (distances.get(), thee, him, normalizeScalarProducts, out_varianceAccountedFor);
}

void DistanceList_Configuration_vaf (DistanceList me, Configuration thee, bool normalizeScalarProducts, double *out_varianceAccountedFor) {
	autoSalience w = DistanceList_Configuration_to_Salience (me, thee, normalizeScalarProducts);
	DistanceList_Configuration_Salience_vaf (me, thee, w.get(), normalizeScalarProducts, out_varianceAccountedFor);
}

void DissimilarityList_Configuration_vaf (DissimilarityList me, Configuration thee, kMDS_TiesHandling tiesHandling, bool normalizeScalarProducts, double *out_varianceAccountedFor) {
	autoSalience w = DissimilarityList_Configuration_to_Salience (me, thee, tiesHandling, normalizeScalarProducts);
	DissimilarityList_Configuration_Salience_vaf (me, thee, w.get(), tiesHandling, normalizeScalarProducts, out_varianceAccountedFor);
}

void DistanceList_Configuration_Salience_vaf (DistanceList me, Configuration thee, Salience him, bool normalizeScalarProducts, double *out_varianceAccountedFor) {
	Melder_require (my size == his numberOfRows && thy numberOfColumns == his numberOfColumns,
		U"Dimensions should agree.");
	autoScalarProductList sp = DistanceList_to_ScalarProductList (me, normalizeScalarProducts);
	ScalarProductList_Configuration_Salience_vaf (sp.get(), thee, him, out_varianceAccountedFor);
}

void ScalarProduct_Configuration_getVariances (ScalarProduct me, Configuration thee, double *out_varianceExplained, double *out_varianceTotal) {
	autoDistance distance = Configuration_to_Distance (thee);
	autoScalarProduct fit = Distance_to_ScalarProduct (distance.get(), 0);
	/*
		ScalarProduct is double centred, i.e., mean == 0.
	*/
	double varianceExplained = 0.0, varianceTotal = 0.0;
	for (integer j = 1; j <= my numberOfRows; j ++) {
		for (integer k = 1; k <= my numberOfColumns; k ++) {
			const double d2 = my data [j] [k] - fit -> data [j] [k];
			varianceExplained += d2 * d2;
			varianceTotal += my data [j] [k] * my data [j] [k];
		}
	}
	
	if (out_varianceExplained)
		*out_varianceExplained = varianceExplained;
	if (out_varianceTotal)
		*out_varianceTotal = varianceTotal;

}

void ScalarProductList_Configuration_Salience_vaf (ScalarProductList me, Configuration thee, Salience him, double *out_varianceAccountedFor) {
	autoVEC w = copy_VEC (thy w.get()); // save weights
	try {
		Melder_require (my size == his numberOfRows && thy numberOfColumns == his numberOfColumns,
			U"Dimensions should agree.");

		double t = 0.0, n = 0.0;
		for (integer i = 1; i <= my size; i ++) {
			const ScalarProduct sp = my at [i];
			Melder_require (sp -> numberOfRows == thy numberOfRows,
				U"Dimension of ScalarProduct ", i, U" should match Configuration.");
			/*
				Weigh configuration before calculating variances
			*/
			for (integer j = 1; j <= thy numberOfColumns; j ++)
				thy w [j] = sqrt (his data [i] [j]);

			double vare, vart;
			ScalarProduct_Configuration_getVariances (sp, thee, & vare, & vart);

			t += vare;
			n += vart;
		}

		if (out_varianceAccountedFor)
			*out_varianceAccountedFor = ( n > 0.0 ? 1.0 - t / n : 0.0 );
		thy w.all()  <<=  w.all(); // restore weights
		
	} catch (MelderError) {
		thy w.all()  <<=  w.all();
		Melder_throw (U"No out_varianceAccountedFor calculated.");
	}
}

/********************** Examples *********************************************/

autoSalience Salience_createCarrollWishExample () {
	try {
		integer numberOfSources = 8;
		constexpr double wx [9] = {0, 1.0, 0.866, 0.707, 0.5,   0.1, 0.5, 0.354, 0.1};
		constexpr double wy [9] = {0, 0.1, 0.5,   0.707, 0.866,   1, 0.1, 0.354, 0.5};
		constexpr conststring32 name [] = { U"", U"1", U"2", U"3", U"4", U"5", U"6", U"7", U"8"};
		autoSalience me = Salience_create (numberOfSources, 2);

		for (integer i = 1; i <= numberOfSources; i ++) {
			my data [i] [1] = wx [i];
			my data [i] [2] = wy [i];
			TableOfReal_setRowLabel (me.get(), i, name [i]);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Salience for Carroll Wish example not created.");
	}
}

autoCollection INDSCAL_createCarrollWishExample (double noiseRange) {
	try {
		autoConfiguration c = Configuration_createCarrollWishExample ();
		const integer numberOfObjects = c -> numberOfRows, numberOfSources = 8;
		autoSalience s = Salience_createCarrollWishExample ();
		autoCollection me = Collection_create ();
		for (integer l = 1; l <= numberOfSources; l ++) {
			c -> w [1] = s -> data [l] [1];
			c -> w [2] = s -> data [l] [2];
			autoDistance d = Configuration_to_Distance (c.get());
			autoDissimilarity dissim = Distance_to_Dissimilarity (d.get());
			for (integer i = 1; i <= numberOfObjects - 1; i ++) {
				for (integer j = i + 1; j <= numberOfObjects; j ++)
					dissim -> data [i] [j] = (dissim -> data [j] [i] += NUMrandomUniform (0.0, noiseRange));
			}
			Thing_setName (dissim.get(), s -> rowLabels [l].get());
			my addItem_move (dissim.move());
		}
		Thing_setName (me.get(), U"CarrollWish");
		return me;
	} catch (MelderError) {
		Melder_throw (U"Collection not created.");
	}
}

void drawSplines (Graphics g, double low, double high, double ymin, double ymax,
	kMDS_splineType splineType, integer order, conststring32 interiorKnots, bool garnish)
{
	constexpr integer maximumNumberOfKnots = 100;
	integer k = order;
	double knot [maximumNumberOfKnots + 1];
	if (k > maximumNumberOfKnots)
		return;
	if (splineType == kMDS_splineType::I_SPLINE)
		k ++;
	for (integer i = 1; i <= k; i ++)
		knot [i] = low;
	integer numberOfKnots = k;

	{ // scope
		conststring8 start = Melder_peek32to8 (interiorKnots); // UGLY; because of non-availability of str32tod
		mutablestring8 end;   // this is not actually mutated, but strtod's signature requires this
		while (*start) {
			const double value = strtod (start, & end);
			start = end;
			if (value < low || value > high) {
				Melder_warning (U"drawSplines: knots should be in interval (", low, U", ", high, U")");
				return;
			}
			if (numberOfKnots == maximumNumberOfKnots) {
				Melder_warning (U"drawSplines: too many knots (", maximumNumberOfKnots + 1, U")");
				return;
			}
			knot [++ numberOfKnots] = value;
		}
	}

	const integer numberOfInteriorKnots = numberOfKnots - k;
	for (integer i = 1; i <= k; i ++) {
		if (numberOfKnots == maximumNumberOfKnots) {
			Melder_warning (U"drawSplines: too many knots (", maximumNumberOfKnots + 1, U")");
			return;
		}
		knot [++numberOfKnots] = high;
	}
	
	const integer nSplines = order + numberOfInteriorKnots;
	if (nSplines == 0)
		return;

	Graphics_setWindow (g, low, high, ymin, ymax);
	Graphics_setInner (g);
	constexpr integer numberOfPoints = 1000;
	autoVEC y = raw_VEC (numberOfPoints);
	const double dx = (high - low) / (numberOfPoints - 1);
	for (integer i = 1; i <= nSplines; i ++) {
		for (integer j = 1; j <= numberOfPoints; j ++) {
			const double x = low + dx * (j - 1);
			double yx;
			if (splineType == kMDS_splineType::M_SPLINE)
				yx = NUMmspline (constVEC (& knot [1], numberOfKnots), order, i, x);
			else
				yx = NUMispline (constVEC (& knot [1], numberOfKnots), order, i, x);
			y [j] = ( yx < ymin ? ymin : ( yx > ymax ? ymax : yx ) );
		}
		Graphics_function (g, y.asArgumentToFunctionThatExpectsOneBasedArray(), 1, numberOfPoints, low, high);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		static MelderString ts;
		const integer lastKnot = ( splineType == kMDS_splineType::I_SPLINE ? numberOfKnots - 2 : numberOfKnots );
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, false, ( splineType == kMDS_splineType::M_SPLINE ? U"\\s{M}\\--spline" : U"\\s{I}\\--spline" ));
		Graphics_marksTop (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		if (low <= knot [order]) {
			if (order == 1)
				MelderString_copy (& ts, U"t__1_");
			else if (order == 2)
				MelderString_copy (& ts,  U"{t__1_, t__2_}");
			else
				MelderString_copy (& ts, U"{t__1_..t__", order, U"_}");
			Graphics_markBottom (g, low, false, false, false, ts.string);
		}
		for (integer i = 1; i <= numberOfInteriorKnots; i ++) {
			if (low <= knot [k + i] && knot [k + i] < high) {
				MelderString_copy (& ts, U"t__", order + i, U"_");
				Graphics_markBottom (g, knot [k + i], false, true, true, ts.string);
				Graphics_markTop (g, knot [k + i], true, false, false, nullptr);
			}
		}
		if (knot [lastKnot - order + 1] <= high) {
			if (order == 1)
				MelderString_copy (& ts, U"t__", lastKnot, U"_");
			else
				MelderString_copy (& ts, U"{t__", ( order == 2 ? lastKnot - 1 : lastKnot - order + 1 ), U"_, t__", lastKnot, U"_}");
			Graphics_markBottom (g, high, false, false, false, ts.string);
		}
	}
}

void drawMDSClassRelations (Graphics g) {
	const integer nBoxes = 6;
	const double boxWidth = 0.3, boxWidth2 = boxWidth / 2.0, boxWidth3 = boxWidth / 3.0;
	const double boxHeight = 0.1, boxHeight2 = boxHeight / 2.0;
	const double boxHeight3 = boxHeight / 3.0;
	const double r_mm = 3, dxt = 0.025, dyt = 0.03;
	const double dboxx = 1.0 - 0.2 - 2.0 * boxWidth, dboxy = (1.0 - 4.0 * boxHeight) / 3.0;
	double x1, x2, xm, x23, x13, y1, y2, ym, y23, y13;
	const double x [7] = {0.0, 0.2, 0.2, 0.7, 0.2, 0.7, 0.2}; /* left */
	const double y [7] = {0.0, 0.9, 0.6, 0.6, 0.3, 0.3, 0.0}; /* bottom */
	const conststring32 text [7] = {U"", U"Confusion", U"Dissimilarity  %\\de__%%ij%_",  U"Similarity", U"Distance  %d__%%ij%_, %d\\'p__%%ij%_", U"ScalarProduct", U"Configuration" };

	Graphics_setWindow (g, -0.05, 1.05, -0.05, 1.05);
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
	for (integer i = 1; i <= nBoxes; i ++) {
		x2 = x [i] + boxWidth;
		y2 = y [i] + boxHeight;
		xm = x [i] + boxWidth2;
		ym = y [i] + boxHeight2;
		Graphics_roundedRectangle (g, x [i], x2, y [i], y2, r_mm);
		Graphics_text (g, xm, ym, text [i]);
	}

	Graphics_setLineWidth (g, 2);
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);

	// Confusion to Dissimilarity

	xm = x [1] + boxWidth2;
	y2 = y [2] + boxHeight;
	Graphics_arrow (g, xm, y [1], xm, y2);
	Graphics_text (g, xm + dxt, y2 + dboxy / 2, U"pdf");

	// Confusion to Similarity

	x1 = x [1] + boxWidth;
	xm = x [3] + boxWidth2;
	ym = y [1] + boxHeight2;
	y2 = y [3] + boxHeight;
	Graphics_line (g, x1, ym, xm, ym);
	Graphics_arrow (g, xm, ym, xm, y2);
	y2 += + dboxy / 2 + dyt / 2;
	Graphics_text (g, xm + dxt, y2, U"average");
	y2 -= dyt;
	Graphics_text (g, xm + dxt, y2, U"houtgast");

	// Dissimilarity to Similarity

	x1 = x [2] + boxWidth;
	y23 = y [2] + 2 * boxHeight3;
	Graphics_arrow (g, x1, y23, x [3], y23);
	y13 = y [2] + boxHeight3;
	Graphics_arrow (g, x [3], y13, x1, y13);

	// Dissimilarity to Distance

	x13 = x [4] + boxWidth3;
	y1 = y [4] + boxHeight;
	Graphics_arrow (g, x13, y1, x13, y [2]);
	x23 = x [4] + 2 * boxWidth3;
	Graphics_arrow (g, x23, y [2], x23, y1);

	x1 = x23 + dxt;
	y1 = y [2] - dyt;
	x2 = 0 + dxt;
	y1 -= dyt;
	Graphics_text (g, x1, y1, U"%d\\'p__%%ij%_ = %\\de__%%ij%_");
	Graphics_text (g, x2, y1, U"absolute");
	y1 -= dyt;
	Graphics_text (g, x1, y1, U"%d\\'p__%%ij%_ = %b\\.c%\\de__%%ij%_");
	Graphics_text (g, x2, y1, U"ratio");
	y1 -= dyt;
	Graphics_text (g, x1, y1, U"%d\\'p__%%ij%_ = %b\\.c%\\de__%%ij%_+%a");
	Graphics_text (g, x2, y1, U"interval");
	y1 -= dyt;
	Graphics_text (g, x1, y1, U"%d\\'p__%%ij%_ = \\s{I}-spline (%\\de__%%ij%_)");
	Graphics_text (g, x2, y1, U"\\s{I}\\--spline");
	y1 -= dyt;
	Graphics_text (g, x1, y1, U"%d\\'p__%%ij%_ = monotone (%\\de__%%ij%_)");
	Graphics_text (g, x2, y1, U"monotone");

	// Distance to ScalarProduct

	x1 = x [4] + boxWidth;
	ym = y [4] + boxHeight2;
	Graphics_arrow (g, x1, ym, x [5], ym);

	// Distance to Configuration

	y1 = y [6] + boxHeight;
	Graphics_arrow (g, x13, y1, x13, y [4]);

	// ScalarProduct to Configuration

	x13 = x [5] + boxWidth3;
	x23 = x [6] + 2 * boxWidth3;
	y1 = y [5] - dboxy / 2;
	Graphics_line (g, x13, y [5], x13, y1);
	Graphics_line (g, x13, y1, x23, y1);
	Graphics_arrow (g, x23, y1, x23, y [6] + boxHeight);
	x1 = x [6] + boxWidth + dboxx / 2;
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
	Graphics_text (g, x1, y1, U"\\s{TORSCA}");
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
	Graphics_text (g, x1, y1, U"\\s{YTL}");

	Graphics_setLineType (g, Graphics_DOTTED);

	x23 = x [5] + 2 * boxWidth3;
	ym = y [6] + boxHeight2;
	Graphics_line (g, x23, y [5], x23, ym);
	Graphics_arrow (g, x23, ym, x [6] + boxWidth, ym);
	x1 = x [6] + boxWidth + dboxx / 2 + boxWidth3;
	Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
	Graphics_text (g, x1, ym, U"\\s{INDSCAL}");

	// Dissimilarity to Configuration

	ym = y [2] + boxHeight2;
	y2 = y [6] + boxHeight2;
	Graphics_line (g, x [2], ym, 0, ym);
	Graphics_line (g, 0, ym, 0, y2);
	Graphics_arrow (g, 0, y2, x [6], y2);

	// Restore settings

	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setLineWidth (g, 1.0);
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
}

/* End of file MDS.cpp */
