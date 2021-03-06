#include "clapack.h"
#include "f2cP.h"

/* Subroutine */ int dlar1v_(integer *n, integer *b1, integer *bn, double
	*lambda, double *d__, double *l, double *ld, double *
	lld, double *pivmin, double *gaptol, double *z__, bool
	*wantnc, integer *negcnt, double *ztz, double *mingma,
	integer *r__, integer *isuppz, double *nrminv, double *resid,
	double *rqcorr, double *work)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2, d__3;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    integer i__;
    double s;
    integer r1, r2;
    double eps, tmp;
    integer neg1, neg2, indp, inds;
    double dplus;


    integer indlpl, indumn;
    double dminus;
    bool sawnan1, sawnan2;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAR1V computes the (scaled) r-th column of the inverse of */
/*  the sumbmatrix in rows B1 through BN of the tridiagonal matrix */
/*  L D L^T - sigma I. When sigma is close to an eigenvalue, the */
/*  computed vector is an accurate eigenvector. Usually, r corresponds */
/*  to the index where the eigenvector is largest in magnitude. */
/*  The following steps accomplish this computation : */
/*  (a) Stationary qd transform,  L D L^T - sigma I = L(+) D(+) L(+)^T, */
/*  (b) Progressive qd transform, L D L^T - sigma I = U(-) D(-) U(-)^T, */
/*  (c) Computation of the diagonal elements of the inverse of */
/*      L D L^T - sigma I by combining the above transforms, and choosing */
/*      r as the index where the diagonal of the inverse is (one of the) */
/*      largest in magnitude. */
/*  (d) Computation of the (scaled) r-th column of the inverse using the */
/*      twisted factorization obtained by combining the top part of the */
/*      the stationary and the bottom part of the progressive transform. */

/*  Arguments */
/*  ========= */

/*  N        (input) INTEGER */
/*           The order of the matrix L D L^T. */

/*  B1       (input) INTEGER */
/*           First index of the submatrix of L D L^T. */

/*  BN       (input) INTEGER */
/*           Last index of the submatrix of L D L^T. */

/*  LAMBDA    (input) DOUBLE PRECISION */
/*           The shift. In order to compute an accurate eigenvector, */
/*           LAMBDA should be a good approximation to an eigenvalue */
/*           of L D L^T. */

/*  L        (input) DOUBLE PRECISION array, dimension (N-1) */
/*           The (n-1) subdiagonal elements of the unit bidiagonal matrix */
/*           L, in elements 1 to N-1. */

/*  D        (input) DOUBLE PRECISION array, dimension (N) */
/*           The n diagonal elements of the diagonal matrix D. */

/*  LD       (input) DOUBLE PRECISION array, dimension (N-1) */
/*           The n-1 elements L(i)*D(i). */

/*  LLD      (input) DOUBLE PRECISION array, dimension (N-1) */
/*           The n-1 elements L(i)*L(i)*D(i). */

/*  PIVMIN   (input) DOUBLE PRECISION */
/*           The minimum pivot in the Sturm sequence. */

/*  GAPTOL   (input) DOUBLE PRECISION */
/*           Tolerance that indicates when eigenvector entries are negligible */
/*           w.r.t. their contribution to the residual. */

/*  Z        (input/output) DOUBLE PRECISION array, dimension (N) */
/*           On input, all entries of Z must be set to 0. */
/*           On output, Z contains the (scaled) r-th column of the */
/*           inverse. The scaling is such that Z(R) equals 1. */

/*  WANTNC   (input) LOGICAL */
/*           Specifies whether NEGCNT has to be computed. */

/*  NEGCNT   (output) INTEGER */
/*           If WANTNC is .TRUE. then NEGCNT = the number of pivots < pivmin */
/*           in the  matrix factorization L D L^T, and NEGCNT = -1 otherwise. */

/*  ZTZ      (output) DOUBLE PRECISION */
/*           The square of the 2-norm of Z. */

/*  MINGMA   (output) DOUBLE PRECISION */
/*           The reciprocal of the largest (in magnitude) diagonal */
/*           element of the inverse of L D L^T - sigma I. */

/*  R        (input/output) INTEGER */
/*           The twist index for the twisted factorization used to */
/*           compute Z. */
/*           On input, 0 <= R <= N. If R is input as 0, R is set to */
/*           the index where (L D L^T - sigma I)^{-1} is largest */
/*           in magnitude. If 1 <= R <= N, R is unchanged. */
/*           On output, R contains the twist index used to compute Z. */
/*           Ideally, R designates the position of the maximum entry in the */
/*           eigenvector. */

/*  ISUPPZ   (output) INTEGER array, dimension (2) */
/*           The support of the vector in Z, i.e., the vector Z is */
/*           nonzero only in elements ISUPPZ(1) through ISUPPZ( 2 ). */

/*  NRMINV   (output) DOUBLE PRECISION */
/*           NRMINV = 1/SQRT( ZTZ ) */

/*  RESID    (output) DOUBLE PRECISION */
/*           The residual of the FP vector. */
/*           RESID = ABS( MINGMA )/SQRT( ZTZ ) */

/*  RQCORR   (output) DOUBLE PRECISION */
/*           The Rayleigh Quotient correction to LAMBDA. */
/*           RQCORR = MINGMA*TMP */

/*  WORK     (workspace) DOUBLE PRECISION array, dimension (4*N) */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Beresford Parlett, University of California, Berkeley, USA */
/*     Jim Demmel, University of California, Berkeley, USA */
/*     Inderjit Dhillon, University of Texas, Austin, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --work;
    --isuppz;
    --z__;
    --lld;
    --ld;
    --l;
    --d__;

    /* Function Body */
    eps = dlamch_("Precision");
    if (*r__ == 0) {
	r1 = *b1;
	r2 = *bn;
    } else {
	r1 = *r__;
	r2 = *r__;
    }
/*     Storage for LPLUS */
    indlpl = 0;
/*     Storage for UMINUS */
    indumn = *n;
    inds = (*n << 1) + 1;
    indp = *n * 3 + 1;
    if (*b1 == 1) {
	work[inds] = 0.;
    } else {
	work[inds + *b1 - 1] = lld[*b1 - 1];
    }

/*     Compute the stationary transform (using the differential form) */
/*     until the index R2. */

    sawnan1 = false;
    neg1 = 0;
    s = work[inds + *b1 - 1] - *lambda;
    i__1 = r1 - 1;
    for (i__ = *b1; i__ <= i__1; ++i__) {
	dplus = d__[i__] + s;
	work[indlpl + i__] = ld[i__] / dplus;
	if (dplus < 0.) {
	    ++neg1;
	}
	work[inds + i__] = s * work[indlpl + i__] * l[i__];
	s = work[inds + i__] - *lambda;
/* L50: */
    }
    sawnan1 = disnan_(&s);
    if (sawnan1) {
	goto L60;
    }
    i__1 = r2 - 1;
    for (i__ = r1; i__ <= i__1; ++i__) {
	dplus = d__[i__] + s;
	work[indlpl + i__] = ld[i__] / dplus;
	work[inds + i__] = s * work[indlpl + i__] * l[i__];
	s = work[inds + i__] - *lambda;
/* L51: */
    }
    sawnan1 = disnan_(&s);

L60:
    if (sawnan1) {
/*        Runs a slower version of the above loop if a NaN is detected */
	neg1 = 0;
	s = work[inds + *b1 - 1] - *lambda;
	i__1 = r1 - 1;
	for (i__ = *b1; i__ <= i__1; ++i__) {
	    dplus = d__[i__] + s;
	    if (abs(dplus) < *pivmin) {
		dplus = -(*pivmin);
	    }
	    work[indlpl + i__] = ld[i__] / dplus;
	    if (dplus < 0.) {
		++neg1;
	    }
	    work[inds + i__] = s * work[indlpl + i__] * l[i__];
	    if (work[indlpl + i__] == 0.) {
		work[inds + i__] = lld[i__];
	    }
	    s = work[inds + i__] - *lambda;
/* L70: */
	}
	i__1 = r2 - 1;
	for (i__ = r1; i__ <= i__1; ++i__) {
	    dplus = d__[i__] + s;
	    if (abs(dplus) < *pivmin) {
		dplus = -(*pivmin);
	    }
	    work[indlpl + i__] = ld[i__] / dplus;
	    work[inds + i__] = s * work[indlpl + i__] * l[i__];
	    if (work[indlpl + i__] == 0.) {
		work[inds + i__] = lld[i__];
	    }
	    s = work[inds + i__] - *lambda;
/* L71: */
	}
    }

/*     Compute the progressive transform (using the differential form) */
/*     until the index R1 */

    sawnan2 = false;
    neg2 = 0;
    work[indp + *bn - 1] = d__[*bn] - *lambda;
    i__1 = r1;
    for (i__ = *bn - 1; i__ >= i__1; --i__) {
	dminus = lld[i__] + work[indp + i__];
	tmp = d__[i__] / dminus;
	if (dminus < 0.) {
	    ++neg2;
	}
	work[indumn + i__] = l[i__] * tmp;
	work[indp + i__ - 1] = work[indp + i__] * tmp - *lambda;
/* L80: */
    }
    tmp = work[indp + r1 - 1];
    sawnan2 = disnan_(&tmp);
    if (sawnan2) {
/*        Runs a slower version of the above loop if a NaN is detected */
	neg2 = 0;
	i__1 = r1;
	for (i__ = *bn - 1; i__ >= i__1; --i__) {
	    dminus = lld[i__] + work[indp + i__];
	    if (abs(dminus) < *pivmin) {
		dminus = -(*pivmin);
	    }
	    tmp = d__[i__] / dminus;
	    if (dminus < 0.) {
		++neg2;
	    }
	    work[indumn + i__] = l[i__] * tmp;
	    work[indp + i__ - 1] = work[indp + i__] * tmp - *lambda;
	    if (tmp == 0.) {
		work[indp + i__ - 1] = d__[i__] - *lambda;
	    }
/* L100: */
	}
    }

/*     Find the index (from R1 to R2) of the largest (in magnitude) */
/*     diagonal element of the inverse */

    *mingma = work[inds + r1 - 1] + work[indp + r1 - 1];
    if (*mingma < 0.) {
	++neg1;
    }
    if (*wantnc) {
	*negcnt = neg1 + neg2;
    } else {
	*negcnt = -1;
    }
    if (abs(*mingma) == 0.) {
	*mingma = eps * work[inds + r1 - 1];
    }
    *r__ = r1;
    i__1 = r2 - 1;
    for (i__ = r1; i__ <= i__1; ++i__) {
	tmp = work[inds + i__] + work[indp + i__];
	if (tmp == 0.) {
	    tmp = eps * work[inds + i__];
	}
	if (abs(tmp) <= abs(*mingma)) {
	    *mingma = tmp;
	    *r__ = i__ + 1;
	}
/* L110: */
    }

/*     Compute the FP vector: solve N^T v = e_r */

    isuppz[1] = *b1;
    isuppz[2] = *bn;
    z__[*r__] = 1.;
    *ztz = 1.;

/*     Compute the FP vector upwards from R */

    if (! sawnan1 && ! sawnan2) {
	i__1 = *b1;
	for (i__ = *r__ - 1; i__ >= i__1; --i__) {
	    z__[i__] = -(work[indlpl + i__] * z__[i__ + 1]);
	    if (((d__1 = z__[i__], abs(d__1)) + (d__2 = z__[i__ + 1], abs(
		    d__2))) * (d__3 = ld[i__], abs(d__3)) < *gaptol) {
		z__[i__] = 0.;
		isuppz[1] = i__ + 1;
		goto L220;
	    }
	    *ztz += z__[i__] * z__[i__];
/* L210: */
	}
L220:
	;
    } else {
/*        Run slower loop if NaN occurred. */
	i__1 = *b1;
	for (i__ = *r__ - 1; i__ >= i__1; --i__) {
	    if (z__[i__ + 1] == 0.) {
		z__[i__] = -(ld[i__ + 1] / ld[i__]) * z__[i__ + 2];
	    } else {
		z__[i__] = -(work[indlpl + i__] * z__[i__ + 1]);
	    }
	    if (((d__1 = z__[i__], abs(d__1)) + (d__2 = z__[i__ + 1], abs(
		    d__2))) * (d__3 = ld[i__], abs(d__3)) < *gaptol) {
		z__[i__] = 0.;
		isuppz[1] = i__ + 1;
		goto L240;
	    }
	    *ztz += z__[i__] * z__[i__];
/* L230: */
	}
L240:
	;
    }
/*     Compute the FP vector downwards from R in blocks of size BLKSIZ */
    if (! sawnan1 && ! sawnan2) {
	i__1 = *bn - 1;
	for (i__ = *r__; i__ <= i__1; ++i__) {
	    z__[i__ + 1] = -(work[indumn + i__] * z__[i__]);
	    if (((d__1 = z__[i__], abs(d__1)) + (d__2 = z__[i__ + 1], abs(
		    d__2))) * (d__3 = ld[i__], abs(d__3)) < *gaptol) {
		z__[i__ + 1] = 0.;
		isuppz[2] = i__;
		goto L260;
	    }
	    *ztz += z__[i__ + 1] * z__[i__ + 1];
/* L250: */
	}
L260:
	;
    } else {
/*        Run slower loop if NaN occurred. */
	i__1 = *bn - 1;
	for (i__ = *r__; i__ <= i__1; ++i__) {
	    if (z__[i__] == 0.) {
		z__[i__ + 1] = -(ld[i__ - 1] / ld[i__]) * z__[i__ - 1];
	    } else {
		z__[i__ + 1] = -(work[indumn + i__] * z__[i__]);
	    }
	    if (((d__1 = z__[i__], abs(d__1)) + (d__2 = z__[i__ + 1], abs(
		    d__2))) * (d__3 = ld[i__], abs(d__3)) < *gaptol) {
		z__[i__ + 1] = 0.;
		isuppz[2] = i__;
		goto L280;
	    }
	    *ztz += z__[i__ + 1] * z__[i__ + 1];
/* L270: */
	}
L280:
	;
    }

/*     Compute quantities for convergence test */

    tmp = 1. / *ztz;
    *nrminv = sqrt(tmp);
    *resid = abs(*mingma) * *nrminv;
    *rqcorr = *mingma * tmp;


    return 0;

/*     End of DLAR1V */

} /* dlar1v_ */

/* Subroutine */ int dlar2v_(integer *n, double *x, double *y,
	double *z__, integer *incx, double *c__, double *s,
	integer *incc)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__;
    double t1, t2, t3, t4, t5, t6;
    integer ic;
    double ci, si;
    integer ix;
    double xi, yi, zi;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAR2V applies a vector of real plane rotations from both sides to */
/*  a sequence of 2-by-2 real symmetric matrices, defined by the elements */
/*  of the vectors x, y and z. For i = 1,2,...,n */

/*     ( x(i)  z(i) ) := (  c(i)  s(i) ) ( x(i)  z(i) ) ( c(i) -s(i) ) */
/*     ( z(i)  y(i) )    ( -s(i)  c(i) ) ( z(i)  y(i) ) ( s(i)  c(i) ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of plane rotations to be applied. */

/*  X       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCX) */
/*          The vector x. */

/*  Y       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCX) */
/*          The vector y. */

/*  Z       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCX) */
/*          The vector z. */

/*  INCX    (input) INTEGER */
/*          The increment between elements of X, Y and Z. INCX > 0. */

/*  C       (input) DOUBLE PRECISION array, dimension (1+(N-1)*INCC) */
/*          The cosines of the plane rotations. */

/*  S       (input) DOUBLE PRECISION array, dimension (1+(N-1)*INCC) */
/*          The sines of the plane rotations. */

/*  INCC    (input) INTEGER */
/*          The increment between elements of C and S. INCC > 0. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --s;
    --c__;
    --z__;
    --y;
    --x;

    /* Function Body */
    ix = 1;
    ic = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	xi = x[ix];
	yi = y[ix];
	zi = z__[ix];
	ci = c__[ic];
	si = s[ic];
	t1 = si * zi;
	t2 = ci * zi;
	t3 = t2 - si * xi;
	t4 = t2 + si * yi;
	t5 = ci * xi + t1;
	t6 = ci * yi - t1;
	x[ix] = ci * t5 + si * t4;
	y[ix] = ci * t6 - si * t3;
	z__[ix] = ci * t4 - si * t5;
	ix += *incx;
	ic += *incc;
/* L10: */
    }

/*     End of DLAR2V */

    return 0;
} /* dlar2v_ */

/* Subroutine */ int dlarf_(const char *side, integer *m, integer *n, double *v, integer *incv, double *tau, double *c__,
	integer *ldc, double *work)
{
	/* Table of constant values */
	static double c_b4 = 1.;
	static double c_b5 = 0.;
	static integer c__1 = 1;

    /* System generated locals */
    integer c_dim1, c_offset;
    double d__1;

    /* Local variables */
    integer i__;
    bool applyleft;
	integer lastc, lastv;


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARF applies a real elementary reflector H to a real m by n matrix */
/*  C, from either the left or the right. H is represented in the form */

/*        H = I - tau * v * v' */

/*  where tau is a real scalar and v is a real vector. */

/*  If tau = 0, then H is taken to be the unit matrix. */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'L': form  H * C */
/*          = 'R': form  C * H */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix C. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix C. */

/*  V       (input) DOUBLE PRECISION array, dimension */
/*                     (1 + (M-1)*abs(INCV)) if SIDE = 'L' */
/*                  or (1 + (N-1)*abs(INCV)) if SIDE = 'R' */
/*          The vector v in the representation of H. V is not used if */
/*          TAU = 0. */

/*  INCV    (input) INTEGER */
/*          The increment between elements of v. INCV <> 0. */

/*  TAU     (input) DOUBLE PRECISION */
/*          The value tau in the representation of H. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC,N) */
/*          On entry, the m by n matrix C. */
/*          On exit, C is overwritten by the matrix H * C if SIDE = 'L', */
/*          or C * H if SIDE = 'R'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C. LDC >= max(1,M). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                         (N) if SIDE = 'L' */
/*                      or (M) if SIDE = 'R' */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --v;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    applyleft = lsame_(side, "L");
    lastv = 0;
    lastc = 0;
    if (*tau != 0.) {
/*     Set up variables for scanning V.  LASTV begins pointing to the end */
/*     of V. */
	if (applyleft) {
	    lastv = *m;
	} else {
	    lastv = *n;
	}
	if (*incv > 0) {
	    i__ = (lastv - 1) * *incv + 1;
	} else {
	    i__ = 1;
	}
/*     Look for the last non-zero row in V. */
	while(lastv > 0 && v[i__] == 0.) {
	    --lastv;
	    i__ -= *incv;
	}
	if (applyleft) {
/*     Scan for the last non-zero column in C(1:lastv,:). */
	    lastc = iladlc_(&lastv, n, &c__[c_offset], ldc);
	} else {
/*     Scan for the last non-zero row in C(:,1:lastv). */
	    lastc = iladlr_(m, &lastv, &c__[c_offset], ldc);
	}
    }
/*     Note that lastc.eq.0 renders the BLAS operations null; no special */
/*     case is needed at this level. */
    if (applyleft) {

/*        Form  H * C */

	if (lastv > 0) {

/*           w(1:lastc,1) := C(1:lastv,1:lastc)' * v(1:lastv,1) */

	    dgemv_("Transpose", &lastv, &lastc, &c_b4, &c__[c_offset], ldc, &
		    v[1], incv, &c_b5, &work[1], &c__1);

/*           C(1:lastv,1:lastc) := C(...) - v(1:lastv,1) * w(1:lastc,1)' */

	    d__1 = -(*tau);
	    dger_(&lastv, &lastc, &d__1, &v[1], incv, &work[1], &c__1, &c__[
		    c_offset], ldc);
	}
    } else {

/*        Form  C * H */

	if (lastv > 0) {

/*           w(1:lastc,1) := C(1:lastc,1:lastv) * v(1:lastv,1) */

	    dgemv_("No transpose", &lastc, &lastv, &c_b4, &c__[c_offset], ldc,
		     &v[1], incv, &c_b5, &work[1], &c__1);

/*           C(1:lastc,1:lastv) := C(...) - w(1:lastc,1) * v(1:lastv,1)' */

	    d__1 = -(*tau);
	    dger_(&lastc, &lastv, &d__1, &work[1], &c__1, &v[1], incv, &c__[
		    c_offset], ldc);
	}
    }
    return 0;

/*     End of DLARF */

} /* dlarf_ */

/* Subroutine */ int dlarfb_(const char *side, const char *trans, const char *direct, const char *storev, integer *m,
	integer *n, integer *k, double *v, integer *ldv, double *t, integer *ldt, double *c__,
	integer *ldc, double *work, integer *ldwork)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b14 = 1.;
	static double c_b25 = -1.;

    /* System generated locals */
    integer c_dim1, c_offset, t_dim1, t_offset, v_dim1, v_offset, work_dim1,
	    work_offset, i__1, i__2;

    /* Local variables */
    integer i__, j;
    integer lastc;
    integer lastv;
	char transt[1];


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARFB applies a real block reflector H or its transpose H' to a */
/*  real m by n matrix C, from either the left or the right. */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'L': apply H or H' from the Left */
/*          = 'R': apply H or H' from the Right */

/*  TRANS   (input) CHARACTER*1 */
/*          = 'N': apply H (No transpose) */
/*          = 'T': apply H' (Transpose) */

/*  DIRECT  (input) CHARACTER*1 */
/*          Indicates how H is formed from a product of elementary */
/*          reflectors */
/*          = 'F': H = H(1) H(2) . . . H(k) (Forward) */
/*          = 'B': H = H(k) . . . H(2) H(1) (Backward) */

/*  STOREV  (input) CHARACTER*1 */
/*          Indicates how the vectors which define the elementary */
/*          reflectors are stored: */
/*          = 'C': Columnwise */
/*          = 'R': Rowwise */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix C. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix C. */

/*  K       (input) INTEGER */
/*          The order of the matrix T (= the number of elementary */
/*          reflectors whose product defines the block reflector). */

/*  V       (input) DOUBLE PRECISION array, dimension */
/*                                (LDV,K) if STOREV = 'C' */
/*                                (LDV,M) if STOREV = 'R' and SIDE = 'L' */
/*                                (LDV,N) if STOREV = 'R' and SIDE = 'R' */
/*          The matrix V. See further details. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. */
/*          If STOREV = 'C' and SIDE = 'L', LDV >= max(1,M); */
/*          if STOREV = 'C' and SIDE = 'R', LDV >= max(1,N); */
/*          if STOREV = 'R', LDV >= K. */

/*  T       (input) DOUBLE PRECISION array, dimension (LDT,K) */
/*          The triangular k by k matrix T in the representation of the */
/*          block reflector. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T. LDT >= K. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC,N) */
/*          On entry, the m by n matrix C. */
/*          On exit, C is overwritten by H*C or H'*C or C*H or C*H'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C. LDA >= max(1,M). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LDWORK,K) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of the array WORK. */
/*          If SIDE = 'L', LDWORK >= max(1,N); */
/*          if SIDE = 'R', LDWORK >= max(1,M). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

    /* Parameter adjustments */
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    work_dim1 = *ldwork;
    work_offset = 1 + work_dim1;
    work -= work_offset;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	return 0;
    }

    if (lsame_(trans, "N")) {
	*(unsigned char *)transt = 'T';
    } else {
	*(unsigned char *)transt = 'N';
    }

    if (lsame_(storev, "C")) {

	if (lsame_(direct, "F")) {

/*           Let  V =  ( V1 )    (first K rows) */
/*                     ( V2 ) */
/*           where  V1  is unit lower triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 ) */
/*                                                  ( C2 ) */

/* Computing MAX */
		i__1 = *k, i__2 = iladlr_(m, k, &v[v_offset], ldv);
		lastv = std::max(i__1,i__2);
		lastc = iladlc_(&lastv, n, &c__[c_offset], ldc);

/*              W := C' * V  =  (C1'*V1 + C2'*V2)  (stored in WORK) */

/*              W := C1' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(&lastc, &c__[j + c_dim1], ldc, &work[j * work_dim1
			    + 1], &c__1);
/* L10: */
		}

/*              W := W * V1 */

		dtrmm_("Right", "Lower", "No transpose", "Unit", &lastc, k, &
			c_b14, &v[v_offset], ldv, &work[work_offset], ldwork);
		if (lastv > *k) {

/*                 W := W + C2'*V2 */

		    i__1 = lastv - *k;
		    dgemm_("Transpose", "No transpose", &lastc, k, &i__1, &
			    c_b14, &c__[*k + 1 + c_dim1], ldc, &v[*k + 1 +
			    v_dim1], ldv, &c_b14, &work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		dtrmm_("Right", "Upper", transt, "Non-unit", &lastc, k, &
			c_b14, &t[t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V * W' */

		if (lastv > *k) {

/*                 C2 := C2 - V2 * W' */

		    i__1 = lastv - *k;
		    dgemm_("No transpose", "Transpose", &i__1, &lastc, k, &
			    c_b25, &v[*k + 1 + v_dim1], ldv, &work[
			    work_offset], ldwork, &c_b14, &c__[*k + 1 +
			    c_dim1], ldc);
		}

/*              W := W * V1' */

		dtrmm_("Right", "Lower", "Transpose", "Unit", &lastc, k, &
			c_b14, &v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = lastc;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[j + i__ * c_dim1] -= work[i__ + j * work_dim1];
/* L20: */
		    }
/* L30: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 ) */

/* Computing MAX */
		i__1 = *k, i__2 = iladlr_(n, k, &v[v_offset], ldv);
		lastv = std::max(i__1,i__2);
		lastc = iladlr_(m, &lastv, &c__[c_offset], ldc);

/*              W := C * V  =  (C1*V1 + C2*V2)  (stored in WORK) */

/*              W := C1 */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(&lastc, &c__[j * c_dim1 + 1], &c__1, &work[j *
			    work_dim1 + 1], &c__1);
/* L40: */
		}

/*              W := W * V1 */

		dtrmm_("Right", "Lower", "No transpose", "Unit", &lastc, k, &
			c_b14, &v[v_offset], ldv, &work[work_offset], ldwork);
		if (lastv > *k) {

/*                 W := W + C2 * V2 */

		    i__1 = lastv - *k;
		    dgemm_("No transpose", "No transpose", &lastc, k, &i__1, &
			    c_b14, &c__[(*k + 1) * c_dim1 + 1], ldc, &v[*k +
			    1 + v_dim1], ldv, &c_b14, &work[work_offset],
			    ldwork);
		}

/*              W := W * T  or  W * T' */

		dtrmm_("Right", "Upper", trans, "Non-unit", &lastc, k, &c_b14,
			 &t[t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V' */

		if (lastv > *k) {

/*                 C2 := C2 - W * V2' */

		    i__1 = lastv - *k;
		    dgemm_("No transpose", "Transpose", &lastc, &i__1, k, &
			    c_b25, &work[work_offset], ldwork, &v[*k + 1 +
			    v_dim1], ldv, &c_b14, &c__[(*k + 1) * c_dim1 + 1],
			     ldc);
		}

/*              W := W * V1' */

		dtrmm_("Right", "Lower", "Transpose", "Unit", &lastc, k, &
			c_b14, &v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = lastc;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[i__ + j * c_dim1] -= work[i__ + j * work_dim1];
/* L50: */
		    }
/* L60: */
		}
	    }

	} else {

/*           Let  V =  ( V1 ) */
/*                     ( V2 )    (last K rows) */
/*           where  V2  is unit upper triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 ) */
/*                                                  ( C2 ) */

/* Computing MAX */
		i__1 = *k, i__2 = iladlr_(m, k, &v[v_offset], ldv);
		lastv = std::max(i__1,i__2);
		lastc = iladlc_(&lastv, n, &c__[c_offset], ldc);

/*              W := C' * V  =  (C1'*V1 + C2'*V2)  (stored in WORK) */

/*              W := C2' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(&lastc, &c__[lastv - *k + j + c_dim1], ldc, &work[
			    j * work_dim1 + 1], &c__1);
/* L70: */
		}

/*              W := W * V2 */

		dtrmm_("Right", "Upper", "No transpose", "Unit", &lastc, k, &
			c_b14, &v[lastv - *k + 1 + v_dim1], ldv, &work[
			work_offset], ldwork);
		if (lastv > *k) {

/*                 W := W + C1'*V1 */

		    i__1 = lastv - *k;
		    dgemm_("Transpose", "No transpose", &lastc, k, &i__1, &
			    c_b14, &c__[c_offset], ldc, &v[v_offset], ldv, &
			    c_b14, &work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		dtrmm_("Right", "Lower", transt, "Non-unit", &lastc, k, &
			c_b14, &t[t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V * W' */

		if (lastv > *k) {

/*                 C1 := C1 - V1 * W' */

		    i__1 = lastv - *k;
		    dgemm_("No transpose", "Transpose", &i__1, &lastc, k, &
			    c_b25, &v[v_offset], ldv, &work[work_offset],
			    ldwork, &c_b14, &c__[c_offset], ldc);
		}

/*              W := W * V2' */

		dtrmm_("Right", "Upper", "Transpose", "Unit", &lastc, k, &
			c_b14, &v[lastv - *k + 1 + v_dim1], ldv, &work[
			work_offset], ldwork);

/*              C2 := C2 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = lastc;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[lastv - *k + j + i__ * c_dim1] -= work[i__ + j *
				work_dim1];
/* L80: */
		    }
/* L90: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 ) */

/* Computing MAX */
		i__1 = *k, i__2 = iladlr_(n, k, &v[v_offset], ldv);
		lastv = std::max(i__1,i__2);
		lastc = iladlr_(m, &lastv, &c__[c_offset], ldc);

/*              W := C * V  =  (C1*V1 + C2*V2)  (stored in WORK) */

/*              W := C2 */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(&lastc, &c__[(*n - *k + j) * c_dim1 + 1], &c__1, &
			    work[j * work_dim1 + 1], &c__1);
/* L100: */
		}

/*              W := W * V2 */

		dtrmm_("Right", "Upper", "No transpose", "Unit", &lastc, k, &
			c_b14, &v[lastv - *k + 1 + v_dim1], ldv, &work[
			work_offset], ldwork);
		if (lastv > *k) {

/*                 W := W + C1 * V1 */

		    i__1 = lastv - *k;
		    dgemm_("No transpose", "No transpose", &lastc, k, &i__1, &
			    c_b14, &c__[c_offset], ldc, &v[v_offset], ldv, &
			    c_b14, &work[work_offset], ldwork);
		}

/*              W := W * T  or  W * T' */

		dtrmm_("Right", "Lower", trans, "Non-unit", &lastc, k, &c_b14,
			 &t[t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V' */

		if (lastv > *k) {

/*                 C1 := C1 - W * V1' */

		    i__1 = lastv - *k;
		    dgemm_("No transpose", "Transpose", &lastc, &i__1, k, &
			    c_b25, &work[work_offset], ldwork, &v[v_offset],
			    ldv, &c_b14, &c__[c_offset], ldc);
		}

/*              W := W * V2' */

		dtrmm_("Right", "Upper", "Transpose", "Unit", &lastc, k, &
			c_b14, &v[lastv - *k + 1 + v_dim1], ldv, &work[
			work_offset], ldwork);

/*              C2 := C2 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = lastc;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[i__ + (lastv - *k + j) * c_dim1] -= work[i__ + j *
				 work_dim1];
/* L110: */
		    }
/* L120: */
		}
	    }
	}

    } else if (lsame_(storev, "R")) {

	if (lsame_(direct, "F")) {

/*           Let  V =  ( V1  V2 )    (V1: first K columns) */
/*           where  V1  is unit upper triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 ) */
/*                                                  ( C2 ) */

/* Computing MAX */
		i__1 = *k, i__2 = iladlc_(k, m, &v[v_offset], ldv);
		lastv = std::max(i__1,i__2);
		lastc = iladlc_(&lastv, n, &c__[c_offset], ldc);

/*              W := C' * V'  =  (C1'*V1' + C2'*V2') (stored in WORK) */

/*              W := C1' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(&lastc, &c__[j + c_dim1], ldc, &work[j * work_dim1
			    + 1], &c__1);
/* L130: */
		}

/*              W := W * V1' */

		dtrmm_("Right", "Upper", "Transpose", "Unit", &lastc, k, &
			c_b14, &v[v_offset], ldv, &work[work_offset], ldwork);
		if (lastv > *k) {

/*                 W := W + C2'*V2' */

		    i__1 = lastv - *k;
		    dgemm_("Transpose", "Transpose", &lastc, k, &i__1, &c_b14,
			     &c__[*k + 1 + c_dim1], ldc, &v[(*k + 1) * v_dim1
			    + 1], ldv, &c_b14, &work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		dtrmm_("Right", "Upper", transt, "Non-unit", &lastc, k, &
			c_b14, &t[t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V' * W' */

		if (lastv > *k) {

/*                 C2 := C2 - V2' * W' */

		    i__1 = lastv - *k;
		    dgemm_("Transpose", "Transpose", &i__1, &lastc, k, &c_b25,
			     &v[(*k + 1) * v_dim1 + 1], ldv, &work[
			    work_offset], ldwork, &c_b14, &c__[*k + 1 +
			    c_dim1], ldc);
		}

/*              W := W * V1 */

		dtrmm_("Right", "Upper", "No transpose", "Unit", &lastc, k, &
			c_b14, &v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = lastc;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[j + i__ * c_dim1] -= work[i__ + j * work_dim1];
/* L140: */
		    }
/* L150: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 ) */

/* Computing MAX */
		i__1 = *k, i__2 = iladlc_(k, n, &v[v_offset], ldv);
		lastv = std::max(i__1,i__2);
		lastc = iladlr_(m, &lastv, &c__[c_offset], ldc);

/*              W := C * V'  =  (C1*V1' + C2*V2')  (stored in WORK) */

/*              W := C1 */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(&lastc, &c__[j * c_dim1 + 1], &c__1, &work[j *
			    work_dim1 + 1], &c__1);
/* L160: */
		}

/*              W := W * V1' */

		dtrmm_("Right", "Upper", "Transpose", "Unit", &lastc, k, &
			c_b14, &v[v_offset], ldv, &work[work_offset], ldwork);
		if (lastv > *k) {

/*                 W := W + C2 * V2' */

		    i__1 = lastv - *k;
		    dgemm_("No transpose", "Transpose", &lastc, k, &i__1, &
			    c_b14, &c__[(*k + 1) * c_dim1 + 1], ldc, &v[(*k +
			    1) * v_dim1 + 1], ldv, &c_b14, &work[work_offset],
			     ldwork);
		}

/*              W := W * T  or  W * T' */

		dtrmm_("Right", "Upper", trans, "Non-unit", &lastc, k, &c_b14,
			 &t[t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V */

		if (lastv > *k) {

/*                 C2 := C2 - W * V2 */

		    i__1 = lastv - *k;
		    dgemm_("No transpose", "No transpose", &lastc, &i__1, k, &
			    c_b25, &work[work_offset], ldwork, &v[(*k + 1) *
			    v_dim1 + 1], ldv, &c_b14, &c__[(*k + 1) * c_dim1
			    + 1], ldc);
		}

/*              W := W * V1 */

		dtrmm_("Right", "Upper", "No transpose", "Unit", &lastc, k, &
			c_b14, &v[v_offset], ldv, &work[work_offset], ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = lastc;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[i__ + j * c_dim1] -= work[i__ + j * work_dim1];
/* L170: */
		    }
/* L180: */
		}

	    }

	} else {

/*           Let  V =  ( V1  V2 )    (V2: last K columns) */
/*           where  V2  is unit lower triangular. */

	    if (lsame_(side, "L")) {

/*              Form  H * C  or  H' * C  where  C = ( C1 ) */
/*                                                  ( C2 ) */

/* Computing MAX */
		i__1 = *k, i__2 = iladlc_(k, m, &v[v_offset], ldv);
		lastv = std::max(i__1,i__2);
		lastc = iladlc_(&lastv, n, &c__[c_offset], ldc);

/*              W := C' * V'  =  (C1'*V1' + C2'*V2') (stored in WORK) */

/*              W := C2' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(&lastc, &c__[lastv - *k + j + c_dim1], ldc, &work[
			    j * work_dim1 + 1], &c__1);
/* L190: */
		}

/*              W := W * V2' */

		dtrmm_("Right", "Lower", "Transpose", "Unit", &lastc, k, &
			c_b14, &v[(lastv - *k + 1) * v_dim1 + 1], ldv, &work[
			work_offset], ldwork);
		if (lastv > *k) {

/*                 W := W + C1'*V1' */

		    i__1 = lastv - *k;
		    dgemm_("Transpose", "Transpose", &lastc, k, &i__1, &c_b14,
			     &c__[c_offset], ldc, &v[v_offset], ldv, &c_b14, &
			    work[work_offset], ldwork);
		}

/*              W := W * T'  or  W * T */

		dtrmm_("Right", "Lower", transt, "Non-unit", &lastc, k, &
			c_b14, &t[t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - V' * W' */

		if (lastv > *k) {

/*                 C1 := C1 - V1' * W' */

		    i__1 = lastv - *k;
		    dgemm_("Transpose", "Transpose", &i__1, &lastc, k, &c_b25,
			     &v[v_offset], ldv, &work[work_offset], ldwork, &
			    c_b14, &c__[c_offset], ldc);
		}

/*              W := W * V2 */

		dtrmm_("Right", "Lower", "No transpose", "Unit", &lastc, k, &
			c_b14, &v[(lastv - *k + 1) * v_dim1 + 1], ldv, &work[
			work_offset], ldwork);

/*              C2 := C2 - W' */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = lastc;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[lastv - *k + j + i__ * c_dim1] -= work[i__ + j *
				work_dim1];
/* L200: */
		    }
/* L210: */
		}

	    } else if (lsame_(side, "R")) {

/*              Form  C * H  or  C * H'  where  C = ( C1  C2 ) */

/* Computing MAX */
		i__1 = *k, i__2 = iladlc_(k, n, &v[v_offset], ldv);
		lastv = std::max(i__1,i__2);
		lastc = iladlr_(m, &lastv, &c__[c_offset], ldc);

/*              W := C * V'  =  (C1*V1' + C2*V2')  (stored in WORK) */

/*              W := C2 */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    dcopy_(&lastc, &c__[(lastv - *k + j) * c_dim1 + 1], &c__1,
			     &work[j * work_dim1 + 1], &c__1);
/* L220: */
		}

/*              W := W * V2' */

		dtrmm_("Right", "Lower", "Transpose", "Unit", &lastc, k, &
			c_b14, &v[(lastv - *k + 1) * v_dim1 + 1], ldv, &work[
			work_offset], ldwork);
		if (lastv > *k) {

/*                 W := W + C1 * V1' */

		    i__1 = lastv - *k;
		    dgemm_("No transpose", "Transpose", &lastc, k, &i__1, &
			    c_b14, &c__[c_offset], ldc, &v[v_offset], ldv, &
			    c_b14, &work[work_offset], ldwork);
		}

/*              W := W * T  or  W * T' */

		dtrmm_("Right", "Lower", trans, "Non-unit", &lastc, k, &c_b14,
			 &t[t_offset], ldt, &work[work_offset], ldwork);

/*              C := C - W * V */

		if (lastv > *k) {

/*                 C1 := C1 - W * V1 */

		    i__1 = lastv - *k;
		    dgemm_("No transpose", "No transpose", &lastc, &i__1, k, &
			    c_b25, &work[work_offset], ldwork, &v[v_offset],
			    ldv, &c_b14, &c__[c_offset], ldc);
		}

/*              W := W * V2 */

		dtrmm_("Right", "Lower", "No transpose", "Unit", &lastc, k, &
			c_b14, &v[(lastv - *k + 1) * v_dim1 + 1], ldv, &work[
			work_offset], ldwork);

/*              C1 := C1 - W */

		i__1 = *k;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = lastc;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			c__[i__ + (lastv - *k + j) * c_dim1] -= work[i__ + j *
				 work_dim1];
/* L230: */
		    }
/* L240: */
		}

	    }

	}
    }

    return 0;

/*     End of DLARFB */

} /* dlarfb_ */

/* Subroutine */ int dlarfg_(integer *n, double *alpha, double *x, integer *incx, double *tau)
{
    /* System generated locals */
    integer i__1;
    double d__1;

    /* Local variables */
    integer j, knt;
    double beta, xnorm, safmin, rsafmn;


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARFG generates a real elementary reflector H of order n, such */
/*  that */

/*        H * ( alpha ) = ( beta ),   H' * H = I. */
/*            (   x   )   (   0  ) */

/*  where alpha and beta are scalars, and x is an (n-1)-element real */
/*  vector. H is represented in the form */

/*        H = I - tau * ( 1 ) * ( 1 v' ) , */
/*                      ( v ) */

/*  where tau is a real scalar and v is a real (n-1)-element */
/*  vector. */

/*  If the elements of x are all zero, then tau = 0 and H is taken to be */
/*  the unit matrix. */

/*  Otherwise  1 <= tau <= 2. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the elementary reflector. */

/*  ALPHA   (input/output) DOUBLE PRECISION */
/*          On entry, the value alpha. */
/*          On exit, it is overwritten with the value beta. */

/*  X       (input/output) DOUBLE PRECISION array, dimension */
/*                         (1+(N-2)*abs(INCX)) */
/*          On entry, the vector x. */
/*          On exit, it is overwritten with the vector v. */

/*  INCX    (input) INTEGER */
/*          The increment between elements of X. INCX > 0. */

/*  TAU     (output) DOUBLE PRECISION */
/*          The value tau. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*n <= 1) {
	*tau = 0.;
	return 0;
    }

    i__1 = *n - 1;
    xnorm = dnrm2_(&i__1, &x[1], incx);

    if (xnorm == 0.) {

/*        H  =  I */

	*tau = 0.;
    } else {

/*        general case */

	d__1 = dlapy2_(alpha, &xnorm);
	beta = -d_sign(&d__1, alpha);
	safmin = dlamch_("S") / dlamch_("E");
	knt = 0;
	if (abs(beta) < safmin) {

/*           XNORM, BETA may be inaccurate; scale X and recompute them */

	    rsafmn = 1. / safmin;
L10:
	    ++knt;
	    i__1 = *n - 1;
	    dscal_(&i__1, &rsafmn, &x[1], incx);
	    beta *= rsafmn;
	    *alpha *= rsafmn;
	    if (abs(beta) < safmin) {
		goto L10;
	    }

/*           New BETA is at most 1, at least SAFMIN */

	    i__1 = *n - 1;
	    xnorm = dnrm2_(&i__1, &x[1], incx);
	    d__1 = dlapy2_(alpha, &xnorm);
	    beta = -d_sign(&d__1, alpha);
	}
	*tau = (beta - *alpha) / beta;
	i__1 = *n - 1;
	d__1 = 1. / (*alpha - beta);
	dscal_(&i__1, &d__1, &x[1], incx);

/*        If ALPHA is subnormal, it may lose relative accuracy */

	i__1 = knt;
	for (j = 1; j <= i__1; ++j) {
	    beta *= safmin;
/* L20: */
	}
	*alpha = beta;
    }

    return 0;

/*     End of DLARFG */

} /* dlarfg_ */

/* Subroutine */ int dlarfp_(integer *n, double *alpha, double *x, integer *incx, double *tau)
{
    /* System generated locals */
    integer i__1;
    double d__1;

    /* Local variables */
    integer j, knt;
    double beta;
    double xnorm;
    double safmin, rsafmn;


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARFP generates a real elementary reflector H of order n, such */
/*  that */

/*        H * ( alpha ) = ( beta ),   H' * H = I. */
/*            (   x   )   (   0  ) */

/*  where alpha and beta are scalars, beta is non-negative, and x is */
/*  an (n-1)-element real vector.  H is represented in the form */

/*        H = I - tau * ( 1 ) * ( 1 v' ) , */
/*                      ( v ) */

/*  where tau is a real scalar and v is a real (n-1)-element */
/*  vector. */

/*  If the elements of x are all zero, then tau = 0 and H is taken to be */
/*  the unit matrix. */

/*  Otherwise  1 <= tau <= 2. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the elementary reflector. */

/*  ALPHA   (input/output) DOUBLE PRECISION */
/*          On entry, the value alpha. */
/*          On exit, it is overwritten with the value beta. */

/*  X       (input/output) DOUBLE PRECISION array, dimension */
/*                         (1+(N-2)*abs(INCX)) */
/*          On entry, the vector x. */
/*          On exit, it is overwritten with the vector v. */

/*  INCX    (input) INTEGER */
/*          The increment between elements of X. INCX > 0. */

/*  TAU     (output) DOUBLE PRECISION */
/*          The value tau. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*n <= 0) {
	*tau = 0.;
	return 0;
    }

    i__1 = *n - 1;
    xnorm = dnrm2_(&i__1, &x[1], incx);

    if (xnorm == 0.) {

/*        H  =  [+/-1, 0; I], sign chosen so ALPHA >= 0 */

	if (*alpha >= 0.) {
/*           When TAU.eq.ZERO, the vector is special-cased to be */
/*           all zeros in the application routines.  We do not need */
/*           to clear it. */
	    *tau = 0.;
	} else {
/*           However, the application routines rely on explicit */
/*           zero checks when TAU.ne.ZERO, and we must clear X. */
	    *tau = 2.;
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		x[(j - 1) * *incx + 1] = 0.;
	    }
	    *alpha = -(*alpha);
	}
    } else {

/*        general case */

	d__1 = dlapy2_(alpha, &xnorm);
	beta = d_sign(&d__1, alpha);
	safmin = dlamch_("S") / dlamch_("E");
	knt = 0;
	if (abs(beta) < safmin) {

/*           XNORM, BETA may be inaccurate; scale X and recompute them */

	    rsafmn = 1. / safmin;
L10:
	    ++knt;
	    i__1 = *n - 1;
	    dscal_(&i__1, &rsafmn, &x[1], incx);
	    beta *= rsafmn;
	    *alpha *= rsafmn;
	    if (abs(beta) < safmin) {
		goto L10;
	    }

/*           New BETA is at most 1, at least SAFMIN */

	    i__1 = *n - 1;
	    xnorm = dnrm2_(&i__1, &x[1], incx);
	    d__1 = dlapy2_(alpha, &xnorm);
	    beta = d_sign(&d__1, alpha);
	}
	*alpha += beta;
	if (beta < 0.) {
	    beta = -beta;
	    *tau = -(*alpha) / beta;
	} else {
	    *alpha = xnorm * (xnorm / *alpha);
	    *tau = *alpha / beta;
	    *alpha = -(*alpha);
	}
	i__1 = *n - 1;
	d__1 = 1. / *alpha;
	dscal_(&i__1, &d__1, &x[1], incx);

/*        If BETA is subnormal, it may lose relative accuracy */

	i__1 = knt;
	for (j = 1; j <= i__1; ++j) {
	    beta *= safmin;
/* L20: */
	}
	*alpha = beta;
    }

    return 0;

/*     End of DLARFP */

} /* dlarfp_ */

/* Subroutine */ int dlarft_(const char *direct, const char *storev, integer *n, integer *k, double *v, integer *ldv,
	double *tau, double *t, integer *ldt)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b8 = 0.;

    /* System generated locals */
    integer t_dim1, t_offset, v_dim1, v_offset, i__1, i__2, i__3;
    double d__1;

    /* Local variables */
    integer i__, j, prevlastv;
    double vii;
    integer lastv;


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARFT forms the triangular factor T of a real block reflector H */
/*  of order n, which is defined as a product of k elementary reflectors. */

/*  If DIRECT = 'F', H = H(1) H(2) . . . H(k) and T is upper triangular; */

/*  If DIRECT = 'B', H = H(k) . . . H(2) H(1) and T is lower triangular. */

/*  If STOREV = 'C', the vector which defines the elementary reflector */
/*  H(i) is stored in the i-th column of the array V, and */

/*     H  =  I - V * T * V' */

/*  If STOREV = 'R', the vector which defines the elementary reflector */
/*  H(i) is stored in the i-th row of the array V, and */

/*     H  =  I - V' * T * V */

/*  Arguments */
/*  ========= */

/*  DIRECT  (input) CHARACTER*1 */
/*          Specifies the order in which the elementary reflectors are */
/*          multiplied to form the block reflector: */
/*          = 'F': H = H(1) H(2) . . . H(k) (Forward) */
/*          = 'B': H = H(k) . . . H(2) H(1) (Backward) */

/*  STOREV  (input) CHARACTER*1 */
/*          Specifies how the vectors which define the elementary */
/*          reflectors are stored (see also Further Details): */
/*          = 'C': columnwise */
/*          = 'R': rowwise */

/*  N       (input) INTEGER */
/*          The order of the block reflector H. N >= 0. */

/*  K       (input) INTEGER */
/*          The order of the triangular factor T (= the number of */
/*          elementary reflectors). K >= 1. */

/*  V       (input/output) DOUBLE PRECISION array, dimension */
/*                               (LDV,K) if STOREV = 'C' */
/*                               (LDV,N) if STOREV = 'R' */
/*          The matrix V. See further details. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. */
/*          If STOREV = 'C', LDV >= max(1,N); if STOREV = 'R', LDV >= K. */

/*  TAU     (input) DOUBLE PRECISION array, dimension (K) */
/*          TAU(i) must contain the scalar factor of the elementary */
/*          reflector H(i). */

/*  T       (output) DOUBLE PRECISION array, dimension (LDT,K) */
/*          The k by k triangular factor T of the block reflector. */
/*          If DIRECT = 'F', T is upper triangular; if DIRECT = 'B', T is */
/*          lower triangular. The rest of the array is not used. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T. LDT >= K. */

/*  Further Details */
/*  =============== */

/*  The shape of the matrix V and the storage of the vectors which define */
/*  the H(i) is best illustrated by the following example with n = 5 and */
/*  k = 3. The elements equal to 1 are not stored; the corresponding */
/*  array elements are modified but restored on exit. The rest of the */
/*  array is not used. */

/*  DIRECT = 'F' and STOREV = 'C':         DIRECT = 'F' and STOREV = 'R': */

/*               V = (  1       )                 V = (  1 v1 v1 v1 v1 ) */
/*                   ( v1  1    )                     (     1 v2 v2 v2 ) */
/*                   ( v1 v2  1 )                     (        1 v3 v3 ) */
/*                   ( v1 v2 v3 ) */
/*                   ( v1 v2 v3 ) */

/*  DIRECT = 'B' and STOREV = 'C':         DIRECT = 'B' and STOREV = 'R': */

/*               V = ( v1 v2 v3 )                 V = ( v1 v1  1       ) */
/*                   ( v1 v2 v3 )                     ( v2 v2 v2  1    ) */
/*                   (  1 v2 v3 )                     ( v3 v3 v3 v3  1 ) */
/*                   (     1 v3 ) */
/*                   (        1 ) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

    /* Parameter adjustments */
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    --tau;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;

    /* Function Body */
    if (*n == 0) {
	return 0;
    }

    if (lsame_(direct, "F")) {
	prevlastv = *n;
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    prevlastv = std::max(i__,prevlastv);
	    if (tau[i__] == 0.) {

/*              H(i)  =  I */

		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    t[j + i__ * t_dim1] = 0.;
/* L10: */
		}
	    } else {

/*              general case */

		vii = v[i__ + i__ * v_dim1];
		v[i__ + i__ * v_dim1] = 1.;
		if (lsame_(storev, "C")) {
/*                 Skip any trailing zeros. */
		    i__2 = i__ + 1;
		    for (lastv = *n; lastv >= i__2; --lastv) {
			if (v[lastv + i__ * v_dim1] != 0.) {
			    break;
			}
		    }
		    j = std::min(lastv,prevlastv);

/*                 T(1:i-1,i) := - tau(i) * V(i:j,1:i-1)' * V(i:j,i) */

		    i__2 = j - i__ + 1;
		    i__3 = i__ - 1;
		    d__1 = -tau[i__];
		    dgemv_("Transpose", &i__2, &i__3, &d__1, &v[i__ + v_dim1],
			     ldv, &v[i__ + i__ * v_dim1], &c__1, &c_b8, &t[
			    i__ * t_dim1 + 1], &c__1);
		} else {
/*                 Skip any trailing zeros. */
		    i__2 = i__ + 1;
		    for (lastv = *n; lastv >= i__2; --lastv) {
			if (v[i__ + lastv * v_dim1] != 0.) {
			    break;
			}
		    }
		    j = std::min(lastv,prevlastv);

/*                 T(1:i-1,i) := - tau(i) * V(1:i-1,i:j) * V(i,i:j)' */

		    i__2 = i__ - 1;
		    i__3 = j - i__ + 1;
		    d__1 = -tau[i__];
		    dgemv_("No transpose", &i__2, &i__3, &d__1, &v[i__ *
			    v_dim1 + 1], ldv, &v[i__ + i__ * v_dim1], ldv, &
			    c_b8, &t[i__ * t_dim1 + 1], &c__1);
		}
		v[i__ + i__ * v_dim1] = vii;

/*              T(1:i-1,i) := T(1:i-1,1:i-1) * T(1:i-1,i) */

		i__2 = i__ - 1;
		dtrmv_("Upper", "No transpose", "Non-unit", &i__2, &t[
			t_offset], ldt, &t[i__ * t_dim1 + 1], &c__1);
		t[i__ + i__ * t_dim1] = tau[i__];
		if (i__ > 1) {
		    prevlastv = std::max(prevlastv,lastv);
		} else {
		    prevlastv = lastv;
		}
	    }
/* L20: */
	}
    } else {
	prevlastv = 1;
	for (i__ = *k; i__ >= 1; --i__) {
	    if (tau[i__] == 0.) {

/*              H(i)  =  I */

		i__1 = *k;
		for (j = i__; j <= i__1; ++j) {
		    t[j + i__ * t_dim1] = 0.;
/* L30: */
		}
	    } else {

/*              general case */

		if (i__ < *k) {
		    if (lsame_(storev, "C")) {
			vii = v[*n - *k + i__ + i__ * v_dim1];
			v[*n - *k + i__ + i__ * v_dim1] = 1.;
/*                    Skip any leading zeros. */
			i__1 = i__ - 1;
			for (lastv = 1; lastv <= i__1; ++lastv) {
			    if (v[lastv + i__ * v_dim1] != 0.) {
				break;
			    }
			}
			j = std::max(lastv,prevlastv);

/*                    T(i+1:k,i) := */
/*                            - tau(i) * V(j:n-k+i,i+1:k)' * V(j:n-k+i,i) */

			i__1 = *n - *k + i__ - j + 1;
			i__2 = *k - i__;
			d__1 = -tau[i__];
			dgemv_("Transpose", &i__1, &i__2, &d__1, &v[j + (i__
				+ 1) * v_dim1], ldv, &v[j + i__ * v_dim1], &
				c__1, &c_b8, &t[i__ + 1 + i__ * t_dim1], &
				c__1);
			v[*n - *k + i__ + i__ * v_dim1] = vii;
		    } else {
			vii = v[i__ + (*n - *k + i__) * v_dim1];
			v[i__ + (*n - *k + i__) * v_dim1] = 1.;
/*                    Skip any leading zeros. */
			i__1 = i__ - 1;
			for (lastv = 1; lastv <= i__1; ++lastv) {
			    if (v[i__ + lastv * v_dim1] != 0.) {
				break;
			    }
			}
			j = std::max(lastv,prevlastv);

/*                    T(i+1:k,i) := */
/*                            - tau(i) * V(i+1:k,j:n-k+i) * V(i,j:n-k+i)' */

			i__1 = *k - i__;
			i__2 = *n - *k + i__ - j + 1;
			d__1 = -tau[i__];
			dgemv_("No transpose", &i__1, &i__2, &d__1, &v[i__ +
				1 + j * v_dim1], ldv, &v[i__ + j * v_dim1],
				ldv, &c_b8, &t[i__ + 1 + i__ * t_dim1], &c__1);
			v[i__ + (*n - *k + i__) * v_dim1] = vii;
		    }

/*                 T(i+1:k,i) := T(i+1:k,i+1:k) * T(i+1:k,i) */

		    i__1 = *k - i__;
		    dtrmv_("Lower", "No transpose", "Non-unit", &i__1, &t[i__
			    + 1 + (i__ + 1) * t_dim1], ldt, &t[i__ + 1 + i__ *
			     t_dim1], &c__1)
			    ;
		    if (i__ > 1) {
			prevlastv = std::min(prevlastv,lastv);
		    } else {
			prevlastv = lastv;
		    }
		}
		t[i__ + i__ * t_dim1] = tau[i__];
	    }
/* L40: */
	}
    }
    return 0;

/*     End of DLARFT */

} /* dlarft_ */

/* Subroutine */ int dlarfx_(const char *side, integer *m, integer *n, double *v, double *tau, double *c__, integer *ldc, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer c_dim1, c_offset, i__1;

    /* Local variables */
    integer j;
    double t1, t2, t3, t4, t5, t6, t7, t8, t9, v1, v2, v3, v4, v5, v6, v7,
	     v8, v9, t10, v10, sum;

/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARFX applies a real elementary reflector H to a real m by n */
/*  matrix C, from either the left or the right. H is represented in the */
/*  form */

/*        H = I - tau * v * v' */

/*  where tau is a real scalar and v is a real vector. */

/*  If tau = 0, then H is taken to be the unit matrix */

/*  This version uses inline code if H has order < 11. */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'L': form  H * C */
/*          = 'R': form  C * H */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix C. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix C. */

/*  V       (input) DOUBLE PRECISION array, dimension (M) if SIDE = 'L' */
/*                                     or (N) if SIDE = 'R' */
/*          The vector v in the representation of H. */

/*  TAU     (input) DOUBLE PRECISION */
/*          The value tau in the representation of H. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC,N) */
/*          On entry, the m by n matrix C. */
/*          On exit, C is overwritten by the matrix H * C if SIDE = 'L', */
/*          or C * H if SIDE = 'R'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C. LDA >= (1,M). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                      (N) if SIDE = 'L' */
/*                      or (M) if SIDE = 'R' */
/*          WORK is not referenced if H has order < 11. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --v;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    if (*tau == 0.) {
	return 0;
    }
    if (lsame_(side, "L")) {

/*        Form  H * C, where H has order m. */

	switch (*m) {
	    case 1:  goto L10;
	    case 2:  goto L30;
	    case 3:  goto L50;
	    case 4:  goto L70;
	    case 5:  goto L90;
	    case 6:  goto L110;
	    case 7:  goto L130;
	    case 8:  goto L150;
	    case 9:  goto L170;
	    case 10:  goto L190;
	}

/*        Code for general M */

	dlarf_(side, m, n, &v[1], &c__1, tau, &c__[c_offset], ldc, &work[1]);
	goto L410;
L10:

/*        Special code for 1 x 1 Householder */

	t1 = 1. - *tau * v[1] * v[1];
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    c__[j * c_dim1 + 1] = t1 * c__[j * c_dim1 + 1];
/* L20: */
	}
	goto L410;
L30:

/*        Special code for 2 x 2 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
/* L40: */
	}
	goto L410;
L50:

/*        Special code for 3 x 3 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
/* L60: */
	}
	goto L410;
L70:

/*        Special code for 4 x 4 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
/* L80: */
	}
	goto L410;
L90:

/*        Special code for 5 x 5 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
/* L100: */
	}
	goto L410;
L110:

/*        Special code for 6 x 6 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5] + v6 * c__[j * c_dim1 + 6];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
	    c__[j * c_dim1 + 6] -= sum * t6;
/* L120: */
	}
	goto L410;
L130:

/*        Special code for 7 x 7 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5] + v6 * c__[j * c_dim1 + 6] + v7 * c__[j *
		    c_dim1 + 7];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
	    c__[j * c_dim1 + 6] -= sum * t6;
	    c__[j * c_dim1 + 7] -= sum * t7;
/* L140: */
	}
	goto L410;
L150:

/*        Special code for 8 x 8 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5] + v6 * c__[j * c_dim1 + 6] + v7 * c__[j *
		    c_dim1 + 7] + v8 * c__[j * c_dim1 + 8];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
	    c__[j * c_dim1 + 6] -= sum * t6;
	    c__[j * c_dim1 + 7] -= sum * t7;
	    c__[j * c_dim1 + 8] -= sum * t8;
/* L160: */
	}
	goto L410;
L170:

/*        Special code for 9 x 9 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	v9 = v[9];
	t9 = *tau * v9;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5] + v6 * c__[j * c_dim1 + 6] + v7 * c__[j *
		    c_dim1 + 7] + v8 * c__[j * c_dim1 + 8] + v9 * c__[j *
		    c_dim1 + 9];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
	    c__[j * c_dim1 + 6] -= sum * t6;
	    c__[j * c_dim1 + 7] -= sum * t7;
	    c__[j * c_dim1 + 8] -= sum * t8;
	    c__[j * c_dim1 + 9] -= sum * t9;
/* L180: */
	}
	goto L410;
L190:

/*        Special code for 10 x 10 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	v9 = v[9];
	t9 = *tau * v9;
	v10 = v[10];
	t10 = *tau * v10;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j * c_dim1 + 1] + v2 * c__[j * c_dim1 + 2] + v3 *
		    c__[j * c_dim1 + 3] + v4 * c__[j * c_dim1 + 4] + v5 * c__[
		    j * c_dim1 + 5] + v6 * c__[j * c_dim1 + 6] + v7 * c__[j *
		    c_dim1 + 7] + v8 * c__[j * c_dim1 + 8] + v9 * c__[j *
		    c_dim1 + 9] + v10 * c__[j * c_dim1 + 10];
	    c__[j * c_dim1 + 1] -= sum * t1;
	    c__[j * c_dim1 + 2] -= sum * t2;
	    c__[j * c_dim1 + 3] -= sum * t3;
	    c__[j * c_dim1 + 4] -= sum * t4;
	    c__[j * c_dim1 + 5] -= sum * t5;
	    c__[j * c_dim1 + 6] -= sum * t6;
	    c__[j * c_dim1 + 7] -= sum * t7;
	    c__[j * c_dim1 + 8] -= sum * t8;
	    c__[j * c_dim1 + 9] -= sum * t9;
	    c__[j * c_dim1 + 10] -= sum * t10;
/* L200: */
	}
	goto L410;
    } else {

/*        Form  C * H, where H has order n. */

	switch (*n) {
	    case 1:  goto L210;
	    case 2:  goto L230;
	    case 3:  goto L250;
	    case 4:  goto L270;
	    case 5:  goto L290;
	    case 6:  goto L310;
	    case 7:  goto L330;
	    case 8:  goto L350;
	    case 9:  goto L370;
	    case 10:  goto L390;
	}

/*        Code for general N */

	dlarf_(side, m, n, &v[1], &c__1, tau, &c__[c_offset], ldc, &work[1]);
	goto L410;
L210:

/*        Special code for 1 x 1 Householder */

	t1 = 1. - *tau * v[1] * v[1];
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    c__[j + c_dim1] = t1 * c__[j + c_dim1];
/* L220: */
	}
	goto L410;
L230:

/*        Special code for 2 x 2 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + (c_dim1 << 1)];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + (c_dim1 << 1)] -= sum * t2;
/* L240: */
	}
	goto L410;
L250:

/*        Special code for 3 x 3 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + (c_dim1 << 1)] + v3 *
		    c__[j + c_dim1 * 3];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + (c_dim1 << 1)] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
/* L260: */
	}
	goto L410;
L270:

/*        Special code for 4 x 4 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + (c_dim1 << 1)] + v3 *
		    c__[j + c_dim1 * 3] + v4 * c__[j + (c_dim1 << 2)];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + (c_dim1 << 1)] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + (c_dim1 << 2)] -= sum * t4;
/* L280: */
	}
	goto L410;
L290:

/*        Special code for 5 x 5 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + (c_dim1 << 1)] + v3 *
		    c__[j + c_dim1 * 3] + v4 * c__[j + (c_dim1 << 2)] + v5 *
		    c__[j + c_dim1 * 5];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + (c_dim1 << 1)] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + (c_dim1 << 2)] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
/* L300: */
	}
	goto L410;
L310:

/*        Special code for 6 x 6 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + (c_dim1 << 1)] + v3 *
		    c__[j + c_dim1 * 3] + v4 * c__[j + (c_dim1 << 2)] + v5 *
		    c__[j + c_dim1 * 5] + v6 * c__[j + c_dim1 * 6];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + (c_dim1 << 1)] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + (c_dim1 << 2)] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
	    c__[j + c_dim1 * 6] -= sum * t6;
/* L320: */
	}
	goto L410;
L330:

/*        Special code for 7 x 7 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + (c_dim1 << 1)] + v3 *
		    c__[j + c_dim1 * 3] + v4 * c__[j + (c_dim1 << 2)] + v5 *
		    c__[j + c_dim1 * 5] + v6 * c__[j + c_dim1 * 6] + v7 * c__[
		    j + c_dim1 * 7];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + (c_dim1 << 1)] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + (c_dim1 << 2)] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
	    c__[j + c_dim1 * 6] -= sum * t6;
	    c__[j + c_dim1 * 7] -= sum * t7;
/* L340: */
	}
	goto L410;
L350:

/*        Special code for 8 x 8 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + (c_dim1 << 1)] + v3 *
		    c__[j + c_dim1 * 3] + v4 * c__[j + (c_dim1 << 2)] + v5 *
		    c__[j + c_dim1 * 5] + v6 * c__[j + c_dim1 * 6] + v7 * c__[
		    j + c_dim1 * 7] + v8 * c__[j + (c_dim1 << 3)];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + (c_dim1 << 1)] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + (c_dim1 << 2)] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
	    c__[j + c_dim1 * 6] -= sum * t6;
	    c__[j + c_dim1 * 7] -= sum * t7;
	    c__[j + (c_dim1 << 3)] -= sum * t8;
/* L360: */
	}
	goto L410;
L370:

/*        Special code for 9 x 9 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	v9 = v[9];
	t9 = *tau * v9;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + (c_dim1 << 1)] + v3 *
		    c__[j + c_dim1 * 3] + v4 * c__[j + (c_dim1 << 2)] + v5 *
		    c__[j + c_dim1 * 5] + v6 * c__[j + c_dim1 * 6] + v7 * c__[
		    j + c_dim1 * 7] + v8 * c__[j + (c_dim1 << 3)] + v9 * c__[
		    j + c_dim1 * 9];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + (c_dim1 << 1)] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + (c_dim1 << 2)] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
	    c__[j + c_dim1 * 6] -= sum * t6;
	    c__[j + c_dim1 * 7] -= sum * t7;
	    c__[j + (c_dim1 << 3)] -= sum * t8;
	    c__[j + c_dim1 * 9] -= sum * t9;
/* L380: */
	}
	goto L410;
L390:

/*        Special code for 10 x 10 Householder */

	v1 = v[1];
	t1 = *tau * v1;
	v2 = v[2];
	t2 = *tau * v2;
	v3 = v[3];
	t3 = *tau * v3;
	v4 = v[4];
	t4 = *tau * v4;
	v5 = v[5];
	t5 = *tau * v5;
	v6 = v[6];
	t6 = *tau * v6;
	v7 = v[7];
	t7 = *tau * v7;
	v8 = v[8];
	t8 = *tau * v8;
	v9 = v[9];
	t9 = *tau * v9;
	v10 = v[10];
	t10 = *tau * v10;
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    sum = v1 * c__[j + c_dim1] + v2 * c__[j + (c_dim1 << 1)] + v3 *
		    c__[j + c_dim1 * 3] + v4 * c__[j + (c_dim1 << 2)] + v5 *
		    c__[j + c_dim1 * 5] + v6 * c__[j + c_dim1 * 6] + v7 * c__[
		    j + c_dim1 * 7] + v8 * c__[j + (c_dim1 << 3)] + v9 * c__[
		    j + c_dim1 * 9] + v10 * c__[j + c_dim1 * 10];
	    c__[j + c_dim1] -= sum * t1;
	    c__[j + (c_dim1 << 1)] -= sum * t2;
	    c__[j + c_dim1 * 3] -= sum * t3;
	    c__[j + (c_dim1 << 2)] -= sum * t4;
	    c__[j + c_dim1 * 5] -= sum * t5;
	    c__[j + c_dim1 * 6] -= sum * t6;
	    c__[j + c_dim1 * 7] -= sum * t7;
	    c__[j + (c_dim1 << 3)] -= sum * t8;
	    c__[j + c_dim1 * 9] -= sum * t9;
	    c__[j + c_dim1 * 10] -= sum * t10;
/* L400: */
	}
	goto L410;
    }
L410:
    return 0;

/*     End of DLARFX */

} /* dlarfx_ */

/* Subroutine */ int dlargv_(integer *n, double *x, integer *incx,
	double *y, integer *incy, double *c__, integer *incc)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    double f, g;
    integer i__;
    double t;
    integer ic, ix, iy;
    double tt;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARGV generates a vector of real plane rotations, determined by */
/*  elements of the real vectors x and y. For i = 1,2,...,n */

/*     (  c(i)  s(i) ) ( x(i) ) = ( a(i) ) */
/*     ( -s(i)  c(i) ) ( y(i) ) = (   0  ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of plane rotations to be generated. */

/*  X       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCX) */
/*          On entry, the vector x. */
/*          On exit, x(i) is overwritten by a(i), for i = 1,...,n. */

/*  INCX    (input) INTEGER */
/*          The increment between elements of X. INCX > 0. */

/*  Y       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCY) */
/*          On entry, the vector y. */
/*          On exit, the sines of the plane rotations. */

/*  INCY    (input) INTEGER */
/*          The increment between elements of Y. INCY > 0. */

/*  C       (output) DOUBLE PRECISION array, dimension (1+(N-1)*INCC) */
/*          The cosines of the plane rotations. */

/*  INCC    (input) INTEGER */
/*          The increment between elements of C. INCC > 0. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --c__;
    --y;
    --x;

    /* Function Body */
    ix = 1;
    iy = 1;
    ic = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	f = x[ix];
	g = y[iy];
	if (g == 0.) {
	    c__[ic] = 1.;
	} else if (f == 0.) {
	    c__[ic] = 0.;
	    y[iy] = 1.;
	    x[ix] = g;
	} else if (abs(f) > abs(g)) {
	    t = g / f;
	    tt = sqrt(t * t + 1.);
	    c__[ic] = 1. / tt;
	    y[iy] = t * c__[ic];
	    x[ix] = f * tt;
	} else {
	    t = f / g;
	    tt = sqrt(t * t + 1.);
	    y[iy] = 1. / tt;
	    c__[ic] = t * y[iy];
	    x[ix] = g * tt;
	}
	ic += *incc;
	iy += *incy;
	ix += *incx;
/* L10: */
    }
    return 0;

/*     End of DLARGV */

} /* dlargv_ */

/* Subroutine */ int dlarnv_(integer *idist, integer *iseed, integer *n,
	double *x)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Builtin functions
    double log(double), sqrt(double), cos(double); */

    /* Local variables */
    integer i__;
    double u[128];
    integer il, iv, il2;



/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARNV returns a vector of n random real numbers from a uniform or */
/*  normal distribution. */

/*  Arguments */
/*  ========= */

/*  IDIST   (input) INTEGER */
/*          Specifies the distribution of the random numbers: */
/*          = 1:  uniform (0,1) */
/*          = 2:  uniform (-1,1) */
/*          = 3:  normal (0,1) */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed of the random number generator; the array */
/*          elements must be between 0 and 4095, and ISEED(4) must be */
/*          odd. */
/*          On exit, the seed is updated. */

/*  N       (input) INTEGER */
/*          The number of random numbers to be generated. */

/*  X       (output) DOUBLE PRECISION array, dimension (N) */
/*          The generated random numbers. */

/*  Further Details */
/*  =============== */

/*  This routine calls the auxiliary routine DLARUV to generate random */
/*  real numbers from a uniform (0,1) distribution, in batches of up to */
/*  128 using vectorisable code. The Box-Muller method is used to */
/*  transform numbers from a uniform to a normal distribution. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --x;
    --iseed;

    /* Function Body */
    i__1 = *n;
    for (iv = 1; iv <= i__1; iv += 64) {
/* Computing MIN */
	i__2 = 64, i__3 = *n - iv + 1;
	il = std::min(i__2,i__3);
	if (*idist == 3) {
	    il2 = il << 1;
	} else {
	    il2 = il;
	}

/*        Call DLARUV to generate IL2 numbers from a uniform (0,1) */
/*        distribution (IL2 <= LV) */

	dlaruv_(&iseed[1], &il2, u);

	if (*idist == 1) {

/*           Copy generated numbers */

	    i__2 = il;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		x[iv + i__ - 1] = u[i__ - 1];
/* L10: */
	    }
	} else if (*idist == 2) {

/*           Convert generated numbers to uniform (-1,1) distribution */

	    i__2 = il;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		x[iv + i__ - 1] = u[i__ - 1] * 2. - 1.;
/* L20: */
	    }
	} else if (*idist == 3) {

/*           Convert generated numbers to normal (0,1) distribution */

	    i__2 = il;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		x[iv + i__ - 1] = sqrt(log(u[(i__ << 1) - 2]) * -2.) * cos(u[(
			i__ << 1) - 1] * 6.2831853071795864769252867663);
/* L30: */
	    }
	}
/* L40: */
    }
    return 0;

/*     End of DLARNV */

} /* dlarnv_ */

/* Subroutine */ int dlarra_(integer *n, double *d__, double *e,
	double *e2, double *spltol, double *tnrm, integer *nsplit,
	 integer *isplit, integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    integer i__;
    double tmp1, eabs;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Compute the splitting points with threshold SPLTOL. */
/*  DLARRA sets any "small" off-diagonal elements to zero. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix. N > 0. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the N diagonal elements of the tridiagonal */
/*          matrix T. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the first (N-1) entries contain the subdiagonal */
/*          elements of the tridiagonal matrix T; E(N) need not be set. */
/*          On exit, the entries E( ISPLIT( I ) ), 1 <= I <= NSPLIT, */
/*          are set to zero, the other entries of E are untouched. */

/*  E2      (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the first (N-1) entries contain the SQUARES of the */
/*          subdiagonal elements of the tridiagonal matrix T; */
/*          E2(N) need not be set. */
/*          On exit, the entries E2( ISPLIT( I ) ), */
/*          1 <= I <= NSPLIT, have been set to zero */

/*  SPLTOL (input) DOUBLE PRECISION */
/*          The threshold for splitting. Two criteria can be used: */
/*          SPLTOL<0 : criterion based on absolute off-diagonal value */
/*          SPLTOL>0 : criterion that preserves relative accuracy */

/*  TNRM (input) DOUBLE PRECISION */
/*          The norm of the matrix. */

/*  NSPLIT  (output) INTEGER */
/*          The number of blocks T splits into. 1 <= NSPLIT <= N. */

/*  ISPLIT  (output) INTEGER array, dimension (N) */
/*          The splitting points, at which T breaks up into blocks. */
/*          The first block consists of rows/columns 1 to ISPLIT(1), */
/*          the second of rows/columns ISPLIT(1)+1 through ISPLIT(2), */
/*          etc., and the NSPLIT-th consists of rows/columns */
/*          ISPLIT(NSPLIT-1)+1 through ISPLIT(NSPLIT)=N. */


/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Beresford Parlett, University of California, Berkeley, USA */
/*     Jim Demmel, University of California, Berkeley, USA */
/*     Inderjit Dhillon, University of Texas, Austin, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --isplit;
    --e2;
    --e;
    --d__;

    /* Function Body */
    *info = 0;
/*     Compute splitting points */
    *nsplit = 1;
    if (*spltol < 0.) {
/*        Criterion based on absolute off-diagonal value */
	tmp1 = abs(*spltol) * *tnrm;
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    eabs = (d__1 = e[i__], abs(d__1));
	    if (eabs <= tmp1) {
		e[i__] = 0.;
		e2[i__] = 0.;
		isplit[*nsplit] = i__;
		++(*nsplit);
	    }
/* L9: */
	}
    } else {
/*        Criterion that guarantees relative accuracy */
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    eabs = (d__1 = e[i__], abs(d__1));
	    if (eabs <= *spltol * sqrt((d__1 = d__[i__], abs(d__1))) * sqrt((
		    d__2 = d__[i__ + 1], abs(d__2)))) {
		e[i__] = 0.;
		e2[i__] = 0.;
		isplit[*nsplit] = i__;
		++(*nsplit);
	    }
/* L10: */
	}
    }
    isplit[*nsplit] = *n;
    return 0;

/*     End of DLARRA */

} /* dlarra_ */

/* Subroutine */ int dlarrb_(integer *n, double *d__, double *lld,
	integer *ifirst, integer *ilast, double *rtol1, double *rtol2,
	 integer *offset, double *w, double *wgap, double *werr,
	double *work, integer *iwork, double *pivmin, double *
	spdiam, integer *twist, integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    integer i__, k, r__, i1, ii, ip;
    double gap, mid, tmp, back, lgap, rgap, left;
    integer iter, nint, prev, next;
    double cvrgd, right, width;
    integer negcnt;
    double mnwdth;
    integer olnint, maxitr;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Given the relatively robust representation(RRR) L D L^T, DLARRB */
/*  does "limited" bisection to refine the eigenvalues of L D L^T, */
/*  W( IFIRST-OFFSET ) through W( ILAST-OFFSET ), to more accuracy. Initial */
/*  guesses for these eigenvalues are input in W, the corresponding estimate */
/*  of the error in these guesses and their gaps are input in WERR */
/*  and WGAP, respectively. During bisection, intervals */
/*  [left, right] are maintained by storing their mid-points and */
/*  semi-widths in the arrays W and WERR respectively. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The N diagonal elements of the diagonal matrix D. */

/*  LLD     (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (N-1) elements L(i)*L(i)*D(i). */

/*  IFIRST  (input) INTEGER */
/*          The index of the first eigenvalue to be computed. */

/*  ILAST   (input) INTEGER */
/*          The index of the last eigenvalue to be computed. */

/*  RTOL1   (input) DOUBLE PRECISION */
/*  RTOL2   (input) DOUBLE PRECISION */
/*          Tolerance for the convergence of the bisection intervals. */
/*          An interval [LEFT,RIGHT] has converged if */
/*          RIGHT-LEFT.LT.MAX( RTOL1*GAP, RTOL2*MAX(|LEFT|,|RIGHT|) ) */
/*          where GAP is the (estimated) distance to the nearest */
/*          eigenvalue. */

/*  OFFSET  (input) INTEGER */
/*          Offset for the arrays W, WGAP and WERR, i.e., the IFIRST-OFFSET */
/*          through ILAST-OFFSET elements of these arrays are to be used. */

/*  W       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On input, W( IFIRST-OFFSET ) through W( ILAST-OFFSET ) are */
/*          estimates of the eigenvalues of L D L^T indexed IFIRST throug */
/*          ILAST. */
/*          On output, these estimates are refined. */

/*  WGAP    (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On input, the (estimated) gaps between consecutive */
/*          eigenvalues of L D L^T, i.e., WGAP(I-OFFSET) is the gap between */
/*          eigenvalues I and I+1. Note that if IFIRST.EQ.ILAST */
/*          then WGAP(IFIRST-OFFSET) must be set to ZERO. */
/*          On output, these gaps are refined. */

/*  WERR    (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On input, WERR( IFIRST-OFFSET ) through WERR( ILAST-OFFSET ) are */
/*          the errors in the estimates of the corresponding elements in W. */
/*          On output, these errors are refined. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*N) */
/*          Workspace. */

/*  IWORK   (workspace) INTEGER array, dimension (2*N) */
/*          Workspace. */

/*  PIVMIN  (input) DOUBLE PRECISION */
/*          The minimum pivot in the Sturm sequence. */

/*  SPDIAM  (input) DOUBLE PRECISION */
/*          The spectral diameter of the matrix. */

/*  TWIST   (input) INTEGER */
/*          The twist index for the twisted factorization that is used */
/*          for the negcount. */
/*          TWIST = N: Compute negcount from L D L^T - LAMBDA I = L+ D+ L+^T */
/*          TWIST = 1: Compute negcount from L D L^T - LAMBDA I = U- D- U-^T */
/*          TWIST = R: Compute negcount from L D L^T - LAMBDA I = N(r) D(r) N(r) */

/*  INFO    (output) INTEGER */
/*          Error flag. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Beresford Parlett, University of California, Berkeley, USA */
/*     Jim Demmel, University of California, Berkeley, USA */
/*     Inderjit Dhillon, University of Texas, Austin, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */

/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --iwork;
    --work;
    --werr;
    --wgap;
    --w;
    --lld;
    --d__;

    /* Function Body */
    *info = 0;

    maxitr = (integer) ((log(*spdiam + *pivmin) - log(*pivmin)) / log(2.)) +
	    2;
    mnwdth = *pivmin * 2.;

    r__ = *twist;
    if (r__ < 1 || r__ > *n) {
	r__ = *n;
    }

/*     Initialize unconverged intervals in [ WORK(2*I-1), WORK(2*I) ]. */
/*     The Sturm Count, Count( WORK(2*I-1) ) is arranged to be I-1, while */
/*     Count( WORK(2*I) ) is stored in IWORK( 2*I ). The integer IWORK( 2*I-1 ) */
/*     for an unconverged interval is set to the index of the next unconverged */
/*     interval, and is -1 or 0 for a converged interval. Thus a linked */
/*     list of unconverged intervals is set up. */

    i1 = *ifirst;
/*     The number of unconverged intervals */
    nint = 0;
/*     The last unconverged interval found */
    prev = 0;
    rgap = wgap[i1 - *offset];
    i__1 = *ilast;
    for (i__ = i1; i__ <= i__1; ++i__) {
	k = i__ << 1;
	ii = i__ - *offset;
	left = w[ii] - werr[ii];
	right = w[ii] + werr[ii];
	lgap = rgap;
	rgap = wgap[ii];
	gap = std::min(lgap,rgap);
/*        Make sure that [LEFT,RIGHT] contains the desired eigenvalue */
/*        Compute negcount from dstqds facto L+D+L+^T = L D L^T - LEFT */

/*        Do while( NEGCNT(LEFT).GT.I-1 ) */

	back = werr[ii];
L20:
	negcnt = dlaneg_(n, &d__[1], &lld[1], &left, pivmin, &r__);
	if (negcnt > i__ - 1) {
	    left -= back;
	    back *= 2.;
	    goto L20;
	}

/*        Do while( NEGCNT(RIGHT).LT.I ) */
/*        Compute negcount from dstqds facto L+D+L+^T = L D L^T - RIGHT */

	back = werr[ii];
L50:
	negcnt = dlaneg_(n, &d__[1], &lld[1], &right, pivmin, &r__);
	if (negcnt < i__) {
	    right += back;
	    back *= 2.;
	    goto L50;
	}
	width = (d__1 = left - right, abs(d__1)) * .5;
/* Computing MAX */
	d__1 = abs(left), d__2 = abs(right);
	tmp = std::max(d__1,d__2);
/* Computing MAX */
	d__1 = *rtol1 * gap, d__2 = *rtol2 * tmp;
	cvrgd = std::max(d__1,d__2);
	if (width <= cvrgd || width <= mnwdth) {
/*           This interval has already converged and does not need refinement. */
/*           (Note that the gaps might change through refining the */
/*            eigenvalues, however, they can only get bigger.) */
/*           Remove it from the list. */
	    iwork[k - 1] = -1;
/*           Make sure that I1 always points to the first unconverged interval */
	    if (i__ == i1 && i__ < *ilast) {
		i1 = i__ + 1;
	    }
	    if (prev >= i1 && i__ <= *ilast) {
		iwork[(prev << 1) - 1] = i__ + 1;
	    }
	} else {
/*           unconverged interval found */
	    prev = i__;
	    ++nint;
	    iwork[k - 1] = i__ + 1;
	    iwork[k] = negcnt;
	}
	work[k - 1] = left;
	work[k] = right;
/* L75: */
    }

/*     Do while( NINT.GT.0 ), i.e. there are still unconverged intervals */
/*     and while (ITER.LT.MAXITR) */

    iter = 0;
L80:
    prev = i1 - 1;
    i__ = i1;
    olnint = nint;
    i__1 = olnint;
    for (ip = 1; ip <= i__1; ++ip) {
	k = i__ << 1;
	ii = i__ - *offset;
	rgap = wgap[ii];
	lgap = rgap;
	if (ii > 1) {
	    lgap = wgap[ii - 1];
	}
	gap = std::min(lgap,rgap);
	next = iwork[k - 1];
	left = work[k - 1];
	right = work[k];
	mid = (left + right) * .5;
/*        semiwidth of interval */
	width = right - mid;
/* Computing MAX */
	d__1 = abs(left), d__2 = abs(right);
	tmp = std::max(d__1,d__2);
/* Computing MAX */
	d__1 = *rtol1 * gap, d__2 = *rtol2 * tmp;
	cvrgd = std::max(d__1,d__2);
	if (width <= cvrgd || width <= mnwdth || iter == maxitr) {
/*           reduce number of unconverged intervals */
	    --nint;
/*           Mark interval as converged. */
	    iwork[k - 1] = 0;
	    if (i1 == i__) {
		i1 = next;
	    } else {
/*              Prev holds the last unconverged interval previously examined */
		if (prev >= i1) {
		    iwork[(prev << 1) - 1] = next;
		}
	    }
	    i__ = next;
	    goto L100;
	}
	prev = i__;

/*        Perform one bisection step */

	negcnt = dlaneg_(n, &d__[1], &lld[1], &mid, pivmin, &r__);
	if (negcnt <= i__ - 1) {
	    work[k - 1] = mid;
	} else {
	    work[k] = mid;
	}
	i__ = next;
L100:
	;
    }
    ++iter;
/*     do another loop if there are still unconverged intervals */
/*     However, in the last iteration, all intervals are accepted */
/*     since this is the best we can do. */
    if (nint > 0 && iter <= maxitr) {
	goto L80;
    }


/*     At this point, all the intervals have converged */
    i__1 = *ilast;
    for (i__ = *ifirst; i__ <= i__1; ++i__) {
	k = i__ << 1;
	ii = i__ - *offset;
/*        All intervals marked by '0' have been refined. */
	if (iwork[k - 1] == 0) {
	    w[ii] = (work[k - 1] + work[k]) * .5;
	    werr[ii] = work[k] - w[ii];
	}
/* L110: */
    }

    i__1 = *ilast;
    for (i__ = *ifirst + 1; i__ <= i__1; ++i__) {
	k = i__ << 1;
	ii = i__ - *offset;
/* Computing MAX */
	d__1 = 0., d__2 = w[ii] - werr[ii] - w[ii - 1] - werr[ii - 1];
	wgap[ii - 1] = std::max(d__1,d__2);
/* L111: */
    }
    return 0;

/*     End of DLARRB */

} /* dlarrb_ */

/* Subroutine */ int dlarrc_(const char *jobt, integer *n, double *vl,
	double *vu, double *d__, double *e, double *pivmin,
	integer *eigcnt, integer *lcnt, integer *rcnt, integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1;

    /* Local variables */
    integer i__;
    double sl, su, tmp, tmp2;
    bool matt;

    double lpivot, rpivot;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Find the number of eigenvalues of the symmetric tridiagonal matrix T */
/*  that are in the interval (VL,VU] if JOBT = 'T', and of L D L^T */
/*  if JOBT = 'L'. */

/*  Arguments */
/*  ========= */

/*  JOBT    (input) CHARACTER*1 */
/*          = 'T':  Compute Sturm count for matrix T. */
/*          = 'L':  Compute Sturm count for matrix L D L^T. */

/*  N       (input) INTEGER */
/*          The order of the matrix. N > 0. */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          The lower and upper bounds for the eigenvalues. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          JOBT = 'T': The N diagonal elements of the tridiagonal matrix T. */
/*          JOBT = 'L': The N diagonal elements of the diagonal matrix D. */

/*  E       (input) DOUBLE PRECISION array, dimension (N) */
/*          JOBT = 'T': The N-1 offdiagonal elements of the matrix T. */
/*          JOBT = 'L': The N-1 offdiagonal elements of the matrix L. */

/*  PIVMIN  (input) DOUBLE PRECISION */
/*          The minimum pivot in the Sturm sequence for T. */

/*  EIGCNT  (output) INTEGER */
/*          The number of eigenvalues of the symmetric tridiagonal matrix T */
/*          that are in the interval (VL,VU] */

/*  LCNT    (output) INTEGER */
/*  RCNT    (output) INTEGER */
/*          The left and right negcounts of the interval. */

/*  INFO    (output) INTEGER */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Beresford Parlett, University of California, Berkeley, USA */
/*     Jim Demmel, University of California, Berkeley, USA */
/*     Inderjit Dhillon, University of Texas, Austin, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --e;
    --d__;

    /* Function Body */
    *info = 0;
    *lcnt = 0;
    *rcnt = 0;
    *eigcnt = 0;
    matt = lsame_(jobt, "T");
    if (matt) {
/*        Sturm sequence count on T */
	lpivot = d__[1] - *vl;
	rpivot = d__[1] - *vu;
	if (lpivot <= 0.) {
	    ++(*lcnt);
	}
	if (rpivot <= 0.) {
	    ++(*rcnt);
	}
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	    d__1 = e[i__];
	    tmp = d__1 * d__1;
	    lpivot = d__[i__ + 1] - *vl - tmp / lpivot;
	    rpivot = d__[i__ + 1] - *vu - tmp / rpivot;
	    if (lpivot <= 0.) {
		++(*lcnt);
	    }
	    if (rpivot <= 0.) {
		++(*rcnt);
	    }
/* L10: */
	}
    } else {
/*        Sturm sequence count on L D L^T */
	sl = -(*vl);
	su = -(*vu);
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    lpivot = d__[i__] + sl;
	    rpivot = d__[i__] + su;
	    if (lpivot <= 0.) {
		++(*lcnt);
	    }
	    if (rpivot <= 0.) {
		++(*rcnt);
	    }
	    tmp = e[i__] * d__[i__] * e[i__];

	    tmp2 = tmp / lpivot;
	    if (tmp2 == 0.) {
		sl = tmp - *vl;
	    } else {
		sl = sl * tmp2 - *vl;
	    }

	    tmp2 = tmp / rpivot;
	    if (tmp2 == 0.) {
		su = tmp - *vu;
	    } else {
		su = su * tmp2 - *vu;
	    }
/* L20: */
	}
	lpivot = d__[*n] + sl;
	rpivot = d__[*n] + su;
	if (lpivot <= 0.) {
	    ++(*lcnt);
	}
	if (rpivot <= 0.) {
	    ++(*rcnt);
	}
    }
    *eigcnt = *rcnt - *lcnt;
    return 0;

/*     end of DLARRC */

} /* dlarrc_ */

/* Subroutine */ int dlarrd_(const char *range, const char *order, integer *n, double *vl, double *vu, integer *il,
	integer *iu, double *gers, double *reltol, double *d__, double *e, double *e2,	double *pivmin,
	integer *nsplit, integer *isplit, integer *m, double *w, double *werr, double *wl, double *wu,
	integer *iblock, integer *indexw, double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;
	static integer c__0 = 0;

    /* System generated locals */
    integer i__1, i__2, i__3;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, ib, ie, je, nb;
    double gl;
    integer im, in;
    double gu;
    integer iw, jee;
    double eps;
    integer nwl;
    double wlu, wul;
    integer nwu;
    double tmp1, tmp2;
    integer iend, jblk, ioff, iout, itmp1, itmp2, jdisc, iinfo;
    double atoli;
    integer iwoff, itmax;
    double wkill, rtoli, uflow, tnorm;
    integer ibegin,irange, idiscl, idumma[1], idiscu;
    bool ncnvrg, toofew;


/*  -- LAPACK auxiliary routine (version 3.2.1)                        -- */
/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */
/*  -- April 2009                                                      -- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARRD computes the eigenvalues of a symmetric tridiagonal */
/*  matrix T to suitable accuracy. This is an auxiliary code to be */
/*  called from DSTEMR. */
/*  The user may ask for all eigenvalues, all eigenvalues */
/*  in the half-open interval (VL, VU], or the IL-th through IU-th */
/*  eigenvalues. */

/*  To avoid overflow, the matrix must be scaled so that its */
/*  largest element is no greater than overflow**(1/2) * */
/*  underflow**(1/4) in absolute value, and for greatest */
/*  accuracy, it should not be much smaller than that. */

/*  See W. Kahan "Accurate Eigenvalues of a Symmetric Tridiagonal */
/*  Matrix", Report CS41, Computer Science Dept., Stanford */
/*  University, July 21, 1966. */

/*  Arguments */
/*  ========= */

/*  RANGE   (input) CHARACTER */
/*          = 'A': ("All")   all eigenvalues will be found. */
/*          = 'V': ("Value") all eigenvalues in the half-open interval */
/*                           (VL, VU] will be found. */
/*          = 'I': ("Index") the IL-th through IU-th eigenvalues (of the */
/*                           entire matrix) will be found. */

/*  ORDER   (input) CHARACTER */
/*          = 'B': ("By Block") the eigenvalues will be grouped by */
/*                              split-off block (see IBLOCK, ISPLIT) and */
/*                              ordered from smallest to largest within */
/*                              the block. */
/*          = 'E': ("Entire matrix") */
/*                              the eigenvalues for the entire matrix */
/*                              will be ordered from smallest to */
/*                              largest. */

/*  N       (input) INTEGER */
/*          The order of the tridiagonal matrix T.  N >= 0. */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues.  Eigenvalues less than or equal */
/*          to VL, or greater than VU, will not be returned.  VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  GERS    (input) DOUBLE PRECISION array, dimension (2*N) */
/*          The N Gerschgorin intervals (the i-th Gerschgorin interval */
/*          is (GERS(2*i-1), GERS(2*i)). */

/*  RELTOL  (input) DOUBLE PRECISION */
/*          The minimum relative width of an interval.  When an interval */
/*          is narrower than RELTOL times the larger (in */
/*          magnitude) endpoint, then it is considered to be */
/*          sufficiently small, i.e., converged.  Note: this should */
/*          always be at least radix*machine epsilon. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the tridiagonal matrix T. */

/*  E       (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) off-diagonal elements of the tridiagonal matrix T. */

/*  E2      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) squared off-diagonal elements of the tridiagonal matrix T. */

/*  PIVMIN  (input) DOUBLE PRECISION */
/*          The minimum pivot allowed in the Sturm sequence for T. */

/*  NSPLIT  (input) INTEGER */
/*          The number of diagonal blocks in the matrix T. */
/*          1 <= NSPLIT <= N. */

/*  ISPLIT  (input) INTEGER array, dimension (N) */
/*          The splitting points, at which T breaks up into submatrices. */
/*          The first submatrix consists of rows/columns 1 to ISPLIT(1), */
/*          the second of rows/columns ISPLIT(1)+1 through ISPLIT(2), */
/*          etc., and the NSPLIT-th consists of rows/columns */
/*          ISPLIT(NSPLIT-1)+1 through ISPLIT(NSPLIT)=N. */
/*          (Only the first NSPLIT elements will actually be used, but */
/*          since the user cannot know a priori what value NSPLIT will */
/*          have, N words must be reserved for ISPLIT.) */

/*  M       (output) INTEGER */
/*          The actual number of eigenvalues found. 0 <= M <= N. */
/*          (See also the description of INFO=2,3.) */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, the first M elements of W will contain the */
/*          eigenvalue approximations. DLARRD computes an interval */
/*          I_j = (a_j, b_j] that includes eigenvalue j. The eigenvalue */
/*          approximation is given as the interval midpoint */
/*          W(j)= ( a_j + b_j)/2. The corresponding error is bounded by */
/*          WERR(j) = abs( a_j - b_j)/2 */

/*  WERR    (output) DOUBLE PRECISION array, dimension (N) */
/*          The error bound on the corresponding eigenvalue approximation */
/*          in W. */

/*  WL      (output) DOUBLE PRECISION */
/*  WU      (output) DOUBLE PRECISION */
/*          The interval (WL, WU] contains all the wanted eigenvalues. */
/*          If RANGE='V', then WL=VL and WU=VU. */
/*          If RANGE='A', then WL and WU are the global Gerschgorin bounds */
/*                        on the spectrum. */
/*          If RANGE='I', then WL and WU are computed by DLAEBZ from the */
/*                        index range specified. */

/*  IBLOCK  (output) INTEGER array, dimension (N) */
/*          At each row/column j where E(j) is zero or small, the */
/*          matrix T is considered to split into a block diagonal */
/*          matrix.  On exit, if INFO = 0, IBLOCK(i) specifies to which */
/*          block (from 1 to the number of blocks) the eigenvalue W(i) */
/*          belongs.  (DLARRD may use the remaining N-M elements as */
/*          workspace.) */

/*  INDEXW  (output) INTEGER array, dimension (N) */
/*          The indices of the eigenvalues within each block (submatrix); */
/*          for example, INDEXW(i)= j and IBLOCK(i)=k imply that the */
/*          i-th eigenvalue W(i) is the j-th eigenvalue in block k. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (4*N) */

/*  IWORK   (workspace) INTEGER array, dimension (3*N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  some or all of the eigenvalues failed to converge or */
/*                were not computed: */
/*                =1 or 3: Bisection failed to converge for some */
/*                        eigenvalues; these eigenvalues are flagged by a */
/*                        negative block number.  The effect is that the */
/*                        eigenvalues may not be as accurate as the */
/*                        absolute and relative tolerances.  This is */
/*                        generally caused by unexpectedly inaccurate */
/*                        arithmetic. */
/*                =2 or 3: RANGE='I' only: Not all of the eigenvalues */
/*                        IL:IU were found. */
/*                        Effect: M < IU+1-IL */
/*                        Cause:  non-monotonic arithmetic, causing the */
/*                                Sturm sequence to be non-monotonic. */
/*                        Cure:   recalculate, using RANGE='A', and pick */
/*                                out eigenvalues IL:IU.  In some cases, */
/*                                increasing the PARAMETER "FUDGE" may */
/*                                make things work. */
/*                = 4:    RANGE='I', and the Gershgorin interval */
/*                        initially used was too small.  No eigenvalues */
/*                        were computed. */
/*                        Probable cause: your machine has sloppy */
/*                                        floating-point arithmetic. */
/*                        Cure: Increase the PARAMETER "FUDGE", */
/*                              recompile, and try again. */

/*  Internal Parameters */
/*  =================== */

/*  FUDGE   DOUBLE PRECISION, default = 2 */
/*          A "fudge factor" to widen the Gershgorin intervals.  Ideally, */
/*          a value of 1 should work, but on machines with sloppy */
/*          arithmetic, this needs to be larger.  The default for */
/*          publicly released versions should be large enough to handle */
/*          the worst machine around.  Note that this has no effect */
/*          on accuracy of the solution. */

/*  Based on contributions by */
/*     W. Kahan, University of California, Berkeley, USA */
/*     Beresford Parlett, University of California, Berkeley, USA */
/*     Jim Demmel, University of California, Berkeley, USA */
/*     Inderjit Dhillon, University of Texas, Austin, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --iwork;
    --work;
    --indexw;
    --iblock;
    --werr;
    --w;
    --isplit;
    --e2;
    --e;
    --d__;
    --gers;

    /* Function Body */
    *info = 0;

/*     Decode RANGE */

    if (lsame_(range, "A")) {
	irange = 1;
    } else if (lsame_(range, "V")) {
	irange = 2;
    } else if (lsame_(range, "I")) {
	irange = 3;
    } else {
	irange = 0;
    }

/*     Check for Errors */

    if (irange <= 0) {
	*info = -1;
    } else if (! (lsame_(order, "B") || lsame_(order,
	    "E"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (irange == 2) {
	if (*vl >= *vu) {
	    *info = -5;
	}
    } else if (irange == 3 && (*il < 1 || *il > std::max(1_integer,*n))) {
	*info = -6;
    } else if (irange == 3 && (*iu < std::min(*n,*il) || *iu > *n)) {
	*info = -7;
    }

    if (*info != 0) {
	return 0;
    }
/*     Initialize error flags */
    *info = 0;
    ncnvrg = false;
    toofew = false;
/*     Quick return if possible */
    *m = 0;
    if (*n == 0) {
	return 0;
    }
/*     Simplification: */
    if (irange == 3 && *il == 1 && *iu == *n) {
	irange = 1;
    }
/*     Get machine constants */
    eps = dlamch_("P");
    uflow = dlamch_("U");
/*     Special Case when N=1 */
/*     Treat case of 1x1 matrix for quick return */
    if (*n == 1) {
	if (irange == 1 || irange == 2 && d__[1] > *vl && d__[1] <= *vu ||
		irange == 3 && *il == 1 && *iu == 1) {
	    *m = 1;
	    w[1] = d__[1];
/*           The computation error of the eigenvalue is zero */
	    werr[1] = 0.;
	    iblock[1] = 1;
	    indexw[1] = 1;
	}
	return 0;
    }
/*     NB is the minimum vector length for vector bisection, or 0 */
/*     if only scalar is to be done. */
    nb = ilaenv_(&c__1, "DSTEBZ", " ", n, &c_n1, &c_n1, &c_n1);
    if (nb <= 1) {
	nb = 0;
    }
/*     Find global spectral radius */
    gl = d__[1];
    gu = d__[1];
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MIN */
	d__1 = gl, d__2 = gers[(i__ << 1) - 1];
	gl = std::min(d__1,d__2);
/* Computing MAX */
	d__1 = gu, d__2 = gers[i__ * 2];
	gu = std::max(d__1,d__2);
/* L5: */
    }
/*     Compute global Gerschgorin bounds and spectral diameter */
/* Computing MAX */
    d__1 = abs(gl), d__2 = abs(gu);
    tnorm = std::max(d__1,d__2);
    gl = gl - tnorm * 2. * eps * *n - *pivmin * 4.;
    gu = gu + tnorm * 2. * eps * *n + *pivmin * 4.;
/*     [JAN/28/2009] remove the line below since SPDIAM variable not use */
/*     SPDIAM = GU - GL */
/*     Input arguments for DLAEBZ: */
/*     The relative tolerance.  An interval (a,b] lies within */
/*     "relative tolerance" if  b-a < RELTOL*max(|a|,|b|), */
    rtoli = *reltol;
/*     Set the absolute tolerance for interval convergence to zero to force */
/*     interval convergence based on relative size of the interval. */
/*     This is dangerous because intervals might not converge when RELTOL is */
/*     small. But at least a very small number should be selected so that for */
/*     strongly graded matrices, the code can get relatively accurate */
/*     eigenvalues. */
    atoli = uflow * 4. + *pivmin * 4.;
    if (irange == 3) {
/*        RANGE='I': Compute an interval containing eigenvalues */
/*        IL through IU. The initial interval [GL,GU] from the global */
/*        Gerschgorin bounds GL and GU is refined by DLAEBZ. */
	itmax = (integer) ((log(tnorm + *pivmin) - log(*pivmin)) / log(2.)) +
		2;
	work[*n + 1] = gl;
	work[*n + 2] = gl;
	work[*n + 3] = gu;
	work[*n + 4] = gu;
	work[*n + 5] = gl;
	work[*n + 6] = gu;
	iwork[1] = -1;
	iwork[2] = -1;
	iwork[3] = *n + 1;
	iwork[4] = *n + 1;
	iwork[5] = *il - 1;
	iwork[6] = *iu;

	dlaebz_(&c__3, &itmax, n, &c__2, &c__2, &nb, &atoli, &rtoli, pivmin, &
		d__[1], &e[1], &e2[1], &iwork[5], &work[*n + 1], &work[*n + 5]
, &iout, &iwork[1], &w[1], &iblock[1], &iinfo);
	if (iinfo != 0) {
	    *info = iinfo;
	    return 0;
	}
/*        On exit, output intervals may not be ordered by ascending negcount */
	if (iwork[6] == *iu) {
	    *wl = work[*n + 1];
	    wlu = work[*n + 3];
	    nwl = iwork[1];
	    *wu = work[*n + 4];
	    wul = work[*n + 2];
	    nwu = iwork[4];
	} else {
	    *wl = work[*n + 2];
	    wlu = work[*n + 4];
	    nwl = iwork[2];
	    *wu = work[*n + 3];
	    wul = work[*n + 1];
	    nwu = iwork[3];
	}
/*        On exit, the interval [WL, WLU] contains a value with negcount NWL, */
/*        and [WUL, WU] contains a value with negcount NWU. */
	if (nwl < 0 || nwl >= *n || nwu < 1 || nwu > *n) {
	    *info = 4;
	    return 0;
	}
    } else if (irange == 2) {
	*wl = *vl;
	*wu = *vu;
    } else if (irange == 1) {
	*wl = gl;
	*wu = gu;
    }
/*     Find Eigenvalues -- Loop Over blocks and recompute NWL and NWU. */
/*     NWL accumulates the number of eigenvalues .le. WL, */
/*     NWU accumulates the number of eigenvalues .le. WU */
    *m = 0;
    iend = 0;
    *info = 0;
    nwl = 0;
    nwu = 0;

    i__1 = *nsplit;
    for (jblk = 1; jblk <= i__1; ++jblk) {
	ioff = iend;
	ibegin = ioff + 1;
	iend = isplit[jblk];
	in = iend - ioff;

	if (in == 1) {
/*           1x1 block */
	    if (*wl >= d__[ibegin] - *pivmin) {
		++nwl;
	    }
	    if (*wu >= d__[ibegin] - *pivmin) {
		++nwu;
	    }
	    if (irange == 1 || *wl < d__[ibegin] - *pivmin && *wu >= d__[
		    ibegin] - *pivmin) {
		++(*m);
		w[*m] = d__[ibegin];
		werr[*m] = 0.;
/*              The gap for a single block doesn't matter for the later */
/*              algorithm and is assigned an arbitrary large value */
		iblock[*m] = jblk;
		indexw[*m] = 1;
	    }
/*        Disabled 2x2 case because of a failure on the following matrix */
/*        RANGE = 'I', IL = IU = 4 */
/*          Original Tridiagonal, d = [ */
/*           -0.150102010615740E+00 */
/*           -0.849897989384260E+00 */
/*           -0.128208148052635E-15 */
/*            0.128257718286320E-15 */
/*          ]; */
/*          e = [ */
/*           -0.357171383266986E+00 */
/*           -0.180411241501588E-15 */
/*           -0.175152352710251E-15 */
/*          ]; */

/*         ELSE IF( IN.EQ.2 ) THEN */
/* *           2x2 block */
/*            DISC = SQRT( (HALF*(D(IBEGIN)-D(IEND)))**2 + E(IBEGIN)**2 ) */
/*            TMP1 = HALF*(D(IBEGIN)+D(IEND)) */
/*            L1 = TMP1 - DISC */
/*            IF( WL.GE. L1-PIVMIN ) */
/*     $         NWL = NWL + 1 */
/*            IF( WU.GE. L1-PIVMIN ) */
/*     $         NWU = NWU + 1 */
/*            IF( IRANGE.EQ.ALLRNG .OR. ( WL.LT.L1-PIVMIN .AND. WU.GE. */
/*     $          L1-PIVMIN ) ) THEN */
/*               M = M + 1 */
/*               W( M ) = L1 */
/* *              The uncertainty of eigenvalues of a 2x2 matrix is very small */
/*               WERR( M ) = EPS * ABS( W( M ) ) * TWO */
/*               IBLOCK( M ) = JBLK */
/*               INDEXW( M ) = 1 */
/*            ENDIF */
/*            L2 = TMP1 + DISC */
/*            IF( WL.GE. L2-PIVMIN ) */
/*     $         NWL = NWL + 1 */
/*            IF( WU.GE. L2-PIVMIN ) */
/*     $         NWU = NWU + 1 */
/*            IF( IRANGE.EQ.ALLRNG .OR. ( WL.LT.L2-PIVMIN .AND. WU.GE. */
/*     $          L2-PIVMIN ) ) THEN */
/*               M = M + 1 */
/*               W( M ) = L2 */
/* *              The uncertainty of eigenvalues of a 2x2 matrix is very small */
/*               WERR( M ) = EPS * ABS( W( M ) ) * TWO */
/*               IBLOCK( M ) = JBLK */
/*               INDEXW( M ) = 2 */
/*            ENDIF */
	} else {
/*           General Case - block of size IN >= 2 */
/*           Compute local Gerschgorin interval and use it as the initial */
/*           interval for DLAEBZ */
	    gu = d__[ibegin];
	    gl = d__[ibegin];
	    tmp1 = 0.;
	    i__2 = iend;
	    for (j = ibegin; j <= i__2; ++j) {
/* Computing MIN */
		d__1 = gl, d__2 = gers[(j << 1) - 1];
		gl = std::min(d__1,d__2);
/* Computing MAX */
		d__1 = gu, d__2 = gers[j * 2];
		gu = std::max(d__1,d__2);
/* L40: */
	    }
/*           [JAN/28/2009] */
/*           change SPDIAM by TNORM in lines 2 and 3 thereafter */
/*           line 1: remove computation of SPDIAM (not useful anymore) */
/*           SPDIAM = GU - GL */
/*           GL = GL - FUDGE*SPDIAM*EPS*IN - FUDGE*PIVMIN */
/*           GU = GU + FUDGE*SPDIAM*EPS*IN + FUDGE*PIVMIN */
	    gl = gl - tnorm * 2. * eps * in - *pivmin * 2.;
	    gu = gu + tnorm * 2. * eps * in + *pivmin * 2.;

	    if (irange > 1) {
		if (gu < *wl) {
/*                 the local block contains none of the wanted eigenvalues */
		    nwl += in;
		    nwu += in;
		    goto L70;
		}
/*              refine search interval if possible, only range (WL,WU] matters */
		gl = std::max(gl,*wl);
		gu = std::min(gu,*wu);
		if (gl >= gu) {
		    goto L70;
		}
	    }
/*           Find negcount of initial interval boundaries GL and GU */
	    work[*n + 1] = gl;
	    work[*n + in + 1] = gu;
	    dlaebz_(&c__1, &c__0, &in, &in, &c__1, &nb, &atoli, &rtoli,
		    pivmin, &d__[ibegin], &e[ibegin], &e2[ibegin], idumma, &
		    work[*n + 1], &work[*n + (in << 1) + 1], &im, &iwork[1], &
		    w[*m + 1], &iblock[*m + 1], &iinfo);
	    if (iinfo != 0) {
		*info = iinfo;
		return 0;
	    }

	    nwl += iwork[1];
	    nwu += iwork[in + 1];
	    iwoff = *m - iwork[1];
/*           Compute Eigenvalues */
	    itmax = (integer) ((log(gu - gl + *pivmin) - log(*pivmin)) / log(
		    2.)) + 2;
	    dlaebz_(&c__2, &itmax, &in, &in, &c__1, &nb, &atoli, &rtoli,
		    pivmin, &d__[ibegin], &e[ibegin], &e2[ibegin], idumma, &
		    work[*n + 1], &work[*n + (in << 1) + 1], &iout, &iwork[1],
		     &w[*m + 1], &iblock[*m + 1], &iinfo);
	    if (iinfo != 0) {
		*info = iinfo;
		return 0;
	    }

/*           Copy eigenvalues into W and IBLOCK */
/*           Use -JBLK for block number for unconverged eigenvalues. */
/*           Loop over the number of output intervals from DLAEBZ */
	    i__2 = iout;
	    for (j = 1; j <= i__2; ++j) {
/*              eigenvalue approximation is middle point of interval */
		tmp1 = (work[j + *n] + work[j + in + *n]) * .5;
/*              semi length of error interval */
		tmp2 = (d__1 = work[j + *n] - work[j + in + *n], abs(d__1)) *
			.5;
		if (j > iout - iinfo) {
/*                 Flag non-convergence. */
		    ncnvrg = true;
		    ib = -jblk;
		} else {
		    ib = jblk;
		}
		i__3 = iwork[j + in] + iwoff;
		for (je = iwork[j] + 1 + iwoff; je <= i__3; ++je) {
		    w[je] = tmp1;
		    werr[je] = tmp2;
		    indexw[je] = je - iwoff;
		    iblock[je] = ib;
/* L50: */
		}
/* L60: */
	    }

	    *m += im;
	}
L70:
	;
    }
/*     If RANGE='I', then (WL,WU) contains eigenvalues NWL+1,...,NWU */
/*     If NWL+1 < IL or NWU > IU, discard extra eigenvalues. */
    if (irange == 3) {
	idiscl = *il - 1 - nwl;
	idiscu = nwu - *iu;

	if (idiscl > 0) {
	    im = 0;
	    i__1 = *m;
	    for (je = 1; je <= i__1; ++je) {
/*              Remove some of the smallest eigenvalues from the left so that */
/*              at the end IDISCL =0. Move all eigenvalues up to the left. */
		if (w[je] <= wlu && idiscl > 0) {
		    --idiscl;
		} else {
		    ++im;
		    w[im] = w[je];
		    werr[im] = werr[je];
		    indexw[im] = indexw[je];
		    iblock[im] = iblock[je];
		}
/* L80: */
	    }
	    *m = im;
	}
	if (idiscu > 0) {
/*           Remove some of the largest eigenvalues from the right so that */
/*           at the end IDISCU =0. Move all eigenvalues up to the left. */
	    im = *m + 1;
	    for (je = *m; je >= 1; --je) {
		if (w[je] >= wul && idiscu > 0) {
		    --idiscu;
		} else {
		    --im;
		    w[im] = w[je];
		    werr[im] = werr[je];
		    indexw[im] = indexw[je];
		    iblock[im] = iblock[je];
		}
/* L81: */
	    }
	    jee = 0;
	    i__1 = *m;
	    for (je = im; je <= i__1; ++je) {
		++jee;
		w[jee] = w[je];
		werr[jee] = werr[je];
		indexw[jee] = indexw[je];
		iblock[jee] = iblock[je];
/* L82: */
	    }
	    *m = *m - im + 1;
	}
	if (idiscl > 0 || idiscu > 0) {
/*           Code to deal with effects of bad arithmetic. (If N(w) is */
/*           monotone non-decreasing, this should never happen.) */
/*           Some low eigenvalues to be discarded are not in (WL,WLU], */
/*           or high eigenvalues to be discarded are not in (WUL,WU] */
/*           so just kill off the smallest IDISCL/largest IDISCU */
/*           eigenvalues, by marking the corresponding IBLOCK = 0 */
	    if (idiscl > 0) {
		wkill = *wu;
		i__1 = idiscl;
		for (jdisc = 1; jdisc <= i__1; ++jdisc) {
		    iw = 0;
		    i__2 = *m;
		    for (je = 1; je <= i__2; ++je) {
			if (iblock[je] != 0 && (w[je] < wkill || iw == 0)) {
			    iw = je;
			    wkill = w[je];
			}
/* L90: */
		    }
		    iblock[iw] = 0;
/* L100: */
		}
	    }
	    if (idiscu > 0) {
		wkill = *wl;
		i__1 = idiscu;
		for (jdisc = 1; jdisc <= i__1; ++jdisc) {
		    iw = 0;
		    i__2 = *m;
		    for (je = 1; je <= i__2; ++je) {
			if (iblock[je] != 0 && (w[je] >= wkill || iw == 0)) {
			    iw = je;
			    wkill = w[je];
			}
/* L110: */
		    }
		    iblock[iw] = 0;
/* L120: */
		}
	    }
/*           Now erase all eigenvalues with IBLOCK set to zero */
	    im = 0;
	    i__1 = *m;
	    for (je = 1; je <= i__1; ++je) {
		if (iblock[je] != 0) {
		    ++im;
		    w[im] = w[je];
		    werr[im] = werr[je];
		    indexw[im] = indexw[je];
		    iblock[im] = iblock[je];
		}
/* L130: */
	    }
	    *m = im;
	}
	if (idiscl < 0 || idiscu < 0) {
	    toofew = true;
	}
    }

    if (irange == 1 && *m != *n || irange == 3 && *m != *iu - *il + 1) {
	toofew = true;
    }
/*     If ORDER='B', do nothing the eigenvalues are already sorted by */
/*        block. */
/*     If ORDER='E', sort the eigenvalues from smallest to largest */
    if (lsame_(order, "E") && *nsplit > 1) {
	i__1 = *m - 1;
	for (je = 1; je <= i__1; ++je) {
	    ie = 0;
	    tmp1 = w[je];
	    i__2 = *m;
	    for (j = je + 1; j <= i__2; ++j) {
		if (w[j] < tmp1) {
		    ie = j;
		    tmp1 = w[j];
		}
/* L140: */
	    }
	    if (ie != 0) {
		tmp2 = werr[ie];
		itmp1 = iblock[ie];
		itmp2 = indexw[ie];
		w[ie] = w[je];
		werr[ie] = werr[je];
		iblock[ie] = iblock[je];
		indexw[ie] = indexw[je];
		w[je] = tmp1;
		werr[je] = tmp2;
		iblock[je] = itmp1;
		indexw[je] = itmp2;
	    }
/* L150: */
	}
    }

    *info = 0;
    if (ncnvrg) {
	++(*info);
    }
    if (toofew) {
	*info += 2;
    }
    return 0;

/*     End of DLARRD */

} /* dlarrd_ */

/* Subroutine */ int dlarre_(const char *range, integer *n, double *vl,
	double *vu, integer *il, integer *iu, double *d__, double
	*e, double *e2, double *rtol1, double *rtol2, double *
	spltol, integer *nsplit, integer *isplit, integer *m, double *w,
	double *werr, double *wgap, integer *iblock, integer *indexw,
	double *gers, double *pivmin, double *work, integer *
	iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__2 = 2;

    /* System generated locals */
    integer i__1, i__2;
    double d__1, d__2, d__3;

    /* Builtin functions
    double sqrt(double), log(double); */

    /* Local variables */
    integer i__, j;
    double s1, s2;
    integer mb;
    double gl;
    integer in, mm;
    double gu;
    integer cnt;
    double eps, tau, tmp, rtl;
    integer cnt1, cnt2;
    double tmp1, eabs;
    integer iend, jblk;
    double eold;
    integer indl;
    double dmax__, emax;
    integer wend, idum, indu;
    double rtol;
    integer iseed[4];
    double avgap, sigma;
    integer iinfo;
    bool norep;
    integer ibegin;
    bool forceb;
    integer irange;
    double sgndef;
    integer wbegin;
    double safmin, spdiam;
    bool usedqd;
    double clwdth, isleft;
    double isrght, bsrtol, dpivot;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  To find the desired eigenvalues of a given real symmetric */
/*  tridiagonal matrix T, DLARRE sets any "small" off-diagonal */
/*  elements to zero, and for each unreduced block T_i, it finds */
/*  (a) a suitable shift at one end of the block's spectrum, */
/*  (b) the base representation, T_i - sigma_i I = L_i D_i L_i^T, and */
/*  (c) eigenvalues of each L_i D_i L_i^T. */
/*  The representations and eigenvalues found are then used by */
/*  DSTEMR to compute the eigenvectors of T. */
/*  The accuracy varies depending on whether bisection is used to */
/*  find a few eigenvalues or the dqds algorithm (subroutine DLASQ2) to */
/*  conpute all and then discard any unwanted one. */
/*  As an added benefit, DLARRE also outputs the n */
/*  Gerschgorin intervals for the matrices L_i D_i L_i^T. */

/*  Arguments */
/*  ========= */

/*  RANGE   (input) CHARACTER */
/*          = 'A': ("All")   all eigenvalues will be found. */
/*          = 'V': ("Value") all eigenvalues in the half-open interval */
/*                           (VL, VU] will be found. */
/*          = 'I': ("Index") the IL-th through IU-th eigenvalues (of the */
/*                           entire matrix) will be found. */

/*  N       (input) INTEGER */
/*          The order of the matrix. N > 0. */

/*  VL      (input/output) DOUBLE PRECISION */
/*  VU      (input/output) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds for the eigenvalues. */
/*          Eigenvalues less than or equal to VL, or greater than VU, */
/*          will not be returned.  VL < VU. */
/*          If RANGE='I' or ='A', DLARRE computes bounds on the desired */
/*          part of the spectrum. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the N diagonal elements of the tridiagonal */
/*          matrix T. */
/*          On exit, the N diagonal elements of the diagonal */
/*          matrices D_i. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the first (N-1) entries contain the subdiagonal */
/*          elements of the tridiagonal matrix T; E(N) need not be set. */
/*          On exit, E contains the subdiagonal elements of the unit */
/*          bidiagonal matrices L_i. The entries E( ISPLIT( I ) ), */
/*          1 <= I <= NSPLIT, contain the base points sigma_i on output. */

/*  E2      (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the first (N-1) entries contain the SQUARES of the */
/*          subdiagonal elements of the tridiagonal matrix T; */
/*          E2(N) need not be set. */
/*          On exit, the entries E2( ISPLIT( I ) ), */
/*          1 <= I <= NSPLIT, have been set to zero */

/*  RTOL1   (input) DOUBLE PRECISION */
/*  RTOL2   (input) DOUBLE PRECISION */
/*           Parameters for bisection. */
/*           An interval [LEFT,RIGHT] has converged if */
/*           RIGHT-LEFT.LT.MAX( RTOL1*GAP, RTOL2*MAX(|LEFT|,|RIGHT|) ) */

/*  SPLTOL (input) DOUBLE PRECISION */
/*          The threshold for splitting. */

/*  NSPLIT  (output) INTEGER */
/*          The number of blocks T splits into. 1 <= NSPLIT <= N. */

/*  ISPLIT  (output) INTEGER array, dimension (N) */
/*          The splitting points, at which T breaks up into blocks. */
/*          The first block consists of rows/columns 1 to ISPLIT(1), */
/*          the second of rows/columns ISPLIT(1)+1 through ISPLIT(2), */
/*          etc., and the NSPLIT-th consists of rows/columns */
/*          ISPLIT(NSPLIT-1)+1 through ISPLIT(NSPLIT)=N. */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues (of all L_i D_i L_i^T) */
/*          found. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          The first M elements contain the eigenvalues. The */
/*          eigenvalues of each of the blocks, L_i D_i L_i^T, are */
/*          sorted in ascending order ( DLARRE may use the */
/*          remaining N-M elements as workspace). */

/*  WERR    (output) DOUBLE PRECISION array, dimension (N) */
/*          The error bound on the corresponding eigenvalue in W. */

/*  WGAP    (output) DOUBLE PRECISION array, dimension (N) */
/*          The separation from the right neighbor eigenvalue in W. */
/*          The gap is only with respect to the eigenvalues of the same block */
/*          as each block has its own representation tree. */
/*          Exception: at the right end of a block we store the left gap */

/*  IBLOCK  (output) INTEGER array, dimension (N) */
/*          The indices of the blocks (submatrices) associated with the */
/*          corresponding eigenvalues in W; IBLOCK(i)=1 if eigenvalue */
/*          W(i) belongs to the first block from the top, =2 if W(i) */
/*          belongs to the second block, etc. */

/*  INDEXW  (output) INTEGER array, dimension (N) */
/*          The indices of the eigenvalues within each block (submatrix); */
/*          for example, INDEXW(i)= 10 and IBLOCK(i)=2 imply that the */
/*          i-th eigenvalue W(i) is the 10-th eigenvalue in block 2 */

/*  GERS    (output) DOUBLE PRECISION array, dimension (2*N) */
/*          The N Gerschgorin intervals (the i-th Gerschgorin interval */
/*          is (GERS(2*i-1), GERS(2*i)). */

/*  PIVMIN  (output) DOUBLE PRECISION */
/*          The minimum pivot in the Sturm sequence for T. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (6*N) */
/*          Workspace. */

/*  IWORK   (workspace) INTEGER array, dimension (5*N) */
/*          Workspace. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          > 0:  A problem occured in DLARRE. */
/*          < 0:  One of the called subroutines signaled an internal problem. */
/*                Needs inspection of the corresponding parameter IINFO */
/*                for further information. */

/*          =-1:  Problem in DLARRD. */
/*          = 2:  No base representation could be found in MAXTRY iterations. */
/*                Increasing MAXTRY and recompilation might be a remedy. */
/*          =-3:  Problem in DLARRB when computing the refined root */
/*                representation for DLASQ2. */
/*          =-4:  Problem in DLARRB when preforming bisection on the */
/*                desired part of the spectrum. */
/*          =-5:  Problem in DLASQ2. */
/*          =-6:  Problem in DLASQ2. */

/*  Further Details */
/*  The base representations are required to suffer very little */
/*  element growth and consequently define all their eigenvalues to */
/*  high relative accuracy. */
/*  =============== */

/*  Based on contributions by */
/*     Beresford Parlett, University of California, Berkeley, USA */
/*     Jim Demmel, University of California, Berkeley, USA */
/*     Inderjit Dhillon, University of Texas, Austin, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --iwork;
    --work;
    --gers;
    --indexw;
    --iblock;
    --wgap;
    --werr;
    --w;
    --isplit;
    --e2;
    --e;
    --d__;

    /* Function Body */
    *info = 0;

/*     Decode RANGE */

    if (lsame_(range, "A")) {
	irange = 1;
    } else if (lsame_(range, "V")) {
	irange = 3;
    } else if (lsame_(range, "I")) {
	irange = 2;
    }
    *m = 0;
/*     Get machine constants */
    safmin = dlamch_("S");
    eps = dlamch_("P");
/*     Set parameters */
    rtl = sqrt(eps);
    bsrtol = sqrt(eps);
/*     Treat case of 1x1 matrix for quick return */
    if (*n == 1) {
	if (irange == 1 || irange == 3 && d__[1] > *vl && d__[1] <= *vu ||
		irange == 2 && *il == 1 && *iu == 1) {
	    *m = 1;
	    w[1] = d__[1];
/*           The computation error of the eigenvalue is zero */
	    werr[1] = 0.;
	    wgap[1] = 0.;
	    iblock[1] = 1;
	    indexw[1] = 1;
	    gers[1] = d__[1];
	    gers[2] = d__[1];
	}
/*        store the shift for the initial RRR, which is zero in this case */
	e[1] = 0.;
	return 0;
    }
/*     General case: tridiagonal matrix of order > 1 */

/*     Init WERR, WGAP. Compute Gerschgorin intervals and spectral diameter. */
/*     Compute maximum off-diagonal entry and pivmin. */
    gl = d__[1];
    gu = d__[1];
    eold = 0.;
    emax = 0.;
    e[*n] = 0.;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	werr[i__] = 0.;
	wgap[i__] = 0.;
	eabs = (d__1 = e[i__], abs(d__1));
	if (eabs >= emax) {
	    emax = eabs;
	}
	tmp1 = eabs + eold;
	gers[(i__ << 1) - 1] = d__[i__] - tmp1;
/* Computing MIN */
	d__1 = gl, d__2 = gers[(i__ << 1) - 1];
	gl = std::min(d__1,d__2);
	gers[i__ * 2] = d__[i__] + tmp1;
/* Computing MAX */
	d__1 = gu, d__2 = gers[i__ * 2];
	gu = std::max(d__1,d__2);
	eold = eabs;
/* L5: */
    }
/*     The minimum pivot allowed in the Sturm sequence for T */
/* Computing MAX */
/* Computing 2nd power */
    d__3 = emax;
    d__1 = 1., d__2 = d__3 * d__3;
    *pivmin = safmin * std::max(d__1,d__2);
/*     Compute spectral diameter. The Gerschgorin bounds give an */
/*     estimate that is wrong by at most a factor of SQRT(2) */
    spdiam = gu - gl;
/*     Compute splitting points */
    dlarra_(n, &d__[1], &e[1], &e2[1], spltol, &spdiam, nsplit, &isplit[1], &
	    iinfo);
/*     Can force use of bisection instead of faster DQDS. */
/*     Option left in the code for future multisection work. */
    forceb = false;
    if (irange == 1 && ! forceb) {
/*        Set interval [VL,VU] that contains all eigenvalues */
	*vl = gl;
	*vu = gu;
    } else {
/*        We call DLARRD to find crude approximations to the eigenvalues */
/*        in the desired range. In case IRANGE = INDRNG, we also obtain the */
/*        interval (VL,VU] that contains all the wanted eigenvalues. */
/*        An interval [LEFT,RIGHT] has converged if */
/*        RIGHT-LEFT.LT.RTOL*MAX(ABS(LEFT),ABS(RIGHT)) */
/*        DLARRD needs a WORK of size 4*N, IWORK of size 3*N */
	dlarrd_(range, "B", n, vl, vu, il, iu, &gers[1], &bsrtol, &d__[1], &e[
		1], &e2[1], pivmin, nsplit, &isplit[1], &mm, &w[1], &werr[1],
		vl, vu, &iblock[1], &indexw[1], &work[1], &iwork[1], &iinfo);
	if (iinfo != 0) {
	    *info = -1;
	    return 0;
	}
/*        Make sure that the entries M+1 to N in W, WERR, IBLOCK, INDEXW are 0 */
	i__1 = *n;
	for (i__ = mm + 1; i__ <= i__1; ++i__) {
	    w[i__] = 0.;
	    werr[i__] = 0.;
	    iblock[i__] = 0;
	    indexw[i__] = 0;
/* L14: */
	}
    }
/* ** */
/*     Loop over unreduced blocks */
    ibegin = 1;
    wbegin = 1;
    i__1 = *nsplit;
    for (jblk = 1; jblk <= i__1; ++jblk) {
	iend = isplit[jblk];
	in = iend - ibegin + 1;
/*        1 X 1 block */
	if (in == 1) {
	    if (irange == 1 || irange == 3 && d__[ibegin] > *vl && d__[ibegin]
		     <= *vu || irange == 2 && iblock[wbegin] == jblk) {
		++(*m);
		w[*m] = d__[ibegin];
		werr[*m] = 0.;
/*              The gap for a single block doesn't matter for the later */
/*              algorithm and is assigned an arbitrary large value */
		wgap[*m] = 0.;
		iblock[*m] = jblk;
		indexw[*m] = 1;
		++wbegin;
	    }
/*           E( IEND ) holds the shift for the initial RRR */
	    e[iend] = 0.;
	    ibegin = iend + 1;
	    goto L170;
	}

/*        Blocks of size larger than 1x1 */

/*        E( IEND ) will hold the shift for the initial RRR, for now set it =0 */
	e[iend] = 0.;

/*        Find local outer bounds GL,GU for the block */
	gl = d__[ibegin];
	gu = d__[ibegin];
	i__2 = iend;
	for (i__ = ibegin; i__ <= i__2; ++i__) {
/* Computing MIN */
	    d__1 = gers[(i__ << 1) - 1];
	    gl = std::min(d__1,gl);
/* Computing MAX */
	    d__1 = gers[i__ * 2];
	    gu = std::max(d__1,gu);
/* L15: */
	}
	spdiam = gu - gl;
	if (! (irange == 1 && ! forceb)) {
/*           Count the number of eigenvalues in the current block. */
	    mb = 0;
	    i__2 = mm;
	    for (i__ = wbegin; i__ <= i__2; ++i__) {
		if (iblock[i__] == jblk) {
		    ++mb;
		} else {
		    goto L21;
		}
/* L20: */
	    }
L21:
	    if (mb == 0) {
/*              No eigenvalue in the current block lies in the desired range */
/*              E( IEND ) holds the shift for the initial RRR */
		e[iend] = 0.;
		ibegin = iend + 1;
		goto L170;
	    } else {
/*              Decide whether dqds or bisection is more efficient */
		usedqd = (double) mb > in * .5 && ! forceb;
		wend = wbegin + mb - 1;
/*              Calculate gaps for the current block */
/*              In later stages, when representations for individual */
/*              eigenvalues are different, we use SIGMA = E( IEND ). */
		sigma = 0.;
		i__2 = wend - 1;
		for (i__ = wbegin; i__ <= i__2; ++i__) {
/* Computing MAX */
		    d__1 = 0., d__2 = w[i__ + 1] - werr[i__ + 1] - (w[i__] +
			    werr[i__]);
		    wgap[i__] = std::max(d__1,d__2);
/* L30: */
		}
/* Computing MAX */
		d__1 = 0., d__2 = *vu - sigma - (w[wend] + werr[wend]);
		wgap[wend] = std::max(d__1,d__2);
/*              Find local index of the first and last desired evalue. */
		indl = indexw[wbegin];
		indu = indexw[wend];
	    }
	}
	if (irange == 1 && ! forceb || usedqd) {
/*           Case of DQDS */
/*           Find approximations to the extremal eigenvalues of the block */
	    dlarrk_(&in, &c__1, &gl, &gu, &d__[ibegin], &e2[ibegin], pivmin, &
		    rtl, &tmp, &tmp1, &iinfo);
	    if (iinfo != 0) {
		*info = -1;
		return 0;
	    }
/* Computing MAX */
	    d__2 = gl, d__3 = tmp - tmp1 - eps * 100. * (d__1 = tmp - tmp1,
		    abs(d__1));
	    isleft = std::max(d__2,d__3);
	    dlarrk_(&in, &in, &gl, &gu, &d__[ibegin], &e2[ibegin], pivmin, &
		    rtl, &tmp, &tmp1, &iinfo);
	    if (iinfo != 0) {
		*info = -1;
		return 0;
	    }
/* Computing MIN */
	    d__2 = gu, d__3 = tmp + tmp1 + eps * 100. * (d__1 = tmp + tmp1,
		    abs(d__1));
	    isrght = std::min(d__2,d__3);
/*           Improve the estimate of the spectral diameter */
	    spdiam = isrght - isleft;
	} else {
/*           Case of bisection */
/*           Find approximations to the wanted extremal eigenvalues */
/* Computing MAX */
	    d__2 = gl, d__3 = w[wbegin] - werr[wbegin] - eps * 100. * (d__1 =
		    w[wbegin] - werr[wbegin], abs(d__1));
	    isleft = std::max(d__2,d__3);
/* Computing MIN */
	    d__2 = gu, d__3 = w[wend] + werr[wend] + eps * 100. * (d__1 = w[
		    wend] + werr[wend], abs(d__1));
	    isrght = std::min(d__2,d__3);
	}
/*        Decide whether the base representation for the current block */
/*        L_JBLK D_JBLK L_JBLK^T = T_JBLK - sigma_JBLK I */
/*        should be on the left or the right end of the current block. */
/*        The strategy is to shift to the end which is "more populated" */
/*        Furthermore, decide whether to use DQDS for the computation of */
/*        the eigenvalue approximations at the end of DLARRE or bisection. */
/*        dqds is chosen if all eigenvalues are desired or the number of */
/*        eigenvalues to be computed is large compared to the blocksize. */
	if (irange == 1 && ! forceb) {
/*           If all the eigenvalues have to be computed, we use dqd */
	    usedqd = true;
/*           INDL is the local index of the first eigenvalue to compute */
	    indl = 1;
	    indu = in;
/*           MB =  number of eigenvalues to compute */
	    mb = in;
	    wend = wbegin + mb - 1;
/*           Define 1/4 and 3/4 points of the spectrum */
	    s1 = isleft + spdiam * .25;
	    s2 = isrght - spdiam * .25;
	} else {
/*           DLARRD has computed IBLOCK and INDEXW for each eigenvalue */
/*           approximation. */
/*           choose sigma */
	    if (usedqd) {
		s1 = isleft + spdiam * .25;
		s2 = isrght - spdiam * .25;
	    } else {
		tmp = std::min(isrght,*vu) - std::max(isleft,*vl);
		s1 = std::max(isleft,*vl) + tmp * .25;
		s2 = std::min(isrght,*vu) - tmp * .25;
	    }
	}
/*        Compute the negcount at the 1/4 and 3/4 points */
	if (mb > 1) {
	    dlarrc_("T", &in, &s1, &s2, &d__[ibegin], &e[ibegin], pivmin, &
		    cnt, &cnt1, &cnt2, &iinfo);
	}
	if (mb == 1) {
	    sigma = gl;
	    sgndef = 1.;
	} else if (cnt1 - indl >= indu - cnt2) {
	    if (irange == 1 && ! forceb) {
		sigma = std::max(isleft,gl);
	    } else if (usedqd) {
/*              use Gerschgorin bound as shift to get pos def matrix */
/*              for dqds */
		sigma = isleft;
	    } else {
/*              use approximation of the first desired eigenvalue of the */
/*              block as shift */
		sigma = std::max(isleft,*vl);
	    }
	    sgndef = 1.;
	} else {
	    if (irange == 1 && ! forceb) {
		sigma = std::min(isrght,gu);
	    } else if (usedqd) {
/*              use Gerschgorin bound as shift to get neg def matrix */
/*              for dqds */
		sigma = isrght;
	    } else {
/*              use approximation of the first desired eigenvalue of the */
/*              block as shift */
		sigma = std::min(isrght,*vu);
	    }
	    sgndef = -1.;
	}
/*        An initial SIGMA has been chosen that will be used for computing */
/*        T - SIGMA I = L D L^T */
/*        Define the increment TAU of the shift in case the initial shift */
/*        needs to be refined to obtain a factorization with not too much */
/*        element growth. */
	if (usedqd) {
/*           The initial SIGMA was to the outer end of the spectrum */
/*           the matrix is definite and we need not retreat. */
	    tau = spdiam * eps * *n + *pivmin * 2.;
	} else {
	    if (mb > 1) {
		clwdth = w[wend] + werr[wend] - w[wbegin] - werr[wbegin];
		avgap = (d__1 = clwdth / (double) (wend - wbegin), abs(
			d__1));
		if (sgndef == 1.) {
/* Computing MAX */
		    d__1 = wgap[wbegin];
		    tau = std::max(d__1,avgap) * .5;
/* Computing MAX */
		    d__1 = tau, d__2 = werr[wbegin];
		    tau = std::max(d__1,d__2);
		} else {
/* Computing MAX */
		    d__1 = wgap[wend - 1];
		    tau = std::max(d__1,avgap) * .5;
/* Computing MAX */
		    d__1 = tau, d__2 = werr[wend];
		    tau = std::max(d__1,d__2);
		}
	    } else {
		tau = werr[wbegin];
	    }
	}

	for (idum = 1; idum <= 6; ++idum) {
/*           Compute L D L^T factorization of tridiagonal matrix T - sigma I. */
/*           Store D in WORK(1:IN), L in WORK(IN+1:2*IN), and reciprocals of */
/*           pivots in WORK(2*IN+1:3*IN) */
	    dpivot = d__[ibegin] - sigma;
	    work[1] = dpivot;
	    dmax__ = abs(work[1]);
	    j = ibegin;
	    i__2 = in - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work[(in << 1) + i__] = 1. / work[i__];
		tmp = e[j] * work[(in << 1) + i__];
		work[in + i__] = tmp;
		dpivot = d__[j + 1] - sigma - tmp * e[j];
		work[i__ + 1] = dpivot;
/* Computing MAX */
		d__1 = dmax__, d__2 = abs(dpivot);
		dmax__ = std::max(d__1,d__2);
		++j;
/* L70: */
	    }
/*           check for element growth */
	    if (dmax__ > spdiam * 64.) {
		norep = true;
	    } else {
		norep = false;
	    }
	    if (usedqd && ! norep) {
/*              Ensure the definiteness of the representation */
/*              All entries of D (of L D L^T) must have the same sign */
		i__2 = in;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    tmp = sgndef * work[i__];
		    if (tmp < 0.) {
			norep = true;
		    }
/* L71: */
		}
	    }
	    if (norep) {
/*              Note that in the case of IRANGE=ALLRNG, we use the Gerschgorin */
/*              shift which makes the matrix definite. So we should end up */
/*              here really only in the case of IRANGE = VALRNG or INDRNG. */
		if (idum == 5) {
		    if (sgndef == 1.) {
/*                    The fudged Gerschgorin shift should succeed */
			sigma = gl - spdiam * 2. * eps * *n - *pivmin * 4.;
		    } else {
			sigma = gu + spdiam * 2. * eps * *n + *pivmin * 4.;
		    }
		} else {
		    sigma -= sgndef * tau;
		    tau *= 2.;
		}
	    } else {
/*              an initial RRR is found */
		goto L83;
	    }
/* L80: */
	}
/*        if the program reaches this point, no base representation could be */
/*        found in MAXTRY iterations. */
	*info = 2;
	return 0;
L83:
/*        At this point, we have found an initial base representation */
/*        T - SIGMA I = L D L^T with not too much element growth. */
/*        Store the shift. */
	e[iend] = sigma;
/*        Store D and L. */
	dcopy_(&in, &work[1], &c__1, &d__[ibegin], &c__1);
	i__2 = in - 1;
	dcopy_(&i__2, &work[in + 1], &c__1, &e[ibegin], &c__1);
	if (mb > 1) {

/*           Perturb each entry of the base representation by a small */
/*           (but random) relative amount to overcome difficulties with */
/*           glued matrices. */

	    for (i__ = 1; i__ <= 4; ++i__) {
		iseed[i__ - 1] = 1;
/* L122: */
	    }
	    i__2 = (in << 1) - 1;
	    dlarnv_(&c__2, iseed, &i__2, &work[1]);
	    i__2 = in - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		d__[ibegin + i__ - 1] *= eps * 8. * work[i__] + 1.;
		e[ibegin + i__ - 1] *= eps * 8. * work[in + i__] + 1.;
/* L125: */
	    }
	    d__[iend] *= eps * 4. * work[in] + 1.;

	}

/*        Don't update the Gerschgorin intervals because keeping track */
/*        of the updates would be too much work in DLARRV. */
/*        We update W instead and use it to locate the proper Gerschgorin */
/*        intervals. */
/*        Compute the required eigenvalues of L D L' by bisection or dqds */
	if (! usedqd) {
/*           If DLARRD has been used, shift the eigenvalue approximations */
/*           according to their representation. This is necessary for */
/*           a uniform DLARRV since dqds computes eigenvalues of the */
/*           shifted representation. In DLARRV, W will always hold the */
/*           UNshifted eigenvalue approximation. */
	    i__2 = wend;
	    for (j = wbegin; j <= i__2; ++j) {
		w[j] -= sigma;
		werr[j] += (d__1 = w[j], abs(d__1)) * eps;
/* L134: */
	    }
/*           call DLARRB to reduce eigenvalue error of the approximations */
/*           from DLARRD */
	    i__2 = iend - 1;
	    for (i__ = ibegin; i__ <= i__2; ++i__) {
/* Computing 2nd power */
		d__1 = e[i__];
		work[i__] = d__[i__] * (d__1 * d__1);
/* L135: */
	    }
/*           use bisection to find EV from INDL to INDU */
	    i__2 = indl - 1;
	    dlarrb_(&in, &d__[ibegin], &work[ibegin], &indl, &indu, rtol1,
		    rtol2, &i__2, &w[wbegin], &wgap[wbegin], &werr[wbegin], &
		    work[(*n << 1) + 1], &iwork[1], pivmin, &spdiam, &in, &
		    iinfo);
	    if (iinfo != 0) {
		*info = -4;
		return 0;
	    }
/*           DLARRB computes all gaps correctly except for the last one */
/*           Record distance to VU/GU */
/* Computing MAX */
	    d__1 = 0., d__2 = *vu - sigma - (w[wend] + werr[wend]);
	    wgap[wend] = std::max(d__1,d__2);
	    i__2 = indu;
	    for (i__ = indl; i__ <= i__2; ++i__) {
		++(*m);
		iblock[*m] = jblk;
		indexw[*m] = i__;
/* L138: */
	    }
	} else {
/*           Call dqds to get all eigs (and then possibly delete unwanted */
/*           eigenvalues). */
/*           Note that dqds finds the eigenvalues of the L D L^T representation */
/*           of T to high relative accuracy. High relative accuracy */
/*           might be lost when the shift of the RRR is subtracted to obtain */
/*           the eigenvalues of T. However, T is not guaranteed to define its */
/*           eigenvalues to high relative accuracy anyway. */
/*           Set RTOL to the order of the tolerance used in DLASQ2 */
/*           This is an ESTIMATED error, the worst case bound is 4*N*EPS */
/*           which is usually too large and requires unnecessary work to be */
/*           done by bisection when computing the eigenvectors */
	    rtol = log((double) in) * 4. * eps;
	    j = ibegin;
	    i__2 = in - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work[(i__ << 1) - 1] = (d__1 = d__[j], abs(d__1));
		work[i__ * 2] = e[j] * e[j] * work[(i__ << 1) - 1];
		++j;
/* L140: */
	    }
	    work[(in << 1) - 1] = (d__1 = d__[iend], abs(d__1));
	    work[in * 2] = 0.;
	    dlasq2_(&in, &work[1], &iinfo);
	    if (iinfo != 0) {
/*              If IINFO = -5 then an index is part of a tight cluster */
/*              and should be changed. The index is in IWORK(1) and the */
/*              gap is in WORK(N+1) */
		*info = -5;
		return 0;
	    } else {
/*              Test that all eigenvalues are positive as expected */
		i__2 = in;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    if (work[i__] < 0.) {
			*info = -6;
			return 0;
		    }
/* L149: */
		}
	    }
	    if (sgndef > 0.) {
		i__2 = indu;
		for (i__ = indl; i__ <= i__2; ++i__) {
		    ++(*m);
		    w[*m] = work[in - i__ + 1];
		    iblock[*m] = jblk;
		    indexw[*m] = i__;
/* L150: */
		}
	    } else {
		i__2 = indu;
		for (i__ = indl; i__ <= i__2; ++i__) {
		    ++(*m);
		    w[*m] = -work[i__];
		    iblock[*m] = jblk;
		    indexw[*m] = i__;
/* L160: */
		}
	    }
	    i__2 = *m;
	    for (i__ = *m - mb + 1; i__ <= i__2; ++i__) {
/*              the value of RTOL below should be the tolerance in DLASQ2 */
		werr[i__] = rtol * (d__1 = w[i__], abs(d__1));
/* L165: */
	    }
	    i__2 = *m - 1;
	    for (i__ = *m - mb + 1; i__ <= i__2; ++i__) {
/*              compute the right gap between the intervals */
/* Computing MAX */
		d__1 = 0., d__2 = w[i__ + 1] - werr[i__ + 1] - (w[i__] + werr[
			i__]);
		wgap[i__] = std::max(d__1,d__2);
/* L166: */
	    }
/* Computing MAX */
	    d__1 = 0., d__2 = *vu - sigma - (w[*m] + werr[*m]);
	    wgap[*m] = std::max(d__1,d__2);
	}
/*        proceed with next block */
	ibegin = iend + 1;
	wbegin = wend + 1;
L170:
	;
    }

    return 0;

/*     end of DLARRE */

} /* dlarre_ */

/* Subroutine */ int dlarrf_(integer *n, double *d__, double *l,
	double *ld, integer *clstrt, integer *clend, double *w,
	double *wgap, double *werr, double *spdiam, double *
	clgapl, double *clgapr, double *pivmin, double *sigma,
	double *dplus, double *lplus, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__;
    double s, bestshift, smlgrowth, eps, tmp, max1, max2, rrr1, rrr2,
	    znm2, growthbound, fail, fact, oldp;
    integer indx;
    double prod;
    integer ktry;
    double fail2, avgap, ldmax, rdmax;
    integer shift;
    bool dorrr1;
    double ldelta;
    bool nofail;
    double mingap, lsigma, rdelta;
    bool forcer;
    double rsigma, clwdth;
    bool sawnan1, sawnan2, tryrrr1;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */
/* * */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Given the initial representation L D L^T and its cluster of close */
/*  eigenvalues (in a relative measure), W( CLSTRT ), W( CLSTRT+1 ), ... */
/*  W( CLEND ), DLARRF finds a new relatively robust representation */
/*  L D L^T - SIGMA I = L(+) D(+) L(+)^T such that at least one of the */
/*  eigenvalues of L(+) D(+) L(+)^T is relatively isolated. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix (subblock, if the matrix splitted). */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The N diagonal elements of the diagonal matrix D. */

/*  L       (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (N-1) subdiagonal elements of the unit bidiagonal */
/*          matrix L. */

/*  LD      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (N-1) elements L(i)*D(i). */

/*  CLSTRT  (input) INTEGER */
/*          The index of the first eigenvalue in the cluster. */

/*  CLEND   (input) INTEGER */
/*          The index of the last eigenvalue in the cluster. */

/*  W       (input) DOUBLE PRECISION array, dimension >=  (CLEND-CLSTRT+1) */
/*          The eigenvalue APPROXIMATIONS of L D L^T in ascending order. */
/*          W( CLSTRT ) through W( CLEND ) form the cluster of relatively */
/*          close eigenalues. */

/*  WGAP    (input/output) DOUBLE PRECISION array, dimension >=  (CLEND-CLSTRT+1) */
/*          The separation from the right neighbor eigenvalue in W. */

/*  WERR    (input) DOUBLE PRECISION array, dimension >=  (CLEND-CLSTRT+1) */
/*          WERR contain the semiwidth of the uncertainty */
/*          interval of the corresponding eigenvalue APPROXIMATION in W */

/*  SPDIAM (input) estimate of the spectral diameter obtained from the */
/*          Gerschgorin intervals */

/*  CLGAPL, CLGAPR (input) absolute gap on each end of the cluster. */
/*          Set by the calling routine to protect against shifts too close */
/*          to eigenvalues outside the cluster. */

/*  PIVMIN  (input) DOUBLE PRECISION */
/*          The minimum pivot allowed in the Sturm sequence. */

/*  SIGMA   (output) DOUBLE PRECISION */
/*          The shift used to form L(+) D(+) L(+)^T. */

/*  DPLUS   (output) DOUBLE PRECISION array, dimension (N) */
/*          The N diagonal elements of the diagonal matrix D(+). */

/*  LPLUS   (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The first (N-1) elements of LPLUS contain the subdiagonal */
/*          elements of the unit bidiagonal matrix L(+). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*N) */
/*          Workspace. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Beresford Parlett, University of California, Berkeley, USA */
/*     Jim Demmel, University of California, Berkeley, USA */
/*     Inderjit Dhillon, University of Texas, Austin, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --work;
    --lplus;
    --dplus;
    --werr;
    --wgap;
    --w;
    --ld;
    --l;
    --d__;

    /* Function Body */
    *info = 0;
    fact = 2.;
    eps = dlamch_("Precision");
    shift = 0;
    forcer = false;
/*     Note that we cannot guarantee that for any of the shifts tried, */
/*     the factorization has a small or even moderate element growth. */
/*     There could be Ritz values at both ends of the cluster and despite */
/*     backing off, there are examples where all factorizations tried */
/*     (in IEEE mode, allowing zero pivots & infinities) have INFINITE */
/*     element growth. */
/*     For this reason, we should use PIVMIN in this subroutine so that at */
/*     least the L D L^T factorization exists. It can be checked afterwards */
/*     whether the element growth caused bad residuals/orthogonality. */
/*     Decide whether the code should accept the best among all */
/*     representations despite large element growth or signal INFO=1 */
    nofail = true;

/*     Compute the average gap length of the cluster */
    clwdth = (d__1 = w[*clend] - w[*clstrt], abs(d__1)) + werr[*clend] + werr[
	    *clstrt];
    avgap = clwdth / (double) (*clend - *clstrt);
    mingap = std::min(*clgapl,*clgapr);
/*     Initial values for shifts to both ends of cluster */
/* Computing MIN */
    d__1 = w[*clstrt], d__2 = w[*clend];
    lsigma = std::min(d__1,d__2) - werr[*clstrt];
/* Computing MAX */
    d__1 = w[*clstrt], d__2 = w[*clend];
    rsigma = std::max(d__1,d__2) + werr[*clend];
/*     Use a small fudge to make sure that we really shift to the outside */
    lsigma -= abs(lsigma) * 4. * eps;
    rsigma += abs(rsigma) * 4. * eps;
/*     Compute upper bounds for how much to back off the initial shifts */
    ldmax = mingap * .25 + *pivmin * 2.;
    rdmax = mingap * .25 + *pivmin * 2.;
/* Computing MAX */
    d__1 = avgap, d__2 = wgap[*clstrt];
    ldelta = std::max(d__1,d__2) / fact;
/* Computing MAX */
    d__1 = avgap, d__2 = wgap[*clend - 1];
    rdelta = std::max(d__1,d__2) / fact;

/*     Initialize the record of the best representation found */

    s = dlamch_("S");
    smlgrowth = 1. / s;
    fail = (double) (*n - 1) * mingap / (*spdiam * eps);
    fail2 = (double) (*n - 1) * mingap / (*spdiam * sqrt(eps));
    bestshift = lsigma;

/*     while (KTRY <= KTRYMAX) */
    ktry = 0;
    growthbound = *spdiam * 8.;
L5:
    sawnan1 = false;
    sawnan2 = false;
/*     Ensure that we do not back off too much of the initial shifts */
    ldelta = std::min(ldmax,ldelta);
    rdelta = std::min(rdmax,rdelta);
/*     Compute the element growth when shifting to both ends of the cluster */
/*     accept the shift if there is no element growth at one of the two ends */
/*     Left end */
    s = -lsigma;
    dplus[1] = d__[1] + s;
    if (abs(dplus[1]) < *pivmin) {
	dplus[1] = -(*pivmin);
/*        Need to set SAWNAN1 because refined RRR test should not be used */
/*        in this case */
	sawnan1 = true;
    }
    max1 = abs(dplus[1]);
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	lplus[i__] = ld[i__] / dplus[i__];
	s = s * lplus[i__] * l[i__] - lsigma;
	dplus[i__ + 1] = d__[i__ + 1] + s;
	if ((d__1 = dplus[i__ + 1], abs(d__1)) < *pivmin) {
	    dplus[i__ + 1] = -(*pivmin);
/*           Need to set SAWNAN1 because refined RRR test should not be used */
/*           in this case */
	    sawnan1 = true;
	}
/* Computing MAX */
	d__2 = max1, d__3 = (d__1 = dplus[i__ + 1], abs(d__1));
	max1 = std::max(d__2,d__3);
/* L6: */
    }
    sawnan1 = sawnan1 || disnan_(&max1);
    if (forcer || max1 <= growthbound && ! sawnan1) {
	*sigma = lsigma;
	shift = 1;
	goto L100;
    }
/*     Right end */
    s = -rsigma;
    work[1] = d__[1] + s;
    if (abs(work[1]) < *pivmin) {
	work[1] = -(*pivmin);
/*        Need to set SAWNAN2 because refined RRR test should not be used */
/*        in this case */
	sawnan2 = true;
    }
    max2 = abs(work[1]);
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	work[*n + i__] = ld[i__] / work[i__];
	s = s * work[*n + i__] * l[i__] - rsigma;
	work[i__ + 1] = d__[i__ + 1] + s;
	if ((d__1 = work[i__ + 1], abs(d__1)) < *pivmin) {
	    work[i__ + 1] = -(*pivmin);
/*           Need to set SAWNAN2 because refined RRR test should not be used */
/*           in this case */
	    sawnan2 = true;
	}
/* Computing MAX */
	d__2 = max2, d__3 = (d__1 = work[i__ + 1], abs(d__1));
	max2 = std::max(d__2,d__3);
/* L7: */
    }
    sawnan2 = sawnan2 || disnan_(&max2);
    if (forcer || max2 <= growthbound && ! sawnan2) {
	*sigma = rsigma;
	shift = 2;
	goto L100;
    }
/*     If we are at this point, both shifts led to too much element growth */
/*     Record the better of the two shifts (provided it didn't lead to NaN) */
    if (sawnan1 && sawnan2) {
/*        both MAX1 and MAX2 are NaN */
	goto L50;
    } else {
	if (! sawnan1) {
	    indx = 1;
	    if (max1 <= smlgrowth) {
		smlgrowth = max1;
		bestshift = lsigma;
	    }
	}
	if (! sawnan2) {
	    if (sawnan1 || max2 <= max1) {
		indx = 2;
	    }
	    if (max2 <= smlgrowth) {
		smlgrowth = max2;
		bestshift = rsigma;
	    }
	}
    }
/*     If we are here, both the left and the right shift led to */
/*     element growth. If the element growth is moderate, then */
/*     we may still accept the representation, if it passes a */
/*     refined test for RRR. This test supposes that no NaN occurred. */
/*     Moreover, we use the refined RRR test only for isolated clusters. */
    if (clwdth < mingap / 128. && std::min(max1,max2) < fail2 && ! sawnan1 && !
	    sawnan2) {
	dorrr1 = true;
    } else {
	dorrr1 = false;
    }
    tryrrr1 = true;
    if (tryrrr1 && dorrr1) {
	if (indx == 1) {
	    tmp = (d__1 = dplus[*n], abs(d__1));
	    znm2 = 1.;
	    prod = 1.;
	    oldp = 1.;
	    for (i__ = *n - 1; i__ >= 1; --i__) {
		if (prod <= eps) {
		    prod = dplus[i__ + 1] * work[*n + i__ + 1] / (dplus[i__] *
			     work[*n + i__]) * oldp;
		} else {
		    prod *= (d__1 = work[*n + i__], abs(d__1));
		}
		oldp = prod;
/* Computing 2nd power */
		d__1 = prod;
		znm2 += d__1 * d__1;
/* Computing MAX */
		d__2 = tmp, d__3 = (d__1 = dplus[i__] * prod, abs(d__1));
		tmp = std::max(d__2,d__3);
/* L15: */
	    }
	    rrr1 = tmp / (*spdiam * sqrt(znm2));
	    if (rrr1 <= 8.) {
		*sigma = lsigma;
		shift = 1;
		goto L100;
	    }
	} else if (indx == 2) {
	    tmp = (d__1 = work[*n], abs(d__1));
	    znm2 = 1.;
	    prod = 1.;
	    oldp = 1.;
	    for (i__ = *n - 1; i__ >= 1; --i__) {
		if (prod <= eps) {
		    prod = work[i__ + 1] * lplus[i__ + 1] / (work[i__] *
			    lplus[i__]) * oldp;
		} else {
		    prod *= (d__1 = lplus[i__], abs(d__1));
		}
		oldp = prod;
/* Computing 2nd power */
		d__1 = prod;
		znm2 += d__1 * d__1;
/* Computing MAX */
		d__2 = tmp, d__3 = (d__1 = work[i__] * prod, abs(d__1));
		tmp = std::max(d__2,d__3);
/* L16: */
	    }
	    rrr2 = tmp / (*spdiam * sqrt(znm2));
	    if (rrr2 <= 8.) {
		*sigma = rsigma;
		shift = 2;
		goto L100;
	    }
	}
    }
L50:
    if (ktry < 1) {
/*        If we are here, both shifts failed also the RRR test. */
/*        Back off to the outside */
/* Computing MAX */
	d__1 = lsigma - ldelta, d__2 = lsigma - ldmax;
	lsigma = std::max(d__1,d__2);
/* Computing MIN */
	d__1 = rsigma + rdelta, d__2 = rsigma + rdmax;
	rsigma = std::min(d__1,d__2);
	ldelta *= 2.;
	rdelta *= 2.;
	++ktry;
	goto L5;
    } else {
/*        None of the representations investigated satisfied our */
/*        criteria. Take the best one we found. */
	if (smlgrowth < fail || nofail) {
	    lsigma = bestshift;
	    rsigma = bestshift;
	    forcer = true;
	    goto L5;
	} else {
	    *info = 1;
	    return 0;
	}
    }
L100:
    if (shift == 1) {
    } else if (shift == 2) {
/*        store new L and D back into DPLUS, LPLUS */
	dcopy_(n, &work[1], &c__1, &dplus[1], &c__1);
	i__1 = *n - 1;
	dcopy_(&i__1, &work[*n + 1], &c__1, &lplus[1], &c__1);
    }
    return 0;

/*     End of DLARRF */

} /* dlarrf_ */

/* Subroutine */ int dlarrj_(integer *n, double *d__, double *e2,
	integer *ifirst, integer *ilast, double *rtol, integer *offset,
	double *w, double *werr, double *work, integer *iwork,
	double *pivmin, double *spdiam, integer *info)
{
    /* System generated locals */
    integer i__1, i__2;
    double d__1, d__2;

    /* Builtin functions
    double log(double); */

    /* Local variables */
    integer i__, j, k, p;
    double s;
    integer i1, i2, ii;
    double fac, mid;
    integer cnt;
    double tmp, left;
    integer iter, nint, prev, next, savi1;
    double right, width, dplus;
    integer olnint, maxitr;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Given the initial eigenvalue approximations of T, DLARRJ */
/*  does  bisection to refine the eigenvalues of T, */
/*  W( IFIRST-OFFSET ) through W( ILAST-OFFSET ), to more accuracy. Initial */
/*  guesses for these eigenvalues are input in W, the corresponding estimate */
/*  of the error in these guesses in WERR. During bisection, intervals */
/*  [left, right] are maintained by storing their mid-points and */
/*  semi-widths in the arrays W and WERR respectively. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The N diagonal elements of T. */

/*  E2      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The Squares of the (N-1) subdiagonal elements of T. */

/*  IFIRST  (input) INTEGER */
/*          The index of the first eigenvalue to be computed. */

/*  ILAST   (input) INTEGER */
/*          The index of the last eigenvalue to be computed. */

/*  RTOL   (input) DOUBLE PRECISION */
/*          Tolerance for the convergence of the bisection intervals. */
/*          An interval [LEFT,RIGHT] has converged if */
/*          RIGHT-LEFT.LT.RTOL*MAX(|LEFT|,|RIGHT|). */

/*  OFFSET  (input) INTEGER */
/*          Offset for the arrays W and WERR, i.e., the IFIRST-OFFSET */
/*          through ILAST-OFFSET elements of these arrays are to be used. */

/*  W       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On input, W( IFIRST-OFFSET ) through W( ILAST-OFFSET ) are */
/*          estimates of the eigenvalues of L D L^T indexed IFIRST through */
/*          ILAST. */
/*          On output, these estimates are refined. */

/*  WERR    (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On input, WERR( IFIRST-OFFSET ) through WERR( ILAST-OFFSET ) are */
/*          the errors in the estimates of the corresponding elements in W. */
/*          On output, these errors are refined. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*N) */
/*          Workspace. */

/*  IWORK   (workspace) INTEGER array, dimension (2*N) */
/*          Workspace. */

/*  PIVMIN  (input) DOUBLE PRECISION */
/*          The minimum pivot in the Sturm sequence for T. */

/*  SPDIAM  (input) DOUBLE PRECISION */
/*          The spectral diameter of T. */

/*  INFO    (output) INTEGER */
/*          Error flag. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Beresford Parlett, University of California, Berkeley, USA */
/*     Jim Demmel, University of California, Berkeley, USA */
/*     Inderjit Dhillon, University of Texas, Austin, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */

/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --iwork;
    --work;
    --werr;
    --w;
    --e2;
    --d__;

    /* Function Body */
    *info = 0;

    maxitr = (integer) ((log(*spdiam + *pivmin) - log(*pivmin)) / log(2.)) +
	    2;

/*     Initialize unconverged intervals in [ WORK(2*I-1), WORK(2*I) ]. */
/*     The Sturm Count, Count( WORK(2*I-1) ) is arranged to be I-1, while */
/*     Count( WORK(2*I) ) is stored in IWORK( 2*I ). The integer IWORK( 2*I-1 ) */
/*     for an unconverged interval is set to the index of the next unconverged */
/*     interval, and is -1 or 0 for a converged interval. Thus a linked */
/*     list of unconverged intervals is set up. */

    i1 = *ifirst;
    i2 = *ilast;
/*     The number of unconverged intervals */
    nint = 0;
/*     The last unconverged interval found */
    prev = 0;
    i__1 = i2;
    for (i__ = i1; i__ <= i__1; ++i__) {
	k = i__ << 1;
	ii = i__ - *offset;
	left = w[ii] - werr[ii];
	mid = w[ii];
	right = w[ii] + werr[ii];
	width = right - mid;
/* Computing MAX */
	d__1 = abs(left), d__2 = abs(right);
	tmp = std::max(d__1,d__2);
/*        The following test prevents the test of converged intervals */
	if (width < *rtol * tmp) {
/*           This interval has already converged and does not need refinement. */
/*           (Note that the gaps might change through refining the */
/*            eigenvalues, however, they can only get bigger.) */
/*           Remove it from the list. */
	    iwork[k - 1] = -1;
/*           Make sure that I1 always points to the first unconverged interval */
	    if (i__ == i1 && i__ < i2) {
		i1 = i__ + 1;
	    }
	    if (prev >= i1 && i__ <= i2) {
		iwork[(prev << 1) - 1] = i__ + 1;
	    }
	} else {
/*           unconverged interval found */
	    prev = i__;
/*           Make sure that [LEFT,RIGHT] contains the desired eigenvalue */

/*           Do while( CNT(LEFT).GT.I-1 ) */

	    fac = 1.;
L20:
	    cnt = 0;
	    s = left;
	    dplus = d__[1] - s;
	    if (dplus < 0.) {
		++cnt;
	    }
	    i__2 = *n;
	    for (j = 2; j <= i__2; ++j) {
		dplus = d__[j] - s - e2[j - 1] / dplus;
		if (dplus < 0.) {
		    ++cnt;
		}
/* L30: */
	    }
	    if (cnt > i__ - 1) {
		left -= werr[ii] * fac;
		fac *= 2.;
		goto L20;
	    }

/*           Do while( CNT(RIGHT).LT.I ) */

	    fac = 1.;
L50:
	    cnt = 0;
	    s = right;
	    dplus = d__[1] - s;
	    if (dplus < 0.) {
		++cnt;
	    }
	    i__2 = *n;
	    for (j = 2; j <= i__2; ++j) {
		dplus = d__[j] - s - e2[j - 1] / dplus;
		if (dplus < 0.) {
		    ++cnt;
		}
/* L60: */
	    }
	    if (cnt < i__) {
		right += werr[ii] * fac;
		fac *= 2.;
		goto L50;
	    }
	    ++nint;
	    iwork[k - 1] = i__ + 1;
	    iwork[k] = cnt;
	}
	work[k - 1] = left;
	work[k] = right;
/* L75: */
    }
    savi1 = i1;

/*     Do while( NINT.GT.0 ), i.e. there are still unconverged intervals */
/*     and while (ITER.LT.MAXITR) */

    iter = 0;
L80:
    prev = i1 - 1;
    i__ = i1;
    olnint = nint;
    i__1 = olnint;
    for (p = 1; p <= i__1; ++p) {
	k = i__ << 1;
	ii = i__ - *offset;
	next = iwork[k - 1];
	left = work[k - 1];
	right = work[k];
	mid = (left + right) * .5;
/*        semiwidth of interval */
	width = right - mid;
/* Computing MAX */
	d__1 = abs(left), d__2 = abs(right);
	tmp = std::max(d__1,d__2);
	if (width < *rtol * tmp || iter == maxitr) {
/*           reduce number of unconverged intervals */
	    --nint;
/*           Mark interval as converged. */
	    iwork[k - 1] = 0;
	    if (i1 == i__) {
		i1 = next;
	    } else {
/*              Prev holds the last unconverged interval previously examined */
		if (prev >= i1) {
		    iwork[(prev << 1) - 1] = next;
		}
	    }
	    i__ = next;
	    goto L100;
	}
	prev = i__;

/*        Perform one bisection step */

	cnt = 0;
	s = mid;
	dplus = d__[1] - s;
	if (dplus < 0.) {
	    ++cnt;
	}
	i__2 = *n;
	for (j = 2; j <= i__2; ++j) {
	    dplus = d__[j] - s - e2[j - 1] / dplus;
	    if (dplus < 0.) {
		++cnt;
	    }
/* L90: */
	}
	if (cnt <= i__ - 1) {
	    work[k - 1] = mid;
	} else {
	    work[k] = mid;
	}
	i__ = next;
L100:
	;
    }
    ++iter;
/*     do another loop if there are still unconverged intervals */
/*     However, in the last iteration, all intervals are accepted */
/*     since this is the best we can do. */
    if (nint > 0 && iter <= maxitr) {
	goto L80;
    }


/*     At this point, all the intervals have converged */
    i__1 = *ilast;
    for (i__ = savi1; i__ <= i__1; ++i__) {
	k = i__ << 1;
	ii = i__ - *offset;
/*        All intervals marked by '0' have been refined. */
	if (iwork[k - 1] == 0) {
	    w[ii] = (work[k - 1] + work[k]) * .5;
	    werr[ii] = work[k] - w[ii];
	}
/* L110: */
    }

    return 0;

/*     End of DLARRJ */

} /* dlarrj_ */

/* Subroutine */ int dlarrk_(integer *n, integer *iw, double *gl,
	double *gu, double *d__, double *e2, double *pivmin,
	double *reltol, double *w, double *werr, integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Builtin functions
    double log(double); */

    /* Local variables */
    integer i__, it;
    double mid, eps, tmp1, tmp2, left, atoli, right;
    integer itmax;
    double rtoli, tnorm;

    integer negcnt;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARRK computes one eigenvalue of a symmetric tridiagonal */
/*  matrix T to suitable accuracy. This is an auxiliary code to be */
/*  called from DSTEMR. */

/*  To avoid overflow, the matrix must be scaled so that its */
/*  largest element is no greater than overflow**(1/2) * */
/*  underflow**(1/4) in absolute value, and for greatest */
/*  accuracy, it should not be much smaller than that. */

/*  See W. Kahan "Accurate Eigenvalues of a Symmetric Tridiagonal */
/*  Matrix", Report CS41, Computer Science Dept., Stanford */
/*  University, July 21, 1966. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the tridiagonal matrix T.  N >= 0. */

/*  IW      (input) INTEGER */
/*          The index of the eigenvalues to be returned. */

/*  GL      (input) DOUBLE PRECISION */
/*  GU      (input) DOUBLE PRECISION */
/*          An upper and a lower bound on the eigenvalue. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the tridiagonal matrix T. */

/*  E2      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) squared off-diagonal elements of the tridiagonal matrix T. */

/*  PIVMIN  (input) DOUBLE PRECISION */
/*          The minimum pivot allowed in the Sturm sequence for T. */

/*  RELTOL  (input) DOUBLE PRECISION */
/*          The minimum relative width of an interval.  When an interval */
/*          is narrower than RELTOL times the larger (in */
/*          magnitude) endpoint, then it is considered to be */
/*          sufficiently small, i.e., converged.  Note: this should */
/*          always be at least radix*machine epsilon. */

/*  W       (output) DOUBLE PRECISION */

/*  WERR    (output) DOUBLE PRECISION */
/*          The error bound on the corresponding eigenvalue approximation */
/*          in W. */

/*  INFO    (output) INTEGER */
/*          = 0:       Eigenvalue converged */
/*          = -1:      Eigenvalue did NOT converge */

/*  Internal Parameters */
/*  =================== */

/*  FUDGE   DOUBLE PRECISION, default = 2 */
/*          A "fudge factor" to widen the Gershgorin intervals. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Get machine constants */
    /* Parameter adjustments */
    --e2;
    --d__;

    /* Function Body */
    eps = dlamch_("P");
/* Computing MAX */
    d__1 = abs(*gl), d__2 = abs(*gu);
    tnorm = std::max(d__1,d__2);
    rtoli = *reltol;
    atoli = *pivmin * 4.;
    itmax = (integer) ((log(tnorm + *pivmin) - log(*pivmin)) / log(2.)) + 2;
    *info = -1;
    left = *gl - tnorm * 2. * eps * *n - *pivmin * 4.;
    right = *gu + tnorm * 2. * eps * *n + *pivmin * 4.;
    it = 0;
L10:

/*     Check if interval converged or maximum number of iterations reached */

    tmp1 = (d__1 = right - left, abs(d__1));
/* Computing MAX */
    d__1 = abs(right), d__2 = abs(left);
    tmp2 = std::max(d__1,d__2);
/* Computing MAX */
    d__1 = std::max(atoli,*pivmin), d__2 = rtoli * tmp2;
    if (tmp1 < std::max(d__1,d__2)) {
	*info = 0;
	goto L30;
    }
    if (it > itmax) {
	goto L30;
    }

/*     Count number of negative pivots for mid-point */

    ++it;
    mid = (left + right) * .5;
    negcnt = 0;
    tmp1 = d__[1] - mid;
    if (abs(tmp1) < *pivmin) {
	tmp1 = -(*pivmin);
    }
    if (tmp1 <= 0.) {
	++negcnt;
    }

    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	tmp1 = d__[i__] - e2[i__ - 1] / tmp1 - mid;
	if (abs(tmp1) < *pivmin) {
	    tmp1 = -(*pivmin);
	}
	if (tmp1 <= 0.) {
	    ++negcnt;
	}
/* L20: */
    }
    if (negcnt >= *iw) {
	right = mid;
    } else {
	left = mid;
    }
    goto L10;
L30:

/*     Converged or maximum number of iterations reached */

    *w = (left + right) * .5;
    *werr = (d__1 = right - left, abs(d__1)) * .5;
    return 0;

/*     End of DLARRK */

} /* dlarrk_ */

/* Subroutine */ int dlarrr_(integer *n, double *d__, double *e,
	integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1;

    /* Local variables */
    integer i__;
    double eps, tmp, tmp2, rmin;

    double offdig, safmin;
    bool yesrel;
    double smlnum, offdig2;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */


/*  Purpose */
/*  ======= */

/*  Perform tests to decide whether the symmetric tridiagonal matrix T */
/*  warrants expensive computations which guarantee high relative accuracy */
/*  in the eigenvalues. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix. N > 0. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The N diagonal elements of the tridiagonal matrix T. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the first (N-1) entries contain the subdiagonal */
/*          elements of the tridiagonal matrix T; E(N) is set to ZERO. */

/*  INFO    (output) INTEGER */
/*          INFO = 0(default) : the matrix warrants computations preserving */
/*                              relative accuracy. */
/*          INFO = 1          : the matrix warrants computations guaranteeing */
/*                              only absolute accuracy. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Beresford Parlett, University of California, Berkeley, USA */
/*     Jim Demmel, University of California, Berkeley, USA */
/*     Inderjit Dhillon, University of Texas, Austin, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     As a default, do NOT go for relative-accuracy preserving computations. */
    /* Parameter adjustments */
    --e;
    --d__;

    /* Function Body */
    *info = 1;
    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    rmin = sqrt(smlnum);
/*     Tests for relative accuracy */

/*     Test for scaled diagonal dominance */
/*     Scale the diagonal entries to one and check whether the sum of the */
/*     off-diagonals is less than one */

/*     The sdd relative error bounds have a 1/(1- 2*x) factor in them, */
/*     x = max(OFFDIG + OFFDIG2), so when x is close to 1/2, no relative */
/*     accuracy is promised.  In the notation of the code fragment below, */
/*     1/(1 - (OFFDIG + OFFDIG2)) is the condition number. */
/*     We don't think it is worth going into "sdd mode" unless the relative */
/*     condition number is reasonable, not 1/macheps. */
/*     The threshold should be compatible with other thresholds used in the */
/*     code. We set  OFFDIG + OFFDIG2 <= .999 =: RELCOND, it corresponds */
/*     to losing at most 3 decimal digits: 1 / (1 - (OFFDIG + OFFDIG2)) <= 1000 */
/*     instead of the current OFFDIG + OFFDIG2 < 1 */

    yesrel = true;
    offdig = 0.;
    tmp = sqrt((abs(d__[1])));
    if (tmp < rmin) {
	yesrel = false;
    }
    if (! yesrel) {
	goto L11;
    }
    i__1 = *n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	tmp2 = sqrt((d__1 = d__[i__], abs(d__1)));
	if (tmp2 < rmin) {
	    yesrel = false;
	}
	if (! yesrel) {
	    goto L11;
	}
	offdig2 = (d__1 = e[i__ - 1], abs(d__1)) / (tmp * tmp2);
	if (offdig + offdig2 >= .999) {
	    yesrel = false;
	}
	if (! yesrel) {
	    goto L11;
	}
	tmp = tmp2;
	offdig = offdig2;
/* L10: */
    }
L11:
    if (yesrel) {
	*info = 0;
	return 0;
    } else {
    }


/*     *** MORE TO BE IMPLEMENTED *** */


/*     Test if the lower bidiagonal matrix L from T = L D L^T */
/*     (zero shift facto) is well conditioned */


/*     Test if the upper bidiagonal matrix U from T = U D U^T */
/*     (zero shift facto) is well conditioned. */
/*     In this case, the matrix needs to be flipped and, at the end */
/*     of the eigenvector computation, the flip needs to be applied */
/*     to the computed eigenvectors (and the support) */


    return 0;

/*     END OF DLARRR */

} /* dlarrr_ */

/* Subroutine */ int dlarrv_(integer *n, double *vl, double *vu,
	double *d__, double *l, double *pivmin, integer *isplit,
	integer *m, integer *dol, integer *dou, double *minrgp,
	double *rtol1, double *rtol2, double *w, double *werr,
	double *wgap, integer *iblock, integer *indexw, double *gers,
	double *z__, integer *ldz, integer *isuppz, double *work,
	integer *iwork, integer *info)
{
	/* Table of constant values */
	static double c_b5 = 0.;
	static integer c__1 = 1;
	static integer c__2 = 2;

    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2, i__3, i__4, i__5;
    double d__1, d__2;
    bool L__1;

    /* Builtin functions
    double log(double); */

    /* Local variables */
    integer minwsize, i__, j, k, p, q, miniwsize, ii;
    double gl;
    integer im, in;
    double gu, gap, eps, tau, tol, tmp;
    integer zto;
    double ztz;
    integer iend, jblk;
    double lgap;
    integer done;
    double rgap, left;
    integer wend, iter;
    double bstw;
    integer itmp1;
    integer indld;
    double fudge;
    integer idone;
    double sigma;
    integer iinfo, iindr;
    double resid;
    bool eskip;
    double right;
    integer nclus, zfrom;
    double rqtol;
    integer iindc1, iindc2;
    bool stp2ii;
    double lambda;
    integer ibegin, indeig;
    bool needbs;
    integer indlld;
    double sgndef, mingma;
    integer oldien, oldncl, wbegin;
    double spdiam;
    integer negcnt;
    integer oldcls;
    double savgap;
    integer ndepth;
    double ssigma;
    bool usedbs;
    integer iindwk, offset;
    double gaptol;
    integer newcls, oldfst, indwrk, windex, oldlst;
    bool usedrq;
    integer newfst, newftt, parity, windmn, windpl, isupmn, newlst, zusedl;
    double bstres;
    integer newsiz, zusedu, zusedw;
    double nrminv, rqcorr;
    bool tryrqc;
    integer isupmx;


/*  -- LAPACK auxiliary routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARRV computes the eigenvectors of the tridiagonal matrix */
/*  T = L D L^T given L, D and APPROXIMATIONS to the eigenvalues of L D L^T. */
/*  The input eigenvalues should have been computed by DLARRE. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix.  N >= 0. */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          Lower and upper bounds of the interval that contains the desired */
/*          eigenvalues. VL < VU. Needed to compute gaps on the left or right */
/*          end of the extremal eigenvalues in the desired RANGE. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the N diagonal elements of the diagonal matrix D. */
/*          On exit, D may be overwritten. */

/*  L       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the (N-1) subdiagonal elements of the unit */
/*          bidiagonal matrix L are in elements 1 to N-1 of L */
/*          (if the matrix is not splitted.) At the end of each block */
/*          is stored the corresponding shift as given by DLARRE. */
/*          On exit, L is overwritten. */

/*  PIVMIN  (in) DOUBLE PRECISION */
/*          The minimum pivot allowed in the Sturm sequence. */

/*  ISPLIT  (input) INTEGER array, dimension (N) */
/*          The splitting points, at which T breaks up into blocks. */
/*          The first block consists of rows/columns 1 to */
/*          ISPLIT( 1 ), the second of rows/columns ISPLIT( 1 )+1 */
/*          through ISPLIT( 2 ), etc. */

/*  M       (input) INTEGER */
/*          The total number of input eigenvalues.  0 <= M <= N. */

/*  DOL     (input) INTEGER */
/*  DOU     (input) INTEGER */
/*          If the user wants to compute only selected eigenvectors from all */
/*          the eigenvalues supplied, he can specify an index range DOL:DOU. */
/*          Or else the setting DOL=1, DOU=M should be applied. */
/*          Note that DOL and DOU refer to the order in which the eigenvalues */
/*          are stored in W. */
/*          If the user wants to compute only selected eigenpairs, then */
/*          the columns DOL-1 to DOU+1 of the eigenvector space Z contain the */
/*          computed eigenvectors. All other columns of Z are set to zero. */

/*  MINRGP  (input) DOUBLE PRECISION */

/*  RTOL1   (input) DOUBLE PRECISION */
/*  RTOL2   (input) DOUBLE PRECISION */
/*           Parameters for bisection. */
/*           An interval [LEFT,RIGHT] has converged if */
/*           RIGHT-LEFT.LT.MAX( RTOL1*GAP, RTOL2*MAX(|LEFT|,|RIGHT|) ) */

/*  W       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          The first M elements of W contain the APPROXIMATE eigenvalues for */
/*          which eigenvectors are to be computed.  The eigenvalues */
/*          should be grouped by split-off block and ordered from */
/*          smallest to largest within the block ( The output array */
/*          W from DLARRE is expected here ). Furthermore, they are with */
/*          respect to the shift of the corresponding root representation */
/*          for their block. On exit, W holds the eigenvalues of the */
/*          UNshifted matrix. */

/*  WERR    (input/output) DOUBLE PRECISION array, dimension (N) */
/*          The first M elements contain the semiwidth of the uncertainty */
/*          interval of the corresponding eigenvalue in W */

/*  WGAP    (input/output) DOUBLE PRECISION array, dimension (N) */
/*          The separation from the right neighbor eigenvalue in W. */

/*  IBLOCK  (input) INTEGER array, dimension (N) */
/*          The indices of the blocks (submatrices) associated with the */
/*          corresponding eigenvalues in W; IBLOCK(i)=1 if eigenvalue */
/*          W(i) belongs to the first block from the top, =2 if W(i) */
/*          belongs to the second block, etc. */

/*  INDEXW  (input) INTEGER array, dimension (N) */
/*          The indices of the eigenvalues within each block (submatrix); */
/*          for example, INDEXW(i)= 10 and IBLOCK(i)=2 imply that the */
/*          i-th eigenvalue W(i) is the 10-th eigenvalue in the second block. */

/*  GERS    (input) DOUBLE PRECISION array, dimension (2*N) */
/*          The N Gerschgorin intervals (the i-th Gerschgorin interval */
/*          is (GERS(2*i-1), GERS(2*i)). The Gerschgorin intervals should */
/*          be computed from the original UNshifted matrix. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M) ) */
/*          If INFO = 0, the first M columns of Z contain the */
/*          orthonormal eigenvectors of the matrix T */
/*          corresponding to the input eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  ISUPPZ  (output) INTEGER array, dimension ( 2*max(1,M) ) */
/*          The support of the eigenvectors in Z, i.e., the indices */
/*          indicating the nonzero elements in Z. The I-th eigenvector */
/*          is nonzero only in elements ISUPPZ( 2*I-1 ) through */
/*          ISUPPZ( 2*I ). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (12*N) */

/*  IWORK   (workspace) INTEGER array, dimension (7*N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */

/*          > 0:  A problem occured in DLARRV. */
/*          < 0:  One of the called subroutines signaled an internal problem. */
/*                Needs inspection of the corresponding parameter IINFO */
/*                for further information. */

/*          =-1:  Problem in DLARRB when refining a child's eigenvalues. */
/*          =-2:  Problem in DLARRF when computing the RRR of a child. */
/*                When a child is inside a tight cluster, it can be difficult */
/*                to find an RRR. A partial remedy from the user's point of */
/*                view is to make the parameter MINRGP smaller and recompile. */
/*                However, as the orthogonality of the computed vectors is */
/*                proportional to 1/MINRGP, the user should be aware that */
/*                he might be trading in precision when he decreases MINRGP. */
/*          =-3:  Problem in DLARRB when refining a single eigenvalue */
/*                after the Rayleigh correction was rejected. */
/*          = 5:  The Rayleigh Quotient Iteration failed to converge to */
/*                full accuracy in MAXITR steps. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Beresford Parlett, University of California, Berkeley, USA */
/*     Jim Demmel, University of California, Berkeley, USA */
/*     Inderjit Dhillon, University of Texas, Austin, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */
/*     .. */
/*     The first N entries of WORK are reserved for the eigenvalues */
    /* Parameter adjustments */
    --d__;
    --l;
    --isplit;
    --w;
    --werr;
    --wgap;
    --iblock;
    --indexw;
    --gers;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --isuppz;
    --work;
    --iwork;

    /* Function Body */
    indld = *n + 1;
    indlld = (*n << 1) + 1;
    indwrk = *n * 3 + 1;
    minwsize = *n * 12;
    i__1 = minwsize;
    for (i__ = 1; i__ <= i__1; ++i__) {
	work[i__] = 0.;
/* L5: */
    }
/*     IWORK(IINDR+1:IINDR+N) hold the twist indices R for the */
/*     factorization used to compute the FP vector */
    iindr = 0;
/*     IWORK(IINDC1+1:IINC2+N) are used to store the clusters of the current */
/*     layer and the one above. */
    iindc1 = *n;
    iindc2 = *n << 1;
    iindwk = *n * 3 + 1;
    miniwsize = *n * 7;
    i__1 = miniwsize;
    for (i__ = 1; i__ <= i__1; ++i__) {
	iwork[i__] = 0;
/* L10: */
    }
    zusedl = 1;
    if (*dol > 1) {
/*        Set lower bound for use of Z */
	zusedl = *dol - 1;
    }
    zusedu = *m;
    if (*dou < *m) {
/*        Set lower bound for use of Z */
	zusedu = *dou + 1;
    }
/*     The width of the part of Z that is used */
    zusedw = zusedu - zusedl + 1;
    dlaset_("Full", n, &zusedw, &c_b5, &c_b5, &z__[zusedl * z_dim1 + 1], ldz);
    eps = dlamch_("Precision");
    rqtol = eps * 2.;

/*     Set expert flags for standard code. */
    tryrqc = true;
    if (*dol == 1 && *dou == *m) {
    } else {
/*        Only selected eigenpairs are computed. Since the other evalues */
/*        are not refined by RQ iteration, bisection has to compute to full */
/*        accuracy. */
	*rtol1 = eps * 4.;
	*rtol2 = eps * 4.;
    }
/*     The entries WBEGIN:WEND in W, WERR, WGAP correspond to the */
/*     desired eigenvalues. The support of the nonzero eigenvector */
/*     entries is contained in the interval IBEGIN:IEND. */
/*     Remark that if k eigenpairs are desired, then the eigenvectors */
/*     are stored in k contiguous columns of Z. */
/*     DONE is the number of eigenvectors already computed */
    done = 0;
    ibegin = 1;
    wbegin = 1;
    i__1 = iblock[*m];
    for (jblk = 1; jblk <= i__1; ++jblk) {
	iend = isplit[jblk];
	sigma = l[iend];
/*        Find the eigenvectors of the submatrix indexed IBEGIN */
/*        through IEND. */
	wend = wbegin - 1;
L15:
	if (wend < *m) {
	    if (iblock[wend + 1] == jblk) {
		++wend;
		goto L15;
	    }
	}
	if (wend < wbegin) {
	    ibegin = iend + 1;
	    goto L170;
	} else if (wend < *dol || wbegin > *dou) {
	    ibegin = iend + 1;
	    wbegin = wend + 1;
	    goto L170;
	}
/*        Find local spectral diameter of the block */
	gl = gers[(ibegin << 1) - 1];
	gu = gers[ibegin * 2];
	i__2 = iend;
	for (i__ = ibegin + 1; i__ <= i__2; ++i__) {
/* Computing MIN */
	    d__1 = gers[(i__ << 1) - 1];
	    gl = std::min(d__1,gl);
/* Computing MAX */
	    d__1 = gers[i__ * 2];
	    gu = std::max(d__1,gu);
/* L20: */
	}
	spdiam = gu - gl;
/*        OLDIEN is the last index of the previous block */
	oldien = ibegin - 1;
/*        Calculate the size of the current block */
	in = iend - ibegin + 1;
/*        The number of eigenvalues in the current block */
	im = wend - wbegin + 1;
/*        This is for a 1x1 block */
	if (ibegin == iend) {
	    ++done;
	    z__[ibegin + wbegin * z_dim1] = 1.;
	    isuppz[(wbegin << 1) - 1] = ibegin;
	    isuppz[wbegin * 2] = ibegin;
	    w[wbegin] += sigma;
	    work[wbegin] = w[wbegin];
	    ibegin = iend + 1;
	    ++wbegin;
	    goto L170;
	}
/*        The desired (shifted) eigenvalues are stored in W(WBEGIN:WEND) */
/*        Note that these can be approximations, in this case, the corresp. */
/*        entries of WERR give the size of the uncertainty interval. */
/*        The eigenvalue approximations will be refined when necessary as */
/*        high relative accuracy is required for the computation of the */
/*        corresponding eigenvectors. */
	dcopy_(&im, &w[wbegin], &c__1, &work[wbegin], &c__1);
/*        We store in W the eigenvalue approximations w.r.t. the original */
/*        matrix T. */
	i__2 = im;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    w[wbegin + i__ - 1] += sigma;
/* L30: */
	}
/*        NDEPTH is the current depth of the representation tree */
	ndepth = 0;
/*        PARITY is either 1 or 0 */
	parity = 1;
/*        NCLUS is the number of clusters for the next level of the */
/*        representation tree, we start with NCLUS = 1 for the root */
	nclus = 1;
	iwork[iindc1 + 1] = 1;
	iwork[iindc1 + 2] = im;
/*        IDONE is the number of eigenvectors already computed in the current */
/*        block */
	idone = 0;
/*        loop while( IDONE.LT.IM ) */
/*        generate the representation tree for the current block and */
/*        compute the eigenvectors */
L40:
	if (idone < im) {
/*           This is a crude protection against infinitely deep trees */
	    if (ndepth > *m) {
		*info = -2;
		return 0;
	    }
/*           breadth first processing of the current level of the representation */
/*           tree: OLDNCL = number of clusters on current level */
	    oldncl = nclus;
/*           reset NCLUS to count the number of child clusters */
	    nclus = 0;

	    parity = 1 - parity;
	    if (parity == 0) {
		oldcls = iindc1;
		newcls = iindc2;
	    } else {
		oldcls = iindc2;
		newcls = iindc1;
	    }
/*           Process the clusters on the current level */
	    i__2 = oldncl;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		j = oldcls + (i__ << 1);
/*              OLDFST, OLDLST = first, last index of current cluster. */
/*                               cluster indices start with 1 and are relative */
/*                               to WBEGIN when accessing W, WGAP, WERR, Z */
		oldfst = iwork[j - 1];
		oldlst = iwork[j];
		if (ndepth > 0) {
/*                 Retrieve relatively robust representation (RRR) of cluster */
/*                 that has been computed at the previous level */
/*                 The RRR is stored in Z and overwritten once the eigenvectors */
/*                 have been computed or when the cluster is refined */
		    if (*dol == 1 && *dou == *m) {
/*                    Get representation from location of the leftmost evalue */
/*                    of the cluster */
			j = wbegin + oldfst - 1;
		    } else {
			if (wbegin + oldfst - 1 < *dol) {
/*                       Get representation from the left end of Z array */
			    j = *dol - 1;
			} else if (wbegin + oldfst - 1 > *dou) {
/*                       Get representation from the right end of Z array */
			    j = *dou;
			} else {
			    j = wbegin + oldfst - 1;
			}
		    }
		    dcopy_(&in, &z__[ibegin + j * z_dim1], &c__1, &d__[ibegin]
, &c__1);
		    i__3 = in - 1;
		    dcopy_(&i__3, &z__[ibegin + (j + 1) * z_dim1], &c__1, &l[
			    ibegin], &c__1);
		    sigma = z__[iend + (j + 1) * z_dim1];
/*                 Set the corresponding entries in Z to zero */
		    dlaset_("Full", &in, &c__2, &c_b5, &c_b5, &z__[ibegin + j
			    * z_dim1], ldz);
		}
/*              Compute DL and DLL of current RRR */
		i__3 = iend - 1;
		for (j = ibegin; j <= i__3; ++j) {
		    tmp = d__[j] * l[j];
		    work[indld - 1 + j] = tmp;
		    work[indlld - 1 + j] = tmp * l[j];
/* L50: */
		}
		if (ndepth > 0) {
/*                 P and Q are index of the first and last eigenvalue to compute */
/*                 within the current block */
		    p = indexw[wbegin - 1 + oldfst];
		    q = indexw[wbegin - 1 + oldlst];
/*                 Offset for the arrays WORK, WGAP and WERR, i.e., th P-OFFSET */
/*                 thru' Q-OFFSET elements of these arrays are to be used. */
/*                  OFFSET = P-OLDFST */
		    offset = indexw[wbegin] - 1;
/*                 perform limited bisection (if necessary) to get approximate */
/*                 eigenvalues to the precision needed. */
		    dlarrb_(&in, &d__[ibegin], &work[indlld + ibegin - 1], &p,
			     &q, rtol1, rtol2, &offset, &work[wbegin], &wgap[
			    wbegin], &werr[wbegin], &work[indwrk], &iwork[
			    iindwk], pivmin, &spdiam, &in, &iinfo);
		    if (iinfo != 0) {
			*info = -1;
			return 0;
		    }
/*                 We also recompute the extremal gaps. W holds all eigenvalues */
/*                 of the unshifted matrix and must be used for computation */
/*                 of WGAP, the entries of WORK might stem from RRRs with */
/*                 different shifts. The gaps from WBEGIN-1+OLDFST to */
/*                 WBEGIN-1+OLDLST are correctly computed in DLARRB. */
/*                 However, we only allow the gaps to become greater since */
/*                 this is what should happen when we decrease WERR */
		    if (oldfst > 1) {
/* Computing MAX */
			d__1 = wgap[wbegin + oldfst - 2], d__2 = w[wbegin +
				oldfst - 1] - werr[wbegin + oldfst - 1] - w[
				wbegin + oldfst - 2] - werr[wbegin + oldfst -
				2];
			wgap[wbegin + oldfst - 2] = std::max(d__1,d__2);
		    }
		    if (wbegin + oldlst - 1 < wend) {
/* Computing MAX */
			d__1 = wgap[wbegin + oldlst - 1], d__2 = w[wbegin +
				oldlst] - werr[wbegin + oldlst] - w[wbegin +
				oldlst - 1] - werr[wbegin + oldlst - 1];
			wgap[wbegin + oldlst - 1] = std::max(d__1,d__2);
		    }
/*                 Each time the eigenvalues in WORK get refined, we store */
/*                 the newly found approximation with all shifts applied in W */
		    i__3 = oldlst;
		    for (j = oldfst; j <= i__3; ++j) {
			w[wbegin + j - 1] = work[wbegin + j - 1] + sigma;
/* L53: */
		    }
		}
/*              Process the current node. */
		newfst = oldfst;
		i__3 = oldlst;
		for (j = oldfst; j <= i__3; ++j) {
		    if (j == oldlst) {
/*                    we are at the right end of the cluster, this is also the */
/*                    boundary of the child cluster */
			newlst = j;
		    } else if (wgap[wbegin + j - 1] >= *minrgp * (d__1 = work[
			    wbegin + j - 1], abs(d__1))) {
/*                    the right relative gap is big enough, the child cluster */
/*                    (NEWFST,..,NEWLST) is well separated from the following */
			newlst = j;
		    } else {
/*                    inside a child cluster, the relative gap is not */
/*                    big enough. */
			goto L140;
		    }
/*                 Compute size of child cluster found */
		    newsiz = newlst - newfst + 1;
/*                 NEWFTT is the place in Z where the new RRR or the computed */
/*                 eigenvector is to be stored */
		    if (*dol == 1 && *dou == *m) {
/*                    Store representation at location of the leftmost evalue */
/*                    of the cluster */
			newftt = wbegin + newfst - 1;
		    } else {
			if (wbegin + newfst - 1 < *dol) {
/*                       Store representation at the left end of Z array */
			    newftt = *dol - 1;
			} else if (wbegin + newfst - 1 > *dou) {
/*                       Store representation at the right end of Z array */
			    newftt = *dou;
			} else {
			    newftt = wbegin + newfst - 1;
			}
		    }
		    if (newsiz > 1) {

/*                    Current child is not a singleton but a cluster. */
/*                    Compute and store new representation of child. */


/*                    Compute left and right cluster gap. */

/*                    LGAP and RGAP are not computed from WORK because */
/*                    the eigenvalue approximations may stem from RRRs */
/*                    different shifts. However, W hold all eigenvalues */
/*                    of the unshifted matrix. Still, the entries in WGAP */
/*                    have to be computed from WORK since the entries */
/*                    in W might be of the same order so that gaps are not */
/*                    exhibited correctly for very close eigenvalues. */
			if (newfst == 1) {
/* Computing MAX */
			    d__1 = 0., d__2 = w[wbegin] - werr[wbegin] - *vl;
			    lgap = std::max(d__1,d__2);
			} else {
			    lgap = wgap[wbegin + newfst - 2];
			}
			rgap = wgap[wbegin + newlst - 1];

/*                    Compute left- and rightmost eigenvalue of child */
/*                    to high precision in order to shift as close */
/*                    as possible and obtain as large relative gaps */
/*                    as possible */

			for (k = 1; k <= 2; ++k) {
			    if (k == 1) {
				p = indexw[wbegin - 1 + newfst];
			    } else {
				p = indexw[wbegin - 1 + newlst];
			    }
			    offset = indexw[wbegin] - 1;
			    dlarrb_(&in, &d__[ibegin], &work[indlld + ibegin
				    - 1], &p, &p, &rqtol, &rqtol, &offset, &
				    work[wbegin], &wgap[wbegin], &werr[wbegin]
, &work[indwrk], &iwork[iindwk], pivmin, &
				    spdiam, &in, &iinfo);
/* L55: */
			}

			if (wbegin + newlst - 1 < *dol || wbegin + newfst - 1
				> *dou) {
/*                       if the cluster contains no desired eigenvalues */
/*                       skip the computation of that branch of the rep. tree */

/*                       We could skip before the refinement of the extremal */
/*                       eigenvalues of the child, but then the representation */
/*                       tree could be different from the one when nothing is */
/*                       skipped. For this reason we skip at this place. */
			    idone = idone + newlst - newfst + 1;
			    goto L139;
			}

/*                    Compute RRR of child cluster. */
/*                    Note that the new RRR is stored in Z */

/*                    DLARRF needs LWORK = 2*N */
			dlarrf_(&in, &d__[ibegin], &l[ibegin], &work[indld +
				ibegin - 1], &newfst, &newlst, &work[wbegin],
				&wgap[wbegin], &werr[wbegin], &spdiam, &lgap,
				&rgap, pivmin, &tau, &z__[ibegin + newftt *
				z_dim1], &z__[ibegin + (newftt + 1) * z_dim1],
				 &work[indwrk], &iinfo);
			if (iinfo == 0) {
/*                       a new RRR for the cluster was found by DLARRF */
/*                       update shift and store it */
			    ssigma = sigma + tau;
			    z__[iend + (newftt + 1) * z_dim1] = ssigma;
/*                       WORK() are the midpoints and WERR() the semi-width */
/*                       Note that the entries in W are unchanged. */
			    i__4 = newlst;
			    for (k = newfst; k <= i__4; ++k) {
				fudge = eps * 3. * (d__1 = work[wbegin + k -
					1], abs(d__1));
				work[wbegin + k - 1] -= tau;
				fudge += eps * 4. * (d__1 = work[wbegin + k -
					1], abs(d__1));
/*                          Fudge errors */
				werr[wbegin + k - 1] += fudge;
/*                          Gaps are not fudged. Provided that WERR is small */
/*                          when eigenvalues are close, a zero gap indicates */
/*                          that a new representation is needed for resolving */
/*                          the cluster. A fudge could lead to a wrong decision */
/*                          of judging eigenvalues 'separated' which in */
/*                          reality are not. This could have a negative impact */
/*                          on the orthogonality of the computed eigenvectors. */
/* L116: */
			    }
			    ++nclus;
			    k = newcls + (nclus << 1);
			    iwork[k - 1] = newfst;
			    iwork[k] = newlst;
			} else {
			    *info = -2;
			    return 0;
			}
		    } else {

/*                    Compute eigenvector of singleton */

			iter = 0;

			tol = log((double) in) * 4. * eps;

			k = newfst;
			windex = wbegin + k - 1;
/* Computing MAX */
			i__4 = windex - 1;
			windmn = std::max(i__4,1_integer);
/* Computing MIN */
			i__4 = windex + 1;
			windpl = std::min(i__4,*m);
			lambda = work[windex];
			++done;
/*                    Check if eigenvector computation is to be skipped */
			if (windex < *dol || windex > *dou) {
			    eskip = true;
			    goto L125;
			} else {
			    eskip = false;
			}
			left = work[windex] - werr[windex];
			right = work[windex] + werr[windex];
			indeig = indexw[windex];
/*                    Note that since we compute the eigenpairs for a child, */
/*                    all eigenvalue approximations are w.r.t the same shift. */
/*                    In this case, the entries in WORK should be used for */
/*                    computing the gaps since they exhibit even very small */
/*                    differences in the eigenvalues, as opposed to the */
/*                    entries in W which might "look" the same. */
			if (k == 1) {
/*                       In the case RANGE='I' and with not much initial */
/*                       accuracy in LAMBDA and VL, the formula */
/*                       LGAP = MAX( ZERO, (SIGMA - VL) + LAMBDA ) */
/*                       can lead to an overestimation of the left gap and */
/*                       thus to inadequately early RQI 'convergence'. */
/*                       Prevent this by forcing a small left gap. */
/* Computing MAX */
			    d__1 = abs(left), d__2 = abs(right);
			    lgap = eps * std::max(d__1,d__2);
			} else {
			    lgap = wgap[windmn];
			}
			if (k == im) {
/*                       In the case RANGE='I' and with not much initial */
/*                       accuracy in LAMBDA and VU, the formula */
/*                       can lead to an overestimation of the right gap and */
/*                       thus to inadequately early RQI 'convergence'. */
/*                       Prevent this by forcing a small right gap. */
/* Computing MAX */
			    d__1 = abs(left), d__2 = abs(right);
			    rgap = eps * std::max(d__1,d__2);
			} else {
			    rgap = wgap[windex];
			}
			gap = std::min(lgap,rgap);
			if (k == 1 || k == im) {
/*                       The eigenvector support can become wrong */
/*                       because significant entries could be cut off due to a */
/*                       large GAPTOL parameter in LAR1V. Prevent this. */
			    gaptol = 0.;
			} else {
			    gaptol = gap * eps;
			}
			isupmn = in;
			isupmx = 1;
/*                    Update WGAP so that it holds the minimum gap */
/*                    to the left or the right. This is crucial in the */
/*                    case where bisection is used to ensure that the */
/*                    eigenvalue is refined up to the required precision. */
/*                    The correct value is restored afterwards. */
			savgap = wgap[windex];
			wgap[windex] = gap;
/*                    We want to use the Rayleigh Quotient Correction */
/*                    as often as possible since it converges quadratically */
/*                    when we are close enough to the desired eigenvalue. */
/*                    However, the Rayleigh Quotient can have the wrong sign */
/*                    and lead us away from the desired eigenvalue. In this */
/*                    case, the best we can do is to use bisection. */
			usedbs = false;
			usedrq = false;
/*                    Bisection is initially turned off unless it is forced */
			needbs = ! tryrqc;
L120:
/*                    Check if bisection should be used to refine eigenvalue */
			if (needbs) {
/*                       Take the bisection as new iterate */
			    usedbs = true;
			    itmp1 = iwork[iindr + windex];
			    offset = indexw[wbegin] - 1;
			    d__1 = eps * 2.;
			    dlarrb_(&in, &d__[ibegin], &work[indlld + ibegin
				    - 1], &indeig, &indeig, &c_b5, &d__1, &
				    offset, &work[wbegin], &wgap[wbegin], &
				    werr[wbegin], &work[indwrk], &iwork[
				    iindwk], pivmin, &spdiam, &itmp1, &iinfo);
			    if (iinfo != 0) {
				*info = -3;
				return 0;
			    }
			    lambda = work[windex];
/*                       Reset twist index from inaccurate LAMBDA to */
/*                       force computation of true MINGMA */
			    iwork[iindr + windex] = 0;
			}
/*                    Given LAMBDA, compute the eigenvector. */
			L__1 = ! usedbs;
			dlar1v_(&in, &c__1, &in, &lambda, &d__[ibegin], &l[
				ibegin], &work[indld + ibegin - 1], &work[
				indlld + ibegin - 1], pivmin, &gaptol, &z__[
				ibegin + windex * z_dim1], &L__1, &negcnt, &
				ztz, &mingma, &iwork[iindr + windex], &isuppz[
				(windex << 1) - 1], &nrminv, &resid, &rqcorr,
				&work[indwrk]);
			if (iter == 0) {
			    bstres = resid;
			    bstw = lambda;
			} else if (resid < bstres) {
			    bstres = resid;
			    bstw = lambda;
			}
/* Computing MIN */
			i__4 = isupmn, i__5 = isuppz[(windex << 1) - 1];
			isupmn = std::min(i__4,i__5);
/* Computing MAX */
			i__4 = isupmx, i__5 = isuppz[windex * 2];
			isupmx = std::max(i__4,i__5);
			++iter;
/*                    sin alpha <= |resid|/gap */
/*                    Note that both the residual and the gap are */
/*                    proportional to the matrix, so ||T|| doesn't play */
/*                    a role in the quotient */

/*                    Convergence test for Rayleigh-Quotient iteration */
/*                    (omitted when Bisection has been used) */

			if (resid > tol * gap && abs(rqcorr) > rqtol * abs(
				lambda) && ! usedbs) {
/*                       We need to check that the RQCORR update doesn't */
/*                       move the eigenvalue away from the desired one and */
/*                       towards a neighbor. -> protection with bisection */
			    if (indeig <= negcnt) {
/*                          The wanted eigenvalue lies to the left */
				sgndef = -1.;
			    } else {
/*                          The wanted eigenvalue lies to the right */
				sgndef = 1.;
			    }
/*                       We only use the RQCORR if it improves the */
/*                       the iterate reasonably. */
			    if (rqcorr * sgndef >= 0. && lambda + rqcorr <=
				    right && lambda + rqcorr >= left) {
				usedrq = true;
/*                          Store new midpoint of bisection interval in WORK */
				if (sgndef == 1.) {
/*                             The current LAMBDA is on the left of the true */
/*                             eigenvalue */
				    left = lambda;
/*                             We prefer to assume that the error estimate */
/*                             is correct. We could make the interval not */
/*                             as a bracket but to be modified if the RQCORR */
/*                             chooses to. In this case, the RIGHT side should */
/*                             be modified as follows: */
/*                              RIGHT = MAX(RIGHT, LAMBDA + RQCORR) */
				} else {
/*                             The current LAMBDA is on the right of the true */
/*                             eigenvalue */
				    right = lambda;
/*                             See comment about assuming the error estimate is */
/*                             correct above. */
/*                              LEFT = MIN(LEFT, LAMBDA + RQCORR) */
				}
				work[windex] = (right + left) * .5;
/*                          Take RQCORR since it has the correct sign and */
/*                          improves the iterate reasonably */
				lambda += rqcorr;
/*                          Update width of error interval */
				werr[windex] = (right - left) * .5;
			    } else {
				needbs = true;
			    }
			    if (right - left < rqtol * abs(lambda)) {
/*                             The eigenvalue is computed to bisection accuracy */
/*                             compute eigenvector and stop */
				usedbs = true;
				goto L120;
			    } else if (iter < 10) {
				goto L120;
			    } else if (iter == 10) {
				needbs = true;
				goto L120;
			    } else {
				*info = 5;
				return 0;
			    }
			} else {
			    stp2ii = false;
			    if (usedrq && usedbs && bstres <= resid) {
				lambda = bstw;
				stp2ii = true;
			    }
			    if (stp2ii) {
/*                          improve error angle by second step */
				L__1 = ! usedbs;
				dlar1v_(&in, &c__1, &in, &lambda, &d__[ibegin]
, &l[ibegin], &work[indld + ibegin -
					1], &work[indlld + ibegin - 1],
					pivmin, &gaptol, &z__[ibegin + windex
					* z_dim1], &L__1, &negcnt, &ztz, &
					mingma, &iwork[iindr + windex], &
					isuppz[(windex << 1) - 1], &nrminv, &
					resid, &rqcorr, &work[indwrk]);
			    }
			    work[windex] = lambda;
			}

/*                    Compute FP-vector support w.r.t. whole matrix */

			isuppz[(windex << 1) - 1] += oldien;
			isuppz[windex * 2] += oldien;
			zfrom = isuppz[(windex << 1) - 1];
			zto = isuppz[windex * 2];
			isupmn += oldien;
			isupmx += oldien;
/*                    Ensure vector is ok if support in the RQI has changed */
			if (isupmn < zfrom) {
			    i__4 = zfrom - 1;
			    for (ii = isupmn; ii <= i__4; ++ii) {
				z__[ii + windex * z_dim1] = 0.;
/* L122: */
			    }
			}
			if (isupmx > zto) {
			    i__4 = isupmx;
			    for (ii = zto + 1; ii <= i__4; ++ii) {
				z__[ii + windex * z_dim1] = 0.;
/* L123: */
			    }
			}
			i__4 = zto - zfrom + 1;
			dscal_(&i__4, &nrminv, &z__[zfrom + windex * z_dim1],
				&c__1);
L125:
/*                    Update W */
			w[windex] = lambda + sigma;
/*                    Recompute the gaps on the left and right */
/*                    But only allow them to become larger and not */
/*                    smaller (which can only happen through "bad" */
/*                    cancellation and doesn't reflect the theory */
/*                    where the initial gaps are underestimated due */
/*                    to WERR being too crude.) */
			if (! eskip) {
			    if (k > 1) {
/* Computing MAX */
				d__1 = wgap[windmn], d__2 = w[windex] - werr[
					windex] - w[windmn] - werr[windmn];
				wgap[windmn] = std::max(d__1,d__2);
			    }
			    if (windex < wend) {
/* Computing MAX */
				d__1 = savgap, d__2 = w[windpl] - werr[windpl]
					 - w[windex] - werr[windex];
				wgap[windex] = std::max(d__1,d__2);
			    }
			}
			++idone;
		    }
/*                 here ends the code for the current child */

L139:
/*                 Proceed to any remaining child nodes */
		    newfst = j + 1;
L140:
		    ;
		}
/* L150: */
	    }
	    ++ndepth;
	    goto L40;
	}
	ibegin = iend + 1;
	wbegin = wend + 1;
L170:
	;
    }

    return 0;

/*     End of DLARRV */

} /* dlarrv_ */

int dlarscl2_(integer *m, integer *n, double *d__, double *x, integer *ldx)
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

/*  DLARSCL2 performs a reciprocal diagonal scaling on an vector: */
/*    x <-- inv(D) * x */
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
	    x[i__ + j * x_dim1] /= d__[i__];
	}
    }
    return 0;
} /* dlarscl2_ */

/* Subroutine */ int dlartg_(double *f, double *g, double *cs,
	double *sn, double *r__)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    integer i__;
    double f1, g1, eps, scale;
    integer count;
    double safmn2, safmx2;

    double safmin;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARTG generate a plane rotation so that */

/*     [  CS  SN  ]  .  [ F ]  =  [ R ]   where CS**2 + SN**2 = 1. */
/*     [ -SN  CS  ]     [ G ]     [ 0 ] */

/*  This is a slower, more accurate version of the BLAS1 routine DROTG, */
/*  with the following other differences: */
/*     F and G are unchanged on return. */
/*     If G=0, then CS=1 and SN=0. */
/*     If F=0 and (G .ne. 0), then CS=0 and SN=1 without doing any */
/*        floating point operations (saves work in DBDSQR when */
/*        there are zeros on the diagonal). */

/*  If F exceeds G in magnitude, CS will be positive. */

/*  Arguments */
/*  ========= */

/*  F       (input) DOUBLE PRECISION */
/*          The first component of vector to be rotated. */

/*  G       (input) DOUBLE PRECISION */
/*          The second component of vector to be rotated. */

/*  CS      (output) DOUBLE PRECISION */
/*          The cosine of the rotation. */

/*  SN      (output) DOUBLE PRECISION */
/*          The sine of the rotation. */

/*  R       (output) DOUBLE PRECISION */
/*          The nonzero component of the rotated vector. */

/*  This version has a few statements commented out for thread safety */
/*  (machine parameters are computed on each entry). 10 feb 03, SJH. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     LOGICAL            FIRST */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Save statement .. */
/*     SAVE               FIRST, SAFMX2, SAFMIN, SAFMN2 */
/*     .. */
/*     .. Data statements .. */
/*     DATA               FIRST / .TRUE. / */
/*     .. */
/*     .. Executable Statements .. */

/*     IF( FIRST ) THEN */
    safmin = dlamch_("S");
    eps = dlamch_("E");
    d__1 = dlamch_("B");
    i__1 = (integer) (log(safmin / eps) / log(dlamch_("B")) / 2.);
    safmn2 = pow_di(&d__1, &i__1);
    safmx2 = 1. / safmn2;
/*        FIRST = .FALSE. */
/*     END IF */
    if (*g == 0.) {
	*cs = 1.;
	*sn = 0.;
	*r__ = *f;
    } else if (*f == 0.) {
	*cs = 0.;
	*sn = 1.;
	*r__ = *g;
    } else {
	f1 = *f;
	g1 = *g;
/* Computing MAX */
	d__1 = abs(f1), d__2 = abs(g1);
	scale = std::max(d__1,d__2);
	if (scale >= safmx2) {
	    count = 0;
L10:
	    ++count;
	    f1 *= safmn2;
	    g1 *= safmn2;
/* Computing MAX */
	    d__1 = abs(f1), d__2 = abs(g1);
	    scale = std::max(d__1,d__2);
	    if (scale >= safmx2) {
		goto L10;
	    }
/* Computing 2nd power */
	    d__1 = f1;
/* Computing 2nd power */
	    d__2 = g1;
	    *r__ = sqrt(d__1 * d__1 + d__2 * d__2);
	    *cs = f1 / *r__;
	    *sn = g1 / *r__;
	    i__1 = count;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		*r__ *= safmx2;
/* L20: */
	    }
	} else if (scale <= safmn2) {
	    count = 0;
L30:
	    ++count;
	    f1 *= safmx2;
	    g1 *= safmx2;
/* Computing MAX */
	    d__1 = abs(f1), d__2 = abs(g1);
	    scale = std::max(d__1,d__2);
	    if (scale <= safmn2) {
		goto L30;
	    }
/* Computing 2nd power */
	    d__1 = f1;
/* Computing 2nd power */
	    d__2 = g1;
	    *r__ = sqrt(d__1 * d__1 + d__2 * d__2);
	    *cs = f1 / *r__;
	    *sn = g1 / *r__;
	    i__1 = count;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		*r__ *= safmn2;
/* L40: */
	    }
	} else {
/* Computing 2nd power */
	    d__1 = f1;
/* Computing 2nd power */
	    d__2 = g1;
	    *r__ = sqrt(d__1 * d__1 + d__2 * d__2);
	    *cs = f1 / *r__;
	    *sn = g1 / *r__;
	}
	if (abs(*f) > abs(*g) && *cs < 0.) {
	    *cs = -(*cs);
	    *sn = -(*sn);
	    *r__ = -(*r__);
	}
    }
    return 0;

/*     End of DLARTG */

} /* dlartg_ */

/* Subroutine */ int dlartv_(integer *n, double *x, integer *incx,
	double *y, integer *incy, double *c__, double *s, integer
	*incc)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, ic, ix, iy;
    double xi, yi;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARTV applies a vector of real plane rotations to elements of the */
/*  real vectors x and y. For i = 1,2,...,n */

/*     ( x(i) ) := (  c(i)  s(i) ) ( x(i) ) */
/*     ( y(i) )    ( -s(i)  c(i) ) ( y(i) ) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of plane rotations to be applied. */

/*  X       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCX) */
/*          The vector x. */

/*  INCX    (input) INTEGER */
/*          The increment between elements of X. INCX > 0. */

/*  Y       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCY) */
/*          The vector y. */

/*  INCY    (input) INTEGER */
/*          The increment between elements of Y. INCY > 0. */

/*  C       (input) DOUBLE PRECISION array, dimension (1+(N-1)*INCC) */
/*          The cosines of the plane rotations. */

/*  S       (input) DOUBLE PRECISION array, dimension (1+(N-1)*INCC) */
/*          The sines of the plane rotations. */

/*  INCC    (input) INTEGER */
/*          The increment between elements of C and S. INCC > 0. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --s;
    --c__;
    --y;
    --x;

    /* Function Body */
    ix = 1;
    iy = 1;
    ic = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	xi = x[ix];
	yi = y[iy];
	x[ix] = c__[ic] * xi + s[ic] * yi;
	y[iy] = c__[ic] * yi - s[ic] * xi;
	ix += *incx;
	iy += *incy;
	ic += *incc;
/* L10: */
    }
    return 0;

/*     End of DLARTV */

} /* dlartv_ */

/* Subroutine */ int dlaruv_(integer *iseed, integer *n, double *x)
{
    /* Initialized data */
    static integer mm[512]	/* was [128][4] */ = { 494,2637,255,2008,1253,
	    3344,4084,1739,3143,3468,688,1657,1238,3166,1292,3422,1270,2016,
	    154,2862,697,1706,491,931,1444,444,3577,3944,2184,1661,3482,657,
	    3023,3618,1267,1828,164,3798,3087,2400,2870,3876,1905,1593,1797,
	    1234,3460,328,2861,1950,617,2070,3331,769,1558,2412,2800,189,287,
	    2045,1227,2838,209,2770,3654,3993,192,2253,3491,2889,2857,2094,
	    1818,688,1407,634,3231,815,3524,1914,516,164,303,2144,3480,119,
	    3357,837,2826,2332,2089,3780,1700,3712,150,2000,3375,1621,3090,
	    3765,1149,3146,33,3082,2741,359,3316,1749,185,2784,2202,2199,1364,
	    1244,2020,3160,2785,2772,1217,1822,1245,2252,3904,2774,997,2573,
	    1148,545,322,789,1440,752,2859,123,1848,643,2405,2638,2344,46,
	    3814,913,3649,339,3808,822,2832,3078,3633,2970,637,2249,2081,4019,
	    1478,242,481,2075,4058,622,3376,812,234,641,4005,1122,3135,2640,
	    2302,40,1832,2247,2034,2637,1287,1691,496,1597,2394,2584,1843,336,
	    1472,2407,433,2096,1761,2810,566,442,41,1238,1086,603,840,3168,
	    1499,1084,3438,2408,1589,2391,288,26,512,1456,171,1677,2657,2270,
	    2587,2961,1970,1817,676,1410,3723,2803,3185,184,663,499,3784,1631,
	    1925,3912,1398,1349,1441,2224,2411,1907,3192,2786,382,37,759,2948,
	    1862,3802,2423,2051,2295,1332,1832,2405,3638,3661,327,3660,716,
	    1842,3987,1368,1848,2366,2508,3754,1766,3572,2893,307,1297,3966,
	    758,2598,3406,2922,1038,2934,2091,2451,1580,1958,2055,1507,1078,
	    3273,17,854,2916,3971,2889,3831,2621,1541,893,736,3992,787,2125,
	    2364,2460,257,1574,3912,1216,3248,3401,2124,2762,149,2245,166,466,
	    4018,1399,190,2879,153,2320,18,712,2159,2318,2091,3443,1510,449,
	    1956,2201,3137,3399,1321,2271,3667,2703,629,2365,2431,1113,3922,
	    2554,184,2099,3228,4012,1921,3452,3901,572,3309,3171,817,3039,
	    1696,1256,3715,2077,3019,1497,1101,717,51,981,1978,1813,3881,76,
	    3846,3694,1682,124,1660,3997,479,1141,886,3514,1301,3604,1888,
	    1836,1990,2058,692,1194,20,3285,2046,2107,3508,3525,3801,2549,
	    1145,2253,305,3301,1065,3133,2913,3285,1241,1197,3729,2501,1673,
	    541,2753,949,2361,1165,4081,2725,3305,3069,3617,3733,409,2157,
	    1361,3973,1865,2525,1409,3445,3577,77,3761,2149,1449,3005,225,85,
	    3673,3117,3089,1349,2057,413,65,1845,697,3085,3441,1573,3689,2941,
	    929,533,2841,4077,721,2821,2249,2397,2817,245,1913,1997,3121,997,
	    1833,2877,1633,981,2009,941,2449,197,2441,285,1473,2741,3129,909,
	    2801,421,4073,2813,2337,1429,1177,1901,81,1669,2633,2269,129,1141,
	    249,3917,2481,3941,2217,2749,3041,1877,345,2861,1809,3141,2825,
	    157,2881,3637,1465,2829,2161,3365,361,2685,3745,2325,3609,3821,
	    3537,517,3017,2141,1537 };

    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, i1, i2, i3, i4, it1, it2, it3, it4;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARUV returns a vector of n random real numbers from a uniform (0,1) */
/*  distribution (n <= 128). */

/*  This is an auxiliary routine called by DLARNV and ZLARNV. */

/*  Arguments */
/*  ========= */

/*  ISEED   (input/output) INTEGER array, dimension (4) */
/*          On entry, the seed of the random number generator; the array */
/*          elements must be between 0 and 4095, and ISEED(4) must be */
/*          odd. */
/*          On exit, the seed is updated. */

/*  N       (input) INTEGER */
/*          The number of random numbers to be generated. N <= 128. */

/*  X       (output) DOUBLE PRECISION array, dimension (N) */
/*          The generated random numbers. */

/*  Further Details */
/*  =============== */

/*  This routine uses a multiplicative congruential method with modulus */
/*  2**48 and multiplier 33952834046453 (see G.S.Fishman, */
/*  'Multiplicative congruential random number generators with modulus */
/*  2**b: an exhaustive analysis for b = 32 and a partial analysis for */
/*  b = 48', Math. Comp. 189, pp 331-344, 1990). */

/*  48-bit integers are stored in 4 integer array elements with 12 bits */
/*  per element. Hence the routine is portable across machines with */
/*  integers of 32 bits or more. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Data statements .. */
    /* Parameter adjustments */
    --iseed;
    --x;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

    i1 = iseed[1];
    i2 = iseed[2];
    i3 = iseed[3];
    i4 = iseed[4];

    i__1 = std::min(*n,128_integer);
    for (i__ = 1; i__ <= i__1; ++i__) {

L20:

/*        Multiply the seed by i-th power of the multiplier modulo 2**48 */

	it4 = i4 * mm[i__ + 383];
	it3 = it4 / 4096;
	it4 -= it3 << 12;
	it3 = it3 + i3 * mm[i__ + 383] + i4 * mm[i__ + 255];
	it2 = it3 / 4096;
	it3 -= it2 << 12;
	it2 = it2 + i2 * mm[i__ + 383] + i3 * mm[i__ + 255] + i4 * mm[i__ +
		127];
	it1 = it2 / 4096;
	it2 -= it1 << 12;
	it1 = it1 + i1 * mm[i__ + 383] + i2 * mm[i__ + 255] + i3 * mm[i__ +
		127] + i4 * mm[i__ - 1];
	it1 %= 4096;

/*        Convert 48-bit integer to a real number in the interval (0,1) */

	x[i__] = ((double) it1 + ((double) it2 + ((double) it3 + (
		double) it4 * 2.44140625e-4) * 2.44140625e-4) *
		2.44140625e-4) * 2.44140625e-4;

	if (x[i__] == 1.) {
/*           If a real number has n bits of precision, and the first */
/*           n bits of the 48-bit integer above happen to be all 1 (which */
/*           will occur about once every 2**n calls), then X( I ) will */
/*           be rounded to exactly 1.0. */
/*           Since X( I ) is not supposed to return exactly 0.0 or 1.0, */
/*           the statistically correct thing to do in this situation is */
/*           simply to iterate again. */
/*           N.B. the case X( I ) = 0.0 should not be possible. */
	    i1 += 2;
	    i2 += 2;
	    i3 += 2;
	    i4 += 2;
	    goto L20;
	}

/* L10: */
    }

/*     Return final value of seed */

    iseed[1] = it1;
    iseed[2] = it2;
    iseed[3] = it3;
    iseed[4] = it4;
    return 0;

/*     End of DLARUV */

} /* dlaruv_ */

/* Subroutine */ int dlarz_(const char *side, integer *m, integer *n, integer *l,
	double *v, integer *incv, double *tau, double *c__,
	integer *ldc, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b5 = 1.;

    /* System generated locals */
    integer c_dim1, c_offset;
    double d__1;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARZ applies a real elementary reflector H to a real M-by-N */
/*  matrix C, from either the left or the right. H is represented in the */
/*  form */

/*        H = I - tau * v * v' */

/*  where tau is a real scalar and v is a real vector. */

/*  If tau = 0, then H is taken to be the unit matrix. */


/*  H is a product of k elementary reflectors as returned by DTZRZF. */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'L': form  H * C */
/*          = 'R': form  C * H */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix C. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix C. */

/*  L       (input) INTEGER */
/*          The number of entries of the vector V containing */
/*          the meaningful part of the Householder vectors. */
/*          If SIDE = 'L', M >= L >= 0, if SIDE = 'R', N >= L >= 0. */

/*  V       (input) DOUBLE PRECISION array, dimension (1+(L-1)*abs(INCV)) */
/*          The vector v in the representation of H as returned by */
/*          DTZRZF. V is not used if TAU = 0. */

/*  INCV    (input) INTEGER */
/*          The increment between elements of v. INCV <> 0. */

/*  TAU     (input) DOUBLE PRECISION */
/*          The value tau in the representation of H. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC,N) */
/*          On entry, the M-by-N matrix C. */
/*          On exit, C is overwritten by the matrix H * C if SIDE = 'L', */
/*          or C * H if SIDE = 'R'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C. LDC >= max(1,M). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                         (N) if SIDE = 'L' */
/*                      or (M) if SIDE = 'R' */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --v;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    if (lsame_(side, "L")) {

/*        Form  H * C */

	if (*tau != 0.) {

/*           w( 1:n ) = C( 1, 1:n ) */

	    dcopy_(n, &c__[c_offset], ldc, &work[1], &c__1);

/*           w( 1:n ) = w( 1:n ) + C( m-l+1:m, 1:n )' * v( 1:l ) */

	    dgemv_("Transpose", l, n, &c_b5, &c__[*m - *l + 1 + c_dim1], ldc,
		    &v[1], incv, &c_b5, &work[1], &c__1);

/*           C( 1, 1:n ) = C( 1, 1:n ) - tau * w( 1:n ) */

	    d__1 = -(*tau);
	    daxpy_(n, &d__1, &work[1], &c__1, &c__[c_offset], ldc);

/*           C( m-l+1:m, 1:n ) = C( m-l+1:m, 1:n ) - ... */
/*                               tau * v( 1:l ) * w( 1:n )' */

	    d__1 = -(*tau);
	    dger_(l, n, &d__1, &v[1], incv, &work[1], &c__1, &c__[*m - *l + 1
		    + c_dim1], ldc);
	}

    } else {

/*        Form  C * H */

	if (*tau != 0.) {

/*           w( 1:m ) = C( 1:m, 1 ) */

	    dcopy_(m, &c__[c_offset], &c__1, &work[1], &c__1);

/*           w( 1:m ) = w( 1:m ) + C( 1:m, n-l+1:n, 1:n ) * v( 1:l ) */

	    dgemv_("No transpose", m, l, &c_b5, &c__[(*n - *l + 1) * c_dim1 +
		    1], ldc, &v[1], incv, &c_b5, &work[1], &c__1);

/*           C( 1:m, 1 ) = C( 1:m, 1 ) - tau * w( 1:m ) */

	    d__1 = -(*tau);
	    daxpy_(m, &d__1, &work[1], &c__1, &c__[c_offset], &c__1);

/*           C( 1:m, n-l+1:n ) = C( 1:m, n-l+1:n ) - ... */
/*                               tau * w( 1:m ) * v( 1:l )' */

	    d__1 = -(*tau);
	    dger_(m, l, &d__1, &work[1], &c__1, &v[1], incv, &c__[(*n - *l +
		    1) * c_dim1 + 1], ldc);

	}

    }

    return 0;

/*     End of DLARZ */

} /* dlarz_ */

/* Subroutine */ int dlarzb_(const char *side, const char *trans, const char *direct, const char *
	storev, integer *m, integer *n, integer *k, integer *l, double *v,
	integer *ldv, double *t, integer *ldt, double *c__, integer *
	ldc, double *work, integer *ldwork)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b13 = 1.;
	static double c_b23 = -1.;

    /* System generated locals */
    integer c_dim1, c_offset, t_dim1, t_offset, v_dim1, v_offset, work_dim1,
	    work_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, info;
    char transt[1];


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARZB applies a real block reflector H or its transpose H**T to */
/*  a real distributed M-by-N  C from the left or the right. */

/*  Currently, only STOREV = 'R' and DIRECT = 'B' are supported. */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'L': apply H or H' from the Left */
/*          = 'R': apply H or H' from the Right */

/*  TRANS   (input) CHARACTER*1 */
/*          = 'N': apply H (No transpose) */
/*          = 'C': apply H' (Transpose) */

/*  DIRECT  (input) CHARACTER*1 */
/*          Indicates how H is formed from a product of elementary */
/*          reflectors */
/*          = 'F': H = H(1) H(2) . . . H(k) (Forward, not supported yet) */
/*          = 'B': H = H(k) . . . H(2) H(1) (Backward) */

/*  STOREV  (input) CHARACTER*1 */
/*          Indicates how the vectors which define the elementary */
/*          reflectors are stored: */
/*          = 'C': Columnwise                        (not supported yet) */
/*          = 'R': Rowwise */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix C. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix C. */

/*  K       (input) INTEGER */
/*          The order of the matrix T (= the number of elementary */
/*          reflectors whose product defines the block reflector). */

/*  L       (input) INTEGER */
/*          The number of columns of the matrix V containing the */
/*          meaningful part of the Householder reflectors. */
/*          If SIDE = 'L', M >= L >= 0, if SIDE = 'R', N >= L >= 0. */

/*  V       (input) DOUBLE PRECISION array, dimension (LDV,NV). */
/*          If STOREV = 'C', NV = K; if STOREV = 'R', NV = L. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. */
/*          If STOREV = 'C', LDV >= L; if STOREV = 'R', LDV >= K. */

/*  T       (input) DOUBLE PRECISION array, dimension (LDT,K) */
/*          The triangular K-by-K matrix T in the representation of the */
/*          block reflector. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T. LDT >= K. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC,N) */
/*          On entry, the M-by-N matrix C. */
/*          On exit, C is overwritten by H*C or H'*C or C*H or C*H'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C. LDC >= max(1,M). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LDWORK,K) */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of the array WORK. */
/*          If SIDE = 'L', LDWORK >= max(1,N); */
/*          if SIDE = 'R', LDWORK >= max(1,M). */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

    /* Parameter adjustments */
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    work_dim1 = *ldwork;
    work_offset = 1 + work_dim1;
    work -= work_offset;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	return 0;
    }

/*     Check for currently supported options */

    info = 0;
    if (! lsame_(direct, "B")) {
	info = -3;
    } else if (! lsame_(storev, "R")) {
	info = -4;
    }
    if (info != 0) {
	i__1 = -info;
	xerbla_("DLARZB", &i__1);
	return 0;
    }

    if (lsame_(trans, "N")) {
	*(unsigned char *)transt = 'T';
    } else {
	*(unsigned char *)transt = 'N';
    }

    if (lsame_(side, "L")) {

/*        Form  H * C  or  H' * C */

/*        W( 1:n, 1:k ) = C( 1:k, 1:n )' */

	i__1 = *k;
	for (j = 1; j <= i__1; ++j) {
	    dcopy_(n, &c__[j + c_dim1], ldc, &work[j * work_dim1 + 1], &c__1);
/* L10: */
	}

/*        W( 1:n, 1:k ) = W( 1:n, 1:k ) + ... */
/*                        C( m-l+1:m, 1:n )' * V( 1:k, 1:l )' */

	if (*l > 0) {
	    dgemm_("Transpose", "Transpose", n, k, l, &c_b13, &c__[*m - *l +
		    1 + c_dim1], ldc, &v[v_offset], ldv, &c_b13, &work[
		    work_offset], ldwork);
	}

/*        W( 1:n, 1:k ) = W( 1:n, 1:k ) * T'  or  W( 1:m, 1:k ) * T */

	dtrmm_("Right", "Lower", transt, "Non-unit", n, k, &c_b13, &t[
		t_offset], ldt, &work[work_offset], ldwork);

/*        C( 1:k, 1:n ) = C( 1:k, 1:n ) - W( 1:n, 1:k )' */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *k;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		c__[i__ + j * c_dim1] -= work[j + i__ * work_dim1];
/* L20: */
	    }
/* L30: */
	}

/*        C( m-l+1:m, 1:n ) = C( m-l+1:m, 1:n ) - ... */
/*                            V( 1:k, 1:l )' * W( 1:n, 1:k )' */

	if (*l > 0) {
	    dgemm_("Transpose", "Transpose", l, n, k, &c_b23, &v[v_offset],
		    ldv, &work[work_offset], ldwork, &c_b13, &c__[*m - *l + 1
		    + c_dim1], ldc);
	}

    } else if (lsame_(side, "R")) {

/*        Form  C * H  or  C * H' */

/*        W( 1:m, 1:k ) = C( 1:m, 1:k ) */

	i__1 = *k;
	for (j = 1; j <= i__1; ++j) {
	    dcopy_(m, &c__[j * c_dim1 + 1], &c__1, &work[j * work_dim1 + 1], &
		    c__1);
/* L40: */
	}

/*        W( 1:m, 1:k ) = W( 1:m, 1:k ) + ... */
/*                        C( 1:m, n-l+1:n ) * V( 1:k, 1:l )' */

	if (*l > 0) {
	    dgemm_("No transpose", "Transpose", m, k, l, &c_b13, &c__[(*n - *
		    l + 1) * c_dim1 + 1], ldc, &v[v_offset], ldv, &c_b13, &
		    work[work_offset], ldwork);
	}

/*        W( 1:m, 1:k ) = W( 1:m, 1:k ) * T  or  W( 1:m, 1:k ) * T' */

	dtrmm_("Right", "Lower", trans, "Non-unit", m, k, &c_b13, &t[t_offset]
, ldt, &work[work_offset], ldwork);

/*        C( 1:m, 1:k ) = C( 1:m, 1:k ) - W( 1:m, 1:k ) */

	i__1 = *k;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		c__[i__ + j * c_dim1] -= work[i__ + j * work_dim1];
/* L50: */
	    }
/* L60: */
	}

/*        C( 1:m, n-l+1:n ) = C( 1:m, n-l+1:n ) - ... */
/*                            W( 1:m, 1:k ) * V( 1:k, 1:l ) */

	if (*l > 0) {
	    dgemm_("No transpose", "No transpose", m, l, k, &c_b23, &work[
		    work_offset], ldwork, &v[v_offset], ldv, &c_b13, &c__[(*n
		    - *l + 1) * c_dim1 + 1], ldc);
	}

    }

    return 0;

/*     End of DLARZB */

} /* dlarzb_ */

/* Subroutine */ int dlarzt_(const char *direct, const char *storev, integer *n, integer *
	k, double *v, integer *ldv, double *tau, double *t,
	integer *ldt)
{
	/* Table of constant values */
	static double c_b8 = 0.;
	static integer c__1 = 1;

    /* System generated locals */
    integer t_dim1, t_offset, v_dim1, v_offset, i__1;
    double d__1;

    /* Local variables */
    integer i__, j, info;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARZT forms the triangular factor T of a real block reflector */
/*  H of order > n, which is defined as a product of k elementary */
/*  reflectors. */

/*  If DIRECT = 'F', H = H(1) H(2) . . . H(k) and T is upper triangular; */

/*  If DIRECT = 'B', H = H(k) . . . H(2) H(1) and T is lower triangular. */

/*  If STOREV = 'C', the vector which defines the elementary reflector */
/*  H(i) is stored in the i-th column of the array V, and */

/*     H  =  I - V * T * V' */

/*  If STOREV = 'R', the vector which defines the elementary reflector */
/*  H(i) is stored in the i-th row of the array V, and */

/*     H  =  I - V' * T * V */

/*  Currently, only STOREV = 'R' and DIRECT = 'B' are supported. */

/*  Arguments */
/*  ========= */

/*  DIRECT  (input) CHARACTER*1 */
/*          Specifies the order in which the elementary reflectors are */
/*          multiplied to form the block reflector: */
/*          = 'F': H = H(1) H(2) . . . H(k) (Forward, not supported yet) */
/*          = 'B': H = H(k) . . . H(2) H(1) (Backward) */

/*  STOREV  (input) CHARACTER*1 */
/*          Specifies how the vectors which define the elementary */
/*          reflectors are stored (see also Further Details): */
/*          = 'C': columnwise                        (not supported yet) */
/*          = 'R': rowwise */

/*  N       (input) INTEGER */
/*          The order of the block reflector H. N >= 0. */

/*  K       (input) INTEGER */
/*          The order of the triangular factor T (= the number of */
/*          elementary reflectors). K >= 1. */

/*  V       (input/output) DOUBLE PRECISION array, dimension */
/*                               (LDV,K) if STOREV = 'C' */
/*                               (LDV,N) if STOREV = 'R' */
/*          The matrix V. See further details. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. */
/*          If STOREV = 'C', LDV >= max(1,N); if STOREV = 'R', LDV >= K. */

/*  TAU     (input) DOUBLE PRECISION array, dimension (K) */
/*          TAU(i) must contain the scalar factor of the elementary */
/*          reflector H(i). */

/*  T       (output) DOUBLE PRECISION array, dimension (LDT,K) */
/*          The k by k triangular factor T of the block reflector. */
/*          If DIRECT = 'F', T is upper triangular; if DIRECT = 'B', T is */
/*          lower triangular. The rest of the array is not used. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T. LDT >= K. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA */

/*  The shape of the matrix V and the storage of the vectors which define */
/*  the H(i) is best illustrated by the following example with n = 5 and */
/*  k = 3. The elements equal to 1 are not stored; the corresponding */
/*  array elements are modified but restored on exit. The rest of the */
/*  array is not used. */

/*  DIRECT = 'F' and STOREV = 'C':         DIRECT = 'F' and STOREV = 'R': */

/*                                              ______V_____ */
/*         ( v1 v2 v3 )                        /            \ */
/*         ( v1 v2 v3 )                      ( v1 v1 v1 v1 v1 . . . . 1 ) */
/*     V = ( v1 v2 v3 )                      ( v2 v2 v2 v2 v2 . . . 1   ) */
/*         ( v1 v2 v3 )                      ( v3 v3 v3 v3 v3 . . 1     ) */
/*         ( v1 v2 v3 ) */
/*            .  .  . */
/*            .  .  . */
/*            1  .  . */
/*               1  . */
/*                  1 */

/*  DIRECT = 'B' and STOREV = 'C':         DIRECT = 'B' and STOREV = 'R': */

/*                                                        ______V_____ */
/*            1                                          /            \ */
/*            .  1                           ( 1 . . . . v1 v1 v1 v1 v1 ) */
/*            .  .  1                        ( . 1 . . . v2 v2 v2 v2 v2 ) */
/*            .  .  .                        ( . . 1 . . v3 v3 v3 v3 v3 ) */
/*            .  .  . */
/*         ( v1 v2 v3 ) */
/*         ( v1 v2 v3 ) */
/*     V = ( v1 v2 v3 ) */
/*         ( v1 v2 v3 ) */
/*         ( v1 v2 v3 ) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Check for currently supported options */

    /* Parameter adjustments */
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    --tau;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;

    /* Function Body */
    info = 0;
    if (! lsame_(direct, "B")) {
	info = -1;
    } else if (! lsame_(storev, "R")) {
	info = -2;
    }
    if (info != 0) {
	i__1 = -info;
	xerbla_("DLARZT", &i__1);
	return 0;
    }

    for (i__ = *k; i__ >= 1; --i__) {
	if (tau[i__] == 0.) {

/*           H(i)  =  I */

	    i__1 = *k;
	    for (j = i__; j <= i__1; ++j) {
		t[j + i__ * t_dim1] = 0.;
/* L10: */
	    }
	} else {

/*           general case */

	    if (i__ < *k) {

/*              T(i+1:k,i) = - tau(i) * V(i+1:k,1:n) * V(i,1:n)' */

		i__1 = *k - i__;
		d__1 = -tau[i__];
		dgemv_("No transpose", &i__1, n, &d__1, &v[i__ + 1 + v_dim1],
			ldv, &v[i__ + v_dim1], ldv, &c_b8, &t[i__ + 1 + i__ *
			t_dim1], &c__1);

/*              T(i+1:k,i) = T(i+1:k,i+1:k) * T(i+1:k,i) */

		i__1 = *k - i__;
		dtrmv_("Lower", "No transpose", "Non-unit", &i__1, &t[i__ + 1
			+ (i__ + 1) * t_dim1], ldt, &t[i__ + 1 + i__ * t_dim1]
, &c__1);
	    }
	    t[i__ + i__ * t_dim1] = tau[i__];
	}
/* L20: */
    }
    return 0;

/*     End of DLARZT */

} /* dlarzt_ */

/* Subroutine */ int dlas2_(double *f, double *g, double *h__,
	double *ssmin, double *ssmax)
{
    /* System generated locals */
    double d__1, d__2;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    double c__, fa, ga, ha, as, at, au, fhmn, fhmx;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAS2  computes the singular values of the 2-by-2 matrix */
/*     [  F   G  ] */
/*     [  0   H  ]. */
/*  On return, SSMIN is the smaller singular value and SSMAX is the */
/*  larger singular value. */

/*  Arguments */
/*  ========= */

/*  F       (input) DOUBLE PRECISION */
/*          The (1,1) element of the 2-by-2 matrix. */

/*  G       (input) DOUBLE PRECISION */
/*          The (1,2) element of the 2-by-2 matrix. */

/*  H       (input) DOUBLE PRECISION */
/*          The (2,2) element of the 2-by-2 matrix. */

/*  SSMIN   (output) DOUBLE PRECISION */
/*          The smaller singular value. */

/*  SSMAX   (output) DOUBLE PRECISION */
/*          The larger singular value. */

/*  Further Details */
/*  =============== */

/*  Barring over/underflow, all output quantities are correct to within */
/*  a few units in the last place (ulps), even in the absence of a guard */
/*  digit in addition/subtraction. */

/*  In IEEE arithmetic, the code works correctly if one matrix element is */
/*  infinite. */

/*  Overflow will not occur unless the largest singular value itself */
/*  overflows, or is within a few ulps of overflow. (On machines with */
/*  partial overflow, like the Cray, overflow may occur if the largest */
/*  singular value is within a factor of 2 of overflow.) */

/*  Underflow is harmless if underflow is gradual. Otherwise, results */
/*  may correspond to a matrix modified by perturbations of size near */
/*  the underflow threshold. */

/*  ==================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    fa = abs(*f);
    ga = abs(*g);
    ha = abs(*h__);
    fhmn = std::min(fa,ha);
    fhmx = std::max(fa,ha);
    if (fhmn == 0.) {
	*ssmin = 0.;
	if (fhmx == 0.) {
	    *ssmax = ga;
	} else {
/* Computing 2nd power */
	    d__1 = std::min(fhmx,ga) / std::max(fhmx,ga);
	    *ssmax = std::max(fhmx,ga) * sqrt(d__1 * d__1 + 1.);
	}
    } else {
	if (ga < fhmx) {
	    as = fhmn / fhmx + 1.;
	    at = (fhmx - fhmn) / fhmx;
/* Computing 2nd power */
	    d__1 = ga / fhmx;
	    au = d__1 * d__1;
	    c__ = 2. / (sqrt(as * as + au) + sqrt(at * at + au));
	    *ssmin = fhmn * c__;
	    *ssmax = fhmx / c__;
	} else {
	    au = fhmx / ga;
	    if (au == 0.) {

/*              Avoid possible harmful underflow if exponent range */
/*              asymmetric (true SSMIN may not underflow even if */
/*              AU underflows) */

		*ssmin = fhmn * fhmx / ga;
		*ssmax = ga;
	    } else {
		as = fhmn / fhmx + 1.;
		at = (fhmx - fhmn) / fhmx;
/* Computing 2nd power */
		d__1 = as * au;
/* Computing 2nd power */
		d__2 = at * au;
		c__ = 1. / (sqrt(d__1 * d__1 + 1.) + sqrt(d__2 * d__2 + 1.));
		*ssmin = fhmn * c__ * au;
		*ssmin += *ssmin;
		*ssmax = ga / (c__ + c__);
	    }
	}
    }
    return 0;

/*     End of DLAS2 */

} /* dlas2_ */

/* Subroutine */ int dlascl_(const char *type__, integer *kl, integer *ku, double *cfrom, double *cto,
	integer *m,	integer *n, double *a, integer *lda, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;

    /* Local variables */
    integer i__, j, k1, k2, k3, k4;
    double mul, cto1;
    bool done;
    double ctoc;
    integer itype;
    double cfrom1;
    double cfromc;
    double bignum, smlnum;


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASCL multiplies the M by N real matrix A by the real scalar */
/*  CTO/CFROM.  This is done without over/underflow as long as the final */
/*  result CTO*A(I,J)/CFROM does not over/underflow. TYPE specifies that */
/*  A may be full, upper triangular, lower triangular, upper Hessenberg, */
/*  or banded. */

/*  Arguments */
/*  ========= */

/*  TYPE    (input) CHARACTER*1 */
/*          TYPE indices the storage type of the input matrix. */
/*          = 'G':  A is a full matrix. */
/*          = 'L':  A is a lower triangular matrix. */
/*          = 'U':  A is an upper triangular matrix. */
/*          = 'H':  A is an upper Hessenberg matrix. */
/*          = 'B':  A is a symmetric band matrix with lower bandwidth KL */
/*                  and upper bandwidth KU and with the only the lower */
/*                  half stored. */
/*          = 'Q':  A is a symmetric band matrix with lower bandwidth KL */
/*                  and upper bandwidth KU and with the only the upper */
/*                  half stored. */
/*          = 'Z':  A is a band matrix with lower bandwidth KL and upper */
/*                  bandwidth KU. */

/*  KL      (input) INTEGER */
/*          The lower bandwidth of A.  Referenced only if TYPE = 'B', */
/*          'Q' or 'Z'. */

/*  KU      (input) INTEGER */
/*          The upper bandwidth of A.  Referenced only if TYPE = 'B', */
/*          'Q' or 'Z'. */

/*  CFROM   (input) DOUBLE PRECISION */
/*  CTO     (input) DOUBLE PRECISION */
/*          The matrix A is multiplied by CTO/CFROM. A(I,J) is computed */
/*          without over/underflow if the final result CTO*A(I,J)/CFROM */
/*          can be represented without over/underflow.  CFROM must be */
/*          nonzero. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The matrix to be multiplied by CTO/CFROM.  See TYPE for the */
/*          storage type. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  INFO    (output) INTEGER */
/*          0  - successful exit */
/*          <0 - if INFO = -i, the i-th argument had an illegal value. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    *info = 0;

    if (lsame_(type__, "G")) {
	itype = 0;
    } else if (lsame_(type__, "L")) {
	itype = 1;
    } else if (lsame_(type__, "U")) {
	itype = 2;
    } else if (lsame_(type__, "H")) {
	itype = 3;
    } else if (lsame_(type__, "B")) {
	itype = 4;
    } else if (lsame_(type__, "Q")) {
	itype = 5;
    } else if (lsame_(type__, "Z")) {
	itype = 6;
    } else {
	itype = -1;
    }

    if (itype == -1) {
	*info = -1;
    } else if (*cfrom == 0. || disnan_(cfrom)) {
	*info = -4;
    } else if (disnan_(cto)) {
	*info = -5;
    } else if (*m < 0) {
	*info = -6;
    } else if (*n < 0 || itype == 4 && *n != *m || itype == 5 && *n != *m) {
	*info = -7;
    } else if (itype <= 3 && *lda <  std::max(1_integer,*m)) {
	*info = -9;
    } else if (itype >= 4) {
/* Computing MAX */
	i__1 = *m - 1;
	if (*kl < 0 || *kl >  std::max(i__1,0_integer)) {
	    *info = -2;
	} else /* if(complicated condition) */ {
/* Computing MAX */
	    i__1 = *n - 1;
	    if (*ku < 0 || *ku >  std::max(i__1,0_integer) || (itype == 4 || itype == 5) &&
		    *kl != *ku) {
		*info = -3;
	    } else if (itype == 4 && *lda < *kl + 1 || itype == 5 && *lda < *
		    ku + 1 || itype == 6 && *lda < (*kl << 1) + *ku + 1) {
		*info = -9;
	    }
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASCL", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *m == 0) {
	return 0;
    }

/*     Get machine parameters */

    smlnum = dlamch_("S");
    bignum = 1. / smlnum;

    cfromc = *cfrom;
    ctoc = *cto;

L10:
    cfrom1 = cfromc * smlnum;
    if (cfrom1 == cfromc) {
/*        CFROMC is an inf.  Multiply by a correctly signed zero for */
/*        finite CTOC, or a NaN if CTOC is infinite. */
	mul = ctoc / cfromc;
	done = true;
	cto1 = ctoc;
    } else {
	cto1 = ctoc / bignum;
	if (cto1 == ctoc) {
/*           CTOC is either 0 or an inf.  In both cases, CTOC itself */
/*           serves as the correct multiplication factor. */
	    mul = ctoc;
	    done = true;
	    cfromc = 1.;
	} else if (abs(cfrom1) > abs(ctoc) && ctoc != 0.) {
	    mul = smlnum;
	    done = false;
	    cfromc = cfrom1;
	} else if (abs(cto1) > abs(cfromc)) {
	    mul = bignum;
	    done = false;
	    ctoc = cto1;
	} else {
	    mul = ctoc / cfromc;
	    done = true;
	}
    }

    if (itype == 0) {

/*        Full matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L20: */
	    }
/* L30: */
	}

    } else if (itype == 1) {

/*        Lower triangular matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = j; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L40: */
	    }
/* L50: */
	}

    } else if (itype == 2) {

/*        Upper triangular matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = std::min(j,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L60: */
	    }
/* L70: */
	}

    } else if (itype == 3) {

/*        Upper Hessenberg matrix */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = j + 1;
	    i__2 = std::min(i__3,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L80: */
	    }
/* L90: */
	}

    } else if (itype == 4) {

/*        Lower half of a symmetric band matrix */

	k3 = *kl + 1;
	k4 = *n + 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = k3, i__4 = k4 - j;
	    i__2 = std::min(i__3,i__4);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L100: */
	    }
/* L110: */
	}

    } else if (itype == 5) {

/*        Upper half of a symmetric band matrix */

	k1 = *ku + 2;
	k3 = *ku + 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    i__2 = k1 - j;
	    i__3 = k3;
	    for (i__ = std::max(i__2,1_integer); i__ <= i__3; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L120: */
	    }
/* L130: */
	}

    } else if (itype == 6) {

/*        Band matrix */

	k1 = *kl + *ku + 2;
	k2 = *kl + 1;
	k3 = (*kl << 1) + *ku + 1;
	k4 = *kl + *ku + 1 + *m;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    i__3 = k1 - j;
/* Computing MIN */
	    i__4 = k3, i__5 = k4 - j;
	    i__2 = std::min(i__4,i__5);
	    for (i__ = std::max(i__3,k2); i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] *= mul;
/* L140: */
	    }
/* L150: */
	}

    }

    if (! done) {
	goto L10;
    }

    return 0;

/*     End of DLASCL */

} /* dlascl_ */

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

/* Subroutine */ int dlasd0_(integer *n, integer *sqre, double *d__,
	double *e, double *u, integer *ldu, double *vt, integer *
	ldvt, integer *smlsiz, integer *iwork, double *work, integer *
	info)
{
	/* Table of constant values */
	static integer c__0 = 0;
	static integer c__2 = 2;

    /* System generated locals */
    integer u_dim1, u_offset, vt_dim1, vt_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, m, i1, ic, lf, nd, ll, nl, nr, im1, ncc, nlf, nrf, iwk,
	    lvl, ndb1, nlp1, nrp1;
    double beta;
    integer idxq, nlvl;
    double alpha;
    integer inode, ndiml, idxqc, ndimr, itemp, sqrei;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Using a divide and conquer approach, DLASD0 computes the singular */
/*  value decomposition (SVD) of a real upper bidiagonal N-by-M */
/*  matrix B with diagonal D and offdiagonal E, where M = N + SQRE. */
/*  The algorithm computes orthogonal matrices U and VT such that */
/*  B = U * S * VT. The singular values S are overwritten on D. */

/*  A related subroutine, DLASDA, computes only the singular values, */
/*  and optionally, the singular vectors in compact form. */

/*  Arguments */
/*  ========= */

/*  N      (input) INTEGER */
/*         On entry, the row dimension of the upper bidiagonal matrix. */
/*         This is also the dimension of the main diagonal array D. */

/*  SQRE   (input) INTEGER */
/*         Specifies the column dimension of the bidiagonal matrix. */
/*         = 0: The bidiagonal matrix has column dimension M = N; */
/*         = 1: The bidiagonal matrix has column dimension M = N+1; */

/*  D      (input/output) DOUBLE PRECISION array, dimension (N) */
/*         On entry D contains the main diagonal of the bidiagonal */
/*         matrix. */
/*         On exit D, if INFO = 0, contains its singular values. */

/*  E      (input) DOUBLE PRECISION array, dimension (M-1) */
/*         Contains the subdiagonal entries of the bidiagonal matrix. */
/*         On exit, E has been destroyed. */

/*  U      (output) DOUBLE PRECISION array, dimension at least (LDQ, N) */
/*         On exit, U contains the left singular vectors. */

/*  LDU    (input) INTEGER */
/*         On entry, leading dimension of U. */

/*  VT     (output) DOUBLE PRECISION array, dimension at least (LDVT, M) */
/*         On exit, VT' contains the right singular vectors. */

/*  LDVT   (input) INTEGER */
/*         On entry, leading dimension of VT. */

/*  SMLSIZ (input) INTEGER */
/*         On entry, maximum size of the subproblems at the */
/*         bottom of the computation tree. */

/*  IWORK  (workspace) INTEGER work array. */
/*         Dimension must be at least (8 * N) */

/*  WORK   (workspace) DOUBLE PRECISION work array. */
/*         Dimension must be at least (3 * M**2 + 2 * M) */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, an singular value did not converge */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --d__;
    --e;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    --iwork;
    --work;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -1;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -2;
    }

    m = *n + *sqre;

    if (*ldu < *n) {
	*info = -6;
    } else if (*ldvt < m) {
	*info = -8;
    } else if (*smlsiz < 3) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD0", &i__1);
	return 0;
    }

/*     If the input matrix is too small, call DLASDQ to find the SVD. */

    if (*n <= *smlsiz) {
	dlasdq_("U", sqre, n, &m, n, &c__0, &d__[1], &e[1], &vt[vt_offset],
		ldvt, &u[u_offset], ldu, &u[u_offset], ldu, &work[1], info);
	return 0;
    }

/*     Set up the computation tree. */

    inode = 1;
    ndiml = inode + *n;
    ndimr = ndiml + *n;
    idxq = ndimr + *n;
    iwk = idxq + *n;
    dlasdt_(n, &nlvl, &nd, &iwork[inode], &iwork[ndiml], &iwork[ndimr],
	    smlsiz);

/*     For the nodes on bottom level of the tree, solve */
/*     their subproblems by DLASDQ. */

    ndb1 = (nd + 1) / 2;
    ncc = 0;
    i__1 = nd;
    for (i__ = ndb1; i__ <= i__1; ++i__) {

/*     IC : center row of each node */
/*     NL : number of rows of left  subproblem */
/*     NR : number of rows of right subproblem */
/*     NLF: starting row of the left   subproblem */
/*     NRF: starting row of the right  subproblem */

	i1 = i__ - 1;
	ic = iwork[inode + i1];
	nl = iwork[ndiml + i1];
	nlp1 = nl + 1;
	nr = iwork[ndimr + i1];
	nrp1 = nr + 1;
	nlf = ic - nl;
	nrf = ic + 1;
	sqrei = 1;
	dlasdq_("U", &sqrei, &nl, &nlp1, &nl, &ncc, &d__[nlf], &e[nlf], &vt[
		nlf + nlf * vt_dim1], ldvt, &u[nlf + nlf * u_dim1], ldu, &u[
		nlf + nlf * u_dim1], ldu, &work[1], info);
	if (*info != 0) {
	    return 0;
	}
	itemp = idxq + nlf - 2;
	i__2 = nl;
	for (j = 1; j <= i__2; ++j) {
	    iwork[itemp + j] = j;
/* L10: */
	}
	if (i__ == nd) {
	    sqrei = *sqre;
	} else {
	    sqrei = 1;
	}
	nrp1 = nr + sqrei;
	dlasdq_("U", &sqrei, &nr, &nrp1, &nr, &ncc, &d__[nrf], &e[nrf], &vt[
		nrf + nrf * vt_dim1], ldvt, &u[nrf + nrf * u_dim1], ldu, &u[
		nrf + nrf * u_dim1], ldu, &work[1], info);
	if (*info != 0) {
	    return 0;
	}
	itemp = idxq + ic;
	i__2 = nr;
	for (j = 1; j <= i__2; ++j) {
	    iwork[itemp + j - 1] = j;
/* L20: */
	}
/* L30: */
    }

/*     Now conquer each subproblem bottom-up. */

    for (lvl = nlvl; lvl >= 1; --lvl) {

/*        Find the first node LF and last node LL on the */
/*        current level LVL. */

	if (lvl == 1) {
	    lf = 1;
	    ll = 1;
	} else {
	    i__1 = lvl - 1;
	    lf = pow_ii(&c__2, &i__1);
	    ll = (lf << 1) - 1;
	}
	i__1 = ll;
	for (i__ = lf; i__ <= i__1; ++i__) {
	    im1 = i__ - 1;
	    ic = iwork[inode + im1];
	    nl = iwork[ndiml + im1];
	    nr = iwork[ndimr + im1];
	    nlf = ic - nl;
	    if (*sqre == 0 && i__ == ll) {
		sqrei = *sqre;
	    } else {
		sqrei = 1;
	    }
	    idxqc = idxq + nlf - 1;
	    alpha = d__[ic];
	    beta = e[ic];
	    dlasd1_(&nl, &nr, &sqrei, &d__[nlf], &alpha, &beta, &u[nlf + nlf *
		     u_dim1], ldu, &vt[nlf + nlf * vt_dim1], ldvt, &iwork[
		    idxqc], &iwork[iwk], &work[1], info);
	    if (*info != 0) {
		return 0;
	    }
/* L40: */
	}
/* L50: */
    }

    return 0;

/*     End of DLASD0 */

} /* dlasd0_ */

/* Subroutine */ int dlasd1_(integer *nl, integer *nr, integer *sqre,
	double *d__, double *alpha, double *beta, double *u,
	integer *ldu, double *vt, integer *ldvt, integer *idxq, integer *
	iwork, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__0 = 0;
	static double c_b7 = 1.;
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer u_dim1, u_offset, vt_dim1, vt_offset, i__1;
    double d__1, d__2;

    /* Local variables */
    integer i__, k, m, n, n1, n2, iq, iz, iu2, ldq, idx, ldu2, ivt2, idxc,
	    idxp, ldvt2;
    integer isigma;
    double orgnrm;
    integer coltyp;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASD1 computes the SVD of an upper bidiagonal N-by-M matrix B, */
/*  where N = NL + NR + 1 and M = N + SQRE. DLASD1 is called from DLASD0. */

/*  A related subroutine DLASD7 handles the case in which the singular */
/*  values (and the singular vectors in factored form) are desired. */

/*  DLASD1 computes the SVD as follows: */

/*                ( D1(in)  0    0     0 ) */
/*    B = U(in) * (   Z1'   a   Z2'    b ) * VT(in) */
/*                (   0     0   D2(in) 0 ) */

/*      = U(out) * ( D(out) 0) * VT(out) */

/*  where Z' = (Z1' a Z2' b) = u' VT', and u is a vector of dimension M */
/*  with ALPHA and BETA in the NL+1 and NL+2 th entries and zeros */
/*  elsewhere; and the entry b is empty if SQRE = 0. */

/*  The left singular vectors of the original matrix are stored in U, and */
/*  the transpose of the right singular vectors are stored in VT, and the */
/*  singular values are in D.  The algorithm consists of three stages: */

/*     The first stage consists of deflating the size of the problem */
/*     when there are multiple singular values or when there are zeros in */
/*     the Z vector.  For each such occurrence the dimension of the */
/*     secular equation problem is reduced by one.  This stage is */
/*     performed by the routine DLASD2. */

/*     The second stage consists of calculating the updated */
/*     singular values. This is done by finding the square roots of the */
/*     roots of the secular equation via the routine DLASD4 (as called */
/*     by DLASD3). This routine also calculates the singular vectors of */
/*     the current problem. */

/*     The final stage consists of computing the updated singular vectors */
/*     directly using the updated singular values.  The singular vectors */
/*     for the current problem are multiplied with the singular vectors */
/*     from the overall problem. */

/*  Arguments */
/*  ========= */

/*  NL     (input) INTEGER */
/*         The row dimension of the upper block.  NL >= 1. */

/*  NR     (input) INTEGER */
/*         The row dimension of the lower block.  NR >= 1. */

/*  SQRE   (input) INTEGER */
/*         = 0: the lower block is an NR-by-NR square matrix. */
/*         = 1: the lower block is an NR-by-(NR+1) rectangular matrix. */

/*         The bidiagonal matrix has row dimension N = NL + NR + 1, */
/*         and column dimension M = N + SQRE. */

/*  D      (input/output) DOUBLE PRECISION array, */
/*                        dimension (N = NL+NR+1). */
/*         On entry D(1:NL,1:NL) contains the singular values of the */
/*         upper block; and D(NL+2:N) contains the singular values of */
/*         the lower block. On exit D(1:N) contains the singular values */
/*         of the modified matrix. */

/*  ALPHA  (input/output) DOUBLE PRECISION */
/*         Contains the diagonal element associated with the added row. */

/*  BETA   (input/output) DOUBLE PRECISION */
/*         Contains the off-diagonal element associated with the added */
/*         row. */

/*  U      (input/output) DOUBLE PRECISION array, dimension(LDU,N) */
/*         On entry U(1:NL, 1:NL) contains the left singular vectors of */
/*         the upper block; U(NL+2:N, NL+2:N) contains the left singular */
/*         vectors of the lower block. On exit U contains the left */
/*         singular vectors of the bidiagonal matrix. */

/*  LDU    (input) INTEGER */
/*         The leading dimension of the array U.  LDU >= max( 1, N ). */

/*  VT     (input/output) DOUBLE PRECISION array, dimension(LDVT,M) */
/*         where M = N + SQRE. */
/*         On entry VT(1:NL+1, 1:NL+1)' contains the right singular */
/*         vectors of the upper block; VT(NL+2:M, NL+2:M)' contains */
/*         the right singular vectors of the lower block. On exit */
/*         VT' contains the right singular vectors of the */
/*         bidiagonal matrix. */

/*  LDVT   (input) INTEGER */
/*         The leading dimension of the array VT.  LDVT >= max( 1, M ). */

/*  IDXQ  (output) INTEGER array, dimension(N) */
/*         This contains the permutation which will reintegrate the */
/*         subproblem just solved back into sorted order, i.e. */
/*         D( IDXQ( I = 1, N ) ) will be in ascending order. */

/*  IWORK  (workspace) INTEGER array, dimension( 4 * N ) */

/*  WORK   (workspace) DOUBLE PRECISION array, dimension( 3*M**2 + 2*M ) */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, an singular value did not converge */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */

/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --d__;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    --idxq;
    --iwork;
    --work;

    /* Function Body */
    *info = 0;

    if (*nl < 1) {
	*info = -1;
    } else if (*nr < 1) {
	*info = -2;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -3;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD1", &i__1);
	return 0;
    }

    n = *nl + *nr + 1;
    m = n + *sqre;

/*     The following values are for bookkeeping purposes only.  They are */
/*     integer pointers which indicate the portion of the workspace */
/*     used by a particular array in DLASD2 and DLASD3. */

    ldu2 = n;
    ldvt2 = m;

    iz = 1;
    isigma = iz + m;
    iu2 = isigma + n;
    ivt2 = iu2 + ldu2 * n;
    iq = ivt2 + ldvt2 * m;

    idx = 1;
    idxc = idx + n;
    coltyp = idxc + n;
    idxp = coltyp + n;

/*     Scale. */

/* Computing MAX */
    d__1 = abs(*alpha), d__2 = abs(*beta);
    orgnrm = std::max(d__1,d__2);
    d__[*nl + 1] = 0.;
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = d__[i__], abs(d__1)) > orgnrm) {
	    orgnrm = (d__1 = d__[i__], abs(d__1));
	}
/* L10: */
    }
    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b7, &n, &c__1, &d__[1], &n, info);
    *alpha /= orgnrm;
    *beta /= orgnrm;

/*     Deflate singular values. */

    dlasd2_(nl, nr, sqre, &k, &d__[1], &work[iz], alpha, beta, &u[u_offset],
	    ldu, &vt[vt_offset], ldvt, &work[isigma], &work[iu2], &ldu2, &
	    work[ivt2], &ldvt2, &iwork[idxp], &iwork[idx], &iwork[idxc], &
	    idxq[1], &iwork[coltyp], info);

/*     Solve Secular Equation and update singular vectors. */

    ldq = k;
    dlasd3_(nl, nr, sqre, &k, &d__[1], &work[iq], &ldq, &work[isigma], &u[
	    u_offset], ldu, &work[iu2], &ldu2, &vt[vt_offset], ldvt, &work[
	    ivt2], &ldvt2, &iwork[idxc], &iwork[coltyp], &work[iz], info);
    if (*info != 0) {
	return 0;
    }

/*     Unscale. */

    dlascl_("G", &c__0, &c__0, &c_b7, &orgnrm, &n, &c__1, &d__[1], &n, info);

/*     Prepare the IDXQ sorting permutation. */

    n1 = k;
    n2 = n - k;
    dlamrg_(&n1, &n2, &d__[1], &c__1, &c_n1, &idxq[1]);

    return 0;

/*     End of DLASD1 */

} /* dlasd1_ */

/* Subroutine */ int dlasd2_(integer *nl, integer *nr, integer *sqre, integer
	*k, double *d__, double *z__, double *alpha, double *
	beta, double *u, integer *ldu, double *vt, integer *ldvt,
	double *dsigma, double *u2, integer *ldu2, double *vt2,
	integer *ldvt2, integer *idxp, integer *idx, integer *idxc, integer *
	idxq, integer *coltyp, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b30 = 0.;

    /* System generated locals */
    integer u_dim1, u_offset, u2_dim1, u2_offset, vt_dim1, vt_offset,
	    vt2_dim1, vt2_offset, i__1;
    double d__1, d__2;

    /* Local variables */
    double c__;
    integer i__, j, m, n;
    double s;
    integer k2;
    double z1;
    integer ct, jp;
    double eps, tau, tol;
    integer psm[4], nlp1, nlp2, idxi, idxj;
    integer ctot[4], idxjp;
    integer jprev;
    double hlftol;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASD2 merges the two sets of singular values together into a single */
/*  sorted set.  Then it tries to deflate the size of the problem. */
/*  There are two ways in which deflation can occur:  when two or more */
/*  singular values are close together or if there is a tiny entry in the */
/*  Z vector.  For each such occurrence the order of the related secular */
/*  equation problem is reduced by one. */

/*  DLASD2 is called from DLASD1. */

/*  Arguments */
/*  ========= */

/*  NL     (input) INTEGER */
/*         The row dimension of the upper block.  NL >= 1. */

/*  NR     (input) INTEGER */
/*         The row dimension of the lower block.  NR >= 1. */

/*  SQRE   (input) INTEGER */
/*         = 0: the lower block is an NR-by-NR square matrix. */
/*         = 1: the lower block is an NR-by-(NR+1) rectangular matrix. */

/*         The bidiagonal matrix has N = NL + NR + 1 rows and */
/*         M = N + SQRE >= N columns. */

/*  K      (output) INTEGER */
/*         Contains the dimension of the non-deflated matrix, */
/*         This is the order of the related secular equation. 1 <= K <=N. */

/*  D      (input/output) DOUBLE PRECISION array, dimension(N) */
/*         On entry D contains the singular values of the two submatrices */
/*         to be combined.  On exit D contains the trailing (N-K) updated */
/*         singular values (those which were deflated) sorted into */
/*         increasing order. */

/*  Z      (output) DOUBLE PRECISION array, dimension(N) */
/*         On exit Z contains the updating row vector in the secular */
/*         equation. */

/*  ALPHA  (input) DOUBLE PRECISION */
/*         Contains the diagonal element associated with the added row. */

/*  BETA   (input) DOUBLE PRECISION */
/*         Contains the off-diagonal element associated with the added */
/*         row. */

/*  U      (input/output) DOUBLE PRECISION array, dimension(LDU,N) */
/*         On entry U contains the left singular vectors of two */
/*         submatrices in the two square blocks with corners at (1,1), */
/*         (NL, NL), and (NL+2, NL+2), (N,N). */
/*         On exit U contains the trailing (N-K) updated left singular */
/*         vectors (those which were deflated) in its last N-K columns. */

/*  LDU    (input) INTEGER */
/*         The leading dimension of the array U.  LDU >= N. */

/*  VT     (input/output) DOUBLE PRECISION array, dimension(LDVT,M) */
/*         On entry VT' contains the right singular vectors of two */
/*         submatrices in the two square blocks with corners at (1,1), */
/*         (NL+1, NL+1), and (NL+2, NL+2), (M,M). */
/*         On exit VT' contains the trailing (N-K) updated right singular */
/*         vectors (those which were deflated) in its last N-K columns. */
/*         In case SQRE =1, the last row of VT spans the right null */
/*         space. */

/*  LDVT   (input) INTEGER */
/*         The leading dimension of the array VT.  LDVT >= M. */

/*  DSIGMA (output) DOUBLE PRECISION array, dimension (N) */
/*         Contains a copy of the diagonal elements (K-1 singular values */
/*         and one zero) in the secular equation. */

/*  U2     (output) DOUBLE PRECISION array, dimension(LDU2,N) */
/*         Contains a copy of the first K-1 left singular vectors which */
/*         will be used by DLASD3 in a matrix multiply (DGEMM) to solve */
/*         for the new left singular vectors. U2 is arranged into four */
/*         blocks. The first block contains a column with 1 at NL+1 and */
/*         zero everywhere else; the second block contains non-zero */
/*         entries only at and above NL; the third contains non-zero */
/*         entries only below NL+1; and the fourth is dense. */

/*  LDU2   (input) INTEGER */
/*         The leading dimension of the array U2.  LDU2 >= N. */

/*  VT2    (output) DOUBLE PRECISION array, dimension(LDVT2,N) */
/*         VT2' contains a copy of the first K right singular vectors */
/*         which will be used by DLASD3 in a matrix multiply (DGEMM) to */
/*         solve for the new right singular vectors. VT2 is arranged into */
/*         three blocks. The first block contains a row that corresponds */
/*         to the special 0 diagonal element in SIGMA; the second block */
/*         contains non-zeros only at and before NL +1; the third block */
/*         contains non-zeros only at and after  NL +2. */

/*  LDVT2  (input) INTEGER */
/*         The leading dimension of the array VT2.  LDVT2 >= M. */

/*  IDXP   (workspace) INTEGER array dimension(N) */
/*         This will contain the permutation used to place deflated */
/*         values of D at the end of the array. On output IDXP(2:K) */
/*         points to the nondeflated D-values and IDXP(K+1:N) */
/*         points to the deflated singular values. */

/*  IDX    (workspace) INTEGER array dimension(N) */
/*         This will contain the permutation used to sort the contents of */
/*         D into ascending order. */

/*  IDXC   (output) INTEGER array dimension(N) */
/*         This will contain the permutation used to arrange the columns */
/*         of the deflated U matrix into three groups:  the first group */
/*         contains non-zero entries only at and above NL, the second */
/*         contains non-zero entries only below NL+2, and the third is */
/*         dense. */

/*  IDXQ   (input/output) INTEGER array dimension(N) */
/*         This contains the permutation which separately sorts the two */
/*         sub-problems in D into ascending order.  Note that entries in */
/*         the first hlaf of this permutation must first be moved one */
/*         position backward; and entries in the second half */
/*         must first have NL+1 added to their values. */

/*  COLTYP (workspace/output) INTEGER array dimension(N) */
/*         As workspace, this will contain a label which will indicate */
/*         which of the following types a column in the U2 matrix or a */
/*         row in the VT2 matrix is: */
/*         1 : non-zero in the upper half only */
/*         2 : non-zero in the lower half only */
/*         3 : dense */
/*         4 : deflated */

/*         On exit, it is an array of dimension 4, with COLTYP(I) being */
/*         the dimension of the I-th type columns. */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --d__;
    --z__;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    --dsigma;
    u2_dim1 = *ldu2;
    u2_offset = 1 + u2_dim1;
    u2 -= u2_offset;
    vt2_dim1 = *ldvt2;
    vt2_offset = 1 + vt2_dim1;
    vt2 -= vt2_offset;
    --idxp;
    --idx;
    --idxc;
    --idxq;
    --coltyp;

    /* Function Body */
    *info = 0;

    if (*nl < 1) {
	*info = -1;
    } else if (*nr < 1) {
	*info = -2;
    } else if (*sqre != 1 && *sqre != 0) {
	*info = -3;
    }

    n = *nl + *nr + 1;
    m = n + *sqre;

    if (*ldu < n) {
	*info = -10;
    } else if (*ldvt < m) {
	*info = -12;
    } else if (*ldu2 < n) {
	*info = -15;
    } else if (*ldvt2 < m) {
	*info = -17;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD2", &i__1);
	return 0;
    }

    nlp1 = *nl + 1;
    nlp2 = *nl + 2;

/*     Generate the first part of the vector Z; and move the singular */
/*     values in the first part of D one position backward. */

    z1 = *alpha * vt[nlp1 + nlp1 * vt_dim1];
    z__[1] = z1;
    for (i__ = *nl; i__ >= 1; --i__) {
	z__[i__ + 1] = *alpha * vt[i__ + nlp1 * vt_dim1];
	d__[i__ + 1] = d__[i__];
	idxq[i__ + 1] = idxq[i__] + 1;
/* L10: */
    }

/*     Generate the second part of the vector Z. */

    i__1 = m;
    for (i__ = nlp2; i__ <= i__1; ++i__) {
	z__[i__] = *beta * vt[i__ + nlp2 * vt_dim1];
/* L20: */
    }

/*     Initialize some reference arrays. */

    i__1 = nlp1;
    for (i__ = 2; i__ <= i__1; ++i__) {
	coltyp[i__] = 1;
/* L30: */
    }
    i__1 = n;
    for (i__ = nlp2; i__ <= i__1; ++i__) {
	coltyp[i__] = 2;
/* L40: */
    }

/*     Sort the singular values into increasing order */

    i__1 = n;
    for (i__ = nlp2; i__ <= i__1; ++i__) {
	idxq[i__] += nlp1;
/* L50: */
    }

/*     DSIGMA, IDXC, IDXC, and the first column of U2 */
/*     are used as storage space. */

    i__1 = n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	dsigma[i__] = d__[idxq[i__]];
	u2[i__ + u2_dim1] = z__[idxq[i__]];
	idxc[i__] = coltyp[idxq[i__]];
/* L60: */
    }

    dlamrg_(nl, nr, &dsigma[2], &c__1, &c__1, &idx[2]);

    i__1 = n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	idxi = idx[i__] + 1;
	d__[i__] = dsigma[idxi];
	z__[i__] = u2[idxi + u2_dim1];
	coltyp[i__] = idxc[idxi];
/* L70: */
    }

/*     Calculate the allowable deflation tolerance */

    eps = dlamch_("Epsilon");
/* Computing MAX */
    d__1 = abs(*alpha), d__2 = abs(*beta);
    tol = std::max(d__1,d__2);
/* Computing MAX */
    d__2 = (d__1 = d__[n], abs(d__1));
    tol = eps * 8. * std::max(d__2,tol);

/*     There are 2 kinds of deflation -- first a value in the z-vector */
/*     is small, second two (or more) singular values are very close */
/*     together (their difference is small). */

/*     If the value in the z-vector is small, we simply permute the */
/*     array so that the corresponding singular value is moved to the */
/*     end. */

/*     If two values in the D-vector are close, we perform a two-sided */
/*     rotation designed to make one of the corresponding z-vector */
/*     entries zero, and then permute the array so that the deflated */
/*     singular value is moved to the end. */

/*     If there are multiple singular values then the problem deflates. */
/*     Here the number of equal singular values are found.  As each equal */
/*     singular value is found, an elementary reflector is computed to */
/*     rotate the corresponding singular subspace so that the */
/*     corresponding components of Z are zero in this new basis. */

    *k = 1;
    k2 = n + 1;
    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	if ((d__1 = z__[j], abs(d__1)) <= tol) {

/*           Deflate due to small z component. */

	    --k2;
	    idxp[k2] = j;
	    coltyp[j] = 4;
	    if (j == n) {
		goto L120;
	    }
	} else {
	    jprev = j;
	    goto L90;
	}
/* L80: */
    }
L90:
    j = jprev;
L100:
    ++j;
    if (j > n) {
	goto L110;
    }
    if ((d__1 = z__[j], abs(d__1)) <= tol) {

/*        Deflate due to small z component. */

	--k2;
	idxp[k2] = j;
	coltyp[j] = 4;
    } else {

/*        Check if singular values are close enough to allow deflation. */

	if ((d__1 = d__[j] - d__[jprev], abs(d__1)) <= tol) {

/*           Deflation is possible. */

	    s = z__[jprev];
	    c__ = z__[j];

/*           Find sqrt(a**2+b**2) without overflow or */
/*           destructive underflow. */

	    tau = dlapy2_(&c__, &s);
	    c__ /= tau;
	    s = -s / tau;
	    z__[j] = tau;
	    z__[jprev] = 0.;

/*           Apply back the Givens rotation to the left and right */
/*           singular vector matrices. */

	    idxjp = idxq[idx[jprev] + 1];
	    idxj = idxq[idx[j] + 1];
	    if (idxjp <= nlp1) {
		--idxjp;
	    }
	    if (idxj <= nlp1) {
		--idxj;
	    }
	    drot_(&n, &u[idxjp * u_dim1 + 1], &c__1, &u[idxj * u_dim1 + 1], &
		    c__1, &c__, &s);
	    drot_(&m, &vt[idxjp + vt_dim1], ldvt, &vt[idxj + vt_dim1], ldvt, &
		    c__, &s);
	    if (coltyp[j] != coltyp[jprev]) {
		coltyp[j] = 3;
	    }
	    coltyp[jprev] = 4;
	    --k2;
	    idxp[k2] = jprev;
	    jprev = j;
	} else {
	    ++(*k);
	    u2[*k + u2_dim1] = z__[jprev];
	    dsigma[*k] = d__[jprev];
	    idxp[*k] = jprev;
	    jprev = j;
	}
    }
    goto L100;
L110:

/*     Record the last singular value. */

    ++(*k);
    u2[*k + u2_dim1] = z__[jprev];
    dsigma[*k] = d__[jprev];
    idxp[*k] = jprev;

L120:

/*     Count up the total number of the various types of columns, then */
/*     form a permutation which positions the four column types into */
/*     four groups of uniform structure (although one or more of these */
/*     groups may be empty). */

    for (j = 1; j <= 4; ++j) {
	ctot[j - 1] = 0;
/* L130: */
    }
    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	ct = coltyp[j];
	++ctot[ct - 1];
/* L140: */
    }

/*     PSM(*) = Position in SubMatrix (of types 1 through 4) */

    psm[0] = 2;
    psm[1] = ctot[0] + 2;
    psm[2] = psm[1] + ctot[1];
    psm[3] = psm[2] + ctot[2];

/*     Fill out the IDXC array so that the permutation which it induces */
/*     will place all type-1 columns first, all type-2 columns next, */
/*     then all type-3's, and finally all type-4's, starting from the */
/*     second column. This applies similarly to the rows of VT. */

    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	jp = idxp[j];
	ct = coltyp[jp];
	idxc[psm[ct - 1]] = j;
	++psm[ct - 1];
/* L150: */
    }

/*     Sort the singular values and corresponding singular vectors into */
/*     DSIGMA, U2, and VT2 respectively.  The singular values/vectors */
/*     which were not deflated go into the first K slots of DSIGMA, U2, */
/*     and VT2 respectively, while those which were deflated go into the */
/*     last N - K slots, except that the first column/row will be treated */
/*     separately. */

    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	jp = idxp[j];
	dsigma[j] = d__[jp];
	idxj = idxq[idx[idxp[idxc[j]]] + 1];
	if (idxj <= nlp1) {
	    --idxj;
	}
	dcopy_(&n, &u[idxj * u_dim1 + 1], &c__1, &u2[j * u2_dim1 + 1], &c__1);
	dcopy_(&m, &vt[idxj + vt_dim1], ldvt, &vt2[j + vt2_dim1], ldvt2);
/* L160: */
    }

/*     Determine DSIGMA(1), DSIGMA(2) and Z(1) */

    dsigma[1] = 0.;
    hlftol = tol / 2.;
    if (abs(dsigma[2]) <= hlftol) {
	dsigma[2] = hlftol;
    }
    if (m > n) {
	z__[1] = dlapy2_(&z1, &z__[m]);
	if (z__[1] <= tol) {
	    c__ = 1.;
	    s = 0.;
	    z__[1] = tol;
	} else {
	    c__ = z1 / z__[1];
	    s = z__[m] / z__[1];
	}
    } else {
	if (abs(z1) <= tol) {
	    z__[1] = tol;
	} else {
	    z__[1] = z1;
	}
    }

/*     Move the rest of the updating row to Z. */

    i__1 = *k - 1;
    dcopy_(&i__1, &u2[u2_dim1 + 2], &c__1, &z__[2], &c__1);

/*     Determine the first column of U2, the first row of VT2 and the */
/*     last row of VT. */

    dlaset_("A", &n, &c__1, &c_b30, &c_b30, &u2[u2_offset], ldu2);
    u2[nlp1 + u2_dim1] = 1.;
    if (m > n) {
	i__1 = nlp1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    vt[m + i__ * vt_dim1] = -s * vt[nlp1 + i__ * vt_dim1];
	    vt2[i__ * vt2_dim1 + 1] = c__ * vt[nlp1 + i__ * vt_dim1];
/* L170: */
	}
	i__1 = m;
	for (i__ = nlp2; i__ <= i__1; ++i__) {
	    vt2[i__ * vt2_dim1 + 1] = s * vt[m + i__ * vt_dim1];
	    vt[m + i__ * vt_dim1] = c__ * vt[m + i__ * vt_dim1];
/* L180: */
	}
    } else {
	dcopy_(&m, &vt[nlp1 + vt_dim1], ldvt, &vt2[vt2_dim1 + 1], ldvt2);
    }
    if (m > n) {
	dcopy_(&m, &vt[m + vt_dim1], ldvt, &vt2[m + vt2_dim1], ldvt2);
    }

/*     The deflated singular values and their corresponding vectors go */
/*     into the back of D, U, and V respectively. */

    if (n > *k) {
	i__1 = n - *k;
	dcopy_(&i__1, &dsigma[*k + 1], &c__1, &d__[*k + 1], &c__1);
	i__1 = n - *k;
	dlacpy_("A", &n, &i__1, &u2[(*k + 1) * u2_dim1 + 1], ldu2, &u[(*k + 1)
		 * u_dim1 + 1], ldu);
	i__1 = n - *k;
	dlacpy_("A", &i__1, &m, &vt2[*k + 1 + vt2_dim1], ldvt2, &vt[*k + 1 +
		vt_dim1], ldvt);
    }

/*     Copy CTOT into COLTYP for referencing in DLASD3. */

    for (j = 1; j <= 4; ++j) {
	coltyp[j] = ctot[j - 1];
/* L190: */
    }

    return 0;

/*     End of DLASD2 */

} /* dlasd2_ */

/* Subroutine */ int dlasd3_(integer *nl, integer *nr, integer *sqre, integer
	*k, double *d__, double *q, integer *ldq, double *dsigma,
	double *u, integer *ldu, double *u2, integer *ldu2,
	double *vt, integer *ldvt, double *vt2, integer *ldvt2,
	integer *idxc, integer *ctot, double *z__, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__0 = 0;
	static double c_b13 = 1.;
	static double c_b26 = 0.;

    /* System generated locals */
    integer q_dim1, q_offset, u_dim1, u_offset, u2_dim1, u2_offset, vt_dim1,
	    vt_offset, vt2_dim1, vt2_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, m, n, jc;
    double rho;
    integer nlp1, nlp2, nrp1;
    double temp;
    integer ctemp;
    integer ktemp;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASD3 finds all the square roots of the roots of the secular */
/*  equation, as defined by the values in D and Z.  It makes the */
/*  appropriate calls to DLASD4 and then updates the singular */
/*  vectors by matrix multiplication. */

/*  This code makes very mild assumptions about floating point */
/*  arithmetic. It will work on machines with a guard digit in */
/*  add/subtract, or on those binary machines without guard digits */
/*  which subtract like the Cray XMP, Cray YMP, Cray C 90, or Cray 2. */
/*  It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  DLASD3 is called from DLASD1. */

/*  Arguments */
/*  ========= */

/*  NL     (input) INTEGER */
/*         The row dimension of the upper block.  NL >= 1. */

/*  NR     (input) INTEGER */
/*         The row dimension of the lower block.  NR >= 1. */

/*  SQRE   (input) INTEGER */
/*         = 0: the lower block is an NR-by-NR square matrix. */
/*         = 1: the lower block is an NR-by-(NR+1) rectangular matrix. */

/*         The bidiagonal matrix has N = NL + NR + 1 rows and */
/*         M = N + SQRE >= N columns. */

/*  K      (input) INTEGER */
/*         The size of the secular equation, 1 =< K = < N. */

/*  D      (output) DOUBLE PRECISION array, dimension(K) */
/*         On exit the square roots of the roots of the secular equation, */
/*         in ascending order. */

/*  Q      (workspace) DOUBLE PRECISION array, */
/*                     dimension at least (LDQ,K). */

/*  LDQ    (input) INTEGER */
/*         The leading dimension of the array Q.  LDQ >= K. */

/*  DSIGMA (input) DOUBLE PRECISION array, dimension(K) */
/*         The first K elements of this array contain the old roots */
/*         of the deflated updating problem.  These are the poles */
/*         of the secular equation. */

/*  U      (output) DOUBLE PRECISION array, dimension (LDU, N) */
/*         The last N - K columns of this matrix contain the deflated */
/*         left singular vectors. */

/*  LDU    (input) INTEGER */
/*         The leading dimension of the array U.  LDU >= N. */

/*  U2     (input/output) DOUBLE PRECISION array, dimension (LDU2, N) */
/*         The first K columns of this matrix contain the non-deflated */
/*         left singular vectors for the split problem. */

/*  LDU2   (input) INTEGER */
/*         The leading dimension of the array U2.  LDU2 >= N. */

/*  VT     (output) DOUBLE PRECISION array, dimension (LDVT, M) */
/*         The last M - K columns of VT' contain the deflated */
/*         right singular vectors. */

/*  LDVT   (input) INTEGER */
/*         The leading dimension of the array VT.  LDVT >= N. */

/*  VT2    (input/output) DOUBLE PRECISION array, dimension (LDVT2, N) */
/*         The first K columns of VT2' contain the non-deflated */
/*         right singular vectors for the split problem. */

/*  LDVT2  (input) INTEGER */
/*         The leading dimension of the array VT2.  LDVT2 >= N. */

/*  IDXC   (input) INTEGER array, dimension ( N ) */
/*         The permutation used to arrange the columns of U (and rows of */
/*         VT) into three groups:  the first group contains non-zero */
/*         entries only at and above (or before) NL +1; the second */
/*         contains non-zero entries only at and below (or after) NL+2; */
/*         and the third is dense. The first column of U and the row of */
/*         VT are treated separately, however. */

/*         The rows of the singular vectors found by DLASD4 */
/*         must be likewise permuted before the matrix multiplies can */
/*         take place. */

/*  CTOT   (input) INTEGER array, dimension ( 4 ) */
/*         A count of the total number of the various types of columns */
/*         in U (or rows in VT), as described in IDXC. The fourth column */
/*         type is any column which has been deflated. */

/*  Z      (input) DOUBLE PRECISION array, dimension (K) */
/*         The first K elements of this array contain the components */
/*         of the deflation-adjusted updating row vector. */

/*  INFO   (output) INTEGER */
/*         = 0:  successful exit. */
/*         < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*         > 0:  if INFO = 1, an singular value did not converge */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --dsigma;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    u2_dim1 = *ldu2;
    u2_offset = 1 + u2_dim1;
    u2 -= u2_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    vt2_dim1 = *ldvt2;
    vt2_offset = 1 + vt2_dim1;
    vt2 -= vt2_offset;
    --idxc;
    --ctot;
    --z__;

    /* Function Body */
    *info = 0;

    if (*nl < 1) {
	*info = -1;
    } else if (*nr < 1) {
	*info = -2;
    } else if (*sqre != 1 && *sqre != 0) {
	*info = -3;
    }

    n = *nl + *nr + 1;
    m = n + *sqre;
    nlp1 = *nl + 1;
    nlp2 = *nl + 2;

    if (*k < 1 || *k > n) {
	*info = -4;
    } else if (*ldq < *k) {
	*info = -7;
    } else if (*ldu < n) {
	*info = -10;
    } else if (*ldu2 < n) {
	*info = -12;
    } else if (*ldvt < m) {
	*info = -14;
    } else if (*ldvt2 < m) {
	*info = -16;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD3", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*k == 1) {
	d__[1] = abs(z__[1]);
	dcopy_(&m, &vt2[vt2_dim1 + 1], ldvt2, &vt[vt_dim1 + 1], ldvt);
	if (z__[1] > 0.) {
	    dcopy_(&n, &u2[u2_dim1 + 1], &c__1, &u[u_dim1 + 1], &c__1);
	} else {
	    i__1 = n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		u[i__ + u_dim1] = -u2[i__ + u2_dim1];
/* L10: */
	    }
	}
	return 0;
    }

/*     Modify values DSIGMA(i) to make sure all DSIGMA(i)-DSIGMA(j) can */
/*     be computed with high relative accuracy (barring over/underflow). */
/*     This is a problem on machines without a guard digit in */
/*     add/subtract (Cray XMP, Cray YMP, Cray C 90 and Cray 2). */
/*     The following code replaces DSIGMA(I) by 2*DSIGMA(I)-DSIGMA(I), */
/*     which on any of these machines zeros out the bottommost */
/*     bit of DSIGMA(I) if it is 1; this makes the subsequent */
/*     subtractions DSIGMA(I)-DSIGMA(J) unproblematic when cancellation */
/*     occurs. On binary machines with a guard digit (almost all */
/*     machines) it does not change DSIGMA(I) at all. On hexadecimal */
/*     and decimal machines with a guard digit, it slightly */
/*     changes the bottommost bits of DSIGMA(I). It does not account */
/*     for hexadecimal or decimal machines without guard digits */
/*     (we know of none). We use a subroutine call to compute */
/*     2*DSIGMA(I) to prevent optimizing compilers from eliminating */
/*     this code. */

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dsigma[i__] = dlamc3_(&dsigma[i__], &dsigma[i__]) - dsigma[i__];
/* L20: */
    }

/*     Keep a copy of Z. */

    dcopy_(k, &z__[1], &c__1, &q[q_offset], &c__1);

/*     Normalize Z. */

    rho = dnrm2_(k, &z__[1], &c__1);
    dlascl_("G", &c__0, &c__0, &rho, &c_b13, k, &c__1, &z__[1], k, info);
    rho *= rho;

/*     Find the new singular values. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	dlasd4_(k, &j, &dsigma[1], &z__[1], &u[j * u_dim1 + 1], &rho, &d__[j],
		 &vt[j * vt_dim1 + 1], info);

/*        If the zero finder fails, the computation is terminated. */

	if (*info != 0) {
	    return 0;
	}
/* L30: */
    }

/*     Compute updated Z. */

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	z__[i__] = u[i__ + *k * u_dim1] * vt[i__ + *k * vt_dim1];
	i__2 = i__ - 1;
	for (j = 1; j <= i__2; ++j) {
	    z__[i__] *= u[i__ + j * u_dim1] * vt[i__ + j * vt_dim1] / (dsigma[
		    i__] - dsigma[j]) / (dsigma[i__] + dsigma[j]);
/* L40: */
	}
	i__2 = *k - 1;
	for (j = i__; j <= i__2; ++j) {
	    z__[i__] *= u[i__ + j * u_dim1] * vt[i__ + j * vt_dim1] / (dsigma[
		    i__] - dsigma[j + 1]) / (dsigma[i__] + dsigma[j + 1]);
/* L50: */
	}
	d__2 = sqrt((d__1 = z__[i__], abs(d__1)));
	z__[i__] = d_sign(&d__2, &q[i__ + q_dim1]);
/* L60: */
    }

/*     Compute left singular vectors of the modified diagonal matrix, */
/*     and store related information for the right singular vectors. */

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	vt[i__ * vt_dim1 + 1] = z__[1] / u[i__ * u_dim1 + 1] / vt[i__ *
		vt_dim1 + 1];
	u[i__ * u_dim1 + 1] = -1.;
	i__2 = *k;
	for (j = 2; j <= i__2; ++j) {
	    vt[j + i__ * vt_dim1] = z__[j] / u[j + i__ * u_dim1] / vt[j + i__
		    * vt_dim1];
	    u[j + i__ * u_dim1] = dsigma[j] * vt[j + i__ * vt_dim1];
/* L70: */
	}
	temp = dnrm2_(k, &u[i__ * u_dim1 + 1], &c__1);
	q[i__ * q_dim1 + 1] = u[i__ * u_dim1 + 1] / temp;
	i__2 = *k;
	for (j = 2; j <= i__2; ++j) {
	    jc = idxc[j];
	    q[j + i__ * q_dim1] = u[jc + i__ * u_dim1] / temp;
/* L80: */
	}
/* L90: */
    }

/*     Update the left singular vector matrix. */

    if (*k == 2) {
	dgemm_("N", "N", &n, k, k, &c_b13, &u2[u2_offset], ldu2, &q[q_offset],
		 ldq, &c_b26, &u[u_offset], ldu);
	goto L100;
    }
    if (ctot[1] > 0) {
	dgemm_("N", "N", nl, k, &ctot[1], &c_b13, &u2[(u2_dim1 << 1) + 1],
		ldu2, &q[q_dim1 + 2], ldq, &c_b26, &u[u_dim1 + 1], ldu);
	if (ctot[3] > 0) {
	    ktemp = ctot[1] + 2 + ctot[2];
	    dgemm_("N", "N", nl, k, &ctot[3], &c_b13, &u2[ktemp * u2_dim1 + 1]
, ldu2, &q[ktemp + q_dim1], ldq, &c_b13, &u[u_dim1 + 1],
		    ldu);
	}
    } else if (ctot[3] > 0) {
	ktemp = ctot[1] + 2 + ctot[2];
	dgemm_("N", "N", nl, k, &ctot[3], &c_b13, &u2[ktemp * u2_dim1 + 1],
		ldu2, &q[ktemp + q_dim1], ldq, &c_b26, &u[u_dim1 + 1], ldu);
    } else {
	dlacpy_("F", nl, k, &u2[u2_offset], ldu2, &u[u_offset], ldu);
    }
    dcopy_(k, &q[q_dim1 + 1], ldq, &u[nlp1 + u_dim1], ldu);
    ktemp = ctot[1] + 2;
    ctemp = ctot[2] + ctot[3];
    dgemm_("N", "N", nr, k, &ctemp, &c_b13, &u2[nlp2 + ktemp * u2_dim1], ldu2,
	     &q[ktemp + q_dim1], ldq, &c_b26, &u[nlp2 + u_dim1], ldu);

/*     Generate the right singular vectors. */

L100:
    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	temp = dnrm2_(k, &vt[i__ * vt_dim1 + 1], &c__1);
	q[i__ + q_dim1] = vt[i__ * vt_dim1 + 1] / temp;
	i__2 = *k;
	for (j = 2; j <= i__2; ++j) {
	    jc = idxc[j];
	    q[i__ + j * q_dim1] = vt[jc + i__ * vt_dim1] / temp;
/* L110: */
	}
/* L120: */
    }

/*     Update the right singular vector matrix. */

    if (*k == 2) {
	dgemm_("N", "N", k, &m, k, &c_b13, &q[q_offset], ldq, &vt2[vt2_offset]
, ldvt2, &c_b26, &vt[vt_offset], ldvt);
	return 0;
    }
    ktemp = ctot[1] + 1;
    dgemm_("N", "N", k, &nlp1, &ktemp, &c_b13, &q[q_dim1 + 1], ldq, &vt2[
	    vt2_dim1 + 1], ldvt2, &c_b26, &vt[vt_dim1 + 1], ldvt);
    ktemp = ctot[1] + 2 + ctot[2];
    if (ktemp <= *ldvt2) {
	dgemm_("N", "N", k, &nlp1, &ctot[3], &c_b13, &q[ktemp * q_dim1 + 1],
		ldq, &vt2[ktemp + vt2_dim1], ldvt2, &c_b13, &vt[vt_dim1 + 1],
		ldvt);
    }

    ktemp = ctot[1] + 1;
    nrp1 = *nr + *sqre;
    if (ktemp > 1) {
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    q[i__ + ktemp * q_dim1] = q[i__ + q_dim1];
/* L130: */
	}
	i__1 = m;
	for (i__ = nlp2; i__ <= i__1; ++i__) {
	    vt2[ktemp + i__ * vt2_dim1] = vt2[i__ * vt2_dim1 + 1];
/* L140: */
	}
    }
    ctemp = ctot[2] + 1 + ctot[3];
    dgemm_("N", "N", k, &nrp1, &ctemp, &c_b13, &q[ktemp * q_dim1 + 1], ldq, &
	    vt2[ktemp + nlp2 * vt2_dim1], ldvt2, &c_b26, &vt[nlp2 * vt_dim1 +
	    1], ldvt);

    return 0;

/*     End of DLASD3 */

} /* dlasd3_ */

/* Subroutine */ int dlasd4_(integer *n, integer *i__, double *d__,
	double *z__, double *delta, double *rho, double *
	sigma, double *work, integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    double a, b, c__;
    integer j;
    double w, dd[3];
    integer ii;
    double dw, zz[3];
    integer ip1;
    double eta, phi, eps, tau, psi;
    integer iim1, iip1;
    double dphi, dpsi;
    integer iter;
    double temp, prew, sg2lb, sg2ub, temp1, temp2, dtiim, delsq, dtiip;
    integer niter;
    double dtisq;
    bool swtch;
    double dtnsq;
    double delsq2, dtnsq1;
    bool swtch3;
   bool orgati;
    double erretm, dtipsq, rhoinv;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This subroutine computes the square root of the I-th updated */
/*  eigenvalue of a positive symmetric rank-one modification to */
/*  a positive diagonal matrix whose entries are given as the squares */
/*  of the corresponding entries in the array d, and that */

/*         0 <= D(i) < D(j)  for  i < j */

/*  and that RHO > 0. This is arranged by the calling routine, and is */
/*  no loss in generality.  The rank-one modified system is thus */

/*         diag( D ) * diag( D ) +  RHO *  Z * Z_transpose. */

/*  where we assume the Euclidean norm of Z is 1. */

/*  The method consists of approximating the rational functions in the */
/*  secular equation by simpler interpolating rational functions. */

/*  Arguments */
/*  ========= */

/*  N      (input) INTEGER */
/*         The length of all arrays. */

/*  I      (input) INTEGER */
/*         The index of the eigenvalue to be computed.  1 <= I <= N. */

/*  D      (input) DOUBLE PRECISION array, dimension ( N ) */
/*         The original eigenvalues.  It is assumed that they are in */
/*         order, 0 <= D(I) < D(J)  for I < J. */

/*  Z      (input) DOUBLE PRECISION array, dimension ( N ) */
/*         The components of the updating vector. */

/*  DELTA  (output) DOUBLE PRECISION array, dimension ( N ) */
/*         If N .ne. 1, DELTA contains (D(j) - sigma_I) in its  j-th */
/*         component.  If N = 1, then DELTA(1) = 1.  The vector DELTA */
/*         contains the information necessary to construct the */
/*         (singular) eigenvectors. */

/*  RHO    (input) DOUBLE PRECISION */
/*         The scalar in the symmetric updating formula. */

/*  SIGMA  (output) DOUBLE PRECISION */
/*         The computed sigma_I, the I-th updated eigenvalue. */

/*  WORK   (workspace) DOUBLE PRECISION array, dimension ( N ) */
/*         If N .ne. 1, WORK contains (D(j) + sigma_I) in its  j-th */
/*         component.  If N = 1, then WORK( 1 ) = 1. */

/*  INFO   (output) INTEGER */
/*         = 0:  successful exit */
/*         > 0:  if INFO = 1, the updating process failed. */

/*  Internal Parameters */
/*  =================== */

/*  Logical variable ORGATI (origin-at-i?) is used for distinguishing */
/*  whether D(i) or D(i+1) is treated as the origin. */

/*            ORGATI = .true.    origin at i */
/*            ORGATI = .false.   origin at i+1 */

/*  Logical variable SWTCH3 (switch-for-3-poles?) is for noting */
/*  if we are working with THREE poles! */

/*  MAXIT is the maximum number of iterations allowed for each */
/*  eigenvalue. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ren-Cang Li, Computer Science Division, University of California */
/*     at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Since this routine is called in an inner loop, we do no argument */
/*     checking. */

/*     Quick return for N=1 and 2. */

    /* Parameter adjustments */
    --work;
    --delta;
    --z__;
    --d__;

    /* Function Body */
    *info = 0;
    if (*n == 1) {

/*        Presumably, I=1 upon entry */

	*sigma = sqrt(d__[1] * d__[1] + *rho * z__[1] * z__[1]);
	delta[1] = 1.;
	work[1] = 1.;
	return 0;
    }
    if (*n == 2) {
	dlasd5_(i__, &d__[1], &z__[1], &delta[1], rho, sigma, &work[1]);
	return 0;
    }

/*     Compute machine epsilon */

    eps = dlamch_("Epsilon");
    rhoinv = 1. / *rho;

/*     The case I = N */

    if (*i__ == *n) {

/*        Initialize some basic variables */

	ii = *n - 1;
	niter = 1;

/*        Calculate initial guess */

	temp = *rho / 2.;

/*        If ||Z||_2 is not one, then TEMP should be set to */
/*        RHO * ||Z||_2^2 / TWO */

	temp1 = temp / (d__[*n] + sqrt(d__[*n] * d__[*n] + temp));
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    work[j] = d__[j] + d__[*n] + temp1;
	    delta[j] = d__[j] - d__[*n] - temp1;
/* L10: */
	}

	psi = 0.;
	i__1 = *n - 2;
	for (j = 1; j <= i__1; ++j) {
	    psi += z__[j] * z__[j] / (delta[j] * work[j]);
/* L20: */
	}

	c__ = rhoinv + psi;
	w = c__ + z__[ii] * z__[ii] / (delta[ii] * work[ii]) + z__[*n] * z__[*
		n] / (delta[*n] * work[*n]);

	if (w <= 0.) {
	    temp1 = sqrt(d__[*n] * d__[*n] + *rho);
	    temp = z__[*n - 1] * z__[*n - 1] / ((d__[*n - 1] + temp1) * (d__[*
		    n] - d__[*n - 1] + *rho / (d__[*n] + temp1))) + z__[*n] *
		    z__[*n] / *rho;

/*           The following TAU is to approximate */
/*           SIGMA_n^2 - D( N )*D( N ) */

	    if (c__ <= temp) {
		tau = *rho;
	    } else {
		delsq = (d__[*n] - d__[*n - 1]) * (d__[*n] + d__[*n - 1]);
		a = -c__ * delsq + z__[*n - 1] * z__[*n - 1] + z__[*n] * z__[*
			n];
		b = z__[*n] * z__[*n] * delsq;
		if (a < 0.) {
		    tau = b * 2. / (sqrt(a * a + b * 4. * c__) - a);
		} else {
		    tau = (a + sqrt(a * a + b * 4. * c__)) / (c__ * 2.);
		}
	    }

/*           It can be proved that */
/*               D(N)^2+RHO/2 <= SIGMA_n^2 < D(N)^2+TAU <= D(N)^2+RHO */

	} else {
	    delsq = (d__[*n] - d__[*n - 1]) * (d__[*n] + d__[*n - 1]);
	    a = -c__ * delsq + z__[*n - 1] * z__[*n - 1] + z__[*n] * z__[*n];
	    b = z__[*n] * z__[*n] * delsq;

/*           The following TAU is to approximate */
/*           SIGMA_n^2 - D( N )*D( N ) */

	    if (a < 0.) {
		tau = b * 2. / (sqrt(a * a + b * 4. * c__) - a);
	    } else {
		tau = (a + sqrt(a * a + b * 4. * c__)) / (c__ * 2.);
	    }

/*           It can be proved that */
/*           D(N)^2 < D(N)^2+TAU < SIGMA(N)^2 < D(N)^2+RHO/2 */

	}

/*        The following ETA is to approximate SIGMA_n - D( N ) */

	eta = tau / (d__[*n] + sqrt(d__[*n] * d__[*n] + tau));

	*sigma = d__[*n] + eta;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - eta;
	    work[j] = d__[j] + d__[*i__] + eta;
/* L30: */
	}

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = ii;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / (delta[j] * work[j]);
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L40: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	temp = z__[*n] / (delta[*n] * work[*n]);
	phi = z__[*n] * temp;
	dphi = temp * temp;
	erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (dpsi
		+ dphi);

	w = rhoinv + phi + psi;

/*        Test for convergence */

	if (abs(w) <= eps * erretm) {
	    goto L240;
	}

/*        Calculate the new step */

	++niter;
	dtnsq1 = work[*n - 1] * delta[*n - 1];
	dtnsq = work[*n] * delta[*n];
	c__ = w - dtnsq1 * dpsi - dtnsq * dphi;
	a = (dtnsq + dtnsq1) * w - dtnsq * dtnsq1 * (dpsi + dphi);
	b = dtnsq * dtnsq1 * w;
	if (c__ < 0.) {
	    c__ = abs(c__);
	}
	if (c__ == 0.) {
	    eta = *rho - *sigma * *sigma;
	} else if (a >= 0.) {
	    eta = (a + sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (c__
		    * 2.);
	} else {
	    eta = b * 2. / (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))
		    );
	}

/*        Note, eta should be positive if w is negative, and */
/*        eta should be negative otherwise. However, */
/*        if for some reason caused by roundoff, eta*w > 0, */
/*        we simply use one Newton step instead. This way */
/*        will guarantee eta*w < 0. */

	if (w * eta > 0.) {
	    eta = -w / (dpsi + dphi);
	}
	temp = eta - dtnsq;
	if (temp > *rho) {
	    eta = *rho + dtnsq;
	}

	tau += eta;
	eta /= *sigma + sqrt(eta + *sigma * *sigma);
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] -= eta;
	    work[j] += eta;
/* L50: */
	}

	*sigma += eta;

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = ii;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / (work[j] * delta[j]);
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L60: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	temp = z__[*n] / (work[*n] * delta[*n]);
	phi = z__[*n] * temp;
	dphi = temp * temp;
	erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (dpsi
		+ dphi);

	w = rhoinv + phi + psi;

/*        Main loop to update the values of the array   DELTA */

	iter = niter + 1;

	for (niter = iter; niter <= 20; ++niter) {

/*           Test for convergence */

	    if (abs(w) <= eps * erretm) {
		goto L240;
	    }

/*           Calculate the new step */

	    dtnsq1 = work[*n - 1] * delta[*n - 1];
	    dtnsq = work[*n] * delta[*n];
	    c__ = w - dtnsq1 * dpsi - dtnsq * dphi;
	    a = (dtnsq + dtnsq1) * w - dtnsq1 * dtnsq * (dpsi + dphi);
	    b = dtnsq1 * dtnsq * w;
	    if (a >= 0.) {
		eta = (a + sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
			c__ * 2.);
	    } else {
		eta = b * 2. / (a - sqrt((d__1 = a * a - b * 4. * c__, abs(
			d__1))));
	    }

/*           Note, eta should be positive if w is negative, and */
/*           eta should be negative otherwise. However, */
/*           if for some reason caused by roundoff, eta*w > 0, */
/*           we simply use one Newton step instead. This way */
/*           will guarantee eta*w < 0. */

	    if (w * eta > 0.) {
		eta = -w / (dpsi + dphi);
	    }
	    temp = eta - dtnsq;
	    if (temp <= 0.) {
		eta /= 2.;
	    }

	    tau += eta;
	    eta /= *sigma + sqrt(eta + *sigma * *sigma);
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] -= eta;
		work[j] += eta;
/* L70: */
	    }

	    *sigma += eta;

/*           Evaluate PSI and the derivative DPSI */

	    dpsi = 0.;
	    psi = 0.;
	    erretm = 0.;
	    i__1 = ii;
	    for (j = 1; j <= i__1; ++j) {
		temp = z__[j] / (work[j] * delta[j]);
		psi += z__[j] * temp;
		dpsi += temp * temp;
		erretm += psi;
/* L80: */
	    }
	    erretm = abs(erretm);

/*           Evaluate PHI and the derivative DPHI */

	    temp = z__[*n] / (work[*n] * delta[*n]);
	    phi = z__[*n] * temp;
	    dphi = temp * temp;
	    erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (
		    dpsi + dphi);

	    w = rhoinv + phi + psi;
/* L90: */
	}

/*        Return with INFO = 1, NITER = MAXIT and not converged */

	*info = 1;
	goto L240;

/*        End for the case I = N */

    } else {

/*        The case for I < N */

	niter = 1;
	ip1 = *i__ + 1;

/*        Calculate initial guess */

	delsq = (d__[ip1] - d__[*i__]) * (d__[ip1] + d__[*i__]);
	delsq2 = delsq / 2.;
	temp = delsq2 / (d__[*i__] + sqrt(d__[*i__] * d__[*i__] + delsq2));
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    work[j] = d__[j] + d__[*i__] + temp;
	    delta[j] = d__[j] - d__[*i__] - temp;
/* L100: */
	}

	psi = 0.;
	i__1 = *i__ - 1;
	for (j = 1; j <= i__1; ++j) {
	    psi += z__[j] * z__[j] / (work[j] * delta[j]);
/* L110: */
	}

	phi = 0.;
	i__1 = *i__ + 2;
	for (j = *n; j >= i__1; --j) {
	    phi += z__[j] * z__[j] / (work[j] * delta[j]);
/* L120: */
	}
	c__ = rhoinv + psi + phi;
	w = c__ + z__[*i__] * z__[*i__] / (work[*i__] * delta[*i__]) + z__[
		ip1] * z__[ip1] / (work[ip1] * delta[ip1]);

	if (w > 0.) {

/*           d(i)^2 < the ith sigma^2 < (d(i)^2+d(i+1)^2)/2 */

/*           We choose d(i) as origin. */

	    orgati = true;
	    sg2lb = 0.;
	    sg2ub = delsq2;
	    a = c__ * delsq + z__[*i__] * z__[*i__] + z__[ip1] * z__[ip1];
	    b = z__[*i__] * z__[*i__] * delsq;
	    if (a > 0.) {
		tau = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(
			d__1))));
	    } else {
		tau = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
			c__ * 2.);
	    }

/*           TAU now is an estimation of SIGMA^2 - D( I )^2. The */
/*           following, however, is the corresponding estimation of */
/*           SIGMA - D( I ). */

	    eta = tau / (d__[*i__] + sqrt(d__[*i__] * d__[*i__] + tau));
	} else {

/*           (d(i)^2+d(i+1)^2)/2 <= the ith sigma^2 < d(i+1)^2/2 */

/*           We choose d(i+1) as origin. */

	    orgati = false;
	    sg2lb = -delsq2;
	    sg2ub = 0.;
	    a = c__ * delsq - z__[*i__] * z__[*i__] - z__[ip1] * z__[ip1];
	    b = z__[ip1] * z__[ip1] * delsq;
	    if (a < 0.) {
		tau = b * 2. / (a - sqrt((d__1 = a * a + b * 4. * c__, abs(
			d__1))));
	    } else {
		tau = -(a + sqrt((d__1 = a * a + b * 4. * c__, abs(d__1)))) /
			(c__ * 2.);
	    }

/*           TAU now is an estimation of SIGMA^2 - D( IP1 )^2. The */
/*           following, however, is the corresponding estimation of */
/*           SIGMA - D( IP1 ). */

	    eta = tau / (d__[ip1] + sqrt((d__1 = d__[ip1] * d__[ip1] + tau,
		    abs(d__1))));
	}

	if (orgati) {
	    ii = *i__;
	    *sigma = d__[*i__] + eta;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		work[j] = d__[j] + d__[*i__] + eta;
		delta[j] = d__[j] - d__[*i__] - eta;
/* L130: */
	    }
	} else {
	    ii = *i__ + 1;
	    *sigma = d__[ip1] + eta;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		work[j] = d__[j] + d__[ip1] + eta;
		delta[j] = d__[j] - d__[ip1] - eta;
/* L140: */
	    }
	}
	iim1 = ii - 1;
	iip1 = ii + 1;

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = iim1;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / (work[j] * delta[j]);
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L150: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	dphi = 0.;
	phi = 0.;
	i__1 = iip1;
	for (j = *n; j >= i__1; --j) {
	    temp = z__[j] / (work[j] * delta[j]);
	    phi += z__[j] * temp;
	    dphi += temp * temp;
	    erretm += phi;
/* L160: */
	}

	w = rhoinv + phi + psi;

/*        W is the value of the secular function with */
/*        its ii-th element removed. */

	swtch3 = false;
	if (orgati) {
	    if (w < 0.) {
		swtch3 = true;
	    }
	} else {
	    if (w > 0.) {
		swtch3 = true;
	    }
	}
	if (ii == 1 || ii == *n) {
	    swtch3 = false;
	}

	temp = z__[ii] / (work[ii] * delta[ii]);
	dw = dpsi + dphi + temp * temp;
	temp = z__[ii] * temp;
	w += temp;
	erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3. +
		abs(tau) * dw;

/*        Test for convergence */

	if (abs(w) <= eps * erretm) {
	    goto L240;
	}

	if (w <= 0.) {
	    sg2lb = std::max(sg2lb,tau);
	} else {
	    sg2ub = std::min(sg2ub,tau);
	}

/*        Calculate the new step */

	++niter;
	if (! swtch3) {
	    dtipsq = work[ip1] * delta[ip1];
	    dtisq = work[*i__] * delta[*i__];
	    if (orgati) {
/* Computing 2nd power */
		d__1 = z__[*i__] / dtisq;
		c__ = w - dtipsq * dw + delsq * (d__1 * d__1);
	    } else {
/* Computing 2nd power */
		d__1 = z__[ip1] / dtipsq;
		c__ = w - dtisq * dw - delsq * (d__1 * d__1);
	    }
	    a = (dtipsq + dtisq) * w - dtipsq * dtisq * dw;
	    b = dtipsq * dtisq * w;
	    if (c__ == 0.) {
		if (a == 0.) {
		    if (orgati) {
			a = z__[*i__] * z__[*i__] + dtipsq * dtipsq * (dpsi +
				dphi);
		    } else {
			a = z__[ip1] * z__[ip1] + dtisq * dtisq * (dpsi +
				dphi);
		    }
		}
		eta = b / a;
	    } else if (a <= 0.) {
		eta = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
			c__ * 2.);
	    } else {
		eta = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(
			d__1))));
	    }
	} else {

/*           Interpolation using THREE most relevant poles */

	    dtiim = work[iim1] * delta[iim1];
	    dtiip = work[iip1] * delta[iip1];
	    temp = rhoinv + psi + phi;
	    if (orgati) {
		temp1 = z__[iim1] / dtiim;
		temp1 *= temp1;
		c__ = temp - dtiip * (dpsi + dphi) - (d__[iim1] - d__[iip1]) *
			 (d__[iim1] + d__[iip1]) * temp1;
		zz[0] = z__[iim1] * z__[iim1];
		if (dpsi < temp1) {
		    zz[2] = dtiip * dtiip * dphi;
		} else {
		    zz[2] = dtiip * dtiip * (dpsi - temp1 + dphi);
		}
	    } else {
		temp1 = z__[iip1] / dtiip;
		temp1 *= temp1;
		c__ = temp - dtiim * (dpsi + dphi) - (d__[iip1] - d__[iim1]) *
			 (d__[iim1] + d__[iip1]) * temp1;
		if (dphi < temp1) {
		    zz[0] = dtiim * dtiim * dpsi;
		} else {
		    zz[0] = dtiim * dtiim * (dpsi + (dphi - temp1));
		}
		zz[2] = z__[iip1] * z__[iip1];
	    }
	    zz[1] = z__[ii] * z__[ii];
	    dd[0] = dtiim;
	    dd[1] = delta[ii] * work[ii];
	    dd[2] = dtiip;
	    dlaed6_(&niter, &orgati, &c__, dd, zz, &w, &eta, info);
	    if (*info != 0) {
		goto L240;
	    }
	}

/*        Note, eta should be positive if w is negative, and */
/*        eta should be negative otherwise. However, */
/*        if for some reason caused by roundoff, eta*w > 0, */
/*        we simply use one Newton step instead. This way */
/*        will guarantee eta*w < 0. */

	if (w * eta >= 0.) {
	    eta = -w / dw;
	}
	if (orgati) {
	    temp1 = work[*i__] * delta[*i__];
	    temp = eta - temp1;
	} else {
	    temp1 = work[ip1] * delta[ip1];
	    temp = eta - temp1;
	}
	if (temp > sg2ub || temp < sg2lb) {
	    if (w < 0.) {
		eta = (sg2ub - tau) / 2.;
	    } else {
		eta = (sg2lb - tau) / 2.;
	    }
	}

	tau += eta;
	eta /= *sigma + sqrt(*sigma * *sigma + eta);

	prew = w;

	*sigma += eta;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    work[j] += eta;
	    delta[j] -= eta;
/* L170: */
	}

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = iim1;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / (work[j] * delta[j]);
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L180: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	dphi = 0.;
	phi = 0.;
	i__1 = iip1;
	for (j = *n; j >= i__1; --j) {
	    temp = z__[j] / (work[j] * delta[j]);
	    phi += z__[j] * temp;
	    dphi += temp * temp;
	    erretm += phi;
/* L190: */
	}

	temp = z__[ii] / (work[ii] * delta[ii]);
	dw = dpsi + dphi + temp * temp;
	temp = z__[ii] * temp;
	w = rhoinv + phi + psi + temp;
	erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3. +
		abs(tau) * dw;

	if (w <= 0.) {
	    sg2lb = std::max(sg2lb,tau);
	} else {
	    sg2ub = std::min(sg2ub,tau);
	}

	swtch = false;
	if (orgati) {
	    if (-w > abs(prew) / 10.) {
		swtch = true;
	    }
	} else {
	    if (w > abs(prew) / 10.) {
		swtch = true;
	    }
	}

/*        Main loop to update the values of the array   DELTA and WORK */

	iter = niter + 1;

	for (niter = iter; niter <= 20; ++niter) {

/*           Test for convergence */

	    if (abs(w) <= eps * erretm) {
		goto L240;
	    }

/*           Calculate the new step */

	    if (! swtch3) {
		dtipsq = work[ip1] * delta[ip1];
		dtisq = work[*i__] * delta[*i__];
		if (! swtch) {
		    if (orgati) {
/* Computing 2nd power */
			d__1 = z__[*i__] / dtisq;
			c__ = w - dtipsq * dw + delsq * (d__1 * d__1);
		    } else {
/* Computing 2nd power */
			d__1 = z__[ip1] / dtipsq;
			c__ = w - dtisq * dw - delsq * (d__1 * d__1);
		    }
		} else {
		    temp = z__[ii] / (work[ii] * delta[ii]);
		    if (orgati) {
			dpsi += temp * temp;
		    } else {
			dphi += temp * temp;
		    }
		    c__ = w - dtisq * dpsi - dtipsq * dphi;
		}
		a = (dtipsq + dtisq) * w - dtipsq * dtisq * dw;
		b = dtipsq * dtisq * w;
		if (c__ == 0.) {
		    if (a == 0.) {
			if (! swtch) {
			    if (orgati) {
				a = z__[*i__] * z__[*i__] + dtipsq * dtipsq *
					(dpsi + dphi);
			    } else {
				a = z__[ip1] * z__[ip1] + dtisq * dtisq * (
					dpsi + dphi);
			    }
			} else {
			    a = dtisq * dtisq * dpsi + dtipsq * dtipsq * dphi;
			}
		    }
		    eta = b / a;
		} else if (a <= 0.) {
		    eta = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1))))
			     / (c__ * 2.);
		} else {
		    eta = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__,
			    abs(d__1))));
		}
	    } else {

/*              Interpolation using THREE most relevant poles */

		dtiim = work[iim1] * delta[iim1];
		dtiip = work[iip1] * delta[iip1];
		temp = rhoinv + psi + phi;
		if (swtch) {
		    c__ = temp - dtiim * dpsi - dtiip * dphi;
		    zz[0] = dtiim * dtiim * dpsi;
		    zz[2] = dtiip * dtiip * dphi;
		} else {
		    if (orgati) {
			temp1 = z__[iim1] / dtiim;
			temp1 *= temp1;
			temp2 = (d__[iim1] - d__[iip1]) * (d__[iim1] + d__[
				iip1]) * temp1;
			c__ = temp - dtiip * (dpsi + dphi) - temp2;
			zz[0] = z__[iim1] * z__[iim1];
			if (dpsi < temp1) {
			    zz[2] = dtiip * dtiip * dphi;
			} else {
			    zz[2] = dtiip * dtiip * (dpsi - temp1 + dphi);
			}
		    } else {
			temp1 = z__[iip1] / dtiip;
			temp1 *= temp1;
			temp2 = (d__[iip1] - d__[iim1]) * (d__[iim1] + d__[
				iip1]) * temp1;
			c__ = temp - dtiim * (dpsi + dphi) - temp2;
			if (dphi < temp1) {
			    zz[0] = dtiim * dtiim * dpsi;
			} else {
			    zz[0] = dtiim * dtiim * (dpsi + (dphi - temp1));
			}
			zz[2] = z__[iip1] * z__[iip1];
		    }
		}
		dd[0] = dtiim;
		dd[1] = delta[ii] * work[ii];
		dd[2] = dtiip;
		dlaed6_(&niter, &orgati, &c__, dd, zz, &w, &eta, info);
		if (*info != 0) {
		    goto L240;
		}
	    }

/*           Note, eta should be positive if w is negative, and */
/*           eta should be negative otherwise. However, */
/*           if for some reason caused by roundoff, eta*w > 0, */
/*           we simply use one Newton step instead. This way */
/*           will guarantee eta*w < 0. */

	    if (w * eta >= 0.) {
		eta = -w / dw;
	    }
	    if (orgati) {
		temp1 = work[*i__] * delta[*i__];
		temp = eta - temp1;
	    } else {
		temp1 = work[ip1] * delta[ip1];
		temp = eta - temp1;
	    }
	    if (temp > sg2ub || temp < sg2lb) {
		if (w < 0.) {
		    eta = (sg2ub - tau) / 2.;
		} else {
		    eta = (sg2lb - tau) / 2.;
		}
	    }

	    tau += eta;
	    eta /= *sigma + sqrt(*sigma * *sigma + eta);

	    *sigma += eta;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		work[j] += eta;
		delta[j] -= eta;
/* L200: */
	    }

	    prew = w;

/*           Evaluate PSI and the derivative DPSI */

	    dpsi = 0.;
	    psi = 0.;
	    erretm = 0.;
	    i__1 = iim1;
	    for (j = 1; j <= i__1; ++j) {
		temp = z__[j] / (work[j] * delta[j]);
		psi += z__[j] * temp;
		dpsi += temp * temp;
		erretm += psi;
/* L210: */
	    }
	    erretm = abs(erretm);

/*           Evaluate PHI and the derivative DPHI */

	    dphi = 0.;
	    phi = 0.;
	    i__1 = iip1;
	    for (j = *n; j >= i__1; --j) {
		temp = z__[j] / (work[j] * delta[j]);
		phi += z__[j] * temp;
		dphi += temp * temp;
		erretm += phi;
/* L220: */
	    }

	    temp = z__[ii] / (work[ii] * delta[ii]);
	    dw = dpsi + dphi + temp * temp;
	    temp = z__[ii] * temp;
	    w = rhoinv + phi + psi + temp;
	    erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3.
		    + abs(tau) * dw;
	    if (w * prew > 0. && abs(w) > abs(prew) / 10.) {
		swtch = ! swtch;
	    }

	    if (w <= 0.) {
		sg2lb = std::max(sg2lb,tau);
	    } else {
		sg2ub = std::min(sg2ub,tau);
	    }

/* L230: */
	}

/*        Return with INFO = 1, NITER = MAXIT and not converged */

	*info = 1;

    }

L240:
    return 0;

/*     End of DLASD4 */

} /* dlasd4_ */

/* Subroutine */ int dlasd5_(integer *i__, double *d__, double *z__,
	double *delta, double *rho, double *dsigma, double *
	work)
{
    /* System generated locals */
    double d__1;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    double b, c__, w, del, tau, delsq;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This subroutine computes the square root of the I-th eigenvalue */
/*  of a positive symmetric rank-one modification of a 2-by-2 diagonal */
/*  matrix */

/*             diag( D ) * diag( D ) +  RHO *  Z * transpose(Z) . */

/*  The diagonal entries in the array D are assumed to satisfy */

/*             0 <= D(i) < D(j)  for  i < j . */

/*  We also assume RHO > 0 and that the Euclidean norm of the vector */
/*  Z is one. */

/*  Arguments */
/*  ========= */

/*  I      (input) INTEGER */
/*         The index of the eigenvalue to be computed.  I = 1 or I = 2. */

/*  D      (input) DOUBLE PRECISION array, dimension ( 2 ) */
/*         The original eigenvalues.  We assume 0 <= D(1) < D(2). */

/*  Z      (input) DOUBLE PRECISION array, dimension ( 2 ) */
/*         The components of the updating vector. */

/*  DELTA  (output) DOUBLE PRECISION array, dimension ( 2 ) */
/*         Contains (D(j) - sigma_I) in its  j-th component. */
/*         The vector DELTA contains the information necessary */
/*         to construct the eigenvectors. */

/*  RHO    (input) DOUBLE PRECISION */
/*         The scalar in the symmetric updating formula. */

/*  DSIGMA (output) DOUBLE PRECISION */
/*         The computed sigma_I, the I-th updated eigenvalue. */

/*  WORK   (workspace) DOUBLE PRECISION array, dimension ( 2 ) */
/*         WORK contains (D(j) + sigma_I) in its  j-th component. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ren-Cang Li, Computer Science Division, University of California */
/*     at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --work;
    --delta;
    --z__;
    --d__;

    /* Function Body */
    del = d__[2] - d__[1];
    delsq = del * (d__[2] + d__[1]);
    if (*i__ == 1) {
	w = *rho * 4. * (z__[2] * z__[2] / (d__[1] + d__[2] * 3.) - z__[1] *
		z__[1] / (d__[1] * 3. + d__[2])) / del + 1.;
	if (w > 0.) {
	    b = delsq + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	    c__ = *rho * z__[1] * z__[1] * delsq;

/*           B > ZERO, always */

/*           The following TAU is DSIGMA * DSIGMA - D( 1 ) * D( 1 ) */

	    tau = c__ * 2. / (b + sqrt((d__1 = b * b - c__ * 4., abs(d__1))));

/*           The following TAU is DSIGMA - D( 1 ) */

	    tau /= d__[1] + sqrt(d__[1] * d__[1] + tau);
	    *dsigma = d__[1] + tau;
	    delta[1] = -tau;
	    delta[2] = del - tau;
	    work[1] = d__[1] * 2. + tau;
	    work[2] = d__[1] + tau + d__[2];
/*           DELTA( 1 ) = -Z( 1 ) / TAU */
/*           DELTA( 2 ) = Z( 2 ) / ( DEL-TAU ) */
	} else {
	    b = -delsq + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	    c__ = *rho * z__[2] * z__[2] * delsq;

/*           The following TAU is DSIGMA * DSIGMA - D( 2 ) * D( 2 ) */

	    if (b > 0.) {
		tau = c__ * -2. / (b + sqrt(b * b + c__ * 4.));
	    } else {
		tau = (b - sqrt(b * b + c__ * 4.)) / 2.;
	    }

/*           The following TAU is DSIGMA - D( 2 ) */

	    tau /= d__[2] + sqrt((d__1 = d__[2] * d__[2] + tau, abs(d__1)));
	    *dsigma = d__[2] + tau;
	    delta[1] = -(del + tau);
	    delta[2] = -tau;
	    work[1] = d__[1] + tau + d__[2];
	    work[2] = d__[2] * 2. + tau;
/*           DELTA( 1 ) = -Z( 1 ) / ( DEL+TAU ) */
/*           DELTA( 2 ) = -Z( 2 ) / TAU */
	}
/*        TEMP = SQRT( DELTA( 1 )*DELTA( 1 )+DELTA( 2 )*DELTA( 2 ) ) */
/*        DELTA( 1 ) = DELTA( 1 ) / TEMP */
/*        DELTA( 2 ) = DELTA( 2 ) / TEMP */
    } else {

/*        Now I=2 */

	b = -delsq + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	c__ = *rho * z__[2] * z__[2] * delsq;

/*        The following TAU is DSIGMA * DSIGMA - D( 2 ) * D( 2 ) */

	if (b > 0.) {
	    tau = (b + sqrt(b * b + c__ * 4.)) / 2.;
	} else {
	    tau = c__ * 2. / (-b + sqrt(b * b + c__ * 4.));
	}

/*        The following TAU is DSIGMA - D( 2 ) */

	tau /= d__[2] + sqrt(d__[2] * d__[2] + tau);
	*dsigma = d__[2] + tau;
	delta[1] = -(del + tau);
	delta[2] = -tau;
	work[1] = d__[1] + tau + d__[2];
	work[2] = d__[2] * 2. + tau;
/*        DELTA( 1 ) = -Z( 1 ) / ( DEL+TAU ) */
/*        DELTA( 2 ) = -Z( 2 ) / TAU */
/*        TEMP = SQRT( DELTA( 1 )*DELTA( 1 )+DELTA( 2 )*DELTA( 2 ) ) */
/*        DELTA( 1 ) = DELTA( 1 ) / TEMP */
/*        DELTA( 2 ) = DELTA( 2 ) / TEMP */
    }
    return 0;

/*     End of DLASD5 */

} /* dlasd5_ */

/* Subroutine */ int dlasd6_(integer *icompq, integer *nl, integer *nr,
	integer *sqre, double *d__, double *vf, double *vl,
	double *alpha, double *beta, integer *idxq, integer *perm,
	integer *givptr, integer *givcol, integer *ldgcol, double *givnum,
	integer *ldgnum, double *poles, double *difl, double *
	difr, double *z__, integer *k, double *c__, double *s,
	double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__0 = 0;
	static double c_b7 = 1.;
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer givcol_dim1, givcol_offset, givnum_dim1, givnum_offset,
	    poles_dim1, poles_offset, i__1;
    double d__1, d__2;

    /* Local variables */
    integer i__, m, n, n1, n2, iw, idx, idxc, idxp, ivfw, ivlw;
    integer isigma;
    double orgnrm;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASD6 computes the SVD of an updated upper bidiagonal matrix B */
/*  obtained by merging two smaller ones by appending a row. This */
/*  routine is used only for the problem which requires all singular */
/*  values and optionally singular vector matrices in factored form. */
/*  B is an N-by-M matrix with N = NL + NR + 1 and M = N + SQRE. */
/*  A related subroutine, DLASD1, handles the case in which all singular */
/*  values and singular vectors of the bidiagonal matrix are desired. */

/*  DLASD6 computes the SVD as follows: */

/*                ( D1(in)  0    0     0 ) */
/*    B = U(in) * (   Z1'   a   Z2'    b ) * VT(in) */
/*                (   0     0   D2(in) 0 ) */

/*      = U(out) * ( D(out) 0) * VT(out) */

/*  where Z' = (Z1' a Z2' b) = u' VT', and u is a vector of dimension M */
/*  with ALPHA and BETA in the NL+1 and NL+2 th entries and zeros */
/*  elsewhere; and the entry b is empty if SQRE = 0. */

/*  The singular values of B can be computed using D1, D2, the first */
/*  components of all the right singular vectors of the lower block, and */
/*  the last components of all the right singular vectors of the upper */
/*  block. These components are stored and updated in VF and VL, */
/*  respectively, in DLASD6. Hence U and VT are not explicitly */
/*  referenced. */

/*  The singular values are stored in D. The algorithm consists of two */
/*  stages: */

/*        The first stage consists of deflating the size of the problem */
/*        when there are multiple singular values or if there is a zero */
/*        in the Z vector. For each such occurrence the dimension of the */
/*        secular equation problem is reduced by one. This stage is */
/*        performed by the routine DLASD7. */

/*        The second stage consists of calculating the updated */
/*        singular values. This is done by finding the roots of the */
/*        secular equation via the routine DLASD4 (as called by DLASD8). */
/*        This routine also updates VF and VL and computes the distances */
/*        between the updated singular values and the old singular */
/*        values. */

/*  DLASD6 is called from DLASDA. */

/*  Arguments */
/*  ========= */

/*  ICOMPQ (input) INTEGER */
/*         Specifies whether singular vectors are to be computed in */
/*         factored form: */
/*         = 0: Compute singular values only. */
/*         = 1: Compute singular vectors in factored form as well. */

/*  NL     (input) INTEGER */
/*         The row dimension of the upper block.  NL >= 1. */

/*  NR     (input) INTEGER */
/*         The row dimension of the lower block.  NR >= 1. */

/*  SQRE   (input) INTEGER */
/*         = 0: the lower block is an NR-by-NR square matrix. */
/*         = 1: the lower block is an NR-by-(NR+1) rectangular matrix. */

/*         The bidiagonal matrix has row dimension N = NL + NR + 1, */
/*         and column dimension M = N + SQRE. */

/*  D      (input/output) DOUBLE PRECISION array, dimension ( NL+NR+1 ). */
/*         On entry D(1:NL,1:NL) contains the singular values of the */
/*         upper block, and D(NL+2:N) contains the singular values */
/*         of the lower block. On exit D(1:N) contains the singular */
/*         values of the modified matrix. */

/*  VF     (input/output) DOUBLE PRECISION array, dimension ( M ) */
/*         On entry, VF(1:NL+1) contains the first components of all */
/*         right singular vectors of the upper block; and VF(NL+2:M) */
/*         contains the first components of all right singular vectors */
/*         of the lower block. On exit, VF contains the first components */
/*         of all right singular vectors of the bidiagonal matrix. */

/*  VL     (input/output) DOUBLE PRECISION array, dimension ( M ) */
/*         On entry, VL(1:NL+1) contains the  last components of all */
/*         right singular vectors of the upper block; and VL(NL+2:M) */
/*         contains the last components of all right singular vectors of */
/*         the lower block. On exit, VL contains the last components of */
/*         all right singular vectors of the bidiagonal matrix. */

/*  ALPHA  (input/output) DOUBLE PRECISION */
/*         Contains the diagonal element associated with the added row. */

/*  BETA   (input/output) DOUBLE PRECISION */
/*         Contains the off-diagonal element associated with the added */
/*         row. */

/*  IDXQ   (output) INTEGER array, dimension ( N ) */
/*         This contains the permutation which will reintegrate the */
/*         subproblem just solved back into sorted order, i.e. */
/*         D( IDXQ( I = 1, N ) ) will be in ascending order. */

/*  PERM   (output) INTEGER array, dimension ( N ) */
/*         The permutations (from deflation and sorting) to be applied */
/*         to each block. Not referenced if ICOMPQ = 0. */

/*  GIVPTR (output) INTEGER */
/*         The number of Givens rotations which took place in this */
/*         subproblem. Not referenced if ICOMPQ = 0. */

/*  GIVCOL (output) INTEGER array, dimension ( LDGCOL, 2 ) */
/*         Each pair of numbers indicates a pair of columns to take place */
/*         in a Givens rotation. Not referenced if ICOMPQ = 0. */

/*  LDGCOL (input) INTEGER */
/*         leading dimension of GIVCOL, must be at least N. */

/*  GIVNUM (output) DOUBLE PRECISION array, dimension ( LDGNUM, 2 ) */
/*         Each number indicates the C or S value to be used in the */
/*         corresponding Givens rotation. Not referenced if ICOMPQ = 0. */

/*  LDGNUM (input) INTEGER */
/*         The leading dimension of GIVNUM and POLES, must be at least N. */

/*  POLES  (output) DOUBLE PRECISION array, dimension ( LDGNUM, 2 ) */
/*         On exit, POLES(1,*) is an array containing the new singular */
/*         values obtained from solving the secular equation, and */
/*         POLES(2,*) is an array containing the poles in the secular */
/*         equation. Not referenced if ICOMPQ = 0. */

/*  DIFL   (output) DOUBLE PRECISION array, dimension ( N ) */
/*         On exit, DIFL(I) is the distance between I-th updated */
/*         (undeflated) singular value and the I-th (undeflated) old */
/*         singular value. */

/*  DIFR   (output) DOUBLE PRECISION array, */
/*                  dimension ( LDGNUM, 2 ) if ICOMPQ = 1 and */
/*                  dimension ( N ) if ICOMPQ = 0. */
/*         On exit, DIFR(I, 1) is the distance between I-th updated */
/*         (undeflated) singular value and the I+1-th (undeflated) old */
/*         singular value. */

/*         If ICOMPQ = 1, DIFR(1:K,2) is an array containing the */
/*         normalizing factors for the right singular vector matrix. */

/*         See DLASD8 for details on DIFL and DIFR. */

/*  Z      (output) DOUBLE PRECISION array, dimension ( M ) */
/*         The first elements of this array contain the components */
/*         of the deflation-adjusted updating row vector. */

/*  K      (output) INTEGER */
/*         Contains the dimension of the non-deflated matrix, */
/*         This is the order of the related secular equation. 1 <= K <=N. */

/*  C      (output) DOUBLE PRECISION */
/*         C contains garbage if SQRE =0 and the C-value of a Givens */
/*         rotation related to the right null space if SQRE = 1. */

/*  S      (output) DOUBLE PRECISION */
/*         S contains garbage if SQRE =0 and the S-value of a Givens */
/*         rotation related to the right null space if SQRE = 1. */

/*  WORK   (workspace) DOUBLE PRECISION array, dimension ( 4 * M ) */

/*  IWORK  (workspace) INTEGER array, dimension ( 3 * N ) */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, an singular value did not converge */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --d__;
    --vf;
    --vl;
    --idxq;
    --perm;
    givcol_dim1 = *ldgcol;
    givcol_offset = 1 + givcol_dim1;
    givcol -= givcol_offset;
    poles_dim1 = *ldgnum;
    poles_offset = 1 + poles_dim1;
    poles -= poles_offset;
    givnum_dim1 = *ldgnum;
    givnum_offset = 1 + givnum_dim1;
    givnum -= givnum_offset;
    --difl;
    --difr;
    --z__;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    n = *nl + *nr + 1;
    m = n + *sqre;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*nl < 1) {
	*info = -2;
    } else if (*nr < 1) {
	*info = -3;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -4;
    } else if (*ldgcol < n) {
	*info = -14;
    } else if (*ldgnum < n) {
	*info = -16;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD6", &i__1);
	return 0;
    }

/*     The following values are for bookkeeping purposes only.  They are */
/*     integer pointers which indicate the portion of the workspace */
/*     used by a particular array in DLASD7 and DLASD8. */

    isigma = 1;
    iw = isigma + n;
    ivfw = iw + m;
    ivlw = ivfw + m;

    idx = 1;
    idxc = idx + n;
    idxp = idxc + n;

/*     Scale. */

/* Computing MAX */
    d__1 = abs(*alpha), d__2 = abs(*beta);
    orgnrm = std::max(d__1,d__2);
    d__[*nl + 1] = 0.;
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = d__[i__], abs(d__1)) > orgnrm) {
	    orgnrm = (d__1 = d__[i__], abs(d__1));
	}
/* L10: */
    }
    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b7, &n, &c__1, &d__[1], &n, info);
    *alpha /= orgnrm;
    *beta /= orgnrm;

/*     Sort and Deflate singular values. */

    dlasd7_(icompq, nl, nr, sqre, k, &d__[1], &z__[1], &work[iw], &vf[1], &
	    work[ivfw], &vl[1], &work[ivlw], alpha, beta, &work[isigma], &
	    iwork[idx], &iwork[idxp], &idxq[1], &perm[1], givptr, &givcol[
	    givcol_offset], ldgcol, &givnum[givnum_offset], ldgnum, c__, s,
	    info);

/*     Solve Secular Equation, compute DIFL, DIFR, and update VF, VL. */

    dlasd8_(icompq, k, &d__[1], &z__[1], &vf[1], &vl[1], &difl[1], &difr[1],
	    ldgnum, &work[isigma], &work[iw], info);

/*     Save the poles if ICOMPQ = 1. */

    if (*icompq == 1) {
	dcopy_(k, &d__[1], &c__1, &poles[poles_dim1 + 1], &c__1);
	dcopy_(k, &work[isigma], &c__1, &poles[(poles_dim1 << 1) + 1], &c__1);
    }

/*     Unscale. */

    dlascl_("G", &c__0, &c__0, &c_b7, &orgnrm, &n, &c__1, &d__[1], &n, info);

/*     Prepare the IDXQ sorting permutation. */

    n1 = *k;
    n2 = n - *k;
    dlamrg_(&n1, &n2, &d__[1], &c__1, &c_n1, &idxq[1]);

    return 0;

/*     End of DLASD6 */

} /* dlasd6_ */

/* Subroutine */ int dlasd7_(integer *icompq, integer *nl, integer *nr,
	integer *sqre, integer *k, double *d__, double *z__,
	double *zw, double *vf, double *vfw, double *vl,
	double *vlw, double *alpha, double *beta, double *
	dsigma, integer *idx, integer *idxp, integer *idxq, integer *perm,
	integer *givptr, integer *givcol, integer *ldgcol, double *givnum,
	integer *ldgnum, double *c__, double *s, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer givcol_dim1, givcol_offset, givnum_dim1, givnum_offset, i__1;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, m, n, k2;
    double z1;
    integer jp;
    double eps, tau, tol;
    integer nlp1, nlp2, idxi, idxj;
    integer idxjp;
    integer jprev;
    double hlftol;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASD7 merges the two sets of singular values together into a single */
/*  sorted set. Then it tries to deflate the size of the problem. There */
/*  are two ways in which deflation can occur:  when two or more singular */
/*  values are close together or if there is a tiny entry in the Z */
/*  vector. For each such occurrence the order of the related */
/*  secular equation problem is reduced by one. */

/*  DLASD7 is called from DLASD6. */

/*  Arguments */
/*  ========= */

/*  ICOMPQ  (input) INTEGER */
/*          Specifies whether singular vectors are to be computed */
/*          in compact form, as follows: */
/*          = 0: Compute singular values only. */
/*          = 1: Compute singular vectors of upper */
/*               bidiagonal matrix in compact form. */

/*  NL     (input) INTEGER */
/*         The row dimension of the upper block. NL >= 1. */

/*  NR     (input) INTEGER */
/*         The row dimension of the lower block. NR >= 1. */

/*  SQRE   (input) INTEGER */
/*         = 0: the lower block is an NR-by-NR square matrix. */
/*         = 1: the lower block is an NR-by-(NR+1) rectangular matrix. */

/*         The bidiagonal matrix has */
/*         N = NL + NR + 1 rows and */
/*         M = N + SQRE >= N columns. */

/*  K      (output) INTEGER */
/*         Contains the dimension of the non-deflated matrix, this is */
/*         the order of the related secular equation. 1 <= K <=N. */

/*  D      (input/output) DOUBLE PRECISION array, dimension ( N ) */
/*         On entry D contains the singular values of the two submatrices */
/*         to be combined. On exit D contains the trailing (N-K) updated */
/*         singular values (those which were deflated) sorted into */
/*         increasing order. */

/*  Z      (output) DOUBLE PRECISION array, dimension ( M ) */
/*         On exit Z contains the updating row vector in the secular */
/*         equation. */

/*  ZW     (workspace) DOUBLE PRECISION array, dimension ( M ) */
/*         Workspace for Z. */

/*  VF     (input/output) DOUBLE PRECISION array, dimension ( M ) */
/*         On entry, VF(1:NL+1) contains the first components of all */
/*         right singular vectors of the upper block; and VF(NL+2:M) */
/*         contains the first components of all right singular vectors */
/*         of the lower block. On exit, VF contains the first components */
/*         of all right singular vectors of the bidiagonal matrix. */

/*  VFW    (workspace) DOUBLE PRECISION array, dimension ( M ) */
/*         Workspace for VF. */

/*  VL     (input/output) DOUBLE PRECISION array, dimension ( M ) */
/*         On entry, VL(1:NL+1) contains the  last components of all */
/*         right singular vectors of the upper block; and VL(NL+2:M) */
/*         contains the last components of all right singular vectors */
/*         of the lower block. On exit, VL contains the last components */
/*         of all right singular vectors of the bidiagonal matrix. */

/*  VLW    (workspace) DOUBLE PRECISION array, dimension ( M ) */
/*         Workspace for VL. */

/*  ALPHA  (input) DOUBLE PRECISION */
/*         Contains the diagonal element associated with the added row. */

/*  BETA   (input) DOUBLE PRECISION */
/*         Contains the off-diagonal element associated with the added */
/*         row. */

/*  DSIGMA (output) DOUBLE PRECISION array, dimension ( N ) */
/*         Contains a copy of the diagonal elements (K-1 singular values */
/*         and one zero) in the secular equation. */

/*  IDX    (workspace) INTEGER array, dimension ( N ) */
/*         This will contain the permutation used to sort the contents of */
/*         D into ascending order. */

/*  IDXP   (workspace) INTEGER array, dimension ( N ) */
/*         This will contain the permutation used to place deflated */
/*         values of D at the end of the array. On output IDXP(2:K) */
/*         points to the nondeflated D-values and IDXP(K+1:N) */
/*         points to the deflated singular values. */

/*  IDXQ   (input) INTEGER array, dimension ( N ) */
/*         This contains the permutation which separately sorts the two */
/*         sub-problems in D into ascending order.  Note that entries in */
/*         the first half of this permutation must first be moved one */
/*         position backward; and entries in the second half */
/*         must first have NL+1 added to their values. */

/*  PERM   (output) INTEGER array, dimension ( N ) */
/*         The permutations (from deflation and sorting) to be applied */
/*         to each singular block. Not referenced if ICOMPQ = 0. */

/*  GIVPTR (output) INTEGER */
/*         The number of Givens rotations which took place in this */
/*         subproblem. Not referenced if ICOMPQ = 0. */

/*  GIVCOL (output) INTEGER array, dimension ( LDGCOL, 2 ) */
/*         Each pair of numbers indicates a pair of columns to take place */
/*         in a Givens rotation. Not referenced if ICOMPQ = 0. */

/*  LDGCOL (input) INTEGER */
/*         The leading dimension of GIVCOL, must be at least N. */

/*  GIVNUM (output) DOUBLE PRECISION array, dimension ( LDGNUM, 2 ) */
/*         Each number indicates the C or S value to be used in the */
/*         corresponding Givens rotation. Not referenced if ICOMPQ = 0. */

/*  LDGNUM (input) INTEGER */
/*         The leading dimension of GIVNUM, must be at least N. */

/*  C      (output) DOUBLE PRECISION */
/*         C contains garbage if SQRE =0 and the C-value of a Givens */
/*         rotation related to the right null space if SQRE = 1. */

/*  S      (output) DOUBLE PRECISION */
/*         S contains garbage if SQRE =0 and the S-value of a Givens */
/*         rotation related to the right null space if SQRE = 1. */

/*  INFO   (output) INTEGER */
/*         = 0:  successful exit. */
/*         < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */

/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --d__;
    --z__;
    --zw;
    --vf;
    --vfw;
    --vl;
    --vlw;
    --dsigma;
    --idx;
    --idxp;
    --idxq;
    --perm;
    givcol_dim1 = *ldgcol;
    givcol_offset = 1 + givcol_dim1;
    givcol -= givcol_offset;
    givnum_dim1 = *ldgnum;
    givnum_offset = 1 + givnum_dim1;
    givnum -= givnum_offset;

    /* Function Body */
    *info = 0;
    n = *nl + *nr + 1;
    m = n + *sqre;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*nl < 1) {
	*info = -2;
    } else if (*nr < 1) {
	*info = -3;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -4;
    } else if (*ldgcol < n) {
	*info = -22;
    } else if (*ldgnum < n) {
	*info = -24;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD7", &i__1);
	return 0;
    }

    nlp1 = *nl + 1;
    nlp2 = *nl + 2;
    if (*icompq == 1) {
	*givptr = 0;
    }

/*     Generate the first part of the vector Z and move the singular */
/*     values in the first part of D one position backward. */

    z1 = *alpha * vl[nlp1];
    vl[nlp1] = 0.;
    tau = vf[nlp1];
    for (i__ = *nl; i__ >= 1; --i__) {
	z__[i__ + 1] = *alpha * vl[i__];
	vl[i__] = 0.;
	vf[i__ + 1] = vf[i__];
	d__[i__ + 1] = d__[i__];
	idxq[i__ + 1] = idxq[i__] + 1;
/* L10: */
    }
    vf[1] = tau;

/*     Generate the second part of the vector Z. */

    i__1 = m;
    for (i__ = nlp2; i__ <= i__1; ++i__) {
	z__[i__] = *beta * vf[i__];
	vf[i__] = 0.;
/* L20: */
    }

/*     Sort the singular values into increasing order */

    i__1 = n;
    for (i__ = nlp2; i__ <= i__1; ++i__) {
	idxq[i__] += nlp1;
/* L30: */
    }

/*     DSIGMA, IDXC, IDXC, and ZW are used as storage space. */

    i__1 = n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	dsigma[i__] = d__[idxq[i__]];
	zw[i__] = z__[idxq[i__]];
	vfw[i__] = vf[idxq[i__]];
	vlw[i__] = vl[idxq[i__]];
/* L40: */
    }

    dlamrg_(nl, nr, &dsigma[2], &c__1, &c__1, &idx[2]);

    i__1 = n;
    for (i__ = 2; i__ <= i__1; ++i__) {
	idxi = idx[i__] + 1;
	d__[i__] = dsigma[idxi];
	z__[i__] = zw[idxi];
	vf[i__] = vfw[idxi];
	vl[i__] = vlw[idxi];
/* L50: */
    }

/*     Calculate the allowable deflation tolerence */

    eps = dlamch_("Epsilon");
/* Computing MAX */
    d__1 = abs(*alpha), d__2 = abs(*beta);
    tol = std::max(d__1,d__2);
/* Computing MAX */
    d__2 = (d__1 = d__[n], abs(d__1));
    tol = eps * 64. * std::max(d__2,tol);

/*     There are 2 kinds of deflation -- first a value in the z-vector */
/*     is small, second two (or more) singular values are very close */
/*     together (their difference is small). */

/*     If the value in the z-vector is small, we simply permute the */
/*     array so that the corresponding singular value is moved to the */
/*     end. */

/*     If two values in the D-vector are close, we perform a two-sided */
/*     rotation designed to make one of the corresponding z-vector */
/*     entries zero, and then permute the array so that the deflated */
/*     singular value is moved to the end. */

/*     If there are multiple singular values then the problem deflates. */
/*     Here the number of equal singular values are found.  As each equal */
/*     singular value is found, an elementary reflector is computed to */
/*     rotate the corresponding singular subspace so that the */
/*     corresponding components of Z are zero in this new basis. */

    *k = 1;
    k2 = n + 1;
    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	if ((d__1 = z__[j], abs(d__1)) <= tol) {

/*           Deflate due to small z component. */

	    --k2;
	    idxp[k2] = j;
	    if (j == n) {
		goto L100;
	    }
	} else {
	    jprev = j;
	    goto L70;
	}
/* L60: */
    }
L70:
    j = jprev;
L80:
    ++j;
    if (j > n) {
	goto L90;
    }
    if ((d__1 = z__[j], abs(d__1)) <= tol) {

/*        Deflate due to small z component. */

	--k2;
	idxp[k2] = j;
    } else {

/*        Check if singular values are close enough to allow deflation. */

	if ((d__1 = d__[j] - d__[jprev], abs(d__1)) <= tol) {

/*           Deflation is possible. */

	    *s = z__[jprev];
	    *c__ = z__[j];

/*           Find sqrt(a**2+b**2) without overflow or */
/*           destructive underflow. */

	    tau = dlapy2_(c__, s);
	    z__[j] = tau;
	    z__[jprev] = 0.;
	    *c__ /= tau;
	    *s = -(*s) / tau;

/*           Record the appropriate Givens rotation */

	    if (*icompq == 1) {
		++(*givptr);
		idxjp = idxq[idx[jprev] + 1];
		idxj = idxq[idx[j] + 1];
		if (idxjp <= nlp1) {
		    --idxjp;
		}
		if (idxj <= nlp1) {
		    --idxj;
		}
		givcol[*givptr + (givcol_dim1 << 1)] = idxjp;
		givcol[*givptr + givcol_dim1] = idxj;
		givnum[*givptr + (givnum_dim1 << 1)] = *c__;
		givnum[*givptr + givnum_dim1] = *s;
	    }
	    drot_(&c__1, &vf[jprev], &c__1, &vf[j], &c__1, c__, s);
	    drot_(&c__1, &vl[jprev], &c__1, &vl[j], &c__1, c__, s);
	    --k2;
	    idxp[k2] = jprev;
	    jprev = j;
	} else {
	    ++(*k);
	    zw[*k] = z__[jprev];
	    dsigma[*k] = d__[jprev];
	    idxp[*k] = jprev;
	    jprev = j;
	}
    }
    goto L80;
L90:

/*     Record the last singular value. */

    ++(*k);
    zw[*k] = z__[jprev];
    dsigma[*k] = d__[jprev];
    idxp[*k] = jprev;

L100:

/*     Sort the singular values into DSIGMA. The singular values which */
/*     were not deflated go into the first K slots of DSIGMA, except */
/*     that DSIGMA(1) is treated separately. */

    i__1 = n;
    for (j = 2; j <= i__1; ++j) {
	jp = idxp[j];
	dsigma[j] = d__[jp];
	vfw[j] = vf[jp];
	vlw[j] = vl[jp];
/* L110: */
    }
    if (*icompq == 1) {
	i__1 = n;
	for (j = 2; j <= i__1; ++j) {
	    jp = idxp[j];
	    perm[j] = idxq[idx[jp] + 1];
	    if (perm[j] <= nlp1) {
		--perm[j];
	    }
/* L120: */
	}
    }

/*     The deflated singular values go back into the last N - K slots of */
/*     D. */

    i__1 = n - *k;
    dcopy_(&i__1, &dsigma[*k + 1], &c__1, &d__[*k + 1], &c__1);

/*     Determine DSIGMA(1), DSIGMA(2), Z(1), VF(1), VL(1), VF(M), and */
/*     VL(M). */

    dsigma[1] = 0.;
    hlftol = tol / 2.;
    if (abs(dsigma[2]) <= hlftol) {
	dsigma[2] = hlftol;
    }
    if (m > n) {
	z__[1] = dlapy2_(&z1, &z__[m]);
	if (z__[1] <= tol) {
	    *c__ = 1.;
	    *s = 0.;
	    z__[1] = tol;
	} else {
	    *c__ = z1 / z__[1];
	    *s = -z__[m] / z__[1];
	}
	drot_(&c__1, &vf[m], &c__1, &vf[1], &c__1, c__, s);
	drot_(&c__1, &vl[m], &c__1, &vl[1], &c__1, c__, s);
    } else {
	if (abs(z1) <= tol) {
	    z__[1] = tol;
	} else {
	    z__[1] = z1;
	}
    }

/*     Restore Z, VF, and VL. */

    i__1 = *k - 1;
    dcopy_(&i__1, &zw[2], &c__1, &z__[2], &c__1);
    i__1 = n - 1;
    dcopy_(&i__1, &vfw[2], &c__1, &vf[2], &c__1);
    i__1 = n - 1;
    dcopy_(&i__1, &vlw[2], &c__1, &vl[2], &c__1);

    return 0;

/*     End of DLASD7 */

} /* dlasd7_ */

/* Subroutine */ int dlasd8_(integer *icompq, integer *k, double *d__,
	double *z__, double *vf, double *vl, double *difl,
	double *difr, integer *lddifr, double *dsigma, double *
	work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__0 = 0;
	static double c_b8 = 1.;

    /* System generated locals */
    integer difr_dim1, difr_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j;
    double dj, rho;
    integer iwk1, iwk2, iwk3;
	double temp;
    integer iwk2i, iwk3i;
    double diflj, difrj, dsigj;
    double dsigjp;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASD8 finds the square roots of the roots of the secular equation, */
/*  as defined by the values in DSIGMA and Z. It makes the appropriate */
/*  calls to DLASD4, and stores, for each  element in D, the distance */
/*  to its two nearest poles (elements in DSIGMA). It also updates */
/*  the arrays VF and VL, the first and last components of all the */
/*  right singular vectors of the original bidiagonal matrix. */

/*  DLASD8 is called from DLASD6. */

/*  Arguments */
/*  ========= */

/*  ICOMPQ  (input) INTEGER */
/*          Specifies whether singular vectors are to be computed in */
/*          factored form in the calling routine: */
/*          = 0: Compute singular values only. */
/*          = 1: Compute singular vectors in factored form as well. */

/*  K       (input) INTEGER */
/*          The number of terms in the rational function to be solved */
/*          by DLASD4.  K >= 1. */

/*  D       (output) DOUBLE PRECISION array, dimension ( K ) */
/*          On output, D contains the updated singular values. */

/*  Z       (input) DOUBLE PRECISION array, dimension ( K ) */
/*          The first K elements of this array contain the components */
/*          of the deflation-adjusted updating row vector. */

/*  VF      (input/output) DOUBLE PRECISION array, dimension ( K ) */
/*          On entry, VF contains  information passed through DBEDE8. */
/*          On exit, VF contains the first K components of the first */
/*          components of all right singular vectors of the bidiagonal */
/*          matrix. */

/*  VL      (input/output) DOUBLE PRECISION array, dimension ( K ) */
/*          On entry, VL contains  information passed through DBEDE8. */
/*          On exit, VL contains the first K components of the last */
/*          components of all right singular vectors of the bidiagonal */
/*          matrix. */

/*  DIFL    (output) DOUBLE PRECISION array, dimension ( K ) */
/*          On exit, DIFL(I) = D(I) - DSIGMA(I). */

/*  DIFR    (output) DOUBLE PRECISION array, */
/*                   dimension ( LDDIFR, 2 ) if ICOMPQ = 1 and */
/*                   dimension ( K ) if ICOMPQ = 0. */
/*          On exit, DIFR(I,1) = D(I) - DSIGMA(I+1), DIFR(K,1) is not */
/*          defined and will not be referenced. */

/*          If ICOMPQ = 1, DIFR(1:K,2) is an array containing the */
/*          normalizing factors for the right singular vector matrix. */

/*  LDDIFR  (input) INTEGER */
/*          The leading dimension of DIFR, must be at least K. */

/*  DSIGMA  (input) DOUBLE PRECISION array, dimension ( K ) */
/*          The first K elements of this array contain the old roots */
/*          of the deflated updating problem.  These are the poles */
/*          of the secular equation. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension at least 3 * K */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, an singular value did not converge */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --d__;
    --z__;
    --vf;
    --vl;
    --difl;
    difr_dim1 = *lddifr;
    difr_offset = 1 + difr_dim1;
    difr -= difr_offset;
    --dsigma;
    --work;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*k < 1) {
	*info = -2;
    } else if (*lddifr < *k) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASD8", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*k == 1) {
	d__[1] = abs(z__[1]);
	difl[1] = d__[1];
	if (*icompq == 1) {
	    difl[2] = 1.;
	    difr[(difr_dim1 << 1) + 1] = 1.;
	}
	return 0;
    }

/*     Modify values DSIGMA(i) to make sure all DSIGMA(i)-DSIGMA(j) can */
/*     be computed with high relative accuracy (barring over/underflow). */
/*     This is a problem on machines without a guard digit in */
/*     add/subtract (Cray XMP, Cray YMP, Cray C 90 and Cray 2). */
/*     The following code replaces DSIGMA(I) by 2*DSIGMA(I)-DSIGMA(I), */
/*     which on any of these machines zeros out the bottommost */
/*     bit of DSIGMA(I) if it is 1; this makes the subsequent */
/*     subtractions DSIGMA(I)-DSIGMA(J) unproblematic when cancellation */
/*     occurs. On binary machines with a guard digit (almost all */
/*     machines) it does not change DSIGMA(I) at all. On hexadecimal */
/*     and decimal machines with a guard digit, it slightly */
/*     changes the bottommost bits of DSIGMA(I). It does not account */
/*     for hexadecimal or decimal machines without guard digits */
/*     (we know of none). We use a subroutine call to compute */
/*     2*DSIGMA(I) to prevent optimizing compilers from eliminating */
/*     this code. */

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dsigma[i__] = dlamc3_(&dsigma[i__], &dsigma[i__]) - dsigma[i__];
/* L10: */
    }

/*     Book keeping. */

    iwk1 = 1;
    iwk2 = iwk1 + *k;
    iwk3 = iwk2 + *k;
    iwk2i = iwk2 - 1;
    iwk3i = iwk3 - 1;

/*     Normalize Z. */

    rho = dnrm2_(k, &z__[1], &c__1);
    dlascl_("G", &c__0, &c__0, &rho, &c_b8, k, &c__1, &z__[1], k, info);
    rho *= rho;

/*     Initialize WORK(IWK3). */

    dlaset_("A", k, &c__1, &c_b8, &c_b8, &work[iwk3], k);

/*     Compute the updated singular values, the arrays DIFL, DIFR, */
/*     and the updated Z. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	dlasd4_(k, &j, &dsigma[1], &z__[1], &work[iwk1], &rho, &d__[j], &work[
		iwk2], info);

/*        If the root finder fails, the computation is terminated. */

	if (*info != 0) {
	    return 0;
	}
	work[iwk3i + j] = work[iwk3i + j] * work[j] * work[iwk2i + j];
	difl[j] = -work[j];
	difr[j + difr_dim1] = -work[j + 1];
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[iwk3i + i__] = work[iwk3i + i__] * work[i__] * work[iwk2i +
		    i__] / (dsigma[i__] - dsigma[j]) / (dsigma[i__] + dsigma[
		    j]);
/* L20: */
	}
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    work[iwk3i + i__] = work[iwk3i + i__] * work[i__] * work[iwk2i +
		    i__] / (dsigma[i__] - dsigma[j]) / (dsigma[i__] + dsigma[
		    j]);
/* L30: */
	}
/* L40: */
    }

/*     Compute updated Z. */

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__2 = sqrt((d__1 = work[iwk3i + i__], abs(d__1)));
	z__[i__] = d_sign(&d__2, &z__[i__]);
/* L50: */
    }

/*     Update VF and VL. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	diflj = difl[j];
	dj = d__[j];
	dsigj = -dsigma[j];
	if (j < *k) {
	    difrj = -difr[j + difr_dim1];
	    dsigjp = -dsigma[j + 1];
	}
	work[j] = -z__[j] / diflj / (dsigma[j] + dj);
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[i__] = z__[i__] / (dlamc3_(&dsigma[i__], &dsigj) - diflj) / (
		    dsigma[i__] + dj);
/* L60: */
	}
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    work[i__] = z__[i__] / (dlamc3_(&dsigma[i__], &dsigjp) + difrj) /
		    (dsigma[i__] + dj);
/* L70: */
	}
	temp = dnrm2_(k, &work[1], &c__1);
	work[iwk2i + j] = ddot_(k, &work[1], &c__1, &vf[1], &c__1) / temp;
	work[iwk3i + j] = ddot_(k, &work[1], &c__1, &vl[1], &c__1) / temp;
	if (*icompq == 1) {
	    difr[j + (difr_dim1 << 1)] = temp;
	}
/* L80: */
    }

    dcopy_(k, &work[iwk2], &c__1, &vf[1], &c__1);
    dcopy_(k, &work[iwk3], &c__1, &vl[1], &c__1);

    return 0;

/*     End of DLASD8 */

} /* dlasd8_ */

/* Subroutine */ int dlasda_(integer *icompq, integer *smlsiz, integer *n,
	integer *sqre, double *d__, double *e, double *u, integer
	*ldu, double *vt, integer *k, double *difl, double *difr,
	double *z__, double *poles, integer *givptr, integer *givcol,
	integer *ldgcol, integer *perm, double *givnum, double *c__,
	double *s, double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__0 = 0;
	static double c_b11 = 0.;
	static double c_b12 = 1.;
	static integer c__1 = 1;
	static integer c__2 = 2;

    /* System generated locals */
    integer givcol_dim1, givcol_offset, perm_dim1, perm_offset, difl_dim1,
	    difl_offset, difr_dim1, difr_offset, givnum_dim1, givnum_offset,
	    poles_dim1, poles_offset, u_dim1, u_offset, vt_dim1, vt_offset,
	    z_dim1, z_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, m, i1, ic, lf, nd, ll, nl, vf, nr, vl, im1, ncc, nlf, nrf,
	     vfi, iwk, vli, lvl, nru, ndb1, nlp1, lvl2, nrp1;
    double beta;
    integer idxq, nlvl;
    double alpha;
    integer inode, ndiml, ndimr, idxqi, itemp;
    integer sqrei;
    integer nwork1, nwork2;
	integer smlszp;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Using a divide and conquer approach, DLASDA computes the singular */
/*  value decomposition (SVD) of a real upper bidiagonal N-by-M matrix */
/*  B with diagonal D and offdiagonal E, where M = N + SQRE. The */
/*  algorithm computes the singular values in the SVD B = U * S * VT. */
/*  The orthogonal matrices U and VT are optionally computed in */
/*  compact form. */

/*  A related subroutine, DLASD0, computes the singular values and */
/*  the singular vectors in explicit form. */

/*  Arguments */
/*  ========= */

/*  ICOMPQ (input) INTEGER */
/*         Specifies whether singular vectors are to be computed */
/*         in compact form, as follows */
/*         = 0: Compute singular values only. */
/*         = 1: Compute singular vectors of upper bidiagonal */
/*              matrix in compact form. */

/*  SMLSIZ (input) INTEGER */
/*         The maximum size of the subproblems at the bottom of the */
/*         computation tree. */

/*  N      (input) INTEGER */
/*         The row dimension of the upper bidiagonal matrix. This is */
/*         also the dimension of the main diagonal array D. */

/*  SQRE   (input) INTEGER */
/*         Specifies the column dimension of the bidiagonal matrix. */
/*         = 0: The bidiagonal matrix has column dimension M = N; */
/*         = 1: The bidiagonal matrix has column dimension M = N + 1. */

/*  D      (input/output) DOUBLE PRECISION array, dimension ( N ) */
/*         On entry D contains the main diagonal of the bidiagonal */
/*         matrix. On exit D, if INFO = 0, contains its singular values. */

/*  E      (input) DOUBLE PRECISION array, dimension ( M-1 ) */
/*         Contains the subdiagonal entries of the bidiagonal matrix. */
/*         On exit, E has been destroyed. */

/*  U      (output) DOUBLE PRECISION array, */
/*         dimension ( LDU, SMLSIZ ) if ICOMPQ = 1, and not referenced */
/*         if ICOMPQ = 0. If ICOMPQ = 1, on exit, U contains the left */
/*         singular vector matrices of all subproblems at the bottom */
/*         level. */

/*  LDU    (input) INTEGER, LDU = > N. */
/*         The leading dimension of arrays U, VT, DIFL, DIFR, POLES, */
/*         GIVNUM, and Z. */

/*  VT     (output) DOUBLE PRECISION array, */
/*         dimension ( LDU, SMLSIZ+1 ) if ICOMPQ = 1, and not referenced */
/*         if ICOMPQ = 0. If ICOMPQ = 1, on exit, VT' contains the right */
/*         singular vector matrices of all subproblems at the bottom */
/*         level. */

/*  K      (output) INTEGER array, */
/*         dimension ( N ) if ICOMPQ = 1 and dimension 1 if ICOMPQ = 0. */
/*         If ICOMPQ = 1, on exit, K(I) is the dimension of the I-th */
/*         secular equation on the computation tree. */

/*  DIFL   (output) DOUBLE PRECISION array, dimension ( LDU, NLVL ), */
/*         where NLVL = floor(log_2 (N/SMLSIZ))). */

/*  DIFR   (output) DOUBLE PRECISION array, */
/*                  dimension ( LDU, 2 * NLVL ) if ICOMPQ = 1 and */
/*                  dimension ( N ) if ICOMPQ = 0. */
/*         If ICOMPQ = 1, on exit, DIFL(1:N, I) and DIFR(1:N, 2 * I - 1) */
/*         record distances between singular values on the I-th */
/*         level and singular values on the (I -1)-th level, and */
/*         DIFR(1:N, 2 * I ) contains the normalizing factors for */
/*         the right singular vector matrix. See DLASD8 for details. */

/*  Z      (output) DOUBLE PRECISION array, */
/*                  dimension ( LDU, NLVL ) if ICOMPQ = 1 and */
/*                  dimension ( N ) if ICOMPQ = 0. */
/*         The first K elements of Z(1, I) contain the components of */
/*         the deflation-adjusted updating row vector for subproblems */
/*         on the I-th level. */

/*  POLES  (output) DOUBLE PRECISION array, */
/*         dimension ( LDU, 2 * NLVL ) if ICOMPQ = 1, and not referenced */
/*         if ICOMPQ = 0. If ICOMPQ = 1, on exit, POLES(1, 2*I - 1) and */
/*         POLES(1, 2*I) contain  the new and old singular values */
/*         involved in the secular equations on the I-th level. */

/*  GIVPTR (output) INTEGER array, */
/*         dimension ( N ) if ICOMPQ = 1, and not referenced if */
/*         ICOMPQ = 0. If ICOMPQ = 1, on exit, GIVPTR( I ) records */
/*         the number of Givens rotations performed on the I-th */
/*         problem on the computation tree. */

/*  GIVCOL (output) INTEGER array, */
/*         dimension ( LDGCOL, 2 * NLVL ) if ICOMPQ = 1, and not */
/*         referenced if ICOMPQ = 0. If ICOMPQ = 1, on exit, for each I, */
/*         GIVCOL(1, 2 *I - 1) and GIVCOL(1, 2 *I) record the locations */
/*         of Givens rotations performed on the I-th level on the */
/*         computation tree. */

/*  LDGCOL (input) INTEGER, LDGCOL = > N. */
/*         The leading dimension of arrays GIVCOL and PERM. */

/*  PERM   (output) INTEGER array, */
/*         dimension ( LDGCOL, NLVL ) if ICOMPQ = 1, and not referenced */
/*         if ICOMPQ = 0. If ICOMPQ = 1, on exit, PERM(1, I) records */
/*         permutations done on the I-th level of the computation tree. */

/*  GIVNUM (output) DOUBLE PRECISION array, */
/*         dimension ( LDU,  2 * NLVL ) if ICOMPQ = 1, and not */
/*         referenced if ICOMPQ = 0. If ICOMPQ = 1, on exit, for each I, */
/*         GIVNUM(1, 2 *I - 1) and GIVNUM(1, 2 *I) record the C- and S- */
/*         values of Givens rotations performed on the I-th level on */
/*         the computation tree. */

/*  C      (output) DOUBLE PRECISION array, */
/*         dimension ( N ) if ICOMPQ = 1, and dimension 1 if ICOMPQ = 0. */
/*         If ICOMPQ = 1 and the I-th subproblem is not square, on exit, */
/*         C( I ) contains the C-value of a Givens rotation related to */
/*         the right null space of the I-th subproblem. */

/*  S      (output) DOUBLE PRECISION array, dimension ( N ) if */
/*         ICOMPQ = 1, and dimension 1 if ICOMPQ = 0. If ICOMPQ = 1 */
/*         and the I-th subproblem is not square, on exit, S( I ) */
/*         contains the S-value of a Givens rotation related to */
/*         the right null space of the I-th subproblem. */

/*  WORK   (workspace) DOUBLE PRECISION array, dimension */
/*         (6 * N + (SMLSIZ + 1)*(SMLSIZ + 1)). */

/*  IWORK  (workspace) INTEGER array. */
/*         Dimension must be at least (7 * N). */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, an singular value did not converge */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --d__;
    --e;
    givnum_dim1 = *ldu;
    givnum_offset = 1 + givnum_dim1;
    givnum -= givnum_offset;
    poles_dim1 = *ldu;
    poles_offset = 1 + poles_dim1;
    poles -= poles_offset;
    z_dim1 = *ldu;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    difr_dim1 = *ldu;
    difr_offset = 1 + difr_dim1;
    difr -= difr_offset;
    difl_dim1 = *ldu;
    difl_offset = 1 + difl_dim1;
    difl -= difl_offset;
    vt_dim1 = *ldu;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    --k;
    --givptr;
    perm_dim1 = *ldgcol;
    perm_offset = 1 + perm_dim1;
    perm -= perm_offset;
    givcol_dim1 = *ldgcol;
    givcol_offset = 1 + givcol_dim1;
    givcol -= givcol_offset;
    --c__;
    --s;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*smlsiz < 3) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -4;
    } else if (*ldu < *n + *sqre) {
	*info = -8;
    } else if (*ldgcol < *n) {
	*info = -17;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASDA", &i__1);
	return 0;
    }

    m = *n + *sqre;

/*     If the input matrix is too small, call DLASDQ to find the SVD. */

    if (*n <= *smlsiz) {
	if (*icompq == 0) {
	    dlasdq_("U", sqre, n, &c__0, &c__0, &c__0, &d__[1], &e[1], &vt[
		    vt_offset], ldu, &u[u_offset], ldu, &u[u_offset], ldu, &
		    work[1], info);
	} else {
	    dlasdq_("U", sqre, n, &m, n, &c__0, &d__[1], &e[1], &vt[vt_offset]
, ldu, &u[u_offset], ldu, &u[u_offset], ldu, &work[1],
		    info);
	}
	return 0;
    }

/*     Book-keeping and  set up the computation tree. */

    inode = 1;
    ndiml = inode + *n;
    ndimr = ndiml + *n;
    idxq = ndimr + *n;
    iwk = idxq + *n;

    ncc = 0;
    nru = 0;

    smlszp = *smlsiz + 1;
    vf = 1;
    vl = vf + m;
    nwork1 = vl + m;
    nwork2 = nwork1 + smlszp * smlszp;

    dlasdt_(n, &nlvl, &nd, &iwork[inode], &iwork[ndiml], &iwork[ndimr],
	    smlsiz);

/*     for the nodes on bottom level of the tree, solve */
/*     their subproblems by DLASDQ. */

    ndb1 = (nd + 1) / 2;
    i__1 = nd;
    for (i__ = ndb1; i__ <= i__1; ++i__) {

/*        IC : center row of each node */
/*        NL : number of rows of left  subproblem */
/*        NR : number of rows of right subproblem */
/*        NLF: starting row of the left   subproblem */
/*        NRF: starting row of the right  subproblem */

	i1 = i__ - 1;
	ic = iwork[inode + i1];
	nl = iwork[ndiml + i1];
	nlp1 = nl + 1;
	nr = iwork[ndimr + i1];
	nlf = ic - nl;
	nrf = ic + 1;
	idxqi = idxq + nlf - 2;
	vfi = vf + nlf - 1;
	vli = vl + nlf - 1;
	sqrei = 1;
	if (*icompq == 0) {
	    dlaset_("A", &nlp1, &nlp1, &c_b11, &c_b12, &work[nwork1], &smlszp);
	    dlasdq_("U", &sqrei, &nl, &nlp1, &nru, &ncc, &d__[nlf], &e[nlf], &
		    work[nwork1], &smlszp, &work[nwork2], &nl, &work[nwork2],
		    &nl, &work[nwork2], info);
	    itemp = nwork1 + nl * smlszp;
	    dcopy_(&nlp1, &work[nwork1], &c__1, &work[vfi], &c__1);
	    dcopy_(&nlp1, &work[itemp], &c__1, &work[vli], &c__1);
	} else {
	    dlaset_("A", &nl, &nl, &c_b11, &c_b12, &u[nlf + u_dim1], ldu);
	    dlaset_("A", &nlp1, &nlp1, &c_b11, &c_b12, &vt[nlf + vt_dim1],
		    ldu);
	    dlasdq_("U", &sqrei, &nl, &nlp1, &nl, &ncc, &d__[nlf], &e[nlf], &
		    vt[nlf + vt_dim1], ldu, &u[nlf + u_dim1], ldu, &u[nlf +
		    u_dim1], ldu, &work[nwork1], info);
	    dcopy_(&nlp1, &vt[nlf + vt_dim1], &c__1, &work[vfi], &c__1);
	    dcopy_(&nlp1, &vt[nlf + nlp1 * vt_dim1], &c__1, &work[vli], &c__1)
		    ;
	}
	if (*info != 0) {
	    return 0;
	}
	i__2 = nl;
	for (j = 1; j <= i__2; ++j) {
	    iwork[idxqi + j] = j;
/* L10: */
	}
	if (i__ == nd && *sqre == 0) {
	    sqrei = 0;
	} else {
	    sqrei = 1;
	}
	idxqi += nlp1;
	vfi += nlp1;
	vli += nlp1;
	nrp1 = nr + sqrei;
	if (*icompq == 0) {
	    dlaset_("A", &nrp1, &nrp1, &c_b11, &c_b12, &work[nwork1], &smlszp);
	    dlasdq_("U", &sqrei, &nr, &nrp1, &nru, &ncc, &d__[nrf], &e[nrf], &
		    work[nwork1], &smlszp, &work[nwork2], &nr, &work[nwork2],
		    &nr, &work[nwork2], info);
	    itemp = nwork1 + (nrp1 - 1) * smlszp;
	    dcopy_(&nrp1, &work[nwork1], &c__1, &work[vfi], &c__1);
	    dcopy_(&nrp1, &work[itemp], &c__1, &work[vli], &c__1);
	} else {
	    dlaset_("A", &nr, &nr, &c_b11, &c_b12, &u[nrf + u_dim1], ldu);
	    dlaset_("A", &nrp1, &nrp1, &c_b11, &c_b12, &vt[nrf + vt_dim1],
		    ldu);
	    dlasdq_("U", &sqrei, &nr, &nrp1, &nr, &ncc, &d__[nrf], &e[nrf], &
		    vt[nrf + vt_dim1], ldu, &u[nrf + u_dim1], ldu, &u[nrf +
		    u_dim1], ldu, &work[nwork1], info);
	    dcopy_(&nrp1, &vt[nrf + vt_dim1], &c__1, &work[vfi], &c__1);
	    dcopy_(&nrp1, &vt[nrf + nrp1 * vt_dim1], &c__1, &work[vli], &c__1)
		    ;
	}
	if (*info != 0) {
	    return 0;
	}
	i__2 = nr;
	for (j = 1; j <= i__2; ++j) {
	    iwork[idxqi + j] = j;
/* L20: */
	}
/* L30: */
    }

/*     Now conquer each subproblem bottom-up. */

    j = pow_ii(&c__2, &nlvl);
    for (lvl = nlvl; lvl >= 1; --lvl) {
	lvl2 = (lvl << 1) - 1;

/*        Find the first node LF and last node LL on */
/*        the current level LVL. */

	if (lvl == 1) {
	    lf = 1;
	    ll = 1;
	} else {
	    i__1 = lvl - 1;
	    lf = pow_ii(&c__2, &i__1);
	    ll = (lf << 1) - 1;
	}
	i__1 = ll;
	for (i__ = lf; i__ <= i__1; ++i__) {
	    im1 = i__ - 1;
	    ic = iwork[inode + im1];
	    nl = iwork[ndiml + im1];
	    nr = iwork[ndimr + im1];
	    nlf = ic - nl;
	    nrf = ic + 1;
	    if (i__ == ll) {
		sqrei = *sqre;
	    } else {
		sqrei = 1;
	    }
	    vfi = vf + nlf - 1;
	    vli = vl + nlf - 1;
	    idxqi = idxq + nlf - 1;
	    alpha = d__[ic];
	    beta = e[ic];
	    if (*icompq == 0) {
		dlasd6_(icompq, &nl, &nr, &sqrei, &d__[nlf], &work[vfi], &
			work[vli], &alpha, &beta, &iwork[idxqi], &perm[
			perm_offset], &givptr[1], &givcol[givcol_offset],
			ldgcol, &givnum[givnum_offset], ldu, &poles[
			poles_offset], &difl[difl_offset], &difr[difr_offset],
			 &z__[z_offset], &k[1], &c__[1], &s[1], &work[nwork1],
			 &iwork[iwk], info);
	    } else {
		--j;
		dlasd6_(icompq, &nl, &nr, &sqrei, &d__[nlf], &work[vfi], &
			work[vli], &alpha, &beta, &iwork[idxqi], &perm[nlf +
			lvl * perm_dim1], &givptr[j], &givcol[nlf + lvl2 *
			givcol_dim1], ldgcol, &givnum[nlf + lvl2 *
			givnum_dim1], ldu, &poles[nlf + lvl2 * poles_dim1], &
			difl[nlf + lvl * difl_dim1], &difr[nlf + lvl2 *
			difr_dim1], &z__[nlf + lvl * z_dim1], &k[j], &c__[j],
			&s[j], &work[nwork1], &iwork[iwk], info);
	    }
	    if (*info != 0) {
		return 0;
	    }
/* L40: */
	}
/* L50: */
    }

    return 0;

/*     End of DLASDA */

} /* dlasda_ */

/* Subroutine */ int dlasdq_(const char *uplo, integer *sqre, integer *n, integer *
	ncvt, integer *nru, integer *ncc, double *d__, double *e,
	double *vt, integer *ldvt, double *u, integer *ldu,
	double *c__, integer *ldc, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer c_dim1, c_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1,
	    i__2;

    /* Local variables */
    integer i__, j;
    double r__, cs, sn;
    integer np1, isub;
    double smin;
    integer sqre1;
    integer iuplo;
    bool rotate;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASDQ computes the singular value decomposition (SVD) of a real */
/*  (upper or lower) bidiagonal matrix with diagonal D and offdiagonal */
/*  E, accumulating the transformations if desired. Letting B denote */
/*  the input bidiagonal matrix, the algorithm computes orthogonal */
/*  matrices Q and P such that B = Q * S * P' (P' denotes the transpose */
/*  of P). The singular values S are overwritten on D. */

/*  The input matrix U  is changed to U  * Q  if desired. */
/*  The input matrix VT is changed to P' * VT if desired. */
/*  The input matrix C  is changed to Q' * C  if desired. */

/*  See "Computing  Small Singular Values of Bidiagonal Matrices With */
/*  Guaranteed High Relative Accuracy," by J. Demmel and W. Kahan, */
/*  LAPACK Working Note #3, for a detailed description of the algorithm. */

/*  Arguments */
/*  ========= */

/*  UPLO  (input) CHARACTER*1 */
/*        On entry, UPLO specifies whether the input bidiagonal matrix */
/*        is upper or lower bidiagonal, and wether it is square are */
/*        not. */
/*           UPLO = 'U' or 'u'   B is upper bidiagonal. */
/*           UPLO = 'L' or 'l'   B is lower bidiagonal. */

/*  SQRE  (input) INTEGER */
/*        = 0: then the input matrix is N-by-N. */
/*        = 1: then the input matrix is N-by-(N+1) if UPLU = 'U' and */
/*             (N+1)-by-N if UPLU = 'L'. */

/*        The bidiagonal matrix has */
/*        N = NL + NR + 1 rows and */
/*        M = N + SQRE >= N columns. */

/*  N     (input) INTEGER */
/*        On entry, N specifies the number of rows and columns */
/*        in the matrix. N must be at least 0. */

/*  NCVT  (input) INTEGER */
/*        On entry, NCVT specifies the number of columns of */
/*        the matrix VT. NCVT must be at least 0. */

/*  NRU   (input) INTEGER */
/*        On entry, NRU specifies the number of rows of */
/*        the matrix U. NRU must be at least 0. */

/*  NCC   (input) INTEGER */
/*        On entry, NCC specifies the number of columns of */
/*        the matrix C. NCC must be at least 0. */

/*  D     (input/output) DOUBLE PRECISION array, dimension (N) */
/*        On entry, D contains the diagonal entries of the */
/*        bidiagonal matrix whose SVD is desired. On normal exit, */
/*        D contains the singular values in ascending order. */

/*  E     (input/output) DOUBLE PRECISION array. */
/*        dimension is (N-1) if SQRE = 0 and N if SQRE = 1. */
/*        On entry, the entries of E contain the offdiagonal entries */
/*        of the bidiagonal matrix whose SVD is desired. On normal */
/*        exit, E will contain 0. If the algorithm does not converge, */
/*        D and E will contain the diagonal and superdiagonal entries */
/*        of a bidiagonal matrix orthogonally equivalent to the one */
/*        given as input. */

/*  VT    (input/output) DOUBLE PRECISION array, dimension (LDVT, NCVT) */
/*        On entry, contains a matrix which on exit has been */
/*        premultiplied by P', dimension N-by-NCVT if SQRE = 0 */
/*        and (N+1)-by-NCVT if SQRE = 1 (not referenced if NCVT=0). */

/*  LDVT  (input) INTEGER */
/*        On entry, LDVT specifies the leading dimension of VT as */
/*        declared in the calling (sub) program. LDVT must be at */
/*        least 1. If NCVT is nonzero LDVT must also be at least N. */

/*  U     (input/output) DOUBLE PRECISION array, dimension (LDU, N) */
/*        On entry, contains a  matrix which on exit has been */
/*        postmultiplied by Q, dimension NRU-by-N if SQRE = 0 */
/*        and NRU-by-(N+1) if SQRE = 1 (not referenced if NRU=0). */

/*  LDU   (input) INTEGER */
/*        On entry, LDU  specifies the leading dimension of U as */
/*        declared in the calling (sub) program. LDU must be at */
/*        least max( 1, NRU ) . */

/*  C     (input/output) DOUBLE PRECISION array, dimension (LDC, NCC) */
/*        On entry, contains an N-by-NCC matrix which on exit */
/*        has been premultiplied by Q'  dimension N-by-NCC if SQRE = 0 */
/*        and (N+1)-by-NCC if SQRE = 1 (not referenced if NCC=0). */

/*  LDC   (input) INTEGER */
/*        On entry, LDC  specifies the leading dimension of C as */
/*        declared in the calling (sub) program. LDC must be at */
/*        least 1. If NCC is nonzero, LDC must also be at least N. */

/*  WORK  (workspace) DOUBLE PRECISION array, dimension (4*N) */
/*        Workspace. Only referenced if one of NCVT, NRU, or NCC is */
/*        nonzero, and if N is at least 2. */

/*  INFO  (output) INTEGER */
/*        On exit, a value of 0 indicates a successful exit. */
/*        If INFO < 0, argument number -INFO is illegal. */
/*        If INFO > 0, the algorithm did not converge, and INFO */
/*        specifies how many superdiagonals did not converge. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --d__;
    --e;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    *info = 0;
    iuplo = 0;
    if (lsame_(uplo, "U")) {
	iuplo = 1;
    }
    if (lsame_(uplo, "L")) {
	iuplo = 2;
    }
    if (iuplo == 0) {
	*info = -1;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ncvt < 0) {
	*info = -4;
    } else if (*nru < 0) {
	*info = -5;
    } else if (*ncc < 0) {
	*info = -6;
    } else if (*ncvt == 0 && *ldvt < 1 || *ncvt > 0 && *ldvt < std::max(1_integer,*n)) {
	*info = -10;
    } else if (*ldu < std::max(1_integer,*nru)) {
	*info = -12;
    } else if (*ncc == 0 && *ldc < 1 || *ncc > 0 && *ldc < std::max(1_integer,*n)) {
	*info = -14;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASDQ", &i__1);
	return 0;
    }
    if (*n == 0) {
	return 0;
    }

/*     ROTATE is true if any singular vectors desired, false otherwise */

    rotate = *ncvt > 0 || *nru > 0 || *ncc > 0;
    np1 = *n + 1;
    sqre1 = *sqre;

/*     If matrix non-square upper bidiagonal, rotate to be lower */
/*     bidiagonal.  The rotations are on the right. */

    if (iuplo == 1 && sqre1 == 1) {
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlartg_(&d__[i__], &e[i__], &cs, &sn, &r__);
	    d__[i__] = r__;
	    e[i__] = sn * d__[i__ + 1];
	    d__[i__ + 1] = cs * d__[i__ + 1];
	    if (rotate) {
		work[i__] = cs;
		work[*n + i__] = sn;
	    }
/* L10: */
	}
	dlartg_(&d__[*n], &e[*n], &cs, &sn, &r__);
	d__[*n] = r__;
	e[*n] = 0.;
	if (rotate) {
	    work[*n] = cs;
	    work[*n + *n] = sn;
	}
	iuplo = 2;
	sqre1 = 0;

/*        Update singular vectors if desired. */

	if (*ncvt > 0) {
	    dlasr_("L", "V", "F", &np1, ncvt, &work[1], &work[np1], &vt[
		    vt_offset], ldvt);
	}
    }

/*     If matrix lower bidiagonal, rotate to be upper bidiagonal */
/*     by applying Givens rotations on the left. */

    if (iuplo == 2) {
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlartg_(&d__[i__], &e[i__], &cs, &sn, &r__);
	    d__[i__] = r__;
	    e[i__] = sn * d__[i__ + 1];
	    d__[i__ + 1] = cs * d__[i__ + 1];
	    if (rotate) {
		work[i__] = cs;
		work[*n + i__] = sn;
	    }
/* L20: */
	}

/*        If matrix (N+1)-by-N lower bidiagonal, one additional */
/*        rotation is needed. */

	if (sqre1 == 1) {
	    dlartg_(&d__[*n], &e[*n], &cs, &sn, &r__);
	    d__[*n] = r__;
	    if (rotate) {
		work[*n] = cs;
		work[*n + *n] = sn;
	    }
	}

/*        Update singular vectors if desired. */

	if (*nru > 0) {
	    if (sqre1 == 0) {
		dlasr_("R", "V", "F", nru, n, &work[1], &work[np1], &u[
			u_offset], ldu);
	    } else {
		dlasr_("R", "V", "F", nru, &np1, &work[1], &work[np1], &u[
			u_offset], ldu);
	    }
	}
	if (*ncc > 0) {
	    if (sqre1 == 0) {
		dlasr_("L", "V", "F", n, ncc, &work[1], &work[np1], &c__[
			c_offset], ldc);
	    } else {
		dlasr_("L", "V", "F", &np1, ncc, &work[1], &work[np1], &c__[
			c_offset], ldc);
	    }
	}
    }

/*     Call DBDSQR to compute the SVD of the reduced real */
/*     N-by-N upper bidiagonal matrix. */

    dbdsqr_("U", n, ncvt, nru, ncc, &d__[1], &e[1], &vt[vt_offset], ldvt, &u[
	    u_offset], ldu, &c__[c_offset], ldc, &work[1], info);

/*     Sort the singular values into ascending order (insertion sort on */
/*     singular values, but only one transposition per singular vector) */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Scan for smallest D(I). */

	isub = i__;
	smin = d__[i__];
	i__2 = *n;
	for (j = i__ + 1; j <= i__2; ++j) {
	    if (d__[j] < smin) {
		isub = j;
		smin = d__[j];
	    }
/* L30: */
	}
	if (isub != i__) {

/*           Swap singular values and vectors. */

	    d__[isub] = d__[i__];
	    d__[i__] = smin;
	    if (*ncvt > 0) {
		dswap_(ncvt, &vt[isub + vt_dim1], ldvt, &vt[i__ + vt_dim1],
			ldvt);
	    }
	    if (*nru > 0) {
		dswap_(nru, &u[isub * u_dim1 + 1], &c__1, &u[i__ * u_dim1 + 1]
, &c__1);
	    }
	    if (*ncc > 0) {
		dswap_(ncc, &c__[isub + c_dim1], ldc, &c__[i__ + c_dim1], ldc)
			;
	    }
	}
/* L40: */
    }

    return 0;

/*     End of DLASDQ */

} /* dlasdq_ */

/* Subroutine */ int dlasdt_(integer *n, integer *lvl, integer *nd, integer *
	inode, integer *ndiml, integer *ndimr, integer *msub)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer i__, il, ir, maxn;
    double temp;
    integer nlvl, llst, ncrnt;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASDT creates a tree of subproblems for bidiagonal divide and */
/*  conquer. */

/*  Arguments */
/*  ========= */

/*   N      (input) INTEGER */
/*          On entry, the number of diagonal elements of the */
/*          bidiagonal matrix. */

/*   LVL    (output) INTEGER */
/*          On exit, the number of levels on the computation tree. */

/*   ND     (output) INTEGER */
/*          On exit, the number of nodes on the tree. */

/*   INODE  (output) INTEGER array, dimension ( N ) */
/*          On exit, centers of subproblems. */

/*   NDIML  (output) INTEGER array, dimension ( N ) */
/*          On exit, row dimensions of left children. */

/*   NDIMR  (output) INTEGER array, dimension ( N ) */
/*          On exit, row dimensions of right children. */

/*   MSUB   (input) INTEGER. */
/*          On entry, the maximum row dimension each subproblem at the */
/*          bottom of the tree can be of. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Find the number of levels on the tree. */

    /* Parameter adjustments */
    --ndimr;
    --ndiml;
    --inode;

    /* Function Body */
    maxn = std::max(1_integer,*n);
    temp = log((double) maxn / (double) (*msub + 1)) / log(2.);
    *lvl = (integer) temp + 1;

    i__ = *n / 2;
    inode[1] = i__ + 1;
    ndiml[1] = i__;
    ndimr[1] = *n - i__ - 1;
    il = 0;
    ir = 1;
    llst = 1;
    i__1 = *lvl - 1;
    for (nlvl = 1; nlvl <= i__1; ++nlvl) {

/*        Constructing the tree at (NLVL+1)-st level. The number of */
/*        nodes created on this level is LLST * 2. */

	i__2 = llst - 1;
	for (i__ = 0; i__ <= i__2; ++i__) {
	    il += 2;
	    ir += 2;
	    ncrnt = llst + i__;
	    ndiml[il] = ndiml[ncrnt] / 2;
	    ndimr[il] = ndiml[ncrnt] - ndiml[il] - 1;
	    inode[il] = inode[ncrnt] - ndimr[il] - 1;
	    ndiml[ir] = ndimr[ncrnt] / 2;
	    ndimr[ir] = ndimr[ncrnt] - ndiml[ir] - 1;
	    inode[ir] = inode[ncrnt] + ndiml[ir] + 1;
/* L10: */
	}
	llst  <<=  1;
/* L20: */
    }
    *nd = (llst << 1) - 1;

    return 0;

/*     End of DLASDT */

} /* dlasdt_ */

/* Subroutine */ int dlaset_(const char *uplo, integer *m, integer *n, double *
	alpha, double *beta, double *a, integer *lda)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, j;



/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASET initializes an m-by-n matrix A to BETA on the diagonal and */
/*  ALPHA on the offdiagonals. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies the part of the matrix A to be set. */
/*          = 'U':      Upper triangular part is set; the strictly lower */
/*                      triangular part of A is not changed. */
/*          = 'L':      Lower triangular part is set; the strictly upper */
/*                      triangular part of A is not changed. */
/*          Otherwise:  All of the matrix A is set. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  ALPHA   (input) DOUBLE PRECISION */
/*          The constant to which the offdiagonal elements are to be set. */

/*  BETA    (input) DOUBLE PRECISION */
/*          The constant to which the diagonal elements are to be set. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On exit, the leading m-by-n submatrix of A is set as follows: */

/*          if UPLO = 'U', A(i,j) = ALPHA, 1<=i<=j-1, 1<=j<=n, */
/*          if UPLO = 'L', A(i,j) = ALPHA, j+1<=i<=m, 1<=j<=n, */
/*          otherwise,     A(i,j) = ALPHA, 1<=i<=m, 1<=j<=n, i.ne.j, */

/*          and, for all UPLO, A(i,i) = BETA, 1<=i<=min(m,n). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/* ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    if (lsame_(uplo, "U")) {

/*        Set the strictly upper triangular or trapezoidal part of the */
/*        array to ALPHA. */

	i__1 = *n;
	for (j = 2; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = j - 1;
	    i__2 = std::min(i__3,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = *alpha;
/* L10: */
	    }
/* L20: */
	}

    } else if (lsame_(uplo, "L")) {

/*        Set the strictly lower triangular or trapezoidal part of the */
/*        array to ALPHA. */

	i__1 = std::min(*m,*n);
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = j + 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = *alpha;
/* L30: */
	    }
/* L40: */
	}

    } else {

/*        Set the leading m-by-n submatrix to ALPHA. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = *alpha;
/* L50: */
	    }
/* L60: */
	}
    }

/*     Set the first min(M,N) diagonal elements to BETA. */

    i__1 = std::min(*m,*n);
    for (i__ = 1; i__ <= i__1; ++i__) {
	a[i__ + i__ * a_dim1] = *beta;
/* L70: */
    }

    return 0;

/*     End of DLASET */

} /* dlaset_ */

/* Subroutine */ int dlasq1_(integer *n, double *d__, double *e,
	double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__2 = 2;
	static integer c__0 = 0;

    /* System generated locals */
    integer i__1, i__2;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__;
    double eps;
    double scale;
    integer iinfo;
    double sigmn;
    double sigmx;
    double safmin;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASQ1 computes the singular values of a real N-by-N bidiagonal */
/*  matrix with diagonal D and off-diagonal E. The singular values */
/*  are computed to high relative accuracy, in the absence of */
/*  denormalization, underflow and overflow. The algorithm was first */
/*  presented in */

/*  "Accurate singular values and differential qd algorithms" by K. V. */
/*  Fernando and B. N. Parlett, Numer. Math., Vol-67, No. 2, pp. 191-230, */
/*  1994, */

/*  and the present implementation is described in "An implementation of */
/*  the dqds Algorithm (Positive Case)", LAPACK Working Note. */

/*  Arguments */
/*  ========= */

/*  N     (input) INTEGER */
/*        The number of rows and columns in the matrix. N >= 0. */

/*  D     (input/output) DOUBLE PRECISION array, dimension (N) */
/*        On entry, D contains the diagonal elements of the */
/*        bidiagonal matrix whose SVD is desired. On normal exit, */
/*        D contains the singular values in decreasing order. */

/*  E     (input/output) DOUBLE PRECISION array, dimension (N) */
/*        On entry, elements E(1:N-1) contain the off-diagonal elements */
/*        of the bidiagonal matrix whose SVD is desired. */
/*        On exit, E is overwritten. */

/*  WORK  (workspace) DOUBLE PRECISION array, dimension (4*N) */

/*  INFO  (output) INTEGER */
/*        = 0: successful exit */
/*        < 0: if INFO = -i, the i-th argument had an illegal value */
/*        > 0: the algorithm failed */
/*             = 1, a split was marked by a positive value in E */
/*             = 2, current block of Z not diagonalized after 30*N */
/*                  iterations (in inner while loop) */
/*             = 3, termination criterion of outer while loop not met */
/*                  (program created more than N unreduced blocks) */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --work;
    --e;
    --d__;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -2;
	i__1 = -(*info);
	xerbla_("DLASQ1", &i__1);
	return 0;
    } else if (*n == 0) {
	return 0;
    } else if (*n == 1) {
	d__[1] = abs(d__[1]);
	return 0;
    } else if (*n == 2) {
	dlas2_(&d__[1], &e[1], &d__[2], &sigmn, &sigmx);
	d__[1] = sigmx;
	d__[2] = sigmn;
	return 0;
    }

/*     Estimate the largest singular value. */

    sigmx = 0.;
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__[i__] = (d__1 = d__[i__], abs(d__1));
/* Computing MAX */
	d__2 = sigmx, d__3 = (d__1 = e[i__], abs(d__1));
	sigmx = std::max(d__2,d__3);
/* L10: */
    }
    d__[*n] = (d__1 = d__[*n], abs(d__1));

/*     Early return if SIGMX is zero (matrix is already diagonal). */

    if (sigmx == 0.) {
	dlasrt_("D", n, &d__[1], &iinfo);
	return 0;
    }

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	d__1 = sigmx, d__2 = d__[i__];
	sigmx = std::max(d__1,d__2);
/* L20: */
    }

/*     Copy D and E into WORK (in the Z format) and scale (squaring the */
/*     input data makes scaling by a power of the radix pointless). */

    eps = dlamch_("Precision");
    safmin = dlamch_("Safe minimum");
    scale = sqrt(eps / safmin);
    dcopy_(n, &d__[1], &c__1, &work[1], &c__2);
    i__1 = *n - 1;
    dcopy_(&i__1, &e[1], &c__1, &work[2], &c__2);
    i__1 = (*n << 1) - 1;
    i__2 = (*n << 1) - 1;
    dlascl_("G", &c__0, &c__0, &sigmx, &scale, &i__1, &c__1, &work[1], &i__2,
	    &iinfo);

/*     Compute the q's and e's. */

    i__1 = (*n << 1) - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	d__1 = work[i__];
	work[i__] = d__1 * d__1;
/* L30: */
    }
    work[*n * 2] = 0.;

    dlasq2_(n, &work[1], info);

    if (*info == 0) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d__[i__] = sqrt(work[i__]);
/* L40: */
	}
	dlascl_("G", &c__0, &c__0, &scale, &sigmx, n, &c__1, &d__[1], n, &
		iinfo);
    }

    return 0;

/*     End of DLASQ1 */

} /* dlasq1_ */

/* Subroutine */ int dlasq2_(integer *n, double *z__, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__2 = 2;
	static integer c__10 = 10;
	static integer c__3 = 3;
	static integer c__4 = 4;
	static integer c__11 = 11;

    /* System generated locals */
    integer i__1, i__2, i__3;
    double d__1, d__2;

    /* Local variables */
    double d__, e, g;
    integer k;
    double s, t;
    integer i0, i4, n0;
    double dn;
    integer pp;
    double dn1, dn2, dee, eps, tau, tol;
    integer ipn4;
    double tol2;
    bool ieee;
    integer nbig;
    double dmin__, emin, emax;
    integer kmin, ndiv, iter;
    double qmin, temp, qmax, zmax;
    integer splt;
    double dmin1, dmin2;
    integer nfail;
    double desig, trace, sigma;
    integer iinfo, ttype;
    double deemin;
    integer iwhila, iwhilb;
    double oldemn, safmin;


/*  -- LAPACK routine (version 3.2)                                    -- */

/*  -- Contributed by Osni Marques of the Lawrence Berkeley National   -- */
/*  -- Laboratory and Beresford Parlett of the Univ. of California at  -- */
/*  -- Berkeley                                                        -- */
/*  -- November 2008                                                   -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASQ2 computes all the eigenvalues of the symmetric positive */
/*  definite tridiagonal matrix associated with the qd array Z to high */
/*  relative accuracy are computed to high relative accuracy, in the */
/*  absence of denormalization, underflow and overflow. */

/*  To see the relation of Z to the tridiagonal matrix, let L be a */
/*  unit lower bidiagonal matrix with subdiagonals Z(2,4,6,,..) and */
/*  let U be an upper bidiagonal matrix with 1's above and diagonal */
/*  Z(1,3,5,,..). The tridiagonal is L*U or, if you prefer, the */
/*  symmetric tridiagonal to which it is similar. */

/*  Note : DLASQ2 defines a logical variable, IEEE, which is true */
/*  on machines which follow ieee-754 floating-point standard in their */
/*  handling of infinities and NaNs, and false otherwise. This variable */
/*  is passed to DLASQ3. */

/*  Arguments */
/*  ========= */

/*  N     (input) INTEGER */
/*        The number of rows and columns in the matrix. N >= 0. */

/*  Z     (input/output) DOUBLE PRECISION array, dimension ( 4*N ) */
/*        On entry Z holds the qd array. On exit, entries 1 to N hold */
/*        the eigenvalues in decreasing order, Z( 2*N+1 ) holds the */
/*        trace, and Z( 2*N+2 ) holds the sum of the eigenvalues. If */
/*        N > 2, then Z( 2*N+3 ) holds the iteration count, Z( 2*N+4 ) */
/*        holds NDIVS/NIN^2, and Z( 2*N+5 ) holds the percentage of */
/*        shifts that failed. */

/*  INFO  (output) INTEGER */
/*        = 0: successful exit */
/*        < 0: if the i-th argument is a scalar and had an illegal */
/*             value, then INFO = -i, if the i-th argument is an */
/*             array and the j-entry had an illegal value, then */
/*             INFO = -(i*100+j) */
/*        > 0: the algorithm failed */
/*              = 1, a split was marked by a positive value in E */
/*              = 2, current block of Z not diagonalized after 30*N */
/*                   iterations (in inner while loop) */
/*              = 3, termination criterion of outer while loop not met */
/*                   (program created more than N unreduced blocks) */

/*  Further Details */
/*  =============== */
/*  Local Variables: I0:N0 defines a current unreduced segment of Z. */
/*  The shifts are accumulated in SIGMA. Iteration count is in ITER. */
/*  Ping-pong is controlled by PP (alternates between 0 and 1). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input arguments. */
/*     (in case DLASQ2 is not called by DLASQ1) */

    /* Parameter adjustments */
    --z__;

    /* Function Body */
    *info = 0;
    eps = dlamch_("Precision");
    safmin = dlamch_("Safe minimum");
    tol = eps * 100.;
/* Computing 2nd power */
    d__1 = tol;
    tol2 = d__1 * d__1;

    if (*n < 0) {
	*info = -1;
	xerbla_("DLASQ2", &c__1);
	return 0;
    } else if (*n == 0) {
	return 0;
    } else if (*n == 1) {

/*        1-by-1 case. */

	if (z__[1] < 0.) {
	    *info = -201;
	    xerbla_("DLASQ2", &c__2);
	}
	return 0;
    } else if (*n == 2) {

/*        2-by-2 case. */

	if (z__[2] < 0. || z__[3] < 0.) {
	    *info = -2;
	    xerbla_("DLASQ2", &c__2);
	    return 0;
	} else if (z__[3] > z__[1]) {
	    d__ = z__[3];
	    z__[3] = z__[1];
	    z__[1] = d__;
	}
	z__[5] = z__[1] + z__[2] + z__[3];
	if (z__[2] > z__[3] * tol2) {
	    t = (z__[1] - z__[3] + z__[2]) * .5;
	    s = z__[3] * (z__[2] / t);
	    if (s <= t) {
		s = z__[3] * (z__[2] / (t * (sqrt(s / t + 1.) + 1.)));
	    } else {
		s = z__[3] * (z__[2] / (t + sqrt(t) * sqrt(t + s)));
	    }
	    t = z__[1] + (s + z__[2]);
	    z__[3] *= z__[1] / t;
	    z__[1] = t;
	}
	z__[2] = z__[3];
	z__[6] = z__[2] + z__[1];
	return 0;
    }

/*     Check for negative data and compute sums of q's and e's. */

    z__[*n * 2] = 0.;
    emin = z__[2];
    qmax = 0.;
    zmax = 0.;
    d__ = 0.;
    e = 0.;

    i__1 = *n - 1 << 1;
    for (k = 1; k <= i__1; k += 2) {
	if (z__[k] < 0.) {
	    *info = -(k + 200);
	    xerbla_("DLASQ2", &c__2);
	    return 0;
	} else if (z__[k + 1] < 0.) {
	    *info = -(k + 201);
	    xerbla_("DLASQ2", &c__2);
	    return 0;
	}
	d__ += z__[k];
	e += z__[k + 1];
/* Computing MAX */
	d__1 = qmax, d__2 = z__[k];
	qmax = std::max (d__1,d__2);
/* Computing MIN */
	d__1 = emin, d__2 = z__[k + 1];
	emin = std::min(d__1,d__2);
/* Computing MAX */
	d__1 = std::max (qmax,zmax), d__2 = z__[k + 1];
	zmax = std::max (d__1,d__2);
/* L10: */
    }
    if (z__[(*n << 1) - 1] < 0.) {
	*info = -((*n << 1) + 199);
	xerbla_("DLASQ2", &c__2);
	return 0;
    }
    d__ += z__[(*n << 1) - 1];
/* Computing MAX */
    d__1 = qmax, d__2 = z__[(*n << 1) - 1];
    qmax = std::max (d__1,d__2);
    zmax = std::max (qmax,zmax);

/*     Check for diagonality. */

    if (e == 0.) {
	i__1 = *n;
	for (k = 2; k <= i__1; ++k) {
	    z__[k] = z__[(k << 1) - 1];
/* L20: */
	}
	dlasrt_("D", n, &z__[1], &iinfo);
	z__[(*n << 1) - 1] = d__;
	return 0;
    }

    trace = d__ + e;

/*     Check for zero data. */

    if (trace == 0.) {
	z__[(*n << 1) - 1] = 0.;
	return 0;
    }

/*     Check whether the machine is IEEE conformable. */

    ieee = ilaenv_(&c__10, "DLASQ2", "N", &c__1, &c__2, &c__3, &c__4) == 1 && ilaenv_(&c__11, "DLASQ2", "N", &c__1, &c__2,
	     &c__3, &c__4) == 1;

/*     Rearrange data for locality: Z=(q1,qq1,e1,ee1,q2,qq2,e2,ee2,...). */

    for (k = *n << 1; k >= 2; k += -2) {
	z__[k * 2] = 0.;
	z__[(k << 1) - 1] = z__[k];
	z__[(k << 1) - 2] = 0.;
	z__[(k << 1) - 3] = z__[k - 1];
/* L30: */
    }

    i0 = 1;
    n0 = *n;

/*     Reverse the qd-array, if warranted. */

    if (z__[(i0 << 2) - 3] * 1.5 < z__[(n0 << 2) - 3]) {
	ipn4 = i0 + n0 << 2;
	i__1 = i0 + n0 - 1 << 1;
	for (i4 = i0 << 2; i4 <= i__1; i4 += 4) {
	    temp = z__[i4 - 3];
	    z__[i4 - 3] = z__[ipn4 - i4 - 3];
	    z__[ipn4 - i4 - 3] = temp;
	    temp = z__[i4 - 1];
	    z__[i4 - 1] = z__[ipn4 - i4 - 5];
	    z__[ipn4 - i4 - 5] = temp;
/* L40: */
	}
    }

/*     Initial split checking via dqd and Li's test. */

    pp = 0;

    for (k = 1; k <= 2; ++k) {

	d__ = z__[(n0 << 2) + pp - 3];
	i__1 = (i0 << 2) + pp;
	for (i4 = (n0 - 1 << 2) + pp; i4 >= i__1; i4 += -4) {
	    if (z__[i4 - 1] <= tol2 * d__) {
		z__[i4 - 1] = -0.;
		d__ = z__[i4 - 3];
	    } else {
		d__ = z__[i4 - 3] * (d__ / (d__ + z__[i4 - 1]));
	    }
/* L50: */
	}

/*        dqd maps Z to ZZ plus Li's test. */

	emin = z__[(i0 << 2) + pp + 1];
	d__ = z__[(i0 << 2) + pp - 3];
	i__1 = (n0 - 1 << 2) + pp;
	for (i4 = (i0 << 2) + pp; i4 <= i__1; i4 += 4) {
	    z__[i4 - (pp << 1) - 2] = d__ + z__[i4 - 1];
	    if (z__[i4 - 1] <= tol2 * d__) {
		z__[i4 - 1] = -0.;
		z__[i4 - (pp << 1) - 2] = d__;
		z__[i4 - (pp << 1)] = 0.;
		d__ = z__[i4 + 1];
	    } else if (safmin * z__[i4 + 1] < z__[i4 - (pp << 1) - 2] &&
		    safmin * z__[i4 - (pp << 1) - 2] < z__[i4 + 1]) {
		temp = z__[i4 + 1] / z__[i4 - (pp << 1) - 2];
		z__[i4 - (pp << 1)] = z__[i4 - 1] * temp;
		d__ *= temp;
	    } else {
		z__[i4 - (pp << 1)] = z__[i4 + 1] * (z__[i4 - 1] / z__[i4 - (
			pp << 1) - 2]);
		d__ = z__[i4 + 1] * (d__ / z__[i4 - (pp << 1) - 2]);
	    }
/* Computing MIN */
	    d__1 = emin, d__2 = z__[i4 - (pp << 1)];
	    emin = std::min(d__1,d__2);
/* L60: */
	}
	z__[(n0 << 2) - pp - 2] = d__;

/*        Now find qmax. */

	qmax = z__[(i0 << 2) - pp - 2];
	i__1 = (n0 << 2) - pp - 2;
	for (i4 = (i0 << 2) - pp + 2; i4 <= i__1; i4 += 4) {
/* Computing MAX */
	    d__1 = qmax, d__2 = z__[i4];
	    qmax = std::max (d__1,d__2);
/* L70: */
	}

/*        Prepare for the next iteration on K. */

	pp = 1 - pp;
/* L80: */
    }

/*     Initialise variables to pass to DLASQ3. */

    ttype = 0;
    dmin1 = 0.;
    dmin2 = 0.;
    dn = 0.;
    dn1 = 0.;
    dn2 = 0.;
    g = 0.;
    tau = 0.;

    iter = 2;
    nfail = 0;
    ndiv = n0 - i0 << 1;

    i__1 = *n + 1;
    for (iwhila = 1; iwhila <= i__1; ++iwhila) {
	if (n0 < 1) {
	    goto L170;
	}

/*        While array unfinished do */

/*        E(N0) holds the value of SIGMA when submatrix in I0:N0 */
/*        splits from the rest of the array, but is negated. */

	desig = 0.;
	if (n0 == *n) {
	    sigma = 0.;
	} else {
	    sigma = -z__[(n0 << 2) - 1];
	}
	if (sigma < 0.) {
	    *info = 1;
	    return 0;
	}

/*        Find last unreduced submatrix's top index I0, find QMAX and */
/*        EMIN. Find Gershgorin-type bound if Q's much greater than E's. */

	emax = 0.;
	if (n0 > i0) {
	    emin = (d__1 = z__[(n0 << 2) - 5], abs(d__1));
	} else {
	    emin = 0.;
	}
	qmin = z__[(n0 << 2) - 3];
	qmax = qmin;
	for (i4 = n0 << 2; i4 >= 8; i4 += -4) {
	    if (z__[i4 - 5] <= 0.) {
		goto L100;
	    }
	    if (qmin >= emax * 4.) {
/* Computing MIN */
		d__1 = qmin, d__2 = z__[i4 - 3];
		qmin = std::min(d__1,d__2);
/* Computing MAX */
		d__1 = emax, d__2 = z__[i4 - 5];
		emax = std::max (d__1,d__2);
	    }
/* Computing MAX */
	    d__1 = qmax, d__2 = z__[i4 - 7] + z__[i4 - 5];
	    qmax = std::max (d__1,d__2);
/* Computing MIN */
	    d__1 = emin, d__2 = z__[i4 - 5];
	    emin = std::min(d__1,d__2);
/* L90: */
	}
	i4 = 4;

L100:
	i0 = i4 / 4;
	pp = 0;

	if (n0 - i0 > 1) {
	    dee = z__[(i0 << 2) - 3];
	    deemin = dee;
	    kmin = i0;
	    i__2 = (n0 << 2) - 3;
	    for (i4 = (i0 << 2) + 1; i4 <= i__2; i4 += 4) {
		dee = z__[i4] * (dee / (dee + z__[i4 - 2]));
		if (dee <= deemin) {
		    deemin = dee;
		    kmin = (i4 + 3) / 4;
		}
/* L110: */
	    }
	    if (kmin - i0 << 1 < n0 - kmin && deemin <= z__[(n0 << 2) - 3] *
		    .5) {
		ipn4 = i0 + n0 << 2;
		pp = 2;
		i__2 = i0 + n0 - 1 << 1;
		for (i4 = i0 << 2; i4 <= i__2; i4 += 4) {
		    temp = z__[i4 - 3];
		    z__[i4 - 3] = z__[ipn4 - i4 - 3];
		    z__[ipn4 - i4 - 3] = temp;
		    temp = z__[i4 - 2];
		    z__[i4 - 2] = z__[ipn4 - i4 - 2];
		    z__[ipn4 - i4 - 2] = temp;
		    temp = z__[i4 - 1];
		    z__[i4 - 1] = z__[ipn4 - i4 - 5];
		    z__[ipn4 - i4 - 5] = temp;
		    temp = z__[i4];
		    z__[i4] = z__[ipn4 - i4 - 4];
		    z__[ipn4 - i4 - 4] = temp;
/* L120: */
		}
	    }
	}

/*        Put -(initial shift) into DMIN. */

/* Computing MAX */
	d__1 = 0., d__2 = qmin - sqrt(qmin) * 2. * sqrt(emax);
	dmin__ = -std::max (d__1,d__2);

/*        Now I0:N0 is unreduced. */
/*        PP = 0 for ping, PP = 1 for pong. */
/*        PP = 2 indicates that flipping was applied to the Z array and */
/*               and that the tests for deflation upon entry in DLASQ3 */
/*               should not be performed. */

	nbig = (n0 - i0 + 1) * 30;
	i__2 = nbig;
	for (iwhilb = 1; iwhilb <= i__2; ++iwhilb) {
	    if (i0 > n0) {
		goto L150;
	    }

/*           While submatrix unfinished take a good dqds step. */

	    dlasq3_(&i0, &n0, &z__[1], &pp, &dmin__, &sigma, &desig, &qmax, &
		    nfail, &iter, &ndiv, &ieee, &ttype, &dmin1, &dmin2, &dn, &
		    dn1, &dn2, &g, &tau);

	    pp = 1 - pp;

/*           When EMIN is very small check for splits. */

	    if (pp == 0 && n0 - i0 >= 3) {
		if (z__[n0 * 4] <= tol2 * qmax || z__[(n0 << 2) - 1] <= tol2 *
			 sigma) {
		    splt = i0 - 1;
		    qmax = z__[(i0 << 2) - 3];
		    emin = z__[(i0 << 2) - 1];
		    oldemn = z__[i0 * 4];
		    i__3 = n0 - 3 << 2;
		    for (i4 = i0 << 2; i4 <= i__3; i4 += 4) {
			if (z__[i4] <= tol2 * z__[i4 - 3] || z__[i4 - 1] <=
				tol2 * sigma) {
			    z__[i4 - 1] = -sigma;
			    splt = i4 / 4;
			    qmax = 0.;
			    emin = z__[i4 + 3];
			    oldemn = z__[i4 + 4];
			} else {
/* Computing MAX */
			    d__1 = qmax, d__2 = z__[i4 + 1];
			    qmax = std::max (d__1,d__2);
/* Computing MIN */
			    d__1 = emin, d__2 = z__[i4 - 1];
			    emin = std::min(d__1,d__2);
/* Computing MIN */
			    d__1 = oldemn, d__2 = z__[i4];
			    oldemn = std::min(d__1,d__2);
			}
/* L130: */
		    }
		    z__[(n0 << 2) - 1] = emin;
		    z__[n0 * 4] = oldemn;
		    i0 = splt + 1;
		}
	    }

/* L140: */
	}

	*info = 2;
	return 0;

/*        end IWHILB */

L150:

/* L160: */
	;
    }

    *info = 3;
    return 0;

/*     end IWHILA */

L170:

/*     Move q's to the front. */

    i__1 = *n;
    for (k = 2; k <= i__1; ++k) {
	z__[k] = z__[(k << 2) - 3];
/* L180: */
    }

/*     Sort and compute sum of eigenvalues. */

    dlasrt_("D", n, &z__[1], &iinfo);

    e = 0.;
    for (k = *n; k >= 1; --k) {
	e += z__[k];
/* L190: */
    }

/*     Store trace, sum(eigenvalues) and information on performance. */

    z__[(*n << 1) + 1] = trace;
    z__[(*n << 1) + 2] = e;
    z__[(*n << 1) + 3] = (double) iter;
/* Computing 2nd power */
    i__1 = *n;
    z__[(*n << 1) + 4] = (double) ndiv / (double) (i__1 * i__1);
    z__[(*n << 1) + 5] = nfail * 100. / (double) iter;
    return 0;

/*     End of DLASQ2 */

} /* dlasq2_ */

/* Subroutine */ int dlasq3_(integer *i0, integer *n0, double *z__, integer *pp, double *dmin__, double *sigma,
	double *desig, double *qmax, integer *nfail, integer *iter, integer *ndiv, bool *ieee,
	integer *ttype, double *dmin1, double *dmin2, double *dn, double *dn1, double *dn2,
	double *g, double *tau)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    double s, t;
    integer j4, nn;
    double eps, tol;
    integer n0in, ipn4;
    double tol2, temp;


/*  -- LAPACK routine (version 3.2)                                    -- */

/*  -- Contributed by Osni Marques of the Lawrence Berkeley National   -- */
/*  -- Laboratory and Beresford Parlett of the Univ. of California at  -- */
/*  -- Berkeley                                                        -- */
/*  -- November 2008                                                   -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASQ3 checks for deflation, computes a shift (TAU) and calls dqds. */
/*  In case of failure it changes shifts, and tries again until output */
/*  is positive. */

/*  Arguments */
/*  ========= */

/*  I0     (input) INTEGER */
/*         First index. */

/*  N0     (input) INTEGER */
/*         Last index. */

/*  Z      (input) DOUBLE PRECISION array, dimension ( 4*N ) */
/*         Z holds the qd array. */

/*  PP     (input/output) INTEGER */
/*         PP=0 for ping, PP=1 for pong. */
/*         PP=2 indicates that flipping was applied to the Z array */
/*         and that the initial tests for deflation should not be */
/*         performed. */

/*  DMIN   (output) DOUBLE PRECISION */
/*         Minimum value of d. */

/*  SIGMA  (output) DOUBLE PRECISION */
/*         Sum of shifts used in current segment. */

/*  DESIG  (input/output) DOUBLE PRECISION */
/*         Lower order part of SIGMA */

/*  QMAX   (input) DOUBLE PRECISION */
/*         Maximum value of q. */

/*  NFAIL  (output) INTEGER */
/*         Number of times shift was too big. */

/*  ITER   (output) INTEGER */
/*         Number of iterations. */

/*  NDIV   (output) INTEGER */
/*         Number of divisions. */

/*  IEEE   (input) LOGICAL */
/*         Flag for IEEE or non IEEE arithmetic (passed to DLASQ5). */

/*  TTYPE  (input/output) INTEGER */
/*         Shift type. */

/*  DMIN1, DMIN2, DN, DN1, DN2, G, TAU (input/output) DOUBLE PRECISION */
/*         These are passed as arguments in order to save their values */
/*         between calls to DLASQ3. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Function .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --z__;

    /* Function Body */
    n0in = *n0;
    eps = dlamch_("Precision");
    tol = eps * 100.;
/* Computing 2nd power */
    d__1 = tol;
    tol2 = d__1 * d__1;

/*     Check for deflation. */

L10:

    if (*n0 < *i0) {
	return 0;
    }
    if (*n0 == *i0) {
	goto L20;
    }
    nn = (*n0 << 2) + *pp;
    if (*n0 == *i0 + 1) {
	goto L40;
    }

/*     Check whether E(N0-1) is negligible, 1 eigenvalue. */

    if (z__[nn - 5] > tol2 * (*sigma + z__[nn - 3]) && z__[nn - (*pp << 1) -
	    4] > tol2 * z__[nn - 7]) {
	goto L30;
    }

L20:

    z__[(*n0 << 2) - 3] = z__[(*n0 << 2) + *pp - 3] + *sigma;
    --(*n0);
    goto L10;

/*     Check  whether E(N0-2) is negligible, 2 eigenvalues. */

L30:

    if (z__[nn - 9] > tol2 * *sigma && z__[nn - (*pp << 1) - 8] > tol2 * z__[
	    nn - 11]) {
	goto L50;
    }

L40:

    if (z__[nn - 3] > z__[nn - 7]) {
	s = z__[nn - 3];
	z__[nn - 3] = z__[nn - 7];
	z__[nn - 7] = s;
    }
    if (z__[nn - 5] > z__[nn - 3] * tol2) {
	t = (z__[nn - 7] - z__[nn - 3] + z__[nn - 5]) * .5;
	s = z__[nn - 3] * (z__[nn - 5] / t);
	if (s <= t) {
	    s = z__[nn - 3] * (z__[nn - 5] / (t * (sqrt(s / t + 1.) + 1.)));
	} else {
	    s = z__[nn - 3] * (z__[nn - 5] / (t + sqrt(t) * sqrt(t + s)));
	}
	t = z__[nn - 7] + (s + z__[nn - 5]);
	z__[nn - 3] *= z__[nn - 7] / t;
	z__[nn - 7] = t;
    }
    z__[(*n0 << 2) - 7] = z__[nn - 7] + *sigma;
    z__[(*n0 << 2) - 3] = z__[nn - 3] + *sigma;
    *n0 += -2;
    goto L10;

L50:
    if (*pp == 2) {
	*pp = 0;
    }

/*     Reverse the qd-array, if warranted. */

    if (*dmin__ <= 0. || *n0 < n0in) {
	if (z__[(*i0 << 2) + *pp - 3] * 1.5 < z__[(*n0 << 2) + *pp - 3]) {
	    ipn4 = *i0 + *n0 << 2;
	    i__1 = *i0 + *n0 - 1 << 1;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		temp = z__[j4 - 3];
		z__[j4 - 3] = z__[ipn4 - j4 - 3];
		z__[ipn4 - j4 - 3] = temp;
		temp = z__[j4 - 2];
		z__[j4 - 2] = z__[ipn4 - j4 - 2];
		z__[ipn4 - j4 - 2] = temp;
		temp = z__[j4 - 1];
		z__[j4 - 1] = z__[ipn4 - j4 - 5];
		z__[ipn4 - j4 - 5] = temp;
		temp = z__[j4];
		z__[j4] = z__[ipn4 - j4 - 4];
		z__[ipn4 - j4 - 4] = temp;
/* L60: */
	    }
	    if (*n0 - *i0 <= 4) {
		z__[(*n0 << 2) + *pp - 1] = z__[(*i0 << 2) + *pp - 1];
		z__[(*n0 << 2) - *pp] = z__[(*i0 << 2) - *pp];
	    }
/* Computing MIN */
	    d__1 = *dmin2, d__2 = z__[(*n0 << 2) + *pp - 1];
	    *dmin2 = std::min(d__1,d__2);
/* Computing MIN */
	    d__1 = z__[(*n0 << 2) + *pp - 1], d__2 = z__[(*i0 << 2) + *pp - 1]
		    , d__1 = std::min(d__1,d__2), d__2 = z__[(*i0 << 2) + *pp + 3];
	    z__[(*n0 << 2) + *pp - 1] = std::min(d__1,d__2);
/* Computing MIN */
	    d__1 = z__[(*n0 << 2) - *pp], d__2 = z__[(*i0 << 2) - *pp], d__1 =
		     std::min(d__1,d__2), d__2 = z__[(*i0 << 2) - *pp + 4];
	    z__[(*n0 << 2) - *pp] = std::min(d__1,d__2);
/* Computing MAX */
	    d__1 = *qmax, d__2 = z__[(*i0 << 2) + *pp - 3], d__1 = std::max(d__1,
		    d__2), d__2 = z__[(*i0 << 2) + *pp + 1];
	    *qmax = std::max(d__1,d__2);
	    *dmin__ = -0.;
	}
    }

/*     Choose a shift. */

    dlasq4_(i0, n0, &z__[1], pp, &n0in, dmin__, dmin1, dmin2, dn, dn1, dn2, tau, ttype, g);

/*     Call dqds until DMIN > 0. */

L70:

    dlasq5_(i0, n0, &z__[1], pp, tau, dmin__, dmin1, dmin2, dn, dn1, dn2,
	    ieee);

    *ndiv += *n0 - *i0 + 2;
    ++(*iter);

/*     Check status. */

    if (*dmin__ >= 0. && *dmin1 > 0.) {

/*        Success. */

	goto L90;

    } else if (*dmin__ < 0. && *dmin1 > 0. && z__[(*n0 - 1 << 2) - *pp] < tol
	    * (*sigma + *dn1) && abs(*dn) < tol * *sigma) {

/*        Convergence hidden by negative DN. */

	z__[(*n0 - 1 << 2) - *pp + 2] = 0.;
	*dmin__ = 0.;
	goto L90;
    } else if (*dmin__ < 0.) {

/*        TAU too big. Select new TAU and try again. */

	++(*nfail);
	if (*ttype < -22) {

/*           Failed twice. Play it safe. */

	    *tau = 0.;
	} else if (*dmin1 > 0.) {

/*           Late failure. Gives excellent shift. */

	    *tau = (*tau + *dmin__) * (1. - eps * 2.);
	    *ttype += -11;
	} else {

/*           Early failure. Divide by 4. */

	    *tau *= .25;
	    *ttype += -12;
	}
	goto L70;
    } else if (disnan_(dmin__)) {

/*        NaN. */

	if (*tau == 0.) {
	    goto L80;
	} else {
	    *tau = 0.;
	    goto L70;
	}
    } else {

/*        Possible underflow. Play it safe. */

	goto L80;
    }

/*     Risk of underflow. */

L80:
    dlasq6_(i0, n0, &z__[1], pp, dmin__, dmin1, dmin2, dn, dn1, dn2);
    *ndiv += *n0 - *i0 + 2;
    ++(*iter);
    *tau = 0.;

L90:
    if (*tau < *sigma) {
	*desig += *tau;
	t = *sigma + *desig;
	*desig -= t - *sigma;
    } else {
	t = *sigma + *tau;
	*desig = *sigma - (t - *tau) + *desig;
    }
    *sigma = t;

    return 0;

/*     End of DLASQ3 */

} /* dlasq3_ */

/* Subroutine */ int dlasq4_(integer *i0, integer *n0, double *z__,
	integer *pp, integer *n0in, double *dmin__, double *dmin1,
	double *dmin2, double *dn, double *dn1, double *dn2,
	double *tau, integer *ttype, double *g)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    double s, a2, b1, b2;
    integer i4, nn, np;
    double gam, gap1, gap2;


/*  -- LAPACK routine (version 3.2)                                    -- */

/*  -- Contributed by Osni Marques of the Lawrence Berkeley National   -- */
/*  -- Laboratory and Beresford Parlett of the Univ. of California at  -- */
/*  -- Berkeley                                                        -- */
/*  -- November 2008                                                   -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASQ4 computes an approximation TAU to the smallest eigenvalue */
/*  using values of d from the previous transform. */

/*  I0    (input) INTEGER */
/*        First index. */

/*  N0    (input) INTEGER */
/*        Last index. */

/*  Z     (input) DOUBLE PRECISION array, dimension ( 4*N ) */
/*        Z holds the qd array. */

/*  PP    (input) INTEGER */
/*        PP=0 for ping, PP=1 for pong. */

/*  NOIN  (input) INTEGER */
/*        The value of N0 at start of EIGTEST. */

/*  DMIN  (input) DOUBLE PRECISION */
/*        Minimum value of d. */

/*  DMIN1 (input) DOUBLE PRECISION */
/*        Minimum value of d, excluding D( N0 ). */

/*  DMIN2 (input) DOUBLE PRECISION */
/*        Minimum value of d, excluding D( N0 ) and D( N0-1 ). */

/*  DN    (input) DOUBLE PRECISION */
/*        d(N) */

/*  DN1   (input) DOUBLE PRECISION */
/*        d(N-1) */

/*  DN2   (input) DOUBLE PRECISION */
/*        d(N-2) */

/*  TAU   (output) DOUBLE PRECISION */
/*        This is the shift. */

/*  TTYPE (output) INTEGER */
/*        Shift type. */

/*  G     (input/output) REAL */
/*        G is passed as an argument in order to save its value between */
/*        calls to DLASQ4. */

/*  Further Details */
/*  =============== */
/*  CNST1 = 9/16 */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     A negative DMIN forces the shift to take that absolute value */
/*     TTYPE records the type of shift. */

    /* Parameter adjustments */
    --z__;

    /* Function Body */
    if (*dmin__ <= 0.) {
	*tau = -(*dmin__);
	*ttype = -1;
	return 0;
    }

    nn = (*n0 << 2) + *pp;
    if (*n0in == *n0) {

/*        No eigenvalues deflated. */

	if (*dmin__ == *dn || *dmin__ == *dn1) {

	    b1 = sqrt(z__[nn - 3]) * sqrt(z__[nn - 5]);
	    b2 = sqrt(z__[nn - 7]) * sqrt(z__[nn - 9]);
	    a2 = z__[nn - 7] + z__[nn - 5];

/*           Cases 2 and 3. */

	    if (*dmin__ == *dn && *dmin1 == *dn1) {
		gap2 = *dmin2 - a2 - *dmin2 * .25;
		if (gap2 > 0. && gap2 > b2) {
		    gap1 = a2 - *dn - b2 / gap2 * b2;
		} else {
		    gap1 = a2 - *dn - (b1 + b2);
		}
		if (gap1 > 0. && gap1 > b1) {
/* Computing MAX */
		    d__1 = *dn - b1 / gap1 * b1, d__2 = *dmin__ * .5;
		    s = std::max(d__1,d__2);
		    *ttype = -2;
		} else {
		    s = 0.;
		    if (*dn > b1) {
			s = *dn - b1;
		    }
		    if (a2 > b1 + b2) {
/* Computing MIN */
			d__1 = s, d__2 = a2 - (b1 + b2);
			s = std::min(d__1,d__2);
		    }
/* Computing MAX */
		    d__1 = s, d__2 = *dmin__ * .333;
		    s = std::max(d__1,d__2);
		    *ttype = -3;
		}
	    } else {

/*              Case 4. */

		*ttype = -4;
		s = *dmin__ * .25;
		if (*dmin__ == *dn) {
		    gam = *dn;
		    a2 = 0.;
		    if (z__[nn - 5] > z__[nn - 7]) {
			return 0;
		    }
		    b2 = z__[nn - 5] / z__[nn - 7];
		    np = nn - 9;
		} else {
		    np = nn - (*pp << 1);
		    b2 = z__[np - 2];
		    gam = *dn1;
		    if (z__[np - 4] > z__[np - 2]) {
			return 0;
		    }
		    a2 = z__[np - 4] / z__[np - 2];
		    if (z__[nn - 9] > z__[nn - 11]) {
			return 0;
		    }
		    b2 = z__[nn - 9] / z__[nn - 11];
		    np = nn - 13;
		}

/*              Approximate contribution to norm squared from I < NN-1. */

		a2 += b2;
		i__1 = (*i0 << 2) - 1 + *pp;
		for (i4 = np; i4 >= i__1; i4 += -4) {
		    if (b2 == 0.) {
			goto L20;
		    }
		    b1 = b2;
		    if (z__[i4] > z__[i4 - 2]) {
			return 0;
		    }
		    b2 *= z__[i4] / z__[i4 - 2];
		    a2 += b2;
		    if (std::max(b2,b1) * 100. < a2 || .563 < a2) {
			goto L20;
		    }
/* L10: */
		}
L20:
		a2 *= 1.05;

/*              Rayleigh quotient residual bound. */

		if (a2 < .563) {
		    s = gam * (1. - sqrt(a2)) / (a2 + 1.);
		}
	    }
	} else if (*dmin__ == *dn2) {

/*           Case 5. */

	    *ttype = -5;
	    s = *dmin__ * .25;

/*           Compute contribution to norm squared from I > NN-2. */

	    np = nn - (*pp << 1);
	    b1 = z__[np - 2];
	    b2 = z__[np - 6];
	    gam = *dn2;
	    if (z__[np - 8] > b2 || z__[np - 4] > b1) {
		return 0;
	    }
	    a2 = z__[np - 8] / b2 * (z__[np - 4] / b1 + 1.);

/*           Approximate contribution to norm squared from I < NN-2. */

	    if (*n0 - *i0 > 2) {
		b2 = z__[nn - 13] / z__[nn - 15];
		a2 += b2;
		i__1 = (*i0 << 2) - 1 + *pp;
		for (i4 = nn - 17; i4 >= i__1; i4 += -4) {
		    if (b2 == 0.) {
			goto L40;
		    }
		    b1 = b2;
		    if (z__[i4] > z__[i4 - 2]) {
			return 0;
		    }
		    b2 *= z__[i4] / z__[i4 - 2];
		    a2 += b2;
		    if (std::max(b2,b1) * 100. < a2 || .563 < a2) {
			goto L40;
		    }
/* L30: */
		}
L40:
		a2 *= 1.05;
	    }

	    if (a2 < .563) {
		s = gam * (1. - sqrt(a2)) / (a2 + 1.);
	    }
	} else {

/*           Case 6, no information to guide us. */

	    if (*ttype == -6) {
		*g += (1. - *g) * .333;
	    } else if (*ttype == -18) {
		*g = .083250000000000005;
	    } else {
		*g = .25;
	    }
	    s = *g * *dmin__;
	    *ttype = -6;
	}

    } else if (*n0in == *n0 + 1) {

/*        One eigenvalue just deflated. Use DMIN1, DN1 for DMIN and DN. */

	if (*dmin1 == *dn1 && *dmin2 == *dn2) {

/*           Cases 7 and 8. */

	    *ttype = -7;
	    s = *dmin1 * .333;
	    if (z__[nn - 5] > z__[nn - 7]) {
		return 0;
	    }
	    b1 = z__[nn - 5] / z__[nn - 7];
	    b2 = b1;
	    if (b2 == 0.) {
		goto L60;
	    }
	    i__1 = (*i0 << 2) - 1 + *pp;
	    for (i4 = (*n0 << 2) - 9 + *pp; i4 >= i__1; i4 += -4) {
		a2 = b1;
		if (z__[i4] > z__[i4 - 2]) {
		    return 0;
		}
		b1 *= z__[i4] / z__[i4 - 2];
		b2 += b1;
		if (std::max(b1,a2) * 100. < b2) {
		    goto L60;
		}
/* L50: */
	    }
L60:
	    b2 = sqrt(b2 * 1.05);
/* Computing 2nd power */
	    d__1 = b2;
	    a2 = *dmin1 / (d__1 * d__1 + 1.);
	    gap2 = *dmin2 * .5 - a2;
	    if (gap2 > 0. && gap2 > b2 * a2) {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - a2 * 1.01 * (b2 / gap2) * b2);
		s = std::max(d__1,d__2);
	    } else {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - b2 * 1.01);
		s = std::max(d__1,d__2);
		*ttype = -8;
	    }
	} else {

/*           Case 9. */

	    s = *dmin1 * .25;
	    if (*dmin1 == *dn1) {
		s = *dmin1 * .5;
	    }
	    *ttype = -9;
	}

    } else if (*n0in == *n0 + 2) {

/*        Two eigenvalues deflated. Use DMIN2, DN2 for DMIN and DN. */

/*        Cases 10 and 11. */

	if (*dmin2 == *dn2 && z__[nn - 5] * 2. < z__[nn - 7]) {
	    *ttype = -10;
	    s = *dmin2 * .333;
	    if (z__[nn - 5] > z__[nn - 7]) {
		return 0;
	    }
	    b1 = z__[nn - 5] / z__[nn - 7];
	    b2 = b1;
	    if (b2 == 0.) {
		goto L80;
	    }
	    i__1 = (*i0 << 2) - 1 + *pp;
	    for (i4 = (*n0 << 2) - 9 + *pp; i4 >= i__1; i4 += -4) {
		if (z__[i4] > z__[i4 - 2]) {
		    return 0;
		}
		b1 *= z__[i4] / z__[i4 - 2];
		b2 += b1;
		if (b1 * 100. < b2) {
		    goto L80;
		}
/* L70: */
	    }
L80:
	    b2 = sqrt(b2 * 1.05);
/* Computing 2nd power */
	    d__1 = b2;
	    a2 = *dmin2 / (d__1 * d__1 + 1.);
	    gap2 = z__[nn - 7] + z__[nn - 9] - sqrt(z__[nn - 11]) * sqrt(z__[
		    nn - 9]) - a2;
	    if (gap2 > 0. && gap2 > b2 * a2) {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - a2 * 1.01 * (b2 / gap2) * b2);
		s = std::max(d__1,d__2);
	    } else {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - b2 * 1.01);
		s = std::max(d__1,d__2);
	    }
	} else {
	    s = *dmin2 * .25;
	    *ttype = -11;
	}
    } else if (*n0in > *n0 + 2) {

/*        Case 12, more than two eigenvalues deflated. No information. */

	s = 0.;
	*ttype = -12;
    }

    *tau = s;
    return 0;

/*     End of DLASQ4 */

} /* dlasq4_ */

/* Subroutine */ int dlasq5_(integer *i0, integer *n0, double *z__,
	integer *pp, double *tau, double *dmin__, double *dmin1,
	double *dmin2, double *dn, double *dnm1, double *dnm2,
	bool *ieee)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    double d__;
    integer j4, j4p2;
    double emin, temp;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASQ5 computes one dqds transform in ping-pong form, one */
/*  version for IEEE machines another for non IEEE machines. */

/*  Arguments */
/*  ========= */

/*  I0    (input) INTEGER */
/*        First index. */

/*  N0    (input) INTEGER */
/*        Last index. */

/*  Z     (input) DOUBLE PRECISION array, dimension ( 4*N ) */
/*        Z holds the qd array. EMIN is stored in Z(4*N0) to avoid */
/*        an extra argument. */

/*  PP    (input) INTEGER */
/*        PP=0 for ping, PP=1 for pong. */

/*  TAU   (input) DOUBLE PRECISION */
/*        This is the shift. */

/*  DMIN  (output) DOUBLE PRECISION */
/*        Minimum value of d. */

/*  DMIN1 (output) DOUBLE PRECISION */
/*        Minimum value of d, excluding D( N0 ). */

/*  DMIN2 (output) DOUBLE PRECISION */
/*        Minimum value of d, excluding D( N0 ) and D( N0-1 ). */

/*  DN    (output) DOUBLE PRECISION */
/*        d(N0), the last value of d. */

/*  DNM1  (output) DOUBLE PRECISION */
/*        d(N0-1). */

/*  DNM2  (output) DOUBLE PRECISION */
/*        d(N0-2). */

/*  IEEE  (input) LOGICAL */
/*        Flag for IEEE or non IEEE arithmetic. */

/*  ===================================================================== */

/*     .. Parameter .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --z__;

    /* Function Body */
    if (*n0 - *i0 - 1 <= 0) {
	return 0;
    }

    j4 = (*i0 << 2) + *pp - 3;
    emin = z__[j4 + 4];
    d__ = z__[j4] - *tau;
    *dmin__ = d__;
    *dmin1 = -z__[j4];

    if (*ieee) {

/*        Code for IEEE arithmetic. */

	if (*pp == 0) {
	    i__1 = *n0 - 3 << 2;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		z__[j4 - 2] = d__ + z__[j4 - 1];
		temp = z__[j4 + 1] / z__[j4 - 2];
		d__ = d__ * temp - *tau;
		*dmin__ = std::min(*dmin__,d__);
		z__[j4] = z__[j4 - 1] * temp;
/* Computing MIN */
		d__1 = z__[j4];
		emin = std::min(d__1,emin);
/* L10: */
	    }
	} else {
	    i__1 = *n0 - 3 << 2;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		z__[j4 - 3] = d__ + z__[j4];
		temp = z__[j4 + 2] / z__[j4 - 3];
		d__ = d__ * temp - *tau;
		*dmin__ = std::min(*dmin__,d__);
		z__[j4 - 1] = z__[j4] * temp;
/* Computing MIN */
		d__1 = z__[j4 - 1];
		emin = std::min(d__1,emin);
/* L20: */
	    }
	}

/*        Unroll last two steps. */

	*dnm2 = d__;
	*dmin2 = *dmin__;
	j4 = (*n0 - 2 << 2) - *pp;
	j4p2 = j4 + (*pp << 1) - 1;
	z__[j4 - 2] = *dnm2 + z__[j4p2];
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]) - *tau;
	*dmin__ = std::min(*dmin__,*dnm1);

	*dmin1 = *dmin__;
	j4 += 4;
	j4p2 = j4 + (*pp << 1) - 1;
	z__[j4 - 2] = *dnm1 + z__[j4p2];
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]) - *tau;
	*dmin__ = std::min(*dmin__,*dn);

    } else {

/*        Code for non IEEE arithmetic. */

	if (*pp == 0) {
	    i__1 = *n0 - 3 << 2;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		z__[j4 - 2] = d__ + z__[j4 - 1];
		if (d__ < 0.) {
		    return 0;
		} else {
		    z__[j4] = z__[j4 + 1] * (z__[j4 - 1] / z__[j4 - 2]);
		    d__ = z__[j4 + 1] * (d__ / z__[j4 - 2]) - *tau;
		}
		*dmin__ = std::min(*dmin__,d__);
/* Computing MIN */
		d__1 = emin, d__2 = z__[j4];
		emin = std::min(d__1,d__2);
/* L30: */
	    }
	} else {
	    i__1 = *n0 - 3 << 2;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		z__[j4 - 3] = d__ + z__[j4];
		if (d__ < 0.) {
		    return 0;
		} else {
		    z__[j4 - 1] = z__[j4 + 2] * (z__[j4] / z__[j4 - 3]);
		    d__ = z__[j4 + 2] * (d__ / z__[j4 - 3]) - *tau;
		}
		*dmin__ = std::min(*dmin__,d__);
/* Computing MIN */
		d__1 = emin, d__2 = z__[j4 - 1];
		emin = std::min(d__1,d__2);
/* L40: */
	    }
	}

/*        Unroll last two steps. */

	*dnm2 = d__;
	*dmin2 = *dmin__;
	j4 = (*n0 - 2 << 2) - *pp;
	j4p2 = j4 + (*pp << 1) - 1;
	z__[j4 - 2] = *dnm2 + z__[j4p2];
	if (*dnm2 < 0.) {
	    return 0;
	} else {
	    z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	    *dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]) - *tau;
	}
	*dmin__ = std::min(*dmin__,*dnm1);

	*dmin1 = *dmin__;
	j4 += 4;
	j4p2 = j4 + (*pp << 1) - 1;
	z__[j4 - 2] = *dnm1 + z__[j4p2];
	if (*dnm1 < 0.) {
	    return 0;
	} else {
	    z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	    *dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]) - *tau;
	}
	*dmin__ = std::min(*dmin__,*dn);

    }

    z__[j4 + 2] = *dn;
    z__[(*n0 << 2) - *pp] = emin;
    return 0;

/*     End of DLASQ5 */

} /* dlasq5_ */

/* Subroutine */ int dlasq6_(integer *i0, integer *n0, double *z__,
	integer *pp, double *dmin__, double *dmin1, double *dmin2,
	double *dn, double *dnm1, double *dnm2)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    double d__;
    integer j4, j4p2;
    double emin, temp;

    double safmin;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASQ6 computes one dqd (shift equal to zero) transform in */
/*  ping-pong form, with protection against underflow and overflow. */

/*  Arguments */
/*  ========= */

/*  I0    (input) INTEGER */
/*        First index. */

/*  N0    (input) INTEGER */
/*        Last index. */

/*  Z     (input) DOUBLE PRECISION array, dimension ( 4*N ) */
/*        Z holds the qd array. EMIN is stored in Z(4*N0) to avoid */
/*        an extra argument. */

/*  PP    (input) INTEGER */
/*        PP=0 for ping, PP=1 for pong. */

/*  DMIN  (output) DOUBLE PRECISION */
/*        Minimum value of d. */

/*  DMIN1 (output) DOUBLE PRECISION */
/*        Minimum value of d, excluding D( N0 ). */

/*  DMIN2 (output) DOUBLE PRECISION */
/*        Minimum value of d, excluding D( N0 ) and D( N0-1 ). */

/*  DN    (output) DOUBLE PRECISION */
/*        d(N0), the last value of d. */

/*  DNM1  (output) DOUBLE PRECISION */
/*        d(N0-1). */

/*  DNM2  (output) DOUBLE PRECISION */
/*        d(N0-2). */

/*  ===================================================================== */

/*     .. Parameter .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Function .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --z__;

    /* Function Body */
    if (*n0 - *i0 - 1 <= 0) {
	return 0;
    }

    safmin = dlamch_("Safe minimum");
    j4 = (*i0 << 2) + *pp - 3;
    emin = z__[j4 + 4];
    d__ = z__[j4];
    *dmin__ = d__;

    if (*pp == 0) {
	i__1 = *n0 - 3 << 2;
	for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
	    z__[j4 - 2] = d__ + z__[j4 - 1];
	    if (z__[j4 - 2] == 0.) {
		z__[j4] = 0.;
		d__ = z__[j4 + 1];
		*dmin__ = d__;
		emin = 0.;
	    } else if (safmin * z__[j4 + 1] < z__[j4 - 2] && safmin * z__[j4
		    - 2] < z__[j4 + 1]) {
		temp = z__[j4 + 1] / z__[j4 - 2];
		z__[j4] = z__[j4 - 1] * temp;
		d__ *= temp;
	    } else {
		z__[j4] = z__[j4 + 1] * (z__[j4 - 1] / z__[j4 - 2]);
		d__ = z__[j4 + 1] * (d__ / z__[j4 - 2]);
	    }
	    *dmin__ = std::min(*dmin__,d__);
/* Computing MIN */
	    d__1 = emin, d__2 = z__[j4];
	    emin = std::min(d__1,d__2);
/* L10: */
	}
    } else {
	i__1 = *n0 - 3 << 2;
	for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
	    z__[j4 - 3] = d__ + z__[j4];
	    if (z__[j4 - 3] == 0.) {
		z__[j4 - 1] = 0.;
		d__ = z__[j4 + 2];
		*dmin__ = d__;
		emin = 0.;
	    } else if (safmin * z__[j4 + 2] < z__[j4 - 3] && safmin * z__[j4
		    - 3] < z__[j4 + 2]) {
		temp = z__[j4 + 2] / z__[j4 - 3];
		z__[j4 - 1] = z__[j4] * temp;
		d__ *= temp;
	    } else {
		z__[j4 - 1] = z__[j4 + 2] * (z__[j4] / z__[j4 - 3]);
		d__ = z__[j4 + 2] * (d__ / z__[j4 - 3]);
	    }
	    *dmin__ = std::min(*dmin__,d__);
/* Computing MIN */
	    d__1 = emin, d__2 = z__[j4 - 1];
	    emin = std::min(d__1,d__2);
/* L20: */
	}
    }

/*     Unroll last two steps. */

    *dnm2 = d__;
    *dmin2 = *dmin__;
    j4 = (*n0 - 2 << 2) - *pp;
    j4p2 = j4 + (*pp << 1) - 1;
    z__[j4 - 2] = *dnm2 + z__[j4p2];
    if (z__[j4 - 2] == 0.) {
	z__[j4] = 0.;
	*dnm1 = z__[j4p2 + 2];
	*dmin__ = *dnm1;
	emin = 0.;
    } else if (safmin * z__[j4p2 + 2] < z__[j4 - 2] && safmin * z__[j4 - 2] <
	    z__[j4p2 + 2]) {
	temp = z__[j4p2 + 2] / z__[j4 - 2];
	z__[j4] = z__[j4p2] * temp;
	*dnm1 = *dnm2 * temp;
    } else {
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dnm1 = z__[j4p2 + 2] * (*dnm2 / z__[j4 - 2]);
    }
    *dmin__ = std::min(*dmin__,*dnm1);

    *dmin1 = *dmin__;
    j4 += 4;
    j4p2 = j4 + (*pp << 1) - 1;
    z__[j4 - 2] = *dnm1 + z__[j4p2];
    if (z__[j4 - 2] == 0.) {
	z__[j4] = 0.;
	*dn = z__[j4p2 + 2];
	*dmin__ = *dn;
	emin = 0.;
    } else if (safmin * z__[j4p2 + 2] < z__[j4 - 2] && safmin * z__[j4 - 2] <
	    z__[j4p2 + 2]) {
	temp = z__[j4p2 + 2] / z__[j4 - 2];
	z__[j4] = z__[j4p2] * temp;
	*dn = *dnm1 * temp;
    } else {
	z__[j4] = z__[j4p2 + 2] * (z__[j4p2] / z__[j4 - 2]);
	*dn = z__[j4p2 + 2] * (*dnm1 / z__[j4 - 2]);
    }
    *dmin__ = std::min(*dmin__,*dn);

    z__[j4 + 2] = *dn;
    z__[(*n0 << 2) - *pp] = emin;
    return 0;

/*     End of DLASQ6 */

} /* dlasq6_ */

/* Subroutine */ int dlasr_(const char *side, const char *pivot, const char *direct, integer *m,
	integer *n, double *c__, double *s, double *a, integer *lda)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, info;
    double temp;

    double ctemp, stemp;



/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASR applies a sequence of plane rotations to a real matrix A, */
/*  from either the left or the right. */

/*  When SIDE = 'L', the transformation takes the form */

/*     A := P*A */

/*  and when SIDE = 'R', the transformation takes the form */

/*     A := A*P**T */

/*  where P is an orthogonal matrix consisting of a sequence of z plane */
/*  rotations, with z = M when SIDE = 'L' and z = N when SIDE = 'R', */
/*  and P**T is the transpose of P. */

/*  When DIRECT = 'F' (Forward sequence), then */

/*     P = P(z-1) * ... * P(2) * P(1) */

/*  and when DIRECT = 'B' (Backward sequence), then */

/*     P = P(1) * P(2) * ... * P(z-1) */

/*  where P(k) is a plane rotation matrix defined by the 2-by-2 rotation */

/*     R(k) = (  c(k)  s(k) ) */
/*          = ( -s(k)  c(k) ). */

/*  When PIVOT = 'V' (Variable pivot), the rotation is performed */
/*  for the plane (k,k+1), i.e., P(k) has the form */

/*     P(k) = (  1                                            ) */
/*            (       ...                                     ) */
/*            (              1                                ) */
/*            (                   c(k)  s(k)                  ) */
/*            (                  -s(k)  c(k)                  ) */
/*            (                                1              ) */
/*            (                                     ...       ) */
/*            (                                            1  ) */

/*  where R(k) appears as a rank-2 modification to the identity matrix in */
/*  rows and columns k and k+1. */

/*  When PIVOT = 'T' (Top pivot), the rotation is performed for the */
/*  plane (1,k+1), so P(k) has the form */

/*     P(k) = (  c(k)                    s(k)                 ) */
/*            (         1                                     ) */
/*            (              ...                              ) */
/*            (                     1                         ) */
/*            ( -s(k)                    c(k)                 ) */
/*            (                                 1             ) */
/*            (                                      ...      ) */
/*            (                                             1 ) */

/*  where R(k) appears in rows and columns 1 and k+1. */

/*  Similarly, when PIVOT = 'B' (Bottom pivot), the rotation is */
/*  performed for the plane (k,z), giving P(k) the form */

/*     P(k) = ( 1                                             ) */
/*            (      ...                                      ) */
/*            (             1                                 ) */
/*            (                  c(k)                    s(k) ) */
/*            (                         1                     ) */
/*            (                              ...              ) */
/*            (                                     1         ) */
/*            (                 -s(k)                    c(k) ) */

/*  where R(k) appears in rows and columns k and z.  The rotations are */
/*  performed without ever forming P(k) explicitly. */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          Specifies whether the plane rotation matrix P is applied to */
/*          A on the left or the right. */
/*          = 'L':  Left, compute A := P*A */
/*          = 'R':  Right, compute A:= A*P**T */

/*  PIVOT   (input) CHARACTER*1 */
/*          Specifies the plane for which P(k) is a plane rotation */
/*          matrix. */
/*          = 'V':  Variable pivot, the plane (k,k+1) */
/*          = 'T':  Top pivot, the plane (1,k+1) */
/*          = 'B':  Bottom pivot, the plane (k,z) */

/*  DIRECT  (input) CHARACTER*1 */
/*          Specifies whether P is a forward or backward sequence of */
/*          plane rotations. */
/*          = 'F':  Forward, P = P(z-1)*...*P(2)*P(1) */
/*          = 'B':  Backward, P = P(1)*P(2)*...*P(z-1) */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  If m <= 1, an immediate */
/*          return is effected. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  If n <= 1, an */
/*          immediate return is effected. */

/*  C       (input) DOUBLE PRECISION array, dimension */
/*                  (M-1) if SIDE = 'L' */
/*                  (N-1) if SIDE = 'R' */
/*          The cosines c(k) of the plane rotations. */

/*  S       (input) DOUBLE PRECISION array, dimension */
/*                  (M-1) if SIDE = 'L' */
/*                  (N-1) if SIDE = 'R' */
/*          The sines s(k) of the plane rotations.  The 2-by-2 plane */
/*          rotation part of the matrix P(k), R(k), has the form */
/*          R(k) = (  c(k)  s(k) ) */
/*                 ( -s(k)  c(k) ). */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The M-by-N matrix A.  On exit, A is overwritten by P*A if */
/*          SIDE = 'R' or by A*P**T if SIDE = 'L'. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters */

    /* Parameter adjustments */
    --c__;
    --s;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    info = 0;
    if (! (lsame_(side, "L") || lsame_(side, "R"))) {
	info = 1;
    } else if (! (lsame_(pivot, "V") || lsame_(pivot,
	    "T") || lsame_(pivot, "B"))) {
	info = 2;
    } else if (! (lsame_(direct, "F") || lsame_(direct,
	    "B"))) {
	info = 3;
    } else if (*m < 0) {
	info = 4;
    } else if (*n < 0) {
	info = 5;
    } else if (*lda < std::max(1_integer,*m)) {
	info = 9;
    }
    if (info != 0) {
	xerbla_("DLASR ", &info);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }
    if (lsame_(side, "L")) {

/*        Form  P * A */

	if (lsame_(pivot, "V")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *n;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[j + 1 + i__ * a_dim1];
			    a[j + 1 + i__ * a_dim1] = ctemp * temp - stemp *
				    a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = stemp * temp + ctemp * a[j
				    + i__ * a_dim1];
/* L10: */
			}
		    }
/* L20: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[j + 1 + i__ * a_dim1];
			    a[j + 1 + i__ * a_dim1] = ctemp * temp - stemp *
				    a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = stemp * temp + ctemp * a[j
				    + i__ * a_dim1];
/* L30: */
			}
		    }
/* L40: */
		}
	    }
	} else if (lsame_(pivot, "T")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m;
		for (j = 2; j <= i__1; ++j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *n;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = ctemp * temp - stemp * a[
				    i__ * a_dim1 + 1];
			    a[i__ * a_dim1 + 1] = stemp * temp + ctemp * a[
				    i__ * a_dim1 + 1];
/* L50: */
			}
		    }
/* L60: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m; j >= 2; --j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = ctemp * temp - stemp * a[
				    i__ * a_dim1 + 1];
			    a[i__ * a_dim1 + 1] = stemp * temp + ctemp * a[
				    i__ * a_dim1 + 1];
/* L70: */
			}
		    }
/* L80: */
		}
	    }
	} else if (lsame_(pivot, "B")) {
	    if (lsame_(direct, "F")) {
		i__1 = *m - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *n;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = stemp * a[*m + i__ * a_dim1]
				     + ctemp * temp;
			    a[*m + i__ * a_dim1] = ctemp * a[*m + i__ *
				    a_dim1] - stemp * temp;
/* L90: */
			}
		    }
/* L100: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *m - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *n;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[j + i__ * a_dim1];
			    a[j + i__ * a_dim1] = stemp * a[*m + i__ * a_dim1]
				     + ctemp * temp;
			    a[*m + i__ * a_dim1] = ctemp * a[*m + i__ *
				    a_dim1] - stemp * temp;
/* L110: */
			}
		    }
/* L120: */
		}
	    }
	}
    } else if (lsame_(side, "R")) {

/*        Form A * P' */

	if (lsame_(pivot, "V")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[i__ + (j + 1) * a_dim1];
			    a[i__ + (j + 1) * a_dim1] = ctemp * temp - stemp *
				     a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = stemp * temp + ctemp * a[
				    i__ + j * a_dim1];
/* L130: */
			}
		    }
/* L140: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[i__ + (j + 1) * a_dim1];
			    a[i__ + (j + 1) * a_dim1] = ctemp * temp - stemp *
				     a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = stemp * temp + ctemp * a[
				    i__ + j * a_dim1];
/* L150: */
			}
		    }
/* L160: */
		}
	    }
	} else if (lsame_(pivot, "T")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n;
		for (j = 2; j <= i__1; ++j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = ctemp * temp - stemp * a[
				    i__ + a_dim1];
			    a[i__ + a_dim1] = stemp * temp + ctemp * a[i__ +
				    a_dim1];
/* L170: */
			}
		    }
/* L180: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n; j >= 2; --j) {
		    ctemp = c__[j - 1];
		    stemp = s[j - 1];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = ctemp * temp - stemp * a[
				    i__ + a_dim1];
			    a[i__ + a_dim1] = stemp * temp + ctemp * a[i__ +
				    a_dim1];
/* L190: */
			}
		    }
/* L200: */
		}
	    }
	} else if (lsame_(pivot, "B")) {
	    if (lsame_(direct, "F")) {
		i__1 = *n - 1;
		for (j = 1; j <= i__1; ++j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__2 = *m;
			for (i__ = 1; i__ <= i__2; ++i__) {
			    temp = a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = stemp * a[i__ + *n * a_dim1]
				     + ctemp * temp;
			    a[i__ + *n * a_dim1] = ctemp * a[i__ + *n *
				    a_dim1] - stemp * temp;
/* L210: */
			}
		    }
/* L220: */
		}
	    } else if (lsame_(direct, "B")) {
		for (j = *n - 1; j >= 1; --j) {
		    ctemp = c__[j];
		    stemp = s[j];
		    if (ctemp != 1. || stemp != 0.) {
			i__1 = *m;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    temp = a[i__ + j * a_dim1];
			    a[i__ + j * a_dim1] = stemp * a[i__ + *n * a_dim1]
				     + ctemp * temp;
			    a[i__ + *n * a_dim1] = ctemp * a[i__ + *n *
				    a_dim1] - stemp * temp;
/* L230: */
			}
		    }
/* L240: */
		}
	    }
	}
    }

    return 0;

/*     End of DLASR */

} /* dlasr_ */

/* Subroutine */ int dlasrt_(const char *id, integer *n, double *d__, integer *info)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer i__, j;
    double d1, d2, d3;
    integer dir;
    double tmp;
    integer endd;

    integer stack[64]	/* was [2][32] */;
    double dmnmx;
    integer start;

    integer stkpnt;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Sort the numbers in D in increasing order (if ID = 'I') or */
/*  in decreasing order (if ID = 'D' ). */

/*  Use Quick Sort, reverting to Insertion sort on arrays of */
/*  size <= 20. Dimension of STACK limits N to about 2**32. */

/*  Arguments */
/*  ========= */

/*  ID      (input) CHARACTER*1 */
/*          = 'I': sort D in increasing order; */
/*          = 'D': sort D in decreasing order. */

/*  N       (input) INTEGER */
/*          The length of the array D. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the array to be sorted. */
/*          On exit, D has been sorted into increasing order */
/*          (D(1) <= ... <= D(N) ) or into decreasing order */
/*          (D(1) >= ... >= D(N) ), depending on ID. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input paramters. */

    /* Parameter adjustments */
    --d__;

    /* Function Body */
    *info = 0;
    dir = -1;
    if (lsame_(id, "D")) {
	dir = 0;
    } else if (lsame_(id, "I")) {
	dir = 1;
    }
    if (dir == -1) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLASRT", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 1) {
	return 0;
    }

    stkpnt = 1;
    stack[0] = 1;
    stack[1] = *n;
L10:
    start = stack[(stkpnt << 1) - 2];
    endd = stack[(stkpnt << 1) - 1];
    --stkpnt;
    if (endd - start <= 20 && endd - start > 0) {

/*        Do Insertion sort on D( START:ENDD ) */

	if (dir == 0) {

/*           Sort into decreasing order */

	    i__1 = endd;
	    for (i__ = start + 1; i__ <= i__1; ++i__) {
		i__2 = start + 1;
		for (j = i__; j >= i__2; --j) {
		    if (d__[j] > d__[j - 1]) {
			dmnmx = d__[j];
			d__[j] = d__[j - 1];
			d__[j - 1] = dmnmx;
		    } else {
			goto L30;
		    }
/* L20: */
		}
L30:
		;
	    }

	} else {

/*           Sort into increasing order */

	    i__1 = endd;
	    for (i__ = start + 1; i__ <= i__1; ++i__) {
		i__2 = start + 1;
		for (j = i__; j >= i__2; --j) {
		    if (d__[j] < d__[j - 1]) {
			dmnmx = d__[j];
			d__[j] = d__[j - 1];
			d__[j - 1] = dmnmx;
		    } else {
			goto L50;
		    }
/* L40: */
		}
L50:
		;
	    }

	}

    } else if (endd - start > 20) {

/*        Partition D( START:ENDD ) and stack parts, largest one first */

/*        Choose partition entry as median of 3 */

	d1 = d__[start];
	d2 = d__[endd];
	i__ = (start + endd) / 2;
	d3 = d__[i__];
	if (d1 < d2) {
	    if (d3 < d1) {
		dmnmx = d1;
	    } else if (d3 < d2) {
		dmnmx = d3;
	    } else {
		dmnmx = d2;
	    }
	} else {
	    if (d3 < d2) {
		dmnmx = d2;
	    } else if (d3 < d1) {
		dmnmx = d3;
	    } else {
		dmnmx = d1;
	    }
	}

	if (dir == 0) {

/*           Sort into decreasing order */

	    i__ = start - 1;
	    j = endd + 1;
L60:
L70:
	    --j;
	    if (d__[j] < dmnmx) {
		goto L70;
	    }
L80:
	    ++i__;
	    if (d__[i__] > dmnmx) {
		goto L80;
	    }
	    if (i__ < j) {
		tmp = d__[i__];
		d__[i__] = d__[j];
		d__[j] = tmp;
		goto L60;
	    }
	    if (j - start > endd - j - 1) {
		++stkpnt;
		stack[(stkpnt << 1) - 2] = start;
		stack[(stkpnt << 1) - 1] = j;
		++stkpnt;
		stack[(stkpnt << 1) - 2] = j + 1;
		stack[(stkpnt << 1) - 1] = endd;
	    } else {
		++stkpnt;
		stack[(stkpnt << 1) - 2] = j + 1;
		stack[(stkpnt << 1) - 1] = endd;
		++stkpnt;
		stack[(stkpnt << 1) - 2] = start;
		stack[(stkpnt << 1) - 1] = j;
	    }
	} else {

/*           Sort into increasing order */

	    i__ = start - 1;
	    j = endd + 1;
L90:
L100:
	    --j;
	    if (d__[j] > dmnmx) {
		goto L100;
	    }
L110:
	    ++i__;
	    if (d__[i__] < dmnmx) {
		goto L110;
	    }
	    if (i__ < j) {
		tmp = d__[i__];
		d__[i__] = d__[j];
		d__[j] = tmp;
		goto L90;
	    }
	    if (j - start > endd - j - 1) {
		++stkpnt;
		stack[(stkpnt << 1) - 2] = start;
		stack[(stkpnt << 1) - 1] = j;
		++stkpnt;
		stack[(stkpnt << 1) - 2] = j + 1;
		stack[(stkpnt << 1) - 1] = endd;
	    } else {
		++stkpnt;
		stack[(stkpnt << 1) - 2] = j + 1;
		stack[(stkpnt << 1) - 1] = endd;
		++stkpnt;
		stack[(stkpnt << 1) - 2] = start;
		stack[(stkpnt << 1) - 1] = j;
	    }
	}
    }
    if (stkpnt > 0) {
	goto L10;
    }
    return 0;

/*     End of DLASRT */

} /* dlasrt_ */

/* Subroutine */ int dlassq_(integer *n, double *x, integer *incx,
	double *scale, double *sumsq)
{
    /* System generated locals */
    integer i__1, i__2;
    double d__1;

    /* Local variables */
    integer ix;
    double absxi;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASSQ  returns the values  scl  and  smsq  such that */

/*     ( scl**2 )*smsq = x( 1 )**2 +...+ x( n )**2 + ( scale**2 )*sumsq, */

/*  where  x( i ) = X( 1 + ( i - 1 )*INCX ). The value of  sumsq  is */
/*  assumed to be non-negative and  scl  returns the value */

/*     scl = max( scale, abs( x( i ) ) ). */

/*  scale and sumsq must be supplied in SCALE and SUMSQ and */
/*  scl and smsq are overwritten on SCALE and SUMSQ respectively. */

/*  The routine makes only one pass through the vector x. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of elements to be used from the vector X. */

/*  X       (input) DOUBLE PRECISION array, dimension (N) */
/*          The vector for which a scaled sum of squares is computed. */
/*             x( i )  = X( 1 + ( i - 1 )*INCX ), 1 <= i <= n. */

/*  INCX    (input) INTEGER */
/*          The increment between successive values of the vector X. */
/*          INCX > 0. */

/*  SCALE   (input/output) DOUBLE PRECISION */
/*          On entry, the value  scale  in the equation above. */
/*          On exit, SCALE is overwritten with  scl , the scaling factor */
/*          for the sum of squares. */

/*  SUMSQ   (input/output) DOUBLE PRECISION */
/*          On entry, the value  sumsq  in the equation above. */
/*          On exit, SUMSQ is overwritten with  smsq , the basic sum of */
/*          squares from which  scl  has been factored out. */

/* ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*n > 0) {
	i__1 = (*n - 1) * *incx + 1;
	i__2 = *incx;
	for (ix = 1; i__2 < 0 ? ix >= i__1 : ix <= i__1; ix += i__2) {
	    if (x[ix] != 0.) {
		absxi = (d__1 = x[ix], abs(d__1));
		if (*scale < absxi) {
/* Computing 2nd power */
		    d__1 = *scale / absxi;
		    *sumsq = *sumsq * (d__1 * d__1) + 1;
		    *scale = absxi;
		} else {
/* Computing 2nd power */
		    d__1 = absxi / *scale;
		    *sumsq += d__1 * d__1;
		}
	    }
/* L10: */
	}
    }
    return 0;

/*     End of DLASSQ */

} /* dlassq_ */

/* Subroutine */ int dlasv2_(double *f, double *g, double *h__,
	double *ssmin, double *ssmax, double *snr, double *
	csr, double *snl, double *csl)
{
	/* Table of constant values */
	static double c_b3 = 2.;
	static double c_b4 = 1.;

    /* System generated locals */
    double d__1;

    /* Local variables */
    double a, d__, l, m, r__, s, t, fa, ga, ha, ft, gt, ht, mm, tt, clt,
	    crt, slt, srt;
    integer pmax;
    double temp;
    bool swap;
    double tsign;

    bool gasmal;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASV2 computes the singular value decomposition of a 2-by-2 */
/*  triangular matrix */
/*     [  F   G  ] */
/*     [  0   H  ]. */
/*  On return, abs(SSMAX) is the larger singular value, abs(SSMIN) is the */
/*  smaller singular value, and (CSL,SNL) and (CSR,SNR) are the left and */
/*  right singular vectors for abs(SSMAX), giving the decomposition */

/*     [ CSL  SNL ] [  F   G  ] [ CSR -SNR ]  =  [ SSMAX   0   ] */
/*     [-SNL  CSL ] [  0   H  ] [ SNR  CSR ]     [  0    SSMIN ]. */

/*  Arguments */
/*  ========= */

/*  F       (input) DOUBLE PRECISION */
/*          The (1,1) element of the 2-by-2 matrix. */

/*  G       (input) DOUBLE PRECISION */
/*          The (1,2) element of the 2-by-2 matrix. */

/*  H       (input) DOUBLE PRECISION */
/*          The (2,2) element of the 2-by-2 matrix. */

/*  SSMIN   (output) DOUBLE PRECISION */
/*          abs(SSMIN) is the smaller singular value. */

/*  SSMAX   (output) DOUBLE PRECISION */
/*          abs(SSMAX) is the larger singular value. */

/*  SNL     (output) DOUBLE PRECISION */
/*  CSL     (output) DOUBLE PRECISION */
/*          The vector (CSL, SNL) is a unit left singular vector for the */
/*          singular value abs(SSMAX). */

/*  SNR     (output) DOUBLE PRECISION */
/*  CSR     (output) DOUBLE PRECISION */
/*          The vector (CSR, SNR) is a unit right singular vector for the */
/*          singular value abs(SSMAX). */

/*  Further Details */
/*  =============== */

/*  Any input parameter may be aliased with any output parameter. */

/*  Barring over/underflow and assuming a guard digit in subtraction, all */
/*  output quantities are correct to within a few units in the last */
/*  place (ulps). */

/*  In IEEE arithmetic, the code works correctly if one matrix element is */
/*  infinite. */

/*  Overflow will not occur unless the largest singular value itself */
/*  overflows or is within a few ulps of overflow. (On machines with */
/*  partial overflow, like the Cray, overflow may occur if the largest */
/*  singular value is within a factor of 2 of overflow.) */

/*  Underflow is harmless if underflow is gradual. Otherwise, results */
/*  may correspond to a matrix modified by perturbations of size near */
/*  the underflow threshold. */

/* ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    ft = *f;
    fa = abs(ft);
    ht = *h__;
    ha = abs(*h__);

/*     PMAX points to the maximum absolute element of matrix */
/*       PMAX = 1 if F largest in absolute values */
/*       PMAX = 2 if G largest in absolute values */
/*       PMAX = 3 if H largest in absolute values */

    pmax = 1;
    swap = ha > fa;
    if (swap) {
	pmax = 3;
	temp = ft;
	ft = ht;
	ht = temp;
	temp = fa;
	fa = ha;
	ha = temp;

/*        Now FA .ge. HA */

    }
    gt = *g;
    ga = abs(gt);
    if (ga == 0.) {

/*        Diagonal matrix */

	*ssmin = ha;
	*ssmax = fa;
	clt = 1.;
	crt = 1.;
	slt = 0.;
	srt = 0.;
    } else {
	gasmal = true;
	if (ga > fa) {
	    pmax = 2;
	    if (fa / ga < dlamch_("EPS")) {

/*              Case of very large GA */

		gasmal = false;
		*ssmax = ga;
		if (ha > 1.) {
		    *ssmin = fa / (ga / ha);
		} else {
		    *ssmin = fa / ga * ha;
		}
		clt = 1.;
		slt = ht / gt;
		srt = 1.;
		crt = ft / gt;
	    }
	}
	if (gasmal) {

/*           Normal case */

	    d__ = fa - ha;
	    if (d__ == fa) {

/*              Copes with infinite F or H */

		l = 1.;
	    } else {
		l = d__ / fa;
	    }

/*           Note that 0 .le. L .le. 1 */

	    m = gt / ft;

/*           Note that abs(M) .le. 1/macheps */

	    t = 2. - l;

/*           Note that T .ge. 1 */

	    mm = m * m;
	    tt = t * t;
	    s = sqrt(tt + mm);

/*           Note that 1 .le. S .le. 1 + 1/macheps */

	    if (l == 0.) {
		r__ = abs(m);
	    } else {
		r__ = sqrt(l * l + mm);
	    }

/*           Note that 0 .le. R .le. 1 + 1/macheps */

	    a = (s + r__) * .5;

/*           Note that 1 .le. A .le. 1 + abs(M) */

	    *ssmin = ha / a;
	    *ssmax = fa * a;
	    if (mm == 0.) {

/*              Note that M is very tiny */

		if (l == 0.) {
		    t = d_sign(&c_b3, &ft) * d_sign(&c_b4, &gt);
		} else {
		    t = gt / d_sign(&d__, &ft) + m / t;
		}
	    } else {
		t = (m / (s + t) + m / (r__ + l)) * (a + 1.);
	    }
	    l = sqrt(t * t + 4.);
	    crt = 2. / l;
	    srt = t / l;
	    clt = (crt + srt * m) / a;
	    slt = ht / ft * srt / a;
	}
    }
    if (swap) {
	*csl = srt;
	*snl = crt;
	*csr = slt;
	*snr = clt;
    } else {
	*csl = clt;
	*snl = slt;
	*csr = crt;
	*snr = srt;
    }

/*     Correct signs of SSMAX and SSMIN */

    if (pmax == 1) {
	tsign = d_sign(&c_b4, csr) * d_sign(&c_b4, csl) * d_sign(&c_b4, f);
    }
    if (pmax == 2) {
	tsign = d_sign(&c_b4, snr) * d_sign(&c_b4, csl) * d_sign(&c_b4, g);
    }
    if (pmax == 3) {
	tsign = d_sign(&c_b4, snr) * d_sign(&c_b4, snl) * d_sign(&c_b4, h__);
    }
    *ssmax = d_sign(ssmax, &tsign);
    d__1 = tsign * d_sign(&c_b4, f) * d_sign(&c_b4, h__);
    *ssmin = d_sign(ssmin, &d__1);
    return 0;

/*     End of DLASV2 */

} /* dlasv2_ */

/* Subroutine */ int dlaswp_(integer *n, double *a, integer *lda, integer
	*k1, integer *k2, integer *ipiv, integer *incx)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, j, k, i1, i2, n32, ip, ix, ix0, inc;
    double temp;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASWP performs a series of row interchanges on the matrix A. */
/*  One row interchange is initiated for each of rows K1 through K2 of A. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the matrix of column dimension N to which the row */
/*          interchanges will be applied. */
/*          On exit, the permuted matrix. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. */

/*  K1      (input) INTEGER */
/*          The first element of IPIV for which a row interchange will */
/*          be done. */

/*  K2      (input) INTEGER */
/*          The last element of IPIV for which a row interchange will */
/*          be done. */

/*  IPIV    (input) INTEGER array, dimension (K2*abs(INCX)) */
/*          The vector of pivot indices.  Only the elements in positions */
/*          K1 through K2 of IPIV are accessed. */
/*          IPIV(K) = L implies rows K and L are to be interchanged. */

/*  INCX    (input) INTEGER */
/*          The increment between successive values of IPIV.  If IPIV */
/*          is negative, the pivots are applied in reverse order. */

/*  Further Details */
/*  =============== */

/*  Modified by */
/*   R. C. Whaley, Computer Science Dept., Univ. of Tenn., Knoxville, USA */

/* ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Interchange row I with row IPIV(I) for each of rows K1 through K2. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --ipiv;

    /* Function Body */
    if (*incx > 0) {
	ix0 = *k1;
	i1 = *k1;
	i2 = *k2;
	inc = 1;
    } else if (*incx < 0) {
	ix0 = (1 - *k2) * *incx + 1;
	i1 = *k2;
	i2 = *k1;
	inc = -1;
    } else {
	return 0;
    }

    n32 = *n / 32 << 5;
    if (n32 != 0) {
	i__1 = n32;
	for (j = 1; j <= i__1; j += 32) {
	    ix = ix0;
	    i__2 = i2;
	    i__3 = inc;
	    for (i__ = i1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__3)
		    {
		ip = ipiv[ix];
		if (ip != i__) {
		    i__4 = j + 31;
		    for (k = j; k <= i__4; ++k) {
			temp = a[i__ + k * a_dim1];
			a[i__ + k * a_dim1] = a[ip + k * a_dim1];
			a[ip + k * a_dim1] = temp;
/* L10: */
		    }
		}
		ix += *incx;
/* L20: */
	    }
/* L30: */
	}
    }
    if (n32 != *n) {
	++n32;
	ix = ix0;
	i__1 = i2;
	i__3 = inc;
	for (i__ = i1; i__3 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__3) {
	    ip = ipiv[ix];
	    if (ip != i__) {
		i__2 = *n;
		for (k = n32; k <= i__2; ++k) {
		    temp = a[i__ + k * a_dim1];
		    a[i__ + k * a_dim1] = a[ip + k * a_dim1];
		    a[ip + k * a_dim1] = temp;
/* L40: */
		}
	    }
	    ix += *incx;
/* L50: */
	}
    }

    return 0;

/*     End of DLASWP */

} /* dlaswp_ */

/* Subroutine */ int dlasy2_(bool *ltranl, bool *ltranr, integer *isgn,
	integer *n1, integer *n2, double *tl, integer *ldtl, double *
	tr, integer *ldtr, double *b, integer *ldb, double *scale,
	double *x, integer *ldx, double *xnorm, integer *info)
{
	/* Table of constant values */
	static integer c__4 = 4;
	static integer c__1 = 1;
	static integer c__16 = 16;
	static integer c__0 = 0;

    /* Initialized data */

    static integer locu12[4] = { 3,4,1,2 };
    static integer locl21[4] = { 2,1,4,3 };
    static integer locu22[4] = { 4,3,2,1 };
    static bool xswpiv[4] = { false,false,true,true };
    static bool bswpiv[4] = { false,true,false,true };

    /* System generated locals */
    integer b_dim1, b_offset, tl_dim1, tl_offset, tr_dim1, tr_offset, x_dim1,
	    x_offset;
    double d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8;

    /* Local variables */
    integer i__, j, k;
    double x2[2], l21, u11, u12;
    integer ip, jp;
    double u22, t16[16]	/* was [4][4] */, gam, bet, eps, sgn, tmp[4],
	    tau1, btmp[4], smin;
    integer ipiv;
    double temp;
    integer jpiv[4];
    double xmax;
    integer ipsv, jpsv;
    bool bswap;
    bool xswap;
    double smlnum;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASY2 solves for the N1 by N2 matrix X, 1 <= N1,N2 <= 2, in */

/*         op(TL)*X + ISGN*X*op(TR) = SCALE*B, */

/*  where TL is N1 by N1, TR is N2 by N2, B is N1 by N2, and ISGN = 1 or */
/*  -1.  op(T) = T or T', where T' denotes the transpose of T. */

/*  Arguments */
/*  ========= */

/*  LTRANL  (input) LOGICAL */
/*          On entry, LTRANL specifies the op(TL): */
/*             = .FALSE., op(TL) = TL, */
/*             = .TRUE., op(TL) = TL'. */

/*  LTRANR  (input) LOGICAL */
/*          On entry, LTRANR specifies the op(TR): */
/*            = .FALSE., op(TR) = TR, */
/*            = .TRUE., op(TR) = TR'. */

/*  ISGN    (input) INTEGER */
/*          On entry, ISGN specifies the sign of the equation */
/*          as described before. ISGN may only be 1 or -1. */

/*  N1      (input) INTEGER */
/*          On entry, N1 specifies the order of matrix TL. */
/*          N1 may only be 0, 1 or 2. */

/*  N2      (input) INTEGER */
/*          On entry, N2 specifies the order of matrix TR. */
/*          N2 may only be 0, 1 or 2. */

/*  TL      (input) DOUBLE PRECISION array, dimension (LDTL,2) */
/*          On entry, TL contains an N1 by N1 matrix. */

/*  LDTL    (input) INTEGER */
/*          The leading dimension of the matrix TL. LDTL >= max(1,N1). */

/*  TR      (input) DOUBLE PRECISION array, dimension (LDTR,2) */
/*          On entry, TR contains an N2 by N2 matrix. */

/*  LDTR    (input) INTEGER */
/*          The leading dimension of the matrix TR. LDTR >= max(1,N2). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,2) */
/*          On entry, the N1 by N2 matrix B contains the right-hand */
/*          side of the equation. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the matrix B. LDB >= max(1,N1). */

/*  SCALE   (output) DOUBLE PRECISION */
/*          On exit, SCALE contains the scale factor. SCALE is chosen */
/*          less than or equal to 1 to prevent the solution overflowing. */

/*  X       (output) DOUBLE PRECISION array, dimension (LDX,2) */
/*          On exit, X contains the N1 by N2 solution. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the matrix X. LDX >= max(1,N1). */

/*  XNORM   (output) DOUBLE PRECISION */
/*          On exit, XNORM is the infinity-norm of the solution. */

/*  INFO    (output) INTEGER */
/*          On exit, INFO is set to */
/*             0: successful exit. */
/*             1: TL and TR have too close eigenvalues, so TL or */
/*                TR is perturbed to get a nonsingular equation. */
/*          NOTE: In the interests of speed, this routine does not */
/*                check the inputs for errors. */

/* ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Data statements .. */
    /* Parameter adjustments */
    tl_dim1 = *ldtl;
    tl_offset = 1 + tl_dim1;
    tl -= tl_offset;
    tr_dim1 = *ldtr;
    tr_offset = 1 + tr_dim1;
    tr -= tr_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Do not check the input parameters for errors */

    *info = 0;

/*     Quick return if possible */

    if (*n1 == 0 || *n2 == 0) {
	return 0;
    }

/*     Set constants to control overflow */

    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    sgn = (double) (*isgn);

    k = *n1 + *n1 + *n2 - 2;
    switch (k) {
	case 1:  goto L10;
	case 2:  goto L20;
	case 3:  goto L30;
	case 4:  goto L50;
    }

/*     1 by 1: TL11*X + SGN*X*TR11 = B11 */

L10:
    tau1 = tl[tl_dim1 + 1] + sgn * tr[tr_dim1 + 1];
    bet = abs(tau1);
    if (bet <= smlnum) {
	tau1 = smlnum;
	bet = smlnum;
	*info = 1;
    }

    *scale = 1.;
    gam = (d__1 = b[b_dim1 + 1], abs(d__1));
    if (smlnum * gam > bet) {
	*scale = 1. / gam;
    }

    x[x_dim1 + 1] = b[b_dim1 + 1] * *scale / tau1;
    *xnorm = (d__1 = x[x_dim1 + 1], abs(d__1));
    return 0;

/*     1 by 2: */
/*     TL11*[X11 X12] + ISGN*[X11 X12]*op[TR11 TR12]  = [B11 B12] */
/*                                       [TR21 TR22] */

L20:

/* Computing MAX */
/* Computing MAX */
    d__7 = (d__1 = tl[tl_dim1 + 1], abs(d__1)), d__8 = (d__2 = tr[tr_dim1 + 1]
	    , abs(d__2)), d__7 = std::max(d__7,d__8), d__8 = (d__3 = tr[(tr_dim1 <<
	     1) + 1], abs(d__3)), d__7 = std::max(d__7,d__8), d__8 = (d__4 = tr[
	    tr_dim1 + 2], abs(d__4)), d__7 = std::max(d__7,d__8), d__8 = (d__5 =
	    tr[(tr_dim1 << 1) + 2], abs(d__5));
    d__6 = eps * std::max(d__7,d__8);
    smin = std::max(d__6,smlnum);
    tmp[0] = tl[tl_dim1 + 1] + sgn * tr[tr_dim1 + 1];
    tmp[3] = tl[tl_dim1 + 1] + sgn * tr[(tr_dim1 << 1) + 2];
    if (*ltranr) {
	tmp[1] = sgn * tr[tr_dim1 + 2];
	tmp[2] = sgn * tr[(tr_dim1 << 1) + 1];
    } else {
	tmp[1] = sgn * tr[(tr_dim1 << 1) + 1];
	tmp[2] = sgn * tr[tr_dim1 + 2];
    }
    btmp[0] = b[b_dim1 + 1];
    btmp[1] = b[(b_dim1 << 1) + 1];
    goto L40;

/*     2 by 1: */
/*          op[TL11 TL12]*[X11] + ISGN* [X11]*TR11  = [B11] */
/*            [TL21 TL22] [X21]         [X21]         [B21] */

L30:
/* Computing MAX */
/* Computing MAX */
    d__7 = (d__1 = tr[tr_dim1 + 1], abs(d__1)), d__8 = (d__2 = tl[tl_dim1 + 1]
	    , abs(d__2)), d__7 = std::max(d__7,d__8), d__8 = (d__3 = tl[(tl_dim1 <<
	     1) + 1], abs(d__3)), d__7 = std::max(d__7,d__8), d__8 = (d__4 = tl[
	    tl_dim1 + 2], abs(d__4)), d__7 = std::max(d__7,d__8), d__8 = (d__5 =
	    tl[(tl_dim1 << 1) + 2], abs(d__5));
    d__6 = eps * std::max(d__7,d__8);
    smin = std::max(d__6,smlnum);
    tmp[0] = tl[tl_dim1 + 1] + sgn * tr[tr_dim1 + 1];
    tmp[3] = tl[(tl_dim1 << 1) + 2] + sgn * tr[tr_dim1 + 1];
    if (*ltranl) {
	tmp[1] = tl[(tl_dim1 << 1) + 1];
	tmp[2] = tl[tl_dim1 + 2];
    } else {
	tmp[1] = tl[tl_dim1 + 2];
	tmp[2] = tl[(tl_dim1 << 1) + 1];
    }
    btmp[0] = b[b_dim1 + 1];
    btmp[1] = b[b_dim1 + 2];
L40:

/*     Solve 2 by 2 system using complete pivoting. */
/*     Set pivots less than SMIN to SMIN. */

    ipiv = idamax_(&c__4, tmp, &c__1);
    u11 = tmp[ipiv - 1];
    if (abs(u11) <= smin) {
	*info = 1;
	u11 = smin;
    }
    u12 = tmp[locu12[ipiv - 1] - 1];
    l21 = tmp[locl21[ipiv - 1] - 1] / u11;
    u22 = tmp[locu22[ipiv - 1] - 1] - u12 * l21;
    xswap = xswpiv[ipiv - 1];
    bswap = bswpiv[ipiv - 1];
    if (abs(u22) <= smin) {
	*info = 1;
	u22 = smin;
    }
    if (bswap) {
	temp = btmp[1];
	btmp[1] = btmp[0] - l21 * temp;
	btmp[0] = temp;
    } else {
	btmp[1] -= l21 * btmp[0];
    }
    *scale = 1.;
    if (smlnum * 2. * abs(btmp[1]) > abs(u22) || smlnum * 2. * abs(btmp[0]) >
	    abs(u11)) {
/* Computing MAX */
	d__1 = abs(btmp[0]), d__2 = abs(btmp[1]);
	*scale = .5 / std::max(d__1,d__2);
	btmp[0] *= *scale;
	btmp[1] *= *scale;
    }
    x2[1] = btmp[1] / u22;
    x2[0] = btmp[0] / u11 - u12 / u11 * x2[1];
    if (xswap) {
	temp = x2[1];
	x2[1] = x2[0];
	x2[0] = temp;
    }
    x[x_dim1 + 1] = x2[0];
    if (*n1 == 1) {
	x[(x_dim1 << 1) + 1] = x2[1];
	*xnorm = (d__1 = x[x_dim1 + 1], abs(d__1)) + (d__2 = x[(x_dim1 << 1)
		+ 1], abs(d__2));
    } else {
	x[x_dim1 + 2] = x2[1];
/* Computing MAX */
	d__3 = (d__1 = x[x_dim1 + 1], abs(d__1)), d__4 = (d__2 = x[x_dim1 + 2]
		, abs(d__2));
	*xnorm = std::max(d__3,d__4);
    }
    return 0;

/*     2 by 2: */
/*     op[TL11 TL12]*[X11 X12] +ISGN* [X11 X12]*op[TR11 TR12] = [B11 B12] */
/*       [TL21 TL22] [X21 X22]        [X21 X22]   [TR21 TR22]   [B21 B22] */

/*     Solve equivalent 4 by 4 system using complete pivoting. */
/*     Set pivots less than SMIN to SMIN. */

L50:
/* Computing MAX */
    d__5 = (d__1 = tr[tr_dim1 + 1], abs(d__1)), d__6 = (d__2 = tr[(tr_dim1 <<
	    1) + 1], abs(d__2)), d__5 = std::max(d__5,d__6), d__6 = (d__3 = tr[
	    tr_dim1 + 2], abs(d__3)), d__5 = std::max(d__5,d__6), d__6 = (d__4 =
	    tr[(tr_dim1 << 1) + 2], abs(d__4));
    smin = std::max(d__5,d__6);
/* Computing MAX */
    d__5 = smin, d__6 = (d__1 = tl[tl_dim1 + 1], abs(d__1)), d__5 = std::max(d__5,
	    d__6), d__6 = (d__2 = tl[(tl_dim1 << 1) + 1], abs(d__2)), d__5 =
	    std::max(d__5,d__6), d__6 = (d__3 = tl[tl_dim1 + 2], abs(d__3)), d__5 =
	     std::max(d__5,d__6), d__6 = (d__4 = tl[(tl_dim1 << 1) + 2], abs(d__4))
	    ;
    smin = std::max(d__5,d__6);
/* Computing MAX */
    d__1 = eps * smin;
    smin = std::max(d__1,smlnum);
    btmp[0] = 0.;
    dcopy_(&c__16, btmp, &c__0, t16, &c__1);
    t16[0] = tl[tl_dim1 + 1] + sgn * tr[tr_dim1 + 1];
    t16[5] = tl[(tl_dim1 << 1) + 2] + sgn * tr[tr_dim1 + 1];
    t16[10] = tl[tl_dim1 + 1] + sgn * tr[(tr_dim1 << 1) + 2];
    t16[15] = tl[(tl_dim1 << 1) + 2] + sgn * tr[(tr_dim1 << 1) + 2];
    if (*ltranl) {
	t16[4] = tl[tl_dim1 + 2];
	t16[1] = tl[(tl_dim1 << 1) + 1];
	t16[14] = tl[tl_dim1 + 2];
	t16[11] = tl[(tl_dim1 << 1) + 1];
    } else {
	t16[4] = tl[(tl_dim1 << 1) + 1];
	t16[1] = tl[tl_dim1 + 2];
	t16[14] = tl[(tl_dim1 << 1) + 1];
	t16[11] = tl[tl_dim1 + 2];
    }
    if (*ltranr) {
	t16[8] = sgn * tr[(tr_dim1 << 1) + 1];
	t16[13] = sgn * tr[(tr_dim1 << 1) + 1];
	t16[2] = sgn * tr[tr_dim1 + 2];
	t16[7] = sgn * tr[tr_dim1 + 2];
    } else {
	t16[8] = sgn * tr[tr_dim1 + 2];
	t16[13] = sgn * tr[tr_dim1 + 2];
	t16[2] = sgn * tr[(tr_dim1 << 1) + 1];
	t16[7] = sgn * tr[(tr_dim1 << 1) + 1];
    }
    btmp[0] = b[b_dim1 + 1];
    btmp[1] = b[b_dim1 + 2];
    btmp[2] = b[(b_dim1 << 1) + 1];
    btmp[3] = b[(b_dim1 << 1) + 2];

/*     Perform elimination */

    for (i__ = 1; i__ <= 3; ++i__) {
	xmax = 0.;
	for (ip = i__; ip <= 4; ++ip) {
	    for (jp = i__; jp <= 4; ++jp) {
		if ((d__1 = t16[ip + (jp << 2) - 5], abs(d__1)) >= xmax) {
		    xmax = (d__1 = t16[ip + (jp << 2) - 5], abs(d__1));
		    ipsv = ip;
		    jpsv = jp;
		}
/* L60: */
	    }
/* L70: */
	}
	if (ipsv != i__) {
	    dswap_(&c__4, &t16[ipsv - 1], &c__4, &t16[i__ - 1], &c__4);
	    temp = btmp[i__ - 1];
	    btmp[i__ - 1] = btmp[ipsv - 1];
	    btmp[ipsv - 1] = temp;
	}
	if (jpsv != i__) {
	    dswap_(&c__4, &t16[(jpsv << 2) - 4], &c__1, &t16[(i__ << 2) - 4],
		    &c__1);
	}
	jpiv[i__ - 1] = jpsv;
	if ((d__1 = t16[i__ + (i__ << 2) - 5], abs(d__1)) < smin) {
	    *info = 1;
	    t16[i__ + (i__ << 2) - 5] = smin;
	}
	for (j = i__ + 1; j <= 4; ++j) {
	    t16[j + (i__ << 2) - 5] /= t16[i__ + (i__ << 2) - 5];
	    btmp[j - 1] -= t16[j + (i__ << 2) - 5] * btmp[i__ - 1];
	    for (k = i__ + 1; k <= 4; ++k) {
		t16[j + (k << 2) - 5] -= t16[j + (i__ << 2) - 5] * t16[i__ + (
			k << 2) - 5];
/* L80: */
	    }
/* L90: */
	}
/* L100: */
    }
    if (abs(t16[15]) < smin) {
	t16[15] = smin;
    }
    *scale = 1.;
    if (smlnum * 8. * abs(btmp[0]) > abs(t16[0]) || smlnum * 8. * abs(btmp[1])
	     > abs(t16[5]) || smlnum * 8. * abs(btmp[2]) > abs(t16[10]) ||
	    smlnum * 8. * abs(btmp[3]) > abs(t16[15])) {
/* Computing MAX */
	d__1 = abs(btmp[0]), d__2 = abs(btmp[1]), d__1 = std::max(d__1,d__2), d__2
		= abs(btmp[2]), d__1 = std::max(d__1,d__2), d__2 = abs(btmp[3]);
	*scale = .125 / std::max(d__1,d__2);
	btmp[0] *= *scale;
	btmp[1] *= *scale;
	btmp[2] *= *scale;
	btmp[3] *= *scale;
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	k = 5 - i__;
	temp = 1. / t16[k + (k << 2) - 5];
	tmp[k - 1] = btmp[k - 1] * temp;
	for (j = k + 1; j <= 4; ++j) {
	    tmp[k - 1] -= temp * t16[k + (j << 2) - 5] * tmp[j - 1];
/* L110: */
	}
/* L120: */
    }
    for (i__ = 1; i__ <= 3; ++i__) {
	if (jpiv[4 - i__ - 1] != 4 - i__) {
	    temp = tmp[4 - i__ - 1];
	    tmp[4 - i__ - 1] = tmp[jpiv[4 - i__ - 1] - 1];
	    tmp[jpiv[4 - i__ - 1] - 1] = temp;
	}
/* L130: */
    }
    x[x_dim1 + 1] = tmp[0];
    x[x_dim1 + 2] = tmp[1];
    x[(x_dim1 << 1) + 1] = tmp[2];
    x[(x_dim1 << 1) + 2] = tmp[3];
/* Computing MAX */
    d__1 = abs(tmp[0]) + abs(tmp[2]), d__2 = abs(tmp[1]) + abs(tmp[3]);
    *xnorm = std::max(d__1,d__2);
    return 0;

/*     End of DLASY2 */

} /* dlasy2_ */

/* Subroutine */ int dlasyf_(const char *uplo, integer *n, integer *nb, integer *kb,
	double *a, integer *lda, integer *ipiv, double *w, integer *ldw, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b8 = -1.;
	static double c_b9 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, w_dim1, w_offset, i__1, i__2, i__3, i__4, i__5;
    double d__1, d__2, d__3;

    /* Local variables */
    integer j, k;
    double t, r1, d11, d21, d22;
    integer jb, jj, kk, jp, kp, kw, kkw, imax, jmax;
    double alpha;
    integer kstep;
    double absakk;
    double colmax, rowmax;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASYF computes a partial factorization of a real symmetric matrix A */
/*  using the Bunch-Kaufman diagonal pivoting method. The partial */
/*  factorization has the form: */

/*  A  =  ( I  U12 ) ( A11  0  ) (  I    0   )  if UPLO = 'U', or: */
/*        ( 0  U22 ) (  0   D  ) ( U12' U22' ) */

/*  A  =  ( L11  0 ) (  D   0  ) ( L11' L21' )  if UPLO = 'L' */
/*        ( L21  I ) (  0  A22 ) (  0    I   ) */

/*  where the order of D is at most NB. The actual order is returned in */
/*  the argument KB, and is either NB or NB-1, or N if N <= NB. */

/*  DLASYF is an auxiliary routine called by DSYTRF. It uses blocked code */
/*  (calling Level 3 BLAS) to update the submatrix A11 (if UPLO = 'U') or */
/*  A22 (if UPLO = 'L'). */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NB      (input) INTEGER */
/*          The maximum number of columns of the matrix A that should be */
/*          factored.  NB should be at least 2 to allow for 2-by-2 pivot */
/*          blocks. */

/*  KB      (output) INTEGER */
/*          The number of columns of A that were actually factored. */
/*          KB is either NB-1 or NB, or N if N <= NB. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          n-by-n upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading n-by-n lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */
/*          On exit, A contains details of the partial factorization. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (output) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D. */
/*          If UPLO = 'U', only the last KB elements of IPIV are set; */
/*          if UPLO = 'L', only the first KB elements are set. */

/*          If IPIV(k) > 0, then rows and columns k and IPIV(k) were */
/*          interchanged and D(k,k) is a 1-by-1 diagonal block. */
/*          If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, then rows and */
/*          columns k-1 and -IPIV(k) were interchanged and D(k-1:k,k-1:k) */
/*          is a 2-by-2 diagonal block.  If UPLO = 'L' and IPIV(k) = */
/*          IPIV(k+1) < 0, then rows and columns k+1 and -IPIV(k) were */
/*          interchanged and D(k:k+1,k:k+1) is a 2-by-2 diagonal block. */

/*  W       (workspace) DOUBLE PRECISION array, dimension (LDW,NB) */

/*  LDW     (input) INTEGER */
/*          The leading dimension of the array W.  LDW >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          > 0: if INFO = k, D(k,k) is exactly zero.  The factorization */
/*               has been completed, but the block diagonal matrix D is */
/*               exactly singular. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --ipiv;
    w_dim1 = *ldw;
    w_offset = 1 + w_dim1;
    w -= w_offset;

    /* Function Body */
    *info = 0;

/*     Initialize ALPHA for use in choosing pivot block size. */

    alpha = (sqrt(17.) + 1.) / 8.;

    if (lsame_(uplo, "U")) {

/*        Factorize the trailing columns of A using the upper triangle */
/*        of A and working backwards, and compute the matrix W = U12*D */
/*        for use in updating A11 */

/*        K is the main loop index, decreasing from N in steps of 1 or 2 */

/*        KW is the column of W which corresponds to column K of A */

	k = *n;
L10:
	kw = *nb + k - *n;

/*        Exit from loop */

	if (k <= *n - *nb + 1 && *nb < *n || k < 1) {
	    goto L30;
	}

/*        Copy column K of A to column KW of W and update it */

	dcopy_(&k, &a[k * a_dim1 + 1], &c__1, &w[kw * w_dim1 + 1], &c__1);
	if (k < *n) {
	    i__1 = *n - k;
	    dgemv_("No transpose", &k, &i__1, &c_b8, &a[(k + 1) * a_dim1 + 1],
		     lda, &w[k + (kw + 1) * w_dim1], ldw, &c_b9, &w[kw *
		    w_dim1 + 1], &c__1);
	}

	kstep = 1;

/*        Determine rows and columns to be interchanged and whether */
/*        a 1-by-1 or 2-by-2 pivot block will be used */

	absakk = (d__1 = w[k + kw * w_dim1], abs(d__1));

/*        IMAX is the row-index of the largest off-diagonal element in */
/*        column K, and COLMAX is its absolute value */

	if (k > 1) {
	    i__1 = k - 1;
	    imax = idamax_(&i__1, &w[kw * w_dim1 + 1], &c__1);
	    colmax = (d__1 = w[imax + kw * w_dim1], abs(d__1));
	} else {
	    colmax = 0.;
	}

	if (std::max(absakk,colmax) == 0.) {

/*           Column K is zero: set INFO and continue */

	    if (*info == 0) {
		*info = k;
	    }
	    kp = k;
	} else {
	    if (absakk >= alpha * colmax) {

/*              no interchange, use 1-by-1 pivot block */

		kp = k;
	    } else {

/*              Copy column IMAX to column KW-1 of W and update it */

		dcopy_(&imax, &a[imax * a_dim1 + 1], &c__1, &w[(kw - 1) *
			w_dim1 + 1], &c__1);
		i__1 = k - imax;
		dcopy_(&i__1, &a[imax + (imax + 1) * a_dim1], lda, &w[imax +
			1 + (kw - 1) * w_dim1], &c__1);
		if (k < *n) {
		    i__1 = *n - k;
		    dgemv_("No transpose", &k, &i__1, &c_b8, &a[(k + 1) *
			    a_dim1 + 1], lda, &w[imax + (kw + 1) * w_dim1],
			    ldw, &c_b9, &w[(kw - 1) * w_dim1 + 1], &c__1);
		}

/*              JMAX is the column-index of the largest off-diagonal */
/*              element in row IMAX, and ROWMAX is its absolute value */

		i__1 = k - imax;
		jmax = imax + idamax_(&i__1, &w[imax + 1 + (kw - 1) * w_dim1],
			 &c__1);
		rowmax = (d__1 = w[jmax + (kw - 1) * w_dim1], abs(d__1));
		if (imax > 1) {
		    i__1 = imax - 1;
		    jmax = idamax_(&i__1, &w[(kw - 1) * w_dim1 + 1], &c__1);
/* Computing MAX */
		    d__2 = rowmax, d__3 = (d__1 = w[jmax + (kw - 1) * w_dim1],
			     abs(d__1));
		    rowmax = std::max(d__2,d__3);
		}

		if (absakk >= alpha * colmax * (colmax / rowmax)) {

/*                 no interchange, use 1-by-1 pivot block */

		    kp = k;
		} else if ((d__1 = w[imax + (kw - 1) * w_dim1], abs(d__1)) >=
			alpha * rowmax) {

/*                 interchange rows and columns K and IMAX, use 1-by-1 */
/*                 pivot block */

		    kp = imax;

/*                 copy column KW-1 of W to column KW */

		    dcopy_(&k, &w[(kw - 1) * w_dim1 + 1], &c__1, &w[kw *
			    w_dim1 + 1], &c__1);
		} else {

/*                 interchange rows and columns K-1 and IMAX, use 2-by-2 */
/*                 pivot block */

		    kp = imax;
		    kstep = 2;
		}
	    }

	    kk = k - kstep + 1;
	    kkw = *nb + kk - *n;

/*           Updated column KP is already stored in column KKW of W */

	    if (kp != kk) {

/*              Copy non-updated column KK to column KP */

		a[kp + k * a_dim1] = a[kk + k * a_dim1];
		i__1 = k - 1 - kp;
		dcopy_(&i__1, &a[kp + 1 + kk * a_dim1], &c__1, &a[kp + (kp +
			1) * a_dim1], lda);
		dcopy_(&kp, &a[kk * a_dim1 + 1], &c__1, &a[kp * a_dim1 + 1], &
			c__1);

/*              Interchange rows KK and KP in last KK columns of A and W */

		i__1 = *n - kk + 1;
		dswap_(&i__1, &a[kk + kk * a_dim1], lda, &a[kp + kk * a_dim1],
			 lda);
		i__1 = *n - kk + 1;
		dswap_(&i__1, &w[kk + kkw * w_dim1], ldw, &w[kp + kkw *
			w_dim1], ldw);
	    }

	    if (kstep == 1) {

/*              1-by-1 pivot block D(k): column KW of W now holds */

/*              W(k) = U(k)*D(k) */

/*              where U(k) is the k-th column of U */

/*              Store U(k) in column k of A */

		dcopy_(&k, &w[kw * w_dim1 + 1], &c__1, &a[k * a_dim1 + 1], &
			c__1);
		r1 = 1. / a[k + k * a_dim1];
		i__1 = k - 1;
		dscal_(&i__1, &r1, &a[k * a_dim1 + 1], &c__1);
	    } else {

/*              2-by-2 pivot block D(k): columns KW and KW-1 of W now */
/*              hold */

/*              ( W(k-1) W(k) ) = ( U(k-1) U(k) )*D(k) */

/*              where U(k) and U(k-1) are the k-th and (k-1)-th columns */
/*              of U */

		if (k > 2) {

/*                 Store U(k) and U(k-1) in columns k and k-1 of A */

		    d21 = w[k - 1 + kw * w_dim1];
		    d11 = w[k + kw * w_dim1] / d21;
		    d22 = w[k - 1 + (kw - 1) * w_dim1] / d21;
		    t = 1. / (d11 * d22 - 1.);
		    d21 = t / d21;
		    i__1 = k - 2;
		    for (j = 1; j <= i__1; ++j) {
			a[j + (k - 1) * a_dim1] = d21 * (d11 * w[j + (kw - 1)
				* w_dim1] - w[j + kw * w_dim1]);
			a[j + k * a_dim1] = d21 * (d22 * w[j + kw * w_dim1] -
				w[j + (kw - 1) * w_dim1]);
/* L20: */
		    }
		}

/*              Copy D(k) to A */

		a[k - 1 + (k - 1) * a_dim1] = w[k - 1 + (kw - 1) * w_dim1];
		a[k - 1 + k * a_dim1] = w[k - 1 + kw * w_dim1];
		a[k + k * a_dim1] = w[k + kw * w_dim1];
	    }
	}

/*        Store details of the interchanges in IPIV */

	if (kstep == 1) {
	    ipiv[k] = kp;
	} else {
	    ipiv[k] = -kp;
	    ipiv[k - 1] = -kp;
	}

/*        Decrease K and return to the start of the main loop */

	k -= kstep;
	goto L10;

L30:

/*        Update the upper triangle of A11 (= A(1:k,1:k)) as */

/*        A11 := A11 - U12*D*U12' = A11 - U12*W' */

/*        computing blocks of NB columns at a time */

	i__1 = -(*nb);
	for (j = (k - 1) / *nb * *nb + 1; i__1 < 0 ? j >= 1 : j <= 1; j +=
		i__1) {
/* Computing MIN */
	    i__2 = *nb, i__3 = k - j + 1;
	    jb = std::min(i__2,i__3);

/*           Update the upper triangle of the diagonal block */

	    i__2 = j + jb - 1;
	    for (jj = j; jj <= i__2; ++jj) {
		i__3 = jj - j + 1;
		i__4 = *n - k;
		dgemv_("No transpose", &i__3, &i__4, &c_b8, &a[j + (k + 1) *
			a_dim1], lda, &w[jj + (kw + 1) * w_dim1], ldw, &c_b9,
			&a[j + jj * a_dim1], &c__1);
/* L40: */
	    }

/*           Update the rectangular superdiagonal block */

	    i__2 = j - 1;
	    i__3 = *n - k;
	    dgemm_("No transpose", "Transpose", &i__2, &jb, &i__3, &c_b8, &a[(
		    k + 1) * a_dim1 + 1], lda, &w[j + (kw + 1) * w_dim1], ldw,
		     &c_b9, &a[j * a_dim1 + 1], lda);
/* L50: */
	}

/*        Put U12 in standard form by partially undoing the interchanges */
/*        in columns k+1:n */

	j = k + 1;
L60:
	jj = j;
	jp = ipiv[j];
	if (jp < 0) {
	    jp = -jp;
	    ++j;
	}
	++j;
	if (jp != jj && j <= *n) {
	    i__1 = *n - j + 1;
	    dswap_(&i__1, &a[jp + j * a_dim1], lda, &a[jj + j * a_dim1], lda);
	}
	if (j <= *n) {
	    goto L60;
	}

/*        Set KB to the number of columns factorized */

	*kb = *n - k;

    } else {

/*        Factorize the leading columns of A using the lower triangle */
/*        of A and working forwards, and compute the matrix W = L21*D */
/*        for use in updating A22 */

/*        K is the main loop index, increasing from 1 in steps of 1 or 2 */

	k = 1;
L70:

/*        Exit from loop */

	if (k >= *nb && *nb < *n || k > *n) {
	    goto L90;
	}

/*        Copy column K of A to column K of W and update it */

	i__1 = *n - k + 1;
	dcopy_(&i__1, &a[k + k * a_dim1], &c__1, &w[k + k * w_dim1], &c__1);
	i__1 = *n - k + 1;
	i__2 = k - 1;
	dgemv_("No transpose", &i__1, &i__2, &c_b8, &a[k + a_dim1], lda, &w[k
		+ w_dim1], ldw, &c_b9, &w[k + k * w_dim1], &c__1);

	kstep = 1;

/*        Determine rows and columns to be interchanged and whether */
/*        a 1-by-1 or 2-by-2 pivot block will be used */

	absakk = (d__1 = w[k + k * w_dim1], abs(d__1));

/*        IMAX is the row-index of the largest off-diagonal element in */
/*        column K, and COLMAX is its absolute value */

	if (k < *n) {
	    i__1 = *n - k;
	    imax = k + idamax_(&i__1, &w[k + 1 + k * w_dim1], &c__1);
	    colmax = (d__1 = w[imax + k * w_dim1], abs(d__1));
	} else {
	    colmax = 0.;
	}

	if (std::max(absakk,colmax) == 0.) {

/*           Column K is zero: set INFO and continue */

	    if (*info == 0) {
		*info = k;
	    }
	    kp = k;
	} else {
	    if (absakk >= alpha * colmax) {

/*              no interchange, use 1-by-1 pivot block */

		kp = k;
	    } else {

/*              Copy column IMAX to column K+1 of W and update it */

		i__1 = imax - k;
		dcopy_(&i__1, &a[imax + k * a_dim1], lda, &w[k + (k + 1) *
			w_dim1], &c__1);
		i__1 = *n - imax + 1;
		dcopy_(&i__1, &a[imax + imax * a_dim1], &c__1, &w[imax + (k +
			1) * w_dim1], &c__1);
		i__1 = *n - k + 1;
		i__2 = k - 1;
		dgemv_("No transpose", &i__1, &i__2, &c_b8, &a[k + a_dim1],
			lda, &w[imax + w_dim1], ldw, &c_b9, &w[k + (k + 1) *
			w_dim1], &c__1);

/*              JMAX is the column-index of the largest off-diagonal */
/*              element in row IMAX, and ROWMAX is its absolute value */

		i__1 = imax - k;
		jmax = k - 1 + idamax_(&i__1, &w[k + (k + 1) * w_dim1], &c__1)
			;
		rowmax = (d__1 = w[jmax + (k + 1) * w_dim1], abs(d__1));
		if (imax < *n) {
		    i__1 = *n - imax;
		    jmax = imax + idamax_(&i__1, &w[imax + 1 + (k + 1) *
			    w_dim1], &c__1);
/* Computing MAX */
		    d__2 = rowmax, d__3 = (d__1 = w[jmax + (k + 1) * w_dim1],
			    abs(d__1));
		    rowmax = std::max(d__2,d__3);
		}

		if (absakk >= alpha * colmax * (colmax / rowmax)) {

/*                 no interchange, use 1-by-1 pivot block */

		    kp = k;
		} else if ((d__1 = w[imax + (k + 1) * w_dim1], abs(d__1)) >=
			alpha * rowmax) {

/*                 interchange rows and columns K and IMAX, use 1-by-1 */
/*                 pivot block */

		    kp = imax;

/*                 copy column K+1 of W to column K */

		    i__1 = *n - k + 1;
		    dcopy_(&i__1, &w[k + (k + 1) * w_dim1], &c__1, &w[k + k *
			    w_dim1], &c__1);
		} else {

/*                 interchange rows and columns K+1 and IMAX, use 2-by-2 */
/*                 pivot block */

		    kp = imax;
		    kstep = 2;
		}
	    }

	    kk = k + kstep - 1;

/*           Updated column KP is already stored in column KK of W */

	    if (kp != kk) {

/*              Copy non-updated column KK to column KP */

		a[kp + k * a_dim1] = a[kk + k * a_dim1];
		i__1 = kp - k - 1;
		dcopy_(&i__1, &a[k + 1 + kk * a_dim1], &c__1, &a[kp + (k + 1)
			* a_dim1], lda);
		i__1 = *n - kp + 1;
		dcopy_(&i__1, &a[kp + kk * a_dim1], &c__1, &a[kp + kp *
			a_dim1], &c__1);

/*              Interchange rows KK and KP in first KK columns of A and W */

		dswap_(&kk, &a[kk + a_dim1], lda, &a[kp + a_dim1], lda);
		dswap_(&kk, &w[kk + w_dim1], ldw, &w[kp + w_dim1], ldw);
	    }

	    if (kstep == 1) {

/*              1-by-1 pivot block D(k): column k of W now holds */

/*              W(k) = L(k)*D(k) */

/*              where L(k) is the k-th column of L */

/*              Store L(k) in column k of A */

		i__1 = *n - k + 1;
		dcopy_(&i__1, &w[k + k * w_dim1], &c__1, &a[k + k * a_dim1], &
			c__1);
		if (k < *n) {
		    r1 = 1. / a[k + k * a_dim1];
		    i__1 = *n - k;
		    dscal_(&i__1, &r1, &a[k + 1 + k * a_dim1], &c__1);
		}
	    } else {

/*              2-by-2 pivot block D(k): columns k and k+1 of W now hold */

/*              ( W(k) W(k+1) ) = ( L(k) L(k+1) )*D(k) */

/*              where L(k) and L(k+1) are the k-th and (k+1)-th columns */
/*              of L */

		if (k < *n - 1) {

/*                 Store L(k) and L(k+1) in columns k and k+1 of A */

		    d21 = w[k + 1 + k * w_dim1];
		    d11 = w[k + 1 + (k + 1) * w_dim1] / d21;
		    d22 = w[k + k * w_dim1] / d21;
		    t = 1. / (d11 * d22 - 1.);
		    d21 = t / d21;
		    i__1 = *n;
		    for (j = k + 2; j <= i__1; ++j) {
			a[j + k * a_dim1] = d21 * (d11 * w[j + k * w_dim1] -
				w[j + (k + 1) * w_dim1]);
			a[j + (k + 1) * a_dim1] = d21 * (d22 * w[j + (k + 1) *
				 w_dim1] - w[j + k * w_dim1]);
/* L80: */
		    }
		}

/*              Copy D(k) to A */

		a[k + k * a_dim1] = w[k + k * w_dim1];
		a[k + 1 + k * a_dim1] = w[k + 1 + k * w_dim1];
		a[k + 1 + (k + 1) * a_dim1] = w[k + 1 + (k + 1) * w_dim1];
	    }
	}

/*        Store details of the interchanges in IPIV */

	if (kstep == 1) {
	    ipiv[k] = kp;
	} else {
	    ipiv[k] = -kp;
	    ipiv[k + 1] = -kp;
	}

/*        Increase K and return to the start of the main loop */

	k += kstep;
	goto L70;

L90:

/*        Update the lower triangle of A22 (= A(k:n,k:n)) as */

/*        A22 := A22 - L21*D*L21' = A22 - L21*W' */

/*        computing blocks of NB columns at a time */

	i__1 = *n;
	i__2 = *nb;
	for (j = k; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {
/* Computing MIN */
	    i__3 = *nb, i__4 = *n - j + 1;
	    jb = std::min(i__3,i__4);

/*           Update the lower triangle of the diagonal block */

	    i__3 = j + jb - 1;
	    for (jj = j; jj <= i__3; ++jj) {
		i__4 = j + jb - jj;
		i__5 = k - 1;
		dgemv_("No transpose", &i__4, &i__5, &c_b8, &a[jj + a_dim1],
			lda, &w[jj + w_dim1], ldw, &c_b9, &a[jj + jj * a_dim1]
, &c__1);
/* L100: */
	    }

/*           Update the rectangular subdiagonal block */

	    if (j + jb <= *n) {
		i__3 = *n - j - jb + 1;
		i__4 = k - 1;
		dgemm_("No transpose", "Transpose", &i__3, &jb, &i__4, &c_b8,
			&a[j + jb + a_dim1], lda, &w[j + w_dim1], ldw, &c_b9,
			&a[j + jb + j * a_dim1], lda);
	    }
/* L110: */
	}

/*        Put L21 in standard form by partially undoing the interchanges */
/*        in columns 1:k-1 */

	j = k - 1;
L120:
	jj = j;
	jp = ipiv[j];
	if (jp < 0) {
	    jp = -jp;
	    --j;
	}
	--j;
	if (jp != jj && j >= 1) {
	    dswap_(&j, &a[jp + a_dim1], lda, &a[jj + a_dim1], lda);
	}
	if (j >= 1) {
	    goto L120;
	}

/*        Set KB to the number of columns factorized */

	*kb = k - 1;

    }
    return 0;

/*     End of DLASYF */

} /* dlasyf_ */

int dlat2s_(const char *uplo, integer *n, double *a, integer *lda, float *sa, integer *ldsa, integer *info)
{
    /* System generated locals */
    integer sa_dim1, sa_offset, a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, j;
    double rmax;
    bool upper;



/*  -- LAPACK PROTOTYPE auxiliary routine (version 3.1.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     May 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAT2S converts a DOUBLE PRECISION triangular matrix, SA, to a SINGLE */
/*  PRECISION triangular matrix, A. */

/*  RMAX is the overflow for the SINGLE PRECISION arithmetic */
/*  DLAS2S checks that all the entries of A are between -RMAX and */
/*  RMAX. If not the convertion is aborted and a flag is raised. */

/*  This is an auxiliary routine so there is no argument checking. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  N       (input) INTEGER */
/*          The number of rows and columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the N-by-N triangular coefficient matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  SA      (output) REAL array, dimension (LDSA,N) */
/*          Only the UPLO part of SA is referenced.  On exit, if INFO=0, */
/*          the N-by-N coefficient matrix SA; if INFO>0, the content of */
/*          the UPLO part of SA is unspecified. */

/*  LDSA    (input) INTEGER */
/*          The leading dimension of the array SA.  LDSA >= max(1,M). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          = 1:  an entry of the matrix A is greater than the SINGLE */
/*                PRECISION overflow threshold, in this case, the content */
/*                of the UPLO part of SA in exit is unspecified. */

/*  ========= */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    sa_dim1 = *ldsa;
    sa_offset = 1 + sa_dim1;
    sa -= sa_offset;

    /* Function Body */
    rmax = slamch_("O");
    upper = lsame_(uplo, "U");
    if (upper) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		if (a[i__ + j * a_dim1] < -rmax || a[i__ + j * a_dim1] > rmax)
			 {
		    *info = 1;
		    goto L50;
		}
		sa[i__ + j * sa_dim1] = a[i__ + j * a_dim1];
/* L10: */
	    }
/* L20: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j; i__ <= i__2; ++i__) {
		if (a[i__ + j * a_dim1] < -rmax || a[i__ + j * a_dim1] > rmax)
			 {
		    *info = 1;
		    goto L50;
		}
		sa[i__ + j * sa_dim1] = a[i__ + j * a_dim1];
/* L30: */
	    }
/* L40: */
	}
    }
L50:

    return 0;

/*     End of DLAT2S */

} /* dlat2s_ */

/* Subroutine */ int dlatbs_(const char *uplo, const char *trans, const char *diag, const char *
	normin, integer *n, integer *kd, double *ab, integer *ldab,
	double *x, double *scale, double *cnorm, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b36 = .5;

    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j;
    double xj, rec, tjj;
    integer jinc, jlen;
    double xbnd;
    integer imax;
    double tmax, tjjs, xmax, grow, sumj;
    integer maind;
    double tscal, uscal;
    integer jlast;
    bool upper;
    double bignum;
    bool notran;
    integer jfirst;
    double smlnum;
    bool nounit;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLATBS solves one of the triangular systems */

/*     A *x = s*b  or  A'*x = s*b */

/*  with scaling to prevent overflow, where A is an upper or lower */
/*  triangular band matrix.  Here A' denotes the transpose of A, x and b */
/*  are n-element vectors, and s is a scaling factor, usually less than */
/*  or equal to 1, chosen so that the components of x will be less than */
/*  the overflow threshold.  If the unscaled problem will not cause */
/*  overflow, the Level 2 BLAS routine DTBSV is called.  If the matrix A */
/*  is singular (A(j,j) = 0 for some j), then s is set to 0 and a */
/*  non-trivial solution to A*x = 0 is returned. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the operation applied to A. */
/*          = 'N':  Solve A * x = s*b  (No transpose) */
/*          = 'T':  Solve A'* x = s*b  (Transpose) */
/*          = 'C':  Solve A'* x = s*b  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  NORMIN  (input) CHARACTER*1 */
/*          Specifies whether CNORM has been set or not. */
/*          = 'Y':  CNORM contains the column norms on entry */
/*          = 'N':  CNORM is not set on entry.  On exit, the norms will */
/*                  be computed and stored in CNORM. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of subdiagonals or superdiagonals in the */
/*          triangular matrix A.  KD >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The upper or lower triangular band matrix A, stored in the */
/*          first KD+1 rows of the array. The j-th column of A is stored */
/*          in the j-th column of the array AB as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  X       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the right hand side b of the triangular system. */
/*          On exit, X is overwritten by the solution vector x. */

/*  SCALE   (output) DOUBLE PRECISION */
/*          The scaling factor s for the triangular system */
/*             A * x = s*b  or  A'* x = s*b. */
/*          If SCALE = 0, the matrix A is singular or badly scaled, and */
/*          the vector x is an exact or approximate solution to A*x = 0. */

/*  CNORM   (input or output) DOUBLE PRECISION array, dimension (N) */

/*          If NORMIN = 'Y', CNORM is an input argument and CNORM(j) */
/*          contains the norm of the off-diagonal part of the j-th column */
/*          of A.  If TRANS = 'N', CNORM(j) must be greater than or equal */
/*          to the infinity-norm, and if TRANS = 'T' or 'C', CNORM(j) */
/*          must be greater than or equal to the 1-norm. */

/*          If NORMIN = 'N', CNORM is an output argument and CNORM(j) */
/*          returns the 1-norm of the offdiagonal part of the j-th column */
/*          of A. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -k, the k-th argument had an illegal value */

/*  Further Details */
/*  ======= ======= */

/*  A rough bound on x is computed; if that is less than overflow, DTBSV */
/*  is called, otherwise, specific code is used which checks for possible */
/*  overflow or divide-by-zero at every operation. */

/*  A columnwise scheme is used for solving A*x = b.  The basic algorithm */
/*  if A is lower triangular is */

/*       x[1:n] := b[1:n] */
/*       for j = 1, ..., n */
/*            x(j) := x(j) / A(j,j) */
/*            x[j+1:n] := x[j+1:n] - x(j) * A[j+1:n,j] */
/*       end */

/*  Define bounds on the components of x after j iterations of the loop: */
/*     M(j) = bound on x[1:j] */
/*     G(j) = bound on x[j+1:n] */
/*  Initially, let M(0) = 0 and G(0) = max{x(i), i=1,...,n}. */

/*  Then for iteration j+1 we have */
/*     M(j+1) <= G(j) / | A(j+1,j+1) | */
/*     G(j+1) <= G(j) + M(j+1) * | A[j+2:n,j+1] | */
/*            <= G(j) ( 1 + CNORM(j+1) / | A(j+1,j+1) | ) */

/*  where CNORM(j+1) is greater than or equal to the infinity-norm of */
/*  column j+1 of A, not counting the diagonal.  Hence */

/*     G(j) <= G(0) product ( 1 + CNORM(i) / | A(i,i) | ) */
/*                  1<=i<=j */
/*  and */

/*     |x(j)| <= ( G(0) / |A(j,j)| ) product ( 1 + CNORM(i) / |A(i,i)| ) */
/*                                   1<=i< j */

/*  Since |x(j)| <= M(j), we use the Level 2 BLAS routine DTBSV if the */
/*  reciprocal of the largest M(j), j=1,..,n, is larger than */
/*  max(underflow, 1/overflow). */

/*  The bound on x(j) is also used to determine when a step in the */
/*  columnwise method can be performed without fear of overflow.  If */
/*  the computed bound is greater than a large constant, x is scaled to */
/*  prevent overflow, but if the bound overflows, x is set to 0, x(j) to */
/*  1, and scale to 0, and a non-trivial solution to A*x = 0 is found. */

/*  Similarly, a row-wise scheme is used to solve A'*x = b.  The basic */
/*  algorithm for A upper triangular is */

/*       for j = 1, ..., n */
/*            x(j) := ( b(j) - A[1:j-1,j]' * x[1:j-1] ) / A(j,j) */
/*       end */

/*  We simultaneously compute two bounds */
/*       G(j) = bound on ( b(i) - A[1:i-1,i]' * x[1:i-1] ), 1<=i<=j */
/*       M(j) = bound on x(i), 1<=i<=j */

/*  The initial values are G(0) = 0, M(0) = max{b(i), i=1,..,n}, and we */
/*  add the constraint G(j) >= G(j-1) and M(j) >= M(j-1) for j >= 1. */
/*  Then the bound on x(j) is */

/*       M(j) <= M(j-1) * ( 1 + CNORM(j) ) / | A(j,j) | */

/*            <= M(0) * product ( ( 1 + CNORM(i) ) / |A(i,i)| ) */
/*                      1<=i<=j */

/*  and we can safely call DTBSV if 1/M(n) and 1/G(n) are both greater */
/*  than max(underflow, 1/overflow). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --x;
    --cnorm;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    notran = lsame_(trans, "N");
    nounit = lsame_(diag, "N");

/*     Test the input parameters. */

    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T") && !
	    lsame_(trans, "C")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (! lsame_(normin, "Y") && ! lsame_(normin,
	     "N")) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*kd < 0) {
	*info = -6;
    } else if (*ldab < *kd + 1) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLATBS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Determine machine dependent parameters to control overflow. */

    smlnum = dlamch_("Safe minimum") / dlamch_("Precision");
    bignum = 1. / smlnum;
    *scale = 1.;

    if (lsame_(normin, "N")) {

/*        Compute the 1-norm of each column, not including the diagonal. */

	if (upper) {

/*           A is upper triangular. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__2 = *kd, i__3 = j - 1;
		jlen = std::min(i__2,i__3);
		cnorm[j] = dasum_(&jlen, &ab[*kd + 1 - jlen + j * ab_dim1], &
			c__1);
/* L10: */
	    }
	} else {

/*           A is lower triangular. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__2 = *kd, i__3 = *n - j;
		jlen = std::min(i__2,i__3);
		if (jlen > 0) {
		    cnorm[j] = dasum_(&jlen, &ab[j * ab_dim1 + 2], &c__1);
		} else {
		    cnorm[j] = 0.;
		}
/* L20: */
	    }
	}
    }

/*     Scale the column norms by TSCAL if the maximum element in CNORM is */
/*     greater than BIGNUM. */

    imax = idamax_(n, &cnorm[1], &c__1);
    tmax = cnorm[imax];
    if (tmax <= bignum) {
	tscal = 1.;
    } else {
	tscal = 1. / (smlnum * tmax);
	dscal_(n, &tscal, &cnorm[1], &c__1);
    }

/*     Compute a bound on the computed solution vector to see if the */
/*     Level 2 BLAS routine DTBSV can be used. */

    j = idamax_(n, &x[1], &c__1);
    xmax = (d__1 = x[j], abs(d__1));
    xbnd = xmax;
    if (notran) {

/*        Compute the growth in A * x = b. */

	if (upper) {
	    jfirst = *n;
	    jlast = 1;
	    jinc = -1;
	    maind = *kd + 1;
	} else {
	    jfirst = 1;
	    jlast = *n;
	    jinc = 1;
	    maind = 1;
	}

	if (tscal != 1.) {
	    grow = 0.;
	    goto L50;
	}

	if (nounit) {

/*           A is non-unit triangular. */

/*           Compute GROW = 1/G(j) and XBND = 1/M(j). */
/*           Initially, G(0) = max{x(i), i=1,...,n}. */

	    grow = 1. / std::max(xbnd,smlnum);
	    xbnd = grow;
	    i__1 = jlast;
	    i__2 = jinc;
	    for (j = jfirst; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L50;
		}

/*              M(j) = G(j-1) / abs(A(j,j)) */

		tjj = (d__1 = ab[maind + j * ab_dim1], abs(d__1));
/* Computing MIN */
		d__1 = xbnd, d__2 = std::min(1.,tjj) * grow;
		xbnd = std::min(d__1,d__2);
		if (tjj + cnorm[j] >= smlnum) {

/*                 G(j) = G(j-1)*( 1 + CNORM(j) / abs(A(j,j)) ) */

		    grow *= tjj / (tjj + cnorm[j]);
		} else {

/*                 G(j) could overflow, set GROW to 0. */

		    grow = 0.;
		}
/* L30: */
	    }
	    grow = xbnd;
	} else {

/*           A is unit triangular. */

/*           Compute GROW = 1/G(j), where G(0) = max{x(i), i=1,...,n}. */

/* Computing MIN */
	    d__1 = 1., d__2 = 1. / std::max(xbnd,smlnum);
	    grow = std::min(d__1,d__2);
	    i__2 = jlast;
	    i__1 = jinc;
	    for (j = jfirst; i__1 < 0 ? j >= i__2 : j <= i__2; j += i__1) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L50;
		}

/*              G(j) = G(j-1)*( 1 + CNORM(j) ) */

		grow *= 1. / (cnorm[j] + 1.);
/* L40: */
	    }
	}
L50:

	;
    } else {

/*        Compute the growth in A' * x = b. */

	if (upper) {
	    jfirst = 1;
	    jlast = *n;
	    jinc = 1;
	    maind = *kd + 1;
	} else {
	    jfirst = *n;
	    jlast = 1;
	    jinc = -1;
	    maind = 1;
	}

	if (tscal != 1.) {
	    grow = 0.;
	    goto L80;
	}

	if (nounit) {

/*           A is non-unit triangular. */

/*           Compute GROW = 1/G(j) and XBND = 1/M(j). */
/*           Initially, M(0) = max{x(i), i=1,...,n}. */

	    grow = 1. / std::max(xbnd,smlnum);
	    xbnd = grow;
	    i__1 = jlast;
	    i__2 = jinc;
	    for (j = jfirst; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L80;
		}

/*              G(j) = max( G(j-1), M(j-1)*( 1 + CNORM(j) ) ) */

		xj = cnorm[j] + 1.;
/* Computing MIN */
		d__1 = grow, d__2 = xbnd / xj;
		grow = std::min(d__1,d__2);

/*              M(j) = M(j-1)*( 1 + CNORM(j) ) / abs(A(j,j)) */

		tjj = (d__1 = ab[maind + j * ab_dim1], abs(d__1));
		if (xj > tjj) {
		    xbnd *= tjj / xj;
		}
/* L60: */
	    }
	    grow = std::min(grow,xbnd);
	} else {

/*           A is unit triangular. */

/*           Compute GROW = 1/G(j), where G(0) = max{x(i), i=1,...,n}. */

/* Computing MIN */
	    d__1 = 1., d__2 = 1. / std::max(xbnd,smlnum);
	    grow = std::min(d__1,d__2);
	    i__2 = jlast;
	    i__1 = jinc;
	    for (j = jfirst; i__1 < 0 ? j >= i__2 : j <= i__2; j += i__1) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L80;
		}

/*              G(j) = ( 1 + CNORM(j) )*G(j-1) */

		xj = cnorm[j] + 1.;
		grow /= xj;
/* L70: */
	    }
	}
L80:
	;
    }

    if (grow * tscal > smlnum) {

/*        Use the Level 2 BLAS solve if the reciprocal of the bound on */
/*        elements of X is not too small. */

	dtbsv_(uplo, trans, diag, n, kd, &ab[ab_offset], ldab, &x[1], &c__1);
    } else {

/*        Use a Level 1 BLAS solve, scaling intermediate results. */

	if (xmax > bignum) {

/*           Scale X so that its components are less than or equal to */
/*           BIGNUM in absolute value. */

	    *scale = bignum / xmax;
	    dscal_(n, scale, &x[1], &c__1);
	    xmax = bignum;
	}

	if (notran) {

/*           Solve A * x = b */

	    i__1 = jlast;
	    i__2 = jinc;
	    for (j = jfirst; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {

/*              Compute x(j) = b(j) / A(j,j), scaling x if necessary. */

		xj = (d__1 = x[j], abs(d__1));
		if (nounit) {
		    tjjs = ab[maind + j * ab_dim1] * tscal;
		} else {
		    tjjs = tscal;
		    if (tscal == 1.) {
			goto L100;
		    }
		}
		tjj = abs(tjjs);
		if (tjj > smlnum) {

/*                    abs(A(j,j)) > SMLNUM: */

		    if (tjj < 1.) {
			if (xj > tjj * bignum) {

/*                          Scale x by 1/b(j). */

			    rec = 1. / xj;
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }
		    x[j] /= tjjs;
		    xj = (d__1 = x[j], abs(d__1));
		} else if (tjj > 0.) {

/*                    0 < abs(A(j,j)) <= SMLNUM: */

		    if (xj > tjj * bignum) {

/*                       Scale x by (1/abs(x(j)))*abs(A(j,j))*BIGNUM */
/*                       to avoid overflow when dividing by A(j,j). */

			rec = tjj * bignum / xj;
			if (cnorm[j] > 1.) {

/*                          Scale by 1/CNORM(j) to avoid overflow when */
/*                          multiplying x(j) times column j. */

			    rec /= cnorm[j];
			}
			dscal_(n, &rec, &x[1], &c__1);
			*scale *= rec;
			xmax *= rec;
		    }
		    x[j] /= tjjs;
		    xj = (d__1 = x[j], abs(d__1));
		} else {

/*                    A(j,j) = 0:  Set x(1:n) = 0, x(j) = 1, and */
/*                    scale = 0, and compute a solution to A*x = 0. */

		    i__3 = *n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			x[i__] = 0.;
/* L90: */
		    }
		    x[j] = 1.;
		    xj = 1.;
		    *scale = 0.;
		    xmax = 0.;
		}
L100:

/*              Scale x if necessary to avoid overflow when adding a */
/*              multiple of column j of A. */

		if (xj > 1.) {
		    rec = 1. / xj;
		    if (cnorm[j] > (bignum - xmax) * rec) {

/*                    Scale x by 1/(2*abs(x(j))). */

			rec *= .5;
			dscal_(n, &rec, &x[1], &c__1);
			*scale *= rec;
		    }
		} else if (xj * cnorm[j] > bignum - xmax) {

/*                 Scale x by 1/2. */

		    dscal_(n, &c_b36, &x[1], &c__1);
		    *scale *= .5;
		}

		if (upper) {
		    if (j > 1) {

/*                    Compute the update */
/*                       x(max(1,j-kd):j-1) := x(max(1,j-kd):j-1) - */
/*                                             x(j)* A(max(1,j-kd):j-1,j) */

/* Computing MIN */
			i__3 = *kd, i__4 = j - 1;
			jlen = std::min(i__3,i__4);
			d__1 = -x[j] * tscal;
			daxpy_(&jlen, &d__1, &ab[*kd + 1 - jlen + j * ab_dim1]
, &c__1, &x[j - jlen], &c__1);
			i__3 = j - 1;
			i__ = idamax_(&i__3, &x[1], &c__1);
			xmax = (d__1 = x[i__], abs(d__1));
		    }
		} else if (j < *n) {

/*                 Compute the update */
/*                    x(j+1:min(j+kd,n)) := x(j+1:min(j+kd,n)) - */
/*                                          x(j) * A(j+1:min(j+kd,n),j) */

/* Computing MIN */
		    i__3 = *kd, i__4 = *n - j;
		    jlen = std::min(i__3,i__4);
		    if (jlen > 0) {
			d__1 = -x[j] * tscal;
			daxpy_(&jlen, &d__1, &ab[j * ab_dim1 + 2], &c__1, &x[
				j + 1], &c__1);
		    }
		    i__3 = *n - j;
		    i__ = j + idamax_(&i__3, &x[j + 1], &c__1);
		    xmax = (d__1 = x[i__], abs(d__1));
		}
/* L110: */
	    }

	} else {

/*           Solve A' * x = b */

	    i__2 = jlast;
	    i__1 = jinc;
	    for (j = jfirst; i__1 < 0 ? j >= i__2 : j <= i__2; j += i__1) {

/*              Compute x(j) = b(j) - sum A(k,j)*x(k). */
/*                                    k<>j */

		xj = (d__1 = x[j], abs(d__1));
		uscal = tscal;
		rec = 1. / std::max(xmax,1.);
		if (cnorm[j] > (bignum - xj) * rec) {

/*                 If x(j) could overflow, scale x by 1/(2*XMAX). */

		    rec *= .5;
		    if (nounit) {
			tjjs = ab[maind + j * ab_dim1] * tscal;
		    } else {
			tjjs = tscal;
		    }
		    tjj = abs(tjjs);
		    if (tjj > 1.) {

/*                       Divide by A(j,j) when scaling x if A(j,j) > 1. */

/* Computing MIN */
			d__1 = 1., d__2 = rec * tjj;
			rec = std::min(d__1,d__2);
			uscal /= tjjs;
		    }
		    if (rec < 1.) {
			dscal_(n, &rec, &x[1], &c__1);
			*scale *= rec;
			xmax *= rec;
		    }
		}

		sumj = 0.;
		if (uscal == 1.) {

/*                 If the scaling needed for A in the dot product is 1, */
/*                 call DDOT to perform the dot product. */

		    if (upper) {
/* Computing MIN */
			i__3 = *kd, i__4 = j - 1;
			jlen = std::min(i__3,i__4);
			sumj = ddot_(&jlen, &ab[*kd + 1 - jlen + j * ab_dim1],
				 &c__1, &x[j - jlen], &c__1);
		    } else {
/* Computing MIN */
			i__3 = *kd, i__4 = *n - j;
			jlen = std::min(i__3,i__4);
			if (jlen > 0) {
			    sumj = ddot_(&jlen, &ab[j * ab_dim1 + 2], &c__1, &
				    x[j + 1], &c__1);
			}
		    }
		} else {

/*                 Otherwise, use in-line code for the dot product. */

		    if (upper) {
/* Computing MIN */
			i__3 = *kd, i__4 = j - 1;
			jlen = std::min(i__3,i__4);
			i__3 = jlen;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    sumj += ab[*kd + i__ - jlen + j * ab_dim1] *
				    uscal * x[j - jlen - 1 + i__];
/* L120: */
			}
		    } else {
/* Computing MIN */
			i__3 = *kd, i__4 = *n - j;
			jlen = std::min(i__3,i__4);
			i__3 = jlen;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    sumj += ab[i__ + 1 + j * ab_dim1] * uscal * x[j +
				    i__];
/* L130: */
			}
		    }
		}

		if (uscal == tscal) {

/*                 Compute x(j) := ( x(j) - sumj ) / A(j,j) if 1/A(j,j) */
/*                 was not used to scale the dotproduct. */

		    x[j] -= sumj;
		    xj = (d__1 = x[j], abs(d__1));
		    if (nounit) {

/*                    Compute x(j) = x(j) / A(j,j), scaling if necessary. */

			tjjs = ab[maind + j * ab_dim1] * tscal;
		    } else {
			tjjs = tscal;
			if (tscal == 1.) {
			    goto L150;
			}
		    }
		    tjj = abs(tjjs);
		    if (tjj > smlnum) {

/*                       abs(A(j,j)) > SMLNUM: */

			if (tjj < 1.) {
			    if (xj > tjj * bignum) {

/*                             Scale X by 1/abs(x(j)). */

				rec = 1. / xj;
				dscal_(n, &rec, &x[1], &c__1);
				*scale *= rec;
				xmax *= rec;
			    }
			}
			x[j] /= tjjs;
		    } else if (tjj > 0.) {

/*                       0 < abs(A(j,j)) <= SMLNUM: */

			if (xj > tjj * bignum) {

/*                          Scale x by (1/abs(x(j)))*abs(A(j,j))*BIGNUM. */

			    rec = tjj * bignum / xj;
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
			x[j] /= tjjs;
		    } else {

/*                       A(j,j) = 0:  Set x(1:n) = 0, x(j) = 1, and */
/*                       scale = 0, and compute a solution to A'*x = 0. */

			i__3 = *n;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    x[i__] = 0.;
/* L140: */
			}
			x[j] = 1.;
			*scale = 0.;
			xmax = 0.;
		    }
L150:
		    ;
		} else {

/*                 Compute x(j) := x(j) / A(j,j) - sumj if the dot */
/*                 product has already been divided by 1/A(j,j). */

		    x[j] = x[j] / tjjs - sumj;
		}
/* Computing MAX */
		d__2 = xmax, d__3 = (d__1 = x[j], abs(d__1));
		xmax = std::max(d__2,d__3);
/* L160: */
	    }
	}
	*scale /= tscal;
    }

/*     Scale the column norms by 1/TSCAL for return. */

    if (tscal != 1.) {
	d__1 = 1. / tscal;
	dscal_(n, &d__1, &cnorm[1], &c__1);
    }

    return 0;

/*     End of DLATBS */

} /* dlatbs_ */

/* Subroutine */ int dlatdf_(integer *ijob, integer *n, double *z__,
	integer *ldz, double *rhs, double *rdsum, double *rdscal,
	integer *ipiv, integer *jpiv)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b23 = 1.;
	static double c_b37 = -1.;

    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer i__, j, k;
    double bm, bp, xm[8], xp[8];
    integer info;
    double temp, work[32];
    double pmone;
    double sminu;
    integer iwork[8];
    double splus;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLATDF uses the LU factorization of the n-by-n matrix Z computed by */
/*  DGETC2 and computes a contribution to the reciprocal Dif-estimate */
/*  by solving Z * x = b for x, and choosing the r.h.s. b such that */
/*  the norm of x is as large as possible. On entry RHS = b holds the */
/*  contribution from earlier solved sub-systems, and on return RHS = x. */

/*  The factorization of Z returned by DGETC2 has the form Z = P*L*U*Q, */
/*  where P and Q are permutation matrices. L is lower triangular with */
/*  unit diagonal elements and U is upper triangular. */

/*  Arguments */
/*  ========= */

/*  IJOB    (input) INTEGER */
/*          IJOB = 2: First compute an approximative null-vector e */
/*              of Z using DGECON, e is normalized and solve for */
/*              Zx = +-e - f with the sign giving the greater value */
/*              of 2-norm(x). About 5 times as expensive as Default. */
/*          IJOB .ne. 2: Local look ahead strategy where all entries of */
/*              the r.h.s. b is choosen as either +1 or -1 (Default). */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix Z. */

/*  Z       (input) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          On entry, the LU part of the factorization of the n-by-n */
/*          matrix Z computed by DGETC2:  Z = P * L * U * Q */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDA >= max(1, N). */

/*  RHS     (input/output) DOUBLE PRECISION array, dimension N. */
/*          On entry, RHS contains contributions from other subsystems. */
/*          On exit, RHS contains the solution of the subsystem with */
/*          entries acoording to the value of IJOB (see above). */

/*  RDSUM   (input/output) DOUBLE PRECISION */
/*          On entry, the sum of squares of computed contributions to */
/*          the Dif-estimate under computation by DTGSYL, where the */
/*          scaling factor RDSCAL (see below) has been factored out. */
/*          On exit, the corresponding sum of squares updated with the */
/*          contributions from the current sub-system. */
/*          If TRANS = 'T' RDSUM is not touched. */
/*          NOTE: RDSUM only makes sense when DTGSY2 is called by STGSYL. */

/*  RDSCAL  (input/output) DOUBLE PRECISION */
/*          On entry, scaling factor used to prevent overflow in RDSUM. */
/*          On exit, RDSCAL is updated w.r.t. the current contributions */
/*          in RDSUM. */
/*          If TRANS = 'T', RDSCAL is not touched. */
/*          NOTE: RDSCAL only makes sense when DTGSY2 is called by */
/*                DTGSYL. */

/*  IPIV    (input) INTEGER array, dimension (N). */
/*          The pivot indices; for 1 <= i <= N, row i of the */
/*          matrix has been interchanged with row IPIV(i). */

/*  JPIV    (input) INTEGER array, dimension (N). */
/*          The pivot indices; for 1 <= j <= N, column j of the */
/*          matrix has been interchanged with column JPIV(j). */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Bo Kagstrom and Peter Poromaa, Department of Computing Science, */
/*     Umea University, S-901 87 Umea, Sweden. */

/*  This routine is a further developed implementation of algorithm */
/*  BSOLVE in [1] using complete pivoting in the LU factorization. */

/*  [1] Bo Kagstrom and Lars Westin, */
/*      Generalized Schur Methods with Condition Estimators for */
/*      Solving the Generalized Sylvester Equation, IEEE Transactions */
/*      on Automatic Control, Vol. 34, No. 7, July 1989, pp 745-751. */

/*  [2] Peter Poromaa, */
/*      On Efficient and Robust Estimators for the Separation */
/*      between two Regular Matrix Pairs with Applications in */
/*      Condition Estimation. Report IMINF-95.05, Departement of */
/*      Computing Science, Umea University, S-901 87 Umea, Sweden, 1995. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --rhs;
    --ipiv;
    --jpiv;

    /* Function Body */
    if (*ijob != 2) {

/*        Apply permutations IPIV to RHS */

	i__1 = *n - 1;
	dlaswp_(&c__1, &rhs[1], ldz, &c__1, &i__1, &ipiv[1], &c__1);

/*        Solve for L-part choosing RHS either to +1 or -1. */

	pmone = -1.;

	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    bp = rhs[j] + 1.;
	    bm = rhs[j] - 1.;
	    splus = 1.;

/*           Look-ahead for L-part RHS(1:N-1) = + or -1, SPLUS and */
/*           SMIN computed more efficiently than in BSOLVE [1]. */

	    i__2 = *n - j;
	    splus += ddot_(&i__2, &z__[j + 1 + j * z_dim1], &c__1, &z__[j + 1
		    + j * z_dim1], &c__1);
	    i__2 = *n - j;
	    sminu = ddot_(&i__2, &z__[j + 1 + j * z_dim1], &c__1, &rhs[j + 1],
		     &c__1);
	    splus *= rhs[j];
	    if (splus > sminu) {
		rhs[j] = bp;
	    } else if (sminu > splus) {
		rhs[j] = bm;
	    } else {

/*              In this case the updating sums are equal and we can */
/*              choose RHS(J) +1 or -1. The first time this happens */
/*              we choose -1, thereafter +1. This is a simple way to */
/*              get good estimates of matrices like Byers well-known */
/*              example (see [1]). (Not done in BSOLVE.) */

		rhs[j] += pmone;
		pmone = 1.;
	    }

/*           Compute the remaining r.h.s. */

	    temp = -rhs[j];
	    i__2 = *n - j;
	    daxpy_(&i__2, &temp, &z__[j + 1 + j * z_dim1], &c__1, &rhs[j + 1],
		     &c__1);

/* L10: */
	}

/*        Solve for U-part, look-ahead for RHS(N) = +-1. This is not done */
/*        in BSOLVE and will hopefully give us a better estimate because */
/*        any ill-conditioning of the original matrix is transfered to U */
/*        and not to L. U(N, N) is an approximation to sigma_min(LU). */

	i__1 = *n - 1;
	dcopy_(&i__1, &rhs[1], &c__1, xp, &c__1);
	xp[*n - 1] = rhs[*n] + 1.;
	rhs[*n] += -1.;
	splus = 0.;
	sminu = 0.;
	for (i__ = *n; i__ >= 1; --i__) {
	    temp = 1. / z__[i__ + i__ * z_dim1];
	    xp[i__ - 1] *= temp;
	    rhs[i__] *= temp;
	    i__1 = *n;
	    for (k = i__ + 1; k <= i__1; ++k) {
		xp[i__ - 1] -= xp[k - 1] * (z__[i__ + k * z_dim1] * temp);
		rhs[i__] -= rhs[k] * (z__[i__ + k * z_dim1] * temp);
/* L20: */
	    }
	    splus += (d__1 = xp[i__ - 1], abs(d__1));
	    sminu += (d__1 = rhs[i__], abs(d__1));
/* L30: */
	}
	if (splus > sminu) {
	    dcopy_(n, xp, &c__1, &rhs[1], &c__1);
	}

/*        Apply the permutations JPIV to the computed solution (RHS) */

	i__1 = *n - 1;
	dlaswp_(&c__1, &rhs[1], ldz, &c__1, &i__1, &jpiv[1], &c_n1);

/*        Compute the sum of squares */

	dlassq_(n, &rhs[1], &c__1, rdscal, rdsum);

    } else {

/*        IJOB = 2, Compute approximate nullvector XM of Z */

	dgecon_("I", n, &z__[z_offset], ldz, &c_b23, &temp, work, iwork, &
		info);
	dcopy_(n, &work[*n], &c__1, xm, &c__1);

/*        Compute RHS */

	i__1 = *n - 1;
	dlaswp_(&c__1, xm, ldz, &c__1, &i__1, &ipiv[1], &c_n1);
	temp = 1. / sqrt(ddot_(n, xm, &c__1, xm, &c__1));
	dscal_(n, &temp, xm, &c__1);
	dcopy_(n, xm, &c__1, xp, &c__1);
	daxpy_(n, &c_b23, &rhs[1], &c__1, xp, &c__1);
	daxpy_(n, &c_b37, xm, &c__1, &rhs[1], &c__1);
	dgesc2_(n, &z__[z_offset], ldz, &rhs[1], &ipiv[1], &jpiv[1], &temp);
	dgesc2_(n, &z__[z_offset], ldz, xp, &ipiv[1], &jpiv[1], &temp);
	if (dasum_(n, xp, &c__1) > dasum_(n, &rhs[1], &c__1)) {
	    dcopy_(n, xp, &c__1, &rhs[1], &c__1);
	}

/*        Compute the sum of squares */

	dlassq_(n, &rhs[1], &c__1, rdscal, rdsum);

    }

    return 0;

/*     End of DLATDF */

} /* dlatdf_ */

/* Subroutine */ int dlatps_(const char *uplo, const char *trans, const char *diag, const char *
	normin, integer *n, double *ap, double *x, double *scale,
	double *cnorm, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b36 = .5;

    /* System generated locals */
    integer i__1, i__2, i__3;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, ip;
    double xj, rec, tjj;
    integer jinc, jlen;
    double xbnd;
    integer imax;
    double tmax, tjjs, xmax, grow, sumj;
    double tscal, uscal;
    integer jlast;
    bool upper;
    double bignum;
    bool notran;
    integer jfirst;
    double smlnum;
    bool nounit;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLATPS solves one of the triangular systems */

/*     A *x = s*b  or  A'*x = s*b */

/*  with scaling to prevent overflow, where A is an upper or lower */
/*  triangular matrix stored in packed form.  Here A' denotes the */
/*  transpose of A, x and b are n-element vectors, and s is a scaling */
/*  factor, usually less than or equal to 1, chosen so that the */
/*  components of x will be less than the overflow threshold.  If the */
/*  unscaled problem will not cause overflow, the Level 2 BLAS routine */
/*  DTPSV is called. If the matrix A is singular (A(j,j) = 0 for some j), */
/*  then s is set to 0 and a non-trivial solution to A*x = 0 is returned. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the operation applied to A. */
/*          = 'N':  Solve A * x = s*b  (No transpose) */
/*          = 'T':  Solve A'* x = s*b  (Transpose) */
/*          = 'C':  Solve A'* x = s*b  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  NORMIN  (input) CHARACTER*1 */
/*          Specifies whether CNORM has been set or not. */
/*          = 'Y':  CNORM contains the column norms on entry */
/*          = 'N':  CNORM is not set on entry.  On exit, the norms will */
/*                  be computed and stored in CNORM. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The upper or lower triangular matrix A, packed columnwise in */
/*          a linear array.  The j-th column of A is stored in the array */
/*          AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */

/*  X       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the right hand side b of the triangular system. */
/*          On exit, X is overwritten by the solution vector x. */

/*  SCALE   (output) DOUBLE PRECISION */
/*          The scaling factor s for the triangular system */
/*             A * x = s*b  or  A'* x = s*b. */
/*          If SCALE = 0, the matrix A is singular or badly scaled, and */
/*          the vector x is an exact or approximate solution to A*x = 0. */

/*  CNORM   (input or output) DOUBLE PRECISION array, dimension (N) */

/*          If NORMIN = 'Y', CNORM is an input argument and CNORM(j) */
/*          contains the norm of the off-diagonal part of the j-th column */
/*          of A.  If TRANS = 'N', CNORM(j) must be greater than or equal */
/*          to the infinity-norm, and if TRANS = 'T' or 'C', CNORM(j) */
/*          must be greater than or equal to the 1-norm. */

/*          If NORMIN = 'N', CNORM is an output argument and CNORM(j) */
/*          returns the 1-norm of the offdiagonal part of the j-th column */
/*          of A. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -k, the k-th argument had an illegal value */

/*  Further Details */
/*  ======= ======= */

/*  A rough bound on x is computed; if that is less than overflow, DTPSV */
/*  is called, otherwise, specific code is used which checks for possible */
/*  overflow or divide-by-zero at every operation. */

/*  A columnwise scheme is used for solving A*x = b.  The basic algorithm */
/*  if A is lower triangular is */

/*       x[1:n] := b[1:n] */
/*       for j = 1, ..., n */
/*            x(j) := x(j) / A(j,j) */
/*            x[j+1:n] := x[j+1:n] - x(j) * A[j+1:n,j] */
/*       end */

/*  Define bounds on the components of x after j iterations of the loop: */
/*     M(j) = bound on x[1:j] */
/*     G(j) = bound on x[j+1:n] */
/*  Initially, let M(0) = 0 and G(0) = max{x(i), i=1,...,n}. */

/*  Then for iteration j+1 we have */
/*     M(j+1) <= G(j) / | A(j+1,j+1) | */
/*     G(j+1) <= G(j) + M(j+1) * | A[j+2:n,j+1] | */
/*            <= G(j) ( 1 + CNORM(j+1) / | A(j+1,j+1) | ) */

/*  where CNORM(j+1) is greater than or equal to the infinity-norm of */
/*  column j+1 of A, not counting the diagonal.  Hence */

/*     G(j) <= G(0) product ( 1 + CNORM(i) / | A(i,i) | ) */
/*                  1<=i<=j */
/*  and */

/*     |x(j)| <= ( G(0) / |A(j,j)| ) product ( 1 + CNORM(i) / |A(i,i)| ) */
/*                                   1<=i< j */

/*  Since |x(j)| <= M(j), we use the Level 2 BLAS routine DTPSV if the */
/*  reciprocal of the largest M(j), j=1,..,n, is larger than */
/*  max(underflow, 1/overflow). */

/*  The bound on x(j) is also used to determine when a step in the */
/*  columnwise method can be performed without fear of overflow.  If */
/*  the computed bound is greater than a large constant, x is scaled to */
/*  prevent overflow, but if the bound overflows, x is set to 0, x(j) to */
/*  1, and scale to 0, and a non-trivial solution to A*x = 0 is found. */

/*  Similarly, a row-wise scheme is used to solve A'*x = b.  The basic */
/*  algorithm for A upper triangular is */

/*       for j = 1, ..., n */
/*            x(j) := ( b(j) - A[1:j-1,j]' * x[1:j-1] ) / A(j,j) */
/*       end */

/*  We simultaneously compute two bounds */
/*       G(j) = bound on ( b(i) - A[1:i-1,i]' * x[1:i-1] ), 1<=i<=j */
/*       M(j) = bound on x(i), 1<=i<=j */

/*  The initial values are G(0) = 0, M(0) = max{b(i), i=1,..,n}, and we */
/*  add the constraint G(j) >= G(j-1) and M(j) >= M(j-1) for j >= 1. */
/*  Then the bound on x(j) is */

/*       M(j) <= M(j-1) * ( 1 + CNORM(j) ) / | A(j,j) | */

/*            <= M(0) * product ( ( 1 + CNORM(i) ) / |A(i,i)| ) */
/*                      1<=i<=j */

/*  and we can safely call DTPSV if 1/M(n) and 1/G(n) are both greater */
/*  than max(underflow, 1/overflow). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --cnorm;
    --x;
    --ap;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    notran = lsame_(trans, "N");
    nounit = lsame_(diag, "N");

/*     Test the input parameters. */

    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T") && !
	    lsame_(trans, "C")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (! lsame_(normin, "Y") && ! lsame_(normin,
	     "N")) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLATPS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Determine machine dependent parameters to control overflow. */

    smlnum = dlamch_("Safe minimum") / dlamch_("Precision");
    bignum = 1. / smlnum;
    *scale = 1.;

    if (lsame_(normin, "N")) {

/*        Compute the 1-norm of each column, not including the diagonal. */

	if (upper) {

/*           A is upper triangular. */

	    ip = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		cnorm[j] = dasum_(&i__2, &ap[ip], &c__1);
		ip += j;
/* L10: */
	    }
	} else {

/*           A is lower triangular. */

	    ip = 1;
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j;
		cnorm[j] = dasum_(&i__2, &ap[ip + 1], &c__1);
		ip = ip + *n - j + 1;
/* L20: */
	    }
	    cnorm[*n] = 0.;
	}
    }

/*     Scale the column norms by TSCAL if the maximum element in CNORM is */
/*     greater than BIGNUM. */

    imax = idamax_(n, &cnorm[1], &c__1);
    tmax = cnorm[imax];
    if (tmax <= bignum) {
	tscal = 1.;
    } else {
	tscal = 1. / (smlnum * tmax);
	dscal_(n, &tscal, &cnorm[1], &c__1);
    }

/*     Compute a bound on the computed solution vector to see if the */
/*     Level 2 BLAS routine DTPSV can be used. */

    j = idamax_(n, &x[1], &c__1);
    xmax = (d__1 = x[j], abs(d__1));
    xbnd = xmax;
    if (notran) {

/*        Compute the growth in A * x = b. */

	if (upper) {
	    jfirst = *n;
	    jlast = 1;
	    jinc = -1;
	} else {
	    jfirst = 1;
	    jlast = *n;
	    jinc = 1;
	}

	if (tscal != 1.) {
	    grow = 0.;
	    goto L50;
	}

	if (nounit) {

/*           A is non-unit triangular. */

/*           Compute GROW = 1/G(j) and XBND = 1/M(j). */
/*           Initially, G(0) = max{x(i), i=1,...,n}. */

	    grow = 1. / std::max(xbnd,smlnum);
	    xbnd = grow;
	    ip = jfirst * (jfirst + 1) / 2;
	    jlen = *n;
	    i__1 = jlast;
	    i__2 = jinc;
	    for (j = jfirst; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L50;
		}

/*              M(j) = G(j-1) / abs(A(j,j)) */

		tjj = (d__1 = ap[ip], abs(d__1));
/* Computing MIN */
		d__1 = xbnd, d__2 = std::min(1.,tjj) * grow;
		xbnd = std::min(d__1,d__2);
		if (tjj + cnorm[j] >= smlnum) {

/*                 G(j) = G(j-1)*( 1 + CNORM(j) / abs(A(j,j)) ) */

		    grow *= tjj / (tjj + cnorm[j]);
		} else {

/*                 G(j) could overflow, set GROW to 0. */

		    grow = 0.;
		}
		ip += jinc * jlen;
		--jlen;
/* L30: */
	    }
	    grow = xbnd;
	} else {

/*           A is unit triangular. */

/*           Compute GROW = 1/G(j), where G(0) = max{x(i), i=1,...,n}. */

/* Computing MIN */
	    d__1 = 1., d__2 = 1. / std::max(xbnd,smlnum);
	    grow = std::min(d__1,d__2);
	    i__2 = jlast;
	    i__1 = jinc;
	    for (j = jfirst; i__1 < 0 ? j >= i__2 : j <= i__2; j += i__1) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L50;
		}

/*              G(j) = G(j-1)*( 1 + CNORM(j) ) */

		grow *= 1. / (cnorm[j] + 1.);
/* L40: */
	    }
	}
L50:

	;
    } else {

/*        Compute the growth in A' * x = b. */

	if (upper) {
	    jfirst = 1;
	    jlast = *n;
	    jinc = 1;
	} else {
	    jfirst = *n;
	    jlast = 1;
	    jinc = -1;
	}

	if (tscal != 1.) {
	    grow = 0.;
	    goto L80;
	}

	if (nounit) {

/*           A is non-unit triangular. */

/*           Compute GROW = 1/G(j) and XBND = 1/M(j). */
/*           Initially, M(0) = max{x(i), i=1,...,n}. */

	    grow = 1. / std::max(xbnd,smlnum);
	    xbnd = grow;
	    ip = jfirst * (jfirst + 1) / 2;
	    jlen = 1;
	    i__1 = jlast;
	    i__2 = jinc;
	    for (j = jfirst; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L80;
		}

/*              G(j) = max( G(j-1), M(j-1)*( 1 + CNORM(j) ) ) */

		xj = cnorm[j] + 1.;
/* Computing MIN */
		d__1 = grow, d__2 = xbnd / xj;
		grow = std::min(d__1,d__2);

/*              M(j) = M(j-1)*( 1 + CNORM(j) ) / abs(A(j,j)) */

		tjj = (d__1 = ap[ip], abs(d__1));
		if (xj > tjj) {
		    xbnd *= tjj / xj;
		}
		++jlen;
		ip += jinc * jlen;
/* L60: */
	    }
	    grow = std::min(grow,xbnd);
	} else {

/*           A is unit triangular. */

/*           Compute GROW = 1/G(j), where G(0) = max{x(i), i=1,...,n}. */

/* Computing MIN */
	    d__1 = 1., d__2 = 1. / std::max(xbnd,smlnum);
	    grow = std::min(d__1,d__2);
	    i__2 = jlast;
	    i__1 = jinc;
	    for (j = jfirst; i__1 < 0 ? j >= i__2 : j <= i__2; j += i__1) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L80;
		}

/*              G(j) = ( 1 + CNORM(j) )*G(j-1) */

		xj = cnorm[j] + 1.;
		grow /= xj;
/* L70: */
	    }
	}
L80:
	;
    }

    if (grow * tscal > smlnum) {

/*        Use the Level 2 BLAS solve if the reciprocal of the bound on */
/*        elements of X is not too small. */

	dtpsv_(uplo, trans, diag, n, &ap[1], &x[1], &c__1);
    } else {

/*        Use a Level 1 BLAS solve, scaling intermediate results. */

	if (xmax > bignum) {

/*           Scale X so that its components are less than or equal to */
/*           BIGNUM in absolute value. */

	    *scale = bignum / xmax;
	    dscal_(n, scale, &x[1], &c__1);
	    xmax = bignum;
	}

	if (notran) {

/*           Solve A * x = b */

	    ip = jfirst * (jfirst + 1) / 2;
	    i__1 = jlast;
	    i__2 = jinc;
	    for (j = jfirst; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {

/*              Compute x(j) = b(j) / A(j,j), scaling x if necessary. */

		xj = (d__1 = x[j], abs(d__1));
		if (nounit) {
		    tjjs = ap[ip] * tscal;
		} else {
		    tjjs = tscal;
		    if (tscal == 1.) {
			goto L100;
		    }
		}
		tjj = abs(tjjs);
		if (tjj > smlnum) {

/*                    abs(A(j,j)) > SMLNUM: */

		    if (tjj < 1.) {
			if (xj > tjj * bignum) {

/*                          Scale x by 1/b(j). */

			    rec = 1. / xj;
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }
		    x[j] /= tjjs;
		    xj = (d__1 = x[j], abs(d__1));
		} else if (tjj > 0.) {

/*                    0 < abs(A(j,j)) <= SMLNUM: */

		    if (xj > tjj * bignum) {

/*                       Scale x by (1/abs(x(j)))*abs(A(j,j))*BIGNUM */
/*                       to avoid overflow when dividing by A(j,j). */

			rec = tjj * bignum / xj;
			if (cnorm[j] > 1.) {

/*                          Scale by 1/CNORM(j) to avoid overflow when */
/*                          multiplying x(j) times column j. */

			    rec /= cnorm[j];
			}
			dscal_(n, &rec, &x[1], &c__1);
			*scale *= rec;
			xmax *= rec;
		    }
		    x[j] /= tjjs;
		    xj = (d__1 = x[j], abs(d__1));
		} else {

/*                    A(j,j) = 0:  Set x(1:n) = 0, x(j) = 1, and */
/*                    scale = 0, and compute a solution to A*x = 0. */

		    i__3 = *n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			x[i__] = 0.;
/* L90: */
		    }
		    x[j] = 1.;
		    xj = 1.;
		    *scale = 0.;
		    xmax = 0.;
		}
L100:

/*              Scale x if necessary to avoid overflow when adding a */
/*              multiple of column j of A. */

		if (xj > 1.) {
		    rec = 1. / xj;
		    if (cnorm[j] > (bignum - xmax) * rec) {

/*                    Scale x by 1/(2*abs(x(j))). */

			rec *= .5;
			dscal_(n, &rec, &x[1], &c__1);
			*scale *= rec;
		    }
		} else if (xj * cnorm[j] > bignum - xmax) {

/*                 Scale x by 1/2. */

		    dscal_(n, &c_b36, &x[1], &c__1);
		    *scale *= .5;
		}

		if (upper) {
		    if (j > 1) {

/*                    Compute the update */
/*                       x(1:j-1) := x(1:j-1) - x(j) * A(1:j-1,j) */

			i__3 = j - 1;
			d__1 = -x[j] * tscal;
			daxpy_(&i__3, &d__1, &ap[ip - j + 1], &c__1, &x[1], &
				c__1);
			i__3 = j - 1;
			i__ = idamax_(&i__3, &x[1], &c__1);
			xmax = (d__1 = x[i__], abs(d__1));
		    }
		    ip -= j;
		} else {
		    if (j < *n) {

/*                    Compute the update */
/*                       x(j+1:n) := x(j+1:n) - x(j) * A(j+1:n,j) */

			i__3 = *n - j;
			d__1 = -x[j] * tscal;
			daxpy_(&i__3, &d__1, &ap[ip + 1], &c__1, &x[j + 1], &
				c__1);
			i__3 = *n - j;
			i__ = j + idamax_(&i__3, &x[j + 1], &c__1);
			xmax = (d__1 = x[i__], abs(d__1));
		    }
		    ip = ip + *n - j + 1;
		}
/* L110: */
	    }

	} else {

/*           Solve A' * x = b */

	    ip = jfirst * (jfirst + 1) / 2;
	    jlen = 1;
	    i__2 = jlast;
	    i__1 = jinc;
	    for (j = jfirst; i__1 < 0 ? j >= i__2 : j <= i__2; j += i__1) {

/*              Compute x(j) = b(j) - sum A(k,j)*x(k). */
/*                                    k<>j */

		xj = (d__1 = x[j], abs(d__1));
		uscal = tscal;
		rec = 1. / std::max(xmax,1.);
		if (cnorm[j] > (bignum - xj) * rec) {

/*                 If x(j) could overflow, scale x by 1/(2*XMAX). */

		    rec *= .5;
		    if (nounit) {
			tjjs = ap[ip] * tscal;
		    } else {
			tjjs = tscal;
		    }
		    tjj = abs(tjjs);
		    if (tjj > 1.) {

/*                       Divide by A(j,j) when scaling x if A(j,j) > 1. */

/* Computing MIN */
			d__1 = 1., d__2 = rec * tjj;
			rec = std::min(d__1,d__2);
			uscal /= tjjs;
		    }
		    if (rec < 1.) {
			dscal_(n, &rec, &x[1], &c__1);
			*scale *= rec;
			xmax *= rec;
		    }
		}

		sumj = 0.;
		if (uscal == 1.) {

/*                 If the scaling needed for A in the dot product is 1, */
/*                 call DDOT to perform the dot product. */

		    if (upper) {
			i__3 = j - 1;
			sumj = ddot_(&i__3, &ap[ip - j + 1], &c__1, &x[1], &
				c__1);
		    } else if (j < *n) {
			i__3 = *n - j;
			sumj = ddot_(&i__3, &ap[ip + 1], &c__1, &x[j + 1], &
				c__1);
		    }
		} else {

/*                 Otherwise, use in-line code for the dot product. */

		    if (upper) {
			i__3 = j - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    sumj += ap[ip - j + i__] * uscal * x[i__];
/* L120: */
			}
		    } else if (j < *n) {
			i__3 = *n - j;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    sumj += ap[ip + i__] * uscal * x[j + i__];
/* L130: */
			}
		    }
		}

		if (uscal == tscal) {

/*                 Compute x(j) := ( x(j) - sumj ) / A(j,j) if 1/A(j,j) */
/*                 was not used to scale the dotproduct. */

		    x[j] -= sumj;
		    xj = (d__1 = x[j], abs(d__1));
		    if (nounit) {

/*                    Compute x(j) = x(j) / A(j,j), scaling if necessary. */

			tjjs = ap[ip] * tscal;
		    } else {
			tjjs = tscal;
			if (tscal == 1.) {
			    goto L150;
			}
		    }
		    tjj = abs(tjjs);
		    if (tjj > smlnum) {

/*                       abs(A(j,j)) > SMLNUM: */

			if (tjj < 1.) {
			    if (xj > tjj * bignum) {

/*                             Scale X by 1/abs(x(j)). */

				rec = 1. / xj;
				dscal_(n, &rec, &x[1], &c__1);
				*scale *= rec;
				xmax *= rec;
			    }
			}
			x[j] /= tjjs;
		    } else if (tjj > 0.) {

/*                       0 < abs(A(j,j)) <= SMLNUM: */

			if (xj > tjj * bignum) {

/*                          Scale x by (1/abs(x(j)))*abs(A(j,j))*BIGNUM. */

			    rec = tjj * bignum / xj;
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
			x[j] /= tjjs;
		    } else {

/*                       A(j,j) = 0:  Set x(1:n) = 0, x(j) = 1, and */
/*                       scale = 0, and compute a solution to A'*x = 0. */

			i__3 = *n;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    x[i__] = 0.;
/* L140: */
			}
			x[j] = 1.;
			*scale = 0.;
			xmax = 0.;
		    }
L150:
		    ;
		} else {

/*                 Compute x(j) := x(j) / A(j,j)  - sumj if the dot */
/*                 product has already been divided by 1/A(j,j). */

		    x[j] = x[j] / tjjs - sumj;
		}
/* Computing MAX */
		d__2 = xmax, d__3 = (d__1 = x[j], abs(d__1));
		xmax = std::max(d__2,d__3);
		++jlen;
		ip += jinc * jlen;
/* L160: */
	    }
	}
	*scale /= tscal;
    }

/*     Scale the column norms by 1/TSCAL for return. */

    if (tscal != 1.) {
	d__1 = 1. / tscal;
	dscal_(n, &d__1, &cnorm[1], &c__1);
    }

    return 0;

/*     End of DLATPS */

} /* dlatps_ */

/* Subroutine */ int dlatrd_(const char *uplo, integer *n, integer *nb, double *
	a, integer *lda, double *e, double *tau, double *w,
	integer *ldw)
{
	/* Table of constant values */
	static double c_b5 = -1.;
	static double c_b6 = 1.;
	static integer c__1 = 1;
	static double c_b16 = 0.;

    /* System generated locals */
    integer a_dim1, a_offset, w_dim1, w_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, iw;
    double alpha;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLATRD reduces NB rows and columns of a real symmetric matrix A to */
/*  symmetric tridiagonal form by an orthogonal similarity */
/*  transformation Q' * A * Q, and returns the matrices V and W which are */
/*  needed to apply the transformation to the unreduced part of A. */

/*  If UPLO = 'U', DLATRD reduces the last NB rows and columns of a */
/*  matrix, of which the upper triangle is supplied; */
/*  if UPLO = 'L', DLATRD reduces the first NB rows and columns of a */
/*  matrix, of which the lower triangle is supplied. */

/*  This is an auxiliary routine called by DSYTRD. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored: */
/*          = 'U': Upper triangular */
/*          = 'L': Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A. */

/*  NB      (input) INTEGER */
/*          The number of rows and columns to be reduced. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          n-by-n upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading n-by-n lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */
/*          On exit: */
/*          if UPLO = 'U', the last NB columns have been reduced to */
/*            tridiagonal form, with the diagonal elements overwriting */
/*            the diagonal elements of A; the elements above the diagonal */
/*            with the array TAU, represent the orthogonal matrix Q as a */
/*            product of elementary reflectors; */
/*          if UPLO = 'L', the first NB columns have been reduced to */
/*            tridiagonal form, with the diagonal elements overwriting */
/*            the diagonal elements of A; the elements below the diagonal */
/*            with the array TAU, represent the  orthogonal matrix Q as a */
/*            product of elementary reflectors. */
/*          See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= (1,N). */

/*  E       (output) DOUBLE PRECISION array, dimension (N-1) */
/*          If UPLO = 'U', E(n-nb:n-1) contains the superdiagonal */
/*          elements of the last NB columns of the reduced matrix; */
/*          if UPLO = 'L', E(1:nb) contains the subdiagonal elements of */
/*          the first NB columns of the reduced matrix. */

/*  TAU     (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The scalar factors of the elementary reflectors, stored in */
/*          TAU(n-nb:n-1) if UPLO = 'U', and in TAU(1:nb) if UPLO = 'L'. */
/*          See Further Details. */

/*  W       (output) DOUBLE PRECISION array, dimension (LDW,NB) */
/*          The n-by-nb matrix W required to update the unreduced part */
/*          of A. */

/*  LDW     (input) INTEGER */
/*          The leading dimension of the array W. LDW >= max(1,N). */

/*  Further Details */
/*  =============== */

/*  If UPLO = 'U', the matrix Q is represented as a product of elementary */
/*  reflectors */

/*     Q = H(n) H(n-1) . . . H(n-nb+1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(i:n) = 0 and v(i-1) = 1; v(1:i-1) is stored on exit in A(1:i-1,i), */
/*  and tau in TAU(i-1). */

/*  If UPLO = 'L', the matrix Q is represented as a product of elementary */
/*  reflectors */

/*     Q = H(1) H(2) . . . H(nb). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i) = 0 and v(i+1) = 1; v(i+1:n) is stored on exit in A(i+1:n,i), */
/*  and tau in TAU(i). */

/*  The elements of the vectors v together form the n-by-nb matrix V */
/*  which is needed, with W, to apply the transformation to the unreduced */
/*  part of the matrix, using a symmetric rank-2k update of the form: */
/*  A := A - V*W' - W*V'. */

/*  The contents of A on exit are illustrated by the following examples */
/*  with n = 5 and nb = 2: */

/*  if UPLO = 'U':                       if UPLO = 'L': */

/*    (  a   a   a   v4  v5 )              (  d                  ) */
/*    (      a   a   v4  v5 )              (  1   d              ) */
/*    (          a   1   v5 )              (  v1  1   a          ) */
/*    (              d   1  )              (  v1  v2  a   a      ) */
/*    (                  d  )              (  v1  v2  a   a   a  ) */

/*  where d denotes a diagonal element of the reduced matrix, a denotes */
/*  an element of the original matrix that is unchanged, and vi denotes */
/*  an element of the vector defining H(i). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --e;
    --tau;
    w_dim1 = *ldw;
    w_offset = 1 + w_dim1;
    w -= w_offset;

    /* Function Body */
    if (*n <= 0) {
	return 0;
    }

    if (lsame_(uplo, "U")) {

/*        Reduce last NB columns of upper triangle */

	i__1 = *n - *nb + 1;
	for (i__ = *n; i__ >= i__1; --i__) {
	    iw = i__ - *n + *nb;
	    if (i__ < *n) {

/*              Update A(1:i,i) */

		i__2 = *n - i__;
		dgemv_("No transpose", &i__, &i__2, &c_b5, &a[(i__ + 1) *
			a_dim1 + 1], lda, &w[i__ + (iw + 1) * w_dim1], ldw, &
			c_b6, &a[i__ * a_dim1 + 1], &c__1);
		i__2 = *n - i__;
		dgemv_("No transpose", &i__, &i__2, &c_b5, &w[(iw + 1) *
			w_dim1 + 1], ldw, &a[i__ + (i__ + 1) * a_dim1], lda, &
			c_b6, &a[i__ * a_dim1 + 1], &c__1);
	    }
	    if (i__ > 1) {

/*              Generate elementary reflector H(i) to annihilate */
/*              A(1:i-2,i) */

		i__2 = i__ - 1;
		dlarfg_(&i__2, &a[i__ - 1 + i__ * a_dim1], &a[i__ * a_dim1 +
			1], &c__1, &tau[i__ - 1]);
		e[i__ - 1] = a[i__ - 1 + i__ * a_dim1];
		a[i__ - 1 + i__ * a_dim1] = 1.;

/*              Compute W(1:i-1,i) */

		i__2 = i__ - 1;
		dsymv_("Upper", &i__2, &c_b6, &a[a_offset], lda, &a[i__ *
			a_dim1 + 1], &c__1, &c_b16, &w[iw * w_dim1 + 1], &
			c__1);
		if (i__ < *n) {
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    dgemv_("Transpose", &i__2, &i__3, &c_b6, &w[(iw + 1) *
			    w_dim1 + 1], ldw, &a[i__ * a_dim1 + 1], &c__1, &
			    c_b16, &w[i__ + 1 + iw * w_dim1], &c__1);
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    dgemv_("No transpose", &i__2, &i__3, &c_b5, &a[(i__ + 1) *
			     a_dim1 + 1], lda, &w[i__ + 1 + iw * w_dim1], &
			    c__1, &c_b6, &w[iw * w_dim1 + 1], &c__1);
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    dgemv_("Transpose", &i__2, &i__3, &c_b6, &a[(i__ + 1) *
			    a_dim1 + 1], lda, &a[i__ * a_dim1 + 1], &c__1, &
			    c_b16, &w[i__ + 1 + iw * w_dim1], &c__1);
		    i__2 = i__ - 1;
		    i__3 = *n - i__;
		    dgemv_("No transpose", &i__2, &i__3, &c_b5, &w[(iw + 1) *
			    w_dim1 + 1], ldw, &w[i__ + 1 + iw * w_dim1], &
			    c__1, &c_b6, &w[iw * w_dim1 + 1], &c__1);
		}
		i__2 = i__ - 1;
		dscal_(&i__2, &tau[i__ - 1], &w[iw * w_dim1 + 1], &c__1);
		i__2 = i__ - 1;
		alpha = tau[i__ - 1] * -.5 * ddot_(&i__2, &w[iw * w_dim1 + 1],
			 &c__1, &a[i__ * a_dim1 + 1], &c__1);
		i__2 = i__ - 1;
		daxpy_(&i__2, &alpha, &a[i__ * a_dim1 + 1], &c__1, &w[iw *
			w_dim1 + 1], &c__1);
	    }

/* L10: */
	}
    } else {

/*        Reduce first NB columns of lower triangle */

	i__1 = *nb;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Update A(i:n,i) */

	    i__2 = *n - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b5, &a[i__ + a_dim1], lda,
		     &w[i__ + w_dim1], ldw, &c_b6, &a[i__ + i__ * a_dim1], &
		    c__1);
	    i__2 = *n - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b5, &w[i__ + w_dim1], ldw,
		     &a[i__ + a_dim1], lda, &c_b6, &a[i__ + i__ * a_dim1], &
		    c__1);
	    if (i__ < *n) {

/*              Generate elementary reflector H(i) to annihilate */
/*              A(i+2:n,i) */

		i__2 = *n - i__;
/* Computing MIN */
		i__3 = i__ + 2;
		dlarfg_(&i__2, &a[i__ + 1 + i__ * a_dim1], &a[std::min(i__3, *n)+
			i__ * a_dim1], &c__1, &tau[i__]);
		e[i__] = a[i__ + 1 + i__ * a_dim1];
		a[i__ + 1 + i__ * a_dim1] = 1.;

/*              Compute W(i+1:n,i) */

		i__2 = *n - i__;
		dsymv_("Lower", &i__2, &c_b6, &a[i__ + 1 + (i__ + 1) * a_dim1]
, lda, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b16, &w[
			i__ + 1 + i__ * w_dim1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b6, &w[i__ + 1 + w_dim1],
			 ldw, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b16, &w[
			i__ * w_dim1 + 1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b5, &a[i__ + 1 +
			a_dim1], lda, &w[i__ * w_dim1 + 1], &c__1, &c_b6, &w[
			i__ + 1 + i__ * w_dim1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b6, &a[i__ + 1 + a_dim1],
			 lda, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b16, &w[
			i__ * w_dim1 + 1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b5, &w[i__ + 1 +
			w_dim1], ldw, &w[i__ * w_dim1 + 1], &c__1, &c_b6, &w[
			i__ + 1 + i__ * w_dim1], &c__1);
		i__2 = *n - i__;
		dscal_(&i__2, &tau[i__], &w[i__ + 1 + i__ * w_dim1], &c__1);
		i__2 = *n - i__;
		alpha = tau[i__] * -.5 * ddot_(&i__2, &w[i__ + 1 + i__ *
			w_dim1], &c__1, &a[i__ + 1 + i__ * a_dim1], &c__1);
		i__2 = *n - i__;
		daxpy_(&i__2, &alpha, &a[i__ + 1 + i__ * a_dim1], &c__1, &w[
			i__ + 1 + i__ * w_dim1], &c__1);
	    }

/* L20: */
	}
    }

    return 0;

/*     End of DLATRD */

} /* dlatrd_ */

/* Subroutine */ int dlatrs_(const char *uplo, const char *trans, const char *diag, const char *
	normin, integer *n, double *a, integer *lda, double *x,
	double *scale, double *cnorm, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b36 = .5;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j;
    double xj, rec, tjj;
    integer jinc;
    double xbnd;
    integer imax;
    double tmax, tjjs, xmax, grow, sumj;
    double tscal, uscal;
    integer jlast;
    bool upper;
    double bignum;
    bool notran;
    integer jfirst;
    double smlnum;
    bool nounit;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLATRS solves one of the triangular systems */

/*     A *x = s*b  or  A'*x = s*b */

/*  with scaling to prevent overflow.  Here A is an upper or lower */
/*  triangular matrix, A' denotes the transpose of A, x and b are */
/*  n-element vectors, and s is a scaling factor, usually less than */
/*  or equal to 1, chosen so that the components of x will be less than */
/*  the overflow threshold.  If the unscaled problem will not cause */
/*  overflow, the Level 2 BLAS routine DTRSV is called.  If the matrix A */
/*  is singular (A(j,j) = 0 for some j), then s is set to 0 and a */
/*  non-trivial solution to A*x = 0 is returned. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the operation applied to A. */
/*          = 'N':  Solve A * x = s*b  (No transpose) */
/*          = 'T':  Solve A'* x = s*b  (Transpose) */
/*          = 'C':  Solve A'* x = s*b  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  NORMIN  (input) CHARACTER*1 */
/*          Specifies whether CNORM has been set or not. */
/*          = 'Y':  CNORM contains the column norms on entry */
/*          = 'N':  CNORM is not set on entry.  On exit, the norms will */
/*                  be computed and stored in CNORM. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The triangular matrix A.  If UPLO = 'U', the leading n by n */
/*          upper triangular part of the array A contains the upper */
/*          triangular matrix, and the strictly lower triangular part of */
/*          A is not referenced.  If UPLO = 'L', the leading n by n lower */
/*          triangular part of the array A contains the lower triangular */
/*          matrix, and the strictly upper triangular part of A is not */
/*          referenced.  If DIAG = 'U', the diagonal elements of A are */
/*          also not referenced and are assumed to be 1. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max (1,N). */

/*  X       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the right hand side b of the triangular system. */
/*          On exit, X is overwritten by the solution vector x. */

/*  SCALE   (output) DOUBLE PRECISION */
/*          The scaling factor s for the triangular system */
/*             A * x = s*b  or  A'* x = s*b. */
/*          If SCALE = 0, the matrix A is singular or badly scaled, and */
/*          the vector x is an exact or approximate solution to A*x = 0. */

/*  CNORM   (input or output) DOUBLE PRECISION array, dimension (N) */

/*          If NORMIN = 'Y', CNORM is an input argument and CNORM(j) */
/*          contains the norm of the off-diagonal part of the j-th column */
/*          of A.  If TRANS = 'N', CNORM(j) must be greater than or equal */
/*          to the infinity-norm, and if TRANS = 'T' or 'C', CNORM(j) */
/*          must be greater than or equal to the 1-norm. */

/*          If NORMIN = 'N', CNORM is an output argument and CNORM(j) */
/*          returns the 1-norm of the offdiagonal part of the j-th column */
/*          of A. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -k, the k-th argument had an illegal value */

/*  Further Details */
/*  ======= ======= */

/*  A rough bound on x is computed; if that is less than overflow, DTRSV */
/*  is called, otherwise, specific code is used which checks for possible */
/*  overflow or divide-by-zero at every operation. */

/*  A columnwise scheme is used for solving A*x = b.  The basic algorithm */
/*  if A is lower triangular is */

/*       x[1:n] := b[1:n] */
/*       for j = 1, ..., n */
/*            x(j) := x(j) / A(j,j) */
/*            x[j+1:n] := x[j+1:n] - x(j) * A[j+1:n,j] */
/*       end */

/*  Define bounds on the components of x after j iterations of the loop: */
/*     M(j) = bound on x[1:j] */
/*     G(j) = bound on x[j+1:n] */
/*  Initially, let M(0) = 0 and G(0) = max{x(i), i=1,...,n}. */

/*  Then for iteration j+1 we have */
/*     M(j+1) <= G(j) / | A(j+1,j+1) | */
/*     G(j+1) <= G(j) + M(j+1) * | A[j+2:n,j+1] | */
/*            <= G(j) ( 1 + CNORM(j+1) / | A(j+1,j+1) | ) */

/*  where CNORM(j+1) is greater than or equal to the infinity-norm of */
/*  column j+1 of A, not counting the diagonal.  Hence */

/*     G(j) <= G(0) product ( 1 + CNORM(i) / | A(i,i) | ) */
/*                  1<=i<=j */
/*  and */

/*     |x(j)| <= ( G(0) / |A(j,j)| ) product ( 1 + CNORM(i) / |A(i,i)| ) */
/*                                   1<=i< j */

/*  Since |x(j)| <= M(j), we use the Level 2 BLAS routine DTRSV if the */
/*  reciprocal of the largest M(j), j=1,..,n, is larger than */
/*  max(underflow, 1/overflow). */

/*  The bound on x(j) is also used to determine when a step in the */
/*  columnwise method can be performed without fear of overflow.  If */
/*  the computed bound is greater than a large constant, x is scaled to */
/*  prevent overflow, but if the bound overflows, x is set to 0, x(j) to */
/*  1, and scale to 0, and a non-trivial solution to A*x = 0 is found. */

/*  Similarly, a row-wise scheme is used to solve A'*x = b.  The basic */
/*  algorithm for A upper triangular is */

/*       for j = 1, ..., n */
/*            x(j) := ( b(j) - A[1:j-1,j]' * x[1:j-1] ) / A(j,j) */
/*       end */

/*  We simultaneously compute two bounds */
/*       G(j) = bound on ( b(i) - A[1:i-1,i]' * x[1:i-1] ), 1<=i<=j */
/*       M(j) = bound on x(i), 1<=i<=j */

/*  The initial values are G(0) = 0, M(0) = max{b(i), i=1,..,n}, and we */
/*  add the constraint G(j) >= G(j-1) and M(j) >= M(j-1) for j >= 1. */
/*  Then the bound on x(j) is */

/*       M(j) <= M(j-1) * ( 1 + CNORM(j) ) / | A(j,j) | */

/*            <= M(0) * product ( ( 1 + CNORM(i) ) / |A(i,i)| ) */
/*                      1<=i<=j */

/*  and we can safely call DTRSV if 1/M(n) and 1/G(n) are both greater */
/*  than max(underflow, 1/overflow). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --x;
    --cnorm;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    notran = lsame_(trans, "N");
    nounit = lsame_(diag, "N");

/*     Test the input parameters. */

    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T") && !
	    lsame_(trans, "C")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (! lsame_(normin, "Y") && ! lsame_(normin,
	     "N")) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLATRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Determine machine dependent parameters to control overflow. */

    smlnum = dlamch_("Safe minimum") / dlamch_("Precision");
    bignum = 1. / smlnum;
    *scale = 1.;

    if (lsame_(normin, "N")) {

/*        Compute the 1-norm of each column, not including the diagonal. */

	if (upper) {

/*           A is upper triangular. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		cnorm[j] = dasum_(&i__2, &a[j * a_dim1 + 1], &c__1);
/* L10: */
	    }
	} else {

/*           A is lower triangular. */

	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j;
		cnorm[j] = dasum_(&i__2, &a[j + 1 + j * a_dim1], &c__1);
/* L20: */
	    }
	    cnorm[*n] = 0.;
	}
    }

/*     Scale the column norms by TSCAL if the maximum element in CNORM is */
/*     greater than BIGNUM. */

    imax = idamax_(n, &cnorm[1], &c__1);
    tmax = cnorm[imax];
    if (tmax <= bignum) {
	tscal = 1.;
    } else {
	tscal = 1. / (smlnum * tmax);
	dscal_(n, &tscal, &cnorm[1], &c__1);
    }

/*     Compute a bound on the computed solution vector to see if the */
/*     Level 2 BLAS routine DTRSV can be used. */

    j = idamax_(n, &x[1], &c__1);
    xmax = (d__1 = x[j], abs(d__1));
    xbnd = xmax;
    if (notran) {

/*        Compute the growth in A * x = b. */

	if (upper) {
	    jfirst = *n;
	    jlast = 1;
	    jinc = -1;
	} else {
	    jfirst = 1;
	    jlast = *n;
	    jinc = 1;
	}

	if (tscal != 1.) {
	    grow = 0.;
	    goto L50;
	}

	if (nounit) {

/*           A is non-unit triangular. */

/*           Compute GROW = 1/G(j) and XBND = 1/M(j). */
/*           Initially, G(0) = max{x(i), i=1,...,n}. */

	    grow = 1. / std::max(xbnd,smlnum);
	    xbnd = grow;
	    i__1 = jlast;
	    i__2 = jinc;
	    for (j = jfirst; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L50;
		}

/*              M(j) = G(j-1) / abs(A(j,j)) */

		tjj = (d__1 = a[j + j * a_dim1], abs(d__1));
/* Computing MIN */
		d__1 = xbnd, d__2 = std::min(1.,tjj) * grow;
		xbnd = std::min(d__1,d__2);
		if (tjj + cnorm[j] >= smlnum) {

/*                 G(j) = G(j-1)*( 1 + CNORM(j) / abs(A(j,j)) ) */

		    grow *= tjj / (tjj + cnorm[j]);
		} else {

/*                 G(j) could overflow, set GROW to 0. */

		    grow = 0.;
		}
/* L30: */
	    }
	    grow = xbnd;
	} else {

/*           A is unit triangular. */

/*           Compute GROW = 1/G(j), where G(0) = max{x(i), i=1,...,n}. */

/* Computing MIN */
	    d__1 = 1., d__2 = 1. / std::max(xbnd,smlnum);
	    grow = std::min(d__1,d__2);
	    i__2 = jlast;
	    i__1 = jinc;
	    for (j = jfirst; i__1 < 0 ? j >= i__2 : j <= i__2; j += i__1) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L50;
		}

/*              G(j) = G(j-1)*( 1 + CNORM(j) ) */

		grow *= 1. / (cnorm[j] + 1.);
/* L40: */
	    }
	}
L50:

	;
    } else {

/*        Compute the growth in A' * x = b. */

	if (upper) {
	    jfirst = 1;
	    jlast = *n;
	    jinc = 1;
	} else {
	    jfirst = *n;
	    jlast = 1;
	    jinc = -1;
	}

	if (tscal != 1.) {
	    grow = 0.;
	    goto L80;
	}

	if (nounit) {

/*           A is non-unit triangular. */

/*           Compute GROW = 1/G(j) and XBND = 1/M(j). */
/*           Initially, M(0) = max{x(i), i=1,...,n}. */

	    grow = 1. / std::max(xbnd,smlnum);
	    xbnd = grow;
	    i__1 = jlast;
	    i__2 = jinc;
	    for (j = jfirst; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L80;
		}

/*              G(j) = max( G(j-1), M(j-1)*( 1 + CNORM(j) ) ) */

		xj = cnorm[j] + 1.;
/* Computing MIN */
		d__1 = grow, d__2 = xbnd / xj;
		grow = std::min(d__1,d__2);

/*              M(j) = M(j-1)*( 1 + CNORM(j) ) / abs(A(j,j)) */

		tjj = (d__1 = a[j + j * a_dim1], abs(d__1));
		if (xj > tjj) {
		    xbnd *= tjj / xj;
		}
/* L60: */
	    }
	    grow = std::min(grow,xbnd);
	} else {

/*           A is unit triangular. */

/*           Compute GROW = 1/G(j), where G(0) = max{x(i), i=1,...,n}. */

/* Computing MIN */
	    d__1 = 1., d__2 = 1. / std::max(xbnd,smlnum);
	    grow = std::min(d__1,d__2);
	    i__2 = jlast;
	    i__1 = jinc;
	    for (j = jfirst; i__1 < 0 ? j >= i__2 : j <= i__2; j += i__1) {

/*              Exit the loop if the growth factor is too small. */

		if (grow <= smlnum) {
		    goto L80;
		}

/*              G(j) = ( 1 + CNORM(j) )*G(j-1) */

		xj = cnorm[j] + 1.;
		grow /= xj;
/* L70: */
	    }
	}
L80:
	;
    }

    if (grow * tscal > smlnum) {

/*        Use the Level 2 BLAS solve if the reciprocal of the bound on */
/*        elements of X is not too small. */

	dtrsv_(uplo, trans, diag, n, &a[a_offset], lda, &x[1], &c__1);
    } else {

/*        Use a Level 1 BLAS solve, scaling intermediate results. */

	if (xmax > bignum) {

/*           Scale X so that its components are less than or equal to */
/*           BIGNUM in absolute value. */

	    *scale = bignum / xmax;
	    dscal_(n, scale, &x[1], &c__1);
	    xmax = bignum;
	}

	if (notran) {

/*           Solve A * x = b */

	    i__1 = jlast;
	    i__2 = jinc;
	    for (j = jfirst; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {

/*              Compute x(j) = b(j) / A(j,j), scaling x if necessary. */

		xj = (d__1 = x[j], abs(d__1));
		if (nounit) {
		    tjjs = a[j + j * a_dim1] * tscal;
		} else {
		    tjjs = tscal;
		    if (tscal == 1.) {
			goto L100;
		    }
		}
		tjj = abs(tjjs);
		if (tjj > smlnum) {

/*                    abs(A(j,j)) > SMLNUM: */

		    if (tjj < 1.) {
			if (xj > tjj * bignum) {

/*                          Scale x by 1/b(j). */

			    rec = 1. / xj;
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }
		    x[j] /= tjjs;
		    xj = (d__1 = x[j], abs(d__1));
		} else if (tjj > 0.) {

/*                    0 < abs(A(j,j)) <= SMLNUM: */

		    if (xj > tjj * bignum) {

/*                       Scale x by (1/abs(x(j)))*abs(A(j,j))*BIGNUM */
/*                       to avoid overflow when dividing by A(j,j). */

			rec = tjj * bignum / xj;
			if (cnorm[j] > 1.) {

/*                          Scale by 1/CNORM(j) to avoid overflow when */
/*                          multiplying x(j) times column j. */

			    rec /= cnorm[j];
			}
			dscal_(n, &rec, &x[1], &c__1);
			*scale *= rec;
			xmax *= rec;
		    }
		    x[j] /= tjjs;
		    xj = (d__1 = x[j], abs(d__1));
		} else {

/*                    A(j,j) = 0:  Set x(1:n) = 0, x(j) = 1, and */
/*                    scale = 0, and compute a solution to A*x = 0. */

		    i__3 = *n;
		    for (i__ = 1; i__ <= i__3; ++i__) {
			x[i__] = 0.;
/* L90: */
		    }
		    x[j] = 1.;
		    xj = 1.;
		    *scale = 0.;
		    xmax = 0.;
		}
L100:

/*              Scale x if necessary to avoid overflow when adding a */
/*              multiple of column j of A. */

		if (xj > 1.) {
		    rec = 1. / xj;
		    if (cnorm[j] > (bignum - xmax) * rec) {

/*                    Scale x by 1/(2*abs(x(j))). */

			rec *= .5;
			dscal_(n, &rec, &x[1], &c__1);
			*scale *= rec;
		    }
		} else if (xj * cnorm[j] > bignum - xmax) {

/*                 Scale x by 1/2. */

		    dscal_(n, &c_b36, &x[1], &c__1);
		    *scale *= .5;
		}

		if (upper) {
		    if (j > 1) {

/*                    Compute the update */
/*                       x(1:j-1) := x(1:j-1) - x(j) * A(1:j-1,j) */

			i__3 = j - 1;
			d__1 = -x[j] * tscal;
			daxpy_(&i__3, &d__1, &a[j * a_dim1 + 1], &c__1, &x[1],
				 &c__1);
			i__3 = j - 1;
			i__ = idamax_(&i__3, &x[1], &c__1);
			xmax = (d__1 = x[i__], abs(d__1));
		    }
		} else {
		    if (j < *n) {

/*                    Compute the update */
/*                       x(j+1:n) := x(j+1:n) - x(j) * A(j+1:n,j) */

			i__3 = *n - j;
			d__1 = -x[j] * tscal;
			daxpy_(&i__3, &d__1, &a[j + 1 + j * a_dim1], &c__1, &
				x[j + 1], &c__1);
			i__3 = *n - j;
			i__ = j + idamax_(&i__3, &x[j + 1], &c__1);
			xmax = (d__1 = x[i__], abs(d__1));
		    }
		}
/* L110: */
	    }

	} else {

/*           Solve A' * x = b */

	    i__2 = jlast;
	    i__1 = jinc;
	    for (j = jfirst; i__1 < 0 ? j >= i__2 : j <= i__2; j += i__1) {

/*              Compute x(j) = b(j) - sum A(k,j)*x(k). */
/*                                    k<>j */

		xj = (d__1 = x[j], abs(d__1));
		uscal = tscal;
		rec = 1. / std::max(xmax,1.);
		if (cnorm[j] > (bignum - xj) * rec) {

/*                 If x(j) could overflow, scale x by 1/(2*XMAX). */

		    rec *= .5;
		    if (nounit) {
			tjjs = a[j + j * a_dim1] * tscal;
		    } else {
			tjjs = tscal;
		    }
		    tjj = abs(tjjs);
		    if (tjj > 1.) {

/*                       Divide by A(j,j) when scaling x if A(j,j) > 1. */

/* Computing MIN */
			d__1 = 1., d__2 = rec * tjj;
			rec = std::min(d__1,d__2);
			uscal /= tjjs;
		    }
		    if (rec < 1.) {
			dscal_(n, &rec, &x[1], &c__1);
			*scale *= rec;
			xmax *= rec;
		    }
		}

		sumj = 0.;
		if (uscal == 1.) {

/*                 If the scaling needed for A in the dot product is 1, */
/*                 call DDOT to perform the dot product. */

		    if (upper) {
			i__3 = j - 1;
			sumj = ddot_(&i__3, &a[j * a_dim1 + 1], &c__1, &x[1],
				&c__1);
		    } else if (j < *n) {
			i__3 = *n - j;
			sumj = ddot_(&i__3, &a[j + 1 + j * a_dim1], &c__1, &x[
				j + 1], &c__1);
		    }
		} else {

/*                 Otherwise, use in-line code for the dot product. */

		    if (upper) {
			i__3 = j - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    sumj += a[i__ + j * a_dim1] * uscal * x[i__];
/* L120: */
			}
		    } else if (j < *n) {
			i__3 = *n;
			for (i__ = j + 1; i__ <= i__3; ++i__) {
			    sumj += a[i__ + j * a_dim1] * uscal * x[i__];
/* L130: */
			}
		    }
		}

		if (uscal == tscal) {

/*                 Compute x(j) := ( x(j) - sumj ) / A(j,j) if 1/A(j,j) */
/*                 was not used to scale the dotproduct. */

		    x[j] -= sumj;
		    xj = (d__1 = x[j], abs(d__1));
		    if (nounit) {
			tjjs = a[j + j * a_dim1] * tscal;
		    } else {
			tjjs = tscal;
			if (tscal == 1.) {
			    goto L150;
			}
		    }

/*                    Compute x(j) = x(j) / A(j,j), scaling if necessary. */

		    tjj = abs(tjjs);
		    if (tjj > smlnum) {

/*                       abs(A(j,j)) > SMLNUM: */

			if (tjj < 1.) {
			    if (xj > tjj * bignum) {

/*                             Scale X by 1/abs(x(j)). */

				rec = 1. / xj;
				dscal_(n, &rec, &x[1], &c__1);
				*scale *= rec;
				xmax *= rec;
			    }
			}
			x[j] /= tjjs;
		    } else if (tjj > 0.) {

/*                       0 < abs(A(j,j)) <= SMLNUM: */

			if (xj > tjj * bignum) {

/*                          Scale x by (1/abs(x(j)))*abs(A(j,j))*BIGNUM. */

			    rec = tjj * bignum / xj;
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
			x[j] /= tjjs;
		    } else {

/*                       A(j,j) = 0:  Set x(1:n) = 0, x(j) = 1, and */
/*                       scale = 0, and compute a solution to A'*x = 0. */

			i__3 = *n;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    x[i__] = 0.;
/* L140: */
			}
			x[j] = 1.;
			*scale = 0.;
			xmax = 0.;
		    }
L150:
		    ;
		} else {

/*                 Compute x(j) := x(j) / A(j,j)  - sumj if the dot */
/*                 product has already been divided by 1/A(j,j). */

		    x[j] = x[j] / tjjs - sumj;
		}
/* Computing MAX */
		d__2 = xmax, d__3 = (d__1 = x[j], abs(d__1));
		xmax = std::max(d__2,d__3);
/* L160: */
	    }
	}
	*scale /= tscal;
    }

/*     Scale the column norms by 1/TSCAL for return. */

    if (tscal != 1.) {
	d__1 = 1. / tscal;
	dscal_(n, &d__1, &cnorm[1], &c__1);
    }

    return 0;

/*     End of DLATRS */

} /* dlatrs_ */

/* Subroutine */ int dlatrz_(integer *m, integer *n, integer *l, double *a, integer *lda, double *tau, double *work)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__;

/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLATRZ factors the M-by-(M+L) real upper trapezoidal matrix */
/*  [ A1 A2 ] = [ A(1:M,1:M) A(1:M,N-L+1:N) ] as ( R  0 ) * Z, by means */
/*  of orthogonal transformations.  Z is an (M+L)-by-(M+L) orthogonal */
/*  matrix and, R and A1 are M-by-M upper triangular matrices. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  L       (input) INTEGER */
/*          The number of columns of the matrix A containing the */
/*          meaningful part of the Householder vectors. N-M >= L >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the leading M-by-N upper trapezoidal part of the */
/*          array A must contain the matrix to be factorized. */
/*          On exit, the leading M-by-M upper triangular part of A */
/*          contains the upper triangular matrix R, and elements N-L+1 to */
/*          N of the first M rows of A, with the array TAU, represent the */
/*          orthogonal matrix Z as a product of M elementary reflectors. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (M) */
/*          The scalar factors of the elementary reflectors. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (M) */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA */

/*  The factorization is obtained by Householder's method.  The kth */
/*  transformation matrix, Z( k ), which is used to introduce zeros into */
/*  the ( m - k + 1 )th row of A, is given in the form */

/*     Z( k ) = ( I     0   ), */
/*              ( 0  T( k ) ) */

/*  where */

/*     T( k ) = I - tau*u( k )*u( k )',   u( k ) = (   1    ), */
/*                                                 (   0    ) */
/*                                                 ( z( k ) ) */

/*  tau is a scalar and z( k ) is an l element vector. tau and z( k ) */
/*  are chosen to annihilate the elements of the kth row of A2. */

/*  The scalar tau is returned in the kth element of TAU and the vector */
/*  u( k ) in the kth row of A2, such that the elements of z( k ) are */
/*  in  a( k, l + 1 ), ..., a( k, n ). The elements of R are returned in */
/*  the upper triangular part of A1. */

/*  Z is given by */

/*     Z =  Z( 1 ) * Z( 2 ) * ... * Z( m ). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input arguments */

/*     Quick return if possible */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    if (*m == 0) {
	return 0;
    } else if (*m == *n) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    tau[i__] = 0.;
/* L10: */
	}
	return 0;
    }

    for (i__ = *m; i__ >= 1; --i__) {

/*        Generate elementary reflector H(i) to annihilate */
/*        [ A(i,i) A(i,n-l+1:n) ] */

	i__1 = *l + 1;
	dlarfp_(&i__1, &a[i__ + i__ * a_dim1], &a[i__ + (*n - *l + 1) *
		a_dim1], lda, &tau[i__]);

/*        Apply H(i) to A(1:i-1,i:n) from the right */

	i__1 = i__ - 1;
	i__2 = *n - i__ + 1;
	dlarz_("Right", &i__1, &i__2, l, &a[i__ + (*n - *l + 1) * a_dim1],
		lda, &tau[i__], &a[i__ * a_dim1 + 1], lda, &work[1]);

/* L20: */
    }

    return 0;

/*     End of DLATRZ */

} /* dlatrz_ */

/* Subroutine */ int dlatzm_(const char *side, integer *m, integer *n, double *
	v, integer *incv, double *tau, double *c1, double *c2,
	integer *ldc, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b5 = 1.;

    /* System generated locals */
    integer c1_dim1, c1_offset, c2_dim1, c2_offset, i__1;
    double d__1;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This routine is deprecated and has been replaced by routine DORMRZ. */

/*  DLATZM applies a Householder matrix generated by DTZRQF to a matrix. */

/*  Let P = I - tau*u*u',   u = ( 1 ), */
/*                              ( v ) */
/*  where v is an (m-1) vector if SIDE = 'L', or a (n-1) vector if */
/*  SIDE = 'R'. */

/*  If SIDE equals 'L', let */
/*         C = [ C1 ] 1 */
/*             [ C2 ] m-1 */
/*               n */
/*  Then C is overwritten by P*C. */

/*  If SIDE equals 'R', let */
/*         C = [ C1, C2 ] m */
/*                1  n-1 */
/*  Then C is overwritten by C*P. */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'L': form P * C */
/*          = 'R': form C * P */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix C. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix C. */

/*  V       (input) DOUBLE PRECISION array, dimension */
/*                  (1 + (M-1)*abs(INCV)) if SIDE = 'L' */
/*                  (1 + (N-1)*abs(INCV)) if SIDE = 'R' */
/*          The vector v in the representation of P. V is not used */
/*          if TAU = 0. */

/*  INCV    (input) INTEGER */
/*          The increment between elements of v. INCV <> 0 */

/*  TAU     (input) DOUBLE PRECISION */
/*          The value tau in the representation of P. */

/*  C1      (input/output) DOUBLE PRECISION array, dimension */
/*                         (LDC,N) if SIDE = 'L' */
/*                         (M,1)   if SIDE = 'R' */
/*          On entry, the n-vector C1 if SIDE = 'L', or the m-vector C1 */
/*          if SIDE = 'R'. */

/*          On exit, the first row of P*C if SIDE = 'L', or the first */
/*          column of C*P if SIDE = 'R'. */

/*  C2      (input/output) DOUBLE PRECISION array, dimension */
/*                         (LDC, N)   if SIDE = 'L' */
/*                         (LDC, N-1) if SIDE = 'R' */
/*          On entry, the (m - 1) x n matrix C2 if SIDE = 'L', or the */
/*          m x (n - 1) matrix C2 if SIDE = 'R'. */

/*          On exit, rows 2:m of P*C if SIDE = 'L', or columns 2:m of C*P */
/*          if SIDE = 'R'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the arrays C1 and C2. LDC >= (1,M). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                      (N) if SIDE = 'L' */
/*                      (M) if SIDE = 'R' */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --v;
    c2_dim1 = *ldc;
    c2_offset = 1 + c2_dim1;
    c2 -= c2_offset;
    c1_dim1 = *ldc;
    c1_offset = 1 + c1_dim1;
    c1 -= c1_offset;
    --work;

    /* Function Body */
    if (std::min(*m,*n) == 0 || *tau == 0.) {
	return 0;
    }

    if (lsame_(side, "L")) {

/*        w := C1 + v' * C2 */

	dcopy_(n, &c1[c1_offset], ldc, &work[1], &c__1);
	i__1 = *m - 1;
	dgemv_("Transpose", &i__1, n, &c_b5, &c2[c2_offset], ldc, &v[1], incv,
		 &c_b5, &work[1], &c__1);

/*        [ C1 ] := [ C1 ] - tau* [ 1 ] * w' */
/*        [ C2 ]    [ C2 ]        [ v ] */

	d__1 = -(*tau);
	daxpy_(n, &d__1, &work[1], &c__1, &c1[c1_offset], ldc);
	i__1 = *m - 1;
	d__1 = -(*tau);
	dger_(&i__1, n, &d__1, &v[1], incv, &work[1], &c__1, &c2[c2_offset],
		ldc);

    } else if (lsame_(side, "R")) {

/*        w := C1 + C2 * v */

	dcopy_(m, &c1[c1_offset], &c__1, &work[1], &c__1);
	i__1 = *n - 1;
	dgemv_("No transpose", m, &i__1, &c_b5, &c2[c2_offset], ldc, &v[1],
		incv, &c_b5, &work[1], &c__1);

/*        [ C1, C2 ] := [ C1, C2 ] - tau* w * [ 1 , v'] */

	d__1 = -(*tau);
	daxpy_(m, &d__1, &work[1], &c__1, &c1[c1_offset], &c__1);
	i__1 = *n - 1;
	d__1 = -(*tau);
	dger_(m, &i__1, &d__1, &work[1], &c__1, &v[1], incv, &c2[c2_offset],
		ldc);
    }

    return 0;

/*     End of DLATZM */

} /* dlatzm_ */

/* Subroutine */ int dlauu2_(const char *uplo, integer *n, double *a, integer *
	lda, integer *info)
{
	/* Table of constant values */
	static double c_b7 = 1.;
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__;
    double aii;
    bool upper;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAUU2 computes the product U * U' or L' * L, where the triangular */
/*  factor U or L is stored in the upper or lower triangular part of */
/*  the array A. */

/*  If UPLO = 'U' or 'u' then the upper triangle of the result is stored, */
/*  overwriting the factor U in A. */
/*  If UPLO = 'L' or 'l' then the lower triangle of the result is stored, */
/*  overwriting the factor L in A. */

/*  This is the unblocked form of the algorithm, calling Level 2 BLAS. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the triangular factor stored in the array A */
/*          is upper or lower triangular: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the triangular factor U or L.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the triangular factor U or L. */
/*          On exit, if UPLO = 'U', the upper triangle of A is */
/*          overwritten with the upper triangle of the product U * U'; */
/*          if UPLO = 'L', the lower triangle of A is overwritten with */
/*          the lower triangle of the product L' * L. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -k, the k-th argument had an illegal value */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAUU2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (upper) {

/*        Compute the product U * U'. */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    aii = a[i__ + i__ * a_dim1];
	    if (i__ < *n) {
		i__2 = *n - i__ + 1;
		a[i__ + i__ * a_dim1] = ddot_(&i__2, &a[i__ + i__ * a_dim1],
			lda, &a[i__ + i__ * a_dim1], lda);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		dgemv_("No transpose", &i__2, &i__3, &c_b7, &a[(i__ + 1) *
			a_dim1 + 1], lda, &a[i__ + (i__ + 1) * a_dim1], lda, &
			aii, &a[i__ * a_dim1 + 1], &c__1);
	    } else {
		dscal_(&i__, &aii, &a[i__ * a_dim1 + 1], &c__1);
	    }
/* L10: */
	}

    } else {

/*        Compute the product L' * L. */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    aii = a[i__ + i__ * a_dim1];
	    if (i__ < *n) {
		i__2 = *n - i__ + 1;
		a[i__ + i__ * a_dim1] = ddot_(&i__2, &a[i__ + i__ * a_dim1], &
			c__1, &a[i__ + i__ * a_dim1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b7, &a[i__ + 1 + a_dim1],
			 lda, &a[i__ + 1 + i__ * a_dim1], &c__1, &aii, &a[i__
			+ a_dim1], lda);
	    } else {
		dscal_(&i__, &aii, &a[i__ + a_dim1], lda);
	    }
/* L20: */
	}
    }

    return 0;

/*     End of DLAUU2 */

} /* dlauu2_ */

/* Subroutine */ int dlauum_(const char *uplo, integer *n, double *a, integer *
	lda, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b15 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, ib, nb;
    bool upper;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAUUM computes the product U * U' or L' * L, where the triangular */
/*  factor U or L is stored in the upper or lower triangular part of */
/*  the array A. */

/*  If UPLO = 'U' or 'u' then the upper triangle of the result is stored, */
/*  overwriting the factor U in A. */
/*  If UPLO = 'L' or 'l' then the lower triangle of the result is stored, */
/*  overwriting the factor L in A. */

/*  This is the blocked form of the algorithm, calling Level 3 BLAS. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the triangular factor stored in the array A */
/*          is upper or lower triangular: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the triangular factor U or L.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the triangular factor U or L. */
/*          On exit, if UPLO = 'U', the upper triangle of A is */
/*          overwritten with the upper triangle of the product U * U'; */
/*          if UPLO = 'L', the lower triangle of A is overwritten with */
/*          the lower triangle of the product L' * L. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -k, the k-th argument had an illegal value */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAUUM", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Determine the block size for this environment. */

    nb = ilaenv_(&c__1, "DLAUUM", uplo, n, &c_n1, &c_n1, &c_n1);

    if (nb <= 1 || nb >= *n) {

/*        Use unblocked code */

	dlauu2_(uplo, n, &a[a_offset], lda, info);
    } else {

/*        Use blocked code */

	if (upper) {

/*           Compute the product U * U'. */

	    i__1 = *n;
	    i__2 = nb;
	    for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
		i__3 = nb, i__4 = *n - i__ + 1;
		ib = std::min(i__3,i__4);
		i__3 = i__ - 1;
		dtrmm_("Right", "Upper", "Transpose", "Non-unit", &i__3, &ib,
			&c_b15, &a[i__ + i__ * a_dim1], lda, &a[i__ * a_dim1
			+ 1], lda)
			;
		dlauu2_("Upper", &ib, &a[i__ + i__ * a_dim1], lda, info);
		if (i__ + ib <= *n) {
		    i__3 = i__ - 1;
		    i__4 = *n - i__ - ib + 1;
		    dgemm_("No transpose", "Transpose", &i__3, &ib, &i__4, &
			    c_b15, &a[(i__ + ib) * a_dim1 + 1], lda, &a[i__ +
			    (i__ + ib) * a_dim1], lda, &c_b15, &a[i__ *
			    a_dim1 + 1], lda);
		    i__3 = *n - i__ - ib + 1;
		    dsyrk_("Upper", "No transpose", &ib, &i__3, &c_b15, &a[
			    i__ + (i__ + ib) * a_dim1], lda, &c_b15, &a[i__ +
			    i__ * a_dim1], lda);
		}
/* L10: */
	    }
	} else {

/*           Compute the product L' * L. */

	    i__2 = *n;
	    i__1 = nb;
	    for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__1) {
/* Computing MIN */
		i__3 = nb, i__4 = *n - i__ + 1;
		ib = std::min(i__3,i__4);
		i__3 = i__ - 1;
		dtrmm_("Left", "Lower", "Transpose", "Non-unit", &ib, &i__3, &
			c_b15, &a[i__ + i__ * a_dim1], lda, &a[i__ + a_dim1],
			lda);
		dlauu2_("Lower", &ib, &a[i__ + i__ * a_dim1], lda, info);
		if (i__ + ib <= *n) {
		    i__3 = i__ - 1;
		    i__4 = *n - i__ - ib + 1;
		    dgemm_("Transpose", "No transpose", &ib, &i__3, &i__4, &
			    c_b15, &a[i__ + ib + i__ * a_dim1], lda, &a[i__ +
			    ib + a_dim1], lda, &c_b15, &a[i__ + a_dim1], lda);
		    i__3 = *n - i__ - ib + 1;
		    dsyrk_("Lower", "Transpose", &ib, &i__3, &c_b15, &a[i__ +
			    ib + i__ * a_dim1], lda, &c_b15, &a[i__ + i__ *
			    a_dim1], lda);
		}
/* L20: */
	    }
	}
    }

    return 0;

/*     End of DLAUUM */

} /* dlauum_ */

/* Subroutine */ int dlazq3_(integer *i0, integer *n0, double *z__,
	integer *pp, double *dmin__, double *sigma, double *desig,
	double *qmax, integer *nfail, integer *iter, integer *ndiv,
	bool *ieee, integer *ttype, double *dmin1, double *dmin2,
	double *dn, double *dn1, double *dn2, double *tau)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    double g, s, t;
    integer j4, nn;
    double eps, tol;
    integer n0in, ipn4;
    double tol2, temp;
    double safmin;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAZQ3 checks for deflation, computes a shift (TAU) and calls dqds. */
/*  In case of failure it changes shifts, and tries again until output */
/*  is positive. */

/*  Arguments */
/*  ========= */

/*  I0     (input) INTEGER */
/*         First index. */

/*  N0     (input) INTEGER */
/*         Last index. */

/*  Z      (input) DOUBLE PRECISION array, dimension ( 4*N ) */
/*         Z holds the qd array. */

/*  PP     (input) INTEGER */
/*         PP=0 for ping, PP=1 for pong. */

/*  DMIN   (output) DOUBLE PRECISION */
/*         Minimum value of d. */

/*  SIGMA  (output) DOUBLE PRECISION */
/*         Sum of shifts used in current segment. */

/*  DESIG  (input/output) DOUBLE PRECISION */
/*         Lower order part of SIGMA */

/*  QMAX   (input) DOUBLE PRECISION */
/*         Maximum value of q. */

/*  NFAIL  (output) INTEGER */
/*         Number of times shift was too big. */

/*  ITER   (output) INTEGER */
/*         Number of iterations. */

/*  NDIV   (output) INTEGER */
/*         Number of divisions. */

/*  IEEE   (input) LOGICAL */
/*         Flag for IEEE or non IEEE arithmetic (passed to DLASQ5). */

/*  TTYPE  (input/output) INTEGER */
/*         Shift type.  TTYPE is passed as an argument in order to save */
/*         its value between calls to DLAZQ3 */

/*  DMIN1  (input/output) REAL */
/*  DMIN2  (input/output) REAL */
/*  DN     (input/output) REAL */
/*  DN1    (input/output) REAL */
/*  DN2    (input/output) REAL */
/*  TAU    (input/output) REAL */
/*         These are passed as arguments in order to save their values */
/*         between calls to DLAZQ3 */

/*  This is a thread safe version of DLASQ3, which passes TTYPE, DMIN1, */
/*  DMIN2, DN, DN1. DN2 and TAU through the argument list in place of */
/*  declaring them in a SAVE statment. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Function .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --z__;

    /* Function Body */
    n0in = *n0;
    eps = dlamch_("Precision");
    safmin = dlamch_("Safe minimum");
    tol = eps * 100.;
/* Computing 2nd power */
    d__1 = tol;
    tol2 = d__1 * d__1;
    g = 0.;

/*     Check for deflation. */

L10:

    if (*n0 < *i0) {
	return 0;
    }
    if (*n0 == *i0) {
	goto L20;
    }
    nn = (*n0 << 2) + *pp;
    if (*n0 == *i0 + 1) {
	goto L40;
    }

/*     Check whether E(N0-1) is negligible, 1 eigenvalue. */

    if (z__[nn - 5] > tol2 * (*sigma + z__[nn - 3]) && z__[nn - (*pp << 1) -
	    4] > tol2 * z__[nn - 7]) {
	goto L30;
    }

L20:

    z__[(*n0 << 2) - 3] = z__[(*n0 << 2) + *pp - 3] + *sigma;
    --(*n0);
    goto L10;

/*     Check  whether E(N0-2) is negligible, 2 eigenvalues. */

L30:

    if (z__[nn - 9] > tol2 * *sigma && z__[nn - (*pp << 1) - 8] > tol2 * z__[
	    nn - 11]) {
	goto L50;
    }

L40:

    if (z__[nn - 3] > z__[nn - 7]) {
	s = z__[nn - 3];
	z__[nn - 3] = z__[nn - 7];
	z__[nn - 7] = s;
    }
    if (z__[nn - 5] > z__[nn - 3] * tol2) {
	t = (z__[nn - 7] - z__[nn - 3] + z__[nn - 5]) * .5;
	s = z__[nn - 3] * (z__[nn - 5] / t);
	if (s <= t) {
	    s = z__[nn - 3] * (z__[nn - 5] / (t * (sqrt(s / t + 1.) + 1.)));
	} else {
	    s = z__[nn - 3] * (z__[nn - 5] / (t + sqrt(t) * sqrt(t + s)));
	}
	t = z__[nn - 7] + (s + z__[nn - 5]);
	z__[nn - 3] *= z__[nn - 7] / t;
	z__[nn - 7] = t;
    }
    z__[(*n0 << 2) - 7] = z__[nn - 7] + *sigma;
    z__[(*n0 << 2) - 3] = z__[nn - 3] + *sigma;
    *n0 += -2;
    goto L10;

L50:

/*     Reverse the qd-array, if warranted. */

    if (*dmin__ <= 0. || *n0 < n0in) {
	if (z__[(*i0 << 2) + *pp - 3] * 1.5 < z__[(*n0 << 2) + *pp - 3]) {
	    ipn4 = *i0 + *n0 << 2;
	    i__1 = *i0 + *n0 - 1 << 1;
	    for (j4 = *i0 << 2; j4 <= i__1; j4 += 4) {
		temp = z__[j4 - 3];
		z__[j4 - 3] = z__[ipn4 - j4 - 3];
		z__[ipn4 - j4 - 3] = temp;
		temp = z__[j4 - 2];
		z__[j4 - 2] = z__[ipn4 - j4 - 2];
		z__[ipn4 - j4 - 2] = temp;
		temp = z__[j4 - 1];
		z__[j4 - 1] = z__[ipn4 - j4 - 5];
		z__[ipn4 - j4 - 5] = temp;
		temp = z__[j4];
		z__[j4] = z__[ipn4 - j4 - 4];
		z__[ipn4 - j4 - 4] = temp;
/* L60: */
	    }
	    if (*n0 - *i0 <= 4) {
		z__[(*n0 << 2) + *pp - 1] = z__[(*i0 << 2) + *pp - 1];
		z__[(*n0 << 2) - *pp] = z__[(*i0 << 2) - *pp];
	    }
/* Computing MIN */
	    d__1 = *dmin2, d__2 = z__[(*n0 << 2) + *pp - 1];
	    *dmin2 = std::min(d__1,d__2);
/* Computing MIN */
	    d__1 = z__[(*n0 << 2) + *pp - 1], d__2 = z__[(*i0 << 2) + *pp - 1]
		    , d__1 = std::min(d__1,d__2), d__2 = z__[(*i0 << 2) + *pp + 3];
	    z__[(*n0 << 2) + *pp - 1] = std::min(d__1,d__2);
/* Computing MIN */
	    d__1 = z__[(*n0 << 2) - *pp], d__2 = z__[(*i0 << 2) - *pp], d__1 =
		     std::min(d__1,d__2), d__2 = z__[(*i0 << 2) - *pp + 4];
	    z__[(*n0 << 2) - *pp] = std::min(d__1,d__2);
/* Computing MAX */
	    d__1 = *qmax, d__2 = z__[(*i0 << 2) + *pp - 3], d__1 = std::max(d__1,
		    d__2), d__2 = z__[(*i0 << 2) + *pp + 1];
	    *qmax = std::max(d__1,d__2);
	    *dmin__ = -0.;
	}
    }

/* Computing MIN */
    d__1 = z__[(*n0 << 2) + *pp - 1], d__2 = z__[(*n0 << 2) + *pp - 9], d__1 =
	     std::min(d__1,d__2), d__2 = *dmin2 + z__[(*n0 << 2) - *pp];
    if (*dmin__ < 0. || safmin * *qmax < std::min(d__1,d__2)) {

/*        Choose a shift. */

	dlazq4_(i0, n0, &z__[1], pp, &n0in, dmin__, dmin1, dmin2, dn, dn1,
		dn2, tau, ttype, &g);

/*        Call dqds until DMIN > 0. */

L80:

	dlasq5_(i0, n0, &z__[1], pp, tau, dmin__, dmin1, dmin2, dn, dn1, dn2,
		ieee);

	*ndiv += *n0 - *i0 + 2;
	++(*iter);

/*        Check status. */

	if (*dmin__ >= 0. && *dmin1 > 0.) {

/*           Success. */

	    goto L100;

	} else if (*dmin__ < 0. && *dmin1 > 0. && z__[(*n0 - 1 << 2) - *pp] <
		tol * (*sigma + *dn1) && abs(*dn) < tol * *sigma) {

/*           Convergence hidden by negative DN. */

	    z__[(*n0 - 1 << 2) - *pp + 2] = 0.;
	    *dmin__ = 0.;
	    goto L100;
	} else if (*dmin__ < 0.) {

/*           TAU too big. Select new TAU and try again. */

	    ++(*nfail);
	    if (*ttype < -22) {

/*              Failed twice. Play it safe. */

		*tau = 0.;
	    } else if (*dmin1 > 0.) {

/*              Late failure. Gives excellent shift. */

		*tau = (*tau + *dmin__) * (1. - eps * 2.);
		*ttype += -11;
	    } else {

/*              Early failure. Divide by 4. */

		*tau *= .25;
		*ttype += -12;
	    }
	    goto L80;
	} else if (*dmin__ != *dmin__) {

/*           NaN. */

	    *tau = 0.;
	    goto L80;
	} else {

/*           Possible underflow. Play it safe. */

	    goto L90;
	}
    }

/*     Risk of underflow. */

L90:
    dlasq6_(i0, n0, &z__[1], pp, dmin__, dmin1, dmin2, dn, dn1, dn2);
    *ndiv += *n0 - *i0 + 2;
    ++(*iter);
    *tau = 0.;

L100:
    if (*tau < *sigma) {
	*desig += *tau;
	t = *sigma + *desig;
	*desig -= t - *sigma;
    } else {
	t = *sigma + *tau;
	*desig = *sigma - (t - *tau) + *desig;
    }
    *sigma = t;

    return 0;

/*     End of DLAZQ3 */

} /* dlazq3_ */

/* Subroutine */ int dlazq4_(integer *i0, integer *n0, double *z__,
	integer *pp, integer *n0in, double *dmin__, double *dmin1,
	double *dmin2, double *dn, double *dn1, double *dn2,
	double *tau, integer *ttype, double *g)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    double s, a2, b1, b2;
    integer i4, nn, np;
    double gam, gap1, gap2;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAZQ4 computes an approximation TAU to the smallest eigenvalue */
/*  using values of d from the previous transform. */

/*  I0    (input) INTEGER */
/*        First index. */

/*  N0    (input) INTEGER */
/*        Last index. */

/*  Z     (input) DOUBLE PRECISION array, dimension ( 4*N ) */
/*        Z holds the qd array. */

/*  PP    (input) INTEGER */
/*        PP=0 for ping, PP=1 for pong. */

/*  N0IN  (input) INTEGER */
/*        The value of N0 at start of EIGTEST. */

/*  DMIN  (input) DOUBLE PRECISION */
/*        Minimum value of d. */

/*  DMIN1 (input) DOUBLE PRECISION */
/*        Minimum value of d, excluding D( N0 ). */

/*  DMIN2 (input) DOUBLE PRECISION */
/*        Minimum value of d, excluding D( N0 ) and D( N0-1 ). */

/*  DN    (input) DOUBLE PRECISION */
/*        d(N) */

/*  DN1   (input) DOUBLE PRECISION */
/*        d(N-1) */

/*  DN2   (input) DOUBLE PRECISION */
/*        d(N-2) */

/*  TAU   (output) DOUBLE PRECISION */
/*        This is the shift. */

/*  TTYPE (output) INTEGER */
/*        Shift type. */

/*  G     (input/output) DOUBLE PRECISION */
/*        G is passed as an argument in order to save its value between */
/*        calls to DLAZQ4 */

/*  Further Details */
/*  =============== */
/*  CNST1 = 9/16 */

/*  This is a thread safe version of DLASQ4, which passes G through the */
/*  argument list in place of declaring G in a SAVE statment. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     A negative DMIN forces the shift to take that absolute value */
/*     TTYPE records the type of shift. */

    /* Parameter adjustments */
    --z__;

    /* Function Body */
    if (*dmin__ <= 0.) {
	*tau = -(*dmin__);
	*ttype = -1;
	return 0;
    }

    nn = (*n0 << 2) + *pp;
    if (*n0in == *n0) {

/*        No eigenvalues deflated. */

	if (*dmin__ == *dn || *dmin__ == *dn1) {

	    b1 = sqrt(z__[nn - 3]) * sqrt(z__[nn - 5]);
	    b2 = sqrt(z__[nn - 7]) * sqrt(z__[nn - 9]);
	    a2 = z__[nn - 7] + z__[nn - 5];

/*           Cases 2 and 3. */

	    if (*dmin__ == *dn && *dmin1 == *dn1) {
		gap2 = *dmin2 - a2 - *dmin2 * .25;
		if (gap2 > 0. && gap2 > b2) {
		    gap1 = a2 - *dn - b2 / gap2 * b2;
		} else {
		    gap1 = a2 - *dn - (b1 + b2);
		}
		if (gap1 > 0. && gap1 > b1) {
/* Computing MAX */
		    d__1 = *dn - b1 / gap1 * b1, d__2 = *dmin__ * .5;
		    s = std::max(d__1,d__2);
		    *ttype = -2;
		} else {
		    s = 0.;
		    if (*dn > b1) {
			s = *dn - b1;
		    }
		    if (a2 > b1 + b2) {
/* Computing MIN */
			d__1 = s, d__2 = a2 - (b1 + b2);
			s = std::min(d__1,d__2);
		    }
/* Computing MAX */
		    d__1 = s, d__2 = *dmin__ * .333;
		    s = std::max(d__1,d__2);
		    *ttype = -3;
		}
	    } else {

/*              Case 4. */

		*ttype = -4;
		s = *dmin__ * .25;
		if (*dmin__ == *dn) {
		    gam = *dn;
		    a2 = 0.;
		    if (z__[nn - 5] > z__[nn - 7]) {
			return 0;
		    }
		    b2 = z__[nn - 5] / z__[nn - 7];
		    np = nn - 9;
		} else {
		    np = nn - (*pp << 1);
		    b2 = z__[np - 2];
		    gam = *dn1;
		    if (z__[np - 4] > z__[np - 2]) {
			return 0;
		    }
		    a2 = z__[np - 4] / z__[np - 2];
		    if (z__[nn - 9] > z__[nn - 11]) {
			return 0;
		    }
		    b2 = z__[nn - 9] / z__[nn - 11];
		    np = nn - 13;
		}

/*              Approximate contribution to norm squared from I < NN-1. */

		a2 += b2;
		i__1 = (*i0 << 2) - 1 + *pp;
		for (i4 = np; i4 >= i__1; i4 += -4) {
		    if (b2 == 0.) {
			goto L20;
		    }
		    b1 = b2;
		    if (z__[i4] > z__[i4 - 2]) {
			return 0;
		    }
		    b2 *= z__[i4] / z__[i4 - 2];
		    a2 += b2;
		    if (std::max(b2,b1) * 100. < a2 || .563 < a2) {
			goto L20;
		    }
/* L10: */
		}
L20:
		a2 *= 1.05;

/*              Rayleigh quotient residual bound. */

		if (a2 < .563) {
		    s = gam * (1. - sqrt(a2)) / (a2 + 1.);
		}
	    }
	} else if (*dmin__ == *dn2) {

/*           Case 5. */

	    *ttype = -5;
	    s = *dmin__ * .25;

/*           Compute contribution to norm squared from I > NN-2. */

	    np = nn - (*pp << 1);
	    b1 = z__[np - 2];
	    b2 = z__[np - 6];
	    gam = *dn2;
	    if (z__[np - 8] > b2 || z__[np - 4] > b1) {
		return 0;
	    }
	    a2 = z__[np - 8] / b2 * (z__[np - 4] / b1 + 1.);

/*           Approximate contribution to norm squared from I < NN-2. */

	    if (*n0 - *i0 > 2) {
		b2 = z__[nn - 13] / z__[nn - 15];
		a2 += b2;
		i__1 = (*i0 << 2) - 1 + *pp;
		for (i4 = nn - 17; i4 >= i__1; i4 += -4) {
		    if (b2 == 0.) {
			goto L40;
		    }
		    b1 = b2;
		    if (z__[i4] > z__[i4 - 2]) {
			return 0;
		    }
		    b2 *= z__[i4] / z__[i4 - 2];
		    a2 += b2;
		    if (std::max(b2,b1) * 100. < a2 || .563 < a2) {
			goto L40;
		    }
/* L30: */
		}
L40:
		a2 *= 1.05;
	    }

	    if (a2 < .563) {
		s = gam * (1. - sqrt(a2)) / (a2 + 1.);
	    }
	} else {

/*           Case 6, no information to guide us. */

	    if (*ttype == -6) {
		*g += (1. - *g) * .333;
	    } else if (*ttype == -18) {
		*g = .083250000000000005;
	    } else {
		*g = .25;
	    }
	    s = *g * *dmin__;
	    *ttype = -6;
	}

    } else if (*n0in == *n0 + 1) {

/*        One eigenvalue just deflated. Use DMIN1, DN1 for DMIN and DN. */

	if (*dmin1 == *dn1 && *dmin2 == *dn2) {

/*           Cases 7 and 8. */

	    *ttype = -7;
	    s = *dmin1 * .333;
	    if (z__[nn - 5] > z__[nn - 7]) {
		return 0;
	    }
	    b1 = z__[nn - 5] / z__[nn - 7];
	    b2 = b1;
	    if (b2 == 0.) {
		goto L60;
	    }
	    i__1 = (*i0 << 2) - 1 + *pp;
	    for (i4 = (*n0 << 2) - 9 + *pp; i4 >= i__1; i4 += -4) {
		a2 = b1;
		if (z__[i4] > z__[i4 - 2]) {
		    return 0;
		}
		b1 *= z__[i4] / z__[i4 - 2];
		b2 += b1;
		if (std::max(b1,a2) * 100. < b2) {
		    goto L60;
		}
/* L50: */
	    }
L60:
	    b2 = sqrt(b2 * 1.05);
/* Computing 2nd power */
	    d__1 = b2;
	    a2 = *dmin1 / (d__1 * d__1 + 1.);
	    gap2 = *dmin2 * .5 - a2;
	    if (gap2 > 0. && gap2 > b2 * a2) {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - a2 * 1.01 * (b2 / gap2) * b2);
		s = std::max(d__1,d__2);
	    } else {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - b2 * 1.01);
		s = std::max(d__1,d__2);
		*ttype = -8;
	    }
	} else {

/*           Case 9. */

	    s = *dmin1 * .25;
	    if (*dmin1 == *dn1) {
		s = *dmin1 * .5;
	    }
	    *ttype = -9;
	}

    } else if (*n0in == *n0 + 2) {

/*        Two eigenvalues deflated. Use DMIN2, DN2 for DMIN and DN. */

/*        Cases 10 and 11. */

	if (*dmin2 == *dn2 && z__[nn - 5] * 2. < z__[nn - 7]) {
	    *ttype = -10;
	    s = *dmin2 * .333;
	    if (z__[nn - 5] > z__[nn - 7]) {
		return 0;
	    }
	    b1 = z__[nn - 5] / z__[nn - 7];
	    b2 = b1;
	    if (b2 == 0.) {
		goto L80;
	    }
	    i__1 = (*i0 << 2) - 1 + *pp;
	    for (i4 = (*n0 << 2) - 9 + *pp; i4 >= i__1; i4 += -4) {
		if (z__[i4] > z__[i4 - 2]) {
		    return 0;
		}
		b1 *= z__[i4] / z__[i4 - 2];
		b2 += b1;
		if (b1 * 100. < b2) {
		    goto L80;
		}
/* L70: */
	    }
L80:
	    b2 = sqrt(b2 * 1.05);
/* Computing 2nd power */
	    d__1 = b2;
	    a2 = *dmin2 / (d__1 * d__1 + 1.);
	    gap2 = z__[nn - 7] + z__[nn - 9] - sqrt(z__[nn - 11]) * sqrt(z__[
		    nn - 9]) - a2;
	    if (gap2 > 0. && gap2 > b2 * a2) {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - a2 * 1.01 * (b2 / gap2) * b2);
		s = std::max(d__1,d__2);
	    } else {
/* Computing MAX */
		d__1 = s, d__2 = a2 * (1. - b2 * 1.01);
		s = std::max(d__1,d__2);
	    }
	} else {
	    s = *dmin2 * .25;
	    *ttype = -11;
	}
    } else if (*n0in > *n0 + 2) {

/*        Case 12, more than two eigenvalues deflated. No information. */

	s = 0.;
	*ttype = -12;
    }

    *tau = s;
    return 0;

/*     End of DLAZQ4 */

} /* dlazq4_ */
