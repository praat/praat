#include "clapack.h"
#include "f2cP.h"

int dlascl2_(integer *m, integer *n, double *d__, double *x, integer *ldx)
{
    /* System generated locals */
    integer x_dim1, x_offset, i__1, i__2;

    /* Local variables */
    integer i__, j;


/*     -- LAPACK routine (version 3.2.1)                               -- */
/*     -- Contributed by James Demmel, Deaglan Halligan, Yozo Hida and -- */
/*     -- Jason Riedy of Univ. of California Berkeley.                 -- */
/*     -- April 2009                                                   -- */

/*     -- LAPACK is a software package provided by Univ. of Tennessee, -- */
/*     -- Univ. of California Berkeley and NAG Ltd.                    -- */

/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASCL2 performs a diagonal scaling on a vector: */
/*    x <-- D * x */
/*  where the diagonal matrix D is stored as a vector. */

/*  Eventually to be replaced by BLAS_dge_diag_scale in the new BLAS */
/*  standard. */

/*  Arguments */
/*  ========= */

/*     M       (input) INTEGER */
/*     The number of rows of D and X. M >= 0. */

/*     N       (input) INTEGER */
/*     The number of columns of D and X. N >= 0. */

/*     D       (input) DOUBLE PRECISION array, length M */
/*     Diagonal matrix D, stored as a vector of length M. */

/*     X       (input/output) DOUBLE PRECISION array, dimension (LDX,N) */
/*     On entry, the vector X to be scaled by D. */
/*     On exit, the scaled vector. */

/*     LDX     (input) INTEGER */
/*     The leading dimension of the vector X. LDX >= 0. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --d__;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;

    /* Function Body */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    x[i__ + j * x_dim1] *= d__[i__];
	}
    }
    return 0;
} /* dlascl2_ */
