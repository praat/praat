#include "clapack.h"
#include "f2cP.h"

bool sisnan_(float *sin__)
{
    /* System generated locals */
    bool ret_val;

/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  SISNAN returns .TRUE. if its argument is NaN, and .FALSE. */
/*  otherwise.  To be replaced by the Fortran 2003 intrinsic in the */
/*  future. */

/*  Arguments */
/*  ========= */

/*  SIN      (input) REAL */
/*          Input to test for NaN. */

/*  ===================================================================== */

/*  .. External Functions .. */
/*  .. */
/*  .. Executable Statements .. */
    ret_val = slaisnan_(sin__, sin__);
    return ret_val;
} /* sisnan_ */
