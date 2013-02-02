/* MDS.cpp
 *
 * Copyright (C) 1993-2013 David Weenink
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

/********************** NUMERICAL STUFF **************************************/

static void NUMdmatrix_into_vector (double **m, double *v, long r1, long r2,
                                    long c1, long c2) {
	long k = 1;

	for (long i = r1; i <= r2; i++) {
		for (long j = c1; j <= c2; j++) {
			v[k++] = m[i][j];
		}
	}
}

static void NUMdvector_into_matrix (const double *v, double **m,
                                    long r1, long r2, long c1, long c2) {
	long k = 1;

	for (long i = r1; i <= r2; i++) {
		for (long j = c1; j <= c2; j++) {
			m[i][j] = v[k++];
		}
	}
}

static void NUMdmatrix_normalizeRows (double **m, long nr, long nc) {
	for (long i = 1; i <= nr; i++) {
		double rowSum = 0;
		for (long j = 1; j <= nc; j++) {
			rowSum += m[i][j];
		}

		if (rowSum != 0) {
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
		Melder_throw ("invalid range.");
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
		for (long j = ifrom; j <= ifrom + n / 2; j++) {
			long tmp = indx[j];
			indx[j] = indx[ito - j + ifrom];
			indx[ito - j + ifrom] = tmp;
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

/************ Configurations & Similarity **************************/

Distances Configurations_to_Distances (Configurations me) {
	try {
		autoDistances thee = Distances_create ();
		for (long i = 1; i <= my size; i++) {
			Configuration conf = (Configuration) (my item[i]);
			autoDistance d = Configuration_to_Distance (conf);
			Thing_setName (d.peek(), Thing_getName (conf));
			Collection_addItem (thee.peek(), d.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Distances not created form Configurations.");
	}
}

Similarity Configurations_to_Similarity_cc (Configurations me, Weight weight) {
	try {
		autoDistances d = Configurations_to_Distances (me);
		autoSimilarity thee = Distances_to_Similarity_cc (d.peek(), weight);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Similarity not created form Configurations.");
	}
}

Similarity Distances_to_Similarity_cc (Distances me, Weight w) {
	try {
		if (my size == 0) {
			Melder_throw ("Distances is empty.");
		}
		if (! TablesOfReal_checkDimensions (me)) {
			Melder_throw ("All matrices must have the same dimensions.");
		}
		autoWeight aw = 0;
		if (w == 0) {
			aw.reset (Weight_create ( ( (Distance) (my item[1])) -> numberOfRows));
			w = aw.peek();
		}

		autoSimilarity thee = Similarity_create (my size);

		for (long i = 1; i <= my size; i++) {
			Distance di = (Distance) (my item[i]);
			wchar_t *name = Thing_getName (di);
			TableOfReal_setRowLabel (thee.peek(), i, name);
			TableOfReal_setColumnLabel (thee.peek(), i, name);
			thy data[i][i] = 1;
			for (long j = i + 1; j <= my size; j++) {
				Distance dj = (Distance) (my item[j]);
				thy data[i][j] = thy data[j][i] = Distance_Weight_congruenceCoefficient (di, dj, w);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Similarity not created form Distancess.");
	}
}


/***************** Transformator **********************************************/

Thing_implement (Transformator, Thing, 0);

Distance structTransformator :: v_transform (MDSVec vec, Distance dist, Weight w) {
	try {
		(void) w;

		autoDistance thee = Distance_create (numberOfPoints);
		TableOfReal_copyLabels (dist, thee.peek(), 1, 1);
		/*
			Absolute scaling
		*/
		for (long i = 1; i <= vec -> nProximities; i++) {
			long ii = vec -> iPoint[i];
			long jj = vec -> jPoint[i];
			thy data[ii][jj] = thy data[jj][ii] = vec -> proximity[i];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Distance not created.");
	}
}

void Transformator_init (I, long numberOfPoints) {
	iam (Transformator);
	my numberOfPoints = numberOfPoints;
	my normalization = 1;
}

Transformator Transformator_create (long numberOfPoints) {
	try {
		autoTransformator me = Thing_new (Transformator);
		Transformator_init (me.peek(), numberOfPoints);
		my normalization = 0;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("No Transformator created.");
	}
}

Distance Transformator_transform (I, MDSVec vec, Distance d, Weight w) {
	iam (Transformator);
	try {
		if (my numberOfPoints != vec -> nPoints ||
		        my numberOfPoints != d -> numberOfRows ||
		        d -> numberOfRows != w -> numberOfRows) {
			Melder_throw (L"Dimensions do not agree.");
		}
		return my v_transform (vec, d, w);
	} catch (MelderError) {
		Melder_throw (me, "Distance not created.");
	}
}

Thing_implement (RatioTransformator, Transformator, 0);

Distance structRatioTransformator :: v_transform (MDSVec vec, Distance d, Weight w) {
	autoDistance thee = Distance_create (numberOfPoints);
	TableOfReal_copyLabels (d, thee.peek(), 1, 1);

	// Determine ratio (eq. 9.4)

	double etaSq = 0, rho = 0;
	for (long i = 1; i <= vec -> nProximities; i++) {
		long ii = vec -> iPoint[i];
		long jj = vec -> jPoint[i];
		double delta_ij = vec -> proximity[i], d_ij = d -> data[ii][jj];
		double tmp = w -> data[ii][jj] * delta_ij * delta_ij;
		etaSq += tmp;
		rho += tmp * d_ij * d_ij;
	}

	// transform

	if (etaSq == 0) {
		Melder_throw ("Eta squared is zero.");
	}
	this -> ratio = rho / etaSq;
	for (long i = 1; i <= vec -> nProximities; i++) {
		long ii = vec -> iPoint[i];
		long jj = vec -> jPoint[i];
		thy data[ii][jj] = thy data[jj][ii] = this -> ratio * vec -> proximity[i];
	}

	if (normalization) {
		Distance_Weight_smacofNormalize (thee.peek(), w);
	}
	return thee.transfer();
}

RatioTransformator RatioTransformator_create (long numberOfPoints) {
	try {
		autoRatioTransformator me = Thing_new (RatioTransformator);
		Transformator_init (me.peek(), numberOfPoints);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("RatioTransformator not created.");
	}
}

Thing_implement (MonotoneTransformator, Transformator, 0);

Distance structMonotoneTransformator :: v_transform (MDSVec vec, Distance d, Weight w) {
	try {
		autoDistance thee = MDSVec_Distance_monotoneRegression (vec, d, tiesProcessing);
		if (normalization) {
			Distance_Weight_smacofNormalize (thee.peek(), w);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Distance not created.");
	}

}

MonotoneTransformator MonotoneTransformator_create (long numberOfPoints) {
	try {
		autoMonotoneTransformator me = Thing_new (MonotoneTransformator);
		Transformator_init (me.peek(), numberOfPoints);
		my tiesProcessing = MDS_PRIMARY_APPROACH;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("MonotoneTransformator not created.");
	}
}

void MonotoneTransformator_setTiesProcessing (MonotoneTransformator me,
        int tiesProcessing) {
	my tiesProcessing = tiesProcessing;
}

Thing_implement (ISplineTransformator, Transformator, 0);

void structISplineTransformator :: v_destroy () {
	NUMvector_free<double> (b, 1);
	NUMvector_free<double> (knot, 1);
	NUMmatrix_free<double> (m, 1, 1);
	ISplineTransformator_Parent :: v_destroy ();
}

Distance structISplineTransformator :: v_transform (MDSVec vec, Distance dist, Weight w) {
	double tol = 1e-6;
	long itermax = 20, nx = vec -> nProximities;
	long nKnots = numberOfInteriorKnots + order + order + 2;

	autoDistance thee = Distance_create (dist -> numberOfRows);
	TableOfReal_copyLabels (dist, thee.peek(), 1, -1);

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

	/*
		Calculate data matrix m.
	*/

	for (long i = 1; i <= nx; i++) {
		double y, x = vec -> proximity [i];
		m [i] [1] = 1;
		for (long j = 2; j <= numberOfParameters; j++) {
			try {
				NUMispline (knot, nKnots, order, j - 1, x, & y);
			} catch (MelderError) {
				Melder_throw ("I-spline[", j - 1, "], data[", i, "d] = ", x);
			}
			m [i] [j] = y;
		}
	}

	NUMsolveNonNegativeLeastSquaresRegression (m, nx, numberOfParameters,
	        d.peek(), tol, itermax, b);

	for (long i = 1; i <= nx; i++) {
		long ii = vec->iPoint[i];
		long jj = vec->jPoint[i];
		double r = 0;

		for (long j = 1; j <= numberOfParameters; j++) {
			r += m[i][j] * b[j];
		}
		thy data[ii][jj] = thy data[jj][ii] = r;
	}

	if (normalization) {
		Distance_Weight_smacofNormalize (thee.peek(), w);
	}
	return thee.transfer();
}

ISplineTransformator ISplineTransformator_create (long numberOfPoints,
        long numberOfInteriorKnots, long order) {
	try {
		autoISplineTransformator me = Thing_new (ISplineTransformator);
		long nData = (numberOfPoints - 1) * numberOfPoints / 2;

		Transformator_init (me.peek(), numberOfPoints);

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
			my b[i] = NUMrandomUniform (0, 1);
		}

		my numberOfInteriorKnots = numberOfInteriorKnots;
		my order = order;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("ISplineTransformator not created.");
	}
}

/***************** CONTINGENCYTABLE **************************************/

Configuration ContingencyTable_to_Configuration_ca (ContingencyTable me, long numberOfDimensions, int scaling) {
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
			Melder_throw ("Dimension too high.");
		}

		/*
			Ref: A. Gifi (1990), Nonlinear Multivariate Analysis, Wiley & Sons,
				reprinted 1996,
				Chapter 8, Multidimensional scaling and Correspondence Analysis.
			Get row and column marginals
		*/

		double sum = 0;
		for (long i = 1; i <= nr; i++) {
			for (long j = 1; j <= nc; j++) {
				rowsum[i] += my data[i][j];
				colsum[j] += my data[i][j];
			}
			if (rowsum[i] <= 0) {
				Melder_throw ("Empty row: ", i, ".");
			}
			sum += rowsum[i];
		}

		for (long j = 1; j <= nc; j++) {
			if (colsum[j] <= 0) {
				Melder_throw ("Empty column: ", j, ".");
			}
		}

		/*
			Remove trivial singular vectors (Eq. 8.24),
			construct Dr^(-1/2) H Dc^(-1/2) - Dr^(1/2) uu' Dc^(1/2) / N
		*/

		for (long i = 1; i <= nr; i++) {
			for (long j = 1; j <= nc; j++) {
				double rc = sqrt (rowsum[i] * colsum[j]);
				h[i][j] = h[i][j] / rc - rc / sum;
			}
		}

		/*
			Singular value decomposition of h
		*/

		autoSVD svd = SVD_create_d (h.peek(), nr, nc);
		SVD_zeroSmallSingularValues (svd.peek(), 0);

		/*
			Scale row vectors and column vectors to configuration.
		*/

		for (long j = 1; j <= numberOfDimensions; j++) {
			double rootsum = sqrt (sum), xfactor, yfactor, lambda = svd -> d[j];
			if (scaling == 1) {
				/*
					Scale row points in the centre of gravity of
					column points (eq 8.5.a)
				*/
				xfactor = rootsum * lambda;
				yfactor = rootsum;
			} else if (scaling == 2) {
				/*
					Scale column points in the centre of gravity of
					row points (8.5.b)
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
			for (long i = 1; i <= nr; i++) {
				thy data[i][j] = svd -> u[i][j] * xfactor / sqrt (rowsum[i]);
			}
			for (long i = 1; i <= nc; i++) {
				thy data[nr + i][j] = svd -> v[i][j] * yfactor / sqrt (colsum[i]);
			}
		}

		TableOfReal_setSequentialColumnLabels (thee.peek(), 0, 0, NULL, 1, 1);
		NUMstrings_copyElements (my rowLabels, thy rowLabels, 1, nr);
		for (long i = 1; i <= nc; i++) {
			if (my columnLabels[i]) {
				TableOfReal_setRowLabel (thee.peek(), nr + i, my columnLabels[i]);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created.");
	}
}

Dissimilarity TableOfReal_to_Dissimilarity (I) {
	iam (TableOfReal);
	try {
		if (my numberOfRows != my numberOfColumns) {
			Melder_throw ("TableOfReal must be a square tabel.");
		}
		TableOfReal_checkPositive (me);
		autoDissimilarity thee = Thing_new (Dissimilarity);
		my structTableOfReal :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Dissimilarity.");
	}
}

Similarity TableOfReal_to_Similarity (I) {
	iam (TableOfReal);
	try {
		if (my numberOfRows != my numberOfColumns) {
			Melder_throw ("TableOfReal must be a square table.");
		}
		TableOfReal_checkPositive (me);
		autoSimilarity thee = Thing_new (Similarity);
		my structTableOfReal :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Similarity.");
	}
}

Distance TableOfReal_to_Distance (I) {
	iam (TableOfReal);
	try {
		if (my numberOfRows != my numberOfColumns) {
			Melder_throw ("TableOfReal must be a square table.");
		}
		TableOfReal_checkPositive (me);
		autoDistance thee = Thing_new (Distance);
		my structTableOfReal :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Distance.");
	}
}

Salience TableOfReal_to_Salience (I) {
	iam (TableOfReal);
	try {
		TableOfReal_checkPositive (me);
		autoSalience thee = Thing_new (Salience);
		my structTableOfReal :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Salience.");
	}
}

Weight TableOfReal_to_Weight (I) {
	iam (TableOfReal);
	try {
		TableOfReal_checkPositive (me);
		autoWeight thee = Thing_new (Weight);
		my structTableOfReal :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Weight.");
	}
}

ScalarProduct TableOfReal_to_ScalarProduct (I) {
	iam (TableOfReal);
	try {
		if (my numberOfRows != my numberOfColumns) {
			Melder_throw ("TableOfReal must be a square table.");
		}
		autoScalarProduct thee = Thing_new (ScalarProduct);
		my structTableOfReal :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to ScalarProduct.");
	}
}

/**************** Covariance & Correlation to Configuration *****************/

Configuration SSCP_to_Configuration (I, long numberOfDimensions) {
	iam (SSCP);
	try {
		autoConfiguration thee = Configuration_create (my numberOfRows, numberOfDimensions);
		autoPCA a = SSCP_to_PCA (me);
		TableOfReal_setSequentialColumnLabels (thee.peek(), 0, 0, NULL, 1, 1);

		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= numberOfDimensions; j++) {
				double s = 0;
				for (long k = 1; k <= my numberOfRows; k++) {
					s += my data[k][i] * a -> eigenvectors[k][j];
				}
				thy data[i][j] = s;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created.");
	}
}

Configuration Covariance_to_Configuration (Covariance me,
        long numberOfDimensions) {
	return SSCP_to_Configuration (me, numberOfDimensions);
}

Configuration Correlation_to_Configuration (Correlation me,
        long numberOfDimensions) {
	return SSCP_to_Configuration (me, numberOfDimensions);
}

/**************************** Weight *****************************************/

Thing_implement (Weight, TableOfReal, 0);

Weight Weight_create (long numberOfPoints) {
	try {
		autoWeight me = Thing_new (Weight);
		TableOfReal_init (me.peek(), numberOfPoints, numberOfPoints);
		for (long i = 1; i <= numberOfPoints; i++) {
			for (long j = 1; j <= numberOfPoints; j++) {
				my data[i][j] = 1;
			}
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Weight not created.");
	}
}


/**************** Salience *****************************************/

Thing_implement (Salience, TableOfReal, 0);

Salience Salience_create (long numberOfSources, long numberOfDimensions) {
	try {
		autoSalience me = Thing_new (Salience);
		TableOfReal_init (me.peek(), numberOfSources, numberOfDimensions);
		Salience_setDefaults (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Salience not created.");
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
			if (my data[i][j] < 0) {
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
			my data[i][j] = 1 / sqrt (my numberOfColumns);
		}
	}
	for (long j = 1; j <= my numberOfColumns; j++) {
		wchar_t s[40];
		swprintf (s, 40, L"dimension %ld", j);
		TableOfReal_setColumnLabel (me, j, s);
	}
}

void Salience_draw (Salience me, Graphics g, int ix, int iy, int garnish) {
	long nc2, nc1 = ix < iy ? (nc2 = iy, ix) : (nc2 = ix, iy);
	double xmin = 0, xmax = 1, ymin = 0, ymax = 1, wmax = 1;

	if (ix < 1 || ix > my numberOfColumns ||
	        iy < 1 || iy > my numberOfColumns) {
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
			Graphics_textBottom (g, 0, my columnLabels[ix]);
		}
		if (my columnLabels[iy]) {
			Graphics_textLeft (g, 0, my columnLabels[iy]);
		}
	}
}

/******** MDSVEC *******************************************/

Thing_implement (MDSVec, Data, 0);

void structMDSVec :: v_destroy () {
	NUMvector_free<double> (proximity, 1);
	NUMvector_free<long> (iPoint, 1);
	NUMvector_free<long> (jPoint, 1);
	MDSVec_Parent :: v_destroy ();
}

MDSVec MDSVec_create (long nPoints) {
	try {
		autoMDSVec me = Thing_new (MDSVec);
		my nPoints = nPoints;
		my nProximities = nPoints * (nPoints - 1) / 2;
		my proximity = NUMvector<double> (1, my nProximities);
		my iPoint = NUMvector<long> (1, my nProximities);
		my jPoint = NUMvector<long> (1, my nProximities);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("MDSVec not created.");
	}
}

MDSVec Dissimilarity_to_MDSVec (Dissimilarity me) {
	try {
		autoMDSVec thee = MDSVec_create (my numberOfRows);

		long k = 0;
		for (long i = 1; i <= my numberOfRows - 1; i++) {
			for (long j = i + 1; j <= my numberOfColumns; j++) {
				double f = (my data[i][j] + my data[j][i]) / 2;
				if (f > 0) {
					k++;
					thy proximity[k] = f;
					thy iPoint[k] = i;
					thy jPoint[k] = j;
				}
			}
		}
		thy nProximities = k;
		NUMsort3 (thy proximity, thy iPoint, thy jPoint, 1, k, 1);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MDSVec created.");
	}
}

/*********************** MDSVECS *******************************************/


Thing_implement (MDSVecs, Ordered, 0);

MDSVecs MDSVecs_create () {
	try {
		autoMDSVecs me = Thing_new (MDSVecs);
		Ordered_init (me.peek(), classMDSVec, 10);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("MDSVecs not created.");
	}
}

MDSVecs Dissimilarities_to_MDSVecs (Dissimilarities me) {
	try {
		autoMDSVecs thee = MDSVecs_create ();
		for (long i = 1; i <= my size; i++) {
			autoMDSVec him = Dissimilarity_to_MDSVec ( (Dissimilarity) (my item[i]));
			Thing_setName (him.peek(), Thing_getName ( (Thing) my item[i]));
			Collection_addItem (thee.peek(), him.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MDSVecs created.");
	}
}


/**************************  CONFUSIONS **************************************/

Thing_implement (Confusions, Proximities, 0);

Confusions Confusions_create () {
	try {
		autoConfusions me = Thing_new (Confusions);
		Proximities_init (me.peek(), classConfusion);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Confusions not created.");
	}
}

Confusion Confusions_sum (Confusions me) {
	try {
		autoTableOfReal sum = TablesOfReal_sum (me);
		autoConfusion thee = TableOfReal_to_Confusion (sum.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not created.");
	}
}


/**************************  DISTANCES **************************************/

Thing_implement (Distances, Proximities, 0);

Distances Distances_create () {
	try {
		autoDistances me = Thing_new (Distances);
		Proximities_init (me.peek(), classDistance);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Distances not created.");
	}
}


/*****************  SCALARPRODUCT ***************************************/

Thing_implement (ScalarProduct, TableOfReal, 0);

ScalarProduct ScalarProduct_create (long numberOfPoints) {
	try {
		autoScalarProduct me = Thing_new (ScalarProduct);
		TableOfReal_init (me.peek(), numberOfPoints, numberOfPoints);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("ScalarProduct not created.");
	}
}


/************* SCALARPRODUCTS **************************************/

Thing_implement (ScalarProducts, TablesOfReal, 0);

ScalarProducts ScalarProducts_create () {
	try {
		autoScalarProducts me = Thing_new (ScalarProducts);
		TablesOfReal_init (me.peek(), classScalarProduct);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("ScalarProducts not created.");
	}
}

/******************  DISSIMILARITY **********************************/

Thing_implement (Dissimilarity, Proximity, 0);

Dissimilarity Dissimilarity_create (long numberOfPoints) {
	try {
		autoDissimilarity me = Thing_new (Dissimilarity);
		Proximity_init (me.peek(), numberOfPoints);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Dissimilarity not created.");
	}
}

int Dissimilarity_getAdditiveConstant (I, double *c) { // Why not: double Diss..(I) ???
	*c = 0;
	try {
		iam (Dissimilarity);
		long nProximities = 0, nPoints = my numberOfRows, nPoints2 = 2 * nPoints;
		double proximity;

		/*
			Return c = average dissimilarity in case of failure
		*/
		if (nPoints < 1) {
			Melder_throw ("Matrix part is empty.");
		}

		*c = 0;
		for (long i = 1; i <= nPoints - 1; i++) {
			for (long j = i + 1; j <= nPoints; j++) {
				proximity = (my data[i][j] + my data[j][i]) / 2;
				if (proximity > 0) {
					nProximities++;
					*c += proximity;
				}
			}
		}
		if (nProximities < 1) {
			Melder_throw ("No proximities larger than zero.");
		}
		*c /= nProximities;

		autoNUMmatrix<double> wd (1, nPoints, 1, nPoints);
		autoNUMmatrix<double> wdsqrt (1, nPoints, 1, nPoints);
		autoNUMmatrix<double> b (1, nPoints2, 1, nPoints2);
		autoNUMvector<double> eigenvalue (1, nPoints2);

		/*
			The matrices D & D1/2 with distances (squared and linear)
		*/

		for (long i = 1; i <= nPoints - 1; i++) {
			for (long j = i + 1; j <= nPoints; j++) {
				proximity = (my data[i][j] + my data[j][i]) / 2;
				wdsqrt[i][j] = - proximity / 2;
				wd[i][j] = - proximity * proximity / 2;
			}
		}

		NUMdoubleCentre (wdsqrt.peek(), 1, nPoints, 1, nPoints);
		NUMdoubleCentre (wd.peek(), 1, nPoints, 1, nPoints);

		/*
			Calculate the B matrix according to eq. 6
		*/

		for (long i = 1; i <= nPoints; i++) {
			for (long j = 1; j <= nPoints; j++) {
				b[i][nPoints + j] = 2 * wd[i][j];
				b[nPoints + i][nPoints + j] = -4 * wdsqrt[i][j];
				b[nPoints + i][i] = -1;
			}
		}

		/*
			Get eigenvalues and sort them descending
		*/

		NUMeigensystem (b.peek(), nPoints2, NULL, eigenvalue.peek());
		if (eigenvalue[1] <= 0) {
			Melder_throw ("Negative eigenvalue.");
		}
		*c = eigenvalue[1];
		return 1;
	} catch (MelderError) {
		Melder_throw ("Additive constant not calculated.");
	}
}


/***************  DISSIMILARITIES **************************************/

Thing_implement (Dissimilarities, Proximities, 0);

Dissimilarities Dissimilarities_create () {
	try {
		autoDissimilarities me = Thing_new (Dissimilarities);
		Proximities_init (me.peek(), classDissimilarity);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Dissimilarities not created.");
	}
}


/*************  SIMILARITY *****************************************/

Thing_implement (Similarity, Proximity, 0);

Similarity Similarity_create (long numberOfPoints) {
	try {
		autoSimilarity me = Thing_new (Similarity);
		Proximity_init (me.peek(), numberOfPoints);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Similarity not created.");
	}
}

Similarity Confusion_to_Similarity (Confusion me, int normalize,
                                    int symmetrizeMethod) {
	try {
		if (my numberOfColumns != my numberOfRows) {
			Melder_throw ("Confusion must be a square table.");
		}

		long nxy = my numberOfColumns;
		autoSimilarity thee = Similarity_create (nxy);

		TableOfReal_copyLabels (me, thee.peek(), 1, 1);

		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);

		if (normalize) {
			NUMdmatrix_normalizeRows (thy data, nxy, nxy);
		}
		if (symmetrizeMethod == 1) {
			return thee.transfer();
		}
		if (symmetrizeMethod == 2) { // Average data
			for (long i = 1; i <= nxy - 1; i++) {
				for (long j = i + 1; j <= nxy; j++) {
					thy data[i][j] = thy data[j][i] =
					                     (thy data[i][j] + thy data[j][i]) / 2;
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
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Similarity created.");
	}
}

Dissimilarity Similarity_to_Dissimilarity (Similarity me,
        double maximumDissimilarity) {
	try {
		long nxy = my numberOfColumns;
		double max = 0;
		autoDissimilarity thee = Dissimilarity_create (nxy);
		TableOfReal_copyLabels (me, thee.peek(), 1, 1);
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
			(L"Your maximumDissimilarity is smaller than the maximum similarity. Some data may be lost.");

		for (long i = 1; i <= nxy; i++) {
			for (long j = 1; j <= nxy; j++) {
				double d = maximumDissimilarity - thy data[i][j];
				thy data[i][j] = d > 0 ? d : 0;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Dissimilarity created.");
	}
}

Distance Dissimilarity_to_Distance (Dissimilarity me, int scale) {
	try {
		double additiveConstant = 0;

		autoDistance thee = Distance_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.peek(), 1, 1);
		if (scale == MDS_ORDINAL &&
		        ! Dissimilarity_getAdditiveConstant (me, &additiveConstant)) {
			Melder_warning (L"Dissimilarity_to_Distance: could not determine "
			                "\"additive constant\", the average dissimilarity was used as "
			                "its value.");
		}
		for (long i = 1; i <= my numberOfRows - 1; i++) {
			for (long j = i + 1; j <= my numberOfColumns; j++) {
				double d = 0.5 * (my data[i][j] + my data[j][i]) + additiveConstant;
				thy data[i][j] = thy data[j][i] = d;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Distance created.");
	}
}

Weight Dissimilarity_to_Weight (Dissimilarity me) {
	try {
		autoWeight thee = Weight_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.peek(), 1, 1);
		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = i; j <= my numberOfRows; j++) {
				if (my data[i][j] > 0) {
					thy data[i][j] = 1;
				}
			}
			thy data[i][i] = 0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Weight created.");
	}
}


Dissimilarity Confusion_to_Dissimilarity_pdf (Confusion me, double minimumConfusionLevel) {
	try {
		if (my numberOfColumns != my numberOfRows) {
			Melder_throw ("Confusion must be a square table.");
		}
		Melder_assert (minimumConfusionLevel > 0);
		autoDissimilarity thee = Dissimilarity_create (my numberOfColumns);
		TableOfReal_copyLabels (me, thee.peek(), 1, 1);
		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);

		/*
			Correct "zero" responses.
		*/

		for (long i = 1; i <= my numberOfColumns; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				if (thy data[i][j] == 0) {
					thy data[i][j] = minimumConfusionLevel;
				}
			}
		}

		NUMdmatrix_normalizeRows (thy data, my numberOfColumns, my numberOfColumns);

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

		for (long i = 1; i <= my numberOfColumns; i++) {
			for (long j = i + 1; j <= my numberOfColumns; j++) {
				double x = thy data[i][j] <= thy data[j][i] ? thy data[i][j] : thy data[j][i];
				double y = thy data[i][j] > thy data[j][i] ? thy data[i][j] : thy data[j][i];
				x = NUMinvGaussQ (x);
				y = NUMinvGaussQ (y);
				double d = x + y * exp ( (y * y - x * x) / 2);
				/* Melder_info ("i, j, x, y, d: %d %d %.17g %.17g %.17g", i, j, x, y, d); */
				thy data[i][j] = thy data [j][i] = d;
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Dissimilarity created from pdf.");
	}
}

void Distance_and_Configuration_drawScatterDiagram (Distance me,
        Configuration him, Graphics g, double xmin, double xmax, double ymin,
        double ymax, double size_mm, const wchar_t *mark, int garnish) {
	autoDistance dist = Configuration_to_Distance (him);
	Proximity_Distance_drawScatterDiagram (me, dist.peek(), g, xmin, xmax, ymin,
	                                       ymax, size_mm, mark, garnish);
}

Dissimilarity Distance_to_Dissimilarity (Distance me) {
	try {
		autoDissimilarity thee = Dissimilarity_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.peek(), 1, 1);
		NUMmatrix_copyElements (my data, thy data, 1, my numberOfRows, 1, my numberOfColumns);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Dissimilarity not created from Distance.");
	}
}

Configuration Distance_to_Configuration_torsca (Distance me, int numberOfDimensions) {
	try {
		if (numberOfDimensions > my numberOfRows) {
			Melder_throw ("Number of dimensions too high.");
		}
		autoScalarProduct sp = Distance_to_ScalarProduct (me, 0);
		autoConfiguration thee = Configuration_create (my numberOfRows, numberOfDimensions);
		TableOfReal_copyLabels (me, thee.peek(), 1, 0);
		NUMprincipalComponents (sp -> data, my numberOfRows, numberOfDimensions, thy data);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created (torsca method).");
	}
}

ScalarProduct Distance_to_ScalarProduct (Distance me, int normalize) {
	try {
		autoScalarProduct thee = ScalarProduct_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.peek(), 1, 1);
		for (long i = 1; i <= my numberOfRows - 1; i++) {
			for (long j = i + 1; j <= my numberOfColumns; j++) {
				/*
					force symmetry by averaging!
				*/
				double d = 0.5 * (my data[i][j] + my data[j][i]);
				thy data[i][j] = thy data[j][i] = - 0.5 * d * d;
			}
		}
		TableOfReal_doubleCentre (thee.peek());
		if (my name) {
			Thing_setName (thee.peek(), my name);
		}
		if (normalize) {
			TableOfReal_normalizeTable (thee.peek(), 1);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no ScalarProduct created.");
	}
}

/**********  Configuration & ..... ***********************************/


Distance Configuration_to_Distance (Configuration me) {
	try {
		autoDistance thee = Distance_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.peek(), 1, -1);
		for (long i = 1; i <= thy numberOfRows - 1; i++) {
			for (long j = i + 1; j <= thy numberOfColumns; j++) {
				double dmax = 0, d = 0;

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
				if (dmax > 0) {
					for (long k = 1; k <= my numberOfColumns; k++) {
						double arg = fabs (my data[i][k] - my data[j][k]) / dmax;
						d += my w[k] * pow (arg, my metric);
					}
				}
				thy data[i][j] = thy data[j][i] = dmax * pow (d, 1.0 / my metric);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Distance created.");
	}
}

void Proximity_Distance_drawScatterDiagram (I, Distance thee, Graphics g,
        double xmin, double xmax, double ymin, double ymax, double size_mm,
        const wchar_t *mark, int garnish) {
	iam (Proximity);
	long n = my numberOfRows * (my numberOfRows - 1) / 2;
	double **x = my data, **y = thy data;

	if (n == 0) {
		return;
	}
	if (! TableOfReal_equalLabels (me, thee, 1, 1)) {
		Melder_throw ("Proximity_Distance_drawScatterDiagram: Dimensions and labels must be the same.");
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
			if (x[i][j] >= xmin && x[i][j] <= xmax &&
			        y[i][j] >= ymin && y[i][j] <= ymax) {
				Graphics_mark (g, x[i][j], y[i][j], size_mm, mark);
			}
		}
	}

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, 1, L"Distance");
		Graphics_textBottom (g, 1, L"Dissimilarity");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

Distances MDSVecs_Distance_monotoneRegression (MDSVecs me, Distance thee, int tiesProcessing) {
	try {
		autoDistances him = Distances_create ();
		for (long i = 1; i <= my size; i++) {
			autoMDSVec vec = (MDSVec) my item[i];
			if (vec -> nPoints != thy numberOfRows) {
				Melder_throw ("Dimension of MDSVec and Distance must be equal.");
			}
			autoDistance fit = MDSVec_Distance_monotoneRegression (vec.peek(), thee, tiesProcessing);
			Collection_addItem (him.peek(), fit.transfer());
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("No Distances created from MDSVecs & Distance.");
	}
}

Distance MDSVec_Distance_monotoneRegression (MDSVec me, Distance thee, int tiesProcessing) {
	try {
		long nProximities = my nProximities;
		if (thy numberOfRows != my nPoints) {
			Melder_throw ("Distance and MDSVVec dimension do not agreee.");
		}
		autoNUMvector<double> distance (1, nProximities);
		autoNUMvector<double> fit (1, nProximities);
		autoDistance him = Distance_create (thy numberOfRows);
		TableOfReal_copyLabels (thee, him.peek(), 1, 1);

		long *iPoint = my iPoint, *jPoint = my jPoint;
		for (long i = 1; i <= nProximities; i++) {
			distance[i] = thy data[iPoint[i]][jPoint[i]];
		}

		if (tiesProcessing == MDS_PRIMARY_APPROACH || MDS_SECONDARY_APPROACH) {
			/*
				Kruskal's primary approach to tie-blocks:
					Sort corresponding distances, with iPoint, and jPoint.
				Kruskal's secondary approach:
					Substitute average distance in each tie block
			*/
			long ib = 1;
			for (long i = 2; i <= nProximities; i++) {
				if (my proximity[i] == my proximity[i - 1]) {
					continue;
				}
				if (i - ib > 1) {
					if (tiesProcessing == MDS_PRIMARY_APPROACH) {
						NUMsort3 (distance.peek(), iPoint, jPoint, ib, i - 1, 1); // sort ascending
					} else if (tiesProcessing == MDS_SECONDARY_APPROACH) {
						double mean = 0;
						for (long j = ib; j <= i - 1; j++) {
							mean += distance[j];
						}
						mean /= (i - ib);
						for (long j = ib; j <= i - 1; j++) {
							distance[j] = mean;
						}
					}
				}
				ib = i;
			}
		}

		NUMmonotoneRegression (distance.peek(), nProximities, fit.peek());

		/*
			Fill Distance with monotone regressed distances
		*/

		for (long i = 1; i <= nProximities; i++) {
			long ip = iPoint[i], jp = jPoint[i];
			his data[ip][jp] = his data[jp][ip] = fit[i];
		}

		/*
			Make rest of distances equal to the maximum fit.
		*/

		for (long i = 1; i <= his numberOfRows - 1; i++) {
			for (long j = i + 1; j <= his numberOfColumns; j++) {
				if (! his data[i][j]) {
					his data[i][j] = his data[j][i] = fit[nProximities];
				}
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("Distance not created.");
	}
}


Distance Dissimilarity_Distance_monotoneRegression (Dissimilarity me, Distance thee, int tiesProcessing) {
	try {
		if (thy numberOfRows != my numberOfRows) {
			Melder_throw ("Dimensions do not agree.");
		}
		autoMDSVec vec = Dissimilarity_to_MDSVec (me);
		autoDistance him = MDSVec_Distance_monotoneRegression (vec.peek(), thee, tiesProcessing);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("Distance not created.");
	}
}

/*************** class Proximities **************************************/

Thing_implement (Proximities, TablesOfReal, 0);

void Proximities_init (I, ClassInfo klas) {
	iam (Proximities);
	TablesOfReal_init (me, klas);
}

Proximities Proximities_create () {
	try {
		autoProximities me = Thing_new (Proximities);
		Proximities_init (me.peek(), classProximity);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Proximities not created.");
	}
}

ScalarProducts Distances_to_ScalarProducts (Distances me, int normalize) {
	try {
		autoScalarProducts thee = ScalarProducts_create ();
		for (long i = 1; i <= my size; i++) {
			autoScalarProduct sp = Distance_to_ScalarProduct ( (Distance) (my item[i]), normalize);
			Collection_addItem (thee.peek(), sp.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no ScalarProducts created.");
	}
}

int Distances_to_Configuration_ytl (Distances me, int numberOfDimensions,
                                    int normalizeScalarProducts, Configuration *out1, Salience *out2) {
	try {
		*out1 = NULL; *out2 = NULL;
		autoScalarProducts sp = Distances_to_ScalarProducts (me, normalizeScalarProducts);
		ScalarProducts_to_Configuration_ytl (sp.peek(), numberOfDimensions, out1, out2);
		return 1;
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created (ytl method).");
	}
}

void ScalarProducts_to_Configuration_ytl (ScalarProducts me, int numberOfDimensions, Configuration *out1, Salience *out2) {
	long numberOfSources = my size;
	autoNUMvector<double **> ci (1, numberOfSources);
	try {
		long nPoints = ( (ScalarProduct) my item[1]) -> numberOfRows;
		double **w;

		autoConfiguration thee = Configuration_create (nPoints, numberOfDimensions);
		autoSalience mdsw = Salience_create (numberOfSources, numberOfDimensions);
		TableOfReal_copyLabels (my item[1], thee.peek(), 1, 0);
		autoNUMvector<double> eval (1, numberOfSources);
		autoNUMmatrix<double> cl (1, numberOfDimensions, 1, numberOfDimensions);
		autoNUMmatrix<double> pmean (1, nPoints, 1, nPoints);
		autoNUMmatrix<double> y (1, nPoints, 1, numberOfDimensions);
		autoNUMmatrix<double> yinv (1, numberOfDimensions, 1, nPoints);
		autoNUMmatrix<double> a (1, numberOfSources, 1, numberOfSources);
		autoNUMmatrix<double> evec (1, numberOfSources, 1, numberOfSources);
		autoNUMmatrix<double> K (1, numberOfDimensions, 1, numberOfDimensions);

		*out1 = NULL; *out2 = NULL;

		Thing_setName (mdsw.peek(), L"ytl");
		Thing_setName (thee.peek(), L"ytl");
		TableOfReal_labelsFromCollectionItemNames (mdsw.peek(), me, 1, 0);

		// Determine the average scalar product matrix (Pmean) of
		// dimension [1..nPoints][1..nPoints].

		for (long i = 1; i <= numberOfSources; i++) {
			ScalarProduct sp = (ScalarProduct) my item[i];
			for (long j = 1; j <= nPoints; j++) {
				for (long k = 1; k <= nPoints; k++) {
					pmean[j][k] += sp->data[j][k];
				}
			}
		}

		if (numberOfSources > 1) {
			for (long j = 1; j <= nPoints; j++) {
				for (long k = 1; k <= nPoints; k++) {
					pmean[j][k] /= numberOfSources;
				}
			}
		}

		// Up to a rotation K, the initial configuration can be found by
		// extracting the first 'numberOfDimensions' principal components of Pmean.

		NUMdmatrix_into_principalComponents (pmean.peek(), nPoints, nPoints, numberOfDimensions, y.peek());
		NUMmatrix_copyElements (y.peek(), thy data, 1, nPoints, 1, numberOfDimensions);

		// We cannot determine weights from only one sp-matrix.

		if (numberOfSources == 1) {
			Melder_throw ("Only one source.");
		}

		// Calculate the C[i] matrices [1..numberOfDimensions][1..numberOfDimensions]
		//  from the P[i] by: C[i] = (y'.y)^-1 . y' . P[i] . y . (y'.y)^-1 ==
		//	 yinv P[i] yinv'

		NUMpseudoInverse (y.peek(), nPoints, numberOfDimensions, yinv.peek(), 1e-14);

		for (long i = 1; i <= numberOfSources; i++) {
			ScalarProduct sp = (ScalarProduct) my item[i];
			ci[i] = NUMmatrix<double> (1, numberOfDimensions, 1, numberOfDimensions);
			for (long j = 1; j <= numberOfDimensions; j++) {
				for (long k = 1; k <= numberOfDimensions; k++) {
					for (long l = 1; l <= nPoints; l++) {
						if (yinv[j][l] != 0) {
							for (long m = 1; m <= nPoints; m++) {
								ci[i][j][k] += yinv[j][l] * sp -> data[l][m] * yinv[k][m];
							}
						}
					}
				}
			}
		}

		// Calculate the A[1..numberOfSources][1..numberOfSources] matrix by (eq.12):
		// a[i][j] = trace (C[i]*C[j]) - trace (C[i]) * trace (C[j]) / numberOfDimensions;
		// Get the first eigenvector and form matrix cl from a linear combination of the C[i]'s

		for (long i = 1; i <= numberOfSources; i++) {
			for (long j = i; j <= numberOfSources; j++) {
				a[j][i] = a[i][j] =  NUMtrace2 (ci[i], ci[j], numberOfDimensions)
				                     - NUMtrace (ci[i], numberOfDimensions) * NUMtrace (ci[j], numberOfDimensions) / numberOfDimensions;
			}
		}

		NUMeigensystem (a.peek(), numberOfSources, evec.peek(), eval.peek());

		for (long i = 1; i <= numberOfSources; i++) {
			for (long j = 1; j <= numberOfDimensions; j++) {
				for (long k = 1; k <= numberOfDimensions; k++) {
					cl[j][k] += ci[i][j][k] * evec[i][1]; /* eq. (7) */
				}
			}
		}

		// The rotation K is obtained from the eigenvectors of cl
		// Is the following still correct??? eigensystem was not sorted??

		NUMeigensystem (cl.peek(), numberOfDimensions, K.peek(), NULL);

		// Now get the configuration: X = Y.K

		for (long i = 1; i <= nPoints; i++) {
			for (long j = 1; j <= numberOfDimensions; j++) {
				double x = 0;
				for (long k = 1; k <= numberOfDimensions; k++) {
					x += y[i][k] * K[k][j];
				}
				thy data [i][j] = x;
			}
		}

		Configuration_normalize (thee.peek(), 0, 1);

		// And finally the weights: W[i] = K' C[i] K   (eq. (5)).
		// We are only interested in the diagonal of the resulting matrix W[i].

		w = mdsw -> data;
		for (long i = 1; i <= numberOfSources; i++) {
			for (long j = 1; j <= numberOfDimensions; j++) {
				double wt = 0;
				for (long k = 1; k <= numberOfDimensions; k++) {
					if (K[k][j] != 0) {
						for (long l = 1; l <= numberOfDimensions; l++) {
							wt += K[k][j] * ci[i][k][l] * K[l][j];
						}
					}
				}
				w[i][j] = wt;
			}
		}
		*out1 = thee.transfer(); *out2 = mdsw.transfer();
		for (long i = 1; i <= numberOfSources; i++) {
			NUMmatrix_free<double> (ci[i], 1, 1);
		}
	} catch (MelderError) {
		for (long i = 1; i <= numberOfSources; i++) {
			NUMmatrix_free<double> (ci[i], 1, 1);
		};
		Melder_throw (me, ": no Configuration (ytl) crerated.");
	}
}

Dissimilarities Distances_to_Dissimilarities (Distances me) {
	try {
		autoDissimilarities thee = Dissimilarities_create ();
		for (long i = 1; i <= my size; i++) {
			wchar_t *name = Thing_getName ( (Thing) my item[i]);
			autoDissimilarity him = Distance_to_Dissimilarity ( (Distance) (my item[i]));
			Thing_setName (him.peek(), name ? name : L"untitled");
			Collection_addItem (thee.peek(), him.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Dissimilarities created.");
	}
}

Distances Dissimilarities_to_Distances (Dissimilarities me, int measurementLevel) {
	try {
		autoDistances thee = Distances_create ();

		for (long i = 1; i <= my size; i++) {
			autoDistance him = Dissimilarity_to_Distance ( (Dissimilarity) my item[i], measurementLevel == MDS_ORDINAL);
			wchar_t *name = Thing_getName ( (Thing) my item[i]);
			Thing_setName (him.peek(), name ? name : L"untitled");
			Collection_addItem (thee.peek(), him.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Distances created.");
	}
}

/*****************  Kruskal *****************************************/

static void smacof_guttmanTransform (Configuration cx, Configuration cz,
                                     Distance disp, Weight weight, double **vplus) {
	long nPoints = cx -> numberOfRows, nDimensions = cx -> numberOfColumns;
	double **z = cz -> data;

	autoNUMmatrix<double> b (1, nPoints, 1, nPoints);
	autoDistance distZ = Configuration_to_Distance (cz);

	// compute B(Z) (eq. 8.25)

	for (long i = 1; i <= nPoints; i++) {
		double sum = 0;
		for (long j = 1; j <= nPoints; j++) {
			double dzij = distZ -> data[i][j];
			if (i == j || dzij == 0) {
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
			double xij = 0;
			for (long k = 1;  k <= nPoints; k++) {
				for (long l = 1; l <= nPoints; l++) {
					xij += vplus[i][k] * b[k][l] * z[l][j];
				}
			}
			cx -> data[i][j] = xij;
		}
	}
}

double Distance_Weight_stress (Distance fit, Distance conf, Weight weight, int type) {
	double eta_fit, eta_conf, rho, stress = NUMundefined, denum, tmp;

	Distance_Weight_rawStressComponents (fit, conf, weight, &eta_fit, &eta_conf, &rho);

	// All formula's for stress, except for raw stress, are independent of the
	// scale of the configuration, i.e., the distances conf[i][j].

	if (type == MDS_NORMALIZED_STRESS) {
		denum = eta_fit * eta_conf;
		if (denum > 0) {
			stress = 1.0 - rho * rho / denum;
		}
	} else if (type == MDS_STRESS_1) {
		denum = eta_fit * eta_conf;
		if (denum > 0) {
			tmp = 1.0 - rho * rho / denum;
			if (tmp > 0) {
				stress = sqrt (tmp);
			}
		}
	} else if (type == MDS_STRESS_2) {
		double m = 0, wsum = 0, var = 0, **w = weight -> data;
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
		if (m > 0) {
			// Get variance

			for (long i = 1; i <= nPoints - 1; i++) {
				for (long j = i + 1; j <= nPoints; j++) {
					tmp = c[i][j] - m;
					var += w[i][j] * tmp * tmp;
				}
			}
			denum = var * eta_fit;
			if (denum > 0) {
				stress = sqrt ( (eta_fit * eta_conf - rho * rho) / denum);
			}
		}
	} else if (type == MDS_RAW_STRESS) {
		stress = eta_fit + eta_conf - 2 * rho ;
	}
	return stress;
}

void Distance_Weight_rawStressComponents (Distance fit, Distance conf,
        Weight weight, double *eta_fit, double *eta_conf, double *rho) {
	long nPoints = conf -> numberOfRows;

	*eta_fit = *eta_conf = *rho = 0;

	double wsum = 0;
	for (long i = 1; i <= nPoints - 1; i++) {
		double *wi = weight -> data[i];
		double *fiti = fit -> data[i];
		double *confi = conf -> data[i];

		for (long j = i + 1; j <= nPoints; j++) {
			*eta_fit += wi[j] * fiti[j] * fiti[j];
			*eta_conf += wi[j] * confi[j] * confi[j];
			*rho += wi[j] * fiti[j] * confi[j];
			wsum += wi[j];
		}
	}
}

double Dissimilarity_Configuration_Transformator_Weight_stress (Dissimilarity d,
        Configuration c, Any transformator, Weight w, int type) {
	Transformator t = (Transformator) transformator;
	long nPoints = d -> numberOfRows;
	double stress = NUMundefined;

	if (nPoints < 1 || nPoints != c -> numberOfRows  || nPoints != t -> numberOfPoints ||
	        (w && nPoints != w -> numberOfRows)) {
		Melder_throw ("Incorrect number of points.");
	}
	autoWeight aw = 0;
	if (w == 0) {
		aw.reset (Weight_create (nPoints));
		w = aw.peek();
	}
	autoDistance cdist = Configuration_to_Distance (c);
	autoMDSVec vec = Dissimilarity_to_MDSVec (d);
	autoDistance fit = Transformator_transform (t, vec.peek(), cdist.peek(), w);

	stress = Distance_Weight_stress (fit.peek(), cdist.peek(), w, type);
	return stress;
}

double Dissimilarity_Configuration_Weight_absolute_stress (Dissimilarity d, Configuration c, Weight w, int type) {
	autoTransformator t = Transformator_create (d -> numberOfRows);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.peek(), w, type);
	return stress;
}

double Dissimilarity_Configuration_Weight_ratio_stress (Dissimilarity d, Configuration c, Weight w, int type) {
	autoRatioTransformator t = RatioTransformator_create (d -> numberOfRows);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.peek(), w, type);
	return stress;
}

double Dissimilarity_Configuration_Weight_interval_stress (Dissimilarity d, Configuration c, Weight w, int type) {
	autoISplineTransformator t = ISplineTransformator_create (d -> numberOfRows, 0, 1);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.peek(), w, type);
	return stress;
}

double Dissimilarity_Configuration_Weight_monotone_stress (Dissimilarity d, Configuration c, Weight w, int tiesProcessing, int type) {
	autoMonotoneTransformator t = MonotoneTransformator_create (d -> numberOfRows);
	MonotoneTransformator_setTiesProcessing (t.peek(), tiesProcessing);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.peek(), w, type);
	return stress;
}

double Dissimilarity_Configuration_Weight_ispline_stress (Dissimilarity d,
        Configuration c, Weight w, long numberOfInteriorKnots, long order, int type) {
	autoISplineTransformator t = ISplineTransformator_create (d -> numberOfRows, numberOfInteriorKnots, order);
	double stress = Dissimilarity_Configuration_Transformator_Weight_stress (d, c, t.peek(), w, type);
	return stress;
}

void Distance_Weight_smacofNormalize (Distance me, Weight w) {
	double sumsq = 0;
	for (long i = 1; i <= my numberOfRows - 1; i++) {
		double *wi = w -> data[i];
		double *di = my data[i];
		for (long j = i + 1; j <= my numberOfRows; j++) {
			sumsq += wi[j] * di[j] * di[j];
		}
	}
	double scale = sqrt (my numberOfRows * (my numberOfRows - 1) / (2 * sumsq));
	for (long i = 1; i <= my numberOfRows - 1; i++) {
		for (long j = i + 1; j <= my numberOfRows; j++) {
			my data[j][i] = (my data[i][j] *= scale);
		}
	}
}

double Distance_Weight_congruenceCoefficient (Distance x, Distance y, Weight w) {
	long nPoints = x -> numberOfRows;
	if (y -> numberOfRows != nPoints || w -> numberOfRows != nPoints) {
		return 0;
	}

	double xy = 0, x2 = 0, y2 = 0;
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

Configuration Dissimilarity_Configuration_Weight_Transformator_smacof (Dissimilarity me, Configuration conf,
        Weight weight, Any transformator, double tolerance, long numberOfIterations, int showProgress, double *stress) {
	try {
		Transformator t = (Transformator) transformator;
		long nPoints = conf -> numberOfRows;
		long nDimensions = conf -> numberOfColumns;
		double tol = 1e-6, stressp = 1e38;
		bool no_weight = weight == 0;

		if (my numberOfRows != nPoints || (!no_weight && weight -> numberOfRows != nPoints) ||
		        t -> numberOfPoints != nPoints) {
			Melder_throw ("Dimensions not in concordance.");
		}
		autoWeight aw = 0;
		if (no_weight) {
			aw.reset (Weight_create (nPoints));
			weight = aw.peek();
		}
		autoNUMmatrix<double> v (1, nPoints, 1, nPoints);
		autoNUMmatrix<double> vplus (1, nPoints, 1, nPoints);
		autoConfiguration z = Data_copy (conf);
		autoMDSVec vec = Dissimilarity_to_MDSVec (me);

		double **w = weight -> data;

		if (showProgress) {
			Melder_progress (0.0, L"MDS analysis");
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

			autoDistance fit = Transformator_transform (t, vec.peek(), dist.peek(), weight);

			// Make conf the Guttman transform of z

			smacof_guttmanTransform (conf, z.peek(), fit.peek(), weight, vplus.peek());

			// Compute stress

			autoDistance cdist = Configuration_to_Distance (conf);

			*stress = Distance_Weight_stress (fit.peek(), cdist.peek(), weight, MDS_NORMALIZED_STRESS);

			// Check stop criterium

			if (fabs (*stress - stressp) / stressp < tolerance) {
				break;
			}

			// Make Z = X

			NUMmatrix_copyElements (conf -> data, z -> data, 1, nPoints, 1, nDimensions);

			stressp = *stress;
			if (showProgress) {
				Melder_progress ( (double) iter / (numberOfIterations + 1), L"kruskal: stress ",
				                   Melder_double (*stress));
			}
		}
		if (showProgress) {
			Melder_progress (1.0, 0);
		}
		return z.transfer();
	} catch (MelderError) {
		if (showProgress) {
			Melder_progress (1.0, 0);
		}
		Melder_throw (me, ": no improved Configuration created (smacof method).");
	}
}

Configuration Dissimilarity_Configuration_Weight_Transformator_multiSmacof (Dissimilarity me, Configuration conf,
        Weight w, Any transformator, double tolerance, long numberOfIterations, long numberOfRepetitions, int showProgress) {
	int showMulti = showProgress && numberOfRepetitions > 1;
	try {
		int showSingle = showProgress && numberOfRepetitions == 1;
		double stress, stressmax = 1e38;

		autoConfiguration cstart = Data_copy (conf);
		autoConfiguration  cbest = Data_copy (conf);

		if (showMulti) {
			Melder_progress (0.0, L"MDS many times");
		}

		for (long i = 1; i <= numberOfRepetitions; i++) {
			autoConfiguration cresult = Dissimilarity_Configuration_Weight_Transformator_smacof
			                            (me, cstart.peek(), w, transformator, tolerance, numberOfIterations, showSingle, &stress);
			if (stress < stressmax) {
				stressmax = stress;
				cbest.reset (cresult.transfer());
			}
			Configuration_randomize (cstart.peek());
			TableOfReal_centreColumns (cstart.peek());

			if (showMulti) {
				Melder_progress ( (double) i / (numberOfRepetitions + 1), Melder_integer (i),
				                   L" from ", Melder_integer (numberOfRepetitions));
			}
		}
		if (showMulti) {
			Melder_progress (1.0, NULL);
		}
		return cbest.transfer();
	} catch (MelderError) {
		if (showMulti) {
			Melder_progress (1.0, NULL);
		}
		Melder_throw (me, ": no improved Configuration created (smacodf method).");
	}
}

Configuration Dissimilarity_Configuration_Weight_absolute_mds (Dissimilarity me, Configuration cstart, Weight w,
        double tolerance, long numberOfIterations, long numberOfRepetitions, int showProgress) {
	try {
		autoTransformator t = Transformator_create (my numberOfRows);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof
		                      (me, cstart, w, t.peek(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no improved Configuration created (absolute mds method).");
	}
}

Configuration Dissimilarity_Configuration_Weight_ratio_mds (Dissimilarity me, Configuration cstart, Weight w,
        double tolerance, long numberOfIterations, long numberOfRepetitions, int showProgress) {
	try {
		autoRatioTransformator t = RatioTransformator_create (my numberOfRows);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof
		                      (me, cstart, w, t.peek(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no improved Configuration created (ratio mds method).");
	}
}

Configuration Dissimilarity_Configuration_Weight_interval_mds (Dissimilarity me, Configuration cstart, Weight w,
        double tolerance, long numberOfIterations, long numberOfRepetitions, int showProgress) {
	try {
		autoISplineTransformator t = ISplineTransformator_create (my numberOfRows, 0, 1);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof
		                      (me, cstart, w, t.peek(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no improved Configuration created (interval mds method).");
	}
}

Configuration Dissimilarity_Configuration_Weight_monotone_mds (Dissimilarity me, Configuration cstart, Weight w,
        int tiesProcessing, double tolerance, long numberOfIterations, long numberOfRepetitions, int showProgress) {
	try {
		autoMonotoneTransformator t = MonotoneTransformator_create (my numberOfRows);
		MonotoneTransformator_setTiesProcessing (t.peek(), tiesProcessing);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof
		                      (me, cstart, w, t.peek(), tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no improved Configuration created (monotone mds method).");
	}
}

Configuration Dissimilarity_Configuration_Weight_ispline_mds (Dissimilarity me, Configuration cstart, Weight w,
        long numberOfInteriorKnots, long order, double tolerance, long numberOfIterations, long numberOfRepetitions,
        int showProgress) {
	try {
		autoISplineTransformator t = ISplineTransformator_create (my numberOfRows, numberOfInteriorKnots, order);
		autoConfiguration c = Dissimilarity_Configuration_Weight_Transformator_multiSmacof (me, cstart, w, t.peek(),
		                      tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no improved Configuration created (ispline mds method).");
	}
}

Configuration Dissimilarity_Weight_absolute_mds (Dissimilarity me, Weight w, long numberOfDimensions,
        double tolerance, long numberOfIterations, long numberOfRepetitions, int showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, MDS_ABSOLUTE);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.peek(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_absolute_mds
		                      (me, cstart.peek(), w, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created (absolute mds method).");
	}
}

Configuration Dissimilarity_Weight_interval_mds (Dissimilarity me, Weight w, long numberOfDimensions,
        double tolerance, long numberOfIterations, long numberOfRepetitions, int showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, MDS_RATIO);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.peek(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_interval_mds
		                      (me, cstart.peek(), w, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created (interval mds method).");
	}
}

Configuration Dissimilarity_Weight_monotone_mds (Dissimilarity me, Weight w, long numberOfDimensions,
        int tiesProcessing, double tolerance, long numberOfIterations, long numberOfRepetitions, int showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, MDS_ORDINAL);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.peek(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_monotone_mds (me, cstart.peek(), w, tiesProcessing,
		                      tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created (monotone mds method).");
	}
}

Configuration Dissimilarity_Weight_ratio_mds (Dissimilarity me, Weight w, long numberOfDimensions, double tolerance,
        long numberOfIterations, long numberOfRepetitions, int showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, MDS_RATIO);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.peek(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_ratio_mds (me, cstart.peek(), w, tolerance,
		                      numberOfIterations, numberOfRepetitions, showProgress);
		return c.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created (ratio mds method).");
	}
}

Configuration Dissimilarity_Weight_ispline_mds (Dissimilarity me, Weight w, long numberOfDimensions,
        long numberOfInteriorKnots, long order, double tolerance, long numberOfIterations, long numberOfRepetitions,
        int showProgress) {
	try {
		autoDistance d = Dissimilarity_to_Distance (me, MDS_ORDINAL);
		autoConfiguration cstart = Distance_to_Configuration_torsca (d.peek(), numberOfDimensions);
		autoConfiguration c = Dissimilarity_Configuration_Weight_ispline_mds (me, cstart.peek(), w,
		                      numberOfInteriorKnots, order, tolerance, numberOfIterations, numberOfRepetitions, showProgress);
		return c.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created (ispline mds method).");
	}
}

/***** classical **/

static void MDSVec_Distances_getStressValues (MDSVec me, Distance ddist,
        Distance dfit, int stress_formula, double *stress, double *s, double *t,
        double *dbar) {
	long nProximities = my nProximities;
	long *iPoint = my iPoint, *jPoint = my jPoint;
	double **dist = ddist -> data, **fit = dfit -> data;

	*s = *t = *dbar = 0;

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

	*stress = *t > 0 ? sqrt (*s / *t) : 0;
}

static double func (Data object, const double p[]) {
	Kruskal me = (Kruskal) object;
	MDSVec him = my vec;
	double **x = my configuration -> data, s, t, dbar, stress;
	double metric = my configuration -> metric;
	long numberOfDimensions = my configuration -> numberOfColumns;
	long numberOfPoints = my configuration -> numberOfRows;
	int tiesProcessing = my process == MDS_CONTINUOUS ? 1 : 0;

	// Substitute results of minimizer into configuration and
	// normalize the configuration

	NUMdvector_into_matrix (p, x, 1, numberOfPoints, 1, numberOfDimensions);

	// Normalize

	NUMcentreColumns (x, 1, numberOfPoints, 1, numberOfDimensions, NULL);
	NUMnormalize (x, numberOfPoints, numberOfDimensions, sqrt (numberOfPoints));

	// Calculate interpoint distances from the configuration

	autoDistance dist = Configuration_to_Distance (my configuration);


	// Monotone regression

	autoDistance fit = MDSVec_Distance_monotoneRegression (my vec, dist.peek(), tiesProcessing);


	// Get numerator and denominator of stress


	MDSVec_Distances_getStressValues (my vec, dist.peek(), fit.peek(), my stress_formula, &stress, &s, &t, &dbar);

	// Gradient calculation.

	for (long i = 1; i <= numberOfPoints; i++) {
		for (long j = 1; j <= numberOfDimensions; j++) {
			my dx[i][j] = 0;
		}
	}

	// Prevent overflow when stress is small

	if (stress < 1e-6) {
		return stress;
	}

	for (long i = 1; i <= his nProximities; i++) {
		long ii = my vec -> iPoint[i], jj = my vec -> jPoint[i];
		double g1 = stress * ( (dist->data[ii][jj] - fit->data[ii][jj]) / s - (dist->data[ii][jj] - dbar) / t);
		for (long j = 1; j <= numberOfDimensions; j++) {
			double dj = x[ii][j] - x[jj][j];
			double g2 = g1 * pow (fabs (dj) / dist->data[ii][jj], metric - 1);
			if (dj < 0) {
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
static void dfunc (Data object, const double p[], double dp[]) {
	Kruskal me = (Kruskal) object;
	Configuration thee = my configuration;
	(void) p;

	long k = 1;
	for (long i = 1; i <= thy numberOfRows; i++) {
		for (long j = 1; j <= thy numberOfColumns; j++) {
			dp[k++] = my dx[i][j];
		}
	}
}

Thing_implement (Kruskal, Thing, 0);

void structKruskal :: v_destroy () {
	NUMmatrix_free<double> (dx, 1, 1);
	forget (configuration);
	forget (proximities);
	forget (vec);
	forget (minimizer);
	Kruskal_Parent :: v_destroy ();
}

Kruskal Kruskal_create (long numberOfPoints, long numberOfDimensions) {
	try {
		autoKruskal me = Thing_new (Kruskal);
		my proximities = Proximities_create ();
		my configuration = Configuration_create (numberOfPoints, numberOfDimensions);
		my dx = NUMmatrix<double> (1, numberOfPoints, 1, numberOfDimensions);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Kruskal not created.");
	}
}

Configuration Dissimilarity_kruskal (Dissimilarity me, long numberOfDimensions, long metric, int tiesProcessing,
                                     int stress_formula, double tolerance, long numberOfIterations, long numberOfRepetitions) {
	try {
		int scale = 1;
		(void) metric;
		autoDistance d = Dissimilarity_to_Distance (me, scale);
		autoConfiguration c = Distance_to_Configuration_torsca (d.peek(), numberOfDimensions);
		Configuration_normalize (c.peek(), 1.0, 0);
		autoConfiguration thee = Dissimilarity_Configuration_kruskal (me, c.peek(), tiesProcessing,
		                         stress_formula, tolerance, numberOfIterations, numberOfRepetitions);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created (kruskal method).");
	}
}

void Dissimilarity_Configuration_drawShepardDiagram (Dissimilarity me,
        Configuration him, Graphics g, double xmin, double xmax, double ymin,
        double ymax, double size_mm, const wchar_t *mark, int garnish) {
	autoDistance dist = Configuration_to_Distance (him);
	Proximity_Distance_drawScatterDiagram (me, dist.peek(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

Distance Dissimilarity_Configuration_monotoneRegression (Dissimilarity dissimilarity, Configuration configuration,
        int tiesProcessing) {
	try {
		autoDistance dist = Configuration_to_Distance (configuration);
		autoDistance result = Dissimilarity_Distance_monotoneRegression (dissimilarity, dist.peek(), tiesProcessing);
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("No Distance created (monotone regression).");
	}
}

Distances Dissimilarities_Configuration_monotoneRegression (Dissimilarities me, Configuration configuration, int tiesProcessing) {
	try {
		autoDistances result = Distances_create ();
		autoDistance dist = Configuration_to_Distance (configuration);
		for (long i = 1; i <= my size; i++) {
			autoDistance d = Dissimilarity_Distance_monotoneRegression ( (Dissimilarity) my item[i], dist.peek(), tiesProcessing);
			Collection_addItem (result.peek(), d.transfer());
		}
		return result.transfer();
	} catch (MelderError) {
		Melder_throw ("No Distances created (monotone regression).");
	}
}


void Dissimilarity_Configuration_drawMonotoneRegression (Dissimilarity me, Configuration him, Graphics g,
        int tiesProcessing, double xmin, double xmax, double ymin, double ymax, double size_mm,
        const wchar_t *mark, int garnish) {
	/* obsolete replace by transformator */
	autoDistance fit = Dissimilarity_Configuration_monotoneRegression (me, him, tiesProcessing);
	Proximity_Distance_drawScatterDiagram (me, fit.peek(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawAbsoluteRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
        double xmin, double xmax, double ymin, double ymax, double size_mm, const wchar_t *mark, int garnish) {
	autoTransformator t = Transformator_create (d->numberOfRows);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.peek(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.peek(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawRatioRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
        double xmin, double xmax, double ymin, double ymax, double size_mm, const wchar_t *mark, int garnish) {
	autoRatioTransformator t = RatioTransformator_create (d -> numberOfRows);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.peek(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.peek(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawIntervalRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
        double xmin, double xmax, double ymin, double ymax, double size_mm, const wchar_t *mark, int garnish) {
	Dissimilarity_Configuration_Weight_drawISplineRegression (d, c, w, g,
	        0, 1, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawMonotoneRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
        int tiesProcessing, double xmin, double xmax, double ymin, double ymax, double size_mm, const wchar_t *mark, int garnish) {
	autoMonotoneTransformator t = MonotoneTransformator_create (d->numberOfRows);
	MonotoneTransformator_setTiesProcessing (t.peek(), tiesProcessing);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.peek(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.peek(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

void Dissimilarity_Configuration_Weight_drawISplineRegression (Dissimilarity d, Configuration c, Weight w, Graphics g,
        long numberOfInternalKnots, long order, double xmin, double xmax, double ymin, double ymax, double size_mm,
        const wchar_t *mark, int garnish) {
	autoISplineTransformator t = ISplineTransformator_create (d->numberOfRows, numberOfInternalKnots, order);
	autoDistance fit = Dissimilarity_Configuration_Transformator_Weight_transform (d, c, t.peek(), w);
	Proximity_Distance_drawScatterDiagram (d, fit.peek(), g, xmin, xmax, ymin, ymax, size_mm, mark, garnish);
}

Distance Dissimilarity_Configuration_Transformator_Weight_transform (Dissimilarity d, Configuration c, Any t, Weight w) {
	try {
		autoWeight aw = 0;
		if (w == 0) {
			aw.reset (Weight_create (d -> numberOfRows));
			w = aw.peek();
		}
		autoDistance cdist = Configuration_to_Distance (c);
		autoMDSVec v = Dissimilarity_to_MDSVec (d);
		autoDistance thee = Transformator_transform (t, v.peek(), cdist.peek(), w);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Distance not created.");
	}
}

double Dissimilarity_Configuration_Weight_Transformator_normalizedStress
(Dissimilarity me, Configuration conf, Weight weight, Transformator t) {
	autoDistance cdist = Configuration_to_Distance (conf);
	autoMDSVec vec = Dissimilarity_to_MDSVec (me);
	autoDistance fdist = Transformator_transform (t, vec.peek(), cdist.peek(), weight);
	double stress = Distance_Weight_stress (fdist.peek(), cdist.peek(), weight, MDS_NORMALIZED_STRESS);
	return stress;
}

double Dissimilarity_Configuration_getStress (Dissimilarity me, Configuration him, int tiesProcessing,
        int stress_formula) {
	autoDistance dist = Configuration_to_Distance (him);
	autoMDSVec vec = Dissimilarity_to_MDSVec (me);
	autoDistance fit = MDSVec_Distance_monotoneRegression (vec.peek(), dist.peek(), tiesProcessing);
	double s, t, dbar;
	double stress;
	MDSVec_Distances_getStressValues (vec.peek(), dist.peek(), fit.peek(), stress_formula, &stress, &s, &t, &dbar);
	return stress;
}

Configuration Dissimilarity_Configuration_kruskal (Dissimilarity me, Configuration him, int tiesProcessing,
        int stress_formula, double tolerance, long numberOfIterations, long numberOfRepetitions) {
	try {
		// The Configuration is normalized: each dimension centred +
		//	total variance set

		long numberOfCoordinates = my numberOfRows * his numberOfColumns;
		long numberOfParameters = numberOfCoordinates - his numberOfColumns - 1;
		long numberOfData = my numberOfRows * (my numberOfRows - 1) / 2;

		if (numberOfData < numberOfParameters) {
			Melder_throw ("The number of data must be larger than number of parameters in the model.");
		}

		autoKruskal thee = Kruskal_create (my numberOfRows, his numberOfColumns);
		TableOfReal_copyLabels (me, thy configuration, 1, 0);
		autoDissimilarity dissimilarity = Data_copy (me);
		Collection_addItem (thy proximities, dissimilarity.transfer());
		thy vec = Dissimilarity_to_MDSVec (me);

		thy minimizer = (Minimizer) VDSmagtMinimizer_create (numberOfCoordinates, (Data) thee.peek(), func, dfunc);

		NUMdmatrix_into_vector (his data, thy minimizer -> p, 1, his numberOfRows, 1, his numberOfColumns);

		thy stress_formula = stress_formula;
		thy process = tiesProcessing;
		Configuration_setMetric (thy configuration, his metric);

		Minimizer_minimizeManyTimes (thy minimizer, numberOfRepetitions, numberOfIterations, tolerance);

		/* call the function to get the best configuration */

		(void) func ( (Data) thee.peek(), ( (Minimizer) (thy minimizer)) -> p);

		autoConfiguration result = Data_copy (thy configuration);
		return result.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Configuration created.");
	}
}

/************************** INDSCAL **************************************/


/*
	Ten Berge, Kiers & Krijnen (1993), Computational Solutions for the
	Problem of Negative Saliences and Nonsymmetry in INDSCAL,
	Journal of Classification 10, 115-124.
*/

static void indscal_iteration_tenBerge (ScalarProducts zc, Configuration xc, Salience weights) {
	long nPoints = xc -> numberOfRows, nDimensions = xc -> numberOfColumns;
	long nSources = zc -> size;
	double **x = xc -> data, **w = weights -> data, lambda;

	// tolerance = 1e-4 is nearly optimal for dominant eigenvector estimation.

	double tolerance = 1e-4;
	autoNUMmatrix<double> wsih (1, nPoints, 1, nPoints);
	autoNUMvector<double> solution (1, nPoints);

	for (long h = 1; h <= nDimensions; h++) {
		autoCollection sprc = Data_copy ( (Collection) zc);
		for (long k = 1; k <= nPoints; k++) {
			for (long l = 1; l <= nPoints; l++) {
				wsih[k][l] = 0;
			}
		}

		for (long i = 1; i <= nSources; i++) {
			ScalarProduct spr = (ScalarProduct) sprc -> item[i];
			double **sih = spr -> data;

			// Construct the S[i][h] matrices (eq. 6)

			for (long j = 1; j <= nDimensions; j++) {
				if (j == h) {
					continue;
				}
				for (long k = 1; k <= nPoints; k++) {
					for (long l = 1; l <= nPoints; l++) {
						sih[k][l] -= x[k][j] * x[l][j] * w[i][j];
					}
				}
			}

			// the weighted S matrix (eq. 8)

			for (long k = 1; k <= nPoints; k++) {
				for (long l = 1; l <= nPoints; l++) {
					wsih[k][l] += w[i][h] * sih[k][l];
				}
			}
		}

		// largest eigenvalue of m (nonsymmetric matrix!!) is optimal solution for this dimension

		for (long k = 1; k <= nPoints; k++) {
			solution[k] = x[k][h];
		}

		NUMdominantEigenvector (wsih.peek(), nPoints, solution.peek(), &lambda, tolerance);

		// normalize the solution: centre and x'x = 1

		double mean = 0;
		for (long k = 1; k <= nPoints; k++) {
			mean += solution[k];
		}
		mean /= nPoints;

		if (mean == 0) {
			continue;
		}

		double scale = 0;
		for (long k = 1; k <= nPoints; k++) {
			solution[k] -= mean;
			scale += solution[k] * solution[k];
		}

		for (long k = 1; k <= nPoints; k++) {
			x[k][h] = solution[k] / sqrt (scale);
		}

		// update weights. Make negative weights zero.

		for (long i = 1; i <= nSources; i++) {
			ScalarProduct spr = (ScalarProduct) sprc -> item[i];
			double **sih = spr -> data, wih = 0;
			for (long k = 1; k <= nPoints; k++) {
				for (long l = 1; l <= nPoints; l++) {
					wih += x[k][h] * sih[k][l] * x[l][h];
				}
			}
			if (wih < 0) {
				wih = 0;
			}
			w[i][h] = wih;
		}
	}
}


void ScalarProducts_Configuration_Salience_indscal (ScalarProducts sp, Configuration configuration, Salience weights,
        double tolerance, long numberOfIterations, int showProgress, Configuration *out1, Salience *out2, double *vaf) {
	try {
		double tol = 1e-6, vafp = 0;
		long nZeros = 0, nSources = sp -> size, iter;

		autoConfiguration x = Data_copy (configuration);
		autoSalience w = Data_copy (weights);

		*out1 = 0; *out2 = 0;

		if (showProgress) {
			Melder_progress (0.0, L"INDSCAL analysis");
		}

		// Solve for X, and W matrix via Alternating Least Squares.

		for (iter = 1; iter <= numberOfIterations; iter++) {
			indscal_iteration_tenBerge (sp, x.peek(), w.peek());

			// Goodness of fit and test criterion.

			ScalarProducts_Configuration_Salience_vaf (sp, x.peek(), w.peek(), vaf);

			if (*vaf > 1 - tol || fabs (*vaf - vafp) /  vafp < tolerance) {
				break;
			}
			vafp = *vaf;
			if (showProgress) {
				Melder_progress ( (double) iter / (numberOfIterations + 1),
				                   L"indscal: vaf ", Melder_double (*vaf));
			}
		}

		// Count number of zero weights

		nZeros = NUMdmatrix_countZeros (w->data, w->numberOfRows, w->numberOfColumns);

		// Set labels & names.

		Thing_setName (x.peek(), L"indscal");
		Thing_setName (w.peek(), L"indscal");
		TableOfReal_labelsFromCollectionItemNames (w.peek(), sp, 1, 0);

		*out1 = x.transfer(); *out2 = w.transfer();

		if (showProgress) {
			MelderInfo_open ();
			MelderInfo_writeLine (L"**************** INDSCAL results on Distances "
			                       "*******************\n\n", Thing_className (sp),
			                       L"number of objects: ",  Melder_integer (nSources));
			for (long i = 1; i <= nSources; i++) {
				MelderInfo_writeLine (L"  ", Thing_getName ( (Thing) sp -> item[i]));
			}
			if (nZeros > 0) {
				MelderInfo_writeLine (L"WARNING: ", Melder_integer (nZeros),  L" zero weight",
				                       (nZeros > 1 ? L"s" : L""), L"!");
			}
			MelderInfo_writeLine (L"\n\nVariance Accounted For = ", Melder_double (*vaf),
			                       L"\nThe optimal configuration was reached in ",
			                       Melder_integer ( (iter > numberOfIterations ? numberOfIterations : iter)), L" iterations.");
			MelderInfo_close ();
		}
		if (showProgress) {
			Melder_progress (1.0, NULL);
		}
	} catch (MelderError) {
		if (showProgress) {
			Melder_progress (1.0, NULL);
		}
		Melder_throw ("No indscal configuration calculated.");
	}
}

void Distances_Configuration_Salience_indscal (Distances distances, Configuration configuration, Salience weights,
        int normalizeScalarProducts, double tolerance, long numberOfIterations, int showProgress,
        Configuration *out1, Salience *out2, double *vaf) {
	try {
		*out1 = 0; *out2 = 0;
		autoScalarProducts sp = Distances_to_ScalarProducts (distances, normalizeScalarProducts);
		ScalarProducts_Configuration_Salience_indscal (sp.peek(), configuration, weights, tolerance, numberOfIterations,
		        showProgress, out1, out2, vaf);
	} catch (MelderError) {
		Melder_throw ("No indscal configuration calculated.");
	}
}


void Dissimilarities_Configuration_Salience_indscal (Dissimilarities dissims, Configuration configuration,
        Salience weights, int tiesProcessing, int normalizeScalarProducts, double tolerance, long numberOfIterations,
        int showProgress, Configuration *out1, Salience *out2, double *vaf) {
	try {
		double tol = 1e-6, vafp = 0;
		long iter, nSources = dissims -> size;;
		autoConfiguration x = Data_copy (configuration);
		autoSalience w = Data_copy (weights);
		autoMDSVecs vecs = Dissimilarities_to_MDSVecs (dissims);

		*out1 = 0; *out2 = 0;

		if (showProgress) {
			Melder_progress (0.0, L"INDSCAL analysis");
		}

		for (iter = 1; iter <= numberOfIterations; iter++) {
			autoDistances distances = MDSVecs_Configuration_Salience_monotoneRegression (vecs.peek(), x.peek(), w.peek(), tiesProcessing);
			autoScalarProducts sp = Distances_to_ScalarProducts (distances.peek(), normalizeScalarProducts);

			indscal_iteration_tenBerge (sp.peek(), x.peek(), w.peek());

			// Goodness of fit and test criterion.

			Distances_Configuration_Salience_vaf (distances.peek(), x.peek(), w.peek(), normalizeScalarProducts, vaf);

			if (*vaf > 1 - tol || fabs (*vaf - vafp) /  vafp < tolerance) {
				break;
			}
			vafp = *vaf;
			if (showProgress) {
				Melder_progress ( (double) iter / (numberOfIterations + 1), L"indscal: vaf ", Melder_double (*vaf));
			}
		}

		// Count number of zero weights

		long nZeros = NUMdmatrix_countZeros (w -> data, w -> numberOfRows, w -> numberOfColumns);

		// Set labels & names.

		Thing_setName (x.peek(), L"indscal_mr"); Thing_setName (w.peek(), L"indscal_mr");
		TableOfReal_labelsFromCollectionItemNames (w.peek(), dissims, 1, 0);

		*out1 = x.transfer(); *out2 = w.transfer();

		if (showProgress) {
			MelderInfo_open ();
			MelderInfo_writeLine (L"**************** INDSCAL with monotone regression *******************");
			MelderInfo_writeLine (Thing_className (dissims));
			MelderInfo_writeLine (L"Number of objects: ", Melder_integer (nSources));
			for (long i = 1; i <= nSources; i++) {
				MelderInfo_writeLine (L"  ", Thing_getName ( (Thing) dissims -> item[i]));
			}
			if (nZeros > 0) {
				MelderInfo_writeLine (L"WARNING: ", Melder_integer (nZeros), L" zero weight", (nZeros > 1 ? L"s" : L""));
			}
			MelderInfo_writeLine (L"Variance Accounted For: ", Melder_double (*vaf));
			MelderInfo_writeLine (L"Based on MONOTONE REGRESSION");
			MelderInfo_writeLine (L"number of iterations: ", Melder_integer ( (iter > numberOfIterations ?	numberOfIterations : iter)));
			MelderInfo_close ();
		}
		if (showProgress) {
			Melder_progress (1.0, NULL);
		}
	} catch (MelderError) {
		if (showProgress) {
			Melder_progress (1.0, NULL);
		}
		Melder_throw ("no inscal configuration calculated.");
	}
}

void Distances_Configuration_indscal (Distances dists, Configuration conf, int normalizeScalarProducts, double tolerance,
                                      long numberOfIterations, int showProgress, Configuration *out1, Salience *out2) {
	try {
		autoSalience w = Salience_create (dists -> size, conf -> numberOfColumns);
		double vaf;
		Distances_Configuration_Salience_indscal (dists, conf, w.peek(), normalizeScalarProducts, tolerance,
		        numberOfIterations, showProgress, out1, out2, &vaf);
	} catch (MelderError) {
		Melder_throw ("No indscal performed.");
	}
}

Distances MDSVecs_Configuration_Salience_monotoneRegression (MDSVecs vecs,
        Configuration conf, Salience weights, int tiesProcessing) {
	try {
		long nDimensions = conf -> numberOfColumns;
		autoNUMvector<double> w (NUMvector_copy (conf -> w, 1, nDimensions), 1);
		autoDistances distances = Distances_create ();
		for (long i = 1; i <= vecs -> size; i++) {
			NUMvector_copyElements (weights -> data[i], conf -> w, 1, nDimensions);
			autoDistance dc = Configuration_to_Distance (conf);
			autoDistance dist = MDSVec_Distance_monotoneRegression ( (MDSVec) vecs -> item[i], dc.peek(), tiesProcessing);
			Collection_addItem (distances.peek(), dist.transfer());
		}
		Configuration_setDefaultWeights (conf);
		return distances.transfer();
	} catch (MelderError) {
		Melder_throw ("No Distances created.");
	}
}


Salience Distances_Configuration_to_Salience (Distances d, Configuration c, int normalize) {
	try {
		autoScalarProducts sp = Distances_to_ScalarProducts (d, normalize);
		autoSalience w = ScalarProducts_Configuration_to_Salience (sp.peek(), c);
		return w.transfer();
	} catch (MelderError) {
		Melder_throw ("No Salience created.");
	}
}

Salience ScalarProducts_Configuration_to_Salience (ScalarProducts me, Configuration him) {
	try {
		autoSalience salience = Salience_create (my size, his numberOfColumns);
		autoConfiguration cx = Data_copy (him);
		indscal_iteration_tenBerge (me, cx.peek(), salience.peek());
		return salience.transfer();
	} catch (MelderError) {
		Melder_throw ("No Salience created.");
	}
}

Salience Dissimilarities_Configuration_to_Salience (Dissimilarities me, Configuration him, int tiesProcessing,
        int normalizeScalarProducts) {
	try {
		autoDistances distances = Dissimilarities_Configuration_monotoneRegression (me, him, tiesProcessing);
		autoSalience w = Distances_Configuration_to_Salience (distances.peek(), him, normalizeScalarProducts);
		return w.transfer();
	} catch (MelderError) {
		Melder_throw ("No Salience created.");
	}
}

void Dissimilarities_Configuration_indscal (Dissimilarities dissims, Configuration conf, int tiesProcessing,
        int normalizeScalarProducts, double tolerance, long numberOfIterations, int showProgress,
        Configuration *out1, Salience *out2) {
	try {
		*out1 = 0; *out2 = 0;
		autoDistances distances = Dissimilarities_Configuration_monotoneRegression (dissims, conf, tiesProcessing);
		auto Salience weights = Distances_Configuration_to_Salience (distances.peek(), conf, normalizeScalarProducts);
		double vaf;
		Dissimilarities_Configuration_Salience_indscal (dissims, conf, weights, tiesProcessing, normalizeScalarProducts,
		        tolerance, numberOfIterations, showProgress, out1, out2, &vaf);
	} catch (MelderError) {
		Melder_throw ("No indscal performed.");
	}
}


void Dissimilarities_indscal (Dissimilarities me, long numberOfDimensions, int tiesProcessing, int normalizeScalarProducts,
                              double tolerance, long numberOfIterations, long numberOfRepetitions, int showProgress, Configuration *out1, Salience *out2) {
	int showMulti = showProgress && numberOfRepetitions > 1;
	try {
		int showSingle = showProgress && numberOfRepetitions == 1;
		double vaf, vafmin = 0;

		*out1 = 0; *out2 = 0;

		autoDistances distances = Dissimilarities_to_Distances (me, MDS_ORDINAL);
		Configuration cstart1 = 0; Salience wstart1 = 0;
		Distances_to_Configuration_ytl (distances.peek(), numberOfDimensions, normalizeScalarProducts, &cstart1, &wstart1);
		autoConfiguration cstart = cstart1;
		autoSalience wstart = wstart1;
		autoConfiguration cbest = Data_copy ( (Configuration) cstart.peek());
		autoSalience wbest = Data_copy ( (Salience) wstart.peek());

		if (showMulti) {
			Melder_progress (0.0, L"Indscal many times");
		}

		for (long iter = 1; iter <= numberOfRepetitions; iter++) {
			Configuration cresult1 = 0; Salience wresult1 = 0;
			Dissimilarities_Configuration_Salience_indscal (me, cstart.peek(), wstart.peek(), tiesProcessing,
			        normalizeScalarProducts, tolerance, numberOfIterations, showSingle, &cresult1, &wresult1, &vaf);
			autoConfiguration cresult = cresult1;
			autoSalience wresult = wresult1;
			if (vaf > vafmin) {
				vafmin = vaf;
				cbest.reset (cresult.transfer());
				wbest.reset (wresult.transfer());
			}
			Configuration_randomize (cstart.peek());
			Configuration_normalize (cstart.peek(), 1.0, 1);
			Salience_setDefaults (wstart.peek());

			if (showMulti) {
				Melder_progress ( (double) iter / (numberOfRepetitions + 1),
				                   Melder_integer (iter), L" from ", Melder_integer (numberOfRepetitions));
			}
		}

		*out1 = cbest.transfer(); *out2 = wbest.transfer();
		if (showMulti) {
			Melder_progress (1.0, 0);
		}
	} catch (MelderError) {
		if (showMulti) {
			Melder_progress (1.0, 0);
		}
		Melder_throw (me, ": no indscal performed.");
	}
}


void Distances_indscal (Distances distances, long numberOfDimensions, int normalizeScalarProducts, double tolerance,
                        long numberOfIterations, long numberOfRepetitions, int showProgress, Configuration *out1, Salience *out2) {
	int showMulti = showProgress && numberOfRepetitions > 1;
	try {
		int showSingle = showProgress && numberOfRepetitions == 1;
		double vaf, vafmin = 0;

		*out1 = 0; *out2 = 0;
		Configuration cstart1 = 0; Salience wstart1 = 0;
		Distances_to_Configuration_ytl (distances, numberOfDimensions, normalizeScalarProducts, &cstart1, &wstart1);
		autoConfiguration cstart = cstart1;
		autoSalience wstart = wstart1;
		autoConfiguration cbest = Data_copy (cstart.peek());
		autoSalience wbest = Data_copy (wstart.peek());

		if (showMulti) {
			Melder_progress (0.0, L"Indscal many times");
		}

		for (long i = 1; i <= numberOfRepetitions; i++) {
			Configuration cresult1 = 0; Salience wresult1 = 0;
			Distances_Configuration_Salience_indscal (distances, cstart.peek(), wstart.peek(), normalizeScalarProducts,
			        tolerance, numberOfIterations, showSingle, &cresult1, &wresult1, &vaf);
			autoConfiguration cresult = cresult1;
			autoSalience wresult = wresult1;
			if (vaf > vafmin) {
				vafmin = vaf;
				cbest.reset (cresult.transfer());
				wbest.reset (wresult.transfer());
			}
			Configuration_randomize (cstart.peek());
			Configuration_normalize (cstart.peek(), 1.0, 1);
			Salience_setDefaults (wstart.peek());

			if (showMulti) {
				Melder_progress ( (double) i / (numberOfRepetitions + 1),
				                   Melder_integer (i), L" from ", Melder_integer (numberOfRepetitions));
			}
		}

		*out1 = cbest.transfer(); *out2 = wbest.transfer();
		if (showMulti) {
			Melder_progress (1.0, NULL);
		}
	} catch (MelderError) {
		if (showMulti) {
			Melder_progress (1.0, NULL);
		}
		Melder_throw (distances, ": no indscal performed.");
	}
}

void Dissimilarities_Configuration_Salience_vaf (Dissimilarities me, Configuration thee, Salience him, int tiesProcessing,
        int normalizeScalarProducts, double *vaf) {
	autoDistances distances = Dissimilarities_Configuration_monotoneRegression (me, thee, tiesProcessing);
	Distances_Configuration_Salience_vaf (distances.peek(), thee, him, normalizeScalarProducts, vaf);
}

void Distances_Configuration_vaf (Distances me, Configuration thee, int normalizeScalarProducts, double *vaf) {
	autoSalience w = Distances_Configuration_to_Salience (me, thee, normalizeScalarProducts);
	Distances_Configuration_Salience_vaf (me, thee, w.peek(), normalizeScalarProducts, vaf);
}

void Dissimilarities_Configuration_vaf (Dissimilarities me, Configuration thee, int tiesProcessing, int normalizeScalarProducts, double *vaf) {
	autoSalience w = Dissimilarities_Configuration_to_Salience (me, thee, tiesProcessing, normalizeScalarProducts);
	Dissimilarities_Configuration_Salience_vaf (me, thee, w.peek(), tiesProcessing, normalizeScalarProducts, vaf);
}

void Distances_Configuration_Salience_vaf (Distances me, Configuration thee, Salience him, int normalizeScalarProducts, double *vaf) {
	if (my size != his numberOfRows || thy numberOfColumns != his numberOfColumns) Melder_throw
		("Dimensions must conform.");

	autoScalarProducts sp = Distances_to_ScalarProducts (me, normalizeScalarProducts);
	ScalarProducts_Configuration_Salience_vaf (sp.peek(), thee, him, vaf);
}

void ScalarProduct_Configuration_getVariances (ScalarProduct me,
        Configuration thee, double *varianceExplained, double *varianceTotal) {
	*varianceExplained = *varianceTotal = 0;
	autoDistance distance = Configuration_to_Distance (thee);
	autoScalarProduct fit = Distance_to_ScalarProduct (distance.peek(), 0);
	/*
		ScalarProduct is double centred, i.e., mean == 0.
	*/
	for (long j = 1; j <= my numberOfRows; j++) {
		for (long k = 1; k <= my numberOfColumns; k++) {
			double d2 = my data[j][k] - fit -> data[j][k];
			*varianceExplained += d2 * d2;
			*varianceTotal += my data[j][k] * my data[j][k];
		}
	}
}

void ScalarProducts_Configuration_Salience_vaf (ScalarProducts me, Configuration thee, Salience him, double *vaf) {
	autoNUMvector<double> w (NUMvector_copy (thy w, 1, thy numberOfColumns), 1); // save weights
	try {
		if (my size != his numberOfRows || thy numberOfColumns != his numberOfColumns) Melder_throw
			("Dimensions of input objects must conform.");

		double t = 0, n = 0;
		for (long i = 1; i <= my size; i++) {

			ScalarProduct sp = (ScalarProduct) my item[i];
			if (sp -> numberOfRows != thy numberOfRows) {
				Melder_throw ("ScalarProduct ", i, " does not match Configuration.");
			}

			// weigh configuration before calculating variances

			for (long j = 1; j <= thy numberOfColumns; j++) {
				thy w[j] = sqrt (his data[i][j]);
			}

			double vare, vart;
			ScalarProduct_Configuration_getVariances (sp, thee, &vare, &vart);

			t += vare; n += vart;
		}

		*vaf = n > 0 ? 1.0 - t / n : 0;
		NUMvector_copyElements (w.peek(), thy w, 1, thy numberOfColumns); // restore weights
	} catch (MelderError) {
		NUMvector_copyElements (w.peek(), thy w, 1, thy numberOfColumns);
		Melder_throw ("No vaf calculasted.");
	}
}


/********************** Examples *********************************************/

Dissimilarity Dissimilarity_createLetterRExample (double noiseStd) {
	try {
		autoConfiguration r = Configuration_createLetterRExample (1);
		autoDistance d = Configuration_to_Distance (r.peek());
		autoDissimilarity me = Distance_to_Dissimilarity (d.peek());
		Thing_setName (me.peek(), L"R");

		for (long i = 1; i <= my numberOfRows - 1; i++) {
			for (long j = i + 1; j <= my numberOfRows; j++) {
				double dis = my data[i][j];
				my data[j][i] = my data[i][j] = dis * dis + 5 + NUMrandomUniform (0, noiseStd);
			}
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Dissimilarity for letter R example not created.");
	}
}

Salience Salience_createCarrollWishExample () {
	try {
		long nSources = 8;
		double wx[9] = {0,   1, 0.866, 0.707, 0.5,   0.1, 0.5, 0.354, 0.1};
		double wy[9] = {0, 0.1, 0.5,   0.707, 0.866,   1, 0.1, 0.354, 0.5};
		wchar_t const *name[] = { L"", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8"};
		autoSalience me = Salience_create (nSources, 2);

		for (long i = 1; i <= nSources; i++) {
			my data[i][1] = wx[i];
			my data[i][2] = wy[i];
			TableOfReal_setRowLabel (me.peek(), i, name[i]);
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Salience for Carroll Wish example not created.");
	}
}

Collection INDSCAL_createCarrollWishExample (double noiseRange) {
	try {
		autoConfiguration c = Configuration_createCarrollWishExample ();
		long nObjects = c -> numberOfRows, nSources = 8;
		autoSalience s = Salience_createCarrollWishExample ();
		autoCollection me = Collection_create (classData, nSources);
		for (long l = 1; l <= nSources; l++) {
			c -> w[1] = s -> data[l][1];
			c -> w[2] = s -> data[l][2];
			autoDistance d = Configuration_to_Distance (c.peek());
			autoDissimilarity dissim = Distance_to_Dissimilarity (d.peek());
			for (long i = 1; i <= nObjects - 1; i++) {
				for (long j = i + 1; j <= nObjects; j++) {
					dissim -> data[i][j] = (dissim -> data[j][i] +=
					                            NUMrandomUniform (0, noiseRange));
				}
			}
			Thing_setName (dissim.peek(), s -> rowLabels[l]);
			Collection_addItem (me.peek(), dissim.transfer());
		}
		Thing_setName (me.peek(), L"CarrollWish");
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Collection not created.");
	}
}

void drawSplines (Graphics g, double low, double high, double ymin, double ymax,
                  int type, long order, wchar_t const *interiorKnots, int garnish) {
	long k = order, numberOfKnots, numberOfInteriorKnots = 0;
	long nSplines, n = 1000;
	double knot[101], y[1001];
	wchar_t *start, *end;

	if (type == MDS_ISPLINE) {
		k++;
	}
	for (long i = 1; i <= k; i++) {
		knot[i] = low;
	}
	numberOfKnots = k;

	start = (wchar_t *) interiorKnots;
	while (*start) {
		double value = wcstod (start, &end);
		start = end;
		if (value < low || value > high) {
			Melder_warning (L"drawSplines: knots must be in interval (", Melder_double (low), L", ", Melder_double (high), L")");
			return;
		}
		if (numberOfKnots == 100) {
			Melder_warning (L"drawSplines: too many knots (101)");
			return;
		}
		knot[++numberOfKnots] = value;
	}

	numberOfInteriorKnots = numberOfKnots - k;
	for (long i = 1; i <= k; i++) {
		knot[++numberOfKnots] = high;
	}

	nSplines = order + numberOfInteriorKnots;

	if (nSplines == 0) {
		return;
	}

	Graphics_setWindow (g, low, high, ymin, ymax);
	Graphics_setInner (g);
	for (long i = 1; i <= nSplines; i++) {
		double x, yx, dx = (high - low) / (n - 1);
		for (long j = 1; j <= n; j++) {
			x = low + dx * (j - 1);
			if (type == MDS_MSPLINE) {
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
		static MelderString ts = { 0 };
		long lastKnot = type == MDS_ISPLINE ? numberOfKnots - 2 : numberOfKnots;
		MelderString_empty (&ts);
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, 0, type == MDS_MSPLINE ? L"\\s{M}\\--spline" : L"\\s{I}\\--spline");
		Graphics_marksTop (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		if (low <= knot[order]) {
			if (order == 1) {
				MelderString_append (&ts, L"t__1_");
			} else if (order == 2) {
				MelderString_append (&ts,  L"{t__1_, t__2_}");
			} else {
				MelderString_append (&ts, L"{t__1_..t__", Melder_integer (order), L"_}");
			}
			Graphics_markBottom (g, low, 0, 0, 0, ts.string);
		}
		for (long i = 1; i <= numberOfInteriorKnots; i++) {
			if (low <= knot[k + i] && knot[k + i] < high) {
				MelderString_empty (&ts);
				MelderString_append (&ts, L"t__", Melder_integer (order + i), L"_");
				Graphics_markBottom (g, knot[k + i], 0, 1, 1, ts.string);
				Graphics_markTop (g, knot[k + i], 1, 0, 0, 0);
			}
		}
		if (knot[lastKnot - order + 1] <= high) {
			MelderString_empty (&ts);
			if (order == 1) {
				MelderString_append (&ts, L"t__", Melder_integer (lastKnot), L"_");
			} else {
				MelderString_append (&ts, L"{t__", Melder_integer (order == 2 ? lastKnot - 1 : lastKnot - order + 1), L"_, t__", Melder_integer (lastKnot), L"_}");
			}
			Graphics_markBottom (g, high, 0, 0, 0, ts.string);
		}
	}
}

void drawMDSClassRelations (Graphics g) {
	long nBoxes = 6;
	double boxWidth = 0.3, boxWidth2 = boxWidth / 2, boxWidth3 = boxWidth / 3;
	double boxHeight = 0.1, boxHeight2 = boxHeight / 2;
	double boxHeight3 = boxHeight / 3;
	double r_mm = 3, dxt = 0.025, dyt = 0.03;
	double dboxx = 1 - 0.2 - 2 * boxWidth, dboxy = (1 - 4 * boxHeight) / 3;
	double x1, x2, xm, x23, x13, y1, y2, ym, y23, y13;
	double x[7] = {0.0, 0.2, 0.2, 0.7, 0.2, 0.7, 0.2}; /* left */
	double y[7] = {0.0, 0.9, 0.6, 0.6, 0.3, 0.3, 0.0}; /* bottom */
	wchar_t const *text[7] = {L"", L"Confusion", L"Dissimilarity  %\\de__%%ij%_",
	                          L"Similarity", L"Distance  %d__%%ij%_, %d\\'p__%%ij%_",
	                          L"ScalarProduct", L"Configuration"
	                         };

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
	Graphics_text (g, xm + dxt, y2 + dboxy / 2, L"pdf");

	// Confusion to Similarity

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

	// Dissimilarity to Configuration

	ym = y[2] + boxHeight2;
	y2 = y[6] + boxHeight2;
	Graphics_line (g, x[2], ym, 0, ym);
	Graphics_line (g, 0, ym, 0, y2);
	Graphics_arrow (g, 0, y2, x[6], y2);

	// Restore settings

	Graphics_setLineType (g, Graphics_DRAWN);
	Graphics_setLineWidth (g, 1);
	Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_BOTTOM);

}

/* End of file MDS.cpp */
