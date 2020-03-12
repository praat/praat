#include "f2c.h"
#include "string.h"

logical lsamen_(integer *n, char *ca, char *cb)
{
    /* System generated locals */
    integer i__1;
    logical ret_val;

    /* Builtin functions */
    //integer i_len(char *, ftnlen);

    /* Local variables */
    static integer i__;
    extern logical lsame_(char *, char *);

    ftnlen ca_len, cb_len;

    ca_len = strlen (ca);
    cb_len = strlen (cb);


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  LSAMEN  tests if the first N letters of CA are the same as the */
/*  first N letters of CB, regardless of case. */
/*  LSAMEN returns .TRUE. if CA and CB are equivalent except for case */
/*  and .FALSE. otherwise.  LSAMEN also returns .FALSE. if LEN( CA ) */
/*  or LEN( CB ) is less than N. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of characters in CA and CB to be compared. */

/*  CA      (input) CHARACTER*(*) */
/*  CB      (input) CHARACTER*(*) */
/*          CA and CB specify two character strings of length at least N. */
/*          Only the first N characters of each string will be accessed. */

/* ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    ret_val = FALSE_;
    if (strlen(ca) < *n || strlen(cb) < *n) {
	goto L20;
    }

/*     Do for each character in the two strings. */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Test if the characters are equal using LSAME. */

	if (! lsame_(ca + (i__ - 1), cb + (i__ - 1))) {
	    goto L20;
	}

/* L10: */
    }
    ret_val = TRUE_;

L20:
    return ret_val;

/*     End of LSAMEN */

} /* lsamen_ */
