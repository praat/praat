/* SVD.cpp
 *
 * Copyright (C) 1994-2017 David Weenink
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
 djmw 20020408 GPL + cosmetic changes.
 djmw 20020415 +SVD_synthesize.
 djmw 20030624 Removed NRC svd calls.
 djmw 20030825 Removed praat_USE_LAPACK external variable.
 djmw 20031018 Removed  bug in SVD_solve that caused incorrect output when nrow > ncol
 djmw 20031101 Changed documentation in SVD_compute + bug correction in SVD_synthesize.
 djmw 20031111 Added GSVD_create_d.
 djmw 20051201 Adapt for numberOfRows < numberOfColumns
 djmw 20060810 Removed #include praat.h
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20070102 Removed the #include "TableOfReal.h"
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20110304 Thing_new
*/

#include "SVD.h"
#include "NUMlapack.h"
#include "NUMmachar.h"
#include "Collection.h"
#include "NUMclapack.h"
#include "NUMcblas.h"

#include "oo_DESTROY.h"
#include "SVD_def.h"
#include "oo_COPY.h"
#include "SVD_def.h"
#include "oo_EQUAL.h"
#include "SVD_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SVD_def.h"
#include "oo_WRITE_TEXT.h"
#include "SVD_def.h"
#include "oo_WRITE_BINARY.h"
#include "SVD_def.h"
#include "oo_READ_TEXT.h"
#include "SVD_def.h"
#include "oo_READ_BINARY.h"
#include "SVD_def.h"
#include "oo_DESCRIPTION.h"
#include "SVD_def.h"

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

void structSVD :: v_info () {
	MelderInfo_writeLine (U"Number of rows: ", numberOfRows);
	MelderInfo_writeLine (U"Number of columns: ", numberOfColumns);
	MelderInfo_writeLine (U"This matrix is", (isTransposed ? U"" : U"not "), U" transposed.");
}

Thing_implement (SVD, Daata, 1);

static void NUMtranspose_d (double **m, integer n) {
	for (integer i = 1; i <= n - 1; i ++) {
		for (integer j = i + 1; j <= n; j ++) {
			double t = m [i] [j];
			m [i] [j] = m [j] [i];
			m [j] [i] = t;
		}
	}
}

/*
	m >=n, mxn matrix A has svd UDV', where u is mxn, D is n and V is nxn.
	m < n, then transpose A. Consider A' with svd (UDV')'= VDU', where v is mxm, D is m and U' is mxn
*/
void SVD_init (SVD me, integer numberOfRows, integer numberOfColumns) {
	if (numberOfRows < numberOfColumns) {
		my isTransposed = true;
		integer tmp = numberOfRows; numberOfRows = numberOfColumns; numberOfColumns = tmp;
	}
	my numberOfRows = numberOfRows;
	my numberOfColumns = numberOfColumns;
	if (! NUMfpp) {
		NUMmachar ();
	}
	my tolerance = NUMfpp -> eps * numberOfRows;
	my u = NUMmatrix<double> (1, numberOfRows, 1, numberOfColumns);
	my v = NUMmatrix<double> (1, numberOfColumns, 1, numberOfColumns);
	my d = NUMvector<double> (1, numberOfColumns);
}

autoSVD SVD_create (integer numberOfRows, integer numberOfColumns) {
	try {
		autoSVD me = Thing_new (SVD);
		SVD_init (me.get(), numberOfRows, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SVD not created.");
	}
}

autoSVD SVD_create_d (double **m, integer numberOfRows, integer numberOfColumns) {
	try {
		autoSVD me = SVD_create (numberOfRows, numberOfColumns);
		SVD_svd_d (me.get(), m);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SVD not created from vector.");
	}
}

autoSVD SVD_create_f (float **m, integer numberOfRows, integer numberOfColumns) {
	try {
		autoSVD me = SVD_create (numberOfRows, numberOfColumns);
		SVD_svd_f (me.get(), m);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SVD not created from vector.");
	}
}

void SVD_svd_d (SVD me, double **m) {
	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			my u [i] [j] = my isTransposed ? m [j] [i] : m [i] [j];
		}
	}
	SVD_compute (me);
}

