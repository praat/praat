#include "f2c.h"
#include "blaswrap.h"

/* Subroutine */ int ilaver_(integer *vers_major__, integer *vers_minor__, 
	integer *vers_patch__)
{

/*  -- LAPACK routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This subroutine return the Lapack version. */

/*  Arguments */
/*  ========= */

/*  VERS_MAJOR   (output) INTEGER */
/*      return the lapack major version */
/*  VERS_MINOR   (output) INTEGER */
/*      return the lapack minor version from the major version */
/*  VERS_PATCH   (output) INTEGER */
/*      return the lapack patch version from the minor version */

/*     .. Executable Statements .. */

    *vers_major__ = 3;
    *vers_minor__ = 1;
    *vers_patch__ = 1;
/*  ===================================================================== */

    return 0;
} /* ilaver_ */
