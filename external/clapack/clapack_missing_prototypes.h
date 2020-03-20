#ifndef _clapack_missing_prototypes_h_
#define _clapack_missing_prototypes_h_
/* clapack_missing_prototypes.h
 *
 * Copyright (C) 2020 David Weenink
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

/* Missing if you want to compile cblas with g++ */

doublereal dasum_(integer *n, doublereal *dx, integer *incx);

int daxpy_(integer *n, doublereal *da, doublereal *dx, 
	integer *incx, doublereal *dy, integer *incy);

int dcopy_(integer *n, doublereal *dx, integer *incx, 
	doublereal *dy, integer *incy);

doublereal ddot_(integer *n, doublereal *dx, integer *incx, doublereal *dy, 
	integer *incy);

int dgbmv_(char *trans, integer *m, integer *n, integer *kl, 
	integer *ku, doublereal *alpha, doublereal *a, integer *lda, 
	doublereal *x, integer *incx, doublereal *beta, doublereal *y, 
	integer *incy);

int dgemm_(const char *transa, const char *transb, integer *m, integer *
	n, integer *k, doublereal *alpha, doublereal *a, integer *lda, 
	doublereal *b, integer *ldb, doublereal *beta, doublereal *c__, 
	integer *ldc);

int dgemv_(const char *trans, integer *m, integer *n, doublereal *
	alpha, doublereal *a, integer *lda, doublereal *x, integer *incx, 
	doublereal *beta, doublereal *y, integer *incy);

int dger_(integer *m, integer *n, doublereal *alpha, 
	doublereal *x, integer *incx, doublereal *y, integer *incy, 
	doublereal *a, integer *lda);

logical disnan_(doublereal *din);

logical dlaisnan_(doublereal *din1, doublereal *din2);

doublereal dlamch_(const char *cmach);

int dlamc1_(integer *beta, integer *t, logical *rnd, logical *ieee1);

int dlamc2_(integer *beta, integer *t, logical *rnd, 
	doublereal *eps, integer *emin, doublereal *rmin, integer *emax, 
	doublereal *rmax);

doublereal dlamc3_(doublereal *a, doublereal *b);

int dlamc4_(integer *emin, doublereal *start, integer *base);

int dlamc5_(integer *beta, integer *p, integer *emin, 
	logical *ieee, integer *emax, doublereal *rmax);

integer dlaneg_(integer *n, doublereal *d__, doublereal *lld, doublereal *
	sigma, doublereal *pivmin, integer *r__);

doublereal dlangb_(const char *norm, integer *n, integer *kl, integer *ku, 
	doublereal *ab, integer *ldab, doublereal *work);

doublereal dlange_(const char *norm, integer *m, integer *n, doublereal *a, integer 
	*lda, doublereal *work);

doublereal dlangt_(const char *norm, integer *n, doublereal *dl, doublereal *d__, 
	doublereal *du);

doublereal dlanhs_(const char *norm, integer *n, doublereal *a, integer *lda, 
	doublereal *work);

doublereal dlansb_(const char *norm, const char *uplo, integer *n, integer *k, doublereal 
	*ab, integer *ldab, doublereal *work);

doublereal dlansp_(const char *norm, const char *uplo, integer *n, doublereal *ap, 
	doublereal *work);

doublereal dlanst_(const char *norm, integer *n, doublereal *d__, doublereal *e);

doublereal dlansy_(const char *norm, const char *uplo, integer *n, doublereal *a, integer 
	*lda, doublereal *work);

doublereal dlantb_(const char *norm, const char *uplo, const char *diag, integer *n, integer *k, 
	 doublereal *ab, integer *ldab, doublereal *work);

doublereal dlantp_(const char *norm, const char *uplo, const char *diag, integer *n, doublereal 
	*ap, doublereal *work);

doublereal dlantr_(const char *norm, const char *uplo, const char *diag, integer *m, integer *n, 
	 doublereal *a, integer *lda, doublereal *work);

