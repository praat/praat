/* Eigen.cpp
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
 djmw 20010719
 djmw 20020402 GPL header.
 djmw 20030701 Modified sorting in Eigen_initFromSquareRootPair.
 djmw 20030703 Added Eigens_alignEigenvectors.
 djmw 20030708 Corrected syntax error in Eigens_alignEigenvectors.
 djmw 20030812 Corrected memory bug in Eigen_initFromSymmetricMatrix.
 djmw 20030825 Removed praat_USE_LAPACK external variable.
 djmw 20031101 Documentation
 djmw 20031107 Moved NUMdmatrix_transpose to NUM2.c
 djmw 20031210 Added rowLabels to Eigen_drawEigenvector and Eigen_and_Strings_drawEigenvector
 djmw 20030322 Extra test in Eigen_initFromSquareRootPair.
 djmw 20040329 Added fractionOfTotal  and cumulative parameters in Eigen_drawEigenvalues_scree.
 djmw 20040622 Less horizontal labels in Eigen_drawEigenvector.
 djmw 20050706 Shortened horizontal offsets in Eigen_drawEigenvalues from 1 to 0.5
 djmw 20051204 Eigen_initFromSquareRoot adapted for nrows < ncols
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20110304 Thing_new
*/

#include "Eigen.h"
#include "NUMmachar.h"
#include "NUMlapack.h"
#include "NUMclapack.h"
#include "NUM2.h"
#include "SVD.h"

#include "oo_DESTROY.h"
#include "Eigen_def.h"
#include "oo_COPY.h"
#include "Eigen_def.h"
#include "oo_EQUAL.h"
#include "Eigen_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Eigen_def.h"
#include "oo_WRITE_TEXT.h"
#include "Eigen_def.h"
#include "oo_READ_TEXT.h"
#include "Eigen_def.h"
#include "oo_WRITE_BINARY.h"
#include "Eigen_def.h"
#include "oo_READ_BINARY.h"
#include "Eigen_def.h"
#include "oo_DESCRIPTION.h"
#include "Eigen_def.h"

Thing_implement (Eigen, Daata, 0);

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

static void Graphics_ticks (Graphics g, double min, double max, bool hasNumber, bool hasTick, bool hasDottedLine, bool integers) {
	double range = max - min, scale = 1.0, tick = min, dtick = 1.0;

	if (range == 0.0) {
		return;
	} else if (range > 1.0) {
		while (range / scale > 10.0) {
			scale *= 10.0;
		}
		range /= scale;
	} else {
		while (range / scale < 10.0) {
			scale /= 10.0;
		}
		range *= scale;
	}

	if (range < 3.0) {
		dtick = 0.5;
	}
	dtick *= scale;
	tick = dtick * floor (min / dtick);
	if (tick < min) {
		tick += dtick;
	}
	while (tick <= max) {
		double num = integers ? round (tick) : tick;
		Graphics_markBottom (g, num, hasNumber, hasTick, hasDottedLine, nullptr);
		tick += dtick;
	}
}

void Eigen_init (Eigen me, long numberOfEigenvalues, long dimension) {
	my numberOfEigenvalues = numberOfEigenvalues;
	my dimension = dimension;
	my eigenvalues = NUMvector<double> (1, numberOfEigenvalues);
	my eigenvectors = NUMmatrix<double> (1, numberOfEigenvalues, 1, dimension);
}

