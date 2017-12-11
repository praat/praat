/* NUMcblas.cpp

	-- LAPACK helper routines  -- Univ. of Tennessee, Univ. of
	California Berkeley, NAG Ltd., Courant Institute, Argonne National Lab,
	and Rice University October 31, 1999 -- translated by f2c (version
	19990503)

	Adapted by David Weenink 20021201
*/

/*
 djmw 20020813 GPL header
 djmw 20071201 Latest modification
 pb 20100120 dlamc3_: declare volatile double ret_val to prevent optimization!
*/


/* #include "blaswrap.h" */
#include "melder.h"
#include "NUMcblas.h"
#include "NUMf2c.h"
#include "NUM2.h"

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

static int dlamc1_ (integer *beta, integer *t, integer *rnd, integer *ieee1);
static int dlamc2_ (integer *beta, integer *t, integer *rnd, double *eps, integer *emin, double *rmin, integer *emax,
                    double *rmax);
static double dlamc3_ (double *, double *);
static int dlamc4_ (integer *emin, double *start, integer *base);
static int dlamc5_ (integer *beta, integer *p, integer *emin, integer *ieee, integer *emax, double *rmax);

int NUMblas_daxpy (integer *n, double *da, double *dx, integer *incx, double *dy, integer *incy) {
	/* System generated locals */
	integer i__1;

	/* Local variables */
	static integer i__, m, ix, iy, mp1;

	--dy;
	--dx;
	/* Function Body */
	if (*n <= 0) {
		return 0;
	}
	if (*da == 0.) {
		return 0;
	}
	if (*incx == 1 && *incy == 1) {
		goto L20;
	}
	/* code for unequal increments or equal increments not equal to 1 */
	ix = 1;
	iy = 1;
	if (*incx < 0) {
		ix = (- (*n) + 1) * *incx + 1;
	}
	if (*incy < 0) {
		iy = (- (*n) + 1) * *incy + 1;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		dy[iy] += *da * dx[ix];
		ix += *incx;
		iy += *incy;
		/* L10: */
	}
	return 0;
	/* code for both increments equal to 1 clean-up loop */
L20:
	m = *n % 4;
	if (m == 0) {
		goto L40;
	}
	i__1 = m;
	for (i__ = 1; i__ <= i__1; ++i__) {
		dy[i__] += *da * dx[i__];
		/* L30: */
	}
	if (*n < 4) {
		return 0;
	}
L40:
	mp1 = m + 1;
	i__1 = *n;
	for (i__ = mp1; i__ <= i__1; i__ += 4) {
		dy[i__] += *da * dx[i__];
		dy[i__ + 1] += *da * dx[i__ + 1];
		dy[i__ + 2] += *da * dx[i__ + 2];
		dy[i__ + 3] += *da * dx[i__ + 3];
		/* L50: */
	}
	return 0;
}								/* NUMblas_daxpy */

int NUMblas_dcopy (integer *n, double *dx, integer *incx, double *dy, integer *incy) {
	/* System generated locals */
	integer i__1;

	/* Local variables */
	static integer i__, m, ix, iy, mp1;

	--dy;
	--dx;
	/* Function Body */
	if (*n <= 0) {
		return 0;
	}
	if (*incx == 1 && *incy == 1) {
		goto L20;
	}
	/* code for unequal increments or equal increments not equal to 1 */
	ix = 1;
	iy = 1;
	if (*incx < 0) {
		ix = (- (*n) + 1) * *incx + 1;
	}
	if (*incy < 0) {
		iy = (- (*n) + 1) * *incy + 1;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		dy[iy] = dx[ix];
		ix += *incx;
		iy += *incy;
		/* L10: */
	}
	return 0;
	/* code for both increments equal to 1 clean-up loop */
L20:
	m = *n % 7;
	if (m == 0) {
		goto L40;
	}
	i__1 = m;
	for (i__ = 1; i__ <= i__1; ++i__) {
		dy[i__] = dx[i__];
		/* L30: */
	}
	if (*n < 7) {
		return 0;
	}
L40:
	mp1 = m + 1;
	i__1 = *n;
	for (i__ = mp1; i__ <= i__1; i__ += 7) {
		dy[i__] = dx[i__];
		dy[i__ + 1] = dx[i__ + 1];
		dy[i__ + 2] = dx[i__ + 2];
		dy[i__ + 3] = dx[i__ + 3];
		dy[i__ + 4] = dx[i__ + 4];
		dy[i__ + 5] = dx[i__ + 5];
		dy[i__ + 6] = dx[i__ + 6];
		/* L50: */
	}
	return 0;
}								/* NUMblas_dcopy */

double NUMblas_ddot (integer *n, double *dx, integer *incx, double *dy, integer *incy) {
	/* System generated locals */
	integer i__1;
	double ret_val;

	/* Local variables */
	static integer i__, m;
	static double dtemp;
	static integer ix, iy, mp1;

	/* Parameter adjustments */
	--dy;
	--dx;
	/* Function Body */
	ret_val = 0.;
	dtemp = 0.;
	if (*n <= 0) {
		return ret_val;
	}
	if (*incx == 1 && *incy == 1) {
		goto L20;
	}
	/* code for unequal increments or equal increments not equal to 1 */
	ix = 1;
	iy = 1;
	if (*incx < 0) {
		ix = (- (*n) + 1) * *incx + 1;
	}
	if (*incy < 0) {
		iy = (- (*n) + 1) * *incy + 1;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		dtemp += dx[ix] * dy[iy];
		ix += *incx;
		iy += *incy;
		/* L10: */
	}
	ret_val = dtemp;
	return ret_val;
	/* code for both increments equal to 1 clean-up loop */
L20:
	m = *n % 5;
	if (m == 0) {
		goto L40;
	}
	i__1 = m;
	for (i__ = 1; i__ <= i__1; ++i__) {
		dtemp += dx[i__] * dy[i__];
		/* L30: */
	}
	if (*n < 5) {
		goto L60;
	}
L40:
	mp1 = m + 1;
	i__1 = *n;
	for (i__ = mp1; i__ <= i__1; i__ += 5) {
		dtemp =
		    dtemp + dx[i__] * dy[i__] + dx[i__ + 1] * dy[i__ + 1] + dx[i__ + 2] * dy[i__ + 2] + dx[i__ +
		            3] * dy[i__ + 3] + dx[i__ + 4] * dy[i__ + 4];
		/* L50: */
	}
L60:
	ret_val = dtemp;
	return ret_val;
}								/* NUMblas_ddot */

int NUMblas_dgemm (const char *transa, const char *transb, integer *m, integer *n, integer *k, double *alpha, double *a, integer *lda,
                   double *b, integer *ldb, double *beta, double *c__, integer *ldc) {
	/* System generated locals */
	integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, i__1, i__2, i__3;

	/* Local variables */
	static integer info;
	static integer nota, notb;
	static double temp;
	static integer i__, j, l, ncola;
	static integer nrowa, nrowb;

#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]
	/*
	   Set NOTA and NOTB as true if A and B respectively are not transposed
	   and set NROWA, NCOLA and NROWB as the number of rows and columns of A
	   and the number of rows of B respectively. Parameter adjustments */
	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	/* Function Body */
	nota = lsame_ (transa, "N");
	notb = lsame_ (transb, "N");
	if (nota) {
		nrowa = *m;
		ncola = *k;
	} else {
		nrowa = *k;
		ncola = *m;
	}
	if (notb) {
		nrowb = *k;
	} else {
		nrowb = *n;
	}
	/* Test the input parameters. */
	info = 0;
	if (!nota && !lsame_ (transa, "C") && !lsame_ (transa, "T")) {
		info = 1;
	} else if (!notb && !lsame_ (transb, "C") && !lsame_ (transb, "T")) {
		info = 2;
	} else if (*m < 0) {
		info = 3;
	} else if (*n < 0) {
		info = 4;
	} else if (*k < 0) {
		info = 5;
	} else if (*lda < MAX (1, nrowa)) {
		info = 8;
	} else if (*ldb < MAX (1, nrowb)) {
		info = 10;
	} else if (*ldc < MAX (1, *m)) {
		info = 13;
	}
	if (info != 0) {
		xerbla_ ("DGEMM ", &info);
		return 0;
	}
	/* Quick return if possible. */
	if (*m == 0 || *n == 0 || ((*alpha == 0. || *k == 0) && *beta == 1.)) {
		return 0;
	}
	/* And if alpha.eq.zero. */
	if (*alpha == 0.) {
		if (*beta == 0.) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					c___ref (i__, j) = 0.;
					/* L10: */
				}
				/* L20: */
			}
		} else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					c___ref (i__, j) = *beta * c___ref (i__, j);
					/* L30: */
				}
				/* L40: */
			}
		}
		return 0;
	}
	/* Start the operations. */
	if (notb) {
		if (nota) {
			/* Form C := alpha*A*B + beta*C. */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (*beta == 0.) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, j) = 0.;
						/* L50: */
					}
				} else if (*beta != 1.) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, j) = *beta * c___ref (i__, j);
						/* L60: */
					}
				}
				i__2 = *k;
				for (l = 1; l <= i__2; ++l) {
					if (b_ref (l, j) != 0.) {
						temp = *alpha * b_ref (l, j);
						i__3 = *m;
						for (i__ = 1; i__ <= i__3; ++i__) {
							c___ref (i__, j) = c___ref (i__, j) + temp * a_ref (i__, l);
							/* L70: */
						}
					}
					/* L80: */
				}
				/* L90: */
			}
		} else {
			/* Form C := alpha*A'*B + beta*C */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp = 0.;
					i__3 = *k;
					for (l = 1; l <= i__3; ++l) {
						temp += a_ref (l, i__) * b_ref (l, j);
						/* L100: */
					}
					if (*beta == 0.) {
						c___ref (i__, j) = *alpha * temp;
					} else {
						c___ref (i__, j) = *alpha * temp + *beta * c___ref (i__, j);
					}
					/* L110: */
				}
				/* L120: */
			}
		}
	} else {
		if (nota) {
			/* Form C := alpha*A*B' + beta*C */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (*beta == 0.) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, j) = 0.;
						/* L130: */
					}
				} else if (*beta != 1.) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, j) = *beta * c___ref (i__, j);
						/* L140: */
					}
				}
				i__2 = *k;
				for (l = 1; l <= i__2; ++l) {
					if (b_ref (j, l) != 0.) {
						temp = *alpha * b_ref (j, l);
						i__3 = *m;
						for (i__ = 1; i__ <= i__3; ++i__) {
							c___ref (i__, j) = c___ref (i__, j) + temp * a_ref (i__, l);
							/* L150: */
						}
					}
					/* L160: */
				}
				/* L170: */
			}
		} else {
			/* Form C := alpha*A'*B' + beta*C */
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp = 0.;
					i__3 = *k;
					for (l = 1; l <= i__3; ++l) {
						temp += a_ref (l, i__) * b_ref (j, l);
						/* L180: */
					}
					if (*beta == 0.) {
						c___ref (i__, j) = *alpha * temp;
					} else {
						c___ref (i__, j) = *alpha * temp + *beta * c___ref (i__, j);
					}
					/* L190: */
				}
				/* L200: */
			}
		}
	}
	return 0;
	/* End of DGEMM . */
}								/* NUMblas_dgemm */

