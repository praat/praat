/* MDS.cpp
 *
 * Copyright (C) 1993-2016 David Weenink, 2015,2017 Paul Boersma
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
 djmw 20030623 Modified calls to NUMeigensystem_d
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
#include "SSCP.h"
#include "PCA.h"

#define TINY 1e-30

Thing_implement (Kruskal, Thing, 0);

Thing_implement (Transformator, Thing, 0);
Thing_implement (RatioTransformator, Transformator, 0);
Thing_implement (MonotoneTransformator, Transformator, 0);
Thing_implement (ISplineTransformator, Transformator, 0);

Thing_implement (MDSVec, Daata, 0);
Thing_implement (MDSVecList, Ordered, 0);

Thing_implement (Weight, TableOfReal, 0);
Thing_implement (Salience, TableOfReal, 0);
Thing_implement (ScalarProduct, TableOfReal, 0);

Thing_implement (ConfusionList, TableOfRealList, 0);
Thing_implement (ProximityList, TableOfRealList, 0);
Thing_implement (ScalarProductList, TableOfRealList, 0);

Thing_implement (DistanceList, ProximityList, 0);
Thing_implement (Dissimilarity, Proximity, 0);
Thing_implement (DissimilarityList, ProximityList, 0);
Thing_implement (Similarity, Proximity, 0);


/********************** NUMERICAL STUFF **************************************/

static void NUMdmatrix_into_vector (double **m, double *v, long r1, long r2, long c1, long c2) {
	long k = 1;

	for (long i = r1; i <= r2; i++) {
		for (long j = c1; j <= c2; j++) {
			v[k++] = m[i][j];
		}
	}
}

static void NUMdvector_into_matrix (const double *v, double **m, long r1, long r2, long c1, long c2) {
	long k = 1;

	for (long i = r1; i <= r2; i++) {
		for (long j = c1; j <= c2; j++) {
			m[i][j] = v[k++];
		}
	}
}

static void NUMdmatrix_normalizeRows (double **m, long nr, long nc) {
	for (long i = 1; i <= nr; i++) {
		double rowSum = 0.0;
		for (long j = 1; j <= nc; j++) {
			rowSum += m[i][j];
		}

		if (rowSum != 0.0) {
			for (long j = 1; j <= nc; j++) {
				m[i][j] /= rowSum;
			}
		}
	}
}

static long NUMdmatrix_countZeros (double **m, long nr, long nc) {
	long nZeros = 0;

	for (long i = 1; i <= nr; i++) {
		for (long j = 1; j <= nc; j++) {
			if (m[i][j] == 0) {
				nZeros++;
			}
		}
	}
	return nZeros;
}

static void NUMsort3 (double *data, long *iPoint, long *jPoint, long ifrom, long ito, int ascending) {
	if (ifrom > ito || ifrom < 1) {
		Melder_throw (U"invalid range.");
	}
	long n = ito - ifrom + 1;
	if (n == 1) {
		return;
	}
	autoNUMvector<long> indx (ifrom, ito);
	autoNUMvector<double> atmp (ifrom, ito);
	autoNUMvector<long> itmp (ifrom, ito);
	//NUMindexx (data + ifrom - 1, n, indx + ifrom - 1);
	NUMindexx (&data [ifrom - 1], n, &indx [ifrom - 1]);
	if (! ascending) {
		for (long j = ifrom; j <= ifrom + n / 2; j ++) {
			long tmp = indx [j];
			indx [j] = indx [ito - j + ifrom];
			indx [ito - j + ifrom] = tmp;
		}
	}
	for (long j = ifrom; j <= ito; j++) {
		indx[j] += ifrom - 1;
	}
	for (long j = ifrom; j <= ito; j++) {
		atmp[j] = data[j];
	}
	for (long j = ifrom; j <= ito; j++) {
		data[j] = atmp[indx[j]];
	}
	for (long j = ifrom; j <= ito; j++) {
		itmp[j] = iPoint[j];
	}
	for (long j = ifrom; j <= ito; j++) {
		iPoint[j] = itmp[indx[j]];
	}
	for (long j = ifrom; j <= ito; j++) {
		itmp[j] = jPoint[j];
	}
	for (long j = ifrom; j <= ito; j++) {
		jPoint[j] = itmp[indx[j]];
	}
}

/************ ConfigurationList & Similarity **************************/

autoDistanceList ConfigurationList_to_DistanceList (ConfigurationList me) {
	try {
		autoDistanceList thee = DistanceList_create ();
		for (long i = 1; i <= my size; i ++) {
			Configuration conf = my at [i];
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
		if (my size == 0) {
			Melder_throw (U"DistanceList is empty.");
		}
		if (! TableOfRealList_haveIdenticalDimensions (my asTableOfRealList())) {
			Melder_throw (U"All matrices must have the same dimensions.");
		}
		autoWeight aw;
		if (! w) {
			aw = Weight_create (my at [1] -> numberOfRows);
			w = aw.get();
		}

		autoSimilarity thee = Similarity_create (my size);

		for (long i = 1; i <= my size; i ++) {
			Distance di = my at [i];
			const char32 *name = Thing_getName (di);
			TableOfReal_setRowLabel (thee.get(), i, name);
			TableOfReal_setColumnLabel (thee.get(), i, name);
			thy data[i][i] = 1;
			for (long j = i + 1; j <= my size; j ++) {
				Distance dj = my at [j];
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

		for (long i = 1; i <= vec -> nProximities; i++) {
			long ii = vec -> iPoint[i];
			long jj = vec -> jPoint[i];
			thy data[ii][jj] = thy data[jj][ii] = vec -> proximity[i];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Distance not created.");
	}
}

void Transformator_init (Transformator me, long numberOfPoints) {
	my numberOfPoints = numberOfPoints;
	my normalization = 1;
}

autoTransformator Transformator_create (long numberOfPoints) {
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
		if (my numberOfPoints != vec -> nPoints ||
		        my numberOfPoints != d -> numberOfRows ||
		        d -> numberOfRows != w -> numberOfRows) {
			Melder_throw (U"Dimensions do not agree.");
		}
		return my v_transform (vec, d, w);
	} catch (MelderError) {
		Melder_throw (me, U"Distance not created.");
	}
}

autoDistance structRatioTransformator :: v_transform (MDSVec vec, Distance d, Weight w) {
	autoDistance thee = Distance_create (numberOfPoints);
	TableOfReal_copyLabels (d, thee.get(), 1, 1);

	// Determine ratio (eq. 9.4)

	double etaSq = 0.0, rho = 0.0;
	for (long i = 1; i <= vec -> nProximities; i ++) {
		long ii = vec -> iPoint [i];
		long jj = vec -> jPoint [i];
		double delta_ij = vec -> proximity[i], d_ij = d -> data [ii] [jj];
		double tmp = w -> data [ii] [jj] * delta_ij * delta_ij;
		etaSq += tmp;
		rho += tmp * d_ij * d_ij;
	}

	// transform

	if (etaSq == 0.0) {
		Melder_throw (U"Eta squared is zero.");
	}
	our ratio = rho / etaSq;
	for (long i = 1; i <= vec -> nProximities; i ++) {
		long ii = vec -> iPoint [i];
		long jj = vec -> jPoint [i];
		thy data [ii] [jj] = thy data [jj] [ii] = our ratio * vec -> proximity [i];
	}

	if (our normalization) {
		Distance_Weight_smacofNormalize (thee.get(), w);
	}
	return thee;
}

autoRatioTransformator RatioTransformator_create (long numberOfPoints) {
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
		if (normalization) {
			Distance_Weight_smacofNormalize (thee.get(), w);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Distance not created.");
	}
}

autoMonotoneTransformator MonotoneTransformator_create (long numberOfPoints) {
	try {
		autoMonotoneTransformator me = Thing_new (MonotoneTransformator);
		Transformator_init (me.get(), numberOfPoints);
		my tiesHandling = MDS_PRIMARY_APPROACH;
		return me;
	} catch (MelderError) {
		Melder_throw (U"MonotoneTransformator not created.");
	}
}

void MonotoneTransformator_setTiesProcessing (MonotoneTransformator me, int tiesHandling) {
	my tiesHandling = tiesHandling;
}

void structISplineTransformator :: v_destroy () noexcept {
	NUMvector_free<double> (b, 1);
	NUMvector_free<double> (knot, 1);
	NUMmatrix_free<double> (m, 1, 1);
	ISplineTransformator_Parent :: v_destroy ();
}

autoDistance structISplineTransformator :: v_transform (MDSVec vec, Distance dist, Weight w) {
	double tol = 1e-6;
	long itermax = 20, nx = vec -> nProximities;
	long nKnots = numberOfInteriorKnots + order + order + 2;

	autoDistance thee = Distance_create (dist -> numberOfRows);
	TableOfReal_copyLabels (dist, thee.get(), 1, -1);

	autoNUMvector<double> d (1, nx);

	for (long i = 1; i <= nx; i++) {
		d[i] = dist -> data[vec -> iPoint[i]][vec -> jPoint[i]];
	}

	/*
		Process knots. Put interior knots at quantiles.
		Guarantee that for each proximity x[i]: knot[j] <= x[i] < knot[j+1]
	*/

	for (long i = 1; i <= order + 1; i++) {
		knot [i] = vec -> proximity [1];
		knot [nKnots - i + 1] = vec -> proximity [nx] * 1.000001;
	}
	for (long i = 1; i <= numberOfInteriorKnots; i++) {
		double fraction = (double) i / (numberOfInteriorKnots + 1);
		knot [order + 1 + i] = NUMquantile (nx, vec -> proximity, fraction);
	}

	// Calculate data matrix m.

	for (long i = 1; i <= nx; i++) {
		double y, x = vec -> proximity [i];
		m[i][1] = 1.0;
		for (long j = 2; j <= numberOfParameters; j++) {
			try {
				NUMispline (knot, nKnots, order, j - 1, x, & y);
			} catch (MelderError) {
				Melder_throw (U"I-spline[", j - 1, U"], data[", i, U"d] = ", x);
			}
			m[i][j] = y;
		}
	}

	NUMsolveNonNegativeLeastSquaresRegression (m, nx, numberOfParameters, d.peek(), tol, itermax, b);

	for (long i = 1; i <= nx; i++) {
		long ii = vec->iPoint[i];
		long jj = vec->jPoint[i];
		double r = 0.0;

		for (long j = 1; j <= numberOfParameters; j++) {
			r += m[i][j] * b[j];
		}
		thy data[ii][jj] = thy data[jj][ii] = r;
	}

	if (normalization) {
		Distance_Weight_smacofNormalize (thee.get(), w);
	}
	return thee;
}

autoISplineTransformator ISplineTransformator_create (long numberOfPoints, long numberOfInteriorKnots, long order) {
	try {
		autoISplineTransformator me = Thing_new (ISplineTransformator);
		long nData = (numberOfPoints - 1) * numberOfPoints / 2;

		Transformator_init (me.get(), numberOfPoints);

		/*
			1 extra parameter for the intercept.
			2 extra knots for the I-spline.
		*/

		my numberOfParameters = numberOfInteriorKnots + order + 1;
		long numberOfKnots = numberOfInteriorKnots + order + order + 2;

		my b = NUMvector<double> (1, my numberOfParameters);
		my knot = NUMvector<double> (1, numberOfKnots);
		my m = NUMmatrix<double> (1, nData, 1, my numberOfParameters);

		for (long i = 1; i <= my numberOfParameters; i++) {
			my b[i] = NUMrandomUniform (0.0, 1.0);
		}

		my numberOfInteriorKnots = numberOfInteriorKnots;
		my order = order;
		return me;
	} catch (MelderError) {
		Melder_throw (U"ISplineTransformator not created.");
	}
}

/***************** CONTINGENCYTABLE **************************************/

autoConfiguration ContingencyTable_to_Configuration_ca (ContingencyTable me, long numberOfDimensions, int scaling) {
	try {
		long nr = my numberOfRows, nc = my numberOfColumns;
		long dimmin = nr < nc ? nr : nc;

		autoNUMmatrix<double> h (NUMmatrix_copy (my data, 1, nr, 1, nc), 1, 1);
		autoNUMvector<double> rowsum (1, nr);
		autoNUMvector<double> colsum (1, nc);
		autoConfiguration thee = Configuration_create (nr + nc, numberOfDimensions);

		if (numberOfDimensions == 0) {
			numberOfDimensions = dimmin - 1;
		}
		if (numberOfDimensions >= dimmin) {
			Melder_throw (U"Dimension too high.");
		}

		// Ref: A. Gifi (1990), Nonlinear Multivariate Analysis, Wiley & Sons, reprinted 1996,
		//		Chapter 8, Multidimensional scaling and Correspondence Analysis.
		// Get row and column marginals

		double sum = 0.0;
		for (long i = 1; i <= nr; i++) {
			for (long j = 1; j <= nc; j++) {
				rowsum[i] += my data[i][j];
				colsum[j] += my data[i][j];
			}
			if (rowsum[i] <= 0.0) {
				Melder_throw (U"Empty row: ", i, U".");
			}
			sum += rowsum[i];
		}

		for (long j = 1; j <= nc; j++) {
			if (colsum[j] <= 0.0) {
				Melder_throw (U"Empty column: ", j, U".");
			}
		}

		// Remove trivial singular vectors (Eq. 8.24),
		// construct Dr^(-1/2) H Dc^(-1/2) - Dr^(1/2) uu' Dc^(1/2) / N

		for (long i = 1; i <= nr; i++) {
			for (long j = 1; j <= nc; j++) {
				double rc = sqrt (rowsum[i] * colsum[j]);
				h[i][j] = h[i][j] / rc - rc / sum;
			}
		}

		// Singular value decomposition of h

		autoSVD svd = SVD_create_d (h.peek(), nr, nc);
		SVD_zeroSmallSingularValues (svd.get(), 0);

		// Scale row vectors and column vectors to configuration.

		for (long j = 1; j <= numberOfDimensions; j++) {
			double rootsum = sqrt (sum), xfactor, yfactor, lambda = svd -> d[j];
			if (scaling == 1) {

				// Scale row points in the centre of gravity of column points (eq 8.5.a)

				xfactor = rootsum * lambda;
				yfactor = rootsum;
			} else if (scaling == 2) {

				// Scale column points in the centre of gravity of row points (8.5.b)

				xfactor = rootsum;
				yfactor = rootsum * lambda;
			} else if (scaling == 3) {

				// Treat row and columns symmetrically (8.5.c).

				xfactor = yfactor = rootsum * sqrt (lambda);
			} else {
				break;
			}
			for (long i = 1; i <= nr; i++) {
				thy data[i][j] = svd -> u[i][j] * xfactor / sqrt (rowsum[i]);
			}
			for (long i = 1; i <= nc; i++) {
				thy data[nr + i][j] = svd -> v[i][j] * yfactor / sqrt (colsum[i]);
			}
		}

		TableOfReal_setSequentialColumnLabels (thee.get(), 0, 0, nullptr, 1, 1);
		NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, nr);
		for (long i = 1; i <= nc; i++) {
			if (my columnLabels[i]) {
				TableOfReal_setRowLabel (thee.get(), nr + i, my columnLabels[i]);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created.");
	}
}

autoDissimilarity TableOfReal_to_Dissimilarity (TableOfReal me) {
	try {
		if (my numberOfRows != my numberOfColumns) {
			Melder_throw (U"TableOfReal must be a square tabel.");
		}
		TableOfReal_checkPositive (me);
		autoDissimilarity thee = Thing_new (Dissimilarity);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Dissimilarity.");
	}
}

autoSimilarity TableOfReal_to_Similarity (TableOfReal me) {
	try {
		if (my numberOfRows != my numberOfColumns) {
			Melder_throw (U"TableOfReal must be a square table.");
		}
		TableOfReal_checkPositive (me);
		autoSimilarity thee = Thing_new (Similarity);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Similarity.");
	}
}

autoDistance TableOfReal_to_Distance (TableOfReal me) {
	try {
		if (my numberOfRows != my numberOfColumns) {
			Melder_throw (U"TableOfReal must be a square table.");
		}
		TableOfReal_checkPositive (me);
		autoDistance thee = Thing_new (Distance);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Distance.");
	}
}

autoSalience TableOfReal_to_Salience (TableOfReal me) {
	try {
		TableOfReal_checkPositive (me);
		autoSalience thee = Thing_new (Salience);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Salience.");
	}
}

autoWeight TableOfReal_to_Weight (TableOfReal me) {
	try {
		TableOfReal_checkPositive (me);
		autoWeight thee = Thing_new (Weight);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Weight.");
	}
}

autoScalarProduct TableOfReal_to_ScalarProduct (TableOfReal me) {
	try {
		if (my numberOfRows != my numberOfColumns) {
			Melder_throw (U"TableOfReal must be a square table.");
		}
		autoScalarProduct thee = Thing_new (ScalarProduct);
		my structTableOfReal :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to ScalarProduct.");
	}
}

/**************** Covariance & Correlation to Configuration *****************/

autoConfiguration SSCP_to_Configuration (SSCP me, long numberOfDimensions) {
	try {
		autoConfiguration thee = Configuration_create (my numberOfRows, numberOfDimensions);
		autoPCA a = SSCP_to_PCA (me);
		TableOfReal_setSequentialColumnLabels (thee.get(), 0, 0, nullptr, 1, 1);

		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= numberOfDimensions; j++) {
				double s = 0.0;
				for (long k = 1; k <= my numberOfRows; k++) {
					s += my data[k][i] * a -> eigenvectors[k][j];
				}
				thy data[i][j] = s;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created.");
	}
}

autoConfiguration Covariance_to_Configuration (Covariance me, long numberOfDimensions) {
	return SSCP_to_Configuration (me, numberOfDimensions);
}

autoConfiguration Correlation_to_Configuration (Correlation me, long numberOfDimensions) {
	return SSCP_to_Configuration (me, numberOfDimensions);
}

/**************************** Weight *****************************************/

autoWeight Weight_create (long numberOfPoints) {
	try {
		autoWeight me = Thing_new (Weight);
		TableOfReal_init (me.get(), numberOfPoints, numberOfPoints);
		for (long i = 1; i <= numberOfPoints; i++) {
			for (long j = 1; j <= numberOfPoints; j++) {
				my data[i][j] = 1.0;
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Weight not created.");
	}
}


/**************** Salience *****************************************/

autoSalience Salience_create (long numberOfSources, long numberOfDimensions) {
	try {
		autoSalience me = Thing_new (Salience);
		TableOfReal_init (me.get(), numberOfSources, numberOfDimensions);
		Salience_setDefaults (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Salience not created.");
	}
}

long Salience_correctNegatives (Salience me) {
	/*
		The weights might be negative.
		We correct this by simply making them positive.
	*/

	long nNegatives = 0;
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			if (my data[i][j] < 0.0) {
				nNegatives++;
				my data[i][j] = - my data[i][j];
			}
		}
	}
	return nNegatives;
}

void Salience_setDefaults (Salience me) {
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			my data[i][j] = 1.0 / sqrt (my numberOfColumns);
		}
	}
	for (long j = 1; j <= my numberOfColumns; j++) {
		TableOfReal_setColumnLabel (me, j, Melder_cat (U"dimension ", j));
	}
}

void Salience_draw (Salience me, Graphics g, int ix, int iy, int garnish) {
	long nc2, nc1 = ix < iy ? (nc2 = iy, ix) : (nc2 = ix, iy);
	double xmin = 0.0, xmax = 1.0, ymin = 0.0, ymax = 1.0, wmax = 1.0;

	if (ix < 1 || ix > my numberOfColumns || iy < 1 || iy > my numberOfColumns) {
		return;
	}

	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = nc1; j <= nc2; j++) {
			if (my data[i][j] > wmax) {
				wmax = my data[i][j];
			}
		}
	}
	xmax = ymax = wmax;
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);

	for (long i = 1; i <= my numberOfRows; i++) {
		if (my rowLabels[i]) {
			Graphics_text (g, my data[i][ix], my data[i][iy], my rowLabels[i]);
		}
	}

	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
	Graphics_line (g, xmin, ymax, xmin, ymin);
	Graphics_line (g, xmin, ymin, xmax, ymin);
	/* Graphics_arc (g, xmin, ymin, xmax - xmin, 0, 90); */
	Graphics_unsetInner (g);

	if (garnish) {
		if (my columnLabels[ix]) {
			Graphics_textBottom (g, false, my columnLabels[ix]);
		}
		if (my columnLabels[iy]) {
			Graphics_textLeft (g, false, my columnLabels[iy]);
		}
	}
}