void SVD_svd_f (SVD me, float **m) {
	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			my u [i] [j] = my isTransposed ? m [j] [i] : m [i] [j];
		}
	}
	SVD_compute (me);
}

void SVD_setTolerance (SVD me, double tolerance) {
	my tolerance = tolerance;
}

double SVD_getTolerance (SVD me) {
	return my tolerance;
}

/*
	Compute svd(A) = U D Vt.
	The svd routine from CLAPACK uses (fortran) column major storage, while	C uses row major storage.
	To solve the problem above we have to transpose the matrix A, calculate the
	solution and transpose the U and Vt matrices of the solution.
	However, if we solve the transposed problem svd(A') = V D U', we have less work to do:
	We may call the algorithm with reverted row/column dimensions, and we switch the U and V'
	output arguments.
	The only thing that we have to do afterwards is transposing the (small) V matrix
	because the SVD-object has row vectors in v.
	The sv's are already sorted.
	int NUMlapack_dgesvd (char *jobu, char *jobvt, integer *m, integer *n, double *a, integer *lda,
		double *s, double *u, integer *ldu, double *vt, integer *ldvt, double *work,
		integer *lwork, integer *info);
*/
void SVD_compute (SVD me) {
	try {
		char jobu = 'S', jobvt = 'O';
		integer m, lda, ldu, ldvt, info, lwork = -1;
		double wt [2];

		lda = ldu = ldvt = m = my numberOfColumns;
		integer n = my numberOfRows;

		(void) NUMlapack_dgesvd (& jobu, & jobvt, & m, & n, & my u [1] [1], & lda, & my d [1], & my v [1] [1], & ldu, nullptr, & ldvt, wt, & lwork, & info);
		Melder_require (info == 0, U"SVD could not be precomputed.");

		lwork = wt [0];
		autoNUMvector<double> work ((integer) 0, lwork);
		(void) NUMlapack_dgesvd (& jobu, & jobvt, & m, & n, & my u [1] [1], & lda, & my d [1], & my v [1] [1], & ldu, nullptr, & ldvt, work.peek(), & lwork, & info);
		Melder_require (info == 0, U"SVD could not be computed.");

		NUMtranspose_d (my v, my numberOfColumns);
		
	} catch (MelderError) {
		Melder_throw (me, U": SVD could not be computed.");
	}
}

// V D^2 V'or V D^-2 V
void SVD_getSquared (SVD me, double **m, bool inverse) {
	for (integer i = 1; i <= my numberOfColumns; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			real80 val = 0.0;
			for (integer k = 1; k <= my numberOfColumns; k ++) {
				if (my d [k] > 0.0) {
					double dsq = my d [k] * my d [k];
					double factor = inverse ? 1.0 / dsq : dsq;
					val += my v [i] [k] * my v [j] [k] * factor;
				}
			}
			m [i] [j] = (real) val;
		}
	}
}

void SVD_solve (SVD me, double b [], double x []) {
	try {
		autoNUMvector<double> t (1, my numberOfColumns);

		/*  Solve UDV' x = b.
			Solution: x = V D^-1 U' b */

		for (integer j = 1; j <= my numberOfColumns; j ++) {
			real80 tmp = 0.0;
			if (my d [j] > 0.0) {
				for (integer i = 1; i <= my numberOfRows; i ++) {
					tmp += my u [i] [j] * b [i];
				}
				tmp /= my d [j];
			}
			t [j] = (real) tmp;
		}

		for (integer j = 1; j <= my numberOfColumns; j ++) {
			real80 tmp = 0.0;
			for (integer i = 1; i <= my numberOfColumns; i ++) {
				tmp += my v [j] [i] * t [i];
			}
			x [j] = (real) tmp;
		}
	} catch (MelderError) {
		Melder_throw (me, U": not solved.");
	}
}