#undef c___ref
#undef b_ref
#undef a_ref

int NUMblas_dger (integer *m, integer *n, double *alpha, double *x, integer *incx, double *y, integer *incy, double *a,
                  integer *lda) {
	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2;

	/* Local variables */
	static integer info;
	static double temp;
	static integer i__, j, ix, jy, kx;

#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
	/* Test the input parameters. Parameter adjustments */
	--x;
	--y;
	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	/* Function Body */
	info = 0;
	if (*m < 0) {
		info = 1;
	} else if (*n < 0) {
		info = 2;
	} else if (*incx == 0) {
		info = 5;
	} else if (*incy == 0) {
		info = 7;
	} else if (*lda < MAX (1, *m)) {
		info = 9;
	}
	if (info != 0) {
		xerbla_ ("DGER  ", &info);
		return 0;
	}
	/* Quick return if possible. */
	if (*m == 0 || *n == 0 || *alpha == 0.) {
		return 0;
	}
	/* Start the operations. In this version the elements of A are accessed
	   sequentially with one pass through A. */
	if (*incy > 0) {
		jy = 1;
	} else {
		jy = 1 - (*n - 1) * *incy;
	}
	if (*incx == 1) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			if (y[jy] != 0.) {
				temp = *alpha * y[jy];
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					a_ref (i__, j) = a_ref (i__, j) + x[i__] * temp;
					/* L10: */
				}
			}
			jy += *incy;
			/* L20: */
		}
	} else {
		if (*incx > 0) {
			kx = 1;
		} else {
			kx = 1 - (*m - 1) * *incx;
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			if (y[jy] != 0.) {
				temp = *alpha * y[jy];
				ix = kx;
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					a_ref (i__, j) = a_ref (i__, j) + x[ix] * temp;
					ix += *incx;
					/* L30: */
				}
			}
			jy += *incy;
			/* L40: */
		}
	}
	return 0;
}								/* NUMblas_dger */

#undef a_ref

int NUMblas_dgemv (const char *trans, integer *m, integer *n, double *alpha, double *a, integer *lda, double *x, integer *incx,
                   double *beta, double *y, integer *incy) {
	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2;

	/* Local variables */
	static integer info;
	static double temp;
	static integer lenx, leny, i__, j;
	static integer ix, iy, jx, jy, kx, ky;

#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]

	/* Parameter adjustments */
	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--x;
	--y;
	/* Function Body */
	info = 0;
	if (!lsame_ (trans, "N") && !lsame_ (trans, "T") && !lsame_ (trans, "C")) {
		info = 1;
	} else if (*m < 0) {
		info = 2;
	} else if (*n < 0) {
		info = 3;
	} else if (*lda < MAX (1, *m)) {
		info = 6;
	} else if (*incx == 0) {
		info = 8;
	} else if (*incy == 0) {
		info = 11;
	}
	if (info != 0) {
		xerbla_ ("DGEMV ", &info);
		return 0;
	}
	/* Quick return if possible. */
	if (*m == 0 || *n == 0 || (*alpha == 0. && *beta == 1.)) {
		return 0;
	}
	/* Set LENX and LENY, the lengths of the vectors x and y, and set up the
	   start points in X and Y. */
	if (lsame_ (trans, "N")) {
		lenx = *n;
		leny = *m;
	} else {
		lenx = *m;
		leny = *n;
	}
	if (*incx > 0) {
		kx = 1;
	} else {
		kx = 1 - (lenx - 1) * *incx;
	}
	if (*incy > 0) {
		ky = 1;
	} else {
		ky = 1 - (leny - 1) * *incy;
	}
	/* Start the operations. In this version the elements of A are accessed
	   sequentially with one pass through A. First form y := beta*y. */
	if (*beta != 1.) {
		if (*incy == 1) {
			if (*beta == 0.) {
				i__1 = leny;
				for (i__ = 1; i__ <= i__1; ++i__) {
					y[i__] = 0.;
					/* L10: */
				}
			} else {
				i__1 = leny;
				for (i__ = 1; i__ <= i__1; ++i__) {
					y[i__] = *beta * y[i__];
					/* L20: */
				}
			}
		} else {
			iy = ky;
			if (*beta == 0.) {
				i__1 = leny;
				for (i__ = 1; i__ <= i__1; ++i__) {
					y[iy] = 0.;
					iy += *incy;
					/* L30: */
				}
			} else {
				i__1 = leny;
				for (i__ = 1; i__ <= i__1; ++i__) {
					y[iy] = *beta * y[iy];
					iy += *incy;
					/* L40: */
				}
			}
		}
	}
	if (*alpha == 0.) {
		return 0;
	}
	if (lsame_ (trans, "N")) {
		/* Form y := alpha*A*x + y. */
		jx = kx;
		if (*incy == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (x[jx] != 0.) {
					temp = *alpha * x[jx];
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						y[i__] += temp * a_ref (i__, j);
						/* L50: */
					}
				}
				jx += *incx;
				/* L60: */
			}
		} else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (x[jx] != 0.) {
					temp = *alpha * x[jx];
					iy = ky;
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						y[iy] += temp * a_ref (i__, j);
						iy += *incy;
						/* L70: */
					}
				}
				jx += *incx;
				/* L80: */
			}
		}
	} else {
		/* Form y := alpha*A'*x + y. */
		jy = ky;
		if (*incx == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				temp = 0.;
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp += a_ref (i__, j) * x[i__];
					/* L90: */
				}
				y[jy] += *alpha * temp;
				jy += *incy;
				/* L100: */
			}
		} else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				temp = 0.;
				ix = kx;
				i__2 = *m;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp += a_ref (i__, j) * x[ix];
					ix += *incx;
					/* L110: */
				}
				y[jy] += *alpha * temp;
				jy += *incy;
				/* L120: */
			}
		}
	}
	return 0;
}								/* NUMblas_dgemv */

#undef a_ref

double NUMblas_dlamch (const char *cmach) {
	/* Initialized data */
	static integer first = TRUE;

	/* System generated locals */
	integer i__1;
	double ret_val;

	/* Builtin functions */
	/* Local variables */
	static double base;
	static integer beta;
	static double emin, prec, emax;
	static integer imin, imax;
	static integer lrnd;
	static double rmin, rmax, t, rmach;
	static double smal, sfmin;
	static integer it;
	static double rnd, eps;

	if (first) {
		first = FALSE;
		dlamc2_ (&beta, &it, &lrnd, &eps, &imin, &rmin, &imax, &rmax);
		base = (double) beta;
		t = (double) it;
		if (lrnd) {
			rnd = 1.;
			i__1 = 1 - it;
			eps = pow_di (&base, &i__1) / 2;
		} else {
			rnd = 0.;
			i__1 = 1 - it;
			eps = pow_di (&base, &i__1);
		}
		prec = eps * base;
		emin = (double) imin;
		emax = (double) imax;
		sfmin = rmin;
		smal = 1. / rmax;
		if (smal >= sfmin) {

			/* Use smal plus a bit, to avoid the possibility of rounding
			   causing overflow when computing 1/sfmin. */

			sfmin = smal * (eps + 1.);
		}
	}

	if (lsame_ (cmach, "E")) {
		rmach = eps;
	} else if (lsame_ (cmach, "S")) {
		rmach = sfmin;
	} else if (lsame_ (cmach, "B")) {
		rmach = base;
	} else if (lsame_ (cmach, "P")) {
		rmach = prec;
	} else if (lsame_ (cmach, "N")) {
		rmach = t;
	} else if (lsame_ (cmach, "R")) {
		rmach = rnd;
	} else if (lsame_ (cmach, "M")) {
		rmach = emin;
	} else if (lsame_ (cmach, "U")) {
		rmach = rmin;
	} else if (lsame_ (cmach, "L")) {
		rmach = emax;
	} else if (lsame_ (cmach, "O")) {
		rmach = rmax;
	}

	ret_val = rmach;
	return ret_val;
}								/* NUMblas_dlamch */

