/* CCA.c
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
 djmw 2001
 djmw 20020402 GPL header
 djme 20021008 removed SVD_sort
 djmw 20031023 Removed one argument from CCA_and_TableOfReal_scores
 djmw 20031106 Removed bug from CCA_and_TableOfReal_scores
 djmw 20031221 Removed bug: CCA_and_TableOfReal_scores (wrong dimensions to Eigen_project_into).
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
	structData :: v_info ();
	MelderInfo_writeLine (L"Number of coefficients: ", Melder_integer (numberOfCoefficients));
	MelderInfo_writeLine (L"ny: ", Melder_integer (y -> dimension));
	MelderInfo_writeLine (L"nx: ", Melder_integer (x -> dimension));
}

Thing_implement (CCA, Data, 0);

CCA CCA_create (long numberOfCoefficients, long ny, long nx) {
	try {
		autoCCA me = Thing_new (CCA);
		my numberOfCoefficients = numberOfCoefficients;
		my yLabels = Thing_new (Strings);
		my xLabels = Thing_new (Strings);
		my y = Eigen_create (numberOfCoefficients, ny);
		my x = Eigen_create (numberOfCoefficients, nx);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("CCA not created.");
	}
}

void CCA_drawEigenvector (CCA me, Graphics g, int x_or_y, long ivec, long first, long last,
                          double ymin, double ymax, int weigh, double size_mm, const wchar_t *mark,	int connect, int garnish) {
	Eigen e = my x;
	Strings labels = my xLabels;
	if (x_or_y == 1) {
		e = my y; labels = my yLabels;
	}
	Eigen_drawEigenvector (e, g, ivec, first, last, ymin, ymax, weigh, size_mm, mark,
	                       connect, labels -> strings, garnish);
}

double CCA_getEigenvectorElement (CCA me, int x_or_y, long ivec, long element) {
	Eigen e = x_or_y == 1 ? my y : my x;
	return Eigen_getEigenvectorElement (e, ivec, element);
}

CCA TableOfReal_to_CCA (TableOfReal me, long ny) {
	try {
		long n = my numberOfRows, nx = my numberOfColumns - ny;

		if (ny < 1 || ny > my numberOfColumns - 1) {
			Melder_throw ("Dimension of first part not correct.");
		}
		if (ny > nx) Melder_throw (L"The dimension of the dependent part (", ny, L") must be less than or equal to "
			                           "the dimension of the independent part (", nx, L").");
		if (n < ny) {
			Melder_throw (L"The number of observations must be larger then ", ny, ".");
		}

		TableOfReal_areAllCellsDefined (me, 0, 0, 0, 0);
		// Use svd as (temporary) storage, and copy data

		autoSVD svdy = SVD_create (n, ny);
		autoSVD svdx = SVD_create (n, nx);

		for (long i = 1; i <= n; i++) {
			for (long j = 1; j <= ny; j++) {
				svdy -> u[i][j] = my data[i][j];
			}
			for (long j = 1; j <= nx; j++) {
				svdx -> u[i][j] = my data[i][ny + j];
			}
		}

		double **uy = svdy -> u;
		double **vy = svdy -> v;
		double **ux = svdx -> u;
		double **vx = svdx -> v;
		double fnormy = NUMfrobeniusnorm (n, ny, uy);
		double fnormx = NUMfrobeniusnorm (n, nx, ux);
		if (fnormy == 0 || fnormx == 0) {
			Melder_throw ("One of the parts of the table contains only zeros.");
		}

		/*
			Centre the data and svd it.
		*/

		NUMcentreColumns (uy, 1, n, 1, ny, NULL);
		NUMcentreColumns (ux, 1, n, 1, nx, NULL);

		SVD_compute (svdy.peek()); SVD_compute (svdx.peek());

		long numberOfZeroedy = SVD_zeroSmallSingularValues (svdy.peek(), 0);
		long numberOfZeroedx = SVD_zeroSmallSingularValues (svdx.peek(), 0);

		/*
			Form the matrix C = ux' uy (use svd-object storage)
		*/

		autoSVD svdc = SVD_create (nx, ny);
		double **uc = svdc -> u;
		double **vc = svdc -> v;

		for (long i = 1; i <= nx; i++) {
			for (long j = 1; j <= ny; j++) {
				double t = 0;
				for (long q = 1; q <= n; q++) {
					t += ux[q][i] * uy[q][j];
				}
				uc[i][j] = t;
			}
		}

		SVD_compute (svdc.peek());
		long numberOfZeroedc = SVD_zeroSmallSingularValues (svdc.peek(), 0);
		long numberOfCoefficients = ny - numberOfZeroedc;

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

		for (long i = 1; i <= numberOfCoefficients; i++) {
			double ccc = svdc -> d[i];
			thy y -> eigenvalues[i] = thy x -> eigenvalues[i] = ccc * ccc;
			for (long j = 1; j <= ny; j++) {
				double t = 0;
				for (long q = 1; q <= ny - numberOfZeroedy; q++) {
					t += vc[q][i] * vy[j][q] / svdy -> d[q];
				}
				evecy[i][j] = t;
			}
			for (long j = 1; j <= nx; j++) {
				double t = 0;
				for (long q = 1; q <= nx - numberOfZeroedx; q++) {
					t += uc[q][i] * vx[j][q] / svdx -> d[q];
				}
				evecx[i][j] = t;
			}
		}

		/*
			Normalize eigenvectors.
		*/
		NUMnormalizeRows (thy y -> eigenvectors, numberOfCoefficients, ny, 1);
		NUMnormalizeRows (thy x -> eigenvectors, numberOfCoefficients, nx, 1);
		Melder_assert (thy x -> dimension == thy xLabels -> numberOfStrings &&
		               thy y -> dimension == thy yLabels -> numberOfStrings);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": CCA not created.");
	}
}

