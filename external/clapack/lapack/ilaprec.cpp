#include "clapack.h"
#include "f2cP.h"

integer ilaprec_(const char *prec)
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

/*  This subroutine translated from a character string specifying an */
/*  intermediate precision to the relevant BLAST-specified integer */
/*  constant. */

/*  ILAPREC returns an INTEGER.  If ILAPREC < 0, then the input is not a */
/*  character indicating a supported intermediate precision.  Otherwise */
/*  ILAPREC returns the constant value corresponding to PREC. */

/*  Arguments */
/*  ========= */
/*  PREC   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'S':  Single */
/*          = 'D':  Double */
/*          = 'I':  Indigenous */
/*          = 'X', 'E':  Extra */
/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */
    if (lsame_(prec, "S")) {
	ret_val = 211;
    } else if (lsame_(prec, "D")) {
	ret_val = 212;
    } else if (lsame_(prec, "I")) {
	ret_val = 213;
    } else if (lsame_(prec, "X") || lsame_(prec, "E")) {
	ret_val = 214;
    } else {
	ret_val = -1;
    }
    return ret_val;

/*     End of ILAPREC */

} /* ilaprec_ */