integer SVD_getMinimumNumberOfSingularValues (SVD me, double fractionOfSumOfSingularValues) {
	double sumOfSingularValues = SVD_getSumOfSingularValues (me, 1, my numberOfColumns);
	double criterion = sumOfSingularValues * fractionOfSumOfSingularValues;
	integer j = 1;
	real80 sum = my d [1];
	while (sum < criterion && j < my numberOfColumns) {
		sum += my d [++ j];
	}
	return j;
}

void SVD_solve2 (SVD me, double b [], double x [], double fractionOfSumOfSingularValues) {
	try {
		integer numberOfComponents = SVD_getMinimumNumberOfSingularValues (me, fractionOfSumOfSingularValues);
		autonumvec t (my numberOfColumns, kTensorInitializationType:: RAW);

		/*  Solve UDV' x = b.
			Solution: x = V D^-1 U' b 
			
			x = sum(i=1,M, (U [i].b)/d [i] V [i];
		
		*/
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			x [j] = 0.0;
		}
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			for (integer i = 1; i <= numberOfComponents; i ++) {
				real80 inproduct = 0.0; // column [i] from U 
				for (integer k = 1; k <= my numberOfRows; k ++) {
					inproduct += my u [k] [i] * b [k];
				}
				x [j] += inproduct * my v [j] [i] / my d [i];
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not solved.");
	}
}


