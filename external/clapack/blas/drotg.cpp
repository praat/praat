#include "cblas.h"
#include "blaswrap.h"

/* Table of constant values */

static double c_b4 = 1.;

/* Subroutine */ int drotg_(double *da, double *db, double *c__, 
	double *s)
{
    /* System generated locals */
    double d__1, d__2;

    /* Builtin functions 
    double sqrt(double), d_sign(double *, double *);*/

    /* Local variables */
    double r__, z__, roe, scale;

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*     construct givens plane rotation. */
/*     jack dongarra, linpack, 3/11/78. */


/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
    roe = *db;
    if (abs(*da) > abs(*db)) {
	roe = *da;
    }
    scale = abs(*da) + abs(*db);
    if (scale != 0.) {
	goto L10;
    }
    *c__ = 1.;
    *s = 0.;
    r__ = 0.;
    z__ = 0.;
    goto L20;
L10:
/* Computing 2nd power */
    d__1 = *da / scale;
/* Computing 2nd power */
    d__2 = *db / scale;
    r__ = scale * sqrt(d__1 * d__1 + d__2 * d__2);
    r__ = d_sign(&c_b4, &roe) * r__;
    *c__ = *da / r__;
    *s = *db / r__;
    z__ = 1.;
    if (abs(*da) > abs(*db)) {
	z__ = *s;
    }
    if (abs(*db) >= abs(*da) && *c__ != 0.) {
	z__ = 1. / *c__;
    }
L20:
    *da = r__;
    *db = z__;
    return 0;
} /* drotg_ */