static int dlamc1_ (integer *beta, integer *t, integer *rnd, integer *ieee1) {
	/* -- LAPACK auxiliary routine (version 3.0) -- Univ. of Tennessee, Univ.
	   of California Berkeley, NAG Ltd., Courant Institute, Argonne National
	   Lab, and Rice University October 31, 1992

	   Purpose =======

	   DLAMC1 determines the machine parameters given by BETA, T, RND, and
	   IEEE1.

	   Arguments =========

	   BETA (output) INTEGER The base of the machine.

	   T (output) INTEGER The number of ( BETA ) digits in the mantissa.

	   RND (output) LOGICAL Specifies whether proper rounding ( RND = .TRUE.
	   ) or chopping ( RND = .FALSE. ) occurs in addition. This may not

	   be a reliable guide to the way in which the machine performs

	   its arithmetic.

	   IEEE1 (output) LOGICAL Specifies whether rounding appears to be done
	   in the IEEE 'round to nearest' style.

	   Further Details ===============

	   The routine is based on the routine ENVRON by Malcolm and incorporates
	   suggestions by Gentleman and Marovich. See

	   Malcolm M. A. (1972) Algorithms to reveal properties of floating-point
	   arithmetic. Comms. of the ACM, 15, 949-951.

	   Gentleman W. M. and Marovich S. B. (1974) More on algorithms that
	   reveal properties of floating point arithmetic units. Comms. of the
	   ACM, 17, 276-277.

	   ===================================================================== */
	/* Initialized data */
	static integer first = TRUE;

	/* System generated locals */
	double d__1, d__2;

	/* Local variables */
	static integer lrnd;
	static double a, b, c, f;
	static integer lbeta;
	static double savec;
	static integer lieee1;
	static double t1, t2;
	static integer lt;
	static double one, qtr;

	if (first) {
		first = FALSE;
		one = 1.;

		/* LBETA, LIEEE1, LT and LRND are the local values of BETA, IEEE1, T
		   and RND.

		   Throughout this routine we use the function DLAMC3 to ensure that
		   relevant values are stored and not held in registers, or are not
		   affected by optimizers. Compute a = 2.0**m with the smallest
		   positive integer m such that fl( a + 1.0 ) = a. */

		a = 1.;
		c = 1.;

		/* + WHILE( C.EQ.ONE )LOOP */
L10:
		if (c == one) {
			a *= 2;
			c = dlamc3_ (&a, &one);
			d__1 = -a;
			c = dlamc3_ (&c, &d__1);
			goto L10;
		}
		/* + END WHILE

		   Now compute b = 2.0**m with the smallest positive integer m such
		   that fl( a + b ) .gt. a. */

		b = 1.;
		c = dlamc3_ (&a, &b);

		/* + WHILE( C.EQ.A )LOOP */
L20:
		if (c == a) {
			b *= 2;
			c = dlamc3_ (&a, &b);
			goto L20;
		}
		/* + END WHILE

		   Now compute the base.  a and c are neighbouring floating point
		   numbers in the interval ( beta**t, beta**( t + 1 ) ) and so their
		   difference is beta. Adding 0.25 to c is to ensure that it is
		   truncated to beta and not ( beta - 1 ). */

		qtr = one / 4;
		savec = c;
		d__1 = -a;
		c = dlamc3_ (&c, &d__1);
		lbeta = (integer) (c + qtr);

		/* Now determine whether rounding or chopping occurs, by adding a bit
		   less than beta/2 and a bit more than beta/2 to a. */

		b = (double) lbeta;
		d__1 = b / 2;
		d__2 = -b / 100;
		f = dlamc3_ (&d__1, &d__2);
		c = dlamc3_ (&f, &a);
		if (c == a) {
			lrnd = TRUE;
		} else {
			lrnd = FALSE;
		}
		d__1 = b / 2;
		d__2 = b / 100;
		f = dlamc3_ (&d__1, &d__2);
		c = dlamc3_ (&f, &a);
		if (lrnd && c == a) {
			lrnd = FALSE;
		}

		/* Try and decide whether rounding is done in the IEEE 'round to
		   nearest' style. B/2 is half a unit in the last place of the two
		   numbers A and SAVEC. Furthermore, A is even, i.e. has last bit
		   zero, and SAVEC is odd. Thus adding B/2 to A should not change A,
		   but adding B/2 to SAVEC should change SAVEC. */

		d__1 = b / 2;
		t1 = dlamc3_ (&d__1, &a);
		d__1 = b / 2;
		t2 = dlamc3_ (&d__1, &savec);
		lieee1 = t1 == a && t2 > savec && lrnd;

		/* Now find the mantissa, t. It should be the integer part of log to
		   the base beta of a, however it is safer to determine t by
		   powering.  So we find t as the smallest positive integer for which
		   fl( beta**t + 1.0 ) = 1.0. */

		lt = 0;
		a = 1.;
		c = 1.;

		/* + WHILE( C.EQ.ONE )LOOP */
L30:
		if (c == one) {
			++lt;
			a *= lbeta;
			c = dlamc3_ (&a, &one);
			d__1 = -a;
			c = dlamc3_ (&c, &d__1);
			goto L30;
		}
		/* + END WHILE */

	}

	*beta = lbeta;
	*t = lt;
	*rnd = lrnd;
	*ieee1 = lieee1;
	return 0;
}								/* dlamc1_ */

static int dlamc2_ (integer *beta, integer *t, integer *rnd, double *eps, integer *emin, double *rmin, integer *emax,
                    double *rmax) {
	/* -- LAPACK auxiliary routine (version 3.0) -- Univ. of Tennessee, Univ.
	   of California Berkeley, NAG Ltd., Courant Institute, Argonne National
	   Lab, and Rice University October 31, 1992

	   Purpose =======

	   DLAMC2 determines the machine parameters specified in its argument
	   list.

	   Arguments =========

	   BETA (output) INTEGER The base of the machine.

	   T (output) INTEGER The number of ( BETA ) digits in the mantissa.

	   RND (output) LOGICAL Specifies whether proper rounding ( RND = .TRUE.
	   ) or chopping ( RND = .FALSE. ) occurs in addition. This may not
	   be a reliable guide to the way in which the machine performs
	   its arithmetic.

	   EPS (output) DOUBLE PRECISION The smallest positive number such that
	   fl( 1.0 - EPS ) .LT. 1.0, where fl denotes the computed value.

	   EMIN (output) INTEGER The minimum exponent before (gradual) underflow
	   occurs.

	   RMIN (output) DOUBLE PRECISION The smallest normalized number for the
	   machine, given by BASE**( EMIN - 1 ), where BASE is the floating point
	   value of BETA.

	   EMAX (output) INTEGER The maximum exponent before overflow occurs.

	   RMAX (output) DOUBLE PRECISION The largest positive number for the
	   machine, given by BASE**EMAX * ( 1 - EPS ), where BASE is the floating
	   point value of BETA.

	   Further Details ===============

	   The computation of EPS is based on a routine PARANOIA by W. Kahan of
	   the University of California at Berkeley.

	   ===================================================================== */
	/* Table of constant values */
	/* Initialized data */
	static integer first = TRUE;
	static integer iwarn = FALSE;

	/* System generated locals */
	integer i__1;
	double d__1, d__2, d__3, d__4, d__5;

	/* Builtin functions */
	/* Local variables */
	static integer ieee;
	static double half;
	static integer lrnd;
	static double leps, zero, a, b, c;
	static integer i, lbeta;
	static double rbase;
	static integer lemin, lemax, gnmin;
	static double smal;
	static integer gpmin;
	static double third, lrmin, lrmax, sixth;
	static integer lieee1;
	static integer lt, ngnmin, ngpmin;
	static double one, two;

	if (first) {
		first = FALSE;
		zero = 0.;
		one = 1.;
		two = 2.;

		/* LBETA, LT, LRND, LEPS, LEMIN and LRMIN are the local values of
		   BETA, T, RND, EPS, EMIN and RMIN. Throughout this routine we use
		   the function DLAMC3 to ensure that relevant values are stored and
		   not held in registers, or are not affected by optimizers. DLAMC1
		   returns the parameters LBETA, LT, LRND and LIEEE1. */

		dlamc1_ (&lbeta, &lt, &lrnd, &lieee1);

		/* Start to find EPS. */

		b = (double) lbeta;
		i__1 = -lt;
		a = pow_di (&b, &i__1);
		leps = a;

		/* Try some tricks to see whether or not this is the correct EPS. */

		b = two / 3;
		half = one / 2;
		d__1 = -half;
		sixth = dlamc3_ (&b, &d__1);
		third = dlamc3_ (&sixth, &sixth);
		d__1 = -half;
		b = dlamc3_ (&third, &d__1);
		b = dlamc3_ (&b, &sixth);
		b = fabs (b);
		if (b < leps) {
			b = leps;
		}

		leps = 1.;

		/* + WHILE( ( LEPS.GT.B ).AND.( B.GT.ZERO ) )LOOP */
L10:
		if (leps > b && b > zero) {
			leps = b;
			d__1 = half * leps;
			/* Computing 5th power */
			d__3 = two, d__4 = d__3, d__3 *= d__3;
			/* Computing 2nd power */
			d__5 = leps;
			d__2 = d__4 * (d__3 * d__3) * (d__5 * d__5);
			c = dlamc3_ (&d__1, &d__2);
			d__1 = -c;
			c = dlamc3_ (&half, &d__1);
			b = dlamc3_ (&half, &c);
			d__1 = -b;
			c = dlamc3_ (&half, &d__1);
			b = dlamc3_ (&half, &c);
			goto L10;
		}
		/* + END WHILE */

		if (a < leps) {
			leps = a;
		}

		/* Computation of EPS complete. Now find EMIN.  Let A = + or - 1, and
		   + or - (1 + BASE**(-3)). Keep dividing A by BETA until (gradual)
		   underflow occurs. This is detected when we cannot recover the
		   previous A. */

		rbase = one / lbeta;
		smal = one;
		for (i = 1; i <= 3; ++i) {
			d__1 = smal * rbase;
			smal = dlamc3_ (&d__1, &zero);
			/* L20: */
		}
		a = dlamc3_ (&one, &smal);
		dlamc4_ (&ngpmin, &one, &lbeta);
		d__1 = -one;
		dlamc4_ (&ngnmin, &d__1, &lbeta);
		dlamc4_ (&gpmin, &a, &lbeta);
		d__1 = -a;
		dlamc4_ (&gnmin, &d__1, &lbeta);
		ieee = FALSE;

		if (ngpmin == ngnmin && gpmin == gnmin) {
			if (ngpmin == gpmin) {
				lemin = ngpmin;
				/* ( Non twos-complement machines, no gradual underflow;
				   e.g., VAX ) */
			} else if (gpmin - ngpmin == 3) {
				lemin = ngpmin - 1 + lt;
				ieee = TRUE;
				/* ( Non twos-complement machines, with gradual underflow;
				   e.g., IEEE standard followers ) */
			} else {
				lemin = MIN (ngpmin, gpmin);
				/* ( A guess; no known machine ) */
				iwarn = TRUE;
			}

		} else if (ngpmin == gpmin && ngnmin == gnmin) {
			if ( (i__1 = ngpmin - ngnmin, labs (i__1)) == 1) {
				lemin = MAX (ngpmin, ngnmin);
				/* ( Twos-complement machines, no gradual underflow; e.g.,
				   CYBER 205 ) */
			} else {
				lemin = MIN (ngpmin, ngnmin);
				/* ( A guess; no known machine ) */
				iwarn = TRUE;
			}

		} else if ( (i__1 = ngpmin - ngnmin, labs (i__1)) == 1 && gpmin == gnmin) {
			if (gpmin - MIN (ngpmin, ngnmin) == 3) {
				lemin = MAX (ngpmin, ngnmin) - 1 + lt;
				/* ( Twos-complement machines with gradual underflow; no
				   known machine ) */
			} else {
				lemin = MIN (ngpmin, ngnmin);
				/* ( A guess; no known machine ) */
				iwarn = TRUE;
			}

		} else {
			/* Computing MIN */
			i__1 = MIN (ngpmin, ngnmin), i__1 = MIN (i__1, gpmin);
			lemin = MIN (i__1, gnmin);
			/* ( A guess; no known machine ) */
			iwarn = TRUE;
		}
		/* Comment out this if block if EMIN is ok */
		if (iwarn) {
			first = TRUE;
			Melder_warning (U"\n\n WARNING. The value EMIN may be incorrect:- " "EMIN = ", lemin,
			                U"\nIf, after inspection, the value EMIN looks acceptable"
			                "please comment out \n the IF block as marked within the"
			                "code of routine DLAMC2, \n otherwise supply EMIN" "explicitly.\n");
		}
		/* ** Assume IEEE arithmetic if we found denormalised numbers above,
		   or if arithmetic seems to round in the IEEE style, determined in
		   routine DLAMC1. A true IEEE machine should have both things true;
		   however, faulty machines may have one or the other. */

		ieee = ieee || lieee1;

		/* Compute RMIN by successive division by BETA. We could compute RMIN
		   as BASE**( EMIN - 1 ), but some machines underflow during this
		   computation. */

		lrmin = 1.;
		i__1 = 1 - lemin;
		for (i = 1; i <= 1 - lemin; ++i) {
			d__1 = lrmin * rbase;
			lrmin = dlamc3_ (&d__1, &zero);
			/* L30: */
		}

		/* Finally, call DLAMC5 to compute EMAX and RMAX. */

		dlamc5_ (&lbeta, &lt, &lemin, &ieee, &lemax, &lrmax);
	}

	*beta = lbeta;
	*t = lt;
	*rnd = lrnd;
	*eps = leps;
	*emin = lemin;
	*rmin = lrmin;
	*emax = lemax;
	*rmax = lrmax;

	return 0;
}								/* dlamc2_ */