/*
	Solve: (A'A - lambda)x = 0 for eigenvalues lambda and eigenvectors x.
	svd(A) = UDV' => A'A = (UDV')'(UDV') = VD^2V'
	(VD^2V'-lambda)x = 0 => (D^2 - lambda)V'x = 0 => solution V'x = I => x = V
	Eigenvectors: the columns of the matrix V
	Eigenvalues: D_i^2
*/
void Eigen_initFromSquareRoot (Eigen me, double **a, long numberOfRows, long numberOfColumns) {
	long numberOfZeroed, numberOfEigenvalues;
	long nsv = MIN (numberOfRows, numberOfColumns);

	my dimension = numberOfColumns;
	autoSVD svd = SVD_create_d (a, numberOfRows, numberOfColumns);

	/*
		Make sv's that are too small zero. These values occur automatically
		when the rank of A'A < numberOfColumns. This could occur when for
		example numberOfRows <= numberOfColumns.
		(n points in  an n-dimensional space define maximally an n-1
		dimensional surface for which we maximally need an n-1 dimensional
		basis.)
	*/

	numberOfZeroed = SVD_zeroSmallSingularValues (svd.get(), 0.0);

	numberOfEigenvalues = nsv - numberOfZeroed;

	Eigen_init (me, numberOfEigenvalues, numberOfColumns);
	long k = 0;
	for (long i = 1; i <= nsv; i++) {
		double t = svd -> d[i];
		if (t > 0.0) {
			my eigenvalues[++k] = t * t;
			for (long j = 1; j <= numberOfColumns; j++) {
				my eigenvectors[k][j] = svd -> v[j][i];
			}
		}
	}
	Eigen_sort (me);
}

void Eigen_initFromSquareRootPair (Eigen me, double **a, long numberOfRows, long numberOfColumns, double **b, long numberOfRows_b) {
	double *u = nullptr, *v = nullptr, maxsv2 = -10.0;
	char jobu = 'N', jobv = 'N', jobq = 'Q';
	long k, ll, m = numberOfRows, n = numberOfColumns, p = numberOfRows_b;
	long lda = m, ldb = p, ldu = lda, ldv = ldb, ldq = n;
	long lwork = MAX (MAX (3 * n, m), p) + n, info;

	/*	Melder_assert (numberOfRows >= numberOfColumns || numberOfRows_b >= numberOfColumns);*/

	my dimension = numberOfColumns;

	autoNUMvector<double> alpha (1, n);
	autoNUMvector<double> beta (1, n);
	autoNUMvector<double> work (1, lwork);
	autoNUMvector<long> iwork (1, n);
	autoNUMmatrix<double> q (1, n, 1, n);
	autoNUMmatrix<double> ac (NUMmatrix_transpose (a, numberOfRows, numberOfColumns), 1, 1);
	autoNUMmatrix<double> bc (NUMmatrix_transpose (b, numberOfRows_b, numberOfColumns), 1, 1);

	(void) NUMlapack_dggsvd (&jobu, &jobv, &jobq, &m, &n, &p, &k, &ll,
	    &ac[1][1], &lda, &bc[1][1], &ldb, &alpha[1], &beta[1], u, &ldu,
	    v, &ldv, &q[1][1], &ldq, &work[1], &iwork[1], &info);

	if (info != 0) {
		Melder_throw (U"dggsvd fails.");
	}

	// Calculate the eigenvalues (alpha[i]/beta[i])^2 and store in alpha[i].

	maxsv2 = -1.0;
	for (long i = k + 1; i <= k + ll; i++) {
		double t = alpha[i] / beta[i];
		alpha[i] = t * t;
		if (alpha[i] > maxsv2) {
			maxsv2 = alpha[i];
		}
	}

	// Deselect the eigenvalues < eps * max_eigenvalue.

	n = 0;
	for (long i = k + 1; i <= k + ll; i++) {
		if (alpha[i] < NUMfpp -> eps * maxsv2) {
			n++; alpha[i] = -1.0;
		}
	}

	if (ll - n < 1) {
		Melder_throw (U"No eigenvectors can be found. Matrix too singular.");
	}

	Eigen_init (me, ll - n, numberOfColumns);

	long ii = 0;
	for (long i = k + 1; i <= k + ll; i++) {
		if (alpha[i] == -1.0) {
			continue;
		}

		my eigenvalues[++ii] = alpha[i];
		for (long j = 1; j <= numberOfColumns; j++) {
			my eigenvectors[ii][j] = q[i][j];
		}
	}

	Eigen_sort (me);

	NUMnormalizeRows (my eigenvectors, my numberOfEigenvalues, numberOfColumns, 1);
}

