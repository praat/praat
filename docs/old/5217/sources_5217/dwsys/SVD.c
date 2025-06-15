/* SVD.c
 *
 * Copyright (C) 1994-2008 David Weenink
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

extern machar_Table NUMfpp;
static void NUMtranspose_d (double **m, long n);

/* if A=UDV' then A' = (UDV')'=VDU' */
static void SVD_transpose (I)
{
	iam (SVD);
	long tmpl = my numberOfRows;
	double **tmpd = my u;

	my u = my v;
	my v = tmpd;
	my numberOfRows = my numberOfColumns;
	my numberOfColumns = tmpl;
}

static void classSVD_info (I)
{
	iam (SVD);
	MelderInfo_writeLine2 (L"Number of rows: ", Melder_integer (my numberOfRows));
	MelderInfo_writeLine2 (L"Number of columns: ", Melder_integer (my numberOfColumns));
}

class_methods (SVD, Data)
	class_method_local (SVD, destroy)
	class_method_local (SVD, equal)
	class_method_local (SVD, canWriteAsEncoding)
	class_method_local (SVD, copy)
	class_method_local (SVD, readText)
	class_method_local (SVD, readBinary)
	class_method_local (SVD, writeText)
	class_method_local (SVD, writeBinary)
	class_method_local (SVD, description)
	class_method_local (SVD, info)
class_methods_end

/*
	m >=n, mxn matrix A has svd UDV', where u is mxn, D is n and V is nxn.
	m < n, mxn matrix A. Consider A' with svd (UDV')'= VDU', where v is mxm, D is m and U' is mxn
*/
int SVD_init (I, long numberOfRows, long numberOfColumns)
{
	iam (SVD);
	long mn_min = MIN(numberOfRows, numberOfColumns);
	my numberOfRows = numberOfRows;
	my numberOfColumns = numberOfColumns;
	if (! NUMfpp) NUMmachar ();
	my tolerance = NUMfpp -> eps * MAX (numberOfRows, numberOfColumns);
	if (((my u = NUMdmatrix (1, numberOfRows, 1, mn_min)) == NULL) ||
		((my v = NUMdmatrix (1, numberOfColumns, 1, mn_min)) == NULL) ||
		((my d = NUMdvector (1, mn_min)) == NULL)) return 0;
	return 1;
}

SVD SVD_create (long numberOfRows, long numberOfColumns)
{
	SVD me = new (SVD);
	
	if (me == NULL || ! SVD_init (me, numberOfRows, numberOfColumns)) forget (me);
	return me;
}

SVD SVD_create_d (double **m, long numberOfRows, long numberOfColumns)
{
	SVD me = SVD_create (numberOfRows, numberOfColumns);
	
	if ((me == NULL) || ! SVD_svd_d (me, m)) forget (me);
	return me;
}

SVD SVD_create_f (float **m, long numberOfRows, long numberOfColumns)
{
	SVD me = SVD_create (numberOfRows, numberOfColumns);
	 
	if ((me == NULL) || ! SVD_svd_f (me, m)) forget (me);
	return me;
}

int SVD_svd_d (I, double **m)
{
	iam (SVD);
	long i, j;
	if (my numberOfRows >= my numberOfColumns)
	{
		/* Store m in u */
		for (i = 1; i <= my numberOfRows; i++ )
		{
			for (j = 1; j <= my numberOfColumns; j++) my u[i][j] = m[i][j];
		}
	}
	else
	{
		/* Store m transposed in v */
		for (i = 1; i <= my numberOfRows; i++ )
		{
			for (j = 1; j <= my numberOfColumns; j++) my v[j][i] = m[i][j];
		}
	}
	return SVD_compute (me);
}

int SVD_svd_f (I, float **m)
{
	iam (SVD);
	long i, j;
	if (my numberOfRows >= my numberOfColumns)
	{
		/* Store in u */
		for (i = 1; i <= my numberOfRows; i++ )
		{
			for (j = 1; j <= my numberOfColumns; j++) my u[j][i] = m[i][j];
		}
	}
	else
	{
		/* Store transposed in v */
		for (i = 1; i <= my numberOfRows; i++ )
		{
			for (j = 1; j <= my numberOfColumns; j++) my v[i][j] = m[j][i];
		}
	}
	
	return SVD_compute (me);
}

void SVD_setTolerance (I, double tolerance)
{
	iam (SVD);
	my tolerance = tolerance;
}

double SVD_getTolerance (I)
{
	iam (SVD);
	return my tolerance;
}

