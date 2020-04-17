#include "clapack.h"
#include "f2cP.h"

void chla_transtype__(char *ret_val, integer ret_val_len, integer *trans)
{

/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     October 2008 */
/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This subroutine translates from a BLAST-specified integer constant to */
/*  the character string specifying a transposition operation. */

/*  CHLA_TRANSTYPE returns an CHARACTER*1.  If CHLA_TRANSTYPE is 'X', */
/*  then input is not an integer indicating a transposition operator. */
/*  Otherwise CHLA_TRANSTYPE returns the constant value corresponding to */
/*  TRANS. */

/*  Arguments */
/*  ========= */
/*  TRANS   (input) INTEGER */
/*          Specifies the form of the system of equations: */
/*          = BLAS_NO_TRANS   = 111 :  No Transpose */
/*          = BLAS_TRANS      = 112 :  Transpose */
/*          = BLAS_CONJ_TRANS = 113 :  Conjugate Transpose */
/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Executable Statements .. */
    if (*trans == 111) {
	*(unsigned char *)ret_val = 'N';
    } else if (*trans == 112) {
	*(unsigned char *)ret_val = 'T';
    } else if (*trans == 113) {
	*(unsigned char *)ret_val = 'C';
    } else {
	*(unsigned char *)ret_val = 'X';
    }
    return ;

/*     End of CHLA_TRANSTYPE */

} /* chla_transtype__ */