void Eigen_initFromSymmetricMatrix (Eigen me, double **a, long n) {
	double wt[1], temp;
	char jobz = 'V', uplo = 'U';
	long lwork = -1, info;

	my dimension = my numberOfEigenvalues = n;

	if (! my eigenvectors) {
		Eigen_init (me, n, n);
	}

	NUMmatrix_copyElements (a, my eigenvectors, 1, n, 1, n);

	// Get size of work array

	(void) NUMlapack_dsyev (&jobz, &uplo, &n, &my eigenvectors[1][1], &n,
	                        &my eigenvalues[1], wt, &lwork, &info);
	if (info != 0) {
		Melder_throw (U"dsyev initialization fails");
	}

	lwork = (long) floor (wt[0]);
	autoNUMvector<double> work (0L, lwork);

	(void) NUMlapack_dsyev (&jobz, &uplo, &n, &my eigenvectors[1][1], &n, &my eigenvalues[1], work.peek(), &lwork, &info);
	if (info != 0) {
		Melder_throw (U"dsyev fails");
	}

	// We want descending order instead of ascending.

	for (long i = 1; i <= n / 2; i++) {
		long ilast = n - i + 1;

		SWAP (my eigenvalues[i], my eigenvalues[ilast])
		for (long j = 1; j <= n; j++) {
			SWAP (my eigenvectors[i][j], my eigenvectors[ilast][j])
		}
	}
}

autoEigen Eigen_create (long numberOfEigenvalues, long dimension) {
	try {
		autoEigen me = Thing_new (Eigen);
		Eigen_init (me.get(), numberOfEigenvalues, dimension);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Eigen not created.");
	}
}

long Eigen_getNumberOfEigenvectors (Eigen me) {
	return my numberOfEigenvalues;
}

double Eigen_getEigenvectorElement (Eigen me, long ivec, long element) {
	if (ivec > my numberOfEigenvalues || element < 1 || element > my dimension) {
		return undefined;
	}
	return my eigenvectors[ivec][element];
}

long Eigen_getDimensionOfComponents (Eigen me) {
	return my dimension;
}

double Eigen_getSumOfEigenvalues (Eigen me, long from, long to) {
	if (from < 1) {
		from = 1;
	}
	if (to < 1) {
		to = my numberOfEigenvalues;
	}
	if (to > my numberOfEigenvalues || from > to) {
		return undefined;
	}
	double sum = 0.0;
	for (long i = from; i <= to; i++) {
		sum += my eigenvalues[i];
	}
	return sum;
}

double Eigen_getCumulativeContributionOfComponents (Eigen me, long from, long to) {
	double partial = 0.0, sum = 0.0;

	if (to == 0) {
		to = my numberOfEigenvalues;
	}
	if (from > 0 && to <= my numberOfEigenvalues && from <= to) {
		for (long i = 1; i <= my numberOfEigenvalues; i++) {
			sum += my eigenvalues[i];
			if (i >= from && i <= to) {
				partial += my eigenvalues[i];
			}
		}
	}
	return sum > 0.0 ? partial / sum : 0.0;

}

long Eigen_getDimensionOfFraction (Eigen me, double fraction) {
	double sum = Eigen_getSumOfEigenvalues (me, 0, 0);

	if (sum == 0.0) {
		return 1;
	}

	long n = 1;
	double p = my eigenvalues[1];
	while (p / sum < fraction && n < my numberOfEigenvalues) {
		p += my eigenvalues[++n];
	}
	return n;
}

void Eigen_sort (Eigen me) {
	double temp, *e = my eigenvalues, **v = my eigenvectors;

	for (long i = 1; i < my numberOfEigenvalues; i++) {
		long k;
		double emax = e[k = i];
		for (long j = i + 1; j <= my numberOfEigenvalues; j++) {
			if (e[j] > emax) {
				emax = e[k = j];
			}
		}
		if (k != i) {

			// Swap eigenvalues and eigenvectors

			SWAP (e[i], e[k])
			for (long j = 1; j <= my dimension; j++) {
				SWAP (v[i][j], v[k][j])
			}
		}
	}
}

void Eigen_invertEigenvector (Eigen me, long ivec) {

	if (ivec < 1 || ivec > my numberOfEigenvalues) {
		return;
	}

	for (long j = 1; j <= my dimension; j++) {
		my eigenvectors[ivec][j] = - my eigenvectors[ivec][j];
	}
}

