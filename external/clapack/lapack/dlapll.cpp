#include "clapack.h"
#include "f2cP.h"

/* Subroutine */ int dlapll_(integer *n, double *x, integer *incx, 
	double *y, integer *incy, double *ssmin)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    double c__, a11, a12, a22, tau;
    double ssmax;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Given two column vectors X and Y, let */

/*                       A = ( X Y ). */

/*  The subroutine first computes the QR factorization of A = Q*R, */
/*  and then computes the SVD of the 2-by-2 upper triangular matrix R. */
/*  The smaller singular value of R is returned in SSMIN, which is used */
/*  as the measurement of the linear dependency of the vectors X and Y. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The length of the vectors X and Y. */

/*  X       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCX) */
/*          On entry, X contains the N-vector X. */
/*          On exit, X is overwritten. */

/*  INCX    (input) INTEGER */
/*          The increment between successive elements of X. INCX > 0. */

/*  Y       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCY) */
/*          On entry, Y contains the N-vector Y. */
/*          On exit, Y is overwritten. */

/*  INCY    (input) INTEGER */
/*          The increment between successive elements of Y. INCY > 0. */

/*  SSMIN   (output) DOUBLE PRECISION */
/*          The smallest singular value of the N-by-2 matrix A = ( X Y ). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

    /* Parameter adjustments */
    --y;
    --x;

    /* Function Body */
    if (*n <= 1) {
	*ssmin = 0.;
	return 0;
    }

/*     Compute the QR factorization of the N-by-2 matrix ( X Y ) */

    dlarfg_(n, &x[1], &x[*incx + 1], incx, &tau);
    a11 = x[1];
    x[1] = 1.;

    c__ = -tau * ddot_(n, &x[1], incx, &y[1], incy);
    daxpy_(n, &c__, &x[1], incx, &y[1], incy);

    i__1 = *n - 1;
    dlarfg_(&i__1, &y[*incy + 1], &y[(*incy << 1) + 1], incy, &tau);

    a12 = y[1];
    a22 = y[*incy + 1];

/*     Compute the SVD of 2-by-2 Upper triangular matrix. */

    dlas2_(&a11, &a12, &a22, ssmin, &ssmax);

    return 0;

/*     End of DLAPLL */

} /* dlapll_ */
