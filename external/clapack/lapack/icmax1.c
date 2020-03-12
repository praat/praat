#include "f2c.h"
#include "blaswrap.h"

integer icmax1_(integer *n, complex *cx, integer *incx)
{
    /* System generated locals */
    integer ret_val, i__1;

    /* Builtin functions */
    double c_abs(complex *);

    /* Local variables */
    integer i__, ix;
    real smax;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  ICMAX1 finds the index of the element whose real part has maximum */
/*  absolute value. */

/*  Based on ICAMAX from Level 1 BLAS. */
/*  The change is to use the 'genuine' absolute value. */

/*  Contributed by Nick Higham for use with CLACON. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of elements in the vector CX. */

/*  CX      (input) COMPLEX array, dimension (N) */
/*          The vector whose elements will be summed. */

/*  INCX    (input) INTEGER */
/*          The spacing between successive values of CX.  INCX >= 1. */

/* ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */

/*     NEXT LINE IS THE ONLY MODIFICATION. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --cx;

    /* Function Body */
    ret_val = 0;
    if (*n < 1) {
	return ret_val;
    }
    ret_val = 1;
    if (*n == 1) {
	return ret_val;
    }
    if (*incx == 1) {
	goto L30;
    }

/*     CODE FOR INCREMENT NOT EQUAL TO 1 */

    ix = 1;
    smax = c_abs(&cx[1]);
    ix += *incx;
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	if (c_abs(&cx[ix]) <= smax) {
	    goto L10;
	}
	ret_val = i__;
	smax = c_abs(&cx[ix]);
L10:
	ix += *incx;
/* L20: */
    }
    return ret_val;

/*     CODE FOR INCREMENT EQUAL TO 1 */

L30:
    smax = c_abs(&cx[1]);
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	if (c_abs(&cx[i__]) <= smax) {
	    goto L40;
	}
	ret_val = i__;
	smax = c_abs(&cx[i__]);
L40:
	;
    }
    return ret_val;

/*     End of ICMAX1 */

} /* icmax1_ */