static double dlamc3_ (double *a, double *b)
/* Purpose =======

   dlamc3_ is intended to force A and B to be stored prior to doing the
   addition of A and B , for use in situations where optimizers might hold
   one of these in a register.

   Arguments =========

   A, B (input) DOUBLE PRECISION The values A and B.

   ===================================================================== */
{
	volatile double ret_val;

	ret_val = *a + *b;
	return ret_val;
}								/* dlamc3_ */

static int dlamc4_ (integer *emin, double *start, integer *base) {
	/* -- LAPACK auxiliary routine (version 2.0) -- Univ. of Tennessee, Univ.
	   of California Berkeley, NAG Ltd., Courant Institute, Argonne National
	   Lab, and Rice University October 31, 1992

	   Purpose =======

	   DLAMC4 is a service routine for DLAMC2.

	   Arguments =========

	   EMIN (output) EMIN The minimum exponent before (gradual) underflow,
	   computed by

	   setting A = START and dividing by BASE until the previous A can not be
	   recovered.

	   START (input) DOUBLE PRECISION The starting point for determining
	   EMIN.

	   BASE (input) INTEGER The base of the machine.

	   ===================================================================== */
	/* System generated locals */
	integer i__1;
	double d__1;

	/* Local variables */
	static double zero, a;
	static integer i;
	static double rbase, b1, b2, c1, c2, d1, d2;
	static double one;

	a = *start;
	one = 1.;
	rbase = one / *base;
	zero = 0.;
	*emin = 1;
	d__1 = a * rbase;
	b1 = dlamc3_ (&d__1, &zero);
	c1 = a;
	c2 = a;
	d1 = a;
	d2 = a;
	/* + WHILE( ( C1.EQ.A ).AND.( C2.EQ.A ).AND. $ ( D1.EQ.A ).AND.( D2.EQ.A
	   ) )LOOP */
L10:
	if (c1 == a && c2 == a && d1 == a && d2 == a) {
		-- (*emin);
		a = b1;
		d__1 = a / *base;
		b1 = dlamc3_ (&d__1, &zero);
		d__1 = b1 * *base;
		c1 = dlamc3_ (&d__1, &zero);
		d1 = zero;
		i__1 = *base;
		for (i = 1; i <= *base; ++i) {
			d1 += b1;
			/* L20: */
		}
		d__1 = a * rbase;
		b2 = dlamc3_ (&d__1, &zero);
		d__1 = b2 / rbase;
		c2 = dlamc3_ (&d__1, &zero);
		d2 = zero;
		i__1 = *base;
		for (i = 1; i <= *base; ++i) {
			d2 += b2;
			/* L30: */
		}
		goto L10;
	}
	/* + END WHILE */

	return 0;
}								/* dlamc4_ */

static int dlamc5_ (integer *beta, integer *p, integer *emin, integer *ieee, integer *emax, double *rmax) {
	/*
	   First compute LEXP and UEXP, two powers of 2 that bound abs(EMIN). We
	   then assume that EMAX + abs(EMIN) will sum approximately to the bound
	   that is closest to abs(EMIN). (EMAX is the exponent of the required
	   number RMAX). */
	/* Table of constant values */
	static double c_b5 = 0.;

	/* System generated locals */
	integer i__1;
	double d__1;

	/* Local variables */
	static integer lexp;
	static double oldy;
	static integer uexp, i;
	static double y, z;
	static integer nbits;
	static double recbas;
	static integer exbits, expsum, try__;

	lexp = 1;
	exbits = 1;
L10:
	try__ = lexp << 1;
	if (try__ <= - (*emin)) {
		lexp = try__;
		++exbits;
		goto L10;
	}
	if (lexp == - (*emin)) {
		uexp = lexp;
	} else {
		uexp = try__;
		++exbits;
	}

	/* Now -LEXP is less than or equal to EMIN, and -UEXP is greater than or
	   equal to EMIN. EXBITS is the number of bits needed to store the
	   exponent. */

	if (uexp + *emin > -lexp - *emin) {
		expsum = lexp << 1;
	} else {
		expsum = uexp << 1;
	}

	/* EXPSUM is the exponent range, approximately equal to EMAX - EMIN + 1 .
	 */

	*emax = expsum + *emin - 1;
	nbits = exbits + 1 + *p;

	/* NBITS is the total number of bits needed to store a floating-point
	   number. */

	if (nbits % 2 == 1 && *beta == 2) {

		/* Either there are an odd number of bits used to store a
		   floating-point number, which is unlikely, or some bits are not
		   used in the representation of numbers, which is possible, (e.g.
		   Cray machines) or the mantissa has an implicit bit, (e.g. IEEE
		   machines, Dec Vax machines), which is perhaps the most likely. We
		   have to assume the last alternative. If this is true, then we need
		   to reduce EMAX by one because there should be some way of
		   representing zero in an implicit-bit system. On machines like
		   Cray, we are reducing EMAX by one unnecessarily. */
		-- (*emax);
	}

	if (*ieee) {
		/* Assume we are on an IEEE machine which reserves one exponent for
		   infinity and NaN. */
		-- (*emax);
	}

	/* Now create RMAX, the largest machine number, which should be equal to
	   (1.0 - BETA**(-P)) * BETA**EMAX . First compute 1.0 - BETA**(-P),
	   being careful that the result is less than 1.0 . */

	recbas = 1. / *beta;
	z = *beta - 1.;
	y = 0.;
	i__1 = *p;
	for (i = 1; i <= *p; ++i) {
		z *= recbas;
		if (y < 1.) {
			oldy = y;
		}
		y = dlamc3_ (&y, &z);
		/* L20: */
	}
	if (y >= 1.) {
		y = oldy;
	}

	/* Now multiply by BETA**EMAX to get RMAX. */

	i__1 = *emax;
	for (i = 1; i <= *emax; ++i) {
		d__1 = y * *beta;
		y = dlamc3_ (&d__1, &c_b5);
		/* L30: */
	}

	*rmax = y;
	return 0;
}								/* dlamc5_ */

double NUMblas_dnrm2 (integer *n, double *x, integer *incx) {
	/* The following loop is equivalent to this call to the LAPACK auxiliary
	   routine: CALL DLASSQ( N, X, INCX, SCALE, SSQ ) */
	/* System generated locals */
	integer i__1, i__2;
	double ret_val, d__1;

	/* Local variables */
	static double norm, scale, absxi;
	static integer ix;
	static double ssq;

	--x;
	/* Function Body */
	if (*n < 1 || *incx < 1) {
		norm = 0.;
	} else if (*n == 1) {
		norm = fabs (x[1]);
	} else {
		scale = 0.;
		ssq = 1.;

		i__1 = (*n - 1) * *incx + 1;
		i__2 = *incx;
		for (ix = 1; i__2 < 0 ? ix >= i__1 : ix <= i__1; ix += i__2) {
			if (x[ix] != 0.) {
				absxi = (d__1 = x[ix], fabs (d__1));
				if (scale < absxi) {
					/* Computing 2nd power */
					d__1 = scale / absxi;
					ssq = ssq * (d__1 * d__1) + 1.;
					scale = absxi;
				} else {
					/* Computing 2nd power */
					d__1 = absxi / scale;
					ssq += d__1 * d__1;
				}
			}
			/* L10: */
		}
		norm = scale * sqrt (ssq);
	}

	ret_val = norm;
	return ret_val;
}								/* NUMblas_dnrm2 */

int NUMblas_drot (integer *n, double *dx, integer *incx, double *dy, integer *incy, double *c__, double *s) {
	/* System generated locals */
	integer i__1;

	/* Local variables */
	static integer i__;
	static double dtemp;
	static integer ix, iy;

	/* applies a plane rotation. jack dongarra, linpack, 3/11/78. modified
	   12/3/93, array(1) declarations changed to array(*) Parameter
	   adjustments */

	--dy;
	--dx;
	/* Function Body */
	if (*n <= 0) {
		return 0;
	}
	if (*incx == 1 && *incy == 1) {
		goto L20;
	}
	/* code for unequal increments or equal increments not equal to 1 */
	ix = 1;
	iy = 1;
	if (*incx < 0) {
		ix = (- (*n) + 1) * *incx + 1;
	}
	if (*incy < 0) {
		iy = (- (*n) + 1) * *incy + 1;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		dtemp = *c__ * dx[ix] + *s * dy[iy];
		dy[iy] = *c__ * dy[iy] - *s * dx[ix];
		dx[ix] = dtemp;
		ix += *incx;
		iy += *incy;
		/* L10: */
	}
	return 0;
	/* code for both increments equal to 1 */
L20:
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		dtemp = *c__ * dx[i__] + *s * dy[i__];
		dy[i__] = *c__ * dy[i__] - *s * dx[i__];
		dx[i__] = dtemp;
		/* L30: */
	}
	return 0;
}								/* NUMblas_drot */