TableOfReal CCA_and_TableOfReal_scores (CCA me, TableOfReal thee, long numberOfFactors) {
	try {
		long n = thy numberOfRows;
		long nx = my x -> dimension, ny = my y -> dimension;

		if (ny + nx != thy numberOfColumns) Melder_throw ("The number of columns in the table (", thy numberOfColumns,
			        ") does not agree with the dimensions of the CCA object (ny + nx = ", ny, " + ", nx, ").");

		if (numberOfFactors == 0) {
			numberOfFactors = my numberOfCoefficients;
		}
		if (numberOfFactors < 1 || numberOfFactors > my numberOfCoefficients) Melder_throw
			("The number of factors must be in interval [1, ", my numberOfCoefficients, L"].");

		autoTableOfReal him = TableOfReal_create (n, 2 * numberOfFactors);
		TableOfReal phim = him.peek();
		NUMstrings_copyElements (thy rowLabels, his rowLabels, 1, thy numberOfRows);
		Eigen_and_TableOfReal_project_into (my y, thee, 1, ny, &phim, 1, numberOfFactors);
		Eigen_and_TableOfReal_project_into (my x, thee, ny + 1, thy numberOfColumns, &phim, numberOfFactors + 1, his numberOfColumns);
		TableOfReal_setSequentialColumnLabels (him.peek(), 1, numberOfFactors, L"y_", 1, 1);
		TableOfReal_setSequentialColumnLabels (him.peek(), numberOfFactors + 1, his numberOfColumns, L"x_", 1, 1);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no TableOfReal with scores created.");
	}
}

TableOfReal CCA_and_TableOfReal_predict (CCA me, TableOfReal thee, long from) {
	try {
		long ny = my y -> dimension, nx = my x -> dimension;
		long nev = my y -> numberOfEigenvalues;

		/*
			We can only predict when we have the largest dimension as input
			and the number of coefficients equals the dimension of the smallest.
		*/

		if (ny != nev) {
			Melder_throw ("There are not enough correlations present for prediction.");
		}

		if (from == 0) {
			from = 1;
		}
		long ncols = thy numberOfColumns - from + 1;
		if (from < 1 || ncols != nx) {
			Melder_throw ("The number of columns to analyze must be equal to ", nx, ".");
		}

		// ???? dimensions if nx .. ny ??

		autoTableOfReal him = Eigen_and_TableOfReal_project (my x, thee, from, ny);
		autoNUMvector<double> buf (1, ny);

		// u = V a -> a = V'u

		double **v = my y -> eigenvectors;
		double *d = my y -> eigenvalues;
		for (long i = 1; i <= thy numberOfRows; i++) {
			NUMvector_copyElements (his data[i], buf.peek(), 1, ny);
			for (long j = 1; j <= ny; j++) {
				double t = 0;
				for (long k = 1; k <= ny; k++) {
					t += sqrt (d[k]) * v[k][j] * buf[k];
				}
				his data [i][j] = t;
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no predictions created.");
	}
}

TableOfReal CCA_and_TableOfReal_factorLoadings (CCA me, TableOfReal thee) {
	try {
		autoCorrelation c = TableOfReal_to_Correlation (thee);
		autoTableOfReal him = CCA_and_Correlation_factorLoadings (me, c.peek());
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no factor loadings created.");
	}
}

double CCA_getCorrelationCoefficient (CCA me, long index) {
	if (index < 1 || index > my numberOfCoefficients) {
		return NUMundefined;
	}

	return sqrt (my y -> eigenvalues[index]);
}

void CCA_getZeroCorrelationProbability (CCA me, long index, double *chisq, long *ndf, double *probability) {
	double lambda = 1, *ev = my y -> eigenvalues;
	long nev = my y -> numberOfEigenvalues;
	long ny = my y -> dimension, nx = my x -> dimension;

	*chisq = *probability = NUMundefined;
	*ndf = 0;

	if (index < 1 || index > nev) {
		return;
	}

	for (long i = index; i <= nev; i++) {
		lambda *= (1 - ev[i]);
	}
	*ndf = (ny - index + 1) * (nx - index + 1);
	*chisq = - (my numberOfObservations - (ny + nx + 3) / 2) * log (lambda);
	*probability = NUMchiSquareQ (*chisq, *ndf);
}

/* End of file CCA.c */