doublereal dlapy2_(doublereal *x, doublereal *y);

doublereal dlapy3_(doublereal *x, doublereal *y, doublereal *z__);

doublereal dnrm2_(integer *n, doublereal *x, integer *incx);

int drotg_(doublereal *da, doublereal *db, doublereal *c__, 
	doublereal *s);

int drotmg_(doublereal *dd1, doublereal *dd2, doublereal *
	dx1, doublereal *dy1, doublereal *dparam);

int drotm_(integer *n, doublereal *dx, integer *incx, 
	doublereal *dy, integer *incy, doublereal *dparam);

int drot_(integer *n, doublereal *dx, integer *incx, 
	doublereal *dy, integer *incy, doublereal *c__, doublereal *s);

int dsbmv_(const char *uplo, integer *n, integer *k, doublereal *
	alpha, doublereal *a, integer *lda, doublereal *x, integer *incx, 
	doublereal *beta, doublereal *y, integer *incy);

int dscal_(integer *n, doublereal *da, doublereal *dx, 
	integer *incx);

doublereal dsdot_(integer *n, real *sx, integer *incx, real *sy, integer *incy);

int dspmv_(const char *uplo, integer *n, doublereal *alpha, 
	doublereal *ap, doublereal *x, integer *incx, doublereal *beta, 
	doublereal *y, integer *incy);

int dspr2_(const char *uplo, integer *n, doublereal *alpha, 
	doublereal *x, integer *incx, doublereal *y, integer *incy, 
	doublereal *ap);

int dspr_(const char *uplo, integer *n, doublereal *alpha, 
	doublereal *x, integer *incx, doublereal *ap);

int dswap_(integer *n, doublereal *dx, integer *incx, 
	doublereal *dy, integer *incy);

int dsyrk_(const char *uplo, const char *trans, integer *n, integer *k, 
	doublereal *alpha, doublereal *a, integer *lda, doublereal *beta, 
	doublereal *c__, integer *ldc);

int dsymm_(const char *side, const char *uplo, integer *m, integer *n, 
	doublereal *alpha, doublereal *a, integer *lda, doublereal *b, 
	integer *ldb, doublereal *beta, doublereal *c__, integer *ldc);

int dsymv_(const char *uplo, integer *n, doublereal *alpha, 
	doublereal *a, integer *lda, doublereal *x, integer *incx, doublereal 
	*beta, doublereal *y, integer *incy);

int dsyr_(const char *uplo, integer *n, doublereal *alpha, 
	doublereal *x, integer *incx, doublereal *a, integer *lda);

int dsyr2_(const char *uplo, integer *n, doublereal *alpha, 
	doublereal *x, integer *incx, doublereal *y, integer *incy, 
	doublereal *a, integer *lda);

int dsyr2k_(const char *uplo, const char *trans, integer *n, integer *k, 
	doublereal *alpha, doublereal *a, integer *lda, doublereal *b, 
	integer *ldb, doublereal *beta, doublereal *c__, integer *ldc);

int dtbmv_(const char *uplo, const char *trans, const char *diag, integer *n, 
	integer *k, doublereal *a, integer *lda, doublereal *x, integer *incx);

int dtbsv_(const char *uplo, const char *trans, const char *diag, integer *n, 
	integer *k, doublereal *a, integer *lda, doublereal *x, integer *incx);

int dtpmv_(const char *uplo, const char *trans, const char *diag, integer *n, 
	doublereal *ap, doublereal *x, integer *incx);

int dtpsv_(const char *uplo, const char *trans, const char *diag, integer *n, 
	doublereal *ap, doublereal *x, integer *incx);

int dtrmv_(const char *uplo, const char *trans, const char *diag, integer *n, 
	doublereal *a, integer *lda, doublereal *x, integer *incx);

int dtrmm_(const char *side, const char *uplo, const char *transa, const char *diag, 
	integer *m, integer *n, doublereal *alpha, doublereal *a, integer *
	lda, doublereal *b, integer *ldb);

