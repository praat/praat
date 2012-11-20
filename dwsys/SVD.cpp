/* SVD.cpp
 *
 * Copyright (C) 1994-2011 David Weenink
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
	MelderInfo_writeLine (L"Number of rows: ", Melder_integer (numberOfRows));
	MelderInfo_writeLine (L"Number of columns: ", Melder_integer (numberOfColumns));
}

Thing_implement (SVD, Data, 0);

static void NUMtranspose_d (double **m, long n);

/* if A=UDV' then A' = (UDV')'=VDU' */
static void SVD_transpose (SVD me) {
	long tmpl = my numberOfRows;
	double **tmpd = my u;

	my u = my v;
	my v = tmpd;
	my numberOfRows = my numberOfColumns;
	my numberOfColumns = tmpl;
}

/*
	m >=n, mxn matrix A has svd UDV', where u is mxn, D is n and V is nxn.
	m < n, mxn matrix A. Consider A' with svd (UDV')'= VDU', where v is mxm, D is m and U' is mxn
*/
void SVD_init (I, long numberOfRows, long numberOfColumns) {
	iam (SVD);
	long mn_min = MIN (numberOfRows, numberOfColumns);
	my numberOfRows = numberOfRows;
	my numberOfColumns = numberOfColumns;
	if (! NUMfpp) {
		NUMmachar ();
	}
	my tolerance = NUMfpp -> eps * MAX (numberOfRows, numberOfColumns);
	my u = NUMmatrix<double> (1, numberOfRows, 1, mn_min);
	my v = NUMmatrix<double> (1, numberOfColumns, 1, mn_min);
	my d = NUMvector<double> (1, mn_min);
}

SVD SVD_create (long numberOfRows, long numberOfColumns) {
	try {
		autoSVD me = Thing_new (SVD);
		SVD_init (me.peek(), numberOfRows, numberOfColumns);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("SVD not created.");
	}
}

SVD SVD_create_d (double **m, long numberOfRows, long numberOfColumns) {
	try {
		autoSVD me = SVD_create (numberOfRows, numberOfColumns);
		SVD_svd_d (me.peek(), m);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("SVD not created from vector.");
	}
}

SVD SVD_create_f (float **m, long numberOfRows, long numberOfColumns) {
	try {
		autoSVD me = SVD_create (numberOfRows, numberOfColumns);
		SVD_svd_f (me.peek(), m);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("SVD not created from vector.");
	}
}

void SVD_svd_d (SVD me, double **m) {
	if (my numberOfRows >= my numberOfColumns) {
		/* Store m in u */
		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				my u[i][j] = m[i][j];
			}
		}
	} else {
		/* Store m transposed in v */
		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				my v[j][i] = m[i][j];
			}
		}
	}
	SVD_compute (me);
}

void SVD_svd_f (SVD me, float **m) {
	if (my numberOfRows >= my numberOfColumns) {
		/* Store in u */
		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				my u[j][i] = m[i][j];
			}
		}
	} else {
		/* Store transposed in v */
		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				my v[i][j] = m[j][i];
			}
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