int NUMblas_dscal (integer *n, double *da, double *dx, integer *incx) {
	/* System generated locals */
	integer i__1, i__2;

	/* Local variables */
	static integer i__, m, nincx, mp1;

	/* Parameter adjustments */
	--dx;
	/* Function Body */
	if (*n <= 0 || *incx <= 0) {
		return 0;
	}
	if (*incx == 1) {
		goto L20;
	}
	/* code for increment not equal to 1 */
	nincx = *n * *incx;
	i__1 = nincx;
	i__2 = *incx;
	for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
		dx[i__] = *da * dx[i__];
		/* L10: */
	}
	return 0;
	/* code for increment equal to 1 clean-up loop */
L20:
	m = *n % 5;
	if (m == 0) {
		goto L40;
	}
	i__2 = m;
	for (i__ = 1; i__ <= i__2; ++i__) {
		dx[i__] = *da * dx[i__];
		/* L30: */
	}
	if (*n < 5) {
		return 0;
	}
L40:
	mp1 = m + 1;
	i__2 = *n;
	for (i__ = mp1; i__ <= i__2; i__ += 5) {
		dx[i__] = *da * dx[i__];
		dx[i__ + 1] = *da * dx[i__ + 1];
		dx[i__ + 2] = *da * dx[i__ + 2];
		dx[i__ + 3] = *da * dx[i__ + 3];
		dx[i__ + 4] = *da * dx[i__ + 4];
		/* L50: */
	}
	return 0;
}								/* dscal_ */

int NUMblas_dswap (integer *n, double *dx, integer *incx, double *dy, integer *incy) {
	/* System generated locals */
	integer i__1;

	/* Local variables */
	static integer i__, m;
	static double dtemp;
	static integer ix, iy, mp1;

	/* interchanges two vectors. uses unrolled loops for increments equal
	   one. jack dongarra, linpack, 3/11/78. modified 12/3/93, array(1)
	   declarations changed to array(*) Parameter adjustments */
	--dy;
	--dx;
	/* Function Body */
	if (*n <= 0) {
		return 0;
	}
	if (*incx == 1 && *incy == 1) {
		goto L20;
	}
	/* code for unequal increments or equal increments not equal to 1 */
	ix = 1;
	iy = 1;
	if (*incx < 0) {
		ix = (- (*n) + 1) * *incx + 1;
	}
	if (*incy < 0) {
		iy = (- (*n) + 1) * *incy + 1;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
		dtemp = dx[ix];
		dx[ix] = dy[iy];
		dy[iy] = dtemp;
		ix += *incx;
		iy += *incy;
		/* L10: */
	}
	return 0;
	/* code for both increments equal to 1 clean-up loop */
L20:
	m = *n % 3;
	if (m == 0) {
		goto L40;
	}
	i__1 = m;
	for (i__ = 1; i__ <= i__1; ++i__) {
		dtemp = dx[i__];
		dx[i__] = dy[i__];
		dy[i__] = dtemp;
		/* L30: */
	}
	if (*n < 3) {
		return 0;
	}
L40:
	mp1 = m + 1;
	i__1 = *n;
	for (i__ = mp1; i__ <= i__1; i__ += 3) {
		dtemp = dx[i__];
		dx[i__] = dy[i__];
		dy[i__] = dtemp;
		dtemp = dx[i__ + 1];
		dx[i__ + 1] = dy[i__ + 1];
		dy[i__ + 1] = dtemp;
		dtemp = dx[i__ + 2];
		dx[i__ + 2] = dy[i__ + 2];
		dy[i__ + 2] = dtemp;
		/* L50: */
	}
	return 0;
}								/* NUMblas_dswap */

int NUMblas_dsymv (const char *uplo, integer *n, double *alpha, double *a, integer *lda, double *x, integer *incx, double *beta,
                   double *y, integer *incy) {
	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2;

	/* Local variables */
	static integer info;
	static double temp1, temp2;
	static integer i__, j;
	static integer ix, iy, jx, jy, kx, ky;

#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--x;
	--y;
	/* Function Body */
	info = 0;
	if (!lsame_ (uplo, "U") && !lsame_ (uplo, "L")) {
		info = 1;
	} else if (*n < 0) {
		info = 2;
	} else if (*lda < MAX (1, *n)) {
		info = 5;
	} else if (*incx == 0) {
		info = 7;
	} else if (*incy == 0) {
		info = 10;
	}
	if (info != 0) {
		xerbla_ ("DSYMV ", &info);
		return 0;
	}
	/* Quick return if possible. */
	if (*n == 0 || (*alpha == 0. && *beta == 1.)) {
		return 0;
	}
	/* Set up the start points in X and Y. */
	if (*incx > 0) {
		kx = 1;
	} else {
		kx = 1 - (*n - 1) * *incx;
	}
	if (*incy > 0) {
		ky = 1;
	} else {
		ky = 1 - (*n - 1) * *incy;
	}
	/* Start the operations. In this version the elements of A are accessed
	   sequentially with one pass through the triangular part of A. First
	   form y := beta*y. */
	if (*beta != 1.) {
		if (*incy == 1) {
			if (*beta == 0.) {
				i__1 = *n;
				for (i__ = 1; i__ <= i__1; ++i__) {
					y[i__] = 0.;
					/* L10: */
				}
			} else {
				i__1 = *n;
				for (i__ = 1; i__ <= i__1; ++i__) {
					y[i__] = *beta * y[i__];
					/* L20: */
				}
			}
		} else {
			iy = ky;
			if (*beta == 0.) {
				i__1 = *n;
				for (i__ = 1; i__ <= i__1; ++i__) {
					y[iy] = 0.;
					iy += *incy;
					/* L30: */
				}
			} else {
				i__1 = *n;
				for (i__ = 1; i__ <= i__1; ++i__) {
					y[iy] = *beta * y[iy];
					iy += *incy;
					/* L40: */
				}
			}
		}
	}
	if (*alpha == 0.) {
		return 0;
	}
	if (lsame_ (uplo, "U")) {
		/* Form y when A is stored in upper triangle. */
		if (*incx == 1 && *incy == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				temp1 = *alpha * x[j];
				temp2 = 0.;
				i__2 = j - 1;
				for (i__ = 1; i__ <= i__2; ++i__) {
					y[i__] += temp1 * a_ref (i__, j);
					temp2 += a_ref (i__, j) * x[i__];
					/* L50: */
				}
				y[j] = y[j] + temp1 * a_ref (j, j) + *alpha * temp2;
				/* L60: */
			}
		} else {
			jx = kx;
			jy = ky;
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				temp1 = *alpha * x[jx];
				temp2 = 0.;
				ix = kx;
				iy = ky;
				i__2 = j - 1;
				for (i__ = 1; i__ <= i__2; ++i__) {
					y[iy] += temp1 * a_ref (i__, j);
					temp2 += a_ref (i__, j) * x[ix];
					ix += *incx;
					iy += *incy;
					/* L70: */
				}
				y[jy] = y[jy] + temp1 * a_ref (j, j) + *alpha * temp2;
				jx += *incx;
				jy += *incy;
				/* L80: */
			}
		}
	} else {
		/* Form y when A is stored in lower triangle. */
		if (*incx == 1 && *incy == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				temp1 = *alpha * x[j];
				temp2 = 0.;
				y[j] += temp1 * a_ref (j, j);
				i__2 = *n;
				for (i__ = j + 1; i__ <= i__2; ++i__) {
					y[i__] += temp1 * a_ref (i__, j);
					temp2 += a_ref (i__, j) * x[i__];
					/* L90: */
				}
				y[j] += *alpha * temp2;
				/* L100: */
			}
		} else {
			jx = kx;
			jy = ky;
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				temp1 = *alpha * x[jx];
				temp2 = 0.;
				y[jy] += temp1 * a_ref (j, j);
				ix = jx;
				iy = jy;
				i__2 = *n;
				for (i__ = j + 1; i__ <= i__2; ++i__) {
					ix += *incx;
					iy += *incy;
					y[iy] += temp1 * a_ref (i__, j);
					temp2 += a_ref (i__, j) * x[ix];
					/* L110: */
				}
				y[jy] += *alpha * temp2;
				jx += *incx;
				jy += *incy;
				/* L120: */
			}
		}
	}
	return 0;
}								/* NUMblas_dsymv */

#undef a_ref

