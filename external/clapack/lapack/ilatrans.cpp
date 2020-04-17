#include "clapack.h"
#include "f2cP.h"

integer ilatrans_(const char *trans)
{
    /* System generated locals */
    integer ret_val;

/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     October 2008 */
/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This subroutine translates from a character string specifying a */
/*  transposition operation to the relevant BLAST-specified integer */
/*  constant. */

/*  ILATRANS returns an INTEGER.  If ILATRANS < 0, then the input is not */
/*  a character indicating a transposition operator.  Otherwise ILATRANS */
/*  returns the constant value corresponding to TRANS. */

/*  Arguments */
/*  ========= */
/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  No transpose */
/*          = 'T':  Transpose */
/*          = 'C':  Conjugate transpose */
/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */
    if (lsame_(trans, "N")) {
	ret_val = 111;
    } else if (lsame_(trans, "T")) {
	ret_val = 112;
    } else if (lsame_(trans, "C")) {
	ret_val = 113;
    } else {
	ret_val = -1;
    }
    return ret_val;

/*     End of ILATRANS */

} /* ilatrans_ */