/******** MDSVEC *******************************************/

void structMDSVec :: v_destroy () noexcept {
	NUMvector_free<double> (proximity, 1);
	NUMvector_free<long> (iPoint, 1);
	NUMvector_free<long> (jPoint, 1);
	MDSVec_Parent :: v_destroy ();
}

autoMDSVec MDSVec_create (long nPoints) {
	try {
		autoMDSVec me = Thing_new (MDSVec);
		my nPoints = nPoints;
		my nProximities = nPoints * (nPoints - 1) / 2;
		my proximity = NUMvector<double> (1, my nProximities);
		my iPoint = NUMvector<long> (1, my nProximities);
		my jPoint = NUMvector<long> (1, my nProximities);
		return me;
	} catch (MelderError) {
		Melder_throw (U"MDSVec not created.");
	}
}

autoMDSVec Dissimilarity_to_MDSVec (Dissimilarity me) {
	try {
		autoMDSVec thee = MDSVec_create (my numberOfRows);

		long k = 0;
		for (long i = 1; i <= my numberOfRows - 1; i++) {
			for (long j = i + 1; j <= my numberOfColumns; j++) {
				double f = (my data[i][j] + my data[j][i]) / 2;
				if (f > 0.0) {
					k++;
					thy proximity[k] = f;
					thy iPoint[k] = i;
					thy jPoint[k] = j;
				}
			}
		}
		thy nProximities = k;
		NUMsort3 (thy proximity, thy iPoint, thy jPoint, 1, k, 1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MDSVec created.");
	}
}

/*********************** MDSVECS *******************************************/

autoMDSVecList DissimilarityList_to_MDSVecList (DissimilarityList me) {
	try {
		autoMDSVecList thee = MDSVecList_create ();
		for (long i = 1; i <= my size; i ++) {
			autoMDSVec him = Dissimilarity_to_MDSVec (my at [i]);
			Thing_setName (him.get(), Thing_getName (my at [i]));
			thy addItem_move (him.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MDSVecs created.");
	}
}


/**************************  CONFUSIONS **************************************/

autoConfusion ConfusionList_sum (ConfusionList me) {
	try {
		autoTableOfReal sum = TableOfRealList_sum (my asTableOfRealList());
		autoConfusion thee = TableOfReal_to_Confusion (sum.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": sum not created.");
	}
}


/**************************  DISTANCES **************************************/


/*****************  SCALARPRODUCT ***************************************/

autoScalarProduct ScalarProduct_create (long numberOfPoints) {
	try {
		autoScalarProduct me = Thing_new (ScalarProduct);
		TableOfReal_init (me.get(), numberOfPoints, numberOfPoints);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ScalarProduct not created.");
	}
}


/************* SCALARPRODUCTS **************************************/


/******************  DISSIMILARITY **********************************/

autoDissimilarity Dissimilarity_create (long numberOfPoints) {
	try {
		autoDissimilarity me = Thing_new (Dissimilarity);
		Proximity_init (me.get(), numberOfPoints);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Dissimilarity not created.");
	}
}

static double Dissimilarity_getAverage (Dissimilarity me) {
	double sum = 0.0;
	long numberOfPositives = 0;
	for (long i = 1; i <= my numberOfRows - 1; i++) {
		for (long j = i + 1; j <= my numberOfRows; j++) {
			double proximity = (my data[i][j] + my data[j][i]) / 2.0;
			if (proximity > 0.0) {
				numberOfPositives++;
				sum += proximity;
			}
		}
	}
	return ( numberOfPositives > 0 ? sum /= numberOfPositives : undefined );
}

double Dissimilarity_getAdditiveConstant (Dissimilarity me) {
	double additiveConstant = undefined;
	try {
		long nPoints = my numberOfRows, nPoints2 = 2 * nPoints;

		// Return c = average dissimilarity in case of failure

		if (nPoints < 1) {
			Melder_throw (U"Matrix part is empty.");
		}

		additiveConstant = Dissimilarity_getAverage (me);
		if (isundef (additiveConstant)) {
			Melder_throw (U"There are no positive dissimilarities.");
		}

		autoNUMmatrix<double> wd (1, nPoints, 1, nPoints);
		autoNUMmatrix<double> wdsqrt (1, nPoints, 1, nPoints);
		autoNUMmatrix<double> b (1, nPoints2, 1, nPoints2);
		autoNUMvector<double> eigenvalue (1, nPoints2);

		// The matrices D & D1/2 with distances (squared and linear)

		for (long i = 1; i <= nPoints - 1; i++) {
			for (long j = i + 1; j <= nPoints; j++) {
				double proximity = (my data[i][j] + my data[j][i]) / 2.0;
				wdsqrt[i][j] = - proximity / 2.0;
				wd[i][j] = - proximity * proximity / 2.0;
			}
		}

		NUMdoubleCentre (wdsqrt.peek(), 1, nPoints, 1, nPoints);
		NUMdoubleCentre (wd.peek(), 1, nPoints, 1, nPoints);

		// Calculate the B matrix according to eq. 6

		for (long i = 1; i <= nPoints; i++) {
			for (long j = 1; j <= nPoints; j++) {
				b[i][nPoints + j] = 2.0 * wd[i][j];
				b[nPoints + i][nPoints + j] = -4.0 * wdsqrt[i][j];
				b[nPoints + i][i] = -1.0;
			}
		}

		// Get eigenvalues and sort them descending

		NUMeigensystem (b.peek(), nPoints2, nullptr, eigenvalue.peek());
		if (eigenvalue[1] <= 0.0) {
			Melder_throw (U"Negative eigenvalue.");
		}
		additiveConstant = eigenvalue[1];
		return additiveConstant;
	} catch (MelderError) {
		Melder_throw (U"Additive constant not calculated.");
	}
}


/***************  DISSIMILARITIES **************************************/


/*************  SIMILARITY *****************************************/

autoSimilarity Similarity_create (long numberOfPoints) {
	try {
		autoSimilarity me = Thing_new (Similarity);
		Proximity_init (me.get(), numberOfPoints);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Similarity not created.");
	}
}

autoSimilarity Confusion_to_Similarity (Confusion me, bool normalize, int symmetrizeMethod) {
	try {
		if (my numberOfColumns != my numberOfRows) {
			Melder_throw (U"Confusion must be a square table.");
		}

		long nxy = my numberOfColumns;
		autoSimilarity thee = Similarity_create (nxy);

		TableOfReal_copyLabels (me, thee.get(), 1, 1);

		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);

		if (normalize) {
			NUMdmatrix_normalizeRows (thy data, nxy, nxy);
		}
		if (symmetrizeMethod == 1) {
			return thee;
		}
		if (symmetrizeMethod == 2) { // Average data
			for (long i = 1; i <= nxy - 1; i++) {
				for (long j = i + 1; j <= nxy; j++) {
					thy data[i][j] = thy data[j][i] = (thy data[i][j] + thy data[j][i]) / 2;
				}
			}
		} else if (symmetrizeMethod == 3) { // Method Houtgast.
			autoNUMmatrix<double> p (NUMmatrix_copy (thy data, 1, nxy, 1, nxy), 1, 1);
			for (long i = 1; i <= nxy; i++) {
				for (long j = i; j <= nxy; j++) {
					double tmp = 0;
					for (long k = 1; k <= nxy; k++) {
						tmp += p[i][k] < p[j][k] ? p[i][k] : p[j][k];
					}
					thy data[j][i] = thy data[i][j] = tmp;
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
		long nxy = my numberOfColumns;
		double max = 0;
		autoDissimilarity thee = Dissimilarity_create (nxy);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);

		for (long i = 1; i <= nxy; i++) {
			for (long j = 1; j <= nxy; j++) {
				if (thy data[i][j] > max) {
					max = thy data[i][j];
				}
			}
		}

		if (maximumDissimilarity <= 0) {
			maximumDissimilarity = max;
		}

		if (maximumDissimilarity < max) Melder_warning
			(U"Your maximumDissimilarity is smaller than the maximum similarity. Some data may be lost.");

		for (long i = 1; i <= nxy; i++) {
			for (long j = 1; j <= nxy; j++) {
				double d = maximumDissimilarity - thy data[i][j];
				thy data[i][j] = d > 0 ? d : 0;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Dissimilarity created.");
	}
}

autoDistance Dissimilarity_to_Distance (Dissimilarity me, int scale) {
	try {
		double additiveConstant = 0.0;

		autoDistance thee = Distance_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		if (scale == MDS_ORDINAL) {
			if (isundef (additiveConstant = Dissimilarity_getAdditiveConstant (me))) {
				Melder_warning (U"Dissimilarity_to_Distance: could not determine \"additive constant\", the average dissimilarity was used as its value.");
			}
		}
		for (long i = 1; i <= my numberOfRows - 1; i++) {
			for (long j = i + 1; j <= my numberOfColumns; j++) {
				double d = 0.5 * (my data[i][j] + my data[j][i]) + additiveConstant;
				thy data[i][j] = thy data[j][i] = d;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Distance created.");
	}
}

autoWeight Dissimilarity_to_Weight (Dissimilarity me) {
	try {
		autoWeight thee = Weight_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = i; j <= my numberOfRows; j++) {
				if (my data[i][j] > 0.0) {
					thy data[i][j] = 1.0;
				}
			}
			thy data[i][i] = 0.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Weight created.");
	}
}


autoDissimilarity Confusion_to_Dissimilarity_pdf (Confusion me, double minimumConfusionLevel) {
	try {
		if (my numberOfColumns != my numberOfRows) {
			Melder_throw (U"Confusion must be a square table.");
		}
		Melder_assert (minimumConfusionLevel > 0.0);
		autoDissimilarity thee = Dissimilarity_create (my numberOfColumns);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);

		// Correct "zero" responses.

		for (long i = 1; i <= my numberOfColumns; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				if (thy data[i][j] == 0.0) {
					thy data[i][j] = minimumConfusionLevel;
				}
			}
		}

		NUMdmatrix_normalizeRows (thy data, my numberOfColumns, my numberOfColumns);

		/*
			Consider the fraction as the fraction overlap between two gaussians with unequal sigmas (1 & s).
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

		for (long i = 1; i <= my numberOfColumns; i++) {
			for (long j = i + 1; j <= my numberOfColumns; j++) {
				double x = thy data[i][j] <= thy data[j][i] ? thy data[i][j] : thy data[j][i];
				double y = thy data[i][j] > thy data[j][i] ? thy data[i][j] : thy data[j][i];
				x = NUMinvGaussQ (x);
				y = NUMinvGaussQ (y);
				double d = x + y * exp ( (y * y - x * x) / 2.0);
				/* Melder_info ("i, j, x, y, d: %d %d %.17g %.17g %.17g", i, j, x, y, d); */
				thy data[i][j] = thy data [j][i] = d;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Dissimilarity created from pdf.");
	}
}

void Distance_and_Configuration_drawScatterDiagram (Distance me, Configuration him, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish) {
	autoDistance dist = Configuration_to_Distance (him);
	Proximity_Distance_drawScatterDiagram (me, dist.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

autoDissimilarity Distance_to_Dissimilarity (Distance me) {
	try {
		autoDissimilarity thee = Dissimilarity_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Dissimilarity not created from Distance.");
	}
}

autoConfiguration Distance_to_Configuration_torsca (Distance me, int numberOfDimensions) {
	try {
		if (numberOfDimensions > my numberOfRows) {
			Melder_throw (U"Number of dimensions too high.");
		}
		autoScalarProduct sp = Distance_to_ScalarProduct (me, false);
		autoConfiguration thee = Configuration_create (my numberOfRows, numberOfDimensions);
		TableOfReal_copyLabels (me, thee.get(), 1, 0);
		NUMprincipalComponents (sp -> data, my numberOfRows, numberOfDimensions, thy data);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (torsca method).");
	}
}

autoScalarProduct Distance_to_ScalarProduct (Distance me, bool normalize) {
	try {
		autoScalarProduct thee = ScalarProduct_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		for (long i = 1; i <= my numberOfRows - 1; i++) {
			for (long j = i + 1; j <= my numberOfColumns; j++) {

				// force symmetry by averaging!

				double d = 0.5 * (my data[i][j] + my data[j][i]);
				thy data[i][j] = thy data[j][i] = - 0.5 * d * d;
			}
		}
		TableOfReal_doubleCentre (thee.get());
		if (my name) {
			Thing_setName (thee.get(), my name);
		}
		if (normalize) {
			TableOfReal_normalizeTable (thee.get(), 1.0);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no ScalarProduct created.");
	}
}

/**********  Configuration & ..... ***********************************/


autoDistance Configuration_to_Distance (Configuration me) {
	try {
		autoDistance thee = Distance_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, -1);
		for (long i = 1; i <= thy numberOfRows - 1; i++) {
			for (long j = i + 1; j <= thy numberOfColumns; j++) {
				double dmax = 0.0, d = 0.0;

				/*
					first divide distance by maximum to prevent overflow when metric is a large number.
					d = (x^n)^(1/n) may overflow if x>1 & n >>1 even if d would not overflow!
					metric changed 24/11/97
					my w[k] * pow (|i-j|) instead of pow (my w[k] * |i-j|)
				*/

				for (long k = 1; k <= my numberOfColumns; k++) {
					double dtmp  = fabs (my data[i][k] - my data[j][k]);
					if (dtmp > dmax) {
						dmax = dtmp;
					}
				}
				if (dmax > 0.0) {
					for (long k = 1; k <= my numberOfColumns; k++) {
						double arg = fabs (my data[i][k] - my data[j][k]) / dmax;
						d += my w[k] * pow (arg, my metric);
					}
				}
				thy data[i][j] = thy data[j][i] = dmax * pow (d, 1.0 / my metric);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Distance created.");
	}
}

void Proximity_Distance_drawScatterDiagram (Proximity me, Distance thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish) {
	long n = my numberOfRows * (my numberOfRows - 1) / 2;
	double **x = my data, **y = thy data;

	if (n == 0) {
		return;
	}
	if (! TableOfReal_equalLabels (me, thee, 1, 1)) {
		Melder_throw (U"Proximity_Distance_drawScatterDiagram: Dimensions and labels must be the same.");
	}
	if (xmax <= xmin) {
		xmin = xmax = x[1][2];
		for (long i = 1; i <= thy numberOfRows - 1; i++) {
			for (long j = i + 1; j <= thy numberOfColumns; j++) {
				if (x[i][j] > xmax) {
					xmax = x[i][j];
				} else if (x[i][j] < xmin) {
					xmin = x[i][j];
				}
			}
		}
	}
	if (ymax <= ymin) {
		ymin = ymax = y[1][2];
		for (long i = 1; i <= my numberOfRows - 1; i++) {
			for (long j = i + 1; j <= my numberOfColumns; j++) {
				if (y[i][j] > ymax) {
					ymax = y[i][j];
				} else if (y[i][j] < ymin) {
					ymin = y[i][j];
				}
			}
		}
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_setInner (g);
	for (long i = 1; i <= thy numberOfRows - 1; i++) {
		for (long j = i + 1; j <= thy numberOfColumns; j++) {
			if (x[i][j] >= xmin && x[i][j] <= xmax && y[i][j] >= ymin && y[i][j] <= ymax) {
				Graphics_mark (g, x[i][j], y[i][j], size_mm, mark);
			}
		}
	}

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, true, U"Distance");
		Graphics_textBottom (g, true, U"Dissimilarity");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

autoDistanceList MDSVecList_Distance_monotoneRegression (MDSVecList me, Distance thee, int tiesHandling) {
	try {
		autoDistanceList him = DistanceList_create ();
		for (long i = 1; i <= my size; i ++) {
			MDSVec vec = my at [i];
			if (vec -> nPoints != thy numberOfRows) {
				Melder_throw (U"Dimension of MDSVec and Distance must be equal.");
			}
			autoDistance fit = MDSVec_Distance_monotoneRegression (vec, thee, tiesHandling);
			his addItem_move (fit.move());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"No DistanceList created from MDSVecList and Distance.");
	}
}

autoDistance MDSVec_Distance_monotoneRegression (MDSVec me, Distance thee, int tiesHandling) {
	try {
		long nProximities = my nProximities;
		if (thy numberOfRows != my nPoints) {
			Melder_throw (U"Distance and MDSVVec dimension do not agreee.");
		}
		autoNUMvector<double> distance (1, nProximities);
		autoNUMvector<double> fit (1, nProximities);
		autoDistance him = Distance_create (thy numberOfRows);
		TableOfReal_copyLabels (thee, him.get(), 1, 1);

		long *iPoint = my iPoint, *jPoint = my jPoint;
		for (long i = 1; i <= nProximities; i++) {
			distance[i] = thy data[iPoint[i]][jPoint[i]];
		}

		if (tiesHandling == MDS_PRIMARY_APPROACH || tiesHandling == MDS_SECONDARY_APPROACH) {
			/*
				Kruskal's primary approach to tie-blocks:
					Sort corresponding distances, with iPoint, and jPoint.
				Kruskal's secondary approach:
					Substitute average distance in each tie block
			*/
			long ib = 1;
			for (long i = 2; i <= nProximities; i++) {
				if (my proximity [i] == my proximity [i - 1]) {
					continue;
				}
				if (i - ib > 1) {
					if (tiesHandling == MDS_PRIMARY_APPROACH) {
						NUMsort3 (distance.peek(), iPoint, jPoint, ib, i - 1, 1); // sort ascending
					} else if (tiesHandling == MDS_SECONDARY_APPROACH) {
						double mean = 0.0;
						for (long j = ib; j <= i - 1; j++) {
							mean += distance [j];
						}
						mean /= (i - ib);
						for (long j = ib; j <= i - 1; j++) {
							distance [j] = mean;
						}
					}
				}
				ib = i;
			}
		}

		NUMmonotoneRegression (distance.peek(), nProximities, fit.peek());

		// Fill Distance with monotone regressed distances

		for (long i = 1; i <= nProximities; i++) {
			long ip = iPoint[i], jp = jPoint[i];
			his data[ip][jp] = his data[jp][ip] = fit[i];
		}

		// Make rest of distances equal to the maximum fit.

		for (long i = 1; i <= his numberOfRows - 1; i++) {
			for (long j = i + 1; j <= his numberOfColumns; j++) {
				if (his data[i][j] == 0.0) {
					his data[i][j] = his data[j][i] = fit[nProximities];
				}
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"Distance not created.");
	}
}


autoDistance Dissimilarity_Distance_monotoneRegression (Dissimilarity me, Distance thee, int tiesHandling) {
	try {
		if (thy numberOfRows != my numberOfRows) {
			Melder_throw (U"Dimensions do not agree.");
		}
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
		for (long i = 1; i <= my size; i ++) {
			autoScalarProduct sp = Distance_to_ScalarProduct (my at [i], normalize);
			thy addItem_move (sp.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no ScalarProducts created.");
	}
}

void DistanceList_to_Configuration_ytl (DistanceList me, int numberOfDimensions, int normalizeScalarProducts, autoConfiguration *out1, autoSalience *out2) {
	try {
		autoScalarProductList sp = DistanceList_to_ScalarProductList (me, normalizeScalarProducts);
		ScalarProductList_to_Configuration_ytl (sp.get(), numberOfDimensions, out1, out2);
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (ytl method).");
	}
}

void ScalarProductList_to_Configuration_ytl (ScalarProductList me, int numberOfDimensions, autoConfiguration *out1, autoSalience *out2) {
	long numberOfSources = my size;
	autoNUMvector<double **> ci (1, numberOfSources);
	try {
		long nPoints = my at [1] -> numberOfRows;

		autoConfiguration thee = Configuration_create (nPoints, numberOfDimensions);
		autoSalience mdsw = Salience_create (numberOfSources, numberOfDimensions);
		TableOfReal_copyLabels (my at [1], thee.get(), 1, 0);
		autoNUMvector<double> eval (1, numberOfSources);
		autoNUMmatrix<double> cl (1, numberOfDimensions, 1, numberOfDimensions);
		autoNUMmatrix<double> pmean (1, nPoints, 1, nPoints);
		autoNUMmatrix<double> y (1, nPoints, 1, numberOfDimensions);
		autoNUMmatrix<double> yinv (1, numberOfDimensions, 1, nPoints);
		autoNUMmatrix<double> a (1, numberOfSources, 1, numberOfSources);
		autoNUMmatrix<double> evec (1, numberOfSources, 1, numberOfSources);
		autoNUMmatrix<double> K (1, numberOfDimensions, 1, numberOfDimensions);

		Thing_setName (mdsw.get(), U"ytl");
		Thing_setName (thee.get(), U"ytl");
		TableOfReal_labelsFromCollectionItemNames (mdsw.get(), (Collection) me, 1, 0);   // FIXME cast

		// Determine the average scalar product matrix (Pmean) of
		// dimension [1..nPoints][1..nPoints].

		for (long i = 1; i <= numberOfSources; i ++) {
			ScalarProduct sp = my at [i];
			for (long j = 1; j <= nPoints; j ++) {
				for (long k = 1; k <= nPoints; k ++) {
					pmean [j] [k] += sp -> data [j] [k];
				}
			}
		}

		if (numberOfSources > 1) {
			for (long j = 1; j <= nPoints; j ++) {
				for (long k = 1; k <= nPoints; k ++) {
					pmean [j] [k] /= numberOfSources;
				}
			}
		}

		// Up to a rotation K, the initial configuration can be found by
		// extracting the first 'numberOfDimensions' principal components of Pmean.

		NUMdmatrix_into_principalComponents (pmean.peek(), nPoints, nPoints, numberOfDimensions, y.peek());
		NUMmatrix_copyElements (y.peek(), thy data, 1, nPoints, 1, numberOfDimensions);

		// We cannot determine weights from only one sp-matrix.

		if (numberOfSources == 1) {
			Melder_throw (U"Only one source.");
		}

		// Calculate the C[i] matrices [1..numberOfDimensions][1..numberOfDimensions]
		//  from the P[i] by: C[i] = (y'.y)^-1 . y' . P[i] . y . (y'.y)^-1 ==
		//	 yinv P[i] yinv'

		NUMpseudoInverse (y.peek(), nPoints, numberOfDimensions, yinv.peek(), 1e-14);

		for (long i = 1; i <= numberOfSources; i ++) {
			ScalarProduct sp = my at [i];
			ci [i] = NUMmatrix<double> (1, numberOfDimensions, 1, numberOfDimensions);
			for (long j = 1; j <= numberOfDimensions; j ++) {
				for (long k = 1; k <= numberOfDimensions; k ++) {
					for (long l = 1; l <= nPoints; l ++) {
						if (yinv [j] [l] != 0.0) {
							for (long m = 1; m <= nPoints; m ++) {
								ci [i] [j] [k] += yinv [j] [l] * sp -> data [l] [m] * yinv [k] [m];
							}
						}
					}
				}
			}
		}

		// Calculate the A[1..numberOfSources][1..numberOfSources] matrix by (eq.12):
		// a[i][j] = trace (C[i]*C[j]) - trace (C[i]) * trace (C[j]) / numberOfDimensions;
		// Get the first eigenvector and form matrix cl from a linear combination of the C[i]'s

		for (long i = 1; i <= numberOfSources; i ++) {
			for (long j = i; j <= numberOfSources; j ++) {
				a [j] [i] = a [i] [j] =  NUMtrace2 (ci [i], ci [j], numberOfDimensions)
					- NUMtrace (ci [i], numberOfDimensions) * NUMtrace (ci [j], numberOfDimensions) / numberOfDimensions;
			}
		}

		NUMeigensystem (a.peek(), numberOfSources, evec.peek(), eval.peek());

		for (long i = 1; i <= numberOfSources; i ++) {
			for (long j = 1; j <= numberOfDimensions; j ++) {
				for (long k = 1; k <= numberOfDimensions; k ++) {
					cl [j] [k] += ci [i] [j] [k] * evec [i] [1]; /* eq. (7) */
				}
			}
		}

		// The rotation K is obtained from the eigenvectors of cl
		// Is the following still correct??? eigensystem was not sorted??

		NUMeigensystem (cl.peek(), numberOfDimensions, K.peek(), nullptr);

		// Now get the configuration: X = Y.K

		for (long i = 1; i <= nPoints; i ++) {
			for (long j = 1; j <= numberOfDimensions; j ++) {
				double x = 0.0;
				for (long k = 1; k <= numberOfDimensions; k ++) {
					x += y [i] [k] * K [k] [j];
				}
				thy data [i] [j] = x;
			}
		}

		Configuration_normalize (thee.get(), 0, true);

		// And finally the weights: W[i] = K' C[i] K   (eq. (5)).
		// We are only interested in the diagonal of the resulting matrix W[i].

		for (long i = 1; i <= numberOfSources; i ++) {
			for (long j = 1; j <= numberOfDimensions; j ++) {
				double wt = 0.0;
				for (long k = 1; k <= numberOfDimensions; k ++) {
					if (K [k] [j] != 0.0) {
						for (long l = 1; l <= numberOfDimensions; l ++) {
							wt += K [k] [j] * ci [i] [k] [l] * K [l] [j];
						}
					}
				}
				mdsw -> data [i] [j] = wt;
			}
		}
		
		*out1 = thee.move(); *out2 = mdsw.move();
		for (long i = 1; i <= numberOfSources; i ++) {
			NUMmatrix_free<double> (ci [i], 1, 1);
		}
	} catch (MelderError) {
		for (long i = 1; i <= numberOfSources; i ++) {
			NUMmatrix_free<double> (ci [i], 1, 1);
		}
		Melder_throw (me, U": no Configuration (ytl) created.");
	}
}

autoDissimilarityList DistanceList_to_DissimilarityList (DistanceList me) {
	try {
		autoDissimilarityList thee = DissimilarityList_create ();
		for (long i = 1; i <= my size; i ++) {
			const char32 *name = Thing_getName (my at [i]);
			autoDissimilarity him = Distance_to_Dissimilarity (my at [i]);
			Thing_setName (him.get(), name ? name : U"untitled");
			thy addItem_move (him.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Dissimilarities created.");
	}
}

autoDistanceList DissimilarityList_to_DistanceList (DissimilarityList me, int measurementLevel) {
	try {
		autoDistanceList thee = DistanceList_create ();

		for (long i = 1; i <= my size; i ++) {
			autoDistance him = Dissimilarity_to_Distance (my at [i], measurementLevel == MDS_ORDINAL);
			const char32 *name = Thing_getName (my at [i]);
			Thing_setName (him.get(), name ? name : U"untitled");
			thy addItem_move (him.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no DistanceList created.");
	}
}

/*****************  Kruskal *****************************************/

static void smacof_guttmanTransform (Configuration cx, Configuration cz, Distance disp, Weight weight, double **vplus) {
	long nPoints = cx -> numberOfRows, nDimensions = cx -> numberOfColumns;
	double **z = cz -> data;

	autoNUMmatrix<double> b (1, nPoints, 1, nPoints);
	autoDistance distZ = Configuration_to_Distance (cz);

	// compute B(Z) (eq. 8.25)

	for (long i = 1; i <= nPoints; i++) {
		double sum = 0.0;
		for (long j = 1; j <= nPoints; j++) {
			double dzij = distZ -> data[i][j];
			if (i == j || dzij == 0.0) {
				continue;
			}
			b[i][j] = - weight -> data[i][j] * disp -> data[i][j] / dzij;
			sum += b[i][j];
		}
		b[i][i] = - sum;
	}

	// Guttman transform: Xu = (V+)B(Z)Z (eq. 8.29)

	for (long i = 1; i <= nPoints; i++) {
		for (long j = 1; j <= nDimensions; j++) {
			double xij = 0.0;
			for (long k = 1;  k <= nPoints; k++) {
				for (long l = 1; l <= nPoints; l++) {
					xij += vplus[i][k] * b[k][l] * z[l][j];
				}
			}
			cx -> data[i][j] = xij;
		}
	}
}

double Distance_Weight_stress (Distance fit, Distance conf, Weight weight, int stressMeasure) {
	double eta_fit, eta_conf, rho, stress = undefined, denum, tmp;

	Distance_Weight_rawStressComponents (fit, conf, weight, &eta_fit, &eta_conf, &rho);

	// All formula's for stress, except for raw stress, are independent of the
	// scale of the configuration, i.e., the distances conf[i][j].

	if (stressMeasure == MDS_NORMALIZED_STRESS) {
		denum = eta_fit * eta_conf;
		if (denum > 0.0) {
			stress = 1.0 - rho * rho / denum;
		}
	} else if (stressMeasure == MDS_STRESS_1) {
		denum = eta_fit * eta_conf;
		if (denum > 0.0) {
			tmp = 1.0 - rho * rho / denum;
			if (tmp > 0.0) {
				stress = sqrt (tmp);
			}
		}
	} else if (stressMeasure == MDS_STRESS_2) {
		double m = 0.0, wsum = 0.0, var = 0.0, **w = weight -> data;
		double **c = conf -> data;
		long nPoints = conf -> numberOfRows;

		// Get average distance

		for (long i = 1; i <= nPoints - 1; i++) {
			for (long j = i + 1; j <= nPoints; j++) {
				m += w[i][j] * c[i][j];
				wsum += w[i][j];
			}
		}
		m /= wsum;
		if (m > 0.0) {
			// Get variance

			for (long i = 1; i <= nPoints - 1; i++) {
				for (long j = i + 1; j <= nPoints; j++) {
					tmp = c[i][j] - m;
					var += w[i][j] * tmp * tmp;
				}
			}
			denum = var * eta_fit;
			if (denum > 0.0) {
				stress = sqrt ( (eta_fit * eta_conf - rho * rho) / denum);
			}
		}
	} else if (stressMeasure == MDS_RAW_STRESS) {
		stress = eta_fit + eta_conf - 2.0 * rho ;
	}
	return stress;
}

void Distance_Weight_rawStressComponents (Distance fit, Distance conf, Weight weight, double *p_etafit, double *p_etaconf, double *p_rho)
{
	long nPoints = conf -> numberOfRows;

	double etafit = 0.0, etaconf = 0.0, rho = 0.0;

	for (long i = 1; i <= nPoints - 1; i++) {
		double *wi = weight -> data[i];
		double *fiti = fit -> data[i];
		double *confi = conf -> data[i];

		for (long j = i + 1; j <= nPoints; j++) {
			etafit += wi[j] * fiti[j] * fiti[j];
			etaconf += wi[j] * confi[j] * confi[j];
			rho += wi[j] * fiti[j] * confi[j];
		}
	}
	if (p_etafit) {
		*p_etafit = etafit;
	}
	if (p_etaconf) {
		*p_etaconf = etaconf;
	}
	if (p_rho) {
		*p_rho = rho;
	}
}

double Dissimilarity_Configuration_Transformator_Weight_stress (Dissimilarity d, Configuration c, Transformator t, Weight w, int stressMeasure) {
	long nPoints = d -> numberOfRows;
	double stress = undefined;

	if (nPoints < 1 || nPoints != c -> numberOfRows  || nPoints != t -> numberOfPoints || (w && nPoints != w -> numberOfRows)) {
		Melder_throw (U"Incorrect number of points.");
	}
	autoWeight aw;
	if (! w) {
		aw = Weight_create (nPoints);
		w = aw.get();
	}
	autoDistance cdist = Configuration_to_Distance (c);
	autoMDSVec vec = Dissimilarity_to_MDSVec (d);
	autoDistance fit = Transformator_transform (t, vec.get(), cdist.get(), w);

	stress = Distance_Weight_stress (fit.get(), cdist.get(), w, stressMeasure);
	return stress;
}

double Dissimilarity_Configuration_Weight_absolute_stress (Dissimilarity d, Configuration c, Weight w, int stressMeasure) {
	autoTransformator t = Transformator_create (d -> numberOfRows);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.get(), w, stressMeasure);
	return stress;
}

double Dissimilarity_Configuration_Weight_ratio_stress (Dissimilarity d, Configuration c, Weight w, int stressMeasure) {
	autoRatioTransformator t = RatioTransformator_create (d -> numberOfRows);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.get(), w, stressMeasure);
	return stress;
}

double Dissimilarity_Configuration_Weight_interval_stress (Dissimilarity d, Configuration c, Weight w, int stressMeasure) {
	autoISplineTransformator t = ISplineTransformator_create (d -> numberOfRows, 0, 1);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.get(), w, stressMeasure);
	return stress;
}

double Dissimilarity_Configuration_Weight_monotone_stress (Dissimilarity d, Configuration c, Weight w, int tiesHandling, int stressMeasure) {
	autoMonotoneTransformator t = MonotoneTransformator_create (d -> numberOfRows);
	MonotoneTransformator_setTiesProcessing (t.get(), tiesHandling);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.get(), w, stressMeasure);
	return stress;
}

double Dissimilarity_Configuration_Weight_ispline_stress (Dissimilarity d, Configuration c, Weight w, long numberOfInteriorKnots, long order, int stressMeasure) {
	autoISplineTransformator t = ISplineTransformator_create (d -> numberOfRows, numberOfInteriorKnots, order);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.get(), w, stressMeasure);
	return stress;
}

void Distance_Weight_smacofNormalize (Distance me, Weight w) {
	double sumsq = 0.0;
	for (long i = 1; i <= my numberOfRows - 1; i++) {
		double *wi = w -> data[i];
		double *di = my data[i];
		for (long j = i + 1; j <= my numberOfRows; j++) {
			sumsq += wi[j] * di[j] * di[j];
		}
	}
	double scale = sqrt (my numberOfRows * (my numberOfRows - 1) / (2.0 * sumsq));
	for (long i = 1; i <= my numberOfRows - 1; i++) {
		for (long j = i + 1; j <= my numberOfRows; j++) {
			my data[j][i] = (my data[i][j] *= scale);
		}
	}
}

double Distance_Weight_congruenceCoefficient (Distance x, Distance y, Weight w) {
	long nPoints = x -> numberOfRows;
	if (y -> numberOfRows != nPoints || w -> numberOfRows != nPoints) {
		return 0.0;
	}

	double xy = 0.0, x2 = 0.0, y2 = 0.0;
	for (long i = 1; i <= nPoints - 1; i++) {
		double *xi = x -> data[i];
		double *yi = y -> data[i];
		double *wi = w -> data[i];
		for (long j = i + 1; j <= nPoints - 1; j++) {
			xy += wi[j] * xi[j] * yi[j];
			x2 += wi[j] * xi[j] * xi[j];
			y2 += wi[j] * yi[j] * yi[j];
		}
	}
	return xy / (sqrt (x2) * sqrt (y2));
}

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_smacof (Dissimilarity me, Configuration conf, Weight weight, Transformator t, double tolerance, long numberOfIterations, bool showProgress, double *stress) {
	try {
		long nPoints = conf -> numberOfRows;
		long nDimensions = conf -> numberOfColumns;
		double tol = 1e-6, stressp = 1e308, stres;
		bool no_weight = ! weight;

		if (my numberOfRows != nPoints || (!no_weight && weight -> numberOfRows != nPoints) || t -> numberOfPoints != nPoints) {
			Melder_throw (U"Dimensions not in concordance.");
		}
		autoWeight aw;
		if (no_weight) {
			aw = Weight_create (nPoints);
			weight = aw.get();
		}
		autoNUMmatrix<double> v (1, nPoints, 1, nPoints);
		autoNUMmatrix<double> vplus (1, nPoints, 1, nPoints);
		autoConfiguration z = Data_copy (conf);
		autoMDSVec vec = Dissimilarity_to_MDSVec (me);

		double **w = weight -> data;

		if (showProgress) {
			Melder_progress (0.0, U"MDS analysis");
		}

		// Get V (eq. 8.19).

		for (long i = 1; i <= nPoints; i++) {
			double wsum = 0;
			for (long j = 1; j <= nPoints; j++) {
				if (i == j) {
					continue;
				}
				v[i][j] = - w[i][j];
				wsum += w[i][j];
			}
			v[i][i] = wsum;
		}

		// V is row and column centered and therefore: rank(V) <= nPoints-1.
		// V^-1 does not exist -> get Moore-Penrose inverse.

		NUMpseudoInverse (v.peek(), nPoints, nPoints, vplus.peek(), tol);
		for (long iter = 1; iter <= numberOfIterations; iter++) {
			autoDistance dist = Configuration_to_Distance (conf);

			// transform & normalization

			autoDistance fit = Transformator_transform (t, vec.get(), dist.get(), weight);

			// Make conf the Guttman transform of z

			smacof_guttmanTransform (conf, z.get(), fit.get(), weight, vplus.peek());

			// Compute stress

			autoDistance cdist = Configuration_to_Distance (conf);

			stres = Distance_Weight_stress (fit.get(), cdist.get(), weight, MDS_NORMALIZED_STRESS);

			// Check stop criterium

			if (fabs (stres - stressp) / stressp < tolerance) {
				break;
			}

			// Make Z = X

			NUMmatrix_copyElements (conf -> data, z -> data, 1, nPoints, 1, nDimensions);

			stressp = stres;
			if (showProgress) {
				Melder_progress ((double) iter / (numberOfIterations + 1), U"kruskal: stress ", stres);
			}
		}
		if (showProgress) {
			Melder_progress (1.0);
		}
		if (stress) {
			*stress = stres;
		}
		return z;
	} catch (MelderError) {
		if (showProgress) {
			Melder_progress (1.0);
		}
		Melder_throw (me, U": no improved Configuration created (smacof method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_Transformator_multiSmacof (Dissimilarity me, Configuration conf,  Weight w, Transformator t, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	int showMulti = showProgress && numberOfRepetitions > 1;
	try {
		bool showSingle = ( showProgress && numberOfRepetitions == 1 );
		double stress, stressmax = 1e308;

		autoConfiguration cstart = Data_copy (conf);
		autoConfiguration  cbest = Data_copy (conf);

		if (showMulti) {
			Melder_progress (0.0, U"MDS many times");
		}

		for (long i = 1; i <= numberOfRepetitions; i++) {
			autoConfiguration cresult = Dissimilarity_Configuration_Weight_Transformator_smacof (me, cstart.get(), w, t, tolerance, numberOfIterations, showSingle, &stress);
			if (stress < stressmax) {
				stressmax = stress;
				cbest = cresult.move();
			}
			Configuration_randomize (cstart.get());
			TableOfReal_centreColumns (cstart.get());

			if (showMulti) {
				Melder_progress ( (double) i / (numberOfRepetitions + 1), i, U" from ", numberOfRepetitions);
			}
		}
		if (showMulti) {
			Melder_progress (1.0);
		}
		return cbest;
	} catch (MelderError) {
		if (showMulti) {
			Melder_progress (1.0);
		}
		Melder_throw (me, U": no improved Configuration created (smacodf method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_absolute_mds (Dissimilarity me, Configuration cstart, Weight w, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	try {
		autoTransformator t = Transformator_create (my numberOfRows);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.get(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no improved Configuration created (absolute mds method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_ratio_mds (Dissimilarity me, Configuration cstart, Weight w, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	try {
		autoRatioTransformator t = RatioTransformator_create (my numberOfRows);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.get(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no improved Configuration created (ratio mds method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_interval_mds (Dissimilarity me, Configuration cstart, Weight w, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	try {
		autoISplineTransformator t = ISplineTransformator_create (my numberOfRows, 0, 1);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.get(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no improved Configuration created (interval mds method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_monotone_mds (Dissimilarity me, Configuration cstart, Weight w, int tiesHandling, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	try {
		autoMonotoneTransformator t = MonotoneTransformator_create (my numberOfRows);
		MonotoneTransformator_setTiesProcessing (t.get(), tiesHandling);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.get(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no improved Configuration created (monotone mds method).");
	}
}

autoConfiguration Dissimilarity_Configuration_Weight_ispline_mds (Dissimilarity me, Configuration cstart, Weight w, long numberOfInteriorKnots, long order, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	try {
		autoISplineTransformator t = ISplineTransformator_create (my numberOfRows, numberOfInteriorKnots, order);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.get(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no improved Configuration created (ispline mds method).");
	}
}

autoConfiguration Dissimilarity_Weight_absolute_mds (Dissimilarity me, Weight w, long numberOfDimensions, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, MDS_ABSOLUTE);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_absolute_mds (me, cstart.get(), w, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (absolute mds method).");
	}
}

autoConfiguration Dissimilarity_Weight_interval_mds (Dissimilarity me, Weight w, long numberOfDimensions, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, MDS_RATIO);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_interval_mds (me, cstart.get(), w, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (interval mds method).");
	}
}

autoConfiguration Dissimilarity_Weight_monotone_mds (Dissimilarity me, Weight w, long numberOfDimensions, int tiesHandling, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, MDS_ORDINAL);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_monotone_mds (me, cstart.get(), w, tiesHandling, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (monotone mds method).");
	}
}

autoConfiguration Dissimilarity_Weight_ratio_mds (Dissimilarity me, Weight w, long numberOfDimensions, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, MDS_RATIO);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_ratio_mds (me, cstart.get(), w, tolerance,
		    numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (ratio mds method).");
	}
}

autoConfiguration Dissimilarity_Weight_ispline_mds (Dissimilarity me, Weight w, long numberOfDimensions, long numberOfInteriorKnots, long order, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, MDS_ORDINAL);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_ispline_mds (me, cstart.get(), w,
		    numberOfInteriorKnots, order, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (ispline mds method).");
	}
}

/***** classical **/

static void MDSVec_Distances_getStressValues (MDSVec me, Distance ddist, Distance dfit, int stress_formula, double *stress, double *s, double *t, double *dbar) {
	long nProximities = my nProximities;
	long *iPoint = my iPoint, *jPoint = my jPoint;
	double **dist = ddist -> data, **fit = dfit -> data;

	*s = *t = *dbar = 0.0;

	if (stress_formula == 2) {
		for (long i = 1; i <= nProximities; i++) {
			*dbar += dist[iPoint[i]][jPoint[i]];
		}
		*dbar /= nProximities;
	}

	for (long i = 1; i <= nProximities; i++) {
		long ii = iPoint[i], jj = jPoint[i];
		double st = dist[ii][jj] - fit[ii][jj];
		double tt = dist[ii][jj] - *dbar;
		*s += st * st; *t += tt * tt;
	}

	*stress = *t > 0.0 ? sqrt (*s / *t) : 0.0;
}

static double func (Daata object, const double p[]) {
	Kruskal me = (Kruskal) object;
	MDSVec him = my vec.get();
	double **x = my configuration -> data, s, t, dbar, stress;
	double metric = my configuration -> metric;
	long numberOfDimensions = my configuration -> numberOfColumns;
	long numberOfPoints = my configuration -> numberOfRows;
	int tiesHandling = my process == MDS_CONTINUOUS ? 1 : 0;

	// Substitute results of minimizer into configuration and
	// normalize the configuration

	NUMdvector_into_matrix (p, x, 1, numberOfPoints, 1, numberOfDimensions);

	// Normalize

	NUMcentreColumns (x, 1, numberOfPoints, 1, numberOfDimensions, nullptr);
	NUMnormalize (x, numberOfPoints, numberOfDimensions, sqrt (numberOfPoints));

	// Calculate interpoint distances from the configuration

	autoDistance dist = Configuration_to_Distance (my configuration.get());

	// Monotone regression

	autoDistance fit = MDSVec_Distance_monotoneRegression (my vec.get(), dist.get(), tiesHandling);

	// Get numerator and denominator of stress

	MDSVec_Distances_getStressValues (my vec.get(), dist.get(), fit.get(), my stress_formula, &stress, &s, &t, &dbar);

	// Gradient calculation.

	for (long i = 1; i <= numberOfPoints; i++) {
		for (long j = 1; j <= numberOfDimensions; j++) {
			my dx[i][j] = 0.0;
		}
	}

	// Prevent overflow when stress is small

	if (stress < 1e-6) {
		return stress;
	}

	for (long i = 1; i <= his nProximities; i++) {
		long ii = my vec -> iPoint[i], jj = my vec -> jPoint[i];
		double g1 = stress * ((dist->data[ii][jj] - fit->data[ii][jj]) / s - (dist->data[ii][jj] - dbar) / t);
		for (long j = 1; j <= numberOfDimensions; j++) {
			double dj = x[ii][j] - x[jj][j];
			double g2 = g1 * pow (fabs (dj) / dist->data[ii][jj], metric - 1.0);
			if (dj < 0.0) {
				g2 = -g2;
			}
			my dx[ii][j] += g2; my dx[jj][j] -= g2;
		}
	}

	// Increment the number of times this function has been called

	(my minimizer -> funcCalls) ++;
	return stress;
}

/* Precondition: configuration was not changed since previous call to func */
static void dfunc (Daata object, const double * /* p */, double dp[]) {
	Kruskal me = (Kruskal) object;
	Configuration thee = my configuration.get();

	long k = 1;
	for (long i = 1; i <= thy numberOfRows; i++) {
		for (long j = 1; j <= thy numberOfColumns; j++) {
			dp[k++] = my dx[i][j];
		}
	}
}

void structKruskal :: v_destroy () noexcept {
	NUMmatrix_free<double> (dx, 1, 1);
	Kruskal_Parent :: v_destroy ();
}

autoKruskal Kruskal_create (long numberOfPoints, long numberOfDimensions) {
	try {
		autoKruskal me = Thing_new (Kruskal);
		my configuration = Configuration_create (numberOfPoints, numberOfDimensions);
		my proximities = ProximityList_create ();
		my dx = NUMmatrix<double> (1, numberOfPoints, 1, numberOfDimensions);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Kruskal not created.");
	}
}

autoConfiguration Dissimilarity_to_Configuration_kruskal (Dissimilarity me, long numberOfDimensions, long /* metric */, int tiesHandling, int stress_formula, double tolerance, long numberOfIterations, long numberOfRepetitions) {
	try {
		int scale = 1;
		autoDistance d = Dissimilarity_to_Distance (me, scale);
		autoConfiguration c = Distance_to_Configuration_torsca (d.get(), numberOfDimensions);
		Configuration_normalize (c.get(), 1.0, false);
		autoConfiguration thee = Dissimilarity_Configuration_kruskal (me, c.get(), tiesHandling, stress_formula, tolerance, numberOfIterations, numberOfRepetitions);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Configuration created (kruskal method).");
	}
}

void Dissimilarity_Configuration_drawShepardDiagram (Dissimilarity me, Configuration him, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish) {
	autoDistance dist = Configuration_to_Distance (him);
	Proximity_Distance_drawScatterDiagram (me, dist.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

autoDistance Dissimilarity_Configuration_monotoneRegression (Dissimilarity dissimilarity, Configuration configuration, int tiesHandling) {
	try {
		autoDistance dist = Configuration_to_Distance (configuration);
		autoDistance result = Dissimilarity_Distance_monotoneRegression (dissimilarity, dist.get(), tiesHandling);
		return result;
	} catch (MelderError) {
		Melder_throw (U"No Distance created (monotone regression).");
	}
}

autoDistanceList DissimilarityList_Configuration_monotoneRegression (DissimilarityList me, Configuration configuration, int tiesHandling) {
	try {
		autoDistanceList thee = DistanceList_create ();
		autoDistance dist = Configuration_to_Distance (configuration);
		for (long i = 1; i <= my size; i ++) {
			autoDistance d = Dissimilarity_Distance_monotoneRegression (my at [i], dist.get(), tiesHandling);
			thy addItem_move (d.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No DistanceList created (monotone regression).");
	}
}

void Dissimilarity_Configuration_drawMonotoneRegression (Dissimilarity me, Configuration him, Graphics g, int tiesHandling, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish) {
	/* obsolete replace by transformator */
	autoDistance fit = Dissimilarity_Configuration_monotoneRegression (me, him, tiesHandling);
	Proximity_Distance_drawScatterDiagram (me, fit.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawAbsoluteRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish) {
	autoTransformator t = Transformator_create (d -> numberOfRows);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.get(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawRatioRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish) {
	autoRatioTransformator t = RatioTransformator_create (d -> numberOfRows);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.get(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawIntervalRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish) {
	Dissimilarity_Configuration_Weight_drawISplineRegression (d, c, w, g, 0.0, 1.0, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawMonotoneRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, int tiesHandling, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish) {
	autoMonotoneTransformator t = MonotoneTransformator_create (d->numberOfRows);
	MonotoneTransformator_setTiesProcessing (t.get(), tiesHandling);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.get(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.get(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawISplineRegression (Dissimilarity d, Configuration c, Weight w, Graphics g, long numberOfInternalKnots, long order, double xmin, double xmax, double ymin, double ymax, double size_mm, const char32 *mark, int garnish) {
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
	double stress = Distance_Weight_stress (fdist.get(), cdist.get(), weight, MDS_NORMALIZED_STRESS);
	return stress;
}

double Dissimilarity_Configuration_getStress (Dissimilarity me, Configuration him, int tiesHandling, int stress_formula) {
	autoDistance dist = Configuration_to_Distance (him);
	autoMDSVec vec = Dissimilarity_to_MDSVec (me);
	autoDistance fit = MDSVec_Distance_monotoneRegression (vec.get(), dist.get(), tiesHandling);
	double s, t, dbar, stress;
	MDSVec_Distances_getStressValues (vec.get(), dist.get(), fit.get(), stress_formula, &stress, &s, &t, &dbar);
	return stress;
}

autoConfiguration Dissimilarity_Configuration_kruskal (Dissimilarity me, Configuration him, int tiesHandling, int stress_formula, double tolerance, long numberOfIterations, long numberOfRepetitions) {
	try {
		// The Configuration is normalized: each dimension centred +
		//	total variance set

		long numberOfCoordinates = my numberOfRows * his numberOfColumns;
		long numberOfParameters = numberOfCoordinates - his numberOfColumns - 1;
		long numberOfData = my numberOfRows * (my numberOfRows - 1) / 2;

		if (numberOfData < numberOfParameters) {
			Melder_throw (U"The number of data must be larger than number of parameters in the model.");
		}

		autoKruskal thee = Kruskal_create (my numberOfRows, his numberOfColumns);
		TableOfReal_copyLabels (me, thy configuration.get(), 1, 0);
		autoDissimilarity dissimilarity = Data_copy (me);
		thy proximities -> addItem_move (dissimilarity.move());
		thy vec = Dissimilarity_to_MDSVec (me);

		thy minimizer = VDSmagtMinimizer_create (numberOfCoordinates, (Daata) thee.get(), func, dfunc);

		NUMdmatrix_into_vector (his data, thy minimizer -> p, 1, his numberOfRows, 1, his numberOfColumns);

		thy stress_formula = stress_formula;
		thy process = tiesHandling;
		Configuration_setMetric (thy configuration.get(), his metric);

		Minimizer_minimizeManyTimes (thy minimizer.get(), numberOfRepetitions, numberOfIterations, tolerance);

		// call the function to get the best configuration

		(void) func ((Daata) thee.get(), thy minimizer -> p); 

		autoConfiguration result = Data_copy (thy configuration.get()); // TODO move from its autoConfiguration
		return result;
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
	long nPoints = xc -> numberOfRows, nDimensions = xc -> numberOfColumns;
	long nSources = zc->size;
	double **x = xc -> data, **w = weights -> data, lambda;

	// tolerance = 1e-4 is nearly optimal for dominant eigenvector estimation.

	double tolerance = 1e-4;
	autoNUMmatrix<double> wsih (1, nPoints, 1, nPoints);
	autoNUMvector<double> solution (1, nPoints);

	for (long h = 1; h <= nDimensions; h ++) {
		autoScalarProductList sprc = Data_copy (zc);
		for (long k = 1; k <= nPoints; k ++) {
			for (long l = 1; l <= nPoints; l ++) {
				wsih [k] [l] = 0.0;
			}
		}

		for (long i = 1; i <= nSources; i ++) {
			ScalarProduct spr = sprc -> at [i];
			double **sih = spr -> data;

			// Construct the S[i][h] matrices (eq. 6)

			for (long j = 1; j <= nDimensions; j ++) {
				if (j == h) {
					continue;
				}
				for (long k = 1; k <= nPoints; k ++) {
					for (long l = 1; l <= nPoints; l ++) {
						sih [k] [l] -= x [k] [j] * x [l] [j] * w [i] [j];
					}
				}
			}

			// the weighted S matrix (eq. 8)

			for (long k = 1; k <= nPoints; k ++) {
				for (long l = 1; l <= nPoints; l ++) {
					wsih [k] [l] += w [i] [h] * sih [k] [l];
				}
			}
		}

		// largest eigenvalue of m (nonsymmetric matrix!!) is optimal solution for this dimension

		for (long k = 1; k <= nPoints; k ++) {
			solution[k] = x [k] [h];
		}

		NUMdominantEigenvector (wsih.peek(), nPoints, solution.peek(), & lambda, tolerance);

		// normalize the solution: centre and x'x = 1

		double mean = 0.0;
		for (long k = 1; k <= nPoints; k++) {
			mean += solution[k];
		}
		mean /= nPoints;

		if (mean == 0.0) {
			continue;
		}

		double scale = 0.0;
		for (long k = 1; k <= nPoints; k++) {
			solution[k] -= mean;
			scale += solution[k] * solution[k];
		}

		for (long k = 1; k <= nPoints; k++) {
			x[k][h] = solution[k] / sqrt (scale);
		}

		// update weights. Make negative weights zero.

		for (long i = 1; i <= nSources; i ++) {
			ScalarProduct spr = sprc -> at [i];
			double **sih = spr -> data, wih = 0.0;
			for (long k = 1; k <= nPoints; k ++) {
				for (long l = 1; l <= nPoints; l ++) {
					wih += x [k] [h] * sih [k] [l] * x [l] [h];
				}
			}
			if (wih < 0.0) {
				wih = 0.0;
			}
			w[i][h] = wih;
		}
	}
}


void ScalarProductList_Configuration_Salience_indscal (ScalarProductList sp, Configuration configuration, Salience weights, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *p_conf, autoSalience *p_sal, double *p_varianceAccountedFor) {
	try {
		double tol = 1e-6, vafp = 0.0, varianceAccountedFor;
		long nSources = sp->size, iter;

		autoConfiguration conf = Data_copy (configuration);
		autoSalience sal = Data_copy (weights);

		if (showProgress) {
			Melder_progress (0.0, U"INDSCAL analysis");
		}

		// Solve for X, and W matrix via Alternating Least Squares.

		for (iter = 1; iter <= numberOfIterations; iter++) {
			indscal_iteration_tenBerge (sp, conf.get(), sal.get());

			// Goodness of fit and test criterion.

			ScalarProductList_Configuration_Salience_vaf (sp, conf.get(), sal.get(), & varianceAccountedFor);

			if (varianceAccountedFor > 1.0 - tol || fabs (varianceAccountedFor - vafp) /  vafp < tolerance) {
				break;
			}
			vafp = varianceAccountedFor;
			if (showProgress) {
				Melder_progress ( (double) iter / (numberOfIterations + 1), U"indscal: varianceAccountedFor ", varianceAccountedFor);
			}
		}

		// Count number of zero weights

		long nZeros = NUMdmatrix_countZeros (sal -> data, sal -> numberOfRows, sal -> numberOfColumns);

		if (p_conf) {
			Thing_setName (conf.get(), U"indscal");
			*p_conf = conf.move();
		}
		if (p_sal) {
			Thing_setName (sal.get(), U"indscal");
			TableOfReal_labelsFromCollectionItemNames (sal.get(), (Collection) sp, 1, 0);   // FIXME cast
			*p_sal = sal.move();
		}
		if (p_varianceAccountedFor) {
			*p_varianceAccountedFor = varianceAccountedFor;
		}
		if (showProgress) {
			MelderInfo_writeLine (U"**************** INDSCAL results on Distances *******************\n\n",
				Thing_className (sp), U"number of objects: ", nSources);
			for (long i = 1; i <= nSources; i ++) {
				MelderInfo_writeLine (U"  ", Thing_getName (sp->at [i]));
			}
			if (nZeros > 0) {
				MelderInfo_writeLine (U"WARNING: ", nZeros,  U" zero weight", (nZeros > 1 ? U"s" : U""), U"!");
			}
			MelderInfo_writeLine (U"\n\nVariance Accounted For = ", varianceAccountedFor, U"\nThe optimal configuration was reached in ", (iter > numberOfIterations ? numberOfIterations : iter), U" iterations.");
			MelderInfo_drain();
		}
		if (showProgress) {
			Melder_progress (1.0);
		}
	} catch (MelderError) {
		if (showProgress) {
			Melder_progress (1.0);
		}
		Melder_throw (U"No indscal configuration calculated.");
	}
}

void DistanceList_Configuration_Salience_indscal (DistanceList distances, Configuration configuration, Salience weights, bool normalizeScalarProducts, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2, double *vaf) {
	try {
		autoScalarProductList sp = DistanceList_to_ScalarProductList (distances, normalizeScalarProducts);
		ScalarProductList_Configuration_Salience_indscal (sp.get(), configuration, weights, tolerance, numberOfIterations, showProgress, out1, out2, vaf);
	} catch (MelderError) {
		Melder_throw (U"No indscal configuration calculated.");
	}
}

void DissimilarityList_Configuration_Salience_indscal (DissimilarityList dissims, Configuration conf, Salience weights, int tiesHandling, bool normalizeScalarProducts, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *p_configuration, autoSalience *p_salience, double *varianceAccountedFor) {
	try {
		double tol = 1e-6, vafp = 0.0, vaf;
		long iter, nSources = dissims->size;
		autoConfiguration configuration = Data_copy (conf);
		autoSalience salience = Data_copy (weights);
		autoMDSVecList mdsveclist = DissimilarityList_to_MDSVecList (dissims);

		if (showProgress) {
			Melder_progress (0.0, U"INDSCAL analysis");
		}

		for (iter = 1; iter <= numberOfIterations; iter++) {
			autoDistanceList distances = MDSVecList_Configuration_Salience_monotoneRegression (mdsveclist.get(), configuration.get(), salience.get(), tiesHandling);
			autoScalarProductList sp = DistanceList_to_ScalarProductList (distances.get(), normalizeScalarProducts);

			indscal_iteration_tenBerge (sp.get(), configuration.get(), salience.get());

			// Goodness of fit and test criterion.

			DistanceList_Configuration_Salience_vaf (distances.get(), configuration.get(), salience.get(), normalizeScalarProducts, &vaf);

			if (vaf > 1.0 - tol || fabs (vaf - vafp) / vafp < tolerance) {
				break;
			}
			vafp = vaf;
			if (showProgress) {
				Melder_progress ((double) iter / (numberOfIterations + 1), U"indscal: vaf ", vaf);
			}
		}

		// Count number of zero weights

		long nZeros = NUMdmatrix_countZeros (salience -> data, salience -> numberOfRows, salience -> numberOfColumns);

		// Set labels & names.

		Thing_setName (configuration.get(), U"indscal_mr");
		Thing_setName (salience.get(), U"indscal_mr");
		TableOfReal_labelsFromCollectionItemNames (salience.get(), (Collection) dissims, 1, 0);   // FIXME cast

		if (p_configuration) {
			*p_configuration = configuration.move();
		}
		if (p_salience) {
			*p_salience = salience.move();
		}
		if (varianceAccountedFor) {
			*varianceAccountedFor = vaf;
		}

		if (showProgress) {
			MelderInfo_writeLine (U"**************** INDSCAL with monotone regression *******************");
			MelderInfo_writeLine (Thing_className (dissims));
			MelderInfo_writeLine (U"Number of objects: ", nSources);
			for (long i = 1; i <= nSources; i++) {
				MelderInfo_writeLine (U"  ", Thing_getName (dissims->at [i]));
			}
			if (nZeros > 0) {
				MelderInfo_writeLine (U"WARNING: ", nZeros, U" zero weight", (nZeros > 1 ? U"s" : U""));
			}
			MelderInfo_writeLine (U"Variance Accounted For: ", vaf);
			MelderInfo_writeLine (U"Based on MONOTONE REGRESSION");
			MelderInfo_writeLine (U"number of iterations: ", (iter > numberOfIterations ?	numberOfIterations : iter));
			MelderInfo_drain();
		}
		if (showProgress) {
			Melder_progress (1.0);
		}
	} catch (MelderError) {
		if (showProgress) {
			Melder_progress (1.0);
		}
		Melder_throw (U"No inscal configuration calculated.");
	}
}

void DistanceList_Configuration_indscal (DistanceList dists, Configuration conf, bool normalizeScalarProducts, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2) {
	try {
		autoSalience w = Salience_create (dists->size, conf -> numberOfColumns);
		double vaf;
		DistanceList_Configuration_Salience_indscal (dists, conf, w.get(), normalizeScalarProducts, tolerance, numberOfIterations, showProgress, out1, out2, &vaf);
	} catch (MelderError) {
		Melder_throw (U"No indscal performed.");
	}
}

autoDistanceList MDSVecList_Configuration_Salience_monotoneRegression (MDSVecList vecs, Configuration conf, Salience weights, int tiesHandling) {
	try {
		long nDimensions = conf -> numberOfColumns;
		autoNUMvector<double> w (NUMvector_copy (conf -> w, 1, nDimensions), 1);
		autoDistanceList distances = DistanceList_create ();
		for (long i = 1; i <= vecs->size; i ++) {
			NUMvector_copyElements (weights -> data[i], conf -> w, 1, nDimensions);
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

autoSalience DissimilarityList_Configuration_to_Salience (DissimilarityList me, Configuration him, int tiesHandling, bool normalizeScalarProducts) {
	try {
		autoDistanceList distances = DissimilarityList_Configuration_monotoneRegression (me, him, tiesHandling);
		autoSalience w = DistanceList_Configuration_to_Salience (distances.get(), him, normalizeScalarProducts);
		return w;
	} catch (MelderError) {
		Melder_throw (U"No Salience created.");
	}
}

void DissimilarityList_Configuration_indscal (DissimilarityList dissims, Configuration conf, int tiesHandling, bool normalizeScalarProducts, double tolerance, long numberOfIterations, bool showProgress, autoConfiguration *out1, autoSalience *out2) {
	try {
		autoDistanceList distances = DissimilarityList_Configuration_monotoneRegression (dissims, conf, tiesHandling);
		autoSalience weights = DistanceList_Configuration_to_Salience (distances.get(), conf, normalizeScalarProducts);
		double vaf;
		DissimilarityList_Configuration_Salience_indscal (dissims, conf, weights.get(), tiesHandling, normalizeScalarProducts, tolerance, numberOfIterations, showProgress, out1, out2, & vaf);
	} catch (MelderError) {
		Melder_throw (U"No indscal performed.");
	}
}

void DissimilarityList_indscal (DissimilarityList me, long numberOfDimensions, int tiesHandling, bool normalizeScalarProducts, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress, autoConfiguration *p_conf, autoSalience *p_sal) {
	int showMulti = showProgress && numberOfRepetitions > 1;
	try {
		bool showSingle = (showProgress && numberOfRepetitions == 1);
		double vaf, vafmin = 0.0;

		autoDistanceList distances = DissimilarityList_to_DistanceList (me, MDS_ORDINAL);
		autoConfiguration cstart; autoSalience wstart;
		DistanceList_to_Configuration_ytl (distances.get(), numberOfDimensions, normalizeScalarProducts, & cstart, & wstart);
		autoConfiguration conf = Data_copy (cstart.get());
		autoSalience sal = Data_copy (wstart.get());

		if (showMulti) {
			Melder_progress (0.0, U"Indscal many times");
		}

		for (long iter = 1; iter <= numberOfRepetitions; iter ++) {
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

			if (showMulti) {
				Melder_progress ( (double) iter / (numberOfRepetitions + 1), iter, U" from ", numberOfRepetitions);
			}
		}

		if (p_conf) {
			*p_conf = conf.move();
		}
		if (p_sal) {
			*p_sal = sal.move();
		}
		if (showMulti) {
			Melder_progress (1.0);
		}
	} catch (MelderError) {
		if (showMulti) {
			Melder_progress (1.0);
		}
		Melder_throw (me, U": no indscal performed.");
	}
}

void DistanceList_to_Configuration_indscal (DistanceList distances, long numberOfDimensions, bool normalizeScalarProducts, double tolerance, long numberOfIterations, long numberOfRepetitions, bool showProgress, autoConfiguration *p_conf, autoSalience *p_sal) {
	int showMulti = showProgress && numberOfRepetitions > 1;
	try {
		bool showSingle = ( showProgress && numberOfRepetitions == 1 );
		double vaf, vafmin = 0.0;

		autoConfiguration cstart;
		autoSalience wstart;
		DistanceList_to_Configuration_ytl (distances, numberOfDimensions, normalizeScalarProducts, & cstart, & wstart);
		autoConfiguration conf = Data_copy (cstart.get());
		autoSalience sal = Data_copy (wstart.get());

		if (showMulti) {
			Melder_progress (0.0, U"Indscal many times");
		}

		for (long i = 1; i <= numberOfRepetitions; i++) {
			autoConfiguration cresult;
			autoSalience wresult;
			DistanceList_Configuration_Salience_indscal (distances, cstart.get(), wstart.get(), normalizeScalarProducts,  tolerance, numberOfIterations, showSingle, &cresult, &wresult, &vaf);
			if (vaf > vafmin) {
				vafmin = vaf;
				conf = cresult.move();
				sal = wresult.move();
			}
			Configuration_randomize (cstart.get());
			Configuration_normalize (cstart.get(), 1.0, true);
			Salience_setDefaults (wstart.get());

			if (showMulti) {
				Melder_progress ((double) i / (numberOfRepetitions + 1), i, U" from ", numberOfRepetitions);
			}
		}

		if (p_conf) {
			*p_conf = conf.move();
		}
		if (p_sal) {
			*p_sal = sal.move();
		}
		if (showMulti) {
			Melder_progress (1.0);
		}
	} catch (MelderError) {
		if (showMulti) {
			Melder_progress (1.0);
		}
		Melder_throw (distances, U": no indscal performed.");
	}
}

void DissimilarityList_Configuration_Salience_vaf (DissimilarityList me, Configuration thee, Salience him, int tiesHandling, bool normalizeScalarProducts, double *vaf) {
	autoDistanceList distances = DissimilarityList_Configuration_monotoneRegression (me, thee, tiesHandling);
	DistanceList_Configuration_Salience_vaf (distances.get(), thee, him, normalizeScalarProducts, vaf);
}

void DistanceList_Configuration_vaf (DistanceList me, Configuration thee, bool normalizeScalarProducts, double *vaf) {
	autoSalience w = DistanceList_Configuration_to_Salience (me, thee, normalizeScalarProducts);
	DistanceList_Configuration_Salience_vaf (me, thee, w.get(), normalizeScalarProducts, vaf);
}

void DissimilarityList_Configuration_vaf (DissimilarityList me, Configuration thee, int tiesHandling, bool normalizeScalarProducts, double *vaf) {
	autoSalience w = DissimilarityList_Configuration_to_Salience (me, thee, tiesHandling, normalizeScalarProducts);
	DissimilarityList_Configuration_Salience_vaf (me, thee, w.get(), tiesHandling, normalizeScalarProducts, vaf);
}

void DistanceList_Configuration_Salience_vaf (DistanceList me, Configuration thee, Salience him, bool normalizeScalarProducts, double *vaf) {
	if (my size != his numberOfRows || thy numberOfColumns != his numberOfColumns) {
		Melder_throw (U"Dimensions must conform.");
	}

	autoScalarProductList sp = DistanceList_to_ScalarProductList (me, normalizeScalarProducts);
	ScalarProductList_Configuration_Salience_vaf (sp.get(), thee, him, vaf);
}

void ScalarProduct_Configuration_getVariances (ScalarProduct me, Configuration thee, double *p_varianceExplained, double *p_varianceTotal) {
	double varianceExplained = 0.0, varianceTotal = 0.0;
	autoDistance distance = Configuration_to_Distance (thee);
	autoScalarProduct fit = Distance_to_ScalarProduct (distance.get(), 0);

	// ScalarProduct is double centred, i.e., mean == 0.

	for (long j = 1; j <= my numberOfRows; j++) {
		for (long k = 1; k <= my numberOfColumns; k++) {
			double d2 = my data[j][k] - fit -> data[j][k];
			varianceExplained += d2 * d2;
			varianceTotal += my data[j][k] * my data[j][k];
		}
	}
	if (p_varianceExplained) {
		*p_varianceExplained = varianceExplained;
	}
	if (p_varianceTotal) {
		*p_varianceTotal = varianceTotal;
	}
}

void ScalarProductList_Configuration_Salience_vaf (ScalarProductList me, Configuration thee, Salience him, double *vaf) {
	autoNUMvector<double> w (NUMvector_copy (thy w, 1, thy numberOfColumns), 1); // save weights
	try {
		if (my size != his numberOfRows || thy numberOfColumns != his numberOfColumns) {
			Melder_throw (U"Dimensions of input objects must conform.");
		}

		double t = 0.0, n = 0.0;
		for (long i = 1; i <= my size; i ++) {

			ScalarProduct sp = my at [i];
			if (sp -> numberOfRows != thy numberOfRows) {
				Melder_throw (U"ScalarProduct ", i, U" does not match Configuration.");
			}

			// weigh configuration before calculating variances

			for (long j = 1; j <= thy numberOfColumns; j ++) {
				thy w [j] = sqrt (his data [i] [j]);
			}

			double vare, vart;
			ScalarProduct_Configuration_getVariances (sp, thee, & vare, & vart);

			t += vare;
			n += vart;
		}

		if (vaf) {
			*vaf = (n > 0.0 ? 1.0 - t / n : 0.0);
		}
		NUMvector_copyElements (w.peek(), thy w, 1, thy numberOfColumns); // restore weights
	} catch (MelderError) {
		NUMvector_copyElements (w.peek(), thy w, 1, thy numberOfColumns);
		Melder_throw (U"No vaf calculasted.");
	}
}

/********************** Examples *********************************************/

autoDissimilarity Dissimilarity_createLetterRExample (double noiseStd) {
	try {
		autoConfiguration r = Configuration_createLetterRExample (1);
		autoDistance d = Configuration_to_Distance (r.get());
		autoDissimilarity me = Distance_to_Dissimilarity (d.get());
		Thing_setName (me.get(), U"R");

		for (long i = 1; i <= my numberOfRows - 1; i++) {
			for (long j = i + 1; j <= my numberOfRows; j++) {
				double dis = my data[i][j];
				my data[j][i] = my data[i][j] = dis * dis + 5.0 + NUMrandomUniform (0.0, noiseStd);
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Dissimilarity for letter R example not created.");
	}
}

autoSalience Salience_createCarrollWishExample () {
	try {
		long numberOfSources = 8;
		double wx[9] = {0, 1.0, 0.866, 0.707, 0.5,   0.1, 0.5, 0.354, 0.1};
		double wy[9] = {0, 0.1, 0.5,   0.707, 0.866,   1, 0.1, 0.354, 0.5};
		const char32 *name[] = { U"", U"1", U"2", U"3", U"4", U"5", U"6", U"7", U"8"};
		autoSalience me = Salience_create (numberOfSources, 2);

		for (long i = 1; i <= numberOfSources; i++) {
			my data[i][1] = wx[i];
			my data[i][2] = wy[i];
			TableOfReal_setRowLabel (me.get(), i, name[i]);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Salience for Carroll Wish example not created.");
	}
}

autoCollection INDSCAL_createCarrollWishExample (double noiseRange) {
	try {
		autoConfiguration c = Configuration_createCarrollWishExample ();
		long numberOfObjects = c -> numberOfRows, numberOfSources = 8;
		autoSalience s = Salience_createCarrollWishExample ();
		autoCollection me = Collection_create ();
		for (long l = 1; l <= numberOfSources; l ++) {
			c -> w [1] = s -> data [l] [1];
			c -> w [2] = s -> data [l] [2];
			autoDistance d = Configuration_to_Distance (c.get());
			autoDissimilarity dissim = Distance_to_Dissimilarity (d.get());
			for (long i = 1; i <= numberOfObjects - 1; i ++) {
				for (long j = i + 1; j <= numberOfObjects; j ++) {
					dissim -> data [i] [j] = (dissim -> data [j] [i] += NUMrandomUniform (0.0, noiseRange));
				}
			}
			Thing_setName (dissim.get(), s -> rowLabels [l]);
			my addItem_move (dissim.move());
		}
		Thing_setName (me.get(), U"CarrollWish");
		return me;
	} catch (MelderError) {
		Melder_throw (U"Collection not created.");
	}
}

void drawSplines (Graphics g, double low, double high, double ymin, double ymax, int splineType, long order, const char32 *interiorKnots, int garnish) {
	long k = order, numberOfKnots, numberOfInteriorKnots = 0;
	long nSplines, n = 1000;
	double knot[101], y[1001];

	if (splineType == MDS_ISPLINE) {
		k++;
	}
	for (long i = 1; i <= k; i++) {
		knot[i] = low;
	}
	numberOfKnots = k;

	{ // scope
		char *start = Melder_peek32to8 (interiorKnots), *end;   // UGLY; because of non-availability of str32tod
		while (*start) {
			double value = strtod (start, &end);
			start = end;
			if (value < low || value > high) {
				Melder_warning (U"drawSplines: knots must be in interval (", low, U", ", high, U")");
				return;
			}
			if (numberOfKnots == 100) {
				Melder_warning (U"drawSplines: too many knots (101)");
				return;
			}
			knot [++ numberOfKnots] = value;
		}
	}

	numberOfInteriorKnots = numberOfKnots - k;
	for (long i = 1; i <= k; i ++) {
		knot[++numberOfKnots] = high;
	}

	nSplines = order + numberOfInteriorKnots;

	if (nSplines == 0) {
		return;
	}

	Graphics_setWindow (g, low, high, ymin, ymax);
	Graphics_setInner (g);
	for (long i = 1; i <= nSplines; i ++) {
		double x, yx, dx = (high - low) / (n - 1);
		for (long j = 1; j <= n; j++) {
			x = low + dx * (j - 1);
			if (splineType == MDS_MSPLINE) {
				(void) NUMmspline (knot, numberOfKnots, order, i, x, &yx);
			} else {
				(void) NUMispline (knot, numberOfKnots, order, i, x, &yx);
			}
			y[j] = yx < ymin ? ymin : yx > ymax ? ymax : yx;
		}
		Graphics_function (g, y, 1, n, low, high);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		static MelderString ts { };
		long lastKnot = splineType == MDS_ISPLINE ? numberOfKnots - 2 : numberOfKnots;
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, false, splineType == MDS_MSPLINE ? U"\\s{M}\\--spline" : U"\\s{I}\\--spline");
		Graphics_marksTop (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		if (low <= knot[order]) {
			if (order == 1) {
				MelderString_copy (&ts, U"t__1_");
			} else if (order == 2) {
				MelderString_copy (&ts,  U"{t__1_, t__2_}");
			} else {
				MelderString_copy (&ts, U"{t__1_..t__", order, U"_}");
			}
			Graphics_markBottom (g, low, false, false, false, ts.string);
		}
		for (long i = 1; i <= numberOfInteriorKnots; i ++) {
			if (low <= knot[k + i] && knot[k + i] < high) {
				MelderString_copy (&ts, U"t__", order + i, U"_");
				Graphics_markBottom (g, knot[k + i], false, true, true, ts.string);
				Graphics_markTop (g, knot[k + i], true, false, false, nullptr);
			}
		}
		if (knot[lastKnot - order + 1] <= high) {
			if (order == 1) {
				MelderString_copy (&ts, U"t__", lastKnot, U"_");
			} else {
				MelderString_copy (&ts, U"{t__", (order == 2 ? lastKnot - 1 : lastKnot - order + 1), U"_, t__", lastKnot, U"_}");
			}
			Graphics_markBottom (g, high, false, false, false, ts.string);
		}
	}
}

void drawMDSClassRelations (Graphics g) {
	long nBoxes = 6;
	double boxWidth = 0.3, boxWidth2 = boxWidth / 2.0, boxWidth3 = boxWidth / 3.0;
	double boxHeight = 0.1, boxHeight2 = boxHeight / 2.0;
	double boxHeight3 = boxHeight / 3.0;
	double r_mm = 3, dxt = 0.025, dyt = 0.03;
	double dboxx = 1.0 - 0.2 - 2.0 * boxWidth, dboxy = (1.0 - 4.0 * boxHeight) / 3.0;
	double x1, x2, xm, x23, x13, y1, y2, ym, y23, y13;
	double x[7] = {0.0, 0.2, 0.2, 0.7, 0.2, 0.7, 0.2}; /* left */
	double y[7] = {0.0, 0.9, 0.6, 0.6, 0.3, 0.3, 0.0}; /* bottom */
	const char32 *text[7] = {U"", U"Confusion", U"Dissimilarity  %\\de__%%ij%_",  U"Similarity", U"Distance  %d__%%ij%_, %d\\'p__%%ij%_", U"ScalarProduct", U"Configuration" };

	Graphics_setWindow (g, -0.05, 1.05, -0.05, 1.05);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (long i = 1; i <= nBoxes; i++) {
		x2 = x[i] + boxWidth; y2 = y[i] + boxHeight;
		xm = x[i] + boxWidth2; ym = y[i] + boxHeight2;
		Graphics_roundedRectangle (g, x[i], x2, y[i], y2, r_mm);
		Graphics_text (g, xm, ym, text[i]);
	}

	Graphics_setLineWidth (g, 2);
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);

	// Confusion to Dissimilarity

	xm = x[1] + boxWidth2;
	y2 = y[2] + boxHeight;
	Graphics_arrow (g, xm, y[1], xm, y2);
	Graphics_text (g, xm + dxt, y2 + dboxy / 2, U"pdf");

	// Confusion to Similarity

	x1 = x[1] + boxWidth;
	xm = x[3] + boxWidth2;
	ym = y[1] + boxHeight2;
	y2 = y[3] + boxHeight;
	Graphics_line (g, x1, ym, xm, ym);
	Graphics_arrow (g, xm, ym, xm, y2);
	y2 += + dboxy / 2 + dyt / 2;
	Graphics_text (g, xm + dxt, y2, U"average");
	y2 -= dyt;
	Graphics_text (g, xm + dxt, y2, U"houtgast");

	// Dissimilarity to Similarity

	x1 = x[2] + boxWidth;
	y23 = y[2] + 2 * boxHeight3;
	Graphics_arrow (g, x1, y23, x[3], y23);
	y13 = y[2] + boxHeight3;
	Graphics_arrow (g, x[3], y13, x1, y13);

	// Dissimilarity to Distance

	x13 = x[4] + boxWidth3;
	y1 = y[4] + boxHeight;
	Graphics_arrow (g, x13, y1, x13, y[2]);
	x23 = x[4] + 2 * boxWidth3;
	Graphics_arrow (g, x23, y[2], x23, y1);

	x1 = x23 + dxt;
	y1 = y[2] - dyt;
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

	x1 = x[4] + boxWidth;
	ym = y[4] + boxHeight2;
	Graphics_arrow (g, x1, ym, x[5], ym);

	// Distance to Configuration

	y1 = y[6] + boxHeight;
	Graphics_arrow (g, x13, y1, x13, y[4]);

	// ScalarProduct to Configuration

	x13 = x[5] + boxWidth3;
	x23 = x[6] + 2 * boxWidth3;
	y1 = y[5] - dboxy / 2;
	Graphics_line (g, x13, y[5], x13, y1);
	Graphics_line (g, x13, y1, x23, y1);
	Graphics_arrow (g, x23, y1, x23, y[6] + boxHeight);
	x1 = x[6] + boxWidth + dboxx / 2;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_text (g, x1, y1, U"\\s{TORSCA}");
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_TOP);
	Graphics_text (g, x1, y1, U"\\s{YTL}");

	Graphics_setLineType (g, Graphics_DOTTED);

	x23 = x[5] + 2 * boxWidth3;
	ym = y[6] + boxHeight2;
	Graphics_line (g, x23, y[5], x23, ym);
	Graphics_arrow (g, x23, ym, x[6] + boxWidth, ym);
	x1 = x[6] + boxWidth + dboxx / 2 + boxWidth3;
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	Graphics_text (g, x1, ym, U"\\s{INDSCAL}");

	// Dissimilarity to Configuration

	ym = y[2] + boxHeight2;
	y2 = y[6] + boxHeight2;
	Graphics_line (g, x[2], ym, 0, ym);
	Graphics_line (g, 0, ym, 0, y2);
	Graphics_arrow (g, 0, y2, x[6], y2);

	// Restore settings

	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setLineWidth (g, 1.0);
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);
}

/* End of file MDS.cpp */