int NUMblas_dsyr2 (const char *uplo, integer *n, double *alpha, double *x, integer *incx, double *y, integer *incy, double *a,
                   integer *lda) {
	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2;

	/* Local variables */
	static integer info;
	static double temp1, temp2;
	static integer i__, j;
	static integer ix, iy, jx, jy, kx, ky;

#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]

	--x;
	--y;
	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	/* Function Body */
	info = 0;
	if (!lsame_ (uplo, "U") && !lsame_ (uplo, "L")) {
		info = 1;
	} else if (*n < 0) {
		info = 2;
	} else if (*incx == 0) {
		info = 5;
	} else if (*incy == 0) {
		info = 7;
	} else if (*lda < MAX (1, *n)) {
		info = 9;
	}
	if (info != 0) {
		xerbla_ ("DSYR2 ", &info);
		return 0;
	}
	/* Quick return if possible. */
	if (*n == 0 || *alpha == 0.) {
		return 0;
	}
	/* Set up the start points in X and Y if the increments are not both
	   unity. */
	if (*incx != 1 || *incy != 1) {
		if (*incx > 0) {
			kx = 1;
		} else {
			kx = 1 - (*n - 1) * *incx;
		}
		if (*incy > 0) {
			ky = 1;
		} else {
			ky = 1 - (*n - 1) * *incy;
		}
		jx = kx;
		jy = ky;
	}
	/* Start the operations. In this version the elements of A are accessed
	   sequentially with one pass through the triangular part of A. */
	if (lsame_ (uplo, "U")) {
		/* Form A when A is stored in the upper triangle. */
		if (*incx == 1 && *incy == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (x[j] != 0. || y[j] != 0.) {
					temp1 = *alpha * y[j];
					temp2 = *alpha * x[j];
					i__2 = j;
					for (i__ = 1; i__ <= i__2; ++i__) {
						a_ref (i__, j) = a_ref (i__, j) + x[i__] * temp1 + y[i__] * temp2;
						/* L10: */
					}
				}
				/* L20: */
			}
		} else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (x[jx] != 0. || y[jy] != 0.) {
					temp1 = *alpha * y[jy];
					temp2 = *alpha * x[jx];
					ix = kx;
					iy = ky;
					i__2 = j;
					for (i__ = 1; i__ <= i__2; ++i__) {
						a_ref (i__, j) = a_ref (i__, j) + x[ix] * temp1 + y[iy] * temp2;
						ix += *incx;
						iy += *incy;
						/* L30: */
					}
				}
				jx += *incx;
				jy += *incy;
				/* L40: */
			}
		}
	} else {
		/* Form A when A is stored in the lower triangle. */
		if (*incx == 1 && *incy == 1) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (x[j] != 0. || y[j] != 0.) {
					temp1 = *alpha * y[j];
					temp2 = *alpha * x[j];
					i__2 = *n;
					for (i__ = j; i__ <= i__2; ++i__) {
						a_ref (i__, j) = a_ref (i__, j) + x[i__] * temp1 + y[i__] * temp2;
						/* L50: */
					}
				}
				/* L60: */
			}
		} else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (x[jx] != 0. || y[jy] != 0.) {
					temp1 = *alpha * y[jy];
					temp2 = *alpha * x[jx];
					ix = jx;
					iy = jy;
					i__2 = *n;
					for (i__ = j; i__ <= i__2; ++i__) {
						a_ref (i__, j) = a_ref (i__, j) + x[ix] * temp1 + y[iy] * temp2;
						ix += *incx;
						iy += *incy;
						/* L70: */
					}
				}
				jx += *incx;
				jy += *incy;
				/* L80: */
			}
		}
	}
	return 0;
}								/* NUMblas_dsyr2 */

#undef a_ref

int NUMblas_dsyr2k (const char *uplo, const char *trans, integer *n, integer *k, double *alpha, double *a, integer *lda, double *b,
                    integer *ldb, double *beta, double *c__, integer *ldc) {
	/* System generated locals */
	integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, i__1, i__2, i__3;

	/* Local variables */
	static integer info;
	static double temp1, temp2;
	static integer i__, j, l;
	static integer nrowa;
	static integer upper;

#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
#define c___ref(a_1,a_2) c__[(a_2)*c_dim1 + a_1]

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;
	c_dim1 = *ldc;
	c_offset = 1 + c_dim1 * 1;
	c__ -= c_offset;
	/* Function Body */
	if (lsame_ (trans, "N")) {
		nrowa = *n;
	} else {
		nrowa = *k;
	}
	upper = lsame_ (uplo, "U");
	info = 0;
	if (!upper && !lsame_ (uplo, "L")) {
		info = 1;
	} else if (!lsame_ (trans, "N") && !lsame_ (trans, "T") && !lsame_ (trans, "C")) {
		info = 2;
	} else if (*n < 0) {
		info = 3;
	} else if (*k < 0) {
		info = 4;
	} else if (*lda < MAX (1, nrowa)) {
		info = 7;
	} else if (*ldb < MAX (1, nrowa)) {
		info = 9;
	} else if (*ldc < MAX (1, *n)) {
		info = 12;
	}
	if (info != 0) {
		xerbla_ ("DSYR2K", &info);
		return 0;
	}
	/* Quick return if possible. */
	if (*n == 0 || ((*alpha == 0. || *k == 0) && *beta == 1.)) {
		return 0;
	}
	/* And when alpha.eq.zero. */
	if (*alpha == 0.) {
		if (upper) {
			if (*beta == 0.) {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					i__2 = j;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, j) = 0.;
						/* L10: */
					}
					/* L20: */
				}
			} else {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					i__2 = j;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, j) = *beta * c___ref (i__, j);
						/* L30: */
					}
					/* L40: */
				}
			}
		} else {
			if (*beta == 0.) {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *n;
					for (i__ = j; i__ <= i__2; ++i__) {
						c___ref (i__, j) = 0.;
						/* L50: */
					}
					/* L60: */
				}
			} else {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *n;
					for (i__ = j; i__ <= i__2; ++i__) {
						c___ref (i__, j) = *beta * c___ref (i__, j);
						/* L70: */
					}
					/* L80: */
				}
			}
		}
		return 0;
	}
	/* Start the operations. */
	if (lsame_ (trans, "N")) {
		/* Form C := alpha*A*B' + alpha*B*A' + C. */
		if (upper) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (*beta == 0.) {
					i__2 = j;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, j) = 0.;
						/* L90: */
					}
				} else if (*beta != 1.) {
					i__2 = j;
					for (i__ = 1; i__ <= i__2; ++i__) {
						c___ref (i__, j) = *beta * c___ref (i__, j);
						/* L100: */
					}
				}
				i__2 = *k;
				for (l = 1; l <= i__2; ++l) {
					if (a_ref (j, l) != 0. || b_ref (j, l) != 0.) {
						temp1 = *alpha * b_ref (j, l);
						temp2 = *alpha * a_ref (j, l);
						i__3 = j;
						for (i__ = 1; i__ <= i__3; ++i__) {
							c___ref (i__, j) =
							    c___ref (i__, j) + a_ref (i__, l) * temp1 + b_ref (i__, l) * temp2;
							/* L110: */
						}
					}
					/* L120: */
				}
				/* L130: */
			}
		} else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				if (*beta == 0.) {
					i__2 = *n;
					for (i__ = j; i__ <= i__2; ++i__) {
						c___ref (i__, j) = 0.;
						/* L140: */
					}
				} else if (*beta != 1.) {
					i__2 = *n;
					for (i__ = j; i__ <= i__2; ++i__) {
						c___ref (i__, j) = *beta * c___ref (i__, j);
						/* L150: */
					}
				}
				i__2 = *k;
				for (l = 1; l <= i__2; ++l) {
					if (a_ref (j, l) != 0. || b_ref (j, l) != 0.) {
						temp1 = *alpha * b_ref (j, l);
						temp2 = *alpha * a_ref (j, l);
						i__3 = *n;
						for (i__ = j; i__ <= i__3; ++i__) {
							c___ref (i__, j) =
							    c___ref (i__, j) + a_ref (i__, l) * temp1 + b_ref (i__, l) * temp2;
							/* L160: */
						}
					}
					/* L170: */
				}
				/* L180: */
			}
		}
	} else {
		/* Form C := alpha*A'*B + alpha*B'*A + C. */
		if (upper) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = j;
				for (i__ = 1; i__ <= i__2; ++i__) {
					temp1 = 0.;
					temp2 = 0.;
					i__3 = *k;
					for (l = 1; l <= i__3; ++l) {
						temp1 += a_ref (l, i__) * b_ref (l, j);
						temp2 += b_ref (l, i__) * a_ref (l, j);
						/* L190: */
					}
					if (*beta == 0.) {
						c___ref (i__, j) = *alpha * temp1 + *alpha * temp2;
					} else {
						c___ref (i__, j) = *beta * c___ref (i__, j) + *alpha * temp1 + *alpha * temp2;
					}
					/* L200: */
				}
				/* L210: */
			}
		} else {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
				i__2 = *n;
				for (i__ = j; i__ <= i__2; ++i__) {
					temp1 = 0.;
					temp2 = 0.;
					i__3 = *k;
					for (l = 1; l <= i__3; ++l) {
						temp1 += a_ref (l, i__) * b_ref (l, j);
						temp2 += b_ref (l, i__) * a_ref (l, j);
						/* L220: */
					}
					if (*beta == 0.) {
						c___ref (i__, j) = *alpha * temp1 + *alpha * temp2;
					} else {
						c___ref (i__, j) = *beta * c___ref (i__, j) + *alpha * temp1 + *alpha * temp2;
					}
					/* L230: */
				}
				/* L240: */
			}
		}
	}
	return 0;
}								/* NUMblas_dsyr2k */

#undef c___ref
#undef b_ref
#undef a_ref