void Eigen_drawEigenvalues (Eigen me, Graphics g, long first, long last, double ymin, double ymax, bool fractionOfTotal, bool cumulative, double size_mm, const char32 *mark, bool garnish) {
	double xmin = first, xmax = last, scale = 1.0, sumOfEigenvalues = 0.0;

	if (first < 1) {
		first = 1;
	}
	if (last < 1 || last > my numberOfEigenvalues) {
		last = my numberOfEigenvalues;
	}
	if (last <= first) {
		first = 1; last = my numberOfEigenvalues;
	}
	xmin = first - 0.5; xmax = last + 0.5;
	if (fractionOfTotal || cumulative) {
		sumOfEigenvalues = Eigen_getSumOfEigenvalues (me, 0, 0);
		if (sumOfEigenvalues <= 0.0) {
			sumOfEigenvalues = 1.0;
		}
		scale = sumOfEigenvalues;
	}
	if (ymax <= ymin) {
		ymax = Eigen_getSumOfEigenvalues (me, ( cumulative ? 1 : first ), first) / scale;
		ymin = Eigen_getSumOfEigenvalues (me, ( cumulative ? 1 : last ), last) / scale;
		if (ymin > ymax) {
			double tmp = ymin; ymin = ymax; ymax = tmp;
		}
		if (ymin == ymax) { // only one eigenvalue
			ymin -= 0.1 * ymin; ymax += 0.1 * ymax;
		}
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	for (long i = first; i <= last; i ++) {
		double accu = Eigen_getSumOfEigenvalues (me, (cumulative ? 1 : i), i);
		Graphics_mark (g, i, accu / scale, size_mm, mark);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, true, ( fractionOfTotal ? ( cumulative ? U"Cumulative fractional eigenvalue" : U"Fractional eigenvalue" ) :
			                   ( cumulative ? U"Cumulative eigenvalue" : U"Eigenvalue" ) ));
		Graphics_ticks (g, first, last, true, true, false, true);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textBottom (g, true, U"Index");
	}
}

