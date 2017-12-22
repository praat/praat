/* CCA.c
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
 djmw 2001
 djmw 20020402 GPL header
 djme 20021008 removed SVD_sort
 djmw 20031023 Removed one argument from CCA_and_TableOfReal_scores
 djmw 20031106 Removed bug from CCA_and_TableOfReal_scores
 djmw 20031221 Removed bug: CCA_TableOfReal_scores (wrong dimensions to Eigen_project_into).
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20081119 Check in TableOfReal_to_CCA if TableOfReal_areAllCellsDefined
*/

#include "CCA_and_Correlation.h"
#include "NUM2.h"
#include "NUMlapack.h"
#include "SVD.h"
#include "Strings_extensions.h"
#include "TableOfReal_extensions.h"
#include "Eigen_and_TableOfReal.h"

#include "oo_DESTROY.h"
#include "CCA_def.h"
#include "oo_COPY.h"
#include "CCA_def.h"
#include "oo_EQUAL.h"
#include "CCA_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "CCA_def.h"
#include "oo_WRITE_TEXT.h"
#include "CCA_def.h"
#include "oo_WRITE_BINARY.h"
#include "CCA_def.h"
#include "oo_READ_TEXT.h"
#include "CCA_def.h"
#include "oo_READ_BINARY.h"
#include "CCA_def.h"
#include "oo_DESCRIPTION.h"
#include "CCA_def.h"

void structCCA :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of coefficients: ", numberOfCoefficients);
	MelderInfo_writeLine (U"ny: ", y -> dimension);
	MelderInfo_writeLine (U"nx: ", x -> dimension);
}

Thing_implement (CCA, Daata, 0);

autoCCA CCA_create (integer numberOfCoefficients, integer ny, integer nx) {
	try {
		autoCCA me = Thing_new (CCA);
		my numberOfCoefficients = numberOfCoefficients;
		my yLabels = Thing_new (Strings);
		my xLabels = Thing_new (Strings);
		my y = Eigen_create (numberOfCoefficients, ny);
		my x = Eigen_create (numberOfCoefficients, nx);
		return me;
	} catch (MelderError) {
		Melder_throw (U"CCA not created.");
	}
}

void CCA_drawEigenvector (CCA me, Graphics g, int x_or_y, integer ivec, integer first, integer last, double ymin, double ymax, int weigh, double size_mm, const char32 *mark,	int connect, int garnish) {
	Eigen e = my x.get();
	Strings labels = my xLabels.get();
	if (x_or_y == 1) {
		e = my y.get();
		labels = my yLabels.get();
	}
	Eigen_drawEigenvector (e, g, ivec, first, last, ymin, ymax, weigh, size_mm, mark, connect, labels -> strings, garnish);
}

double CCA_getEigenvectorElement (CCA me, int x_or_y, integer ivec, integer element) {
	Eigen e = ( x_or_y == 1 ? my y.get() : my x.get() );
	return Eigen_getEigenvectorElement (e, ivec, element);
}