static void NUMtranspose_d (double **m, long n) {
	for (long i = 1; i <= n - 1; i++) {
		for (long j = i + 1; j <= n; j++) {
			double t = m[i][j];
			m[i][j] = m[j][i];
			m[j][i] = t;
		}
	}
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
	int NUMlapack_dgesvd (char *jobu, char *jobvt, long *m, long *n, double *a, long *lda,
		double *s, double *u, long *ldu, double *vt, long *ldvt, double *work,
		long *lwork, long *info);
*/
void SVD_compute (SVD me) {
	try {
		char jobu = 'S', jobvt = 'O';
		long m, lda, ldu, ldvt, info, lwork = -1;
		double wt[2];
		int transpose = my numberOfRows < my numberOfColumns;

		/* transpose: if rows < cols then data in v */
		if (transpose) {
			SVD_transpose (me);
		}

		lda = ldu = ldvt = m = my numberOfColumns;
		long n = my numberOfRows;

		(void) NUMlapack_dgesvd (&jobu, &jobvt, &m, &n, &my u[1][1], &lda, &my d[1], &my v[1][1], &ldu,
		                         NULL, &ldvt, wt, &lwork, &info);

		if (info != 0) {
			Melder_throw ("SVD not precomputed.");
		}

		lwork = wt[0];
		autoNUMvector<double> work (0L, lwork);
		(void) NUMlapack_dgesvd (&jobu, &jobvt, &m, &n, &my u[1][1], &lda, &my d[1], &my v[1][1], &ldu,
		                         NULL, &ldvt, work.peek(), &lwork, &info);
		if (info != 0) {
			Melder_throw ("SVD not computed.");
		}

		NUMtranspose_d (my v, MIN (m, n));
		if (transpose) {
			SVD_transpose (me);
		}
	} catch (MelderError) {
		Melder_throw (me, ": SVD could not be computed.");
	}
}

// V D^2 V'or V D^-2 V
void SVD_getSquared (SVD me, double **m, bool inverse) {
	if (inverse) {
		for (long i = 1; i <= my numberOfColumns; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				double val = 0;
				for (long k = 1; k <= my numberOfColumns; k++) {
					if (my d[k] > 0) {
						val += my v[i][k] * my v[j][k] / (my d[k] * my d[k]);
					}
				}
				m[i][j] = val;
			}
		}
	} else {
		for (long i = 1; i <= my numberOfColumns; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				double val = 0;
				for (long k = 1; k <= my numberOfColumns; k++) {
					val += my d[k] * my d[k] * my v[i][k] * my v[j][k];
				}
				m[i][j] = val;
			}
		}
	}
}

void SVD_solve (SVD me, double b[], double x[]) {
	try {
		long mn_min = MIN (my numberOfRows, my numberOfColumns);

		autoNUMvector<double> t (1, mn_min);

		/*  Solve UDV' x = b.
			Solution: x = V D^-1 U' b */

		for (long j = 1; j <= mn_min; j++) {
			double tmp = 0;
			if (my d[j] > 0) {
				for (long i = 1; i <= my numberOfRows; i++) {
					tmp += my u[i][j] * b[i];
				}
				tmp /= my d[j];
			}
			t[j] = tmp;
		}

		for (long j = 1; j <= my numberOfColumns; j++) {
			double tmp = 0;
			for (long i = 1; i <= mn_min; i++) {
				tmp += my v[j][i] * t[i];
			}
			x[j] = tmp;
		}
	} catch (MelderError) {
		Melder_throw (me, ": not solved.");
	}
}