static void NUMtranspose_d (double **m, long n)
{
	long i, j;
	for (i = 1; i <= n - 1; i++)
	{
		for (j = i + 1; j <= n; j++)
		{
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
int SVD_compute (I)
{
	iam (SVD);
	char jobu = 'S', jobvt = 'O';
	long m, n, lda, ldu, ldvt, info, lwork = -1;
	double *work = NULL, wt[2];
	int transpose = my numberOfRows < my numberOfColumns;

	/* transpose: if rows < cols then data in v */
	if (transpose) SVD_transpose (me);
	
	lda = ldu = ldvt = m = my numberOfColumns;
	n = my numberOfRows;
	
	(void) NUMlapack_dgesvd (&jobu, &jobvt, &m, &n, &my u[1][1], &lda, &my d[1], &my v[1][1], &ldu,
		NULL, &ldvt, wt, &lwork, &info);

	if (info != 0) goto end;

	lwork = wt[0];
	work = NUMdvector (1, lwork);
	if (work == NULL) goto end;
	(void) NUMlapack_dgesvd(&jobu, &jobvt, &m, &n, &my u[1][1], &lda, &my d[1], &my v[1][1], &ldu,
		NULL, &ldvt, &work[1], &lwork, &info);

	NUMtranspose_d (my v, MIN(m, n));

	NUMdvector_free (work, 1);
end:
	if (transpose) SVD_transpose (me);
	return info == 0;
}


int SVD_solve (I, double b[], double x[])
{
	iam (SVD);
	double *t, tmp;
	long i, j, mn_min = MIN (my numberOfRows, my numberOfColumns);

	t = NUMdvector (1, mn_min);
	if (t == NULL) return 0;
	
	/*  Solve UDV' x = b.
		Solution: x = V D^-1 U' b */

	for (j = 1; j <= mn_min; j++)
	{
		tmp = 0;
		if (my d[j] > 0)
		{
			for (i = 1; i <= my numberOfRows; i++)
			{
				tmp += my u[i][j] * b[i];
			}
			tmp /= my d[j];
		}
		t[j] = tmp;
	}

	for (j = 1; j <= my numberOfColumns; j++)
	{
		tmp = 0;
		for (i = 1; i <= mn_min; i++)
		{
			tmp += my v[j][i] * t[i];
		}
		x[j] = tmp;
	}
	
	NUMdvector_free (t, 1);
	return 1;
}

int SVD_sort (I)
{
	iam (SVD); SVD thee = NULL; 
	long i, j, mn_min = MIN (my numberOfRows, my numberOfColumns);
	long *index = NULL;
	
	if (((thee = Data_copy (me)) == NULL) ||
		((index = NUMlvector (1, mn_min)) == NULL)) goto end;
	
	NUMindexx (my d, mn_min, index);
			
	for (j = 1; j <= mn_min; j++)
	{
		long from = index[mn_min - j + 1];
		my d[j] = thy d[from];
		for (i = 1; i <= my numberOfRows; i++) my u[i][j] = thy u[i][from];
		for (i = 1; i <= my numberOfColumns; i++) my v[i][j] = thy v[i][from];
	}
end:
	forget (thee);
	NUMlvector_free (index, 1);
	return ! Melder_hasError ();
}

long SVD_zeroSmallSingularValues (I, double tolerance)
{
	iam (SVD);
	long i, numberOfZeroed = 0, mn_min = MIN (my numberOfRows, my numberOfColumns);
	double dmax = my d[1];
	
	if (tolerance == 0) tolerance = my tolerance;
	for (i = 2; i <= mn_min; i++)
	{
		if (my d[i] > dmax) dmax = my d[i];
	}
	for (i = 1; i <= mn_min; i++)
	{
		if (my d[i] < dmax * tolerance)
		{
			my d[i] = 0; numberOfZeroed++;
		}
	}
	return numberOfZeroed;
}


long SVD_getRank (I)
{
	iam (SVD);
	long i, rank = 0, mn_min = MIN (my numberOfRows, my numberOfColumns);
	for (i = 1; i <= mn_min; i++)
	{
		if (my d[i] > 0) rank++;
	}
	return rank;
}

/*
	SVD of A = U D V'.
	If u[i] is the i-th column vector of U and v[i] the i-th column vector of V and s[i] the i-th singular value,
	we can write the svd expansion  A = sum_{i=1}^n {d[i] u[i] v[i]'}.
	Golub & van Loan, 3rd ed, p 71.
*/
int SVD_synthesize (I, long sv_from, long sv_to, double **m)
{
	iam (SVD);
	long i, j, k, mn_min = MIN (my numberOfRows, my numberOfColumns);

	if (sv_to == 0) sv_to = mn_min;

	if (sv_from > sv_to || sv_from < 1 || sv_to > mn_min) return
		Melder_error3 (L"SVD_synthesize: indices must be in range [1, ", Melder_integer (mn_min), L"].");

	for (i = 1; i <= my numberOfRows; i++)
	{
		for (j = 1; j <= my numberOfColumns; j++) m[i][j] = 0;
	}

	for (k = sv_from; k <= sv_to; k++)
	{
		for (i = 1; i <= my numberOfRows; i++)
		{
			for (j = 1; j <= my numberOfColumns; j++)
			{
				m[i][j] += my d[k] * my u[i][k] * my v[j][k];
			}
		}
	}
	return 1;
}

static void classGSVD_info (I)
{
	iam (GSVD);
	MelderInfo_writeLine2 (L"Number of columns: ", Melder_integer (my numberOfColumns));
}

class_methods (GSVD, Data)
	class_method_local (GSVD, destroy)
	class_method_local (GSVD, equal)
	class_method_local (GSVD, canWriteAsEncoding)
	class_method_local (GSVD, copy)
	class_method_local (GSVD, readText)
	class_method_local (GSVD, readBinary)
	class_method_local (GSVD, writeText)
	class_method_local (GSVD, writeBinary)
	class_method_local (GSVD, description)
	class_method_local (GSVD, info)
class_methods_end

GSVD GSVD_create (long numberOfColumns)
{
	GSVD me = new (GSVD);

	if (me == NULL) return NULL;

	my numberOfColumns = numberOfColumns;

	if (((my q = NUMdmatrix(1, numberOfColumns, 1, numberOfColumns)) == NULL) ||
		((my r = NUMdmatrix(1, numberOfColumns, 1, numberOfColumns)) == NULL) ||
		((my d1 = NUMdvector (1, numberOfColumns)) == NULL) ||
		((my d2 = NUMdvector (1, numberOfColumns)) == NULL)) forget (me);
	return me;
}

GSVD GSVD_create_d (double **m1, long numberOfRows1, long numberOfColumns,
	double **m2, long numberOfRows2)
{
	long m = numberOfRows1, n = numberOfColumns, p = numberOfRows2, kl;
	long i, j, k, l, lwork = MAX (MAX (3*n, m), p) + n, *iwork = NULL, info;
	double **a = NULL, **b = NULL, *work = NULL, *pr;
	double *alpha = NULL, *beta = NULL, **q = NULL;
	GSVD me = NULL;

	/*
		Store the matrices a and b as column major!
	*/
	if (((a = NUMdmatrix_transpose (m1, m, n)) == NULL) ||
		((b = NUMdmatrix_transpose (m2, p, n)) == NULL) ||
		((alpha = NUMdvector (1, n)) == NULL) ||
		((beta = NUMdvector (1, n)) == NULL) ||
		((q = NUMdmatrix (1, n, 1, n)) == NULL) ||
		((work = NUMdvector (1, lwork)) == NULL) ||
		((iwork = NUMlvector (1, n)) == NULL)) goto end;

	{
		char jobu1 = 'N', jobu2 = 'N', jobq = 'Q';
		(void) NUMlapack_dggsvd (&jobu1, &jobu2, &jobq, &m, &n, &p, &k, &l,
			&a[1][1], &m, &b[1][1], &p, &alpha[1], &beta[1], NULL, &m,
			NULL, &p, &q[1][1], &n, &work[1], &iwork[1], &info);
		if (info != 0)
		{
			(void) Melder_error2 (L"Error info = ", Melder_integer (info));
			goto end;
		}
	}

	kl = k + l;
	me = GSVD_create (kl);
	if (me == NULL) return NULL;
	
	for (i = 1; i <= kl; i++)
	{
		my d1[i] = alpha[i];
		my d2[i] = beta[i];
	}

	/*
		Transpose q
	*/
	for (i = 1; i <= n; i++)
	{
		for (j = i + 1; j <= n; j++)
		{
			my q[i][j] = q[j][i];
			my q[j][i] = q[i][j];
		}
		my q[i][i] = q[i][i];
	}
	/*
		Get R from a(1:k+l,n-k-l+1:n)
	*/
	pr = &a[1][1];
	for (i = 1; i <= kl; i++)
	{
		for (j = i; j <= kl; j++)
		{
			my r[i][j] = pr[i - 1 + (n - kl + j - 1) * m]; /* from col-major */
		}
	}
end:
	NUMdmatrix_free (q, 1, 1); NUMdvector_free (alpha,1);
	NUMdvector_free (beta,1); NUMlvector_free (iwork, 1);
	NUMdvector_free (work, 1); NUMdmatrix_free (b, 1, 1);
	NUMdmatrix_free (a, 1, 1);
	if (Melder_hasError ()) forget (me);
	return me;
}

void GSVD_setTolerance (GSVD me, double tolerance)
{
	my tolerance = tolerance;
}

double GSVD_getTolerance (GSVD me)
{
	return my tolerance;
}

#undef MAX
#undef MIN

/* End of file SVD.c */
