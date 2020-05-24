#ifndef _cblas_h_
#define _cblas_h_
/* cblas.h
 *
 * Copyright (C) 2020 David Weenink, Paul Boersma
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

#include "melder.h"   // for integer

double dasum_ (integer *n, double *dx, integer *incx);

int daxpy_ (integer *n, double *da, double *dx,
	integer *incx, double *dy, integer *incy);

int dcopy_ (integer *n, double *dx, integer *incx,
	double *dy, integer *incy);

double ddot_ (integer *n, double *dx, integer *incx, double *dy,
	integer *incy);

int dgbmv_ (const char *trans, integer *m, integer *n, integer *kl,
	integer *ku, double *alpha, double *a, integer *lda,
	double *x, integer *incx, double *beta, double *y,
	integer *incy);

int dgemm_ (const char *transa, const char *transb, integer *m, integer *
	n, integer *k, double *alpha, double *a, integer *lda,
	double *b, integer *ldb, double *beta, double *c__,
	integer *ldc);

int dgemv_ (const char *trans, integer *m, integer *n, double *
	alpha, double *a, integer *lda, double *x, integer *incx,
	double *beta, double *y, integer *incy);

int dger_ (integer *m, integer *n, double *alpha,
	double *x, integer *incx, double *y, integer *incy,
	double *a, integer *lda);

double dnrm2_ (integer *n, double *x, integer *incx);

int drot_ (integer *n, double *dx, integer *incx,
	double *dy, integer *incy, double *c__, double *s);

int drotg_ (double *da, double *db, double *c__, 
	double *s);

int drotmg_ (double *dd1, double *dd2, double *
	dx1, double *dy1, double *dparam);

int drotm_ (integer *n, double *dx, integer *incx, 
	double *dy, integer *incy, double *dparam);

int dsbmv_ (const char *uplo, integer *n, integer *k, double *
	alpha, double *a, integer *lda, double *x, integer *incx,
	double *beta, double *y, integer *incy);

int dscal_(integer *n, double *da, double *dx, integer *incx);

double dsdot_(integer *n, float *sx, integer *incx, float *sy, integer *incy);

int dspmv_(const char *uplo, integer *n, double *alpha,
	double *ap, double *x, integer *incx, double *beta,
	double *y, integer *incy);

int dspr_ (const char *uplo, integer *n, double *alpha,
	double *x, integer *incx, double *ap);

int dspr2_ (const char *uplo, integer *n, double *alpha,
	double *x, integer *incx, double *y, integer *incy,
	double *ap);

int dswap_ (integer *n, double *dx, integer *incx,
	double *dy, integer *incy);

int dsymm_ (const char *side, const char *uplo, integer *m, integer *n,
	double *alpha, double *a, integer *lda, double *b,
	integer *ldb, double *beta, double *c__, integer *ldc);

int dsymv_ (const char *uplo, integer *n, double *alpha,
	double *a, integer *lda, double *x, integer *incx, double 
	*beta, double *y, integer *incy);


int dsyr_ (const char *uplo, integer *n, double *alpha,
	double *x, integer *incx, double *a, integer *lda);

int dsyr2_ (const char *uplo, integer *n, double *alpha,
	double *x, integer *incx, double *y, integer *incy,
	double *a, integer *lda);

int dsyr2k_ (const char *uplo, const char *trans, integer *n, integer *k,
	double *alpha, double *a, integer *lda, double *b,
	integer *ldb, double *beta, double *c__, integer *ldc);

int dsyrk_ (const char *uplo, const char *trans, integer *n, integer *k,
	double *alpha, double *a, integer *lda, double *beta,
	double *c__, integer *ldc);

int dtbmv_ (const char *uplo, const char *trans, const char *diag, integer *n,
	integer *k, double *a, integer *lda, double *x, integer *incx);

int dtbsv_ (const char *uplo, const char *trans, const char *diag, integer *n,
	integer *k, double *a, integer *lda, double *x, integer *incx);

int dtpmv_ (const char *uplo, const char *trans, const char *diag, integer *n,
	double *ap, double *x, integer *incx);

int dtpsv_ (const char *uplo, const char *trans, const char *diag, integer *n,
	double *ap, double *x, integer *incx);

int dtrmm_ (const char *side, const char *uplo, const char *transa, const char *diag,
	integer *m, integer *n, double *alpha, double *a, integer *
	lda, double *b, integer *ldb);

int dtrmv_ (const char *uplo, const char *trans, const char *diag, integer *n,
	double *a, integer *lda, double *x, integer *incx);

int dtrsm_ (const char *side, const char *uplo, const char *transa, const char *diag,
	integer *m, integer *n, double *alpha, double *a, integer *
	lda, double *b, integer *ldb);

int dtrsv_ (const char *uplo, const char *trans, const char *diag, integer *n,
	double *a, integer *lda, double *x, integer *incx);

integer idamax_ (integer *n, double *dx, integer *incx);

#endif /* _cblas_h_  */