void SVD_sort (SVD me) {
	try {
		autoSVD thee = Data_copy (me);
		autoNUMvector<integer> index (1, my numberOfColumns);

		NUMindexx (my d, my numberOfColumns, index.peek());

		for (integer j = 1; j <= my numberOfColumns; j ++) {
			integer from = index [my numberOfColumns - j + 1];
			my d [j] = thy d [from];
			for (integer i = 1; i <= my numberOfRows; i ++) {
				my u [i] [j] = thy u [i] [from];
			}
			for (integer i = 1; i <= my numberOfColumns; i ++) {
				my v [i] [j] = thy v [i] [from];
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": not sorted.");
	}
}

double SVD_getConditionNumber (SVD me) {
	return my d[my numberOfColumns] > 0.0 ? my d[1] / my d[my numberOfColumns] : undefined;
}

double SVD_getSumOfSingularValuesAsFractionOfTotal (SVD me, integer from, integer to) {
	double part = SVD_getSumOfSingularValues (me, from, to);
	double total = SVD_getSumOfSingularValues (me, 1, my numberOfColumns);
	return part / total;
}

double SVD_getSumOfSingularValues (SVD me, integer from, integer to) {
	from = from == 0 ? 1 : from;
	to = to == 0 ? my numberOfColumns : to;
	Melder_require (from > 0 && from <= to && to <= my numberOfColumns, U"The range should be within [1,", my numberOfColumns, U"].");
	real80 sum = 0.0;
	for (integer i = from; i <= to; i ++) {
		sum += my d [i];
	}
	return (real) sum;
}

integer SVD_zeroSmallSingularValues (SVD me, double tolerance) {
	integer numberOfZeroed = 0;
	double dmax = my d [1];

	if (tolerance == 0.0) {
		tolerance = my tolerance;
	}
	for (integer i = 2; i <= my numberOfColumns; i ++) {
		if (my d [i] > dmax) {
			dmax = my d [i];
		}
	}
	for (integer i = 1; i <= my numberOfColumns; i ++) {
		if (my d [i] < dmax * tolerance) {
			my d [i] = 0.0; numberOfZeroed ++;
		}
	}
	return numberOfZeroed;
}


integer SVD_getRank (SVD me) {
	integer rank = 0;
	for (integer i = 1; i <= my numberOfColumns; i ++) {
		if (my d [i] > 0.0) {
			rank ++;
		}
	}
	return rank;
}

/*
	SVD of A = U D V'.
	If u [i] is the i-th column vector of U and v [i] the i-th column vector of V and s [i] the i-th singular value,
	we can write the svd expansion  A = sum_{i=1}^n {d [i] u [i] v [i]'}.
	Golub & van Loan, 3rd ed, p 71.
*/
void SVD_synthesize (SVD me, integer sv_from, integer sv_to, double **m) {
	try {
		if (sv_to == 0) {
			sv_to = my numberOfColumns;
		}
		Melder_require (sv_from > 0 && sv_from <= sv_to && sv_to <= my numberOfColumns, U"Indices must be in range [1, ", my numberOfColumns, U"].");
		for (integer i = 1; i <= my numberOfRows; i ++) {
			for (integer j = 1; j <= my numberOfColumns; j ++) {
				if (my isTransposed) {
					m [j] [i] = 0.0;
				} else {
					m [i] [j] = 0.0;
				}
			}
		}
		for (integer k = sv_from; k <= sv_to; k ++) {
			for (integer i = 1; i <= my numberOfRows; i ++) {
				for (integer j = 1; j <= my numberOfColumns; j ++) {
					double value = my d [k] * my u [i] [k] * my v [j] [k];
					if (my isTransposed) {
						m [j] [i] += value;
					} else {
						m [i] [j] += value;
					}
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": no synthesis.");
	}
}

Thing_implement (GSVD, Daata, 0);

void structGSVD :: v_info () {
	MelderInfo_writeLine (U"Number of columns: ", numberOfColumns);
}

autoGSVD GSVD_create (integer numberOfColumns) {
	try {
		autoGSVD me = Thing_new (GSVD);
		my numberOfColumns = numberOfColumns;

		my q = NUMmatrix<double> (1, numberOfColumns, 1, numberOfColumns);
		my r = NUMmatrix<double> (1, numberOfColumns, 1, numberOfColumns);
		my d1 = NUMvector<double> (1, numberOfColumns);
		my d2 = NUMvector<double> (1, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"GSVD not created.");
	}
}

autoGSVD GSVD_create_d (double **m1, integer numberOfRows1, integer numberOfColumns, double **m2, integer numberOfRows2) {
	try {
		integer m = numberOfRows1, n = numberOfColumns, p = numberOfRows2;
		integer lwork = MAX (MAX (3 * n, m), p) + n;

		// Store the matrices a and b as column major!
		autoNUMmatrix<double> a (NUMmatrix_transpose (m1, m, n), 1, 1);
		autoNUMmatrix<double> b (NUMmatrix_transpose (m2, p, n), 1, 1);
		autoNUMmatrix<double> q (1, n, 1, n);
		autoNUMvector<double> alpha (1, n);
		autoNUMvector<double> beta (1, n);
		autoNUMvector<double> work (1, lwork);
		autoNUMvector<integer> iwork (1, n);


		char jobu1 = 'N', jobu2 = 'N', jobq = 'Q';
		integer k, l, info;
		NUMlapack_dggsvd (& jobu1, & jobu2, & jobq, & m, & n, & p, & k, & l,
		    & a [1] [1], & m, & b [1] [1], & p, & alpha [1], & beta [1], nullptr, & m,
		    nullptr, & p, & q [1] [1], & n, & work [1], & iwork [1], & info);
		Melder_require (info == 0, U"dggsvd failed, error = ", info);

		integer kl = k + l;
		autoGSVD me = GSVD_create (kl);

		for (integer i = 1; i <= kl; i ++) {
			my d1 [i] = alpha [i];
			my d2 [i] = beta [i];
		}

		// Transpose q

		for (integer i = 1; i <= n; i ++) {
			for (integer j = i + 1; j <= n; j ++) {
				my q [i] [j] = q [j] [i];
				my q [j] [i] = q [i] [j];
			}
			my q [i] [i] = q [i] [i];
		}

		// Get R from a(1:k+l,n-k-l+1:n)

		double *pr = & a [1] [1];
		for (integer i = 1; i <= kl; i ++) {
			for (integer j = i; j <= kl; j ++) {
				my r [i] [j] = pr [i - 1 + (n - kl + j - 1) * m]; /* from col-major */
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"GSVD not created.");
	}
}

void GSVD_setTolerance (GSVD me, double tolerance) {
	my tolerance = tolerance;
}

double GSVD_getTolerance (GSVD me) {
	return my tolerance;
}

#undef MAX
#undef MIN

/* End of file SVD.c */