int NUMblas_dtrmm (const char *side, const char *uplo, const char *transa, const char *diag, integer *m, integer *n, double *alpha, double *a,
                   integer *lda, double *b, integer *ldb) {
	/* System generated locals */
	integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3;

	/* Local variables */
	static integer info;
	static double temp;
	static integer i__, j, k;
	static integer lside;
	static integer nrowa;
	static integer upper;
	static integer nounit;

#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]
	/* Level 3 Blas routine. -- Written on 8-February-1989. Jack Dongarra,
	   Argonne National Laboratory. Iain Duff, AERE Harwell. Jeremy Du Croz,
	   Numerical Algorithms Group Ltd. Sven Hammarling, Numerical Algorithms
	   Group Ltd. Test the input parameters. Parameter adjustments */
	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;
	/* Function Body */
	lside = lsame_ (side, "L");
	if (lside) {
		nrowa = *m;
	} else {
		nrowa = *n;
	}
	nounit = lsame_ (diag, "N");
	upper = lsame_ (uplo, "U");
	info = 0;
	if (!lside && !lsame_ (side, "R")) {
		info = 1;
	} else if (!upper && !lsame_ (uplo, "L")) {
		info = 2;
	} else if (!lsame_ (transa, "N") && !lsame_ (transa, "T") && !lsame_ (transa, "C")) {
		info = 3;
	} else if (!lsame_ (diag, "U") && !lsame_ (diag, "N")) {
		info = 4;
	} else if (*m < 0) {
		info = 5;
	} else if (*n < 0) {
		info = 6;
	} else if (*lda < MAX (1, nrowa)) {
		info = 9;
	} else if (*ldb < MAX (1, *m)) {
		info = 11;
	}
	if (info != 0) {
		xerbla_ ("DTRMM ", &info);
		return 0;
	}
	/* Quick return if possible. */
	if (*n == 0) {
		return 0;
	}
	/* And when alpha.eq.zero. */
	if (*alpha == 0.) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
				b_ref (i__, j) = 0.;
				/* L10: */
			}
			/* L20: */
		}
		return 0;
	}
	/* Start the operations. */
	if (lside) {
		if (lsame_ (transa, "N")) {
			/* Form B := alpha*A*B. */
			if (upper) {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *m;
					for (k = 1; k <= i__2; ++k) {
						if (b_ref (k, j) != 0.) {
							temp = *alpha * b_ref (k, j);
							i__3 = k - 1;
							for (i__ = 1; i__ <= i__3; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) + temp * a_ref (i__, k);
								/* L30: */
							}
							if (nounit) {
								temp *= a_ref (k, k);
							}
							b_ref (k, j) = temp;
						}
						/* L40: */
					}
					/* L50: */
				}
			} else {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					for (k = *m; k >= 1; --k) {
						if (b_ref (k, j) != 0.) {
							temp = *alpha * b_ref (k, j);
							b_ref (k, j) = temp;
							if (nounit) {
								b_ref (k, j) = b_ref (k, j) * a_ref (k, k);
							}
							i__2 = *m;
							for (i__ = k + 1; i__ <= i__2; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) + temp * a_ref (i__, k);
								/* L60: */
							}
						}
						/* L70: */
					}
					/* L80: */
				}
			}
		} else {
			/* Form B := alpha*A'*B. */
			if (upper) {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					for (i__ = *m; i__ >= 1; --i__) {
						temp = b_ref (i__, j);
						if (nounit) {
							temp *= a_ref (i__, i__);
						}
						i__2 = i__ - 1;
						for (k = 1; k <= i__2; ++k) {
							temp += a_ref (k, i__) * b_ref (k, j);
							/* L90: */
						}
						b_ref (i__, j) = *alpha * temp;
						/* L100: */
					}
					/* L110: */
				}
			} else {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						temp = b_ref (i__, j);
						if (nounit) {
							temp *= a_ref (i__, i__);
						}
						i__3 = *m;
						for (k = i__ + 1; k <= i__3; ++k) {
							temp += a_ref (k, i__) * b_ref (k, j);
							/* L120: */
						}
						b_ref (i__, j) = *alpha * temp;
						/* L130: */
					}
					/* L140: */
				}
			}
		}
	} else {
		if (lsame_ (transa, "N")) {
			/* Form B := alpha*B*A. */
			if (upper) {
				for (j = *n; j >= 1; --j) {
					temp = *alpha;
					if (nounit) {
						temp *= a_ref (j, j);
					}
					i__1 = *m;
					for (i__ = 1; i__ <= i__1; ++i__) {
						b_ref (i__, j) = temp * b_ref (i__, j);
						/* L150: */
					}
					i__1 = j - 1;
					for (k = 1; k <= i__1; ++k) {
						if (a_ref (k, j) != 0.) {
							temp = *alpha * a_ref (k, j);
							i__2 = *m;
							for (i__ = 1; i__ <= i__2; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) + temp * b_ref (i__, k);
								/* L160: */
							}
						}
						/* L170: */
					}
					/* L180: */
				}
			} else {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					temp = *alpha;
					if (nounit) {
						temp *= a_ref (j, j);
					}
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						b_ref (i__, j) = temp * b_ref (i__, j);
						/* L190: */
					}
					i__2 = *n;
					for (k = j + 1; k <= i__2; ++k) {
						if (a_ref (k, j) != 0.) {
							temp = *alpha * a_ref (k, j);
							i__3 = *m;
							for (i__ = 1; i__ <= i__3; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) + temp * b_ref (i__, k);
								/* L200: */
							}
						}
						/* L210: */
					}
					/* L220: */
				}
			}
		} else {
			/* Form B := alpha*B*A'. */
			if (upper) {
				i__1 = *n;
				for (k = 1; k <= i__1; ++k) {
					i__2 = k - 1;
					for (j = 1; j <= i__2; ++j) {
						if (a_ref (j, k) != 0.) {
							temp = *alpha * a_ref (j, k);
							i__3 = *m;
							for (i__ = 1; i__ <= i__3; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) + temp * b_ref (i__, k);
								/* L230: */
							}
						}
						/* L240: */
					}
					temp = *alpha;
					if (nounit) {
						temp *= a_ref (k, k);
					}
					if (temp != 1.) {
						i__2 = *m;
						for (i__ = 1; i__ <= i__2; ++i__) {
							b_ref (i__, k) = temp * b_ref (i__, k);
							/* L250: */
						}
					}
					/* L260: */
				}
			} else {
				for (k = *n; k >= 1; --k) {
					i__1 = *n;
					for (j = k + 1; j <= i__1; ++j) {
						if (a_ref (j, k) != 0.) {
							temp = *alpha * a_ref (j, k);
							i__2 = *m;
							for (i__ = 1; i__ <= i__2; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) + temp * b_ref (i__, k);
								/* L270: */
							}
						}
						/* L280: */
					}
					temp = *alpha;
					if (nounit) {
						temp *= a_ref (k, k);
					}
					if (temp != 1.) {
						i__1 = *m;
						for (i__ = 1; i__ <= i__1; ++i__) {
							b_ref (i__, k) = temp * b_ref (i__, k);
							/* L290: */
						}
					}
					/* L300: */
				}
			}
		}
	}
	return 0;
}								/* NUMblas_dtrmm */

#undef b_ref
#undef a_ref

int NUMblas_dtrmv (const char *uplo, const char *trans, const char *diag, integer *n, double *a, integer *lda, double *x, integer *incx) {
	/* System generated locals */
	integer a_dim1, a_offset, i__1, i__2;

	/* Local variables */
	static integer info;
	static double temp;
	static integer i__, j;
	static integer ix, jx, kx;
	static integer nounit;

#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
	/* -- Written on 22-October-1986. Jack Dongarra, Argonne National Lab.
	   Jeremy Du Croz, Nag Central Office. Sven Hammarling, Nag Central
	   Office. Richard Hanson, Sandia National Labs. Test the input
	   parameters. Parameter adjustments */
	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	--x;
	/* Function Body */
	info = 0;
	if (!lsame_ (uplo, "U") && !lsame_ (uplo, "L")) {
		info = 1;
	} else if (!lsame_ (trans, "N") && !lsame_ (trans, "T") && !lsame_ (trans, "C")) {
		info = 2;
	} else if (!lsame_ (diag, "U") && !lsame_ (diag, "N")) {
		info = 3;
	} else if (*n < 0) {
		info = 4;
	} else if (*lda < MAX (1, *n)) {
		info = 6;
	} else if (*incx == 0) {
		info = 8;
	}
	if (info != 0) {
		xerbla_ ("DTRMV ", &info);
		return 0;
	}
	/* Quick return if possible. */
	if (*n == 0) {
		return 0;
	}
	nounit = lsame_ (diag, "N");
	/* Set up the start point in X if the increment is not unity. This will
	   be ( N - 1 )*INCX too small for descending loops. */
	if (*incx <= 0) {
		kx = 1 - (*n - 1) * *incx;
	} else if (*incx != 1) {
		kx = 1;
	}
	/* Start the operations. In this version the elements of A are accessed
	   sequentially with one pass through A. */
	if (lsame_ (trans, "N")) {
		/* Form x := A*x. */
		if (lsame_ (uplo, "U")) {
			if (*incx == 1) {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					if (x[j] != 0.) {
						temp = x[j];
						i__2 = j - 1;
						for (i__ = 1; i__ <= i__2; ++i__) {
							x[i__] += temp * a_ref (i__, j);
							/* L10: */
						}
						if (nounit) {
							x[j] *= a_ref (j, j);
						}
					}
					/* L20: */
				}
			} else {
				jx = kx;
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					if (x[jx] != 0.) {
						temp = x[jx];
						ix = kx;
						i__2 = j - 1;
						for (i__ = 1; i__ <= i__2; ++i__) {
							x[ix] += temp * a_ref (i__, j);
							ix += *incx;
							/* L30: */
						}
						if (nounit) {
							x[jx] *= a_ref (j, j);
						}
					}
					jx += *incx;
					/* L40: */
				}
			}
		} else {
			if (*incx == 1) {
				for (j = *n; j >= 1; --j) {
					if (x[j] != 0.) {
						temp = x[j];
						i__1 = j + 1;
						for (i__ = *n; i__ >= i__1; --i__) {
							x[i__] += temp * a_ref (i__, j);
							/* L50: */
						}
						if (nounit) {
							x[j] *= a_ref (j, j);
						}
					}
					/* L60: */
				}
			} else {
				kx += (*n - 1) * *incx;
				jx = kx;
				for (j = *n; j >= 1; --j) {
					if (x[jx] != 0.) {
						temp = x[jx];
						ix = kx;
						i__1 = j + 1;
						for (i__ = *n; i__ >= i__1; --i__) {
							x[ix] += temp * a_ref (i__, j);
							ix -= *incx;
							/* L70: */
						}
						if (nounit) {
							x[jx] *= a_ref (j, j);
						}
					}
					jx -= *incx;
					/* L80: */
				}
			}
		}
	} else {
		/* Form x := A'*x. */
		if (lsame_ (uplo, "U")) {
			if (*incx == 1) {
				for (j = *n; j >= 1; --j) {
					temp = x[j];
					if (nounit) {
						temp *= a_ref (j, j);
					}
					for (i__ = j - 1; i__ >= 1; --i__) {
						temp += a_ref (i__, j) * x[i__];
						/* L90: */
					}
					x[j] = temp;
					/* L100: */
				}
			} else {
				jx = kx + (*n - 1) * *incx;
				for (j = *n; j >= 1; --j) {
					temp = x[jx];
					ix = jx;
					if (nounit) {
						temp *= a_ref (j, j);
					}
					for (i__ = j - 1; i__ >= 1; --i__) {
						ix -= *incx;
						temp += a_ref (i__, j) * x[ix];
						/* L110: */
					}
					x[jx] = temp;
					jx -= *incx;
					/* L120: */
				}
			}
		} else {
			if (*incx == 1) {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					temp = x[j];
					if (nounit) {
						temp *= a_ref (j, j);
					}
					i__2 = *n;
					for (i__ = j + 1; i__ <= i__2; ++i__) {
						temp += a_ref (i__, j) * x[i__];
						/* L130: */
					}
					x[j] = temp;
					/* L140: */
				}
			} else {
				jx = kx;
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					temp = x[jx];
					ix = jx;
					if (nounit) {
						temp *= a_ref (j, j);
					}
					i__2 = *n;
					for (i__ = j + 1; i__ <= i__2; ++i__) {
						ix += *incx;
						temp += a_ref (i__, j) * x[ix];
						/* L150: */
					}
					x[jx] = temp;
					jx += *incx;
					/* L160: */
				}
			}
		}
	}
	return 0;
}								/* NUMblas_dtrmv */

#undef a_ref