autoCCA TableOfReal_to_CCA (TableOfReal me, integer ny) {
	try {
		integer n = my numberOfRows, nx = my numberOfColumns - ny;
		Melder_require (ny > 0 && ny < my numberOfColumns, U"Dimension of first part not correct.");
		Melder_require (ny <= nx, U"The dimension of the dependent part (", ny, U") should not exceed "
				"the dimension of the independent part (", nx, U").");
		Melder_require (n >= ny, U"The number of observations should be larger then ", ny, U".");
		Melder_require (! NUMdmatrix_containsUndefinedElements (my data, 1, my numberOfRows, 1, my numberOfColumns),
			U"At least one of the table's elements is undefined."); 	
		
		// Use svd as (temporary) storage, and copy data

		autoSVD svdy = SVD_create (n, ny);
		autoSVD svdx = SVD_create (n, nx);

		for (integer i = 1; i <= n; i ++) {
			for (integer j = 1; j <= ny; j ++) {
				svdy -> u [i] [j] = my data [i] [j];
			}
			for (integer j = 1; j <= nx; j ++) {
				svdx -> u [i] [j] = my data [i] [ny + j];
			}
		}

		double **uy = svdy -> u;
		double **vy = svdy -> v;
		double **ux = svdx -> u;
		double **vx = svdx -> v;
		double fnormy = NUMfrobeniusnorm (n, ny, uy);
		double fnormx = NUMfrobeniusnorm (n, nx, ux);
		
		Melder_require (fnormy > 0.0 && fnormx > 0.0, U"One of the parts of the table contains only zeros.");
		
		// Centre the data and svd it.

		NUMcentreColumns (uy, 1, n, 1, ny, nullptr);
		NUMcentreColumns (ux, 1, n, 1, nx, nullptr);

		SVD_compute (svdy.get());
		SVD_compute (svdx.get());

		integer numberOfZeroedy = SVD_zeroSmallSingularValues (svdy.get(), 0.0);
		integer numberOfZeroedx = SVD_zeroSmallSingularValues (svdx.get(), 0.0);

		// Form the matrix C = ux' uy (use svd-object storage)

		autoSVD svdc = SVD_create (nx, ny);
		double **uc = svdc -> u;
		double **vc = svdc -> v;

		for (integer i = 1; i <= nx; i ++) {
			for (integer j = 1; j <= ny; j ++) {
				double t = 0.0;
				for (integer q = 1; q <= n; q ++) {
					t += ux [q] [i] * uy [q] [j];
				}
				uc [i] [j] = t;
			}
		}

		SVD_compute (svdc.get());
		integer numberOfZeroedc = SVD_zeroSmallSingularValues (svdc.get(), 0.0);
		integer numberOfCoefficients = ny - numberOfZeroedc;

		autoCCA thee = CCA_create (numberOfCoefficients, ny, nx);
		thy yLabels = strings_to_Strings (my columnLabels, 1, ny);
		thy xLabels = strings_to_Strings (my columnLabels, ny + 1, my numberOfColumns);

		double **evecy = thy y -> eigenvectors;
		double **evecx = thy x -> eigenvectors;
		thy numberOfObservations = n;

		/*
			Y = Vy * inv(Dy) * Vc
			X = Vx * inv(Dx) * Uc
			For the eigenvectors we want a row representation:
			colums(Y) = rows(Y') = rows(Vc' * inv(Dy) * Vy')
			colums(X) = rows(X') = rows(Uc' * inv(Dx) * Vx')
			rows(Y') = evecy[i][j] = Vc[k][i] * Vy[j][k] / Dy[k]
			rows(X') = evecx[i][j] = Uc[k][i] * Vx[j][k] / Dx[k]
		*/

		for (integer i = 1; i <= numberOfCoefficients; i ++) {
			double ccc = svdc -> d [i];
			thy y -> eigenvalues [i] = thy x -> eigenvalues [i] = ccc * ccc;
			for (integer j = 1; j <= ny; j ++) {
				double t = 0.0;
				for (integer q = 1; q <= ny - numberOfZeroedy; q ++) {
					t += vc [q] [i] * vy [j] [q] / svdy -> d [q];
				}
				evecy [i] [j] = t;
			}
			for (integer j = 1; j <= nx; j ++) {
				double t = 0.0;
				for (integer q = 1; q <= nx - numberOfZeroedx; q ++) {
					t += uc [q] [i] * vx [j] [q] / svdx -> d [q];
				}
				evecx [i] [j] = t;
			}
		}

		// Normalize eigenvectors.

		NUMnormalizeRows (thy y -> eigenvectors, numberOfCoefficients, ny, 1);
		NUMnormalizeRows (thy x -> eigenvectors, numberOfCoefficients, nx, 1);
		Melder_assert (thy x -> dimension == thy xLabels -> numberOfStrings &&
		               thy y -> dimension == thy yLabels -> numberOfStrings);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": CCA not created.");
	}
}