void Eigen_drawEigenvector (Eigen me, Graphics g, long ivec, long first, long last, double ymin, double ymax, bool weigh, double size_mm, const char32 *mark, bool connect, char32 **rowLabels, bool garnish) {
	double xmin = first, xmax = last;

	if (ivec < 1 || ivec > my numberOfEigenvalues) {
		return;
	}

	if (last <= first) {
		first = 1; last = my dimension;
		xmin = 0.5; xmax = last + 0.5;
	}
	double *vec = my eigenvectors[ivec];
	double w = weigh ? sqrt (my eigenvalues[ivec]) : 1.0;

	// If ymax < ymin the eigenvector will automatically be drawn inverted.

	if (ymax == ymin) {
		NUMvector_extrema (vec, first, last, &ymin, &ymax);
		ymax *= w; ymin *= w;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	for (long i = first; i <= last; i++) {
		Graphics_mark (g, i, w * vec[i], size_mm, mark);
		if (connect && i > first) {
			Graphics_line (g, i - 1.0, w * vec[i - 1], i, w * vec[i]);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_markBottom (g, first, false, true, false, rowLabels ? rowLabels[first] : Melder_integer (first));
		Graphics_markBottom (g, last, false, true, false, rowLabels ? rowLabels[last] : Melder_integer (last));
		Graphics_drawInnerBox (g);
		if (ymin * ymax < 0.0) {
			Graphics_markLeft (g, 0.0, true, true, true, nullptr);
		}
		Graphics_marksLeft (g, 2, true, true, false);
		if (! rowLabels) {
			Graphics_textBottom (g, true, U"Element number");
		}
	}
}

void Eigens_alignEigenvectors (OrderedOf<structEigen>* me) {
	if (my size < 2) {
		return;
	}

	Eigen e1 = my at [1];
	double **evec1 = e1 -> eigenvectors;
	long nev1 = e1 -> numberOfEigenvalues;
	long dimension = e1 -> dimension;

	for (long i = 2; i <= my size; i ++) {
		Eigen e2 = my at [i];
		if (e2 -> dimension != dimension) {
			Melder_throw (U"The dimension of the eigenvectors must be equal (offending object is ",  i, U").");
		}
	}

	/*
		Correlate eigenvectors.
		If r < 0 then mirror the eigenvector.
	*/

	for (long i = 2; i <= my size; i ++) {
		Eigen e2 = my at [i];
		double **evec2 = e2 -> eigenvectors;

		for (long j = 1; j <= MIN (nev1, e2 -> numberOfEigenvalues); j ++) {
			double ip = 0.0;
			for (long k = 1; k <= dimension; k ++) {
				ip += evec1 [j] [k] * evec2 [j] [k];
			}
			if (ip < 0.0) {
				for (long k = 1; k <= dimension; k ++) {
					evec2 [j] [k] = - evec2 [j] [k];
				}
			}
		}
	}
}

static void Eigens_getAnglesBetweenSubspaces (Eigen me, Eigen thee, long ivec_from, long ivec_to, double *angles_degrees) {
	long nvectors = ivec_to - ivec_from + 1;
	for (long i = 1; i <= nvectors; i++) {
		angles_degrees[i] = undefined;
	}
	long nmin = my numberOfEigenvalues < thy numberOfEigenvalues ? my numberOfEigenvalues : thy numberOfEigenvalues;

	if (my dimension != thy dimension) {
		Melder_throw (U"The eigenvectors must have the same dimension.");
	}
	if (ivec_from > ivec_to || ivec_from < 1 || ivec_to > nmin) {
		Melder_throw (U"Eigenvector range too large.");
	}

	autoNUMmatrix<double> c (1, nvectors, 1, nvectors);

	/*
		Algorithm 12.4.3 Golub & van Loan
		Because we deal with eigenvectors we don't have to do the QR decomposition,
			the columns in the Q's are the eigenvectors.
		Compute C.
	*/

	for (long i = 1; i <= nvectors; i++) {
		for (long j = 1; j <= nvectors; j++) {
			for (long k = 1; k <= my dimension; k++) {
				c[i][j] += my eigenvectors[ivec_from + i - 1][k] * thy eigenvectors[ivec_from + j - 1][k];
			}
		}
	}
	autoSVD svd = SVD_create_d (c.peek(), nvectors, nvectors);
	for (long i = 1; i <= nvectors; i++) {
		angles_degrees[i] = acos (svd -> d[i]) * 180.0 / NUMpi;
	}
}

double Eigens_getAngleBetweenEigenplanes_degrees (Eigen me, Eigen thee) {
	double angles_degrees[3];

	Eigens_getAnglesBetweenSubspaces (me, thee, 1, 2, angles_degrees);
	return angles_degrees[2];
}

/* Very low level */

void Eigen_and_matrix_into_matrix_principalComponents (Eigen me, double **from, long numberOfRows, long from_colbegin, double **to, long numberOfDimensionsToKeep, long to_colbegin) {
	/*
	 * Preconditions:
	 * 
	 * 	from[numberOfRows, from_colbegin - 1 + my dimension] exists
	 * 	to [numberOfRows, to_colbegin - 1 + numberOfDimensionsToKeep] exists
	 * 
	 * Project/rotate the vectors in matrix 'from' along the 'numberOfDimensionsToKeep' eigenvectors into the matrix 'to'.
	 */
	from_colbegin = from_colbegin <= 0 ? 1 : from_colbegin;
	to_colbegin = to_colbegin <= 0 ? 1 : to_colbegin;
	if (numberOfDimensionsToKeep > my numberOfEigenvalues) {
		Melder_throw (U"The number of dimensions to keep must not be larger than the number of eigenvalues.");
	}
	for (long irow = 1; irow <= numberOfRows; irow ++) {
		for (long icol = 1; icol <= numberOfDimensionsToKeep; icol ++) {
			double r = 0.0;
			for (long k = 1; k <= my dimension; k ++) {
				// eigenvector[icol] is in row[icol] of my eigenvectors
				r += my eigenvectors  [icol] [k] * from [irow] [from_colbegin + k - 1];
			}
			to [irow] [to_colbegin + icol - 1] = r;
		}
	}
}


#undef MAX
#undef MIN
#undef SWAP

/* End of file Eigen.cpp */