void SVD_sort (SVD me) {
	try {
		long mn_min = MIN (my numberOfRows, my numberOfColumns);
		autoSVD thee = Data_copy (me);
		autoNUMvector<long> index (1, mn_min);

		NUMindexx (my d, mn_min, index.peek());

		for (long j = 1; j <= mn_min; j++) {
			long from = index[mn_min - j + 1];
			my d[j] = thy d[from];
			for (long i = 1; i <= my numberOfRows; i++) {
				my u[i][j] = thy u[i][from];
			}
			for (long i = 1; i <= my numberOfColumns; i++) {
				my v[i][j] = thy v[i][from];
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": not sorted.");
	}
}

long SVD_zeroSmallSingularValues (SVD me, double tolerance) {
	long numberOfZeroed = 0, mn_min = MIN (my numberOfRows, my numberOfColumns);
	double dmax = my d[1];

	if (tolerance == 0) {
		tolerance = my tolerance;
	}
	for (long i = 2; i <= mn_min; i++) {
		if (my d[i] > dmax) {
			dmax = my d[i];
		}
	}
	for (long i = 1; i <= mn_min; i++) {
		if (my d[i] < dmax * tolerance) {
			my d[i] = 0; numberOfZeroed++;
		}
	}
	return numberOfZeroed;
}


long SVD_getRank (SVD me) {
	long rank = 0, mn_min = MIN (my numberOfRows, my numberOfColumns);
	for (long i = 1; i <= mn_min; i++) {
		if (my d[i] > 0) {
			rank++;
		}
	}
	return rank;
}

/*
	SVD of A = U D V'.
	If u[i] is the i-th column vector of U and v[i] the i-th column vector of V and s[i] the i-th singular value,
	we can write the svd expansion  A = sum_{i=1}^n {d[i] u[i] v[i]'}.
	Golub & van Loan, 3rd ed, p 71.
*/
void SVD_synthesize (SVD me, long sv_from, long sv_to, double **m) {
	try {
		long mn_min = MIN (my numberOfRows, my numberOfColumns);

		if (sv_to == 0) {
			sv_to = mn_min;
		}

		if (sv_from > sv_to || sv_from < 1 || sv_to > mn_min) {
			Melder_throw ("Indices must be in range [1, ", mn_min, "].");
		}

		for (long i = 1; i <= my numberOfRows; i++) {
			for (long j = 1; j <= my numberOfColumns; j++) {
				m[i][j] = 0;
			}
		}

		for (long k = sv_from; k <= sv_to; k++) {
			for (long i = 1; i <= my numberOfRows; i++) {
				for (long j = 1; j <= my numberOfColumns; j++) {
					m[i][j] += my d[k] * my u[i][k] * my v[j][k];
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": no synthesis.");
	}
}

Thing_implement (GSVD, Data, 0);

void structGSVD :: v_info () {
	MelderInfo_writeLine (L"Number of columns: ", Melder_integer (numberOfColumns));
}

GSVD GSVD_create (long numberOfColumns) {
	try {
		autoGSVD me = Thing_new (GSVD);
		my numberOfColumns = numberOfColumns;

		my q = NUMmatrix<double> (1, numberOfColumns, 1, numberOfColumns);
		my r = NUMmatrix<double> (1, numberOfColumns, 1, numberOfColumns);
		my d1 = NUMvector<double> (1, numberOfColumns);
		my d2 = NUMvector<double> (1, numberOfColumns);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("GSVD not created.");
	}
}

GSVD GSVD_create_d (double **m1, long numberOfRows1, long numberOfColumns, double **m2, long numberOfRows2) {
	try {
		long m = numberOfRows1, n = numberOfColumns, p = numberOfRows2;
		long lwork = MAX (MAX (3 * n, m), p) + n;

		// Store the matrices a and b as column major!
		autoNUMmatrix<double> a (NUMmatrix_transpose (m1, m, n), 1, 1);
		autoNUMmatrix<double> b (NUMmatrix_transpose (m2, p, n), 1, 1);
		autoNUMmatrix<double> q (1, n, 1, n);
		autoNUMvector<double> alpha (1, n);
		autoNUMvector<double> beta (1, n);
		autoNUMvector<double> work (1, lwork);
		autoNUMvector<long> iwork (1, n);


		char jobu1 = 'N', jobu2 = 'N', jobq = 'Q';
		long k, l, info;
		NUMlapack_dggsvd (&jobu1, &jobu2, &jobq, &m, &n, &p, &k, &l,
		    &a[1][1], &m, &b[1][1], &p, &alpha[1], &beta[1], NULL, &m,
		    NULL, &p, &q[1][1], &n, &work[1], &iwork[1], &info);
		if (info != 0) {
			Melder_throw ("dggsvd failed, error = ", info);
		}


		long kl = k + l;
		autoGSVD me = GSVD_create (kl);

		for (long i = 1; i <= kl; i++) {
			my d1[i] = alpha[i];
			my d2[i] = beta[i];
		}

		// Transpose q

		for (long i = 1; i <= n; i++) {
			for (long j = i + 1; j <= n; j++) {
				my q[i][j] = q[j][i];
				my q[j][i] = q[i][j];
			}
			my q[i][i] = q[i][i];
		}

		// Get R from a(1:k+l,n-k-l+1:n)

		double *pr = &a[1][1];
		for (long i = 1; i <= kl; i++) {
			for (long j = i; j <= kl; j++) {
				my r[i][j] = pr[i - 1 + (n - kl + j - 1) * m]; /* from col-major */
			}
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("GSVD not created.");
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