int dtrsm_(const char *side, const char *uplo, const char *transa, const char *diag, 
	integer *m, integer *n, doublereal *alpha, doublereal *a, integer *
	lda, doublereal *b, integer *ldb);

int dtrsv_(const char *uplo, const char *trans, const char *diag, integer *n, 
	doublereal *a, integer *lda, doublereal *x, integer *incx);

integer i_dnnt(double *);

inline integer i_nint (float *x) {
	return (integer)(*x >= 0.0 ? floor(*x + 0.5) : -floor(0.5 - *x));
}
inline double pow_dd (double *a, double *b) {
	return pow (*a, *b);
}

static inline double d_abs (double *x) {
	return abs (*x);
}
static inline double d_acos (double *x) {
	return acos (*x);
}
static inline double d_asin (double *x) {
	return asin (*x);
}
static inline double d_atan (double *x) {
	return atan (*x);
}
static inline double d_atn2 (double *x, double *y) {
	return atan2 (*x,*y);
}
static inline void d_cnjg (doublecomplex *r, doublecomplex *z) {
	double zi = z->i;
	r->r = z->r;
	r->i = -zi;
}
static inline double d_cos (double *x) {
	return cos (*x);
}
static inline double d_cosh (double *x) {
	return cosh (*x);
}
static inline double d_dim (double *a, double *b) {
	return ( *a > *b ? *a - *b : 0 );
}
static inline double d_exp (double *x) {
	return exp (*x);
}
static inline double d_imag (doublecomplex *z) {
	return z->i;
}
static inline double d_int (double *x) {
	return ( *x>0 ? floor(*x) : -floor(- *x) );
}
static inline double d_lg10 (double *x) {
	return log10 (*x);
}
static inline double d_log (double *x) {
	return log (*x);
}
// TODO djmw is this std::fmod (*x, *y) ??
static inline double d_mod (double *x, double *y) {
#ifdef IEEE_drem
	double ya = *y;
	if (ya < 0.0)
		ya = -ya;
	double xa = *x;
	double z = drem (xa, ya);
	if (xa > 0) {
		if (z < 0)
			z += ya;
		}
	else if (z > 0)
		z -= ya;
	return z;
#else
	double quotient = *x / *y;
	if (quotient >= 0)
		quotient = floor (quotient);
	else
		quotient = -floor(-quotient);
	return (*x - (*y) * quotient );
#endif
}

static inline double d_prod (real *x, real *y) {
	return  (*x) * (*y);
}
static inline double d_sign (double *a, double *b) {
	double x = (*a >= 0 ? *a : - *a);
	return ( *b >= 0 ? x : -x );
}
static inline double d_sin (double *x) {
	return sin (*x);
}
static inline double d_sinh (double *x) {
	return sinh (*x);
}
static inline double d_sqrt (double *x) {
	return sqrt (*x);
}
static inline double d_tan (double *x) {
	return tan (*x);
}
static inline double d_tanh (double *x) {
	return tanh (*x);
}

double pow_di(double *, integer *);
integer pow_ii(integer *, integer *);
double pow_ri (real *, integer *);
void s_cat(char *, char **, integer *, integer *, integer);
integer s_cmp (const char *, const char *, integer, integer);
void s_copy(char *, const char *, integer, integer);
double slamch_(const char *cmach);

/* Missing if you want to compile clapack with g++ */

integer icmax1_(integer *n, complex *cx, integer *incx);

integer idamax_(integer *n, doublereal *dx, integer *incx);

integer ieeeck_(integer *ispec, real *zero, real *one);

integer ilaenv_(integer *ispec, const char *name__, const char *opts, integer *n1, 
	integer *n2, integer *n3, integer *n4);

integer iparmq_(integer *ispec, const char *name__, const char *opts, integer *n, integer 
	*ilo, integer *ihi, integer *lwork);

logical lsame_(const char *ca, const char *cb);

logical lsamen_(integer *n, const char *ca, const char *cb);

#endif _/* clapack_missing_prototypes_h_ */
