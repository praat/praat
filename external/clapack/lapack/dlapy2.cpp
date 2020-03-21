#include "clapack.h"
#include "blaswrap.h"

double dlapy2_(double *x, double *y)
{
    /* System generated locals */
    double ret_val, d__1;

    /* Builtin functions 
    double sqrt(double); */

    /* Local variables */
    double w, z__, xabs, yabs;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAPY2 returns sqrt(x**2+y**2), taking care not to cause unnecessary */
/*  overflow. */

/*  Arguments */
/*  ========= */

/*  X       (input) DOUBLE PRECISION */
/*  Y       (input) DOUBLE PRECISION */
/*          X and Y specify the values x and y. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    xabs = abs(*x);
    yabs = abs(*y);
    w = std::max(xabs,yabs);
    z__ = std::min(xabs,yabs);
    if (z__ == 0.) {
	ret_val = w;
    } else {
/* Computing 2nd power */
	d__1 = z__ / w;
	ret_val = w * sqrt(d__1 * d__1 + 1.);
    }
    return ret_val;

/*     End of DLAPY2 */

} /* dlapy2_ */