int NUMblas_dtrsm (const char *side, const char *uplo, const char *transa, const char *diag, integer *m, integer *n,
                   double *alpha, double *a, integer *lda, double *b, integer *ldb) {
	/* System generated locals */
	integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3;

	/* Local variables */
	static integer info;
	static double temp;
	static integer i__, j, k;
	static integer lside;
	static integer nrowa;
	static integer upper;
	static integer nounit;

#define a_ref(a_1,a_2) a[(a_2)*a_dim1 + a_1]
#define b_ref(a_1,a_2) b[(a_2)*b_dim1 + a_1]

	a_dim1 = *lda;
	a_offset = 1 + a_dim1 * 1;
	a -= a_offset;
	b_dim1 = *ldb;
	b_offset = 1 + b_dim1 * 1;
	b -= b_offset;
	/* Function Body */
	lside = lsame_ (side, "L");
	if (lside) {
		nrowa = *m;
	} else {
		nrowa = *n;
	}
	nounit = lsame_ (diag, "N");
	upper = lsame_ (uplo, "U");
	info = 0;
	if (!lside && !lsame_ (side, "R")) {
		info = 1;
	} else if (!upper && !lsame_ (uplo, "L")) {
		info = 2;
	} else if (!lsame_ (transa, "N") && !lsame_ (transa, "T") && !lsame_ (transa, "C")) {
		info = 3;
	} else if (!lsame_ (diag, "U") && !lsame_ (diag, "N")) {
		info = 4;
	} else if (*m < 0) {
		info = 5;
	} else if (*n < 0) {
		info = 6;
	} else if (*lda < MAX (1, nrowa)) {
		info = 9;
	} else if (*ldb < MAX (1, *m)) {
		info = 11;
	}
	if (info != 0) {
		xerbla_ ("DTRSM ", &info);
		return 0;
	}
	/* Quick return if possible. */
	if (*n == 0) {
		return 0;
	}
	/* And when alpha.eq.zero. */
	if (*alpha == 0.) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
				b_ref (i__, j) = 0.;
				/* L10: */
			}
			/* L20: */
		}
		return 0;
	}
	/* Start the operations. */
	if (lside) {
		if (lsame_ (transa, "N")) {
			/* Form B := alpha*inv( A )*B. */
			if (upper) {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					if (*alpha != 1.) {
						i__2 = *m;
						for (i__ = 1; i__ <= i__2; ++i__) {
							b_ref (i__, j) = *alpha * b_ref (i__, j);
							/* L30: */
						}
					}
					for (k = *m; k >= 1; --k) {
						if (b_ref (k, j) != 0.) {
							if (nounit) {
								b_ref (k, j) = b_ref (k, j) / a_ref (k, k);
							}
							i__2 = k - 1;
							for (i__ = 1; i__ <= i__2; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) - b_ref (k, j) * a_ref (i__, k);
								/* L40: */
							}
						}
						/* L50: */
					}
					/* L60: */
				}
			} else {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					if (*alpha != 1.) {
						i__2 = *m;
						for (i__ = 1; i__ <= i__2; ++i__) {
							b_ref (i__, j) = *alpha * b_ref (i__, j);
							/* L70: */
						}
					}
					i__2 = *m;
					for (k = 1; k <= i__2; ++k) {
						if (b_ref (k, j) != 0.) {
							if (nounit) {
								b_ref (k, j) = b_ref (k, j) / a_ref (k, k);
							}
							i__3 = *m;
							for (i__ = k + 1; i__ <= i__3; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) - b_ref (k, j) * a_ref (i__, k);
								/* L80: */
							}
						}
						/* L90: */
					}
					/* L100: */
				}
			}
		} else {
			/* Form B := alpha*inv( A' )*B. */
			if (upper) {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					i__2 = *m;
					for (i__ = 1; i__ <= i__2; ++i__) {
						temp = *alpha * b_ref (i__, j);
						i__3 = i__ - 1;
						for (k = 1; k <= i__3; ++k) {
							temp -= a_ref (k, i__) * b_ref (k, j);
							/* L110: */
						}
						if (nounit) {
							temp /= a_ref (i__, i__);
						}
						b_ref (i__, j) = temp;
						/* L120: */
					}
					/* L130: */
				}
			} else {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					for (i__ = *m; i__ >= 1; --i__) {
						temp = *alpha * b_ref (i__, j);
						i__2 = *m;
						for (k = i__ + 1; k <= i__2; ++k) {
							temp -= a_ref (k, i__) * b_ref (k, j);
							/* L140: */
						}
						if (nounit) {
							temp /= a_ref (i__, i__);
						}
						b_ref (i__, j) = temp;
						/* L150: */
					}
					/* L160: */
				}
			}
		}
	} else {
		if (lsame_ (transa, "N")) {
			/* Form B := alpha*B*inv( A ). */
			if (upper) {
				i__1 = *n;
				for (j = 1; j <= i__1; ++j) {
					if (*alpha != 1.) {
						i__2 = *m;
						for (i__ = 1; i__ <= i__2; ++i__) {
							b_ref (i__, j) = *alpha * b_ref (i__, j);
							/* L170: */
						}
					}
					i__2 = j - 1;
					for (k = 1; k <= i__2; ++k) {
						if (a_ref (k, j) != 0.) {
							i__3 = *m;
							for (i__ = 1; i__ <= i__3; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) - a_ref (k, j) * b_ref (i__, k);
								/* L180: */
							}
						}
						/* L190: */
					}
					if (nounit) {
						temp = 1. / a_ref (j, j);
						i__2 = *m;
						for (i__ = 1; i__ <= i__2; ++i__) {
							b_ref (i__, j) = temp * b_ref (i__, j);
							/* L200: */
						}
					}
					/* L210: */
				}
			} else {
				for (j = *n; j >= 1; --j) {
					if (*alpha != 1.) {
						i__1 = *m;
						for (i__ = 1; i__ <= i__1; ++i__) {
							b_ref (i__, j) = *alpha * b_ref (i__, j);
							/* L220: */
						}
					}
					i__1 = *n;
					for (k = j + 1; k <= i__1; ++k) {
						if (a_ref (k, j) != 0.) {
							i__2 = *m;
							for (i__ = 1; i__ <= i__2; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) - a_ref (k, j) * b_ref (i__, k);
								/* L230: */
							}
						}
						/* L240: */
					}
					if (nounit) {
						temp = 1. / a_ref (j, j);
						i__1 = *m;
						for (i__ = 1; i__ <= i__1; ++i__) {
							b_ref (i__, j) = temp * b_ref (i__, j);
							/* L250: */
						}
					}
					/* L260: */
				}
			}
		} else {
			/* Form B := alpha*B*inv( A' ). */
			if (upper) {
				for (k = *n; k >= 1; --k) {
					if (nounit) {
						temp = 1. / a_ref (k, k);
						i__1 = *m;
						for (i__ = 1; i__ <= i__1; ++i__) {
							b_ref (i__, k) = temp * b_ref (i__, k);
							/* L270: */
						}
					}
					i__1 = k - 1;
					for (j = 1; j <= i__1; ++j) {
						if (a_ref (j, k) != 0.) {
							temp = a_ref (j, k);
							i__2 = *m;
							for (i__ = 1; i__ <= i__2; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) - temp * b_ref (i__, k);
								/* L280: */
							}
						}
						/* L290: */
					}
					if (*alpha != 1.) {
						i__1 = *m;
						for (i__ = 1; i__ <= i__1; ++i__) {
							b_ref (i__, k) = *alpha * b_ref (i__, k);
							/* L300: */
						}
					}
					/* L310: */
				}
			} else {
				i__1 = *n;
				for (k = 1; k <= i__1; ++k) {
					if (nounit) {
						temp = 1. / a_ref (k, k);
						i__2 = *m;
						for (i__ = 1; i__ <= i__2; ++i__) {
							b_ref (i__, k) = temp * b_ref (i__, k);
							/* L320: */
						}
					}
					i__2 = *n;
					for (j = k + 1; j <= i__2; ++j) {
						if (a_ref (j, k) != 0.) {
							temp = a_ref (j, k);
							i__3 = *m;
							for (i__ = 1; i__ <= i__3; ++i__) {
								b_ref (i__, j) = b_ref (i__, j) - temp * b_ref (i__, k);
								/* L330: */
							}
						}
						/* L340: */
					}
					if (*alpha != 1.) {
						i__2 = *m;
						for (i__ = 1; i__ <= i__2; ++i__) {
							b_ref (i__, k) = *alpha * b_ref (i__, k);
							/* L350: */
						}
					}
					/* L360: */
				}
			}
		}
	}
	return 0;
} /* NUMblas_dtrsm */

#undef b_ref
#undef a_ref

integer NUMblas_idamax (integer *n, double *dx, integer *incx) {
	/* System generated locals */
	integer ret_val, i__1;
	double d__1;

	/* Local variables */
	static double dmax__;
	static integer i__, ix;

	/* finds the index of element having max. absolute value. jack
	   dongarra, linpack, 3/11/78. modified 3/93 to return if incx .le. 0.
	   modified 12/3/93, array(1) declarations changed to array(*) Parameter
	   adjustments */
	--dx;
	/* Function Body */
	ret_val = 0;
	if (*n < 1 || *incx <= 0) {
		return ret_val;
	}
	ret_val = 1;
	if (*n == 1) {
		return ret_val;
	}
	if (*incx == 1) {
		goto L20;
	}
	/* code for increment not equal to 1 */
	ix = 1;
	dmax__ = fabs (dx[1]);
	ix += *incx;
	i__1 = *n;
	for (i__ = 2; i__ <= i__1; ++i__) {
		if ( (d__1 = dx[ix], fabs (d__1)) <= dmax__) {
			goto L5;
		}
		ret_val = i__;
		dmax__ = (d__1 = dx[ix], fabs (d__1));
L5:
		ix += *incx;
		/* L10: */
	}
	return ret_val;
	/* code for increment equal to 1 */
L20:
	dmax__ = fabs (dx[1]);
	i__1 = *n;
	for (i__ = 2; i__ <= i__1; ++i__) {
		if ( (d__1 = dx[i__], fabs (d__1)) <= dmax__) {
			goto L30;
		}
		ret_val = i__;
		dmax__ = (d__1 = dx[i__], fabs (d__1));
L30:
		;
	}
	return ret_val;
}								/* NUMblas_idamax */

#undef MAX
#undef MIN

/* End of file NUMcblas.cpp */
