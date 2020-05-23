#include "clapack.h"
#include "f2cP.h"

/* Subroutine */ bool disnan_(double *din)
{
    /* System generated locals */
    bool ret_val;

    /* Local variables */

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DISNAN returns .TRUE. if its argument is NaN, and .FALSE. */
/*  otherwise.  To be replaced by the Fortran 2003 intrinsic in the */
/*  future. */

/*  Arguments */
/*  ========= */

/*  DIN      (input) DOUBLE PRECISION */
/*          Input to test for NaN. */

/*  ===================================================================== */

/*  .. External Functions .. */
/*  .. */
/*  .. Executable Statements .. */
    ret_val = dlaisnan_(din, din);
    return ret_val;
} /* disnan_ */
