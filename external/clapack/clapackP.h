#ifndef _clapackP_h_
#define _clapackP_h_
/* clapackP.h
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

logical disnan_ (double *din);

logical dlaisnan_ (double *din1, double *din2);

integer dlaneg_ (integer *n, double *d__, double *lld, double *
	sigma, double *pivmin, integer *r__);

double dlangb_ (const char *norm, integer *n, integer *kl, integer *ku, 
	double *ab, integer *ldab, double *work);

double dlange_ (const char *norm, integer *m, integer *n, double *a, integer 
	*lda, double *work);

double dlangt_ (const char *norm, integer *n, double *dl, double *d__, 
	double *du);

double dlanhs_ (const char *norm, integer *n, double *a, integer *lda, 
	double *work);

double dlansb_ (const char *norm, const char *uplo, integer *n, integer *k, double 
	*ab, integer *ldab, double *work);

double dlansp_ (const char *norm, const char *uplo, integer *n, double *ap,
	double *work);

double dlanst_ (const char *norm, integer *n, double *d__, double *e);

double dlansy_ (const char *norm, const char *uplo, integer *n, double *a, integer
	*lda, double *work);

double dlantb_ (const char *norm, const char *uplo, const char *diag, integer *n, integer *k,
	 double *ab, integer *ldab, double *work);

double dlantp_ (const char *norm, const char *uplo, const char *diag, integer *n, double *ap, double *work);

double dlantr_ (const char *norm, const char *uplo, const char *diag, integer *m, integer *n,
	 double *a, integer *lda, double *work);

double dlapy2_ (double *x, double *y);

double dlapy3_ (double *x, double *y, double *z__);

double dnrm2_ (integer *n, double *x, integer *incx);

double slamch_ (const char *cmach);

int dlamc1_ (integer *beta, integer *t, logical *rnd, logical *ieee1);

int dlamc2_ (integer *beta, integer *t, logical *rnd,
	double *eps, integer *emin, double *rmin, integer *emax, double *rmax);

double dlamc3_ (double *a, double *b);

int dlamc4_ (integer *emin, double *start, integer *base);

int dlamc5_ (integer *beta, integer *p, integer *emin,
	logical *ieee, integer *emax, double *rmax);

integer icmax1_ (integer *n, complex *cx, integer *incx);

integer ieeeck_ (integer *ispec, real *zero, real *one);

integer ilaenv_ (integer *ispec, const char *name__, const char *opts, integer *n1,
	integer *n2, integer *n3, integer *n4);

integer iparmq_ (integer *ispec, const char *name__, const char *opts, integer *n, integer
	*ilo, integer *ihi, integer *lwork);

logical lsame_ (const char *ca, const char *cb);

logical lsamen_ (integer *n, const char *ca, const char *cb);

static inline void xerbla_ (const char *src, integer *info) {
	Melder_throw (Melder_peek8to32 (src), U": parameter ", *info, U" not correct!");
}

#endif /* clapackP_h_ */
