#include "clapack.h"
#include "f2cP.h"

/* Subroutine */ int dtbcon_(const char *norm, const char *uplo, const char *diag, integer *n,
	integer *kd, double *ab, integer *ldab, double *rcond,
	double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, i__1;
    double d__1;

    /* Local variables */
    integer ix, kase, kase1;
    double scale;
    integer isave[3];
    double anorm;
    bool upper;
    double xnorm;
    double ainvnm;
    bool onenrm;
    char normin[1];
    double smlnum;
    bool nounit;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     Modified to call DLACN2 in place of DLACON, 5 Feb 03, SJH. */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTBCON estimates the reciprocal of the condition number of a */
/*  triangular band matrix A, in either the 1-norm or the infinity-norm. */

/*  The norm of A is computed and an estimate is obtained for */
/*  norm(inv(A)), then the reciprocal of the condition number is */
/*  computed as */
/*     RCOND = 1 / ( norm(A) * norm(inv(A)) ). */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies whether the 1-norm condition number or the */
/*          infinity-norm condition number is required: */
/*          = '1' or 'O':  1-norm; */
/*          = 'I':         Infinity-norm. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of superdiagonals or subdiagonals of the */
/*          triangular band matrix A.  KD >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The upper or lower triangular band matrix A, stored in the */
/*          first kd+1 rows of the array. The j-th column of A is stored */
/*          in the j-th column of the array AB as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */
/*          If DIAG = 'U', the diagonal elements of A are not referenced */
/*          and are assumed to be 1. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of the matrix A, */
/*          computed as RCOND = 1/(norm(A) * norm(inv(A))). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

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
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    onenrm = *(unsigned char *)norm == '1' || lsame_(norm, "O");
    nounit = lsame_(diag, "N");

    if (! onenrm && ! lsame_(norm, "I")) {
	*info = -1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*kd < 0) {
	*info = -5;
    } else if (*ldab < *kd + 1) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTBCON", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	*rcond = 1.;
	return 0;
    }

    *rcond = 0.;
    smlnum = dlamch_("Safe minimum") * (double) std::max(1_integer,*n);

/*     Compute the norm of the triangular matrix A. */

    anorm = dlantb_(norm, uplo, diag, n, kd, &ab[ab_offset], ldab, &work[1]);

/*     Continue only if ANORM > 0. */

    if (anorm > 0.) {

/*        Estimate the norm of the inverse of A. */

	ainvnm = 0.;
	*(unsigned char *)normin = 'N';
	if (onenrm) {
	    kase1 = 1;
	} else {
	    kase1 = 2;
	}
	kase = 0;
L10:
	dlacn2_(n, &work[*n + 1], &work[1], &iwork[1], &ainvnm, &kase, isave);
	if (kase != 0) {
	    if (kase == kase1) {

/*              Multiply by inv(A). */

		dlatbs_(uplo, "No transpose", diag, normin, n, kd, &ab[
			ab_offset], ldab, &work[1], &scale, &work[(*n << 1) +
			1], info)
			;
	    } else {

/*              Multiply by inv(A'). */

		dlatbs_(uplo, "Transpose", diag, normin, n, kd, &ab[ab_offset]
, ldab, &work[1], &scale, &work[(*n << 1) + 1], info);
	    }
	    *(unsigned char *)normin = 'Y';

/*           Multiply by 1/SCALE if doing so will not cause overflow. */

	    if (scale != 1.) {
		ix = idamax_(n, &work[1], &c__1);
		xnorm = (d__1 = work[ix], abs(d__1));
		if (scale < xnorm * smlnum || scale == 0.) {
		    goto L20;
		}
		drscl_(n, &scale, &work[1], &c__1);
	    }
	    goto L10;
	}

/*        Compute the estimate of the reciprocal condition number. */

	if (ainvnm != 0.) {
	    *rcond = 1. / anorm / ainvnm;
	}
    }

L20:
    return 0;

/*     End of DTBCON */

} /* dtbcon_ */

/* Subroutine */ int dtbrfs_(const char *uplo, const char *trans, const char *diag, integer *n,
	integer *kd, integer *nrhs, double *ab, integer *ldab, double
	*b, integer *ldb, double *x, integer *ldx, double *ferr,
	double *berr, double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b19 = -1.;

    /* System generated locals */
    integer ab_dim1, ab_offset, b_dim1, b_offset, x_dim1, x_offset, i__1,
	    i__2, i__3, i__4, i__5;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k;
    double s, xk;
    integer nz;
    double eps;
    integer kase;
    double safe1, safe2;
    integer isave[3];
    bool upper;
	double safmin;
    bool notran;
    char transt[1];
    bool nounit;
    double lstres;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     Modified to call DLACN2 in place of DLACON, 5 Feb 03, SJH. */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTBRFS provides error bounds and backward error estimates for the */
/*  solution to a system of linear equations with a triangular band */
/*  coefficient matrix. */

/*  The solution matrix X must be computed by DTBTRS or some other */
/*  means before entering this routine.  DTBRFS does not do iterative */
/*  refinement because doing so cannot improve the backward error. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B  (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of superdiagonals or subdiagonals of the */
/*          triangular band matrix A.  KD >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The upper or lower triangular band matrix A, stored in the */
/*          first kd+1 rows of the array. The j-th column of A is stored */
/*          in the j-th column of the array AB as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */
/*          If DIAG = 'U', the diagonal elements of A are not referenced */
/*          and are assumed to be 1. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The right hand side matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          The solution matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  FERR    (output) DOUBLE PRECISION array, dimension (NRHS) */
/*          The estimated forward error bound for each solution vector */
/*          X(j) (the j-th column of the solution matrix X). */
/*          If XTRUE is the true solution corresponding to X(j), FERR(j) */
/*          is an estimated upper bound for the magnitude of the largest */
/*          element in (X(j) - XTRUE) divided by the magnitude of the */
/*          largest element in X(j).  The estimate is as reliable as */
/*          the estimate for RCOND, and is almost always a slight */
/*          overestimate of the true error. */

/*  BERR    (output) DOUBLE PRECISION array, dimension (NRHS) */
/*          The componentwise relative backward error of each solution */
/*          vector X(j) (i.e., the smallest relative change in */
/*          any element of A or B that makes X(j) an exact solution). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

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
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    --ferr;
    --berr;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    notran = lsame_(trans, "N");
    nounit = lsame_(diag, "N");

    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T") && !
	    lsame_(trans, "C")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*kd < 0) {
	*info = -5;
    } else if (*nrhs < 0) {
	*info = -6;
    } else if (*ldab < *kd + 1) {
	*info = -8;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -10;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTBRFS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    ferr[j] = 0.;
	    berr[j] = 0.;
/* L10: */
	}
	return 0;
    }

    if (notran) {
	*(unsigned char *)transt = 'T';
    } else {
	*(unsigned char *)transt = 'N';
    }

/*     NZ = maximum number of nonzero elements in each row of A, plus 1 */

    nz = *kd + 2;
    eps = dlamch_("Epsilon");
    safmin = dlamch_("Safe minimum");
    safe1 = nz * safmin;
    safe2 = safe1 / eps;

/*     Do for each right hand side */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {

/*        Compute residual R = B - op(A) * X, */
/*        where op(A) = A or A', depending on TRANS. */

	dcopy_(n, &x[j * x_dim1 + 1], &c__1, &work[*n + 1], &c__1);
	dtbmv_(uplo, trans, diag, n, kd, &ab[ab_offset], ldab, &work[*n + 1],
		&c__1);
	daxpy_(n, &c_b19, &b[j * b_dim1 + 1], &c__1, &work[*n + 1], &c__1);

/*        Compute componentwise relative backward error from formula */

/*        max(i) ( abs(R(i)) / ( abs(op(A))*abs(X) + abs(B) )(i) ) */

/*        where abs(Z) is the componentwise absolute value of the matrix */
/*        or vector Z.  If the i-th component of the denominator is less */
/*        than SAFE2, then SAFE1 is added to the i-th components of the */
/*        numerator and denominator before dividing. */

	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[i__] = (d__1 = b[i__ + j * b_dim1], abs(d__1));
/* L20: */
	}

	if (notran) {

/*           Compute abs(A)*abs(X) + abs(B). */

	    if (upper) {
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
/* Computing MAX */
			i__3 = 1, i__4 = k - *kd;
			i__5 = k;
			for (i__ = std::max(i__3,i__4); i__ <= i__5; ++i__) {
			    work[i__] += (d__1 = ab[*kd + 1 + i__ - k + k *
				    ab_dim1], abs(d__1)) * xk;
/* L30: */
			}
/* L40: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
/* Computing MAX */
			i__5 = 1, i__3 = k - *kd;
			i__4 = k - 1;
			for (i__ = std::max(i__5,i__3); i__ <= i__4; ++i__) {
			    work[i__] += (d__1 = ab[*kd + 1 + i__ - k + k *
				    ab_dim1], abs(d__1)) * xk;
/* L50: */
			}
			work[k] += xk;
/* L60: */
		    }
		}
	    } else {
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
/* Computing MIN */
			i__5 = *n, i__3 = k + *kd;
			i__4 = std::min(i__5,i__3);
			for (i__ = k; i__ <= i__4; ++i__) {
			    work[i__] += (d__1 = ab[i__ + 1 - k + k * ab_dim1]
				    , abs(d__1)) * xk;
/* L70: */
			}
/* L80: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
/* Computing MIN */
			i__5 = *n, i__3 = k + *kd;
			i__4 = std::min(i__5,i__3);
			for (i__ = k + 1; i__ <= i__4; ++i__) {
			    work[i__] += (d__1 = ab[i__ + 1 - k + k * ab_dim1]
				    , abs(d__1)) * xk;
/* L90: */
			}
			work[k] += xk;
/* L100: */
		    }
		}
	    }
	} else {

/*           Compute abs(A')*abs(X) + abs(B). */

	    if (upper) {
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = 0.;
/* Computing MAX */
			i__4 = 1, i__5 = k - *kd;
			i__3 = k;
			for (i__ = std::max(i__4,i__5); i__ <= i__3; ++i__) {
			    s += (d__1 = ab[*kd + 1 + i__ - k + k * ab_dim1],
				    abs(d__1)) * (d__2 = x[i__ + j * x_dim1],
				    abs(d__2));
/* L110: */
			}
			work[k] += s;
/* L120: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = (d__1 = x[k + j * x_dim1], abs(d__1));
/* Computing MAX */
			i__3 = 1, i__4 = k - *kd;
			i__5 = k - 1;
			for (i__ = std::max(i__3,i__4); i__ <= i__5; ++i__) {
			    s += (d__1 = ab[*kd + 1 + i__ - k + k * ab_dim1],
				    abs(d__1)) * (d__2 = x[i__ + j * x_dim1],
				    abs(d__2));
/* L130: */
			}
			work[k] += s;
/* L140: */
		    }
		}
	    } else {
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = 0.;
/* Computing MIN */
			i__3 = *n, i__4 = k + *kd;
			i__5 = std::min(i__3,i__4);
			for (i__ = k; i__ <= i__5; ++i__) {
			    s += (d__1 = ab[i__ + 1 - k + k * ab_dim1], abs(
				    d__1)) * (d__2 = x[i__ + j * x_dim1], abs(
				    d__2));
/* L150: */
			}
			work[k] += s;
/* L160: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = (d__1 = x[k + j * x_dim1], abs(d__1));
/* Computing MIN */
			i__3 = *n, i__4 = k + *kd;
			i__5 = std::min(i__3,i__4);
			for (i__ = k + 1; i__ <= i__5; ++i__) {
			    s += (d__1 = ab[i__ + 1 - k + k * ab_dim1], abs(
				    d__1)) * (d__2 = x[i__ + j * x_dim1], abs(
				    d__2));
/* L170: */
			}
			work[k] += s;
/* L180: */
		    }
		}
	    }
	}
	s = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (work[i__] > safe2) {
/* Computing MAX */
		d__2 = s, d__3 = (d__1 = work[*n + i__], abs(d__1)) / work[
			i__];
		s = std::max(d__2,d__3);
	    } else {
/* Computing MAX */
		d__2 = s, d__3 = ((d__1 = work[*n + i__], abs(d__1)) + safe1)
			/ (work[i__] + safe1);
		s = std::max(d__2,d__3);
	    }
/* L190: */
	}
	berr[j] = s;

/*        Bound error from formula */

/*        norm(X - XTRUE) / norm(X) .le. FERR = */
/*        norm( abs(inv(op(A)))* */
/*           ( abs(R) + NZ*EPS*( abs(op(A))*abs(X)+abs(B) ))) / norm(X) */

/*        where */
/*          norm(Z) is the magnitude of the largest component of Z */
/*          inv(op(A)) is the inverse of op(A) */
/*          abs(Z) is the componentwise absolute value of the matrix or */
/*             vector Z */
/*          NZ is the maximum number of nonzeros in any row of A, plus 1 */
/*          EPS is machine epsilon */

/*        The i-th component of abs(R)+NZ*EPS*(abs(op(A))*abs(X)+abs(B)) */
/*        is incremented by SAFE1 if the i-th component of */
/*        abs(op(A))*abs(X) + abs(B) is less than SAFE2. */

/*        Use DLACN2 to estimate the infinity-norm of the matrix */
/*           inv(op(A)) * diag(W), */
/*        where W = abs(R) + NZ*EPS*( abs(op(A))*abs(X)+abs(B) ))) */

	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (work[i__] > safe2) {
		work[i__] = (d__1 = work[*n + i__], abs(d__1)) + nz * eps *
			work[i__];
	    } else {
		work[i__] = (d__1 = work[*n + i__], abs(d__1)) + nz * eps *
			work[i__] + safe1;
	    }
/* L200: */
	}

	kase = 0;
L210:
	dlacn2_(n, &work[(*n << 1) + 1], &work[*n + 1], &iwork[1], &ferr[j], &
		kase, isave);
	if (kase != 0) {
	    if (kase == 1) {

/*              Multiply by diag(W)*inv(op(A)'). */

		dtbsv_(uplo, transt, diag, n, kd, &ab[ab_offset], ldab, &work[
			*n + 1], &c__1);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L220: */
		}
	    } else {

/*              Multiply by inv(op(A))*diag(W). */

		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L230: */
		}
		dtbsv_(uplo, trans, diag, n, kd, &ab[ab_offset], ldab, &work[*
			n + 1], &c__1);
	    }
	    goto L210;
	}

/*        Normalize error. */

	lstres = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = lstres, d__3 = (d__1 = x[i__ + j * x_dim1], abs(d__1));
	    lstres = std::max(d__2,d__3);
/* L240: */
	}
	if (lstres != 0.) {
	    ferr[j] /= lstres;
	}

/* L250: */
    }

    return 0;

/*     End of DTBRFS */

} /* dtbrfs_ */

/* Subroutine */ int dtbtrs_(const char *uplo, const char *trans, const char *diag, integer *n,
	integer *kd, integer *nrhs, double *ab, integer *ldab, double
	*b, integer *ldb, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, b_dim1, b_offset, i__1;

    /* Local variables */
    integer j;
    bool upper;
    bool nounit;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTBTRS solves a triangular system of the form */

/*     A * X = B  or  A**T * X = B, */

/*  where A is a triangular band matrix of order N, and B is an */
/*  N-by NRHS matrix.  A check is made to verify that A is nonsingular. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form the system of equations: */
/*          = 'N':  A * X = B  (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of superdiagonals or subdiagonals of the */
/*          triangular band matrix A.  KD >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The upper or lower triangular band matrix A, stored in the */
/*          first kd+1 rows of AB.  The j-th column of A is stored */
/*          in the j-th column of the array AB as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */
/*          If DIAG = 'U', the diagonal elements of A are not referenced */
/*          and are assumed to be 1. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the right hand side matrix B. */
/*          On exit, if INFO = 0, the solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, the i-th diagonal element of A is zero, */
/*                indicating that the matrix is singular and the */
/*                solutions X have not been computed. */

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
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    nounit = lsame_(diag, "N");
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! lsame_(trans, "N") && ! lsame_(trans,
	    "T") && ! lsame_(trans, "C")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*kd < 0) {
	*info = -5;
    } else if (*nrhs < 0) {
	*info = -6;
    } else if (*ldab < *kd + 1) {
	*info = -8;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTBTRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Check for singularity. */

    if (nounit) {
	if (upper) {
	    i__1 = *n;
	    for (*info = 1; *info <= i__1; ++(*info)) {
		if (ab[*kd + 1 + *info * ab_dim1] == 0.) {
		    return 0;
		}
/* L10: */
	    }
	} else {
	    i__1 = *n;
	    for (*info = 1; *info <= i__1; ++(*info)) {
		if (ab[*info * ab_dim1 + 1] == 0.) {
		    return 0;
		}
/* L20: */
	    }
	}
    }
    *info = 0;

/*     Solve A * X = B  or  A' * X = B. */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	dtbsv_(uplo, trans, diag, n, kd, &ab[ab_offset], ldab, &b[j * b_dim1
		+ 1], &c__1);
/* L30: */
    }

    return 0;

/*     End of DTBTRS */

} /* dtbtrs_ */

int dtfsm_(const char *transr, const char *side, const char *uplo, const char *trans,
	 const char *diag, integer *m, integer *n, double *alpha, double *a, double *b, integer *ldb)
{
	/* Table of constant values */
	static double c_b23 = -1.;
	static double c_b27 = 1.;

    /* System generated locals */
    integer b_dim1, b_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, k, m1, m2, n1, n2, info;
    bool normaltransr, lside, lower, misodd, nisodd, notrans;


/*  -- LAPACK routine (version 3.2.1)                                    -- */

/*  -- Contributed by Fred Gustavson of the IBM Watson Research Center -- */
/*  -- April 2009                                                      -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Level 3 BLAS like routine for A in RFP Format. */

/*  DTFSM  solves the matrix equation */

/*     op( A )*X = alpha*B  or  X*op( A ) = alpha*B */

/*  where alpha is a scalar, X and B are m by n matrices, A is a unit, or */
/*  non-unit,  upper or lower triangular matrix  and  op( A )  is one  of */

/*     op( A ) = A   or   op( A ) = A'. */

/*  A is in Rectangular Full Packed (RFP) Format. */

/*  The matrix X is overwritten on B. */

/*  Arguments */
/*  ========== */

/*  TRANSR - (input) CHARACTER */
/*          = 'N':  The Normal Form of RFP A is stored; */
/*          = 'T':  The Transpose Form of RFP A is stored. */

/*  SIDE   - (input) CHARACTER */
/*           On entry, SIDE specifies whether op( A ) appears on the left */
/*           or right of X as follows: */

/*              SIDE = 'L' or 'l'   op( A )*X = alpha*B. */

/*              SIDE = 'R' or 'r'   X*op( A ) = alpha*B. */

/*           Unchanged on exit. */

/*  UPLO   - (input) CHARACTER */
/*           On entry, UPLO specifies whether the RFP matrix A came from */
/*           an upper or lower triangular matrix as follows: */
/*           UPLO = 'U' or 'u' RFP A came from an upper triangular matrix */
/*           UPLO = 'L' or 'l' RFP A came from a  lower triangular matrix */

/*           Unchanged on exit. */

/*  TRANS  - (input) CHARACTER */
/*           On entry, TRANS  specifies the form of op( A ) to be used */
/*           in the matrix multiplication as follows: */

/*              TRANS  = 'N' or 'n'   op( A ) = A. */

/*              TRANS  = 'T' or 't'   op( A ) = A'. */

/*           Unchanged on exit. */

/*  DIAG   - (input) CHARACTER */
/*           On entry, DIAG specifies whether or not RFP A is unit */
/*           triangular as follows: */

/*              DIAG = 'U' or 'u'   A is assumed to be unit triangular. */

/*              DIAG = 'N' or 'n'   A is not assumed to be unit */
/*                                  triangular. */

/*           Unchanged on exit. */

/*  M      - (input) INTEGER. */
/*           On entry, M specifies the number of rows of B. M must be at */
/*           least zero. */
/*           Unchanged on exit. */

/*  N      - (input) INTEGER. */
/*           On entry, N specifies the number of columns of B.  N must be */
/*           at least zero. */
/*           Unchanged on exit. */

/*  ALPHA  - (input) DOUBLE PRECISION. */
/*           On entry,  ALPHA specifies the scalar  alpha. When  alpha is */
/*           zero then  A is not referenced and  B need not be set before */
/*           entry. */
/*           Unchanged on exit. */

/*  A      - (input) DOUBLE PRECISION array, dimension (NT); */
/*           NT = N*(N+1)/2. On entry, the matrix A in RFP Format. */
/*           RFP Format is described by TRANSR, UPLO and N as follows: */
/*           If TRANSR='N' then RFP A is (0:N,0:K-1) when N is even; */
/*           K=N/2. RFP A is (0:N-1,0:K) when N is odd; K=N/2. If */
/*           TRANSR = 'T' then RFP is the transpose of RFP A as */
/*           defined when TRANSR = 'N'. The contents of RFP A are defined */
/*           by UPLO as follows: If UPLO = 'U' the RFP A contains the NT */
/*           elements of upper packed A either in normal or */
/*           transpose Format. If UPLO = 'L' the RFP A contains */
/*           the NT elements of lower packed A either in normal or */
/*           transpose Format. The LDA of RFP A is (N+1)/2 when */
/*           TRANSR = 'T'. When TRANSR is 'N' the LDA is N+1 when N is */
/*           even and is N when is odd. */
/*           See the Note below for more details. Unchanged on exit. */

/*  B      - (input/ouptut) DOUBLE PRECISION array,  DIMENSION (LDB,N) */
/*           Before entry,  the leading  m by n part of the array  B must */
/*           contain  the  right-hand  side  matrix  B,  and  on exit  is */
/*           overwritten by the solution matrix  X. */

/*  LDB    - (input) INTEGER. */
/*           On entry, LDB specifies the first dimension of B as declared */
/*           in  the  calling  (sub)  program.   LDB  must  be  at  least */
/*           max( 1, m ). */
/*           Unchanged on exit. */

/*  Further Details */
/*  =============== */

/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  even. We give an example where N = 6. */

/*      AP is Upper             AP is Lower */

/*   00 01 02 03 04 05       00 */
/*      11 12 13 14 15       10 11 */
/*         22 23 24 25       20 21 22 */
/*            33 34 35       30 31 32 33 */
/*               44 45       40 41 42 43 44 */
/*                  55       50 51 52 53 54 55 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:5,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(4:6,0:2) consists of */
/*  the transpose of the first three columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(1:6,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:2,0:2) consists of */
/*  the transpose of the last three columns of AP lower. */
/*  This covers the case N even and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        03 04 05                33 43 53 */
/*        13 14 15                00 44 54 */
/*        23 24 25                10 11 55 */
/*        33 34 35                20 21 22 */
/*        00 44 45                30 31 32 */
/*        01 11 55                40 41 42 */
/*        02 12 22                50 51 52 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */


/*           RFP A                   RFP A */

/*     03 13 23 33 00 01 02    33 00 10 20 30 40 50 */
/*     04 14 24 34 44 11 12    43 44 11 21 31 41 51 */
/*     05 15 25 35 45 55 22    53 54 55 22 32 42 52 */


/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  odd. We give an example where N = 5. */

/*     AP is Upper                 AP is Lower */

/*   00 01 02 03 04              00 */
/*      11 12 13 14              10 11 */
/*         22 23 24              20 21 22 */
/*            33 34              30 31 32 33 */
/*               44              40 41 42 43 44 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:4,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(3:4,0:1) consists of */
/*  the transpose of the first two columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(0:4,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:1,1:2) consists of */
/*  the transpose of the last two columns of AP lower. */
/*  This covers the case N odd and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        02 03 04                00 33 43 */
/*        12 13 14                10 11 44 */
/*        22 23 24                20 21 22 */
/*        00 33 34                30 31 32 */
/*        01 11 44                40 41 42 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */

/*           RFP A                   RFP A */

/*     02 12 22 00 01             00 10 20 30 40 50 */
/*     03 13 23 33 11             33 11 21 31 41 51 */
/*     04 14 24 34 44             43 44 22 32 42 52 */

/*  Reference */
/*  ========= */

/*  ===================================================================== */

/*     .. */
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
    b_dim1 = *ldb - 1 - 0 + 1;
    b_offset = 0 + b_dim1 * 0;
    b -= b_offset;

    /* Function Body */
    info = 0;
    normaltransr = lsame_(transr, "N");
    lside = lsame_(side, "L");
    lower = lsame_(uplo, "L");
    notrans = lsame_(trans, "N");
    if (! normaltransr && ! lsame_(transr, "T")) {
	info = -1;
    } else if (! lside && ! lsame_(side, "R")) {
	info = -2;
    } else if (! lower && ! lsame_(uplo, "U")) {
	info = -3;
    } else if (! notrans && ! lsame_(trans, "T")) {
	info = -4;
    } else if (! lsame_(diag, "N") && ! lsame_(diag,
	    "U")) {
	info = -5;
    } else if (*m < 0) {
	info = -6;
    } else if (*n < 0) {
	info = -7;
    } else if (*ldb < std::max(1_integer,*m)) {
	info = -11;
    }
    if (info != 0) {
	i__1 = -info;
	xerbla_("DTFSM ", &i__1);
	return 0;
    }

/*     Quick return when ( (N.EQ.0).OR.(M.EQ.0) ) */

    if (*m == 0 || *n == 0) {
	return 0;
    }

/*     Quick return when ALPHA.EQ.(0D+0) */

    if (*alpha == 0.) {
	i__1 = *n - 1;
	for (j = 0; j <= i__1; ++j) {
	    i__2 = *m - 1;
	    for (i__ = 0; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = 0.;
/* L10: */
	    }
/* L20: */
	}
	return 0;
    }

    if (lside) {

/*        SIDE = 'L' */

/*        A is M-by-M. */
/*        If M is odd, set NISODD = .TRUE., and M1 and M2. */
/*        If M is even, NISODD = .FALSE., and M. */

	if (*m % 2 == 0) {
	    misodd = false;
	    k = *m / 2;
	} else {
	    misodd = true;
	    if (lower) {
		m2 = *m / 2;
		m1 = *m - m2;
	    } else {
		m1 = *m / 2;
		m2 = *m - m1;
	    }
	}


	if (misodd) {

/*           SIDE = 'L' and N is odd */

	    if (normaltransr) {

/*              SIDE = 'L', N is odd, and TRANSR = 'N' */

		if (lower) {

/*                 SIDE  ='L', N is odd, TRANSR = 'N', and UPLO = 'L' */

		    if (notrans) {

/*                    SIDE  ='L', N is odd, TRANSR = 'N', UPLO = 'L', and */
/*                    TRANS = 'N' */

			if (*m == 1) {
			    dtrsm_("L", "L", "N", diag, &m1, n, alpha, a, m, &b[b_offset], ldb);
			} else {
			    dtrsm_("L", "L", "N", diag, &m1, n, alpha, a, m, &b[b_offset], ldb);
			    dgemm_("N", "N", &m2, n, &m1, &c_b23, &a[m1], m, &b[b_offset], ldb, alpha, &b[m1], ldb);
			    dtrsm_("L", "U", "T", diag, &m2, n, &c_b27, &a[*m], m, &b[m1], ldb);
			}

		    } else {

/*                    SIDE  ='L', N is odd, TRANSR = 'N', UPLO = 'L', and */
/*                    TRANS = 'T' */

			if (*m == 1) {
			    dtrsm_("L", "L", "T", diag, &m1, n, alpha, a, m, &b[b_offset], ldb);
			} else {
			    dtrsm_("L", "U", "N", diag, &m2, n, alpha, &a[*m], m, &b[m1], ldb);
			    dgemm_("T", "N", &m1, n, &m2, &c_b23, &a[m1], m, &b[m1], ldb, alpha, &b[b_offset], ldb);
			    dtrsm_("L", "L", "T", diag, &m1, n, &c_b27, a, m, &b[b_offset], ldb);
			}

		    }

		} else {

/*                 SIDE  ='L', N is odd, TRANSR = 'N', and UPLO = 'U' */

		    if (! notrans) {

/*                    SIDE  ='L', N is odd, TRANSR = 'N', UPLO = 'U', and */
/*                    TRANS = 'N' */

			dtrsm_("L", "L", "N", diag, &m1, n, alpha, &a[m2], m, &b[b_offset], ldb);
			dgemm_("T", "N", &m2, n, &m1, &c_b23, a, m, &b[b_offset], ldb, alpha, &b[m1], ldb);
			dtrsm_("L", "U", "T", diag, &m2, n, &c_b27, &a[m1], m, &b[m1], ldb);

		    } else {

/*                    SIDE  ='L', N is odd, TRANSR = 'N', UPLO = 'U', and */
/*                    TRANS = 'T' */

			dtrsm_("L", "U", "N", diag, &m2, n, alpha, &a[m1], m, &b[m1], ldb);
			dgemm_("N", "N", &m1, n, &m2, &c_b23, a, m, &b[m1], ldb, alpha, &b[b_offset], ldb);
			dtrsm_("L", "L", "T", diag, &m1, n, &c_b27, &a[m2], m, &b[b_offset], ldb);

		    }

		}

	    } else {

/*              SIDE = 'L', N is odd, and TRANSR = 'T' */

		if (lower) {

/*                 SIDE  ='L', N is odd, TRANSR = 'T', and UPLO = 'L' */

		    if (notrans) {

/*                    SIDE  ='L', N is odd, TRANSR = 'T', UPLO = 'L', and */
/*                    TRANS = 'N' */

			if (*m == 1) {
			    dtrsm_("L", "U", "T", diag, &m1, n, alpha, a, &m1, &b[b_offset], ldb);
			} else {
			    dtrsm_("L", "U", "T", diag, &m1, n, alpha, a, &m1, &b[b_offset], ldb);
			    dgemm_("T", "N", &m2, n, &m1, &c_b23, &a[m1 * m1], &m1, &b[b_offset], ldb, alpha, &b[m1], ldb);
			    dtrsm_("L", "L", "N", diag, &m2, n, &c_b27, &a[1], &m1, &b[m1], ldb);
			}

		    } else {

/*                    SIDE  ='L', N is odd, TRANSR = 'T', UPLO = 'L', and */
/*                    TRANS = 'T' */

			if (*m == 1) {
			    dtrsm_("L", "U", "N", diag, &m1, n, alpha, a, &m1, &b[b_offset], ldb);
			} else {
			    dtrsm_("L", "L", "T", diag, &m2, n, alpha, &a[1], &m1, &b[m1], ldb);
			    dgemm_("N", "N", &m1, n, &m2, &c_b23, &a[m1 * m1], &m1, &b[m1], ldb, alpha, &b[b_offset], ldb);
			    dtrsm_("L", "U", "N", diag, &m1, n, &c_b27, a, &m1, &b[b_offset], ldb);
			}

		    }

		} else {

/*                 SIDE  ='L', N is odd, TRANSR = 'T', and UPLO = 'U' */

		    if (! notrans) {

/*                    SIDE  ='L', N is odd, TRANSR = 'T', UPLO = 'U', and */
/*                    TRANS = 'N' */

			dtrsm_("L", "U", "T", diag, &m1, n, alpha, &a[m2 * m2], &m2, &b[b_offset], ldb);
			dgemm_("N", "N", &m2, n, &m1, &c_b23, a, &m2, &b[b_offset], ldb, alpha, &b[m1], ldb);
			dtrsm_("L", "L", "N", diag, &m2, n, &c_b27, &a[m1 * m2], &m2, &b[m1], ldb);

		    } else {

/*                    SIDE  ='L', N is odd, TRANSR = 'T', UPLO = 'U', and */
/*                    TRANS = 'T' */

			dtrsm_("L", "L", "T", diag, &m2, n, alpha, &a[m1 * m2], &m2, &b[m1], ldb);
			dgemm_("T", "N", &m1, n, &m2, &c_b23, a, &m2, &b[m1], ldb, alpha, &b[b_offset], ldb);
			dtrsm_("L", "U", "N", diag, &m1, n, &c_b27, &a[m2 * m2], &m2, &b[b_offset], ldb);

		    }

		}

	    }

	} else {

/*           SIDE = 'L' and N is even */

	    if (normaltransr) {

/*              SIDE = 'L', N is even, and TRANSR = 'N' */

		if (lower) {

/*                 SIDE  ='L', N is even, TRANSR = 'N', and UPLO = 'L' */

		    if (notrans) {

/*                    SIDE  ='L', N is even, TRANSR = 'N', UPLO = 'L', */
/*                    and TRANS = 'N' */

			i__1 = *m + 1;
			dtrsm_("L", "L", "N", diag, &k, n, alpha, &a[1], &i__1, &b[b_offset], ldb);
			i__1 = *m + 1;
			dgemm_("N", "N", &k, n, &k, &c_b23, &a[k + 1], &i__1, &b[b_offset], ldb, alpha, &b[k], ldb);
			i__1 = *m + 1;
			dtrsm_("L", "U", "T", diag, &k, n, &c_b27, a, &i__1, &b[k], ldb);

		    } else {

/*                    SIDE  ='L', N is even, TRANSR = 'N', UPLO = 'L', */
/*                    and TRANS = 'T' */

			i__1 = *m + 1;
			dtrsm_("L", "U", "N", diag, &k, n, alpha, a, &i__1, &b[k], ldb);
			i__1 = *m + 1;
			dgemm_("T", "N", &k, n, &k, &c_b23, &a[k + 1], &i__1, &b[k], ldb, alpha, &b[b_offset], ldb);
			i__1 = *m + 1;
			dtrsm_("L", "L", "T", diag, &k, n, &c_b27, &a[1], &i__1, &b[b_offset], ldb);

		    }

		} else {

/*                 SIDE  ='L', N is even, TRANSR = 'N', and UPLO = 'U' */

		    if (! notrans) {

/*                    SIDE  ='L', N is even, TRANSR = 'N', UPLO = 'U', */
/*                    and TRANS = 'N' */

			i__1 = *m + 1;
			dtrsm_("L", "L", "N", diag, &k, n, alpha, &a[k + 1], &i__1, &b[b_offset], ldb);
			i__1 = *m + 1;
			dgemm_("T", "N", &k, n, &k, &c_b23, a, &i__1, &b[b_offset], ldb, alpha, &b[k], ldb);
			i__1 = *m + 1;
			dtrsm_("L", "U", "T", diag, &k, n, &c_b27, &a[k], &i__1, &b[k], ldb);

		    } else {

/*                    SIDE  ='L', N is even, TRANSR = 'N', UPLO = 'U', */
/*                    and TRANS = 'T' */
			i__1 = *m + 1;
			dtrsm_("L", "U", "N", diag, &k, n, alpha, &a[k], &i__1, &b[k], ldb);
			i__1 = *m + 1;
			dgemm_("N", "N", &k, n, &k, &c_b23, a, &i__1, &b[k], ldb, alpha, &b[b_offset], ldb);
			i__1 = *m + 1;
			dtrsm_("L", "L", "T", diag, &k, n, &c_b27, &a[k + 1], &i__1, &b[b_offset], ldb);

		    }

		}

	    } else {

/*              SIDE = 'L', N is even, and TRANSR = 'T' */

		if (lower) {

/*                 SIDE  ='L', N is even, TRANSR = 'T', and UPLO = 'L' */

		    if (notrans) {

/*                    SIDE  ='L', N is even, TRANSR = 'T', UPLO = 'L', */
/*                    and TRANS = 'N' */

			dtrsm_("L", "U", "T", diag, &k, n, alpha, &a[k], &k, &b[b_offset], ldb);
			dgemm_("T", "N", &k, n, &k, &c_b23, &a[k * (k + 1)], &k, &b[b_offset], ldb, alpha, &b[k], ldb);
			dtrsm_("L", "L", "N", diag, &k, n, &c_b27, a, &k, &b[k], ldb);

		    } else {

/*                    SIDE  ='L', N is even, TRANSR = 'T', UPLO = 'L', */
/*                    and TRANS = 'T' */

			dtrsm_("L", "L", "T", diag, &k, n, alpha, a, &k, &b[k], ldb);
			dgemm_("N", "N", &k, n, &k, &c_b23, &a[k * (k + 1)], &k, &b[k], ldb, alpha, &b[b_offset], ldb);
			dtrsm_("L", "U", "N", diag, &k, n, &c_b27, &a[k], &k, &b[b_offset], ldb);

		    }

		} else {

/*                 SIDE  ='L', N is even, TRANSR = 'T', and UPLO = 'U' */

		    if (! notrans) {

/*                    SIDE  ='L', N is even, TRANSR = 'T', UPLO = 'U', */
/*                    and TRANS = 'N' */

			dtrsm_("L", "U", "T", diag, &k, n, alpha, &a[k * (k + 1)], &k, &b[b_offset], ldb);
			dgemm_("N", "N", &k, n, &k, &c_b23, a, &k, &b[b_offset], ldb, alpha, &b[k], ldb);
			dtrsm_("L", "L", "N", diag, &k, n, &c_b27, &a[k * k], &k, &b[k], ldb);

		    } else {

/*                    SIDE  ='L', N is even, TRANSR = 'T', UPLO = 'U', */
/*                    and TRANS = 'T' */

			dtrsm_("L", "L", "T", diag, &k, n, alpha, &a[k * k], &k, &b[k], ldb);
			dgemm_("T", "N", &k, n, &k, &c_b23, a, &k, &b[k], ldb, alpha, &b[b_offset], ldb);
			dtrsm_("L", "U", "N", diag, &k, n, &c_b27, &a[k * (k + 1)], &k, &b[b_offset], ldb);

		    }

		}

	    }

	}

    } else {

/*        SIDE = 'R' */

/*        A is N-by-N. */
/*        If N is odd, set NISODD = .TRUE., and N1 and N2. */
/*        If N is even, NISODD = .FALSE., and K. */

	if (*n % 2 == 0) {
	    nisodd = false;
	    k = *n / 2;
	} else {
	    nisodd = true;
	    if (lower) {
		n2 = *n / 2;
		n1 = *n - n2;
	    } else {
		n1 = *n / 2;
		n2 = *n - n1;
	    }
	}

	if (nisodd) {

/*           SIDE = 'R' and N is odd */

	    if (normaltransr) {

/*              SIDE = 'R', N is odd, and TRANSR = 'N' */

		if (lower) {

/*                 SIDE  ='R', N is odd, TRANSR = 'N', and UPLO = 'L' */

		    if (notrans) {

/*                    SIDE  ='R', N is odd, TRANSR = 'N', UPLO = 'L', and */
/*                    TRANS = 'N' */

			dtrsm_("R", "U", "T", diag, m, &n2, alpha, &a[*n], n, &b[n1 * b_dim1], ldb);
			dgemm_("N", "N", m, &n1, &n2, &c_b23, &b[n1 * b_dim1], ldb, &a[n1], n, alpha, b, ldb);
			dtrsm_("R", "L", "N", diag, m, &n1, &c_b27, a, n, b, ldb);

		    } else {

/*                    SIDE  ='R', N is odd, TRANSR = 'N', UPLO = 'L', and */
/*                    TRANS = 'T' */

			dtrsm_("R", "L", "T", diag, m, &n1, alpha, a, n, b, ldb);
			dgemm_("N", "T", m, &n2, &n1, &c_b23, b, ldb, &a[n1], n, alpha, &b[n1 * b_dim1], ldb);
			dtrsm_("R", "U", "N", diag, m, &n2, &c_b27, &a[*n], n, &b[n1 * b_dim1], ldb);

		    }

		} else {

/*                 SIDE  ='R', N is odd, TRANSR = 'N', and UPLO = 'U' */

		    if (notrans) {

/*                    SIDE  ='R', N is odd, TRANSR = 'N', UPLO = 'U', and */
/*                    TRANS = 'N' */

			dtrsm_("R", "L", "T", diag, m, &n1, alpha, &a[n2], n, b, ldb);
			dgemm_("N", "N", m, &n2, &n1, &c_b23, b, ldb, a, n, alpha, &b[n1 * b_dim1], ldb);
			dtrsm_("R", "U", "N", diag, m, &n2, &c_b27, &a[n1], n, &b[n1 * b_dim1], ldb);

		    } else {

/*                    SIDE  ='R', N is odd, TRANSR = 'N', UPLO = 'U', and */
/*                    TRANS = 'T' */

			dtrsm_("R", "U", "T", diag, m, &n2, alpha, &a[n1], n, &b[n1 * b_dim1], ldb);
			dgemm_("N", "T", m, &n1, &n2, &c_b23, &b[n1 * b_dim1], ldb, a, n, alpha, b, ldb);
			dtrsm_("R", "L", "N", diag, m, &n1, &c_b27, &a[n2], n, b, ldb);

		    }

		}

	    } else {

/*              SIDE = 'R', N is odd, and TRANSR = 'T' */

		if (lower) {

/*                 SIDE  ='R', N is odd, TRANSR = 'T', and UPLO = 'L' */

		    if (notrans) {

/*                    SIDE  ='R', N is odd, TRANSR = 'T', UPLO = 'L', and */
/*                    TRANS = 'N' */

			dtrsm_("R", "L", "N", diag, m, &n2, alpha, &a[1], &n1, &b[n1 * b_dim1], ldb);
			dgemm_("N", "T", m, &n1, &n2, &c_b23, &b[n1 * b_dim1], ldb, &a[n1 * n1], &n1, alpha, b, ldb);
			dtrsm_("R", "U", "T", diag, m, &n1, &c_b27, a, &n1, b, ldb);

		    } else {

/*                    SIDE  ='R', N is odd, TRANSR = 'T', UPLO = 'L', and */
/*                    TRANS = 'T' */

			dtrsm_("R", "U", "N", diag, m, &n1, alpha, a, &n1, b, ldb);
			dgemm_("N", "N", m, &n2, &n1, &c_b23, b, ldb, &a[n1 * n1], &n1, alpha, &b[n1 * b_dim1], ldb);
			dtrsm_("R", "L", "T", diag, m, &n2, &c_b27, &a[1], &n1, &b[n1 * b_dim1], ldb);

		    }

		} else {

/*                 SIDE  ='R', N is odd, TRANSR = 'T', and UPLO = 'U' */

		    if (notrans) {

/*                    SIDE  ='R', N is odd, TRANSR = 'T', UPLO = 'U', and */
/*                    TRANS = 'N' */

			dtrsm_("R", "U", "N", diag, m, &n1, alpha, &a[n2 * n2], &n2, b, ldb);
			dgemm_("N", "T", m, &n2, &n1, &c_b23, b, ldb, a, &n2, alpha, &b[n1 * b_dim1], ldb);
			dtrsm_("R", "L", "T", diag, m, &n2, &c_b27, &a[n1 * n2], &n2, &b[n1 * b_dim1], ldb);

		    } else {

/*                    SIDE  ='R', N is odd, TRANSR = 'T', UPLO = 'U', and */
/*                    TRANS = 'T' */

			dtrsm_("R", "L", "N", diag, m, &n2, alpha, &a[n1 * n2], &n2, &b[n1 * b_dim1], ldb);
			dgemm_("N", "N", m, &n1, &n2, &c_b23, &b[n1 * b_dim1], ldb, a, &n2, alpha, b, ldb);
			dtrsm_("R", "U", "T", diag, m, &n1, &c_b27, &a[n2 * n2], &n2, b, ldb);

		    }

		}

	    }

	} else {

/*           SIDE = 'R' and N is even */

	    if (normaltransr) {

/*              SIDE = 'R', N is even, and TRANSR = 'N' */

		if (lower) {

/*                 SIDE  ='R', N is even, TRANSR = 'N', and UPLO = 'L' */

		    if (notrans) {

/*                    SIDE  ='R', N is even, TRANSR = 'N', UPLO = 'L', */
/*                    and TRANS = 'N' */

			i__1 = *n + 1;
			dtrsm_("R", "U", "T", diag, m, &k, alpha, a, &i__1, &b[k * b_dim1], ldb);
			i__1 = *n + 1;
			dgemm_("N", "N", m, &k, &k, &c_b23, &b[k * b_dim1], ldb, &a[k + 1], &i__1, alpha, b, ldb);
			i__1 = *n + 1;
			dtrsm_("R", "L", "N", diag, m, &k, &c_b27, &a[1], &i__1, b, ldb);

		    } else {

/*                    SIDE  ='R', N is even, TRANSR = 'N', UPLO = 'L', */
/*                    and TRANS = 'T' */

			i__1 = *n + 1;
			dtrsm_("R", "L", "T", diag, m, &k, alpha, &a[1], &i__1, b, ldb);
			i__1 = *n + 1;
			dgemm_("N", "T", m, &k, &k, &c_b23, b, ldb, &a[k + 1], &i__1, alpha, &b[k * b_dim1], ldb);
			i__1 = *n + 1;
			dtrsm_("R", "U", "N", diag, m, &k, &c_b27, a, &i__1, &b[k * b_dim1], ldb);

		    }

		} else {

/*                 SIDE  ='R', N is even, TRANSR = 'N', and UPLO = 'U' */

		    if (notrans) {

/*                    SIDE  ='R', N is even, TRANSR = 'N', UPLO = 'U', */
/*                    and TRANS = 'N' */

			i__1 = *n + 1;
			dtrsm_("R", "L", "T", diag, m, &k, alpha, &a[k + 1], &i__1, b, ldb);
			i__1 = *n + 1;
			dgemm_("N", "N", m, &k, &k, &c_b23, b, ldb, a, &i__1, alpha, &b[k * b_dim1], ldb);
			i__1 = *n + 1;
			dtrsm_("R", "U", "N", diag, m, &k, &c_b27, &a[k], &i__1, &b[k * b_dim1], ldb);

		    } else {

/*                    SIDE  ='R', N is even, TRANSR = 'N', UPLO = 'U', */
/*                    and TRANS = 'T' */

			i__1 = *n + 1;
			dtrsm_("R", "U", "T", diag, m, &k, alpha, &a[k], &i__1, &b[k * b_dim1], ldb);
			i__1 = *n + 1;
			dgemm_("N", "T", m, &k, &k, &c_b23, &b[k * b_dim1], ldb, a, &i__1, alpha, b, ldb);
			i__1 = *n + 1;
			dtrsm_("R", "L", "N", diag, m, &k, &c_b27, &a[k + 1], &i__1, b, ldb);

		    }

		}

	    } else {

/*              SIDE = 'R', N is even, and TRANSR = 'T' */

		if (lower) {

/*                 SIDE  ='R', N is even, TRANSR = 'T', and UPLO = 'L' */

		    if (notrans) {

/*                    SIDE  ='R', N is even, TRANSR = 'T', UPLO = 'L', */
/*                    and TRANS = 'N' */

			dtrsm_("R", "L", "N", diag, m, &k, alpha, a, &k, &b[k * b_dim1], ldb);
			dgemm_("N", "T", m, &k, &k, &c_b23, &b[k * b_dim1], ldb, &a[(k + 1) * k], &k, alpha, b, ldb);
			dtrsm_("R", "U", "T", diag, m, &k, &c_b27, &a[k], &k, b, ldb);

		    } else {

/*                    SIDE  ='R', N is even, TRANSR = 'T', UPLO = 'L', */
/*                    and TRANS = 'T' */

			dtrsm_("R", "U", "N", diag, m, &k, alpha, &a[k], &k, b, ldb);
			dgemm_("N", "N", m, &k, &k, &c_b23, b, ldb, &a[(k + 1) * k], &k, alpha, &b[k * b_dim1], ldb);
			dtrsm_("R", "L", "T", diag, m, &k, &c_b27, a, &k, &b[k * b_dim1], ldb);

		    }

		} else {

/*                 SIDE  ='R', N is even, TRANSR = 'T', and UPLO = 'U' */

		    if (notrans) {

/*                    SIDE  ='R', N is even, TRANSR = 'T', UPLO = 'U', */
/*                    and TRANS = 'N' */

			dtrsm_("R", "U", "N", diag, m, &k, alpha, &a[(k + 1) * k], &k, b, ldb);
			dgemm_("N", "T", m, &k, &k, &c_b23, b, ldb, a, &k, alpha, &b[k * b_dim1], ldb);
			dtrsm_("R", "L", "T", diag, m, &k, &c_b27, &a[k * k], &k, &b[k * b_dim1], ldb);

		    } else {

/*                    SIDE  ='R', N is even, TRANSR = 'T', UPLO = 'U', */
/*                    and TRANS = 'T' */

			dtrsm_("R", "L", "N", diag, m, &k, alpha, &a[k * k], &k, &b[k * b_dim1], ldb);
			dgemm_("N", "N", m, &k, &k, &c_b23, &b[k * b_dim1], ldb, a, &k, alpha, b, ldb);
			dtrsm_("R", "U", "T", diag, m, &k, &c_b27, &a[(k + 1) * k], &k, b, ldb);

		    }

		}

	    }

	}
    }

    return 0;

/*     End of DTFSM */

} /* dtfsm_ */

int dtftri_(const char *transr, const char *uplo, const char *diag, integer *n, double *a, integer *info)
{
	/* Table of constant values */
	static double c_b13 = -1.;
	static double c_b18 = 1.;

    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer k, n1, n2;
    bool normaltransr, lower, nisodd;


/*  -- LAPACK routine (version 3.2) -- */

/*  -- Contributed by Fred Gustavson of the IBM Watson Research Center -- */
/*  -- November 2008 -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTFTRI computes the inverse of a triangular matrix A stored in RFP */
/*  format. */

/*  This is a Level 3 BLAS version of the algorithm. */

/*  Arguments */
/*  ========= */

/*  TRANSR    (input) CHARACTER */
/*          = 'N':  The Normal TRANSR of RFP A is stored; */
/*          = 'T':  The Transpose TRANSR of RFP A is stored. */

/*  UPLO    (input) CHARACTER */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  DIAG    (input) CHARACTER */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION  array, dimension (0:nt-1); */
/*          nt=N*(N+1)/2. On entry, the triangular factor of a Hermitian */
/*          Positive Definite matrix A in RFP format. RFP format is */
/*          described by TRANSR, UPLO, and N as follows: If TRANSR = 'N' */
/*          then RFP A is (0:N,0:k-1) when N is even; k=N/2. RFP A is */
/*          (0:N-1,0:k) when N is odd; k=N/2. IF TRANSR = 'T' then RFP is */
/*          the transpose of RFP A as defined when */
/*          TRANSR = 'N'. The contents of RFP A are defined by UPLO as */
/*          follows: If UPLO = 'U' the RFP A contains the nt elements of */
/*          upper packed A; If UPLO = 'L' the RFP A contains the nt */
/*          elements of lower packed A. The LDA of RFP A is (N+1)/2 when */
/*          TRANSR = 'T'. When TRANSR is 'N' the LDA is N+1 when N is */
/*          even and N is odd. See the Note below for more details. */

/*          On exit, the (triangular) inverse of the original matrix, in */
/*          the same storage format. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, A(i,i) is exactly zero.  The triangular */
/*               matrix is singular and its inverse can not be computed. */

/*  Notes */
/*  ===== */

/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  even. We give an example where N = 6. */

/*      AP is Upper             AP is Lower */

/*   00 01 02 03 04 05       00 */
/*      11 12 13 14 15       10 11 */
/*         22 23 24 25       20 21 22 */
/*            33 34 35       30 31 32 33 */
/*               44 45       40 41 42 43 44 */
/*                  55       50 51 52 53 54 55 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:5,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(4:6,0:2) consists of */
/*  the transpose of the first three columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(1:6,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:2,0:2) consists of */
/*  the transpose of the last three columns of AP lower. */
/*  This covers the case N even and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        03 04 05                33 43 53 */
/*        13 14 15                00 44 54 */
/*        23 24 25                10 11 55 */
/*        33 34 35                20 21 22 */
/*        00 44 45                30 31 32 */
/*        01 11 55                40 41 42 */
/*        02 12 22                50 51 52 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */


/*           RFP A                   RFP A */

/*     03 13 23 33 00 01 02    33 00 10 20 30 40 50 */
/*     04 14 24 34 44 11 12    43 44 11 21 31 41 51 */
/*     05 15 25 35 45 55 22    53 54 55 22 32 42 52 */


/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  odd. We give an example where N = 5. */

/*     AP is Upper                 AP is Lower */

/*   00 01 02 03 04              00 */
/*      11 12 13 14              10 11 */
/*         22 23 24              20 21 22 */
/*            33 34              30 31 32 33 */
/*               44              40 41 42 43 44 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:4,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(3:4,0:1) consists of */
/*  the transpose of the first two columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(0:4,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:1,1:2) consists of */
/*  the transpose of the last two columns of AP lower. */
/*  This covers the case N odd and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        02 03 04                00 33 43 */
/*        12 13 14                10 11 44 */
/*        22 23 24                20 21 22 */
/*        00 33 34                30 31 32 */
/*        01 11 44                40 41 42 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */

/*           RFP A                   RFP A */

/*     02 12 22 00 01             00 10 20 30 40 50 */
/*     03 13 23 33 11             33 11 21 31 41 51 */
/*     04 14 24 34 44             43 44 22 32 42 52 */

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

    *info = 0;
    normaltransr = lsame_(transr, "N");
    lower = lsame_(uplo, "L");
    if (! normaltransr && ! lsame_(transr, "T")) {
	*info = -1;
    } else if (! lower && ! lsame_(uplo, "U")) {
	*info = -2;
    } else if (! lsame_(diag, "N") && ! lsame_(diag,
	    "U")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTFTRI", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     If N is odd, set NISODD = .TRUE. */
/*     If N is even, set K = N/2 and NISODD = .FALSE. */

    if (*n % 2 == 0) {
	k = *n / 2;
	nisodd = false;
    } else {
	nisodd = true;
    }

/*     Set N1 and N2 depending on LOWER */

    if (lower) {
	n2 = *n / 2;
	n1 = *n - n2;
    } else {
	n1 = *n / 2;
	n2 = *n - n1;
    }


/*     start execution: there are eight cases */

    if (nisodd) {

/*        N is odd */

	if (normaltransr) {

/*           N is odd and TRANSR = 'N' */

	    if (lower) {

/*             SRPA for LOWER, NORMAL and N is odd ( a(0:n-1,0:n1-1) ) */
/*             T1 -> a(0,0), T2 -> a(0,1), S -> a(n1,0) */
/*             T1 -> a(0), T2 -> a(n), S -> a(n1) */

		dtrtri_("L", diag, &n1, a, n, info);
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("R", "L", "N", diag, &n2, &n1, &c_b13, a, n, &a[n1], n);
		dtrtri_("U", diag, &n2, &a[*n], n, info)
			;
		if (*info > 0) {
		    *info += n1;
		}
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("L", "U", "T", diag, &n2, &n1, &c_b18, &a[*n], n, &a[
			n1], n);

	    } else {

/*             SRPA for UPPER, NORMAL and N is odd ( a(0:n-1,0:n2-1) */
/*             T1 -> a(n1+1,0), T2 -> a(n1,0), S -> a(0,0) */
/*             T1 -> a(n2), T2 -> a(n1), S -> a(0) */

		dtrtri_("L", diag, &n1, &a[n2], n, info)
			;
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("L", "L", "T", diag, &n1, &n2, &c_b13, &a[n2], n, a, n);
		dtrtri_("U", diag, &n2, &a[n1], n, info)
			;
		if (*info > 0) {
		    *info += n1;
		}
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("R", "U", "N", diag, &n1, &n2, &c_b18, &a[n1], n, a, n);

	    }

	} else {

/*           N is odd and TRANSR = 'T' */

	    if (lower) {

/*              SRPA for LOWER, TRANSPOSE and N is odd */
/*              T1 -> a(0), T2 -> a(1), S -> a(0+n1*n1) */

		dtrtri_("U", diag, &n1, a, &n1, info);
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("L", "U", "N", diag, &n1, &n2, &c_b13, a, &n1, &a[n1 * n1], &n1);
		dtrtri_("L", diag, &n2, &a[1], &n1, info);
		if (*info > 0) {
		    *info += n1;
		}
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("R", "L", "T", diag, &n1, &n2, &c_b18, &a[1], &n1, &a[n1 * n1], &n1);

	    } else {

/*              SRPA for UPPER, TRANSPOSE and N is odd */
/*              T1 -> a(0+n2*n2), T2 -> a(0+n1*n2), S -> a(0) */

		dtrtri_("U", diag, &n1, &a[n2 * n2], &n2, info);
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("R", "U", "T", diag, &n2, &n1, &c_b13, &a[n2 * n2], &n2, a, &n2);
		dtrtri_("L", diag, &n2, &a[n1 * n2], &n2, info);
		if (*info > 0) {
		    *info += n1;
		}
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("L", "L", "N", diag, &n2, &n1, &c_b18, &a[n1 * n2], &n2, a, &n2);
	    }

	}

    } else {

/*        N is even */

	if (normaltransr) {

/*           N is even and TRANSR = 'N' */

	    if (lower) {

/*              SRPA for LOWER, NORMAL, and N is even ( a(0:n,0:k-1) ) */
/*              T1 -> a(1,0), T2 -> a(0,0), S -> a(k+1,0) */
/*              T1 -> a(1), T2 -> a(0), S -> a(k+1) */

		i__1 = *n + 1;
		dtrtri_("L", diag, &k, &a[1], &i__1, info);
		if (*info > 0) {
		    return 0;
		}
		i__1 = *n + 1;
		i__2 = *n + 1;
		dtrmm_("R", "L", "N", diag, &k, &k, &c_b13, &a[1], &i__1, &a[k + 1], &i__2);
		i__1 = *n + 1;
		dtrtri_("U", diag, &k, a, &i__1, info);
		if (*info > 0) {
		    *info += k;
		}
		if (*info > 0) {
		    return 0;
		}
		i__1 = *n + 1;
		i__2 = *n + 1;
		dtrmm_("L", "U", "T", diag, &k, &k, &c_b18, a, &i__1, &a[k + 1], &i__2)
			;

	    } else {

/*              SRPA for UPPER, NORMAL, and N is even ( a(0:n,0:k-1) ) */
/*              T1 -> a(k+1,0) ,  T2 -> a(k,0),   S -> a(0,0) */
/*              T1 -> a(k+1), T2 -> a(k), S -> a(0) */

		i__1 = *n + 1;
		dtrtri_("L", diag, &k, &a[k + 1], &i__1, info);
		if (*info > 0) {
		    return 0;
		}
		i__1 = *n + 1;
		i__2 = *n + 1;
		dtrmm_("L", "L", "T", diag, &k, &k, &c_b13, &a[k + 1], &i__1, a, &i__2);
		i__1 = *n + 1;
		dtrtri_("U", diag, &k, &a[k], &i__1, info);
		if (*info > 0) {
		    *info += k;
		}
		if (*info > 0) {
		    return 0;
		}
		i__1 = *n + 1;
		i__2 = *n + 1;
		dtrmm_("R", "U", "N", diag, &k, &k, &c_b18, &a[k], &i__1, a, &i__2);
	    }
	} else {

/*           N is even and TRANSR = 'T' */

	    if (lower) {

/*              SRPA for LOWER, TRANSPOSE and N is even (see paper) */
/*              T1 -> B(0,1), T2 -> B(0,0), S -> B(0,k+1) */
/*              T1 -> a(0+k), T2 -> a(0+0), S -> a(0+k*(k+1)); lda=k */

		dtrtri_("U", diag, &k, &a[k], &k, info);
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("L", "U", "N", diag, &k, &k, &c_b13, &a[k], &k, &a[k * (k + 1)], &k);
		dtrtri_("L", diag, &k, a, &k, info);
		if (*info > 0) {
		    *info += k;
		}
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("R", "L", "T", diag, &k, &k, &c_b18, a, &k, &a[k * (k + 1)], &k)
			;
	    } else {

/*              SRPA for UPPER, TRANSPOSE and N is even (see paper) */
/*              T1 -> B(0,k+1),     T2 -> B(0,k),   S -> B(0,0) */
/*              T1 -> a(0+k*(k+1)), T2 -> a(0+k*k), S -> a(0+0)); lda=k */

		dtrtri_("U", diag, &k, &a[k * (k + 1)], &k, info);
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("R", "U", "T", diag, &k, &k, &c_b13, &a[k * (k + 1)], &k, a, &k);
		dtrtri_("L", diag, &k, &a[k * k], &k, info);
		if (*info > 0) {
		    *info += k;
		}
		if (*info > 0) {
		    return 0;
		}
		dtrmm_("L", "L", "N", diag, &k, &k, &c_b18, &a[k * k], &k, a, &k);
	    }
	}
    }

    return 0;

/*     End of DTFTRI */

} /* dtftri_ */

int dtfttp_(const char *transr, const char *uplo, integer *n, double *arf, double *ap, integer *info)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    integer i__, j, k, n1, n2, ij, jp, js, lda, ijp;
    bool normaltransr, lower, nisodd;


/*  -- LAPACK routine (version 3.2)                                    -- */

/*  -- Contributed by Fred Gustavson of the IBM Watson Research Center -- */
/*  -- November 2008                                                   -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTFTTP copies a triangular matrix A from rectangular full packed */
/*  format (TF) to standard packed format (TP). */

/*  Arguments */
/*  ========= */

/*  TRANSR   (input) CHARACTER */
/*          = 'N':  ARF is in Normal format; */
/*          = 'T':  ARF is in Transpose format; */

/*  UPLO    (input) CHARACTER */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A. N >= 0. */

/*  ARF     (input) DOUBLE PRECISION array, dimension ( N*(N+1)/2 ), */
/*          On entry, the upper or lower triangular matrix A stored in */
/*          RFP format. For a further discussion see Notes below. */

/*  AP      (output) DOUBLE PRECISION array, dimension ( N*(N+1)/2 ), */
/*          On exit, the upper or lower triangular matrix A, packed */
/*          columnwise in a linear array. The j-th column of A is stored */
/*          in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Notes */
/*  ===== */

/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  even. We give an example where N = 6. */

/*      AP is Upper             AP is Lower */

/*   00 01 02 03 04 05       00 */
/*      11 12 13 14 15       10 11 */
/*         22 23 24 25       20 21 22 */
/*            33 34 35       30 31 32 33 */
/*               44 45       40 41 42 43 44 */
/*                  55       50 51 52 53 54 55 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:5,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(4:6,0:2) consists of */
/*  the transpose of the first three columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(1:6,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:2,0:2) consists of */
/*  the transpose of the last three columns of AP lower. */
/*  This covers the case N even and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        03 04 05                33 43 53 */
/*        13 14 15                00 44 54 */
/*        23 24 25                10 11 55 */
/*        33 34 35                20 21 22 */
/*        00 44 45                30 31 32 */
/*        01 11 55                40 41 42 */
/*        02 12 22                50 51 52 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */


/*           RFP A                   RFP A */

/*     03 13 23 33 00 01 02    33 00 10 20 30 40 50 */
/*     04 14 24 34 44 11 12    43 44 11 21 31 41 51 */
/*     05 15 25 35 45 55 22    53 54 55 22 32 42 52 */


/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  odd. We give an example where N = 5. */

/*     AP is Upper                 AP is Lower */

/*   00 01 02 03 04              00 */
/*      11 12 13 14              10 11 */
/*         22 23 24              20 21 22 */
/*            33 34              30 31 32 33 */
/*               44              40 41 42 43 44 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:4,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(3:4,0:1) consists of */
/*  the transpose of the first two columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(0:4,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:1,1:2) consists of */
/*  the transpose of the last two columns of AP lower. */
/*  This covers the case N odd and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        02 03 04                00 33 43 */
/*        12 13 14                10 11 44 */
/*        22 23 24                20 21 22 */
/*        00 33 34                30 31 32 */
/*        01 11 44                40 41 42 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */

/*           RFP A                   RFP A */

/*     02 12 22 00 01             00 10 20 30 40 50 */
/*     03 13 23 33 11             33 11 21 31 41 51 */
/*     04 14 24 34 44             43 44 22 32 42 52 */

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

/*     Test the input parameters. */

    *info = 0;
    normaltransr = lsame_(transr, "N");
    lower = lsame_(uplo, "L");
    if (! normaltransr && ! lsame_(transr, "T")) {
	*info = -1;
    } else if (! lower && ! lsame_(uplo, "U")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTFTTP", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (normaltransr) {
	    ap[0] = arf[0];
	} else {
	    ap[0] = arf[0];
	}
	return 0;
    }

/*     Size of array ARF(0:NT-1) */

   // nt = *n * (*n + 1) / 2;

/*     Set N1 and N2 depending on LOWER */

    if (lower) {
	n2 = *n / 2;
	n1 = *n - n2;
    } else {
	n1 = *n / 2;
	n2 = *n - n1;
    }

/*     If N is odd, set NISODD = .TRUE. */
/*     If N is even, set K = N/2 and NISODD = .FALSE. */

/*     set lda of ARF^C; ARF^C is (0:(N+1)/2-1,0:N-noe) */
/*     where noe = 0 if n is even, noe = 1 if n is odd */

    if (*n % 2 == 0) {
	k = *n / 2;
	nisodd = false;
	lda = *n + 1;
    } else {
	nisodd = true;
	lda = *n;
    }

/*     ARF^C has lda rows and n+1-noe cols */

    if (! normaltransr) {
	lda = (*n + 1) / 2;
    }

/*     start execution: there are eight cases */

    if (nisodd) {

/*        N is odd */

	if (normaltransr) {

/*           N is odd and TRANSR = 'N' */

	    if (lower) {

/*             SRPA for LOWER, NORMAL and N is odd ( a(0:n-1,0:n1-1) ) */
/*             T1 -> a(0,0), T2 -> a(0,1), S -> a(n1,0) */
/*             T1 -> a(0), T2 -> a(n), S -> a(n1); lda = n */

		ijp = 0;
		jp = 0;
		i__1 = n2;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = *n - 1;
		    for (i__ = j; i__ <= i__2; ++i__) {
			ij = i__ + jp;
			ap[ijp] = arf[ij];
			++ijp;
		    }
		    jp += lda;
		}
		i__1 = n2 - 1;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__2 = n2;
		    for (j = i__ + 1; j <= i__2; ++j) {
			ij = i__ + j * lda;
			ap[ijp] = arf[ij];
			++ijp;
		    }
		}

	    } else {

/*             SRPA for UPPER, NORMAL and N is odd ( a(0:n-1,0:n2-1) */
/*             T1 -> a(n1+1,0), T2 -> a(n1,0), S -> a(0,0) */
/*             T1 -> a(n2), T2 -> a(n1), S -> a(0) */

		ijp = 0;
		i__1 = n1 - 1;
		for (j = 0; j <= i__1; ++j) {
		    ij = n2 + j;
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			ap[ijp] = arf[ij];
			++ijp;
			ij += lda;
		    }
		}
		js = 0;
		i__1 = *n - 1;
		for (j = n1; j <= i__1; ++j) {
		    ij = js;
		    i__2 = js + j;
		    for (ij = js; ij <= i__2; ++ij) {
			ap[ijp] = arf[ij];
			++ijp;
		    }
		    js += lda;
		}

	    }

	} else {

/*           N is odd and TRANSR = 'T' */

	    if (lower) {

/*              SRPA for LOWER, TRANSPOSE and N is odd */
/*              T1 -> A(0,0) , T2 -> A(1,0) , S -> A(0,n1) */
/*              T1 -> a(0+0) , T2 -> a(1+0) , S -> a(0+n1*n1); lda=n1 */

		ijp = 0;
		i__1 = n2;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__2 = *n * lda - 1;
		    i__3 = lda;
		    for (ij = i__ * (lda + 1); i__3 < 0 ? ij >= i__2 : ij <= i__2; ij += i__3) {
			ap[ijp] = arf[ij];
			++ijp;
		    }
		}
		js = 1;
		i__1 = n2 - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__3 = js + n2 - j - 1;
		    for (ij = js; ij <= i__3; ++ij) {
			ap[ijp] = arf[ij];
			++ijp;
		    }
		    js = js + lda + 1;
		}

	    } else {

/*              SRPA for UPPER, TRANSPOSE and N is odd */
/*              T1 -> A(0,n1+1), T2 -> A(0,n1), S -> A(0,0) */
/*              T1 -> a(n2*n2), T2 -> a(n1*n2), S -> a(0); lda = n2 */

		ijp = 0;
		js = n2 * lda;
		i__1 = n1 - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__3 = js + j;
		    for (ij = js; ij <= i__3; ++ij) {
			ap[ijp] = arf[ij];
			++ijp;
		    }
		    js += lda;
		}
		i__1 = n1;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__3 = i__ + (n1 + i__) * lda;
		    i__2 = lda;
		    for (ij = i__; i__2 < 0 ? ij >= i__3 : ij <= i__3; ij += i__2) {
			ap[ijp] = arf[ij];
			++ijp;
		    }
		}

	    }

	}

    } else {

/*        N is even */

	if (normaltransr) {

/*           N is even and TRANSR = 'N' */

	    if (lower) {

/*              SRPA for LOWER, NORMAL, and N is even ( a(0:n,0:k-1) ) */
/*              T1 -> a(1,0), T2 -> a(0,0), S -> a(k+1,0) */
/*              T1 -> a(1), T2 -> a(0), S -> a(k+1) */

		ijp = 0;
		jp = 0;
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = *n - 1;
		    for (i__ = j; i__ <= i__2; ++i__) {
			ij = i__ + 1 + jp;
			ap[ijp] = arf[ij];
			++ijp;
		    }
		    jp += lda;
		}
		i__1 = k - 1;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__2 = k - 1;
		    for (j = i__; j <= i__2; ++j) {
			ij = i__ + j * lda;
			ap[ijp] = arf[ij];
			++ijp;
		    }
		}

	    } else {

/*              SRPA for UPPER, NORMAL, and N is even ( a(0:n,0:k-1) ) */
/*              T1 -> a(k+1,0) ,  T2 -> a(k,0),   S -> a(0,0) */
/*              T1 -> a(k+1), T2 -> a(k), S -> a(0) */

		ijp = 0;
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    ij = k + 1 + j;
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			ap[ijp] = arf[ij];
			++ijp;
			ij += lda;
		    }
		}
		js = 0;
		i__1 = *n - 1;
		for (j = k; j <= i__1; ++j) {
		    ij = js;
		    i__2 = js + j;
		    for (ij = js; ij <= i__2; ++ij) {
			ap[ijp] = arf[ij];
			++ijp;
		    }
		    js += lda;
		}

	    }

	} else {

/*           N is even and TRANSR = 'T' */

	    if (lower) {

/*              SRPA for LOWER, TRANSPOSE and N is even (see paper) */
/*              T1 -> B(0,1), T2 -> B(0,0), S -> B(0,k+1) */
/*              T1 -> a(0+k), T2 -> a(0+0), S -> a(0+k*(k+1)); lda=k */

		ijp = 0;
		i__1 = k - 1;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__2 = (*n + 1) * lda - 1;
		    i__3 = lda;
		    for (ij = i__ + (i__ + 1) * lda; i__3 < 0 ? ij >= i__2 : ij <= i__2; ij += i__3) {
			ap[ijp] = arf[ij];
			++ijp;
		    }
		}
		js = 0;
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__3 = js + k - j - 1;
		    for (ij = js; ij <= i__3; ++ij) {
			ap[ijp] = arf[ij];
			++ijp;
		    }
		    js = js + lda + 1;
		}

	    } else {

/*              SRPA for UPPER, TRANSPOSE and N is even (see paper) */
/*              T1 -> B(0,k+1),     T2 -> B(0,k),   S -> B(0,0) */
/*              T1 -> a(0+k*(k+1)), T2 -> a(0+k*k), S -> a(0+0)); lda=k */

		ijp = 0;
		js = (k + 1) * lda;
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__3 = js + j;
		    for (ij = js; ij <= i__3; ++ij) {
			ap[ijp] = arf[ij];
			++ijp;
		    }
		    js += lda;
		}
		i__1 = k - 1;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__3 = i__ + (k + i__) * lda;
		    i__2 = lda;
		    for (ij = i__; i__2 < 0 ? ij >= i__3 : ij <= i__3; ij += i__2) {
			ap[ijp] = arf[ij];
			++ijp;
		    }
		}

	    }

	}

    }

    return 0;

/*     End of DTFTTP */

} /* dtfttp_ */

int dtfttr_(const char *transr, const char *uplo, integer *n, double *arf, double *a, integer *lda, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, k, l, n1, n2, ij, nt, nx2, np1x2;
    bool normaltransr, lower, nisodd;


/*  -- LAPACK routine (version 3.2)                                    -- */

/*  -- Contributed by Fred Gustavson of the IBM Watson Research Center -- */
/*  -- November 2008                                                   -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTFTTR copies a triangular matrix A from rectangular full packed */
/*  format (TF) to standard full format (TR). */

/*  Arguments */
/*  ========= */

/*  TRANSR   (input) CHARACTER */
/*          = 'N':  ARF is in Normal format; */
/*          = 'T':  ARF is in Transpose format. */

/*  UPLO    (input) CHARACTER */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrices ARF and A. N >= 0. */

/*  ARF     (input) DOUBLE PRECISION array, dimension (N*(N+1)/2). */
/*          On entry, the upper (if UPLO = 'U') or lower (if UPLO = 'L') */
/*          matrix A in RFP format. See the "Notes" below for more */
/*          details. */

/*  A       (output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On exit, the triangular matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of the array A contains */
/*          the upper triangular matrix, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading N-by-N lower triangular part of the array A contains */
/*          the lower triangular matrix, and the strictly upper */
/*          triangular part of A is not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Notes */
/*  ===== */

/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  even. We give an example where N = 6. */

/*      AP is Upper             AP is Lower */

/*   00 01 02 03 04 05       00 */
/*      11 12 13 14 15       10 11 */
/*         22 23 24 25       20 21 22 */
/*            33 34 35       30 31 32 33 */
/*               44 45       40 41 42 43 44 */
/*                  55       50 51 52 53 54 55 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:5,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(4:6,0:2) consists of */
/*  the transpose of the first three columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(1:6,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:2,0:2) consists of */
/*  the transpose of the last three columns of AP lower. */
/*  This covers the case N even and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        03 04 05                33 43 53 */
/*        13 14 15                00 44 54 */
/*        23 24 25                10 11 55 */
/*        33 34 35                20 21 22 */
/*        00 44 45                30 31 32 */
/*        01 11 55                40 41 42 */
/*        02 12 22                50 51 52 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */


/*           RFP A                   RFP A */

/*     03 13 23 33 00 01 02    33 00 10 20 30 40 50 */
/*     04 14 24 34 44 11 12    43 44 11 21 31 41 51 */
/*     05 15 25 35 45 55 22    53 54 55 22 32 42 52 */


/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  odd. We give an example where N = 5. */

/*     AP is Upper                 AP is Lower */

/*   00 01 02 03 04              00 */
/*      11 12 13 14              10 11 */
/*         22 23 24              20 21 22 */
/*            33 34              30 31 32 33 */
/*               44              40 41 42 43 44 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:4,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(3:4,0:1) consists of */
/*  the transpose of the first two columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(0:4,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:1,1:2) consists of */
/*  the transpose of the last two columns of AP lower. */
/*  This covers the case N odd and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        02 03 04                00 33 43 */
/*        12 13 14                10 11 44 */
/*        22 23 24                20 21 22 */
/*        00 33 34                30 31 32 */
/*        01 11 44                40 41 42 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */

/*           RFP A                   RFP A */

/*     02 12 22 00 01             00 10 20 30 40 50 */
/*     03 13 23 33 11             33 11 21 31 41 51 */
/*     04 14 24 34 44             43 44 22 32 42 52 */

/*  Reference */
/*  ========= */

/*  ===================================================================== */

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
    a_dim1 = *lda - 1 - 0 + 1;
    a_offset = 0 + a_dim1 * 0;
    a -= a_offset;

    /* Function Body */
    *info = 0;
    normaltransr = lsame_(transr, "N");
    lower = lsame_(uplo, "L");
    if (! normaltransr && ! lsame_(transr, "T")) {
	*info = -1;
    } else if (! lower && ! lsame_(uplo, "U")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTFTTR", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 1) {
	if (*n == 1) {
	    a[0] = arf[0];
	}
	return 0;
    }

/*     Size of array ARF(0:nt-1) */

    nt = *n * (*n + 1) / 2;

/*     set N1 and N2 depending on LOWER: for N even N1=N2=K */

    if (lower) {
	n2 = *n / 2;
	n1 = *n - n2;
    } else {
	n1 = *n / 2;
	n2 = *n - n1;
    }

/*     If N is odd, set NISODD = .TRUE., LDA=N+1 and A is (N+1)--by--K2. */
/*     If N is even, set K = N/2 and NISODD = .FALSE., LDA=N and A is */
/*     N--by--(N+1)/2. */

    if (*n % 2 == 0) {
	k = *n / 2;
	nisodd = false;
	if (! lower) {
	    np1x2 = *n + *n + 2;
	}
    } else {
	nisodd = true;
	if (! lower) {
	    nx2 = *n + *n;
	}
    }

    if (nisodd) {

/*        N is odd */

	if (normaltransr) {

/*           N is odd and TRANSR = 'N' */

	    if (lower) {

/*              N is odd, TRANSR = 'N', and UPLO = 'L' */

		ij = 0;
		i__1 = n2;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = n2 + j;
		    for (i__ = n1; i__ <= i__2; ++i__) {
			a[n2 + j + i__ * a_dim1] = arf[ij];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (i__ = j; i__ <= i__2; ++i__) {
			a[i__ + j * a_dim1] = arf[ij];
			++ij;
		    }
		}

	    } else {

/*              N is odd, TRANSR = 'N', and UPLO = 'U' */

		ij = nt - *n;
		i__1 = n1;
		for (j = *n - 1; j >= i__1; --j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			a[i__ + j * a_dim1] = arf[ij];
			++ij;
		    }
		    i__2 = n1 - 1;
		    for (l = j - n1; l <= i__2; ++l) {
			a[j - n1 + l * a_dim1] = arf[ij];
			++ij;
		    }
		    ij -= nx2;
		}

	    }

	} else {

/*           N is odd and TRANSR = 'T' */

	    if (lower) {

/*              N is odd, TRANSR = 'T', and UPLO = 'L' */

		ij = 0;
		i__1 = n2 - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			a[j + i__ * a_dim1] = arf[ij];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (i__ = n1 + j; i__ <= i__2; ++i__) {
			a[i__ + (n1 + j) * a_dim1] = arf[ij];
			++ij;
		    }
		}
		i__1 = *n - 1;
		for (j = n2; j <= i__1; ++j) {
		    i__2 = n1 - 1;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			a[j + i__ * a_dim1] = arf[ij];
			++ij;
		    }
		}

	    } else {

/*              N is odd, TRANSR = 'T', and UPLO = 'U' */

		ij = 0;
		i__1 = n1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = *n - 1;
		    for (i__ = n1; i__ <= i__2; ++i__) {
			a[j + i__ * a_dim1] = arf[ij];
			++ij;
		    }
		}
		i__1 = n1 - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			a[i__ + j * a_dim1] = arf[ij];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (l = n2 + j; l <= i__2; ++l) {
			a[n2 + j + l * a_dim1] = arf[ij];
			++ij;
		    }
		}

	    }

	}

    } else {

/*        N is even */

	if (normaltransr) {

/*           N is even and TRANSR = 'N' */

	    if (lower) {

/*              N is even, TRANSR = 'N', and UPLO = 'L' */

		ij = 0;
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = k + j;
		    for (i__ = k; i__ <= i__2; ++i__) {
			a[k + j + i__ * a_dim1] = arf[ij];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (i__ = j; i__ <= i__2; ++i__) {
			a[i__ + j * a_dim1] = arf[ij];
			++ij;
		    }
		}

	    } else {

/*              N is even, TRANSR = 'N', and UPLO = 'U' */

		ij = nt - *n - 1;
		i__1 = k;
		for (j = *n - 1; j >= i__1; --j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			a[i__ + j * a_dim1] = arf[ij];
			++ij;
		    }
		    i__2 = k - 1;
		    for (l = j - k; l <= i__2; ++l) {
			a[j - k + l * a_dim1] = arf[ij];
			++ij;
		    }
		    ij -= np1x2;
		}

	    }

	} else {

/*           N is even and TRANSR = 'T' */

	    if (lower) {

/*              N is even, TRANSR = 'T', and UPLO = 'L' */

		ij = 0;
		j = k;
		i__1 = *n - 1;
		for (i__ = k; i__ <= i__1; ++i__) {
		    a[i__ + j * a_dim1] = arf[ij];
		    ++ij;
		}
		i__1 = k - 2;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			a[j + i__ * a_dim1] = arf[ij];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (i__ = k + 1 + j; i__ <= i__2; ++i__) {
			a[i__ + (k + 1 + j) * a_dim1] = arf[ij];
			++ij;
		    }
		}
		i__1 = *n - 1;
		for (j = k - 1; j <= i__1; ++j) {
		    i__2 = k - 1;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			a[j + i__ * a_dim1] = arf[ij];
			++ij;
		    }
		}

	    } else {

/*              N is even, TRANSR = 'T', and UPLO = 'U' */

		ij = 0;
		i__1 = k;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = *n - 1;
		    for (i__ = k; i__ <= i__2; ++i__) {
			a[j + i__ * a_dim1] = arf[ij];
			++ij;
		    }
		}
		i__1 = k - 2;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			a[i__ + j * a_dim1] = arf[ij];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (l = k + 1 + j; l <= i__2; ++l) {
			a[k + 1 + j + l * a_dim1] = arf[ij];
			++ij;
		    }
		}
/*              Note that here, on exit of the loop, J = K-1 */
		i__1 = j;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    a[i__ + j * a_dim1] = arf[ij];
		    ++ij;
		}

	    }

	}

    }

    return 0;

/*     End of DTFTTR */

} /* dtfttr_ */

/* Subroutine */ int dtgevc_(const char *side, const char *howmny, bool *select,
	integer *n, double *s, integer *lds, double *p, integer *ldp,
	double *vl, integer *ldvl, double *vr, integer *ldvr, integer
	*mm, integer *m, double *work, integer *info)
{
	/* Table of constant values */
	static bool c_true = true;
	static integer c__2 = 2;
	static double c_b34 = 1.;
	static integer c__1 = 1;
	static double c_b36 = 0.;
	static bool c_false = false;

    /* System generated locals */
    integer p_dim1, p_offset, s_dim1, s_offset, vl_dim1, vl_offset, vr_dim1,
	    vr_offset, i__1, i__2, i__3, i__4, i__5;
    double d__1, d__2, d__3, d__4, d__5, d__6;

    /* Local variables */
    integer i__, j, ja, jc, je, na, im, jr, jw, nw;
    double big;
    bool lsa, lsb;
    double ulp, sum[4]	/* was [2][2] */;
    integer ibeg, ieig, iend;
    double dmin__, temp, xmax, sump[4]	/* was [2][2] */, sums[4]
	    /* was [2][2] */;
    double cim2a, cim2b, cre2a, cre2b, temp2, bdiag[2], acoef, scale;
    bool ilall;
    integer iside;
    double sbeta;
    bool il2by2;
    integer iinfo;
    double small;
    bool compl_x; // djmw changed variable from "compl" to compl_x because the c++ compiler protested.
    double anorm, bnorm;
    bool compr;
    double temp2i;
    double temp2r;
    bool ilabad, ilbbad;
    double acoefa, bcoefa, cimaga, cimagb;
    bool ilback;
    double bcoefi, ascale, bscale, creala, crealb;
    double bcoefr, salfar, safmin;
    double xscale, bignum;
    bool ilcomp, ilcplx;
    integer ihwmny;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */


/*  Purpose */
/*  ======= */

/*  DTGEVC computes some or all of the right and/or left eigenvectors of */
/*  a pair of real matrices (S,P), where S is a quasi-triangular matrix */
/*  and P is upper triangular.  Matrix pairs of this type are produced by */
/*  the generalized Schur factorization of a matrix pair (A,B): */

/*     A = Q*S*Z**T,  B = Q*P*Z**T */

/*  as computed by DGGHRD + DHGEQZ. */

/*  The right eigenvector x and the left eigenvector y of (S,P) */
/*  corresponding to an eigenvalue w are defined by: */

/*     S*x = w*P*x,  (y**H)*S = w*(y**H)*P, */

/*  where y**H denotes the conjugate tranpose of y. */
/*  The eigenvalues are not input to this routine, but are computed */
/*  directly from the diagonal blocks of S and P. */

/*  This routine returns the matrices X and/or Y of right and left */
/*  eigenvectors of (S,P), or the products Z*X and/or Q*Y, */
/*  where Z and Q are input matrices. */
/*  If Q and Z are the orthogonal factors from the generalized Schur */
/*  factorization of a matrix pair (A,B), then Z*X and Q*Y */
/*  are the matrices of right and left eigenvectors of (A,B). */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'R': compute right eigenvectors only; */
/*          = 'L': compute left eigenvectors only; */
/*          = 'B': compute both right and left eigenvectors. */

/*  HOWMNY  (input) CHARACTER*1 */
/*          = 'A': compute all right and/or left eigenvectors; */
/*          = 'B': compute all right and/or left eigenvectors, */
/*                 backtransformed by the matrices in VR and/or VL; */
/*          = 'S': compute selected right and/or left eigenvectors, */
/*                 specified by the bool array SELECT. */

/*  SELECT  (input) LOGICAL array, dimension (N) */
/*          If HOWMNY='S', SELECT specifies the eigenvectors to be */
/*          computed.  If w(j) is a real eigenvalue, the corresponding */
/*          real eigenvector is computed if SELECT(j) is .TRUE.. */
/*          If w(j) and w(j+1) are the real and imaginary parts of a */
/*          complex eigenvalue, the corresponding complex eigenvector */
/*          is computed if either SELECT(j) or SELECT(j+1) is .TRUE., */
/*          and on exit SELECT(j) is set to .TRUE. and SELECT(j+1) is */
/*          set to .FALSE.. */
/*          Not referenced if HOWMNY = 'A' or 'B'. */

/*  N       (input) INTEGER */
/*          The order of the matrices S and P.  N >= 0. */

/*  S       (input) DOUBLE PRECISION array, dimension (LDS,N) */
/*          The upper quasi-triangular matrix S from a generalized Schur */
/*          factorization, as computed by DHGEQZ. */

/*  LDS     (input) INTEGER */
/*          The leading dimension of array S.  LDS >= max(1,N). */

/*  P       (input) DOUBLE PRECISION array, dimension (LDP,N) */
/*          The upper triangular matrix P from a generalized Schur */
/*          factorization, as computed by DHGEQZ. */
/*          2-by-2 diagonal blocks of P corresponding to 2-by-2 blocks */
/*          of S must be in positive diagonal form. */

/*  LDP     (input) INTEGER */
/*          The leading dimension of array P.  LDP >= max(1,N). */

/*  VL      (input/output) DOUBLE PRECISION array, dimension (LDVL,MM) */
/*          On entry, if SIDE = 'L' or 'B' and HOWMNY = 'B', VL must */
/*          contain an N-by-N matrix Q (usually the orthogonal matrix Q */
/*          of left Schur vectors returned by DHGEQZ). */
/*          On exit, if SIDE = 'L' or 'B', VL contains: */
/*          if HOWMNY = 'A', the matrix Y of left eigenvectors of (S,P); */
/*          if HOWMNY = 'B', the matrix Q*Y; */
/*          if HOWMNY = 'S', the left eigenvectors of (S,P) specified by */
/*                      SELECT, stored consecutively in the columns of */
/*                      VL, in the same order as their eigenvalues. */

/*          A complex eigenvector corresponding to a complex eigenvalue */
/*          is stored in two consecutive columns, the first holding the */
/*          real part, and the second the imaginary part. */

/*          Not referenced if SIDE = 'R'. */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of array VL.  LDVL >= 1, and if */
/*          SIDE = 'L' or 'B', LDVL >= N. */

/*  VR      (input/output) DOUBLE PRECISION array, dimension (LDVR,MM) */
/*          On entry, if SIDE = 'R' or 'B' and HOWMNY = 'B', VR must */
/*          contain an N-by-N matrix Z (usually the orthogonal matrix Z */
/*          of right Schur vectors returned by DHGEQZ). */

/*          On exit, if SIDE = 'R' or 'B', VR contains: */
/*          if HOWMNY = 'A', the matrix X of right eigenvectors of (S,P); */
/*          if HOWMNY = 'B' or 'b', the matrix Z*X; */
/*          if HOWMNY = 'S' or 's', the right eigenvectors of (S,P) */
/*                      specified by SELECT, stored consecutively in the */
/*                      columns of VR, in the same order as their */
/*                      eigenvalues. */

/*          A complex eigenvector corresponding to a complex eigenvalue */
/*          is stored in two consecutive columns, the first holding the */
/*          real part and the second the imaginary part. */

/*          Not referenced if SIDE = 'L'. */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the array VR.  LDVR >= 1, and if */
/*          SIDE = 'R' or 'B', LDVR >= N. */

/*  MM      (input) INTEGER */
/*          The number of columns in the arrays VL and/or VR. MM >= M. */

/*  M       (output) INTEGER */
/*          The number of columns in the arrays VL and/or VR actually */
/*          used to store the eigenvectors.  If HOWMNY = 'A' or 'B', M */
/*          is set to N.  Each selected real eigenvector occupies one */
/*          column and each selected complex eigenvector occupies two */
/*          columns. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (6*N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  the 2-by-2 block (INFO:INFO+1) does not have a complex */
/*                eigenvalue. */

/*  Further Details */
/*  =============== */

/*  Allocation of workspace: */
/*  ---------- -- --------- */

/*     WORK( j ) = 1-norm of j-th column of A, above the diagonal */
/*     WORK( N+j ) = 1-norm of j-th column of B, above the diagonal */
/*     WORK( 2*N+1:3*N ) = real part of eigenvector */
/*     WORK( 3*N+1:4*N ) = imaginary part of eigenvector */
/*     WORK( 4*N+1:5*N ) = real part of back-transformed eigenvector */
/*     WORK( 5*N+1:6*N ) = imaginary part of back-transformed eigenvector */

/*  Rowwise vs. columnwise solution methods: */
/*  ------- --  ---------- -------- ------- */

/*  Finding a generalized eigenvector consists basically of solving the */
/*  singular triangular system */

/*   (A - w B) x = 0     (for right) or:   (A - w B)**H y = 0  (for left) */

/*  Consider finding the i-th right eigenvector (assume all eigenvalues */
/*  are real). The equation to be solved is: */
/*       n                   i */
/*  0 = sum  C(j,k) v(k)  = sum  C(j,k) v(k)     for j = i,. . .,1 */
/*      k=j                 k=j */

/*  where  C = (A - w B)  (The components v(i+1:n) are 0.) */

/*  The "rowwise" method is: */

/*  (1)  v(i) := 1 */
/*  for j = i-1,. . .,1: */
/*                          i */
/*      (2) compute  s = - sum C(j,k) v(k)   and */
/*                        k=j+1 */

/*      (3) v(j) := s / C(j,j) */

/*  Step 2 is sometimes called the "dot product" step, since it is an */
/*  inner product between the j-th row and the portion of the eigenvector */
/*  that has been computed so far. */

/*  The "columnwise" method consists basically in doing the sums */
/*  for all the rows in parallel.  As each v(j) is computed, the */
/*  contribution of v(j) times the j-th column of C is added to the */
/*  partial sums.  Since FORTRAN arrays are stored columnwise, this has */
/*  the advantage that at each step, the elements of C that are accessed */
/*  are adjacent to one another, whereas with the rowwise method, the */
/*  elements accessed at a step are spaced LDS (and LDP) words apart. */

/*  When finding left eigenvectors, the matrix in question is the */
/*  transpose of the one in storage, so the rowwise method then */
/*  actually accesses columns of A and B at each step, and so is the */
/*  preferred method. */

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

/*     Decode and Test the input parameters */

    /* Parameter adjustments */
    --select;
    s_dim1 = *lds;
    s_offset = 1 + s_dim1;
    s -= s_offset;
    p_dim1 = *ldp;
    p_offset = 1 + p_dim1;
    p -= p_offset;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    --work;

    /* Function Body */
    if (lsame_(howmny, "A")) {
	ihwmny = 1;
	ilall = true;
	ilback = false;
    } else if (lsame_(howmny, "S")) {
	ihwmny = 2;
	ilall = false;
	ilback = false;
    } else if (lsame_(howmny, "B")) {
	ihwmny = 3;
	ilall = true;
	ilback = true;
    } else {
	ihwmny = -1;
	ilall = true;
    }

    if (lsame_(side, "R")) {
	iside = 1;
	compl_x = false;
	compr = true;
    } else if (lsame_(side, "L")) {
	iside = 2;
	compl_x = true;
	compr = false;
    } else if (lsame_(side, "B")) {
	iside = 3;
	compl_x = true;
	compr = true;
    } else {
	iside = -1;
    }

    *info = 0;
    if (iside < 0) {
	*info = -1;
    } else if (ihwmny < 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lds < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldp < std::max(1_integer,*n)) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTGEVC", &i__1);
	return 0;
    }

/*     Count the number of eigenvectors to be computed */

    if (! ilall) {
	im = 0;
	ilcplx = false;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (ilcplx) {
		ilcplx = false;
		goto L10;
	    }
	    if (j < *n) {
		if (s[j + 1 + j * s_dim1] != 0.) {
		    ilcplx = true;
		}
	    }
	    if (ilcplx) {
		if (select[j] || select[j + 1]) {
		    im += 2;
		}
	    } else {
		if (select[j]) {
		    ++im;
		}
	    }
L10:
	    ;
	}
    } else {
	im = *n;
    }

/*     Check 2-by-2 diagonal blocks of A, B */

    ilabad = false;
    ilbbad = false;
    i__1 = *n - 1;
    for (j = 1; j <= i__1; ++j) {
	if (s[j + 1 + j * s_dim1] != 0.) {
	    if (p[j + j * p_dim1] == 0. || p[j + 1 + (j + 1) * p_dim1] == 0.
		    || p[j + (j + 1) * p_dim1] != 0.) {
		ilbbad = true;
	    }
	    if (j < *n - 1) {
		if (s[j + 2 + (j + 1) * s_dim1] != 0.) {
		    ilabad = true;
		}
	    }
	}
/* L20: */
    }

    if (ilabad) {
	*info = -5;
    } else if (ilbbad) {
	*info = -7;
    } else if (compl_x && *ldvl < *n || *ldvl < 1) {
	*info = -10;
    } else if (compr && *ldvr < *n || *ldvr < 1) {
	*info = -12;
    } else if (*mm < im) {
	*info = -13;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTGEVC", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *m = im;
    if (*n == 0) {
	return 0;
    }

/*     Machine Constants */

    safmin = dlamch_("Safe minimum");
    big = 1. / safmin;
    dlabad_(&safmin, &big);
    ulp = dlamch_("Epsilon") * dlamch_("Base");
    small = safmin * *n / ulp;
    big = 1. / small;
    bignum = 1. / (safmin * *n);

/*     Compute the 1-norm of each column of the strictly upper triangular */
/*     part (i.e., excluding all elements belonging to the diagonal */
/*     blocks) of A and B to check for possible overflow in the */
/*     triangular solver. */

    anorm = (d__1 = s[s_dim1 + 1], abs(d__1));
    if (*n > 1) {
	anorm += (d__1 = s[s_dim1 + 2], abs(d__1));
    }
    bnorm = (d__1 = p[p_dim1 + 1], abs(d__1));
    work[1] = 0.;
    work[*n + 1] = 0.;

    i__1 = *n;
    for (j = 2; j <= i__1; ++j) {
	temp = 0.;
	temp2 = 0.;
	if (s[j + (j - 1) * s_dim1] == 0.) {
	    iend = j - 1;
	} else {
	    iend = j - 2;
	}
	i__2 = iend;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    temp += (d__1 = s[i__ + j * s_dim1], abs(d__1));
	    temp2 += (d__1 = p[i__ + j * p_dim1], abs(d__1));
/* L30: */
	}
	work[j] = temp;
	work[*n + j] = temp2;
/* Computing MIN */
	i__3 = j + 1;
	i__2 = std::min(i__3,*n);
	for (i__ = iend + 1; i__ <= i__2; ++i__) {
	    temp += (d__1 = s[i__ + j * s_dim1], abs(d__1));
	    temp2 += (d__1 = p[i__ + j * p_dim1], abs(d__1));
/* L40: */
	}
	anorm = std::max(anorm,temp);
	bnorm = std::max(bnorm,temp2);
/* L50: */
    }
    ascale = 1. / std::max(anorm,safmin);
    bscale = 1. / std::max(bnorm,safmin);

/*     Left eigenvectors */

    if (compl_x) {
	ieig = 0;

/*        Main loop over eigenvalues */

	ilcplx = false;
	i__1 = *n;
	for (je = 1; je <= i__1; ++je) {

/*           Skip this iteration if (a) HOWMNY='S' and SELECT=.FALSE., or */
/*           (b) this would be the second of a complex pair. */
/*           Check for complex eigenvalue, so as to be sure of which */
/*           entry(-ies) of SELECT to look at. */

	    if (ilcplx) {
		ilcplx = false;
		goto L220;
	    }
	    nw = 1;
	    if (je < *n) {
		if (s[je + 1 + je * s_dim1] != 0.) {
		    ilcplx = true;
		    nw = 2;
		}
	    }
	    if (ilall) {
		ilcomp = true;
	    } else if (ilcplx) {
		ilcomp = select[je] || select[je + 1];
	    } else {
		ilcomp = select[je];
	    }
	    if (! ilcomp) {
		goto L220;
	    }

/*           Decide if (a) singular pencil, (b) real eigenvalue, or */
/*           (c) complex eigenvalue. */

	    if (! ilcplx) {
		if ((d__1 = s[je + je * s_dim1], abs(d__1)) <= safmin && (
			d__2 = p[je + je * p_dim1], abs(d__2)) <= safmin) {

/*                 Singular matrix pencil -- return unit eigenvector */

		    ++ieig;
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vl[jr + ieig * vl_dim1] = 0.;
/* L60: */
		    }
		    vl[ieig + ieig * vl_dim1] = 1.;
		    goto L220;
		}
	    }

/*           Clear vector */

	    i__2 = nw * *n;
	    for (jr = 1; jr <= i__2; ++jr) {
		work[(*n << 1) + jr] = 0.;
/* L70: */
	    }
/*                                                 T */
/*           Compute coefficients in  ( a A - b B )  y = 0 */
/*              a  is  ACOEF */
/*              b  is  BCOEFR + i*BCOEFI */

	    if (! ilcplx) {

/*              Real eigenvalue */

/* Computing MAX */
		d__3 = (d__1 = s[je + je * s_dim1], abs(d__1)) * ascale, d__4
			= (d__2 = p[je + je * p_dim1], abs(d__2)) * bscale,
			d__3 = std::max(d__3,d__4);
		temp = 1. / std::max(d__3,safmin);
		salfar = temp * s[je + je * s_dim1] * ascale;
		sbeta = temp * p[je + je * p_dim1] * bscale;
		acoef = sbeta * ascale;
		bcoefr = salfar * bscale;
		bcoefi = 0.;

/*              Scale to avoid underflow */

		scale = 1.;
		lsa = abs(sbeta) >= safmin && abs(acoef) < small;
		lsb = abs(salfar) >= safmin && abs(bcoefr) < small;
		if (lsa) {
		    scale = small / abs(sbeta) * std::min(anorm,big);
		}
		if (lsb) {
/* Computing MAX */
		    d__1 = scale, d__2 = small / abs(salfar) * std::min(bnorm,big);
		    scale = std::max(d__1,d__2);
		}
		if (lsa || lsb) {
/* Computing MIN */
/* Computing MAX */
		    d__3 = 1., d__4 = abs(acoef), d__3 = std::max(d__3,d__4), d__4
			    = abs(bcoefr);
		    d__1 = scale, d__2 = 1. / (safmin * std::max(d__3,d__4));
		    scale = std::min(d__1,d__2);
		    if (lsa) {
			acoef = ascale * (scale * sbeta);
		    } else {
			acoef = scale * acoef;
		    }
		    if (lsb) {
			bcoefr = bscale * (scale * salfar);
		    } else {
			bcoefr = scale * bcoefr;
		    }
		}
		acoefa = abs(acoef);
		bcoefa = abs(bcoefr);

/*              First component is 1 */

		work[(*n << 1) + je] = 1.;
		xmax = 1.;
	    } else {

/*              Complex eigenvalue */

		d__1 = safmin * 100.;
		dlag2_(&s[je + je * s_dim1], lds, &p[je + je * p_dim1], ldp, &
			d__1, &acoef, &temp, &bcoefr, &temp2, &bcoefi);
		bcoefi = -bcoefi;
		if (bcoefi == 0.) {
		    *info = je;
		    return 0;
		}

/*              Scale to avoid over/underflow */

		acoefa = abs(acoef);
		bcoefa = abs(bcoefr) + abs(bcoefi);
		scale = 1.;
		if (acoefa * ulp < safmin && acoefa >= safmin) {
		    scale = safmin / ulp / acoefa;
		}
		if (bcoefa * ulp < safmin && bcoefa >= safmin) {
/* Computing MAX */
		    d__1 = scale, d__2 = safmin / ulp / bcoefa;
		    scale = std::max(d__1,d__2);
		}
		if (safmin * acoefa > ascale) {
		    scale = ascale / (safmin * acoefa);
		}
		if (safmin * bcoefa > bscale) {
/* Computing MIN */
		    d__1 = scale, d__2 = bscale / (safmin * bcoefa);
		    scale = std::min(d__1,d__2);
		}
		if (scale != 1.) {
		    acoef = scale * acoef;
		    acoefa = abs(acoef);
		    bcoefr = scale * bcoefr;
		    bcoefi = scale * bcoefi;
		    bcoefa = abs(bcoefr) + abs(bcoefi);
		}

/*              Compute first two components of eigenvector */

		temp = acoef * s[je + 1 + je * s_dim1];
		temp2r = acoef * s[je + je * s_dim1] - bcoefr * p[je + je *
			p_dim1];
		temp2i = -bcoefi * p[je + je * p_dim1];
		if (abs(temp) > abs(temp2r) + abs(temp2i)) {
		    work[(*n << 1) + je] = 1.;
		    work[*n * 3 + je] = 0.;
		    work[(*n << 1) + je + 1] = -temp2r / temp;
		    work[*n * 3 + je + 1] = -temp2i / temp;
		} else {
		    work[(*n << 1) + je + 1] = 1.;
		    work[*n * 3 + je + 1] = 0.;
		    temp = acoef * s[je + (je + 1) * s_dim1];
		    work[(*n << 1) + je] = (bcoefr * p[je + 1 + (je + 1) *
			    p_dim1] - acoef * s[je + 1 + (je + 1) * s_dim1]) /
			     temp;
		    work[*n * 3 + je] = bcoefi * p[je + 1 + (je + 1) * p_dim1]
			     / temp;
		}
/* Computing MAX */
		d__5 = (d__1 = work[(*n << 1) + je], abs(d__1)) + (d__2 =
			work[*n * 3 + je], abs(d__2)), d__6 = (d__3 = work[(*
			n << 1) + je + 1], abs(d__3)) + (d__4 = work[*n * 3 +
			je + 1], abs(d__4));
		xmax = std::max(d__5,d__6);
	    }

/* Computing MAX */
	    d__1 = ulp * acoefa * anorm, d__2 = ulp * bcoefa * bnorm, d__1 =
		    std::max(d__1,d__2);
	    dmin__ = std::max(d__1,safmin);

/*                                           T */
/*           Triangular solve of  (a A - b B)  y = 0 */

/*                                   T */
/*           (rowwise in  (a A - b B) , or columnwise in (a A - b B) ) */

	    il2by2 = false;

	    i__2 = *n;
	    for (j = je + nw; j <= i__2; ++j) {
		if (il2by2) {
		    il2by2 = false;
		    goto L160;
		}

		na = 1;
		bdiag[0] = p[j + j * p_dim1];
		if (j < *n) {
		    if (s[j + 1 + j * s_dim1] != 0.) {
			il2by2 = true;
			bdiag[1] = p[j + 1 + (j + 1) * p_dim1];
			na = 2;
		    }
		}

/*              Check whether scaling is necessary for dot products */

		xscale = 1. / std::max(1.,xmax);
/* Computing MAX */
		d__1 = work[j], d__2 = work[*n + j], d__1 = std::max(d__1,d__2),
			d__2 = acoefa * work[j] + bcoefa * work[*n + j];
		temp = std::max(d__1,d__2);
		if (il2by2) {
/* Computing MAX */
		    d__1 = temp, d__2 = work[j + 1], d__1 = std::max(d__1,d__2),
			    d__2 = work[*n + j + 1], d__1 = std::max(d__1,d__2),
			    d__2 = acoefa * work[j + 1] + bcoefa * work[*n +
			    j + 1];
		    temp = std::max(d__1,d__2);
		}
		if (temp > bignum * xscale) {
		    i__3 = nw - 1;
		    for (jw = 0; jw <= i__3; ++jw) {
			i__4 = j - 1;
			for (jr = je; jr <= i__4; ++jr) {
			    work[(jw + 2) * *n + jr] = xscale * work[(jw + 2)
				    * *n + jr];
/* L80: */
			}
/* L90: */
		    }
		    xmax *= xscale;
		}

/*              Compute dot products */

/*                    j-1 */
/*              SUM = sum  conjg( a*S(k,j) - b*P(k,j) )*x(k) */
/*                    k=je */

/*              To reduce the op count, this is done as */

/*              _        j-1                  _        j-1 */
/*              a*conjg( sum  S(k,j)*x(k) ) - b*conjg( sum  P(k,j)*x(k) ) */
/*                       k=je                          k=je */

/*              which may cause underflow problems if A or B are close */
/*              to underflow.  (E.g., less than SMALL.) */


/*              A series of compiler directives to defeat vectorization */
/*              for the next loop */

/* $PL$ CMCHAR=' ' */
/* DIR$          NEXTSCALAR */
/* $DIR          SCALAR */
/* DIR$          NEXT SCALAR */
/* VD$L          NOVECTOR */
/* DEC$          NOVECTOR */
/* VD$           NOVECTOR */
/* VDIR          NOVECTOR */
/* VOCL          LOOP,SCALAR */
/* IBM           PREFER SCALAR */
/* $PL$ CMCHAR='*' */

		i__3 = nw;
		for (jw = 1; jw <= i__3; ++jw) {

/* $PL$ CMCHAR=' ' */
/* DIR$             NEXTSCALAR */
/* $DIR             SCALAR */
/* DIR$             NEXT SCALAR */
/* VD$L             NOVECTOR */
/* DEC$             NOVECTOR */
/* VD$              NOVECTOR */
/* VDIR             NOVECTOR */
/* VOCL             LOOP,SCALAR */
/* IBM              PREFER SCALAR */
/* $PL$ CMCHAR='*' */

		    i__4 = na;
		    for (ja = 1; ja <= i__4; ++ja) {
			sums[ja + (jw << 1) - 3] = 0.;
			sump[ja + (jw << 1) - 3] = 0.;

			i__5 = j - 1;
			for (jr = je; jr <= i__5; ++jr) {
			    sums[ja + (jw << 1) - 3] += s[jr + (j + ja - 1) *
				    s_dim1] * work[(jw + 1) * *n + jr];
			    sump[ja + (jw << 1) - 3] += p[jr + (j + ja - 1) *
				    p_dim1] * work[(jw + 1) * *n + jr];
/* L100: */
			}
/* L110: */
		    }
/* L120: */
		}

/* $PL$ CMCHAR=' ' */
/* DIR$          NEXTSCALAR */
/* $DIR          SCALAR */
/* DIR$          NEXT SCALAR */
/* VD$L          NOVECTOR */
/* DEC$          NOVECTOR */
/* VD$           NOVECTOR */
/* VDIR          NOVECTOR */
/* VOCL          LOOP,SCALAR */
/* IBM           PREFER SCALAR */
/* $PL$ CMCHAR='*' */

		i__3 = na;
		for (ja = 1; ja <= i__3; ++ja) {
		    if (ilcplx) {
			sum[ja - 1] = -acoef * sums[ja - 1] + bcoefr * sump[
				ja - 1] - bcoefi * sump[ja + 1];
			sum[ja + 1] = -acoef * sums[ja + 1] + bcoefr * sump[
				ja + 1] + bcoefi * sump[ja - 1];
		    } else {
			sum[ja - 1] = -acoef * sums[ja - 1] + bcoefr * sump[
				ja - 1];
		    }
/* L130: */
		}

/*                                  T */
/*              Solve  ( a A - b B )  y = SUM(,) */
/*              with scaling and perturbation of the denominator */

		dlaln2_(&c_true, &na, &nw, &dmin__, &acoef, &s[j + j * s_dim1]
, lds, bdiag, &bdiag[1], sum, &c__2, &bcoefr, &bcoefi,
			 &work[(*n << 1) + j], n, &scale, &temp, &iinfo);
		if (scale < 1.) {
		    i__3 = nw - 1;
		    for (jw = 0; jw <= i__3; ++jw) {
			i__4 = j - 1;
			for (jr = je; jr <= i__4; ++jr) {
			    work[(jw + 2) * *n + jr] = scale * work[(jw + 2) *
				     *n + jr];
/* L140: */
			}
/* L150: */
		    }
		    xmax = scale * xmax;
		}
		xmax = std::max(xmax,temp);
L160:
		;
	    }

/*           Copy eigenvector to VL, back transforming if */
/*           HOWMNY='B'. */

	    ++ieig;
	    if (ilback) {
		i__2 = nw - 1;
		for (jw = 0; jw <= i__2; ++jw) {
		    i__3 = *n + 1 - je;
		    dgemv_("N", n, &i__3, &c_b34, &vl[je * vl_dim1 + 1], ldvl,
			     &work[(jw + 2) * *n + je], &c__1, &c_b36, &work[(
			    jw + 4) * *n + 1], &c__1);
/* L170: */
		}
		dlacpy_(" ", n, &nw, &work[(*n << 2) + 1], n, &vl[je *
			vl_dim1 + 1], ldvl);
		ibeg = 1;
	    } else {
		dlacpy_(" ", n, &nw, &work[(*n << 1) + 1], n, &vl[ieig *
			vl_dim1 + 1], ldvl);
		ibeg = je;
	    }

/*           Scale eigenvector */

	    xmax = 0.;
	    if (ilcplx) {
		i__2 = *n;
		for (j = ibeg; j <= i__2; ++j) {
/* Computing MAX */
		    d__3 = xmax, d__4 = (d__1 = vl[j + ieig * vl_dim1], abs(
			    d__1)) + (d__2 = vl[j + (ieig + 1) * vl_dim1],
			    abs(d__2));
		    xmax = std::max(d__3,d__4);
/* L180: */
		}
	    } else {
		i__2 = *n;
		for (j = ibeg; j <= i__2; ++j) {
/* Computing MAX */
		    d__2 = xmax, d__3 = (d__1 = vl[j + ieig * vl_dim1], abs(
			    d__1));
		    xmax = std::max(d__2,d__3);
/* L190: */
		}
	    }

	    if (xmax > safmin) {
		xscale = 1. / xmax;

		i__2 = nw - 1;
		for (jw = 0; jw <= i__2; ++jw) {
		    i__3 = *n;
		    for (jr = ibeg; jr <= i__3; ++jr) {
			vl[jr + (ieig + jw) * vl_dim1] = xscale * vl[jr + (
				ieig + jw) * vl_dim1];
/* L200: */
		    }
/* L210: */
		}
	    }
	    ieig = ieig + nw - 1;

L220:
	    ;
	}
    }

/*     Right eigenvectors */

    if (compr) {
	ieig = im + 1;

/*        Main loop over eigenvalues */

	ilcplx = false;
	for (je = *n; je >= 1; --je) {

/*           Skip this iteration if (a) HOWMNY='S' and SELECT=.FALSE., or */
/*           (b) this would be the second of a complex pair. */
/*           Check for complex eigenvalue, so as to be sure of which */
/*           entry(-ies) of SELECT to look at -- if complex, SELECT(JE) */
/*           or SELECT(JE-1). */
/*           If this is a complex pair, the 2-by-2 diagonal block */
/*           corresponding to the eigenvalue is in rows/columns JE-1:JE */

	    if (ilcplx) {
		ilcplx = false;
		goto L500;
	    }
	    nw = 1;
	    if (je > 1) {
		if (s[je + (je - 1) * s_dim1] != 0.) {
		    ilcplx = true;
		    nw = 2;
		}
	    }
	    if (ilall) {
		ilcomp = true;
	    } else if (ilcplx) {
		ilcomp = select[je] || select[je - 1];
	    } else {
		ilcomp = select[je];
	    }
	    if (! ilcomp) {
		goto L500;
	    }

/*           Decide if (a) singular pencil, (b) real eigenvalue, or */
/*           (c) complex eigenvalue. */

	    if (! ilcplx) {
		if ((d__1 = s[je + je * s_dim1], abs(d__1)) <= safmin && (
			d__2 = p[je + je * p_dim1], abs(d__2)) <= safmin) {

/*                 Singular matrix pencil -- unit eigenvector */

		    --ieig;
		    i__1 = *n;
		    for (jr = 1; jr <= i__1; ++jr) {
			vr[jr + ieig * vr_dim1] = 0.;
/* L230: */
		    }
		    vr[ieig + ieig * vr_dim1] = 1.;
		    goto L500;
		}
	    }

/*           Clear vector */

	    i__1 = nw - 1;
	    for (jw = 0; jw <= i__1; ++jw) {
		i__2 = *n;
		for (jr = 1; jr <= i__2; ++jr) {
		    work[(jw + 2) * *n + jr] = 0.;
/* L240: */
		}
/* L250: */
	    }

/*           Compute coefficients in  ( a A - b B ) x = 0 */
/*              a  is  ACOEF */
/*              b  is  BCOEFR + i*BCOEFI */

	    if (! ilcplx) {

/*              Real eigenvalue */

/* Computing MAX */
		d__3 = (d__1 = s[je + je * s_dim1], abs(d__1)) * ascale, d__4
			= (d__2 = p[je + je * p_dim1], abs(d__2)) * bscale,
			d__3 = std::max(d__3,d__4);
		temp = 1. / std::max(d__3,safmin);
		salfar = temp * s[je + je * s_dim1] * ascale;
		sbeta = temp * p[je + je * p_dim1] * bscale;
		acoef = sbeta * ascale;
		bcoefr = salfar * bscale;
		bcoefi = 0.;

/*              Scale to avoid underflow */

		scale = 1.;
		lsa = abs(sbeta) >= safmin && abs(acoef) < small;
		lsb = abs(salfar) >= safmin && abs(bcoefr) < small;
		if (lsa) {
		    scale = small / abs(sbeta) * std::min(anorm,big);
		}
		if (lsb) {
/* Computing MAX */
		    d__1 = scale, d__2 = small / abs(salfar) * std::min(bnorm,big);
		    scale = std::max(d__1,d__2);
		}
		if (lsa || lsb) {
/* Computing MIN */
/* Computing MAX */
		    d__3 = 1., d__4 = abs(acoef), d__3 = std::max(d__3,d__4), d__4
			    = abs(bcoefr);
		    d__1 = scale, d__2 = 1. / (safmin * std::max(d__3,d__4));
		    scale = std::min(d__1,d__2);
		    if (lsa) {
			acoef = ascale * (scale * sbeta);
		    } else {
			acoef = scale * acoef;
		    }
		    if (lsb) {
			bcoefr = bscale * (scale * salfar);
		    } else {
			bcoefr = scale * bcoefr;
		    }
		}
		acoefa = abs(acoef);
		bcoefa = abs(bcoefr);

/*              First component is 1 */

		work[(*n << 1) + je] = 1.;
		xmax = 1.;

/*              Compute contribution from column JE of A and B to sum */
/*              (See "Further Details", above.) */

		i__1 = je - 1;
		for (jr = 1; jr <= i__1; ++jr) {
		    work[(*n << 1) + jr] = bcoefr * p[jr + je * p_dim1] -
			    acoef * s[jr + je * s_dim1];
/* L260: */
		}
	    } else {

/*              Complex eigenvalue */

		d__1 = safmin * 100.;
		dlag2_(&s[je - 1 + (je - 1) * s_dim1], lds, &p[je - 1 + (je -
			1) * p_dim1], ldp, &d__1, &acoef, &temp, &bcoefr, &
			temp2, &bcoefi);
		if (bcoefi == 0.) {
		    *info = je - 1;
		    return 0;
		}

/*              Scale to avoid over/underflow */

		acoefa = abs(acoef);
		bcoefa = abs(bcoefr) + abs(bcoefi);
		scale = 1.;
		if (acoefa * ulp < safmin && acoefa >= safmin) {
		    scale = safmin / ulp / acoefa;
		}
		if (bcoefa * ulp < safmin && bcoefa >= safmin) {
/* Computing MAX */
		    d__1 = scale, d__2 = safmin / ulp / bcoefa;
		    scale = std::max(d__1,d__2);
		}
		if (safmin * acoefa > ascale) {
		    scale = ascale / (safmin * acoefa);
		}
		if (safmin * bcoefa > bscale) {
/* Computing MIN */
		    d__1 = scale, d__2 = bscale / (safmin * bcoefa);
		    scale = std::min(d__1,d__2);
		}
		if (scale != 1.) {
		    acoef = scale * acoef;
		    acoefa = abs(acoef);
		    bcoefr = scale * bcoefr;
		    bcoefi = scale * bcoefi;
		    bcoefa = abs(bcoefr) + abs(bcoefi);
		}

/*              Compute first two components of eigenvector */
/*              and contribution to sums */

		temp = acoef * s[je + (je - 1) * s_dim1];
		temp2r = acoef * s[je + je * s_dim1] - bcoefr * p[je + je *
			p_dim1];
		temp2i = -bcoefi * p[je + je * p_dim1];
		if (abs(temp) >= abs(temp2r) + abs(temp2i)) {
		    work[(*n << 1) + je] = 1.;
		    work[*n * 3 + je] = 0.;
		    work[(*n << 1) + je - 1] = -temp2r / temp;
		    work[*n * 3 + je - 1] = -temp2i / temp;
		} else {
		    work[(*n << 1) + je - 1] = 1.;
		    work[*n * 3 + je - 1] = 0.;
		    temp = acoef * s[je - 1 + je * s_dim1];
		    work[(*n << 1) + je] = (bcoefr * p[je - 1 + (je - 1) *
			    p_dim1] - acoef * s[je - 1 + (je - 1) * s_dim1]) /
			     temp;
		    work[*n * 3 + je] = bcoefi * p[je - 1 + (je - 1) * p_dim1]
			     / temp;
		}

/* Computing MAX */
		d__5 = (d__1 = work[(*n << 1) + je], abs(d__1)) + (d__2 =
			work[*n * 3 + je], abs(d__2)), d__6 = (d__3 = work[(*
			n << 1) + je - 1], abs(d__3)) + (d__4 = work[*n * 3 +
			je - 1], abs(d__4));
		xmax = std::max(d__5,d__6);

/*              Compute contribution from columns JE and JE-1 */
/*              of A and B to the sums. */

		creala = acoef * work[(*n << 1) + je - 1];
		cimaga = acoef * work[*n * 3 + je - 1];
		crealb = bcoefr * work[(*n << 1) + je - 1] - bcoefi * work[*n
			* 3 + je - 1];
		cimagb = bcoefi * work[(*n << 1) + je - 1] + bcoefr * work[*n
			* 3 + je - 1];
		cre2a = acoef * work[(*n << 1) + je];
		cim2a = acoef * work[*n * 3 + je];
		cre2b = bcoefr * work[(*n << 1) + je] - bcoefi * work[*n * 3
			+ je];
		cim2b = bcoefi * work[(*n << 1) + je] + bcoefr * work[*n * 3
			+ je];
		i__1 = je - 2;
		for (jr = 1; jr <= i__1; ++jr) {
		    work[(*n << 1) + jr] = -creala * s[jr + (je - 1) * s_dim1]
			     + crealb * p[jr + (je - 1) * p_dim1] - cre2a * s[
			    jr + je * s_dim1] + cre2b * p[jr + je * p_dim1];
		    work[*n * 3 + jr] = -cimaga * s[jr + (je - 1) * s_dim1] +
			    cimagb * p[jr + (je - 1) * p_dim1] - cim2a * s[jr
			    + je * s_dim1] + cim2b * p[jr + je * p_dim1];
/* L270: */
		}
	    }

/* Computing MAX */
	    d__1 = ulp * acoefa * anorm, d__2 = ulp * bcoefa * bnorm, d__1 =
		    std::max(d__1,d__2);
	    dmin__ = std::max(d__1,safmin);

/*           Columnwise triangular solve of  (a A - b B)  x = 0 */

	    il2by2 = false;
	    for (j = je - nw; j >= 1; --j) {

/*              If a 2-by-2 block, is in position j-1:j, wait until */
/*              next iteration to process it (when it will be j:j+1) */

		if (! il2by2 && j > 1) {
		    if (s[j + (j - 1) * s_dim1] != 0.) {
			il2by2 = true;
			goto L370;
		    }
		}
		bdiag[0] = p[j + j * p_dim1];
		if (il2by2) {
		    na = 2;
		    bdiag[1] = p[j + 1 + (j + 1) * p_dim1];
		} else {
		    na = 1;
		}

/*              Compute x(j) (and x(j+1), if 2-by-2 block) */

		dlaln2_(&c_false, &na, &nw, &dmin__, &acoef, &s[j + j *
			s_dim1], lds, bdiag, &bdiag[1], &work[(*n << 1) + j],
			n, &bcoefr, &bcoefi, sum, &c__2, &scale, &temp, &
			iinfo);
		if (scale < 1.) {

		    i__1 = nw - 1;
		    for (jw = 0; jw <= i__1; ++jw) {
			i__2 = je;
			for (jr = 1; jr <= i__2; ++jr) {
			    work[(jw + 2) * *n + jr] = scale * work[(jw + 2) *
				     *n + jr];
/* L280: */
			}
/* L290: */
		    }
		}
/* Computing MAX */
		d__1 = scale * xmax;
		xmax = std::max(d__1,temp);

		i__1 = nw;
		for (jw = 1; jw <= i__1; ++jw) {
		    i__2 = na;
		    for (ja = 1; ja <= i__2; ++ja) {
			work[(jw + 1) * *n + j + ja - 1] = sum[ja + (jw << 1)
				- 3];
/* L300: */
		    }
/* L310: */
		}

/*              w = w + x(j)*(a S(*,j) - b P(*,j) ) with scaling */

		if (j > 1) {

/*                 Check whether scaling is necessary for sum. */

		    xscale = 1. / std::max(1.,xmax);
		    temp = acoefa * work[j] + bcoefa * work[*n + j];
		    if (il2by2) {
/* Computing MAX */
			d__1 = temp, d__2 = acoefa * work[j + 1] + bcoefa *
				work[*n + j + 1];
			temp = std::max(d__1,d__2);
		    }
/* Computing MAX */
		    d__1 = std::max(temp,acoefa);
		    temp = std::max(d__1,bcoefa);
		    if (temp > bignum * xscale) {

			i__1 = nw - 1;
			for (jw = 0; jw <= i__1; ++jw) {
			    i__2 = je;
			    for (jr = 1; jr <= i__2; ++jr) {
				work[(jw + 2) * *n + jr] = xscale * work[(jw
					+ 2) * *n + jr];
/* L320: */
			    }
/* L330: */
			}
			xmax *= xscale;
		    }

/*                 Compute the contributions of the off-diagonals of */
/*                 column j (and j+1, if 2-by-2 block) of A and B to the */
/*                 sums. */


		    i__1 = na;
		    for (ja = 1; ja <= i__1; ++ja) {
			if (ilcplx) {
			    creala = acoef * work[(*n << 1) + j + ja - 1];
			    cimaga = acoef * work[*n * 3 + j + ja - 1];
			    crealb = bcoefr * work[(*n << 1) + j + ja - 1] -
				    bcoefi * work[*n * 3 + j + ja - 1];
			    cimagb = bcoefi * work[(*n << 1) + j + ja - 1] +
				    bcoefr * work[*n * 3 + j + ja - 1];
			    i__2 = j - 1;
			    for (jr = 1; jr <= i__2; ++jr) {
				work[(*n << 1) + jr] = work[(*n << 1) + jr] -
					creala * s[jr + (j + ja - 1) * s_dim1]
					 + crealb * p[jr + (j + ja - 1) *
					p_dim1];
				work[*n * 3 + jr] = work[*n * 3 + jr] -
					cimaga * s[jr + (j + ja - 1) * s_dim1]
					 + cimagb * p[jr + (j + ja - 1) *
					p_dim1];
/* L340: */
			    }
			} else {
			    creala = acoef * work[(*n << 1) + j + ja - 1];
			    crealb = bcoefr * work[(*n << 1) + j + ja - 1];
			    i__2 = j - 1;
			    for (jr = 1; jr <= i__2; ++jr) {
				work[(*n << 1) + jr] = work[(*n << 1) + jr] -
					creala * s[jr + (j + ja - 1) * s_dim1]
					 + crealb * p[jr + (j + ja - 1) *
					p_dim1];
/* L350: */
			    }
			}
/* L360: */
		    }
		}
		il2by2 = false;
L370:
		;
	    }

/*           Copy eigenvector to VR, back transforming if */
/*           HOWMNY='B'. */

	    ieig -= nw;
	    if (ilback) {

		i__1 = nw - 1;
		for (jw = 0; jw <= i__1; ++jw) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			work[(jw + 4) * *n + jr] = work[(jw + 2) * *n + 1] *
				vr[jr + vr_dim1];
/* L380: */
		    }

/*                 A series of compiler directives to defeat */
/*                 vectorization for the next loop */


		    i__2 = je;
		    for (jc = 2; jc <= i__2; ++jc) {
			i__3 = *n;
			for (jr = 1; jr <= i__3; ++jr) {
			    work[(jw + 4) * *n + jr] += work[(jw + 2) * *n +
				    jc] * vr[jr + jc * vr_dim1];
/* L390: */
			}
/* L400: */
		    }
/* L410: */
		}

		i__1 = nw - 1;
		for (jw = 0; jw <= i__1; ++jw) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vr[jr + (ieig + jw) * vr_dim1] = work[(jw + 4) * *n +
				jr];
/* L420: */
		    }
/* L430: */
		}

		iend = *n;
	    } else {
		i__1 = nw - 1;
		for (jw = 0; jw <= i__1; ++jw) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vr[jr + (ieig + jw) * vr_dim1] = work[(jw + 2) * *n +
				jr];
/* L440: */
		    }
/* L450: */
		}

		iend = je;
	    }

/*           Scale eigenvector */

	    xmax = 0.;
	    if (ilcplx) {
		i__1 = iend;
		for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
		    d__3 = xmax, d__4 = (d__1 = vr[j + ieig * vr_dim1], abs(
			    d__1)) + (d__2 = vr[j + (ieig + 1) * vr_dim1],
			    abs(d__2));
		    xmax = std::max(d__3,d__4);
/* L460: */
		}
	    } else {
		i__1 = iend;
		for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
		    d__2 = xmax, d__3 = (d__1 = vr[j + ieig * vr_dim1], abs(
			    d__1));
		    xmax = std::max(d__2,d__3);
/* L470: */
		}
	    }

	    if (xmax > safmin) {
		xscale = 1. / xmax;
		i__1 = nw - 1;
		for (jw = 0; jw <= i__1; ++jw) {
		    i__2 = iend;
		    for (jr = 1; jr <= i__2; ++jr) {
			vr[jr + (ieig + jw) * vr_dim1] = xscale * vr[jr + (
				ieig + jw) * vr_dim1];
/* L480: */
		    }
/* L490: */
		}
	    }
L500:
	    ;
	}
    }

    return 0;

/*     End of DTGEVC */

} /* dtgevc_ */

/* Subroutine */ int dtgex2_(bool *wantq, bool *wantz, integer *n,
	double *a, integer *lda, double *b, integer *ldb, double *
	q, integer *ldq, double *z__, integer *ldz, integer *j1, integer *
	n1, integer *n2, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__4 = 4;
	static double c_b5 = 0.;
	static integer c__1 = 1;
	static integer c__2 = 2;
	static double c_b42 = 1.;
	static double c_b48 = -1.;
	static integer c__0 = 0;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, z_dim1,
	    z_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    double f, g;
    integer i__, m;
    double s[16]	/* was [4][4] */, t[16]	/* was [4][4] */, be[2], ai[2]
	    , ar[2], sa, sb, li[16]	/* was [4][4] */, ir[16]	/*
	    was [4][4] */, ss, ws, eps;
    bool weak;
    double ddum;
    integer idum;
    double taul[4], dsum;
    double taur[4], scpy[16] /* was [4][4] */, tcpy[16]	/* was [4][4] */;
    double scale, bqra21, brqa21;
    double licop[16] /* was [4][4] */;
    integer linfo;
    double ircop[16] /* was [4][4] */, dnorm;
    integer iwork[4];
    double dscale;
	bool dtrong;
    double thresh, smlnum;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTGEX2 swaps adjacent diagonal blocks (A11, B11) and (A22, B22) */
/*  of size 1-by-1 or 2-by-2 in an upper (quasi) triangular matrix pair */
/*  (A, B) by an orthogonal equivalence transformation. */

/*  (A, B) must be in generalized real Schur canonical form (as returned */
/*  by DGGES), i.e. A is block upper triangular with 1-by-1 and 2-by-2 */
/*  diagonal blocks. B is upper triangular. */

/*  Optionally, the matrices Q and Z of generalized Schur vectors are */
/*  updated. */

/*         Q(in) * A(in) * Z(in)' = Q(out) * A(out) * Z(out)' */
/*         Q(in) * B(in) * Z(in)' = Q(out) * B(out) * Z(out)' */


/*  Arguments */
/*  ========= */

/*  WANTQ   (input) LOGICAL */
/*          .TRUE. : update the left transformation matrix Q; */
/*          .FALSE.: do not update Q. */

/*  WANTZ   (input) LOGICAL */
/*          .TRUE. : update the right transformation matrix Z; */
/*          .FALSE.: do not update Z. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B. N >= 0. */

/*  A      (input/output) DOUBLE PRECISION arrays, dimensions (LDA,N) */
/*          On entry, the matrix A in the pair (A, B). */
/*          On exit, the updated matrix A. */

/*  LDA     (input)  INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,N). */

/*  B      (input/output) DOUBLE PRECISION arrays, dimensions (LDB,N) */
/*          On entry, the matrix B in the pair (A, B). */
/*          On exit, the updated matrix B. */

/*  LDB     (input)  INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,N). */

/*  Q       (input/output) DOUBLE PRECISION array, dimension (LDZ,N) */
/*          On entry, if WANTQ = .TRUE., the orthogonal matrix Q. */
/*          On exit, the updated matrix Q. */
/*          Not referenced if WANTQ = .FALSE.. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. LDQ >= 1. */
/*          If WANTQ = .TRUE., LDQ >= N. */

/*  Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N) */
/*          On entry, if WANTZ =.TRUE., the orthogonal matrix Z. */
/*          On exit, the updated matrix Z. */
/*          Not referenced if WANTZ = .FALSE.. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z. LDZ >= 1. */
/*          If WANTZ = .TRUE., LDZ >= N. */

/*  J1      (input) INTEGER */
/*          The index to the first block (A11, B11). 1 <= J1 <= N. */

/*  N1      (input) INTEGER */
/*          The order of the first block (A11, B11). N1 = 0, 1 or 2. */

/*  N2      (input) INTEGER */
/*          The order of the second block (A22, B22). N2 = 0, 1 or 2. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)). */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          LWORK >=  MAX( 1, N*(N2+N1), (N2+N1)*(N2+N1)*2 ) */

/*  INFO    (output) INTEGER */
/*            =0: Successful exit */
/*            >0: If INFO = 1, the transformed matrix (A, B) would be */
/*                too far from generalized Schur form; the blocks are */
/*                not swapped and (A, B) and (Q, Z) are unchanged. */
/*                The problem of swapping is too ill-conditioned. */
/*            <0: If INFO = -16: LWORK is too small. Appropriate value */
/*                for LWORK is returned in WORK(1). */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Bo Kagstrom and Peter Poromaa, Department of Computing Science, */
/*     Umea University, S-901 87 Umea, Sweden. */

/*  In the current code both weak and strong stability tests are */
/*  performed. The user can omit the strong stability test by changing */
/*  the internal logical parameter WANDS to .FALSE.. See ref. [2] for */
/*  details. */

/*  [1] B. Kagstrom; A Direct Method for Reordering Eigenvalues in the */
/*      Generalized Real Schur Form of a Regular Matrix Pair (A, B), in */
/*      M.S. Moonen et al (eds), Linear Algebra for Large Scale and */
/*      Real-Time Applications, Kluwer Academic Publ. 1993, pp 195-218. */

/*  [2] B. Kagstrom and P. Poromaa; Computing Eigenspaces with Specified */
/*      Eigenvalues of a Regular Matrix Pair (A, B) and Condition */
/*      Estimation: Theory, Algorithms and Software, */
/*      Report UMINF - 94.04, Department of Computing Science, Umea */
/*      University, S-901 87 Umea, Sweden, 1994. Also as LAPACK Working */
/*      Note 87. To appear in Numerical Algorithms, 1996. */

/*  ===================================================================== */
/*  Replaced various illegal calls to DCOPY by calls to DLASET, or by DO */
/*  loops. Sven Hammarling, 1/5/02. */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    *info = 0;

/*     Quick return if possible */

    if (*n <= 1 || *n1 <= 0 || *n2 <= 0) {
	return 0;
    }
    if (*n1 > *n || *j1 + *n1 > *n) {
	return 0;
    }
    m = *n1 + *n2;
/* Computing MAX */
    i__1 = 1, i__2 = *n * m, i__1 = std::max(i__1,i__2), i__2 = m * m << 1;
    if (*lwork < std::max(i__1,i__2)) {
	*info = -16;
/* Computing MAX */
	i__1 = 1, i__2 = *n * m, i__1 = std::max(i__1,i__2), i__2 = m * m << 1;
	work[1] = (double) std::max(i__1,i__2);
	return 0;
    }

    weak = false;
    dtrong = false;

/*     Make a local copy of selected block */

    dlaset_("Full", &c__4, &c__4, &c_b5, &c_b5, li, &c__4);
    dlaset_("Full", &c__4, &c__4, &c_b5, &c_b5, ir, &c__4);
    dlacpy_("Full", &m, &m, &a[*j1 + *j1 * a_dim1], lda, s, &c__4);
    dlacpy_("Full", &m, &m, &b[*j1 + *j1 * b_dim1], ldb, t, &c__4);

/*     Compute threshold for testing acceptance of swapping. */

    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    dscale = 0.;
    dsum = 1.;
    dlacpy_("Full", &m, &m, s, &c__4, &work[1], &m);
    i__1 = m * m;
    dlassq_(&i__1, &work[1], &c__1, &dscale, &dsum);
    dlacpy_("Full", &m, &m, t, &c__4, &work[1], &m);
    i__1 = m * m;
    dlassq_(&i__1, &work[1], &c__1, &dscale, &dsum);
    dnorm = dscale * sqrt(dsum);
/* Computing MAX */
    d__1 = eps * 10. * dnorm;
    thresh = std::max(d__1,smlnum);

    if (m == 2) {

/*        CASE 1: Swap 1-by-1 and 1-by-1 blocks. */

/*        Compute orthogonal QL and RQ that swap 1-by-1 and 1-by-1 blocks */
/*        using Givens rotations and perform the swap tentatively. */

	f = s[5] * t[0] - t[5] * s[0];
	g = s[5] * t[4] - t[5] * s[4];
	sb = abs(t[5]);
	sa = abs(s[5]);
	dlartg_(&f, &g, &ir[4], ir, &ddum);
	ir[1] = -ir[4];
	ir[5] = ir[0];
	drot_(&c__2, s, &c__1, &s[4], &c__1, ir, &ir[1]);
	drot_(&c__2, t, &c__1, &t[4], &c__1, ir, &ir[1]);
	if (sa >= sb) {
	    dlartg_(s, &s[1], li, &li[1], &ddum);
	} else {
	    dlartg_(t, &t[1], li, &li[1], &ddum);
	}
	drot_(&c__2, s, &c__4, &s[1], &c__4, li, &li[1]);
	drot_(&c__2, t, &c__4, &t[1], &c__4, li, &li[1]);
	li[5] = li[0];
	li[4] = -li[1];

/*        Weak stability test: */
/*           |S21| + |T21| <= O(EPS * F-norm((S, T))) */

	ws = abs(s[1]) + abs(t[1]);
	weak = ws <= thresh;
	if (! weak) {
	    goto L70;
	}

	if (true) {

/*           Strong stability test: */
/*             F-norm((A-QL'*S*QR, B-QL'*T*QR)) <= O(EPS*F-norm((A,B))) */

	    dlacpy_("Full", &m, &m, &a[*j1 + *j1 * a_dim1], lda, &work[m * m
		    + 1], &m);
	    dgemm_("N", "N", &m, &m, &m, &c_b42, li, &c__4, s, &c__4, &c_b5, &
		    work[1], &m);
	    dgemm_("N", "T", &m, &m, &m, &c_b48, &work[1], &m, ir, &c__4, &
		    c_b42, &work[m * m + 1], &m);
	    dscale = 0.;
	    dsum = 1.;
	    i__1 = m * m;
	    dlassq_(&i__1, &work[m * m + 1], &c__1, &dscale, &dsum);

	    dlacpy_("Full", &m, &m, &b[*j1 + *j1 * b_dim1], ldb, &work[m * m
		    + 1], &m);
	    dgemm_("N", "N", &m, &m, &m, &c_b42, li, &c__4, t, &c__4, &c_b5, &
		    work[1], &m);
	    dgemm_("N", "T", &m, &m, &m, &c_b48, &work[1], &m, ir, &c__4, &
		    c_b42, &work[m * m + 1], &m);
	    i__1 = m * m;
	    dlassq_(&i__1, &work[m * m + 1], &c__1, &dscale, &dsum);
	    ss = dscale * sqrt(dsum);
	    dtrong = ss <= thresh;
	    if (! dtrong) {
		goto L70;
	    }
	}

/*        Update (A(J1:J1+M-1, M+J1:N), B(J1:J1+M-1, M+J1:N)) and */
/*               (A(1:J1-1, J1:J1+M), B(1:J1-1, J1:J1+M)). */

	i__1 = *j1 + 1;
	drot_(&i__1, &a[*j1 * a_dim1 + 1], &c__1, &a[(*j1 + 1) * a_dim1 + 1],
		&c__1, ir, &ir[1]);
	i__1 = *j1 + 1;
	drot_(&i__1, &b[*j1 * b_dim1 + 1], &c__1, &b[(*j1 + 1) * b_dim1 + 1],
		&c__1, ir, &ir[1]);
	i__1 = *n - *j1 + 1;
	drot_(&i__1, &a[*j1 + *j1 * a_dim1], lda, &a[*j1 + 1 + *j1 * a_dim1],
		lda, li, &li[1]);
	i__1 = *n - *j1 + 1;
	drot_(&i__1, &b[*j1 + *j1 * b_dim1], ldb, &b[*j1 + 1 + *j1 * b_dim1],
		ldb, li, &li[1]);

/*        Set  N1-by-N2 (2,1) - blocks to ZERO. */

	a[*j1 + 1 + *j1 * a_dim1] = 0.;
	b[*j1 + 1 + *j1 * b_dim1] = 0.;

/*        Accumulate transformations into Q and Z if requested. */

	if (*wantz) {
	    drot_(n, &z__[*j1 * z_dim1 + 1], &c__1, &z__[(*j1 + 1) * z_dim1 +
		    1], &c__1, ir, &ir[1]);
	}
	if (*wantq) {
	    drot_(n, &q[*j1 * q_dim1 + 1], &c__1, &q[(*j1 + 1) * q_dim1 + 1],
		    &c__1, li, &li[1]);
	}

/*        Exit with INFO = 0 if swap was successfully performed. */

	return 0;

    } else {

/*        CASE 2: Swap 1-by-1 and 2-by-2 blocks, or 2-by-2 */
/*                and 2-by-2 blocks. */

/*        Solve the generalized Sylvester equation */
/*                 S11 * R - L * S22 = SCALE * S12 */
/*                 T11 * R - L * T22 = SCALE * T12 */
/*        for R and L. Solutions in LI and IR. */

	dlacpy_("Full", n1, n2, &t[(*n1 + 1 << 2) - 4], &c__4, li, &c__4);
	dlacpy_("Full", n1, n2, &s[(*n1 + 1 << 2) - 4], &c__4, &ir[*n2 + 1 + (
		*n1 + 1 << 2) - 5], &c__4);
	dtgsy2_("N", &c__0, n1, n2, s, &c__4, &s[*n1 + 1 + (*n1 + 1 << 2) - 5]
, &c__4, &ir[*n2 + 1 + (*n1 + 1 << 2) - 5], &c__4, t, &c__4, &
		t[*n1 + 1 + (*n1 + 1 << 2) - 5], &c__4, li, &c__4, &scale, &
		dsum, &dscale, iwork, &idum, &linfo);

/*        Compute orthogonal matrix QL: */

/*                    QL' * LI = [ TL ] */
/*                               [ 0  ] */
/*        where */
/*                    LI =  [      -L              ] */
/*                          [ SCALE * identity(N2) ] */

	i__1 = *n2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dscal_(n1, &c_b48, &li[(i__ << 2) - 4], &c__1);
	    li[*n1 + i__ + (i__ << 2) - 5] = scale;
/* L10: */
	}
	dgeqr2_(&m, n2, li, &c__4, taul, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}
	dorg2r_(&m, &m, n2, li, &c__4, taul, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}

/*        Compute orthogonal matrix RQ: */

/*                    IR * RQ' =   [ 0  TR], */

/*         where IR = [ SCALE * identity(N1), R ] */

	i__1 = *n1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    ir[*n2 + i__ + (i__ << 2) - 5] = scale;
/* L20: */
	}
	dgerq2_(n1, &m, &ir[*n2], &c__4, taur, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}
	dorgr2_(&m, &m, n1, ir, &c__4, taur, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}

/*        Perform the swapping tentatively: */

	dgemm_("T", "N", &m, &m, &m, &c_b42, li, &c__4, s, &c__4, &c_b5, &
		work[1], &m);
	dgemm_("N", "T", &m, &m, &m, &c_b42, &work[1], &m, ir, &c__4, &c_b5,
		s, &c__4);
	dgemm_("T", "N", &m, &m, &m, &c_b42, li, &c__4, t, &c__4, &c_b5, &
		work[1], &m);
	dgemm_("N", "T", &m, &m, &m, &c_b42, &work[1], &m, ir, &c__4, &c_b5,
		t, &c__4);
	dlacpy_("F", &m, &m, s, &c__4, scpy, &c__4);
	dlacpy_("F", &m, &m, t, &c__4, tcpy, &c__4);
	dlacpy_("F", &m, &m, ir, &c__4, ircop, &c__4);
	dlacpy_("F", &m, &m, li, &c__4, licop, &c__4);

/*        Triangularize the B-part by an RQ factorization. */
/*        Apply transformation (from left) to A-part, giving S. */

	dgerq2_(&m, &m, t, &c__4, taur, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}
	dormr2_("R", "T", &m, &m, &m, t, &c__4, taur, s, &c__4, &work[1], &
		linfo);
	if (linfo != 0) {
	    goto L70;
	}
	dormr2_("L", "N", &m, &m, &m, t, &c__4, taur, ir, &c__4, &work[1], &
		linfo);
	if (linfo != 0) {
	    goto L70;
	}

/*        Compute F-norm(S21) in BRQA21. (T21 is 0.) */

	dscale = 0.;
	dsum = 1.;
	i__1 = *n2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlassq_(n1, &s[*n2 + 1 + (i__ << 2) - 5], &c__1, &dscale, &dsum);
/* L30: */
	}
	brqa21 = dscale * sqrt(dsum);

/*        Triangularize the B-part by a QR factorization. */
/*        Apply transformation (from right) to A-part, giving S. */

	dgeqr2_(&m, &m, tcpy, &c__4, taul, &work[1], &linfo);
	if (linfo != 0) {
	    goto L70;
	}
	dorm2r_("L", "T", &m, &m, &m, tcpy, &c__4, taul, scpy, &c__4, &work[1]
, info);
	dorm2r_("R", "N", &m, &m, &m, tcpy, &c__4, taul, licop, &c__4, &work[
		1], info);
	if (linfo != 0) {
	    goto L70;
	}

/*        Compute F-norm(S21) in BQRA21. (T21 is 0.) */

	dscale = 0.;
	dsum = 1.;
	i__1 = *n2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlassq_(n1, &scpy[*n2 + 1 + (i__ << 2) - 5], &c__1, &dscale, &
		    dsum);
/* L40: */
	}
	bqra21 = dscale * sqrt(dsum);

/*        Decide which method to use. */
/*          Weak stability test: */
/*             F-norm(S21) <= O(EPS * F-norm((S, T))) */

	if (bqra21 <= brqa21 && bqra21 <= thresh) {
	    dlacpy_("F", &m, &m, scpy, &c__4, s, &c__4);
	    dlacpy_("F", &m, &m, tcpy, &c__4, t, &c__4);
	    dlacpy_("F", &m, &m, ircop, &c__4, ir, &c__4);
	    dlacpy_("F", &m, &m, licop, &c__4, li, &c__4);
	} else if (brqa21 >= thresh) {
	    goto L70;
	}

/*        Set lower triangle of B-part to zero */

	i__1 = m - 1;
	i__2 = m - 1;
	dlaset_("Lower", &i__1, &i__2, &c_b5, &c_b5, &t[1], &c__4);

	if (true) {

/*           Strong stability test: */
/*              F-norm((A-QL*S*QR', B-QL*T*QR')) <= O(EPS*F-norm((A,B))) */

	    dlacpy_("Full", &m, &m, &a[*j1 + *j1 * a_dim1], lda, &work[m * m
		    + 1], &m);
	    dgemm_("N", "N", &m, &m, &m, &c_b42, li, &c__4, s, &c__4, &c_b5, &
		    work[1], &m);
	    dgemm_("N", "N", &m, &m, &m, &c_b48, &work[1], &m, ir, &c__4, &
		    c_b42, &work[m * m + 1], &m);
	    dscale = 0.;
	    dsum = 1.;
	    i__1 = m * m;
	    dlassq_(&i__1, &work[m * m + 1], &c__1, &dscale, &dsum);

	    dlacpy_("Full", &m, &m, &b[*j1 + *j1 * b_dim1], ldb, &work[m * m
		    + 1], &m);
	    dgemm_("N", "N", &m, &m, &m, &c_b42, li, &c__4, t, &c__4, &c_b5, &
		    work[1], &m);
	    dgemm_("N", "N", &m, &m, &m, &c_b48, &work[1], &m, ir, &c__4, &
		    c_b42, &work[m * m + 1], &m);
	    i__1 = m * m;
	    dlassq_(&i__1, &work[m * m + 1], &c__1, &dscale, &dsum);
	    ss = dscale * sqrt(dsum);
	    dtrong = ss <= thresh;
	    if (! dtrong) {
		goto L70;
	    }

	}

/*        If the swap is accepted ("weakly" and "strongly"), apply the */
/*        transformations and set N1-by-N2 (2,1)-block to zero. */

	dlaset_("Full", n1, n2, &c_b5, &c_b5, &s[*n2], &c__4);

/*        copy back M-by-M diagonal block starting at index J1 of (A, B) */

	dlacpy_("F", &m, &m, s, &c__4, &a[*j1 + *j1 * a_dim1], lda)
		;
	dlacpy_("F", &m, &m, t, &c__4, &b[*j1 + *j1 * b_dim1], ldb)
		;
	dlaset_("Full", &c__4, &c__4, &c_b5, &c_b5, t, &c__4);

/*        Standardize existing 2-by-2 blocks. */

	i__1 = m * m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    work[i__] = 0.;
/* L50: */
	}
	work[1] = 1.;
	t[0] = 1.;
	idum = *lwork - m * m - 2;
	if (*n2 > 1) {
	    dlagv2_(&a[*j1 + *j1 * a_dim1], lda, &b[*j1 + *j1 * b_dim1], ldb,
		    ar, ai, be, &work[1], &work[2], t, &t[1]);
	    work[m + 1] = -work[2];
	    work[m + 2] = work[1];
	    t[*n2 + (*n2 << 2) - 5] = t[0];
	    t[4] = -t[1];
	}
	work[m * m] = 1.;
	t[m + (m << 2) - 5] = 1.;

	if (*n1 > 1) {
	    dlagv2_(&a[*j1 + *n2 + (*j1 + *n2) * a_dim1], lda, &b[*j1 + *n2 +
		    (*j1 + *n2) * b_dim1], ldb, taur, taul, &work[m * m + 1],
		    &work[*n2 * m + *n2 + 1], &work[*n2 * m + *n2 + 2], &t[*
		    n2 + 1 + (*n2 + 1 << 2) - 5], &t[m + (m - 1 << 2) - 5]);
	    work[m * m] = work[*n2 * m + *n2 + 1];
	    work[m * m - 1] = -work[*n2 * m + *n2 + 2];
	    t[m + (m << 2) - 5] = t[*n2 + 1 + (*n2 + 1 << 2) - 5];
	    t[m - 1 + (m << 2) - 5] = -t[m + (m - 1 << 2) - 5];
	}
	dgemm_("T", "N", n2, n1, n2, &c_b42, &work[1], &m, &a[*j1 + (*j1 + *
		n2) * a_dim1], lda, &c_b5, &work[m * m + 1], n2);
	dlacpy_("Full", n2, n1, &work[m * m + 1], n2, &a[*j1 + (*j1 + *n2) *
		a_dim1], lda);
	dgemm_("T", "N", n2, n1, n2, &c_b42, &work[1], &m, &b[*j1 + (*j1 + *
		n2) * b_dim1], ldb, &c_b5, &work[m * m + 1], n2);
	dlacpy_("Full", n2, n1, &work[m * m + 1], n2, &b[*j1 + (*j1 + *n2) *
		b_dim1], ldb);
	dgemm_("N", "N", &m, &m, &m, &c_b42, li, &c__4, &work[1], &m, &c_b5, &
		work[m * m + 1], &m);
	dlacpy_("Full", &m, &m, &work[m * m + 1], &m, li, &c__4);
	dgemm_("N", "N", n2, n1, n1, &c_b42, &a[*j1 + (*j1 + *n2) * a_dim1],
		lda, &t[*n2 + 1 + (*n2 + 1 << 2) - 5], &c__4, &c_b5, &work[1],
		 n2);
	dlacpy_("Full", n2, n1, &work[1], n2, &a[*j1 + (*j1 + *n2) * a_dim1],
		lda);
	dgemm_("N", "N", n2, n1, n1, &c_b42, &b[*j1 + (*j1 + *n2) * b_dim1],
		ldb, &t[*n2 + 1 + (*n2 + 1 << 2) - 5], &c__4, &c_b5, &work[1],
		 n2);
	dlacpy_("Full", n2, n1, &work[1], n2, &b[*j1 + (*j1 + *n2) * b_dim1],
		ldb);
	dgemm_("T", "N", &m, &m, &m, &c_b42, ir, &c__4, t, &c__4, &c_b5, &
		work[1], &m);
	dlacpy_("Full", &m, &m, &work[1], &m, ir, &c__4);

/*        Accumulate transformations into Q and Z if requested. */

	if (*wantq) {
	    dgemm_("N", "N", n, &m, &m, &c_b42, &q[*j1 * q_dim1 + 1], ldq, li,
		     &c__4, &c_b5, &work[1], n);
	    dlacpy_("Full", n, &m, &work[1], n, &q[*j1 * q_dim1 + 1], ldq);

	}

	if (*wantz) {
	    dgemm_("N", "N", n, &m, &m, &c_b42, &z__[*j1 * z_dim1 + 1], ldz,
		    ir, &c__4, &c_b5, &work[1], n);
	    dlacpy_("Full", n, &m, &work[1], n, &z__[*j1 * z_dim1 + 1], ldz);

	}

/*        Update (A(J1:J1+M-1, M+J1:N), B(J1:J1+M-1, M+J1:N)) and */
/*                (A(1:J1-1, J1:J1+M), B(1:J1-1, J1:J1+M)). */

	i__ = *j1 + m;
	if (i__ <= *n) {
	    i__1 = *n - i__ + 1;
	    dgemm_("T", "N", &m, &i__1, &m, &c_b42, li, &c__4, &a[*j1 + i__ *
		    a_dim1], lda, &c_b5, &work[1], &m);
	    i__1 = *n - i__ + 1;
	    dlacpy_("Full", &m, &i__1, &work[1], &m, &a[*j1 + i__ * a_dim1],
		    lda);
	    i__1 = *n - i__ + 1;
	    dgemm_("T", "N", &m, &i__1, &m, &c_b42, li, &c__4, &b[*j1 + i__ *
		    b_dim1], lda, &c_b5, &work[1], &m);
	    i__1 = *n - i__ + 1;
	    dlacpy_("Full", &m, &i__1, &work[1], &m, &b[*j1 + i__ * b_dim1],
		    ldb);
	}
	i__ = *j1 - 1;
	if (i__ > 0) {
	    dgemm_("N", "N", &i__, &m, &m, &c_b42, &a[*j1 * a_dim1 + 1], lda,
		    ir, &c__4, &c_b5, &work[1], &i__);
	    dlacpy_("Full", &i__, &m, &work[1], &i__, &a[*j1 * a_dim1 + 1],
		    lda);
	    dgemm_("N", "N", &i__, &m, &m, &c_b42, &b[*j1 * b_dim1 + 1], ldb,
		    ir, &c__4, &c_b5, &work[1], &i__);
	    dlacpy_("Full", &i__, &m, &work[1], &i__, &b[*j1 * b_dim1 + 1],
		    ldb);
	}

/*        Exit with INFO = 0 if swap was successfully performed. */

	return 0;

    }

/*     Exit with INFO = 1 if swap was rejected. */

L70:

    *info = 1;
    return 0;

/*     End of DTGEX2 */

} /* dtgex2_ */

/* Subroutine */ int dtgexc_(bool *wantq, bool *wantz, integer *n,
	double *a, integer *lda, double *b, integer *ldb, double *
	q, integer *ldq, double *z__, integer *ldz, integer *ifst,
	integer *ilst, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__2 = 2;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, z_dim1,
	    z_offset, i__1;

    /* Local variables */
    integer nbf, nbl, here, lwmin;
	integer nbnext;
    bool lquery;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTGEXC reorders the generalized real Schur decomposition of a real */
/*  matrix pair (A,B) using an orthogonal equivalence transformation */

/*                 (A, B) = Q * (A, B) * Z', */

/*  so that the diagonal block of (A, B) with row index IFST is moved */
/*  to row ILST. */

/*  (A, B) must be in generalized real Schur canonical form (as returned */
/*  by DGGES), i.e. A is block upper triangular with 1-by-1 and 2-by-2 */
/*  diagonal blocks. B is upper triangular. */

/*  Optionally, the matrices Q and Z of generalized Schur vectors are */
/*  updated. */

/*         Q(in) * A(in) * Z(in)' = Q(out) * A(out) * Z(out)' */
/*         Q(in) * B(in) * Z(in)' = Q(out) * B(out) * Z(out)' */


/*  Arguments */
/*  ========= */

/*  WANTQ   (input) LOGICAL */
/*          .TRUE. : update the left transformation matrix Q; */
/*          .FALSE.: do not update Q. */

/*  WANTZ   (input) LOGICAL */
/*          .TRUE. : update the right transformation matrix Z; */
/*          .FALSE.: do not update Z. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B. N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the matrix A in generalized real Schur canonical */
/*          form. */
/*          On exit, the updated matrix A, again in generalized */
/*          real Schur canonical form. */

/*  LDA     (input)  INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,N) */
/*          On entry, the matrix B in generalized real Schur canonical */
/*          form (A,B). */
/*          On exit, the updated matrix B, again in generalized */
/*          real Schur canonical form (A,B). */

/*  LDB     (input)  INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,N). */

/*  Q       (input/output) DOUBLE PRECISION array, dimension (LDZ,N) */
/*          On entry, if WANTQ = .TRUE., the orthogonal matrix Q. */
/*          On exit, the updated matrix Q. */
/*          If WANTQ = .FALSE., Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. LDQ >= 1. */
/*          If WANTQ = .TRUE., LDQ >= N. */

/*  Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N) */
/*          On entry, if WANTZ = .TRUE., the orthogonal matrix Z. */
/*          On exit, the updated matrix Z. */
/*          If WANTZ = .FALSE., Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z. LDZ >= 1. */
/*          If WANTZ = .TRUE., LDZ >= N. */

/*  IFST    (input/output) INTEGER */
/*  ILST    (input/output) INTEGER */
/*          Specify the reordering of the diagonal blocks of (A, B). */
/*          The block with row index IFST is moved to row ILST, by a */
/*          sequence of swapping between adjacent blocks. */
/*          On exit, if IFST pointed on entry to the second row of */
/*          a 2-by-2 block, it is changed to point to the first row; */
/*          ILST always points to the first row of the block in its */
/*          final position (which may differ from its input value by */
/*          +1 or -1). 1 <= IFST, ILST <= N. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          LWORK >= 1 when N <= 1, otherwise LWORK >= 4*N + 16. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*           =0:  successful exit. */
/*           <0:  if INFO = -i, the i-th argument had an illegal value. */
/*           =1:  The transformed matrix pair (A, B) would be too far */
/*                from generalized Schur form; the problem is ill- */
/*                conditioned. (A, B) may have been partially reordered, */
/*                and ILST points to the first row of the current */
/*                position of the block being moved. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Bo Kagstrom and Peter Poromaa, Department of Computing Science, */
/*     Umea University, S-901 87 Umea, Sweden. */

/*  [1] B. Kagstrom; A Direct Method for Reordering Eigenvalues in the */
/*      Generalized Real Schur Form of a Regular Matrix Pair (A, B), in */
/*      M.S. Moonen et al (eds), Linear Algebra for Large Scale and */
/*      Real-Time Applications, Kluwer Academic Publ. 1993, pp 195-218. */

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

/*     Decode and test input arguments. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1;
    if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldq < 1 || *wantq && *ldq < std::max(1_integer,*n)) {
	*info = -9;
    } else if (*ldz < 1 || *wantz && *ldz < std::max(1_integer,*n)) {
	*info = -11;
    } else if (*ifst < 1 || *ifst > *n) {
	*info = -12;
    } else if (*ilst < 1 || *ilst > *n) {
	*info = -13;
    }

    if (*info == 0) {
	if (*n <= 1) {
	    lwmin = 1;
	} else {
	    lwmin = (*n << 2) + 16;
	}
	work[1] = (double) lwmin;

	if (*lwork < lwmin && ! lquery) {
	    *info = -15;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTGEXC", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 1) {
	return 0;
    }

/*     Determine the first row of the specified block and find out */
/*     if it is 1-by-1 or 2-by-2. */

    if (*ifst > 1) {
	if (a[*ifst + (*ifst - 1) * a_dim1] != 0.) {
	    --(*ifst);
	}
    }
    nbf = 1;
    if (*ifst < *n) {
	if (a[*ifst + 1 + *ifst * a_dim1] != 0.) {
	    nbf = 2;
	}
    }

/*     Determine the first row of the final block */
/*     and find out if it is 1-by-1 or 2-by-2. */

    if (*ilst > 1) {
	if (a[*ilst + (*ilst - 1) * a_dim1] != 0.) {
	    --(*ilst);
	}
    }
    nbl = 1;
    if (*ilst < *n) {
	if (a[*ilst + 1 + *ilst * a_dim1] != 0.) {
	    nbl = 2;
	}
    }
    if (*ifst == *ilst) {
	return 0;
    }

    if (*ifst < *ilst) {

/*        Update ILST. */

	if (nbf == 2 && nbl == 1) {
	    --(*ilst);
	}
	if (nbf == 1 && nbl == 2) {
	    ++(*ilst);
	}

	here = *ifst;

L10:

/*        Swap with next one below. */

	if (nbf == 1 || nbf == 2) {

/*           Current block either 1-by-1 or 2-by-2. */

	    nbnext = 1;
	    if (here + nbf + 1 <= *n) {
		if (a[here + nbf + 1 + (here + nbf) * a_dim1] != 0.) {
		    nbnext = 2;
		}
	    }
	    dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset], ldb, &q[
		    q_offset], ldq, &z__[z_offset], ldz, &here, &nbf, &nbnext,
		     &work[1], lwork, info);
	    if (*info != 0) {
		*ilst = here;
		return 0;
	    }
	    here += nbnext;

/*           Test if 2-by-2 block breaks into two 1-by-1 blocks. */

	    if (nbf == 2) {
		if (a[here + 1 + here * a_dim1] == 0.) {
		    nbf = 3;
		}
	    }

	} else {

/*           Current block consists of two 1-by-1 blocks, each of which */
/*           must be swapped individually. */

	    nbnext = 1;
	    if (here + 3 <= *n) {
		if (a[here + 3 + (here + 2) * a_dim1] != 0.) {
		    nbnext = 2;
		}
	    }
	    i__1 = here + 1;
	    dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset], ldb, &q[
		    q_offset], ldq, &z__[z_offset], ldz, &i__1, &c__1, &
		    nbnext, &work[1], lwork, info);
	    if (*info != 0) {
		*ilst = here;
		return 0;
	    }
	    if (nbnext == 1) {

/*              Swap two 1-by-1 blocks. */

		dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset], ldb,
			 &q[q_offset], ldq, &z__[z_offset], ldz, &here, &c__1,
			 &c__1, &work[1], lwork, info);
		if (*info != 0) {
		    *ilst = here;
		    return 0;
		}
		++here;

	    } else {

/*              Recompute NBNEXT in case of 2-by-2 split. */

		if (a[here + 2 + (here + 1) * a_dim1] == 0.) {
		    nbnext = 1;
		}
		if (nbnext == 2) {

/*                 2-by-2 block did not split. */

		    dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset],
			    ldb, &q[q_offset], ldq, &z__[z_offset], ldz, &
			    here, &c__1, &nbnext, &work[1], lwork, info);
		    if (*info != 0) {
			*ilst = here;
			return 0;
		    }
		    here += 2;
		} else {

/*                 2-by-2 block did split. */

		    dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset],
			    ldb, &q[q_offset], ldq, &z__[z_offset], ldz, &
			    here, &c__1, &c__1, &work[1], lwork, info);
		    if (*info != 0) {
			*ilst = here;
			return 0;
		    }
		    ++here;
		    dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset],
			    ldb, &q[q_offset], ldq, &z__[z_offset], ldz, &
			    here, &c__1, &c__1, &work[1], lwork, info);
		    if (*info != 0) {
			*ilst = here;
			return 0;
		    }
		    ++here;
		}

	    }
	}
	if (here < *ilst) {
	    goto L10;
	}
    } else {
	here = *ifst;

L20:

/*        Swap with next one below. */

	if (nbf == 1 || nbf == 2) {

/*           Current block either 1-by-1 or 2-by-2. */

	    nbnext = 1;
	    if (here >= 3) {
		if (a[here - 1 + (here - 2) * a_dim1] != 0.) {
		    nbnext = 2;
		}
	    }
	    i__1 = here - nbnext;
	    dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset], ldb, &q[
		    q_offset], ldq, &z__[z_offset], ldz, &i__1, &nbnext, &nbf,
		     &work[1], lwork, info);
	    if (*info != 0) {
		*ilst = here;
		return 0;
	    }
	    here -= nbnext;

/*           Test if 2-by-2 block breaks into two 1-by-1 blocks. */

	    if (nbf == 2) {
		if (a[here + 1 + here * a_dim1] == 0.) {
		    nbf = 3;
		}
	    }

	} else {

/*           Current block consists of two 1-by-1 blocks, each of which */
/*           must be swapped individually. */

	    nbnext = 1;
	    if (here >= 3) {
		if (a[here - 1 + (here - 2) * a_dim1] != 0.) {
		    nbnext = 2;
		}
	    }
	    i__1 = here - nbnext;
	    dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset], ldb, &q[
		    q_offset], ldq, &z__[z_offset], ldz, &i__1, &nbnext, &
		    c__1, &work[1], lwork, info);
	    if (*info != 0) {
		*ilst = here;
		return 0;
	    }
	    if (nbnext == 1) {

/*              Swap two 1-by-1 blocks. */

		dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset], ldb,
			 &q[q_offset], ldq, &z__[z_offset], ldz, &here, &
			nbnext, &c__1, &work[1], lwork, info);
		if (*info != 0) {
		    *ilst = here;
		    return 0;
		}
		--here;
	    } else {

/*             Recompute NBNEXT in case of 2-by-2 split. */

		if (a[here + (here - 1) * a_dim1] == 0.) {
		    nbnext = 1;
		}
		if (nbnext == 2) {

/*                 2-by-2 block did not split. */

		    i__1 = here - 1;
		    dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset],
			    ldb, &q[q_offset], ldq, &z__[z_offset], ldz, &
			    i__1, &c__2, &c__1, &work[1], lwork, info);
		    if (*info != 0) {
			*ilst = here;
			return 0;
		    }
		    here += -2;
		} else {

/*                 2-by-2 block did split. */

		    dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset],
			    ldb, &q[q_offset], ldq, &z__[z_offset], ldz, &
			    here, &c__1, &c__1, &work[1], lwork, info);
		    if (*info != 0) {
			*ilst = here;
			return 0;
		    }
		    --here;
		    dtgex2_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset],
			    ldb, &q[q_offset], ldq, &z__[z_offset], ldz, &
			    here, &c__1, &c__1, &work[1], lwork, info);
		    if (*info != 0) {
			*ilst = here;
			return 0;
		    }
		    --here;
		}
	    }
	}
	if (here > *ilst) {
	    goto L20;
	}
    }
    *ilst = here;
    work[1] = (double) lwmin;
    return 0;

/*     End of DTGEXC */

} /* dtgexc_ */

/* Subroutine */ int dtgsen_(integer *ijob, bool *wantq, bool *wantz,
	bool *select, integer *n, double *a, integer *lda, double *
	b, integer *ldb, double *alphar, double *alphai, double *
	beta, double *q, integer *ldq, double *z__, integer *ldz,
	integer *m, double *pl, double *pr, double *dif,
	double *work, integer *lwork, integer *iwork, integer *liwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__2 = 2;
	static double c_b28 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, z_dim1,
	    z_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer i__, k, n1, n2, kk, ks, mn2, ijb;
    double eps;
    integer kase;
    bool pair;
    integer ierr;
    double dsum;
    bool swap;
    integer isave[3];
    bool wantd;
    integer lwmin;
    bool wantp;
    bool wantd1, wantd2;
    double dscale, rdscal;
    integer liwmin;
    double smlnum;
    bool lquery;


/*  -- LAPACK routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     Modified to call DLACN2 in place of DLACON, 5 Feb 03, SJH. */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTGSEN reorders the generalized real Schur decomposition of a real */
/*  matrix pair (A, B) (in terms of an orthonormal equivalence trans- */
/*  formation Q' * (A, B) * Z), so that a selected cluster of eigenvalues */
/*  appears in the leading diagonal blocks of the upper quasi-triangular */
/*  matrix A and the upper triangular B. The leading columns of Q and */
/*  Z form orthonormal bases of the corresponding left and right eigen- */
/*  spaces (deflating subspaces). (A, B) must be in generalized real */
/*  Schur canonical form (as returned by DGGES), i.e. A is block upper */
/*  triangular with 1-by-1 and 2-by-2 diagonal blocks. B is upper */
/*  triangular. */

/*  DTGSEN also computes the generalized eigenvalues */

/*              w(j) = (ALPHAR(j) + i*ALPHAI(j))/BETA(j) */

/*  of the reordered matrix pair (A, B). */

/*  Optionally, DTGSEN computes the estimates of reciprocal condition */
/*  numbers for eigenvalues and eigenspaces. These are Difu[(A11,B11), */
/*  (A22,B22)] and Difl[(A11,B11), (A22,B22)], i.e. the separation(s) */
/*  between the matrix pairs (A11, B11) and (A22,B22) that correspond to */
/*  the selected cluster and the eigenvalues outside the cluster, resp., */
/*  and norms of "projections" onto left and right eigenspaces w.r.t. */
/*  the selected cluster in the (1,1)-block. */

/*  Arguments */
/*  ========= */

/*  IJOB    (input) INTEGER */
/*          Specifies whether condition numbers are required for the */
/*          cluster of eigenvalues (PL and PR) or the deflating subspaces */
/*          (Difu and Difl): */
/*           =0: Only reorder w.r.t. SELECT. No extras. */
/*           =1: Reciprocal of norms of "projections" onto left and right */
/*               eigenspaces w.r.t. the selected cluster (PL and PR). */
/*           =2: Upper bounds on Difu and Difl. F-norm-based estimate */
/*               (DIF(1:2)). */
/*           =3: Estimate of Difu and Difl. 1-norm-based estimate */
/*               (DIF(1:2)). */
/*               About 5 times as expensive as IJOB = 2. */
/*           =4: Compute PL, PR and DIF (i.e. 0, 1 and 2 above): Economic */
/*               version to get it all. */
/*           =5: Compute PL, PR and DIF (i.e. 0, 1 and 3 above) */

/*  WANTQ   (input) LOGICAL */
/*          .TRUE. : update the left transformation matrix Q; */
/*          .FALSE.: do not update Q. */

/*  WANTZ   (input) LOGICAL */
/*          .TRUE. : update the right transformation matrix Z; */
/*          .FALSE.: do not update Z. */

/*  SELECT  (input) LOGICAL array, dimension (N) */
/*          SELECT specifies the eigenvalues in the selected cluster. */
/*          To select a real eigenvalue w(j), SELECT(j) must be set to */
/*          .TRUE.. To select a complex conjugate pair of eigenvalues */
/*          w(j) and w(j+1), corresponding to a 2-by-2 diagonal block, */
/*          either SELECT(j) or SELECT(j+1) or both must be set to */
/*          .TRUE.; a complex conjugate pair of eigenvalues must be */
/*          either both included in the cluster or both excluded. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B. N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension(LDA,N) */
/*          On entry, the upper quasi-triangular matrix A, with (A, B) in */
/*          generalized real Schur canonical form. */
/*          On exit, A is overwritten by the reordered matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension(LDB,N) */
/*          On entry, the upper triangular matrix B, with (A, B) in */
/*          generalized real Schur canonical form. */
/*          On exit, B is overwritten by the reordered matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,N). */

/*  ALPHAR  (output) DOUBLE PRECISION array, dimension (N) */
/*  ALPHAI  (output) DOUBLE PRECISION array, dimension (N) */
/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, (ALPHAR(j) + ALPHAI(j)*i)/BETA(j), j=1,...,N, will */
/*          be the generalized eigenvalues.  ALPHAR(j) + ALPHAI(j)*i */
/*          and BETA(j),j=1,...,N  are the diagonals of the complex Schur */
/*          form (S,T) that would result if the 2-by-2 diagonal blocks of */
/*          the real generalized Schur form of (A,B) were further reduced */
/*          to triangular form using complex unitary transformations. */
/*          If ALPHAI(j) is zero, then the j-th eigenvalue is real; if */
/*          positive, then the j-th and (j+1)-st eigenvalues are a */
/*          complex conjugate pair, with ALPHAI(j+1) negative. */

/*  Q       (input/output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          On entry, if WANTQ = .TRUE., Q is an N-by-N matrix. */
/*          On exit, Q has been postmultiplied by the left orthogonal */
/*          transformation matrix which reorder (A, B); The leading M */
/*          columns of Q form orthonormal bases for the specified pair of */
/*          left eigenspaces (deflating subspaces). */
/*          If WANTQ = .FALSE., Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= 1; */
/*          and if WANTQ = .TRUE., LDQ >= N. */

/*  Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N) */
/*          On entry, if WANTZ = .TRUE., Z is an N-by-N matrix. */
/*          On exit, Z has been postmultiplied by the left orthogonal */
/*          transformation matrix which reorder (A, B); The leading M */
/*          columns of Z form orthonormal bases for the specified pair of */
/*          left eigenspaces (deflating subspaces). */
/*          If WANTZ = .FALSE., Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z. LDZ >= 1; */
/*          If WANTZ = .TRUE., LDZ >= N. */

/*  M       (output) INTEGER */
/*          The dimension of the specified pair of left and right eigen- */
/*          spaces (deflating subspaces). 0 <= M <= N. */

/*  PL      (output) DOUBLE PRECISION */
/*  PR      (output) DOUBLE PRECISION */
/*          If IJOB = 1, 4 or 5, PL, PR are lower bounds on the */
/*          reciprocal of the norm of "projections" onto left and right */
/*          eigenspaces with respect to the selected cluster. */
/*          0 < PL, PR <= 1. */
/*          If M = 0 or M = N, PL = PR  = 1. */
/*          If IJOB = 0, 2 or 3, PL and PR are not referenced. */

/*  DIF     (output) DOUBLE PRECISION array, dimension (2). */
/*          If IJOB >= 2, DIF(1:2) store the estimates of Difu and Difl. */
/*          If IJOB = 2 or 4, DIF(1:2) are F-norm-based upper bounds on */
/*          Difu and Difl. If IJOB = 3 or 5, DIF(1:2) are 1-norm-based */
/*          estimates of Difu and Difl. */
/*          If M = 0 or N, DIF(1:2) = F-norm([A, B]). */
/*          If IJOB = 0 or 1, DIF is not referenced. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, */
/*          dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >=  4*N+16. */
/*          If IJOB = 1, 2 or 4, LWORK >= MAX(4*N+16, 2*M*(N-M)). */
/*          If IJOB = 3 or 5, LWORK >= MAX(4*N+16, 4*M*(N-M)). */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          IF IJOB = 0, IWORK is not referenced.  Otherwise, */
/*          on exit, if INFO = 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. LIWORK >= 1. */
/*          If IJOB = 1, 2 or 4, LIWORK >=  N+6. */
/*          If IJOB = 3 or 5, LIWORK >= MAX(2*M*(N-M), N+6). */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal size of the IWORK array, */
/*          returns this value as the first entry of the IWORK array, and */
/*          no error message related to LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*            =0: Successful exit. */
/*            <0: If INFO = -i, the i-th argument had an illegal value. */
/*            =1: Reordering of (A, B) failed because the transformed */
/*                matrix pair (A, B) would be too far from generalized */
/*                Schur form; the problem is very ill-conditioned. */
/*                (A, B) may have been partially reordered. */
/*                If requested, 0 is returned in DIF(*), PL and PR. */

/*  Further Details */
/*  =============== */

/*  DTGSEN first collects the selected eigenvalues by computing */
/*  orthogonal U and W that move them to the top left corner of (A, B). */
/*  In other words, the selected eigenvalues are the eigenvalues of */
/*  (A11, B11) in: */

/*                U'*(A, B)*W = (A11 A12) (B11 B12) n1 */
/*                              ( 0  A22),( 0  B22) n2 */
/*                                n1  n2    n1  n2 */

/*  where N = n1+n2 and U' means the transpose of U. The first n1 columns */
/*  of U and W span the specified pair of left and right eigenspaces */
/*  (deflating subspaces) of (A, B). */

/*  If (A, B) has been obtained from the generalized real Schur */
/*  decomposition of a matrix pair (C, D) = Q*(A, B)*Z', then the */
/*  reordered generalized real Schur form of (C, D) is given by */

/*           (C, D) = (Q*U)*(U'*(A, B)*W)*(Z*W)', */

/*  and the first n1 columns of Q*U and Z*W span the corresponding */
/*  deflating subspaces of (C, D) (Q and Z store Q*U and Z*W, resp.). */

/*  Note that if the selected eigenvalue is sufficiently ill-conditioned, */
/*  then its value may differ significantly from its value before */
/*  reordering. */

/*  The reciprocal condition numbers of the left and right eigenspaces */
/*  spanned by the first n1 columns of U and W (or Q*U and Z*W) may */
/*  be returned in DIF(1:2), corresponding to Difu and Difl, resp. */

/*  The Difu and Difl are defined as: */

/*       Difu[(A11, B11), (A22, B22)] = sigma-min( Zu ) */
/*  and */
/*       Difl[(A11, B11), (A22, B22)] = Difu[(A22, B22), (A11, B11)], */

/*  where sigma-min(Zu) is the smallest singular value of the */
/*  (2*n1*n2)-by-(2*n1*n2) matrix */

/*       Zu = [ kron(In2, A11)  -kron(A22', In1) ] */
/*            [ kron(In2, B11)  -kron(B22', In1) ]. */

/*  Here, Inx is the identity matrix of size nx and A22' is the */
/*  transpose of A22. kron(X, Y) is the Kronecker product between */
/*  the matrices X and Y. */

/*  When DIF(2) is small, small changes in (A, B) can cause large changes */
/*  in the deflating subspace. An approximate (asymptotic) bound on the */
/*  maximum angular error in the computed deflating subspaces is */

/*       EPS * norm((A, B)) / DIF(2), */

/*  where EPS is the machine precision. */

/*  The reciprocal norm of the projectors on the left and right */
/*  eigenspaces associated with (A11, B11) may be returned in PL and PR. */
/*  They are computed as follows. First we compute L and R so that */
/*  P*(A, B)*Q is block diagonal, where */

/*       P = ( I -L ) n1           Q = ( I R ) n1 */
/*           ( 0  I ) n2    and        ( 0 I ) n2 */
/*             n1 n2                    n1 n2 */

/*  and (L, R) is the solution to the generalized Sylvester equation */

/*       A11*R - L*A22 = -A12 */
/*       B11*R - L*B22 = -B12 */

/*  Then PL = (F-norm(L)**2+1)**(-1/2) and PR = (F-norm(R)**2+1)**(-1/2). */
/*  An approximate (asymptotic) bound on the average absolute error of */
/*  the selected eigenvalues is */

/*       EPS * norm((A, B)) / PL. */

/*  There are also global error bounds which valid for perturbations up */
/*  to a certain restriction:  A lower bound (x) on the smallest */
/*  F-norm(E,F) for which an eigenvalue of (A11, B11) may move and */
/*  coalesce with an eigenvalue of (A22, B22) under perturbation (E,F), */
/*  (i.e. (A + E, B + F), is */

/*   x = min(Difu,Difl)/((1/(PL*PL)+1/(PR*PR))**(1/2)+2*max(1/PL,1/PR)). */

/*  An approximate bound on x can be computed from DIF(1:2), PL and PR. */

/*  If y = ( F-norm(E,F) / x) <= 1, the angles between the perturbed */
/*  (L', R') and unperturbed (L, R) left and right deflating subspaces */
/*  associated with the selected cluster in the (1,1)-blocks can be */
/*  bounded as */

/*   max-angle(L, L') <= arctan( y * PL / (1 - y * (1 - PL * PL)**(1/2)) */
/*   max-angle(R, R') <= arctan( y * PR / (1 - y * (1 - PR * PR)**(1/2)) */

/*  See LAPACK User's Guide section 4.11 or the following references */
/*  for more information. */

/*  Note that if the default method for computing the Frobenius-norm- */
/*  based estimate DIF is not wanted (see DLATDF), then the parameter */
/*  IDIFJB (see below) should be changed from 3 to 4 (routine DLATDF */
/*  (IJOB = 2 will be used)). See DTGSYL for more details. */

/*  Based on contributions by */
/*     Bo Kagstrom and Peter Poromaa, Department of Computing Science, */
/*     Umea University, S-901 87 Umea, Sweden. */

/*  References */
/*  ========== */

/*  [1] B. Kagstrom; A Direct Method for Reordering Eigenvalues in the */
/*      Generalized Real Schur Form of a Regular Matrix Pair (A, B), in */
/*      M.S. Moonen et al (eds), Linear Algebra for Large Scale and */
/*      Real-Time Applications, Kluwer Academic Publ. 1993, pp 195-218. */

/*  [2] B. Kagstrom and P. Poromaa; Computing Eigenspaces with Specified */
/*      Eigenvalues of a Regular Matrix Pair (A, B) and Condition */
/*      Estimation: Theory, Algorithms and Software, */
/*      Report UMINF - 94.04, Department of Computing Science, Umea */
/*      University, S-901 87 Umea, Sweden, 1994. Also as LAPACK Working */
/*      Note 87. To appear in Numerical Algorithms, 1996. */

/*  [3] B. Kagstrom and P. Poromaa, LAPACK-Style Algorithms and Software */
/*      for Solving the Generalized Sylvester Equation and Estimating the */
/*      Separation between Regular Matrix Pairs, Report UMINF - 93.23, */
/*      Department of Computing Science, Umea University, S-901 87 Umea, */
/*      Sweden, December 1993, Revised April 1994, Also as LAPACK Working */
/*      Note 75. To appear in ACM Trans. on Math. Software, Vol 22, No 1, */
/*      1996. */

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

/*     Decode and test the input parameters */

    /* Parameter adjustments */
    --select;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --alphar;
    --alphai;
    --beta;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --dif;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1 || *liwork == -1;

    if (*ijob < 0 || *ijob > 5) {
	*info = -1;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -9;
    } else if (*ldq < 1 || *wantq && *ldq < *n) {
	*info = -14;
    } else if (*ldz < 1 || *wantz && *ldz < *n) {
	*info = -16;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTGSEN", &i__1);
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    ierr = 0;

    wantp = *ijob == 1 || *ijob >= 4;
    wantd1 = *ijob == 2 || *ijob == 4;
    wantd2 = *ijob == 3 || *ijob == 5;
    wantd = wantd1 || wantd2;

/*     Set M to the dimension of the specified pair of deflating */
/*     subspaces. */

    *m = 0;
    pair = false;
    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {
	if (pair) {
	    pair = false;
	} else {
	    if (k < *n) {
		if (a[k + 1 + k * a_dim1] == 0.) {
		    if (select[k]) {
			++(*m);
		    }
		} else {
		    pair = true;
		    if (select[k] || select[k + 1]) {
			*m += 2;
		    }
		}
	    } else {
		if (select[*n]) {
		    ++(*m);
		}
	    }
	}
/* L10: */
    }

    if (*ijob == 1 || *ijob == 2 || *ijob == 4) {
/* Computing MAX */
	i__1 = 1, i__2 = (*n << 2) + 16, i__1 = std::max(i__1,i__2), i__2 = (*m <<
		1) * (*n - *m);
	lwmin = std::max(i__1,i__2);
/* Computing MAX */
	i__1 = 1, i__2 = *n + 6;
	liwmin = std::max(i__1,i__2);
    } else if (*ijob == 3 || *ijob == 5) {
/* Computing MAX */
	i__1 = 1, i__2 = (*n << 2) + 16, i__1 = std::max(i__1,i__2), i__2 = (*m <<
		2) * (*n - *m);
	lwmin = std::max(i__1,i__2);
/* Computing MAX */
	i__1 = 1, i__2 = (*m << 1) * (*n - *m), i__1 = std::max(i__1,i__2), i__2 =
		*n + 6;
	liwmin = std::max(i__1,i__2);
    } else {
/* Computing MAX */
	i__1 = 1, i__2 = (*n << 2) + 16;
	lwmin = std::max(i__1,i__2);
	liwmin = 1;
    }

    work[1] = (double) lwmin;
    iwork[1] = liwmin;

    if (*lwork < lwmin && ! lquery) {
	*info = -22;
    } else if (*liwork < liwmin && ! lquery) {
	*info = -24;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTGSEN", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible. */

    if (*m == *n || *m == 0) {
	if (wantp) {
	    *pl = 1.;
	    *pr = 1.;
	}
	if (wantd) {
	    dscale = 0.;
	    dsum = 1.;
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		dlassq_(n, &a[i__ * a_dim1 + 1], &c__1, &dscale, &dsum);
		dlassq_(n, &b[i__ * b_dim1 + 1], &c__1, &dscale, &dsum);
/* L20: */
	    }
	    dif[1] = dscale * sqrt(dsum);
	    dif[2] = dif[1];
	}
	goto L60;
    }

/*     Collect the selected blocks at the top-left corner of (A, B). */

    ks = 0;
    pair = false;
    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {
	if (pair) {
	    pair = false;
	} else {

	    swap = select[k];
	    if (k < *n) {
		if (a[k + 1 + k * a_dim1] != 0.) {
		    pair = true;
		    swap = swap || select[k + 1];
		}
	    }

	    if (swap) {
		++ks;

/*              Swap the K-th block to position KS. */
/*              Perform the reordering of diagonal blocks in (A, B) */
/*              by orthogonal transformation matrices and update */
/*              Q and Z accordingly (if requested): */

		kk = k;
		if (k != ks) {
		    dtgexc_(wantq, wantz, n, &a[a_offset], lda, &b[b_offset],
			    ldb, &q[q_offset], ldq, &z__[z_offset], ldz, &kk,
			    &ks, &work[1], lwork, &ierr);
		}

		if (ierr > 0) {

/*                 Swap is rejected: exit. */

		    *info = 1;
		    if (wantp) {
			*pl = 0.;
			*pr = 0.;
		    }
		    if (wantd) {
			dif[1] = 0.;
			dif[2] = 0.;
		    }
		    goto L60;
		}

		if (pair) {
		    ++ks;
		}
	    }
	}
/* L30: */
    }
    if (wantp) {

/*        Solve generalized Sylvester equation for R and L */
/*        and compute PL and PR. */

	n1 = *m;
	n2 = *n - *m;
	i__ = n1 + 1;
	ijb = 0;
	dlacpy_("Full", &n1, &n2, &a[i__ * a_dim1 + 1], lda, &work[1], &n1);
	dlacpy_("Full", &n1, &n2, &b[i__ * b_dim1 + 1], ldb, &work[n1 * n2 +
		1], &n1);
	i__1 = *lwork - (n1 << 1) * n2;
	dtgsyl_("N", &ijb, &n1, &n2, &a[a_offset], lda, &a[i__ + i__ * a_dim1]
, lda, &work[1], &n1, &b[b_offset], ldb, &b[i__ + i__ *
		b_dim1], ldb, &work[n1 * n2 + 1], &n1, &dscale, &dif[1], &
		work[(n1 * n2 << 1) + 1], &i__1, &iwork[1], &ierr);

/*        Estimate the reciprocal of norms of "projections" onto left */
/*        and right eigenspaces. */

	rdscal = 0.;
	dsum = 1.;
	i__1 = n1 * n2;
	dlassq_(&i__1, &work[1], &c__1, &rdscal, &dsum);
	*pl = rdscal * sqrt(dsum);
	if (*pl == 0.) {
	    *pl = 1.;
	} else {
	    *pl = dscale / (sqrt(dscale * dscale / *pl + *pl) * sqrt(*pl));
	}
	rdscal = 0.;
	dsum = 1.;
	i__1 = n1 * n2;
	dlassq_(&i__1, &work[n1 * n2 + 1], &c__1, &rdscal, &dsum);
	*pr = rdscal * sqrt(dsum);
	if (*pr == 0.) {
	    *pr = 1.;
	} else {
	    *pr = dscale / (sqrt(dscale * dscale / *pr + *pr) * sqrt(*pr));
	}
    }

    if (wantd) {

/*        Compute estimates of Difu and Difl. */

	if (wantd1) {
	    n1 = *m;
	    n2 = *n - *m;
	    i__ = n1 + 1;
	    ijb = 3;

/*           Frobenius norm-based Difu-estimate. */

	    i__1 = *lwork - (n1 << 1) * n2;
	    dtgsyl_("N", &ijb, &n1, &n2, &a[a_offset], lda, &a[i__ + i__ *
		    a_dim1], lda, &work[1], &n1, &b[b_offset], ldb, &b[i__ +
		    i__ * b_dim1], ldb, &work[n1 * n2 + 1], &n1, &dscale, &
		    dif[1], &work[(n1 << 1) * n2 + 1], &i__1, &iwork[1], &
		    ierr);

/*           Frobenius norm-based Difl-estimate. */

	    i__1 = *lwork - (n1 << 1) * n2;
	    dtgsyl_("N", &ijb, &n2, &n1, &a[i__ + i__ * a_dim1], lda, &a[
		    a_offset], lda, &work[1], &n2, &b[i__ + i__ * b_dim1],
		    ldb, &b[b_offset], ldb, &work[n1 * n2 + 1], &n2, &dscale,
		    &dif[2], &work[(n1 << 1) * n2 + 1], &i__1, &iwork[1], &
		    ierr);
	} else {


/*           Compute 1-norm-based estimates of Difu and Difl using */
/*           reversed communication with DLACN2. In each step a */
/*           generalized Sylvester equation or a transposed variant */
/*           is solved. */

	    kase = 0;
	    n1 = *m;
	    n2 = *n - *m;
	    i__ = n1 + 1;
	    ijb = 0;
	    mn2 = (n1 << 1) * n2;

/*           1-norm-based estimate of Difu. */

L40:
	    dlacn2_(&mn2, &work[mn2 + 1], &work[1], &iwork[1], &dif[1], &kase,
		     isave);
	    if (kase != 0) {
		if (kase == 1) {

/*                 Solve generalized Sylvester equation. */

		    i__1 = *lwork - (n1 << 1) * n2;
		    dtgsyl_("N", &ijb, &n1, &n2, &a[a_offset], lda, &a[i__ +
			    i__ * a_dim1], lda, &work[1], &n1, &b[b_offset],
			    ldb, &b[i__ + i__ * b_dim1], ldb, &work[n1 * n2 +
			    1], &n1, &dscale, &dif[1], &work[(n1 << 1) * n2 +
			    1], &i__1, &iwork[1], &ierr);
		} else {

/*                 Solve the transposed variant. */

		    i__1 = *lwork - (n1 << 1) * n2;
		    dtgsyl_("T", &ijb, &n1, &n2, &a[a_offset], lda, &a[i__ +
			    i__ * a_dim1], lda, &work[1], &n1, &b[b_offset],
			    ldb, &b[i__ + i__ * b_dim1], ldb, &work[n1 * n2 +
			    1], &n1, &dscale, &dif[1], &work[(n1 << 1) * n2 +
			    1], &i__1, &iwork[1], &ierr);
		}
		goto L40;
	    }
	    dif[1] = dscale / dif[1];

/*           1-norm-based estimate of Difl. */

L50:
	    dlacn2_(&mn2, &work[mn2 + 1], &work[1], &iwork[1], &dif[2], &kase,
		     isave);
	    if (kase != 0) {
		if (kase == 1) {

/*                 Solve generalized Sylvester equation. */

		    i__1 = *lwork - (n1 << 1) * n2;
		    dtgsyl_("N", &ijb, &n2, &n1, &a[i__ + i__ * a_dim1], lda,
			    &a[a_offset], lda, &work[1], &n2, &b[i__ + i__ *
			    b_dim1], ldb, &b[b_offset], ldb, &work[n1 * n2 +
			    1], &n2, &dscale, &dif[2], &work[(n1 << 1) * n2 +
			    1], &i__1, &iwork[1], &ierr);
		} else {

/*                 Solve the transposed variant. */

		    i__1 = *lwork - (n1 << 1) * n2;
		    dtgsyl_("T", &ijb, &n2, &n1, &a[i__ + i__ * a_dim1], lda,
			    &a[a_offset], lda, &work[1], &n2, &b[i__ + i__ *
			    b_dim1], ldb, &b[b_offset], ldb, &work[n1 * n2 +
			    1], &n2, &dscale, &dif[2], &work[(n1 << 1) * n2 +
			    1], &i__1, &iwork[1], &ierr);
		}
		goto L50;
	    }
	    dif[2] = dscale / dif[2];

	}
    }

L60:

/*     Compute generalized eigenvalues of reordered pair (A, B) and */
/*     normalize the generalized Schur form. */

    pair = false;
    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {
	if (pair) {
	    pair = false;
	} else {

	    if (k < *n) {
		if (a[k + 1 + k * a_dim1] != 0.) {
		    pair = true;
		}
	    }

	    if (pair) {

/*             Compute the eigenvalue(s) at position K. */

		work[1] = a[k + k * a_dim1];
		work[2] = a[k + 1 + k * a_dim1];
		work[3] = a[k + (k + 1) * a_dim1];
		work[4] = a[k + 1 + (k + 1) * a_dim1];
		work[5] = b[k + k * b_dim1];
		work[6] = b[k + 1 + k * b_dim1];
		work[7] = b[k + (k + 1) * b_dim1];
		work[8] = b[k + 1 + (k + 1) * b_dim1];
		d__1 = smlnum * eps;
		dlag2_(&work[1], &c__2, &work[5], &c__2, &d__1, &beta[k], &
			beta[k + 1], &alphar[k], &alphar[k + 1], &alphai[k]);
		alphai[k + 1] = -alphai[k];

	    } else {

		if (d_sign(&c_b28, &b[k + k * b_dim1]) < 0.) {

/*                 If B(K,K) is negative, make it positive */

		    i__2 = *n;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			a[k + i__ * a_dim1] = -a[k + i__ * a_dim1];
			b[k + i__ * b_dim1] = -b[k + i__ * b_dim1];
			q[i__ + k * q_dim1] = -q[i__ + k * q_dim1];
/* L70: */
		    }
		}

		alphar[k] = a[k + k * a_dim1];
		alphai[k] = 0.;
		beta[k] = b[k + k * b_dim1];

	    }
	}
/* L80: */
    }

    work[1] = (double) lwmin;
    iwork[1] = liwmin;

    return 0;

/*     End of DTGSEN */

} /* dtgsen_ */

/* Subroutine */ int dtgsja_(const char *jobu, const char *jobv, const char *jobq, integer *m,
	integer *p, integer *n, integer *k, integer *l, double *a,
	integer *lda, double *b, integer *ldb, double *tola,
	double *tolb, double *alpha, double *beta, double *u,
	integer *ldu, double *v, integer *ldv, double *q, integer *
	ldq, double *work, integer *ncycle, integer *info)
{
	/* Table of constant values */
	static double c_b13 = 0.;
	static double c_b14 = 1.;
	static integer c__1 = 1;
	static double c_b43 = -1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, u_dim1,
	    u_offset, v_dim1, v_offset, i__1, i__2, i__3, i__4;
    double d__1;

    /* Local variables */
    integer i__, j;
    double a1, a2, a3, b1, b2, b3, csq, csu, csv, snq, rwk, snu, snv;
    double gamma;
    bool initq, initu, initv, wantq, upper;
    double error, ssmin;
    bool wantu, wantv;
    integer kcycle;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTGSJA computes the generalized singular value decomposition (GSVD) */
/*  of two real upper triangular (or trapezoidal) matrices A and B. */

/*  On entry, it is assumed that matrices A and B have the following */
/*  forms, which may be obtained by the preprocessing subroutine DGGSVP */
/*  from a general M-by-N matrix A and P-by-N matrix B: */

/*               N-K-L  K    L */
/*     A =    K ( 0    A12  A13 ) if M-K-L >= 0; */
/*            L ( 0     0   A23 ) */
/*        M-K-L ( 0     0    0  ) */

/*             N-K-L  K    L */
/*     A =  K ( 0    A12  A13 ) if M-K-L < 0; */
/*        M-K ( 0     0   A23 ) */

/*             N-K-L  K    L */
/*     B =  L ( 0     0   B13 ) */
/*        P-L ( 0     0    0  ) */

/*  where the K-by-K matrix A12 and L-by-L matrix B13 are nonsingular */
/*  upper triangular; A23 is L-by-L upper triangular if M-K-L >= 0, */
/*  otherwise A23 is (M-K)-by-L upper trapezoidal. */

/*  On exit, */

/*              U'*A*Q = D1*( 0 R ),    V'*B*Q = D2*( 0 R ), */

/*  where U, V and Q are orthogonal matrices, Z' denotes the transpose */
/*  of Z, R is a nonsingular upper triangular matrix, and D1 and D2 are */
/*  ``diagonal'' matrices, which are of the following structures: */

/*  If M-K-L >= 0, */

/*                      K  L */
/*         D1 =     K ( I  0 ) */
/*                  L ( 0  C ) */
/*              M-K-L ( 0  0 ) */

/*                    K  L */
/*         D2 = L   ( 0  S ) */
/*              P-L ( 0  0 ) */

/*                 N-K-L  K    L */
/*    ( 0 R ) = K (  0   R11  R12 ) K */
/*              L (  0    0   R22 ) L */

/*  where */

/*    C = diag( ALPHA(K+1), ... , ALPHA(K+L) ), */
/*    S = diag( BETA(K+1),  ... , BETA(K+L) ), */
/*    C**2 + S**2 = I. */

/*    R is stored in A(1:K+L,N-K-L+1:N) on exit. */

/*  If M-K-L < 0, */

/*                 K M-K K+L-M */
/*      D1 =   K ( I  0    0   ) */
/*           M-K ( 0  C    0   ) */

/*                   K M-K K+L-M */
/*      D2 =   M-K ( 0  S    0   ) */
/*           K+L-M ( 0  0    I   ) */
/*             P-L ( 0  0    0   ) */

/*                 N-K-L  K   M-K  K+L-M */
/* ( 0 R ) =    K ( 0    R11  R12  R13  ) */
/*            M-K ( 0     0   R22  R23  ) */
/*          K+L-M ( 0     0    0   R33  ) */

/*  where */
/*  C = diag( ALPHA(K+1), ... , ALPHA(M) ), */
/*  S = diag( BETA(K+1),  ... , BETA(M) ), */
/*  C**2 + S**2 = I. */

/*  R = ( R11 R12 R13 ) is stored in A(1:M, N-K-L+1:N) and R33 is stored */
/*      (  0  R22 R23 ) */
/*  in B(M-K+1:L,N+M-K-L+1:N) on exit. */

/*  The computation of the orthogonal transformation matrices U, V or Q */
/*  is optional.  These matrices may either be formed explicitly, or they */
/*  may be postmultiplied into input matrices U1, V1, or Q1. */

/*  Arguments */
/*  ========= */

/*  JOBU    (input) CHARACTER*1 */
/*          = 'U':  U must contain an orthogonal matrix U1 on entry, and */
/*                  the product U1*U is returned; */
/*          = 'I':  U is initialized to the unit matrix, and the */
/*                  orthogonal matrix U is returned; */
/*          = 'N':  U is not computed. */

/*  JOBV    (input) CHARACTER*1 */
/*          = 'V':  V must contain an orthogonal matrix V1 on entry, and */
/*                  the product V1*V is returned; */
/*          = 'I':  V is initialized to the unit matrix, and the */
/*                  orthogonal matrix V is returned; */
/*          = 'N':  V is not computed. */

/*  JOBQ    (input) CHARACTER*1 */
/*          = 'Q':  Q must contain an orthogonal matrix Q1 on entry, and */
/*                  the product Q1*Q is returned; */
/*          = 'I':  Q is initialized to the unit matrix, and the */
/*                  orthogonal matrix Q is returned; */
/*          = 'N':  Q is not computed. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B.  P >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B.  N >= 0. */

/*  K       (input) INTEGER */
/*  L       (input) INTEGER */
/*          K and L specify the subblocks in the input matrices A and B: */
/*          A23 = A(K+1:MIN(K+L,M),N-L+1:N) and B13 = B(1:L,N-L+1:N) */
/*          of A and B, whose GSVD is going to be computed by DTGSJA. */
/*          See Further details. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, A(N-K+1:N,1:MIN(K+L,M) ) contains the triangular */
/*          matrix R or part of R.  See Purpose for details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,N) */
/*          On entry, the P-by-N matrix B. */
/*          On exit, if necessary, B(M-K+1:L,N+M-K-L+1:N) contains */
/*          a part of R.  See Purpose for details. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,P). */

/*  TOLA    (input) DOUBLE PRECISION */
/*  TOLB    (input) DOUBLE PRECISION */
/*          TOLA and TOLB are the convergence criteria for the Jacobi- */
/*          Kogbetliantz iteration procedure. Generally, they are the */
/*          same as used in the preprocessing step, say */
/*              TOLA = max(M,N)*norm(A)*MAZHEPS, */
/*              TOLB = max(P,N)*norm(B)*MAZHEPS. */

/*  ALPHA   (output) DOUBLE PRECISION array, dimension (N) */
/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, ALPHA and BETA contain the generalized singular */
/*          value pairs of A and B; */
/*            ALPHA(1:K) = 1, */
/*            BETA(1:K)  = 0, */
/*          and if M-K-L >= 0, */
/*            ALPHA(K+1:K+L) = diag(C), */
/*            BETA(K+1:K+L)  = diag(S), */
/*          or if M-K-L < 0, */
/*            ALPHA(K+1:M)= C, ALPHA(M+1:K+L)= 0 */
/*            BETA(K+1:M) = S, BETA(M+1:K+L) = 1. */
/*          Furthermore, if K+L < N, */
/*            ALPHA(K+L+1:N) = 0 and */
/*            BETA(K+L+1:N)  = 0. */

/*  U       (input/output) DOUBLE PRECISION array, dimension (LDU,M) */
/*          On entry, if JOBU = 'U', U must contain a matrix U1 (usually */
/*          the orthogonal matrix returned by DGGSVP). */
/*          On exit, */
/*          if JOBU = 'I', U contains the orthogonal matrix U; */
/*          if JOBU = 'U', U contains the product U1*U. */
/*          If JOBU = 'N', U is not referenced. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U. LDU >= max(1,M) if */
/*          JOBU = 'U'; LDU >= 1 otherwise. */

/*  V       (input/output) DOUBLE PRECISION array, dimension (LDV,P) */
/*          On entry, if JOBV = 'V', V must contain a matrix V1 (usually */
/*          the orthogonal matrix returned by DGGSVP). */
/*          On exit, */
/*          if JOBV = 'I', V contains the orthogonal matrix V; */
/*          if JOBV = 'V', V contains the product V1*V. */
/*          If JOBV = 'N', V is not referenced. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. LDV >= max(1,P) if */
/*          JOBV = 'V'; LDV >= 1 otherwise. */

/*  Q       (input/output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          On entry, if JOBQ = 'Q', Q must contain a matrix Q1 (usually */
/*          the orthogonal matrix returned by DGGSVP). */
/*          On exit, */
/*          if JOBQ = 'I', Q contains the orthogonal matrix Q; */
/*          if JOBQ = 'Q', Q contains the product Q1*Q. */
/*          If JOBQ = 'N', Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. LDQ >= max(1,N) if */
/*          JOBQ = 'Q'; LDQ >= 1 otherwise. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*N) */

/*  NCYCLE  (output) INTEGER */
/*          The number of cycles required for convergence. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          = 1:  the procedure does not converge after MAXIT cycles. */

/*  Internal Parameters */
/*  =================== */

/*  MAXIT   INTEGER */
/*          MAXIT specifies the total loops that the iterative procedure */
/*          may take. If after MAXIT cycles, the routine fails to */
/*          converge, we return INFO = 1. */

/*  Further Details */
/*  =============== */

/*  DTGSJA essentially uses a variant of Kogbetliantz algorithm to reduce */
/*  min(L,M-K)-by-L triangular (or trapezoidal) matrix A23 and L-by-L */
/*  matrix B13 to the form: */

/*           U1'*A13*Q1 = C1*R1; V1'*B13*Q1 = S1*R1, */

/*  where U1, V1 and Q1 are orthogonal matrix, and Z' is the transpose */
/*  of Z.  C1 and S1 are diagonal matrices satisfying */

/*                C1**2 + S1**2 = I, */

/*  and R1 is an L-by-L nonsingular upper triangular matrix. */

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

/*     Decode and test the input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --alpha;
    --beta;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --work;

    /* Function Body */
    initu = lsame_(jobu, "I");
    wantu = initu || lsame_(jobu, "U");

    initv = lsame_(jobv, "I");
    wantv = initv || lsame_(jobv, "V");

    initq = lsame_(jobq, "I");
    wantq = initq || lsame_(jobq, "Q");

    *info = 0;
    if (! (initu || wantu || lsame_(jobu, "N"))) {
	*info = -1;
    } else if (! (initv || wantv || lsame_(jobv, "N")))
	    {
	*info = -2;
    } else if (! (initq || wantq || lsame_(jobq, "N")))
	    {
	*info = -3;
    } else if (*m < 0) {
	*info = -4;
    } else if (*p < 0) {
	*info = -5;
    } else if (*n < 0) {
	*info = -6;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -10;
    } else if (*ldb < std::max(1_integer,*p)) {
	*info = -12;
    } else if (*ldu < 1 || wantu && *ldu < *m) {
	*info = -18;
    } else if (*ldv < 1 || wantv && *ldv < *p) {
	*info = -20;
    } else if (*ldq < 1 || wantq && *ldq < *n) {
	*info = -22;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTGSJA", &i__1);
	return 0;
    }

/*     Initialize U, V and Q, if necessary */

    if (initu) {
	dlaset_("Full", m, m, &c_b13, &c_b14, &u[u_offset], ldu);
    }
    if (initv) {
	dlaset_("Full", p, p, &c_b13, &c_b14, &v[v_offset], ldv);
    }
    if (initq) {
	dlaset_("Full", n, n, &c_b13, &c_b14, &q[q_offset], ldq);
    }

/*     Loop until convergence */

    upper = false;
    for (kcycle = 1; kcycle <= 40; ++kcycle) {

	upper = ! upper;

	i__1 = *l - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *l;
	    for (j = i__ + 1; j <= i__2; ++j) {

		a1 = 0.;
		a2 = 0.;
		a3 = 0.;
		if (*k + i__ <= *m) {
		    a1 = a[*k + i__ + (*n - *l + i__) * a_dim1];
		}
		if (*k + j <= *m) {
		    a3 = a[*k + j + (*n - *l + j) * a_dim1];
		}

		b1 = b[i__ + (*n - *l + i__) * b_dim1];
		b3 = b[j + (*n - *l + j) * b_dim1];

		if (upper) {
		    if (*k + i__ <= *m) {
			a2 = a[*k + i__ + (*n - *l + j) * a_dim1];
		    }
		    b2 = b[i__ + (*n - *l + j) * b_dim1];
		} else {
		    if (*k + j <= *m) {
			a2 = a[*k + j + (*n - *l + i__) * a_dim1];
		    }
		    b2 = b[j + (*n - *l + i__) * b_dim1];
		}

		dlags2_(&upper, &a1, &a2, &a3, &b1, &b2, &b3, &csu, &snu, &
			csv, &snv, &csq, &snq);

/*              Update (K+I)-th and (K+J)-th rows of matrix A: U'*A */

		if (*k + j <= *m) {
		    drot_(l, &a[*k + j + (*n - *l + 1) * a_dim1], lda, &a[*k
			    + i__ + (*n - *l + 1) * a_dim1], lda, &csu, &snu);
		}

/*              Update I-th and J-th rows of matrix B: V'*B */

		drot_(l, &b[j + (*n - *l + 1) * b_dim1], ldb, &b[i__ + (*n - *
			l + 1) * b_dim1], ldb, &csv, &snv);

/*              Update (N-L+I)-th and (N-L+J)-th columns of matrices */
/*              A and B: A*Q and B*Q */

/* Computing MIN */
		i__4 = *k + *l;
		i__3 = std::min(i__4,*m);
		drot_(&i__3, &a[(*n - *l + j) * a_dim1 + 1], &c__1, &a[(*n - *
			l + i__) * a_dim1 + 1], &c__1, &csq, &snq);

		drot_(l, &b[(*n - *l + j) * b_dim1 + 1], &c__1, &b[(*n - *l +
			i__) * b_dim1 + 1], &c__1, &csq, &snq);

		if (upper) {
		    if (*k + i__ <= *m) {
			a[*k + i__ + (*n - *l + j) * a_dim1] = 0.;
		    }
		    b[i__ + (*n - *l + j) * b_dim1] = 0.;
		} else {
		    if (*k + j <= *m) {
			a[*k + j + (*n - *l + i__) * a_dim1] = 0.;
		    }
		    b[j + (*n - *l + i__) * b_dim1] = 0.;
		}

/*              Update orthogonal matrices U, V, Q, if desired. */

		if (wantu && *k + j <= *m) {
		    drot_(m, &u[(*k + j) * u_dim1 + 1], &c__1, &u[(*k + i__) *
			     u_dim1 + 1], &c__1, &csu, &snu);
		}

		if (wantv) {
		    drot_(p, &v[j * v_dim1 + 1], &c__1, &v[i__ * v_dim1 + 1],
			    &c__1, &csv, &snv);
		}

		if (wantq) {
		    drot_(n, &q[(*n - *l + j) * q_dim1 + 1], &c__1, &q[(*n - *
			    l + i__) * q_dim1 + 1], &c__1, &csq, &snq);
		}

/* L10: */
	    }
/* L20: */
	}

	if (! upper) {

/*           The matrices A13 and B13 were lower triangular at the start */
/*           of the cycle, and are now upper triangular. */

/*           Convergence test: test the parallelism of the corresponding */
/*           rows of A and B. */

	    error = 0.;
/* Computing MIN */
	    i__2 = *l, i__3 = *m - *k;
	    i__1 = std::min(i__2,i__3);
	    for (i__ = 1; i__ <= i__1; ++i__) {
		i__2 = *l - i__ + 1;
		dcopy_(&i__2, &a[*k + i__ + (*n - *l + i__) * a_dim1], lda, &
			work[1], &c__1);
		i__2 = *l - i__ + 1;
		dcopy_(&i__2, &b[i__ + (*n - *l + i__) * b_dim1], ldb, &work[*
			l + 1], &c__1);
		i__2 = *l - i__ + 1;
		dlapll_(&i__2, &work[1], &c__1, &work[*l + 1], &c__1, &ssmin);
		error = std::max(error,ssmin);
/* L30: */
	    }

	    if (abs(error) <= std::min(*tola,*tolb)) {
		goto L50;
	    }
	}

/*        End of cycle loop */

/* L40: */
    }

/*     The algorithm has not converged after MAXIT cycles. */

    *info = 1;
    goto L100;

L50:

/*     If ERROR <= MIN(TOLA,TOLB), then the algorithm has converged. */
/*     Compute the generalized singular value pairs (ALPHA, BETA), and */
/*     set the triangular matrix R to array A. */

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	alpha[i__] = 1.;
	beta[i__] = 0.;
/* L60: */
    }

/* Computing MIN */
    i__2 = *l, i__3 = *m - *k;
    i__1 = std::min(i__2,i__3);
    for (i__ = 1; i__ <= i__1; ++i__) {

	a1 = a[*k + i__ + (*n - *l + i__) * a_dim1];
	b1 = b[i__ + (*n - *l + i__) * b_dim1];

	if (a1 != 0.) {
	    gamma = b1 / a1;

/*           change sign if necessary */

	    if (gamma < 0.) {
		i__2 = *l - i__ + 1;
		dscal_(&i__2, &c_b43, &b[i__ + (*n - *l + i__) * b_dim1], ldb)
			;
		if (wantv) {
		    dscal_(p, &c_b43, &v[i__ * v_dim1 + 1], &c__1);
		}
	    }

	    d__1 = abs(gamma);
	    dlartg_(&d__1, &c_b14, &beta[*k + i__], &alpha[*k + i__], &rwk);

	    if (alpha[*k + i__] >= beta[*k + i__]) {
		i__2 = *l - i__ + 1;
		d__1 = 1. / alpha[*k + i__];
		dscal_(&i__2, &d__1, &a[*k + i__ + (*n - *l + i__) * a_dim1],
			lda);
	    } else {
		i__2 = *l - i__ + 1;
		d__1 = 1. / beta[*k + i__];
		dscal_(&i__2, &d__1, &b[i__ + (*n - *l + i__) * b_dim1], ldb);
		i__2 = *l - i__ + 1;
		dcopy_(&i__2, &b[i__ + (*n - *l + i__) * b_dim1], ldb, &a[*k
			+ i__ + (*n - *l + i__) * a_dim1], lda);
	    }

	} else {

	    alpha[*k + i__] = 0.;
	    beta[*k + i__] = 1.;
	    i__2 = *l - i__ + 1;
	    dcopy_(&i__2, &b[i__ + (*n - *l + i__) * b_dim1], ldb, &a[*k +
		    i__ + (*n - *l + i__) * a_dim1], lda);

	}

/* L70: */
    }

/*     Post-assignment */

    i__1 = *k + *l;
    for (i__ = *m + 1; i__ <= i__1; ++i__) {
	alpha[i__] = 0.;
	beta[i__] = 1.;
/* L80: */
    }

    if (*k + *l < *n) {
	i__1 = *n;
	for (i__ = *k + *l + 1; i__ <= i__1; ++i__) {
	    alpha[i__] = 0.;
	    beta[i__] = 0.;
/* L90: */
	}
    }

L100:
    *ncycle = kcycle;
    return 0;

/*     End of DTGSJA */

} /* dtgsja_ */

/* Subroutine */ int dtgsna_(const char *job, const char *howmny, bool *select,
	integer *n, double *a, integer *lda, double *b, integer *ldb,
	double *vl, integer *ldvl, double *vr, integer *ldvr,
	double *s, double *dif, integer *mm, integer *m, double *
	work, integer *lwork, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b19 = 1.;
	static double c_b21 = 0.;
	static integer c__2 = 2;
	static bool c_false = false;
	static integer c__3 = 3;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, vl_dim1, vl_offset, vr_dim1,
	    vr_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, k;
    double c1, c2;
    integer n1, n2, ks, iz;
    double eps, beta, cond;
    bool pair;
    integer ierr;
    double uhav, uhbv;
    integer ifst;
    double lnrm;
    integer ilst;
    double rnrm;
    double root1, root2, scale;
    double uhavi, uhbvi, tmpii;
    integer lwmin;
    bool wants;
    double tmpir, tmpri, dummy[1], tmprr;
    double dummy1[1];
    double alphai, alphar;
    bool wantbh, wantdf, somcon;
    double alprqt;
    double smlnum;
    bool lquery;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTGSNA estimates reciprocal condition numbers for specified */
/*  eigenvalues and/or eigenvectors of a matrix pair (A, B) in */
/*  generalized real Schur canonical form (or of any matrix pair */
/*  (Q*A*Z', Q*B*Z') with orthogonal matrices Q and Z, where */
/*  Z' denotes the transpose of Z. */

/*  (A, B) must be in generalized real Schur form (as returned by DGGES), */
/*  i.e. A is block upper triangular with 1-by-1 and 2-by-2 diagonal */
/*  blocks. B is upper triangular. */


/*  Arguments */
/*  ========= */

/*  JOB     (input) CHARACTER*1 */
/*          Specifies whether condition numbers are required for */
/*          eigenvalues (S) or eigenvectors (DIF): */
/*          = 'E': for eigenvalues only (S); */
/*          = 'V': for eigenvectors only (DIF); */
/*          = 'B': for both eigenvalues and eigenvectors (S and DIF). */

/*  HOWMNY  (input) CHARACTER*1 */
/*          = 'A': compute condition numbers for all eigenpairs; */
/*          = 'S': compute condition numbers for selected eigenpairs */
/*                 specified by the array SELECT. */

/*  SELECT  (input) LOGICAL array, dimension (N) */
/*          If HOWMNY = 'S', SELECT specifies the eigenpairs for which */
/*          condition numbers are required. To select condition numbers */
/*          for the eigenpair corresponding to a real eigenvalue w(j), */
/*          SELECT(j) must be set to .TRUE.. To select condition numbers */
/*          corresponding to a complex conjugate pair of eigenvalues w(j) */
/*          and w(j+1), either SELECT(j) or SELECT(j+1) or both, must be */
/*          set to .TRUE.. */
/*          If HOWMNY = 'A', SELECT is not referenced. */

/*  N       (input) INTEGER */
/*          The order of the square matrix pair (A, B). N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The upper quasi-triangular matrix A in the pair (A,B). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,N). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,N) */
/*          The upper triangular matrix B in the pair (A,B). */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,N). */

/*  VL      (input) DOUBLE PRECISION array, dimension (LDVL,M) */
/*          If JOB = 'E' or 'B', VL must contain left eigenvectors of */
/*          (A, B), corresponding to the eigenpairs specified by HOWMNY */
/*          and SELECT. The eigenvectors must be stored in consecutive */
/*          columns of VL, as returned by DTGEVC. */
/*          If JOB = 'V', VL is not referenced. */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of the array VL. LDVL >= 1. */
/*          If JOB = 'E' or 'B', LDVL >= N. */

/*  VR      (input) DOUBLE PRECISION array, dimension (LDVR,M) */
/*          If JOB = 'E' or 'B', VR must contain right eigenvectors of */
/*          (A, B), corresponding to the eigenpairs specified by HOWMNY */
/*          and SELECT. The eigenvectors must be stored in consecutive */
/*          columns ov VR, as returned by DTGEVC. */
/*          If JOB = 'V', VR is not referenced. */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the array VR. LDVR >= 1. */
/*          If JOB = 'E' or 'B', LDVR >= N. */

/*  S       (output) DOUBLE PRECISION array, dimension (MM) */
/*          If JOB = 'E' or 'B', the reciprocal condition numbers of the */
/*          selected eigenvalues, stored in consecutive elements of the */
/*          array. For a complex conjugate pair of eigenvalues two */
/*          consecutive elements of S are set to the same value. Thus */
/*          S(j), DIF(j), and the j-th columns of VL and VR all */
/*          correspond to the same eigenpair (but not in general the */
/*          j-th eigenpair, unless all eigenpairs are selected). */
/*          If JOB = 'V', S is not referenced. */

/*  DIF     (output) DOUBLE PRECISION array, dimension (MM) */
/*          If JOB = 'V' or 'B', the estimated reciprocal condition */
/*          numbers of the selected eigenvectors, stored in consecutive */
/*          elements of the array. For a complex eigenvector two */
/*          consecutive elements of DIF are set to the same value. If */
/*          the eigenvalues cannot be reordered to compute DIF(j), DIF(j) */
/*          is set to 0; this can only occur when the true value would be */
/*          very small anyway. */
/*          If JOB = 'E', DIF is not referenced. */

/*  MM      (input) INTEGER */
/*          The number of elements in the arrays S and DIF. MM >= M. */

/*  M       (output) INTEGER */
/*          The number of elements of the arrays S and DIF used to store */
/*          the specified condition numbers; for each selected real */
/*          eigenvalue one element is used, and for each selected complex */
/*          conjugate pair of eigenvalues, two elements are used. */
/*          If HOWMNY = 'A', M is set to N. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= max(1,N). */
/*          If JOB = 'V' or 'B' LWORK >= 2*N*(N+2)+16. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace) INTEGER array, dimension (N + 6) */
/*          If JOB = 'E', IWORK is not referenced. */

/*  INFO    (output) INTEGER */
/*          =0: Successful exit */
/*          <0: If INFO = -i, the i-th argument had an illegal value */


/*  Further Details */
/*  =============== */

/*  The reciprocal of the condition number of a generalized eigenvalue */
/*  w = (a, b) is defined as */

/*       S(w) = (|u'Av|**2 + |u'Bv|**2)**(1/2) / (norm(u)*norm(v)) */

/*  where u and v are the left and right eigenvectors of (A, B) */
/*  corresponding to w; |z| denotes the absolute value of the complex */
/*  number, and norm(u) denotes the 2-norm of the vector u. */
/*  The pair (a, b) corresponds to an eigenvalue w = a/b (= u'Av/u'Bv) */
/*  of the matrix pair (A, B). If both a and b equal zero, then (A B) is */
/*  singular and S(I) = -1 is returned. */

/*  An approximate error bound on the chordal distance between the i-th */
/*  computed generalized eigenvalue w and the corresponding exact */
/*  eigenvalue lambda is */

/*       chord(w, lambda) <= EPS * norm(A, B) / S(I) */

/*  where EPS is the machine precision. */

/*  The reciprocal of the condition number DIF(i) of right eigenvector u */
/*  and left eigenvector v corresponding to the generalized eigenvalue w */
/*  is defined as follows: */

/*  a) If the i-th eigenvalue w = (a,b) is real */

/*     Suppose U and V are orthogonal transformations such that */

/*                U'*(A, B)*V  = (S, T) = ( a   *  ) ( b  *  )  1 */
/*                                        ( 0  S22 ),( 0 T22 )  n-1 */
/*                                          1  n-1     1 n-1 */

/*     Then the reciprocal condition number DIF(i) is */

/*                Difl((a, b), (S22, T22)) = sigma-min( Zl ), */

/*     where sigma-min(Zl) denotes the smallest singular value of the */
/*     2(n-1)-by-2(n-1) matrix */

/*         Zl = [ kron(a, In-1)  -kron(1, S22) ] */
/*              [ kron(b, In-1)  -kron(1, T22) ] . */

/*     Here In-1 is the identity matrix of size n-1. kron(X, Y) is the */
/*     Kronecker product between the matrices X and Y. */

/*     Note that if the default method for computing DIF(i) is wanted */
/*     (see DLATDF), then the parameter DIFDRI (see below) should be */
/*     changed from 3 to 4 (routine DLATDF(IJOB = 2 will be used)). */
/*     See DTGSYL for more details. */

/*  b) If the i-th and (i+1)-th eigenvalues are complex conjugate pair, */

/*     Suppose U and V are orthogonal transformations such that */

/*                U'*(A, B)*V = (S, T) = ( S11  *   ) ( T11  *  )  2 */
/*                                       ( 0    S22 ),( 0    T22) n-2 */
/*                                         2    n-2     2    n-2 */

/*     and (S11, T11) corresponds to the complex conjugate eigenvalue */
/*     pair (w, conjg(w)). There exist unitary matrices U1 and V1 such */
/*     that */

/*         U1'*S11*V1 = ( s11 s12 )   and U1'*T11*V1 = ( t11 t12 ) */
/*                      (  0  s22 )                    (  0  t22 ) */

/*     where the generalized eigenvalues w = s11/t11 and */
/*     conjg(w) = s22/t22. */

/*     Then the reciprocal condition number DIF(i) is bounded by */

/*         min( d1, max( 1, |real(s11)/real(s22)| )*d2 ) */

/*     where, d1 = Difl((s11, t11), (s22, t22)) = sigma-min(Z1), where */
/*     Z1 is the complex 2-by-2 matrix */

/*              Z1 =  [ s11  -s22 ] */
/*                    [ t11  -t22 ], */

/*     This is done by computing (using real arithmetic) the */
/*     roots of the characteristical polynomial det(Z1' * Z1 - lambda I), */
/*     where Z1' denotes the conjugate transpose of Z1 and det(X) denotes */
/*     the determinant of X. */

/*     and d2 is an upper bound on Difl((S11, T11), (S22, T22)), i.e. an */
/*     upper bound on sigma-min(Z2), where Z2 is (2n-2)-by-(2n-2) */

/*              Z2 = [ kron(S11', In-2)  -kron(I2, S22) ] */
/*                   [ kron(T11', In-2)  -kron(I2, T22) ] */

/*     Note that if the default method for computing DIF is wanted (see */
/*     DLATDF), then the parameter DIFDRI (see below) should be changed */
/*     from 3 to 4 (routine DLATDF(IJOB = 2 will be used)). See DTGSYL */
/*     for more details. */

/*  For each eigenvalue/vector specified by SELECT, DIF stores a */
/*  Frobenius norm-based estimate of Difl. */

/*  An approximate error bound for the i-th computed eigenvector VL(i) or */
/*  VR(i) is given by */

/*             EPS * norm(A, B) / DIF(i). */

/*  See ref. [2-3] for more details and further references. */

/*  Based on contributions by */
/*     Bo Kagstrom and Peter Poromaa, Department of Computing Science, */
/*     Umea University, S-901 87 Umea, Sweden. */

/*  References */
/*  ========== */

/*  [1] B. Kagstrom; A Direct Method for Reordering Eigenvalues in the */
/*      Generalized Real Schur Form of a Regular Matrix Pair (A, B), in */
/*      M.S. Moonen et al (eds), Linear Algebra for Large Scale and */
/*      Real-Time Applications, Kluwer Academic Publ. 1993, pp 195-218. */

/*  [2] B. Kagstrom and P. Poromaa; Computing Eigenspaces with Specified */
/*      Eigenvalues of a Regular Matrix Pair (A, B) and Condition */
/*      Estimation: Theory, Algorithms and Software, */
/*      Report UMINF - 94.04, Department of Computing Science, Umea */
/*      University, S-901 87 Umea, Sweden, 1994. Also as LAPACK Working */
/*      Note 87. To appear in Numerical Algorithms, 1996. */

/*  [3] B. Kagstrom and P. Poromaa, LAPACK-Style Algorithms and Software */
/*      for Solving the Generalized Sylvester Equation and Estimating the */
/*      Separation between Regular Matrix Pairs, Report UMINF - 93.23, */
/*      Department of Computing Science, Umea University, S-901 87 Umea, */
/*      Sweden, December 1993, Revised April 1994, Also as LAPACK Working */
/*      Note 75.  To appear in ACM Trans. on Math. Software, Vol 22, */
/*      No 1, 1996. */

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

/*     Decode and test the input parameters */

    /* Parameter adjustments */
    --select;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    --s;
    --dif;
    --work;
    --iwork;

    /* Function Body */
    wantbh = lsame_(job, "B");
    wants = lsame_(job, "E") || wantbh;
    wantdf = lsame_(job, "V") || wantbh;

    somcon = lsame_(howmny, "S");

    *info = 0;
    lquery = *lwork == -1;

    if (! wants && ! wantdf) {
	*info = -1;
    } else if (! lsame_(howmny, "A") && ! somcon) {
	*info = -2;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -8;
    } else if (wants && *ldvl < *n) {
	*info = -10;
    } else if (wants && *ldvr < *n) {
	*info = -12;
    } else {

/*        Set M to the number of eigenpairs for which condition numbers */
/*        are required, and test MM. */

	if (somcon) {
	    *m = 0;
	    pair = false;
	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {
		if (pair) {
		    pair = false;
		} else {
		    if (k < *n) {
			if (a[k + 1 + k * a_dim1] == 0.) {
			    if (select[k]) {
				++(*m);
			    }
			} else {
			    pair = true;
			    if (select[k] || select[k + 1]) {
				*m += 2;
			    }
			}
		    } else {
			if (select[*n]) {
			    ++(*m);
			}
		    }
		}
/* L10: */
	    }
	} else {
	    *m = *n;
	}

	if (*n == 0) {
	    lwmin = 1;
	} else if (lsame_(job, "V") || lsame_(job,
		"B")) {
	    lwmin = (*n << 1) * (*n + 2) + 16;
	} else {
	    lwmin = *n;
	}
	work[1] = (double) lwmin;

	if (*mm < *m) {
	    *info = -15;
	} else if (*lwork < lwmin && ! lquery) {
	    *info = -18;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTGSNA", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    ks = 0;
    pair = false;

    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {

/*        Determine whether A(k,k) begins a 1-by-1 or 2-by-2 block. */

	if (pair) {
	    pair = false;
	    goto L20;
	} else {
	    if (k < *n) {
		pair = a[k + 1 + k * a_dim1] != 0.;
	    }
	}

/*        Determine whether condition numbers are required for the k-th */
/*        eigenpair. */

	if (somcon) {
	    if (pair) {
		if (! select[k] && ! select[k + 1]) {
		    goto L20;
		}
	    } else {
		if (! select[k]) {
		    goto L20;
		}
	    }
	}

	++ks;

	if (wants) {

/*           Compute the reciprocal condition number of the k-th */
/*           eigenvalue. */

	    if (pair) {

/*              Complex eigenvalue pair. */

		d__1 = dnrm2_(n, &vr[ks * vr_dim1 + 1], &c__1);
		d__2 = dnrm2_(n, &vr[(ks + 1) * vr_dim1 + 1], &c__1);
		rnrm = dlapy2_(&d__1, &d__2);
		d__1 = dnrm2_(n, &vl[ks * vl_dim1 + 1], &c__1);
		d__2 = dnrm2_(n, &vl[(ks + 1) * vl_dim1 + 1], &c__1);
		lnrm = dlapy2_(&d__1, &d__2);
		dgemv_("N", n, n, &c_b19, &a[a_offset], lda, &vr[ks * vr_dim1
			+ 1], &c__1, &c_b21, &work[1], &c__1);
		tmprr = ddot_(n, &work[1], &c__1, &vl[ks * vl_dim1 + 1], &
			c__1);
		tmpri = ddot_(n, &work[1], &c__1, &vl[(ks + 1) * vl_dim1 + 1],
			 &c__1);
		dgemv_("N", n, n, &c_b19, &a[a_offset], lda, &vr[(ks + 1) *
			vr_dim1 + 1], &c__1, &c_b21, &work[1], &c__1);
		tmpii = ddot_(n, &work[1], &c__1, &vl[(ks + 1) * vl_dim1 + 1],
			 &c__1);
		tmpir = ddot_(n, &work[1], &c__1, &vl[ks * vl_dim1 + 1], &
			c__1);
		uhav = tmprr + tmpii;
		uhavi = tmpir - tmpri;
		dgemv_("N", n, n, &c_b19, &b[b_offset], ldb, &vr[ks * vr_dim1
			+ 1], &c__1, &c_b21, &work[1], &c__1);
		tmprr = ddot_(n, &work[1], &c__1, &vl[ks * vl_dim1 + 1], &
			c__1);
		tmpri = ddot_(n, &work[1], &c__1, &vl[(ks + 1) * vl_dim1 + 1],
			 &c__1);
		dgemv_("N", n, n, &c_b19, &b[b_offset], ldb, &vr[(ks + 1) *
			vr_dim1 + 1], &c__1, &c_b21, &work[1], &c__1);
		tmpii = ddot_(n, &work[1], &c__1, &vl[(ks + 1) * vl_dim1 + 1],
			 &c__1);
		tmpir = ddot_(n, &work[1], &c__1, &vl[ks * vl_dim1 + 1], &
			c__1);
		uhbv = tmprr + tmpii;
		uhbvi = tmpir - tmpri;
		uhav = dlapy2_(&uhav, &uhavi);
		uhbv = dlapy2_(&uhbv, &uhbvi);
		cond = dlapy2_(&uhav, &uhbv);
		s[ks] = cond / (rnrm * lnrm);
		s[ks + 1] = s[ks];

	    } else {

/*              Real eigenvalue. */

		rnrm = dnrm2_(n, &vr[ks * vr_dim1 + 1], &c__1);
		lnrm = dnrm2_(n, &vl[ks * vl_dim1 + 1], &c__1);
		dgemv_("N", n, n, &c_b19, &a[a_offset], lda, &vr[ks * vr_dim1
			+ 1], &c__1, &c_b21, &work[1], &c__1);
		uhav = ddot_(n, &work[1], &c__1, &vl[ks * vl_dim1 + 1], &c__1)
			;
		dgemv_("N", n, n, &c_b19, &b[b_offset], ldb, &vr[ks * vr_dim1
			+ 1], &c__1, &c_b21, &work[1], &c__1);
		uhbv = ddot_(n, &work[1], &c__1, &vl[ks * vl_dim1 + 1], &c__1)
			;
		cond = dlapy2_(&uhav, &uhbv);
		if (cond == 0.) {
		    s[ks] = -1.;
		} else {
		    s[ks] = cond / (rnrm * lnrm);
		}
	    }
	}

	if (wantdf) {
	    if (*n == 1) {
		dif[ks] = dlapy2_(&a[a_dim1 + 1], &b[b_dim1 + 1]);
		goto L20;
	    }

/*           Estimate the reciprocal condition number of the k-th */
/*           eigenvectors. */
	    if (pair) {

/*              Copy the  2-by 2 pencil beginning at (A(k,k), B(k, k)). */
/*              Compute the eigenvalue(s) at position K. */

		work[1] = a[k + k * a_dim1];
		work[2] = a[k + 1 + k * a_dim1];
		work[3] = a[k + (k + 1) * a_dim1];
		work[4] = a[k + 1 + (k + 1) * a_dim1];
		work[5] = b[k + k * b_dim1];
		work[6] = b[k + 1 + k * b_dim1];
		work[7] = b[k + (k + 1) * b_dim1];
		work[8] = b[k + 1 + (k + 1) * b_dim1];
		d__1 = smlnum * eps;
		dlag2_(&work[1], &c__2, &work[5], &c__2, &d__1, &beta, dummy1,
			 &alphar, dummy, &alphai);
		alprqt = 1.;
		c1 = (alphar * alphar + alphai * alphai + beta * beta) * 2.;
		c2 = beta * 4. * beta * alphai * alphai;
		root1 = c1 + sqrt(c1 * c1 - c2 * 4.);
		root2 = c2 / root1;
		root1 /= 2.;
/* Computing MIN */
		d__1 = sqrt(root1), d__2 = sqrt(root2);
		cond = std::min(d__1,d__2);
	    }

/*           Copy the matrix (A, B) to the array WORK and swap the */
/*           diagonal block beginning at A(k,k) to the (1,1) position. */

	    dlacpy_("Full", n, n, &a[a_offset], lda, &work[1], n);
	    dlacpy_("Full", n, n, &b[b_offset], ldb, &work[*n * *n + 1], n);
	    ifst = k;
	    ilst = 1;

	    i__2 = *lwork - (*n << 1) * *n;
	    dtgexc_(&c_false, &c_false, n, &work[1], n, &work[*n * *n + 1], n,
		     dummy, &c__1, dummy1, &c__1, &ifst, &ilst, &work[(*n * *
		    n << 1) + 1], &i__2, &ierr);

	    if (ierr > 0) {

/*              Ill-conditioned problem - swap rejected. */

		dif[ks] = 0.;
	    } else {

/*              Reordering successful, solve generalized Sylvester */
/*              equation for R and L, */
/*                         A22 * R - L * A11 = A12 */
/*                         B22 * R - L * B11 = B12, */
/*              and compute estimate of Difl((A11,B11), (A22, B22)). */

		n1 = 1;
		if (work[2] != 0.) {
		    n1 = 2;
		}
		n2 = *n - n1;
		if (n2 == 0) {
		    dif[ks] = cond;
		} else {
		    i__ = *n * *n + 1;
		    iz = (*n << 1) * *n + 1;
		    i__2 = *lwork - (*n << 1) * *n;
		    dtgsyl_("N", &c__3, &n2, &n1, &work[*n * n1 + n1 + 1], n,
			    &work[1], n, &work[n1 + 1], n, &work[*n * n1 + n1
			    + i__], n, &work[i__], n, &work[n1 + i__], n, &
			    scale, &dif[ks], &work[iz + 1], &i__2, &iwork[1],
			    &ierr);

		    if (pair) {
/* Computing MIN */
			d__1 = std::max(1.,alprqt) * dif[ks];
			dif[ks] = std::min(d__1,cond);
		    }
		}
	    }
	    if (pair) {
		dif[ks + 1] = dif[ks];
	    }
	}
	if (pair) {
	    ++ks;
	}

L20:
	;
    }
    work[1] = (double) lwmin;
    return 0;

/*     End of DTGSNA */

} /* dtgsna_ */

/* Subroutine */ int dtgsy2_(const char *trans, integer *ijob, integer *m, integer *
	n, double *a, integer *lda, double *b, integer *ldb,
	double *c__, integer *ldc, double *d__, integer *ldd,
	double *e, integer *lde, double *f, integer *ldf, double *
	scale, double *rdsum, double *rdscal, integer *iwork, integer
	*pq, integer *info)
{
	/* Table of constant values */
	static integer c__8 = 8;
	static integer c__1 = 1;
	static double c_b27 = -1.;
	static double c_b42 = 1.;
	static double c_b56 = 0.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, d_dim1,
	    d_offset, e_dim1, e_offset, f_dim1, f_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, j, k, p, q;
    double z__[64]	/* was [8][8] */;
    integer ie, je, mb, nb, ii, jj, is, js;
    double rhs[8];
    integer isp1, jsp1;
    integer ierr, zdim, ipiv[8], jpiv[8];
    double alpha;
    double scaloc;
    bool notran;


/*  -- LAPACK auxiliary routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     January 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTGSY2 solves the generalized Sylvester equation: */

/*              A * R - L * B = scale * C                (1) */
/*              D * R - L * E = scale * F, */

/*  using Level 1 and 2 BLAS. where R and L are unknown M-by-N matrices, */
/*  (A, D), (B, E) and (C, F) are given matrix pairs of size M-by-M, */
/*  N-by-N and M-by-N, respectively, with real entries. (A, D) and (B, E) */
/*  must be in generalized Schur canonical form, i.e. A, B are upper */
/*  quasi triangular and D, E are upper triangular. The solution (R, L) */
/*  overwrites (C, F). 0 <= SCALE <= 1 is an output scaling factor */
/*  chosen to avoid overflow. */

/*  In matrix notation solving equation (1) corresponds to solve */
/*  Z*x = scale*b, where Z is defined as */

/*         Z = [ kron(In, A)  -kron(B', Im) ]             (2) */
/*             [ kron(In, D)  -kron(E', Im) ], */

/*  Ik is the identity matrix of size k and X' is the transpose of X. */
/*  kron(X, Y) is the Kronecker product between the matrices X and Y. */
/*  In the process of solving (1), we solve a number of such systems */
/*  where Dim(In), Dim(In) = 1 or 2. */

/*  If TRANS = 'T', solve the transposed system Z'*y = scale*b for y, */
/*  which is equivalent to solve for R and L in */

/*              A' * R  + D' * L   = scale *  C           (3) */
/*              R  * B' + L  * E'  = scale * -F */

/*  This case is used to compute an estimate of Dif[(A, D), (B, E)] = */
/*  sigma_min(Z) using reverse communicaton with DLACON. */

/*  DTGSY2 also (IJOB >= 1) contributes to the computation in DTGSYL */
/*  of an upper bound on the separation between to matrix pairs. Then */
/*  the input (A, D), (B, E) are sub-pencils of the matrix pair in */
/*  DTGSYL. See DTGSYL for details. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          = 'N', solve the generalized Sylvester equation (1). */
/*          = 'T': solve the 'transposed' system (3). */

/*  IJOB    (input) INTEGER */
/*          Specifies what kind of functionality to be performed. */
/*          = 0: solve (1) only. */
/*          = 1: A contribution from this subsystem to a Frobenius */
/*               norm-based estimate of the separation between two matrix */
/*               pairs is computed. (look ahead strategy is used). */
/*          = 2: A contribution from this subsystem to a Frobenius */
/*               norm-based estimate of the separation between two matrix */
/*               pairs is computed. (DGECON on sub-systems is used.) */
/*          Not referenced if TRANS = 'T'. */

/*  M       (input) INTEGER */
/*          On entry, M specifies the order of A and D, and the row */
/*          dimension of C, F, R and L. */

/*  N       (input) INTEGER */
/*          On entry, N specifies the order of B and E, and the column */
/*          dimension of C, F, R and L. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA, M) */
/*          On entry, A contains an upper quasi triangular matrix. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the matrix A. LDA >= max(1, M). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB, N) */
/*          On entry, B contains an upper quasi triangular matrix. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the matrix B. LDB >= max(1, N). */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC, N) */
/*          On entry, C contains the right-hand-side of the first matrix */
/*          equation in (1). */
/*          On exit, if IJOB = 0, C has been overwritten by the */
/*          solution R. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the matrix C. LDC >= max(1, M). */

/*  D       (input) DOUBLE PRECISION array, dimension (LDD, M) */
/*          On entry, D contains an upper triangular matrix. */

/*  LDD     (input) INTEGER */
/*          The leading dimension of the matrix D. LDD >= max(1, M). */

/*  E       (input) DOUBLE PRECISION array, dimension (LDE, N) */
/*          On entry, E contains an upper triangular matrix. */

/*  LDE     (input) INTEGER */
/*          The leading dimension of the matrix E. LDE >= max(1, N). */

/*  F       (input/output) DOUBLE PRECISION array, dimension (LDF, N) */
/*          On entry, F contains the right-hand-side of the second matrix */
/*          equation in (1). */
/*          On exit, if IJOB = 0, F has been overwritten by the */
/*          solution L. */

/*  LDF     (input) INTEGER */
/*          The leading dimension of the matrix F. LDF >= max(1, M). */

/*  SCALE   (output) DOUBLE PRECISION */
/*          On exit, 0 <= SCALE <= 1. If 0 < SCALE < 1, the solutions */
/*          R and L (C and F on entry) will hold the solutions to a */
/*          slightly perturbed system but the input matrices A, B, D and */
/*          E have not been changed. If SCALE = 0, R and L will hold the */
/*          solutions to the homogeneous system with C = F = 0. Normally, */
/*          SCALE = 1. */

/*  RDSUM   (input/output) DOUBLE PRECISION */
/*          On entry, the sum of squares of computed contributions to */
/*          the Dif-estimate under computation by DTGSYL, where the */
/*          scaling factor RDSCAL (see below) has been factored out. */
/*          On exit, the corresponding sum of squares updated with the */
/*          contributions from the current sub-system. */
/*          If TRANS = 'T' RDSUM is not touched. */
/*          NOTE: RDSUM only makes sense when DTGSY2 is called by DTGSYL. */

/*  RDSCAL  (input/output) DOUBLE PRECISION */
/*          On entry, scaling factor used to prevent overflow in RDSUM. */
/*          On exit, RDSCAL is updated w.r.t. the current contributions */
/*          in RDSUM. */
/*          If TRANS = 'T', RDSCAL is not touched. */
/*          NOTE: RDSCAL only makes sense when DTGSY2 is called by */
/*                DTGSYL. */

/*  IWORK   (workspace) INTEGER array, dimension (M+N+2) */

/*  PQ      (output) INTEGER */
/*          On exit, the number of subsystems (of size 2-by-2, 4-by-4 and */
/*          8-by-8) solved by this routine. */

/*  INFO    (output) INTEGER */
/*          On exit, if INFO is set to */
/*            =0: Successful exit */
/*            <0: If INFO = -i, the i-th argument had an illegal value. */
/*            >0: The matrix pairs (A, D) and (B, E) have common or very */
/*                close eigenvalues. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Bo Kagstrom and Peter Poromaa, Department of Computing Science, */
/*     Umea University, S-901 87 Umea, Sweden. */

/*  ===================================================================== */
/*  Replaced various illegal calls to DCOPY by calls to DLASET. */
/*  Sven Hammarling, 27/5/02. */

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

/*     Decode and test input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    d_dim1 = *ldd;
    d_offset = 1 + d_dim1;
    d__ -= d_offset;
    e_dim1 = *lde;
    e_offset = 1 + e_dim1;
    e -= e_offset;
    f_dim1 = *ldf;
    f_offset = 1 + f_dim1;
    f -= f_offset;
    --iwork;

    /* Function Body */
    *info = 0;
    ierr = 0;
    notran = lsame_(trans, "N");
    if (! notran && ! lsame_(trans, "T")) {
	*info = -1;
    } else if (notran) {
	if (*ijob < 0 || *ijob > 2) {
	    *info = -2;
	}
    }
    if (*info == 0) {
	if (*m <= 0) {
	    *info = -3;
	} else if (*n <= 0) {
	    *info = -4;
	} else if (*lda < std::max(1_integer,*m)) {
	    *info = -5;
	} else if (*ldb < std::max(1_integer,*n)) {
	    *info = -8;
	} else if (*ldc < std::max(1_integer,*m)) {
	    *info = -10;
	} else if (*ldd < std::max(1_integer,*m)) {
	    *info = -12;
	} else if (*lde < std::max(1_integer,*n)) {
	    *info = -14;
	} else if (*ldf < std::max(1_integer,*m)) {
	    *info = -16;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTGSY2", &i__1);
	return 0;
    }

/*     Determine block structure of A */

    *pq = 0;
    p = 0;
    i__ = 1;
L10:
    if (i__ > *m) {
	goto L20;
    }
    ++p;
    iwork[p] = i__;
    if (i__ == *m) {
	goto L20;
    }
    if (a[i__ + 1 + i__ * a_dim1] != 0.) {
	i__ += 2;
    } else {
	++i__;
    }
    goto L10;
L20:
    iwork[p + 1] = *m + 1;

/*     Determine block structure of B */

    q = p + 1;
    j = 1;
L30:
    if (j > *n) {
	goto L40;
    }
    ++q;
    iwork[q] = j;
    if (j == *n) {
	goto L40;
    }
    if (b[j + 1 + j * b_dim1] != 0.) {
	j += 2;
    } else {
	++j;
    }
    goto L30;
L40:
    iwork[q + 1] = *n + 1;
    *pq = p * (q - p - 1);

    if (notran) {

/*        Solve (I, J) - subsystem */
/*           A(I, I) * R(I, J) - L(I, J) * B(J, J) = C(I, J) */
/*           D(I, I) * R(I, J) - L(I, J) * E(J, J) = F(I, J) */
/*        for I = P, P - 1, ..., 1; J = 1, 2, ..., Q */

	*scale = 1.;
	scaloc = 1.;
	i__1 = q;
	for (j = p + 2; j <= i__1; ++j) {
	    js = iwork[j];
	    jsp1 = js + 1;
	    je = iwork[j + 1] - 1;
	    nb = je - js + 1;
	    for (i__ = p; i__ >= 1; --i__) {

		is = iwork[i__];
		isp1 = is + 1;
		ie = iwork[i__ + 1] - 1;
		mb = ie - is + 1;
		zdim = mb * nb << 1;

		if (mb == 1 && nb == 1) {

/*                 Build a 2-by-2 system Z * x = RHS */

		    z__[0] = a[is + is * a_dim1];
		    z__[1] = d__[is + is * d_dim1];
		    z__[8] = -b[js + js * b_dim1];
		    z__[9] = -e[js + js * e_dim1];

/*                 Set up right hand side(s) */

		    rhs[0] = c__[is + js * c_dim1];
		    rhs[1] = f[is + js * f_dim1];

/*                 Solve Z * x = RHS */

		    dgetc2_(&zdim, z__, &c__8, ipiv, jpiv, &ierr);
		    if (ierr > 0) {
			*info = ierr;
		    }

		    if (*ijob == 0) {
			dgesc2_(&zdim, z__, &c__8, rhs, ipiv, jpiv, &scaloc);
			if (scaloc != 1.) {
			    i__2 = *n;
			    for (k = 1; k <= i__2; ++k) {
				dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &
					c__1);
				dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L50: */
			    }
			    *scale *= scaloc;
			}
		    } else {
			dlatdf_(ijob, &zdim, z__, &c__8, rhs, rdsum, rdscal,
				ipiv, jpiv);
		    }

/*                 Unpack solution vector(s) */

		    c__[is + js * c_dim1] = rhs[0];
		    f[is + js * f_dim1] = rhs[1];

/*                 Substitute R(I, J) and L(I, J) into remaining */
/*                 equation. */

		    if (i__ > 1) {
			alpha = -rhs[0];
			i__2 = is - 1;
			daxpy_(&i__2, &alpha, &a[is * a_dim1 + 1], &c__1, &
				c__[js * c_dim1 + 1], &c__1);
			i__2 = is - 1;
			daxpy_(&i__2, &alpha, &d__[is * d_dim1 + 1], &c__1, &
				f[js * f_dim1 + 1], &c__1);
		    }
		    if (j < q) {
			i__2 = *n - je;
			daxpy_(&i__2, &rhs[1], &b[js + (je + 1) * b_dim1],
				ldb, &c__[is + (je + 1) * c_dim1], ldc);
			i__2 = *n - je;
			daxpy_(&i__2, &rhs[1], &e[js + (je + 1) * e_dim1],
				lde, &f[is + (je + 1) * f_dim1], ldf);
		    }

		} else if (mb == 1 && nb == 2) {

/*                 Build a 4-by-4 system Z * x = RHS */

		    z__[0] = a[is + is * a_dim1];
		    z__[1] = 0.;
		    z__[2] = d__[is + is * d_dim1];
		    z__[3] = 0.;

		    z__[8] = 0.;
		    z__[9] = a[is + is * a_dim1];
		    z__[10] = 0.;
		    z__[11] = d__[is + is * d_dim1];

		    z__[16] = -b[js + js * b_dim1];
		    z__[17] = -b[js + jsp1 * b_dim1];
		    z__[18] = -e[js + js * e_dim1];
		    z__[19] = -e[js + jsp1 * e_dim1];

		    z__[24] = -b[jsp1 + js * b_dim1];
		    z__[25] = -b[jsp1 + jsp1 * b_dim1];
		    z__[26] = 0.;
		    z__[27] = -e[jsp1 + jsp1 * e_dim1];

/*                 Set up right hand side(s) */

		    rhs[0] = c__[is + js * c_dim1];
		    rhs[1] = c__[is + jsp1 * c_dim1];
		    rhs[2] = f[is + js * f_dim1];
		    rhs[3] = f[is + jsp1 * f_dim1];

/*                 Solve Z * x = RHS */

		    dgetc2_(&zdim, z__, &c__8, ipiv, jpiv, &ierr);
		    if (ierr > 0) {
			*info = ierr;
		    }

		    if (*ijob == 0) {
			dgesc2_(&zdim, z__, &c__8, rhs, ipiv, jpiv, &scaloc);
			if (scaloc != 1.) {
			    i__2 = *n;
			    for (k = 1; k <= i__2; ++k) {
				dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &
					c__1);
				dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L60: */
			    }
			    *scale *= scaloc;
			}
		    } else {
			dlatdf_(ijob, &zdim, z__, &c__8, rhs, rdsum, rdscal,
				ipiv, jpiv);
		    }

/*                 Unpack solution vector(s) */

		    c__[is + js * c_dim1] = rhs[0];
		    c__[is + jsp1 * c_dim1] = rhs[1];
		    f[is + js * f_dim1] = rhs[2];
		    f[is + jsp1 * f_dim1] = rhs[3];

/*                 Substitute R(I, J) and L(I, J) into remaining */
/*                 equation. */

		    if (i__ > 1) {
			i__2 = is - 1;
			dger_(&i__2, &nb, &c_b27, &a[is * a_dim1 + 1], &c__1,
				rhs, &c__1, &c__[js * c_dim1 + 1], ldc);
			i__2 = is - 1;
			dger_(&i__2, &nb, &c_b27, &d__[is * d_dim1 + 1], &
				c__1, rhs, &c__1, &f[js * f_dim1 + 1], ldf);
		    }
		    if (j < q) {
			i__2 = *n - je;
			daxpy_(&i__2, &rhs[2], &b[js + (je + 1) * b_dim1],
				ldb, &c__[is + (je + 1) * c_dim1], ldc);
			i__2 = *n - je;
			daxpy_(&i__2, &rhs[2], &e[js + (je + 1) * e_dim1],
				lde, &f[is + (je + 1) * f_dim1], ldf);
			i__2 = *n - je;
			daxpy_(&i__2, &rhs[3], &b[jsp1 + (je + 1) * b_dim1],
				ldb, &c__[is + (je + 1) * c_dim1], ldc);
			i__2 = *n - je;
			daxpy_(&i__2, &rhs[3], &e[jsp1 + (je + 1) * e_dim1],
				lde, &f[is + (je + 1) * f_dim1], ldf);
		    }

		} else if (mb == 2 && nb == 1) {

/*                 Build a 4-by-4 system Z * x = RHS */

		    z__[0] = a[is + is * a_dim1];
		    z__[1] = a[isp1 + is * a_dim1];
		    z__[2] = d__[is + is * d_dim1];
		    z__[3] = 0.;

		    z__[8] = a[is + isp1 * a_dim1];
		    z__[9] = a[isp1 + isp1 * a_dim1];
		    z__[10] = d__[is + isp1 * d_dim1];
		    z__[11] = d__[isp1 + isp1 * d_dim1];

		    z__[16] = -b[js + js * b_dim1];
		    z__[17] = 0.;
		    z__[18] = -e[js + js * e_dim1];
		    z__[19] = 0.;

		    z__[24] = 0.;
		    z__[25] = -b[js + js * b_dim1];
		    z__[26] = 0.;
		    z__[27] = -e[js + js * e_dim1];

/*                 Set up right hand side(s) */

		    rhs[0] = c__[is + js * c_dim1];
		    rhs[1] = c__[isp1 + js * c_dim1];
		    rhs[2] = f[is + js * f_dim1];
		    rhs[3] = f[isp1 + js * f_dim1];

/*                 Solve Z * x = RHS */

		    dgetc2_(&zdim, z__, &c__8, ipiv, jpiv, &ierr);
		    if (ierr > 0) {
			*info = ierr;
		    }
		    if (*ijob == 0) {
			dgesc2_(&zdim, z__, &c__8, rhs, ipiv, jpiv, &scaloc);
			if (scaloc != 1.) {
			    i__2 = *n;
			    for (k = 1; k <= i__2; ++k) {
				dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &
					c__1);
				dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L70: */
			    }
			    *scale *= scaloc;
			}
		    } else {
			dlatdf_(ijob, &zdim, z__, &c__8, rhs, rdsum, rdscal,
				ipiv, jpiv);
		    }

/*                 Unpack solution vector(s) */

		    c__[is + js * c_dim1] = rhs[0];
		    c__[isp1 + js * c_dim1] = rhs[1];
		    f[is + js * f_dim1] = rhs[2];
		    f[isp1 + js * f_dim1] = rhs[3];

/*                 Substitute R(I, J) and L(I, J) into remaining */
/*                 equation. */

		    if (i__ > 1) {
			i__2 = is - 1;
			dgemv_("N", &i__2, &mb, &c_b27, &a[is * a_dim1 + 1],
				lda, rhs, &c__1, &c_b42, &c__[js * c_dim1 + 1]
, &c__1);
			i__2 = is - 1;
			dgemv_("N", &i__2, &mb, &c_b27, &d__[is * d_dim1 + 1],
				 ldd, rhs, &c__1, &c_b42, &f[js * f_dim1 + 1],
				 &c__1);
		    }
		    if (j < q) {
			i__2 = *n - je;
			dger_(&mb, &i__2, &c_b42, &rhs[2], &c__1, &b[js + (je
				+ 1) * b_dim1], ldb, &c__[is + (je + 1) *
				c_dim1], ldc);
			i__2 = *n - je;
			dger_(&mb, &i__2, &c_b42, &rhs[2], &c__1, &e[js + (je
				+ 1) * e_dim1], lde, &f[is + (je + 1) *
				f_dim1], ldf);
		    }

		} else if (mb == 2 && nb == 2) {

/*                 Build an 8-by-8 system Z * x = RHS */

		    dlaset_("F", &c__8, &c__8, &c_b56, &c_b56, z__, &c__8);

		    z__[0] = a[is + is * a_dim1];
		    z__[1] = a[isp1 + is * a_dim1];
		    z__[4] = d__[is + is * d_dim1];

		    z__[8] = a[is + isp1 * a_dim1];
		    z__[9] = a[isp1 + isp1 * a_dim1];
		    z__[12] = d__[is + isp1 * d_dim1];
		    z__[13] = d__[isp1 + isp1 * d_dim1];

		    z__[18] = a[is + is * a_dim1];
		    z__[19] = a[isp1 + is * a_dim1];
		    z__[22] = d__[is + is * d_dim1];

		    z__[26] = a[is + isp1 * a_dim1];
		    z__[27] = a[isp1 + isp1 * a_dim1];
		    z__[30] = d__[is + isp1 * d_dim1];
		    z__[31] = d__[isp1 + isp1 * d_dim1];

		    z__[32] = -b[js + js * b_dim1];
		    z__[34] = -b[js + jsp1 * b_dim1];
		    z__[36] = -e[js + js * e_dim1];
		    z__[38] = -e[js + jsp1 * e_dim1];

		    z__[41] = -b[js + js * b_dim1];
		    z__[43] = -b[js + jsp1 * b_dim1];
		    z__[45] = -e[js + js * e_dim1];
		    z__[47] = -e[js + jsp1 * e_dim1];

		    z__[48] = -b[jsp1 + js * b_dim1];
		    z__[50] = -b[jsp1 + jsp1 * b_dim1];
		    z__[54] = -e[jsp1 + jsp1 * e_dim1];

		    z__[57] = -b[jsp1 + js * b_dim1];
		    z__[59] = -b[jsp1 + jsp1 * b_dim1];
		    z__[63] = -e[jsp1 + jsp1 * e_dim1];

/*                 Set up right hand side(s) */

		    k = 1;
		    ii = mb * nb + 1;
		    i__2 = nb - 1;
		    for (jj = 0; jj <= i__2; ++jj) {
			dcopy_(&mb, &c__[is + (js + jj) * c_dim1], &c__1, &
				rhs[k - 1], &c__1);
			dcopy_(&mb, &f[is + (js + jj) * f_dim1], &c__1, &rhs[
				ii - 1], &c__1);
			k += mb;
			ii += mb;
/* L80: */
		    }

/*                 Solve Z * x = RHS */

		    dgetc2_(&zdim, z__, &c__8, ipiv, jpiv, &ierr);
		    if (ierr > 0) {
			*info = ierr;
		    }
		    if (*ijob == 0) {
			dgesc2_(&zdim, z__, &c__8, rhs, ipiv, jpiv, &scaloc);
			if (scaloc != 1.) {
			    i__2 = *n;
			    for (k = 1; k <= i__2; ++k) {
				dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &
					c__1);
				dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L90: */
			    }
			    *scale *= scaloc;
			}
		    } else {
			dlatdf_(ijob, &zdim, z__, &c__8, rhs, rdsum, rdscal,
				ipiv, jpiv);
		    }

/*                 Unpack solution vector(s) */

		    k = 1;
		    ii = mb * nb + 1;
		    i__2 = nb - 1;
		    for (jj = 0; jj <= i__2; ++jj) {
			dcopy_(&mb, &rhs[k - 1], &c__1, &c__[is + (js + jj) *
				c_dim1], &c__1);
			dcopy_(&mb, &rhs[ii - 1], &c__1, &f[is + (js + jj) *
				f_dim1], &c__1);
			k += mb;
			ii += mb;
/* L100: */
		    }

/*                 Substitute R(I, J) and L(I, J) into remaining */
/*                 equation. */

		    if (i__ > 1) {
			i__2 = is - 1;
			dgemm_("N", "N", &i__2, &nb, &mb, &c_b27, &a[is *
				a_dim1 + 1], lda, rhs, &mb, &c_b42, &c__[js *
				c_dim1 + 1], ldc);
			i__2 = is - 1;
			dgemm_("N", "N", &i__2, &nb, &mb, &c_b27, &d__[is *
				d_dim1 + 1], ldd, rhs, &mb, &c_b42, &f[js *
				f_dim1 + 1], ldf);
		    }
		    if (j < q) {
			k = mb * nb + 1;
			i__2 = *n - je;
			dgemm_("N", "N", &mb, &i__2, &nb, &c_b42, &rhs[k - 1],
				 &mb, &b[js + (je + 1) * b_dim1], ldb, &c_b42,
				 &c__[is + (je + 1) * c_dim1], ldc);
			i__2 = *n - je;
			dgemm_("N", "N", &mb, &i__2, &nb, &c_b42, &rhs[k - 1],
				 &mb, &e[js + (je + 1) * e_dim1], lde, &c_b42,
				 &f[is + (je + 1) * f_dim1], ldf);
		    }

		}

/* L110: */
	    }
/* L120: */
	}
    } else {

/*        Solve (I, J) - subsystem */
/*             A(I, I)' * R(I, J) + D(I, I)' * L(J, J)  =  C(I, J) */
/*             R(I, I)  * B(J, J) + L(I, J)  * E(J, J)  = -F(I, J) */
/*        for I = 1, 2, ..., P, J = Q, Q - 1, ..., 1 */

	*scale = 1.;
	scaloc = 1.;
	i__1 = p;
	for (i__ = 1; i__ <= i__1; ++i__) {

	    is = iwork[i__];
	    isp1 = is + 1;
	    ie = i__;
	    mb = ie - is + 1;
	    i__2 = p + 2;
	    for (j = q; j >= i__2; --j) {

		js = iwork[j];
		jsp1 = js + 1;
		je = iwork[j + 1] - 1;
		nb = je - js + 1;
		zdim = mb * nb << 1;
		if (mb == 1 && nb == 1) {

/*                 Build a 2-by-2 system Z' * x = RHS */

		    z__[0] = a[is + is * a_dim1];
		    z__[1] = -b[js + js * b_dim1];
		    z__[8] = d__[is + is * d_dim1];
		    z__[9] = -e[js + js * e_dim1];

/*                 Set up right hand side(s) */

		    rhs[0] = c__[is + js * c_dim1];
		    rhs[1] = f[is + js * f_dim1];

/*                 Solve Z' * x = RHS */

		    dgetc2_(&zdim, z__, &c__8, ipiv, jpiv, &ierr);
		    if (ierr > 0) {
			*info = ierr;
		    }

		    dgesc2_(&zdim, z__, &c__8, rhs, ipiv, jpiv, &scaloc);
		    if (scaloc != 1.) {
			i__3 = *n;
			for (k = 1; k <= i__3; ++k) {
			    dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &c__1);
			    dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L130: */
			}
			*scale *= scaloc;
		    }

/*                 Unpack solution vector(s) */

		    c__[is + js * c_dim1] = rhs[0];
		    f[is + js * f_dim1] = rhs[1];

/*                 Substitute R(I, J) and L(I, J) into remaining */
/*                 equation. */

		    if (j > p + 2) {
			alpha = rhs[0];
			i__3 = js - 1;
			daxpy_(&i__3, &alpha, &b[js * b_dim1 + 1], &c__1, &f[
				is + f_dim1], ldf);
			alpha = rhs[1];
			i__3 = js - 1;
			daxpy_(&i__3, &alpha, &e[js * e_dim1 + 1], &c__1, &f[
				is + f_dim1], ldf);
		    }
		    if (i__ < p) {
			alpha = -rhs[0];
			i__3 = *m - ie;
			daxpy_(&i__3, &alpha, &a[is + (ie + 1) * a_dim1], lda,
				 &c__[ie + 1 + js * c_dim1], &c__1);
			alpha = -rhs[1];
			i__3 = *m - ie;
			daxpy_(&i__3, &alpha, &d__[is + (ie + 1) * d_dim1],
				ldd, &c__[ie + 1 + js * c_dim1], &c__1);
		    }

		} else if (mb == 1 && nb == 2) {

/*                 Build a 4-by-4 system Z' * x = RHS */

		    z__[0] = a[is + is * a_dim1];
		    z__[1] = 0.;
		    z__[2] = -b[js + js * b_dim1];
		    z__[3] = -b[jsp1 + js * b_dim1];

		    z__[8] = 0.;
		    z__[9] = a[is + is * a_dim1];
		    z__[10] = -b[js + jsp1 * b_dim1];
		    z__[11] = -b[jsp1 + jsp1 * b_dim1];

		    z__[16] = d__[is + is * d_dim1];
		    z__[17] = 0.;
		    z__[18] = -e[js + js * e_dim1];
		    z__[19] = 0.;

		    z__[24] = 0.;
		    z__[25] = d__[is + is * d_dim1];
		    z__[26] = -e[js + jsp1 * e_dim1];
		    z__[27] = -e[jsp1 + jsp1 * e_dim1];

/*                 Set up right hand side(s) */

		    rhs[0] = c__[is + js * c_dim1];
		    rhs[1] = c__[is + jsp1 * c_dim1];
		    rhs[2] = f[is + js * f_dim1];
		    rhs[3] = f[is + jsp1 * f_dim1];

/*                 Solve Z' * x = RHS */

		    dgetc2_(&zdim, z__, &c__8, ipiv, jpiv, &ierr);
		    if (ierr > 0) {
			*info = ierr;
		    }
		    dgesc2_(&zdim, z__, &c__8, rhs, ipiv, jpiv, &scaloc);
		    if (scaloc != 1.) {
			i__3 = *n;
			for (k = 1; k <= i__3; ++k) {
			    dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &c__1);
			    dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L140: */
			}
			*scale *= scaloc;
		    }

/*                 Unpack solution vector(s) */

		    c__[is + js * c_dim1] = rhs[0];
		    c__[is + jsp1 * c_dim1] = rhs[1];
		    f[is + js * f_dim1] = rhs[2];
		    f[is + jsp1 * f_dim1] = rhs[3];

/*                 Substitute R(I, J) and L(I, J) into remaining */
/*                 equation. */

		    if (j > p + 2) {
			i__3 = js - 1;
			daxpy_(&i__3, rhs, &b[js * b_dim1 + 1], &c__1, &f[is
				+ f_dim1], ldf);
			i__3 = js - 1;
			daxpy_(&i__3, &rhs[1], &b[jsp1 * b_dim1 + 1], &c__1, &
				f[is + f_dim1], ldf);
			i__3 = js - 1;
			daxpy_(&i__3, &rhs[2], &e[js * e_dim1 + 1], &c__1, &f[
				is + f_dim1], ldf);
			i__3 = js - 1;
			daxpy_(&i__3, &rhs[3], &e[jsp1 * e_dim1 + 1], &c__1, &
				f[is + f_dim1], ldf);
		    }
		    if (i__ < p) {
			i__3 = *m - ie;
			dger_(&i__3, &nb, &c_b27, &a[is + (ie + 1) * a_dim1],
				lda, rhs, &c__1, &c__[ie + 1 + js * c_dim1],
				ldc);
			i__3 = *m - ie;
			dger_(&i__3, &nb, &c_b27, &d__[is + (ie + 1) * d_dim1]
, ldd, &rhs[2], &c__1, &c__[ie + 1 + js *
				c_dim1], ldc);
		    }

		} else if (mb == 2 && nb == 1) {

/*                 Build a 4-by-4 system Z' * x = RHS */

		    z__[0] = a[is + is * a_dim1];
		    z__[1] = a[is + isp1 * a_dim1];
		    z__[2] = -b[js + js * b_dim1];
		    z__[3] = 0.;

		    z__[8] = a[isp1 + is * a_dim1];
		    z__[9] = a[isp1 + isp1 * a_dim1];
		    z__[10] = 0.;
		    z__[11] = -b[js + js * b_dim1];

		    z__[16] = d__[is + is * d_dim1];
		    z__[17] = d__[is + isp1 * d_dim1];
		    z__[18] = -e[js + js * e_dim1];
		    z__[19] = 0.;

		    z__[24] = 0.;
		    z__[25] = d__[isp1 + isp1 * d_dim1];
		    z__[26] = 0.;
		    z__[27] = -e[js + js * e_dim1];

/*                 Set up right hand side(s) */

		    rhs[0] = c__[is + js * c_dim1];
		    rhs[1] = c__[isp1 + js * c_dim1];
		    rhs[2] = f[is + js * f_dim1];
		    rhs[3] = f[isp1 + js * f_dim1];

/*                 Solve Z' * x = RHS */

		    dgetc2_(&zdim, z__, &c__8, ipiv, jpiv, &ierr);
		    if (ierr > 0) {
			*info = ierr;
		    }

		    dgesc2_(&zdim, z__, &c__8, rhs, ipiv, jpiv, &scaloc);
		    if (scaloc != 1.) {
			i__3 = *n;
			for (k = 1; k <= i__3; ++k) {
			    dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &c__1);
			    dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L150: */
			}
			*scale *= scaloc;
		    }

/*                 Unpack solution vector(s) */

		    c__[is + js * c_dim1] = rhs[0];
		    c__[isp1 + js * c_dim1] = rhs[1];
		    f[is + js * f_dim1] = rhs[2];
		    f[isp1 + js * f_dim1] = rhs[3];

/*                 Substitute R(I, J) and L(I, J) into remaining */
/*                 equation. */

		    if (j > p + 2) {
			i__3 = js - 1;
			dger_(&mb, &i__3, &c_b42, rhs, &c__1, &b[js * b_dim1
				+ 1], &c__1, &f[is + f_dim1], ldf);
			i__3 = js - 1;
			dger_(&mb, &i__3, &c_b42, &rhs[2], &c__1, &e[js *
				e_dim1 + 1], &c__1, &f[is + f_dim1], ldf);
		    }
		    if (i__ < p) {
			i__3 = *m - ie;
			dgemv_("T", &mb, &i__3, &c_b27, &a[is + (ie + 1) *
				a_dim1], lda, rhs, &c__1, &c_b42, &c__[ie + 1
				+ js * c_dim1], &c__1);
			i__3 = *m - ie;
			dgemv_("T", &mb, &i__3, &c_b27, &d__[is + (ie + 1) *
				d_dim1], ldd, &rhs[2], &c__1, &c_b42, &c__[ie
				+ 1 + js * c_dim1], &c__1);
		    }

		} else if (mb == 2 && nb == 2) {

/*                 Build an 8-by-8 system Z' * x = RHS */

		    dlaset_("F", &c__8, &c__8, &c_b56, &c_b56, z__, &c__8);

		    z__[0] = a[is + is * a_dim1];
		    z__[1] = a[is + isp1 * a_dim1];
		    z__[4] = -b[js + js * b_dim1];
		    z__[6] = -b[jsp1 + js * b_dim1];

		    z__[8] = a[isp1 + is * a_dim1];
		    z__[9] = a[isp1 + isp1 * a_dim1];
		    z__[13] = -b[js + js * b_dim1];
		    z__[15] = -b[jsp1 + js * b_dim1];

		    z__[18] = a[is + is * a_dim1];
		    z__[19] = a[is + isp1 * a_dim1];
		    z__[20] = -b[js + jsp1 * b_dim1];
		    z__[22] = -b[jsp1 + jsp1 * b_dim1];

		    z__[26] = a[isp1 + is * a_dim1];
		    z__[27] = a[isp1 + isp1 * a_dim1];
		    z__[29] = -b[js + jsp1 * b_dim1];
		    z__[31] = -b[jsp1 + jsp1 * b_dim1];

		    z__[32] = d__[is + is * d_dim1];
		    z__[33] = d__[is + isp1 * d_dim1];
		    z__[36] = -e[js + js * e_dim1];

		    z__[41] = d__[isp1 + isp1 * d_dim1];
		    z__[45] = -e[js + js * e_dim1];

		    z__[50] = d__[is + is * d_dim1];
		    z__[51] = d__[is + isp1 * d_dim1];
		    z__[52] = -e[js + jsp1 * e_dim1];
		    z__[54] = -e[jsp1 + jsp1 * e_dim1];

		    z__[59] = d__[isp1 + isp1 * d_dim1];
		    z__[61] = -e[js + jsp1 * e_dim1];
		    z__[63] = -e[jsp1 + jsp1 * e_dim1];

/*                 Set up right hand side(s) */

		    k = 1;
		    ii = mb * nb + 1;
		    i__3 = nb - 1;
		    for (jj = 0; jj <= i__3; ++jj) {
			dcopy_(&mb, &c__[is + (js + jj) * c_dim1], &c__1, &
				rhs[k - 1], &c__1);
			dcopy_(&mb, &f[is + (js + jj) * f_dim1], &c__1, &rhs[
				ii - 1], &c__1);
			k += mb;
			ii += mb;
/* L160: */
		    }


/*                 Solve Z' * x = RHS */

		    dgetc2_(&zdim, z__, &c__8, ipiv, jpiv, &ierr);
		    if (ierr > 0) {
			*info = ierr;
		    }

		    dgesc2_(&zdim, z__, &c__8, rhs, ipiv, jpiv, &scaloc);
		    if (scaloc != 1.) {
			i__3 = *n;
			for (k = 1; k <= i__3; ++k) {
			    dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &c__1);
			    dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L170: */
			}
			*scale *= scaloc;
		    }

/*                 Unpack solution vector(s) */

		    k = 1;
		    ii = mb * nb + 1;
		    i__3 = nb - 1;
		    for (jj = 0; jj <= i__3; ++jj) {
			dcopy_(&mb, &rhs[k - 1], &c__1, &c__[is + (js + jj) *
				c_dim1], &c__1);
			dcopy_(&mb, &rhs[ii - 1], &c__1, &f[is + (js + jj) *
				f_dim1], &c__1);
			k += mb;
			ii += mb;
/* L180: */
		    }

/*                 Substitute R(I, J) and L(I, J) into remaining */
/*                 equation. */

		    if (j > p + 2) {
			i__3 = js - 1;
			dgemm_("N", "T", &mb, &i__3, &nb, &c_b42, &c__[is +
				js * c_dim1], ldc, &b[js * b_dim1 + 1], ldb, &
				c_b42, &f[is + f_dim1], ldf);
			i__3 = js - 1;
			dgemm_("N", "T", &mb, &i__3, &nb, &c_b42, &f[is + js *
				 f_dim1], ldf, &e[js * e_dim1 + 1], lde, &
				c_b42, &f[is + f_dim1], ldf);
		    }
		    if (i__ < p) {
			i__3 = *m - ie;
			dgemm_("T", "N", &i__3, &nb, &mb, &c_b27, &a[is + (ie
				+ 1) * a_dim1], lda, &c__[is + js * c_dim1],
				ldc, &c_b42, &c__[ie + 1 + js * c_dim1], ldc);
			i__3 = *m - ie;
			dgemm_("T", "N", &i__3, &nb, &mb, &c_b27, &d__[is + (
				ie + 1) * d_dim1], ldd, &f[is + js * f_dim1],
				ldf, &c_b42, &c__[ie + 1 + js * c_dim1], ldc);
		    }

		}

/* L190: */
	    }
/* L200: */
	}

    }
    return 0;

/*     End of DTGSY2 */

} /* dtgsy2_ */

/* Subroutine */ int dtgsyl_(const char *trans, integer *ijob, integer *m, integer *
	n, double *a, integer *lda, double *b, integer *ldb,
	double *c__, integer *ldc, double *d__, integer *ldd,
	double *e, integer *lde, double *f, integer *ldf, double *
	scale, double *dif, double *work, integer *lwork, integer *
	iwork, integer *info)
{
	/* Table of constant values */

	static integer c__2 = 2;
	static integer c_n1 = -1;
	static integer c__5 = 5;
	static double c_b14 = 0.;
	static integer c__1 = 1;
	static double c_b51 = -1.;
	static double c_b52 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, d_dim1,
	    d_offset, e_dim1, e_offset, f_dim1, f_offset, i__1, i__2, i__3,
	    i__4;

    /* Local variables */
    integer i__, j, k, p, q, ie, je, mb, nb, is, js, pq;
    double dsum;
    integer ppqq;
    integer ifunc, linfo, lwmin;
    double scale2;
    double dscale, scaloc;
    integer iround;
    bool notran;
    integer isolve;
    bool lquery;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTGSYL solves the generalized Sylvester equation: */

/*              A * R - L * B = scale * C                 (1) */
/*              D * R - L * E = scale * F */

/*  where R and L are unknown m-by-n matrices, (A, D), (B, E) and */
/*  (C, F) are given matrix pairs of size m-by-m, n-by-n and m-by-n, */
/*  respectively, with real entries. (A, D) and (B, E) must be in */
/*  generalized (real) Schur canonical form, i.e. A, B are upper quasi */
/*  triangular and D, E are upper triangular. */

/*  The solution (R, L) overwrites (C, F). 0 <= SCALE <= 1 is an output */
/*  scaling factor chosen to avoid overflow. */

/*  In matrix notation (1) is equivalent to solve  Zx = scale b, where */
/*  Z is defined as */

/*             Z = [ kron(In, A)  -kron(B', Im) ]         (2) */
/*                 [ kron(In, D)  -kron(E', Im) ]. */

/*  Here Ik is the identity matrix of size k and X' is the transpose of */
/*  X. kron(X, Y) is the Kronecker product between the matrices X and Y. */

/*  If TRANS = 'T', DTGSYL solves the transposed system Z'*y = scale*b, */
/*  which is equivalent to solve for R and L in */

/*              A' * R  + D' * L   = scale *  C           (3) */
/*              R  * B' + L  * E'  = scale * (-F) */

/*  This case (TRANS = 'T') is used to compute an one-norm-based estimate */
/*  of Dif[(A,D), (B,E)], the separation between the matrix pairs (A,D) */
/*  and (B,E), using DLACON. */

/*  If IJOB >= 1, DTGSYL computes a Frobenius norm-based estimate */
/*  of Dif[(A,D),(B,E)]. That is, the reciprocal of a lower bound on the */
/*  reciprocal of the smallest singular value of Z. See [1-2] for more */
/*  information. */

/*  This is a level 3 BLAS algorithm. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          = 'N', solve the generalized Sylvester equation (1). */
/*          = 'T', solve the 'transposed' system (3). */

/*  IJOB    (input) INTEGER */
/*          Specifies what kind of functionality to be performed. */
/*           =0: solve (1) only. */
/*           =1: The functionality of 0 and 3. */
/*           =2: The functionality of 0 and 4. */
/*           =3: Only an estimate of Dif[(A,D), (B,E)] is computed. */
/*               (look ahead strategy IJOB  = 1 is used). */
/*           =4: Only an estimate of Dif[(A,D), (B,E)] is computed. */
/*               ( DGECON on sub-systems is used ). */
/*          Not referenced if TRANS = 'T'. */

/*  M       (input) INTEGER */
/*          The order of the matrices A and D, and the row dimension of */
/*          the matrices C, F, R and L. */

/*  N       (input) INTEGER */
/*          The order of the matrices B and E, and the column dimension */
/*          of the matrices C, F, R and L. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA, M) */
/*          The upper quasi triangular matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1, M). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB, N) */
/*          The upper quasi triangular matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1, N). */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC, N) */
/*          On entry, C contains the right-hand-side of the first matrix */
/*          equation in (1) or (3). */
/*          On exit, if IJOB = 0, 1 or 2, C has been overwritten by */
/*          the solution R. If IJOB = 3 or 4 and TRANS = 'N', C holds R, */
/*          the solution achieved during the computation of the */
/*          Dif-estimate. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C. LDC >= max(1, M). */

/*  D       (input) DOUBLE PRECISION array, dimension (LDD, M) */
/*          The upper triangular matrix D. */

/*  LDD     (input) INTEGER */
/*          The leading dimension of the array D. LDD >= max(1, M). */

/*  E       (input) DOUBLE PRECISION array, dimension (LDE, N) */
/*          The upper triangular matrix E. */

/*  LDE     (input) INTEGER */
/*          The leading dimension of the array E. LDE >= max(1, N). */

/*  F       (input/output) DOUBLE PRECISION array, dimension (LDF, N) */
/*          On entry, F contains the right-hand-side of the second matrix */
/*          equation in (1) or (3). */
/*          On exit, if IJOB = 0, 1 or 2, F has been overwritten by */
/*          the solution L. If IJOB = 3 or 4 and TRANS = 'N', F holds L, */
/*          the solution achieved during the computation of the */
/*          Dif-estimate. */

/*  LDF     (input) INTEGER */
/*          The leading dimension of the array F. LDF >= max(1, M). */

/*  DIF     (output) DOUBLE PRECISION */
/*          On exit DIF is the reciprocal of a lower bound of the */
/*          reciprocal of the Dif-function, i.e. DIF is an upper bound of */
/*          Dif[(A,D), (B,E)] = sigma_min(Z), where Z as in (2). */
/*          IF IJOB = 0 or TRANS = 'T', DIF is not touched. */

/*  SCALE   (output) DOUBLE PRECISION */
/*          On exit SCALE is the scaling factor in (1) or (3). */
/*          If 0 < SCALE < 1, C and F hold the solutions R and L, resp., */
/*          to a slightly perturbed system but the input matrices A, B, D */
/*          and E have not been changed. If SCALE = 0, C and F hold the */
/*          solutions R and L, respectively, to the homogeneous system */
/*          with C = F = 0. Normally, SCALE = 1. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK > = 1. */
/*          If IJOB = 1 or 2 and TRANS = 'N', LWORK >= max(1,2*M*N). */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace) INTEGER array, dimension (M+N+6) */

/*  INFO    (output) INTEGER */
/*            =0: successful exit */
/*            <0: If INFO = -i, the i-th argument had an illegal value. */
/*            >0: (A, D) and (B, E) have common or close eigenvalues. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Bo Kagstrom and Peter Poromaa, Department of Computing Science, */
/*     Umea University, S-901 87 Umea, Sweden. */

/*  [1] B. Kagstrom and P. Poromaa, LAPACK-Style Algorithms and Software */
/*      for Solving the Generalized Sylvester Equation and Estimating the */
/*      Separation between Regular Matrix Pairs, Report UMINF - 93.23, */
/*      Department of Computing Science, Umea University, S-901 87 Umea, */
/*      Sweden, December 1993, Revised April 1994, Also as LAPACK Working */
/*      Note 75.  To appear in ACM Trans. on Math. Software, Vol 22, */
/*      No 1, 1996. */

/*  [2] B. Kagstrom, A Perturbation Analysis of the Generalized Sylvester */
/*      Equation (AR - LB, DR - LE ) = (C, F), SIAM J. Matrix Anal. */
/*      Appl., 15(4):1045-1060, 1994 */

/*  [3] B. Kagstrom and L. Westin, Generalized Schur Methods with */
/*      Condition Estimators for Solving the Generalized Sylvester */
/*      Equation, IEEE Transactions on Automatic Control, Vol. 34, No. 7, */
/*      July 1989, pp 745-751. */

/*  ===================================================================== */
/*  Replaced various illegal calls to DCOPY by calls to DLASET. */
/*  Sven Hammarling, 1/5/02. */

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

/*     Decode and test input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    d_dim1 = *ldd;
    d_offset = 1 + d_dim1;
    d__ -= d_offset;
    e_dim1 = *lde;
    e_offset = 1 + e_dim1;
    e -= e_offset;
    f_dim1 = *ldf;
    f_offset = 1 + f_dim1;
    f -= f_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    notran = lsame_(trans, "N");
    lquery = *lwork == -1;

    if (! notran && ! lsame_(trans, "T")) {
	*info = -1;
    } else if (notran) {
	if (*ijob < 0 || *ijob > 4) {
	    *info = -2;
	}
    }
    if (*info == 0) {
	if (*m <= 0) {
	    *info = -3;
	} else if (*n <= 0) {
	    *info = -4;
	} else if (*lda < std::max(1_integer,*m)) {
	    *info = -6;
	} else if (*ldb < std::max(1_integer,*n)) {
	    *info = -8;
	} else if (*ldc < std::max(1_integer,*m)) {
	    *info = -10;
	} else if (*ldd < std::max(1_integer,*m)) {
	    *info = -12;
	} else if (*lde < std::max(1_integer,*n)) {
	    *info = -14;
	} else if (*ldf < std::max(1_integer,*m)) {
	    *info = -16;
	}
    }

    if (*info == 0) {
	if (notran) {
	    if (*ijob == 1 || *ijob == 2) {
/* Computing MAX */
		i__1 = 1, i__2 = (*m << 1) * *n;
		lwmin = std::max(i__1,i__2);
	    } else {
		lwmin = 1;
	    }
	} else {
	    lwmin = 1;
	}
	work[1] = (double) lwmin;

	if (*lwork < lwmin && ! lquery) {
	    *info = -20;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTGSYL", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	*scale = 1.;
	if (notran) {
	    if (*ijob != 0) {
		*dif = 0.;
	    }
	}
	return 0;
    }

/*     Determine optimal block sizes MB and NB */

    mb = ilaenv_(&c__2, "DTGSYL", trans, m, n, &c_n1, &c_n1);
    nb = ilaenv_(&c__5, "DTGSYL", trans, m, n, &c_n1, &c_n1);

    isolve = 1;
    ifunc = 0;
    if (notran) {
	if (*ijob >= 3) {
	    ifunc = *ijob - 2;
	    dlaset_("F", m, n, &c_b14, &c_b14, &c__[c_offset], ldc)
		    ;
	    dlaset_("F", m, n, &c_b14, &c_b14, &f[f_offset], ldf);
	} else if (*ijob >= 1) {
	    isolve = 2;
	}
    }

    if (mb <= 1 && nb <= 1 || mb >= *m && nb >= *n) {

	i__1 = isolve;
	for (iround = 1; iround <= i__1; ++iround) {

/*           Use unblocked Level 2 solver */

	    dscale = 0.;
	    dsum = 1.;
	    pq = 0;
	    dtgsy2_(trans, &ifunc, m, n, &a[a_offset], lda, &b[b_offset], ldb,
		     &c__[c_offset], ldc, &d__[d_offset], ldd, &e[e_offset],
		    lde, &f[f_offset], ldf, scale, &dsum, &dscale, &iwork[1],
		    &pq, info);
	    if (dscale != 0.) {
		if (*ijob == 1 || *ijob == 3) {
		    *dif = sqrt((double) ((*m << 1) * *n)) / (dscale *
			    sqrt(dsum));
		} else {
		    *dif = sqrt((double) pq) / (dscale * sqrt(dsum));
		}
	    }

	    if (isolve == 2 && iround == 1) {
		if (notran) {
		    ifunc = *ijob;
		}
		scale2 = *scale;
		dlacpy_("F", m, n, &c__[c_offset], ldc, &work[1], m);
		dlacpy_("F", m, n, &f[f_offset], ldf, &work[*m * *n + 1], m);
		dlaset_("F", m, n, &c_b14, &c_b14, &c__[c_offset], ldc);
		dlaset_("F", m, n, &c_b14, &c_b14, &f[f_offset], ldf);
	    } else if (isolve == 2 && iround == 2) {
		dlacpy_("F", m, n, &work[1], m, &c__[c_offset], ldc);
		dlacpy_("F", m, n, &work[*m * *n + 1], m, &f[f_offset], ldf);
		*scale = scale2;
	    }
/* L30: */
	}

	return 0;
    }

/*     Determine block structure of A */

    p = 0;
    i__ = 1;
L40:
    if (i__ > *m) {
	goto L50;
    }
    ++p;
    iwork[p] = i__;
    i__ += mb;
    if (i__ >= *m) {
	goto L50;
    }
    if (a[i__ + (i__ - 1) * a_dim1] != 0.) {
	++i__;
    }
    goto L40;
L50:

    iwork[p + 1] = *m + 1;
    if (iwork[p] == iwork[p + 1]) {
	--p;
    }

/*     Determine block structure of B */

    q = p + 1;
    j = 1;
L60:
    if (j > *n) {
	goto L70;
    }
    ++q;
    iwork[q] = j;
    j += nb;
    if (j >= *n) {
	goto L70;
    }
    if (b[j + (j - 1) * b_dim1] != 0.) {
	++j;
    }
    goto L60;
L70:

    iwork[q + 1] = *n + 1;
    if (iwork[q] == iwork[q + 1]) {
	--q;
    }

    if (notran) {

	i__1 = isolve;
	for (iround = 1; iround <= i__1; ++iround) {

/*           Solve (I, J)-subsystem */
/*               A(I, I) * R(I, J) - L(I, J) * B(J, J) = C(I, J) */
/*               D(I, I) * R(I, J) - L(I, J) * E(J, J) = F(I, J) */
/*           for I = P, P - 1,..., 1; J = 1, 2,..., Q */

	    dscale = 0.;
	    dsum = 1.;
	    pq = 0;
	    *scale = 1.;
	    i__2 = q;
	    for (j = p + 2; j <= i__2; ++j) {
		js = iwork[j];
		je = iwork[j + 1] - 1;
		nb = je - js + 1;
		for (i__ = p; i__ >= 1; --i__) {
		    is = iwork[i__];
		    ie = iwork[i__ + 1] - 1;
		    mb = ie - is + 1;
		    ppqq = 0;
		    dtgsy2_(trans, &ifunc, &mb, &nb, &a[is + is * a_dim1],
			    lda, &b[js + js * b_dim1], ldb, &c__[is + js *
			    c_dim1], ldc, &d__[is + is * d_dim1], ldd, &e[js
			    + js * e_dim1], lde, &f[is + js * f_dim1], ldf, &
			    scaloc, &dsum, &dscale, &iwork[q + 2], &ppqq, &
			    linfo);
		    if (linfo > 0) {
			*info = linfo;
		    }

		    pq += ppqq;
		    if (scaloc != 1.) {
			i__3 = js - 1;
			for (k = 1; k <= i__3; ++k) {
			    dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &c__1);
			    dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L80: */
			}
			i__3 = je;
			for (k = js; k <= i__3; ++k) {
			    i__4 = is - 1;
			    dscal_(&i__4, &scaloc, &c__[k * c_dim1 + 1], &
				    c__1);
			    i__4 = is - 1;
			    dscal_(&i__4, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L90: */
			}
			i__3 = je;
			for (k = js; k <= i__3; ++k) {
			    i__4 = *m - ie;
			    dscal_(&i__4, &scaloc, &c__[ie + 1 + k * c_dim1],
				    &c__1);
			    i__4 = *m - ie;
			    dscal_(&i__4, &scaloc, &f[ie + 1 + k * f_dim1], &
				    c__1);
/* L100: */
			}
			i__3 = *n;
			for (k = je + 1; k <= i__3; ++k) {
			    dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &c__1);
			    dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L110: */
			}
			*scale *= scaloc;
		    }

/*                 Substitute R(I, J) and L(I, J) into remaining */
/*                 equation. */

		    if (i__ > 1) {
			i__3 = is - 1;
			dgemm_("N", "N", &i__3, &nb, &mb, &c_b51, &a[is *
				a_dim1 + 1], lda, &c__[is + js * c_dim1], ldc,
				 &c_b52, &c__[js * c_dim1 + 1], ldc);
			i__3 = is - 1;
			dgemm_("N", "N", &i__3, &nb, &mb, &c_b51, &d__[is *
				d_dim1 + 1], ldd, &c__[is + js * c_dim1], ldc,
				 &c_b52, &f[js * f_dim1 + 1], ldf);
		    }
		    if (j < q) {
			i__3 = *n - je;
			dgemm_("N", "N", &mb, &i__3, &nb, &c_b52, &f[is + js *
				 f_dim1], ldf, &b[js + (je + 1) * b_dim1],
				ldb, &c_b52, &c__[is + (je + 1) * c_dim1],
				ldc);
			i__3 = *n - je;
			dgemm_("N", "N", &mb, &i__3, &nb, &c_b52, &f[is + js *
				 f_dim1], ldf, &e[js + (je + 1) * e_dim1],
				lde, &c_b52, &f[is + (je + 1) * f_dim1], ldf);
		    }
/* L120: */
		}
/* L130: */
	    }
	    if (dscale != 0.) {
		if (*ijob == 1 || *ijob == 3) {
		    *dif = sqrt((double) ((*m << 1) * *n)) / (dscale *
			    sqrt(dsum));
		} else {
		    *dif = sqrt((double) pq) / (dscale * sqrt(dsum));
		}
	    }
	    if (isolve == 2 && iround == 1) {
		if (notran) {
		    ifunc = *ijob;
		}
		scale2 = *scale;
		dlacpy_("F", m, n, &c__[c_offset], ldc, &work[1], m);
		dlacpy_("F", m, n, &f[f_offset], ldf, &work[*m * *n + 1], m);
		dlaset_("F", m, n, &c_b14, &c_b14, &c__[c_offset], ldc);
		dlaset_("F", m, n, &c_b14, &c_b14, &f[f_offset], ldf);
	    } else if (isolve == 2 && iround == 2) {
		dlacpy_("F", m, n, &work[1], m, &c__[c_offset], ldc);
		dlacpy_("F", m, n, &work[*m * *n + 1], m, &f[f_offset], ldf);
		*scale = scale2;
	    }
/* L150: */
	}

    } else {

/*        Solve transposed (I, J)-subsystem */
/*             A(I, I)' * R(I, J)  + D(I, I)' * L(I, J)  =  C(I, J) */
/*             R(I, J)  * B(J, J)' + L(I, J)  * E(J, J)' = -F(I, J) */
/*        for I = 1,2,..., P; J = Q, Q-1,..., 1 */

	*scale = 1.;
	i__1 = p;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    is = iwork[i__];
	    ie = iwork[i__ + 1] - 1;
	    mb = ie - is + 1;
	    i__2 = p + 2;
	    for (j = q; j >= i__2; --j) {
		js = iwork[j];
		je = iwork[j + 1] - 1;
		nb = je - js + 1;
		dtgsy2_(trans, &ifunc, &mb, &nb, &a[is + is * a_dim1], lda, &
			b[js + js * b_dim1], ldb, &c__[is + js * c_dim1], ldc,
			 &d__[is + is * d_dim1], ldd, &e[js + js * e_dim1],
			lde, &f[is + js * f_dim1], ldf, &scaloc, &dsum, &
			dscale, &iwork[q + 2], &ppqq, &linfo);
		if (linfo > 0) {
		    *info = linfo;
		}
		if (scaloc != 1.) {
		    i__3 = js - 1;
		    for (k = 1; k <= i__3; ++k) {
			dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &c__1);
			dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L160: */
		    }
		    i__3 = je;
		    for (k = js; k <= i__3; ++k) {
			i__4 = is - 1;
			dscal_(&i__4, &scaloc, &c__[k * c_dim1 + 1], &c__1);
			i__4 = is - 1;
			dscal_(&i__4, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L170: */
		    }
		    i__3 = je;
		    for (k = js; k <= i__3; ++k) {
			i__4 = *m - ie;
			dscal_(&i__4, &scaloc, &c__[ie + 1 + k * c_dim1], &
				c__1);
			i__4 = *m - ie;
			dscal_(&i__4, &scaloc, &f[ie + 1 + k * f_dim1], &c__1)
				;
/* L180: */
		    }
		    i__3 = *n;
		    for (k = je + 1; k <= i__3; ++k) {
			dscal_(m, &scaloc, &c__[k * c_dim1 + 1], &c__1);
			dscal_(m, &scaloc, &f[k * f_dim1 + 1], &c__1);
/* L190: */
		    }
		    *scale *= scaloc;
		}

/*              Substitute R(I, J) and L(I, J) into remaining equation. */

		if (j > p + 2) {
		    i__3 = js - 1;
		    dgemm_("N", "T", &mb, &i__3, &nb, &c_b52, &c__[is + js *
			    c_dim1], ldc, &b[js * b_dim1 + 1], ldb, &c_b52, &
			    f[is + f_dim1], ldf);
		    i__3 = js - 1;
		    dgemm_("N", "T", &mb, &i__3, &nb, &c_b52, &f[is + js *
			    f_dim1], ldf, &e[js * e_dim1 + 1], lde, &c_b52, &
			    f[is + f_dim1], ldf);
		}
		if (i__ < p) {
		    i__3 = *m - ie;
		    dgemm_("T", "N", &i__3, &nb, &mb, &c_b51, &a[is + (ie + 1)
			     * a_dim1], lda, &c__[is + js * c_dim1], ldc, &
			    c_b52, &c__[ie + 1 + js * c_dim1], ldc);
		    i__3 = *m - ie;
		    dgemm_("T", "N", &i__3, &nb, &mb, &c_b51, &d__[is + (ie +
			    1) * d_dim1], ldd, &f[is + js * f_dim1], ldf, &
			    c_b52, &c__[ie + 1 + js * c_dim1], ldc);
		}
/* L200: */
	    }
/* L210: */
	}

    }

    work[1] = (double) lwmin;

    return 0;

/*     End of DTGSYL */

} /* dtgsyl_ */

/* Subroutine */ int dtpcon_(const char *norm, const char *uplo, const char *diag, integer *n,
	double *ap, double *rcond, double *work, integer *iwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1;
    double d__1;

    /* Local variables */
    integer ix, kase, kase1;
    double scale;
    integer isave[3];
    double anorm;
    bool upper;
    double xnorm;
    double ainvnm;
    bool onenrm;
    char normin[1];
    double smlnum;
    bool nounit;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     Modified to call DLACN2 in place of DLACON, 5 Feb 03, SJH. */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTPCON estimates the reciprocal of the condition number of a packed */
/*  triangular matrix A, in either the 1-norm or the infinity-norm. */

/*  The norm of A is computed and an estimate is obtained for */
/*  norm(inv(A)), then the reciprocal of the condition number is */
/*  computed as */
/*     RCOND = 1 / ( norm(A) * norm(inv(A)) ). */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies whether the 1-norm condition number or the */
/*          infinity-norm condition number is required: */
/*          = '1' or 'O':  1-norm; */
/*          = 'I':         Infinity-norm. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The upper or lower triangular matrix A, packed columnwise in */
/*          a linear array.  The j-th column of A is stored in the array */
/*          AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */
/*          If DIAG = 'U', the diagonal elements of A are not referenced */
/*          and are assumed to be 1. */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of the matrix A, */
/*          computed as RCOND = 1/(norm(A) * norm(inv(A))). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

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
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --iwork;
    --work;
    --ap;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    onenrm = *(unsigned char *)norm == '1' || lsame_(norm, "O");
    nounit = lsame_(diag, "N");

    if (! onenrm && ! lsame_(norm, "I")) {
	*info = -1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTPCON", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	*rcond = 1.;
	return 0;
    }

    *rcond = 0.;
    smlnum = dlamch_("Safe minimum") * (double) std::max(1_integer,*n);

/*     Compute the norm of the triangular matrix A. */

    anorm = dlantp_(norm, uplo, diag, n, &ap[1], &work[1]);

/*     Continue only if ANORM > 0. */

    if (anorm > 0.) {

/*        Estimate the norm of the inverse of A. */

	ainvnm = 0.;
	*(unsigned char *)normin = 'N';
	if (onenrm) {
	    kase1 = 1;
	} else {
	    kase1 = 2;
	}
	kase = 0;
L10:
	dlacn2_(n, &work[*n + 1], &work[1], &iwork[1], &ainvnm, &kase, isave);
	if (kase != 0) {
	    if (kase == kase1) {

/*              Multiply by inv(A). */

		dlatps_(uplo, "No transpose", diag, normin, n, &ap[1], &work[
			1], &scale, &work[(*n << 1) + 1], info);
	    } else {

/*              Multiply by inv(A'). */

		dlatps_(uplo, "Transpose", diag, normin, n, &ap[1], &work[1],
			&scale, &work[(*n << 1) + 1], info);
	    }
	    *(unsigned char *)normin = 'Y';

/*           Multiply by 1/SCALE if doing so will not cause overflow. */

	    if (scale != 1.) {
		ix = idamax_(n, &work[1], &c__1);
		xnorm = (d__1 = work[ix], abs(d__1));
		if (scale < xnorm * smlnum || scale == 0.) {
		    goto L20;
		}
		drscl_(n, &scale, &work[1], &c__1);
	    }
	    goto L10;
	}

/*        Compute the estimate of the reciprocal condition number. */

	if (ainvnm != 0.) {
	    *rcond = 1. / anorm / ainvnm;
	}
    }

L20:
    return 0;

/*     End of DTPCON */

} /* dtpcon_ */

/* Subroutine */ int dtprfs_(const char *uplo, const char *trans, const char *diag, integer *n,
	integer *nrhs, double *ap, double *b, integer *ldb,
	double *x, integer *ldx, double *ferr, double *berr,
	double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b19 = -1.;

    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1, i__2, i__3;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k;
    double s;
    integer kc;
    double xk;
    integer nz;
    double eps;
    integer kase;
    double safe1, safe2;
    integer isave[3];
    bool upper;
    double safmin;
    bool notran;
    char transt[1];
    bool nounit;
    double lstres;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     Modified to call DLACN2 in place of DLACON, 5 Feb 03, SJH. */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTPRFS provides error bounds and backward error estimates for the */
/*  solution to a system of linear equations with a triangular packed */
/*  coefficient matrix. */

/*  The solution matrix X must be computed by DTPTRS or some other */
/*  means before entering this routine.  DTPRFS does not do iterative */
/*  refinement because doing so cannot improve the backward error. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B  (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The upper or lower triangular matrix A, packed columnwise in */
/*          a linear array.  The j-th column of A is stored in the array */
/*          AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */
/*          If DIAG = 'U', the diagonal elements of A are not referenced */
/*          and are assumed to be 1. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The right hand side matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          The solution matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  FERR    (output) DOUBLE PRECISION array, dimension (NRHS) */
/*          The estimated forward error bound for each solution vector */
/*          X(j) (the j-th column of the solution matrix X). */
/*          If XTRUE is the true solution corresponding to X(j), FERR(j) */
/*          is an estimated upper bound for the magnitude of the largest */
/*          element in (X(j) - XTRUE) divided by the magnitude of the */
/*          largest element in X(j).  The estimate is as reliable as */
/*          the estimate for RCOND, and is almost always a slight */
/*          overestimate of the true error. */

/*  BERR    (output) DOUBLE PRECISION array, dimension (NRHS) */
/*          The componentwise relative backward error of each solution */
/*          vector X(j) (i.e., the smallest relative change in */
/*          any element of A or B that makes X(j) an exact solution). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

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
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    --ap;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    --ferr;
    --berr;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    notran = lsame_(trans, "N");
    nounit = lsame_(diag, "N");

    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T") && !
	    lsame_(trans, "C")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*nrhs < 0) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -8;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTPRFS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    ferr[j] = 0.;
	    berr[j] = 0.;
/* L10: */
	}
	return 0;
    }

    if (notran) {
	*(unsigned char *)transt = 'T';
    } else {
	*(unsigned char *)transt = 'N';
    }

/*     NZ = maximum number of nonzero elements in each row of A, plus 1 */

    nz = *n + 1;
    eps = dlamch_("Epsilon");
    safmin = dlamch_("Safe minimum");
    safe1 = nz * safmin;
    safe2 = safe1 / eps;

/*     Do for each right hand side */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {

/*        Compute residual R = B - op(A) * X, */
/*        where op(A) = A or A', depending on TRANS. */

	dcopy_(n, &x[j * x_dim1 + 1], &c__1, &work[*n + 1], &c__1);
	dtpmv_(uplo, trans, diag, n, &ap[1], &work[*n + 1], &c__1);
	daxpy_(n, &c_b19, &b[j * b_dim1 + 1], &c__1, &work[*n + 1], &c__1);

/*        Compute componentwise relative backward error from formula */

/*        max(i) ( abs(R(i)) / ( abs(op(A))*abs(X) + abs(B) )(i) ) */

/*        where abs(Z) is the componentwise absolute value of the matrix */
/*        or vector Z.  If the i-th component of the denominator is less */
/*        than SAFE2, then SAFE1 is added to the i-th components of the */
/*        numerator and denominator before dividing. */

	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[i__] = (d__1 = b[i__ + j * b_dim1], abs(d__1));
/* L20: */
	}

	if (notran) {

/*           Compute abs(A)*abs(X) + abs(B). */

	    if (upper) {
		kc = 1;
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = k;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    work[i__] += (d__1 = ap[kc + i__ - 1], abs(d__1))
				    * xk;
/* L30: */
			}
			kc += k;
/* L40: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = k - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    work[i__] += (d__1 = ap[kc + i__ - 1], abs(d__1))
				    * xk;
/* L50: */
			}
			work[k] += xk;
			kc += k;
/* L60: */
		    }
		}
	    } else {
		kc = 1;
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = *n;
			for (i__ = k; i__ <= i__3; ++i__) {
			    work[i__] += (d__1 = ap[kc + i__ - k], abs(d__1))
				    * xk;
/* L70: */
			}
			kc = kc + *n - k + 1;
/* L80: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = *n;
			for (i__ = k + 1; i__ <= i__3; ++i__) {
			    work[i__] += (d__1 = ap[kc + i__ - k], abs(d__1))
				    * xk;
/* L90: */
			}
			work[k] += xk;
			kc = kc + *n - k + 1;
/* L100: */
		    }
		}
	    }
	} else {

/*           Compute abs(A')*abs(X) + abs(B). */

	    if (upper) {
		kc = 1;
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = 0.;
			i__3 = k;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    s += (d__1 = ap[kc + i__ - 1], abs(d__1)) * (d__2
				    = x[i__ + j * x_dim1], abs(d__2));
/* L110: */
			}
			work[k] += s;
			kc += k;
/* L120: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = k - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    s += (d__1 = ap[kc + i__ - 1], abs(d__1)) * (d__2
				    = x[i__ + j * x_dim1], abs(d__2));
/* L130: */
			}
			work[k] += s;
			kc += k;
/* L140: */
		    }
		}
	    } else {
		kc = 1;
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = 0.;
			i__3 = *n;
			for (i__ = k; i__ <= i__3; ++i__) {
			    s += (d__1 = ap[kc + i__ - k], abs(d__1)) * (d__2
				    = x[i__ + j * x_dim1], abs(d__2));
/* L150: */
			}
			work[k] += s;
			kc = kc + *n - k + 1;
/* L160: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = *n;
			for (i__ = k + 1; i__ <= i__3; ++i__) {
			    s += (d__1 = ap[kc + i__ - k], abs(d__1)) * (d__2
				    = x[i__ + j * x_dim1], abs(d__2));
/* L170: */
			}
			work[k] += s;
			kc = kc + *n - k + 1;
/* L180: */
		    }
		}
	    }
	}
	s = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (work[i__] > safe2) {
/* Computing MAX */
		d__2 = s, d__3 = (d__1 = work[*n + i__], abs(d__1)) / work[
			i__];
		s = std::max(d__2,d__3);
	    } else {
/* Computing MAX */
		d__2 = s, d__3 = ((d__1 = work[*n + i__], abs(d__1)) + safe1)
			/ (work[i__] + safe1);
		s = std::max(d__2,d__3);
	    }
/* L190: */
	}
	berr[j] = s;

/*        Bound error from formula */

/*        norm(X - XTRUE) / norm(X) .le. FERR = */
/*        norm( abs(inv(op(A)))* */
/*           ( abs(R) + NZ*EPS*( abs(op(A))*abs(X)+abs(B) ))) / norm(X) */

/*        where */
/*          norm(Z) is the magnitude of the largest component of Z */
/*          inv(op(A)) is the inverse of op(A) */
/*          abs(Z) is the componentwise absolute value of the matrix or */
/*             vector Z */
/*          NZ is the maximum number of nonzeros in any row of A, plus 1 */
/*          EPS is machine epsilon */

/*        The i-th component of abs(R)+NZ*EPS*(abs(op(A))*abs(X)+abs(B)) */
/*        is incremented by SAFE1 if the i-th component of */
/*        abs(op(A))*abs(X) + abs(B) is less than SAFE2. */

/*        Use DLACN2 to estimate the infinity-norm of the matrix */
/*           inv(op(A)) * diag(W), */
/*        where W = abs(R) + NZ*EPS*( abs(op(A))*abs(X)+abs(B) ))) */

	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (work[i__] > safe2) {
		work[i__] = (d__1 = work[*n + i__], abs(d__1)) + nz * eps *
			work[i__];
	    } else {
		work[i__] = (d__1 = work[*n + i__], abs(d__1)) + nz * eps *
			work[i__] + safe1;
	    }
/* L200: */
	}

	kase = 0;
L210:
	dlacn2_(n, &work[(*n << 1) + 1], &work[*n + 1], &iwork[1], &ferr[j], &
		kase, isave);
	if (kase != 0) {
	    if (kase == 1) {

/*              Multiply by diag(W)*inv(op(A)'). */

		dtpsv_(uplo, transt, diag, n, &ap[1], &work[*n + 1], &c__1);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L220: */
		}
	    } else {

/*              Multiply by inv(op(A))*diag(W). */

		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L230: */
		}
		dtpsv_(uplo, trans, diag, n, &ap[1], &work[*n + 1], &c__1);
	    }
	    goto L210;
	}

/*        Normalize error. */

	lstres = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = lstres, d__3 = (d__1 = x[i__ + j * x_dim1], abs(d__1));
	    lstres = std::max(d__2,d__3);
/* L240: */
	}
	if (lstres != 0.) {
	    ferr[j] /= lstres;
	}

/* L250: */
    }

    return 0;

/*     End of DTPRFS */

} /* dtprfs_ */

/* Subroutine */ int dtptri_(const char *uplo, const char *diag, integer *n, double *
	ap, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer j, jc, jj;
    double ajj;
    bool upper;
    integer jclast;
    bool nounit;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTPTRI computes the inverse of a real upper or lower triangular */
/*  matrix A stored in packed format. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangular matrix A, stored */
/*          columnwise in a linear array.  The j-th column of A is stored */
/*          in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*((2*n-j)/2) = A(i,j) for j<=i<=n. */
/*          See below for further details. */
/*          On exit, the (triangular) inverse of the original matrix, in */
/*          the same packed storage format. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, A(i,i) is exactly zero.  The triangular */
/*                matrix is singular and its inverse can not be computed. */

/*  Further Details */
/*  =============== */

/*  A triangular matrix A can be transferred to packed storage using one */
/*  of the following program segments: */

/*  UPLO = 'U':                      UPLO = 'L': */

/*        JC = 1                           JC = 1 */
/*        DO 2 J = 1, N                    DO 2 J = 1, N */
/*           DO 1 I = 1, J                    DO 1 I = J, N */
/*              AP(JC+I-1) = A(I,J)              AP(JC+I-J) = A(I,J) */
/*      1    CONTINUE                    1    CONTINUE */
/*           JC = JC + J                      JC = JC + N - J + 1 */
/*      2 CONTINUE                       2 CONTINUE */

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

/*     Test the input parameters. */

    /* Parameter adjustments */
    --ap;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    nounit = lsame_(diag, "N");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTPTRI", &i__1);
	return 0;
    }

/*     Check for singularity if non-unit. */

    if (nounit) {
	if (upper) {
	    jj = 0;
	    i__1 = *n;
	    for (*info = 1; *info <= i__1; ++(*info)) {
		jj += *info;
		if (ap[jj] == 0.) {
		    return 0;
		}
/* L10: */
	    }
	} else {
	    jj = 1;
	    i__1 = *n;
	    for (*info = 1; *info <= i__1; ++(*info)) {
		if (ap[jj] == 0.) {
		    return 0;
		}
		jj = jj + *n - *info + 1;
/* L20: */
	    }
	}
	*info = 0;
    }

    if (upper) {

/*        Compute inverse of upper triangular matrix. */

	jc = 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (nounit) {
		ap[jc + j - 1] = 1. / ap[jc + j - 1];
		ajj = -ap[jc + j - 1];
	    } else {
		ajj = -1.;
	    }

/*           Compute elements 1:j-1 of j-th column. */

	    i__2 = j - 1;
	    dtpmv_("Upper", "No transpose", diag, &i__2, &ap[1], &ap[jc], &
		    c__1);
	    i__2 = j - 1;
	    dscal_(&i__2, &ajj, &ap[jc], &c__1);
	    jc += j;
/* L30: */
	}

    } else {

/*        Compute inverse of lower triangular matrix. */

	jc = *n * (*n + 1) / 2;
	for (j = *n; j >= 1; --j) {
	    if (nounit) {
		ap[jc] = 1. / ap[jc];
		ajj = -ap[jc];
	    } else {
		ajj = -1.;
	    }
	    if (j < *n) {

/*              Compute elements j+1:n of j-th column. */

		i__1 = *n - j;
		dtpmv_("Lower", "No transpose", diag, &i__1, &ap[jclast], &ap[
			jc + 1], &c__1);
		i__1 = *n - j;
		dscal_(&i__1, &ajj, &ap[jc + 1], &c__1);
	    }
	    jclast = jc;
	    jc = jc - *n + j - 2;
/* L40: */
	}
    }

    return 0;

/*     End of DTPTRI */

} /* dtptri_ */

/* Subroutine */ int dtptrs_(const char *uplo, const char *trans, const char *diag, integer *n,
	integer *nrhs, double *ap, double *b, integer *ldb, integer *
	info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer b_dim1, b_offset, i__1;

    /* Local variables */
    integer j, jc;
    bool upper;
    bool nounit;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTPTRS solves a triangular system of the form */

/*     A * X = B  or  A**T * X = B, */

/*  where A is a triangular matrix of order N stored in packed format, */
/*  and B is an N-by-NRHS matrix.  A check is made to verify that A is */
/*  nonsingular. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B  (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The upper or lower triangular matrix A, packed columnwise in */
/*          a linear array.  The j-th column of A is stored in the array */
/*          AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the right hand side matrix B. */
/*          On exit, if INFO = 0, the solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, the i-th diagonal element of A is zero, */
/*                indicating that the matrix is singular and the */
/*                solutions X have not been computed. */

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
    --ap;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    nounit = lsame_(diag, "N");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! lsame_(trans, "N") && ! lsame_(trans,
	    "T") && ! lsame_(trans, "C")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*nrhs < 0) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTPTRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Check for singularity. */

    if (nounit) {
	if (upper) {
	    jc = 1;
	    i__1 = *n;
	    for (*info = 1; *info <= i__1; ++(*info)) {
		if (ap[jc + *info - 1] == 0.) {
		    return 0;
		}
		jc += *info;
/* L10: */
	    }
	} else {
	    jc = 1;
	    i__1 = *n;
	    for (*info = 1; *info <= i__1; ++(*info)) {
		if (ap[jc] == 0.) {
		    return 0;
		}
		jc = jc + *n - *info + 1;
/* L20: */
	    }
	}
    }
    *info = 0;

/*     Solve A * x = b  or  A' * x = b. */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	dtpsv_(uplo, trans, diag, n, &ap[1], &b[j * b_dim1 + 1], &c__1);
/* L30: */
    }

    return 0;

/*     End of DTPTRS */

} /* dtptrs_ */

int dtpttf_(const char *transr, const char *uplo, integer *n, double *ap, double *arf, integer *info)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    integer i__, j, k, n1, n2, ij, jp, js, lda, ijp;
    bool normaltransr, lower, nisodd;


/*  -- LAPACK routine (version 3.2)                                    -- */

/*  -- Contributed by Fred Gustavson of the IBM Watson Research Center -- */
/*  -- November 2008                                                   -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */

/*  Purpose */
/*  ======= */

/*  DTPTTF copies a triangular matrix A from standard packed format (TP) */
/*  to rectangular full packed format (TF). */

/*  Arguments */
/*  ========= */

/*  TRANSR   (input) CHARACTER */
/*          = 'N':  ARF in Normal format is wanted; */
/*          = 'T':  ARF in Conjugate-transpose format is wanted. */

/*  UPLO    (input) CHARACTER */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension ( N*(N+1)/2 ), */
/*          On entry, the upper or lower triangular matrix A, packed */
/*          columnwise in a linear array. The j-th column of A is stored */
/*          in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */

/*  ARF     (output) DOUBLE PRECISION array, dimension ( N*(N+1)/2 ), */
/*          On exit, the upper or lower triangular matrix A stored in */
/*          RFP format. For a further discussion see Notes below. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Notes */
/*  ===== */

/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  even. We give an example where N = 6. */

/*      AP is Upper             AP is Lower */

/*   00 01 02 03 04 05       00 */
/*      11 12 13 14 15       10 11 */
/*         22 23 24 25       20 21 22 */
/*            33 34 35       30 31 32 33 */
/*               44 45       40 41 42 43 44 */
/*                  55       50 51 52 53 54 55 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:5,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(4:6,0:2) consists of */
/*  the transpose of the first three columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(1:6,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:2,0:2) consists of */
/*  the transpose of the last three columns of AP lower. */
/*  This covers the case N even and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        03 04 05                33 43 53 */
/*        13 14 15                00 44 54 */
/*        23 24 25                10 11 55 */
/*        33 34 35                20 21 22 */
/*        00 44 45                30 31 32 */
/*        01 11 55                40 41 42 */
/*        02 12 22                50 51 52 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */


/*           RFP A                   RFP A */

/*     03 13 23 33 00 01 02    33 00 10 20 30 40 50 */
/*     04 14 24 34 44 11 12    43 44 11 21 31 41 51 */
/*     05 15 25 35 45 55 22    53 54 55 22 32 42 52 */


/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  odd. We give an example where N = 5. */

/*     AP is Upper                 AP is Lower */

/*   00 01 02 03 04              00 */
/*      11 12 13 14              10 11 */
/*         22 23 24              20 21 22 */
/*            33 34              30 31 32 33 */
/*               44              40 41 42 43 44 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:4,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(3:4,0:1) consists of */
/*  the transpose of the first two columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(0:4,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:1,1:2) consists of */
/*  the transpose of the last two columns of AP lower. */
/*  This covers the case N odd and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        02 03 04                00 33 43 */
/*        12 13 14                10 11 44 */
/*        22 23 24                20 21 22 */
/*        00 33 34                30 31 32 */
/*        01 11 44                40 41 42 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */

/*           RFP A                   RFP A */

/*     02 12 22 00 01             00 10 20 30 40 50 */
/*     03 13 23 33 11             33 11 21 31 41 51 */
/*     04 14 24 34 44             43 44 22 32 42 52 */

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

    *info = 0;
    normaltransr = lsame_(transr, "N");
    lower = lsame_(uplo, "L");
    if (! normaltransr && ! lsame_(transr, "T")) {
	*info = -1;
    } else if (! lower && ! lsame_(uplo, "U")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTPTTF", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (normaltransr) {
	    arf[0] = ap[0];
	} else {
	    arf[0] = ap[0];
	}
	return 0;
    }

/*     Size of array ARF(0:NT-1) */

   // nt = *n * (*n + 1) / 2;

/*     Set N1 and N2 depending on LOWER */

    if (lower) {
	n2 = *n / 2;
	n1 = *n - n2;
    } else {
	n1 = *n / 2;
	n2 = *n - n1;
    }

/*     If N is odd, set NISODD = .TRUE. */
/*     If N is even, set K = N/2 and NISODD = .FALSE. */

/*     set lda of ARF^C; ARF^C is (0:(N+1)/2-1,0:N-noe) */
/*     where noe = 0 if n is even, noe = 1 if n is odd */

    if (*n % 2 == 0) {
	k = *n / 2;
	nisodd = false;
	lda = *n + 1;
    } else {
	nisodd = true;
	lda = *n;
    }

/*     ARF^C has lda rows and n+1-noe cols */

    if (! normaltransr) {
	lda = (*n + 1) / 2;
    }

/*     start execution: there are eight cases */

    if (nisodd) {

/*        N is odd */

	if (normaltransr) {

/*           N is odd and TRANSR = 'N' */

	    if (lower) {

/*              N is odd, TRANSR = 'N', and UPLO = 'L' */

		ijp = 0;
		jp = 0;
		i__1 = n2;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = *n - 1;
		    for (i__ = j; i__ <= i__2; ++i__) {
			ij = i__ + jp;
			arf[ij] = ap[ijp];
			++ijp;
		    }
		    jp += lda;
		}
		i__1 = n2 - 1;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__2 = n2;
		    for (j = i__ + 1; j <= i__2; ++j) {
			ij = i__ + j * lda;
			arf[ij] = ap[ijp];
			++ijp;
		    }
		}

	    } else {

/*              N is odd, TRANSR = 'N', and UPLO = 'U' */

		ijp = 0;
		i__1 = n1 - 1;
		for (j = 0; j <= i__1; ++j) {
		    ij = n2 + j;
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			arf[ij] = ap[ijp];
			++ijp;
			ij += lda;
		    }
		}
		js = 0;
		i__1 = *n - 1;
		for (j = n1; j <= i__1; ++j) {
		    ij = js;
		    i__2 = js + j;
		    for (ij = js; ij <= i__2; ++ij) {
			arf[ij] = ap[ijp];
			++ijp;
		    }
		    js += lda;
		}

	    }

	} else {

/*           N is odd and TRANSR = 'T' */

	    if (lower) {

/*              N is odd, TRANSR = 'T', and UPLO = 'L' */

		ijp = 0;
		i__1 = n2;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__2 = *n * lda - 1;
		    i__3 = lda;
		    for (ij = i__ * (lda + 1); i__3 < 0 ? ij >= i__2 : ij <=
			    i__2; ij += i__3) {
			arf[ij] = ap[ijp];
			++ijp;
		    }
		}
		js = 1;
		i__1 = n2 - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__3 = js + n2 - j - 1;
		    for (ij = js; ij <= i__3; ++ij) {
			arf[ij] = ap[ijp];
			++ijp;
		    }
		    js = js + lda + 1;
		}

	    } else {

/*              N is odd, TRANSR = 'T', and UPLO = 'U' */

		ijp = 0;
		js = n2 * lda;
		i__1 = n1 - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__3 = js + j;
		    for (ij = js; ij <= i__3; ++ij) {
			arf[ij] = ap[ijp];
			++ijp;
		    }
		    js += lda;
		}
		i__1 = n1;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__3 = i__ + (n1 + i__) * lda;
		    i__2 = lda;
		    for (ij = i__; i__2 < 0 ? ij >= i__3 : ij <= i__3; ij +=
			    i__2) {
			arf[ij] = ap[ijp];
			++ijp;
		    }
		}

	    }

	}

    } else {

/*        N is even */

	if (normaltransr) {

/*           N is even and TRANSR = 'N' */

	    if (lower) {

/*              N is even, TRANSR = 'N', and UPLO = 'L' */

		ijp = 0;
		jp = 0;
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = *n - 1;
		    for (i__ = j; i__ <= i__2; ++i__) {
			ij = i__ + 1 + jp;
			arf[ij] = ap[ijp];
			++ijp;
		    }
		    jp += lda;
		}
		i__1 = k - 1;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__2 = k - 1;
		    for (j = i__; j <= i__2; ++j) {
			ij = i__ + j * lda;
			arf[ij] = ap[ijp];
			++ijp;
		    }
		}

	    } else {

/*              N is even, TRANSR = 'N', and UPLO = 'U' */

		ijp = 0;
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    ij = k + 1 + j;
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			arf[ij] = ap[ijp];
			++ijp;
			ij += lda;
		    }
		}
		js = 0;
		i__1 = *n - 1;
		for (j = k; j <= i__1; ++j) {
		    ij = js;
		    i__2 = js + j;
		    for (ij = js; ij <= i__2; ++ij) {
			arf[ij] = ap[ijp];
			++ijp;
		    }
		    js += lda;
		}

	    }

	} else {

/*           N is even and TRANSR = 'T' */

	    if (lower) {

/*              N is even, TRANSR = 'T', and UPLO = 'L' */

		ijp = 0;
		i__1 = k - 1;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__2 = (*n + 1) * lda - 1;
		    i__3 = lda;
		    for (ij = i__ + (i__ + 1) * lda; i__3 < 0 ? ij >= i__2 :
			    ij <= i__2; ij += i__3) {
			arf[ij] = ap[ijp];
			++ijp;
		    }
		}
		js = 0;
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__3 = js + k - j - 1;
		    for (ij = js; ij <= i__3; ++ij) {
			arf[ij] = ap[ijp];
			++ijp;
		    }
		    js = js + lda + 1;
		}

	    } else {

/*              N is even, TRANSR = 'T', and UPLO = 'U' */

		ijp = 0;
		js = (k + 1) * lda;
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__3 = js + j;
		    for (ij = js; ij <= i__3; ++ij) {
			arf[ij] = ap[ijp];
			++ijp;
		    }
		    js += lda;
		}
		i__1 = k - 1;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    i__3 = i__ + (k + i__) * lda;
		    i__2 = lda;
		    for (ij = i__; i__2 < 0 ? ij >= i__3 : ij <= i__3; ij +=
			    i__2) {
			arf[ij] = ap[ijp];
			++ijp;
		    }
		}

	    }

	}

    }

    return 0;

/*     End of DTPTTF */

} /* dtpttf_ */

int dtpttr_(const char *uplo, integer *n, double *ap, double *a, integer *lda, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, k;
    bool lower;

/*  -- LAPACK routine (version 3.2)                                    -- */

/*  -- Contributed by Julien Langou of the Univ. of Colorado Denver    -- */
/*  -- November 2008                                                   -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTPTTR copies a triangular matrix A from standard packed format (TP) */
/*  to standard full format (TR). */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER */
/*          = 'U':  A is upper triangular. */
/*          = 'L':  A is lower triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A. N >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension ( N*(N+1)/2 ), */
/*          On entry, the upper or lower triangular matrix A, packed */
/*          columnwise in a linear array. The j-th column of A is stored */
/*          in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */

/*  A       (output) DOUBLE PRECISION array, dimension ( LDA, N ) */
/*          On exit, the triangular matrix A.  If UPLO = 'U', the leading */
/*          N-by-N upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading N-by-N lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

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

/*     Test the input parameters. */

    /* Parameter adjustments */
    --ap;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    *info = 0;
    lower = lsame_(uplo, "L");
    if (! lower && ! lsame_(uplo, "U")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTPTTR", &i__1);
	return 0;
    }

    if (lower) {
	k = 0;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j; i__ <= i__2; ++i__) {
		++k;
		a[i__ + j * a_dim1] = ap[k];
	    }
	}
    } else {
	k = 0;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		++k;
		a[i__ + j * a_dim1] = ap[k];
	    }
	}
    }


    return 0;

/*     End of DTPTTR */

} /* dtpttr_ */

/* Subroutine */ int dtrcon_(const char *norm, const char *uplo, const char *diag, integer *n,
	double *a, integer *lda, double *rcond, double *work,
	integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1;
    double d__1;

    /* Local variables */
    integer ix, kase, kase1;
    double scale;
    integer isave[3];
    double anorm;
    bool upper;
    double xnorm;
    double ainvnm;
    bool onenrm;
    char normin[1];
    double smlnum;
    bool nounit;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     Modified to call DLACN2 in place of DLACON, 5 Feb 03, SJH. */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRCON estimates the reciprocal of the condition number of a */
/*  triangular matrix A, in either the 1-norm or the infinity-norm. */

/*  The norm of A is computed and an estimate is obtained for */
/*  norm(inv(A)), then the reciprocal of the condition number is */
/*  computed as */
/*     RCOND = 1 / ( norm(A) * norm(inv(A)) ). */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies whether the 1-norm condition number or the */
/*          infinity-norm condition number is required: */
/*          = '1' or 'O':  1-norm; */
/*          = 'I':         Infinity-norm. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The triangular matrix A.  If UPLO = 'U', the leading N-by-N */
/*          upper triangular part of the array A contains the upper */
/*          triangular matrix, and the strictly lower triangular part of */
/*          A is not referenced.  If UPLO = 'L', the leading N-by-N lower */
/*          triangular part of the array A contains the lower triangular */
/*          matrix, and the strictly upper triangular part of A is not */
/*          referenced.  If DIAG = 'U', the diagonal elements of A are */
/*          also not referenced and are assumed to be 1. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of the matrix A, */
/*          computed as RCOND = 1/(norm(A) * norm(inv(A))). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

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
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    onenrm = *(unsigned char *)norm == '1' || lsame_(norm, "O");
    nounit = lsame_(diag, "N");

    if (! onenrm && ! lsame_(norm, "I")) {
	*info = -1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTRCON", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	*rcond = 1.;
	return 0;
    }

    *rcond = 0.;
    smlnum = dlamch_("Safe minimum") * (double) std::max(1_integer,*n);

/*     Compute the norm of the triangular matrix A. */

    anorm = dlantr_(norm, uplo, diag, n, n, &a[a_offset], lda, &work[1]);

/*     Continue only if ANORM > 0. */

    if (anorm > 0.) {

/*        Estimate the norm of the inverse of A. */

	ainvnm = 0.;
	*(unsigned char *)normin = 'N';
	if (onenrm) {
	    kase1 = 1;
	} else {
	    kase1 = 2;
	}
	kase = 0;
L10:
	dlacn2_(n, &work[*n + 1], &work[1], &iwork[1], &ainvnm, &kase, isave);
	if (kase != 0) {
	    if (kase == kase1) {

/*              Multiply by inv(A). */

		dlatrs_(uplo, "No transpose", diag, normin, n, &a[a_offset],
			lda, &work[1], &scale, &work[(*n << 1) + 1], info);
	    } else {

/*              Multiply by inv(A'). */

		dlatrs_(uplo, "Transpose", diag, normin, n, &a[a_offset], lda,
			 &work[1], &scale, &work[(*n << 1) + 1], info);
	    }
	    *(unsigned char *)normin = 'Y';

/*           Multiply by 1/SCALE if doing so will not cause overflow. */

	    if (scale != 1.) {
		ix = idamax_(n, &work[1], &c__1);
		xnorm = (d__1 = work[ix], abs(d__1));
		if (scale < xnorm * smlnum || scale == 0.) {
		    goto L20;
		}
		drscl_(n, &scale, &work[1], &c__1);
	    }
	    goto L10;
	}

/*        Compute the estimate of the reciprocal condition number. */

	if (ainvnm != 0.) {
	    *rcond = 1. / anorm / ainvnm;
	}
    }

L20:
    return 0;

/*     End of DTRCON */

} /* dtrcon_ */

/* Subroutine */ int dtrevc_(const char *side, const char *howmny, bool *select,
	integer *n, double *t, integer *ldt, double *vl, integer *
	ldvl, double *vr, integer *ldvr, integer *mm, integer *m,
	double *work, integer *info)
{
	/* Table of constant values */
	static bool c_false = false;
	static integer c__1 = 1;
	static double c_b22 = 1.;
	static double c_b25 = 0.;
	static integer c__2 = 2;
	static bool c_true = true;

    /* System generated locals */
    integer t_dim1, t_offset, vl_dim1, vl_offset, vr_dim1, vr_offset, i__1,
	    i__2, i__3;
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    integer i__, j, k;
    double x[4]	/* was [2][2] */;
    integer j1, j2, n2, ii, ki, ip, is;
    double wi, wr, rec, ulp, beta, emax;
    bool pair;
    bool allv;
    integer ierr;
    double unfl, ovfl, smin;
    bool over;
    double vmax;
    integer jnxt;
    double scale;
    double remax;
    bool leftv, bothv;
    double vcrit;
    bool somev;
    double xnorm;
    double bignum;
    bool rightv;
    double smlnum;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTREVC computes some or all of the right and/or left eigenvectors of */
/*  a real upper quasi-triangular matrix T. */
/*  Matrices of this type are produced by the Schur factorization of */
/*  a real general matrix:  A = Q*T*Q**T, as computed by DHSEQR. */

/*  The right eigenvector x and the left eigenvector y of T corresponding */
/*  to an eigenvalue w are defined by: */

/*     T*x = w*x,     (y**H)*T = w*(y**H) */

/*  where y**H denotes the conjugate transpose of y. */
/*  The eigenvalues are not input to this routine, but are read directly */
/*  from the diagonal blocks of T. */

/*  This routine returns the matrices X and/or Y of right and left */
/*  eigenvectors of T, or the products Q*X and/or Q*Y, where Q is an */
/*  input matrix.  If Q is the orthogonal factor that reduces a matrix */
/*  A to Schur form T, then Q*X and Q*Y are the matrices of right and */
/*  left eigenvectors of A. */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'R':  compute right eigenvectors only; */
/*          = 'L':  compute left eigenvectors only; */
/*          = 'B':  compute both right and left eigenvectors. */

/*  HOWMNY  (input) CHARACTER*1 */
/*          = 'A':  compute all right and/or left eigenvectors; */
/*          = 'B':  compute all right and/or left eigenvectors, */
/*                  backtransformed by the matrices in VR and/or VL; */
/*          = 'S':  compute selected right and/or left eigenvectors, */
/*                  as indicated by the logical array SELECT. */

/*  SELECT  (input/output) LOGICAL array, dimension (N) */
/*          If HOWMNY = 'S', SELECT specifies the eigenvectors to be */
/*          computed. */
/*          If w(j) is a real eigenvalue, the corresponding real */
/*          eigenvector is computed if SELECT(j) is .TRUE.. */
/*          If w(j) and w(j+1) are the real and imaginary parts of a */
/*          complex eigenvalue, the corresponding complex eigenvector is */
/*          computed if either SELECT(j) or SELECT(j+1) is .TRUE., and */
/*          on exit SELECT(j) is set to .TRUE. and SELECT(j+1) is set to */
/*          .FALSE.. */
/*          Not referenced if HOWMNY = 'A' or 'B'. */

/*  N       (input) INTEGER */
/*          The order of the matrix T. N >= 0. */

/*  T       (input) DOUBLE PRECISION array, dimension (LDT,N) */
/*          The upper quasi-triangular matrix T in Schur canonical form. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T. LDT >= max(1,N). */

/*  VL      (input/output) DOUBLE PRECISION array, dimension (LDVL,MM) */
/*          On entry, if SIDE = 'L' or 'B' and HOWMNY = 'B', VL must */
/*          contain an N-by-N matrix Q (usually the orthogonal matrix Q */
/*          of Schur vectors returned by DHSEQR). */
/*          On exit, if SIDE = 'L' or 'B', VL contains: */
/*          if HOWMNY = 'A', the matrix Y of left eigenvectors of T; */
/*          if HOWMNY = 'B', the matrix Q*Y; */
/*          if HOWMNY = 'S', the left eigenvectors of T specified by */
/*                           SELECT, stored consecutively in the columns */
/*                           of VL, in the same order as their */
/*                           eigenvalues. */
/*          A complex eigenvector corresponding to a complex eigenvalue */
/*          is stored in two consecutive columns, the first holding the */
/*          real part, and the second the imaginary part. */
/*          Not referenced if SIDE = 'R'. */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of the array VL.  LDVL >= 1, and if */
/*          SIDE = 'L' or 'B', LDVL >= N. */

/*  VR      (input/output) DOUBLE PRECISION array, dimension (LDVR,MM) */
/*          On entry, if SIDE = 'R' or 'B' and HOWMNY = 'B', VR must */
/*          contain an N-by-N matrix Q (usually the orthogonal matrix Q */
/*          of Schur vectors returned by DHSEQR). */
/*          On exit, if SIDE = 'R' or 'B', VR contains: */
/*          if HOWMNY = 'A', the matrix X of right eigenvectors of T; */
/*          if HOWMNY = 'B', the matrix Q*X; */
/*          if HOWMNY = 'S', the right eigenvectors of T specified by */
/*                           SELECT, stored consecutively in the columns */
/*                           of VR, in the same order as their */
/*                           eigenvalues. */
/*          A complex eigenvector corresponding to a complex eigenvalue */
/*          is stored in two consecutive columns, the first holding the */
/*          real part and the second the imaginary part. */
/*          Not referenced if SIDE = 'L'. */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the array VR.  LDVR >= 1, and if */
/*          SIDE = 'R' or 'B', LDVR >= N. */

/*  MM      (input) INTEGER */
/*          The number of columns in the arrays VL and/or VR. MM >= M. */

/*  M       (output) INTEGER */
/*          The number of columns in the arrays VL and/or VR actually */
/*          used to store the eigenvectors. */
/*          If HOWMNY = 'A' or 'B', M is set to N. */
/*          Each selected real eigenvector occupies one column and each */
/*          selected complex eigenvector occupies two columns. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The algorithm used in this program is basically backward (forward) */
/*  substitution, with scaling to make the the code robust against */
/*  possible overflow. */

/*  Each eigenvector is normalized so that the element of largest */
/*  magnitude has magnitude 1; here the magnitude of a complex number */
/*  (x,y) is taken to be |x| + |y|. */

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
/*     .. Local Arrays .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Decode and test the input parameters */

    /* Parameter adjustments */
    --select;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    --work;

    /* Function Body */
    bothv = lsame_(side, "B");
    rightv = lsame_(side, "R") || bothv;
    leftv = lsame_(side, "L") || bothv;

    allv = lsame_(howmny, "A");
    over = lsame_(howmny, "B");
    somev = lsame_(howmny, "S");

    *info = 0;
    if (! rightv && ! leftv) {
	*info = -1;
    } else if (! allv && ! over && ! somev) {
	*info = -2;
    } else if (*n < 0) {
	*info = -4;
    } else if (*ldt < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldvl < 1 || leftv && *ldvl < *n) {
	*info = -8;
    } else if (*ldvr < 1 || rightv && *ldvr < *n) {
	*info = -10;
    } else {

/*        Set M to the number of columns required to store the selected */
/*        eigenvectors, standardize the array SELECT if necessary, and */
/*        test MM. */

	if (somev) {
	    *m = 0;
	    pair = false;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (pair) {
		    pair = false;
		    select[j] = false;
		} else {
		    if (j < *n) {
			if (t[j + 1 + j * t_dim1] == 0.) {
			    if (select[j]) {
				++(*m);
			    }
			} else {
			    pair = true;
			    if (select[j] || select[j + 1]) {
				select[j] = true;
				*m += 2;
			    }
			}
		    } else {
			if (select[*n]) {
			    ++(*m);
			}
		    }
		}
/* L10: */
	    }
	} else {
	    *m = *n;
	}

	if (*mm < *m) {
	    *info = -11;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTREVC", &i__1);
	return 0;
    }

/*     Quick return if possible. */

    if (*n == 0) {
	return 0;
    }

/*     Set the constants to control overflow. */

    unfl = dlamch_("Safe minimum");
    ovfl = 1. / unfl;
    dlabad_(&unfl, &ovfl);
    ulp = dlamch_("Precision");
    smlnum = unfl * (*n / ulp);
    bignum = (1. - ulp) / smlnum;

/*     Compute 1-norm of each column of strictly upper triangular */
/*     part of T to control overflow in triangular solver. */

    work[1] = 0.;
    i__1 = *n;
    for (j = 2; j <= i__1; ++j) {
	work[j] = 0.;
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[j] += (d__1 = t[i__ + j * t_dim1], abs(d__1));
/* L20: */
	}
/* L30: */
    }

/*     Index IP is used to specify the real or complex eigenvalue: */
/*       IP = 0, real eigenvalue, */
/*            1, first of conjugate complex pair: (wr,wi) */
/*           -1, second of conjugate complex pair: (wr,wi) */

    n2 = *n << 1;

    if (rightv) {

/*        Compute right eigenvectors. */

	ip = 0;
	is = *m;
	for (ki = *n; ki >= 1; --ki) {

	    if (ip == 1) {
		goto L130;
	    }
	    if (ki == 1) {
		goto L40;
	    }
	    if (t[ki + (ki - 1) * t_dim1] == 0.) {
		goto L40;
	    }
	    ip = -1;

L40:
	    if (somev) {
		if (ip == 0) {
		    if (! select[ki]) {
			goto L130;
		    }
		} else {
		    if (! select[ki - 1]) {
			goto L130;
		    }
		}
	    }

/*           Compute the KI-th eigenvalue (WR,WI). */

	    wr = t[ki + ki * t_dim1];
	    wi = 0.;
	    if (ip != 0) {
		wi = sqrt((d__1 = t[ki + (ki - 1) * t_dim1], abs(d__1))) *
			sqrt((d__2 = t[ki - 1 + ki * t_dim1], abs(d__2)));
	    }
/* Computing MAX */
	    d__1 = ulp * (abs(wr) + abs(wi));
	    smin = std::max(d__1,smlnum);

	    if (ip == 0) {

/*              Real right eigenvector */

		work[ki + *n] = 1.;

/*              Form right-hand side */

		i__1 = ki - 1;
		for (k = 1; k <= i__1; ++k) {
		    work[k + *n] = -t[k + ki * t_dim1];
/* L50: */
		}

/*              Solve the upper quasi-triangular system: */
/*                 (T(1:KI-1,1:KI-1) - WR)*X = SCALE*WORK. */

		jnxt = ki - 1;
		for (j = ki - 1; j >= 1; --j) {
		    if (j > jnxt) {
			goto L60;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j - 1;
		    if (j > 1) {
			if (t[j + (j - 1) * t_dim1] != 0.) {
			    j1 = j - 1;
			    jnxt = j - 2;
			}
		    }

		    if (j1 == j2) {

/*                    1-by-1 diagonal block */

			dlaln2_(&c_false, &c__1, &c__1, &smin, &c_b22, &t[j +
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &c_b25, x, &c__2, &scale, &xnorm,
				&ierr);

/*                    Scale X(1,1) to avoid overflow when updating */
/*                    the right-hand side. */

			if (xnorm > 1.) {
			    if (work[j] > bignum / xnorm) {
				x[0] /= xnorm;
				scale /= xnorm;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			}
			work[j + *n] = x[0];

/*                    Update right-hand side */

			i__1 = j - 1;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);

		    } else {

/*                    2-by-2 diagonal block */

			dlaln2_(&c_false, &c__2, &c__1, &smin, &c_b22, &t[j -
				1 + (j - 1) * t_dim1], ldt, &c_b22, &c_b22, &
				work[j - 1 + *n], n, &wr, &c_b25, x, &c__2, &
				scale, &xnorm, &ierr);

/*                    Scale X(1,1) and X(2,1) to avoid overflow when */
/*                    updating the right-hand side. */

			if (xnorm > 1.) {
/* Computing MAX */
			    d__1 = work[j - 1], d__2 = work[j];
			    beta = std::max(d__1,d__2);
			    if (beta > bignum / xnorm) {
				x[0] /= xnorm;
				x[1] /= xnorm;
				scale /= xnorm;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			}
			work[j - 1 + *n] = x[0];
			work[j + *n] = x[1];

/*                    Update right-hand side */

			i__1 = j - 2;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[(j - 1) * t_dim1 + 1], &c__1,
				&work[*n + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[1];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);
		    }
L60:
		    ;
		}

/*              Copy the vector x or Q*x to VR and normalize. */

		if (! over) {
		    dcopy_(&ki, &work[*n + 1], &c__1, &vr[is * vr_dim1 + 1], &
			    c__1);

		    ii = idamax_(&ki, &vr[is * vr_dim1 + 1], &c__1);
		    remax = 1. / (d__1 = vr[ii + is * vr_dim1], abs(d__1));
		    dscal_(&ki, &remax, &vr[is * vr_dim1 + 1], &c__1);

		    i__1 = *n;
		    for (k = ki + 1; k <= i__1; ++k) {
			vr[k + is * vr_dim1] = 0.;
/* L70: */
		    }
		} else {
		    if (ki > 1) {
			i__1 = ki - 1;
			dgemv_("N", n, &i__1, &c_b22, &vr[vr_offset], ldvr, &
				work[*n + 1], &c__1, &work[ki + *n], &vr[ki *
				vr_dim1 + 1], &c__1);
		    }

		    ii = idamax_(n, &vr[ki * vr_dim1 + 1], &c__1);
		    remax = 1. / (d__1 = vr[ii + ki * vr_dim1], abs(d__1));
		    dscal_(n, &remax, &vr[ki * vr_dim1 + 1], &c__1);
		}

	    } else {

/*              Complex right eigenvector. */

/*              Initial solve */
/*                [ (T(KI-1,KI-1) T(KI-1,KI) ) - (WR + I* WI)]*X = 0. */
/*                [ (T(KI,KI-1)   T(KI,KI)   )               ] */

		if ((d__1 = t[ki - 1 + ki * t_dim1], abs(d__1)) >= (d__2 = t[
			ki + (ki - 1) * t_dim1], abs(d__2))) {
		    work[ki - 1 + *n] = 1.;
		    work[ki + n2] = wi / t[ki - 1 + ki * t_dim1];
		} else {
		    work[ki - 1 + *n] = -wi / t[ki + (ki - 1) * t_dim1];
		    work[ki + n2] = 1.;
		}
		work[ki + *n] = 0.;
		work[ki - 1 + n2] = 0.;

/*              Form right-hand side */

		i__1 = ki - 2;
		for (k = 1; k <= i__1; ++k) {
		    work[k + *n] = -work[ki - 1 + *n] * t[k + (ki - 1) *
			    t_dim1];
		    work[k + n2] = -work[ki + n2] * t[k + ki * t_dim1];
/* L80: */
		}

/*              Solve upper quasi-triangular system: */
/*              (T(1:KI-2,1:KI-2) - (WR+i*WI))*X = SCALE*(WORK+i*WORK2) */

		jnxt = ki - 2;
		for (j = ki - 2; j >= 1; --j) {
		    if (j > jnxt) {
			goto L90;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j - 1;
		    if (j > 1) {
			if (t[j + (j - 1) * t_dim1] != 0.) {
			    j1 = j - 1;
			    jnxt = j - 2;
			}
		    }

		    if (j1 == j2) {

/*                    1-by-1 diagonal block */

			dlaln2_(&c_false, &c__1, &c__2, &smin, &c_b22, &t[j +
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &wi, x, &c__2, &scale, &xnorm, &
				ierr);

/*                    Scale X(1,1) and X(1,2) to avoid overflow when */
/*                    updating the right-hand side. */

			if (xnorm > 1.) {
			    if (work[j] > bignum / xnorm) {
				x[0] /= xnorm;
				x[2] /= xnorm;
				scale /= xnorm;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			    dscal_(&ki, &scale, &work[n2 + 1], &c__1);
			}
			work[j + *n] = x[0];
			work[j + n2] = x[2];

/*                    Update the right-hand side */

			i__1 = j - 1;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);
			i__1 = j - 1;
			d__1 = -x[2];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				n2 + 1], &c__1);

		    } else {

/*                    2-by-2 diagonal block */

			dlaln2_(&c_false, &c__2, &c__2, &smin, &c_b22, &t[j -
				1 + (j - 1) * t_dim1], ldt, &c_b22, &c_b22, &
				work[j - 1 + *n], n, &wr, &wi, x, &c__2, &
				scale, &xnorm, &ierr);

/*                    Scale X to avoid overflow when updating */
/*                    the right-hand side. */

			if (xnorm > 1.) {
/* Computing MAX */
			    d__1 = work[j - 1], d__2 = work[j];
			    beta = std::max(d__1,d__2);
			    if (beta > bignum / xnorm) {
				rec = 1. / xnorm;
				x[0] *= rec;
				x[2] *= rec;
				x[1] *= rec;
				x[3] *= rec;
				scale *= rec;
			    }
			}

/*                    Scale if necessary */

			if (scale != 1.) {
			    dscal_(&ki, &scale, &work[*n + 1], &c__1);
			    dscal_(&ki, &scale, &work[n2 + 1], &c__1);
			}
			work[j - 1 + *n] = x[0];
			work[j + *n] = x[1];
			work[j - 1 + n2] = x[2];
			work[j + n2] = x[3];

/*                    Update the right-hand side */

			i__1 = j - 2;
			d__1 = -x[0];
			daxpy_(&i__1, &d__1, &t[(j - 1) * t_dim1 + 1], &c__1,
				&work[*n + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[1];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				*n + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[2];
			daxpy_(&i__1, &d__1, &t[(j - 1) * t_dim1 + 1], &c__1,
				&work[n2 + 1], &c__1);
			i__1 = j - 2;
			d__1 = -x[3];
			daxpy_(&i__1, &d__1, &t[j * t_dim1 + 1], &c__1, &work[
				n2 + 1], &c__1);
		    }
L90:
		    ;
		}

/*              Copy the vector x or Q*x to VR and normalize. */

		if (! over) {
		    dcopy_(&ki, &work[*n + 1], &c__1, &vr[(is - 1) * vr_dim1
			    + 1], &c__1);
		    dcopy_(&ki, &work[n2 + 1], &c__1, &vr[is * vr_dim1 + 1], &
			    c__1);

		    emax = 0.;
		    i__1 = ki;
		    for (k = 1; k <= i__1; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vr[k + (is - 1) * vr_dim1]
				, abs(d__1)) + (d__2 = vr[k + is * vr_dim1],
				abs(d__2));
			emax = std::max(d__3,d__4);
/* L100: */
		    }

		    remax = 1. / emax;
		    dscal_(&ki, &remax, &vr[(is - 1) * vr_dim1 + 1], &c__1);
		    dscal_(&ki, &remax, &vr[is * vr_dim1 + 1], &c__1);

		    i__1 = *n;
		    for (k = ki + 1; k <= i__1; ++k) {
			vr[k + (is - 1) * vr_dim1] = 0.;
			vr[k + is * vr_dim1] = 0.;
/* L110: */
		    }

		} else {

		    if (ki > 2) {
			i__1 = ki - 2;
			dgemv_("N", n, &i__1, &c_b22, &vr[vr_offset], ldvr, &
				work[*n + 1], &c__1, &work[ki - 1 + *n], &vr[(
				ki - 1) * vr_dim1 + 1], &c__1);
			i__1 = ki - 2;
			dgemv_("N", n, &i__1, &c_b22, &vr[vr_offset], ldvr, &
				work[n2 + 1], &c__1, &work[ki + n2], &vr[ki *
				vr_dim1 + 1], &c__1);
		    } else {
			dscal_(n, &work[ki - 1 + *n], &vr[(ki - 1) * vr_dim1
				+ 1], &c__1);
			dscal_(n, &work[ki + n2], &vr[ki * vr_dim1 + 1], &
				c__1);
		    }

		    emax = 0.;
		    i__1 = *n;
		    for (k = 1; k <= i__1; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vr[k + (ki - 1) * vr_dim1]
				, abs(d__1)) + (d__2 = vr[k + ki * vr_dim1],
				abs(d__2));
			emax = std::max(d__3,d__4);
/* L120: */
		    }
		    remax = 1. / emax;
		    dscal_(n, &remax, &vr[(ki - 1) * vr_dim1 + 1], &c__1);
		    dscal_(n, &remax, &vr[ki * vr_dim1 + 1], &c__1);
		}
	    }

	    --is;
	    if (ip != 0) {
		--is;
	    }
L130:
	    if (ip == 1) {
		ip = 0;
	    }
	    if (ip == -1) {
		ip = 1;
	    }
/* L140: */
	}
    }

    if (leftv) {

/*        Compute left eigenvectors. */

	ip = 0;
	is = 1;
	i__1 = *n;
	for (ki = 1; ki <= i__1; ++ki) {

	    if (ip == -1) {
		goto L250;
	    }
	    if (ki == *n) {
		goto L150;
	    }
	    if (t[ki + 1 + ki * t_dim1] == 0.) {
		goto L150;
	    }
	    ip = 1;

L150:
	    if (somev) {
		if (! select[ki]) {
		    goto L250;
		}
	    }

/*           Compute the KI-th eigenvalue (WR,WI). */

	    wr = t[ki + ki * t_dim1];
	    wi = 0.;
	    if (ip != 0) {
		wi = sqrt((d__1 = t[ki + (ki + 1) * t_dim1], abs(d__1))) *
			sqrt((d__2 = t[ki + 1 + ki * t_dim1], abs(d__2)));
	    }
/* Computing MAX */
	    d__1 = ulp * (abs(wr) + abs(wi));
	    smin = std::max(d__1,smlnum);

	    if (ip == 0) {

/*              Real left eigenvector. */

		work[ki + *n] = 1.;

/*              Form right-hand side */

		i__2 = *n;
		for (k = ki + 1; k <= i__2; ++k) {
		    work[k + *n] = -t[ki + k * t_dim1];
/* L160: */
		}

/*              Solve the quasi-triangular system: */
/*                 (T(KI+1:N,KI+1:N) - WR)'*X = SCALE*WORK */

		vmax = 1.;
		vcrit = bignum;

		jnxt = ki + 1;
		i__2 = *n;
		for (j = ki + 1; j <= i__2; ++j) {
		    if (j < jnxt) {
			goto L170;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j + 1;
		    if (j < *n) {
			if (t[j + 1 + j * t_dim1] != 0.) {
			    j2 = j + 1;
			    jnxt = j + 2;
			}
		    }

		    if (j1 == j2) {

/*                    1-by-1 diagonal block */

/*                    Scale if necessary to avoid overflow when forming */
/*                    the right-hand side. */

			if (work[j] > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 1;
			work[j + *n] -= ddot_(&i__3, &t[ki + 1 + j * t_dim1],
				&c__1, &work[ki + 1 + *n], &c__1);

/*                    Solve (T(J,J)-WR)'*X = WORK */

			dlaln2_(&c_false, &c__1, &c__1, &smin, &c_b22, &t[j +
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &c_b25, x, &c__2, &scale, &xnorm,
				&ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			}
			work[j + *n] = x[0];
/* Computing MAX */
			d__2 = (d__1 = work[j + *n], abs(d__1));
			vmax = std::max(d__2,vmax);
			vcrit = bignum / vmax;

		    } else {

/*                    2-by-2 diagonal block */

/*                    Scale if necessary to avoid overflow when forming */
/*                    the right-hand side. */

/* Computing MAX */
			d__1 = work[j], d__2 = work[j + 1];
			beta = std::max(d__1,d__2);
			if (beta > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 1;
			work[j + *n] -= ddot_(&i__3, &t[ki + 1 + j * t_dim1],
				&c__1, &work[ki + 1 + *n], &c__1);

			i__3 = j - ki - 1;
			work[j + 1 + *n] -= ddot_(&i__3, &t[ki + 1 + (j + 1) *
				 t_dim1], &c__1, &work[ki + 1 + *n], &c__1);

/*                    Solve */
/*                      [T(J,J)-WR   T(J,J+1)     ]'* X = SCALE*( WORK1 ) */
/*                      [T(J+1,J)    T(J+1,J+1)-WR]             ( WORK2 ) */

			dlaln2_(&c_true, &c__2, &c__1, &smin, &c_b22, &t[j +
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &c_b25, x, &c__2, &scale, &xnorm,
				&ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			}
			work[j + *n] = x[0];
			work[j + 1 + *n] = x[1];

/* Computing MAX */
			d__3 = (d__1 = work[j + *n], abs(d__1)), d__4 = (d__2
				= work[j + 1 + *n], abs(d__2)), d__3 = std::max(
				d__3,d__4);
			vmax = std::max(d__3,vmax);
			vcrit = bignum / vmax;

		    }
L170:
		    ;
		}

/*              Copy the vector x or Q*x to VL and normalize. */

		if (! over) {
		    i__2 = *n - ki + 1;
		    dcopy_(&i__2, &work[ki + *n], &c__1, &vl[ki + is *
			    vl_dim1], &c__1);

		    i__2 = *n - ki + 1;
		    ii = idamax_(&i__2, &vl[ki + is * vl_dim1], &c__1) + ki -
			    1;
		    remax = 1. / (d__1 = vl[ii + is * vl_dim1], abs(d__1));
		    i__2 = *n - ki + 1;
		    dscal_(&i__2, &remax, &vl[ki + is * vl_dim1], &c__1);

		    i__2 = ki - 1;
		    for (k = 1; k <= i__2; ++k) {
			vl[k + is * vl_dim1] = 0.;
/* L180: */
		    }

		} else {

		    if (ki < *n) {
			i__2 = *n - ki;
			dgemv_("N", n, &i__2, &c_b22, &vl[(ki + 1) * vl_dim1
				+ 1], ldvl, &work[ki + 1 + *n], &c__1, &work[
				ki + *n], &vl[ki * vl_dim1 + 1], &c__1);
		    }

		    ii = idamax_(n, &vl[ki * vl_dim1 + 1], &c__1);
		    remax = 1. / (d__1 = vl[ii + ki * vl_dim1], abs(d__1));
		    dscal_(n, &remax, &vl[ki * vl_dim1 + 1], &c__1);

		}

	    } else {

/*              Complex left eigenvector. */

/*               Initial solve: */
/*                 ((T(KI,KI)    T(KI,KI+1) )' - (WR - I* WI))*X = 0. */
/*                 ((T(KI+1,KI) T(KI+1,KI+1))                ) */

		if ((d__1 = t[ki + (ki + 1) * t_dim1], abs(d__1)) >= (d__2 =
			t[ki + 1 + ki * t_dim1], abs(d__2))) {
		    work[ki + *n] = wi / t[ki + (ki + 1) * t_dim1];
		    work[ki + 1 + n2] = 1.;
		} else {
		    work[ki + *n] = 1.;
		    work[ki + 1 + n2] = -wi / t[ki + 1 + ki * t_dim1];
		}
		work[ki + 1 + *n] = 0.;
		work[ki + n2] = 0.;

/*              Form right-hand side */

		i__2 = *n;
		for (k = ki + 2; k <= i__2; ++k) {
		    work[k + *n] = -work[ki + *n] * t[ki + k * t_dim1];
		    work[k + n2] = -work[ki + 1 + n2] * t[ki + 1 + k * t_dim1]
			    ;
/* L190: */
		}

/*              Solve complex quasi-triangular system: */
/*              ( T(KI+2,N:KI+2,N) - (WR-i*WI) )*X = WORK1+i*WORK2 */

		vmax = 1.;
		vcrit = bignum;

		jnxt = ki + 2;
		i__2 = *n;
		for (j = ki + 2; j <= i__2; ++j) {
		    if (j < jnxt) {
			goto L200;
		    }
		    j1 = j;
		    j2 = j;
		    jnxt = j + 1;
		    if (j < *n) {
			if (t[j + 1 + j * t_dim1] != 0.) {
			    j2 = j + 1;
			    jnxt = j + 2;
			}
		    }

		    if (j1 == j2) {

/*                    1-by-1 diagonal block */

/*                    Scale if necessary to avoid overflow when */
/*                    forming the right-hand side elements. */

			if (work[j] > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + n2], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 2;
			work[j + *n] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1],
				&c__1, &work[ki + 2 + *n], &c__1);
			i__3 = j - ki - 2;
			work[j + n2] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1],
				&c__1, &work[ki + 2 + n2], &c__1);

/*                    Solve (T(J,J)-(WR-i*WI))*(X11+i*X12)= WK+I*WK2 */

			d__1 = -wi;
			dlaln2_(&c_false, &c__1, &c__2, &smin, &c_b22, &t[j +
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &d__1, x, &c__2, &scale, &xnorm, &
				ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + n2], &c__1);
			}
			work[j + *n] = x[0];
			work[j + n2] = x[2];
/* Computing MAX */
			d__3 = (d__1 = work[j + *n], abs(d__1)), d__4 = (d__2
				= work[j + n2], abs(d__2)), d__3 = std::max(d__3,
				d__4);
			vmax = std::max(d__3,vmax);
			vcrit = bignum / vmax;

		    } else {

/*                    2-by-2 diagonal block */

/*                    Scale if necessary to avoid overflow when forming */
/*                    the right-hand side elements. */

/* Computing MAX */
			d__1 = work[j], d__2 = work[j + 1];
			beta = std::max(d__1,d__2);
			if (beta > vcrit) {
			    rec = 1. / vmax;
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &rec, &work[ki + n2], &c__1);
			    vmax = 1.;
			    vcrit = bignum;
			}

			i__3 = j - ki - 2;
			work[j + *n] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1],
				&c__1, &work[ki + 2 + *n], &c__1);

			i__3 = j - ki - 2;
			work[j + n2] -= ddot_(&i__3, &t[ki + 2 + j * t_dim1],
				&c__1, &work[ki + 2 + n2], &c__1);

			i__3 = j - ki - 2;
			work[j + 1 + *n] -= ddot_(&i__3, &t[ki + 2 + (j + 1) *
				 t_dim1], &c__1, &work[ki + 2 + *n], &c__1);

			i__3 = j - ki - 2;
			work[j + 1 + n2] -= ddot_(&i__3, &t[ki + 2 + (j + 1) *
				 t_dim1], &c__1, &work[ki + 2 + n2], &c__1);

/*                    Solve 2-by-2 complex linear equation */
/*                      ([T(j,j)   T(j,j+1)  ]'-(wr-i*wi)*I)*X = SCALE*B */
/*                      ([T(j+1,j) T(j+1,j+1)]             ) */

			d__1 = -wi;
			dlaln2_(&c_true, &c__2, &c__2, &smin, &c_b22, &t[j +
				j * t_dim1], ldt, &c_b22, &c_b22, &work[j + *
				n], n, &wr, &d__1, x, &c__2, &scale, &xnorm, &
				ierr);

/*                    Scale if necessary */

			if (scale != 1.) {
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + *n], &c__1);
			    i__3 = *n - ki + 1;
			    dscal_(&i__3, &scale, &work[ki + n2], &c__1);
			}
			work[j + *n] = x[0];
			work[j + n2] = x[2];
			work[j + 1 + *n] = x[1];
			work[j + 1 + n2] = x[3];
/* Computing MAX */
			d__1 = abs(x[0]), d__2 = abs(x[2]), d__1 = std::max(d__1,
				d__2), d__2 = abs(x[1]), d__1 = std::max(d__1,d__2)
				, d__2 = abs(x[3]), d__1 = std::max(d__1,d__2);
			vmax = std::max(d__1,vmax);
			vcrit = bignum / vmax;

		    }
L200:
		    ;
		}

/*              Copy the vector x or Q*x to VL and normalize. */

		if (! over) {
		    i__2 = *n - ki + 1;
		    dcopy_(&i__2, &work[ki + *n], &c__1, &vl[ki + is *
			    vl_dim1], &c__1);
		    i__2 = *n - ki + 1;
		    dcopy_(&i__2, &work[ki + n2], &c__1, &vl[ki + (is + 1) *
			    vl_dim1], &c__1);

		    emax = 0.;
		    i__2 = *n;
		    for (k = ki; k <= i__2; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vl[k + is * vl_dim1], abs(
				d__1)) + (d__2 = vl[k + (is + 1) * vl_dim1],
				abs(d__2));
			emax = std::max(d__3,d__4);
/* L220: */
		    }
		    remax = 1. / emax;
		    i__2 = *n - ki + 1;
		    dscal_(&i__2, &remax, &vl[ki + is * vl_dim1], &c__1);
		    i__2 = *n - ki + 1;
		    dscal_(&i__2, &remax, &vl[ki + (is + 1) * vl_dim1], &c__1)
			    ;

		    i__2 = ki - 1;
		    for (k = 1; k <= i__2; ++k) {
			vl[k + is * vl_dim1] = 0.;
			vl[k + (is + 1) * vl_dim1] = 0.;
/* L230: */
		    }
		} else {
		    if (ki < *n - 1) {
			i__2 = *n - ki - 1;
			dgemv_("N", n, &i__2, &c_b22, &vl[(ki + 2) * vl_dim1
				+ 1], ldvl, &work[ki + 2 + *n], &c__1, &work[
				ki + *n], &vl[ki * vl_dim1 + 1], &c__1);
			i__2 = *n - ki - 1;
			dgemv_("N", n, &i__2, &c_b22, &vl[(ki + 2) * vl_dim1
				+ 1], ldvl, &work[ki + 2 + n2], &c__1, &work[
				ki + 1 + n2], &vl[(ki + 1) * vl_dim1 + 1], &
				c__1);
		    } else {
			dscal_(n, &work[ki + *n], &vl[ki * vl_dim1 + 1], &
				c__1);
			dscal_(n, &work[ki + 1 + n2], &vl[(ki + 1) * vl_dim1
				+ 1], &c__1);
		    }

		    emax = 0.;
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
/* Computing MAX */
			d__3 = emax, d__4 = (d__1 = vl[k + ki * vl_dim1], abs(
				d__1)) + (d__2 = vl[k + (ki + 1) * vl_dim1],
				abs(d__2));
			emax = std::max(d__3,d__4);
/* L240: */
		    }
		    remax = 1. / emax;
		    dscal_(n, &remax, &vl[ki * vl_dim1 + 1], &c__1);
		    dscal_(n, &remax, &vl[(ki + 1) * vl_dim1 + 1], &c__1);

		}

	    }

	    ++is;
	    if (ip != 0) {
		++is;
	    }
L250:
	    if (ip == -1) {
		ip = 0;
	    }
	    if (ip == 1) {
		ip = -1;
	    }

/* L260: */
	}

    }

    return 0;

/*     End of DTREVC */

} /* dtrevc_ */

/* Subroutine */ int dtrexc_(const char *compq, integer *n, double *t, integer *
	ldt, double *q, integer *ldq, integer *ifst, integer *ilst,
	double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__2 = 2;

    /* System generated locals */
    integer q_dim1, q_offset, t_dim1, t_offset, i__1;

    /* Local variables */
    integer nbf, nbl, here;
    bool wantq;
    integer nbnext;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTREXC reorders the real Schur factorization of a real matrix */
/*  A = Q*T*Q**T, so that the diagonal block of T with row index IFST is */
/*  moved to row ILST. */

/*  The real Schur form T is reordered by an orthogonal similarity */
/*  transformation Z**T*T*Z, and optionally the matrix Q of Schur vectors */
/*  is updated by postmultiplying it with Z. */

/*  T must be in Schur canonical form (as returned by DHSEQR), that is, */
/*  block upper triangular with 1-by-1 and 2-by-2 diagonal blocks; each */
/*  2-by-2 diagonal block has its diagonal elements equal and its */
/*  off-diagonal elements of opposite sign. */

/*  Arguments */
/*  ========= */

/*  COMPQ   (input) CHARACTER*1 */
/*          = 'V':  update the matrix Q of Schur vectors; */
/*          = 'N':  do not update Q. */

/*  N       (input) INTEGER */
/*          The order of the matrix T. N >= 0. */

/*  T       (input/output) DOUBLE PRECISION array, dimension (LDT,N) */
/*          On entry, the upper quasi-triangular matrix T, in Schur */
/*          Schur canonical form. */
/*          On exit, the reordered upper quasi-triangular matrix, again */
/*          in Schur canonical form. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T. LDT >= max(1,N). */

/*  Q       (input/output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          On entry, if COMPQ = 'V', the matrix Q of Schur vectors. */
/*          On exit, if COMPQ = 'V', Q has been postmultiplied by the */
/*          orthogonal transformation matrix Z which reorders T. */
/*          If COMPQ = 'N', Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= max(1,N). */

/*  IFST    (input/output) INTEGER */
/*  ILST    (input/output) INTEGER */
/*          Specify the reordering of the diagonal blocks of T. */
/*          The block with row index IFST is moved to row ILST, by a */
/*          sequence of transpositions between adjacent blocks. */
/*          On exit, if IFST pointed on entry to the second row of a */
/*          2-by-2 block, it is changed to point to the first row; ILST */
/*          always points to the first row of the block in its final */
/*          position (which may differ from its input value by +1 or -1). */
/*          1 <= IFST <= N; 1 <= ILST <= N. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          = 1:  two adjacent blocks were too close to swap (the problem */
/*                is very ill-conditioned); T may have been partially */
/*                reordered, and ILST points to the first row of the */
/*                current position of the block being moved. */

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

/*     Decode and test the input arguments. */

    /* Parameter adjustments */
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --work;

    /* Function Body */
    *info = 0;
    wantq = lsame_(compq, "V");
    if (! wantq && ! lsame_(compq, "N")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ldt < std::max(1_integer,*n)) {
	*info = -4;
    } else if (*ldq < 1 || wantq && *ldq < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ifst < 1 || *ifst > *n) {
	*info = -7;
    } else if (*ilst < 1 || *ilst > *n) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTREXC", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 1) {
	return 0;
    }

/*     Determine the first row of specified block */
/*     and find out it is 1 by 1 or 2 by 2. */

    if (*ifst > 1) {
	if (t[*ifst + (*ifst - 1) * t_dim1] != 0.) {
	    --(*ifst);
	}
    }
    nbf = 1;
    if (*ifst < *n) {
	if (t[*ifst + 1 + *ifst * t_dim1] != 0.) {
	    nbf = 2;
	}
    }

/*     Determine the first row of the final block */
/*     and find out it is 1 by 1 or 2 by 2. */

    if (*ilst > 1) {
	if (t[*ilst + (*ilst - 1) * t_dim1] != 0.) {
	    --(*ilst);
	}
    }
    nbl = 1;
    if (*ilst < *n) {
	if (t[*ilst + 1 + *ilst * t_dim1] != 0.) {
	    nbl = 2;
	}
    }

    if (*ifst == *ilst) {
	return 0;
    }

    if (*ifst < *ilst) {

/*        Update ILST */

	if (nbf == 2 && nbl == 1) {
	    --(*ilst);
	}
	if (nbf == 1 && nbl == 2) {
	    ++(*ilst);
	}

	here = *ifst;

L10:

/*        Swap block with next one below */

	if (nbf == 1 || nbf == 2) {

/*           Current block either 1 by 1 or 2 by 2 */

	    nbnext = 1;
	    if (here + nbf + 1 <= *n) {
		if (t[here + nbf + 1 + (here + nbf) * t_dim1] != 0.) {
		    nbnext = 2;
		}
	    }
	    dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &here, &
		    nbf, &nbnext, &work[1], info);
	    if (*info != 0) {
		*ilst = here;
		return 0;
	    }
	    here += nbnext;

/*           Test if 2 by 2 block breaks into two 1 by 1 blocks */

	    if (nbf == 2) {
		if (t[here + 1 + here * t_dim1] == 0.) {
		    nbf = 3;
		}
	    }

	} else {

/*           Current block consists of two 1 by 1 blocks each of which */
/*           must be swapped individually */

	    nbnext = 1;
	    if (here + 3 <= *n) {
		if (t[here + 3 + (here + 2) * t_dim1] != 0.) {
		    nbnext = 2;
		}
	    }
	    i__1 = here + 1;
	    dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &i__1, &
		    c__1, &nbnext, &work[1], info);
	    if (*info != 0) {
		*ilst = here;
		return 0;
	    }
	    if (nbnext == 1) {

/*              Swap two 1 by 1 blocks, no problems possible */

		dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &
			here, &c__1, &nbnext, &work[1], info);
		++here;
	    } else {

/*              Recompute NBNEXT in case 2 by 2 split */

		if (t[here + 2 + (here + 1) * t_dim1] == 0.) {
		    nbnext = 1;
		}
		if (nbnext == 2) {

/*                 2 by 2 Block did not split */

		    dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &
			    here, &c__1, &nbnext, &work[1], info);
		    if (*info != 0) {
			*ilst = here;
			return 0;
		    }
		    here += 2;
		} else {

/*                 2 by 2 Block did split */

		    dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &
			    here, &c__1, &c__1, &work[1], info);
		    i__1 = here + 1;
		    dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &
			    i__1, &c__1, &c__1, &work[1], info);
		    here += 2;
		}
	    }
	}
	if (here < *ilst) {
	    goto L10;
	}

    } else {

	here = *ifst;
L20:

/*        Swap block with next one above */

	if (nbf == 1 || nbf == 2) {

/*           Current block either 1 by 1 or 2 by 2 */

	    nbnext = 1;
	    if (here >= 3) {
		if (t[here - 1 + (here - 2) * t_dim1] != 0.) {
		    nbnext = 2;
		}
	    }
	    i__1 = here - nbnext;
	    dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &i__1, &
		    nbnext, &nbf, &work[1], info);
	    if (*info != 0) {
		*ilst = here;
		return 0;
	    }
	    here -= nbnext;

/*           Test if 2 by 2 block breaks into two 1 by 1 blocks */

	    if (nbf == 2) {
		if (t[here + 1 + here * t_dim1] == 0.) {
		    nbf = 3;
		}
	    }

	} else {

/*           Current block consists of two 1 by 1 blocks each of which */
/*           must be swapped individually */

	    nbnext = 1;
	    if (here >= 3) {
		if (t[here - 1 + (here - 2) * t_dim1] != 0.) {
		    nbnext = 2;
		}
	    }
	    i__1 = here - nbnext;
	    dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &i__1, &
		    nbnext, &c__1, &work[1], info);
	    if (*info != 0) {
		*ilst = here;
		return 0;
	    }
	    if (nbnext == 1) {

/*              Swap two 1 by 1 blocks, no problems possible */

		dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &
			here, &nbnext, &c__1, &work[1], info);
		--here;
	    } else {

/*              Recompute NBNEXT in case 2 by 2 split */

		if (t[here + (here - 1) * t_dim1] == 0.) {
		    nbnext = 1;
		}
		if (nbnext == 2) {

/*                 2 by 2 Block did not split */

		    i__1 = here - 1;
		    dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &
			    i__1, &c__2, &c__1, &work[1], info);
		    if (*info != 0) {
			*ilst = here;
			return 0;
		    }
		    here += -2;
		} else {

/*                 2 by 2 Block did split */

		    dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &
			    here, &c__1, &c__1, &work[1], info);
		    i__1 = here - 1;
		    dlaexc_(&wantq, n, &t[t_offset], ldt, &q[q_offset], ldq, &
			    i__1, &c__1, &c__1, &work[1], info);
		    here += -2;
		}
	    }
	}
	if (here > *ilst) {
	    goto L20;
	}
    }
    *ilst = here;

    return 0;

/*     End of DTREXC */

} /* dtrexc_ */

/* Subroutine */ int dtrrfs_(const char *uplo, const char *trans, const char *diag, integer *n,
	integer *nrhs, double *a, integer *lda, double *b, integer *
	ldb, double *x, integer *ldx, double *ferr, double *berr,
	double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b19 = -1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset, i__1, i__2,
	    i__3;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k;
    double s, xk;
    integer nz;
    double eps;
    integer kase;
    double safe1, safe2;
    integer isave[3];
    bool upper;
    double safmin;
    bool notran;
    char transt[1];
    bool nounit;
    double lstres;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     Modified to call DLACN2 in place of DLACON, 5 Feb 03, SJH. */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRRFS provides error bounds and backward error estimates for the */
/*  solution to a system of linear equations with a triangular */
/*  coefficient matrix. */

/*  The solution matrix X must be computed by DTRTRS or some other */
/*  means before entering this routine.  DTRRFS does not do iterative */
/*  refinement because doing so cannot improve the backward error. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B  (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The triangular matrix A.  If UPLO = 'U', the leading N-by-N */
/*          upper triangular part of the array A contains the upper */
/*          triangular matrix, and the strictly lower triangular part of */
/*          A is not referenced.  If UPLO = 'L', the leading N-by-N lower */
/*          triangular part of the array A contains the lower triangular */
/*          matrix, and the strictly upper triangular part of A is not */
/*          referenced.  If DIAG = 'U', the diagonal elements of A are */
/*          also not referenced and are assumed to be 1. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The right hand side matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          The solution matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  FERR    (output) DOUBLE PRECISION array, dimension (NRHS) */
/*          The estimated forward error bound for each solution vector */
/*          X(j) (the j-th column of the solution matrix X). */
/*          If XTRUE is the true solution corresponding to X(j), FERR(j) */
/*          is an estimated upper bound for the magnitude of the largest */
/*          element in (X(j) - XTRUE) divided by the magnitude of the */
/*          largest element in X(j).  The estimate is as reliable as */
/*          the estimate for RCOND, and is almost always a slight */
/*          overestimate of the true error. */

/*  BERR    (output) DOUBLE PRECISION array, dimension (NRHS) */
/*          The componentwise relative backward error of each solution */
/*          vector X(j) (i.e., the smallest relative change in */
/*          any element of A or B that makes X(j) an exact solution). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

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
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    --ferr;
    --berr;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    notran = lsame_(trans, "N");
    nounit = lsame_(diag, "N");

    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T") && !
	    lsame_(trans, "C")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*nrhs < 0) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -9;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -11;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTRRFS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    ferr[j] = 0.;
	    berr[j] = 0.;
/* L10: */
	}
	return 0;
    }

    if (notran) {
	*(unsigned char *)transt = 'T';
    } else {
	*(unsigned char *)transt = 'N';
    }

/*     NZ = maximum number of nonzero elements in each row of A, plus 1 */

    nz = *n + 1;
    eps = dlamch_("Epsilon");
    safmin = dlamch_("Safe minimum");
    safe1 = nz * safmin;
    safe2 = safe1 / eps;

/*     Do for each right hand side */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {

/*        Compute residual R = B - op(A) * X, */
/*        where op(A) = A or A', depending on TRANS. */

	dcopy_(n, &x[j * x_dim1 + 1], &c__1, &work[*n + 1], &c__1);
	dtrmv_(uplo, trans, diag, n, &a[a_offset], lda, &work[*n + 1], &c__1);
	daxpy_(n, &c_b19, &b[j * b_dim1 + 1], &c__1, &work[*n + 1], &c__1);

/*        Compute componentwise relative backward error from formula */

/*        max(i) ( abs(R(i)) / ( abs(op(A))*abs(X) + abs(B) )(i) ) */

/*        where abs(Z) is the componentwise absolute value of the matrix */
/*        or vector Z.  If the i-th component of the denominator is less */
/*        than SAFE2, then SAFE1 is added to the i-th components of the */
/*        numerator and denominator before dividing. */

	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[i__] = (d__1 = b[i__ + j * b_dim1], abs(d__1));
/* L20: */
	}

	if (notran) {

/*           Compute abs(A)*abs(X) + abs(B). */

	    if (upper) {
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = k;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    work[i__] += (d__1 = a[i__ + k * a_dim1], abs(
				    d__1)) * xk;
/* L30: */
			}
/* L40: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = k - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    work[i__] += (d__1 = a[i__ + k * a_dim1], abs(
				    d__1)) * xk;
/* L50: */
			}
			work[k] += xk;
/* L60: */
		    }
		}
	    } else {
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = *n;
			for (i__ = k; i__ <= i__3; ++i__) {
			    work[i__] += (d__1 = a[i__ + k * a_dim1], abs(
				    d__1)) * xk;
/* L70: */
			}
/* L80: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			xk = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = *n;
			for (i__ = k + 1; i__ <= i__3; ++i__) {
			    work[i__] += (d__1 = a[i__ + k * a_dim1], abs(
				    d__1)) * xk;
/* L90: */
			}
			work[k] += xk;
/* L100: */
		    }
		}
	    }
	} else {

/*           Compute abs(A')*abs(X) + abs(B). */

	    if (upper) {
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = 0.;
			i__3 = k;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    s += (d__1 = a[i__ + k * a_dim1], abs(d__1)) * (
				    d__2 = x[i__ + j * x_dim1], abs(d__2));
/* L110: */
			}
			work[k] += s;
/* L120: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = k - 1;
			for (i__ = 1; i__ <= i__3; ++i__) {
			    s += (d__1 = a[i__ + k * a_dim1], abs(d__1)) * (
				    d__2 = x[i__ + j * x_dim1], abs(d__2));
/* L130: */
			}
			work[k] += s;
/* L140: */
		    }
		}
	    } else {
		if (nounit) {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = 0.;
			i__3 = *n;
			for (i__ = k; i__ <= i__3; ++i__) {
			    s += (d__1 = a[i__ + k * a_dim1], abs(d__1)) * (
				    d__2 = x[i__ + j * x_dim1], abs(d__2));
/* L150: */
			}
			work[k] += s;
/* L160: */
		    }
		} else {
		    i__2 = *n;
		    for (k = 1; k <= i__2; ++k) {
			s = (d__1 = x[k + j * x_dim1], abs(d__1));
			i__3 = *n;
			for (i__ = k + 1; i__ <= i__3; ++i__) {
			    s += (d__1 = a[i__ + k * a_dim1], abs(d__1)) * (
				    d__2 = x[i__ + j * x_dim1], abs(d__2));
/* L170: */
			}
			work[k] += s;
/* L180: */
		    }
		}
	    }
	}
	s = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (work[i__] > safe2) {
/* Computing MAX */
		d__2 = s, d__3 = (d__1 = work[*n + i__], abs(d__1)) / work[
			i__];
		s = std::max(d__2,d__3);
	    } else {
/* Computing MAX */
		d__2 = s, d__3 = ((d__1 = work[*n + i__], abs(d__1)) + safe1)
			/ (work[i__] + safe1);
		s = std::max(d__2,d__3);
	    }
/* L190: */
	}
	berr[j] = s;

/*        Bound error from formula */

/*        norm(X - XTRUE) / norm(X) .le. FERR = */
/*        norm( abs(inv(op(A)))* */
/*           ( abs(R) + NZ*EPS*( abs(op(A))*abs(X)+abs(B) ))) / norm(X) */

/*        where */
/*          norm(Z) is the magnitude of the largest component of Z */
/*          inv(op(A)) is the inverse of op(A) */
/*          abs(Z) is the componentwise absolute value of the matrix or */
/*             vector Z */
/*          NZ is the maximum number of nonzeros in any row of A, plus 1 */
/*          EPS is machine epsilon */

/*        The i-th component of abs(R)+NZ*EPS*(abs(op(A))*abs(X)+abs(B)) */
/*        is incremented by SAFE1 if the i-th component of */
/*        abs(op(A))*abs(X) + abs(B) is less than SAFE2. */

/*        Use DLACN2 to estimate the infinity-norm of the matrix */
/*           inv(op(A)) * diag(W), */
/*        where W = abs(R) + NZ*EPS*( abs(op(A))*abs(X)+abs(B) ))) */

	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (work[i__] > safe2) {
		work[i__] = (d__1 = work[*n + i__], abs(d__1)) + nz * eps *
			work[i__];
	    } else {
		work[i__] = (d__1 = work[*n + i__], abs(d__1)) + nz * eps *
			work[i__] + safe1;
	    }
/* L200: */
	}

	kase = 0;
L210:
	dlacn2_(n, &work[(*n << 1) + 1], &work[*n + 1], &iwork[1], &ferr[j], &
		kase, isave);
	if (kase != 0) {
	    if (kase == 1) {

/*              Multiply by diag(W)*inv(op(A)'). */

		dtrsv_(uplo, transt, diag, n, &a[a_offset], lda, &work[*n + 1]
, &c__1);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L220: */
		}
	    } else {

/*              Multiply by inv(op(A))*diag(W). */

		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L230: */
		}
		dtrsv_(uplo, trans, diag, n, &a[a_offset], lda, &work[*n + 1],
			 &c__1);
	    }
	    goto L210;
	}

/*        Normalize error. */

	lstres = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = lstres, d__3 = (d__1 = x[i__ + j * x_dim1], abs(d__1));
	    lstres = std::max(d__2,d__3);
/* L240: */
	}
	if (lstres != 0.) {
	    ferr[j] /= lstres;
	}

/* L250: */
    }

    return 0;

/*     End of DTRRFS */

} /* dtrrfs_ */

/* Subroutine */ int dtrsen_(const char *job, const char *compq, bool *select, integer
	*n, double *t, integer *ldt, double *q, integer *ldq,
	double *wr, double *wi, integer *m, double *s, double
	*sep, double *work, integer *lwork, integer *iwork, integer *
	liwork, integer *info)
{
	/* Table of constant values */
	static integer c_n1 = -1;

    /* System generated locals */
    integer q_dim1, q_offset, t_dim1, t_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer k, n1, n2, kk, nn, ks;
    double est;
    integer kase;
    bool pair;
    integer ierr;
    bool swap;
    double scale;
    integer isave[3], lwmin;
    bool wantq, wants;
    double rnorm;
    bool wantbh;
    integer liwmin;
    bool wantsp, lquery;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRSEN reorders the real Schur factorization of a real matrix */
/*  A = Q*T*Q**T, so that a selected cluster of eigenvalues appears in */
/*  the leading diagonal blocks of the upper quasi-triangular matrix T, */
/*  and the leading columns of Q form an orthonormal basis of the */
/*  corresponding right invariant subspace. */

/*  Optionally the routine computes the reciprocal condition numbers of */
/*  the cluster of eigenvalues and/or the invariant subspace. */

/*  T must be in Schur canonical form (as returned by DHSEQR), that is, */
/*  block upper triangular with 1-by-1 and 2-by-2 diagonal blocks; each */
/*  2-by-2 diagonal block has its diagonal elemnts equal and its */
/*  off-diagonal elements of opposite sign. */

/*  Arguments */
/*  ========= */

/*  JOB     (input) CHARACTER*1 */
/*          Specifies whether condition numbers are required for the */
/*          cluster of eigenvalues (S) or the invariant subspace (SEP): */
/*          = 'N': none; */
/*          = 'E': for eigenvalues only (S); */
/*          = 'V': for invariant subspace only (SEP); */
/*          = 'B': for both eigenvalues and invariant subspace (S and */
/*                 SEP). */

/*  COMPQ   (input) CHARACTER*1 */
/*          = 'V': update the matrix Q of Schur vectors; */
/*          = 'N': do not update Q. */

/*  SELECT  (input) LOGICAL array, dimension (N) */
/*          SELECT specifies the eigenvalues in the selected cluster. To */
/*          select a real eigenvalue w(j), SELECT(j) must be set to */
/*          .TRUE.. To select a complex conjugate pair of eigenvalues */
/*          w(j) and w(j+1), corresponding to a 2-by-2 diagonal block, */
/*          either SELECT(j) or SELECT(j+1) or both must be set to */
/*          .TRUE.; a complex conjugate pair of eigenvalues must be */
/*          either both included in the cluster or both excluded. */

/*  N       (input) INTEGER */
/*          The order of the matrix T. N >= 0. */

/*  T       (input/output) DOUBLE PRECISION array, dimension (LDT,N) */
/*          On entry, the upper quasi-triangular matrix T, in Schur */
/*          canonical form. */
/*          On exit, T is overwritten by the reordered matrix T, again in */
/*          Schur canonical form, with the selected eigenvalues in the */
/*          leading diagonal blocks. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T. LDT >= max(1,N). */

/*  Q       (input/output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          On entry, if COMPQ = 'V', the matrix Q of Schur vectors. */
/*          On exit, if COMPQ = 'V', Q has been postmultiplied by the */
/*          orthogonal transformation matrix which reorders T; the */
/*          leading M columns of Q form an orthonormal basis for the */
/*          specified invariant subspace. */
/*          If COMPQ = 'N', Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. */
/*          LDQ >= 1; and if COMPQ = 'V', LDQ >= N. */

/*  WR      (output) DOUBLE PRECISION array, dimension (N) */
/*  WI      (output) DOUBLE PRECISION array, dimension (N) */
/*          The real and imaginary parts, respectively, of the reordered */
/*          eigenvalues of T. The eigenvalues are stored in the same */
/*          order as on the diagonal of T, with WR(i) = T(i,i) and, if */
/*          T(i:i+1,i:i+1) is a 2-by-2 diagonal block, WI(i) > 0 and */
/*          WI(i+1) = -WI(i). Note that if a complex eigenvalue is */
/*          sufficiently ill-conditioned, then its value may differ */
/*          significantly from its value before reordering. */

/*  M       (output) INTEGER */
/*          The dimension of the specified invariant subspace. */
/*          0 < = M <= N. */

/*  S       (output) DOUBLE PRECISION */
/*          If JOB = 'E' or 'B', S is a lower bound on the reciprocal */
/*          condition number for the selected cluster of eigenvalues. */
/*          S cannot underestimate the true reciprocal condition number */
/*          by more than a factor of sqrt(N). If M = 0 or N, S = 1. */
/*          If JOB = 'N' or 'V', S is not referenced. */

/*  SEP     (output) DOUBLE PRECISION */
/*          If JOB = 'V' or 'B', SEP is the estimated reciprocal */
/*          condition number of the specified invariant subspace. If */
/*          M = 0 or N, SEP = norm(T). */
/*          If JOB = 'N' or 'E', SEP is not referenced. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If JOB = 'N', LWORK >= max(1,N); */
/*          if JOB = 'E', LWORK >= max(1,M*(N-M)); */
/*          if JOB = 'V' or 'B', LWORK >= max(1,2*M*(N-M)). */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. */
/*          If JOB = 'N' or 'E', LIWORK >= 1; */
/*          if JOB = 'V' or 'B', LIWORK >= max(1,M*(N-M)). */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal size of the IWORK array, */
/*          returns this value as the first entry of the IWORK array, and */
/*          no error message related to LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          = 1: reordering of T failed because some eigenvalues are too */
/*               close to separate (the problem is very ill-conditioned); */
/*               T may have been partially reordered, and WR and WI */
/*               contain the eigenvalues in the same order as in T; S and */
/*               SEP (if requested) are set to zero. */

/*  Further Details */
/*  =============== */

/*  DTRSEN first collects the selected eigenvalues by computing an */
/*  orthogonal transformation Z to move them to the top left corner of T. */
/*  In other words, the selected eigenvalues are the eigenvalues of T11 */
/*  in: */

/*                Z'*T*Z = ( T11 T12 ) n1 */
/*                         (  0  T22 ) n2 */
/*                            n1  n2 */

/*  where N = n1+n2 and Z' means the transpose of Z. The first n1 columns */
/*  of Z span the specified invariant subspace of T. */

/*  If T has been obtained from the real Schur factorization of a matrix */
/*  A = Q*T*Q', then the reordered real Schur factorization of A is given */
/*  by A = (Q*Z)*(Z'*T*Z)*(Q*Z)', and the first n1 columns of Q*Z span */
/*  the corresponding invariant subspace of A. */

/*  The reciprocal condition number of the average of the eigenvalues of */
/*  T11 may be returned in S. S lies between 0 (very badly conditioned) */
/*  and 1 (very well conditioned). It is computed as follows. First we */
/*  compute R so that */

/*                         P = ( I  R ) n1 */
/*                             ( 0  0 ) n2 */
/*                               n1 n2 */

/*  is the projector on the invariant subspace associated with T11. */
/*  R is the solution of the Sylvester equation: */

/*                        T11*R - R*T22 = T12. */

/*  Let F-norm(M) denote the Frobenius-norm of M and 2-norm(M) denote */
/*  the two-norm of M. Then S is computed as the lower bound */

/*                      (1 + F-norm(R)**2)**(-1/2) */

/*  on the reciprocal of 2-norm(P), the true reciprocal condition number. */
/*  S cannot underestimate 1 / 2-norm(P) by more than a factor of */
/*  sqrt(N). */

/*  An approximate error bound for the computed average of the */
/*  eigenvalues of T11 is */

/*                         EPS * norm(T) / S */

/*  where EPS is the machine precision. */

/*  The reciprocal condition number of the right invariant subspace */
/*  spanned by the first n1 columns of Z (or of Q*Z) is returned in SEP. */
/*  SEP is defined as the separation of T11 and T22: */

/*                     sep( T11, T22 ) = sigma-min( C ) */

/*  where sigma-min(C) is the smallest singular value of the */
/*  n1*n2-by-n1*n2 matrix */

/*     C  = kprod( I(n2), T11 ) - kprod( transpose(T22), I(n1) ) */

/*  I(m) is an m by m identity matrix, and kprod denotes the Kronecker */
/*  product. We estimate sigma-min(C) by the reciprocal of an estimate of */
/*  the 1-norm of inverse(C). The true reciprocal 1-norm of inverse(C) */
/*  cannot differ from sigma-min(C) by more than a factor of sqrt(n1*n2). */

/*  When SEP is small, small changes in T can cause large changes in */
/*  the invariant subspace. An approximate bound on the maximum angular */
/*  error in the computed right invariant subspace is */

/*                      EPS * norm(T) / SEP */

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

/*     Decode and test the input parameters */

    /* Parameter adjustments */
    --select;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --wr;
    --wi;
    --work;
    --iwork;

    /* Function Body */
    wantbh = lsame_(job, "B");
    wants = lsame_(job, "E") || wantbh;
    wantsp = lsame_(job, "V") || wantbh;
    wantq = lsame_(compq, "V");

    *info = 0;
    lquery = *lwork == -1;
    if (! lsame_(job, "N") && ! wants && ! wantsp) {
	*info = -1;
    } else if (! lsame_(compq, "N") && ! wantq) {
	*info = -2;
    } else if (*n < 0) {
	*info = -4;
    } else if (*ldt < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldq < 1 || wantq && *ldq < *n) {
	*info = -8;
    } else {

/*        Set M to the dimension of the specified invariant subspace, */
/*        and test LWORK and LIWORK. */

	*m = 0;
	pair = false;
	i__1 = *n;
	for (k = 1; k <= i__1; ++k) {
	    if (pair) {
		pair = false;
	    } else {
		if (k < *n) {
		    if (t[k + 1 + k * t_dim1] == 0.) {
			if (select[k]) {
			    ++(*m);
			}
		    } else {
			pair = true;
			if (select[k] || select[k + 1]) {
			    *m += 2;
			}
		    }
		} else {
		    if (select[*n]) {
			++(*m);
		    }
		}
	    }
/* L10: */
	}

	n1 = *m;
	n2 = *n - *m;
	nn = n1 * n2;

	if (wantsp) {
/* Computing MAX */
	    i__1 = 1, i__2 = nn << 1;
	    lwmin = std::max(i__1,i__2);
	    liwmin = std::max(1_integer,nn);
	} else if (lsame_(job, "N")) {
	    lwmin = std::max(1_integer,*n);
	    liwmin = 1;
	} else if (lsame_(job, "E")) {
	    lwmin = std::max(1_integer,nn);
	    liwmin = 1;
	}

	if (*lwork < lwmin && ! lquery) {
	    *info = -15;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -17;
	}
    }

    if (*info == 0) {
	work[1] = (double) lwmin;
	iwork[1] = liwmin;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTRSEN", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible. */

    if (*m == *n || *m == 0) {
	if (wants) {
	    *s = 1.;
	}
	if (wantsp) {
	    *sep = dlange_("1", n, n, &t[t_offset], ldt, &work[1]);
	}
	goto L40;
    }

/*     Collect the selected blocks at the top-left corner of T. */

    ks = 0;
    pair = false;
    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {
	if (pair) {
	    pair = false;
	} else {
	    swap = select[k];
	    if (k < *n) {
		if (t[k + 1 + k * t_dim1] != 0.) {
		    pair = true;
		    swap = swap || select[k + 1];
		}
	    }
	    if (swap) {
		++ks;

/*              Swap the K-th block to position KS. */

		ierr = 0;
		kk = k;
		if (k != ks) {
		    dtrexc_(compq, n, &t[t_offset], ldt, &q[q_offset], ldq, &
			    kk, &ks, &work[1], &ierr);
		}
		if (ierr == 1 || ierr == 2) {

/*                 Blocks too close to swap: exit. */

		    *info = 1;
		    if (wants) {
			*s = 0.;
		    }
		    if (wantsp) {
			*sep = 0.;
		    }
		    goto L40;
		}
		if (pair) {
		    ++ks;
		}
	    }
	}
/* L20: */
    }

    if (wants) {

/*        Solve Sylvester equation for R: */

/*           T11*R - R*T22 = scale*T12 */

	dlacpy_("F", &n1, &n2, &t[(n1 + 1) * t_dim1 + 1], ldt, &work[1], &n1);
	dtrsyl_("N", "N", &c_n1, &n1, &n2, &t[t_offset], ldt, &t[n1 + 1 + (n1
		+ 1) * t_dim1], ldt, &work[1], &n1, &scale, &ierr);

/*        Estimate the reciprocal of the condition number of the cluster */
/*        of eigenvalues. */

	rnorm = dlange_("F", &n1, &n2, &work[1], &n1, &work[1]);
	if (rnorm == 0.) {
	    *s = 1.;
	} else {
	    *s = scale / (sqrt(scale * scale / rnorm + rnorm) * sqrt(rnorm));
	}
    }

    if (wantsp) {

/*        Estimate sep(T11,T22). */

	est = 0.;
	kase = 0;
L30:
	dlacn2_(&nn, &work[nn + 1], &work[1], &iwork[1], &est, &kase, isave);
	if (kase != 0) {
	    if (kase == 1) {

/*              Solve  T11*R - R*T22 = scale*X. */

		dtrsyl_("N", "N", &c_n1, &n1, &n2, &t[t_offset], ldt, &t[n1 +
			1 + (n1 + 1) * t_dim1], ldt, &work[1], &n1, &scale, &
			ierr);
	    } else {

/*              Solve  T11'*R - R*T22' = scale*X. */

		dtrsyl_("T", "T", &c_n1, &n1, &n2, &t[t_offset], ldt, &t[n1 +
			1 + (n1 + 1) * t_dim1], ldt, &work[1], &n1, &scale, &
			ierr);
	    }
	    goto L30;
	}

	*sep = scale / est;
    }

L40:

/*     Store the output eigenvalues in WR and WI. */

    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {
	wr[k] = t[k + k * t_dim1];
	wi[k] = 0.;
/* L50: */
    }
    i__1 = *n - 1;
    for (k = 1; k <= i__1; ++k) {
	if (t[k + 1 + k * t_dim1] != 0.) {
	    wi[k] = sqrt((d__1 = t[k + (k + 1) * t_dim1], abs(d__1))) * sqrt((
		    d__2 = t[k + 1 + k * t_dim1], abs(d__2)));
	    wi[k + 1] = -wi[k];
	}
/* L60: */
    }

    work[1] = (double) lwmin;
    iwork[1] = liwmin;

    return 0;

/*     End of DTRSEN */

} /* dtrsen_ */

/* Subroutine */ int dtrsna_(const char *job, const char *howmny, bool *select,
	integer *n, double *t, integer *ldt, double *vl, integer *
	ldvl, double *vr, integer *ldvr, double *s, double *sep,
	integer *mm, integer *m, double *work, integer *ldwork, integer *
	iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static bool c_true = true;
	static bool c_false = false;

    /* System generated locals */
    integer t_dim1, t_offset, vl_dim1, vl_offset, vr_dim1, vr_offset,
	    work_dim1, work_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, k, n2;
    double cs;
    integer nn, ks;
    double sn, mu, eps, est;
    integer kase;
    double cond;
    bool pair;
    integer ierr;
    double dumm, prod;
    integer ifst;
    double lnrm;
    integer ilst;
    double rnrm;
    double prod1, prod2, scale, delta;
    integer isave[3];
    bool wants;
    double dummy[1];
    double bignum;
    bool wantbh;
    bool somcon;
    double smlnum;
    bool wantsp;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     Modified to call DLACN2 in place of DLACON, 5 Feb 03, SJH. */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRSNA estimates reciprocal condition numbers for specified */
/*  eigenvalues and/or right eigenvectors of a real upper */
/*  quasi-triangular matrix T (or of any matrix Q*T*Q**T with Q */
/*  orthogonal). */

/*  T must be in Schur canonical form (as returned by DHSEQR), that is, */
/*  block upper triangular with 1-by-1 and 2-by-2 diagonal blocks; each */
/*  2-by-2 diagonal block has its diagonal elements equal and its */
/*  off-diagonal elements of opposite sign. */

/*  Arguments */
/*  ========= */

/*  JOB     (input) CHARACTER*1 */
/*          Specifies whether condition numbers are required for */
/*          eigenvalues (S) or eigenvectors (SEP): */
/*          = 'E': for eigenvalues only (S); */
/*          = 'V': for eigenvectors only (SEP); */
/*          = 'B': for both eigenvalues and eigenvectors (S and SEP). */

/*  HOWMNY  (input) CHARACTER*1 */
/*          = 'A': compute condition numbers for all eigenpairs; */
/*          = 'S': compute condition numbers for selected eigenpairs */
/*                 specified by the array SELECT. */

/*  SELECT  (input) LOGICAL array, dimension (N) */
/*          If HOWMNY = 'S', SELECT specifies the eigenpairs for which */
/*          condition numbers are required. To select condition numbers */
/*          for the eigenpair corresponding to a real eigenvalue w(j), */
/*          SELECT(j) must be set to .TRUE.. To select condition numbers */
/*          corresponding to a complex conjugate pair of eigenvalues w(j) */
/*          and w(j+1), either SELECT(j) or SELECT(j+1) or both, must be */
/*          set to .TRUE.. */
/*          If HOWMNY = 'A', SELECT is not referenced. */

/*  N       (input) INTEGER */
/*          The order of the matrix T. N >= 0. */

/*  T       (input) DOUBLE PRECISION array, dimension (LDT,N) */
/*          The upper quasi-triangular matrix T, in Schur canonical form. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T. LDT >= max(1,N). */

/*  VL      (input) DOUBLE PRECISION array, dimension (LDVL,M) */
/*          If JOB = 'E' or 'B', VL must contain left eigenvectors of T */
/*          (or of any Q*T*Q**T with Q orthogonal), corresponding to the */
/*          eigenpairs specified by HOWMNY and SELECT. The eigenvectors */
/*          must be stored in consecutive columns of VL, as returned by */
/*          DHSEIN or DTREVC. */
/*          If JOB = 'V', VL is not referenced. */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of the array VL. */
/*          LDVL >= 1; and if JOB = 'E' or 'B', LDVL >= N. */

/*  VR      (input) DOUBLE PRECISION array, dimension (LDVR,M) */
/*          If JOB = 'E' or 'B', VR must contain right eigenvectors of T */
/*          (or of any Q*T*Q**T with Q orthogonal), corresponding to the */
/*          eigenpairs specified by HOWMNY and SELECT. The eigenvectors */
/*          must be stored in consecutive columns of VR, as returned by */
/*          DHSEIN or DTREVC. */
/*          If JOB = 'V', VR is not referenced. */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the array VR. */
/*          LDVR >= 1; and if JOB = 'E' or 'B', LDVR >= N. */

/*  S       (output) DOUBLE PRECISION array, dimension (MM) */
/*          If JOB = 'E' or 'B', the reciprocal condition numbers of the */
/*          selected eigenvalues, stored in consecutive elements of the */
/*          array. For a complex conjugate pair of eigenvalues two */
/*          consecutive elements of S are set to the same value. Thus */
/*          S(j), SEP(j), and the j-th columns of VL and VR all */
/*          correspond to the same eigenpair (but not in general the */
/*          j-th eigenpair, unless all eigenpairs are selected). */
/*          If JOB = 'V', S is not referenced. */

/*  SEP     (output) DOUBLE PRECISION array, dimension (MM) */
/*          If JOB = 'V' or 'B', the estimated reciprocal condition */
/*          numbers of the selected eigenvectors, stored in consecutive */
/*          elements of the array. For a complex eigenvector two */
/*          consecutive elements of SEP are set to the same value. If */
/*          the eigenvalues cannot be reordered to compute SEP(j), SEP(j) */
/*          is set to 0; this can only occur when the true value would be */
/*          very small anyway. */
/*          If JOB = 'E', SEP is not referenced. */

/*  MM      (input) INTEGER */
/*          The number of elements in the arrays S (if JOB = 'E' or 'B') */
/*           and/or SEP (if JOB = 'V' or 'B'). MM >= M. */

/*  M       (output) INTEGER */
/*          The number of elements of the arrays S and/or SEP actually */
/*          used to store the estimated condition numbers. */
/*          If HOWMNY = 'A', M is set to N. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LDWORK,N+6) */
/*          If JOB = 'E', WORK is not referenced. */

/*  LDWORK  (input) INTEGER */
/*          The leading dimension of the array WORK. */
/*          LDWORK >= 1; and if JOB = 'V' or 'B', LDWORK >= N. */

/*  IWORK   (workspace) INTEGER array, dimension (2*(N-1)) */
/*          If JOB = 'E', IWORK is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The reciprocal of the condition number of an eigenvalue lambda is */
/*  defined as */

/*          S(lambda) = |v'*u| / (norm(u)*norm(v)) */

/*  where u and v are the right and left eigenvectors of T corresponding */
/*  to lambda; v' denotes the conjugate-transpose of v, and norm(u) */
/*  denotes the Euclidean norm. These reciprocal condition numbers always */
/*  lie between zero (very badly conditioned) and one (very well */
/*  conditioned). If n = 1, S(lambda) is defined to be 1. */

/*  An approximate error bound for a computed eigenvalue W(i) is given by */

/*                      EPS * norm(T) / S(i) */

/*  where EPS is the machine precision. */

/*  The reciprocal of the condition number of the right eigenvector u */
/*  corresponding to lambda is defined as follows. Suppose */

/*              T = ( lambda  c  ) */
/*                  (   0    T22 ) */

/*  Then the reciprocal condition number is */

/*          SEP( lambda, T22 ) = sigma-min( T22 - lambda*I ) */

/*  where sigma-min denotes the smallest singular value. We approximate */
/*  the smallest singular value by the reciprocal of an estimate of the */
/*  one-norm of the inverse of T22 - lambda*I. If n = 1, SEP(1) is */
/*  defined to be abs(T(1,1)). */

/*  An approximate error bound for a computed right eigenvector VR(i) */
/*  is given by */

/*                      EPS * norm(T) / SEP(i) */

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

/*     Decode and test the input parameters */

    /* Parameter adjustments */
    --select;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    --s;
    --sep;
    work_dim1 = *ldwork;
    work_offset = 1 + work_dim1;
    work -= work_offset;
    --iwork;

    /* Function Body */
    wantbh = lsame_(job, "B");
    wants = lsame_(job, "E") || wantbh;
    wantsp = lsame_(job, "V") || wantbh;

    somcon = lsame_(howmny, "S");

    *info = 0;
    if (! wants && ! wantsp) {
	*info = -1;
    } else if (! lsame_(howmny, "A") && ! somcon) {
	*info = -2;
    } else if (*n < 0) {
	*info = -4;
    } else if (*ldt < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldvl < 1 || wants && *ldvl < *n) {
	*info = -8;
    } else if (*ldvr < 1 || wants && *ldvr < *n) {
	*info = -10;
    } else {

/*        Set M to the number of eigenpairs for which condition numbers */
/*        are required, and test MM. */

	if (somcon) {
	    *m = 0;
	    pair = false;
	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {
		if (pair) {
		    pair = false;
		} else {
		    if (k < *n) {
			if (t[k + 1 + k * t_dim1] == 0.) {
			    if (select[k]) {
				++(*m);
			    }
			} else {
			    pair = true;
			    if (select[k] || select[k + 1]) {
				*m += 2;
			    }
			}
		    } else {
			if (select[*n]) {
			    ++(*m);
			}
		    }
		}
/* L10: */
	    }
	} else {
	    *m = *n;
	}

	if (*mm < *m) {
	    *info = -13;
	} else if (*ldwork < 1 || wantsp && *ldwork < *n) {
	    *info = -16;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTRSNA", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (somcon) {
	    if (! select[1]) {
		return 0;
	    }
	}
	if (wants) {
	    s[1] = 1.;
	}
	if (wantsp) {
	    sep[1] = (d__1 = t[t_dim1 + 1], abs(d__1));
	}
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

    ks = 0;
    pair = false;
    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {

/*        Determine whether T(k,k) begins a 1-by-1 or 2-by-2 block. */

	if (pair) {
	    pair = false;
	    goto L60;
	} else {
	    if (k < *n) {
		pair = t[k + 1 + k * t_dim1] != 0.;
	    }
	}

/*        Determine whether condition numbers are required for the k-th */
/*        eigenpair. */

	if (somcon) {
	    if (pair) {
		if (! select[k] && ! select[k + 1]) {
		    goto L60;
		}
	    } else {
		if (! select[k]) {
		    goto L60;
		}
	    }
	}

	++ks;

	if (wants) {

/*           Compute the reciprocal condition number of the k-th */
/*           eigenvalue. */

	    if (! pair) {

/*              Real eigenvalue. */

		prod = ddot_(n, &vr[ks * vr_dim1 + 1], &c__1, &vl[ks *
			vl_dim1 + 1], &c__1);
		rnrm = dnrm2_(n, &vr[ks * vr_dim1 + 1], &c__1);
		lnrm = dnrm2_(n, &vl[ks * vl_dim1 + 1], &c__1);
		s[ks] = abs(prod) / (rnrm * lnrm);
	    } else {

/*              Complex eigenvalue. */

		prod1 = ddot_(n, &vr[ks * vr_dim1 + 1], &c__1, &vl[ks *
			vl_dim1 + 1], &c__1);
		prod1 += ddot_(n, &vr[(ks + 1) * vr_dim1 + 1], &c__1, &vl[(ks
			+ 1) * vl_dim1 + 1], &c__1);
		prod2 = ddot_(n, &vl[ks * vl_dim1 + 1], &c__1, &vr[(ks + 1) *
			vr_dim1 + 1], &c__1);
		prod2 -= ddot_(n, &vl[(ks + 1) * vl_dim1 + 1], &c__1, &vr[ks *
			 vr_dim1 + 1], &c__1);
		d__1 = dnrm2_(n, &vr[ks * vr_dim1 + 1], &c__1);
		d__2 = dnrm2_(n, &vr[(ks + 1) * vr_dim1 + 1], &c__1);
		rnrm = dlapy2_(&d__1, &d__2);
		d__1 = dnrm2_(n, &vl[ks * vl_dim1 + 1], &c__1);
		d__2 = dnrm2_(n, &vl[(ks + 1) * vl_dim1 + 1], &c__1);
		lnrm = dlapy2_(&d__1, &d__2);
		cond = dlapy2_(&prod1, &prod2) / (rnrm * lnrm);
		s[ks] = cond;
		s[ks + 1] = cond;
	    }
	}

	if (wantsp) {

/*           Estimate the reciprocal condition number of the k-th */
/*           eigenvector. */

/*           Copy the matrix T to the array WORK and swap the diagonal */
/*           block beginning at T(k,k) to the (1,1) position. */

	    dlacpy_("Full", n, n, &t[t_offset], ldt, &work[work_offset],
		    ldwork);
	    ifst = k;
	    ilst = 1;
	    dtrexc_("No Q", n, &work[work_offset], ldwork, dummy, &c__1, &
		    ifst, &ilst, &work[(*n + 1) * work_dim1 + 1], &ierr);

	    if (ierr == 1 || ierr == 2) {

/*              Could not swap because blocks not well separated */

		scale = 1.;
		est = bignum;
	    } else {

/*              Reordering successful */

		if (work[work_dim1 + 2] == 0.) {

/*                 Form C = T22 - lambda*I in WORK(2:N,2:N). */

		    i__2 = *n;
		    for (i__ = 2; i__ <= i__2; ++i__) {
			work[i__ + i__ * work_dim1] -= work[work_dim1 + 1];
/* L20: */
		    }
		    n2 = 1;
		    nn = *n - 1;
		} else {

/*                 Triangularize the 2 by 2 block by unitary */
/*                 transformation U = [  cs   i*ss ] */
/*                                    [ i*ss   cs  ]. */
/*                 such that the (1,1) position of WORK is complex */
/*                 eigenvalue lambda with positive imaginary part. (2,2) */
/*                 position of WORK is the complex eigenvalue lambda */
/*                 with negative imaginary  part. */

		    mu = sqrt((d__1 = work[(work_dim1 << 1) + 1], abs(d__1)))
			    * sqrt((d__2 = work[work_dim1 + 2], abs(d__2)));
		    delta = dlapy2_(&mu, &work[work_dim1 + 2]);
		    cs = mu / delta;
		    sn = -work[work_dim1 + 2] / delta;

/*                 Form */

/*                 C' = WORK(2:N,2:N) + i*[rwork(1) ..... rwork(n-1) ] */
/*                                        [   mu                     ] */
/*                                        [         ..               ] */
/*                                        [             ..           ] */
/*                                        [                  mu      ] */
/*                 where C' is conjugate transpose of complex matrix C, */
/*                 and RWORK is stored starting in the N+1-st column of */
/*                 WORK. */

		    i__2 = *n;
		    for (j = 3; j <= i__2; ++j) {
			work[j * work_dim1 + 2] = cs * work[j * work_dim1 + 2]
				;
			work[j + j * work_dim1] -= work[work_dim1 + 1];
/* L30: */
		    }
		    work[(work_dim1 << 1) + 2] = 0.;

		    work[(*n + 1) * work_dim1 + 1] = mu * 2.;
		    i__2 = *n - 1;
		    for (i__ = 2; i__ <= i__2; ++i__) {
			work[i__ + (*n + 1) * work_dim1] = sn * work[(i__ + 1)
				 * work_dim1 + 1];
/* L40: */
		    }
		    n2 = 2;
		    nn = *n - 1 << 1;
		}

/*              Estimate norm(inv(C')) */

		est = 0.;
		kase = 0;
L50:
		dlacn2_(&nn, &work[(*n + 2) * work_dim1 + 1], &work[(*n + 4) *
			 work_dim1 + 1], &iwork[1], &est, &kase, isave);
		if (kase != 0) {
		    if (kase == 1) {
			if (n2 == 1) {

/*                       Real eigenvalue: solve C'*x = scale*c. */

			    i__2 = *n - 1;
			    dlaqtr_(&c_true, &c_true, &i__2, &work[(work_dim1
				    << 1) + 2], ldwork, dummy, &dumm, &scale,
				    &work[(*n + 4) * work_dim1 + 1], &work[(*
				    n + 6) * work_dim1 + 1], &ierr);
			} else {

/*                       Complex eigenvalue: solve */
/*                       C'*(p+iq) = scale*(c+id) in real arithmetic. */

			    i__2 = *n - 1;
			    dlaqtr_(&c_true, &c_false, &i__2, &work[(
				    work_dim1 << 1) + 2], ldwork, &work[(*n +
				    1) * work_dim1 + 1], &mu, &scale, &work[(*
				    n + 4) * work_dim1 + 1], &work[(*n + 6) *
				    work_dim1 + 1], &ierr);
			}
		    } else {
			if (n2 == 1) {

/*                       Real eigenvalue: solve C*x = scale*c. */

			    i__2 = *n - 1;
			    dlaqtr_(&c_false, &c_true, &i__2, &work[(
				    work_dim1 << 1) + 2], ldwork, dummy, &
				    dumm, &scale, &work[(*n + 4) * work_dim1
				    + 1], &work[(*n + 6) * work_dim1 + 1], &
				    ierr);
			} else {

/*                       Complex eigenvalue: solve */
/*                       C*(p+iq) = scale*(c+id) in real arithmetic. */

			    i__2 = *n - 1;
			    dlaqtr_(&c_false, &c_false, &i__2, &work[(
				    work_dim1 << 1) + 2], ldwork, &work[(*n +
				    1) * work_dim1 + 1], &mu, &scale, &work[(*
				    n + 4) * work_dim1 + 1], &work[(*n + 6) *
				    work_dim1 + 1], &ierr);

			}
		    }

		    goto L50;
		}
	    }

	    sep[ks] = scale / std::max(est,smlnum);
	    if (pair) {
		sep[ks + 1] = sep[ks];
	    }
	}

	if (pair) {
	    ++ks;
	}

L60:
	;
    }
    return 0;

/*     End of DTRSNA */

} /* dtrsna_ */

/* Subroutine */ int dtrsyl_(const char *trana, const char *tranb, integer *isgn, integer
	*m, integer *n, double *a, integer *lda, double *b, integer *
	ldb, double *c__, integer *ldc, double *scale, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static bool c_false = false;
	static integer c__2 = 2;
	static double c_b26 = 1.;
	static double c_b30 = 0.;
	static bool c_true = true;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset, i__1, i__2,
	    i__3, i__4;
    double d__1, d__2;

    /* Local variables */
    integer j, k, l;
    double x[4]	/* was [2][2] */;
    integer k1, k2, l1, l2;
    double a11, db, da11, vec[4]	/* was [2][2] */, dum[1], eps, sgn;
    integer ierr;
    double smin, suml, sumr;
    integer knext, lnext;
    double xnorm;
    double scaloc;
    double bignum;
    bool notrna, notrnb;
    double smlnum;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRSYL solves the real Sylvester matrix equation: */

/*     op(A)*X + X*op(B) = scale*C or */
/*     op(A)*X - X*op(B) = scale*C, */

/*  where op(A) = A or A**T, and  A and B are both upper quasi- */
/*  triangular. A is M-by-M and B is N-by-N; the right hand side C and */
/*  the solution X are M-by-N; and scale is an output scale factor, set */
/*  <= 1 to avoid overflow in X. */

/*  A and B must be in Schur canonical form (as returned by DHSEQR), that */
/*  is, block upper triangular with 1-by-1 and 2-by-2 diagonal blocks; */
/*  each 2-by-2 diagonal block has its diagonal elements equal and its */
/*  off-diagonal elements of opposite sign. */

/*  Arguments */
/*  ========= */

/*  TRANA   (input) CHARACTER*1 */
/*          Specifies the option op(A): */
/*          = 'N': op(A) = A    (No transpose) */
/*          = 'T': op(A) = A**T (Transpose) */
/*          = 'C': op(A) = A**H (Conjugate transpose = Transpose) */

/*  TRANB   (input) CHARACTER*1 */
/*          Specifies the option op(B): */
/*          = 'N': op(B) = B    (No transpose) */
/*          = 'T': op(B) = B**T (Transpose) */
/*          = 'C': op(B) = B**H (Conjugate transpose = Transpose) */

/*  ISGN    (input) INTEGER */
/*          Specifies the sign in the equation: */
/*          = +1: solve op(A)*X + X*op(B) = scale*C */
/*          = -1: solve op(A)*X - X*op(B) = scale*C */

/*  M       (input) INTEGER */
/*          The order of the matrix A, and the number of rows in the */
/*          matrices X and C. M >= 0. */

/*  N       (input) INTEGER */
/*          The order of the matrix B, and the number of columns in the */
/*          matrices X and C. N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,M) */
/*          The upper quasi-triangular matrix A, in Schur canonical form. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,M). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,N) */
/*          The upper quasi-triangular matrix B, in Schur canonical form. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,N). */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC,N) */
/*          On entry, the M-by-N right hand side matrix C. */
/*          On exit, C is overwritten by the solution matrix X. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C. LDC >= max(1,M) */

/*  SCALE   (output) DOUBLE PRECISION */
/*          The scale factor, scale, set <= 1 to avoid overflow in X. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          = 1: A and B have common or very close eigenvalues; perturbed */
/*               values were used to solve the equation (but the matrices */
/*               A and B are unchanged). */

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

/*     Decode and Test input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;

    /* Function Body */
    notrna = lsame_(trana, "N");
    notrnb = lsame_(tranb, "N");

    *info = 0;
    if (! notrna && ! lsame_(trana, "T") && ! lsame_(
	    trana, "C")) {
	*info = -1;
    } else if (! notrnb && ! lsame_(tranb, "T") && !
	    lsame_(tranb, "C")) {
	*info = -2;
    } else if (*isgn != 1 && *isgn != -1) {
	*info = -3;
    } else if (*m < 0) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -7;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -9;
    } else if (*ldc < std::max(1_integer,*m)) {
	*info = -11;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTRSYL", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }

/*     Set constants to control overflow */

    eps = dlamch_("P");
    smlnum = dlamch_("S");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);
    smlnum = smlnum * (double) (*m * *n) / eps;
    bignum = 1. / smlnum;

/* Computing MAX */
    d__1 = smlnum, d__2 = eps * dlange_("M", m, m, &a[a_offset], lda, dum), d__1 = std::max(d__1,d__2), d__2 = eps * dlange_("M", n, n,
	    &b[b_offset], ldb, dum);
    smin = std::max(d__1,d__2);

    *scale = 1.;
    sgn = (double) (*isgn);

    if (notrna && notrnb) {

/*        Solve    A*X + ISGN*X*B = scale*C. */

/*        The (K,L)th block of X is determined starting from */
/*        bottom-left corner column by column by */

/*         A(K,K)*X(K,L) + ISGN*X(K,L)*B(L,L) = C(K,L) - R(K,L) */

/*        Where */
/*                  M                         L-1 */
/*        R(K,L) = SUM [A(K,I)*X(I,L)] + ISGN*SUM [X(K,J)*B(J,L)]. */
/*                I=K+1                       J=1 */

/*        Start column loop (index = L) */
/*        L1 (L2) : column index of the first (first) row of X(K,L). */

	lnext = 1;
	i__1 = *n;
	for (l = 1; l <= i__1; ++l) {
	    if (l < lnext) {
		goto L60;
	    }
	    if (l == *n) {
		l1 = l;
		l2 = l;
	    } else {
		if (b[l + 1 + l * b_dim1] != 0.) {
		    l1 = l;
		    l2 = l + 1;
		    lnext = l + 2;
		} else {
		    l1 = l;
		    l2 = l;
		    lnext = l + 1;
		}
	    }

/*           Start row loop (index = K) */
/*           K1 (K2): row index of the first (last) row of X(K,L). */

	    knext = *m;
	    for (k = *m; k >= 1; --k) {
		if (k > knext) {
		    goto L50;
		}
		if (k == 1) {
		    k1 = k;
		    k2 = k;
		} else {
		    if (a[k + (k - 1) * a_dim1] != 0.) {
			k1 = k - 1;
			k2 = k;
			knext = k - 2;
		    } else {
			k1 = k;
			k2 = k;
			knext = k - 1;
		    }
		}

		if (l1 == l2 && k1 == k2) {
		    i__2 = *m - k1;
/* Computing MIN */
		    i__3 = k1 + 1;
/* Computing MIN */
		    i__4 = k1 + 1;
		    suml = ddot_(&i__2, &a[k1 + std::min(i__3, *m)* a_dim1], lda, &
			    c__[std::min(i__4, *m)+ l1 * c_dim1], &c__1);
		    i__2 = l1 - 1;
		    sumr = ddot_(&i__2, &c__[k1 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);
		    scaloc = 1.;

		    a11 = a[k1 + k1 * a_dim1] + sgn * b[l1 + l1 * b_dim1];
		    da11 = abs(a11);
		    if (da11 <= smin) {
			a11 = smin;
			da11 = smin;
			*info = 1;
		    }
		    db = abs(vec[0]);
		    if (da11 < 1. && db > 1.) {
			if (db > bignum * da11) {
			    scaloc = 1. / db;
			}
		    }
		    x[0] = vec[0] * scaloc / a11;

		    if (scaloc != 1.) {
			i__2 = *n;
			for (j = 1; j <= i__2; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L10: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];

		} else if (l1 == l2 && k1 != k2) {

		    i__2 = *m - k2;
/* Computing MIN */
		    i__3 = k2 + 1;
/* Computing MIN */
		    i__4 = k2 + 1;
		    suml = ddot_(&i__2, &a[k1 + std::min(i__3, *m)* a_dim1], lda, &
			    c__[std::min(i__4, *m)+ l1 * c_dim1], &c__1);
		    i__2 = l1 - 1;
		    sumr = ddot_(&i__2, &c__[k1 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__2 = *m - k2;
/* Computing MIN */
		    i__3 = k2 + 1;
/* Computing MIN */
		    i__4 = k2 + 1;
		    suml = ddot_(&i__2, &a[k2 + std::min(i__3, *m)* a_dim1], lda, &
			    c__[std::min(i__4, *m)+ l1 * c_dim1], &c__1);
		    i__2 = l1 - 1;
		    sumr = ddot_(&i__2, &c__[k2 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[1] = c__[k2 + l1 * c_dim1] - (suml + sgn * sumr);

		    d__1 = -sgn * b[l1 + l1 * b_dim1];
		    dlaln2_(&c_false, &c__2, &c__1, &smin, &c_b26, &a[k1 + k1
			    * a_dim1], lda, &c_b26, &c_b26, vec, &c__2, &d__1,
			     &c_b30, x, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__2 = *n;
			for (j = 1; j <= i__2; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L20: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k2 + l1 * c_dim1] = x[1];

		} else if (l1 != l2 && k1 == k2) {

		    i__2 = *m - k1;
/* Computing MIN */
		    i__3 = k1 + 1;
/* Computing MIN */
		    i__4 = k1 + 1;
		    suml = ddot_(&i__2, &a[k1 + std::min(i__3, *m)* a_dim1], lda, &
			    c__[std::min(i__4, *m)+ l1 * c_dim1], &c__1);
		    i__2 = l1 - 1;
		    sumr = ddot_(&i__2, &c__[k1 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[0] = sgn * (c__[k1 + l1 * c_dim1] - (suml + sgn *
			    sumr));

		    i__2 = *m - k1;
/* Computing MIN */
		    i__3 = k1 + 1;
/* Computing MIN */
		    i__4 = k1 + 1;
		    suml = ddot_(&i__2, &a[k1 + std::min(i__3, *m)* a_dim1], lda, &
			    c__[std::min(i__4, *m)+ l2 * c_dim1], &c__1);
		    i__2 = l1 - 1;
		    sumr = ddot_(&i__2, &c__[k1 + c_dim1], ldc, &b[l2 *
			    b_dim1 + 1], &c__1);
		    vec[1] = sgn * (c__[k1 + l2 * c_dim1] - (suml + sgn *
			    sumr));

		    d__1 = -sgn * a[k1 + k1 * a_dim1];
		    dlaln2_(&c_true, &c__2, &c__1, &smin, &c_b26, &b[l1 + l1 *
			     b_dim1], ldb, &c_b26, &c_b26, vec, &c__2, &d__1,
			    &c_b30, x, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__2 = *n;
			for (j = 1; j <= i__2; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L30: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k1 + l2 * c_dim1] = x[1];

		} else if (l1 != l2 && k1 != k2) {

		    i__2 = *m - k2;
/* Computing MIN */
		    i__3 = k2 + 1;
/* Computing MIN */
		    i__4 = k2 + 1;
		    suml = ddot_(&i__2, &a[k1 + std::min(i__3, *m)* a_dim1], lda, &
			    c__[std::min(i__4, *m)+ l1 * c_dim1], &c__1);
		    i__2 = l1 - 1;
		    sumr = ddot_(&i__2, &c__[k1 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__2 = *m - k2;
/* Computing MIN */
		    i__3 = k2 + 1;
/* Computing MIN */
		    i__4 = k2 + 1;
		    suml = ddot_(&i__2, &a[k1 + std::min(i__3, *m)* a_dim1], lda, &
			    c__[std::min(i__4, *m)+ l2 * c_dim1], &c__1);
		    i__2 = l1 - 1;
		    sumr = ddot_(&i__2, &c__[k1 + c_dim1], ldc, &b[l2 *
			    b_dim1 + 1], &c__1);
		    vec[2] = c__[k1 + l2 * c_dim1] - (suml + sgn * sumr);

		    i__2 = *m - k2;
/* Computing MIN */
		    i__3 = k2 + 1;
/* Computing MIN */
		    i__4 = k2 + 1;
		    suml = ddot_(&i__2, &a[k2 + std::min(i__3, *m)* a_dim1], lda, &
			    c__[std::min(i__4, *m)+ l1 * c_dim1], &c__1);
		    i__2 = l1 - 1;
		    sumr = ddot_(&i__2, &c__[k2 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[1] = c__[k2 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__2 = *m - k2;
/* Computing MIN */
		    i__3 = k2 + 1;
/* Computing MIN */
		    i__4 = k2 + 1;
		    suml = ddot_(&i__2, &a[k2 + std::min(i__3, *m)* a_dim1], lda, &
			    c__[std::min(i__4, *m)+ l2 * c_dim1], &c__1);
		    i__2 = l1 - 1;
		    sumr = ddot_(&i__2, &c__[k2 + c_dim1], ldc, &b[l2 *
			    b_dim1 + 1], &c__1);
		    vec[3] = c__[k2 + l2 * c_dim1] - (suml + sgn * sumr);

		    dlasy2_(&c_false, &c_false, isgn, &c__2, &c__2, &a[k1 +
			    k1 * a_dim1], lda, &b[l1 + l1 * b_dim1], ldb, vec,
			     &c__2, &scaloc, x, &c__2, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__2 = *n;
			for (j = 1; j <= i__2; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L40: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k1 + l2 * c_dim1] = x[2];
		    c__[k2 + l1 * c_dim1] = x[1];
		    c__[k2 + l2 * c_dim1] = x[3];
		}

L50:
		;
	    }

L60:
	    ;
	}

    } else if (! notrna && notrnb) {

/*        Solve    A' *X + ISGN*X*B = scale*C. */

/*        The (K,L)th block of X is determined starting from */
/*        upper-left corner column by column by */

/*          A(K,K)'*X(K,L) + ISGN*X(K,L)*B(L,L) = C(K,L) - R(K,L) */

/*        Where */
/*                   K-1                        L-1 */
/*          R(K,L) = SUM [A(I,K)'*X(I,L)] +ISGN*SUM [X(K,J)*B(J,L)] */
/*                   I=1                        J=1 */

/*        Start column loop (index = L) */
/*        L1 (L2): column index of the first (last) row of X(K,L) */

	lnext = 1;
	i__1 = *n;
	for (l = 1; l <= i__1; ++l) {
	    if (l < lnext) {
		goto L120;
	    }
	    if (l == *n) {
		l1 = l;
		l2 = l;
	    } else {
		if (b[l + 1 + l * b_dim1] != 0.) {
		    l1 = l;
		    l2 = l + 1;
		    lnext = l + 2;
		} else {
		    l1 = l;
		    l2 = l;
		    lnext = l + 1;
		}
	    }

/*           Start row loop (index = K) */
/*           K1 (K2): row index of the first (last) row of X(K,L) */

	    knext = 1;
	    i__2 = *m;
	    for (k = 1; k <= i__2; ++k) {
		if (k < knext) {
		    goto L110;
		}
		if (k == *m) {
		    k1 = k;
		    k2 = k;
		} else {
		    if (a[k + 1 + k * a_dim1] != 0.) {
			k1 = k;
			k2 = k + 1;
			knext = k + 2;
		    } else {
			k1 = k;
			k2 = k;
			knext = k + 1;
		    }
		}

		if (l1 == l2 && k1 == k2) {
		    i__3 = k1 - 1;
		    suml = ddot_(&i__3, &a[k1 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__3 = l1 - 1;
		    sumr = ddot_(&i__3, &c__[k1 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);
		    scaloc = 1.;

		    a11 = a[k1 + k1 * a_dim1] + sgn * b[l1 + l1 * b_dim1];
		    da11 = abs(a11);
		    if (da11 <= smin) {
			a11 = smin;
			da11 = smin;
			*info = 1;
		    }
		    db = abs(vec[0]);
		    if (da11 < 1. && db > 1.) {
			if (db > bignum * da11) {
			    scaloc = 1. / db;
			}
		    }
		    x[0] = vec[0] * scaloc / a11;

		    if (scaloc != 1.) {
			i__3 = *n;
			for (j = 1; j <= i__3; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L70: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];

		} else if (l1 == l2 && k1 != k2) {

		    i__3 = k1 - 1;
		    suml = ddot_(&i__3, &a[k1 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__3 = l1 - 1;
		    sumr = ddot_(&i__3, &c__[k1 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__3 = k1 - 1;
		    suml = ddot_(&i__3, &a[k2 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__3 = l1 - 1;
		    sumr = ddot_(&i__3, &c__[k2 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[1] = c__[k2 + l1 * c_dim1] - (suml + sgn * sumr);

		    d__1 = -sgn * b[l1 + l1 * b_dim1];
		    dlaln2_(&c_true, &c__2, &c__1, &smin, &c_b26, &a[k1 + k1 *
			     a_dim1], lda, &c_b26, &c_b26, vec, &c__2, &d__1,
			    &c_b30, x, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__3 = *n;
			for (j = 1; j <= i__3; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L80: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k2 + l1 * c_dim1] = x[1];

		} else if (l1 != l2 && k1 == k2) {

		    i__3 = k1 - 1;
		    suml = ddot_(&i__3, &a[k1 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__3 = l1 - 1;
		    sumr = ddot_(&i__3, &c__[k1 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[0] = sgn * (c__[k1 + l1 * c_dim1] - (suml + sgn *
			    sumr));

		    i__3 = k1 - 1;
		    suml = ddot_(&i__3, &a[k1 * a_dim1 + 1], &c__1, &c__[l2 *
			    c_dim1 + 1], &c__1);
		    i__3 = l1 - 1;
		    sumr = ddot_(&i__3, &c__[k1 + c_dim1], ldc, &b[l2 *
			    b_dim1 + 1], &c__1);
		    vec[1] = sgn * (c__[k1 + l2 * c_dim1] - (suml + sgn *
			    sumr));

		    d__1 = -sgn * a[k1 + k1 * a_dim1];
		    dlaln2_(&c_true, &c__2, &c__1, &smin, &c_b26, &b[l1 + l1 *
			     b_dim1], ldb, &c_b26, &c_b26, vec, &c__2, &d__1,
			    &c_b30, x, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__3 = *n;
			for (j = 1; j <= i__3; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L90: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k1 + l2 * c_dim1] = x[1];

		} else if (l1 != l2 && k1 != k2) {

		    i__3 = k1 - 1;
		    suml = ddot_(&i__3, &a[k1 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__3 = l1 - 1;
		    sumr = ddot_(&i__3, &c__[k1 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__3 = k1 - 1;
		    suml = ddot_(&i__3, &a[k1 * a_dim1 + 1], &c__1, &c__[l2 *
			    c_dim1 + 1], &c__1);
		    i__3 = l1 - 1;
		    sumr = ddot_(&i__3, &c__[k1 + c_dim1], ldc, &b[l2 *
			    b_dim1 + 1], &c__1);
		    vec[2] = c__[k1 + l2 * c_dim1] - (suml + sgn * sumr);

		    i__3 = k1 - 1;
		    suml = ddot_(&i__3, &a[k2 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__3 = l1 - 1;
		    sumr = ddot_(&i__3, &c__[k2 + c_dim1], ldc, &b[l1 *
			    b_dim1 + 1], &c__1);
		    vec[1] = c__[k2 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__3 = k1 - 1;
		    suml = ddot_(&i__3, &a[k2 * a_dim1 + 1], &c__1, &c__[l2 *
			    c_dim1 + 1], &c__1);
		    i__3 = l1 - 1;
		    sumr = ddot_(&i__3, &c__[k2 + c_dim1], ldc, &b[l2 *
			    b_dim1 + 1], &c__1);
		    vec[3] = c__[k2 + l2 * c_dim1] - (suml + sgn * sumr);

		    dlasy2_(&c_true, &c_false, isgn, &c__2, &c__2, &a[k1 + k1
			    * a_dim1], lda, &b[l1 + l1 * b_dim1], ldb, vec, &
			    c__2, &scaloc, x, &c__2, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__3 = *n;
			for (j = 1; j <= i__3; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L100: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k1 + l2 * c_dim1] = x[2];
		    c__[k2 + l1 * c_dim1] = x[1];
		    c__[k2 + l2 * c_dim1] = x[3];
		}

L110:
		;
	    }
L120:
	    ;
	}

    } else if (! notrna && ! notrnb) {

/*        Solve    A'*X + ISGN*X*B' = scale*C. */

/*        The (K,L)th block of X is determined starting from */
/*        top-right corner column by column by */

/*           A(K,K)'*X(K,L) + ISGN*X(K,L)*B(L,L)' = C(K,L) - R(K,L) */

/*        Where */
/*                     K-1                          N */
/*            R(K,L) = SUM [A(I,K)'*X(I,L)] + ISGN*SUM [X(K,J)*B(L,J)']. */
/*                     I=1                        J=L+1 */

/*        Start column loop (index = L) */
/*        L1 (L2): column index of the first (last) row of X(K,L) */

	lnext = *n;
	for (l = *n; l >= 1; --l) {
	    if (l > lnext) {
		goto L180;
	    }
	    if (l == 1) {
		l1 = l;
		l2 = l;
	    } else {
		if (b[l + (l - 1) * b_dim1] != 0.) {
		    l1 = l - 1;
		    l2 = l;
		    lnext = l - 2;
		} else {
		    l1 = l;
		    l2 = l;
		    lnext = l - 1;
		}
	    }

/*           Start row loop (index = K) */
/*           K1 (K2): row index of the first (last) row of X(K,L) */

	    knext = 1;
	    i__1 = *m;
	    for (k = 1; k <= i__1; ++k) {
		if (k < knext) {
		    goto L170;
		}
		if (k == *m) {
		    k1 = k;
		    k2 = k;
		} else {
		    if (a[k + 1 + k * a_dim1] != 0.) {
			k1 = k;
			k2 = k + 1;
			knext = k + 2;
		    } else {
			k1 = k;
			k2 = k;
			knext = k + 1;
		    }
		}

		if (l1 == l2 && k1 == k2) {
		    i__2 = k1 - 1;
		    suml = ddot_(&i__2, &a[k1 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__2 = *n - l1;
/* Computing MIN */
		    i__3 = l1 + 1;
/* Computing MIN */
		    i__4 = l1 + 1;
		    sumr = ddot_(&i__2, &c__[k1 + std::min(i__3, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__4, *n)* b_dim1], ldb);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);
		    scaloc = 1.;

		    a11 = a[k1 + k1 * a_dim1] + sgn * b[l1 + l1 * b_dim1];
		    da11 = abs(a11);
		    if (da11 <= smin) {
			a11 = smin;
			da11 = smin;
			*info = 1;
		    }
		    db = abs(vec[0]);
		    if (da11 < 1. && db > 1.) {
			if (db > bignum * da11) {
			    scaloc = 1. / db;
			}
		    }
		    x[0] = vec[0] * scaloc / a11;

		    if (scaloc != 1.) {
			i__2 = *n;
			for (j = 1; j <= i__2; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L130: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];

		} else if (l1 == l2 && k1 != k2) {

		    i__2 = k1 - 1;
		    suml = ddot_(&i__2, &a[k1 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__2 = *n - l2;
/* Computing MIN */
		    i__3 = l2 + 1;
/* Computing MIN */
		    i__4 = l2 + 1;
		    sumr = ddot_(&i__2, &c__[k1 + std::min(i__3, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__4, *n)* b_dim1], ldb);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__2 = k1 - 1;
		    suml = ddot_(&i__2, &a[k2 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__2 = *n - l2;
/* Computing MIN */
		    i__3 = l2 + 1;
/* Computing MIN */
		    i__4 = l2 + 1;
		    sumr = ddot_(&i__2, &c__[k2 + std::min(i__3, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__4, *n)* b_dim1], ldb);
		    vec[1] = c__[k2 + l1 * c_dim1] - (suml + sgn * sumr);

		    d__1 = -sgn * b[l1 + l1 * b_dim1];
		    dlaln2_(&c_true, &c__2, &c__1, &smin, &c_b26, &a[k1 + k1 *
			     a_dim1], lda, &c_b26, &c_b26, vec, &c__2, &d__1,
			    &c_b30, x, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__2 = *n;
			for (j = 1; j <= i__2; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L140: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k2 + l1 * c_dim1] = x[1];

		} else if (l1 != l2 && k1 == k2) {

		    i__2 = k1 - 1;
		    suml = ddot_(&i__2, &a[k1 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__2 = *n - l2;
/* Computing MIN */
		    i__3 = l2 + 1;
/* Computing MIN */
		    i__4 = l2 + 1;
		    sumr = ddot_(&i__2, &c__[k1 + std::min(i__3, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__4, *n)* b_dim1], ldb);
		    vec[0] = sgn * (c__[k1 + l1 * c_dim1] - (suml + sgn *
			    sumr));

		    i__2 = k1 - 1;
		    suml = ddot_(&i__2, &a[k1 * a_dim1 + 1], &c__1, &c__[l2 *
			    c_dim1 + 1], &c__1);
		    i__2 = *n - l2;
/* Computing MIN */
		    i__3 = l2 + 1;
/* Computing MIN */
		    i__4 = l2 + 1;
		    sumr = ddot_(&i__2, &c__[k1 + std::min(i__3, *n)* c_dim1], ldc,
			     &b[l2 + std::min(i__4, *n)* b_dim1], ldb);
		    vec[1] = sgn * (c__[k1 + l2 * c_dim1] - (suml + sgn *
			    sumr));

		    d__1 = -sgn * a[k1 + k1 * a_dim1];
		    dlaln2_(&c_false, &c__2, &c__1, &smin, &c_b26, &b[l1 + l1
			    * b_dim1], ldb, &c_b26, &c_b26, vec, &c__2, &d__1,
			     &c_b30, x, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__2 = *n;
			for (j = 1; j <= i__2; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L150: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k1 + l2 * c_dim1] = x[1];

		} else if (l1 != l2 && k1 != k2) {

		    i__2 = k1 - 1;
		    suml = ddot_(&i__2, &a[k1 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__2 = *n - l2;
/* Computing MIN */
		    i__3 = l2 + 1;
/* Computing MIN */
		    i__4 = l2 + 1;
		    sumr = ddot_(&i__2, &c__[k1 + std::min(i__3, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__4, *n)* b_dim1], ldb);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__2 = k1 - 1;
		    suml = ddot_(&i__2, &a[k1 * a_dim1 + 1], &c__1, &c__[l2 *
			    c_dim1 + 1], &c__1);
		    i__2 = *n - l2;
/* Computing MIN */
		    i__3 = l2 + 1;
/* Computing MIN */
		    i__4 = l2 + 1;
		    sumr = ddot_(&i__2, &c__[k1 + std::min(i__3, *n)* c_dim1], ldc,
			     &b[l2 + std::min(i__4, *n)* b_dim1], ldb);
		    vec[2] = c__[k1 + l2 * c_dim1] - (suml + sgn * sumr);

		    i__2 = k1 - 1;
		    suml = ddot_(&i__2, &a[k2 * a_dim1 + 1], &c__1, &c__[l1 *
			    c_dim1 + 1], &c__1);
		    i__2 = *n - l2;
/* Computing MIN */
		    i__3 = l2 + 1;
/* Computing MIN */
		    i__4 = l2 + 1;
		    sumr = ddot_(&i__2, &c__[k2 + std::min(i__3, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__4, *n)* b_dim1], ldb);
		    vec[1] = c__[k2 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__2 = k1 - 1;
		    suml = ddot_(&i__2, &a[k2 * a_dim1 + 1], &c__1, &c__[l2 *
			    c_dim1 + 1], &c__1);
		    i__2 = *n - l2;
/* Computing MIN */
		    i__3 = l2 + 1;
/* Computing MIN */
		    i__4 = l2 + 1;
		    sumr = ddot_(&i__2, &c__[k2 + std::min(i__3, *n)* c_dim1], ldc,
			     &b[l2 + std::min(i__4, *n)* b_dim1], ldb);
		    vec[3] = c__[k2 + l2 * c_dim1] - (suml + sgn * sumr);

		    dlasy2_(&c_true, &c_true, isgn, &c__2, &c__2, &a[k1 + k1 *
			     a_dim1], lda, &b[l1 + l1 * b_dim1], ldb, vec, &
			    c__2, &scaloc, x, &c__2, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__2 = *n;
			for (j = 1; j <= i__2; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L160: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k1 + l2 * c_dim1] = x[2];
		    c__[k2 + l1 * c_dim1] = x[1];
		    c__[k2 + l2 * c_dim1] = x[3];
		}

L170:
		;
	    }
L180:
	    ;
	}

    } else if (notrna && ! notrnb) {

/*        Solve    A*X + ISGN*X*B' = scale*C. */

/*        The (K,L)th block of X is determined starting from */
/*        bottom-right corner column by column by */

/*            A(K,K)*X(K,L) + ISGN*X(K,L)*B(L,L)' = C(K,L) - R(K,L) */

/*        Where */
/*                      M                          N */
/*            R(K,L) = SUM [A(K,I)*X(I,L)] + ISGN*SUM [X(K,J)*B(L,J)']. */
/*                    I=K+1                      J=L+1 */

/*        Start column loop (index = L) */
/*        L1 (L2): column index of the first (last) row of X(K,L) */

	lnext = *n;
	for (l = *n; l >= 1; --l) {
	    if (l > lnext) {
		goto L240;
	    }
	    if (l == 1) {
		l1 = l;
		l2 = l;
	    } else {
		if (b[l + (l - 1) * b_dim1] != 0.) {
		    l1 = l - 1;
		    l2 = l;
		    lnext = l - 2;
		} else {
		    l1 = l;
		    l2 = l;
		    lnext = l - 1;
		}
	    }

/*           Start row loop (index = K) */
/*           K1 (K2): row index of the first (last) row of X(K,L) */

	    knext = *m;
	    for (k = *m; k >= 1; --k) {
		if (k > knext) {
		    goto L230;
		}
		if (k == 1) {
		    k1 = k;
		    k2 = k;
		} else {
		    if (a[k + (k - 1) * a_dim1] != 0.) {
			k1 = k - 1;
			k2 = k;
			knext = k - 2;
		    } else {
			k1 = k;
			k2 = k;
			knext = k - 1;
		    }
		}

		if (l1 == l2 && k1 == k2) {
		    i__1 = *m - k1;
/* Computing MIN */
		    i__2 = k1 + 1;
/* Computing MIN */
		    i__3 = k1 + 1;
		    suml = ddot_(&i__1, &a[k1 + std::min(i__2, *m)* a_dim1], lda, &
			    c__[std::min(i__3, *m)+ l1 * c_dim1], &c__1);
		    i__1 = *n - l1;
/* Computing MIN */
		    i__2 = l1 + 1;
/* Computing MIN */
		    i__3 = l1 + 1;
		    sumr = ddot_(&i__1, &c__[k1 + std::min(i__2, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__3, *n)* b_dim1], ldb);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);
		    scaloc = 1.;

		    a11 = a[k1 + k1 * a_dim1] + sgn * b[l1 + l1 * b_dim1];
		    da11 = abs(a11);
		    if (da11 <= smin) {
			a11 = smin;
			da11 = smin;
			*info = 1;
		    }
		    db = abs(vec[0]);
		    if (da11 < 1. && db > 1.) {
			if (db > bignum * da11) {
			    scaloc = 1. / db;
			}
		    }
		    x[0] = vec[0] * scaloc / a11;

		    if (scaloc != 1.) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L190: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];

		} else if (l1 == l2 && k1 != k2) {

		    i__1 = *m - k2;
/* Computing MIN */
		    i__2 = k2 + 1;
/* Computing MIN */
		    i__3 = k2 + 1;
		    suml = ddot_(&i__1, &a[k1 + std::min(i__2, *m)* a_dim1], lda, &
			    c__[std::min(i__3, *m)+ l1 * c_dim1], &c__1);
		    i__1 = *n - l2;
/* Computing MIN */
		    i__2 = l2 + 1;
/* Computing MIN */
		    i__3 = l2 + 1;
		    sumr = ddot_(&i__1, &c__[k1 + std::min(i__2, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__3, *n)* b_dim1], ldb);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__1 = *m - k2;
/* Computing MIN */
		    i__2 = k2 + 1;
/* Computing MIN */
		    i__3 = k2 + 1;
		    suml = ddot_(&i__1, &a[k2 + std::min(i__2, *m)* a_dim1], lda, &
			    c__[std::min(i__3, *m)+ l1 * c_dim1], &c__1);
		    i__1 = *n - l2;
/* Computing MIN */
		    i__2 = l2 + 1;
/* Computing MIN */
		    i__3 = l2 + 1;
		    sumr = ddot_(&i__1, &c__[k2 + std::min(i__2, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__3, *n)* b_dim1], ldb);
		    vec[1] = c__[k2 + l1 * c_dim1] - (suml + sgn * sumr);

		    d__1 = -sgn * b[l1 + l1 * b_dim1];
		    dlaln2_(&c_false, &c__2, &c__1, &smin, &c_b26, &a[k1 + k1
			    * a_dim1], lda, &c_b26, &c_b26, vec, &c__2, &d__1,
			     &c_b30, x, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L200: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k2 + l1 * c_dim1] = x[1];

		} else if (l1 != l2 && k1 == k2) {

		    i__1 = *m - k1;
/* Computing MIN */
		    i__2 = k1 + 1;
/* Computing MIN */
		    i__3 = k1 + 1;
		    suml = ddot_(&i__1, &a[k1 + std::min(i__2, *m)* a_dim1], lda, &
			    c__[std::min(i__3, *m)+ l1 * c_dim1], &c__1);
		    i__1 = *n - l2;
/* Computing MIN */
		    i__2 = l2 + 1;
/* Computing MIN */
		    i__3 = l2 + 1;
		    sumr = ddot_(&i__1, &c__[k1 + std::min(i__2, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__3, *n)* b_dim1], ldb);
		    vec[0] = sgn * (c__[k1 + l1 * c_dim1] - (suml + sgn *
			    sumr));

		    i__1 = *m - k1;
/* Computing MIN */
		    i__2 = k1 + 1;
/* Computing MIN */
		    i__3 = k1 + 1;
		    suml = ddot_(&i__1, &a[k1 + std::min(i__2, *m)* a_dim1], lda, &
			    c__[std::min(i__3, *m)+ l2 * c_dim1], &c__1);
		    i__1 = *n - l2;
/* Computing MIN */
		    i__2 = l2 + 1;
/* Computing MIN */
		    i__3 = l2 + 1;
		    sumr = ddot_(&i__1, &c__[k1 + std::min(i__2, *n)* c_dim1], ldc,
			     &b[l2 + std::min(i__3, *n)* b_dim1], ldb);
		    vec[1] = sgn * (c__[k1 + l2 * c_dim1] - (suml + sgn *
			    sumr));

		    d__1 = -sgn * a[k1 + k1 * a_dim1];
		    dlaln2_(&c_false, &c__2, &c__1, &smin, &c_b26, &b[l1 + l1
			    * b_dim1], ldb, &c_b26, &c_b26, vec, &c__2, &d__1,
			     &c_b30, x, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L210: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k1 + l2 * c_dim1] = x[1];

		} else if (l1 != l2 && k1 != k2) {

		    i__1 = *m - k2;
/* Computing MIN */
		    i__2 = k2 + 1;
/* Computing MIN */
		    i__3 = k2 + 1;
		    suml = ddot_(&i__1, &a[k1 + std::min(i__2, *m)* a_dim1], lda, &
			    c__[std::min(i__3, *m)+ l1 * c_dim1], &c__1);
		    i__1 = *n - l2;
/* Computing MIN */
		    i__2 = l2 + 1;
/* Computing MIN */
		    i__3 = l2 + 1;
		    sumr = ddot_(&i__1, &c__[k1 + std::min(i__2, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__3, *n)* b_dim1], ldb);
		    vec[0] = c__[k1 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__1 = *m - k2;
/* Computing MIN */
		    i__2 = k2 + 1;
/* Computing MIN */
		    i__3 = k2 + 1;
		    suml = ddot_(&i__1, &a[k1 + std::min(i__2, *m)* a_dim1], lda, &
			    c__[std::min(i__3, *m)+ l2 * c_dim1], &c__1);
		    i__1 = *n - l2;
/* Computing MIN */
		    i__2 = l2 + 1;
/* Computing MIN */
		    i__3 = l2 + 1;
		    sumr = ddot_(&i__1, &c__[k1 + std::min(i__2, *n)* c_dim1], ldc,
			     &b[l2 + std::min(i__3, *n)* b_dim1], ldb);
		    vec[2] = c__[k1 + l2 * c_dim1] - (suml + sgn * sumr);

		    i__1 = *m - k2;
/* Computing MIN */
		    i__2 = k2 + 1;
/* Computing MIN */
		    i__3 = k2 + 1;
		    suml = ddot_(&i__1, &a[k2 + std::min(i__2, *m)* a_dim1], lda, &
			    c__[std::min(i__3, *m)+ l1 * c_dim1], &c__1);
		    i__1 = *n - l2;
/* Computing MIN */
		    i__2 = l2 + 1;
/* Computing MIN */
		    i__3 = l2 + 1;
		    sumr = ddot_(&i__1, &c__[k2 + std::min(i__2, *n)* c_dim1], ldc,
			     &b[l1 + std::min(i__3, *n)* b_dim1], ldb);
		    vec[1] = c__[k2 + l1 * c_dim1] - (suml + sgn * sumr);

		    i__1 = *m - k2;
/* Computing MIN */
		    i__2 = k2 + 1;
/* Computing MIN */
		    i__3 = k2 + 1;
		    suml = ddot_(&i__1, &a[k2 + std::min(i__2, *m)* a_dim1], lda, &
			    c__[std::min(i__3, *m)+ l2 * c_dim1], &c__1);
		    i__1 = *n - l2;
/* Computing MIN */
		    i__2 = l2 + 1;
/* Computing MIN */
		    i__3 = l2 + 1;
		    sumr = ddot_(&i__1, &c__[k2 + std::min(i__2, *n)* c_dim1], ldc,
			     &b[l2 + std::min(i__3, *n)* b_dim1], ldb);
		    vec[3] = c__[k2 + l2 * c_dim1] - (suml + sgn * sumr);

		    dlasy2_(&c_false, &c_true, isgn, &c__2, &c__2, &a[k1 + k1
			    * a_dim1], lda, &b[l1 + l1 * b_dim1], ldb, vec, &
			    c__2, &scaloc, x, &c__2, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 1;
		    }

		    if (scaloc != 1.) {
			i__1 = *n;
			for (j = 1; j <= i__1; ++j) {
			    dscal_(m, &scaloc, &c__[j * c_dim1 + 1], &c__1);
/* L220: */
			}
			*scale *= scaloc;
		    }
		    c__[k1 + l1 * c_dim1] = x[0];
		    c__[k1 + l2 * c_dim1] = x[2];
		    c__[k2 + l1 * c_dim1] = x[1];
		    c__[k2 + l2 * c_dim1] = x[3];
		}

L230:
		;
	    }
L240:
	    ;
	}

    }

    return 0;

/*     End of DTRSYL */

} /* dtrsyl_ */

/* Subroutine */ int dtrti2_(const char *uplo, const char *diag, integer *n, double *
	a, integer *lda, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer j;
    double ajj;
    bool upper;
    bool nounit;

/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRTI2 computes the inverse of a real upper or lower triangular */
/*  matrix. */

/*  This is the Level 2 BLAS version of the algorithm. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the triangular matrix A.  If UPLO = 'U', the */
/*          leading n by n upper triangular part of the array A contains */
/*          the upper triangular matrix, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading n by n lower triangular part of the array A contains */
/*          the lower triangular matrix, and the strictly upper */
/*          triangular part of A is not referenced.  If DIAG = 'U', the */
/*          diagonal elements of A are also not referenced and are */
/*          assumed to be 1. */

/*          On exit, the (triangular) inverse of the original matrix, in */
/*          the same storage format. */

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
    nounit = lsame_(diag, "N");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTRTI2", &i__1);
	return 0;
    }

    if (upper) {

/*        Compute inverse of upper triangular matrix. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (nounit) {
		a[j + j * a_dim1] = 1. / a[j + j * a_dim1];
		ajj = -a[j + j * a_dim1];
	    } else {
		ajj = -1.;
	    }

/*           Compute elements 1:j-1 of j-th column. */

	    i__2 = j - 1;
	    dtrmv_("Upper", "No transpose", diag, &i__2, &a[a_offset], lda, &
		    a[j * a_dim1 + 1], &c__1);
	    i__2 = j - 1;
	    dscal_(&i__2, &ajj, &a[j * a_dim1 + 1], &c__1);
/* L10: */
	}
    } else {

/*        Compute inverse of lower triangular matrix. */

	for (j = *n; j >= 1; --j) {
	    if (nounit) {
		a[j + j * a_dim1] = 1. / a[j + j * a_dim1];
		ajj = -a[j + j * a_dim1];
	    } else {
		ajj = -1.;
	    }
	    if (j < *n) {

/*              Compute elements j+1:n of j-th column. */

		i__1 = *n - j;
		dtrmv_("Lower", "No transpose", diag, &i__1, &a[j + 1 + (j +
			1) * a_dim1], lda, &a[j + 1 + j * a_dim1], &c__1);
		i__1 = *n - j;
		dscal_(&i__1, &ajj, &a[j + 1 + j * a_dim1], &c__1);
	    }
/* L20: */
	}
    }

    return 0;

/*     End of DTRTI2 */

} /* dtrti2_ */

/* Subroutine */ int dtrtri_(const char *uplo, const char *diag, integer *n, double *
	a, integer *lda, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__2 = 2;
	static double c_b18 = 1.;
	static double c_b22 = -1.;

    /* System generated locals */
    char * a__1[2];
    integer a_dim1, a_offset, i__1, i__2[2], i__3, i__4, i__5;
    char ch__1[3] = { 0 };

    /* Local variables */
    integer j, jb, nb, nn;
    bool upper;
    bool nounit;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRTRI computes the inverse of a real upper or lower triangular */
/*  matrix A. */

/*  This is the Level 3 BLAS version of the algorithm. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the triangular matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of the array A contains */
/*          the upper triangular matrix, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading N-by-N lower triangular part of the array A contains */
/*          the lower triangular matrix, and the strictly upper */
/*          triangular part of A is not referenced.  If DIAG = 'U', the */
/*          diagonal elements of A are also not referenced and are */
/*          assumed to be 1. */
/*          On exit, the (triangular) inverse of the original matrix, in */
/*          the same storage format. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, A(i,i) is exactly zero.  The triangular */
/*               matrix is singular and its inverse can not be computed. */

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
    nounit = lsame_(diag, "N");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTRTRI", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Check for singularity if non-unit. */

    if (nounit) {
	i__1 = *n;
	for (*info = 1; *info <= i__1; ++(*info)) {
	    if (a[*info + *info * a_dim1] == 0.) {
		return 0;
	    }
/* L10: */
	}
	*info = 0;
    }

/*     Determine the block size for this environment. */

/* Writing concatenation */
    i__2[0] = 1, a__1[0] = const_cast<char *> (uplo);
    i__2[1] = 1, a__1[1] = const_cast<char *> (diag);
    s_cat(ch__1, a__1, i__2, &c__2, 2_integer);
    nb = ilaenv_(&c__1, "DTRTRI", ch__1, n, &c_n1, &c_n1, &c_n1);
    if (nb <= 1 || nb >= *n) {

/*        Use unblocked code */

	dtrti2_(uplo, diag, n, &a[a_offset], lda, info);
    } else {

/*        Use blocked code */

	if (upper) {

/*           Compute inverse of upper triangular matrix */

	    i__1 = *n;
	    i__3 = nb;
	    for (j = 1; i__3 < 0 ? j >= i__1 : j <= i__1; j += i__3) {
/* Computing MIN */
		i__4 = nb, i__5 = *n - j + 1;
		jb = std::min(i__4,i__5);

/*              Compute rows 1:j-1 of current block column */

		i__4 = j - 1;
		dtrmm_("Left", "Upper", "No transpose", diag, &i__4, &jb, &
			c_b18, &a[a_offset], lda, &a[j * a_dim1 + 1], lda);
		i__4 = j - 1;
		dtrsm_("Right", "Upper", "No transpose", diag, &i__4, &jb, &
			c_b22, &a[j + j * a_dim1], lda, &a[j * a_dim1 + 1],
			lda);

/*              Compute inverse of current diagonal block */

		dtrti2_("Upper", diag, &jb, &a[j + j * a_dim1], lda, info);
/* L20: */
	    }
	} else {

/*           Compute inverse of lower triangular matrix */

	    nn = (*n - 1) / nb * nb + 1;
	    i__3 = -nb;
	    for (j = nn; i__3 < 0 ? j >= 1 : j <= 1; j += i__3) {
/* Computing MIN */
		i__1 = nb, i__4 = *n - j + 1;
		jb = std::min(i__1,i__4);
		if (j + jb <= *n) {

/*                 Compute rows j+jb:n of current block column */

		    i__1 = *n - j - jb + 1;
		    dtrmm_("Left", "Lower", "No transpose", diag, &i__1, &jb,
			    &c_b18, &a[j + jb + (j + jb) * a_dim1], lda, &a[j
			    + jb + j * a_dim1], lda);
		    i__1 = *n - j - jb + 1;
		    dtrsm_("Right", "Lower", "No transpose", diag, &i__1, &jb,
			     &c_b22, &a[j + j * a_dim1], lda, &a[j + jb + j *
			    a_dim1], lda);
		}

/*              Compute inverse of current diagonal block */

		dtrti2_("Lower", diag, &jb, &a[j + j * a_dim1], lda, info);
/* L30: */
	    }
	}
    }

    return 0;

/*     End of DTRTRI */

} /* dtrtri_ */

/* Subroutine */ int dtrtrs_(const char *uplo, const char *trans, const char *diag, integer *n,
	integer *nrhs, double *a, integer *lda, double *b, integer *
	ldb, integer *info)
{
	/* Table of constant values */
	static double c_b12 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;

    /* Local variables */
    bool nounit;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRTRS solves a triangular system of the form */

/*     A * X = B  or  A**T * X = B, */

/*  where A is a triangular matrix of order N, and B is an N-by-NRHS */
/*  matrix.  A check is made to verify that A is nonsingular. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B  (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The triangular matrix A.  If UPLO = 'U', the leading N-by-N */
/*          upper triangular part of the array A contains the upper */
/*          triangular matrix, and the strictly lower triangular part of */
/*          A is not referenced.  If UPLO = 'L', the leading N-by-N lower */
/*          triangular part of the array A contains the lower triangular */
/*          matrix, and the strictly upper triangular part of A is not */
/*          referenced.  If DIAG = 'U', the diagonal elements of A are */
/*          also not referenced and are assumed to be 1. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the right hand side matrix B. */
/*          On exit, if INFO = 0, the solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, the i-th diagonal element of A is zero, */
/*               indicating that the matrix is singular and the solutions */
/*               X have not been computed. */

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
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    nounit = lsame_(diag, "N");
    if (! lsame_(uplo, "U") && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! lsame_(trans, "N") && ! lsame_(trans,
	    "T") && ! lsame_(trans, "C")) {
	*info = -2;
    } else if (! nounit && ! lsame_(diag, "U")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*nrhs < 0) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTRTRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Check for singularity. */

    if (nounit) {
	i__1 = *n;
	for (*info = 1; *info <= i__1; ++(*info)) {
	    if (a[*info + *info * a_dim1] == 0.) {
		return 0;
	    }
/* L10: */
	}
    }
    *info = 0;

/*     Solve A * x = b  or  A' * x = b. */

    dtrsm_("Left", uplo, trans, diag, n, nrhs, &c_b12, &a[a_offset], lda, &b[
	    b_offset], ldb);

    return 0;

/*     End of DTRTRS */

} /* dtrtrs_ */

int dtrttf_(const char *transr, const char *uplo, integer *n, double *a, integer *lda, double *arf, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, k, l, n1, n2, ij, nt, nx2, np1x2;
    bool normaltransr, lower, nisodd;


/*  -- LAPACK routine (version 3.2)                                    -- */

/*  -- Contributed by Fred Gustavson of the IBM Watson Research Center -- */
/*  -- November 2008                                                   -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRTTF copies a triangular matrix A from standard full format (TR) */
/*  to rectangular full packed format (TF) . */

/*  Arguments */
/*  ========= */

/*  TRANSR   (input) CHARACTER */
/*          = 'N':  ARF in Normal form is wanted; */
/*          = 'T':  ARF in Transpose form is wanted. */

/*  UPLO    (input) CHARACTER */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A. N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N). */
/*          On entry, the triangular matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of the array A contains */
/*          the upper triangular matrix, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading N-by-N lower triangular part of the array A contains */
/*          the lower triangular matrix, and the strictly upper */
/*          triangular part of A is not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the matrix A. LDA >= max(1,N). */

/*  ARF     (output) DOUBLE PRECISION array, dimension (NT). */
/*          NT=N*(N+1)/2. On exit, the triangular matrix A in RFP format. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Notes */
/*  ===== */

/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  even. We give an example where N = 6. */

/*      AP is Upper             AP is Lower */

/*   00 01 02 03 04 05       00 */
/*      11 12 13 14 15       10 11 */
/*         22 23 24 25       20 21 22 */
/*            33 34 35       30 31 32 33 */
/*               44 45       40 41 42 43 44 */
/*                  55       50 51 52 53 54 55 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:5,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(4:6,0:2) consists of */
/*  the transpose of the first three columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(1:6,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:2,0:2) consists of */
/*  the transpose of the last three columns of AP lower. */
/*  This covers the case N even and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        03 04 05                33 43 53 */
/*        13 14 15                00 44 54 */
/*        23 24 25                10 11 55 */
/*        33 34 35                20 21 22 */
/*        00 44 45                30 31 32 */
/*        01 11 55                40 41 42 */
/*        02 12 22                50 51 52 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */


/*           RFP A                   RFP A */

/*     03 13 23 33 00 01 02    33 00 10 20 30 40 50 */
/*     04 14 24 34 44 11 12    43 44 11 21 31 41 51 */
/*     05 15 25 35 45 55 22    53 54 55 22 32 42 52 */


/*  We first consider Rectangular Full Packed (RFP) Format when N is */
/*  odd. We give an example where N = 5. */

/*     AP is Upper                 AP is Lower */

/*   00 01 02 03 04              00 */
/*      11 12 13 14              10 11 */
/*         22 23 24              20 21 22 */
/*            33 34              30 31 32 33 */
/*               44              40 41 42 43 44 */


/*  Let TRANSR = 'N'. RFP holds AP as follows: */
/*  For UPLO = 'U' the upper trapezoid A(0:4,0:2) consists of the last */
/*  three columns of AP upper. The lower triangle A(3:4,0:1) consists of */
/*  the transpose of the first two columns of AP upper. */
/*  For UPLO = 'L' the lower trapezoid A(0:4,0:2) consists of the first */
/*  three columns of AP lower. The upper triangle A(0:1,1:2) consists of */
/*  the transpose of the last two columns of AP lower. */
/*  This covers the case N odd and TRANSR = 'N'. */

/*         RFP A                   RFP A */

/*        02 03 04                00 33 43 */
/*        12 13 14                10 11 44 */
/*        22 23 24                20 21 22 */
/*        00 33 34                30 31 32 */
/*        01 11 44                40 41 42 */

/*  Now let TRANSR = 'T'. RFP A in both UPLO cases is just the */
/*  transpose of RFP A above. One therefore gets: */

/*           RFP A                   RFP A */

/*     02 12 22 00 01             00 10 20 30 40 50 */
/*     03 13 23 33 11             33 11 21 31 41 51 */
/*     04 14 24 34 44             43 44 22 32 42 52 */

/*  Reference */
/*  ========= */

/*  ===================================================================== */

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
    a_dim1 = *lda - 1 - 0 + 1;
    a_offset = 0 + a_dim1 * 0;
    a -= a_offset;

    /* Function Body */
    *info = 0;
    normaltransr = lsame_(transr, "N");
    lower = lsame_(uplo, "L");
    if (! normaltransr && ! lsame_(transr, "T")) {
	*info = -1;
    } else if (! lower && ! lsame_(uplo, "U")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTRTTF", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 1) {
	if (*n == 1) {
	    arf[0] = a[0];
	}
	return 0;
    }

/*     Size of array ARF(0:nt-1) */

    nt = *n * (*n + 1) / 2;

/*     Set N1 and N2 depending on LOWER: for N even N1=N2=K */

    if (lower) {
	n2 = *n / 2;
	n1 = *n - n2;
    } else {
	n1 = *n / 2;
	n2 = *n - n1;
    }

/*     If N is odd, set NISODD = .TRUE., LDA=N+1 and A is (N+1)--by--K2. */
/*     If N is even, set K = N/2 and NISODD = .FALSE., LDA=N and A is */
/*     N--by--(N+1)/2. */

    if (*n % 2 == 0) {
	k = *n / 2;
	nisodd = false;
	if (! lower) {
	    np1x2 = *n + *n + 2;
	}
    } else {
	nisodd = true;
	if (! lower) {
	    nx2 = *n + *n;
	}
    }

    if (nisodd) {

/*        N is odd */

	if (normaltransr) {

/*           N is odd and TRANSR = 'N' */

	    if (lower) {

/*              N is odd, TRANSR = 'N', and UPLO = 'L' */

		ij = 0;
		i__1 = n2;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = n2 + j;
		    for (i__ = n1; i__ <= i__2; ++i__) {
			arf[ij] = a[n2 + j + i__ * a_dim1];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (i__ = j; i__ <= i__2; ++i__) {
			arf[ij] = a[i__ + j * a_dim1];
			++ij;
		    }
		}

	    } else {

/*              N is odd, TRANSR = 'N', and UPLO = 'U' */

		ij = nt - *n;
		i__1 = n1;
		for (j = *n - 1; j >= i__1; --j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			arf[ij] = a[i__ + j * a_dim1];
			++ij;
		    }
		    i__2 = n1 - 1;
		    for (l = j - n1; l <= i__2; ++l) {
			arf[ij] = a[j - n1 + l * a_dim1];
			++ij;
		    }
		    ij -= nx2;
		}

	    }

	} else {

/*           N is odd and TRANSR = 'T' */

	    if (lower) {

/*              N is odd, TRANSR = 'T', and UPLO = 'L' */

		ij = 0;
		i__1 = n2 - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			arf[ij] = a[j + i__ * a_dim1];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (i__ = n1 + j; i__ <= i__2; ++i__) {
			arf[ij] = a[i__ + (n1 + j) * a_dim1];
			++ij;
		    }
		}
		i__1 = *n - 1;
		for (j = n2; j <= i__1; ++j) {
		    i__2 = n1 - 1;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			arf[ij] = a[j + i__ * a_dim1];
			++ij;
		    }
		}

	    } else {

/*              N is odd, TRANSR = 'T', and UPLO = 'U' */

		ij = 0;
		i__1 = n1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = *n - 1;
		    for (i__ = n1; i__ <= i__2; ++i__) {
			arf[ij] = a[j + i__ * a_dim1];
			++ij;
		    }
		}
		i__1 = n1 - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			arf[ij] = a[i__ + j * a_dim1];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (l = n2 + j; l <= i__2; ++l) {
			arf[ij] = a[n2 + j + l * a_dim1];
			++ij;
		    }
		}

	    }

	}

    } else {

/*        N is even */

	if (normaltransr) {

/*           N is even and TRANSR = 'N' */

	    if (lower) {

/*              N is even, TRANSR = 'N', and UPLO = 'L' */

		ij = 0;
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = k + j;
		    for (i__ = k; i__ <= i__2; ++i__) {
			arf[ij] = a[k + j + i__ * a_dim1];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (i__ = j; i__ <= i__2; ++i__) {
			arf[ij] = a[i__ + j * a_dim1];
			++ij;
		    }
		}

	    } else {

/*              N is even, TRANSR = 'N', and UPLO = 'U' */

		ij = nt - *n - 1;
		i__1 = k;
		for (j = *n - 1; j >= i__1; --j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			arf[ij] = a[i__ + j * a_dim1];
			++ij;
		    }
		    i__2 = k - 1;
		    for (l = j - k; l <= i__2; ++l) {
			arf[ij] = a[j - k + l * a_dim1];
			++ij;
		    }
		    ij -= np1x2;
		}

	    }

	} else {

/*           N is even and TRANSR = 'T' */

	    if (lower) {

/*              N is even, TRANSR = 'T', and UPLO = 'L' */

		ij = 0;
		j = k;
		i__1 = *n - 1;
		for (i__ = k; i__ <= i__1; ++i__) {
		    arf[ij] = a[i__ + j * a_dim1];
		    ++ij;
		}
		i__1 = k - 2;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			arf[ij] = a[j + i__ * a_dim1];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (i__ = k + 1 + j; i__ <= i__2; ++i__) {
			arf[ij] = a[i__ + (k + 1 + j) * a_dim1];
			++ij;
		    }
		}
		i__1 = *n - 1;
		for (j = k - 1; j <= i__1; ++j) {
		    i__2 = k - 1;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			arf[ij] = a[j + i__ * a_dim1];
			++ij;
		    }
		}

	    } else {

/*              N is even, TRANSR = 'T', and UPLO = 'U' */

		ij = 0;
		i__1 = k;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = *n - 1;
		    for (i__ = k; i__ <= i__2; ++i__) {
			arf[ij] = a[j + i__ * a_dim1];
			++ij;
		    }
		}
		i__1 = k - 2;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 0; i__ <= i__2; ++i__) {
			arf[ij] = a[i__ + j * a_dim1];
			++ij;
		    }
		    i__2 = *n - 1;
		    for (l = k + 1 + j; l <= i__2; ++l) {
			arf[ij] = a[k + 1 + j + l * a_dim1];
			++ij;
		    }
		}
/*              Note that here, on exit of the loop, J = K-1 */
		i__1 = j;
		for (i__ = 0; i__ <= i__1; ++i__) {
		    arf[ij] = a[i__ + j * a_dim1];
		    ++ij;
		}

	    }

	}

    }

    return 0;

/*     End of DTRTTF */

} /* dtrttf_ */

int dtrttp_(const char *uplo, integer *n, double *a, integer *lda, double *ap, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, k;
    bool lower;


/*  -- LAPACK routine (version 3.2) -- */
/*  -- Contributed by Fred Gustavson of the IBM Watson Research Center -- */
/*  --            and Julien Langou of the Univ. of Colorado Denver    -- */
/*  -- November 2008 -- */

/*  -- LAPACK is a software package provided by Univ. of Tennessee,    -- */
/*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..-- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRTTP copies a triangular matrix A from full format (TR) to standard */
/*  packed format (TP). */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER */
/*          = 'U':  A is upper triangular. */
/*          = 'L':  A is lower triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrices AP and A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On exit, the triangular matrix A.  If UPLO = 'U', the leading */
/*          N-by-N upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading N-by-N lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  AP      (output) DOUBLE PRECISION array, dimension (N*(N+1)/2 */
/*          On exit, the upper or lower triangular matrix A, packed */
/*          columnwise in a linear array. The j-th column of A is stored */
/*          in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

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

/*     Test the input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --ap;

    /* Function Body */
    *info = 0;
    lower = lsame_(uplo, "L");
    if (! lower && ! lsame_(uplo, "U")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTRTTP", &i__1);
	return 0;
    }

    if (lower) {
	k = 0;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = j; i__ <= i__2; ++i__) {
		++k;
		ap[k] = a[i__ + j * a_dim1];
	    }
	}
    } else {
	k = 0;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		++k;
		ap[k] = a[i__ + j * a_dim1];
	    }
	}
    }


    return 0;

/*     End of DTRTTP */

} /* dtrttp_ */

/* Subroutine */ int dtzrqf_(integer *m, integer *n, double *a, integer *lda, double *tau, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b8 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer i__, k, m1;

/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This routine is deprecated and has been replaced by routine DTZRZF. */

/*  DTZRQF reduces the M-by-N ( M<=N ) real upper trapezoidal matrix A */
/*  to upper triangular form by means of orthogonal transformations. */

/*  The upper trapezoidal matrix A is factored as */

/*     A = ( R  0 ) * Z, */

/*  where Z is an N-by-N orthogonal matrix and R is an M-by-M upper */
/*  triangular matrix. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= M. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the leading M-by-N upper trapezoidal part of the */
/*          array A must contain the matrix to be factorized. */
/*          On exit, the leading M-by-M upper triangular part of A */
/*          contains the upper triangular matrix R, and elements M+1 to */
/*          N of the first M rows of A, with the array TAU, represent the */
/*          orthogonal matrix Z as a product of M elementary reflectors. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (M) */
/*          The scalar factors of the elementary reflectors. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The factorization is obtained by Householder's method.  The kth */
/*  transformation matrix, Z( k ), which is used to introduce zeros into */
/*  the ( m - k + 1 )th row of A, is given in the form */

/*     Z( k ) = ( I     0   ), */
/*              ( 0  T( k ) ) */

/*  where */

/*     T( k ) = I - tau*u( k )*u( k )',   u( k ) = (   1    ), */
/*                                                 (   0    ) */
/*                                                 ( z( k ) ) */

/*  tau is a scalar and z( k ) is an ( n - m ) element vector. */
/*  tau and z( k ) are chosen to annihilate the elements of the kth row */
/*  of X. */

/*  The scalar tau is returned in the kth element of TAU and the vector */
/*  u( k ) in the kth row of A, such that the elements of z( k ) are */
/*  in  a( k, m + 1 ), ..., a( k, n ). The elements of R are returned in */
/*  the upper triangular part of A. */

/*  Z is given by */

/*     Z =  Z( 1 ) * Z( 2 ) * ... * Z( m ). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < *m) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTZRQF", &i__1);
	return 0;
    }

/*     Perform the factorization. */

    if (*m == 0) {
	return 0;
    }
    if (*m == *n) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    tau[i__] = 0.;
/* L10: */
	}
    } else {
/* Computing MIN */
	i__1 = *m + 1;
	m1 = std::min(i__1,*n);
	for (k = *m; k >= 1; --k) {

/*           Use a Householder reflection to zero the kth row of A. */
/*           First set up the reflection. */

	    i__1 = *n - *m + 1;
	    dlarfp_(&i__1, &a[k + k * a_dim1], &a[k + m1 * a_dim1], lda, &tau[
		    k]);

	    if (tau[k] != 0. && k > 1) {

/*              We now perform the operation  A := A*P( k ). */

/*              Use the first ( k - 1 ) elements of TAU to store  a( k ), */
/*              where  a( k ) consists of the first ( k - 1 ) elements of */
/*              the  kth column  of  A.  Also  let  B  denote  the  first */
/*              ( k - 1 ) rows of the last ( n - m ) columns of A. */

		i__1 = k - 1;
		dcopy_(&i__1, &a[k * a_dim1 + 1], &c__1, &tau[1], &c__1);

/*              Form   w = a( k ) + B*z( k )  in TAU. */

		i__1 = k - 1;
		i__2 = *n - *m;
		dgemv_("No transpose", &i__1, &i__2, &c_b8, &a[m1 * a_dim1 +
			1], lda, &a[k + m1 * a_dim1], lda, &c_b8, &tau[1], &
			c__1);

/*              Now form  a( k ) := a( k ) - tau*w */
/*              and       B      := B      - tau*w*z( k )'. */

		i__1 = k - 1;
		d__1 = -tau[k];
		daxpy_(&i__1, &d__1, &tau[1], &c__1, &a[k * a_dim1 + 1], &
			c__1);
		i__1 = k - 1;
		i__2 = *n - *m;
		d__1 = -tau[k];
		dger_(&i__1, &i__2, &d__1, &tau[1], &c__1, &a[k + m1 * a_dim1]
, lda, &a[m1 * a_dim1 + 1], lda);
	    }
/* L20: */
	}
    }

    return 0;

/*     End of DTZRQF */

} /* dtzrqf_ */

/* Subroutine */ int dtzrzf_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;

    /* Local variables */
    integer i__, m1, ib, nb, ki, kk, mu, nx, iws, nbmin;
	integer ldwork, lwkopt;
    bool lquery;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTZRZF reduces the M-by-N ( M<=N ) real upper trapezoidal matrix A */
/*  to upper triangular form by means of orthogonal transformations. */

/*  The upper trapezoidal matrix A is factored as */

/*     A = ( R  0 ) * Z, */

/*  where Z is an N-by-N orthogonal matrix and R is an M-by-M upper */
/*  triangular matrix. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= M. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the leading M-by-N upper trapezoidal part of the */
/*          array A must contain the matrix to be factorized. */
/*          On exit, the leading M-by-M upper triangular part of A */
/*          contains the upper triangular matrix R, and elements M+1 to */
/*          N of the first M rows of A, with the array TAU, represent the */
/*          orthogonal matrix Z as a product of M elementary reflectors. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1_integer,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (M) */
/*          The scalar factors of the elementary reflectors. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1_integer,M). */
/*          For optimum performance LWORK >= M*NB, where NB is */
/*          the optimal blocksize. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

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

/*  tau is a scalar and z( k ) is an ( n - m ) element vector. */
/*  tau and z( k ) are chosen to annihilate the elements of the kth row */
/*  of X. */

/*  The scalar tau is returned in the kth element of TAU and the vector */
/*  u( k ) in the kth row of A, such that the elements of z( k ) are */
/*  in  a( k, m + 1 ), ..., a( k, n ). The elements of R are returned in */
/*  the upper triangular part of A. */

/*  Z is given by */

/*     Z =  Z( 1 ) * Z( 2 ) * ... * Z( m ). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < *m) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }

    if (*info == 0) {
	if (*m == 0 || *m == *n) {
	    lwkopt = 1;
	} else {

/*           Determine the block size. */

	    nb = ilaenv_(&c__1, "DGERQF", " ", m, n, &c_n1, &c_n1);
	    lwkopt = *m * nb;
	}
	work[1] = (double) lwkopt;

	if (*lwork < std::max(1_integer,*m) && ! lquery) {
	    *info = -7;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DTZRZF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

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

    nbmin = 2;
    nx = 1;
    iws = *m;
    if (nb > 1 && nb < *m) {

/*        Determine when to cross over from blocked to unblocked code. */

/* Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "DGERQF", " ", m, n, &c_n1, &c_n1);
	nx = std::max(i__1,i__2);
	if (nx < *m) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *m;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  reduce NB and */
/*              determine the minimum value of NB. */

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "DGERQF", " ", m, n, &c_n1, &
			c_n1);
		nbmin = std::max(i__1,i__2);
	    }
	}
    }

    if (nb >= nbmin && nb < *m && nx < *m) {

/*        Use blocked code initially. */
/*        The last kk rows are handled by the block method. */

/* Computing MIN */
	i__1 = *m + 1;
	m1 = std::min(i__1,*n);
	ki = (*m - nx - 1) / nb * nb;
/* Computing MIN */
	i__1 = *m, i__2 = ki + nb;
	kk = std::min(i__1,i__2);

	i__1 = *m - kk + 1;
	i__2 = -nb;
	for (i__ = *m - kk + ki + 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1;
		i__ += i__2) {
/* Computing MIN */
	    i__3 = *m - i__ + 1;
	    ib = std::min(i__3,nb);

/*           Compute the TZ factorization of the current block */
/*           A(i:i+ib-1,i:n) */

	    i__3 = *n - i__ + 1;
	    i__4 = *n - *m;
	    dlatrz_(&ib, &i__3, &i__4, &a[i__ + i__ * a_dim1], lda, &tau[i__],
		     &work[1]);
	    if (i__ > 1) {

/*              Form the triangular factor of the block reflector */
/*              H = H(i+ib-1) . . . H(i+1) H(i) */

		i__3 = *n - *m;
		dlarzt_("Backward", "Rowwise", &i__3, &ib, &a[i__ + m1 *
			a_dim1], lda, &tau[i__], &work[1], &ldwork);

/*              Apply H to A(1:i-1,i:n) from the right */

		i__3 = i__ - 1;
		i__4 = *n - i__ + 1;
		i__5 = *n - *m;
		dlarzb_("Right", "No transpose", "Backward", "Rowwise", &i__3,
			 &i__4, &ib, &i__5, &a[i__ + m1 * a_dim1], lda, &work[
			1], &ldwork, &a[i__ * a_dim1 + 1], lda, &work[ib + 1],
			 &ldwork)
			;
	    }
/* L20: */
	}
	mu = i__ + nb - 1;
    } else {
	mu = *m;
    }

/*     Use unblocked code to factor the last or only block */

    if (mu > 0) {
	i__2 = *n - *m;
	dlatrz_(&mu, n, &i__2, &a[a_offset], lda, &tau[1], &work[1]);
    }

    work[1] = (double) lwkopt;

    return 0;

/*     End of DTZRZF */

} /* dtzrzf_ */