autoTableOfReal CCA_TableOfReal_scores (CCA me, TableOfReal thee, integer numberOfFactors) {
	try {
		integer n = thy numberOfRows;
		integer nx = my x -> dimension, ny = my y -> dimension;

		Melder_require (ny + nx == thy numberOfColumns, U"The number of columns in the table (", thy numberOfColumns,
			U") should agree with the dimensions of the CCA object (ny + nx = ", ny, U" + ", nx, U").");

		if (numberOfFactors == 0) {
			numberOfFactors = my numberOfCoefficients;
		}
		Melder_require (numberOfFactors > 0 && numberOfFactors <= my numberOfCoefficients, 
			U"The number of factors should be in interval [1, ", my numberOfCoefficients, U"].");
		
		autoTableOfReal him = TableOfReal_create (n, 2 * numberOfFactors);
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, thy numberOfRows);
		Eigen_TableOfReal_into_TableOfReal_projectRows (my y.get(), thee, 1, him.get(), 1, numberOfFactors);
		Eigen_TableOfReal_into_TableOfReal_projectRows (my x.get(), thee, ny + 1, him.get(), numberOfFactors + 1, numberOfFactors);
		TableOfReal_setSequentialColumnLabels (him.get(), 1, numberOfFactors, U"y_", 1, 1);
		TableOfReal_setSequentialColumnLabels (him.get(), numberOfFactors + 1, his numberOfColumns, U"x_", 1, 1);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no TableOfReal with scores created.");
	}
}

autoTableOfReal CCA_TableOfReal_predict (CCA me, TableOfReal thee, integer from) {
	try {
		integer ny = my y -> dimension, nx = my x -> dimension;
		integer nev = my y -> numberOfEigenvalues;

		/*
			We can only predict when we have the largest dimension as input
			and the number of coefficients equals the dimension of the smallest.
		*/
		Melder_require (ny == nev, U"There are not enough correlations present for prediction.");
		

		if (from == 0) {
			from = 1;
		}
		integer ncols = thy numberOfColumns - from + 1;
		Melder_require (from > 0 && ncols == nx, U"The number of columns to analyze should be equal to ", nx, U".");

		// ???? dimensions if nx .. ny ??

		autoTableOfReal him = Eigen_TableOfReal_to_TableOfReal_projectRows (my x.get(), thee, from, ny);
		autoNUMvector<double> buf (1, ny);

		// u = V a -> a = V'u

		double **v = my y -> eigenvectors;
		double *d = my y -> eigenvalues;
		for (integer i = 1; i <= thy numberOfRows; i ++) {
			NUMvector_copyElements (his data [i], buf.peek(), 1, ny);
			for (integer j = 1; j <= ny; j ++) {
				double t = 0.0;
				for (integer k = 1; k <= ny; k ++) {
					t += sqrt (d [k]) * v [k] [j] * buf [k];
				}
				his data [i] [j] = t;
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no predictions created.");
	}
}

autoTableOfReal CCA_TableOfReal_factorLoadings (CCA me, TableOfReal thee) {
	try {
		autoCorrelation c = TableOfReal_to_Correlation (thee);
		autoTableOfReal him = CCA_Correlation_factorLoadings (me, c.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no factor loadings created.");
	}
}

double CCA_getCorrelationCoefficient (CCA me, integer index) {
	if (index < 1 || index > my numberOfCoefficients) {
		return undefined;
	}
	return sqrt (my y -> eigenvalues[index]);
}

void CCA_getZeroCorrelationProbability (CCA me, integer index, double *p_prob, double *p_chisq, double *p_df) {
	double lambda = 1.0, *ev = my y -> eigenvalues;
	integer nev = my y -> numberOfEigenvalues;
	integer ny = my y -> dimension, nx = my x -> dimension;

	double chisq = undefined, prob = undefined, df = undefined;

	if (index >= 1 && index <= nev) {
		for (integer i = index; i <= nev; i ++) {
			lambda *= 1.0 - ev [i];
		}
		df = (ny - index + 1) * (nx - index + 1);
		chisq = - (my numberOfObservations - (ny + nx + 3.0) / 2.0) * log (lambda);
		prob = NUMchiSquareQ (chisq, df);
	}
	if (p_chisq) {
		*p_chisq = chisq;
	}
	if (p_df) {
		*p_df = df;
	}
	if (p_prob) {
		*p_prob = prob;
	}
}

/* End of file CCA.c */
